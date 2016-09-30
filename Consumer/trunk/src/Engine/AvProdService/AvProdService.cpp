////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ccSvcDbg.cpp : Defines the entry point for the DLL application.
//

#define INITIIDS
// (precompiled headers turned off for this file)
#include "stdafx.h"

#include <ccSerializeInterface.h>
#include <ccSymMemoryStreamIMpl.h>

#include <ccServiceLoader.h>
#include <ccSymDebugOutput.h>

#include <ccSymModuleLifetimeMgrHelper.h>
#include <ccSymDelayLoader.h>

#include <ccSettingsManagerHelper.h>

#include <SvcMgr.h>
#include <SessionAppClientImpl.h>

#include <NAVEventFactoryLoader.h>
#include <ISymMceCmdLoader.h>	

#include <AvProdLoggingLoader.h>

#include <AvProdCommands.h>
#include <AvProdSvcClientInterface.h>
#include "AvProdSvcClientImpl.h"
#include <AvProdSvcClientLoader.h>

#include "SymSvcPlugin.h"

#include <AvInterfaces.h> // AntiVirus component

///////////////////////////////////////////////////////////////////////////////
// Global module handle.
HANDLE g_hModuleHandle = NULL;

///////////////////////////////////////////////////////////////////////////////
// ATL Module declaration
CAppModule _Module;

///////////////////////////////////////////////////////////////////////////////
// Initialize resource loader
cc::CResourceLoader g_Resources(&_AtlBaseModule, _T("AVPSVC32.loc"));

///////////////////////////////////////////////////////////////////////////////
// Initialize module lifetime manager
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

///////////////////////////////////////////////////////////////////////////////
// Initialize debug output
ccSym::CDebugOutput g_DebugOutput(_T("AVPSVC32"));
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
// Object map
SYM_OBJECT_MAP_BEGIN()                         
    INIT_RESOURCES()
    SYM_OBJECT_ENTRY ( ccService::IID_ServicePlugin, AvProdSvc::CSymSvcPlugin )
    SYM_OBJECT_ENTRY( AvProdSvc::CLSID_AvProdSvcClient, AvProdSvc::CAvProdSvcClientImpl)
SYM_OBJECT_MAP_END()                           


///////////////////////////////////////////////////////////////////////////////
// DllMain()
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
                     )
{
    switch( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
        g_hModuleHandle = hModule;
        DisableThreadLibraryCalls( (HINSTANCE) hModule );
        break;
    }

    return TRUE;
}


