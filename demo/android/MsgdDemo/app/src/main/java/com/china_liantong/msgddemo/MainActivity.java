package com.china_liantong.msgddemo;

import android.app.ProgressDialog;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.auto.msgd.MsgdClient;

import java.net.InetAddress;
import java.net.UnknownHostException;

import static com.auto.msgd.MsgdClient.MSGSUB_EVT_EN.EN_MSGSUB_EVT_CONNECTED;
import static com.auto.msgd.MsgdClient.MSGSUB_EVT_EN.EN_MSGSUB_EVT_DISCONNECTED;
import static com.auto.msgd.MsgdClient.MSGSUB_EVT_EN.EN_MSGSUB_EVT_RECONNECTING;

public class MainActivity extends AppCompatActivity implements MsgdClient.onMsgdClientListener {
    final String TAG = "MSGDMAIN";
    private TextView mRecvMsgTextView = null;
    private EditText mServerIpEditText = null;
    private EditText mServerPortEditText = null;
    private EditText mUserGroupEditText = null;
    private EditText mUserNameEditText = null;
    private Button mStartBtn = null;
    private Button mClearBtn = null;
    ScrollView mScroll = null;
    private boolean mFlagStarted = false;
    private boolean mFlagPressed = false;
    private ProgressDialog mProgressDialog = null;
    private String recvMsg = "";


    Handler mHandler = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        init_view();
    }

    private String getServerIpByHostName(String hostName) {
        InetAddress address = null;
        String autoHostIp = null;
        try {
            //address = InetAddress.getByName("www.autoinhome.com");
            address = InetAddress.getByName(hostName);
        } catch (UnknownHostException e) {
            e.printStackTrace();
            return null;
        }
        if (address != null) {
            autoHostIp = address.getHostAddress();
            Log.d(TAG, "autoHostIp ip:"+autoHostIp);
        }
        return autoHostIp;
    }
    private void init_view()
    {
        mProgressDialog = new ProgressDialog(this);
        mRecvMsgTextView = (TextView)findViewById(R.id.recvmsgTextView);
        mServerIpEditText = (EditText)findViewById(R.id.serverIpEdit);
        mServerIpEditText.setText("www.smartinhome.cn");
        mServerPortEditText = (EditText)findViewById(R.id.serverPortEdit);
        mServerPortEditText.setText("7000");
        mUserGroupEditText = (EditText)findViewById(R.id.userGroupEdit);
        mUserGroupEditText.setText("auto");
        mUserNameEditText = (EditText)findViewById(R.id.userNameEdit);
        mUserNameEditText.setText("jnidemo");
        mStartBtn = (Button) findViewById(R.id.startBtn);
        mStartBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mFlagPressed)
                {
                    MsgdClient.getInstence().stop();
//                    new Thread(new Runnable() {
//                        @Override
//                        public void run() {
//                            MsgdClient.getInstence().stop();
//                        }
//                    }).start();
                    mStartBtn.setText("start");
                    mServerIpEditText.setFocusableInTouchMode(true);
                    mServerPortEditText.setFocusableInTouchMode(true);
                    mUserGroupEditText.setFocusableInTouchMode(true);
                    mUserNameEditText.setFocusableInTouchMode(true);

                    mServerIpEditText.setFocusable(true);
                    mServerPortEditText.setFocusable(true);
                    mUserGroupEditText.setFocusable(true);
                    mUserNameEditText.setFocusable(true);

                    mFlagPressed = false;
                } else {
                    if ((mServerIpEditText.length() <= 0)
                            || (mServerPortEditText.length() <= 0)
                            || (mUserGroupEditText.length() <= 0)
                            || (mUserNameEditText.length() <= 0)) {
                        Log.e(TAG, "please input the para");
                        return;
                    }
                    mFlagPressed = true;
                    mStartBtn.setText("stop");
                    mServerIpEditText.setFocusable(false);
                    mServerPortEditText.setFocusable(false);
                    mUserGroupEditText.setFocusable(false);
                    mUserNameEditText.setFocusable(false);

                    new Thread(new StartRunnable()).start();
                    mProgressDialog = ProgressDialog.show(MainActivity.this, "Connect to server...", "Please wait...", true, false);
                    mProgressDialog.show();
                    hideProgressDlg(5);
                }
            }
        });
        mClearBtn = (Button) findViewById(R.id.clearBtn);
        mClearBtn.setOnClickListener(new View.OnClickListener() {
             @Override
             public void onClick(View v) {
                 recvMsg = "";
                 mRecvMsgTextView.setText("");
             }
         });
        mScroll = (ScrollView)findViewById(R.id.sv_show);
    }

    class StartRunnable implements Runnable {
        @Override
        public void run() {
            Log.e(TAG,"StartRunnable enter");
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            Log.e(TAG,"StartRunnable 2");
            String hostName = mServerIpEditText.getText().toString();
            String serverIp = getServerIpByHostName(hostName);
            if (serverIp == null) {
                Log.e(TAG, "get ip by hostName failed:"+hostName);
                hideProgressDlg(0);
                return;
            }
            MsgdClient msgdClient = MsgdClient.getInstence();
            msgdClient.setListener(MainActivity.this);
            String keepAliveMsg = "msgdDemoKa";
            msgdClient.setKeepaliveInfo(20, keepAliveMsg, keepAliveMsg.length());
            //msgdClient.setTopicInfo("auto", "jnidemo");
            String userGrp = mUserGroupEditText.getText().toString();
            String userName = mUserNameEditText.getText().toString();
            msgdClient.setTopicInfo(userGrp, userName);
            String serverPortStr = mServerPortEditText.getText().toString();
            int serverPort = Integer.valueOf(serverPortStr);
            msgdClient.start(serverIp, serverPort, true);
            //mStartBtn.setText("stop");
            //mFlagStarted = true;
            Log.i(TAG, "userGrp:"+userGrp+" userName:"+userName+" serverIp:"+serverIp+" port:"+serverPort);
        }
    }

    @Override
    protected void onStart()
    {
        super.onStart();

        mHandler = new Handler();
    }

    @Override
    protected void onStop()
    {
        super.onStop();
    }

    private void hideProgressDlg(int sec) {
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mProgressDialog.dismiss();
                    }
                });
            }
        }, sec*1000);
    }
    int mEvtFlag = 0;
    @Override
    public void onEvent(MsgdClient.MSGSUB_EVT_EN evtType) {
        Log.i(TAG, "recv msgd event:"+evtType);
        if (evtType == EN_MSGSUB_EVT_DISCONNECTED){
            mEvtFlag = 1;
            mFlagStarted = false;
        } else if (evtType == EN_MSGSUB_EVT_CONNECTED){
            mEvtFlag = 2;
            mFlagStarted = true;
        } else if (evtType == EN_MSGSUB_EVT_RECONNECTING){
            mEvtFlag = 3;
            mFlagStarted = false;
        }
        recvMsg += evtType+"\n";
        updateMsg();
    }

    @Override
    public void onMsgReceive(String msg) {
        Log.i(TAG, "recv msgd msg:"+msg);
        recvMsg += msg+"\n";
        updateMsg();
    }

    private void updateMsg()
    {
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mRecvMsgTextView.setText(recvMsg);
                        // 内层高度超过外层
                        int offset = mRecvMsgTextView.getMeasuredHeight()
                                - mScroll.getMeasuredHeight();
                        if (offset < 0) {
                            offset = 0;
                        }
                        mScroll.scrollTo(0, offset);
                        //Log.i(TAG, "scrollTo:"+offset);
                        if (mEvtFlag>0) {
                            if (mFlagStarted) {
                                //mStartBtn.setText("stop");
                                Toast.makeText(MainActivity.this, "connect to server success", Toast.LENGTH_LONG).show();
                            } else {
                                //MsgdClient.getInstence().stop();
                                //mStartBtn.setText("start");
                                if (mEvtFlag==2) {
                                    Toast.makeText(MainActivity.this, "reconnect to server", Toast.LENGTH_LONG).show();
                                } else {
                                    Toast.makeText(MainActivity.this, "disconnect to server", Toast.LENGTH_LONG).show();
                                }
                            }

                            mEvtFlag = 0;

                            mProgressDialog.dismiss();
                        }
                    }
                });
            }
        },10);
    }
}
