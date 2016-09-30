////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// hammertime.h : main header file for the HAMMERTIME application
//

#if !defined(AFX_HAMMERTIME_H__088F11EA_3E03_4303_A480_CF6E92FF0FA1__INCLUDED_)
#define AFX_HAMMERTIME_H__088F11EA_3E03_4303_A480_CF6E92FF0FA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHammertimeApp:
// See hammertime.cpp for the implementation of this class
//

class CHammertimeApp : public CWinApp
{
public:
	CHammertimeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHammertimeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHammertimeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HAMMERTIME_H__088F11EA_3E03_4303_A480_CF6E92FF0FA1__INCLUDED_)
