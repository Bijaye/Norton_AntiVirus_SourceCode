// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdgen.c_v   1.0   06 Feb 1997 20:50:20   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV General options
//      configuration.
//
// Contains:
//      CPDialogProcGeneral()
//
// See Also:
//      GENSTR.CPP for strings and control structures.
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/cpld/VCS/navdgen.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:20   RFULLER
// Initial revision
// 
//    Rev 1.2   26 Oct 1996 17:07:48   MKEATIN
// If no strings is entered for szBackupExt, it will default to VIR.
// 
//    Rev 1.1   07 Aug 1996 16:49:02   JBRENNA
// Port in changes from DOSNAV environment
// 
// 
//    Rev 1.23   13 Jun 1996 12:02:48   JALLEE
// #if USE_PASSWORD -> #ifdef USE_PASSWORD.
// 
//    Rev 1.22   28 Dec 1994 11:43:52   DALLEE
// Latest NAVBOOT revision.
    //
    //    Rev 1.2   12 Dec 1994 15:48:40   DALLEE
    // Commented out password protection.
    //
    //    Rev 1.1   06 Dec 1994 20:48:12   DALLEE
    // CVT1 script.
    //
//    Rev 1.21   28 Dec 1994 11:26:04   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:48   DALLEE
    // From Nirvana
    //
    //    Rev 1.19   12 Oct 1993 16:46:58   BARRY
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

#define GENERAL_OK_BUTTON      ( 0)
#define GENERAL_CANCEL_BUTTON  ( 1)
#define GENERAL_HELP_BUTTON    ( 2)


//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************
extern  LPNAVOPTIONS    lpNavOptions;

//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

UINT GeneralCP_ENTERING (VOID);
VOID LOCAL PASCAL GeneralAccept (VOID);
VOID LOCAL PASCAL DisableGeneralControls (DIALOG_RECORD *lpdrDialog);


//************************************************************************
// CPDialogProcGeneral()
//
// This is the callback routine for the Options CP General Dialog.
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
// 2/18/93 DALLEE Function created.
//************************************************************************

UINT CPDialogProcGeneral (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern  CheckBoxRec cbGeneralBackup;
    extern  TERec       teGeneralBackupExt;
    extern  CheckBoxRec cbScanHighMemory;
    extern  ButtonsRec	buOkCancelHelp;

    extern  BYTE        byAcceptCPDialog;

    auto    WORD        wResult;
#ifdef USE_PASSWORD    //&?
    static  BOOL        bNormalProcess;
#endif
    

    switch (uMessage)
        {
        case CP_PRE_EVENT:              // Event received, not yet processed
                                        // Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (uParameter != ESC);
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:             // Event received and processed
                                        // Do we need to disable any controls?
#ifdef USE_PASSWORD    //&?
	    if (bNormalProcess)
#endif
                {
            	DisableGeneralControls(lpDialog);
                }
            return (CONTINUE_DIALOG);

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case

            switch (buOkCancelHelp.value)   // Current button selected
                {
                case GENERAL_OK_BUTTON:
                                        // Options saved on CP_LEAVING message
                    return (ACCEPT_DIALOG);

                case GENERAL_CANCEL_BUTTON:
                                        // Set save flag FALSE
                    byAcceptCPDialog = FALSE;
                    return (ABORT_DIALOG);

                case GENERAL_HELP_BUTTON:
                                        // Call General settings help here
		    HyperHelpDisplay();
                    return (EVENT_AVAIL);
                }

            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED

        case CP_PRE_ENTERING:           // Haven't entered dialog yet
            break;

        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptGeneral info from database
                                        //  and initialize dialog data
	    HyperHelpTopicSet(HELP_DLG_CPL_GENERAL);
#ifndef USE_PASSWORD //&?
            wResult = GeneralCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog,
	                              PASSWORD_VIEWGENOPT,
                                      GeneralCP_ENTERING,
                                      &bNormalProcess);
	    if (bNormalProcess)
#endif
                {
                DisableGeneralControls(lpDialog);
                }
            return (wResult);

        case CP_LEAVING:                // Dialog box is losing focus
                                        // If the dialog was not canceled,
                                        // save options if necessary.
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.

            if ( uParameter != ACCEPT_DIALOG )
                byAcceptCPDialog = FALSE;

            DialogSetFocus(lpDialog, &buOkCancelHelp);

            if ( byAcceptCPDialog
#ifdef USE_PASSWORD    //&?
                 && !NeedPassword (PASSWORD_VIEWGENOPT)
#endif
                 )
                {
                GeneralAccept();
                }

            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);
        } // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcGeneral()


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

//************************************************************************
// GeneralCP_ENTERING()
//
// This routine handles the CP_ENTERING message for the General options
// Control Panel entry.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Continue w/ dialog
//      ABORT_DIALOG                    Cancel dialog - couldn't allocate
//                                       memory for the temp GENOPT struct
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

UINT GeneralCP_ENTERING (VOID)
{
    extern  CheckBoxRec     cbGeneralBackup;
    extern  CheckBoxRec     cbScanHighMemory;
    extern  TERec           teGeneralBackupExt;


    UpdateCheckBox(&cbGeneralBackup,      lpNavOptions->general.bBackupRep);
    UpdateCheckBox(&cbScanHighMemory,     lpNavOptions->general.bScanHighMemory);

    NAVDLoadTextEdit(&teGeneralBackupExt, lpNavOptions->general.szBackExt);
    _TextEditWrite(&teGeneralBackupExt, TRUE, FALSE);

    return (EVENT_AVAIL);
} // End GeneralCP_ENTERING()


//************************************************************************
// GeneralAccept()
//
// This routine updates the temporary GENOPT struct *lpOptGen from the
// dialog controls, and if changes were made, saves them to the .INI file
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL GeneralAccept (VOID)
{
    extern  CheckBoxRec     cbGeneralBackup;
    extern  CheckBoxRec     cbScanHighMemory;
    extern  TERec           teGeneralBackupExt;


    lpNavOptions->general.bBackupRep      = cbGeneralBackup.value;
    lpNavOptions->general.bScanHighMemory = cbScanHighMemory.value;

    if (STRLEN(teGeneralBackupExt.string))
        STRCPY(lpNavOptions->general.szBackExt, teGeneralBackupExt.string);
    else
        STRCPY(lpNavOptions->general.szBackExt, "VIR");
       

} // End GeneralAccept()



//************************************************************************
// DisableGeneralControls()
//
// This routine disables the Backup Extension TERec depending
// on the state of the Backup File When Repairing CheckBoxRec.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/16/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL DisableGeneralControls(DIALOG_RECORD *lpdrDialog)
{
    extern  CheckBoxRec cbGeneralBackup;
    extern  TERec       teGeneralBackupExt;

                                        // If the CheckBox is TRUE,
                                        // the TE should be enabled.
                                        // Also, the inverse.
                                        // Update only if necessary.
    if ( cbGeneralBackup.value ==
         (BYTE)((teGeneralBackupExt.flags & TE_DISABLED) ? TRUE : FALSE) )
        {
        TextEditEnable(lpdrDialog, &teGeneralBackupExt,
                       cbGeneralBackup.value);
        }
} // End DisableGeneralControls()

