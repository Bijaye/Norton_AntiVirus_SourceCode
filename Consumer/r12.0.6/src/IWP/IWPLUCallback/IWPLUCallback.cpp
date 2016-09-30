// IWPLUCallback.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "IWPLUCallback.h"

class CIWPLUCallbackModule : public CAtlDllModuleT< CIWPLUCallbackModule >
{
public :
	DECLARE_LIBID(LIBID_IWPLUCallbackLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_IWPLUCALLBACK, "{3D36ED0A-A197-4750-AD66-4C7B799CB6D4}")
};

CIWPLUCallbackModule _AtlModule;


// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if(DLL_PROCESS_ATTACH == dwReason)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hInstance);
    }

    return _AtlModule.DllMain(dwReason, lpReserved); 
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}
