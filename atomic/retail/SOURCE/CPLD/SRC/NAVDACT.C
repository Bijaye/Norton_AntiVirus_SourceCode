// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdact.c_v   1.0   06 Feb 1997 20:50:20   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV Activity Log options
//      configuration
//
// Contains:
//      CPDialogProcActivity()
//
// See Also:
//      ACTSTR.CPP for strings and control structures.
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/cpld/VCS/navdact.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:20   RFULLER
// Initial revision
// 
//    Rev 1.1   07 Aug 1996 16:47:20   JBRENNA
// Port in changes from NAVDOS.
// 
// 
//    Rev 1.41   06 Aug 1996 17:26:20   MZAREMB
// Copied the static buffer back to the options structure when editing is 
// complete.
// 
//    Rev 1.40   05 Aug 1996 18:26:48   MZAREMB
// Created a static buffer to hold the text edit string from the options file
// (as it is being changed by the password function when the user steps through
// the options and does not enter a password immediately.)
// 
//    Rev 1.39   13 Jun 1996 11:52:44   JALLEE
// Changed if USE_PASSWORD to #ifdef USE_PASSWORD
// 
//    Rev 1.38   05 May 1996 11:24:42   MZAREMB
// Fixed GPF in DX by adding CPL_PRE_ENTERING ifdef in DialogProc.
// 
//    Rev 1.37   09 Apr 1996 10:55:38   MZAREMB
// Improved the handling of th euser entering a zero for the maximum size 
// of the activity log.  Now works similar to NAVW: automatically enter a 1K 
// default if the user clicks OK and the log size is zero.
// 
//    Rev 1.36   31 Mar 1996 18:27:54   MZAREMB
// Added code to prevent exiting activity log dialog if the log size is set 
// to zero.  STS#NAV 51781.
// 
//    Rev 1.35   28 Dec 1994 11:44:02   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 15:48:46   DALLEE
    // Commented out password protection.
    //
    //    Rev 1.1   06 Dec 1994 20:48:16   DALLEE
    // CVT1 script.
    //
//    Rev 1.34   28 Dec 1994 11:34:34   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:52   DALLEE
    // From Nirvana
    //
    //    Rev 1.33   15 Jun 1994 23:04:58   DALLEE
    // Don't complain about path if it's blanked because of password protect.
    // Also, move initialization of path Text Edit after password check, since
    // the check clears the control.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "file.h"
#include "symalog.h"

#include "navutil.h"
#include "navdprot.h"

#include "options.h"

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define ACTIVITY_OK_BUTTON      (0)
#define ACTIVITY_CANCEL_BUTTON  (1)
#define ACTIVITY_HELP_BUTTON    (2)

#define ALT_RIGHT_ARROW		413


//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern  LPNAVOPTIONS    lpNavOptions;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

UINT ValidActivityLogSize (VOID);
UINT ActivityCP_ENTERING (VOID);
UINT LOCAL PASCAL ActivityMemory (VOID);
VOID LOCAL PASCAL ActivityAccept (VOID);
UINT TECheckLogFilename ( TERec * pTE );// Callback to validate filename
VOID LOCAL PASCAL DisableActivityControls (DIALOG_RECORD *lpdrDialog);

static  BYTE    far szLogPath[SYM_MAX_PATH];

//************************************************************************
// CPDialogProcActivity()
//
// This is the callback routine for the Options CP Activity Log Dialog.
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
// 2/12/93 DALLEE Function created.
// 6/06/94 DALLEE, validate activity log path when accepting dialog.
//************************************************************************

UINT CPDialogProcActivity (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern  char            SZ_LOG_FILE_FILTER[];
    extern  char            SZ_LOG_FILE_TITLE[];
    extern  BYTE *          apszInvalidPathError[];

    extern  ButtonsRec      buEllipses;
    extern  ButtonsRec	    buOkCancelHelp;

    extern  CheckBoxRec     cbActivityKnownVirus;
    extern  CheckBoxRec     cbActivityUnknownVirus;
    extern  CheckBoxRec     cbActivityInocChange;
    extern  CheckBoxRec     cbActivityVirusLike;
    extern  CheckBoxRec     cbActivityStartEnd;
    extern  CheckBoxRec     cbActivityVirusList;
    extern  CheckBoxRec     cbActivityKeepKB;

    extern  TERec           teActivityKeepKB;
    extern  TERec           teActivityLogFile;
    extern  BYTE            byAcceptCPDialog;

    auto    WORD            wResult;
    auto    WORD            wOldHelp;

#ifdef USE_PASSWORD    //&?
 static BOOL     bNormalProcess;
#endif


    switch (uMessage)
        {
        case CP_PRE_EVENT:              // Event received, not yet processed

	    switch (uParameter)
	    	{
		case ALT_RIGHT_ARROW :
    	            if (lpDialog->item->item == &teActivityLogFile)
			{
                        HyperHelpTopicSet(HELP_DLG_LOGFILE_BROWSE);
                        TunnelSelectFile(&teActivityLogFile, 
                                         SZ_LOG_FILE_FILTER,
                                         SZ_LOG_FILE_TITLE);
                        HyperHelpTopicSet(HELP_DLG_CPL_ACTIVITY);

		        DialogSetFocus(lpDialog, &buEllipses);
		        return(EVENT_USED);
			}
		    break;
		}

					// Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (uParameter != ESC);
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:             // Event received and processed
                                        // Check combo boxes, TE's here?
#ifdef USE_PASSWORD    //&?
            if (bNormalProcess)
#endif
                {
                DisableActivityControls(lpDialog);
                }
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case
            if ((ButtonsRec *) lpDialog->item->item == &buEllipses)
                {
                HyperHelpTopicSet(HELP_DLG_LOGFILE_BROWSE);
                TunnelSelectFile(&teActivityLogFile, SZ_LOG_FILE_FILTER,
                                                     SZ_LOG_FILE_TITLE);
                HyperHelpTopicSet(HELP_DLG_CPL_ACTIVITY);
                return (EVENT_AVAIL);
                }

            switch (buOkCancelHelp.value)   // Current button selected
                {
                case ACTIVITY_OK_BUTTON:
                                        // Data is saved on CP_LEAVING
                    return (ACCEPT_DIALOG);

                case ACTIVITY_CANCEL_BUTTON:
                                        // Set save flag = FALSE
                    byAcceptCPDialog = FALSE;
                    return (ABORT_DIALOG);

                case ACTIVITY_HELP_BUTTON:
                                        // Call activity settings help here
                    HyperHelpDisplay();
                    return (EVENT_AVAIL);
                } // End switch (lpbuttons->value)

            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED


        case CP_PRE_ENTERING:           // Haven't entered the dialog yet
            
                return ( ActivityMemory() );

        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptActivity info from database
                                        //  and initialize dialog data
            wOldHelp = HyperHelpTopicSet(HELP_DLG_CPL_ACTIVITY);
#ifndef USE_PASSWORD //&?
            wResult = ActivityCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog, 
                                      PASSWORD_VIEWACTIVITYOPT,
                                      ActivityCP_ENTERING,
                                      &bNormalProcess);
            if (bNormalProcess)
#endif
                {
                DisableActivityControls(lpDialog);
                }
            return (wResult);

        case CP_LEAVING:                // Dialog box is losing focus,
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.
            if ( uParameter != ACCEPT_DIALOG )
                {
                byAcceptCPDialog = FALSE;
                }

            if ( byAcceptCPDialog
#ifdef USE_PASSWORD
                 && !NeedPassword(PASSWORD_VIEWACTIVITYOPT)
#endif
                 )
                {
                if ((FALSE == TECheckLogFilename(&teActivityLogFile)) ||
                    (FALSE == ValidActivityLogSize()))
                    {
                                        // User tried to accept dialog
                                        // with invalid activity log path,
                                        // or no activity log size.
                    return (CONTINUE_DIALOG);  // Don't allow exit.
                    }
                else
                    {
                    ActivityAccept();   // save options.
                    }
                }

            HyperHelpTopicSet(wOldHelp);

            DialogSetFocus(lpDialog, &buOkCancelHelp);

            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);

        } // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcActivity()


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

UINT LOCAL PASCAL ActivityMemory (VOID)
{
    extern TERec        teActivityLogFile;
                                        // Assign TERec string pointer
                                        // to the buffer we just locked.
    STRCPY(szLogPath, lpNavOptions->activity.szLogFile);
    teActivityLogFile.string = szLogPath;

    return(EVENT_AVAIL);
}


//************************************************************************
// ActivityCP_ENTERING()
//
// This routine handles the CP_ENTERING message for the Activity options
// Control Panel entry.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Continue w/ dialog
//      ABORT_DIALOG                    Cancel dialog - couldn't allocate
//                                       memory for the temp ACTOPT struct
//************************************************************************
// 3/14/93 DALLEE Function created.
//************************************************************************

UINT ActivityCP_ENTERING (VOID)
{
    extern CheckBoxRec  cbActivityKnownVirus;
    extern CheckBoxRec  cbActivityUnknownVirus;
    extern CheckBoxRec  cbActivityInocChange;
    extern CheckBoxRec  cbActivityVirusLike;
    extern CheckBoxRec  cbActivityStartEnd;
    extern CheckBoxRec  cbActivityVirusList;
    extern CheckBoxRec  cbActivityKeepKB;
    extern TERec        teActivityKeepKB;
    extern TERec        teActivityLogFile;
                                        // Enough spaces to hold a converted
    auto   char         szNumber[7];    // UINT value

    UpdateCheckBox(&cbActivityKnownVirus,   lpNavOptions->activity.bKnown);
    UpdateCheckBox(&cbActivityUnknownVirus, lpNavOptions->activity.bUnknown);
    UpdateCheckBox(&cbActivityInocChange,   lpNavOptions->activity.bInocChange);
    UpdateCheckBox(&cbActivityVirusLike,    lpNavOptions->activity.bVirusLike);
    UpdateCheckBox(&cbActivityStartEnd,     lpNavOptions->activity.bStartEnd);
    UpdateCheckBox(&cbActivityVirusList,    lpNavOptions->activity.bVirusList);
    UpdateCheckBox(&cbActivityKeepKB,       lpNavOptions->activity.bKeepUpTo);

                                        // Convert UINT to string with base 10
                                        // and no thousands separator
    NAVDLoadTextEdit(&teActivityKeepKB,
      _ConvertWordToString(lpNavOptions->activity.uKiloBytes, szNumber, 10, 0));

    ActivityMemory();                   // Load name in teActivityLogFile.

    _TextEditWrite(&teActivityKeepKB, TRUE, FALSE);
    _TextEditWrite(&teActivityLogFile, TRUE, FALSE);

    return (EVENT_AVAIL);
} // End ActivityCP_ENTERING()


//***************************************************************************
// TECheckLogFilename()
// 
// Description:
//      Checks the Activity Log filename when leaving the field.  Called
//      by the dialog manager.
//
// Returns:
//      FALSE                           Bad path field.
//      TRUE                            Path OK.
//***************************************************************************
// 10/05/1993 BARRY Function Created.
//  6/06/1994 DALLEE, Check valid DOS path (not just for device name).
//***************************************************************************
UINT TECheckLogFilename ( TERec * pTE )
{
    extern  BYTE *      apszBadLogFilename[];
    extern  BYTE *      apszInvalidPathError[];

    auto    BYTE **     ppbyErrorString = NULL;

                                        // Can't log to a device.
    if ( NameIsDevice ( pTE->string ) )
        {
        ppbyErrorString = apszBadLogFilename;
        }
                                        // Make sure path is valid.
    else if (FALSE == NameValidatePath(pTE->string))
        {
        ppbyErrorString = apszInvalidPathError;
        }

                                        // Display error, if any.
    if (NULL != ppbyErrorString)
        {
        CursorPush();                   // Dialog doesn't preserve cursor
        StdDlgError(ppbyErrorString);
        CursorPop();
        return (FALSE);                 // Don't allow this path.
        }
    
    return (TRUE);
}


//************************************************************************
// ActivityAccept()
//
// This routine updates the temporary ACTOPT struct *lpOptAct from the
// dialog controls, and if changes were made, saves them to the .INI file
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/14/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL ActivityAccept (VOID)
{
    extern CheckBoxRec  cbActivityKnownVirus;
    extern CheckBoxRec  cbActivityUnknownVirus;
    extern CheckBoxRec  cbActivityInocChange;
    extern CheckBoxRec  cbActivityVirusLike;
    extern CheckBoxRec  cbActivityStartEnd;
    extern CheckBoxRec  cbActivityVirusList;
    extern CheckBoxRec  cbActivityKeepKB;
    extern TERec        teActivityKeepKB;
    extern TERec        teActivityLogFile;


    lpNavOptions->activity.bKnown      = cbActivityKnownVirus.value;
    lpNavOptions->activity.bUnknown    = cbActivityUnknownVirus.value;
    lpNavOptions->activity.bInocChange = cbActivityInocChange.value;
    lpNavOptions->activity.bVirusLike  = cbActivityVirusLike.value;
    lpNavOptions->activity.bStartEnd   = cbActivityStartEnd.value;
    lpNavOptions->activity.bVirusList  = cbActivityVirusList.value;
    lpNavOptions->activity.bKeepUpTo   = cbActivityKeepKB.value;

    ConvertStringToWord(&lpNavOptions->activity.uKiloBytes,
                         teActivityKeepKB.string);
    STRCPY(lpNavOptions->activity.szLogFile,  teActivityLogFile.string);
} // End ActivityAccept()



//************************************************************************
// DisableActivityControls()
//
// This routine enables/disables the activity log text edits depending on
// the state of the corresponding check boxes.
//
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/14/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL DisableActivityControls (DIALOG_RECORD *lpdrDialog)
{
    extern CheckBoxRec  cbActivityKeepKB;
    extern TERec        teActivityKeepKB;

                                        // If the CheckBox is TRUE,
                                        // the TE should be enabled.
                                        // Also, the inverse.
                                        // Update only if necessary.
    if (cbActivityKeepKB.value ==
            (BYTE)((teActivityKeepKB.flags & TE_DISABLED) ? TRUE : FALSE))
        {
        TextEditEnable(lpdrDialog, &teActivityKeepKB,
                       cbActivityKeepKB.value);
        }
} // End DisableActivityControls()

//***************************************************************************
// ValidActivityLogSize()
// 
// Description:
//      Checks the Activity Log size when leaving the field.  Called
//      by the dialog manager.
//
// Returns:
//      FALSE                           Log size is zero.
//      TRUE                            Valid log size.
//***************************************************************************
// 03/31/1996 MZAREMB Function Created.
//***************************************************************************

UINT ValidActivityLogSize (VOID)
{
    UINT    uLogSize;
    extern TERec        teActivityKeepKB;
    extern CheckBoxRec  cbActivityKeepKB;

    ConvertStringToWord(&uLogSize, teActivityKeepKB.string);

    if ( 0 == uLogSize ) 
        {
        if ( TRUE == cbActivityKeepKB.value )   // Control is enabled?
            {
            uLogSize = 1;                       // Store a valid value 
                                                // into the text edit field
            _ConvertWordToString(uLogSize,teActivityKeepKB.string, 10, 0);
            }
        }
    
    return (TRUE);
}
