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

#include "windows.h"

//************************************************************************
// Global Variables ******************************************************
//************************************************************************

extern "C"
{
HINSTANCE g_hInstance = NULL;
}

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
            g_hInstance = hInstDLL;
            break;

        case DLL_PROCESS_DETACH:
            g_hInstance = NULL;
            break;
    }

    return (bResult);
}
