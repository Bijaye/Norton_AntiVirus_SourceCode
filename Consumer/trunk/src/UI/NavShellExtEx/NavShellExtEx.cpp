////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NavShellExtEx.cpp : Implementation of DLL Exports.


#include "stdafx.h"

#define INITIIDS
#include "ccVerifyTrustInterface.h"

#include "resource.h"
#include "NAVShellExtEx_h.h"
#include "dlldatax.h"
#include "NavShellExtEx_i.c"


//Resource loading
#include "..\\NavShellExtExRes\\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL L"NavShExt.loc"
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

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

class CNavShellExtExModule : public CAtlDllModuleT< CNavShellExtExModule >
{
public :
	DECLARE_LIBID(LIBID_NavShellExtExLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NAVSHELLEXTEX, "{50FBD810-BC18-42A9-B2D4-0E8352AA8CA5}")
};

CNavShellExtExModule _AtlModule;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	CCTRACEI(_T("DllMain(%d)"), dwReason);

    if (DLL_PROCESS_ATTACH == dwReason)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hInstance);
    }

#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif




// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
	CCTRACEI(_T("DllCanUnloadNow()"));

#ifdef _MERGE_PROXYSTUB
    HRESULT hr = PrxDllCanUnloadNow();
    if (FAILED(hr))
        return hr;
#endif
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	//  No trust check. (Loaded into non-SYMC processes.)

	CCTRACEI(_T("DllGetClassObject()"));

	_InitResources();

#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	CCTRACEI(_T("DllRegisterServer()"));

	_InitResources();

    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
#endif
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	CCTRACEI(_T("DllUnregisterServer()"));

	HRESULT hr = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
    if (FAILED(hr))
        return hr;
    hr = PrxDllRegisterServer();
    if (FAILED(hr))
        return hr;
    hr = PrxDllUnregisterServer();
#endif
	return hr;
}

