////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AVScanner.cpp : Defines the entry point for the DLL application.
//


#define INITIIDS

#include "stdafx.h"
#include "AVScanObject.h"
#include "ccInstanceFactory.h"
#include "ccResourceLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include <stdio.h>
#include <string>

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("AvPreScn"));
IMPLEMENT_CCTRACE(::g_DebugOutput);


cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);

cc::CResourceLoader g_resources(&_AtlBaseModule, _T("AvPreScn.loc"));

ATL::CDynamicStdCallThunk::CThunksHeap ATL::CDynamicStdCallThunk::s_ThunkHeap;

HMODULE g_hModule						= NULL;
const TCHAR NULL_CHAR					= _T('\0');
const TCHAR DIRECTORY_DELIMITER_CHAR	= _T('\\');


using namespace std;

namespace {
	static ccLib::CCriticalSection s_cs;
	static bool InitResources()
	{
		bool bRet = true;
		ccLib::CSingleLock lk(&s_cs, INFINITE, false);
		if(NULL == g_resources.GetResourceInstance())
		{
			if(g_resources.Initialize())
				_AtlBaseModule.SetResourceInstance(g_resources.GetResourceInstance());
			else
			{
				CCTRACEE(_T("Failed to Initialize resources."));
				bRet = false;
			}
		}
		return bRet;
	}
}

SYM_OBJECT_MAP_BEGIN()    
	if(!InitResources()) return SYMERR_INVALID_FILE;
	SYM_OBJECT_ENTRY( CLSID_AVScanObject, CAVScanObject )
	SYM_PREINST_ENUM_ENTRY();
SYM_OBJECT_MAP_END()   

SYM_PREINST_SCANNER_MAP_BEGIN()
	SYM_PREINST_SCANNER_ENTRY(CLSID_AVScanObject)
SYM_PREINST_SCANNER_MAP_END()


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if ( DLL_PROCESS_ATTACH == ul_reason_for_call )
	{
#ifdef _DEBUG
		::MessageBox( NULL, _T("DllMain: Attach to me"), _T("AvPreScn.dll"), MB_OK | MB_ICONINFORMATION );
#endif	// _DEBUG

		g_hModule = (HMODULE) hModule;

		// Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
		VERIFY( ::DisableThreadLibraryCalls((HMODULE) hModule) );
	}
	else if ( DLL_PROCESS_DETACH == ul_reason_for_call )
	{
		// Have all objects implemented in this DLL been destroyed?
		ASSERT( 0 == g_DLLObjectCount );
	}

	return TRUE;
}


// __stdcall - for RunDll compatibility:
int CALLBACK PostRebootProc( HWND hWnd, HINSTANCE /* hInst */, LPCTSTR lpszCmdLine, int /* nCmdShow */ )
{
	CCTRCTXE0(_T("NOT_IMPLEMENTED"));
	return E_NOTIMPL;
}
