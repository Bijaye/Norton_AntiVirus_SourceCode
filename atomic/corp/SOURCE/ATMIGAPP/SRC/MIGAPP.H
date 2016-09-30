/////////////////////////////////////////////////////////////////////
// File: MigApp.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGAPP/VCS/MIGAPP.H_v   1.0   27 May 1998 17:27:28   SEDWARD  $
//
// Description:
//  This is the header file for the "migapp.exe" program, which is
//  used to copy over the main NAV executables in a Win95->NT5
//  migration.  It just displays a dialog that informs the user of
//  the need to upgrade to a NT5-specific version.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGAPP/VCS/MIGAPP.H_v  $
// 
//    Rev 1.0   27 May 1998 17:27:28   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#if !defined(AFX_MIGAPP_H__EB9F5099_659D_11D1_9D4A_00C04FB906C0__INCLUDED_)
#define AFX_MIGAPP_H__EB9F5099_659D_11D1_9D4A_00C04FB906C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMigrateApp:
// See MigApp.cpp for the implementation of this class
//

class CMigrateApp : public CWinApp
{
public:
	CMigrateApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMigrateApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMigrateApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIGAPP_H__EB9F5099_659D_11D1_9D4A_00C04FB906C0__INCLUDED_)