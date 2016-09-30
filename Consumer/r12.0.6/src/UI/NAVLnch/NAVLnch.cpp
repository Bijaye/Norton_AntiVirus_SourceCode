// NAVLnch.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f NAVLnchps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"

#include <initguid.h>

#define SIMON_INITGUID
#include <SIMON.h>

#define INITIIDS
#include "NAVTrust.h"
#include "NAVSettingsHelperEx.h"

#include "NAVLnch.h"

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#include "ComponentPepHelper.h"

#include "NAVLnch_i.c"
#include "AppLauncher.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

// TRACE implementation
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVLNCH"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_AppLauncher, CAppLauncher)
END_OBJECT_MAP()

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
CString _g_csHandlerPath;
SIMON::CSimonModule _SimonModule;

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVLNCHLib);
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
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
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


