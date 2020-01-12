#ifndef _MSGD_DEMO_CLIENT_H_
#define _MSGD_DEMO_CLIENT_H_

#include <stdio.h>
#include "os_log.h"

#define __func__ __FUNCTION__
#define sleep(x) Sleep(1000*x)
#define usleep(x) Sleep(x/1000)

//#define TAG "MSGD_SVR_MAIN:"
//#define MSGD_SVR_LOGI(fmt, ...) {OS_log(LVL_INFO, 0, TAG"[%s]%s(%d) "fmt, __FILE__,__func__,__LINE__, ##__VA_ARGS__);}
//#define MSGD_SVR_LOGW(fmt, ...) {OS_log(LVL_WRN, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
//#define MSGD_SVR_LOGE(fmt, ...) {OS_log(LVL_ERR, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
//#define MSGD_SVR_LOGT(fmt, ...) {OS_log(LVL_TRACE, 0, TAG"[%s]%s(%d) "fmt, __FILE__, __func__,__LINE__, ##__VA_ARGS__);}

#define TAG "MSGD_SVR_MAIN:"
#define MSGD_DEMO_LOGI(fmt, ...) {printf(TAG"[%s]%s(%d) "fmt"\n", __FILE__,__func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_DEMO_LOGW(fmt, ...) {printf(TAG"[%s]%s(%d) "fmt"\n", __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_DEMO_LOGE(fmt, ...) {printf(TAG"[%s]%s(%d) "fmt"\n", __FILE__, __func__,__LINE__, ##__VA_ARGS__);}
#define MSGD_DEMO_LOGT(fmt, ...) {printf(TAG"[%s]%s(%d) "fmt"\n", __FILE__, __func__,__LINE__, ##__VA_ARGS__);}


// evtType == 0, means connect
// evtType == 1, means disconnect
typedef void (*MSGD_DEMO_CLIENT_EVT_CB)(int evtType);
typedef void (*MSGD_DEMO_CLIENT_MSG_CB)(char *inBuff, int inBuffLen);
void msgd_demo_client_set_mod(int mode);//0--pub, 1--sub, 2---both
int msgd_demo_client_start(char *svrIp, int svrPort, char *group, char *user, MSGD_DEMO_CLIENT_MSG_CB msgFunc, MSGD_DEMO_CLIENT_EVT_CB evtFunc);
int msgd_demo_client_send_msg(char *dstGroup, char *dstUser, int32_t persistTime, char *pMsg, int msgLen, int32_t msgId);
void msgd_demo_client_stop();

#endif