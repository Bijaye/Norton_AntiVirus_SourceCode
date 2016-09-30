// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/NAVENTRY.CPv   1.0   31 May 1997 15:52:56   MKEATIN  $
//
// Description:  Standard entry code for DLLs
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/NAVENTRY.CPv  $
// 
//    Rev 1.0   31 May 1997 15:52:56   MKEATIN
// Initial revision.
// 
//    Rev 1.0   17 Jun 1996 15:18:50   MKEATIN
// Initial revision.
//************************************************************************

#include "platform.h"

HINSTANCE hInstance;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (
    HINSTANCE hInst,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine );

int WINAPI WEP (
    int nParam );

// ---------------------------------------------------------------------------


BOOL WINAPI DllMain (
    HINSTANCE   hInstDLL,
    DWORD       dwReason,
    LPVOID      lpvReserved )
{
    BOOL        bResult = TRUE;

    switch (dwReason)
        {
        case DLL_PROCESS_ATTACH:
            bResult = LibMain(hInstDLL, 0, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            WEP(0);
            break;
        }   // switch

    return (bResult);
}   //  DllMain


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
{
   hInstance = hInst;
   return (1);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI WEP(int nParam)
{
   return(1);  // Return success to windows //
}

