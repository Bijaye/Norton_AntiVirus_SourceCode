// CfgWizDll.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f CfgWizDllps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#define SIMON_INITGUID
#include <SIMON.h>

#define INITIIDS
#include "ccverifytrustinterface.h"
#include "ccSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "IWPPrivateSettingsInterface.h"
#include "WSCHelperInterface.h"
#include "ccServicesConfigInterface.h"
#include "IICFManager.h"
#include "SymIDSI.h"
#include "IDSLoader.h"
#include "IDSLoaderLink.h"

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#define _PEP_IMPL_
#include "cltPEPConstants.h"
#include "PEPClientBase.h"
#include "AVPEPComponent.h"

#include "NAVCfgWizDll_h.h"
#include "NAVCfgWizDll_i.c"
#include "NAVCfgWizMgr.h"
#include "NAVBusinessRules.h"


///////////////////////////////////////////////////////////////////////////////
//
// Global variables.
//
CComModule _Module;
SIMON::CSimonModule _SimonModule;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_NAVCfgWizMgr, CNAVCfgWizMgr)
END_OBJECT_MAP()

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVCFGWIZDLLLib);
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


