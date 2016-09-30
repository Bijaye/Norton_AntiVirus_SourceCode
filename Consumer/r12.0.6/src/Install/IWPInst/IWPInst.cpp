// IWPInst.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "ccLibStaticLink.h"

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

