////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVTaskWizard.h : main header file for the NAVTASKWIZARD DLL
//

#if !defined(AFX_NAVTASKWIZARD_H__0C4E996A_1073_48BC_A2A1_D209F0F154FF__INCLUDED_)
#define AFX_NAVTASKWIZARD_H__0C4E996A_1073_48BC_A2A1_D209F0F154FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "..\\NavTaskWizardRes\\resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNAVTaskWizardApp
// See NAVTaskWizard.cpp for the implementation of this class
//

class CNAVTaskWizardApp : public CWinApp
{
public:
	CNAVTaskWizardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNAVTaskWizardApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNAVTaskWizardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAVTASKWIZARD_H__0C4E996A_1073_48BC_A2A1_D209F0F154FF__INCLUDED_)
