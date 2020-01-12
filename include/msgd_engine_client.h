
/*********************************************************************/
/* Filename:    msgd_engine_client.h                                                                             */
/*                                                                                                                  */
/*                                                                                                                  */
/* Description: the header of message Subcriber&Publisher , the Goups[] and Users[],  */
/*                 send or receive the massage from/to the msgd server                           */
/*                                                                                                                  */
/*  Author:      wangyunjin                                                                                */
/*  Email:      wyj0617mail@aliyun.com                                                             */
/*   Create:   2016-12-15                                                           		          */
/**********************************************************************/

/*********************************************************************/
/* Change History:                                                                                          */
/*     2016-12-15     wangyunjin  Created                                                             */
/*********************************************************************/


#ifndef _MSGD_ENEINE_CLIENT_H_
#define _MSGD_ENEINE_CLIENT_H_

#ifndef NULL
#define NULL 0
#endif

//#include "os_global.h"

#ifdef WIN32
#define __func__ __FUNCTION__
#endif

#if __cplusplus
extern "C"
{       
#endif  

typedef enum
{
	EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_SUCCESS=0x2000,//new client connect
	EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_KEEPALIVE,  //socket keepalive
	EN_MSGD_CLIENT_EVT_TYPE_CONNECTION_CLOSE,  //socket closed
	EN_MSGD_CLIENT_EVT_TYPE_MSG_RECEIVE_NOTIFY,  //msg has  receive notify	
	EN_MSGD_CLIENT_EVT_TYPE_MAX
}MSGD_CLIENT_EVT_TYPE_E;

typedef enum
{
	EN_MSGD_CLIENT_TYPE_NORMAL=0x1000,//for normal, P2P msg
	EN_MSGD_CLIENT_TYPE_PUBLISHER,//for publisher
	EN_MSGD_CLIENT_TYPE_SUBSCRIBER,     //for subcriber
	EN_MSGD_CLIENT_TYPE_BOTH,          // for publisher and subcriber
	EN_MSGD_CLIENT_TYPE_MAX
}MSGD_CLIENT_TYPE_E;

typedef struct
{
    int32_t errCode;
    int32_t msgId;
}MSGD_CLIENT_MSG_RESPONSE_INFO_S;

typedef struct
{
    MSGD_CLIENT_EVT_TYPE_E evtType;
    union
    {
        MSGD_CLIENT_MSG_RESPONSE_INFO_S responseInfo;
    }info;
    void *userData;
}MSGD_CLIENT_EVT_INFO_S;


typedef void * MSGDCLIHandle;

typedef void (*MSGD_CLIENT_FUNC_EVT_CB)(MSGDCLIHandle handle, MSGD_CLIENT_EVT_INFO_S *evtInfo);
typedef void (*MSGD_CLIENT_FUNC_MSG_CB)(MSGDCLIHandle handle, char *inBuff, int inBuffLen);

/*
* function:  msgd_client_init
* Target: initialize the instance conenct to msgd-server and get the handle
*
* para: handle---the return handle of the server instanse
*         cliType----register client type, subscriber or publisher, both
*         remoteHostIp---the msgd server IP addr
* 	    remoteHostPort---the msgd server port to tcp connect
*
* return: =0---successful
* 		-1---failed
*
*/
DLLEXPORT int32_t msgd_client_init(MSGDCLIHandle *handle, MSGD_CLIENT_TYPE_E cliType, char *remoteHostIp, int remoteHostPort);

/*
* function:  msgd_client_register_topics
* Target: register several groups and users in msgd server for recv msg about this groups and users
*
* para: handle---the handle create in msgd_client_init
* 	    group[]---the groups listenning message
* 	    groupNum---the  num of the groups
* 	    user[]---the users listenning message
* 	    userNum---the  num of the users
*
* return: =0---successful
* 		-1---failed
*
*/
//int32_t msgd_client_register_topics(MSGDCLIHandle handle, char *group[], int groupNum, char *user[], int userNum); //subcriber multi group

/*
* function:  msgd_client_register_topic
* Target: register one group and one user in msgd server for recv msg about this group and user
*
* para: handle---the handle create in msgd_client_init
* 	    groupName---the group listenning message
* 	    userName---the user listenning message
*
* return: =0---successful
* 		-1---failed
*
*/
DLLEXPORT int32_t msgd_client_register_topic(MSGDCLIHandle handle, char *groupName, char *userName);

/*
* function:  msgd_client_set_keepalive
* Target: send the keepalive msg to msgd server when internvalue seconds to keep the tcp connection
*
* para: handle---the handle create in msgd_client_init
* 	    interntimeSec---the intervalue seconds when sending message, must between 1~600 sec
* 	    aliveMsg---the keepalive Msg to send
* 	    aliveMsgLen---the keepalive Msg length, must between 1~128
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_client_set_keepalive(MSGDCLIHandle handle, int32_t interntimeSec, char *aliveMsg, int32_t aliveMsgLen);

/*
* function:  msgd_client_set_listener
* Target: set the callback function to recv the connection event and message from msgd server
*
* para: handle---the handle create in msgd_client_init
* 	    evtCb---the connection event callback function
* 	    msgCb---the msg recv from msgd server callback funtion
* 	    userData---the private user data when callback function calling
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_client_set_listener(MSGDCLIHandle handle, MSGD_CLIENT_FUNC_EVT_CB evtCb, MSGD_CLIENT_FUNC_MSG_CB msgCb, void *userData);

/*
* function:  msgd_client_start
* Target: start the task to listen the message
*
* para: handle---the handle create in msgd_client_init
*         encrypt---the encrypt key string for encode the msg
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_client_start(MSGDCLIHandle handle, char *encrypt);

/*
* function:  msgd_client_send_p2p_msg
* Target: send the msg to the dstGroup-dstUser
*
* para: handle---the handle create in msgd_client_init
* 	    dstGroup---the dest group name of sending
* 	    dstUser---the dest user name of sending
* 	    persistTime---the persistent Time(sec) if publishing dstObj offline
* 	    msg---the msg content  of publishing
* 	    msgLen---the msg content .length of publishing
*         msgId-----the id of the special msg
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_client_send_p2p_msg(MSGDCLIHandle handle, char *dstGroup, char *dstUser, int32_t persistTime, char *msg, int32_t msgLen, int32_t msgId);

/*
* function:  msgd_client_publish_msg
* Target: send publish-msg to the dstGroup, and this client must be TYPE=EN_MSGD_CLIENT_TYPE_PUBLISHER or EN_MSGD_CLIENT_TYPE_BOTH
*
* para: handle---the handle create in msgd_client_init
* 	    dstGroup---the dest group name of publishing
* 	    persistTime---the persistent Time(sec) if publishing dstObj offline
* 	    msg---the msg content  of publishing
* 	    msgLen---the msg content .length of publishing
*         msgId-----the id of the special msg
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_client_publish_msg(MSGDCLIHandle handle, char *dstGroup, int32_t persistTime, char *msg, int32_t msgLen, int32_t msgId);


/*
* function:  msgd_client_stop
* Target: stop the task, not listen the message
*
* para: handle---the handle create in msgd_client_init
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_client_stop(MSGDCLIHandle handle);

/*
* function:  msgd_client_uninit
* Target: stop the task and destroy the object, disconnect to the msgd server
*
* para: handle---the handle create in msgd_client_init, and handle==NULL when success
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_client_uninit(MSGDCLIHandle *handle);


DLLEXPORT const char *msgd_client_get_current_version();


#if __cplusplus
}
#endif

#endif //_MSGD_ENEINE_CLIENT_H_


