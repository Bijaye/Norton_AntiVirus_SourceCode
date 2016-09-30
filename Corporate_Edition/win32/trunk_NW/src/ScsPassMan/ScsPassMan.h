// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScsPassMan.h : main header file for the ScsPassMan application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CScsPassManApp:
// See ScsPassMan.cpp for the implementation of this class
//

class CScsPassManApp : public CWinApp
{
public:
	CScsPassManApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CScsPassManApp theApp;