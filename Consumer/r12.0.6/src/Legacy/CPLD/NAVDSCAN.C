// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdscan.c_v   1.0   06 Feb 1997 20:50:20   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV scanner options
//      configuration.
//
// Contains:
//      CPDialogProcScanner()
//
// See Also:
//************************************************************************
// $Log:   S:/cpld/VCS/navdscan.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:20   RFULLER
// Initial revision
// 
//    Rev 1.1   07 Aug 1996 16:48:58   JBRENNA
// Port in changes from DOSNAV environment
// 
// 
//    Rev 1.32   12 Jul 1996 11:26:02   MZAREMB
// Added check to see if this is "Navboot" emulation, and disable certain 
// controls and functions that are not applicable.
// 
//    Rev 1.31   13 Jun 1996 12:02:56   JALLEE
// #if USE_PASSWORD -> #ifdef USE_PASSWORD.
// 
//    Rev 1.30   28 Dec 1994 11:43:56   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 14:00:48   DALLEE
    // Removed controls no longer in NAVBOOT.EXE
    // Removed password protection for options.
    //
    //    Rev 1.1   06 Dec 1994 20:48:28   DALLEE
    // CVT1 script.
    //
//    Rev 1.29   28 Dec 1994 11:34:28   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:56   DALLEE
    // From Nirvana
    //
    //    Rev 1.27   12 Oct 1993 16:46:56   BARRY
    // New CP_LEAVING handling to fix Alt-F4 bug
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"

#include "navutil.h"
#include "navdprot.h"

#include "options.h"

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define SCANNER_OK_BUTTON        (0)
#define SCANNER_CANCEL_BUTTON    (1)
#define SCANNER_ADVANCED_BUTTON  (2)
#define SCANNER_HELP_BUTTON      (3)

#define SCANNER_RB_ALL_FILES     (0)
#define SCANNER_RB_PROGRAM_FILES (1)

#define SCANNER_ADVANCED_OK_BUTTON      (0)
#define SCANNER_ADVANCED_CANCEL_BUTTON  (1)
#define SCANNER_ADVANCED_HELP_BUTTON    (2)


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL DisableScannerControls    (DIALOG_RECORD *lpdrDialog);
UINT ScannerCP_ENTERING        (VOID);
VOID LOCAL PASCAL ScannerAccept             (VOID);
VOID LOCAL PASCAL AdvancedDialog            (VOID);
VOID LOCAL PASCAL AdvancedInit              (VOID);
VOID LOCAL PASCAL AdvancedAccept            (VOID);
int  STATIC       AdvancedButtonsProc       (DIALOG_RECORD *lpdrDialog);


//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern  LPNAVOPTIONS    lpNavOptions;
extern  BOOL    bNavboot;


//************************************************************************
// CPDialogProcScanner()
//
// This is the callback routine for the Options Control Panel Scanner Dialog.
//
// Parameters:
//  LPCPL         lpCPLDialog,          // Pointer to related CPL structure
//  UINT          uMessage,             // Message
//  UINT          uParameter,           // Additional message parameter
//  DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
//
// Returns:
//  EVENT_AVAIL                         Further processing of message
//  ABORT_DIALOG                        Cancel button hit
//  ACCEPT_DIALOG                       OK button hit
//************************************************************************
// 2/1/93 DALLEE Function created.
//************************************************************************

UINT CPDialogProcScanner (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern  ButtonsRec      buProgramFiles;
    extern  ButtonsRec 	    buScannerButtons;

    extern  BYTE            byAcceptCPDialog;
    extern  CheckBoxRec     cbScannerMemory;
    extern  CheckBoxRec     cbScannerMBR;
    extern  CheckBoxRec     cbScannerBootRecords;
    extern  CheckBoxRec     cbScannerCompressed;

    extern  CheckBoxRec     cbScannerRepair;
    extern  CheckBoxRec     cbScannerContinue;
    extern  CheckBoxRec     cbScannerDelete;
    extern  CheckBoxRec     cbScannerExclude;

    extern  RadioButtonsRec rbScannerWhatFiles;
    extern  ComboBoxRec     cxScannerVirusFound;

    auto    WORD            wResult;
#ifdef USE_PASSWORD    //&?
    static  BOOL	    bNormalProcess;
#endif


    switch (uMessage)
        {
        case CP_PRE_EVENT:
	                                // Event received, not yet processed.
                                        // Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (uParameter != ESC);
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:
#ifdef USE_PASSWORD    //&?
            if (bNormalProcess)
#endif
                {
                DisableScannerControls(lpDialog);
                }

            return (CONTINUE_DIALOG);

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case
            if ((ButtonsRec *) lpDialog->item->item == &buProgramFiles)
                {
                ProgramExtDialog(lpNavOptions->general.szExt,
                                 lpNavOptions->general.szDef);
                return (EVENT_AVAIL);
                }

            switch (buScannerButtons.value)
                {
                case SCANNER_OK_BUTTON:
                                        // byAcceptCPDialog should == TRUE,
                                        // so data will be saved on CP_LEAVING.
                    return (ACCEPT_DIALOG);

                case SCANNER_CANCEL_BUTTON:
                                        // Set save flag FALSE
                    byAcceptCPDialog = FALSE;
                    return (ABORT_DIALOG);

                case SCANNER_ADVANCED_BUTTON:
                                        // Call advanced options dialog
                    AdvancedDialog();
                    break;

                case SCANNER_HELP_BUTTON:
                                        // Call scanner settings help here
		    HyperHelpDisplay();
                    break;
                }                       

            HyperHelpTopicSet(HELP_DLG_CPL_SCANNER);
            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED

        case CP_PRE_ENTERING:           // Haven't entered dialog yet
            break;

        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptScan info from database
                                        //  and initialize dialog data
	    HyperHelpTopicSet(HELP_DLG_CPL_SCANNER);

#ifndef USE_PASSWORD
            wResult = ScannerCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog, 
                                      PASSWORD_VIEWSCANOPT,
                                      ScannerCP_ENTERING,
                                      &bNormalProcess);
            if (bNormalProcess)
#endif
                {
                DisableScannerControls(lpDialog);
                }
            return (wResult);

        case CP_LEAVING:                // Dialog box is losing focus
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.
            if ( uParameter != ACCEPT_DIALOG )
                byAcceptCPDialog = FALSE;

            DialogSetFocus(lpDialog, &buScannerButtons);
                                        // Free temp SCANOPT struct
                                        // If the dialog was not canceled,
                                        // save options if necessary.
            if ( byAcceptCPDialog
#ifdef USE_PASSWORD    //&?
                 && !NeedPassword(PASSWORD_VIEWSCANOPT)
#endif
                 )
                {
                ScannerAccept();
                }

            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);
        }                               // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcScanner()


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

//************************************************************************
// ScannerCP_ENTERING()
//
// This routine handles the CP_ENTERING message for the Options Scanner
// Control Panel entry.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL
//************************************************************************

UINT ScannerCP_ENTERING (VOID)
{
    extern CheckBoxRec      cbScannerMemory;
    extern CheckBoxRec      cbScannerMBR;
    extern CheckBoxRec      cbScannerBootRecords;
    extern CheckBoxRec      cbScannerCompressed;
    extern CheckBoxRec      cbScannerRepair;
    extern CheckBoxRec      cbScannerContinue;
    extern CheckBoxRec      cbScannerDelete;
    extern CheckBoxRec      cbScannerExclude;
    extern ComboBoxRec      cxScannerVirusFound;
    extern RadioButtonsRec  rbScannerWhatFiles;


    UpdateCheckBox(&cbScannerMemory,      lpNavOptions->scanner.bMemory);
    UpdateCheckBox(&cbScannerMBR,         lpNavOptions->scanner.bMasterBootRec);
    UpdateCheckBox(&cbScannerBootRecords, lpNavOptions->scanner.bBootRecs);
#ifdef USE_ZIP //&?
    if ( !bNavboot ) 
        UpdateCheckBox(&cbScannerCompressed,  lpNavOptions->general.bScanZipFiles);
#endif
    UpdateCheckBox(&cbScannerRepair,      lpNavOptions->scanner.bDispRepair);
    UpdateCheckBox(&cbScannerContinue,    lpNavOptions->scanner.bDispCont);
    UpdateCheckBox(&cbScannerDelete,      lpNavOptions->scanner.bDispDelete);
    UpdateCheckBox(&cbScannerExclude,     lpNavOptions->scanner.bDispExcl);

    RadioButtonsSetValue(&rbScannerWhatFiles, (BYTE)(lpNavOptions->scanner.bScanAll==TRUE ?
                         SCANNER_RB_ALL_FILES : SCANNER_RB_PROGRAM_FILES));

                                        // -1 for TSR
    ComboBoxSetSelection(&cxScannerVirusFound, lpNavOptions->scanner.uAction-1);

    return (EVENT_AVAIL);
} // End ScannerCP_ENTERING()


//************************************************************************
// ScannerAccept()
//
// This routine saves the current settings.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL ScannerAccept (VOID)
{
    extern CheckBoxRec      cbScannerMemory;
    extern CheckBoxRec      cbScannerMBR;
    extern CheckBoxRec      cbScannerBootRecords;
    extern CheckBoxRec      cbScannerCompressed;
    extern CheckBoxRec      cbScannerRepair;
    extern CheckBoxRec      cbScannerContinue;
    extern CheckBoxRec      cbScannerDelete;
    extern CheckBoxRec      cbScannerExclude;
    extern ComboBoxRec      cxScannerVirusFound;
    extern RadioButtonsRec  rbScannerWhatFiles;


    lpNavOptions->scanner.bMemory        = cbScannerMemory.value;
    lpNavOptions->scanner.bMasterBootRec = cbScannerMBR.value;
    lpNavOptions->scanner.bBootRecs      = cbScannerBootRecords.value;
#ifdef USE_ZIP //&?
    lpNavOptions->general.bScanZipFiles  = cbScannerCompressed.value;
#endif
    lpNavOptions->scanner.bDispRepair    = cbScannerRepair.value;
    lpNavOptions->scanner.bDispCont      = cbScannerContinue.value;
    lpNavOptions->scanner.bDispDelete    = cbScannerDelete.value;
    lpNavOptions->scanner.bDispExcl      = cbScannerExclude.value;

    lpNavOptions->scanner.bScanAll       = (rbScannerWhatFiles.value ==
                                 SCANNER_RB_ALL_FILES);

                                        // +1 for TSR
    lpNavOptions->scanner.uAction        = (BYTE) cxScannerVirusFound.value + 1;

} // End ScannerAccept()


//************************************************************************
// DisableScannerControls()
//
// This routine disables the Program Files button, and How to Respond
// checkboxes based on the settings of the other dialog controls.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 7/12/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DisableScannerControls (DIALOG_RECORD *lpdrDialog)
{
    extern  ButtonsRec      buProgramFiles;
    extern  RadioButtonsRec rbScannerWhatFiles;
    extern  CheckBoxRec     cbScannerRepair;
    extern  CheckBoxRec     cbScannerContinue;
    extern  CheckBoxRec     cbScannerDelete;
    extern  CheckBoxRec     cbScannerExclude;
    extern  ComboBoxRec     cxScannerVirusFound;

    auto    BYTE            byEnableCheckBoxes;

    ButtonEnable(&buProgramFiles, 0,
                 (BYTE)(rbScannerWhatFiles.value == SCANNER_RB_PROGRAM_FILES));

                                        // +1 for TSR, don't ask...
    byEnableCheckBoxes = (cxScannerVirusFound.value + 1 == SCAN_PROMPT);

    CheckBoxEnable(lpdrDialog, &cbScannerRepair,    byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbScannerContinue,  byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbScannerDelete,    byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbScannerExclude,   byEnableCheckBoxes);
} // End DisableScannerControls()


//------------------------------------------------------------------------
// Scanner Advanced Dialog Stuff
//------------------------------------------------------------------------

//************************************************************************
// AdvancedDialog()
//
// This routine displays the scanner advanced settings dialog and updates
// the settings information if necessary.
//
// Parameters:
//      none                            Will eventually take a pointer to
//                                       the scanner settings data structure
//
// Returns:
//      nothing
//************************************************************************
// 2/18/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AdvancedDialog(VOID)
{
                                        // These externs are declared in
                                        // SCANSTR.C
    extern CheckBoxRec  cbScannerAdvancedAllowNetwork;
    extern CheckBoxRec  cbScannerAdvancedAllowStop;
    extern CheckBoxRec  cbScannerAdvancedNotify;
    extern CheckBoxRec  cbScannerAdvancedAllFloppy;
    extern CheckBoxRec  cbScannerAdvancedAllHard;
    extern CheckBoxRec  cbScannerAdvancedAllNetwork;
    extern DIALOG       dlScannerAdvanced;

    DIALOG_RECORD       *lpdrDialog;
    WORD                wResult;


    AdvancedInit();
    HyperHelpTopicSet(HELP_DLG_CPL_SCANNER_ADVANCED);

    lpdrDialog = DialogOpen2( &dlScannerAdvanced,
                              &cbScannerAdvancedAllowNetwork,
                              &cbScannerAdvancedAllowStop,
                              &cbScannerAdvancedNotify,
                              &cbScannerAdvancedAllFloppy,
                              &cbScannerAdvancedAllHard,
                              &cbScannerAdvancedAllNetwork);

    if (lpdrDialog != NULL)
        {
        do
            {
            wResult = StdEventLoop(NULL,    // Menu
                                   lpdrDialog,
                                   AdvancedButtonsProc,
                                   NULL,    // Menu function
                                   NULL,    // Pre function
                                   NULL);   // Post function
            if (wResult == ACCEPT_DIALOG)
                {
                AdvancedAccept();
                break;
                }
            }
            while (wResult != ABORT_DIALOG);

        DialogClose(lpdrDialog, wResult);
        }
} // End AdvancedDialog()


//************************************************************************
// AdvancedButtonsProc()
//
// This routine handles the button presses in the Scanner Advanced dialog.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//
// Returns:
//      ACCEPT_DIALOG                   Ok hit
//      ABORT_DIALOG                    Cancel hit
//      EVENT_AVAIL                     Help, or no button.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

int STATIC AdvancedButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    BYTE                byValue;


    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)           // Current button selected
        {
        case SCANNER_ADVANCED_OK_BUTTON:
            return (ACCEPT_DIALOG);

        case SCANNER_ADVANCED_CANCEL_BUTTON:
            return (ABORT_DIALOG);

        case SCANNER_ADVANCED_HELP_BUTTON:
                                        // Call monitor startup help here
	    HyperHelpDisplay();
            return (EVENT_AVAIL);

        default:
            return (EVENT_AVAIL);
        }
} // End AdvancedButtonsProc()


//************************************************************************
// AdvancedInit()
//
// This routine sets up the scanner advanced options dialog controls
// based on the values in the temporary SCANOPT structure lpOptScan.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AdvancedInit (VOID)
{
    extern CheckBoxRec cbScannerAdvancedAllowNetwork;
    extern CheckBoxRec cbScannerAdvancedAllowStop;
    extern CheckBoxRec cbScannerAdvancedNotify;
    extern CheckBoxRec cbScannerAdvancedAllFloppy;
    extern CheckBoxRec cbScannerAdvancedAllHard;
    extern CheckBoxRec cbScannerAdvancedAllNetwork;


    cbScannerAdvancedAllowNetwork.value = lpNavOptions->scanner.bAllowNetScan;
    cbScannerAdvancedAllowStop.value    = lpNavOptions->scanner.bAllowScanStop;
    cbScannerAdvancedNotify.value       = lpNavOptions->scanner.bPrestoNotify;
    cbScannerAdvancedAllFloppy.value    = lpNavOptions->scanner.bPresFlop;
    cbScannerAdvancedAllHard.value      = lpNavOptions->scanner.bPresHard;
    cbScannerAdvancedAllNetwork.value   = lpNavOptions->scanner.bPresNet;
} // End AdvancedInit()


//************************************************************************
// AdvancedAccept()
//
// This routine is called if the advanced dialog has been accepted.
// It sets the values of the temporary SCANOPT structure *lpOptScan to
// match the dialog controls.
//
// NOTE: The advanced options changes are not saved to the .INI file
//       unless and until the main scanner options dialog is accepted.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AdvancedAccept (VOID)
{
    extern CheckBoxRec cbScannerAdvancedAllowNetwork;
    extern CheckBoxRec cbScannerAdvancedAllowStop;
    extern CheckBoxRec cbScannerAdvancedNotify;
    extern CheckBoxRec cbScannerAdvancedAllFloppy;
    extern CheckBoxRec cbScannerAdvancedAllHard;
    extern CheckBoxRec cbScannerAdvancedAllNetwork;


    lpNavOptions->scanner.bAllowNetScan  = cbScannerAdvancedAllowNetwork.value;
    lpNavOptions->scanner.bAllowScanStop = cbScannerAdvancedAllowStop.value;
    lpNavOptions->scanner.bPrestoNotify  = cbScannerAdvancedNotify.value;
    lpNavOptions->scanner.bPresFlop      = cbScannerAdvancedAllFloppy.value;
    lpNavOptions->scanner.bPresHard      = cbScannerAdvancedAllHard.value;
    lpNavOptions->scanner.bPresNet       = cbScannerAdvancedAllNetwork.value;

} // End AdvancedAccept()

