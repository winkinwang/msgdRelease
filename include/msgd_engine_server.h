/*********************************************************************/
/* Filename:    msgd_engine_server.h                                                                           	   */
/*                                                                                                                  */
/*                                                                                                                  */
/* Description: the description message dispatcher manager server for transfering    */
/*                  from publisher to subcriber, manager all the subcriber and publisher  */
/*                  connection                                                                                */
/*                                                                                                                  */
/*  Author:      wangyunjin                                                                                */
/*  Email:      wyj0617mail@aliyun.com                                                             */
/*   Create:   2016-07-05                                                          		          */
/**********************************************************************/

/*********************************************************************/
/* Change History:                                                                                          */
/*     2016-07-05     wangyunjin  Created                                                             */
/*********************************************************************/


#ifndef __MSGD_ENGINE_SERVER_H_
#define __MSGD_ENGINE_SERVER_H_

#ifndef NULL
#define NULL 0
#endif

#ifdef WIN32
#define __func__ __FUNCTION__
#endif

#if __cplusplus
extern "C"
{       
#endif


typedef void * MSGDMNGHandle;

typedef enum
{
    EN_MSGD_MNG_CLIENT_TYPE_NORMAL=0x0,//for normal, P2P msg
	EN_MSGD_MNG_CLIENT_TYPE_PUBLISHER,//for publisher
	EN_MSGD_MNG_CLIENT_TYPE_SUBSCRIBER,     //for subcriber
	EN_MSGD_MNG_CLIENT_TYPE_BOTH,          // for publisher and subcriber
	EN_MSGD_MNG_CLIENT_TYPE_MAX
}MSGD_MNG_CLIENT_TYPE_E;

typedef enum
{
	EN_MSGD_MNG_CONN_EVT_TYPE_NEW_CONNECTION=0x2000,//new client connect
	EN_MSGD_MNG_CONN_EVT_TYPE_CONNECTION_CLOSE,  //socket closed
	EN_MSGD_MNG_CONN_EVT_TYPE_MAX
}MSGD_MNG_CONN_EVT_TYPE_E;

typedef enum
{
	EN_MSGD_MNG_MEMBER_EVT_TYPE_ADD_CLIENT=0x2100,//new client ONLINE
	EN_MSGD_MNG_MEMBER_EVT_TYPE_REMOVE_CLIENT, //client OFFLINE
	EN_MSGD_MNG_MEMBER_EVT_TYPE_MAX
}MSGD_MNG_MEMBER_EVT_TYPE_E;


typedef struct
{
    MSGD_MNG_CLIENT_TYPE_E cliType;
    char group[32];
    char user[32];
    char remoteIp[32];
    int64_t createTime;//创建时间, time_t t = time(NULL);  struct tm * timeinfo = localtime(&rawtime);  asctime(timeinfo)
    int64_t lastAliveTime;//上一次alive时间, time_t t = time(NULL);
}MSGD_MNG_CLI_S;


typedef struct
{
    MSGD_MNG_MEMBER_EVT_TYPE_E evtType;
    MSGD_MNG_CLI_S cliInfo;    
}MSGD_MNG_MEMBER_EVT_INFO_S;


typedef enum
{
	EN_MSGD_MNG_MSG_PUBLISH_EVT_TYPE_SUCCESS=0x4000,//publish success
	EN_MSGD_MNG_MSG_PUBLISH_EVT_TYPE_FAILED,  //publish failed
	EN_MSGD_MNG_MSG_PUBLISH_EVT_TYPE_MAX
}MSGD_MNG_MSG_PUBLISH_EVT_TYPE_E;


typedef struct
{
    char srcGroup[32];
    char srcUser[32];
    char dstGroup[32];
    char dstUser[32];
    int32_t persistentTime;//持续时间, sec
    int32_t srcMsgId;
    char *pMsgData;
    int32_t msgLen;
}MSGD_MNG_PUBLISH_MSG_S;


typedef struct
{
    MSGD_MNG_MSG_PUBLISH_EVT_TYPE_E evtType;
    MSGD_MNG_PUBLISH_MSG_S msgInfo;    
}MSGD_MNG_MSG_PUBLISH_INFO_S;


typedef enum
{
	EN_MSGD_MNG_EVT_TYPE_CONNECTION=0x3000,//client connection event
	EN_MSGD_MNG_EVT_TYPE_MEMBER,//client member event
	EN_MSGD_MNG_EVT_TYPE_MSG_PUBLISH,  //client message publish event
	EN_MSGD_MNG_EVT_TYPE_MAX
}MSGD_MNG_EVT_TYPE_E;


typedef struct
{
    MSGD_MNG_EVT_TYPE_E evtType;
    union{
        MSGD_MNG_CONN_EVT_TYPE_E connectEvtType;
        MSGD_MNG_MEMBER_EVT_INFO_S memberEvtInfo;
        MSGD_MNG_MSG_PUBLISH_INFO_S msgPublishInfo;
    } info;
    void *userData;
}MSGD_MNG_EVT_INFO_S;



typedef void (*MSGD_MANAGER_FUNC_EVT_CB)(MSGDMNGHandle handle, MSGD_MNG_EVT_INFO_S *evtInfo);

/*
* function:  msgd_manager_init
* Target: initialize the server object and get the handle
*
* para: handle---return handle of the manager instanse
* 	    localIp---the local IP addr to bind when host has multi network hw, default 127.0.0.1
* 	    listenPort---the port for tcp listen
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_init(MSGDMNGHandle *handle, char *localIp, int listenPort);

/*
* function:  msgd_manager_set_listener
* Target: set the callback function to recv the connection event from subcriber and publisher
*
* para: handle---the handle create in msgd_manager_init
* 	    evtCb---the connection event callback function
* 	    userData---the private user data when callback function calling
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_set_listener(MSGDMNGHandle handle, MSGD_MANAGER_FUNC_EVT_CB evtCb, void *userData);

/*
* function:  msgd_manager_start
* Target: start the working task to accept the connection from subcriber and publisher, and transfer the msg
*
* para: handle---the handle create in msgd_manager_init
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_start(MSGDMNGHandle handle);

/*
* function:  msgd_manager_get_client_num
* Target: get the register client num by the special clitype
*
* para: handle---the handle create in msgd_manager_init
* 	    cliType---the type of the client
*
* return: >=0---successful, this type client num
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_get_client_num(MSGDMNGHandle handle, MSGD_MNG_CLIENT_TYPE_E cliType);

/*
* function:  msgd_manager_get_clients
* Target: get the register clients info by the special clitype
*
* para: handle---the handle create in msgd_manager_init
* 	    cliType---the type of the client
* 	    startPos---the start position, must small than the num 
* 	    outCliInfos---the output info array of the clients buffer
* 	    maxNum---the max size of the outCliInfos
*
* return: >=0---successful, this return client real num
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_get_clients(MSGDMNGHandle handle, MSGD_MNG_CLIENT_TYPE_E cliType, int32_t startPos, MSGD_MNG_CLI_S outCliInfos[], int32_t maxNum);

/*
* function:  msgd_manager_remove_client
* Target:   remove the client has connected.
*
* para: handle---the handle create in msgd_manager_init
* 	    cliInfo---the client object
*
* return: >=0---successful, this return client real num
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_remove_client(MSGDMNGHandle handle, MSGD_MNG_CLI_S *cliInfo);

/*
* function:  msgd_manager_inject_msgs
* Target: inject the msgs not published into the manager when the dstGroup_dstUser online
*
* para: handle---the handle create in msgd_manager_init
* 	    msgInfo---the array of publish-msg
* 	    num---the num of the msg array
*
* return: >0---successful, this real num injected success
* 		<=0---failed
*/
DLLEXPORT int32_t msgd_manager_inject_msgs(MSGDMNGHandle handle, MSGD_MNG_PUBLISH_MSG_S msgInfo[], int32_t num);


/*
* function:  msgd_manager_stop
* Target: stop the working task
*
* para: handle---the handle create in msgd_manager_init
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_stop(MSGDMNGHandle handle);

/*
* function:  msgd_manager_uninit
* Target: stop the working task and destroy the object create by msgd_manager_init
*
* para: handle---the handle create in msgd_manager_init
*
* return: =0---successful
* 		-1---failed
*/
DLLEXPORT int32_t msgd_manager_uninit(MSGDMNGHandle *handle);




DLLEXPORT const char *msgd_manager_get_current_version();

#if __cplusplus
}
#endif

#endif //__MESSAGE_DISPATCHER_H_

