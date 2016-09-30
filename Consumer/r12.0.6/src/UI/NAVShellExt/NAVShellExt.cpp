// NAVShellExt.cpp : Implementation of DLL Exports.


#include "StdAfx.h"
#include "Resource.h"
#include <initguid.h>

#define SIMON_INITGUID
#include <SIMON.h>
#include <SymInterface.h>
#include "NAVShellExt.h"

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#include "ComponentPepHelper.h"

#include "NAVShellExt_i.c"
#include "IEToolBandObj.h"
#include "IEContextMenuObj.h"
#include "cnavextbho.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"


CComModule _Module;
SIMON::CSimonModule _SimonModule;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_IEToolBand, CIEToolBand)
	OBJECT_ENTRY(CLSID_IEContextMenu, CIEContextMenu)
	OBJECT_ENTRY(CLSID_CNavExtBho, CCNavExtBho)
END_OBJECT_MAP()

// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            


// Utility functions object
COSInfo  g_OSInfo;
CNAVInfo g_NAVInfo;

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_NAVSHELLEXTLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
        _Module.Term();
	}

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
#define CLSID_TOOLBAND _T("{42CDD1BF-3FFB-4238-8AD1-7859DF00B1D6}")
#define REG_PATH       _T("Software\\Microsoft\\Internet Explorer\\Toolbar")
STDAPI DllUnregisterServer(void)
{
	CRegKey rk;

	// Remove the ToolBand from the registry
	if (ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, REG_PATH))
	{
		rk.DeleteValue(CLSID_TOOLBAND);
		rk.Close();
	}

	if (ERROR_SUCCESS == rk.Open(HKEY_CURRENT_USER, REG_PATH _T("\\ShellBrowser")))
	{
		rk.DeleteValue(CLSID_TOOLBAND);
		rk.Close();
	}

	if (ERROR_SUCCESS == rk.Open(HKEY_CURRENT_USER, REG_PATH _T("\\WebBrowser")))
	{
		rk.DeleteValue(CLSID_TOOLBAND);
		rk.Close();
	}

    return _Module.UnregisterServer(TRUE);
}


