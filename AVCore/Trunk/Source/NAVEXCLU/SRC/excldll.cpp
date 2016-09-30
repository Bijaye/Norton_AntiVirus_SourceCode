// Copyright 1993 Symantec Corporation                                  
//***********************************************************************
//                                                                
// $Header:   S:/navexclu/VCS/excldll.cpv   1.0   06 Feb 1997 20:57:52   RFULLER  $ 
//                                                                
// Description:                                                   
//***********************************************************************
// $Log:   S:/navexclu/VCS/excldll.cpv  $
// 
//    Rev 1.0   06 Feb 1997 20:57:52   RFULLER
// Initial revision
// 
//    Rev 1.3   13 May 1996 18:07:08   jmillar
// fix Ron's fat finger of back arrow in line one - repeat after me - I will
// compile all changes before checking them in - repeat 100,000 times.
// 
//    Rev 1.2   10 May 1996 10:59:04   RHAYES
// Unicode conversion update.
// 
//    Rev 1.1   19 Mar 1996 13:53:30   jworden
// Double byte enable
// 
//    Rev 1.0   30 Jan 1996 15:52:58   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 13:22:02   BARRY
// Initial revision.
// 
//    Rev 1.2   05 Jan 1995 13:47:04   MFALLEN
// No change.
// 
//    Rev 1.1   04 Nov 1994 17:39:36   MFALLEN
// Moved to W32
// 
//
//***********************************************************************

#include "platform.h"

#ifdef SYM_WIN

//************************************************************************
// Global Variables
//************************************************************************

HINSTANCE   hInstance = NULL;

#ifdef SYM_WIN32

// -------------------------------------------------------------------------

int WINAPI LibMain(
    HINSTANCE hModule, 
    UINT wDataSeg, 
    UINT cbHeapSize,
    LPCTSTR lpszCmdLine);

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
   FUNCTION: LibMain(HANDLE, WORD, WORD, LPTSTR)

   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
             the DLL is loaded.  The LibEntry routine is provided in
             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
             source LIBENTRY.ASM is also provided.)  
             the initialization is successful.
           
*****************************************************************************/

int WINAPI LibMain(HINSTANCE hModule, UINT wDataSeg, UINT cbHeapSize,
                       LPCTSTR lpszCmdLine)
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
