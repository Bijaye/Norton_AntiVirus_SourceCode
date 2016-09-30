// qspak32.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
#if 0
	//
	// DBUCHES: We cannot do this, becuase we may be loaded by the scriptblocking
	// DLL, which is loaded INPROC by the windows scripting host, which is NOT a SYMC
	// signed application.
	//
	// Force load failure if the loading application is not signed
	// by Symantec.
	if(!cct_IsSymantecApplication())
		return FALSE;
#endif

    if(DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);
    }

    return TRUE;
}

