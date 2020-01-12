#pragma once
#include "afxwin.h"

// MsgdDemoServer dialog

#define MsgdServerUserEventId  1002

class MsgdDemoServer : public CDialogEx
{
	DECLARE_DYNAMIC(MsgdDemoServer)

public:
	MsgdDemoServer(CWnd* pParent = NULL);   // standard constructor
	virtual ~MsgdDemoServer();

// Dialog Data
	enum { IDD = IDD_DIALOG_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnProcessUserEvent(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonServerStart();
	UINT m_serverConnectionNum;
	UINT m_serverListenport;
};
