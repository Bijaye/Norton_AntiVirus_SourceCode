/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/n32opts.h_v   1.4   19 Feb 1998 11:57:16   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
//
// n32opts.h : main header file for the N32OPTS DLL
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/n32opts.h_v  $
// 
//    Rev 1.4   19 Feb 1998 11:57:16   mdunn
// Ported NEC changes from ROSNEC.
// 
//    Rev 1.1   16 Sep 1997 19:55:58   MDUNN
// Added the IS_NEC() function to the app class.
// 
//    Rev 1.3   11 Jul 1997 16:56:02   DBUCHES
// Few changes for Savanna folks.  Now, clients can specify whether to use
// the NT specific pages."
// 
//    Rev 1.2   20 Jun 1997 12:03:52   DBUCHES
// Added help support.
// 
//    Rev 1.1   27 May 1997 22:19:14   DBUCHES
// Added support for dynamically loading ALERTUI.DLL
// 
//    Rev 1.0   14 May 1997 15:02:36   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_N32OPTS_H__0235E9C4_C1A7_11D0_8AD5_0060977341AC__INCLUDED_)
#define AFX_N32OPTS_H__0235E9C4_C1A7_11D0_8AD5_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CN32optsApp
// See n32opts.cpp for the implementation of this class
//

class CN32optsApp : public CWinApp
{
public:
	void SetNTFlag(BOOL bFlag);
	BOOL IsNT();
    BOOL IsNEC();
	CN32optsApp();
    HINSTANCE m_hInstAlertUI;
    CWnd *m_pOptionsWindow;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CN32optsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CN32optsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bUseNTStuff;
};


/////////////////////////////////////////////////////////////////////////////
// Usefull global macros.

#define IS_NT()(((CN32optsApp*)AfxGetApp())->IsNT())
#define IS_NEC()(((CN32optsApp*)AfxGetApp())->IsNEC())
#define ALERTUI_INSTANCE()(((CN32optsApp*)AfxGetApp())->m_hInstAlertUI )


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_N32OPTS_H__0235E9C4_C1A7_11D0_8AD5_0060977341AC__INCLUDED_)
