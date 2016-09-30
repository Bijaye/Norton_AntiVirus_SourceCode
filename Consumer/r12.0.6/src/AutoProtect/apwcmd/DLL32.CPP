// Copyright 1997 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/APWCMD/VCS/dll32.cpv   1.0   28 May 1997 12:25:52   JBRENNA  $
//
// Description:
//   Provide the DllMain function for this library.
//
//***********************************************************************
// $Log:   S:/APWCMD/VCS/dll32.cpv  $
//
//    Rev 1.0   28 May 1997 12:25:52   JBRENNA
// Initial revision.
//
//***********************************************************************
#include "Stdafx.h"


// This is for SymInterface IIDs - All headers that have an IID used by either
// apwcmd9x or apwcmdNT must be included here
#define INITIIDS
#include "NAVTrust.h"
#include "Scanner.h"
#include "defutilsinterface.h"
#include "NAVDefutilsLoader.h"
#include "ccSettingsinterface.h"
#include "ccEventManagerHelper.h"
#include "ccAlertInterface.h"
#include "AvEvents.h"
#include "SymInterfaceHelpers.h"
#include "ScanTask.h"     // for IScanTask
#include "NAVEventFactoryLoader.h"
#include "ExclusionManagerInterface.h"
#include "ISymMceCmdLoader.h"	// detect optional Media center components
#include "AVScanManagerLoader.h"
#undef INITIIDS

#include "ccLib.h"

// ccLib delay loader
// This will allow the apwcmd9x and apwcmdNT Dll's delay load
// the ccLib dll at runtime.
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("APWCMD"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

//************************************************************************
// Global Variables ******************************************************
//************************************************************************

extern "C"
{
HINSTANCE g_hInstance = NULL;
}

//Resource loading
#include "..\ApwCmdRes\ApwCmdNTRes\resource.h"
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("apwcmdNT.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);


/*************************************************************************
 * DllMain
 *
 * Description:
 *   Called when a new process is either using or not using this DLL
 *   anymore.
 ************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    BOOL bResult = TRUE;

    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
            DisableThreadLibraryCalls((HMODULE)hInstDLL);

            g_hInstance = hInstDLL;
            break;

        case DLL_PROCESS_DETACH:
            g_hInstance = NULL;
            break;
    }

    return (bResult);
}
