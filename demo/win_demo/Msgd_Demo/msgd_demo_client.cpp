#include "stdafx.h"
#include "msgd_demo_client.h"

#define __func__ __FUNCTION__
#define sleep(x) Sleep(1000*x)
#define usleep(x) Sleep(x/1000)

#include "os_log.h"
#include "msgd_engine_client.h"

#define TAG "MSGD_DEMO_C:"

static int32_t startFlag = 0;
static MSGDCLIHandle gMsgdClientHandle = 0;
static MSGD_DEMO_CLIENT_EVT_CB gEvtCb = NULL;
static MSGD_DEMO_CLIENT_MSG_CB gMsgCb = NULL;
static int gCliMod = 2;

int32_t gFlagConnected = 0;
void _msgd_demo_client_event_callback(MSGDCLIHandle handle, MSGD_CLIENT_EVT_INFO_S *evtInfo)
{
	switch(evtInfo->evtType)
	{
		case EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_SUCCESS:
		    gFlagConnected = 1;
			MSGD_DEMO_LOGT("EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_SUCCESS ");
			break;
		case EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_KEEPALIVE:
            MSGD_DEMO_LOGT("EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_KEEPALIVE");
    		break;
        case EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_CLOSE:
            gFlagConnected = 0;
            MSGD_DEMO_LOGT("EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_CLOSE");
			if (gEvtCb != NULL)
				gEvtCb(1);
			//msgd_client_stop();
    		break;
        case EN_MSGD_CLIENT_EVT_TYPE_MSG_RECEIVE_NOTIFY:
            MSGD_DEMO_LOGT("EN_MSGD_CLIENT_EVT_TYPE_MSG_RECEIVE_NOTIFY[errCode:%d, msgId:%d]", evtInfo->info.responseInfo.errCode, evtInfo->info.responseInfo.errCode);
    		break;
		default:
			MSGD_DEMO_LOGT("evt invalid(evtType=0x%x)", evtInfo->evtType);
			break;
	}
	
	return;
}


void _msgd_demo_client_msg_callback(MSGDCLIHandle handle, char *inBuff, int inBuffLen)
{
    //MSGD_DEMO_LOGT("Begin msgLen:%d, msg:%s", inBuffLen, inBuff);
	if (gMsgCb!= NULL)
		gMsgCb(inBuff, inBuffLen);
	//MSGD_DEMO_LOGT("End msgLen:%d, msg:%s", inBuffLen, inBuff);
}


void msgd_demo_client_set_mod(int mode)  //0--pub, 1--sub, 2---both
{
	gCliMod = mode;
}


int msgd_demo_client_start(char *svrIp, int svrPort, char *group, char *user, MSGD_DEMO_CLIENT_MSG_CB msgFunc, MSGD_DEMO_CLIENT_EVT_CB evtFunc)
{
	int32_t ret;

	if (startFlag == 1)
	{
		MSGD_DEMO_LOGE(" has started");
		return -1;
	}

	gMsgCb = msgFunc;
    gEvtCb = evtFunc;


    MSGDCLIHandle handle;
	MSGD_CLIENT_TYPE_E cliType = EN_MSGD_CLIENT_TYPE_BOTH;
	ret = msgd_client_init(&handle, cliType, svrIp, svrPort);
	if (ret != 0)
	{
		MSGD_DEMO_LOGE("msgsub_init failed");
		return -1;
	}

	ret = msgd_client_set_listener(handle, (MSGD_CLIENT_FUNC_EVT_CB)_msgd_demo_client_event_callback, (MSGD_CLIENT_FUNC_MSG_CB)_msgd_demo_client_msg_callback, (void *)handle);
	if (ret < 0)
	{
		MSGD_DEMO_LOGE("msgsub_set_listener failed");
		return -1;
	}

    ret = msgd_client_register_topic(handle, group, user);
	if (ret < 0)
	{
		MSGD_DEMO_LOGE("msgsub_set_topic failed");
		return -1;
	}

	char aliveMsg[64];
    sprintf(aliveMsg, "win_sub_%s_%s", group, user);
    int32_t aliveMsgLen = strlen(aliveMsg);
    ret = msgd_client_set_keepalive(handle, 30, aliveMsg, aliveMsgLen);
	if (ret < 0)
	{
		MSGD_DEMO_LOGE("msgsub_set_keepalive failed");
		return -1;
	}

	char *encrypt = "hello windows";
    ret = msgd_client_start(handle, encrypt);
    if (ret != 0)
    {
        MSGD_DEMO_LOGE("msgsub_start failed");
        return -1;
    }
	MSGD_DEMO_LOGT("msgd client connect to %s:%d", svrIp, svrPort);

	gMsgdClientHandle = handle;
	startFlag = 1;

	if (gEvtCb != NULL)
		gEvtCb(0);

	return 0;
}

int msgd_demo_client_send_msg(char *dstGroup, char *dstUser, int32_t persistTime, char *pMsg, int msgLen, int32_t msgId)
{
	int32_t ret = 0;

	if (startFlag != 1)
	{
		MSGD_DEMO_LOGE(" not started");
		return -1;
	}

	MSGD_DEMO_LOGT(" msgd_client_publish_msg:%s,%s,%d,%s,%d,%d", dstGroup, dstUser, persistTime, pMsg, msgLen, msgId);
	ret = msgd_client_send_p2p_msg(gMsgdClientHandle, dstGroup, dstUser, persistTime, pMsg, msgLen, msgId);
	if (ret != 0)
	{
		MSGD_DEMO_LOGE(" msgd_client_publish_msg failed");
		return -1;
	}
	return 0;
}

void msgd_demo_client_stop()
{
	int ret;

	if (startFlag != 1)
	{
		MSGD_DEMO_LOGE(" has stoped");
		return;
	}

    ret = msgd_client_stop(gMsgdClientHandle);
    if (ret != 0)
    {
        MSGD_DEMO_LOGE( "msgd_client_stop failed");
        return;
    }
 
	msgd_client_uninit(&gMsgdClientHandle);
	startFlag = 0;
	MSGD_DEMO_LOGT( "msgd client stoped");
}
