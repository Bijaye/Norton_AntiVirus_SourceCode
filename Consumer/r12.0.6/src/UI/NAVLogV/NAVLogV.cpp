// NAVLogV.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#define INITIIDS
#include "SymInterface.h"
#include "ccLogViewerInterface.h"
#include "NAVTrust.h"
#include "ccSettingsManagerHelper.h"

#include "NAVLogV.h"
#include "NAVLogVPlugin.h"

#include "AVCategory.h"
#include "AVEvent.h"

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


#include "OSInfo.h" // Toolbox

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
#include "..\navlogv\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navlogv.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);


BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if (DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hInstance);

        COSInfo OSInfo;
        g_bIsAdmin = OSInfo.IsAdminProcess();
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
