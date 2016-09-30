// SSPUnitTest.cpp : main source file for SSPUnitTest.exe
//
#define INITIIDS
#include "stdafx.h"

#include "resource.h"

// Exception Helpers
#include "ccExceptionInfo.h"
#include "ccCatch.h"


// ccEvtMgr headers
#include "ccEventManagerHelper.h"
#include "ccSymMultiEventFactoryHelper.h"
#include "ccProSubLoader.h"
#include "ccProviderHelper.h"
#include "ccLogManagerHelper.h"

#include "ccAlertInterface.h"
#include "ccAlertLoader.h"

#include "HPPEventsInterface.h"
#include "HPPUtilityInterface.h"

#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"

#include "aboutdlg.h"
#include "RegKeyMonitor.h"
#include "MainDlg.h"

CAppModule _Module;

// TRACE implementation
ccSym::CDebugOutput g_Log(_T("SSPUnitTest"));
IMPLEMENT_CCTRACE(::g_Log);

// ccCrashHandler implementation 
#include "ccSymCrashHandler.h"
ccSym::CCrashHandler g_CrashHandler;

// The SymInterface object map for this module.
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            


int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    // Enable the crash handler
    if (g_CrashHandler.LoadOptions() == FALSE)
    {
        CCTRACEE(_T("WinMain() : g_CrashHandler.LoadOptions() == FALSE"));
    }

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
