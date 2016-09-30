/////////////////////////////////////////////////////////////////////////////////////////////
// NAVInst.cpp: entry point for the install DLL
//
// COLLIN DAVIS & KEVIN MONG
/////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#define INITIIDS
#include "defutilsinterface.h"
#include "ccSettingsInterface.h"
#include "ccVerifyTrustInterface.h"
#include "ccServicesConfigInterface.h"
#include "ccEventManagerHelper.h"
#include "Iquaran.h"
#include "ccSymStringImpl.h"
#include "nscIConsoleServer.h"
#include "ccMemoryInterface.h"
#include "EmailProxyInterface.h"  // For Email options

// HPP headers
#include "HPPUtilityLoader.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if(DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);

        ghInstance = (HINSTANCE) hModule;
    }
    
    return TRUE;
}

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()