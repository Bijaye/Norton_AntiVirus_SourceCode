////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TriggerEvent.h : main header file for the TRIGGEREVENT application
//

#if !defined(AFX_TRIGGEREVENT_H__C5880BA5_561F_11D5_AE6C_00E02995804B__INCLUDED_)
#define AFX_TRIGGEREVENT_H__C5880BA5_561F_11D5_AE6C_00E02995804B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTriggerEventApp:
// See TriggerEvent.cpp for the implementation of this class
//

class CTriggerEventApp : public CWinApp
{
public:
	CTriggerEventApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriggerEventApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTriggerEventApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGGEREVENT_H__C5880BA5_561F_11D5_AE6C_00E02995804B__INCLUDED_)
