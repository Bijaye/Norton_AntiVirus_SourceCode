////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CfgWizDll.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f CfgWizDllps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#define SIMON_INITGUID
#include <SIMON.h>

//#define _INIT_COSVERSIONINFO
//#include "SSOSInfo.h"

#define INITIIDS
#include "ccverifytrustinterface.h"
#include "ccSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "ccServicesConfigInterface.h"
#include "SymIDSI.h"
#include "IDSLoader.h"
#include "IDSLoaderLink.h"
#include "IMConfigLoader.h"
#include "SNDHelper.h"
#include "FWLoaders.h"

// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#define _PEP_IMPL_
#include "cltLicenseConstants.h"
#include "cltProductHelper.h"
#include "V2LicensingAuthGuids.h"
#include "cltProdInfoPEPConstants.h"
#include "uiPEPConstants.h"

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
	static bool bTrustVerified = false;

	if (!bTrustVerified) 
	{
		CSymDLL_ReverseTrustCheck trustcheck;
		HRESULT hrTrust = trustcheck.DoCheck();	// check the calling process
		if (hrTrust == S_OK) 
		{
			bTrustVerified = true;
		}
		else
		{
			return hrTrust;
		}
	}

	// Serve up the object.
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


