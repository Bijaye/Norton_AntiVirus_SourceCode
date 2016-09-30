// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdpass.c_v   1.0   06 Feb 1997 20:50:22   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV Password options
//      configuration.
//
// Contains:
//      CPDialogProcPassword()
//      AskForPassword()
//
// See Also:
//      PASSSTR.CPP for strings and control structures.
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/cpld/VCS/navdpass.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:22   RFULLER
// Initial revision
// 
//    Rev 1.1   07 Aug 1996 16:49:04   JBRENNA
// Port in changes from DOSNAV environment
// 
// 
//    Rev 1.56   12 Jul 1996 12:25:30   MZAREMB
// Disabled functionality for navboot emulation.
// 
//    Rev 1.55   13 Jun 1996 12:02:52   JALLEE
// #if USE_PASSWORD -> #ifdef USE_PASSWORD.
// 
//    Rev 1.54   06 Jun 1995 14:12:44   BARRY
// ifdef entire file. Wasn't compiling because some help links have been removed
// 
//    Rev 1.53   28 Dec 1994 11:44:00   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   06 Dec 1994 21:34:30   DALLEE
    // Put back functions removed for mini-NAV.
    //
    //    Rev 1.1   30 Nov 1994 17:05:00   DALLEE
    // CVT1 script
    //
//    Rev 1.52   28 Dec 1994 11:34:32   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:58   DALLEE
    // From Nirvana
    //
    //    Rev 1.50   12 Oct 1993 16:47:02   BARRY
    // New CP_LEAVING handling to fix Alt-F4 bug
    //
    //    Rev 1.49   18 Sep 1993 15:00:08   DALLEE
    // When complaining no password is set, use StdDlgMessage() instead of
    // StdDlgOk() so only the OK button is displayed, not OK | Cancel.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "nlibcfg.h"
#include "passwd.h"

#include "navutil.h"
#include "navdprot.h"

#include "options.h"

#ifdef USE_PASSWORD                        // ENTIRE FILE

MODULE_NAME;

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define PASSWORD_OK_BUTTON      ( 0)
#define PASSWORD_CANCEL_BUTTON  ( 1)
#define PASSWORD_HELP_BUTTON    ( 2)

#define PASSWORD_RB_MAXIMUM     ( 0)
#define PASSWORD_RB_CUSTOM      ( 1)


//************************************************************************
// GLOBAL VARIABLES
//************************************************************************

HGLOBAL hPasswordList;
                                 // Pointer to array of BOOL's
                                 // corresponding to the password
                                 // protected items list entries.
BYTE    *lpbProtected;      	// TRUE = protected, FALSE = not.
                                 // corresponding to the password
BYTE    boolFirstTime;
BOOL    bUpDnKeysAreESC;
BOOL    bPasswordMatched;
BYTE    byPreviousProtectType;

extern TERec teNAVOldPassword ;         // PASSSTR.STR
extern TERec teNAVNewPassword ;         // PASSSTR.STR
extern TERec teNAVConfirmPassword ;     // PASSSTR.STR


                                      // protected items list entries.
//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************
extern  LPNAVOPTIONS    lpNavOptions;
extern  BOOL    bNavboot;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

WORD LOCAL PASCAL PasswordMemory (VOID);
VOID LOCAL PASCAL GreyText(DIALOG_RECORD *lpDialog, BOOL bGreyit);
UINT PasswordCP_ENTERING (VOID);
VOID LOCAL PASCAL PasswordAccept (VOID);
VOID LOCAL PASCAL NAVDlgChangePassword(VOID);
UINT STATIC NAVInquirePassFilter(DIALOG_RECORD *dr, UINT *pEvent);
UINT LOCAL PASCAL CheckPassword(struct TERec *te,DWORD dwCorrectCRC, BOOL bComplain);
UINT LOCAL PASCAL CheckNewPassword(struct TERec *te);
VOID LOCAL PASCAL DisablePasswordControls (DIALOG_RECORD *lpdrDialog);
UINT LOCAL DialogBox2Internal(DialogRec *dialog,
                              UINT (*EventFilter)(DIALOG_RECORD *dr, UINT *wItemNumber),
                              ...);


//************************************************************************
// CPDialogProcPassword()
//
// This is the callback routine for the Options CP Password Dialog.
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

UINT CPDialogProcPassword (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern  ButtonsRec      buPasswordSet;
    extern  CheckBoxRec     cbPasswordProtect;
    extern  ListRec         lrPasswordItems;
    extern  ButtonsRec 	    buOkCancelHelp;
    extern  RadioButtonsRec rbPasswordProtectType;

    extern  BYTE            byAcceptCPDialog;

    auto    WORD            wResult;
    auto    DWORD           dwListEntry;
    static  BOOL            bNormalProcess;


    switch (uMessage)
        {
        case CP_PRE_EVENT:
					// Event received, not yet processed
                                        // Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (uParameter != ESC);

                                        // Check for toggle list item with
                                        // the spacebar
            if ((uParameter == ' ') &&
                    (lpDialog->item->item == &lrPasswordItems))
                {
                                        // Set dwListEntry = active_entry
                dwListEntry = lrPasswordItems.active_entry;

                                        // If we have a valid active entry,
                                        // toggle its protected state
                if (dwListEntry < MAX_PROTECT_FEATURE)
                    {

                    lpbProtected[dwListEntry] = !lpbProtected[dwListEntry];
                    ListWrite(&lrPasswordItems, TRUE, TRUE);
                    return (EVENT_USED);
                    }
                }

            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:             // Event received and processed
                                        // Check combo boxes, TE's here?
	    if (bNormalProcess)
                DisablePasswordControls(lpDialog);

            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case
            if ((ButtonsRec *) lpDialog->item->item == &buPasswordSet)
                {
                NAVDlgChangePassword();
	        HyperHelpTopicSet(HELP_DLG_CPL_PASSWORD);
                return (EVENT_AVAIL);
                }

            switch (buOkCancelHelp.value)   // Current button selected
                {
                case PASSWORD_OK_BUTTON:
                                        // Data is saved on CP_LEAVING message
                    return (ACCEPT_DIALOG);

                case PASSWORD_CANCEL_BUTTON:
                                        // Set save flag FALSE
                    byAcceptCPDialog = FALSE;
                    return (ABORT_DIALOG);

                case PASSWORD_HELP_BUTTON:
                                        // Call password settings help here
		    HyperHelpDisplay();
                    return (EVENT_AVAIL);
                }

            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED

        case CP_PRE_ENTERING:           // Haven't entered dialog yet
            return ( PasswordMemory() );

        case CP_ENTERING:
					// Dialog box now active box
                                        // Load OptPassword info from database
                                        //  and initialize dialog data
	    HyperHelpTopicSet(HELP_DLG_CPL_PASSWORD);
            wResult = ProcessPassword(lpDialog, 
                                      PASSWORD_ALWAYS, 
                                      PasswordCP_ENTERING,
                                      &bNormalProcess);

	    if (bNormalProcess)
                DisablePasswordControls(lpDialog);

            return (wResult);

        case CP_LEAVING:
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.

            if ( uParameter != ACCEPT_DIALOG )
                byAcceptCPDialog = FALSE;

                                        // Make sure scroll bar is enabled
            ListScrollBarEnable(lpDialog, &lrPasswordItems, TRUE);
                                        // Dialog box is losing focus
            DialogSetFocus(lpDialog, &buOkCancelHelp);
                                        // If the dialog was not canceled,
                                        // save options if necessary.
            if (byAcceptCPDialog && !NeedPassword (PASSWORD_ALWAYS))
                PasswordAccept();
                                        // Free temp PASWOPT struct
            MemUnlock(hPasswordList, lpbProtected );
            MemFree(hPasswordList);
                                        // Reset password back to 0 state
            if (lpNavOptions->password.dwPassword == NO_PASSWORD)
	        lpNavOptions->password.dwPassword = 0;

            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);
        } // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcPassword()


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

//************************************************************************
//************************************************************************
//************************************************************************

WORD LOCAL PASCAL PasswordMemory (VOID)
{
    auto        LPBYTE          lpPasswordList;

                                        // Allocate space for the local
                                        // copy of the password options,
    hPasswordList  = MemAlloc(GHND, sizeof(BYTE) * MAX_PROTECT_FEATURE);
    lpPasswordList = MemLock(hPasswordList);
    if ((hPasswordList == NULL) || (lpPasswordList == NULL))
        return (ABORT_DIALOG);
                                        // Set the address of the protection
                                        // status array.
    lpbProtected = lpPasswordList;

    if (lpNavOptions->password.dwPassword == 0)
	lpNavOptions->password.dwPassword = NO_PASSWORD;

    return (EVENT_AVAIL);
}

//************************************************************************
// PasswordCP_ENTERING()
//
// This routine handles the CP_ENTERING message for the Password options
// Control Panel entry.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Continue w/ dialog
//      ABORT_DIALOG                    Cancel dialog - couldn't allocate
//                                       memory for the temp PASWOPT struct
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

UINT PasswordCP_ENTERING (VOID)
{
    extern      CheckBoxRec     cbPasswordProtect;
    extern      RadioButtonsRec rbPasswordProtectType;
    extern      ListRec         lrPasswordItems;


    UpdateCheckBox(&cbPasswordProtect, lpNavOptions->password.bProtect);

    RadioButtonsSetValue(&rbPasswordProtectType,
            (BYTE) (lpNavOptions->password.bMaxProtection == TRUE ?
            PASSWORD_RB_MAXIMUM : PASSWORD_RB_CUSTOM));

    MEMCPY(lpbProtected, 
           lpNavOptions->password.bProtectFeature, 
           MAX_PROTECT_FEATURE);

                                        // Set it to a bogus value, so it
                                        // will force it to rewrite, with
                                        // disabled state.
    byPreviousProtectType = (BYTE) RADIO_NO_SELECTION;

    return (EVENT_AVAIL);
} // End PasswordCP_ENTERING()


//************************************************************************
// PasswordAccept()
//
// This routine updates the temporary PASWOPT struct *lpOptPasw from the
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

VOID LOCAL PASCAL PasswordAccept (VOID)
{
    extern CheckBoxRec      cbPasswordProtect;
    extern RadioButtonsRec  rbPasswordProtectType;
    extern LPSTR            lpszNoPassword [];


    lpNavOptions->password.bProtect = cbPasswordProtect.value;

                                        // If some options are set to be
                                        // protected, complain if a password
                                        // has not been set.
    if ( (TRUE == lpNavOptions->password.bProtect) &&
         (NO_PASSWORD == lpNavOptions->password.dwPassword) )
        {
        lpNavOptions->password.bProtect = FALSE;
        StdDlgMessage(lpszNoPassword);
        }

    lpNavOptions->password.bMaxProtection =
            (rbPasswordProtectType.value == PASSWORD_RB_MAXIMUM);

    MEMCPY(lpNavOptions->password.bProtectFeature, 
           lpbProtected, 
           MAX_PROTECT_FEATURE);

} // End PasswordAccept()


//************************************************************************
// EnablePasswordListBox()
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 7/12/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL EnablePasswordListBox(DIALOG_RECORD *lpdrDialog, BYTE byEnable)
{
    extern      ListRec         lrPasswordItems;

                                        // Rewrite list box
    ListWrite(&lrPasswordItems, TRUE, FALSE);

    _WinSetAttr(
            lrPasswordItems.rect.top,
            lrPasswordItems.rect.left,
            lrPasswordItems.rect.right  - lrPasswordItems.rect.left + 1,
            lrPasswordItems.rect.bottom - lrPasswordItems.rect.top + 1,
            (BYTE) ((byEnable) ? NORMAL : GREY));

    ListScrollBarEnable(lpdrDialog, &lrPasswordItems, byEnable);
}

//************************************************************************
// DisablePasswordControls()
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 7/12/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DisablePasswordControls (DIALOG_RECORD *lpdrDialog)
{
    extern      ButtonsRec      buPasswordSet;
    extern      CheckBoxRec     cbPasswordProtect;
    extern      RadioButtonsRec rbPasswordProtectType;
    extern      ListRec         lrPasswordItems;

    auto        BYTE            byEnable;
    auto        BYTE            byPreviousState;


    WinSuspendUpdates();

    byPreviousState = !ButtonIsDisabled(&buPasswordSet, 0);

    byEnable = (BYTE) cbPasswordProtect.value;
                                        // Only toggle, if different
    if ( byPreviousState != byEnable )
        {
                                        // Enable/disable Password button
        ButtonEnable(&buPasswordSet, 0, byEnable);
                                        // Enable/disable checkbox
        RadioButtonsFieldEnable(lpdrDialog, &rbPasswordProtectType, byEnable);

                                        // Enable/disable List Box
        EnablePasswordListBox(lpdrDialog, 
                (BYTE) (byEnable && (BYTE)(rbPasswordProtectType.value != PASSWORD_RB_MAXIMUM)));
        }
                                        // We need to disable the list box
                                        // if maximum protection enabled.
    if ( byEnable && 
         ((BYTE) rbPasswordProtectType.value != byPreviousProtectType))
        {
        EnablePasswordListBox(lpdrDialog, 
                   (BYTE)(rbPasswordProtectType.value != PASSWORD_RB_MAXIMUM));
        byPreviousProtectType = (BYTE) rbPasswordProtectType.value;
        }

    WinResumeUpdates();

} // End DisablePasswordControls()


//************************************************************************
// PasswordListWriteEntry()
//
// This is the password protected items list write entry routine.
// Check marks are drawn preceding protected items.
//
// Parameters:
//      ListRec *  lplrList,
//      LPVOID     lpEntry,
//      int        wAttr,               Attribute to write with
//      DWORD      dwEntry,             Number of entry in list
//
// Returns:
//      Nothing
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

VOID PasswordListWriteEntry (
    ListRec *  lplrList,
    LPVOID     lpvEntry,
    int        iAttr,
    DWORD      dwEntry)
{
    extern      CheckBoxRec     cbPasswordProtect;
    extern      RadioButtonsRec rbPasswordProtectType;

    auto        BOOL            bCheckMark;


    ColorSaveAll();
    ColorSetAttr(iAttr);

    if (lpvEntry == NULL)
        {
        FastWriteSpaces(lplrList->column_width);
        }
    else
        {
                                        // If protected, write a check mark
        FastWriteSpaces(1);
        bCheckMark = (BOOL) (cbPasswordProtect.value && 
                        ((rbPasswordProtectType.value == PASSWORD_RB_MAXIMUM) ||
                         lpbProtected[dwEntry]));
        FastWriteChar((char)((bCheckMark) ? charTable.checkMark : ' '));
        FastWriteSpaces(2);

        FastWriteLine(*(char **)lpvEntry, 0, lplrList->column_width - 4);
        }

    ColorRestoreAll();

} // End PasswordListWriteEntry()


//************************************************************************
// PasswordListMouseProc()
//
// This routine handles mouse events in the password protected items
// list box.
//
// Parameters:
//      ListRec     *lplrList
//      DWORD       dwEntry             Entry number mouse is on
//      BYTERECT    *lpRect             Pointer to rectangle around entry
//      WORD        wButtons            Mouse buttons which are down
//      int         iMouseClicks        Number of mouse clicks (1 or 2)
//
// Returns:
//      1                               Selected entry with left button
//      2                               Selected entry with right button
//      3                               Canceled selection
//************************************************************************
// 3/15/93 DALLEE Function created.
//************************************************************************

int PasswordListMouseProc (
        ListRec     *lplrList,
        DWORD       dwEntry,
        BYTERECT    *lpRect,
        WORD        wButtons,
        int         iMouseClicks)
{
    static  DWORD   dwLastEntry = (DWORD) -1;
    auto    int     iResult;

    iResult = MouseGeneralTrackControl(lpRect);

                                        // If a selection was made
                                        // with the left button, toggle
                                        // the protection status of that item.
                                        // If this is a double click on
                                        // the same entry, ignore it, since
                                        // we toggled the state on the first
                                        // click.
    if ((iResult == MOUSE_LEFT) &&
            ((iMouseClicks != 2) || (dwEntry != dwLastEntry)))
        {
        lpbProtected[dwEntry] = !lpbProtected[dwEntry];
        ListWrite(lplrList, TRUE, TRUE);
        dwLastEntry = dwEntry;
        }

    else
        {
                                        // Reset our check var for double
        dwLastEntry = (DWORD) -1;       // clicks.

                                        // if iResult == 0, the mouse left
                                        // the entry before the button(s)
        if (iResult == 0)               // was/were released, so return Cancel.
            {
            iResult = 3;
            }
        }

    return (iResult);

} // End PasswordListMouseProc()



/*---------------04-22-93 04:37:04pm-SKURTZ--------
 * Localized version of StdDlgChangePassword
 *-------------------------------------------------*/


/*  */
/*----------------------------------------------------------------------*
 * CheckNewPassword() -- compare password with existing                 *
 *----------------------------------------------------------------------*/
UINT LOCAL PASCAL CheckNewPassword(struct TERec *te)
{
    extern      BYTE *  apszBadNewPassword[];

    STRLWR(te->string);
    if (STRICMP(te->string, teNAVNewPassword.string) != 0)
        {
        StdDlgError(apszBadNewPassword);
        return(FALSE);
        }

    return(TRUE);
}

/*----------------------------------------------------------------------*
 * CheckPassword() -- compare password with existing                    *
 * If bComplain set, then error dialog will appear - otherwise, silent. *
 *----------------------------------------------------------------------*/
UINT LOCAL PASCAL CheckPassword(TERec *te, DWORD dwCorrectCRC, BOOL bComplain)
{
    extern      BYTE *  apszBadPassword[];
    auto	DWORD   dwTestCRCval;



    if ( dwCorrectCRC != NO_PASSWORD )
        {
        STRLWR(te->string);
        EncodePassWord(te->string,&dwTestCRCval);

        if (dwTestCRCval != dwCorrectCRC)
            {
	    if (bComplain)
                StdDlgError(apszBadPassword);
            return(FALSE);
            }
        }

    return(TRUE);
}

/*----------------------------------------------------------------------*
 * ChangePasswordEventFilter() -- Filter events for user name dialog    *
 *----------------------------------------------------------------------*/

UINT STATIC ChangePasswordEventFilter(DIALOG_RECORD *dr, UINT *pEvent)
{
    extern      DialogRec           removePasswordDlg;
    extern 	ButtonsRec 	    buOkCancelHelp;       //  OPTSTR.CPP
    static      BOOL                bOKState;

    auto        UINT                wResult;
    auto        UINT                wItem;
    auto	UINT		    wOriginalEvent;



    wOriginalEvent = *pEvent;


    if (boolFirstTime)
        {
        boolFirstTime = FALSE;

        CursorPush();
        ButtonEnable (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
        bOKState = FALSE;
        CursorPop();

        return(EVENT_AVAIL);
        }

    wItem = dr->item_num;

    if (*pEvent == (UINT) MOUSE_EVENT)
        {


        wResult = DialogProcessEvent(dr, *pEvent);
        if ((wResult == ACCEPT_DIALOG &&
	     buOkCancelHelp.value == PASSWORD_CANCEL_BUTTON) ||
            (wResult == ABORT_DIALOG))
            {
            return(wResult);
            }


        if ((wResult == ACCEPT_DIALOG &&
	     buOkCancelHelp.value == PASSWORD_HELP_BUTTON))
            {
	    HyperHelpDisplay();
            DialogChangeField(dr, wItem);
	    ButtonSetDefault (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
            return(EVENT_USED);
            }

        }
    else
        {


	if (*pEvent == ESC)
	    return(EVENT_AVAIL);


        if (wItem != (UINT)DialogGetItemNum(dr, &buOkCancelHelp))
            {
            if (*pEvent == CURSOR_UP   ||
                *pEvent == CURSOR_LEFT ||
                *pEvent == CURSOR_RIGHT||
                *pEvent == SHIFT_TAB    )
                {
                *pEvent = 0;
                }
            }

        wResult = EVENT_AVAIL;
        }



    if (wResult == ACCEPT_DIALOG        ||
        *pEvent == CR                   ||
        *pEvent == TAB                  ||
        *pEvent == CURSOR_DOWN           )
        {
        if (wItem == (UINT)DialogGetItemNum(dr, &teNAVOldPassword))
            {
            if (CheckPassword(&teNAVOldPassword,lpNavOptions->password.dwPassword,TRUE))
                {
                CursorPush ();
                TextEditEnable (dr, &teNAVNewPassword, TRUE);
                TextEditEnable (dr, &teNAVOldPassword, FALSE);
                CursorPop ();

                DialogSetFocus(dr, &teNAVNewPassword);
                }
            else
                {
                *teNAVOldPassword.string = '\0';	// bad password. null it
                DialogChangeField(dr, wItem);	// move back focus
                }

            *pEvent = 0;
	    wOriginalEvent = NULL;
            wResult = EVENT_AVAIL;
            }
        else if (wItem == (UINT)DialogGetItemNum(dr, &teNAVNewPassword))
            {
            if (teNAVNewPassword.string[0] == EOS)
                {
                if (teNAVOldPassword.string[0] != EOS)
                    {
                    if (DialogAlertBox(&removePasswordDlg) == 0)
                        {
                        CursorPush ();
                        ButtonEnable (&buOkCancelHelp, PASSWORD_OK_BUTTON, TRUE);
                        ButtonSetDefault (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
                        CursorPop ();
			bPasswordMatched = FALSE;
                        wResult = ACCEPT_DIALOG;
                        }
                    else
                        {
                        *pEvent = 0;
			wOriginalEvent = NULL;
                        DialogChangeField(dr, wItem);
                        wResult = EVENT_AVAIL;
                        }
                    }
                else
                    {
                    ButtonSetDefault (&buOkCancelHelp, PASSWORD_CANCEL_BUTTON, FALSE);
                    wResult = EVENT_AVAIL;
                    }
                }
            else
                {
                STRLWR(teNAVNewPassword.string);

                CursorPush ();
                TextEditEnable (dr, &teNAVNewPassword, FALSE);
                TextEditEnable (dr, &teNAVConfirmPassword, TRUE);
                CursorPop ();

                DialogSetFocus(dr, &teNAVConfirmPassword);
                *pEvent = 0;
		wOriginalEvent = NULL;
                wResult = EVENT_AVAIL;
                }

            }
        else if (wItem == (UINT)DialogGetItemNum(dr, &teNAVConfirmPassword))
            {
            if (!CheckNewPassword(&teNAVConfirmPassword))
                {
                *teNAVConfirmPassword.string = EOS;

                *pEvent = 0;
                DialogChangeField(dr, wItem);
                wResult = EVENT_AVAIL;
                }
            else
                {
                ButtonEnable (&buOkCancelHelp, PASSWORD_OK_BUTTON, TRUE);
                ButtonSetDefault (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
                TextEditEnable (dr, &teNAVConfirmPassword, FALSE);
                }
            }
        }
    else
        {
        if (wItem == (UINT)DialogGetItemNum(dr, &teNAVConfirmPassword))
            {
            if ( teNAVConfirmPassword.string[0] )
                {
                ButtonEnable (&buOkCancelHelp, PASSWORD_OK_BUTTON, TRUE);
                ButtonSetDefault (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
                }
            else
                {
                ButtonEnable (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
                }
            }
        }



    if (wResult == EVENT_AVAIL &&
        wOriginalEvent != 0)
	{
	if (DialogProcessEvent(dr, wOriginalEvent) == ACCEPT_DIALOG)
	    {
	    if (buOkCancelHelp.value == PASSWORD_HELP_BUTTON)
	    	{
	        HyperHelpDisplay();
            	DialogChangeField(dr, wItem);
            	ButtonSetDefault (&buOkCancelHelp, PASSWORD_CANCEL_BUTTON, FALSE);
	    	return(EVENT_USED);
	    	}
	    return(EVENT_AVAIL);
	    }
	return(EVENT_USED);
	}


    return(wResult);
}


/*----------------------------------------------------------------------*
 * NAVDlgChangePassword() - allow user to enter or change the password        *
 *----------------------------------------------------------------------*/

VOID LOCAL PASCAL NAVDlgChangePassword(VOID)
{
    extern      DialogRec  	NAVChangePasswordDlg;
    extern      char    	changePasswordTitle[];
    extern      char    	NewPasswordTitle[];
    extern 	ButtonsRec 	buOkCancelHelp;       //  OPTSTR.CPP

    auto        int     	result;
    auto	ButtonsRec  	buTempButtonRec = buOkCancelHelp;
    auto        char            szOldPassword[PASSWORD_LEN + 1];
    auto        char            szNewPassword[PASSWORD_LEN + 1];
    auto        char            szConfirmPassword[PASSWORD_LEN + 1];


    teNAVOldPassword.string = szOldPassword;
    teNAVNewPassword.string = szNewPassword;
    teNAVConfirmPassword.string = szConfirmPassword;

    HyperHelpTopicSet(HELP_DLG_SET_PASSWORD);
    if (lpNavOptions->password.dwPassword == NO_PASSWORD)
        {
        teNAVOldPassword.flags |= TE_DISABLED;
        NAVChangePasswordDlg.title = NewPasswordTitle;
        }
    else
        {
        teNAVNewPassword.flags |= TE_DISABLED;
        NAVChangePasswordDlg.title = changePasswordTitle;
        }


    teNAVConfirmPassword.flags |= TE_DISABLED;

    teNAVConfirmPassword.string[0] = 
    teNAVNewPassword.string[0]     =
    teNAVOldPassword.string[0]     = EOS;

    result = DialogBox2Internal(&NAVChangePasswordDlg,
                        ChangePasswordEventFilter,
                        &teNAVOldPassword,
                        &teNAVNewPassword,
                        &teNAVConfirmPassword,
			&buOkCancelHelp
			);

    teNAVOldPassword.flags &= ~(TE_DISABLED);
    teNAVNewPassword.flags &= ~(TE_DISABLED);
    buOkCancelHelp = buTempButtonRec;		//restore button record.


    switch (result)
	{
	case CR:
        case PASSWORD_OK_BUTTON:
            if (CheckPassword(&teNAVNewPassword,lpNavOptions->password.dwPassword,FALSE))
            	{
                EncodePassWord(teNAVNewPassword.string, &lpNavOptions->password.dwPassword);
            	}
	    else
		{
	    	if (teNAVNewPassword.string[0] == EOS)
		    lpNavOptions->password.dwPassword = NO_PASSWORD;
                else
                    EncodePassWord(teNAVNewPassword.string, &lpNavOptions->password.dwPassword);
		}

	    bPasswordMatched = TRUE;
	    break;

        case PASSWORD_CANCEL_BUTTON:
	    break;

	case PASSWORD_HELP_BUTTON:
	    HyperHelpDisplay();
	    break;

	}
}



/**************************************************************************
 * NAVInquirePassFilter
 *
 * DESCRIPTION  : filter events for Password inquiry dialog Mostly just
 * 		  for the help screen.
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 04-27-93 at 06:24:26pm Function created
 ************************************************************************* */
UINT STATIC NAVInquirePassFilter(DIALOG_RECORD *dr, UINT *pEvent)
{
    extern 	ButtonsRec 	    buOkCancelHelp;       //  OPTSTR.CPP
    extern      UINT 		    wCPLExtrnlKeyStroke;
    auto        UINT                wResult;
    auto        UINT                wItem;



    wItem = dr->item_num;


    if (bUpDnKeysAreESC &&
    	wItem == (UINT)DialogGetItemNum(dr, &teNAVConfirmPassword) &&
	(*pEvent == CURSOR_UP 	||
	 *pEvent == CURSOR_DOWN ||
	 *pEvent == HOME 	||
	 *pEvent == END		||
	 *pEvent == PG_UP	||
	 *pEvent == PG_DN))
	{
	wCPLExtrnlKeyStroke = *pEvent;
	*pEvent = ESC;
	return(EVENT_AVAIL);
	}

    if (teNAVConfirmPassword.string[0] == EOS)
	{
	if (!ButtonIsDisabled(&buOkCancelHelp, PASSWORD_OK_BUTTON))
	    {
            ButtonEnable (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
            ButtonSetDefault (&buOkCancelHelp, PASSWORD_CANCEL_BUTTON, FALSE);
	    }
	}
    else
	{
	if (ButtonIsDisabled(&buOkCancelHelp, PASSWORD_OK_BUTTON))
	    {
            ButtonEnable (&buOkCancelHelp, PASSWORD_OK_BUTTON, TRUE);
            ButtonSetDefault (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
	    }
	}



    wResult = DialogProcessEvent(dr, *pEvent);


    if ((wResult == ACCEPT_DIALOG &&
	buOkCancelHelp.value == PASSWORD_HELP_BUTTON))
        {
	HyperHelpDisplay();
        DialogChangeField(dr, wItem);
	ButtonSetDefault (&buOkCancelHelp, PASSWORD_OK_BUTTON, FALSE);
        return(EVENT_USED);
        }


    return(wResult);

}


/**************************************************************************
 * ProcessPassword()
 *
 * DESCRIPTION  : 
 *
 *************************************************************************
 ************************************************************************* */
UINT PASCAL ProcessPassword(DIALOG_RECORD *lpDialog, 
                            WORD wFeature, 
                            CPLENTERPROC fnCPLEntering,
                            LPBOOL lpbNormalProcess)
{
    auto        UINT            uResult = EVENT_AVAIL;
    auto        BOOL            bNormalProcess = FALSE;
    auto        BYTE	        byPWResult;


    if ( NeedPassword(wFeature) )
        {
        WinSuspendUpdates();

        DialogClearAndDisableControls(lpDialog);
        
        GreyText(lpDialog, TRUE);

        WinResumeUpdates();

        byPWResult = (BYTE)AskForPassword(wFeature);

        if ( byPWResult )
            {
            GreyText(lpDialog, FALSE);
            bNormalProcess = TRUE;
            }
        }
    else
        {
        bNormalProcess = TRUE;
        }

    if ( bNormalProcess )
        {
        WinSuspendUpdates();
        DialogEnableControls(lpDialog, TRUE);
        uResult = (*fnCPLEntering)();
        WinResumeUpdates();
        }

    *lpbNormalProcess = bNormalProcess;
    return (uResult);
}


/**************************************************************************
 * ResetPassword
 *
 * DESCRIPTION  : Resets the global password state to the "I don't have it yet"
 *                state.
 *
 *************************************************************************
 ************************************************************************* */
VOID PASCAL ResetPassword(VOID)
{
    bPasswordMatched = FALSE;
}


/**************************************************************************
 * NeedPassword
 *
 * DESCRIPTION  : Do we need to prompt for a password?
 *
 * IN      : Which domain listed in lpszPasswordItems you are about to enter.
 *
 * RETURN VALUES: TRUE if :
 *                -We need to prompt for a password.
 *
 * 		  FALSE if:
 *                -We don't need to prompt for a password.
 *************************************************************************
 ************************************************************************* */
BOOL PASCAL NeedPassword(WORD wFeature)
{
    auto        BOOL            bValue;


    /*---------------04-28-93 02:28:26pm-SKURTZ--------
     * Have we passed this way before, or is no password
     * set up?
     * If password protection is turned off, return true
     * all the time.
     *-------------------------------------------------*/

    if (bPasswordMatched ||
        !navOptions.password.bProtect ||
        (navOptions.password.dwPassword == 0) )
	{
        bValue = FALSE;
        }
    else
        {
        /*---------------04-28-93 02:28:58pm-SKURTZ--------
        * Get protection state of this feature
        *-------------------------------------------------*/
        bValue = (wFeature == PASSWORD_ALWAYS) ? TRUE : 
                                navOptions.password.bProtectFeature[wFeature];

        /*---------------04-28-93 03:35:59pm-SKURTZ--------
        * if Max protection turned on. Then ALL domains are
        * automatically tested.
        *-------------------------------------------------*/
        if (navOptions.password.bMaxProtection)
	    bValue = TRUE;
        }

    return (bValue);
}



/**************************************************************************
 * AskForPassword
 *
 * DESCRIPTION  : Ask for password before entering a domain.
 *
 * IN      : Which domain listed in lpszPasswordItems you are about to enter.
 *
 * RETURN VALUES: TRUE if :
 * 		  -This domain not password protected.
 *                -password successfully entered
 * 		  -user has already been in this domain.
 *
 * 		  FALSE if:
 * 		  -CANCEL entered
 * 		  -Incorrect password entered.
 *************************************************************************
 * SKURTZ : 04-27-93 at 05:39:53pm Function created
 ************************************************************************* */
BOOL PASCAL AskForPassword(WORD wFeature)
{

    extern      NAVOPTIONS	navOptions;
    extern      ButtonsRec 	buOkCancelHelp;       //  OPTSTR.CPP
    extern      DialogRec 	NAVInquirePasswordDlg;
    auto        BOOL		bValue;
    auto        WORD		wResult;
    auto        ButtonsRec  	buTempButtonRec = buOkCancelHelp;
    auto        BOOL		bDone = FALSE;
    auto        char            szConfirmPassword[PASSWORD_LEN + 1];
    auto        UINT            i;
    auto        WORD            wOldHelp;
    static      WORD            CPL_ENTRY[] =
                        {
                        PASSWORD_VIEWSCANOPT,
                        PASSWORD_VIEWTSROPT,
                        PASSWORD_VIEWALERTOPT,
                        PASSWORD_VIEWEXCLOPT,
                        PASSWORD_VIEWACTIVITYOPT,
                        PASSWORD_VIEWINOCOPT,
                        PASSWORD_VIEWGENOPT,
                        PASSWORD_ALWAYS,
                        0
                        };

    if ( bNavboot )                     // Functionality disabled in NAVBOOT emulation
        return (TRUE);
                                        
                                        // Do we need to prompt for a password?
    if ( !NeedPassword(wFeature) )
        return(TRUE);

    bUpDnKeysAreESC = FALSE;
    for (i = 0; CPL_ENTRY[i]; i++)
        if ( wFeature == CPL_ENTRY[i] )
            {
            bUpDnKeysAreESC = TRUE;
            break;
            }

    /*---------------04-28-93 02:28:26pm-SKURTZ--------
     * Have we passed this way before, or is no password
     * set up?
     *-------------------------------------------------*/

    if (navOptions.password.dwPassword == 0)
	navOptions.password.dwPassword = NO_PASSWORD;

    teNAVConfirmPassword.string = szConfirmPassword;
    teNAVConfirmPassword.flags = TE_NO_ECHO | TE_NO_STRIP;

    wOldHelp = HyperHelpTopicGet();
    bValue = TRUE;
    while (!bDone)
	{
        HyperHelpTopicSet(HELP_DLG_VERIFY_PASSWORD);

        *teNAVConfirmPassword.string = EOS;

    	wResult = DialogBox2Internal(&NAVInquirePasswordDlg,
			NAVInquirePassFilter,
                        &teNAVConfirmPassword,
			&buOkCancelHelp);

	switch (wResult)
	    {
	    case PASSWORD_OK_BUTTON :
	    case CR:
    	    	if (CheckPassword(&teNAVConfirmPassword,navOptions.password.dwPassword,TRUE))
		    {
		    bPasswordMatched = TRUE;  // indicate we've been here
		    bValue = TRUE;
		    bDone = TRUE;
		    }
		break;


	    case PASSWORD_CANCEL_BUTTON :
	    case ABORT_DIALOG :
	    case ESC :
	    	bValue = FALSE;
	    	bDone = TRUE;
		break;

	    case PASSWORD_HELP_BUTTON:
		HyperHelpDisplay();
		break;

	    }
	}


    buOkCancelHelp = buTempButtonRec;		//restore record.

    if (navOptions.password.dwPassword == NO_PASSWORD)
	navOptions.password.dwPassword = 0;

    HyperHelpTopicSet(wOldHelp);

    return(bValue);
}




UINT LOCAL DialogBox2Internal(DialogRec *dialog,
                              UINT EventFilter (DIALOG_RECORD *dr, UINT *wItemNumber),
                              int args)
{
    auto     COLORS            *dialogColor;
    auto     UINT               response;

    dialogColor = DialogGetColor ();
    response = DialogBoxInternal (dialog,
    				  dialogColor,
                   		(DialogFilter)EventFilter,
				(char **)&args);
    return (response);
}



/**************************************************************************
 * GreyText
 *
 * DESCRIPTION  : Greys the entire dialog, by greying out 2 rectangles.
 *
 *************************************************************************
 * SKURTZ : 05-05-93 at 10:15:13am Function created
 ************************************************************************* */

VOID LOCAL PASCAL GreyText(DIALOG_RECORD *lpDialog, BOOL bGreyIt)
{
    auto        BYTE            byAttr;
    auto        BYTE            byButtonLeft, byButtonBottom;
    auto        DIALOG_ITEM	*item;
    auto        UINT            i;
    auto        ButtonsRec      *b;
    auto        WORD            wNumButtons;
    auto        ListRec         *lr;
    auto        BYTE            byListRight;



    byAttr = (bGreyIt) ? GREY : NORMAL;
                                        // Loop through all controls, looking
                                        // for multi-button set.  This is the
                                        // one we base our rectangles on.
    for (i = 0, item = lpDialog->first_item; i < (UINT) lpDialog->num_items; i++, item++)
        {
        if ( item->type == BUTTON_FIELD )
            {
                                    // if only 1 button
            b = (ButtonsRec *) (item->item);
            wNumButtons = count_buttons(b->strings);
                                        // Must have button set with at least
                                        // 3 or more buttons
            if ( wNumButtons >= 3 )
                {
                byButtonLeft   = b->col;
                byButtonBottom = (BYTE) (b->row + (wNumButtons * 2));
                break;
                }
            }
        }


    lr = (ListRec *) lpDialog->first_item->item;
    byListRight = lr->row + lr->rect.right;

                                        // Rectangle region #1
    _WinSetAttr(
                (BYTE)(lr->row - 1),
                byListRight,
                byButtonLeft - byListRight,
                lpDialog->saveRect.bottom - lr->row,
		byAttr);

                                        // Rectangle region #2
    _WinSetAttr(
                byButtonBottom,
                byButtonLeft,
                lpDialog->saveRect.right  - byButtonLeft - 4,
                lpDialog->saveRect.bottom - byButtonBottom - 1,
		byAttr);
}


/*----------------------------------------------------------------------*
 * This procedure enables/disables all controls in a dialog.            *
 *----------------------------------------------------------------------*/
VOID PASCAL DialogEnableControls (DIALOG_RECORD	*lpDialog, BYTE bEnable)
{
    auto        DIALOG_ITEM	*item;
    auto        UINT            i;

                                        // Loop through all controls in the
                                        // dialog, enabling/disabling them
    for (i = 0, item = lpDialog->first_item; i < (UINT) lpDialog->num_items; i++, item++)
        {
                                        // Skip over first control, if specified
                                        // (MAJOR HACK!  Needed for CPLs)
        if ( i == 0 )
            continue;

        switch (item->type)
	    {
	    case TE_FIELD:
	    case SPECIAL_TE_FIELD:
	        TextEditEnable(lpDialog, (TERec *)(item->item), bEnable);
	        break;

	    case BUTTON_FIELD:
                {
                auto    ButtonsRec      *b;
                auto    WORD            wNumButtons;
                auto    WORD            j;

                                        // Disable all buttons, except
                                        // OK/CANCEL/HELP.  Also, disable
                                        // if only 1 button
                b = (ButtonsRec *) (item->item);
                wNumButtons = count_buttons(b->strings);
                for (j = 0; j < wNumButtons; j++)
                    {
                    if ( (wNumButtons == 1) ||
                         (j >= 2 && j < (wNumButtons - 1)) )
	                ButtonEnable(b, j, bEnable);
                    }
                }
	        break;

	    case CHECK_FIELD :
	        CheckBoxEnable (lpDialog, (CheckBoxRec *)(item->item), bEnable);
                break;

	    case RADIO_FIELD :
	        RadioButtonsFieldEnable(lpDialog, (RadioButtonsRec *)(item->item), bEnable);
                break;

	    case LIST_FIELD :
	        ListScrollBarEnable(lpDialog, (ListRec *)(item->item), bEnable);
	        break;

	    case COMBO_BOX_FIELD :
                ComboBoxEnable (lpDialog, (ComboBoxRec *)(item->item), bEnable);
	        break;
	    }
        }
}


/*----------------------------------------------------------------------*
 * This procedure enables/disables all controls in a dialog.            *
 *----------------------------------------------------------------------*/
VOID PASCAL DialogClearAndDisableControls (DIALOG_RECORD *lpDialog)
{
    auto        DIALOG_ITEM	*item;
    auto        UINT            i;

                                        // Loop through all controls in the
                                        // dialog, clearing them.
    for (i = 0, item = lpDialog->first_item; i < (UINT) lpDialog->num_items; i++, item++)
        {
                                        // Skip over first control, if specified
                                        // (MAJOR HACK!  Needed for CPLs)
        if ( i == 0 )
            continue;

        switch (item->type)
	    {
	    case TE_FIELD:
	    case SPECIAL_TE_FIELD:
	        ((TERec *)(item->item))->string[0] = EOS;
	        break;

	    case CHECK_FIELD :
	        ((CheckBoxRec *)(item->item))->value = 0;
                break;

	    case RADIO_FIELD :
	        ((RadioButtonsRec *)(item->item))->value = (BYTE) RADIO_NO_SELECTION;
                break;

	    case COMBO_BOX_FIELD :
                ((ComboBoxRec *)(item->item))->value = (DWORD) COMBO_NO_SELECTION;
	        break;

	    case LIST_FIELD :
	    case BUTTON_FIELD:
	        break;

	    }
        }
                                        // Disable the controls
    DialogEnableControls (lpDialog, FALSE);
}

#endif                                  // USE_PASSWORD (ENTIRE FILE)
