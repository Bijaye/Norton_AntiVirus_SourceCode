////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NavShellExtHelper.cpp : Implementation of WinMain


#include "stdafx.h"
#include "resource.h"
#include "NavShellExtHelper_h.h"
#include "cltProductHelper.h"

#include "SymInterface.h"

#include "NavShellExtHelper_i.c"

#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLib.h"
#include "ccLibDllLink.h"

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NavShcom"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

//Resource loading
#include "..\\NavShellExtExRes\\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL L"NavShExt.loc"
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

void _InitResources()
{
	if( NULL == g_ResLoader.GetResourceInstance() ) 
	{
		if( g_ResLoader.Initialize() ) 
		{
			_AtlBaseModule.SetResourceInstance( g_ResLoader.GetResourceInstance());

		}
	}
}

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

SIMON::CSimonModule _SimonModule;

class CNavShellExtHelperModule : public CAtlExeModuleT< CNavShellExtHelperModule >
{
public :
	DECLARE_LIBID(LIBID_NavShellExtHelperLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NAVSHELLEXTHELPER, "{ED14BD80-37DB-4A36-BC35-E17A7054038A}")
};

CNavShellExtHelperModule _AtlModule;

//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	_InitResources();

#ifdef IMPERSONATE_CLIENT
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// This provides a NULL DACL which will allow access to everyone.
    CSecurityDescriptor sd;
    sd.InitializeFromThreadToken();
    hr = CoInitializeSecurity(sd, -1, NULL, NULL,
        RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    _ASSERTE(SUCCEEDED(hr));
#endif

	int iRet = _AtlModule.WinMain(nShowCmd);

#ifdef IMPERSONATE_CLIENT
	CoUninitialize();
#endif
	return iRet;
}

