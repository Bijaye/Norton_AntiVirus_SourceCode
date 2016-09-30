// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/VIRSCAN/VCS/scn_end.cpv   1.12   28 Jun 1996 11:25:40   JBRENNA  $
//
// Description:
//      Contains the routine to clean up and free all the buffers used
//      during scanning.
//
// Contains:
//      ScanEnd()
//
// See Also:
//      SCN_INIT.C for VirScanInit()
//************************************************************************
// $Log:   S:/VIRSCAN/VCS/scn_end.cpv  $
// 
//    Rev 1.12   28 Jun 1996 11:25:40   JBRENNA
// MKeatin - Make changes so NAVEX32.DLL loads and unloads
// statically.
// 
//    Rev 1.11   14 Feb 1996 13:46:22   MKEATIN
// Fixed syntax error
// 
//    Rev 1.10   14 Feb 1996 12:45:22   MKEATIN
// Made changes for navex/dx platform
// 
//    Rev 1.9   14 Feb 1996 10:03:22   CNACHEN
// Updated for PAM support.
// 
//    Rev 1.8   12 Jan 1996 07:11:36   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.7.2.7   17 Dec 1995 18:53:58   MKEATIN
// Changed cleanup for Dos NavEx buffer
// 
//    Rev 1.7.2.6   31 Oct 1995 13:06:14   CNACHEN
// Added (IFDEF'd) PAM support for W32
// 
//    Rev 1.7.2.5   13 Oct 1995 14:37:52   DCHI
// Changed avex.h to navex.h.
// 
//    Rev 1.7.2.4   18 Sep 1995 13:07:22   DCHI
// Added #ifdef SYM_TSR so that TSR does not get new 64K barrier code.
// 
//    Rev 1.7.2.3   12 Sep 1995 16:26:14   DCHI
// Added call to FreeScanFileData() when virus buffers are freed.
// 
//    Rev 1.7.2.2   07 Sep 1995 16:16:52   DALLEE
// Changed Loader.h to Avexload.h
// 
//    Rev 1.7.2.1   07 Sep 1995 13:49:20   CNACHEN
// Added external virus detection code.
// 
//    Rev 1.7.2.0   12 Jul 1995 18:59:50   MPYE
// Branch base for version QAK8
// 
//    Rev 1.7   27 Dec 1994 19:05:56   DALLEE
// Use CERTLIBN for DOS platform now, too.
// 
//    Rev 1.6   19 Dec 1994 22:05:22   DALLEE
// Convert .WIN to CERTLIBN
// 
//    Rev 1.5   30 Nov 1994 12:28:32   DALLEE
// Use CTS4.H for SYM_DOS, too.
// 
//    Rev 1.4   28 Nov 1994 15:29:10   MFALLEN
// include sts4.h if W16 platform.
// 
//    Rev 1.3   12 May 1994 11:04:04   MFALLEN
// cts4 2 ctsn
// 
//    Rev 1.2   27 Apr 1994 10:56:40   BRAD
// Use SYM_MAX_PATH only
// 
//    Rev 1.1   22 Apr 1994 18:15:58   MFALLEN
// Changed _loadds to SYM_EXPORT. If the TSR will need _loadds then this must
// be a macro. Without this change the LINK32 will not generate an import lib.
// 
//    Rev 1.0   22 Apr 1994 10:58:58   MFALLEN
// Initial revision.
// 
//    Rev 1.11   15 Mar 1994 11:09:30   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.10   02 Mar 1994 17:12:32   BRAD
// AWK script applied
// 
//    Rev 1.9   11 Feb 1994 15:54:56   MARKL
// Quake 2 changes moved to trunk.
// 
//    Rev 1.8.1.0   19 Oct 1993 08:56:18   SCOTTP
// Branch base for version QAK2
// 
//    Rev 1.8   12 Oct 1993 16:42:42   PATRICKM
// Baseline from QAK1 branch
// 
//    Rev 1.7.1.0   03 Sep 1993 22:52:48   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.7   23 Aug 1993 01:51:20   BARRY
// Added MODULE_NAME and fixed MemUnlock params for VMM debugging
// 
//    Rev 1.6   01 Jul 1993 17:24:06   BRAD
// Added 'uUser', so multiple programs can use the VIRSCAN.DLL.
// 
//    Rev 1.5   10 Jun 1993 02:51:10   CGUC
// Changed SYM_EXPORT to _loadds
// 
//    Rev 1.4   28 Apr 1993 09:10:08   MFALLEN
// 
//    Rev 1.3   22 Apr 1993 10:39:18   MFALLEN
// N30Notes
// 
//    Rev 1.2   26 Mar 1993 14:52:54   DALLEE
// Support for DOS.
//************************************************************************

#include "platform.h"
#include "ctsn.h"
#include "virscan.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#ifdef SYM_WIN32
#include "navex.h"
#endif

#ifdef SYM_DOS

#include "navex.h"
#include "avexload.h"                   // for DOS loader code

extern GLOBAL_LOADER_INFO_TYPE      gLoaderInfo;

#endif

extern VIRSCAN  ScanData;
extern UINT     uUsers;

MODULE_NAME;

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************
VOID LOCAL PASCAL FreeVirScanBuffers(VOID);

//************************************************************************
// ScanEnd()
//
//  Cleans up after VirScanInit(). This function
//  should be called to terminate scanning.
//
// Parameters: none
//
// Returns:
//  TRUE if OK, or FALSE if error
//                                           
//************************************************************************
// 3/\93 Martin function created.
//************************************************************************



BOOL SYM_EXPORT WINAPI VirusScanEnd (VOID)
{
    if ( uUsers )
        {
        uUsers--;                       // Unregister a user

                                        // If last user, free memory
        if ( uUsers == 0 )
            FreeVirScanBuffers();
        }

   return (TRUE);
} // End VirScanEnd()


VOID LOCAL PASCAL FreeVirScanBuffers(VOID)
{

#ifdef SYM_WIN32
    NavEx32End();
#endif

#ifdef SYM_DOS

    if (gLoaderInfo.lpOverlayPtr) // NavEx buffer (the same as MemScan buffer)
        {
        gLoaderInfo.lpOverlayPtr = NULL;    
        gLoaderInfo.bInitialized = FALSE;    
        }

#endif

//////////////////////////////////////////////////////////////////////////////
// PAM stuff!
//////////////////////////////////////////////////////////////////////////////

#ifdef USE_PAM
    if (ScanData.hGPAM != NULL)
        {
            PAMLocalClose(ScanData.hLPAM);
            PAMGlobalClose(ScanData.hGPAM);

            ScanData.hGPAM = NULL;
            ScanData.hLPAM = NULL;
        }
#endif
//////////////////////////////////////////////////////////////////////////////

    if (ScanData.hWorkMem)
        {
        MemRelease ( ScanData.hWorkMem );
        MemFree( ScanData.hWorkMem );
        }

    if (ScanData.hFileScanData)
        {
#ifndef SYM_TSR
        // File scan data has a special format

        FreeScanFileData(ScanData.lpFileScanData);  // DCHI
#endif
        MemRelease (ScanData.hFileScanData );
        MemFree   (ScanData.hFileScanData);
        }

    if (ScanData.hMemoryScanData)
        {
        MemRelease (ScanData.hMemoryScanData );
        MemFree   (ScanData.hMemoryScanData);
        }

    if (ScanData.hBootScanData)
        {
        MemRelease ( ScanData.hBootScanData );
        MemFree   ( ScanData.hBootScanData );
        }

    if (ScanData.hCallbacks)
        {
        MemRelease(ScanData.hCallbacks);
        MemFree(ScanData.hCallbacks);
        }
}

