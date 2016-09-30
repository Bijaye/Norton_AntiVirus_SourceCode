// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SAVClientMover.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <afxtempl.h>
#include "MoveResults.h"
#include "vpcommon.h"
#include "SAVClientMover.h"
#include "SAVClientMoverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSAVClientMoverApp

BEGIN_MESSAGE_MAP(CSAVClientMoverApp, CWinApp)
//{{AFX_MSG_MAP(CSAVClientMoverApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG
ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSAVClientMoverApp construction

CSAVClientMoverApp::CSAVClientMoverApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSAVClientMoverApp object

CSAVClientMoverApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSAVClientMoverApp initialization

BOOL CSAVClientMoverApp::InitInstance()
{
    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    CSAVClientMoverDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if ( nResponse == IDOK )
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if ( nResponse == IDCANCEL )
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
