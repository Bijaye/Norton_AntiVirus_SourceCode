#define INITIIDS
#include "Stdafx.h"
#include "avevents.h"
#include "NAVTrust.h"
#include "ScanMgrInterface.h"
#include "ccSettingsInterface.h"
#include "ccEraserInterface.h"
#include "iquaran.h"
#include "ccErrorDisplayInterface.h"
#include "OEHScan.h"
#include "CommonUIInterface.h"
#include "ccSerializeInterface.h"
#undef INITIIDS

#define _INIT_COSVERSIONINFO
#include "StahlSoft.h"  // For Options
#include "SSOsinfo.h"


#include "ScanManager.h"

#include "SingleInstanceInterface.h"
#include "SingleInstance.h"

///////////////////////////////////////////////////////////////////////////////
//
// Global variables.
//

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;
HINSTANCE g_hInstance;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("ScanMgr"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// Debug logging of files scanned
ccSym::CDebugOutput g_FileDebugOutput(_T("FilesScanned"));

///////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{		
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hInstance);

        g_hInstance = hInstance;
        
        // Startup single instance manager
        CSingleInstanceData::Startup();
	}
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        // Shutdown single instance manager
        CSingleInstanceData::Shutdown();
    }

	return TRUE;
}

SMRESULT WINAPI GetScanManager(IScanManager** ppScanManager)
{
	*ppScanManager = NULL;

	// Force load failure if the loading application is not signed by Symantec.
    // Check the Navlnch dll for a valid symantec signature
	if( NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecApplication() )
        return SMRESULT_ERROR_INVALID_SIGNATURE;

	// Create the Scan Manager object.

	CScanManager* pManager = new CScanManager;

	if (pManager == NULL)
		return SMRESULT_ERROR_MEMORY;

	// Bump up the reference count to 1.

	pManager->AddRef();

	*ppScanManager = pManager;

	return SMRESULT_OK;
}

#include "ccSymModuleLifetimeMgrHelper.h"
::cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

// ISingleInstanceInterface
//   Single instance manager added here for convenience.
//   Expectation is that only NavW32 will be using it.
//   If that changes, should consider moving into other
//   shared dll somewhere.

// Object Map for SymInterface
SYM_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY(IID_IScanManager, CScanManager)
    SYM_OBJECT_ENTRY(CLSID_SingleInstance,  CSingleInstance)
SYM_OBJECT_MAP_END()