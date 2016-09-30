// NAVError.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NAVErrorps.mk in the project directory.

#include "stdafx.h"
#define INITIIDS
#include "SymInterface.h"
#include "NAVTrust.h"
#include "ccSettingsInterface.h"
#include "ccErrorDisplayInterface.h"
#include "ccErrorDisplayLoader.h"

#include "NAVErrorResource.h"
#include <initguid.h>
#include "NAVError.h"

#include "NAVError_i.c"
#include "NAVCOMError.h"

#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVError"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_NAVCOMError, CNAVCOMError)
END_OBJECT_MAP()


//Resource loading code
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("naverror.loc")
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVERRORLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// _InitResources - Sets up resource loading
/////////////////////////////////////////////////////////////////////////////
void _InitResources()
{
    if(NULL == g_ResLoader.GetResourceInstance())
    {
        if(g_ResLoader.Initialize())
        {
            _Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
        }
    }

}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    // init resources
    _InitResources();
    
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}
