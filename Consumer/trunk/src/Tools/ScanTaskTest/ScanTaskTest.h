////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskTest.h : main header file for the SCANTASKTEST application
//

#if !defined(AFX_SCANTASKTEST_H__6FBB232B_0787_494A_AA4E_AD853E05CDB5__INCLUDED_)
#define AFX_SCANTASKTEST_H__6FBB232B_0787_494A_AA4E_AD853E05CDB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CScanTaskTestApp:
// See ScanTaskTest.cpp for the implementation of this class
//

class CScanTaskTestApp : public CWinApp
{
public:
	CScanTaskTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanTaskTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CScanTaskTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANTASKTEST_H__6FBB232B_0787_494A_AA4E_AD853E05CDB5__INCLUDED_)
