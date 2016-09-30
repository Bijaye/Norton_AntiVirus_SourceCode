// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScsPassMan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DarwinResCommon.h"
#include "ScsPassMan.h"
#include "ScsPassManDlg.h"
#include "WelcomeWizardPage.h"
#include "ConnectWizardPage.h"
#include "PasswordManageWizardPage.h"
#include "FinishWizardPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DARWIN_DECLARE_RESOURCE_LOADER(_T("ScsPassManRes.dll"), _T("ScsPassMan"))

// CScsPassManApp

BEGIN_MESSAGE_MAP(CScsPassManApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CScsPassManApp construction

CScsPassManApp::CScsPassManApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CScsPassManApp object

CScsPassManApp theApp;


// CScsPassManApp initialization

BOOL CScsPassManApp::InitInstance()
{
	if(SUCCEEDED(g_ResLoader.Initialize()))
	{
		AfxSetResourceHandle(g_ResLoader.GetResourceInstance());
	}
	else
	{
		return FALSE;
	}
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

	CScsPassManDlg dlg;
	CWelcomeWizardPage welcomeWizardPage;
	CConnectWizardPage connectWizardPage;
	CPasswordManageWizardPage passwordManageWizardPage;
	CFinishWizardPage finishWizardPage;

	dlg.AddPage( &welcomeWizardPage, CWelcomeWizardPage::IDD );
	dlg.AddPage( &connectWizardPage, CConnectWizardPage::IDD );
	dlg.AddPage( &passwordManageWizardPage, CPasswordManageWizardPage::IDD );
	dlg.AddPage( &finishWizardPage, CFinishWizardPage::IDD );

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
