
// MsgdDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "os_log.h"
#include "Msgd_Demo.h"
#include "MsgdDemoDlg.h"
#include "MsgdDemoServer.h"
#include "msgd_engine_client.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <winsock2.h>
#include <afxsock.h>
// CMsgdDemoDlg dialog

#include "os_log.h"
#include "os_mutex.h"
#include "os_msgq.h"
#include "os_thread.h"
#include "os_vector.h"
#include "os_socket.h"
#include "trpc_engine.h"

#include "msgd_demo_client.h"

#pragma comment(lib,"msgd.lib")
#pragma comment(lib,"trpc.lib")
#pragma comment(lib,"OS_lib.lib")

struct SUB_MSG_T{
	int len;
	char *pMsg;
};

static int flagConnected = 0;
static struct SUB_MSG_T g_subMsg;

#define TAG "MSGD_CLIENT_MAIN:"
#define MSGD_CLIENT_LOGI(fmt, ...) {OS_log(LVL_INFO, 0, TAG"[%s]%s(%d) "fmt, __FILE__,__func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_CLIENT_LOGW(fmt, ...) {OS_log(LVL_WRN, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_CLIENT_LOGE(fmt, ...) {OS_log(LVL_ERR, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_CLIENT_LOGT(fmt, ...) {OS_log(LVL_TRACE, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}

static CMsgdDemoDlg *g_dlg = NULL;
static char hostIpBuff[64];
void get_local_ip_function(void *para)
{	
	getLocalIp(hostIpBuff, 64, "eth0");
	HWND hwnd = g_dlg->GetSafeHwnd();
	::SendMessage(hwnd,UserEventId,(WPARAM)1,(LPARAM)1);
	MSGD_CLIENT_LOGT("sendMessage get local ip");
}

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "winspool.h"

// CSerialDlg 消息处理程序
void InitConsoleWindow()
{
    int nCrt = 0;
    FILE* fp;
    AllocConsole();
    nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    fp = _fdopen(nCrt, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
}

void DeInitConsoleWindow()
{
	FreeConsole();
    //FILE* fp = stdout;
	//fclose(fp);	
}

CMsgdDemoDlg::CMsgdDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMsgdDemoDlg::IDD, pParent)
	, mServerDomainName(_T("www.smartinhome.cn"))
	, mServerIpAddrStr(_T("192.168.32.128"))
	, mServerListenPort(7897)
	, mLocalGroupName(_T("auto"))
	, mLocalUserName(_T("wyj"))
	, mDstGroupName(_T("auto"))
	, mDstUserName(_T("wyj"))
	, mSendMsgBuffer(_T("hello"))
	, mMsgdRecvBuffStr(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_dlg = this;
	AfxSocketInit();
	InitConsoleWindow();
	char *localFile="1.log";
    OS_logInit(NULL, NULL, 1);
    OS_logSetAppName("MsgdDemo");
    OS_logSetGrade(0,LVL_INFO);

    OS_verShow();
	//MSGD_CLIENT_LOGT("CMsgdDemoDlg constract");

	strcpy(hostIpBuff, "127.0.0.1");

	HWND hwnd = g_dlg->GetSafeHwnd();
	::PostMessage(hwnd, UserEventId, (WPARAM)4, (LPARAM)1);
}

void CMsgdDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER_DOMAIN, mServerDomainName);
	DDV_MaxChars(pDX, mServerDomainName, 64);
	DDX_Text(pDX, IDC_EDIT_IP_ADDR, mServerIpAddrStr);
	DDV_MaxChars(pDX, mServerIpAddrStr, 16);
	DDX_Text(pDX, IDC_EDIT_SVR_PORT, mServerListenPort);
	DDV_MinMaxInt(pDX, mServerListenPort, 80, 65535);
	DDX_Text(pDX, IDC_EDIT_LOCAL_GROUP_NAME, mLocalGroupName);
	DDV_MaxChars(pDX, mLocalGroupName, 31);
	DDX_Text(pDX, IDC_EDIT_LOCAL_USER_NAME, mLocalUserName);
	DDV_MaxChars(pDX, mLocalUserName, 31);
	DDX_Text(pDX, IDC_EDIT_DST_GROUP_NAME, mDstGroupName);
	DDV_MaxChars(pDX, mDstGroupName, 31);
	DDX_Text(pDX, IDC_EDIT_DST_USER_NAME, mDstUserName);
	DDV_MaxChars(pDX, mDstUserName, 31);
	DDX_Text(pDX, IDC_EDIT_SEND_MSG_BUFFER, mSendMsgBuffer);
	DDV_MaxChars(pDX, mSendMsgBuffer, 256);
	DDX_Control(pDX, IDC_EDIT_REVICE_MSG, mMsgdRecvBuffCtr);
	DDX_Text(pDX, IDC_EDIT_REVICE_MSG, mMsgdRecvBuffStr);
}

BEGIN_MESSAGE_MAP(CMsgdDemoDlg, CDialogEx)
	ON_MESSAGE(UserEventId, &CMsgdDemoDlg::OnProcessUserEvent)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GET_DOMAIN_IP, &CMsgdDemoDlg::OnBnClickedButtonGetDomainIp)
	ON_BN_CLICKED(IDC_BUTTON_GET_LOCAL_IP, &CMsgdDemoDlg::OnBnClickedButtonGetLocalIp)
	ON_BN_CLICKED(IDC_BUTTON_START_CONNECT_SVR, &CMsgdDemoDlg::OnBnClickedButtonStartConnectSvr)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MSG, &CMsgdDemoDlg::OnBnClickedButtonSendMsg)
	ON_BN_CLICKED(IDC_BUTTON_START_LOCAL_SEVER, &CMsgdDemoDlg::OnBnClickedButtonStartLocalSever)
END_MESSAGE_MAP()


afx_msg LRESULT CMsgdDemoDlg::OnProcessUserEvent(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1)
	{
		MSGD_CLIENT_LOGT(" svrip:%s", hostIpBuff);
		mServerIpAddrStr = hostIpBuff;
		UpdateData(false);
	}
	else if (wParam == 3)
	{
		MSGD_CLIENT_LOGT("msg:%s   len:%d", g_subMsg.pMsg, g_subMsg.len);

		if (g_subMsg.len <= 0)
			return 0;

		CString cMsg;
		cMsg.Format("recv msg:%s\r\n", g_subMsg.pMsg);
		free(g_subMsg.pMsg);
		g_subMsg.len = 0;

		/*if (m_subMsgRecvControl.GetLineCount()>20)
			m_subMsgRecv = "";
		else
			m_subMsgRecv+="\r\n";
		m_subMsgRecv+="recv new msg:";
		m_subMsgRecv+=cMsg;

		m_subMsgRecvControl.SetSel(-1, -1);
		m_subMsgRecvControl.ReplaceSel(cMsg+"\r\n");
		m_subMsgRecvControl.LineScroll(m_subMsgRecvControl.GetLineCount(), 0);
		UpdateData(FALSE);*/


		int iLineNum=mMsgdRecvBuffCtr.GetLineCount();
		if(iLineNum<=10)
		{
			mMsgdRecvBuffCtr.SetSel(-1, -1);
			mMsgdRecvBuffCtr.ReplaceSel(cMsg);
			mMsgdRecvBuffStr += cMsg;
		}
		else
		{
			//m_subMsgRecvControl.SetSel(0, -1);
			mMsgdRecvBuffCtr.Clear();
			mMsgdRecvBuffStr = "";
			mMsgdRecvBuffCtr.ReplaceSel(cMsg);
		}
		UpdateData(false);
	}
	else if (wParam == 4)
	{
		if (flagConnected == 1)
		{
			GetDlgItem(IDC_BUTTON_START_CONNECT_SVR)->SetWindowTextA(_T("停止"));
			GetDlgItem(IDC_EDIT_IP_ADDR)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_SVR_PORT)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_LOCAL_GROUP_NAME)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_LOCAL_USER_NAME)->EnableWindow(FALSE);

			GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_START_CONNECT_SVR)->SetWindowTextA(_T("开始"));
			GetDlgItem(IDC_EDIT_IP_ADDR)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SVR_PORT)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_LOCAL_GROUP_NAME)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_LOCAL_USER_NAME)->EnableWindow(TRUE);

			GetDlgItem(IDC_BUTTON_SEND_MSG)->EnableWindow(FALSE);

			msgd_demo_client_stop();
		}
	}
	return 0;
}

// CMsgdDemoDlg message handlers

BOOL CMsgdDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString dlgTitle = "MsgdDemo:";
	dlgTitle.Append(msgd_client_get_current_version());
	SetWindowText(dlgTitle);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMsgdDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMsgdDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void getHostIpFunc(void *para)
{
	char *hostName = (char *)para;
	//hostent* host = gethostbyname("www.autoinhome.com");
	MSGD_CLIENT_LOGT("hostName:%s", hostName);
	hostent* host = gethostbyname(hostName);
	if (host == NULL)
	{
		MSGD_CLIENT_LOGE("host == NULL");
		return;
	}

	char* pszIP = (char *)inet_ntoa(*(struct in_addr *)(host->h_addr));
	strncpy(hostIpBuff, pszIP, 64);
	HWND hwnd = g_dlg->GetSafeHwnd();
	::SendMessage(hwnd, UserEventId, (WPARAM)1, (LPARAM)1);
	MSGD_CLIENT_LOGT("get host ip[%s=%s] and sendMessage", hostName, hostIpBuff);
}


void CMsgdDemoDlg::OnBnClickedButtonGetDomainIp()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	MSGD_CLIENT_LOGT("sendMessage %s", mServerDomainName.GetBuffer());

	int threadId2 = OS_thread_create((OS_THREAD_FUNC)getHostIpFunc, mServerDomainName.GetBuffer());
	return;
}


void CMsgdDemoDlg::OnBnClickedButtonGetLocalIp()
{
	// TODO: Add your control notification handler code here

	UpdateData();
	if (mServerDomainName=="show")
	{
		static int consoleShowFlag = 0;
		if (consoleShowFlag == 0)
		{
			InitConsoleWindow();
			consoleShowFlag = 1;
		}
		else
		{
			DeInitConsoleWindow();
			consoleShowFlag = 0;
		}
	}

	int threadId1 = OS_thread_create((OS_THREAD_FUNC)get_local_ip_function, NULL);
}


// evtType == 0, means connect
// evtType == 1, means disconnect
void msgd_demo_client_evt_func(int evtType)
{
	MSGD_CLIENT_LOGT("evtType:%d", evtType);
	if (evtType == 0)
		flagConnected = 1;
	else
		flagConnected = 0;

	HWND hwnd = g_dlg->GetSafeHwnd();
	::PostMessage(hwnd, UserEventId, (WPARAM)4, (LPARAM)1);
}


void msgd_demo_client_msg_func(char *inBuff, int inBuffLen)
{
	g_subMsg.len = inBuffLen;
	g_subMsg.pMsg = (char *)malloc(inBuffLen+1);
	memcpy(g_subMsg.pMsg, inBuff, inBuffLen);
	g_subMsg.pMsg[inBuffLen] = 0;
	MSGD_CLIENT_LOGT("recvMsg[%d]:%s",g_subMsg.len, g_subMsg.pMsg);
	HWND hwnd = g_dlg->GetSafeHwnd();
	//::SendMessage(hwnd,UserEventId,3,(LPARAM)0);
	::PostMessage(hwnd,UserEventId,3,(LPARAM)0);
}

void CMsgdDemoDlg::OnBnClickedButtonStartConnectSvr()
{
	// TODO: Add your control notification handler code here

	if (flagConnected == 1)
	{
		MSGD_CLIENT_LOGT("flagConnected == 1 and stop");
		msgd_demo_client_stop();
		GetDlgItem(IDC_BUTTON_START_CONNECT_SVR)->SetWindowTextA(_T("开始"));
		return;
	}

	UpdateData();
	MSGD_CLIENT_LOGT("connect to server:%s:%d", mServerIpAddrStr.GetBuffer(), mServerListenPort);
	MSGD_CLIENT_LOGT("localInfo:%s:%s", mLocalGroupName.GetBuffer(), mLocalUserName.GetBuffer());

	int mode = 2;
	char *svrIp = mServerIpAddrStr.GetBuffer();
	int svrPort = mServerListenPort;
	char *group = mLocalGroupName.GetBuffer();
	char *user = mLocalUserName.GetBuffer();
	msgd_demo_client_set_mod(mode);
	int ret = msgd_demo_client_start(svrIp, svrPort, group, user, (MSGD_DEMO_CLIENT_MSG_CB)msgd_demo_client_msg_func, (MSGD_DEMO_CLIENT_EVT_CB)msgd_demo_client_evt_func);
	if (ret < 0)
	{
		MSGD_CLIENT_LOGE("msgd_client_start failed");
		return;	
	}
}


void CMsgdDemoDlg::OnBnClickedButtonSendMsg()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	char *dstGroup = mDstGroupName.GetBuffer();
	char *dstUser = mDstUserName.GetBuffer();
	int32_t persistTime = 20;
	char *pMsg = mSendMsgBuffer.GetBuffer();
	int msgLen = mSendMsgBuffer.GetLength();
	int32_t msgId = 0;

	if ((mDstGroupName.GetLength()<=0)||(mDstUserName.GetLength()<=0)||
		(persistTime<0)||(mSendMsgBuffer.GetLength()<=0))
	{
		MSGD_CLIENT_LOGE("input para invalid");
		return;
	}

	if (flagConnected != 1)
	{
		MSGD_CLIENT_LOGE("has not connect to server....");
		return;
	}
	int ret = msgd_demo_client_send_msg(dstGroup, dstUser, persistTime, pMsg, msgLen, msgId);
	if (ret < 0)
	{
		MSGD_CLIENT_LOGE("msgd_client_send_msg failed");
		return;	
	}
}

static MsgdDemoServer *msgdDemoServer = NULL;
void CMsgdDemoDlg::OnBnClickedButtonStartLocalSever()
{
	// TODO: Add your control notification handler code here
	if (msgdDemoServer == NULL)
	{
		msgdDemoServer = new MsgdDemoServer();
		msgdDemoServer->Create(IDD_DIALOG_SERVER);
	}
	else
		MSGD_CLIENT_LOGE("msgdDemoServer has create");
	//int ret = msgdDemoServer->DoModal();
	msgdDemoServer->ShowWindow(SW_SHOW);
}
