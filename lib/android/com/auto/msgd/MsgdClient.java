package com.auto.msgd;

import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

import static com.auto.msgd.MsgdClient.MSGSUB_EVT_EN.EN_MSGSUB_EVT_RECONNECTING;

/**
 * Created by yunjinwang on 2016/7/12.
 */
public class MsgdClient {
    private final static String TAG = "MsgdClientJava";
    private static MsgdClient gmMsgdClient = null;
    private onMsgdClientListener gMsgdClientListener = null;
    private int gCurStatus = 0; //0--disconnect, 1--connecting, 2---connected,3---reconnecting,4---disconnect
    private Timer gTimer = null;
    private TimerTask gTimerTask = null;
    private String gHostIp = "192.168.0.105";
    private int gHostPort = 7896;
    private boolean gFlagReconnect = false;

    private MsgdClient() {
        initMsgdClient();
        gCurStatus = 0;
        gTimer = new Timer();
    }

    static public MsgdClient getInstence() {
        if (gmMsgdClient == null)
            gmMsgdClient = new MsgdClient();

        return gmMsgdClient;
    }

    static {
        System.loadLibrary("MsgdJni");//导入生成的链接库文件
    }

    private native int initMsgdClient();
    private native int uninitMsgdClient();
    private native int startMsgdClient(String hostIP, int hostPort);//本地方法
    private native String stopMsgdClient();
    private native int setMsgdClientTopic(String group, String userName);
    private native int setMsgdClientKeepalive(int intralValueSec, String aliveMsg, int msgLen);

    public void setListener(onMsgdClientListener msgListener) {
        gMsgdClientListener = msgListener;
    }

    public void setTopicInfo(String group, String userName) {
        setMsgdClientTopic(group, userName);
    }

    public void setKeepaliveInfo(int intralValueSec, String aliveMsg, int msgLen) {
        setMsgdClientKeepalive(intralValueSec, aliveMsg, msgLen);
    }


    public boolean start(String hostIp, int port, boolean flagReconnect) {
        //String gHostIp = "192.168.0.105";
        //int gHostPort = 7896;
        gHostIp = hostIp;
        gHostPort = port;
        Log.i(TAG, "start hostIp:"+gHostIp+" port:"+gHostPort);
        gCurStatus = 1;
        stopMsgdClient();
        int ret = startMsgdClient(gHostIp, gHostPort);
        if (ret != 0) {
            gCurStatus = 4;
            Log.i(TAG, "reconnect sever failed and reconnect");
            if (gCurStatus >= 3) {
                scheduleReconnect();
            }
        }
        gFlagReconnect = flagReconnect;

        return true;
    }

    public boolean stop() {
        gCurStatus = 0;
        gFlagReconnect = false;

        if (gTimerTask != null) {
            gTimerTask.cancel();
        }
        stopMsgdClient();
        return true;
    }

    public void onReceivedMessageListener(String msg) {
        Log.i(TAG, "onReceivedMessageListener:"+msg);
        if (gMsgdClientListener != null)
            gMsgdClientListener.onMsgReceive(msg);
    }

    public enum MSGSUB_EVT_EN{
        EN_MSGSUB_EVT_CONNECTED,
        EN_MSGSUB_EVT_DISCONNECTED,
        EN_MSGSUB_EVT_KEEPALIVE,
        EN_MSGSUB_EVT_RECONNECTING,
        EN_MSGSUB_EVT_UNKNOWN
    };
    public void onReceivedEventListener(int evtCode) {
        Log.i(TAG, "onReceivedEventListener:"+evtCode);
        MSGSUB_EVT_EN evtType = MSGSUB_EVT_EN.EN_MSGSUB_EVT_UNKNOWN;
        if (evtCode == 0x2000) {
            gCurStatus = 2;
            evtType = MSGSUB_EVT_EN.EN_MSGSUB_EVT_CONNECTED;
        } else if (evtCode == 0x2001) {
            evtType = MSGSUB_EVT_EN.EN_MSGSUB_EVT_KEEPALIVE;
        } else if (evtCode == 0x2002) {
            Log.i(TAG, "onReceivedEventListener and scheduleReconnect");
            //if (gCurStatus != 0) {
                scheduleReconnect();
            //}
            gCurStatus = 4;
            evtType = MSGSUB_EVT_EN.EN_MSGSUB_EVT_DISCONNECTED;
        } else {
            Log.i(TAG, "onReceivedEventListener:"+evtCode+" invalid");
            return;
        }

        if (gMsgdClientListener != null)
            gMsgdClientListener.onEvent(evtType);
    }

    private void scheduleReconnect() {
        //Log.i(TAG, "enter reconnect sever....");
        if (!gFlagReconnect) {
            Log.i(TAG, "exit scheduleReconnect function");
            return;
        }

        if (gTimerTask != null) {
            gTimerTask.cancel();
        }
        gTimerTask = new TimerTask() {
            @Override
            public void run() {
                if (gCurStatus >= 3) {
                    if (gMsgdClientListener != null)
                        gMsgdClientListener.onEvent(EN_MSGSUB_EVT_RECONNECTING);

                    stopMsgdClient();
                    if (!gFlagReconnect) {
                        Log.i(TAG, "exit scheduleReconnect function2");
                        return;
                    }
                    gCurStatus = 3;
//                    if (gCurStatus >= 3) {
//                        scheduleReconnect();
//                    }
                    Log.i(TAG, "reconnect to startMsgdClient1");
                    int ret = startMsgdClient(gHostIp, gHostPort);
                    Log.i(TAG, "reconnect to startMsgdClient2");
                    if (ret != 0) {
                        Log.i(TAG, "reconnect sever failed and reconnect");
                        if (gCurStatus >= 3) {
                            scheduleReconnect();
                        }
                    } else  {
                        if (gCurStatus >= 3) {
                            scheduleReconnect();
                        }
                    }
                }
            }
        };
        gTimer.schedule(gTimerTask, 3000);
        Log.i(TAG, "schedule to reconnect sever after 3s");
    }

    public interface onMsgdClientListener{
        public void onEvent(MSGSUB_EVT_EN evtType);
        public void onMsgReceive(String msg);
    }
}
