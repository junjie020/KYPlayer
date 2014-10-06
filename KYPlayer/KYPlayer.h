
// KYPlayer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKYPlayerApp:
// See KYPlayer.cpp for the implementation of this class
//

class CKYPlayerApp : public CWinApp
{
public:
	CKYPlayerApp();

// Overrides
public:
	virtual BOOL InitInstance();

	virtual BOOL OnIdle(LONG lCount);

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CKYPlayerApp theApp;