// IWP.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"


///////////////////////////////////////////////////////////////////////////////
// SYM INTERFACE
#define INITIIDS
#include "plugin.h"
#include "IWPSettings.h"
#include "IWPPrivateSettingsInterface.h"
#include "ccSerializableEventEx.h"
#include "SymNetDrvAlertEventEx.h"
#include "SymNetDrvLogEventEx.h"
#include "ccAlertInterface.h"
#include "SymFWAgtInterface.h"
#include "ccLogViewerEventFactory.h"
#include "ThreatLevelLoader.h"
#include "SymIDSI.h"
#include "ccLoginInterface.h"
#include "ccLoginLoader.h"
#include "ccSymInterfaceLoader.h"
#include "NAVEventCommon.h" // For CNAVEventCommonQIPtr
#include "EMSubscriber.h"

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


// ccApp Plug-in
//
// {1E837CAF-DD21-4300-AB04-23C7CA7184B8}
SYM_DEFINE_OBJECT_ID(IWP_CCAPP_PLUGIN_OBJECT_ID, 
	0x1e837caf, 0xdd21, 0x4300, 0xab, 0x4, 0x23, 0xc7, 0xca, 0x71, 0x84, 0xb8);

DECLARE_CCAPP_PLUGIN(IWP_CCAPP_PLUGIN_OBJECT_ID)

SYM_OBJECT_MAP_BEGIN()                           
	SYM_OBJECT_ENTRY(IWP_CCAPP_PLUGIN_OBJECT_ID, CPlugin)
    SYM_OBJECT_ENTRY(IWP::IID_IWPSettings, CIWPSettings)
    SYM_OBJECT_ENTRY(IWP::IID_IWPPrivateSettings, CIWPSettings)
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

