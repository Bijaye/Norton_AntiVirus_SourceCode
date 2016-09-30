////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// StatusTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

// For SymInterface
#define INITIIDS
#include "AvEvents.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "NavEventFactoryLoader.h"
#include "NAVEventCommon.h"
#include "EMSubscriber.h"
#include "NSCLoader.h"

#include "StatusTest.h"
#include "StatusTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("StatusTest"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Module lifetime management for trust cache
#include "ccSymModuleLifetimeMgrHelper.h"
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

#include "ccCoInitialize.h"

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

// CStatusTestApp

BEGIN_MESSAGE_MAP(CStatusTestApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CStatusTestApp construction

CStatusTestApp::CStatusTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CStatusTestApp object

CStatusTestApp theApp;


// CStatusTestApp initialization

BOOL CStatusTestApp::InitInstance()
{
    ccLib::CCoInitialize COM (ccLib::CCoInitialize::eSTAModel);

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CStatusTestDlg dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
