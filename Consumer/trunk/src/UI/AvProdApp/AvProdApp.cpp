////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// HomePageProtectionApp.cpp : Defines the entry point for the DLL application.
//


#define INITIIDS
// (precompiled headers turned off for this file)
#include "stdafx.h"

#include <ccAppPlugin.h>
#include <AvProdSvcCommIds.h>

#include <ccEventManagerHelper.h>
#include <ccSerializableEventHelper.h>
#include <ccSymMultiEventFactoryHelper.h>
#include <ccProSubLoader.h>
#include <ccProviderHelper.h>
#include <ccLogManagerHelper.h>
#include <ccVerifyTrustInterface.h>
#include <ccAlertInterface.h>
#include <ccAlertLoader.h>
#include <ccSettingsInterface.h>
#include <ccServiceInterface.h>

#include <SessionAppWindow.h>
#include <SessionAppWindowThread.h>
#include <SessionAppServerImpl.h>
#include <SessionAppComm.h>

#include <SettingsEventHelperLoader.h>

#include <AVInterfaceLoader.h>

#include <ISymMceCmdLoader.h>	// detect optional Media center components

#include "SymAppPlugin.h"

#include <AvProdCommands.h>
#include <AvProdLoggingLoader.h>
#include <ScanUILoader.h>

#include <uiElementInterface.h>
#include <uiUpdateInterface.h>
#include <uiNumberDataInterface.h>

#include <isDataClientLoader.h>
#include <uiNISDataElementGuids.h>
#include <isSymTheme.h>

#include <ccSymModuleLifetimeMgrHelper.h>
#include <ccSymDelayLoader.h>

#include <ccSerializeInterface.h>
#include <ccSymMemoryStreamIMpl.h>

///////////////////////////////////////////////////////////////////////////////
// ATL Module declaration
CAppModule _Module;

///////////////////////////////////////////////////////////////////////////////
// Initialize resource loader
cc::CResourceLoader g_Resources(&_AtlBaseModule, _T("AVPAPP32.loc"));

ATL::CDynamicStdCallThunk::CThunksHeap ATL::CDynamicStdCallThunk::s_ThunkHeap;

///////////////////////////////////////////////////////////////////////////////
// Initialize module lifetime manager
cc::IModuleLifetimeMgrPtr g_Manager;
IMPLEMENT_MODULEMANAGER(g_Manager);

///////////////////////////////////////////////////////////////////////////////
// Initialize debug output
ccSym::CDebugOutput g_DebugOutput(_T("AVPAPP32"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

///////////////////////////////////////////////////////////////////////////////
// ccDelayLoader implementation
ccSym::CDelayLoader g_DelayLoader;

///////////////////////////////////////////////////////////////////////////////
// utility function for verifying and init'ing resource module
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
			CCTRCTXE0(_T("Failed to Initialize resources."));
			bRet = false;
		}
	}
	return bRet;
}

#define INIT_RESOURCES() \
	if(false == _InitResources()) \
		return SYMERR_INVALID_FILE; 

///////////////////////////////////////////////////////////////////////////////
// set up the ccApp entrypoint
DECLARE_CCAPP_PLUGIN( AvProdApp::CLSID_SymAppPlugin );

///////////////////////////////////////////////////////////////////////////////
// The SymInterface object map for this module.
SYM_OBJECT_MAP_BEGIN()                  
	INIT_RESOURCES()
    SYM_OBJECT_ENTRY( AvProdApp::CLSID_SymAppPlugin,  AvProdApp::CSymAppPlugin )
SYM_OBJECT_MAP_END()                            


///////////////////////////////////////////////////////////////////////////////
// DllMain()
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

