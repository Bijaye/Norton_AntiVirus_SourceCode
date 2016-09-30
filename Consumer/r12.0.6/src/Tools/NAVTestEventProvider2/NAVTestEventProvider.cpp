// NAVTestEventProvider.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NAVTestEventProvider.h"
#include "NAVTestEventProviderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include "ccLibStatic.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVTestEventProvider"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Crash handler
#include "ccSymCrashHandler.h"
ccSym::CCrashHandler g_CrashHandler;

// CNAVTestEventProviderApp

BEGIN_MESSAGE_MAP(CNAVTestEventProviderApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNAVTestEventProviderApp construction

CNAVTestEventProviderApp::CNAVTestEventProviderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CNAVTestEventProviderApp object

CNAVTestEventProviderApp theApp;


// CNAVTestEventProviderApp initialization

BOOL CNAVTestEventProviderApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

    // Attempt to initialize COM - MTA
    //
	HRESULT hrCOMInit = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    bool bCOMInitialized = false;

    if ( hrCOMInit != S_FALSE ) // already init
    {
        if ( hrCOMInit == S_OK )
            bCOMInitialized = true;
        else
        {
            if( FAILED (hrCOMInit))
            {
                CCTRACEE ( "CProviderManager::DoWork () failed to COINIT_MULTITHREADED" );
                return FALSE;
            }
        }
    }

	CNAVTestEventProviderDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
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

    if ( bCOMInitialized )
        CoUninitialize ();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
