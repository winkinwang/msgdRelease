# msgdRelease

一，现状及目的

随着互联网和物联网的发展，消息通信和交互不再仅限于人与人，而充斥于人与人、人与物、物与物之间；消息通信的主体也跨越各平台，

PC（Windows, OSX, Linux等)，移动端（Android,IOS,WP等）及电子设备（MCU,ARM等）。

目前常用的消息推送机制有：基于HTTP的轮循、基于XMPP、基于MQTT及第三方的推送系统（如极光、云巴等），每种机制都有其优劣，

具体可网上搜索或参考http://blog.csdn.net/thetryndamere/article/details/53141696。考虑到大部分推送收费、代码不可控、功能强大但是臃肿、

平台扩展等因素，本人基于C语言开发了一套msgd，可实现设备间点对点私密消息交互、以组为单元的群消息推送。



最近看到百度的天工物接入的一个技术视频，主旨是介绍IOT消息推送和数据交互，《如何做一个千万级并发的物联网接入架构》，很值得学习下 。

http://edu.csdn.net/course/detail/2970



二，支持平台

经过努力，目前已实现使用平台有：windows、Linux、 android、OSX、ESP8266 non-os，后续有需要，会继续扩展使用平台。



三，接口说明

msgd分服务端和客户端，接口分两部分。

服务端：


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

客户端：
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
* function:  msgd_client_send_p2p_msg
* Target: send the msg to the dstGroup-dstUser
*
* para: handle---the handle create in msgd_client_init
* dstGroup---the dest group name of sending
* dstUser---the dest user name of sending
* persistTime---the persistent Time(sec) if publishing dstObj offline
* msg---the msg content  of publishing
* msgLen---the msg content .length of publishing
* msgId-----the id of the special msg
*
* return: =0---successful
*         -1---failed
*/
DLLEXPORT int32_t msgd_client_send_p2p_msg(MSGDCLIHandle handle, char *dstGroup, char *dstUser, int32_t persistTime, char *msg, int32_t msgLen, int32_t msgId);
 
 
/*
* function:  msgd_client_publish_msg
* Target: send publish-msg to the dstGroup, and this client must be TYPE=EN_MSGD_CLIENT_TYPE_PUBLISHER or EN_MSGD_CLIENT_TYPE_BOTH
*
* para: handle---the handle create in msgd_client_init
* dstGroup---the dest group name of publishing
* persistTime---the persistent Time(sec) if publishing dstObj offline
* msg---the msg content  of publishing
* msgLen---the msg content .length of publishing
* msgId-----the id of the special msg
*
* return: =0---successful
*         -1---failed
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
具体接口说明及使用，请下载SDK包参考头文件。

注：ESP8266平台仅实现客户端，前往下载：http://blog.csdn.net/yunjinwang/article/details/52667777



四，结束

由于能力有限，SDK中可能会出现很多问题，针对不同平台或者同平台不同ROM环境下均有可能出现异常，如有bug和建议，请留言或者mail我：wyj0617mail@aliyun.com。

由于基本C语言开发，跨平台特性很好，如有其它平台需要扩展的，也可联系我进行适配。
