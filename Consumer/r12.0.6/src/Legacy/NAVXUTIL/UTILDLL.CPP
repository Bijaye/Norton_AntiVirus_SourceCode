// Copyright 1993 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/utildll.cpv   1.3   18 May 1998 22:00:20   tcashin  $
//
// Description:
//      This source file contains those routines necessary for starting up
//      and gracefully terminating the NAVWUTIL.DLL.  This Dynamic Link
//      Library contains utility routines used by both the Norton AntiVirus
//      Clinic and its related Dynamic Link Libraries.
//***********************************************************************
// $Log:   S:/NAVXUTIL/VCS/utildll.cpv  $
// 
//    Rev 1.3   18 May 1998 22:00:20   tcashin
// iquaran.h and #define INITGUID have to be ifdef'd for WIN32.
// 
//    Rev 1.2   18 May 1998 21:50:46   tcashin
// Define INITGUID for the NAVXUTIL DLL here.
// 
//    Rev 1.1   14 Aug 1997 13:38:36   DBUCHES
// Added global navRpcCriticalSection object.
//
//    Rev 1.0   06 Feb 1997 21:05:14   RFULLER
// Initial revision
//
//    Rev 1.0   30 Jan 1996 15:56:32   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:34   BARRY
// Initial revision.
//
//    Rev 1.2   20 Jan 1995 19:09:38   MFALLEN
// All the DOS and VXD strings are accessed using the LoadString() functions
// that emulates it's Windows counterpart. All the string ID's are stored in the
// stbexts.h file that identical for all the platforms. DOS and VXD strings are
// contained in the UTILSTR.STR file.
//
//    Rev 1.1   19 Dec 1994 21:46:34   DALLEE
// Put back String_Init() and String_End() for SYM_WIN16.
//
//    Rev 1.0   16 Dec 1994 10:37:04   MFALLEN
// Initial revision.
//
//    Rev 1.6   13 Sep 1994 15:41:56   MFALLEN
// DALLEE checking in MFALLEN's clean-up changes.
//***********************************************************************

#include "platform.h"

#ifdef SYM_WIN32
    #define INITGUID
    #include "iquaran.h"
#endif

#ifdef SYM_WIN

//************************************************************************
// Global Variables
//************************************************************************

HINSTANCE           hInstance = NULL;

#ifdef SYM_WIN32

// -------------------------------------------------------------------------

int WINAPI LibMain(
    HINSTANCE hModule,
    UINT wDataSeg,
    UINT cbHeapSize,
    LPCSTR lpszCmdLine);

int SYM_EXPORT WINAPI WEP (
    int bSystemExit);

// -------------------------------------------------------------------------

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
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


/****************************************************************************
   FUNCTION: LibMain(HANDLE, WORD, WORD, LPSTR)

   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
             the DLL is loaded.  The LibEntry routine is provided in
             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
             source LIBENTRY.ASM is also provided.)
             the initialization is successful.

*****************************************************************************/

int WINAPI LibMain(HINSTANCE hModule, UINT wDataSeg, UINT cbHeapSize,
                       LPCSTR lpszCmdLine)
{
#ifdef SYM_WIN16
    if (cbHeapSize != 0)
        UnlockData(0);
#endif
                                       // Save global instance
    hInstance = hModule;

    return (1);
}

/****************************************************************************
    FUNCTION:  WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded (no
              remaining tasks still have the DLL loaded).  It is strongly
              recommended that a DLL have a WEP() function, even if it does
              nothing but returns success (1), as in this example.

*******************************************************************************/
int SYM_EXPORT WINAPI WEP (int bSystemExit)
{
    return(1);
}

#endif
