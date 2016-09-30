//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// qconsole.h : main header file for the QCONSOLE application
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/qconsole.h_v  $
// 
//    Rev 1.3   01 Jul 1998 18:11:26   VGLOSTE
// Made the function IsTrialPeriodOverridden() a member function instead 
// of a static function.
// 
//    Rev 1.2   11 Jun 1998 15:48:26   DBuches
// Added support for trial ware.
// 
//    Rev 1.1   28 Apr 1998 15:14:42   DBuches
// Added single instance support.  Also hooked up NAV about box.
// 
//    Rev 1.0   27 Feb 1998 15:10:26   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QCONSOLE_H__396DC359_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
#define AFX_QCONSOLE_H__396DC359_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"       // main symbols
#include "qconres.h"


/////////////////////////////////////////////////////////////////////////////
// CQconsoleApp:
// See qconsole.cpp for the implementation of this class
//

class CQconsoleApp : public CWinApp
{
public:
	CQconsoleApp();
    BOOL IsQConsoleEnabled(){ return m_bQConsoleEnabled; }

    int m_nShowCmd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQconsoleApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CQconsoleApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool IsCfgWizFinished();
	BOOL LaunchCfgWiz();

    HANDLE  m_hMutex;
    BOOL    m_bQConsoleEnabled;
	CString m_sAppName;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONSOLE_H__396DC359_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
