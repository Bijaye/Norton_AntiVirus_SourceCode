// UI Test Harness.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "UI Test Harness.h"
#include "UI Test HarnessDlg.h"
#import "SymCorpUI.tlb" raw_interfaces_only
#include "SymCorpUI.h"
#include <StrSafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUITestHarnessApp
BEGIN_MESSAGE_MAP(CUITestHarnessApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUITestHarnessApp construction
CUITestHarnessApp::CUITestHarnessApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CUITestHarnessApp object
CUITestHarnessApp theApp;


// CUITestHarnessApp initialization
BOOL CUITestHarnessApp::InitInstance()
{
    SymCorpUILib::ISymCorpUIPtr     symCorpUI;
    TCHAR                           errorMessage[MAX_PATH]      = _T("");
    HRESULT                         returnValHR                 = E_FAIL;

    returnValHR = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(returnValHR))
    {
        StringCchPrintf(errorMessage, sizeof(errorMessage)/sizeof(errorMessage[0]), _T("Error 0x%08x initializing COM."), returnValHR);
        MessageBox(NULL, errorMessage, _T("Error"), MB_OK | MB_ICONINFORMATION);
    }
    // InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
    // Enable embedding of ActiveX controls
	AfxEnableControlContainer();

    returnValHR = symCorpUI.CreateInstance(CLSID_SymCorpUI);
    if (SUCCEEDED(returnValHR))
    {
        symCorpUI->ShowUI(NULL);
        symCorpUI.Release();
    }
    else
    {
        StringCchPrintf(errorMessage, sizeof(errorMessage)/sizeof(errorMessage[0]), _T("Error 0x%08x creating CLSID_SymCorpUI object."), returnValHR);
        MessageBox(NULL, errorMessage, _T("Error"), MB_OK | MB_ICONINFORMATION);
    }

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}