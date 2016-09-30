// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdinoc.c_v   1.0   06 Feb 1997 20:50:18   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV inoculation options
//      configuration.
//
// Contains:
//      CPDialogProcInoc()
//
// See Also:
//      INOCSTR.CPP for strings and control structures.
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/cpld/VCS/navdinoc.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:18   RFULLER
// Initial revision
// 
//    Rev 1.3   01 Nov 1996 18:04:34   MKEATIN
// Do not lowercase trail bytes.
// 
//    Rev 1.2   25 Oct 1996 12:07:30   MKEATIN
// Save inoculation path in InocAccept()
// 
//    Rev 1.1   30 Sep 1996 14:38:56   JALLEE
// Port of DBCS changes from Nirvan Plus.
// 
//    Rev 1.0   30 Jan 1996 15:56:52   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:09:56   BARRY
// Initial revision.
// 
//    Rev 1.31   01 Jul 1995 17:03:06   BARRY
// Fix inoc path validation
// 
//    Rev 1.30   28 Dec 1994 11:43:56   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 15:48:18   DALLEE
    // Commented out password protection.
    //
    //    Rev 1.1   06 Dec 1994 20:48:10   DALLEE
    // CVT1 script.
    //
//    Rev 1.29   28 Dec 1994 11:34:26   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:48   DALLEE
    // From Nirvana
    //
    //    Rev 1.28   15 Jun 1994 23:06:20   DALLEE
    // Move initialization of path text edit after the password check since it
    // clears the control.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "dbcs.h"

#include "navutil.h"
#include "navdprot.h"

#include "options.h"

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define INOC_OK_BUTTON          0
#define INOC_CANCEL_BUTTON      1
#define INOC_HELP_BUTTON        2


//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern  LPNAVOPTIONS    lpNavOptions;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL DisableInocControls   (DIALOG_RECORD *lpdrDialog);
VOID LOCAL PASCAL InocAccept            (VOID);
UINT InocCP_ENTERING       (VOID);
UINT TECheckInocPath ( TERec * pTE );
UINT TEInocPathFilter ( TERec *te, UINT *c);

static BYTE far szInocPath[SYM_MAX_PATH];

//************************************************************************
// CPDialogProcInoc()
//
// This is the callback routine for the Options CP Inoculation Dialog.
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
// 2/16/93 DALLEE Function created.
//************************************************************************

UINT CPDialogProcInoc (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern  BYTE            byAcceptCPDialog;
    extern  ButtonsRec	    buOkCancelHelp;
    extern  CheckBoxRec     cbInocUse;
    extern  CheckBoxRec     cbInocFloppies;
    extern  CheckBoxRec     cbInocSystem;
    extern  CheckBoxRec     cbInocRepair;
    extern  CheckBoxRec     cbInocInoculate;
    extern  CheckBoxRec     cbInocStop;
    extern  CheckBoxRec     cbInocDelete;
    extern  CheckBoxRec     cbInocContinue;
    extern  CheckBoxRec     cbInocExclude;

    extern  ComboBoxRec     cxInocRespondNotInoc;
    extern  ComboBoxRec     cxInocRespondChanged;

    extern  TERec           teInocPath;

#ifdef USE_PASSWORD    //&?
    static  BOOL                bNormalProcess;
#endif
    auto    WORD                wResult;


    switch (uMessage)
        {
        case CP_PRE_EVENT:              // Event received, not yet processed.
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
                DisableInocControls(lpDialog);
                }
            return (CONTINUE_DIALOG);

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case
            switch (buOkCancelHelp.value)   // Current button selected
                {
                case INOC_OK_BUTTON:
                                        // Data is saved on CP_LEAVING
                    return (ACCEPT_DIALOG);

                case INOC_CANCEL_BUTTON:
                                        // Don't save on exit
                    byAcceptCPDialog = FALSE;
                    return (ABORT_DIALOG);

                case INOC_HELP_BUTTON:
                                        // Call inoculation settings help here
		    HyperHelpDisplay();
                    return (EVENT_AVAIL);
                }

            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED

#ifdef SYM_DOSX
        case CP_PRE_ENTERING:           
            STRCPY(szInocPath, lpNavOptions->inoc.szNetInocPath);
            teInocPath.string = szInocPath;
            return (EVENT_AVAIL);       
#endif        

        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptInoc info from database
                                        //  and initialize dialog data
            HyperHelpTopicSet(HELP_DLG_CPL_INOCULATION);
#ifndef USE_PASSWORD //&?
            wResult = InocCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog, 
                                      PASSWORD_VIEWINOCOPT,
                                      InocCP_ENTERING,
                                      &bNormalProcess);
            if (bNormalProcess)
#endif
                {
                DisableInocControls(lpDialog);
                }
            return (wResult);

        case CP_LEAVING:                // Dialog box is losing focus
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.

            if ( uParameter != ACCEPT_DIALOG )
                byAcceptCPDialog = FALSE;


            if ( byAcceptCPDialog
#ifdef USE_PASSWORD    //&?
                 && !NeedPassword (PASSWORD_VIEWINOCOPT)
#endif
                 )
                {
                if ( FALSE == TECheckInocPath(&teInocPath) )
                    return (CONTINUE_DIALOG);
                else
                    InocAccept();       // save options.        
                }
            
            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);
        
        } // End switch (uMessage)
    
    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcInoc()


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

//************************************************************************
// InocCP_ENTERING()
//
// This routine handles loads the dialog items with the inoc options
// values.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Continue w/ dialog
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

UINT InocCP_ENTERING (VOID)
{
    extern  CheckBoxRec     cbInocUse;
    extern  CheckBoxRec     cbInocFloppies;
    extern  CheckBoxRec     cbInocSystem;
    extern  CheckBoxRec     cbInocRepair;
    extern  CheckBoxRec     cbInocInoculate;
    extern  CheckBoxRec     cbInocStop;
    extern  CheckBoxRec     cbInocDelete;
    extern  CheckBoxRec     cbInocContinue;
    extern  CheckBoxRec     cbInocExclude;
    extern  ComboBoxRec     cxInocRespondNotInoc;
    extern  ComboBoxRec     cxInocRespondChanged;
    extern  TERec           teInocPath;

    auto    UINT            uValue;


    UpdateCheckBox(&cbInocUse,       lpNavOptions->inoc.bUseInoc);
    UpdateCheckBox(&cbInocFloppies,  lpNavOptions->inoc.bInocFloppies);
    UpdateCheckBox(&cbInocSystem,    lpNavOptions->inoc.bUseSystemInoc);
    UpdateCheckBox(&cbInocRepair,    lpNavOptions->inoc.bDispRepair);
    UpdateCheckBox(&cbInocDelete,    lpNavOptions->inoc.bDispDelete);
    UpdateCheckBox(&cbInocInoculate, lpNavOptions->inoc.bDispInoc);
    UpdateCheckBox(&cbInocContinue,  lpNavOptions->inoc.bDispCont);
    UpdateCheckBox(&cbInocStop,      lpNavOptions->inoc.bDispStop);
    UpdateCheckBox(&cbInocExclude,   lpNavOptions->inoc.bDispExcl);

    // ************************************************************ //
    // Note that the -1 is for TSR
    // ************************************************************ //
    ComboBoxSetSelection(&cxInocRespondNotInoc,
                         lpNavOptions->inoc.uActionNewInoc-1);

    // *********************************************************** //
    // This may look goofy but it's required for TSR
    // *********************************************************** //

    uValue = lpNavOptions->inoc.uActionInocChg;
    if (uValue != INOC_CHANGE_PROMPT )
        uValue--;
    uValue--;                           // 0-based
                                        // Set Unknown Virus Action
    ComboBoxSetSelection(&cxInocRespondChanged, uValue);

    // Done with ComboBox Kludges
    //--------------------------------------------------------------------

                                        // Assign TERec string pointer
    STRCPY(szInocPath, lpNavOptions->inoc.szNetInocPath);
    teInocPath.string = szInocPath;
    _TextEditWrite(&teInocPath, TRUE, FALSE);

    return (EVENT_AVAIL);
} // End InocCP_ENTERING()


//************************************************************************
// InocAccept()
//
// This routine updates the settings structure.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL InocAccept (VOID)
{
    extern CheckBoxRec     cbInocUse;
    extern CheckBoxRec     cbInocFloppies;
    extern CheckBoxRec     cbInocSystem;
    extern CheckBoxRec     cbInocRepair;
    extern CheckBoxRec     cbInocInoculate;
    extern CheckBoxRec     cbInocStop;
    extern CheckBoxRec     cbInocDelete;
    extern CheckBoxRec     cbInocContinue;
    extern CheckBoxRec     cbInocExclude;
    extern ComboBoxRec     cxInocRespondNotInoc;
    extern ComboBoxRec     cxInocRespondChanged;
    extern TERec           teInocPath;
    auto   UINT            uValue;


    lpNavOptions->inoc.bUseInoc     = cbInocUse.value;
    lpNavOptions->inoc.bInocFloppies= cbInocFloppies.value;
    lpNavOptions->inoc.bUseSystemInoc = cbInocSystem.value;
    lpNavOptions->inoc.bDispRepair  = cbInocRepair.value;
    lpNavOptions->inoc.bDispInoc    = cbInocInoculate.value;
    lpNavOptions->inoc.bDispStop    = cbInocStop.value;
    lpNavOptions->inoc.bDispDelete  = cbInocDelete.value;
    lpNavOptions->inoc.bDispCont    = cbInocContinue.value;
    lpNavOptions->inoc.bDispExcl    = cbInocExclude.value;

    STRCPY(lpNavOptions->inoc.szNetInocPath, teInocPath.string);

                                        // Note that the +1 is for TSR
    lpNavOptions->inoc.uActionNewInoc = (BYTE) (cxInocRespondNotInoc.value+1);

    // *********************************************************** //
    // This may look goofy but it's required for TSR
    // *********************************************************** //

                                        // 0-based; add 1
    uValue = (UINT) cxInocRespondChanged.value+1;

    if (uValue != INOC_CHANGE_PROMPT)
        uValue++;                       // skip an entry

    lpNavOptions->inoc.uActionInocChg = uValue;

} // End InocAccept()


//************************************************************************
// DisableInocControls()
//
// This routine disables the How to Respond checkboxes based on the
// settings of the other dialog controls.
//
// Parameters:
//      DIALOG_RECORD * lpdrDialog      Current dialog.
//
// Returns:
//      Nothing.
//************************************************************************
// 7/13/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DisableInocControls (DIALOG_RECORD *lpdrDialog)
{
    extern  CheckBoxRec     cbInocUse;
    extern  CheckBoxRec     cbInocFloppies;
 
    extern  CheckBoxRec     cbInocRepair;
    extern  CheckBoxRec     cbInocInoculate;
    extern  CheckBoxRec     cbInocStop;
    extern  CheckBoxRec     cbInocDelete;
    extern  CheckBoxRec     cbInocContinue;
    extern  CheckBoxRec     cbInocExclude;

    extern  ComboBoxRec     cxInocRespondNotInoc;
    extern  ComboBoxRec     cxInocRespondChanged;

    auto    BYTE            byEnableCheckBoxes;

    CheckBoxEnable(lpdrDialog,&cbInocFloppies, cbInocUse.value);

                                        // +1 for TSR, don't ask...
    byEnableCheckBoxes = ( (cxInocRespondNotInoc.value + 1 == INOC_PROMPT) ||
                       (cxInocRespondChanged.value + 1 == INOC_CHANGE_PROMPT));

    CheckBoxEnable(lpdrDialog, &cbInocRepair,   byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbInocInoculate,byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbInocStop,     byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbInocDelete,   byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbInocContinue, byEnableCheckBoxes);
    CheckBoxEnable(lpdrDialog, &cbInocExclude,  byEnableCheckBoxes);

} // End DisableInocControls()

//***************************************************************************
// TECheckInocPath()
// 
// Description:
//      Validates Inoculation path when exiting. The path must begin
//      with a backslash and conform to 8.3 rules (last part not
//      implemented yet). Called by the dialog manager.
//
//      TEInocPathFilter() does most of the validation to make
//      sure other bogus chars don't get in.
// 
// Return Value: 
//      FALSE           Bad path field.
//      TRUE            Path OK.
// 
//***************************************************************************
// 07/01/1995 BARRY Function Created.                                   
//***************************************************************************
UINT TECheckInocPath ( TERec * pTE )
{
    extern BYTE *       apszInvalidInocPathError[];
    BOOL                bOK = TRUE;

    if ( pTE->string[0] != '\\' ||
         FALSE == NameValidatePath(pTE->string))
        {
        CursorPush();                   // Dialog doesn't preserve cursor
        StdDlgError(apszInvalidInocPathError);
        CursorPop();
        bOK = FALSE;
        }

    return bOK;
}

//***************************************************************************
// TEInocPathFilter()
// 
// Description:
//      Validate character-by-character input to the inoc path field.
//      Only allow valid 8.3 path characters and don't allow any volume
//      names (path can't include a volume).
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: TEUpperPathCharsOnlyFilter (this fn was copied from there)
// 
//***************************************************************************
// 07/01/1995 BARRY Function Created.                                   
//***************************************************************************
UINT TEInocPathFilter (TERec *te, UINT *c)
{
                                        // Skip out on DBCS trail bytes.
    if (te->insert_pt > 0)
    {
        if (DBCSIsLeadByte (te->string[te->insert_pt-1]))
            return (EVENT_AVAIL);
    }

    if (*c == ' ' || *c == ':' )
        {
	Blip();
        return (EVENT_USED);
	}

    if (*c == '/')
	*c = '\\';

    if (!NameIsValidFileCharEx((BYTE) *c, FALSE, TRUE))
        {
	if (*c != '\\' && *c != '.')
	    {
	    Blip();
            return (EVENT_USED);
	    }
	}

                                        // Handles all double backslash cases
                                        // (including UNC)
    if (*c == '\\' &&
       ((te->insert_pt != 0 && *c == te->string[te->insert_pt-1]) ||
	*c == te->string[te->insert_pt]  ))
	{
	Blip();
        return (EVENT_USED);
	}

                                        // Don't lowercase trail bytes
    if (DBCSGetByteType(&(te->string[0]), 
                        &(te->string[te->insert_pt])) != _MBC_TRAIL)
        {
        *c = (BYTE) CharToLower((BYTE) *c); // Character looks OK. Make lower case
        }
    return (EVENT_AVAIL);
}




