// MsgdDemoServer.cpp : implementation file
//

#include "stdafx.h"
#include <winsock2.h>
#include <afxsock.h>
// CMsgdDemoDlg dialog

#include "os_log.h"
#include "os_mutex.h"
#include "os_msgq.h"
#include "os_thread.h"
#include "os_vector.h"
#include "os_socket.h"
#include "Msgd_Demo.h"
#include "MsgdDemoServer.h"
#include "afxdialogex.h"
#include "msgd_engine_server.h"

#define TAG "MSGD_SVR_MAIN:"
#define MSGD_SVR_LOGI(fmt, ...) {OS_log(LVL_INFO, 0, TAG"[%s]%s(%d) "fmt, __FILE__,__func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_SVR_LOGW(fmt, ...) {OS_log(LVL_WRN, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_SVR_LOGE(fmt, ...) {OS_log(LVL_ERR, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_SVR_LOGT(fmt, ...) {OS_log(LVL_TRACE, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}

static MSGDMNGHandle gMsgdServerHandle = NULL;
static MsgdDemoServer *gMsgdDemoServer = NULL;
static int gFlagInited = 0;
// MsgdDemoServer dialog

IMPLEMENT_DYNAMIC(MsgdDemoServer, CDialogEx)

MsgdDemoServer::MsgdDemoServer(CWnd* pParent /*=NULL*/)
	: CDialogEx(MsgdDemoServer::IDD, pParent)
	, m_serverListenport(7897)
	, m_serverConnectionNum(0)
{
	gMsgdDemoServer = this;
}

MsgdDemoServer::~MsgdDemoServer()
{
}

void MsgdDemoServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONNECTION_NUM, m_serverConnectionNum);
	DDV_MinMaxUInt(pDX, m_serverConnectionNum, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_SVR_LISTEN_PORT, m_serverListenport);
	DDV_MinMaxUInt(pDX, m_serverListenport, 81, 65535);
}

afx_msg LRESULT MsgdDemoServer::OnProcessUserEvent(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1)
	{
		MSGD_SVR_LOGT("update the connection num");
		UpdateData(false);
	}
	else if (wParam == 2)
	{
		if (gFlagInited == 1)
		{
			GetDlgItem(IDC_BUTTON_SERVER_START)->SetWindowTextA(_T("Í£Ö¹"));
			GetDlgItem(IDC_EDIT_SVR_LISTEN_PORT)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_SERVER_START)->SetWindowTextA(_T("Æô¶¯"));
			GetDlgItem(IDC_EDIT_SVR_LISTEN_PORT)->EnableWindow(TRUE);
		}
	}
	
	return 0;
}

BEGIN_MESSAGE_MAP(MsgdDemoServer, CDialogEx)
	ON_MESSAGE(MsgdServerUserEventId, &MsgdDemoServer::OnProcessUserEvent)
	ON_BN_CLICKED(IDC_BUTTON_SERVER_START, &MsgdDemoServer::OnBnClickedButtonServerStart)
END_MESSAGE_MAP()


// MsgdDemoServer message handlers
void _msgd_demo_svr_process_member_event(MSGD_MNG_MEMBER_EVT_INFO_S *memberEvtInfo)
{
    int32_t num = 0;
    int32_t i;
    MSGD_MNG_CLI_S *pCliInfo = &memberEvtInfo->cliInfo; 
    switch(memberEvtInfo->evtType)
	{
		case EN_MSGD_MNG_MEMBER_EVT_TYPE_ADD_CLIENT:
		{
			MSGD_SVR_LOGT("EN_MSGD_MNG_MEMBER_EVT_TYPE_ADD_CLIENT");
			gMsgdDemoServer->m_serverConnectionNum++;
			break;
	    }
		case EN_MSGD_MNG_MEMBER_EVT_TYPE_REMOVE_CLIENT:
            MSGD_SVR_LOGT("EN_MSGD_MNG_MEMBER_EVT_TYPE_REMOVE_CLIENT");
			gMsgdDemoServer->m_serverConnectionNum--;
    		break;
		default:
			MSGD_SVR_LOGE("evt invalid(evtType=0x%x)", memberEvtInfo->evtType);
			return;
	}
	MSGD_MNG_CLI_S *cliInfo = &memberEvtInfo->cliInfo;
    MSGD_SVR_LOGT(" type:%d, g:%s, u:%s, ip:%s", cliInfo->cliType, cliInfo->group, cliInfo->user, cliInfo->remoteIp);

	HWND hwnd = gMsgdDemoServer->GetSafeHwnd();
	::PostMessage(hwnd, MsgdServerUserEventId, (WPARAM)1, (LPARAM)0);
}


void _msgd_demo_svr_event_callback(MSGDMNGHandle handle, MSGD_MNG_EVT_INFO_S *evtInfo)
{
	switch(evtInfo->evtType)
	{
		case EN_MSGD_MNG_EVT_TYPE_CONNECTION:
			MSGD_SVR_LOGT("EN_MSGD_MNG_EVT_TYPE_CONNECTION:%d", evtInfo->info.connectEvtType);
			break;
		case EN_MSGD_MNG_EVT_TYPE_MEMBER:
            MSGD_SVR_LOGT("EN_MSGD_MNG_EVT_TYPE_MEMBER");
            _msgd_demo_svr_process_member_event(&evtInfo->info.memberEvtInfo);
    		break;
    	case EN_MSGD_MNG_EVT_TYPE_MSG_PUBLISH:
			MSGD_SVR_LOGT("EN_MSGD_MNG_EVT_TYPE_MSG_PUBLISH");
			//msgd_svr_process_publishmsg_event(&evtInfo->info.msgPublishInfo);
			break;
		default:
			MSGD_SVR_LOGE("evt invalid(evtType=0x%x)", evtInfo->evtType);
			return;
	}

	return;
}

int start_msgd_server(int listPort)
{
	MSGDMNGHandle handle;
	int ret;

   
	ret = msgd_manager_init(&handle, NULL, listPort);
	if (ret != 0)
	{
		MSGD_SVR_LOGE("msgmanager_init failed");
		return -1;
	}

	ret = msgd_manager_set_listener(handle, (MSGD_MANAGER_FUNC_EVT_CB)_msgd_demo_svr_event_callback, (void *)handle);
	if (ret < 0)
	{
		MSGD_SVR_LOGE("msgmanager_set_listener failed");
		return -1;
	}
    
    ret = msgd_manager_start(handle);
    if (ret != 0)
    {
        MSGD_SVR_LOGE("msgmanager_start failed");
        return -1;
    }

	gMsgdServerHandle = handle;
	return 0;
}

void stop_msgd_server()
{
	int ret = msgd_manager_stop(gMsgdServerHandle);
    if (ret != 0)
    {
        MSGD_SVR_LOGE("msgd_manager_stop failed");
        //return -1;
    }
	gMsgdDemoServer->m_serverConnectionNum = 0;
	msgd_manager_uninit(&gMsgdServerHandle);
}

void MsgdDemoServer::OnBnClickedButtonServerStart()
{
	// TODO: Add your control notification handler code 

	if (gFlagInited == 1)
	{
		MSGD_SVR_LOGE("has started");
		stop_msgd_server();
		gFlagInited = 0;
	}
	else
	{
		if (0 > start_msgd_server(m_serverListenport))
		{
			stop_msgd_server();
			gFlagInited=0;
		}
		else
			gFlagInited=1;
	}
	HWND hwnd = gMsgdDemoServer->GetSafeHwnd();
	::PostMessage(hwnd, MsgdServerUserEventId, (WPARAM)2, (LPARAM)0);
}
