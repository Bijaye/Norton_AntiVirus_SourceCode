// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SAVClientMover.h : main header file for the SAVCLIENTMOVER application
//

#if !defined(AFX_SAVCLIENTMOVER_H__4E34E480_B6CB_4CD7_8A36_82F028768AC6__INCLUDED_)
#define AFX_SAVCLIENTMOVER_H__4E34E480_B6CB_4CD7_8A36_82F028768AC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSAVClientMoverApp:
// See SAVClientMover.cpp for the implementation of this class
//

class CSAVClientMoverApp : public CWinApp
{
public:
	CSAVClientMoverApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSAVClientMoverApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSAVClientMoverApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVCLIENTMOVER_H__4E34E480_B6CB_4CD7_8A36_82F028768AC6__INCLUDED_)
