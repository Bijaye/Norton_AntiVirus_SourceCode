////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// SessionAppClient.cpp : main source file for SessionAppClient.exe
//

#define INITIIDS
// (precompiled headers turned off for this file)
#include "stdafx.h"

#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

#include "SessionAppClientImpl.h"

#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccSymDelayLoader.h"

CAppModule _Module;

///////////////////////////////////////////////////////////////////////////////
// Initialize module lifetime manager
cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);

///////////////////////////////////////////////////////////////////////////////
// Initialize debug output
ccSym::CDebugOutput g_Log(_T("AVPAPP32"));
IMPLEMENT_CCTRACE(::g_Log);

///////////////////////////////////////////////////////////////////////////////
// ccDelayLoader implementation
ccSym::CDelayLoader g_DelayLoader;

///////////////////////////////////////////////////////////////////////////////
// DEP safe ATL thunk
ATL::CDynamicStdCallThunk::CThunksHeap ATL::CDynamicStdCallThunk::s_ThunkHeap;

///////////////////////////////////////////////////////////////////////////////
// The SymInterface object map for this module.
SYM_OBJECT_MAP_BEGIN()                  
SYM_OBJECT_MAP_END()                            

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
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
