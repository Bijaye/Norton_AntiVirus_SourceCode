// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdreal.c_v   1.0   06 Feb 1997 20:50:16   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV System Monitor options
//      configuration
//
// Contains:
//      CPDialogProcMonitor()
//
// See Also:
//      REALSTR.CPP for strings and control structures.
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/cpld/VCS/navdreal.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:16   RFULLER
// Initial revision
// 
//    Rev 1.1   07 Aug 1996 16:49:08   JBRENNA
// Port in changes from DOSNAV environment
// 
// 
//    Rev 1.40   13 Jun 1996 12:02:54   JALLEE
// #if USE_PASSWORD -> #ifdef USE_PASSWORD.
// 
//    Rev 1.39   28 Dec 1994 11:43:58   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 15:48:44   DALLEE
    // Commented out password protection.
    //
    //    Rev 1.1   06 Dec 1994 20:48:14   DALLEE
    // CVT1 script.
    //
//    Rev 1.38   28 Dec 1994 11:34:30   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:50   DALLEE
    // From Nirvana
    //
    //    Rev 1.36   12 Oct 1993 16:47:00   BARRY
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

#define MONITOR_OK_BUTTON               (0)
#define MONITOR_CANCEL_BUTTON           (1)
#define MONITOR_ADVANCED_BUTTON         (2)
#define MONITOR_STARTUP_BUTTON          (3)
#define MONITOR_SENSOR_BUTTON           (4)
#define MONITOR_HELP_BUTTON             (5)

#define MONITOR_STARTUP_OK_BUTTON       (0)
#define MONITOR_STARTUP_CANCEL_BUTTON   (1)
#define MONITOR_STARTUP_HELP_BUTTON     (2)

#define MONITOR_SENSOR_OK_BUTTON        (0)
#define MONITOR_SENSOR_CANCEL_BUTTON    (1)
#define MONITOR_SENSOR_HELP_BUTTON      (2)

#define MONITOR_ADVANCED_OK_BUTTON      (0)
#define MONITOR_ADVANCED_CANCEL_BUTTON  (1)
#define MONITOR_ADVANCED_HELP_BUTTON    (2)

#define MONITOR_RB_ALL_FILES            (0)
#define MONITOR_RB_PROGRAM_FILES        (1)

#define MONITOR_STARTUP_RB_DEVICE       (0)
#define MONITOR_STARTUP_RB_TSR          (1)


//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern  LPNAVOPTIONS    lpNavOptions;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

UINT MonitorCP_ENTERING (VOID);
VOID LOCAL PASCAL MonitorAccept (VOID);

VOID LOCAL PASCAL DisableMonitorControls (DIALOG_RECORD *lpdrDialog);

VOID LOCAL PASCAL AdvancedDialog    (VOID);
VOID LOCAL PASCAL AdvancedInit      (VOID);
VOID LOCAL PASCAL AdvancedAccept    (VOID);
VOID STATIC       AdvancedPreProc   (DIALOG_RECORD *lpdrDialog, int *lpiEvent);
int  STATIC       AdvancedButtonsProc (DIALOG_RECORD *lpdrDialog);

VOID LOCAL PASCAL StartupDialog     (VOID);
VOID LOCAL PASCAL StartupInit       (VOID);
VOID LOCAL PASCAL StartupAccept     (VOID);
int  STATIC       StartupButtonsProc(DIALOG_RECORD *lpdrDialog);

VOID LOCAL PASCAL SensorDialog      (VOID);
VOID LOCAL PASCAL SensorInit        (VOID);
VOID LOCAL PASCAL SensorAccept      (VOID);
VOID STATIC       SensorPreProc     (DIALOG_RECORD *lpdrDialog, int *lpiEvent);
int  STATIC       SensorButtonsProc (DIALOG_RECORD *lpdrDialog);


//************************************************************************
// CPDialogProcMonitor()
//
// This is the callback routine for the Options CP Monitor Dialog.
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

UINT CPDialogProcMonitor (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern 	RadioButtonsRec rbMonitorWhatToScan;
    extern 	ButtonsRec      buProgramFiles;
    extern 	BYTE            byAcceptCPDialog;

    extern	CheckBoxRec     cbMonitorScanWhenRun;
    extern	CheckBoxRec     cbMonitorScanWhenOpened;
    extern	CheckBoxRec     cbMonitorScanWhenCreated;
    extern	CheckBoxRec     cbMonitorRepair;
    extern	CheckBoxRec     cbMonitorContinue;
    extern	CheckBoxRec     cbMonitorExclude;
    extern	CheckBoxRec     cbMonitorDelete;
    extern	CheckBoxRec     cbMonitorStop;

    extern	ComboBoxRec     cxMonitorVirusFound;

    extern      ButtonsRec      buMonitorButtons;

    auto   	WORD            wResult;
#ifdef USE_PASSWORD    //&?
    static      BOOL            bNormalProcess;
#endif



    switch (uMessage)
        {
        case CP_PRE_EVENT:              // Event received, not yet processed

                                        // Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (uParameter != ESC);
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:             // Event received and processed
                                        // If we're scanning program files only
                                        // then enable the Program Extensions
                                        // dialog button.
#ifdef USE_PASSWORD
            if (bNormalProcess)
#endif
                {
                DisableMonitorControls(lpDialog);
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
                }
            else
                {                       // Current button selected
                switch (buMonitorButtons.value)
                    {
                    case MONITOR_OK_BUTTON:
                                        // Options saved if necessary on
                                        // CP_LEAVING
                        return (ACCEPT_DIALOG);

                    case MONITOR_CANCEL_BUTTON:
                                        // Set save flag FALSE
                        byAcceptCPDialog = FALSE;
                        return (ABORT_DIALOG);

                    case MONITOR_ADVANCED_BUTTON:
                                        // Call advanced options dialog
                        AdvancedDialog();
                        break;

                    case MONITOR_STARTUP_BUTTON:
                                        // Call startup options dialog
                        StartupDialog();
                        break;

                    case MONITOR_SENSOR_BUTTON:
                        SensorDialog();
                        break;

                    case MONITOR_HELP_BUTTON:
			HyperHelpDisplay();
                        break;
                    }
                }
            HyperHelpTopicSet(HELP_DLG_CPL_SYSMON);

            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED

        case CP_PRE_ENTERING:           // Haven't entered dialog yet
            break;

        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptMonitor info from database
                                        //  and initialize dialog data
            HyperHelpTopicSet(HELP_DLG_CPL_SYSMON);
#ifndef USE_PASSWORD //&?
            wResult = MonitorCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog,
                                      PASSWORD_VIEWTSROPT,
                                      MonitorCP_ENTERING,
                                      &bNormalProcess);
	    if (bNormalProcess)
#endif
                {
                DisableMonitorControls(lpDialog);
                }
            return(wResult);

        case CP_LEAVING:                // Dialog box is losing focus.
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.
            if (uParameter != ACCEPT_DIALOG)
                {
                byAcceptCPDialog = FALSE;
                }

            DialogSetFocus(lpDialog, &buMonitorButtons);
                                        // If the dialog was not canceled,
                                        // save options if necessary.
            if ( byAcceptCPDialog
#ifdef USE_PASSWORD    //&?
                 && !NeedPassword (PASSWORD_VIEWTSROPT)
#endif
                 )
                {
                MonitorAccept();
                }

            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);
        } // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcMonitor()


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

//************************************************************************
// MonitorCP_ENTERING()
//
// This routine handles the CP_ENTERING message for the Options System
// Monitor Control Panel entry.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Continue w/ dialog
//      ABORT_DIALOG                    Cancel dialog - couldn't allocate
//                                       memory for the temp TSROPT struct
//************************************************************************
// 3/11/93 DALLEE Function created.
//************************************************************************

UINT MonitorCP_ENTERING (VOID)
{
    extern CheckBoxRec     cbMonitorScanWhenRun;
    extern CheckBoxRec     cbMonitorScanWhenOpened;
    extern CheckBoxRec     cbMonitorScanWhenCreated;
    extern CheckBoxRec     cbMonitorRepair;
    extern CheckBoxRec     cbMonitorContinue;
    extern CheckBoxRec     cbMonitorExclude;
    extern CheckBoxRec     cbMonitorDelete;
    extern CheckBoxRec     cbMonitorStop;
    extern ComboBoxRec     cxMonitorVirusFound;
    extern RadioButtonsRec rbMonitorWhatToScan;


    UpdateCheckBox(&cbMonitorScanWhenRun,    lpNavOptions->tsr.bScanRun);
    UpdateCheckBox(&cbMonitorScanWhenOpened, lpNavOptions->tsr.bScanOpen);
    UpdateCheckBox(&cbMonitorScanWhenCreated,lpNavOptions->tsr.bScanCreate);
    UpdateCheckBox(&cbMonitorRepair,         lpNavOptions->tsr.bDispRepair);
    UpdateCheckBox(&cbMonitorContinue,       lpNavOptions->tsr.bDispContinue);
    UpdateCheckBox(&cbMonitorExclude,        lpNavOptions->tsr.bDispExclude);
    UpdateCheckBox(&cbMonitorDelete,         lpNavOptions->tsr.bDispDelete);
    UpdateCheckBox(&cbMonitorStop,           lpNavOptions->tsr.bDispStop);

    RadioButtonsSetValue(&rbMonitorWhatToScan, (BYTE)(lpNavOptions->tsr.bScanAll==TRUE ?
                         MONITOR_RB_ALL_FILES : MONITOR_RB_PROGRAM_FILES));

                                        // -1 for TSR
    ComboBoxSetSelection(&cxMonitorVirusFound, lpNavOptions->tsr.uActionKnown-1);

    return (EVENT_AVAIL);
} // End MonitorCP_ENTERING()


//************************************************************************
// MonitorAccept()
//
// This routine updates the temporary TSROPT struct *lpOptTSR from the
// dialog controls, and if changes were made, saves them to the .INI file
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/10/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL MonitorAccept (VOID)
{
    extern CheckBoxRec     cbMonitorScanWhenRun;
    extern CheckBoxRec     cbMonitorScanWhenOpened;
    extern CheckBoxRec     cbMonitorScanWhenCreated;
    extern CheckBoxRec     cbMonitorRepair;
    extern CheckBoxRec     cbMonitorContinue;
    extern CheckBoxRec     cbMonitorExclude;
    extern CheckBoxRec     cbMonitorDelete;
    extern CheckBoxRec     cbMonitorStop;
    extern ComboBoxRec     cxMonitorVirusFound;
    extern RadioButtonsRec rbMonitorWhatToScan;


    lpNavOptions->tsr.bScanRun      = cbMonitorScanWhenRun.value;
    lpNavOptions->tsr.bScanOpen     = cbMonitorScanWhenOpened.value;
    lpNavOptions->tsr.bScanCreate   = cbMonitorScanWhenCreated.value;
    lpNavOptions->tsr.bDispRepair   = cbMonitorRepair.value;
    lpNavOptions->tsr.bDispContinue = cbMonitorContinue.value;
    lpNavOptions->tsr.bDispExclude  = cbMonitorExclude.value;
    lpNavOptions->tsr.bDispDelete   = cbMonitorDelete.value;
    lpNavOptions->tsr.bDispStop     = cbMonitorStop.value;

    lpNavOptions->tsr.bScanAll      = (rbMonitorWhatToScan.value ==
                               MONITOR_RB_ALL_FILES);

                                        // +1 for TSR
    lpNavOptions->tsr.uActionKnown  = (BYTE) cxMonitorVirusFound.value + 1;

} // End MonitorAccept()


//************************************************************************
// DisableMonitorControls()
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

VOID LOCAL PASCAL DisableMonitorControls (DIALOG_RECORD *lpdrDialog)
{
    extern  ButtonsRec      buProgramFiles;
    extern  RadioButtonsRec rbMonitorWhatToScan;

    extern  CheckBoxRec     cbMonitorRepair;
    extern  CheckBoxRec     cbMonitorContinue;
    extern  CheckBoxRec     cbMonitorDelete;
    extern  CheckBoxRec     cbMonitorExclude;
    extern  CheckBoxRec     cbMonitorStop;
    extern  ComboBoxRec     cxMonitorVirusFound;

    auto    BYTE            byEnableCheckBoxes;

    ButtonEnable(&buProgramFiles, 0,
                 (BYTE)(rbMonitorWhatToScan.value == MONITOR_RB_PROGRAM_FILES));

                                        // +1 for TSR, don't ask...
    byEnableCheckBoxes = (cxMonitorVirusFound.value + 1 == TSR_PROMPT);

    CheckBoxEnable(lpdrDialog, &cbMonitorRepair,    byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbMonitorContinue,  byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbMonitorDelete,    byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbMonitorExclude,   byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbMonitorStop,      byEnableCheckBoxes);

} // End DisableMonitorControls()


//------------------------------------------------------------------------
// System Monitor Startup Dialog
//------------------------------------------------------------------------

//************************************************************************
// StartupDialog()
//
// This routine displays the system monitor startup settings dialog
// and updates the settings information if necessary.
//
// Parameters:
//      none
//
// Returns:
//      nothing
//************************************************************************
// 2/9/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL StartupDialog(VOID)
{
                                        // These externs are declared in
                                        //  REALSTR.CPP
    extern CheckBoxRec      cbMonitorStartupMemory;
    extern CheckBoxRec      cbMonitorStartupMBR;
    extern CheckBoxRec      cbMonitorStartupBootRecords;
    extern CheckBoxRec      cbMonitorStartupHMA;
    extern CheckBoxRec      cbMonitorStartupUMBs;
    extern CheckBoxRec      cbMonitorStartupEMS;
    extern RadioButtonsRec  rbMonitorStartupBypassKeys;
    extern CheckBoxRec      cbMonitorStartupCanBeUnloaded;
    extern CheckBoxRec      cbMonitorStartupHideIcon;
    extern DIALOG           dlMonitorStartup;

    auto   DIALOG_RECORD    *lpdrDialog;
    auto   WORD             wResult;


    StartupInit();
    HyperHelpTopicSet(HELP_DLG_STARTUP);

    lpdrDialog = DialogOpen2( &dlMonitorStartup,

                              &cbMonitorStartupMemory,
                              &cbMonitorStartupMBR,
                              &cbMonitorStartupBootRecords,

                              &cbMonitorStartupHMA,
                              &cbMonitorStartupUMBs,
                              &cbMonitorStartupEMS,

                              &rbMonitorStartupBypassKeys,
                              &cbMonitorStartupCanBeUnloaded,
                              &cbMonitorStartupHideIcon);


    if (lpdrDialog != NULL)
        {
        do
            {
            wResult = StdEventLoop(NULL,    // Menu
                                   lpdrDialog,
                                   StartupButtonsProc,
                                   NULL,    // Menu items function
                                   NULL,        // StartupPreProc,
                                   NULL);   // Post event function
            if (wResult == ACCEPT_DIALOG)
                {
                StartupAccept();
                break;
                }
            }
            while (wResult != ABORT_DIALOG);

        DialogClose(lpdrDialog, wResult);
        }
} // End StarupDialog()


//************************************************************************
// StartupButtonsProc()
//
// This routine handles the button presses in the System Monitor startup
// dialog.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//
// Returns:
//      ACCEPT_DIALOG                   Ok button hit
//      ABORT_DIALOG                    Cancel hit
//      EVENT_AVAIL                     Help, or default
//************************************************************************
// 2/27/93 DALLEE Function created.
//************************************************************************

int STATIC StartupButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    BYTE                byValue;


    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)           // Current button selected
        {
        case MONITOR_STARTUP_OK_BUTTON:
            return (ACCEPT_DIALOG);

        case MONITOR_STARTUP_CANCEL_BUTTON:
            return (ABORT_DIALOG);

        case MONITOR_STARTUP_HELP_BUTTON:
                                        // Call monitor startup help here
            HyperHelpDisplay();
            return (EVENT_AVAIL);

        default:
            return (EVENT_AVAIL);
        }
} // End StartupButtonsProc()



//************************************************************************
// StartupInit()
//
// This routine sets up the monitor startup options dialog controls
// based on the values in the temporary TSROPT structure lpOptTSR.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/11/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL StartupInit (VOID)
{
    extern CheckBoxRec     cbMonitorStartupHMA;
    extern CheckBoxRec     cbMonitorStartupUMBs;
    extern CheckBoxRec     cbMonitorStartupEMS;
    extern CheckBoxRec     cbMonitorStartupMemory;
    extern CheckBoxRec     cbMonitorStartupMBR;
    extern CheckBoxRec     cbMonitorStartupBootRecords;
    extern RadioButtonsRec rbMonitorStartupBypassKeys;
    extern CheckBoxRec     cbMonitorStartupCanBeUnloaded;
    extern CheckBoxRec     cbMonitorStartupHideIcon;


    cbMonitorStartupHMA.value           = lpNavOptions->tsr.bUseHMA;
    cbMonitorStartupUMBs.value          = lpNavOptions->tsr.bUseUMB;
    cbMonitorStartupEMS.value           = lpNavOptions->tsr.bUseEMS;
    cbMonitorStartupMemory.value        = lpNavOptions->tsr.bScanMem;
    cbMonitorStartupMBR.value           = lpNavOptions->tsr.bScanMBoot;
    cbMonitorStartupBootRecords.value   = lpNavOptions->tsr.bScanBootRecs;
    rbMonitorStartupBypassKeys.value    = lpNavOptions->tsr.uBypassKey;
    cbMonitorStartupCanBeUnloaded.value = lpNavOptions->tsr.bUnloadable;
    cbMonitorStartupHideIcon.value      = lpNavOptions->tsr.bHideIcon;

} // End StartupInit()


//************************************************************************
// StartupAccept()
//
// This routine is called if the startup dialog has been accepted.
// It sets the values of the temporary TSROPTS structure *lpOptTSR to
// match the dialog controls.
//
// NOTE: The startup options changes are not saved to the .INI file
//       unless and until the main monitor options dialog is accepted.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/11/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL StartupAccept (VOID)
{
    extern CheckBoxRec     cbMonitorStartupHMA;
    extern CheckBoxRec     cbMonitorStartupUMBs;
    extern CheckBoxRec     cbMonitorStartupEMS;
    extern CheckBoxRec     cbMonitorStartupMemory;
    extern CheckBoxRec     cbMonitorStartupMBR;
    extern CheckBoxRec     cbMonitorStartupBootRecords;
    extern RadioButtonsRec rbMonitorStartupBypassKeys;
    extern CheckBoxRec     cbMonitorStartupCanBeUnloaded;
    extern CheckBoxRec     cbMonitorStartupHideIcon;


    lpNavOptions->tsr.bUseHMA       = cbMonitorStartupHMA.value;
    lpNavOptions->tsr.bUseUMB       = cbMonitorStartupUMBs.value;
    lpNavOptions->tsr.bUseEMS       = cbMonitorStartupEMS.value;
    lpNavOptions->tsr.bScanMem      = cbMonitorStartupMemory.value;
    lpNavOptions->tsr.bScanMBoot    = cbMonitorStartupMBR.value;
    lpNavOptions->tsr.bScanBootRecs = cbMonitorStartupBootRecords.value;
    lpNavOptions->tsr.uBypassKey    = rbMonitorStartupBypassKeys.value;
    lpNavOptions->tsr.bUnloadable   = cbMonitorStartupCanBeUnloaded.value;
    lpNavOptions->tsr.bHideIcon     = cbMonitorStartupHideIcon.value;

} // End StartupAccept()


//------------------------------------------------------------------------
// System Monitor Advanced Dialog
//------------------------------------------------------------------------

//************************************************************************
// AdvancedDialog()
//
// This routine displays the system monitor advanced settings dialog
// and updates the settings information if necessary.
//
// Parameters:
//      none                                Will eventually take a pointer
//                                           to the system monitor settings
//                                           data structure.
//
// Returns:
//      nothing
//************************************************************************
// 2/18/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AdvancedDialog(VOID)
{
                                        // These externs are declared in
                                        //  REALSTR.CPP
    extern ComboBoxRec      cxMonitorAdvancedFormatHard;
    extern ComboBoxRec      cxMonitorAdvancedHardBoot;
    extern ComboBoxRec      cxMonitorAdvancedFloppyBoot;
    extern ComboBoxRec      cxMonitorAdvancedWriteEXE;
    extern ComboBoxRec      cxMonitorAdvancedAttributeChange;
    extern CheckBoxRec      cbMonitorAdvancedFloppyAccess;
    extern CheckBoxRec      cbMonitorAdvancedFloppyReboot;
    extern CheckBoxRec      cbMonitorAdvancedFloppyBoth;
    extern DIALOG           dlMonitorAdvanced;

    auto   DIALOG_RECORD    *lpdrDialog;
    auto   WORD             wResult;


    AdvancedInit();
    HyperHelpTopicSet(HELP_DLG_SYSMON_ADVANCED);

    lpdrDialog = DialogOpen2 ( &dlMonitorAdvanced,
                               &cxMonitorAdvancedFormatHard,
                               &cxMonitorAdvancedHardBoot,
                               &cxMonitorAdvancedFloppyBoot,
                               &cxMonitorAdvancedWriteEXE,
                               &cxMonitorAdvancedAttributeChange,
                               &cbMonitorAdvancedFloppyAccess,
                               &cbMonitorAdvancedFloppyReboot,
                               &cbMonitorAdvancedFloppyBoth);

    if (lpdrDialog != NULL)
        {
        do
            {
            wResult = StdEventLoop(NULL,
                                   lpdrDialog,
                                   AdvancedButtonsProc,
                                   NULL,
                                   AdvancedPreProc,
                                   NULL);
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
// This routine handles the button presses in the System Monitor advanced
// dialog.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//
// Returns:
//      ACCEPT_DIALOG                   Ok button hit
//      ABORT_DIALOG                    Cancel hit
//      EVENT_AVAIL                     Help, or default
//************************************************************************
// 2/27/93 DALLEE Function created.
//************************************************************************

int STATIC AdvancedButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    BYTE                byValue;


    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)           // Current button selected
        {
        case MONITOR_ADVANCED_OK_BUTTON:
            return (ACCEPT_DIALOG);

        case MONITOR_ADVANCED_CANCEL_BUTTON:
            return (ABORT_DIALOG);

        case MONITOR_ADVANCED_HELP_BUTTON:
                                        // Call monitor startup help here
	    HyperHelpDisplay();
            return (EVENT_AVAIL);

        default:
            return (EVENT_AVAIL);
        }
} // End AdvancedButtonsProc()


//************************************************************************
// AdvancedPreProc()
//
// This routine disables the Check Both Drives checkbox if the system
// monitor does not check floppies when rebooting.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//      int           *lpiEvent         Current event
//
// Returns:
//      nothing
//************************************************************************
// 2/27/93 DALLEE Function created.
//************************************************************************

VOID STATIC AdvancedPreProc (DIALOG_RECORD *lpdrDialog, int *lpiEvent)
{
    extern CheckBoxRec  cbMonitorAdvancedFloppyReboot;
    extern CheckBoxRec  cbMonitorAdvancedFloppyBoth;

                                        // Enable/Disable only if necessary
                                        // since CheckBoxEnable always redraws
    if (cbMonitorAdvancedFloppyReboot.value == TRUE)
        {
        if (cbMonitorAdvancedFloppyBoth.flags & CB_DISABLED)
            {
            CheckBoxEnable(lpdrDialog, &cbMonitorAdvancedFloppyBoth, TRUE);
            }
        }
    else
        {
        if ( !(cbMonitorAdvancedFloppyBoth.flags & CB_DISABLED) )
            {
            CheckBoxEnable(lpdrDialog, &cbMonitorAdvancedFloppyBoth, FALSE);
            }
        }
} // End AdvancedPreProc()


//************************************************************************
// AdvancedInit()
//
// This routine sets up the monitor advanced options dialog controls
// based on the values in the temporary TSROPT structure lpOptTSR.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/11/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AdvancedInit (VOID)
{
    extern CheckBoxRec cbMonitorAdvancedFloppyAccess;
    extern CheckBoxRec cbMonitorAdvancedFloppyReboot;
    extern CheckBoxRec cbMonitorAdvancedFloppyBoth;

    extern ComboBoxRec cxMonitorAdvancedFormatHard;
    extern ComboBoxRec cxMonitorAdvancedHardBoot;
    extern ComboBoxRec cxMonitorAdvancedFloppyBoot;
    extern ComboBoxRec cxMonitorAdvancedWriteEXE;
    extern ComboBoxRec cxMonitorAdvancedAttributeChange;


    cbMonitorAdvancedFloppyAccess.value     = lpNavOptions->tsr.bChkFlopAccess;
    cbMonitorAdvancedFloppyReboot.value     = lpNavOptions->tsr.bChkFlopOnBoot;
    cbMonitorAdvancedFloppyBoth.value       = lpNavOptions->tsr.bChkBothFlops;

    cxMonitorAdvancedFormatHard.value       = (DWORD) lpNavOptions->tsr.uFormat;
    cxMonitorAdvancedHardBoot.value         = (DWORD) lpNavOptions->tsr.uWriteHard;
    cxMonitorAdvancedFloppyBoot.value       = (DWORD) lpNavOptions->tsr.uWriteFlop;
    cxMonitorAdvancedWriteEXE.value         = (DWORD) lpNavOptions->tsr.uWriteExe;
    cxMonitorAdvancedAttributeChange.value  = (DWORD) lpNavOptions->tsr.uROAttrib;

} // End AdvancedInit()


//************************************************************************
// AdvancedAccept()
//
// This routine is called if the advanced dialog has been accepted.
// It sets the values of the temporary TSROPT structure *lpOptTSR to
// match the dialog controls.
//
// NOTE: The advanced options changes are not saved to the .INI file
//       unless and until the main monitor options dialog is accepted.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/11/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AdvancedAccept (VOID)
{
    extern CheckBoxRec cbMonitorAdvancedFloppyAccess;
    extern CheckBoxRec cbMonitorAdvancedFloppyReboot;
    extern CheckBoxRec cbMonitorAdvancedFloppyBoth;

    extern ComboBoxRec cxMonitorAdvancedFormatHard;
    extern ComboBoxRec cxMonitorAdvancedHardBoot;
    extern ComboBoxRec cxMonitorAdvancedFloppyBoot;
    extern ComboBoxRec cxMonitorAdvancedWriteEXE;
    extern ComboBoxRec cxMonitorAdvancedAttributeChange;


    lpNavOptions->tsr.bChkFlopAccess = cbMonitorAdvancedFloppyAccess.value;
    lpNavOptions->tsr.bChkFlopOnBoot = cbMonitorAdvancedFloppyReboot.value;
    lpNavOptions->tsr.bChkBothFlops  = cbMonitorAdvancedFloppyBoth.value;

    lpNavOptions->tsr.uFormat        = (BYTE) cxMonitorAdvancedFormatHard.value;
    lpNavOptions->tsr.uWriteHard     = (BYTE) cxMonitorAdvancedHardBoot.value;
    lpNavOptions->tsr.uWriteFlop     = (BYTE) cxMonitorAdvancedFloppyBoot.value;
    lpNavOptions->tsr.uWriteExe      = (BYTE) cxMonitorAdvancedWriteEXE.value;
    lpNavOptions->tsr.uROAttrib      = (BYTE) cxMonitorAdvancedAttributeChange.value;

} // End AdvancedAccept()


//------------------------------------------------------------------------
// System Monitor Sensor Dialog
//------------------------------------------------------------------------

//************************************************************************
// SensorDialog()
//
// This routine displays the system monitor virus sensor settings dialog
// and updates the settings information if necessary.
//
// Parameters:
//      none
//
// Returns:
//      nothing
//************************************************************************
// 6/2/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL SensorDialog (VOID)
{
                                        // These externs are declared in
                                        //  REALSTR.CPP
    extern  CheckBoxRec     cbMonitorSensorUse;
    extern  CheckBoxRec     cbMonitorSensorRepair;
    extern  CheckBoxRec     cbMonitorSensorContinue;
    extern  CheckBoxRec     cbMonitorSensorDelete;
    extern  CheckBoxRec     cbMonitorSensorExclude;
    extern  ComboBoxRec     cxMonitorSensorUnknownFound;
    extern  DIALOG          dlMonitorSensor;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult;


    SensorInit();
    HyperHelpTopicSet(HELP_DLG_SENSOR);

    lpdrDialog = DialogOpen2( &dlMonitorSensor,
                              &cbMonitorSensorUse,
                              &cxMonitorSensorUnknownFound,

                              &cbMonitorSensorRepair,
                              &cbMonitorSensorContinue,
                              &cbMonitorSensorDelete,
                              &cbMonitorSensorExclude );

    if (lpdrDialog != NULL)
        {
        do
            {
            wResult = StdEventLoop(NULL,    // Menu
                                   lpdrDialog,
                                   SensorButtonsProc,
                                   NULL,    // Menu items function
                                   SensorPreProc,
                                   NULL);   // Post event function
            if (wResult == ACCEPT_DIALOG)
                {
                SensorAccept();
                break;
                }
            }
            while (wResult != ABORT_DIALOG);

        DialogClose(lpdrDialog, wResult);
        }
} // End StarupDialog()


//************************************************************************
// SensorButtonsProc()
//
// This routine handles the button presses in the System Monitor Sensor
// dialog.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//
// Returns:
//      ACCEPT_DIALOG                   Ok button hit
//      ABORT_DIALOG                    Cancel hit
//      EVENT_AVAIL                     Help, or default
//************************************************************************
// 6/2/93 DALLEE Function created.
//************************************************************************

int STATIC SensorButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    auto    int     iReturn = EVENT_AVAIL;
    BYTE                byValue;


    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)           // Current button selected
        {
        case MONITOR_SENSOR_OK_BUTTON:
            iReturn = ACCEPT_DIALOG;
            break;

        case MONITOR_SENSOR_CANCEL_BUTTON:
            iReturn = ABORT_DIALOG;
            break;

        case MONITOR_SENSOR_HELP_BUTTON:
            HyperHelpDisplay();
            break;
        }

    return (iReturn);

} // End SensorButtonsProc()


//************************************************************************
// SensorPreProc()
//
// This routine disables the "Buttons to Display if Prompted" buttons
// depending on the state of the "How to Respond" combo box.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//      int           *lpiEvent         Current event
//
// Returns:
//      nothing
//************************************************************************
// 7/12/93 DALLEE Function created.
//************************************************************************

VOID STATIC SensorPreProc (DIALOG_RECORD *lpdrDialog, int *lpiEvent)
{
    extern  CheckBoxRec     cbMonitorSensorRepair;
    extern  CheckBoxRec     cbMonitorSensorContinue;
    extern  CheckBoxRec     cbMonitorSensorDelete;
    extern  CheckBoxRec     cbMonitorSensorExclude;
    extern  ComboBoxRec     cxMonitorSensorUnknownFound;


    auto    BYTE            byEnableCheckBoxes;

    byEnableCheckBoxes = (cxMonitorSensorUnknownFound.value + 1 == TSR_PROMPT);

                                        // Enable/Disable only if necessary
                                        // since CheckBoxEnable always redraws
                                        // (+1 for TSR, don't ask...)
    if (!(cbMonitorSensorRepair.flags & CB_DISABLED) != byEnableCheckBoxes)
        {
        CheckBoxEnable(lpdrDialog, &cbMonitorSensorRepair,  byEnableCheckBoxes);
        CheckBoxEnable(lpdrDialog, &cbMonitorSensorContinue,byEnableCheckBoxes);
        CheckBoxEnable(lpdrDialog, &cbMonitorSensorDelete,  byEnableCheckBoxes);
        CheckBoxEnable(lpdrDialog, &cbMonitorSensorExclude, byEnableCheckBoxes);
        }
} // End SensorPreProc()


//************************************************************************
// SensorInit()
//
// This routine sets up the monitor startup options dialog controls
// based on the values in the temporary TSROPT structure lpOptTSR.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 6/2/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL SensorInit (VOID)
{
    extern  CheckBoxRec     cbMonitorSensorUse;
    extern  CheckBoxRec     cbMonitorSensorRepair;
    extern  CheckBoxRec     cbMonitorSensorContinue;
    extern  CheckBoxRec     cbMonitorSensorDelete;
    extern  CheckBoxRec     cbMonitorSensorExclude;
    extern  ComboBoxRec     cxMonitorSensorUnknownFound;

    auto    UINT            uComboValue;

    cbMonitorSensorUse.value           = lpNavOptions->tsr.bScanUnknown;
    cbMonitorSensorRepair.value        = lpNavOptions->tsr.bDispUnknownRepair;
    cbMonitorSensorDelete.value        = lpNavOptions->tsr.bDispUnknownDelete;
    cbMonitorSensorContinue.value      = lpNavOptions->tsr.bDispUnknownContinue;
    cbMonitorSensorExclude.value       = lpNavOptions->tsr.bDispUnknownExclude;

    // Dumb kludge for TSR since values should go 1, 3, 4, 5.
    uComboValue = lpNavOptions->tsr.uActionUnknown;
    if (uComboValue != TSR_PROMPT)
        {
        uComboValue--;
        }
                                        // Combo box is 0-based
    cxMonitorSensorUnknownFound.value  = (DWORD) uComboValue - 1;

} // End SensorInit()


//************************************************************************
// SensorAccept()
//
// This routine is called if the sensor dialog has been accepted.
// It sets the values of the temporary TSROPTS structure *lpOptTSR to
// match the dialog controls.
//
// NOTE: The sensor options changes are not saved to the .INI file
//       unless and until the main monitor options dialog is accepted.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 6/2/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL SensorAccept (VOID)
{
    extern  CheckBoxRec     cbMonitorSensorUse;
    extern  CheckBoxRec     cbMonitorSensorRepair;
    extern  CheckBoxRec     cbMonitorSensorContinue;
    extern  CheckBoxRec     cbMonitorSensorDelete;
    extern  CheckBoxRec     cbMonitorSensorExclude;
    extern  ComboBoxRec     cxMonitorSensorUnknownFound;

    auto    UINT            uComboValue;

    lpNavOptions->tsr.bScanUnknown  = cbMonitorSensorUse.value;
    lpNavOptions->tsr.bDispUnknownRepair   = cbMonitorSensorRepair.value;
    lpNavOptions->tsr.bDispUnknownDelete   = cbMonitorSensorDelete.value;
    lpNavOptions->tsr.bDispUnknownContinue = cbMonitorSensorContinue.value;
    lpNavOptions->tsr.bDispUnknownExclude  = cbMonitorSensorExclude.value;

    // Dumb kludge for TSR since values should go 1, 3, 4, 5.

                                        // Combobox is 0-based. Add 1.
    uComboValue = (UINT) cxMonitorSensorUnknownFound.value + 1;
    if (uComboValue != TSR_PROMPT)
        {
        uComboValue++;
        }
    lpNavOptions->tsr.uActionUnknown = uComboValue;

} // End SensorAccept()

