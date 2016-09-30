// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/scanact.c_v   1.0   06 Feb 1997 21:08:46   RFULLER  $
//
// Description:
//      These are the functions for actions taken after scanning for
//      viruses.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/scanact.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:46   RFULLER
// Initial revision
// 
//    Rev 1.1   08 Aug 1996 13:27:28   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.59   26 Jun 1996 09:42:28   JALLEE
// Corrected Problems Found list generation for entries containing infected
// zip files.
// 
//    Rev 1.58   13 Jun 1996 14:04:40   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.57   29 Nov 1995 22:18:50   MKEATIN
// Fixed dialog messages
// 
//    Rev 1.56   21 Jun 1995 11:40:34   SZIADEH
// activated mus4_beep()
// 
//    Rev 1.55   07 Jun 1995 10:27:30   BARRY
// Remove ZIP dependencies
// 
//    Rev 1.54   14 Mar 1995 17:46:04   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.53   29 Dec 1994 16:46:14   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.52   28 Dec 1994 14:15:52   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.4   27 Dec 1994 19:36:22   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.3   27 Dec 1994 15:41:52   DALLEE
    // Commented out MUS4_BEEP.
    //
    //    Rev 1.2   12 Dec 1994 18:10:56   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.1   05 Dec 1994 18:10:16   DALLEE
    // CVT1 script.
    //
//    Rev 1.51   28 Dec 1994 13:53:26   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:16   DALLEE
    // Initial revision.
    //
    //    Rev 1.46   04 Nov 1993 16:01:18   DALLEE
    // Allow repairs (under DOS) for DIR2 again.  Repairs should only be disabled
    // for Windows where the low-level tweaking of directory entries required
    // isn't feasable.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"

#include "ctsn.h"
#include "virscan.h"

#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"

#include "navdprot.h"
#include "scand.h"
#ifdef USE_NETWORKALERTS   //&?
 #include "netalert.h"
#endif


//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define VIRUS_FOUND_REPAIR_BUTTON       0
#define VIRUS_FOUND_DELETE_BUTTON       1
#define VIRUS_FOUND_EXCLUDE_BUTTON      2
#define VIRUS_FOUND_INFO_BUTTON         3
#define VIRUS_FOUND_STOP_BUTTON         4
#define VIRUS_FOUND_CONTINUE_BUTTON     5


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL PromptResponse (LPNONAV lpNoNav);
VOID LOCAL PASCAL DisablePromptResponseButtons (LPNONAV lpNoNav);
int STATIC PromptResponseButtonsProc(DIALOG_RECORD *lpdrDialog);


//************************************************************************
// RespondToVirus()
//
// This routine decides what action to take when a virus is found by
// the scanner depending on the Scanner Options settings.
//
// Parameters:
//      LPNONAV     lpNav               The present scan's information
//
// Returns:
//      Nothing
//************************************************************************
// 3/28/93 DALLEE Function created.
//************************************************************************

VOID PASCAL RespondToVirus (LPNONAV lpNoNav)
{
#ifndef NAVSCAN
    auto    LOGSTRUCT   Log;
#ifdef USE_NETWORKALERTS   //&?
    auto    NETALERT    rAlert;
#endif
#endif  // !NAVSCAN

                                        // VirusIndex = -1 is error
    if ( (lpNoNav->Infected.uVirusSigIndex != 0) &&
         (lpNoNav->Infected.uVirusSigIndex != (WORD) -1) )
        {
        if (lpNoNav->Alert.bBeep && lpNoNav->Opt.bPrestoNotify)
            {
            MUS4_BEEP(BEEP_KNOWNBOX);
            }
        lpNoNav->Stats.Files.uInfected++;

        ErrorLevelSet ( ERRORLEVEL_VIRUSFOUND ) ;

#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS   //&?
        MEMSET(&rAlert, '\0', sizeof(rAlert));
        rAlert.lpNet = lpNoNav->lpNetUserItem;
        rAlert.lpInfected = &lpNoNav->Infected;
        rAlert.lpAlertOpt = &lpNoNav->Alert;
        rAlert.uTypeOfEvent = EV_ALERT_KNOWN_VIRUS;

        NetAlert(&rAlert);
#endif

                                        // Switch on what type of action to
                                        // take for known virus.
        switch (lpNoNav->Opt.uAction)
            {
            case SCAN_PROMPT:
                                        // If Immediate Notification is set,
                                        // act now.
                if (lpNoNav->Opt.bPrestoNotify)
                    {
                    ScanStopWatch(lpNoNav, FALSE);
                    PromptResponse(lpNoNav);
                    ScanStopWatch(lpNoNav, TRUE);
                    }
                break;

            case SCAN_REPAUTO:
                ScanRepairFile(lpNoNav, &lpNoNav->Infected);

                                        // Should we notify the user now?
                if (lpNoNav->Opt.bPrestoNotify == TRUE)
                    {
                    ScanStopWatch(lpNoNav, FALSE);
                    ScanActionMessage(&lpNoNav->Infected);
                    ScanStopWatch(lpNoNav, TRUE);
                    }
                break;

            case SCAN_DELAUTO:
                DeleteFile(lpNoNav, &lpNoNav->Infected);

                                        // Should we notify the user now?
                if (lpNoNav->Opt.bPrestoNotify == TRUE)
                    {
                    ScanStopWatch(lpNoNav, FALSE);
                    ScanActionMessage(&lpNoNav->Infected);
                    ScanStopWatch(lpNoNav, TRUE);
                    }
                break;

            case SCAN_REPORT:
                                        // If Immediate Notification is set,
                                        // act now.
                if (lpNoNav->Opt.bPrestoNotify == TRUE)
                    {
                    ScanStopWatch(lpNoNav, FALSE);
                    ScanNotify(lpNoNav);
                    ScanStopWatch(lpNoNav, TRUE);
                    }
                break;

            case SCAN_HALTCPU:
                if (NOERR == NAVDWriteLog(lpNoNav))
                    {
                    MEMSET(&Log, '\0', sizeof(Log));
                    Log.uEvent      = LOGEV_HALTCPU;
                    Log.lpFileName  = lpNoNav->Infected.szFullPath;
                    Log.lpVirusName = lpNoNav->Infected.Notes.lpVirName;
                    Log.lpAct       = &lpNoNav->Act;
                    LogEvent(&Log);
                    }
                ScanHalt(lpNoNav);
                break;
            } // End switch(lpNoNav->...
#endif  // !NAVSCAN

        AddVirusInfo(lpNoNav);
        }

} // End RespondToVirus()


//************************************************************************
// PromptResponse()
//
// This routine prompts the user for what action to take when a
// virus has been found in a file.
//
// Parameters:
//      LPNONAV lpNoNav                 Info on this scan.
//
// Returns:
//      Nothing
//************************************************************************
// 3/28/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL PromptResponse (LPNONAV lpNoNav)
{
    extern  DIALOG_RECORD   *glpScanFilesDialog;
    extern  BOOL            bScreenStarted;

    extern  DIALOG          dlVirusFound;
    extern  ButtonsRec      buVirusFound;
    extern  char            *lpszVirusFoundPromptStrings[];

    auto    BOOL            bDone = FALSE;
    auto    WORD            wResult;
    auto    DIALOG_RECORD   *lpdrDialog;

    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][50 + 1];
    auto    char            lpMessageBuffer [5][50 + 1];
    auto    LPSTR           lpszMessage [5] =
                                {
                                lpMessageBuffer[0],
                                lpMessageBuffer[1],
                                lpMessageBuffer[2],
                                lpMessageBuffer[3],
                                lpMessageBuffer[4]
                                };

                                        // Make sure screen IO is started.
    if (!bScreenStarted)
        {
        InitDisplay(TRUE);
        glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
        }

                                        // Set up the dialog.
    dlVirusFound.buttons = &buVirusFound;
    dlVirusFound.strings = lpszVirusFoundPromptStrings;

    if ( lpNoNav->Alert.bDispAlertMsg )
        {
        lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
                                     lpNoNav->Alert.szAlertMsg,
                                     50);
        CutString(lpAlertMessageBuffer[1],
              lpszAlertMessageNext,
              50);
        }
    else
        {
        lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
        }

    ScanFoundCreateMessage(lpszMessage, &lpNoNav->Infected);

    lpdrDialog = DialogOpen2( &dlVirusFound,
                              lpszMessage[0],
                              lpszMessage[1],
                              lpszMessage[2],
                              lpszMessage[3],
                              lpszMessage[4],
                              lpAlertMessageBuffer[0],
                              lpAlertMessageBuffer[1]);

    DisablePromptResponseButtons(lpNoNav);

    do
        {
        wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

                                        // If ESC was hit, continue with
                                        // the scan (if allowed).
        if ((wResult == ABORT_DIALOG) && (lpNoNav->Opt.bDispCont))
            {
            bDone = TRUE;
            }
                                        // Otherwise check for a button
                                        // press.
        else if (wResult == ACCEPT_DIALOG)
            {
            wResult = PromptResponseButtonsProc(lpdrDialog);
            if ((wResult == ACCEPT_DIALOG) || (wResult == ABORT_DIALOG))
                {
                bDone = TRUE;
                }
            DisablePromptResponseButtons(lpNoNav);
            }
        }
        while (!bDone);

    DialogClose(lpdrDialog, wResult);
} // End PromptResponse()


//************************************************************************
// PromptResponseButtonsProc()
//
// This routine handles button presses for the Virus Found prompt for
// response dialog.
//
// Parameters:
//      DIALOG_RECORD *drDialog
//
// Returns:
//      EVENT_AVAIL                     further processing of event
//      ABORT_DIALOG                    Stop hit
//      ACCEPT_DIALOG                   Continue hit
//************************************************************************
// 3/2/93 DALLEE Function created.
//************************************************************************

int STATIC PromptResponseButtonsProc(DIALOG_RECORD *lpdrDialog)
{
    extern  LPNONAV     glpNoNav;
    extern  BOOL        gbContinueScan;

    auto    int         nRetVal = EVENT_AVAIL;

    switch (lpdrDialog->d.buttons->value)   // Current button selected
        {
        case VIRUS_FOUND_REPAIR_BUTTON:
            if (ScanRepairFile(glpNoNav, &glpNoNav->Infected))
                {
                nRetVal = ACCEPT_DIALOG;
                }
            ScanActionMessage(&glpNoNav->Infected);
            break;

        case VIRUS_FOUND_DELETE_BUTTON:
            if (DeleteFile(glpNoNav, &glpNoNav->Infected) == TRUE)
                {
                nRetVal = ACCEPT_DIALOG;
                }
            ScanActionMessage(&glpNoNav->Infected);
            break;

        case VIRUS_FOUND_EXCLUDE_BUTTON:
#ifdef USE_PASSWORD    //&?
            if (AskForPassword(PASSWORD_MODIEXCLUSIONS))
#endif
                {
                if (ScanExcludeInfectedFile(glpNoNav, &glpNoNav->Infected))
                    {
                    nRetVal = ACCEPT_DIALOG;
                    }
                ScanActionMessage(&glpNoNav->Infected);
                }
            break;

        case VIRUS_FOUND_INFO_BUTTON:
            ScanInfoDialog(TRUE);
            break;

        case VIRUS_FOUND_STOP_BUTTON:
            gbContinueScan = FALSE;
            nRetVal = ABORT_DIALOG;
            break;

        case VIRUS_FOUND_CONTINUE_BUTTON:
            nRetVal = ACCEPT_DIALOG;
            break;
        } // End switch (lpButtons->value)

    return (nRetVal);                   // default return value
} // End PromptResponseButtonsProc()


//************************************************************************
// DisablePromptResponseButtons()
//
// This routine disables buttons in the PromptResponse dialog depending
// on the SCANNER options.
//
// Parameters:
//      LPNONAV     lpNoNav             Options for the present scan.
//
// Returns:
//      nothing
//************************************************************************
// 5/3/93 DALLEE, Function created.
// 8/9/93 DALLEE, Use FileStatus for buttons to display.
//************************************************************************

VOID LOCAL PASCAL DisablePromptResponseButtons (LPNONAV lpNoNav)
{
    extern  ButtonsRec          buVirusFound;
    extern  FILESTATUSSTRUCT    FileStatus[];

    auto    WORD                wStatus;
    auto    BYTE                byRepair;
    auto    BYTE                byDelete;
    auto    BYTE                byExclude;

    wStatus = lpNoNav->Infected.wStatus;

					//*** WARNING ********************
					// DIR2 repairs are enabled under
					// DOS, but not under Windows.
					// Since DIR2 links executable dir
					// entries to itself, the repair
					// involves low-level tweaking
					// which is not feasable under Windows.
					// Deletes aren't allowed in either
					// clinic since this would invalidate
					// the directories and destroy all
					// infected files.
					//		- DALLEE 11/03/93
    byRepair  = FileStatus[wStatus].byCanRepair && lpNoNav->Opt.bDispRepair;

    byDelete  = FileStatus[wStatus].byCanDelete &&
                lpNoNav->Opt.bDispDelete        &&
                !(lpNoNav->Infected.Notes.wInfo & bitINFO_DIR2);

    byExclude = FileStatus[wStatus].byCanExclude && lpNoNav->Opt.bDispExcl;

    ButtonEnable(&buVirusFound,
                 VIRUS_FOUND_REPAIR_BUTTON,
                 byRepair);

    ButtonEnable(&buVirusFound,
                 VIRUS_FOUND_DELETE_BUTTON,
                 byDelete);

    ButtonEnable(&buVirusFound,
                 VIRUS_FOUND_EXCLUDE_BUTTON,
                 byExclude);

    ButtonEnable(&buVirusFound,
                 VIRUS_FOUND_STOP_BUTTON,
                 lpNoNav->Opt.bAllowScanStop);

                                        // Force at least the Continue button
                                        // to be displayed if every other
                                        // button is disabled.
    if (!byRepair   &&
        !byDelete   &&
        !byExclude  &&
        !lpNoNav->Opt.bAllowScanStop)
	{
    	ButtonEnable(&buVirusFound,
                     VIRUS_FOUND_CONTINUE_BUTTON,
                     TRUE);
	}
    else
	{
    	ButtonEnable(&buVirusFound,
                     VIRUS_FOUND_CONTINUE_BUTTON,
                     lpNoNav->Opt.bDispCont);
	}

} // End DisablePrompResponseButtons()

//************************************************************************
// RespondToVirusInZip()
//
// This routine decides what action to take when a virus is found by
// the scanner in a ZIP file depending on the Scanner Options settings.
//
// Parameters:
//      LPNONAV     lpNav               The present scan's information
//
// Returns:
//      Nothing
//************************************************************************
// 7/13/93 DALLEE Function created.
// 8/12/93 DALLEE, Simplify filename creation.  Now no longer storing
//              paths internal to the zip file.
// 9/18/93 BarryG Handle HALT.  NOTE:  This function should be merged with
//                RespondToVirus() to avoid inconsistencies.
//************************************************************************

VOID PASCAL RespondToVirusInZip (LPNONAV lpNoNav)
{
    extern  BOOL        gbContinueScan;
#ifndef NAVSCAN
    auto    LOGSTRUCT   Log;
    auto    NETALERT    rAlert;
#endif  // !NAVSCAN
                                        // ------------------------------
                                        // Get the filename in the form
                                        // "C:\DIR\FILE.ZIP#INFECTED.FIL"
                                        // ------------------------------
    STRCPY(lpNoNav->Infected.szFullPath, lpNoNav->Zip.lpZipFileName);
    StringAppendChar(lpNoNav->Infected.szFullPath, ZIP_SEPARATOR);
    STRCAT(lpNoNav->Infected.szFullPath, lpNoNav->Zip.szZipChildFile);

                                        // VirusIndex = -1 is error
    if ( (lpNoNav->Infected.uVirusSigIndex != 0) &&
         (lpNoNav->Infected.uVirusSigIndex != (WORD) -1) )
        {
        lpNoNav->Infected.wStatus = FILESTATUS_ZIPINFECTED;

        if (lpNoNav->Alert.bBeep && lpNoNav->Opt.bPrestoNotify)
            {
            MUS4_BEEP(BEEP_KNOWNBOX);
            }
        lpNoNav->Stats.Files.uInfected++;

        ErrorLevelSet ( ERRORLEVEL_VIRUSFOUND ) ;

#ifndef NAVSCAN
        MEMSET(&rAlert, '\0', sizeof(rAlert));
        rAlert.lpNet = lpNoNav->lpNetUserItem;
        rAlert.lpInfected = &lpNoNav->Infected;
        rAlert.lpAlertOpt = &lpNoNav->Alert;
        rAlert.uTypeOfEvent = EV_ALERT_KNOWN_VIRUS;

        NetAlert(&rAlert);

        switch ( lpNoNav->Opt.uAction )
            {
            case SCAN_PROMPT:
            case SCAN_REPAUTO:
            case SCAN_DELAUTO:
            case SCAN_REPORT:
                                        // If Immediate Notification is set,
                                        // act now.
                if (lpNoNav->Opt.bPrestoNotify == TRUE)
                {
                ScanStopWatch(lpNoNav, FALSE);
                ScanNotify(lpNoNav);
                ScanStopWatch(lpNoNav, TRUE);
                }
                break ;

            case SCAN_HALTCPU:
                // Can't act now because ZIP engine must cleanup.
                // So, force an exit for ZIP, and let main loop actually
                // do the halt (everything else preparing for it is done
                // here).

                gbContinueScan = FALSE; // Force an exit from ZIP

                                        // Save log information
                                        // (including this new entry)
                if (NOERR == NAVDWriteLog(lpNoNav))
                    {
                    MEMSET(&Log, '\0', sizeof(Log));
                    Log.uEvent      = LOGEV_HALTCPU;
                    Log.lpFileName  = lpNoNav->Infected.szFullPath;
                    Log.lpVirusName = lpNoNav->Infected.Notes.lpVirName;
                    Log.lpAct       = &lpNoNav->Act;
                    LogEvent(&Log);
                    }

                // Last step (ScanHalt) is done when exiting ZipProcessFile()
                break ;
            }
#endif  // !NAVSCAN

        AddVirusInfo(lpNoNav);
        }

} //n End RespondToVirusInZip()

