
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "os_log.h"
#include "os_thread.h"
#include "msgd_engine_client.h"

#define TAG "MSGD_CLI_MAIN:"

//#define MSGD_CLIENT_MAIN_DEBUG 1

int32_t gFlagConnected = 0;
void msgs_event_callback(MSGDCLIHandle handle, MSGD_CLIENT_EVT_INFO_S *evtInfo)
{
	switch(evtInfo->evtType)
	{
		case EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_SUCCESS:
		    gFlagConnected = 1;
			OS_log(LVL_TRACE, 0, "%s EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_SUCCESS ",__func__);
			break;
		case EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_KEEPALIVE:
            OS_log(LVL_TRACE, 0, "%s EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_KEEPALIVE",__func__);
    		break;
        case EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_CLOSE:
            gFlagConnected = 0;
            OS_log(LVL_TRACE, 0, "%s EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_CLOSE",__func__);
    		break;
        case EN_MSGD_CLIENT_EVT_TYPE_MSG_RECEIVE_NOTIFY:
            OS_log(LVL_TRACE, 0, "%s EN_MSGD_CLIENT_EVT_TYPE_MSG_RECEIVE_NOTIFY[errCode:%d, msgId:%d]",__func__, evtInfo->info.responseInfo.errCode, evtInfo->info.responseInfo.msgId);
    		break;
		default:
			OS_log(LVL_ERR, 0, "%s evt invalid(evtType=0x%x)",__func__, evtInfo->evtType);
			break;
	}
	
	return;
}


MSGDCLIHandle gTestHandle[2000];
int32_t gTestLoopNum = 0;
void msgs_msg_callback(MSGDCLIHandle handle, char *inBuff, int inBuffLen)
{
    int32_t i;
    /*char client[32];
    for(i = 0; i<gTestLoopNum; i++)
    {
        if (gTestHandle[i] == handle)
        {
            sprintf(client, "Client_%d", i);
            break;
        }
    }*/
    OS_log(LVL_TRACE, 0, "recv msgLen:%d, msg:%s", inBuffLen, inBuff);
}


int test(char **argv)
{
    int i, ret;
    

    gTestLoopNum = atoi(argv[5]);
    for(i = 0; i<gTestLoopNum; i++)
    {
        MSGDCLIHandle handle;

    	ret = msgd_client_init(&handle, EN_MSGD_CLIENT_TYPE_BOTH, argv[1], atoi(argv[2]));
    	if (ret != 0)
    	{
    		OS_log(LVL_ERR, 3, "msgd_client_init failed");
    		return -1;
    	}
    	ret = msgd_client_set_listener(handle, (MSGD_CLIENT_FUNC_EVT_CB)msgs_event_callback, (MSGD_CLIENT_FUNC_MSG_CB)msgs_msg_callback, (void *)handle);
    	if (ret < 0)
    	{
    		OS_log(LVL_ERR, 3, "msgd_client_set_listener failed");
    		return -1;
    	}
        char userId[64];
        sprintf(userId, "%s_%d", argv[4], i);
        ret = msgd_client_register_topic(handle, argv[3], userId);
    	if (ret < 0)
    	{
    		OS_log(LVL_ERR, 3, "msgd_client_set_topic failed");
    		return -1;
    	}

        char aliveMsg[128];
        sprintf(aliveMsg, "%s_keepalive", userId);
        int32_t aliveMsgLen = strlen(aliveMsg);
        ret = msgd_client_set_keepalive(handle, 50, aliveMsg, aliveMsgLen);
    	if (ret < 0)
    	{
    		OS_log(LVL_ERR, 3, "msgd_client_set_keepalive failed");
    		return -1;
    	}
        ret = msgd_client_start(handle, "HELLO");
        if (ret != 0)
        {
            OS_log(LVL_ERR, 3, "msgd_client_start[%s] failed", userId);
            return -1;
        }
       
        gTestHandle[i] = handle;
    	OS_log(LVL_TRACE, 3, "start success[%s].....", userId);

    	OS_msleep(100);
    }

#if 0
    sleep(3);
    for(i = 0; i<loopNum; i++)
    {
        ret = msgd_client_stop(ghandle[i]);
        if (ret != 0)
        {
            OS_log(LVL_ERR, 3, "msgd_client_stop failed");
            return -1;
        }

    	msgd_client_uninit(&ghandle[i]);
    }
#else
    while (1)
        sleep(5);
#endif
    return 0;
}

int main(int argc, char **argv)
{
	int ret, logLvl=LVL_WRN;
	int svrPort = 7895;

    signal(SIGILL, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

#if defined(MSGD_CLIENT_MAIN_DEBUG)
    if (argc < 5)
    {
        printf("./bin svrip svrport  group user \n");
        //printf("./bin svrip svrport  group user [clientNum]\n");
        return -1;
    }
    logLvl = LVL_INFO;
#else
    if (argc < 8)
    {
        printf("./bin svrip svrport  localGroup localUser dstGroup dstUser persistTimeSec\n");
        return -1;
    }
    logLvl = LVL_WRN;
#endif
    OS_logInit(NULL, NULL, 1);
    OS_logSetAppName(TAG);
    OS_logSetGrade(0,logLvl);
    //OS_logSetDomainMask(0, OS_LOG_DOMAIN_ALL);

    OS_verShow();


#if defined(MSGD_CLIENT_MAIN_DEBUG)
    if (argc >= 6)
    {
        test(argv);
        return 0;
    }
#endif

    MSGDCLIHandle handle;
    svrPort= atoi(argv[2]);

	ret = msgd_client_init(&handle, EN_MSGD_CLIENT_TYPE_BOTH, argv[1], svrPort);
	if (ret != 0)
	{
		OS_log(LVL_ERR, 3, "msgd_client_init failed");
		return -1;
	}
	ret = msgd_client_set_listener(handle, (MSGD_CLIENT_FUNC_EVT_CB)msgs_event_callback, (MSGD_CLIENT_FUNC_MSG_CB)msgs_msg_callback, (void *)handle);
	if (ret < 0)
	{
		OS_log(LVL_ERR, 3, "msgd_client_set_listener failed");
		return -1;
	}

    ret = msgd_client_register_topic(handle, argv[3], argv[4]);
	if (ret < 0)
	{
		OS_log(LVL_ERR, 3, "msgd_client_set_topic failed");
		return -1;
	}

    char *aliveMsg = "test keepalive";
    int32_t aliveMsgLen = strlen(aliveMsg);
    ret = msgd_client_set_keepalive(handle, 30, aliveMsg, aliveMsgLen);
	if (ret < 0)
	{
		OS_log(LVL_ERR, 3, "msgd_client_set_keepalive failed");
		return -1;
	}
    ret = msgd_client_start(handle, "test");
    if (ret != 0)
    {
        OS_log(LVL_ERR, 3, "msgd_client_start failed");
        return -1;
    }
    
	OS_log(LVL_TRACE, 3, "loop.....");

	char pushMsg[512];
	//int times = 0;
	while(1)
	{		
        //sprintf(pushMsg, "test msg:%d", times++);
        //strncpy(pushMsg, argv[5], 511);
        if (gFlagConnected == 0)
        {
            OS_log(LVL_TRACE, 3, "not connect to server.....");
            sleep(1);
            continue;
        }
        printf("Please input msg:");
        fflush(NULL);
        gets(pushMsg);
        if (strlen(pushMsg) <= 0)
            continue;
		msgd_client_send_p2p_msg(handle, argv[5], argv[6], atoi(argv[7]), pushMsg, strlen(pushMsg), 123);
		//OS_log(LVL_TRACE, 0, "msgd_client_publish_msg(%s) success", pushMsg);
        //break;
	}

    ret = msgd_client_stop(handle);
    if (ret != 0)
    {
        OS_log(LVL_ERR, 3, "msgd_client_start failed");
        return -1;
    }

	msgd_client_uninit(&handle);
    OS_log(LVL_WRN, 0, "main thread exit");
	return 0;
}



