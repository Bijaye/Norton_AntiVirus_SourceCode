// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
// avNSCPlg.cpp : Defines the entry point for the DLL application.
//
#define INITIIDS
#include "stdafx.h"

// does not use pch to allow iid def

#include "INITGUID.h"
#include "CSelfRegistrar.h"
#include "CProduct.h"
#include "CFeature.h"
#include "CClientCallback.h"
#include "FeaturePassThrough.h"
#include "AdminCheckFeature.h"

#include "ccResourceLoader.h"

// ccLib delay loader
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

ccSym::CDebugOutput g_DebugOutput(_T("avNSCPlg"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

CComModule _Module;

// path name to this module (avNSCPlg.dll)
TCHAR g_szModulePath[MAX_PATH]; 

// resource file
#define SYMC_RESOURCE_DLL _T("avNSCPlg.loc")
CAtlBaseModule _ModuleRes;
HMODULE g_ResModule;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);
#define INIT_RESOURCES() \
_InitResources();\
// Don't use this in DllMain!!
void _InitResources()
{
	if( NULL == g_ResLoader.GetResourceInstance() ) 
	{
		if( g_ResLoader.Initialize() ) 
		{
            g_ResModule = g_ResLoader.GetResourceInstance();
            _Module.SetResourceInstance( g_ResLoader.GetResourceInstance());
		}
	}
}



SYM_OBJECT_MAP_BEGIN()                           
    INIT_RESOURCES()
    SYM_OBJECT_ENTRY( nsc::IID_nscISelfRegistrar, CSelfRegistrar)
    // ISelfRegistrar class id is the same as the interface id
    SYM_OBJECT_ENTRY(CLSID_NAV_Product,CProduct)
	SYM_OBJECT_ENTRY(CLSID_NAV_Feature_NAVLicense,CNAVLicenseFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_AutoProtect,CAutoProtectFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_EmailScanning,CEmailScanningFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_SpywareProtection,CSpywareProtectionFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_IFP,CIFPFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_IMScanning,CIMScanningFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_WormBlocking,CWormBlockingFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_FSS,CFSSFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_VirusDefs,CVirusDefsFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_Scan_Now,CScanNowFeature)
    SYM_OBJECT_ENTRY(CLSID_NAV_Feature_Quarantine,CQuarantineFeature)

    // client callback
    SYM_OBJECT_ENTRY(CLSID_NAV_Client_Callback,CClientCallback)
    
    // feature passthrough
    SYM_OBJECT_ENTRY(IID_IFeaturePassThru, CFeaturePassThrough)
    SYM_OBJECT_ENTRY(CLSID_CAdminCheck, CAdminCheckFeature)
SYM_OBJECT_MAP_END()    




BOOL APIENTRY DllMain( HMODULE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID /*lpReserved*/
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        ::DisableThreadLibraryCalls(hModule);
        if (!::GetModuleFileName(hModule, g_szModulePath, MAX_PATH))
        {
            DWORD dwError = ::GetLastError();
            TRACEE (_T("GetModuleFileName failed: %d"), dwError);
            g_szModulePath[0] = 0;
        }
    }

    return TRUE;
}

