// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/scandlg.c_v   1.0   06 Feb 1997 21:08:58   RFULLER  $
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
// $Log:   S:/scand/VCS/scandlg.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:58   RFULLER
// Initial revision
// 
//    Rev 1.3   08 Aug 1996 13:27:26   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.61   30 Jul 1996 14:50:26   JALLEE
// Moved initialization of gbContinueScan to before MBR scan.  Now we can interrupt
// or stop during boot scans.
// 
//    Rev 1.60   18 Jul 1996 19:12:06   MZAREMB
// Fixed GPF in DX when lpgCallBack was being changed in avapi and the buffer
// it pointed to was removed later.  GPF was actually in FIL4_ATTR, but was 
// initiated by the change to lpgCallBack. Now saves and restores lpgCallBack.
// 
//    Rev 1.59   17 May 1996 12:13:12   MZAREMB
// Display dashes instead of numbers for "cleaned" fields to look like
// Windows version.
// 
//    Rev 1.58   06 May 1996 09:07:08   MZAREMB
// Initial Pegasus update: added NAVSCAN #ifdefs.
// 
//    Rev 1.57   04 Mar 1996 18:38:50   MKEATIN
// Ported DX fixes from KIRIN
// 
//    Rev 1.1   05 Feb 1996 17:52:28   MKEATIN
// Modified code relating to flag bFixedDiskPresent
// 
//    Rev 1.0   30 Jan 1996 16:00:16   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:53:30   BARRY
// Initial revision.
// 
//    Rev 1.56   17 Dec 1995 18:42:52   MKEATIN
// Call NavExInit() even if bMemoryHasBeenScanned is true
// 
//    Rev 1.55   14 Nov 1995 18:55:34   MKEATIN
// NavExInit is called during/after memory scanning. Added it here in case no 
// memory scanning occurs.
// 
//    Rev 1.54   02 Nov 1995 17:04:14   JWORDEN
// Convert MasterBootRec booleans to counters to allow for multiple MBRs
// 
//    Rev 1.53   01 Jun 1995 21:51:50   DSACKING
// Ported changes from 3.0.6
// 
//    Rev 1.52   15 Mar 1995 17:44:34   DALLEE
// Another mem/boot fillbar bug.  Needed to initialize to zero.
// 
//    Rev 1.51   14 Mar 1995 18:44:18   DALLEE
// Fixed minor problem w/ Memory fillbar showing 100% introduced w/ last change.
// 
//    Rev 1.50   14 Mar 1995 17:45:58   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.49   29 Dec 1994 16:46:06   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.48   28 Dec 1994 14:16:06   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   27 Dec 1994 19:36:26   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.1   05 Dec 1994 18:10:18   DALLEE
    // CVT1 script.
    //
//    Rev 1.47   28 Dec 1994 13:53:42   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:22   DALLEE
    // Initial revision.
    //
    //    Rev 1.41   15 Oct 1993 14:41:02   DALLEE
    // NONAV struct now keeps track of kilobytes.
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

DIALOG_RECORD   *glpScanFilesDialog;
DIALOG_RECORD   *glpMemoryBootDialog;

static  BOOL bMemoryHasBeenScanned = FALSE;

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern BOOL gbContinueScan;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID STATIC PASCAL DisplayMemoryBootStats (LPNONAV lpNoNav);


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
    extern  OutputFieldRec  ofScanMemoryBar;
    extern  BOOL            bFixedDiskPresent; // flag set in Navd.c by
    extern  LPSTR           LPSZ_ZIP_CANT_WRITE[];
    auto    BOOL            bMasterBootRec;
    auto    BOOL            bUseSystemInoc;
#if !defined(NAVSCAN) && !defined(NAVNET)
    auto    BOOL            bScanZipFiles;
    extern  BOOL            bAllowZipScanning;
#endif
    extern  LPCALLBACKREV1  lpgCallBack;        // Will be changed in avapi
    auto    LPCALLBACKREV1  lpgCallBackSave;    //  so save the original here
    
    gbContinueScan = TRUE;

    bMasterBootRec = lpNoNav->Opt.bMasterBootRec;
    bUseSystemInoc = lpNoNav->Inoc.bUseSystemInoc;
#if !defined(NAVSCAN) && !defined(NAVNET)
    bScanZipFiles  = lpNoNav->Gen.bScanZipFiles;
#endif
                                               // CheckForFixedDisk()                                                                                            
    if (!bFixedDiskPresent)
        {
        lpNoNav->Opt.bMasterBootRec = FALSE;
        lpNoNav->Inoc.bUseSystemInoc = FALSE;
        }

#if !defined(NAVSCAN) && !defined(NAVNET)
    if (lpNoNav->Gen.bScanZipFiles && !bAllowZipScanning)
        {
        lpNoNav->Gen.bScanZipFiles = FALSE;
        StdDlgMessage(LPSZ_ZIP_CANT_WRITE);
        }
#endif
    lpgCallBackSave = lpgCallBack;      // Save the original callback address

                                        // OpenScanFilesDialog may return
                                        // NULL if screen IO is not yet
                                        // initialized.
    glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);

                                        // Setup timeout for critical errors
    if ( lpNoNav -> Alert . bRemoveAfter )  
        DiskErrorSetTimeout((DWORD)lpNoNav->Alert.uSeconds*TICKS_PER_SECOND,2);

    if (lpNoNav->Opt.bMemory || lpNoNav->Opt.bMasterBootRec || lpNoNav->Opt.bBootRecs)
        {
        // Turn off the flag if memory scanning is turned off (in case
        //   the option changes after we have already scanned).
        if (lpNoNav->Opt.bMemory == FALSE)
            bMemoryHasBeenScanned = FALSE;

                                        // Open the Memory and Boot dialog
                                        // and zero all the fillbar fields.
        glpMemoryBootDialog = OpenMemoryBootDialog(lpNoNav);
                                        // Scan for viruses in Memory
        if (lpNoNav->Opt.bMemory == TRUE)
            {
            if (!bMemoryHasBeenScanned)
                {
                ScanMemory(lpNoNav);   // NavExInit() is called during a mem scan
                bMemoryHasBeenScanned = TRUE;
                }
#ifndef NAVSCAN            
            else
                {
                NavExInit();
                }
#endif
            }
#ifndef NAVSCAN            
        else
            {
            NavExInit();
            }
#endif
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

        if (NULL != glpMemoryBootDialog)
            {
            DialogClose(glpMemoryBootDialog, ACCEPT_DIALOG);
            glpMemoryBootDialog = NULL;
            }
        } // End if (lpNoNav->Opt.bMemory || ...bMasterBootRec || ...bBootRecs)
#ifndef NAVSCAN            
    else    
        {    
        NavExInit();    
        }    
#endif
                                        // Display Mem & Boot results.
    DisplayMemoryBootStats(lpNoNav);

                                        //--------------------------
                                        // Check System Inoculation.
                                        //--------------------------

#ifndef NAVSCAN
    if (lpNoNav->Inoc.bUseSystemInoc && bFixedDiskPresent) 
        {
        gbContinueScan = SystemInocCheck (lpNoNav);
        }
#endif

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

    if (NULL != glpScanFilesDialog)
        {
        DialogClose(glpScanFilesDialog, ACCEPT_DIALOG);
        glpScanFilesDialog = NULL;
        }
    
    lpNoNav->Opt.bMasterBootRec  = bMasterBootRec;
    lpNoNav->Inoc.bUseSystemInoc = bUseSystemInoc;
#if !defined(NAVSCAN) && !defined(NAVNET)
    lpNoNav->Gen.bScanZipFiles   = bScanZipFiles;
#endif
    lpgCallBack = lpgCallBackSave;      // Put the original back in.  If this
                                        //   is not done, DX will GPF when 
                                        //   lpgCalBack is referenced.

    return (ACCEPT_DIALOG);
} // End ScanDialog()


//************************************************************************
// UpdateScanDialog()
//
// This routine reads the data from the NONAV structure and updates
// the Scan Dialog output fields.
//
// Parameters:
//      LPNONAV     lpNoNav               Pointer to the scan information
//
// Returns:
//      Nothing.
//************************************************************************
// 3/24/93 DALLEE Function created.
//************************************************************************

VOID PASCAL UpdateScanDialog (LPNONAV lpNoNav)
{
    extern  OutputFieldRec  ofScanDirectory;

    static  DWORD           dwLastTime;

                                        // Do dialog updates only after
                                        // specified interval.
    if (12 < TimerElapsedTicks(dwLastTime))
        {
        dwLastTime = TimerTicks();
        ForceUpdateScanDialog(lpNoNav);
        }

} // End UpdateScanDialog()


//************************************************************************
// ForceUpdateScanDialog()
//
// This routine is called to force the update of the Scan Dialog.
// UpdateScanDialog() will only update it if one second has passed
// since the last update.
// This routine is used when dir's change and at the end of scan.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to info on this scan.
//
// Returns:
//      Nothing
//************************************************************************
// 5/28/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ForceUpdateScanDialog (LPNONAV lpNoNav)
{
    extern  OutputFieldRec  ofScanPercentBar;
    extern  OutputFieldRec  ofScanDirectory;
    extern  OutputFieldRec  ofScanFilesScan;
    extern  OutputFieldRec  ofScanFilesInfect;
    extern  OutputFieldRec  ofScanFilesClean;
    extern  char            SZ_DASH [];

    auto    BYTE            szTempNum[16];
    auto    DWORD           dwTemp;

    if (NULL != glpScanFilesDialog)
        {
                                        // Update the fillbar.
                                        //===============================
                                        // dwTemp = 95% of total to scan
                                        // this is the kludge so we don't
                                        // ever hit 100% early.
        dwTemp = lpNoNav->dwTotalKBytes/20;
        dwTemp = lpNoNav->dwTotalKBytes - dwTemp;

        if (lpNoNav->dwScannedKBytes < dwTemp)
            dwTemp = lpNoNav->dwScannedKBytes;

        StdDlgUpdateFillBar(&ofScanPercentBar,
                            dwTemp,
                            lpNoNav->dwTotalKBytes);

                                        // Update the directory we are scanning.
        DialogOutput(&ofScanDirectory,  lpNoNav->szTheDirectory);

                                        // Update Files statistics
        DialogOutput(&ofScanFilesScan, &lpNoNav->Stats.Files.dwScanned);

        dwTemp = lpNoNav->Stats.Files.uInfected;
        DialogOutput(&ofScanFilesInfect, &dwTemp);

        if (lpNoNav->Stats.Files.uCleaned)
            _ConvertLongToString(lpNoNav->Stats.Files.uCleaned, szTempNum, 10, 0);
        else
            STRCPY(szTempNum, SZ_DASH);
                    
        DialogOutput(&ofScanFilesClean, &szTempNum);
        }
} // End ForceUpdateScanDialog()


//************************************************************************
// DisplayMemoryBootStats()
//
// This routine displays the scanned, infected, cleaned columns for
// memory and the MBR in the Scanning for Viruses dialog.
//
// Parameters:
//      LPNONAV lpNoNav                 Struct with the statistics info.
//
// Returns:
//      Nothing
//************************************************************************
// 9/05/93 DALLEE, pulled this out of ForceUpdateScanDialog() since it
//                 happens only once and was slowing down the scan.
//************************************************************************

VOID STATIC PASCAL DisplayMemoryBootStats (LPNONAV lpNoNav)
{
    extern  OutputFieldRec  ofScanMemScan;
    extern  OutputFieldRec  ofScanMemInfect;
    extern  OutputFieldRec  ofScanMemClean;
    extern  OutputFieldRec  ofScanMBRScan;
    extern  OutputFieldRec  ofScanMBRInfect;
    extern  OutputFieldRec  ofScanMBRClean;
    extern  OutputFieldRec  ofScanBootScan;
    extern  OutputFieldRec  ofScanBootInfect;
    extern  OutputFieldRec  ofScanBootClean;
    extern  char            SZ_YES  [];
    extern  char            SZ_NO   [];
    extern  char            SZ_DASH [];

    auto    BYTE            szTempNum[16];

    if (NULL != glpScanFilesDialog)
        {
        DialogOutput(&ofScanMemScan,    lpNoNav->Opt.bMemory ? SZ_YES : SZ_NO);
        DialogOutput(&ofScanMemInfect,  SZ_NO);
        DialogOutput(&ofScanMemClean,   SZ_DASH);

                                        // Update the MBR statistics
        DialogOutput(&ofScanMBRScan,   (LPVOID)lpNoNav->Stats.MasterBootRec.uScanned);
        DialogOutput(&ofScanMBRInfect, (LPVOID)lpNoNav->Stats.MasterBootRec.uInfected);
        
        if ( lpNoNav->Stats.MasterBootRec.uCleaned ) 
            {
            _ConvertLongToString(lpNoNav->Stats.MasterBootRec.uCleaned, szTempNum, 10, 0);
            }
        else
            {
            STRCPY(szTempNum, SZ_DASH);
            }    
        DialogOutput(&ofScanMBRClean,  (LPVOID)szTempNum);

                                        // Update Boot Records statistics
        DialogOutput(&ofScanBootScan,   (LPVOID)lpNoNav->Stats.BootRecs.uScanned);
        DialogOutput(&ofScanBootInfect, (LPVOID)lpNoNav->Stats.BootRecs.uInfected);
        
        if ( lpNoNav->Stats.BootRecs.uCleaned ) 
            {
            _ConvertLongToString(lpNoNav->Stats.BootRecs.uCleaned, szTempNum, 10, 0);
            }
        else
            {
            STRCPY(szTempNum, SZ_DASH);
            }    
        
        DialogOutput(&ofScanBootClean,  (LPVOID)szTempNum);
        }

} // End DisplayMemoryBootStats()


//************************************************************************
// ScanMemoryBootFillBar()
//
// This routine draws the dummy memory and boot scanning fillbars.
//
// Parameters:
//      OutputFieldRec  *lpFillBar      Which fillbar to draw.
//
// Returns:
//      Nothing
//************************************************************************
// 5/27/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanMemoryBootFillBar (OutputFieldRec *lpFillBar, BOOL bDone)
{
    auto    WORD    i;

    if (glpMemoryBootDialog)
        {
        for (i = bDone ? FILLBAR_TICKS : 0; i <= FILLBAR_TICKS; i++)
            {
            _WaitForTick();
            StdDlgUpdateFillBar(lpFillBar, (DWORD) i, (DWORD) FILLBAR_TICKS);
            }
        }
} // ScanMemoryBootFillBar()


//************************************************************************
// OpenScanFilesDialog()
//
// Display the scanning files... / inoculating files... dialog.
//
// Parameters:
//      LPNONAV lpNoNav             Allow stop, plus file, mem, boot stats.
//
// Returns:
//      DIALOG_RECORD   *           if screen IO initialized.
//      NULL                        otherwise.
//************************************************************************
// 03/13/95 DALLEE, created.
//************************************************************************

DIALOG_RECORD * PASCAL OpenScanFilesDialog (LPNONAV lpNoNav)
{
    extern  OutputFieldRec  ofScanOrInoculate;
    extern  OutputFieldRec  ofScanPercentBar;
    extern  OutputFieldRec  ofScanDirectory;
    extern  OutputFieldRec  ofScanMemScan;
    extern  OutputFieldRec  ofScanMemInfect;
    extern  OutputFieldRec  ofScanMemClean;
    extern  OutputFieldRec  ofScanMBRScan;
    extern  OutputFieldRec  ofScanMBRInfect;
    extern  OutputFieldRec  ofScanMBRClean;
    extern  OutputFieldRec  ofScanBootScan;
    extern  OutputFieldRec  ofScanBootInfect;
    extern  OutputFieldRec  ofScanBootClean;
    extern  OutputFieldRec  ofScanFilesScan;
    extern  OutputFieldRec  ofScanFilesInfect;
    extern  OutputFieldRec  ofScanFilesClean;
    extern  DIALOG          dlScan;

    extern  ButtonsRec      buScanStop;
    extern  UINT            guTypeScan;
    extern  BOOL            bScreenStarted;

    extern  char            SZ_INOCULATING_FILES    [];
    extern  char            SZ_SCANNING_FOR_VIRUSES [];

    auto    DIALOG_RECORD  *lpdrDialog;

    if (bScreenStarted)
        {
        lpdrDialog = DialogOpen2( &dlScan,
                                  &ofScanOrInoculate,
                                  &ofScanPercentBar,
                                  &ofScanDirectory,
                                  &ofScanMemScan,
                                  &ofScanMemInfect,
                                  &ofScanMemClean,
                                  &ofScanMBRScan,
                                  &ofScanMBRInfect,
                                  &ofScanMBRClean,
                                  &ofScanBootScan,
                                  &ofScanBootInfect,
                                  &ofScanBootClean,
                                  &ofScanFilesScan,
                                  &ofScanFilesInfect,
                                  &ofScanFilesClean );

                                        // Display what we're doing.
        DialogOutput( &ofScanOrInoculate,
                      guTypeScan == SCAN_INOCULATION ? SZ_INOCULATING_FILES :
                                                       SZ_SCANNING_FOR_VIRUSES );

                                        // Disable the STOP button, if we
                                        // need to.
        ButtonEnable(&buScanStop, 0, lpNoNav->Opt.bAllowScanStop);

        DisplayMemoryBootStats(lpNoNav);
        ForceUpdateScanDialog(lpNoNav);

        WinFlush();
        }
    else
        {
        lpdrDialog = NULL;
        }

    return (lpdrDialog);
} // OpenScanFilesDialog()


//************************************************************************
// OpenMemoryBootDialog()
//
// Display the scanning memory, mbr, boot dialog, if screen IO has been
// started.
//
// Parameters:
//      LPNONAV     lpNoNav     Whether mem/MBR have been scanned already.
//
// Returns:
//      DIALOG_RECORD *         pointer to open dialog.
//      NULL                    if screen IO hasn't been initialized.
//************************************************************************
// 03/13/94 DALLEE created.
//************************************************************************

DIALOG_RECORD * PASCAL OpenMemoryBootDialog (LPNONAV    lpNoNav)
{
    extern  OutputFieldRec  ofScanMemoryBar;
    extern  OutputFieldRec  ofScanMBRBar;
    extern  OutputFieldRec  ofScanBootRecordsBar;
    extern  DIALOG          dlScanMemoryBoot;

    extern  BOOL            bScreenStarted;

    auto    DIALOG_RECORD  *lpdrDialog;

    if (bScreenStarted)
        {
        lpdrDialog = DialogOpen2(&dlScanMemoryBoot,
                                 &ofScanMemoryBar,
                                 &ofScanMBRBar,
                                 &ofScanBootRecordsBar);

        StdDlgUpdateFillBar(&ofScanMemoryBar, 0, 1);
        StdDlgUpdateFillBar(&ofScanMemoryBar, bMemoryHasBeenScanned, 1);

                                        // -------------------------------------
                                        // This is kind of cheating but the MBRs
                                        // scan so quickly that no one could see
                                        // the bar fill anyway
                                        // -------------------------------------
        StdDlgUpdateFillBar(&ofScanMBRBar, 0, 1);
        StdDlgUpdateFillBar(&ofScanMBRBar, (lpNoNav->Stats.MasterBootRec.uScanned > 0), 1);

        StdDlgUpdateFillBar(&ofScanBootRecordsBar, 0, 1);
        }
    else
        {
        lpdrDialog = NULL;
        }

    return (lpdrDialog);
} // OpenMemoryBootDialog()

