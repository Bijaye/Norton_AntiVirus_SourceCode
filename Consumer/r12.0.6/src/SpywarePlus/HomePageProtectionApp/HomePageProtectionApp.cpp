// HomePageProtectionApp.cpp : Defines the entry point for the DLL application.
//


#define INITIIDS
// (precompiled headers turned off for this file)
#include "stdafx.h"

#include "ccAppPlugin.h"
#include "NAVSettingsHelperEx.h"
#include "ccEventManagerHelper.h"
#include "ccSerializableEventHelper.h"
#include "ccSymMultiEventFactoryHelper.h"
#include "ccProSubLoader.h"
#include "ccProviderHelper.h"
#include "ccLogManagerHelper.h"
#include "ccVerifyTrustInterface.h"
#include "ccAlertInterface.h"
#include "ccAlertLoader.h"


#include "HPPAppInterface.h"

#include "HPPSessionSymAppPlugin.h"
#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "BBEventID.h"
#include "BBSystemMonitorEventsInterface.h"

#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccSymDelayLoader.h"

CAppModule _Module;

cc::CResourceLoader g_Resources(&_AtlBaseModule, _T("HPPRES32.loc"));

cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);

// TRACE implementation
ccSym::CDebugOutput g_Log(_T("HPP32"));
IMPLEMENT_CCTRACE(::g_Log);

// ccDelayLoader implementation
ccSym::CDelayLoader g_DelayLoader;

bool _InitResources()
{
	bool bRet = true;
	if(NULL == g_Resources.GetResourceInstance())
	{
		if(g_Resources.Initialize())
		{
			_AtlBaseModule.SetResourceInstance(g_Resources.GetResourceInstance());
		}
		else
		{
			CCTRACEE(_T("Failed to Initialize resources."));
			bRet = false;
		}
	}
	return bRet;
}

#define INIT_RESOURCES() \
	if(false == _InitResources()) \
		return SYMERR_INVALID_FILE; 

// set up the ccApp entrypoint
DECLARE_CCAPP_PLUGIN( CLSID_HPPSessionSymAppPlugin );

// The SymInterface object map for this module.
SYM_OBJECT_MAP_BEGIN()                  
	INIT_RESOURCES()
    SYM_OBJECT_ENTRY( CLSID_HPPSessionSymAppPlugin,  CHPPSessionSymAppPlugin )
SYM_OBJECT_MAP_END()                            


BOOL APIENTRY DllMain( HMODULE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch(ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            _Module.Init(NULL, hModule);
            DisableThreadLibraryCalls(hModule);
            _AtlBaseModule.AddResourceInstance(hModule);
            break;

        case DLL_PROCESS_DETACH:
            _Module.Term();
            break;

    }
    
    return TRUE;
}

