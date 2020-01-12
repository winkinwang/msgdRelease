
// MsgdDemoDlg.h : header file
//

#pragma once
#include "afxwin.h"

#define UserEventId  1001

// CMsgdDemoDlg dialog
class CMsgdDemoDlg : public CDialogEx
{
// Construction
public:
	CMsgdDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MSGD_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString mServerDomainName;
	CString mServerIpAddrStr;
	int mServerListenPort;
	CString mLocalGroupName;
	CString mLocalUserName;
	CString mDstGroupName;
	CString mDstUserName;
	CString mSendMsgBuffer;
	afx_msg void OnBnClickedButtonGetDomainIp();
	afx_msg LRESULT OnProcessUserEvent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonGetLocalIp();
	afx_msg void OnBnClickedButtonStartConnectSvr();
	afx_msg void OnBnClickedButtonSendMsg();
	CEdit mMsgdRecvBuffCtr;
	CString mMsgdRecvBuffStr;
	afx_msg void OnBnClickedButtonStartLocalSever();
};
