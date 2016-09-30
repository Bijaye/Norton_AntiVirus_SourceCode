// StatusHelper.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#define INITGUID
#include <initguid.h>

///////////////////////////////////////////////////////////////////////////////
// ****************
// SYM INTERFACES
// ****************
#define INITIIDS
#include "StatusManager.h"
#include "NAVTrust.h"
#include "StateDefs.h"  // For the DefUtils interface
#include "NAVEventCommon.h"
#include "nscIConsoleServer.h"

#include "ISWSCHelper_Loader.h" // New IS shared loader using Trust Cache
#include "WSCHelper.h"
#include "IWPSettingsInterface.h"

// ccEraser interface
#include "ccEraserInterface.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
// TRACE implementation
ccSym::CDebugOutput g_DebugOutput(_T("StatusHp"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

// {A8D91209-A4F7-48cc-89C8-7F2D7C2289C7}
SYM_DEFINE_OBJECT_ID(STATUSHELPER_OBJECT_ID, 
	0xa8d91209, 0xa4f7, 0x48cc, 0x89, 0xc8, 0x7f, 0x2d, 0x7c, 0x22, 0x89, 0xc7);

DECLARE_CCAPP_PLUGIN(STATUSHELPER_OBJECT_ID)

SYM_OBJECT_MAP_BEGIN()                          
	SYM_OBJECT_ENTRY(STATUSHELPER_OBJECT_ID, CStatusManager) 
SYM_OBJECT_MAP_END()

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if (DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);

#ifdef _DEBUG
        ::MessageBox ( NULL, "StatusHelper", "starting", MB_OK );
#endif
    }

    return TRUE;
}

