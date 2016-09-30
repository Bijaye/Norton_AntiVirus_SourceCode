// SNLog.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"

#define INITIIDS
#include "SymInterface.h"
#include "ccSerializableEventEx.h"
#include "ccEventFactoryEx.h"
#include "SNLogSymNetiCategory.h"
#include "SNLogLogViewerPlugin.h"
#include "ccEventManagerHelperEx.h"
#include "ccLoginInterface.h"
#include "ccSettingsManagerHelper.h"
#include "LogViewerEventFactory.h"
#include "ShowModeInterface.h"
#include "ccSymDebugOutput.h"
#include "ccResourceLoader.h"

#include "IWPPrivateSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "IWPPrivateSettingsLoader.h"
#include "IWPSettingsLoader.h"

// well... it works... would be nice if this was shared a little better...
#include "IWPSettingsLoader.cpp"

#include "ccLibDll.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

ccSym::CDebugOutput g_DebugOutput("NavSNLog");
IMPLEMENT_CCTRACE(g_DebugOutput);

HINSTANCE g_hInst;

// Must have a global module class named _ModuleRes for CResourceLoader to work
CAtlBaseModule _ModuleRes;
::cc::CResourceLoader g_ResLoader(&_ModuleRes, SYMC_RESOURCE_DLL);


/////////////////////////////////////////////////////////////////////////////
//	Resource Initialization
/////////////////////////////////////////////////////////////////////////////

bool	_InitResources()
{
	bool	bRet = true;
	if( NULL == g_ResLoader.GetResourceInstance() ) 
	{
		if( !g_ResLoader.Initialize() ) 
		{
			CCTRACEE(_T("Failed to Initialize resources."));
			bRet = false;
		}
	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        g_hInst = hInstance;
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {}
    return TRUE;    // ok
}

