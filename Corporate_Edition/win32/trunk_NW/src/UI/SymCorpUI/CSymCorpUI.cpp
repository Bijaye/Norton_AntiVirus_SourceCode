// CSymCorpUI.cpp : Implementation of CSymCorpUI

#include "stdafx.h"
#include "CSymCorpUI.h"
#include "SymCorpUIApp.h"
#include "TestHarnessDlg.h"

// CSymCorpUI
STDMETHODIMP CSymCorpUI::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ISymCorpUI
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

// ISymCorpUI
STDMETHODIMP CSymCorpUI::ShowUI( DWORD userArg )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

//    CTestHarnessDlg testHarness;

//    testHarness.DoModal();
    CSymCorpUIApp* thisApp = (CSymCorpUIApp*) AfxGetApp();
    if (thisApp != NULL)
    {
        // We don't register, unregister, print, etc., so no need for ParseCommandLine/ProcessShellCommand
        /*
        // Send a File|New command through
	    CCommandLineInfo cmdInfo;
	    thisApp->ParseCommandLine(cmdInfo);
	    if (!thisApp->ProcessShellCommand(cmdInfo))
		    return FALSE;
*/
        thisApp->OnFileNew();

	    // Show the main frame
        CWnd* mainWindow = thisApp->GetMainWnd();
        mainWindow->ShowWindow(SW_SHOW);
	    mainWindow->UpdateWindow();
        thisApp->Run();
    }
    return S_OK;
}
