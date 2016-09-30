// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CALLBACK/VCS/NAVENTRY.CPv   1.0   18 Jun 1997 13:16:26   MKEATIN  $
//
// Description:  Standard entry code for DLLs
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/CALLBACK/VCS/NAVENTRY.CPv  $
// 
//    Rev 1.0   18 Jun 1997 13:16:26   MKEATIN
// Initial revision.
// 
//    Rev 1.1   17 Jun 1997 18:09:50   ksackin
// Set up proper entry points for Win16/32.
//
//    Rev 1.0   17 Jun 1997 15:08:32   MKEATIN
// Initial revision.
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

#ifndef SYM_WIN32
int WINAPI LibMain (
    HINSTANCE hInst,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine );

int WINAPI WEP (
    int nParam );
#endif

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
            hInstance = hInstDLL;
            break;

        case DLL_PROCESS_DETACH:
            break;
        }   // switch

    return (bResult);
}   //  DllMain


#ifndef SYM_WIN32
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
#endif
