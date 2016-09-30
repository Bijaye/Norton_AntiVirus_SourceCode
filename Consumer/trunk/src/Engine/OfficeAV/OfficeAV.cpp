////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Copyright 1999 Symantec, SABU, AV Core team
// -------------------------------------------------------------------------
//
// Author:  Danny Wong
// Date:    06/02/99
//
// OfficeAV.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To merge the proxy/stub code into the object DLL, add the file 
//      dlldatax.c to the project.  Make sure precompiled headers 
//      are turned off for this file, and add _MERGE_PROXYSTUB to the 
//      defines for the project.  
//
//      If you are not running WinNT4.0 or Win95 with DCOM, then you
//      need to remove the following define from dlldatax.c
//      #define _WIN32_WINNT 0x0400
//
//      Further, if you are running MIDL without /Oicf switch, you also 
//      need to remove the following define from dlldatax.c.
//      #define USE_STUBLESS_PROXY
//
//      Modify the custom build rule for OfficeAV.idl by adding the following 
//      files to the Outputs.
//          OfficeAV_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f OfficeAVps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "OfficeAV_h.h"

#include "OfficeAV_i.c"
#include "OfficeAntiVirus.h"

#include "ccLibDllLink.h"

#define INITIIDS
#include "ccSettingsMgdHelper.h"
#include "ccSymDebugOutput.h"
#include "ccSymDelayLoader.h"
#include "ccResourceLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"

cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

ccSym::CDelayLoader delayLoader;

ccSym::CDebugOutput g_DebugOutput(_T("OfficeAV"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// CATID for Microsoft Office AntiVirus COM category
// {56FFCC31-D398-11d0-B2AE-00A0C908FA49}
DEFINE_GUID(CATID_MSOfficeAntiVirus, 
0x56ffcc30, 0xd398, 0x11d0, 0xb2, 0xae, 0x0, 0xa0, 0xc9, 0x8, 0xfa, 0x49);

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_OfficeAntiVirus, COfficeAntiVirus)
END_OBJECT_MAP()

// Must have a global module class named _ModuleRes for CResourceLoader to work
// We are calling it _ModuleRes to avoid conflict with the already defined _Module.
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    lpReserved;
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_OfficeAntiVirusLib);
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
#ifdef _MERGE_PROXYSTUB
    if (PrxDllCanUnloadNow() != S_OK)
        return S_FALSE;
#endif
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	//  No trust check. (Loaded into non-SYMC processes.)
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    HRESULT hRes = PrxDllRegisterServer();
    if (FAILED(hRes))
        return hRes;
#endif
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _Module.RegisterServer(TRUE);
    if (SUCCEEDED(hr))
    {
        ICatRegister * pc = 0;

        // According to KB passing in CLSCTX_REMOTE_SERVER (either by itself or through CLSCTX_SERVER or CLSCTX_ALL)
        // fails on Windows 95 pre-DCOM. Have to use CLSCTX_INPROC_SERVER only
        hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
                              0,
                              CLSCTX_INPROC_SERVER,
                              IID_ICatRegister,
                              (LPVOID *)&pc);
        if (SUCCEEDED(hr))
        {
            // create category in case it does not exist
            CATEGORYINFO    rgcc[1];
            ZeroMemory(&rgcc[0], sizeof(CATEGORYINFO));
            rgcc[0].catid = CATID_MSOfficeAntiVirus;
            rgcc[0].lcid = 0x409;                   // english description
            pc->RegisterCategories(1, rgcc);

            // register the object in this category
            CATID cats[1];
            cats[0] = CATID_MSOfficeAntiVirus;
            hr = pc->RegisterClassImplCategories(CLSID_OfficeAntiVirus, sizeof(cats)/sizeof(CATID), cats);
            pc->Release();
        }
    }
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
#ifdef _MERGE_PROXYSTUB
    PrxDllUnregisterServer();
#endif
    ICatRegister * pc = 0;

    // According to KB passing in CLSCTX_REMOTE_SERVER (either by itself or through CLSCTX_SERVER or CLSCTX_ALL)
    // fails on Windows 95 pre-DCOM. Have to use CLSCTX_INPROC_SERVER only
    if (SUCCEEDED(CoCreateInstance(CLSID_StdComponentCategoriesMgr,
                                   0,
                                   CLSCTX_INPROC_SERVER,
                                   IID_ICatRegister,
                                   (LPVOID *)&pc)))
    {
        // unregister the object from the category
        CATID cats[1];
        cats[0] = CATID_MSOfficeAntiVirus;
        pc->UnRegisterClassImplCategories(CLSID_OfficeAntiVirus, sizeof(cats)/sizeof(CATID), cats);
        pc->Release();
    }


    return _Module.UnregisterServer(TRUE);
}