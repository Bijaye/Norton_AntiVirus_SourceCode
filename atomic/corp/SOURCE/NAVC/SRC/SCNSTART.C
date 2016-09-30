// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/scnstart.c_v   1.5   11 May 1998 18:16:10   mdunn  $
//
// Description:
//      This file contains ScanStart() which will initialize the scanning
//      structures, scan all drives/directories passed in the handle
//      to the szz of drives/directories, and end the scan.
//
// Contains:
//      ScanStart()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVC/VCS/scnstart.c_v  $
// 
//    Rev 1.5   11 May 1998 18:16:10   mdunn
// Fixed 2 function calls in ScanStart() that VC 5 didn't like.  Changed
// parameters of "szString" to "&szString[0]"
// 
//    Rev 1.4   13 Aug 1997 14:30:26   MKEATIN
// Removed a dubugging printf statement.
// 
//    Rev 1.3   13 Aug 1997 14:28:28   MKEATIN
// VirusScanInit() now takes an extra parameter - the full path to NAVEX15.INF
// 
//    Rev 1.2   18 Jun 1997 18:38:28   MKEATIN
// Now calling InitCallBacks() before VirusScanInit().
// 
//    Rev 1.1   17 Jun 1997 17:19:10   MKEATIN
// VirusScanInit now passes &gstNAVCallBacks.
// 
//    Rev 1.0   06 Feb 1997 20:56:26   RFULLER
// Initial revision
// 
//    Rev 1.1   06 Jan 1997 21:16:24   MKEATIN
// Added startup scanning functionality.
// 
//    Rev 1.0   31 Dec 1996 15:20:20   MKEATIN
// Initial revision.
// 
//    Rev 1.3   19 Nov 1996 11:57:56   JBELDEN
// added fprintf for logging
// 
//    Rev 1.2   12 Nov 1996 17:49:52   JBELDEN
// replaced navdlgerr code with printf
// 
//    Rev 1.1   07 Nov 1996 16:57:48   JBELDEN
// removed call to routine in scanfnd.c that was removed.
// 
//    Rev 1.0   02 Oct 1996 12:59:02   JBELDEN
// Initial revision.
//
//************************************************************************

#include "platform.h"
#include <stdio.h>
#include "stddos.h"
#include "stddlg.h"
#include "file.h"

#include "ctsn.h"
#include "virscan.h"

#include "navdprot.h"
#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"

#include "scand.h"
#include "inoc.h"
#ifdef USE_NETWORKALERTS   //&?
#include "netalert.h"
#endif

MODULE_NAME;

extern   FILE  *fLogFile;
extern   BOOL  bAutoExportLog;


//************************************************************************
// LOCAL TYPEDEFS
//************************************************************************

typedef struct tagPROBLEMSTRUC
    {
    WORD	wTestMask;
    WORD	wCount;
    }PROBLEMSTRUC, FAR * LPPROBLEMSTRUC;

//************************************************************************
// GLOBAL VARIABLES
//************************************************************************

LPNONAV glpNoNav;

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************
BOOL STATIC PASCAL ChkInfectedFiles(LPNONAV     lpNoNav,WORD wMask);
BOOL STATIC PASCAL TestFoundCallback(LPINFECTEDFILE lpInfected,
                                     LPNONAV        lpNoNav,
                                     WORD           wEntry,
                                     LPARAM         lParam );


//************************************************************************
// ScanStart()
//
// This routine is the entry point for all scanning.  It calls all the
// selected scans and pops up the action and results dialogs.
//
// Parameters:
//      LPSCANSTRUCT    lpScan          Info on what and how to scan.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/18/93 DALLEE function largely lifted from Martin N_SCNSTR.C
//************************************************************************

VOID PASCAL ScanStart(LPSCANSTRUCT lpScan)
{
    extern  BOOL        bStartupMode;
    extern  BOOL        gbContinueScan;
    extern  char        SZ_NO_DAT_FILES [];
    extern  char        SZ_ERR_WRITE_LOG [];

    auto    LPSTR       lpScanList;
    auto    HGLOBAL     hNoNav;
    auto    LPNONAV     lpNoNav;
    auto    WORD        wResult;
    auto    LOGSTRUCT   Log;
    auto    LPSTR       szNavexInfFile[SYM_MAX_PATH+1];
#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS   //&?
    auto    NETALERT    rAlert;
#endif
#endif


    _ClearErrorFar();                   // Make sure all is clear

    HyperHelpTopicSet(HELP_DLG_ASK_ACTION);
                                        // Make sure our scan list is all
                                        // uppercase
    if ((lpScanList = MemLock(lpScan->hScanList)) != NULL)
        {
        STRUPR(lpScanList);
        MemUnlock(lpScan->hScanList, lpScanList);
        }

                                        // Allocate and initialize NONAV
                                        // structure.
    if ((hNoNav = MemAlloc(GHND, sizeof(NONAV))) != NULL)
        {
        lpNoNav = (NONAV *) MemLock(hNoNav);

                                        //&? DALLEE, temp fix until I stop
                                        // using StdEventLoop() in the
                                        // Problems Found sub-dialogs --
                                        // Setup global LPNONAV for button
                                        // procedures which can't be passed
        glpNoNav = lpNoNav;             // arguments.

                                        // Read necessary info from the .INI
                                        // file.
        LoadNoNavIni(lpNoNav);
        lpNoNav->lpScan = lpScan;

        GetNavexInfFile(0, &szNavexInfFile[0], sizeof(szNavexInfFile));

                                        // Override app specific callbacks.
        InitCallBacks(&gstNAVCallBacks, 0);

        wResult = VirusScanInit(&gstNAVCallBacks, &szNavexInfFile[0]);

        if (wResult == NOERR_DAT_CHANGED)
            {
            wResult = NOERR;
            }

        if (wResult == NOERR)
            {
            wResult = ScanDialog(lpNoNav);


                                        // Display the Problems Found dialog
                                        // if Immediate Notification is off and
                                        // if any problems were encountered.
            if (lpNoNav->Opt.bPrestoNotify == FALSE)
                {
                if ( lpNoNav->Alert.bBeep               &&
                     !bStartupMode                      &&
                     (lpNoNav->Stats.Files.uInfected    ||
                      lpNoNav->Stats.BootRecs.uInfected ||
                      lpNoNav->Stats.MasterBootRec.uInfected) )
                    {
                    MUS4_BEEP(BEEP_KNOWNBOX);
                    }
                }

#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS   //&?
                                        // Alert end of scan.
            MEMSET(&rAlert, '\0', sizeof(rAlert));
            rAlert.lpNet = lpNoNav->lpNetUserItem;
            rAlert.lpAlertOpt = &lpNoNav->Alert;
            rAlert.uTypeOfEvent = EV_ALERT_END_SCAN;

            NetAlert(&rAlert);
#endif
#endif  // !NAVSCAN

            if ( (PROGRAM_INTERACTIVE == lpScan->wCallWhenDone) &&
                 (!bStartupMode) )
                {
                ScanResultsDialog(lpNoNav);
                }

            if (NOERR == (wResult = NAVDWriteLog(lpNoNav)))
                {
                                        // Log the end of the scan.
                MEMSET(&Log, '\0', sizeof(Log));
                Log.uEvent  = (gbContinueScan ? LOGEV_SCANEND
                                              : LOGEV_SCANINTERRUPTED);
                if (NULL != (lpScanList = MemLock(lpNoNav->lpScan->hScanList)))
                    {
                    BuildWhatScanned(lpScanList,
                                     lpNoNav->Infected.szFullPath,
                                     sizeof(lpNoNav->Infected.szFullPath) - 1);
                    MemUnlock(lpNoNav->lpScan->hScanList, &lpScanList);
                    }
                else
                    {
                    lpNoNav->Infected.szFullPath[0] = EOS;
                    }
                Log.lpFileName = lpNoNav->Infected.szFullPath;

                Log.lpAct   = &lpNoNav->Act;
                wResult = LogEvent(&Log);
                }

            if (NOERR != wResult)
                {
                printf(SZ_ERR_WRITE_LOG);
                }

            NAVDFreeLinkList(lpNoNav);
            }
        else                            // VirusScanInit() failed.
            {
            printf(SZ_NO_DAT_FILES);
            if (bAutoExportLog)
               fprintf(fLogFile, SZ_NO_DAT_FILES);
            }

        VirusScanEnd();

        MemUnlock(hNoNav, lpNoNav);
        MemFree(hNoNav);
        }
} // End ScanStart()


/**************************************************************************
 * ChkInfectedFiles
 *
 * DESCRIPTION  : Searches all files in infection list for one that fits
 *		  supplied mask.  Returns TRUE if one was found.
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 06-08-93 at 02:54:09pm Function created
 ************************************************************************* */

BOOL STATIC PASCAL ChkInfectedFiles(LPNONAV     lpNoNav,WORD wMask)
{
    auto	PROBLEMSTRUC	ProblemTally;
    auto	LPPROBLEMSTRUC  lpProblemTally = &ProblemTally;


    ProblemTally.wCount = 0;
    ProblemTally.wTestMask = wMask;


    ReadAllInfectedFiles(lpNoNav,
                         TestFoundCallback,
                         (LPARAM)lpProblemTally);


    return (BOOL)(ProblemTally.wCount > 0);

} // End ChkInfectedFiles()


/**************************************************************************
 * TestFoundCallback
 *
 * DESCRIPTION  : Callback procedure that tests file in infection list against
 *		  test mask
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 06-08-93 at 02:54:49pm Function created
 ************************************************************************* */
BOOL STATIC PASCAL TestFoundCallback(LPINFECTEDFILE lpInfected,
                                     LPNONAV        lpNoNav,
                                     WORD           wEntry,
                                     LPARAM         lParam )

{
    auto   LPPROBLEMSTRUC    lpTally;


    lpTally = (LPPROBLEMSTRUC)lParam;

    if (lpTally->wTestMask == lpInfected->wStatus)
	{
	lpTally->wCount++;            // if fits mask, then indicate and
	return(FALSE);		      // stop search now.
	}


    return(TRUE);

} // End TestFoundCallback()

