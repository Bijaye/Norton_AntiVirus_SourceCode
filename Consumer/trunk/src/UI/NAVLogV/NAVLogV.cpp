// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVLogV.cpp : Defines the entry point for the DLL application.
//

#define INITIIDS
#include "stdafx.h"

#define INITGUID
#include "Guiddef.h"

#if !defined(_NO_SIMON_INITGUID)
#define SIMON_INITGUID
#endif
#include "initguid.h"

#include "SymHTML.h"
#include "SymHTMLDocumentImpl.h"

#include "AvDataIds.h"
#include "Provider.h"
#include "Module.h"

#include "SymInterface.h"
#include "ccLogViewerInterface.h"
#include "NAVTrust.h"
#include "ccSettingsManagerHelper.h"

#include "NAVLogV.h"
#include "NAVLogVPlugin.h"

#include "AVCategory.h"
#include "AVEvent.h"

#include "AvProdLoggingLoader.h"
#include "AvProdClientActivity.h"

#include "AvInterfaceLoader.h"
#include "QBackupLoader.h"
#include "AVSubmissionInterface.h"

#include "StockFrameworkObjectsInterface.h"

#include "uiDataInterface.h"
#include "uiDateDataInterface.h"
#include "uiStringDataInterface.h"
#include "uiNumberData.h"

#include <ScanUILoader.h>
#include <ScanTaskLoader.h>

#include <isDataClientLoader.h>
#include <uiNISDataElementGuids.h>

#include "ISCanIRun.h"

#include "uiAvProviderElements.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVLogV"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

void _InitResources();
#define INIT_RESOURCES() \
    _InitResources();


#include "ccOSInfo.h"


// For Virus Alerts
//
#include "VirusAlertCategory.h"

// For Virus List
//
//#include "VirusListEvent.h"
//#include "VirusListCategory.h"

// For Errors list
//
#include "ErrorCategory.h"

// For Application activities list
//
#include "AppCategory.h"

using namespace std;

SYM_OBJECT_MAP_BEGIN()                          
    INIT_RESOURCES()
	SYM_OBJECT_ENTRY(NAV_LOG_VIEWER_PLUGIN_OBJECT_ID, CNAVLogViewerPlugin)
	SYM_OBJECT_ENTRY(NAV_AV_EVENT_OBJECT_ID, CAVEvent)
    
    SYM_OBJECT_ENTRY(NAV_VIRUS_ALERT_CATEGORY_OBJECT_ID, CVirusAlertCategory )
    SYM_OBJECT_ENTRY(NAV_ERROR_CATEGORY_OBJECT_ID, CErrorCategory)
    SYM_OBJECT_ENTRY(NAV_APP_CATEGORY_OBJECT_ID, CAppCategory)
    
    SYM_OBJECT_ENTRY( CLSID_MCAVModule, CMCAVModule )
    
    SYM_OBJECT_ENTRY( AvProd::AvModuleLogging::CLSID_AutoProtectClientActivities, CAutoProtectClientActivity )
    SYM_OBJECT_ENTRY( AvProd::AvModuleLogging::CLSID_ManualScanClientActivities, CManualScanClientActivity )
    SYM_OBJECT_ENTRY( AvProd::AvModuleLogging::CLSID_EmailScanClientActivities, CEmailScanClientActivity )
    SYM_OBJECT_ENTRY( AvProd::AvModuleLogging::CLSID_ThreatDataEz, CThreatDataEz)

    //SYM_OBJECT_ENTRY(NAV_VIRUS_LIST_CATEGORY_OBJECT_ID, CVirusListCategory)
	//SYM_OBJECT_ENTRY(NAV_VIRUS_LIST_EVENT_OBJECT_ID, CVirusListEvent)
SYM_OBJECT_MAP_END() 

// DRM code removed so we can debug
#ifndef _DEBUG
    #include "Simon.h"
    SIMON::CSimonModule _SimonModule;
#endif // _DEBUG

bool g_bIsAdmin;
bool g_bRawMode;        // Display data in raw format so we can see what's being logged.

//Resource loading code
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navlogv.loc")
CAppModule _Module;
::cc::CResourceLoader g_ResLoader(&_AtlBaseModule, SYMC_RESOURCE_DLL);

cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);


BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if (DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hInstance);
        _AtlBaseModule.AddResourceInstance(hInstance);

        ccLib::COSInfo m_OSInfo;
        g_bIsAdmin = m_OSInfo.IsAdministrator();
    }

    return TRUE;
}

SYMRESULT WINAPI GetLVPluginObject(SYMOBJECT_ID* pID)
{
    if ( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication ())
        return SYMERR_ACCESSDENIED;

	if (pID == NULL)
		return SYMERR_INVALIDARG;

	*pID = NAV_LOG_VIEWER_PLUGIN_OBJECT_ID;

	return SYM_OK;
}

HRESULT WINAPI GetProviderModule(SYMOBJECT_ID* pID)
{
    if (pID == NULL)
    {
        return E_INVALIDARG;
    }

    *pID = CLSID_MCAVModule;

    return SYM_OK;
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
            _AtlBaseModule.SetResourceInstance(g_ResLoader.GetResourceInstance());
        }
    }

}
