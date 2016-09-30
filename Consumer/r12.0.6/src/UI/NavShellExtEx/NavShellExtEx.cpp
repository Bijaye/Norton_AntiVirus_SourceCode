// NavShellExtEx.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "NavShellExtEx.h"
#include "dlldatax.h"


class CNavShellExtExModule : public CAtlDllModuleT< CNavShellExtExModule >
{
public :
	DECLARE_LIBID(LIBID_NavShellExtExLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NAVSHELLEXTEX, "{50FBD810-BC18-42A9-B2D4-0E8352AA8CA5}")
};

CNavShellExtExModule _AtlModule;
//#include "ccSymDebugOutput.h"
//ccSym::CDebugOutput g_DebugOutput(_T("NavShellExtEx"));
//IMPLEMENT_CCTRACE(g_DebugOutput);

#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	g_Log.LogT("DllMain(%d)", dwReason);

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
	g_Log.LogT("DllCanUnloadNow()");

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
	g_Log.LogT("DllGetClassObject()");

#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
	g_Log.LogT("DllRegisterServer()");

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
	g_Log.LogT("DllUnregisterServer()");

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

