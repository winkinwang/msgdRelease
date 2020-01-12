
// Msgd_Demo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMsgdDemoApp:
// See Msgd_Demo.cpp for the implementation of this class
//

class CMsgdDemoApp : public CWinApp
{
public:
	CMsgdDemoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMsgdDemoApp theApp;