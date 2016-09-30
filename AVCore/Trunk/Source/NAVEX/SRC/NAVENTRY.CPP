//************************************************************************
//
// $Header:   S:/NAVEX/VCS/naventry.cpv   1.5   18 Apr 1997 17:57:28   AOONWAL  $
//
// Description:
//      Contains AVEX DLL entry code for Windows platforms.
//
// Notes:
//      Should only be part of Windows compilations.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/naventry.cpv  $
// 
//    Rev 1.5   18 Apr 1997 17:57:28   AOONWAL
// Modified during DEV1 malfunction
// 
//    Rev ABID  16 Apr 1997 19:45:30   DCHI
// Changes so that mvpdef.dat is only loaded from NAV directory.
// 
//    Rev 1.4   07 Apr 1997 18:11:02   DCHI
// Added MVP support.
// 
//    Rev 1.3   26 Dec 1996 15:22:24   AOONWAL
// No change.
// 
//    Rev 1.2   02 Dec 1996 14:00:18   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:59:12   AOONWAL
// No change.
// 
//    Rev 1.0   13 Oct 1995 13:04:42   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"

#include "mvp.h"

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

#if defined(SYM_WIN32)

// ---------------------------------------------------------------------------
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

#endif

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
{
   hInstance = hInst;

#ifdef SYM_WIN16

   if (wHeapSize > 0)
      UnlockData (0);

#endif

    MVPLoadData((LPVOID)hInst);

   return (1);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI WEP(int nParam)
{
    MVPUnloadData();

   return(1);  // Return success to windows //
}


