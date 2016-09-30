/////////////////////////////////////////////////////////////////////
// File: MigApp.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGAPP/VCS/MIGAPP.CPv   1.0   27 May 1998 17:27:18   SEDWARD  $
//
// Description:
//  This is the main app file for the "migapp.exe" program, which is
//  used to copy over the main NAV executables in a Win95->NT5
//  migration.  It just displays a dialog that informs the user of
//  the need to upgrade to a NT5-specific version.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGAPP/VCS/MIGAPP.CPv  $
// 
//    Rev 1.0   27 May 1998 17:27:18   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "MigApp.h"
#include "MigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMigrateApp

BEGIN_MESSAGE_MAP(CMigrateApp, CWinApp)
	//{{AFX_MSG_MAP(CMigrateApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMigrateApp construction

CMigrateApp::CMigrateApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMigrateApp object

CMigrateApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMigrateApp initialization

BOOL CMigrateApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMigrateDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}