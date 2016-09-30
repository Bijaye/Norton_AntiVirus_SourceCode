// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/scandlg.c_v   1.2   08 Jun 1997 23:24:22   BGERHAR  $
//
// Description:
//      These are the functions for the DOS NAV Scan Files for Viruses
//      dialog.
//
// Contains:
//
// See Also:
//      SCNSTR.C for strings and control structures.
//************************************************************************
// $Log:   S:/NAVC/VCS/scandlg.c_v  $
// 
//    Rev 1.2   08 Jun 1997 23:24:22   BGERHAR
// Don't wrap for deep scan or def directories
// 
//    Rev 1.1   29 Apr 1997 12:46:36   JTAYLOR
// Removed calls to NavExInit()
// 
//    Rev 1.0   06 Feb 1997 20:56:28   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:32   MKEATIN
// Initial revision.
// 
//    Rev 1.6   07 Nov 1996 12:28:58   JBELDEN
// removed all extra code related to dialog boxes.
// 
//    Rev 1.5   31 Oct 1996 17:59:58   JBELDEN
// removed dialog box code
// 
//    Rev 1.4   31 Oct 1996 13:50:44   JBELDEN
// removed call to systeminoccheck
// 
//    Rev 1.3   04 Oct 1996 17:00:00   JBELDEN
// Made several of the functions stubs...will probably do more
// of this.
//
//    Rev 1.2   03 Oct 1996 16:31:46   JBELDEN
// added DOSPrint for scanning memory.
//
//    Rev 1.1   03 Oct 1996 14:55:18   JBELDEN
// added changes for TTYSCAN
//
//    Rev 1.0   02 Oct 1996 12:58:54   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"

#include "ctsn.h"
#include "virscan.h"

#include "navdprot.h"
#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"
#include "scand.h"


//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define FILLBAR_TICKS           7


//************************************************************************
// GLOBAL VARIABLES
//************************************************************************

static  BOOL bMemoryHasBeenScanned = FALSE;

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern BOOL gbContinueScan;


//************************************************************************
// ScanDialog()
//
// This routine brings up the scan for viruses dialog and launches the
// scanner engine.
//
// Parameters:
//      LPNONAV     lpNoNav             NONAV struct holds info on this
//                                      scan.
//
// Returns:
//      ACCEPT_DIALOG                   Scan completed
//      ABORT_DIALOG                    Stop button hit
//************************************************************************
// 3/1/93  DALLEE, Function created.
// 6/18/93 DALLEE, Moved GetBytesToScan() after memory scan.
// 7/30/96 JALLEE, Moved gbContinueScan = TRUE before the boot/mem scan.
//************************************************************************

WORD PASCAL ScanDialog (LPNONAV lpNoNav)
{
    extern  BOOL            bFixedDiskPresent; // flag set in Navd.c by
    extern  LPSTR           LPSZ_ZIP_CANT_WRITE[];

    extern  char           szScanningMemory[];
    extern  char           szOK[];

    auto    BOOL            bMasterBootRec;
    auto    BOOL            bUseSystemInoc;
#ifndef NAVSCAN
    auto    BOOL            bScanZipFiles;
    extern  BOOL            bAllowZipScanning;
#endif
    extern  LPCALLBACKREV1  lpgCallBack;        // Will be changed in avapi
    auto    LPCALLBACKREV1  lpgCallBackSave;    //  so save the original here


    gbContinueScan = TRUE;

    bMasterBootRec = lpNoNav->Opt.bMasterBootRec;
    bUseSystemInoc = lpNoNav->Inoc.bUseSystemInoc;
#ifndef NAVSCAN
    bScanZipFiles  = lpNoNav->Gen.bScanZipFiles;
#endif
                                               // CheckForFixedDisk()
    if (!bFixedDiskPresent)
        {
        lpNoNav->Opt.bMasterBootRec = FALSE;
        lpNoNav->Inoc.bUseSystemInoc = FALSE;
        }

    lpgCallBackSave = lpgCallBack;      // Save the original callback address

                                        // OpenScanFilesDialog may return
                                        // NULL if screen IO is not yet
                                        // initialized.

                                        // Setup timeout for critical errors
    if ( lpNoNav -> Alert . bRemoveAfter )
        DiskErrorSetTimeout((DWORD)lpNoNav->Alert.uSeconds*TICKS_PER_SECOND,2);

    if (lpNoNav->Opt.bMemory || lpNoNav->Opt.bMasterBootRec || lpNoNav->Opt.bBootRecs)
        {
        // Turn off the flag if memory scanning is turned off (in case
        //   the option changes after we have already scanned).
        if (lpNoNav->Opt.bMemory == FALSE)
            bMemoryHasBeenScanned = FALSE;

        if (lpNoNav->Opt.bMemory == TRUE)
            {
            if (!bMemoryHasBeenScanned)
                {
                DOSPrint(szScanningMemory);
                ScanMemory(lpNoNav);   // NavExInit() is called during a mem scan
                DOSPrint(szOK);
                bMemoryHasBeenScanned = TRUE;
                }
            }
                                        // Scan for viruses in the
                                        // Master Boot Record
        if (lpNoNav->Opt.bMasterBootRec == TRUE && bFixedDiskPresent)
            {
            ScanMBR(lpNoNav);
            }

                                        // Scan for viruses in Boot Records
        if (lpNoNav->Opt.bBootRecs == TRUE)
            {
            ScanBootRecords(lpNoNav);
            }

        } // End if (lpNoNav->Opt.bMemory || ...bMasterBootRec || ...bBootRecs)

    DiskErrorClearTimeout ( );
                                        // Get bytes to scan after the memory
                                        // scan.  The disk access involved
                                        // can put boot virus traces into
                                        // memory.
    if (!lpNoNav->lpScan->bMemOnly &&
	!lpNoNav->lpScan->bBootOnly)
        {

        EnableStopWatch(TRUE);          // OK to start timing

    	ScanFiles(lpNoNav);             // Sets CriticalTimeout internally

        EnableStopWatch(FALSE);         // Ignore timing requests from here out
        }


    lpNoNav->Opt.bMasterBootRec  = bMasterBootRec;
    lpNoNav->Inoc.bUseSystemInoc = bUseSystemInoc;
#ifndef NAVSCAN
    lpNoNav->Gen.bScanZipFiles   = bScanZipFiles;
#endif
    lpgCallBack = lpgCallBackSave;      // Put the original back in.  If this
                                        //   is not done, DX will GPF when
                                        //   lpgCalBack is referenced.

    return (ACCEPT_DIALOG);
} // End ScanDialog()


