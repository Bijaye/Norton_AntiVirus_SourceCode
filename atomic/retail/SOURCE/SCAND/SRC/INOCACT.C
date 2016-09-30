// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/inocact.c_v   1.0   06 Feb 1997 21:08:46   RFULLER  $
//
// Description:
//      These are the functions to determine what action to take when
//      an uninoculated file, or a changed file is found during scanning.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/inocact.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:46   RFULLER
// Initial revision
// 
//    Rev 1.4   29 Oct 1996 16:10:40   BGERHAR
// Re-enable alerts. These stay in. The one displayed while creating goes.
// 
//    Rev 1.3   13 Sep 1996 16:17:02   JALLEE
// Removed custom alert messages from inoc stuff.  STS 68584
// 
//    Rev 1.2   08 Aug 1996 13:26:00   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.34   30 Jul 1996 12:18:10   JALLEE
// Added custom alert message to MBR / Boot , SYSINTEG, Inoculation dialogs.
// 
//    Rev 1.33   13 Jun 1996 14:04:32   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.32   04 Mar 1996 18:38:46   MKEATIN
// Ported DX fixes from KIRIN
// 
//    Rev 1.1   31 Jan 1996 16:15:36   MKEATIN
// Some code still sent an array of three strings instead of five to cutstring()
// 
//    Rev 1.0   30 Jan 1996 16:00:08   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:53:22   BARRY
// Initial revision.
// 
//    Rev 1.31.1.0   11 Dec 1995 16:33:34   JREARDON
// Branch base for version LUG1
// 
//    Rev 1.31   14 Mar 1995 17:46:06   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.30   06 Feb 1995 13:36:16   DALLEE
// SmartInocOpen() parameter change: Takes full path of file for which we're 
// opening the database -- necessary to distinguish NAVINOC from NAVINOCL?
// 
//    Rev 1.29   29 Dec 1994 16:46:32   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.28   28 Dec 1994 14:15:56   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.3   27 Dec 1994 19:36:20   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.2   12 Dec 1994 18:10:54   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.1   05 Dec 1994 18:10:12   DALLEE
    // CVT1 script.
    //
//    Rev 1.27   28 Dec 1994 13:53:30   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:08   DALLEE
    // Initial revision.
    //
    //    Rev 1.24   15 Oct 1993 00:24:42   DALLEE
    // Added password checking for Inoc and Exclude.
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
#include "inoc.h"
#ifdef USE_NETWORKALERTS //&?
 #include "netalert.h"
#endif


//************************************************************************
// DEFINES
//************************************************************************

                                        // #define's for uninoculated files
                                        // and Immediate notification.
#define INOC_INOCULATE_BUTTON           0
#define INOC_EXCLUDE_BUTTON             1
#define INOC_DELETE_BUTTON              2
#define INOC_CONTINUE_BUTTON            3
#define INOC_STOP_BUTTON                4

                                        // #define's for changed files and
                                        // immediate notification
#define INOC_CHANGE_REPAIR_BUTTON       0
#define INOC_CHANGE_DELETE_BUTTON       1
#define INOC_CHANGE_INOCULATE_BUTTON    2
#define INOC_CHANGE_EXCLUDE_BUTTON      3
#define INOC_CHANGE_CONTINUE_BUTTON     4
#define INOC_CHANGE_STOP_BUTTON         5


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL PromptNotInoc                 (LPNONAV lpNoNav);
WORD LOCAL PASCAL PromptNotInocButtonsProc      (DIALOG_RECORD *lpdrDialog);
VOID LOCAL PASCAL PromptInocChange              (LPNONAV lpNoNav);
WORD LOCAL PASCAL PromptInocChangeButtonsProc   (DIALOG_RECORD *lpdrDialog);
VOID LOCAL PASCAL DisableNotInocButtons         (LPNONAV lpNoNav);
VOID LOCAL PASCAL DisableInocChangeButtons      (LPNONAV lpNoNav);


//************************************************************************
// RespondToNotInoc()
//
// This routine decides what action to take when an uninoculated file is
// found during scanning.
//
// Parameters:
//      LPNONAV     lpNoNav             Information on the present scan.
//
// Returns:
//      Nothing
//************************************************************************
// 5/4/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL RespondToNotInoc (LPNONAV lpNoNav)
{
    auto    UINT    uResult;

    //*****************************************************************
    // See if user needs to provide the password
    // If they don't know the password, stop asking and switch to just
    // reporting.
    // NOTE: We ask for the password here so we have a chance to change
    //       the status to REPORT.
    //*****************************************************************

#ifdef USE_PASSWORD    //&?
    switch (lpNoNav->Inoc.uActionNewInoc)
        {
        case INOC_PROMPT:
            if (!lpNoNav->Opt.bPrestoNotify)
                {
                break;                  // Wait until the end!
                }

        case INOC_AUTO_FILEONLY:
        case INOC_AUTO:
            if (!AskForPassword(PASSWORD_INOCFILES))
                {
                lpNoNav->Inoc.uActionNewInoc = INOC_REPORT;
                }
            
            break;
        }
#endif

    switch (lpNoNav->Inoc.uActionNewInoc)
        {
        case INOC_PROMPT:
                                        // If Immediate Notification is set,
                                        // act now.
            if (lpNoNav->Opt.bPrestoNotify == TRUE)
                {
                ScanStopWatch(lpNoNav, FALSE);
                PromptNotInoc(lpNoNav);
                ScanStopWatch(lpNoNav, TRUE);
                }
            break;

        case INOC_AUTO:
        case INOC_AUTO_FILEONLY:
            uResult = (lpNoNav->uInocOpenOK == SMARTOPEN_RW) &&
                      InocFile(lpNoNav->Infected.szFullPath);

            lpNoNav->Infected.wStatus = UpdateFileStatus(
                                                lpNoNav->Infected.wStatus,
                                                PROCESS_INOCULATE,
                                                uResult);
            break;

        case INOC_REPORT:
                                    // If Immediate Notification is set,
                                    // act now.
            if (lpNoNav->Opt.bPrestoNotify == TRUE)
                {
                ScanStopWatch(lpNoNav, FALSE);
                ScanNotify(lpNoNav);
                ScanStopWatch(lpNoNav, TRUE);
                }
            break;
        } // End switch (lpNoNav->Inoc...
} // End RespondToNotInoc()


//************************************************************************
// PromptNotInoc()
//
// This routine prompts the user for the action to take when a file
// which is not inoculated is found.
//
// Parameters:
//      LPNONAV     lpNoNav             Information on the current scan.
//
// Returns:
//      Nothing
//************************************************************************

VOID LOCAL PASCAL PromptNotInoc (LPNONAV lpNoNav)
{
    extern  DIALOG_RECORD   *glpScanFilesDialog;
    extern  BOOL            bScreenStarted;

    extern  DIALOG          dlNotInocFound;
    extern  ButtonsRec      buNotInocFound;
    extern  char            *lpszNotInocPromptFound [];

    auto    BOOL            bDone = FALSE;
    auto    WORD            wResult;
    auto    DIALOG_RECORD   *lpdrDialog;

    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][SCAN_PROMPT_MESSAGE_WIDTH + 1];
    auto    char            lpMessageBuffer [SCAN_PROMPT_MESSAGE_LINES]
                                            [SCAN_PROMPT_MESSAGE_WIDTH + 1];
    auto    LPSTR           lpszMessage [SCAN_PROMPT_MESSAGE_LINES] =
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

    dlNotInocFound.buttons = &buNotInocFound;
    dlNotInocFound.strings = lpszNotInocPromptFound;

    if ( lpNoNav->Alert.bDispAlertMsg )
        {
        lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
                                     lpNoNav->Alert.szAlertMsg,
                                     SCAN_PROMPT_MESSAGE_WIDTH);
        CutString(lpAlertMessageBuffer[1],
              lpszAlertMessageNext,
              SCAN_PROMPT_MESSAGE_WIDTH);
        }
    else
        {
        lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
        }

    ScanFoundCreateMessage(lpszMessage, &lpNoNav->Infected);

    lpdrDialog = DialogOpen2( &dlNotInocFound,
                              lpszMessage[0],
                              lpszMessage[1],
                              lpszMessage[2],
                              lpszMessage[3],
                              lpszMessage[4],
                              lpAlertMessageBuffer[0],
                              lpAlertMessageBuffer[1]);

    DisableNotInocButtons(lpNoNav);

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
            wResult = PromptNotInocButtonsProc(lpdrDialog);
            if ((wResult == ACCEPT_DIALOG) || (wResult == ABORT_DIALOG))
                {
                bDone = TRUE;
                }
            DisableNotInocButtons(lpNoNav);
            }
        }
        while (!bDone);

    DialogClose(lpdrDialog, wResult);
} // End PromptResponse()


//************************************************************************
// PromptNotInocButtonsProc()
//
// This routine handles the button presses in the prompt for response
// to file which has not been inoculated dialog.
//
// Parameters:
//      DIALOG_RECORD   *lpdrDialog     The current dialog.
//
// Returns:
//      EVENT_AVAIL                     Continue with the dialog.
//      ABORT_DIALOG                    Quit the dialog.
//      ACCEPT_DIALOG                   Exit the dialog.
//************************************************************************

WORD LOCAL PASCAL PromptNotInocButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    extern  LPNONAV     glpNoNav;
    extern  BOOL        gbContinueScan;

    auto    BYTE        byValue;
    auto    WORD        wResult = EVENT_AVAIL;

    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)                    // Current button selected
        {
        case INOC_INOCULATE_BUTTON:
            SmartInocClose();
                                        // Open the database for Read/Write
            glpNoNav->uInocOpenOK
                 = SmartInocOpen(glpNoNav->Infected.szFullPath, TRUE);

            if ( glpNoNav -> uInocOpenOK != SMARTOPEN_ERROR )
                {
                if (InocFile(glpNoNav->Infected.szFullPath))
                    {
                    wResult = ACCEPT_DIALOG;
                    }
                SmartInocClose();
                }
            glpNoNav->Infected.wStatus = UpdateFileStatus(
                                                glpNoNav->Infected.wStatus,
                                                PROCESS_INOCULATE,
                                                wResult == ACCEPT_DIALOG);
            ScanActionMessage(&glpNoNav->Infected);

            glpNoNav->uInocOpenOK
                 = SmartInocOpen(glpNoNav->Infected.szFullPath, FALSE);

            break;

        case INOC_EXCLUDE_BUTTON:
#ifdef USE_PASSWORD    //&?
            if (AskForPassword(PASSWORD_MODIEXCLUSIONS))
#endif
                {
                if (ScanExcludeInfectedFile(glpNoNav, &glpNoNav->Infected))
                    {
                    wResult = ACCEPT_DIALOG;
                    }
                ScanActionMessage(&glpNoNav->Infected);
                }
            break;

        case INOC_DELETE_BUTTON:
            if (DeleteFile(glpNoNav, &glpNoNav->Infected))
                {
                wResult = ACCEPT_DIALOG;
                }
            ScanActionMessage(&glpNoNav->Infected);
            break;

        case INOC_CONTINUE_BUTTON:
            wResult = ACCEPT_DIALOG;
            break;

        case INOC_STOP_BUTTON:
            gbContinueScan = FALSE;
            wResult = ABORT_DIALOG;
            break;
        } // End switch(lpButtons->...
    return (wResult);
} // End PromptNotInocButtonsProc()


//************************************************************************
// RespondToInocChange()
//
// This routine determines what action to take when a changed file is
// found during scanning.
//
// Parameters:
//      LPNONAV     lpNoNav             Information on the current scan.
//
// Returns:
//      Nothing
//************************************************************************
// 5/4/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL RespondToInocChange (LPNONAV lpNoNav)
{
#ifdef USE_NETWORKALERTS   //&?
    auto    NETALERT        rAlert;
#endif
    auto    UINT            uResult;

    lpNoNav->Infected.wStatus = FILESTATUS_INOCULATEDCHANGED;

                                        // Special case for reinoc
                                        // automatically if run from
                                        // Tools | Inoculation...
    if (lpNoNav->Inoc.uActionNewInoc == INOC_AUTO_FILEONLY)
        {
        uResult = (lpNoNav->uInocOpenOK == SMARTOPEN_RW) &&
                  InocFile(lpNoNav->Infected.szFullPath);

        lpNoNav->Infected.wStatus = UpdateFileStatus(lpNoNav->Infected.wStatus,
                                                     PROCESS_INOCULATE,
                                                     uResult);
        }
    else
        {
#ifdef USE_NETWORKALERTS   //&?
        MEMSET(&rAlert, '\0', sizeof(rAlert));
        rAlert.lpNet = lpNoNav->lpNetUserItem;
        rAlert.lpInfected = &lpNoNav->Infected;
        rAlert.lpAlertOpt = &lpNoNav->Alert;
        rAlert.uTypeOfEvent = EV_ALERT_INOC_CHANGE;

        NetAlert(&rAlert);
#endif

        //*****************************************************************
        // See if user needs to provide the password
        // If they don't know the password, stop asking and switch to just
        // reporting.
        // NOTE: We ask for the password here so we have a chance to change
        //       the status to REPORT.
        //*****************************************************************

#ifdef USE_PASSWORD    //&?
        if ( lpNoNav->Inoc.uActionInocChg == INOC_CHANGE_PROMPT &&
             ! lpNoNav->Opt.bPrestoNotify )
            {
            if (!AskForPassword(PASSWORD_INOCFILES))
                {
                lpNoNav->Inoc.uActionInocChg = INOC_CHANGE_REPORT;
                }
            }
#endif

        switch (lpNoNav->Inoc.uActionInocChg)
            {
            case INOC_CHANGE_PROMPT:
                                            // If Immediate Notification is set,
                                            // act now.
                if (lpNoNav->Opt.bPrestoNotify == TRUE)
                    {
                    ScanStopWatch(lpNoNav, FALSE);
                    PromptInocChange(lpNoNav);
                    ScanStopWatch(lpNoNav, TRUE);
                    }
                break;

            case INOC_CHANGE_REPORT:
                                            // If Immediate Notification is set,
                                            // act now.
                if (lpNoNav->Opt.bPrestoNotify == TRUE)
                    {
                    ScanStopWatch(lpNoNav, FALSE);
                    ScanNotify(lpNoNav);
                    ScanStopWatch(lpNoNav, TRUE);
                    }
                break;
            } // End switch (lpNoNav->Inoc...
        }
} // End RespondToInocChange()


//************************************************************************
// PromptInocChange()
//
// This routine prompts the user for the action to take when a file
// whose inoculation information has changed is found.
//
// Parameters:
//      LPNONAV     lpNoNav             Information on the current scan.
//
// Returns:
//      Nothing
//************************************************************************

VOID LOCAL PASCAL PromptInocChange (LPNONAV lpNoNav)
{
    extern  DIALOG_RECORD   *glpScanFilesDialog;
    extern  BOOL            bScreenStarted;

    extern  DIALOG          dlInocChangeFound;
    extern  ButtonsRec      buInocChangeFound;
    extern  char            *lpszInocChangePromptFound [];

    auto    BOOL            bDone = FALSE;
    auto    WORD            wResult;
    auto    DIALOG_RECORD   *lpdrDialog;

    auto    LPSTR           lpszAlertMessageNext;
    auto    char            lpAlertMessageBuffer [2][SCAN_PROMPT_MESSAGE_WIDTH + 1];
    auto    char            lpMessageBuffer [SCAN_PROMPT_MESSAGE_LINES]
                                            [SCAN_PROMPT_MESSAGE_WIDTH + 1];
    auto    LPSTR           lpszMessage [SCAN_PROMPT_MESSAGE_LINES] =
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

    dlInocChangeFound.buttons = &buInocChangeFound;
    dlInocChangeFound.strings = lpszInocChangePromptFound;

    if ( lpNoNav->Alert.bDispAlertMsg )
        {
        lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
                                     lpNoNav->Alert.szAlertMsg,
                                     SCAN_PROMPT_MESSAGE_WIDTH);
        CutString(lpAlertMessageBuffer[1],
              lpszAlertMessageNext,
              SCAN_PROMPT_MESSAGE_WIDTH);
        }
    else
        {
        lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
        }

    ScanFoundCreateMessage(lpszMessage, &lpNoNav->Infected);

    lpdrDialog = DialogOpen2( &dlInocChangeFound,
                              lpszMessage[0],
                              lpszMessage[1],
                              lpszMessage[2],
                              lpszMessage[3],
                              lpszMessage[4],
                              lpAlertMessageBuffer[0],
                              lpAlertMessageBuffer[1]);

    DisableInocChangeButtons(lpNoNav);

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
            wResult = PromptInocChangeButtonsProc(lpdrDialog);
            if ((wResult == ACCEPT_DIALOG) || (wResult == ABORT_DIALOG))
                {
                bDone = TRUE;
                }
            DisableInocChangeButtons(lpNoNav);
            }
        }
        while (!bDone);

    DialogClose(lpdrDialog, wResult);
} // End PromptInocChange()


//************************************************************************
// PromptInocChangeButtonsProc()
//
// This routine handles the button presses in the prompt for response
// to file whose inoculation information has changed dialog.
//
// Parameters:
//      DIALOG_RECORD   *lpdrDialog     The current dialog.
//
// Returns:
//      EVENT_AVAIL                     Continue with the dialog.
//      ABORT_DIALOG                    Quit the dialog.
//      ACCEPT_DIALOG                   Exit the dialog.
//************************************************************************

WORD LOCAL PASCAL PromptInocChangeButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    extern  LPNONAV     glpNoNav;
    extern  BOOL        gbContinueScan;

    auto    BYTE        byValue;
    auto    WORD        wResult = EVENT_AVAIL;

    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)                    // Current button selected
        {
        case INOC_CHANGE_REPAIR_BUTTON:
            if (ScanRepairFile(glpNoNav, &glpNoNav->Infected))
                {
                wResult = ACCEPT_DIALOG;
                }
            ScanActionMessage(&glpNoNav->Infected);
            break;

        case INOC_CHANGE_DELETE_BUTTON:
            if (DeleteFile(glpNoNav, &glpNoNav->Infected))
                {
                wResult = ACCEPT_DIALOG;
                }
            ScanActionMessage(&glpNoNav->Infected);
            break;

        case INOC_CHANGE_INOCULATE_BUTTON:
            SmartInocClose();
                                        // Open the database for Read/Write
            glpNoNav->uInocOpenOK
                 = SmartInocOpen(glpNoNav->Infected.szFullPath, TRUE);

            if ( glpNoNav->uInocOpenOK != SMARTOPEN_ERROR )
                {
                if (InocFile(glpNoNav->Infected.szFullPath))
                    {
                    wResult = ACCEPT_DIALOG;
                    }
                SmartInocClose();
                }
            glpNoNav->Infected.wStatus = UpdateFileStatus(
                                                glpNoNav->Infected.wStatus,
                                                PROCESS_INOCULATE,
                                                wResult == ACCEPT_DIALOG);
            ScanActionMessage(&glpNoNav->Infected);
            glpNoNav->uInocOpenOK
                 = SmartInocOpen(glpNoNav->Infected.szFullPath, FALSE);
            break;

        case INOC_CHANGE_EXCLUDE_BUTTON:
#ifdef USE_PASSWORD    //&?
            if (AskForPassword(PASSWORD_MODIEXCLUSIONS))
#endif
                {
                if (ScanExcludeInfectedFile(glpNoNav, &glpNoNav->Infected))
                    {
                    wResult = ACCEPT_DIALOG;
                    }
                ScanActionMessage(&glpNoNav->Infected);
                }
            break;

        case INOC_CHANGE_CONTINUE_BUTTON:
            wResult = ACCEPT_DIALOG;
            break;

        case INOC_CHANGE_STOP_BUTTON:
            gbContinueScan = FALSE;
            wResult = ABORT_DIALOG;
            break;
        } // End switch(lpButtons->...
    return (wResult);
} // End PromptNotInocButtonsProc()


//************************************************************************
// DisableNotInocButtons()
//
// This routine disables the buttons in the file not inoculated dialog
// depending on the options settings.
//
// Parameters:
//      LPNONAV     lpNoNav             Current scan and options information.
//
// Returns:
//      Nothing
//************************************************************************
// 5/5/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DisableNotInocButtons (LPNONAV lpNoNav)
{
    extern  FILESTATUSSTRUCT    FileStatus[];
    extern  ButtonsRec          buNotInocFound;

    auto    WORD                wStatus;
    auto    BYTE                byInoculate;
    auto    BYTE                byExclude;
    auto    BYTE                byDelete;

    wStatus = lpNoNav->Infected.wStatus;

    byInoculate = FileStatus[wStatus].byCanInoc    && lpNoNav->Inoc.bDispInoc;
    byExclude   = FileStatus[wStatus].byCanExclude && lpNoNav->Inoc.bDispExcl;
    byDelete    = FileStatus[wStatus].byCanDelete  && lpNoNav->Inoc.bDispDelete;

    ButtonEnable(&buNotInocFound, INOC_INOCULATE_BUTTON, byInoculate);
    ButtonEnable(&buNotInocFound, INOC_EXCLUDE_BUTTON, byExclude);
    ButtonEnable(&buNotInocFound, INOC_DELETE_BUTTON, byDelete);

    ButtonEnable(&buNotInocFound,
                 INOC_STOP_BUTTON,
                 lpNoNav->Opt.bAllowScanStop);

    if (!byInoculate    &&
        !byExclude      &&
        !byDelete       &&
	!lpNoNav->Opt.bAllowScanStop)
	{
    	ButtonEnable(&buNotInocFound,
		    INOC_CONTINUE_BUTTON,
                    TRUE);
	}
    else
        {
        ButtonEnable(&buNotInocFound,
		    INOC_CONTINUE_BUTTON,
                    lpNoNav->Inoc.bDispCont);
        }
} // End DisableNotInocButtons()


//************************************************************************
// DisableInocChangeButtons()
//
// This routine disables the buttons in the inoculated file changed dialog
// depending on the options settings.
//
// Parameters:
//      LPNONAV     lpNoNav             Current scan and options information.
//
// Returns:
//      Nothing
//************************************************************************
// 5/5/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DisableInocChangeButtons (LPNONAV lpNoNav)
{
    extern  FILESTATUSSTRUCT    FileStatus[];
    extern  ButtonsRec          buInocChangeFound;

    auto    WORD                wStatus;
    auto    BYTE                byRepair;
    auto    BYTE                byDelete;
    auto    BYTE                byInoculate;
    auto    BYTE                byExclude;

    wStatus = lpNoNav->Infected.wStatus;

    byRepair    = FileStatus[wStatus].byCanRepair  && lpNoNav->Inoc.bDispRepair;
    byDelete    = FileStatus[wStatus].byCanDelete  && lpNoNav->Inoc.bDispDelete;
    byInoculate = FileStatus[wStatus].byCanInoc    && lpNoNav->Inoc.bDispInoc;
    byExclude   = FileStatus[wStatus].byCanExclude && lpNoNav->Inoc.bDispExcl;


    ButtonEnable(&buInocChangeFound, INOC_CHANGE_REPAIR_BUTTON,    byRepair);
    ButtonEnable(&buInocChangeFound, INOC_CHANGE_DELETE_BUTTON,    byDelete);
    ButtonEnable(&buInocChangeFound, INOC_CHANGE_INOCULATE_BUTTON, byInoculate);
    ButtonEnable(&buInocChangeFound, INOC_CHANGE_EXCLUDE_BUTTON,   byExclude);

    ButtonEnable(&buInocChangeFound,
                 INOC_CHANGE_STOP_BUTTON,
                 lpNoNav->Opt.bAllowScanStop);

    if (!byRepair       &&
        !byDelete       &&
        !byInoculate    &&
        !byExclude      &&
        !lpNoNav->Opt.bAllowScanStop)
	{
    	ButtonEnable(&buInocChangeFound,
		    INOC_CHANGE_CONTINUE_BUTTON,
		    TRUE);
	}
    else
        {
        ButtonEnable(&buInocChangeFound,
		    INOC_CHANGE_CONTINUE_BUTTON,
                    lpNoNav->Inoc.bDispCont);
        }

} // End DisableNotInocButtons()

