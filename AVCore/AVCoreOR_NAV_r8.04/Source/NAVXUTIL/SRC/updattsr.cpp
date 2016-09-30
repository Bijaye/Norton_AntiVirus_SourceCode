// Copyright 1993 Symantec Corporation                                  
//***********************************************************************
//                                                                
// $Header:   S:/navxutil/VCS/updattsr.cpv   1.0   06 Feb 1997 21:05:10   RFULLER  $
//                                                                
// Description:                                                   
//      Updates the TSR file, with user's settings and virus defs.
//                                                                
// Contains (as exports):                                                      
//                                                                
// See Also:                                                      
//***********************************************************************
// $Log:   S:/navxutil/VCS/updattsr.cpv  $
// 
//    Rev 1.0   06 Feb 1997 21:05:10   RFULLER
// Initial revision
// 
//    Rev 1.1   22 Aug 1996 13:10:32   JBRENNA
// Remove the call to InsertSmallOVL for KIRIN. Because KIRIN is not building
// the NAVTSR project, this function was showing up as an unresolved symbol.
// 
//    Rev 1.0   30 Jan 1996 15:56:24   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:26   BARRY
// Initial revision.
// 
//    Rev 1.2   07 Feb 1995 20:29:14   MFALLEN
// ifdef unused variable.
// 
//    Rev 1.1   06 Feb 1995 15:54:00   DALLEE
// UpdateTSR() re-enabled for DOS platform.
// Now takes lpNavOptions to build custom TSR (still limited beneath this
//   in InsertSmallOVL() to only build 5k size).
// 
//    Rev 1.0   16 Dec 1994 10:37:04   MFALLEN
// Initial revision.
// 
//    Rev 1.5   15 Dec 1994 20:14:10   MFALLEN
// From NIRVANA
// 
//    Rev 1.4   10 Sep 1993 03:36:24   BARRY
// Updated buffer size and RTLink usage (not passing DataBuffer anymore)
// 
//    Rev 1.3   31 Aug 1993 16:25:02   BARRY
// Changed file name to avoid duplicate UPDATE.C in NAVD
// 
//    Rev 1.2   23 Aug 1993 00:39:32   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
// 
//    Rev 1.1   13 Aug 1993 01:47:48   BARRY
// Special-case code for RTLINK to reduce VMM needs
// 
//    Rev 1.0   12 Aug 1993 21:53:56   MFALLEN
// Initial revision.
// 
//    Rev 1.4   28 Jul 1993 14:55:00   DALLEE
// Disable TSR (DOS or Windows) around call to update TSR.
// 
//    Rev 1.3   01 Jul 1993 20:30:04   MFALLEN
// Moved #includes around
// 
//    Rev 1.2   07 Jun 1993 19:53:50   BRAD
// Pass the directory where NAV TSR exists to the function.
// 
//    Rev 1.1   07 Jun 1993 19:23:32   BRAD
// Created INSERT.H file.
// 
//    Rev 1.0   07 Jun 1993 18:52:02   BRAD
// Initial revision.
//***********************************************************************

#include "platform.h"
#include "disk.h"
#include "file.h"
#include "navutil.h"
#include "insert.h"
#include "tsr.h"
#include "tsrcomm.h"

MODULE_NAME;

//************************************************************************
// UpdateTSR()
//
// This routine calls InsertOVL() that reconfigures the NAV TSR so the
// new NAV options should take effect immediatelly.
//
// INPUTS:
//      lpszDir                 Directory to configure NAV TSR in
//      lpNavOptions            Specify options for creating TSR.
//
// RETURNS:
//      nothing
//
//************************************************************************
// 05/31/93 Martin Function Created
// 08/\93 BarryG Static buffer for RTLink builds
// 09/10/93 BarryG hDataBuffer is no longer used
//************************************************************************
UINT WINAPI UpdateTSR (LPSTR        lpszDir,
                       LPNAVOPTIONS lpNavOptions)
{
    auto     HGLOBAL     hCodeBuffer = NULL;
    auto     HGLOBAL     hHdrBuffer = NULL;
    auto     LPVOID      lpCodeBuffer = NULL;
    auto     LPVOID      lpHdrBuffer = NULL;

    auto     UINT        uStatus = ERR;

#ifdef SYM_DOS
    auto    CALLBACKREV1    rCallbacks;
#endif

    // Allocate and lock memory for the TSR

    if ( (hCodeBuffer = MemAlloc ( GHND , SIZE_CODEBUFFER )) != NULL )
        lpCodeBuffer = MemLock ( hCodeBuffer ) ;

    if ( (hHdrBuffer = MemAlloc ( GHND , SIZE_HDRBUFFER )) != NULL )
        lpHdrBuffer = MemLock ( hHdrBuffer ) ;

    // If memory allocated successfully, build the TSR

    if ( lpCodeBuffer != NULL && lpHdrBuffer != NULL )
        {
        auto    BYTE    byDisk;
        auto    char    szPreviousDir[SYM_MAX_PATH+1];

                                        // Get the old values                           
        byDisk = DiskGet();
        DirGet(byDisk, szPreviousDir);
                                        // Change to the correct directory
        DiskSet(lpszDir[0]);
        DirSet(lpszDir);
#ifdef SYM_DOS
        TSR_OFF;

        InitCertlib(&rCallbacks, INIT_CERTLIB_USE_DEFAULT);

        // JBRENNA: This was turned off because KIRIN was getting an
        // unresolved symbol. KIRIN is not building the NAVTSR project.
#ifdef COMPILING_TSR
        uStatus = InsertSmallOVL( lpNavOptions,
                                  lpCodeBuffer, SIZE_CODEBUFFER,
                                  lpHdrBuffer, SIZE_HDRBUFFER );
#endif

        TSR_ON;
#endif
                                        // Change to the previous old dir
        DiskSet(byDisk);
        DirSet(szPreviousDir);
        }

    if (lpCodeBuffer)
        MemUnlock (hCodeBuffer, lpCodeBuffer);

    if (lpHdrBuffer)
        MemUnlock (hHdrBuffer, lpHdrBuffer);

    if (hCodeBuffer)
        MemFree(hCodeBuffer);

    if (hHdrBuffer)
        MemFree(hHdrBuffer);

    return (uStatus);
}
