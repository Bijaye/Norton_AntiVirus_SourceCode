// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SCAND/VCS/scnstart.c_v   1.2   15 Aug 1997 10:19:12   JBRENNA  $
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
// $Log:   S:/SCAND/VCS/scnstart.c_v  $
// 
//    Rev 1.2   15 Aug 1997 10:19:12   JBRENNA
// Add NULL to VirusScanInit call. Should we be passing <NAVDIR>\NAVEX15.INF?
// 
//    Rev 1.1   17 Jun 1997 18:06:26   MKEATIN
// Now passing &gstNAVCallBacks in VirusScanInit().
// 
//    Rev 1.0   06 Feb 1997 21:08:50   RFULLER
// Initial revision
// 
//    Rev 1.2   08 Aug 1996 15:46:40   JBRENNA
// LuigiPlus Port:
//   JWORDEN: Pass file name in Infected.szFullOemPath to match shared
//     functions in NAVXUTIL.
// 
// 
//    Rev 1.1   08 Aug 1996 13:28:02   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.60   13 Jun 1996 14:04:42   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.59   02 Nov 1995 17:04:22   JWORDEN
// Convert MasterBootRec booleans to counters to allow for multiple MBRs
// 
//    Rev 1.58   21 Jun 1995 11:40:08   SZIADEH
// activated mus4_beep()
// 
//    Rev 1.57   22 Mar 1995 15:38:40   DALLEE
// Pop up Scan Results screen if in startup mode and anything happened
// during the scan.
// 
//    Rev 1.56   15 Mar 1995 17:46:16   DALLEE
// Replace StdDlgError() with NavDlgError() to ensure we pop up if hidden.
// 
//    Rev 1.55   14 Mar 1995 17:45:58   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.54   07 Feb 1995 13:46:50   DALLEE
// Removed bUpdateTSR.  TSR is now created based on /STARTUP switch.
// 
//    Rev 1.53   29 Dec 1994 16:46:18   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.52   28 Dec 1994 14:16:04   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.4   27 Dec 1994 19:36:28   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.3   27 Dec 1994 15:41:54   DALLEE
    // Commented out MUS4_BEEP.
    //
    //    Rev 1.2   12 Dec 1994 18:10:58   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.1   05 Dec 1994 18:10:22   DALLEE
    // CVT1 script.
    //
//    Rev 1.51   28 Dec 1994 13:53:40   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:40   DALLEE
    // Initial revision.
    //
    //    Rev 1.43   13 Dec 1993 13:50:22   DALLEE
    // New scan end and scan interrupted logging -- adds items scanned.
//************************************************************************

#include "platform.h"
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
#include "navcb.h"
#ifdef USE_NETWORKALERTS   //&?
#include "netalert.h"
#endif

MODULE_NAME;

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
    extern  LPSTR       LPSZ_NO_DAT_FILES [];
    extern  LPSTR       LPSZ_ERR_WRITE_LOG [];

    auto    LPSTR       lpScanList;
    auto    HGLOBAL     hNoNav;
    auto    LPNONAV     lpNoNav;
    auto    WORD        wResult;
    auto    LOGSTRUCT   Log;
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

        wResult = VirusScanInit(&gstNAVCallBacks, NULL);
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
                if ( lpNoNav->Alert.bBeep &&
                     (lpNoNav->Stats.Files.uInfected    ||
                      lpNoNav->Stats.BootRecs.uInfected ||
                      lpNoNav->Stats.MasterBootRec.uInfected) )
                    {
                    MUS4_BEEP(BEEP_KNOWNBOX);
                    }

                if ( (lpNoNav->lpInfArr) &&
                     (lpNoNav->lpInfArr->uNumUsed) )
                    {
                    ScanFoundDialog(lpNoNav);
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

            if ( (PROGRAM_INTERACTIVE == lpScan->wCallWhenDone) ||
                 (bStartupMode && lpNoNav->lpInfArr && lpNoNav->lpInfArr->uNumUsed) )
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
                    lpNoNav->Infected.szFullOemPath[0] = EOS;
                    }
                Log.lpFileName = lpNoNav->Infected.szFullPath;

                Log.lpAct   = &lpNoNav->Act;
                wResult = LogEvent(&Log);
                }

            if (NOERR != wResult)
                {
                NavDlgError(LPSZ_ERR_WRITE_LOG);
                }
                 
            NAVDFreeLinkList(lpNoNav);
            }
        else                            // VirusScanInit() failed.
            {
            NavDlgError(LPSZ_NO_DAT_FILES);
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

