////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// IWP.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"


///////////////////////////////////////////////////////////////////////////////
// SYM INTERFACE
#define INITIIDS
#include "IWPSettings.h"
#include "ccSymInterfaceLoader.h"
#include "ccSettingsInterface.h"

#include "FWInterface.h"
#include "FWSetupInterface.h"
#include "FWLoaders.h"
#include "SymNetInterface.h"

#include "ArbitratorInterface.h"
#include "ArbitratorLoader.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
// TRACE implementation
ccSym::CDebugOutput g_DebugOutput(_T("IWP"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Module lifetime management for trust cache
#include "ccSymModuleLifetimeMgrHelper.h"
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);


SYM_OBJECT_MAP_BEGIN()                           
	SYM_OBJECT_ENTRY(IWP::IID_IWPSettings, IWP::CIWPSettings)
	SYM_OBJECT_ENTRY(IWP::IID_IWPSettings2, IWP::CIWPSettings)
SYM_OBJECT_MAP_END()

HINSTANCE g_hInstance = NULL;

BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if ( DLL_PROCESS_ATTACH == ul_reason_for_call )
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hInstance);

        g_hInstance = hInstance;
    }

    if ( DLL_PROCESS_DETACH == ul_reason_for_call )
    {
        g_hInstance = NULL;
    }

    return TRUE;
}

