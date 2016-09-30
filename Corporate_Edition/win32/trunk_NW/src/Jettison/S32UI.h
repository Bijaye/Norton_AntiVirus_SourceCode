// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// S32UI.h : main header file for the S32UI application
//

#if !defined(AFX_S32UI_H__8905E01B_E911_11D1_A75E_0000E8D3EFD6__INCLUDED_)
#define AFX_S32UI_H__8905E01B_E911_11D1_A75E_0000E8D3EFD6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CS32UIApp:
// See S32UI.cpp for the implementation of this class
//

class CS32UIApp : public CWinApp
{
public:
	CS32UIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CS32UIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CS32UIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_S32UI_H__8905E01B_E911_11D1_A75E_0000E8D3EFD6__INCLUDED_)
