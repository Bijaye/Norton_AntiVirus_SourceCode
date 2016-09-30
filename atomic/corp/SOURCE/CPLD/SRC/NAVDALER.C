// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdaler.c_v   1.0   06 Feb 1997 20:50:22   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV Alert options
//      configuration
//
// Contains:
//      CPDialogProcAlert()
//
// See Also:
//      ALERSTR.CPP for strings and control structures.
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/cpld/VCS/navdaler.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:22   RFULLER
// Initial revision
// 
//    Rev 1.3   12 Nov 1996 14:36:34   MKEATIN
// Don't allow the "Remove Alert.." dialog to be set to 0.  If the user does 
// enter 0, force it to 1.
// 
//    Rev 1.2   19 Sep 1996 16:34:26   JALLEE
// Fixed NAV/NAVBOOT bug, NAVBOOT gpf'd when closing the dialog by return button.
// Was attempting to use NAV's buttons.
// 
//    Rev 1.1   07 Aug 1996 16:49:06   JBRENNA
// Port in changes from DOSNAV environment
// 
// 
//    Rev 1.60   07 Aug 1996 08:28:48   MZAREMB
// Re-added the multiple-personality changes required by this module for
// NAV/NAVBOOT shared code.
// 
//    Rev 1.59   02 Aug 1996 13:46:52   MZAREMB
// Limited the size of the STRCPY as the program was GPF'ing when a username 
// which was typed in was illegal and extremely large.
// 
//    Rev 1.58   16 Jul 1996 11:29:10   JALLEE
// Fixed GPF in network alerts,  CPLD now works as in windows.  We allocate an
// extra item in the network alerting user list and make sure it is always zeroed.
// This is clumsy, but I want do not want to change any code shared between,
// DX and WIN at this time.  A more elegant fix might involve passing more info
// (num of elements in net user list) to NetAlert() in Navxutil.
// 
//    Rev 1.54   13 Jun 1996 12:02:14   JALLEE
// #if USE_PASSWORD -> #ifdef USE_PASSWORD.
// 
//    Rev 1.53   13 Jun 1996 10:32:14   JALLEE
// buAlertButtons name change to buNavAlertButtons
// 
//    Rev 1.52   18 Oct 1995 18:46:36   MKEATIN
// Made static variable bNormalProcess available for USE_PASSWORD
// 
//    Rev 1.51   28 Dec 1994 11:43:54   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 14:00:28   DALLEE
    // Removed controls no longer in NAVBOOT.EXE
    // Removed password protection for options.
    //
    //    Rev 1.1   06 Dec 1994 20:48:18   DALLEE
    // CVT1 script.
    //
//    Rev 1.50   28 Dec 1994 11:34:24   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:54   DALLEE
    // From Nirvana
    //
    //    Rev 1.47   12 Oct 1993 16:47:04   BARRY
    // New CP_LEAVING handling to fix Alt-F4 bug
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "dosnet.h"

#define BINDERY_OBJECT_TIME FAKE_BINDERY_OBJECT_TIME
#include "navutil.h"
#undef BINDERY_OBJECT_TIME

#include "navdprot.h"

#include "options.h"
#include "common.h"

MODULE_NAME;

extern  BOOL    bNavboot;

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

enum {  ALERT_OK_BUTTON = 0,
        ALERT_CANCEL_BUTTON,
#ifdef USE_NETWORKALERTS   //&?
        ALERT_OTHERS_BUTTON,
#endif
        ALERT_HELP_BUTTON   };

enum {  ALERT_OTHERS_OK_BUTTON = 0,
        ALERT_OTHERS_CANCEL_BUTTON,
        ALERT_OTHERS_HELP_BUTTON    };

#define ALT_RIGHT_ARROW		413


//************************************************************************
// STATIC VARIABLES
//************************************************************************
static  HGLOBAL         hBuffers;
static  BOOL            bNormalProcess;

#ifdef USE_NETWORKALERTS   //&?
 static  char            SZ_SEPS[] = "/";
 static  char            SZ_GROUP_START[] = "[";
 static  char            SZ_GROUP_END[]   = "]";
 static  BOOL            bNetUserMemIsMaxSize;
#endif  // USE_NETWORKALERTS

//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************
extern  LPNAVOPTIONS    lpNavOptions;


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

UINT AlertCP_ENTERING (VOID);
VOID LOCAL PASCAL AlertAccept (VOID);
VOID LOCAL PASCAL DisableAlertControls (DIALOG_RECORD *lpdrDialog);
UINT LOCAL PASCAL AlertMemory (VOID);

#ifdef USE_NETWORKALERTS   //&?
 VOID LOCAL PASCAL OthersDialog (VOID);
 VOID LOCAL PASCAL OthersInit (VOID);
 VOID LOCAL PASCAL OthersAccept (VOID);
 int  STATIC       OthersButtonsProc (DIALOG_RECORD *lpdrDialog);

 VOID LOCAL PASCAL NetworkUsersDlg (VOID);
 VOID LOCAL PASCAL NetworkUsersInit (VOID);
 VOID LOCAL PASCAL NetworkUsersAccept (VOID);
 int  STATIC       NetworkUsersButtonsProc (DIALOG_RECORD *lpdrDialog);

 VOID LOCAL PASCAL NetUserToSelectedObjects(NETSELECTOBJECTSDOS * lprSelectObjs,
					   LPNETUSER lpNetUser);

 VOID LOCAL PASCAL SelectedObjectsToNetUser(NETSELECTOBJECTSDOS * lprSelectObjs,
					   LPNETUSER lpNetUser);

 VOID LOCAL PASCAL BuildAlertNetUsrString(VOID);
 UINT TEAlertNetworkUserValidate(TERec *te, UINT *c);

 VOID LOCAL PASCAL StringToNetUsers (LPSTR lpszNetUsers, LPNETUSER lpNetUser);
 VOID LOCAL PASCAL StringToSingleNetUser(char * szString,LPNETUSERITEM lpNetUserItem);
 BOOL LOCAL PASCAL AllocReAllocMemory(LPNAVOPTIONS lpNavOptions);
#endif  // USE_NETWORKALERTS

//************************************************************************
// CPDialogProcAlert()
//
// This is the callback routine for the Options CP Alert Dialog.
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

UINT CPDialogProcAlert (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern  	ButtonsRec      buEllipses;
    extern  	BYTE            byAcceptCPDialog;

    extern  	CheckBoxRec     cbAlertDisplayMessage;
    extern  	CheckBoxRec     cbAlertAudible;
    extern  	CheckBoxRec     cbAlertNetworkUsers;
    extern  	CheckBoxRec     cbAlertNetworkConsole;
    extern  	CheckBoxRec     cbAlertNLM;
    extern  	CheckBoxRec     cbAlertRemoveDialog;

    extern  	TERec           teAlertMessage;
    extern  	TERec           teAlertNetworkUsers;
    extern  	TERec           teAlertRemoveAfter;

    extern      ButtonsRec      buNavAlertButtons;
    extern      ButtonsRec      buNavBootAlertButtons;

    auto        WORD            wResult;
    auto        WORD            wOldHelp;
    auto        WORD            wButtonValue;

    switch (uMessage)
        {
        case CP_PRE_EVENT:              // Event received, not yet processed
#ifdef USE_NETWORKALERTS   //&?
            if ( !bNavboot ) 
                {
                switch (uParameter)
                    {
                    case ALT_RIGHT_ARROW :
                        if (lpDialog->item->item == &teAlertNetworkUsers)
                            {
                            NetworkUsersDlg();
                            DialogSetFocus(lpDialog, &buEllipses);
                            return(EVENT_USED);
                            }
                        break;
                    }
                }
#endif
                                        // Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (uParameter != ESC);
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:             // Should we disable any alert dialog
                                        // controls?
            DisableAlertControls(lpDialog);
            return (CONTINUE_DIALOG);

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case
#ifdef USE_NETWORKALERTS   //&?
            if ( !bNavboot ) 
                {
                if ((ButtonsRec *) lpDialog->item->item == &buEllipses)
                    {
                    NetworkUsersDlg();
                    return (EVENT_AVAIL);
                    }
                }    
#endif

            if (bNavboot)
                wButtonValue = buNavBootAlertButtons.value;
            else
                wButtonValue = buNavAlertButtons.value;

            switch (wButtonValue)   // Current button selected
                {
                case ALERT_OK_BUTTON:
                                        // Data save is necessary on
                                        // CP_LEAVING
                    return (ACCEPT_DIALOG);

                case ALERT_CANCEL_BUTTON:
                                        // Set save flag FALSE
                    byAcceptCPDialog = FALSE;
                    return (ABORT_DIALOG);

#ifdef USE_NETWORKALERTS   //&?
                case ALERT_OTHERS_BUTTON:
                                        // Call alert others options dialog
                    if ( !bNavboot ) 
                        OthersDialog();
                    else                // this is ugly, but the program is
                                        //  maintaining a "dual" personality
                                        //  and this is the work-around.
                        HyperHelpDisplay();
                    return (EVENT_AVAIL);
#endif

                case ALERT_HELP_BUTTON:
                                        // Call alert settings help here
		    HyperHelpDisplay();
                    return (EVENT_AVAIL);
                }

            return (EVENT_AVAIL);       // Default for CP_BUTTON_PRESSED

        case CP_PRE_ENTERING:           // Haven't entered dialog yet
            return ( AlertMemory() );

        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptAlert info from database
                                        //  and initialize dialog data
            if (bNavboot)
                wOldHelp = HyperHelpTopicSet(HELP_DLG_CPL_ALERTS_NB);
            else
                wOldHelp = HyperHelpTopicSet(HELP_DLG_CPL_ALERTS);
#ifndef USE_PASSWORD    //&?
            wResult = AlertCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog,
                                      PASSWORD_VIEWALERTOPT,
                                      AlertCP_ENTERING,
                                      &bNormalProcess);
            if ( bNormalProcess )
#endif  //&? Careful to keep if() with next block.
                {
#ifdef USE_NETWORKALERTS
                if ( !bNavboot ) 
                    BuildAlertNetUsrString();
#endif
                DisableAlertControls(lpDialog);
                }
            return (wResult);

        case CP_LEAVING:                // Dialog box is losing focus
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.

            if ( uParameter != ACCEPT_DIALOG )
                byAcceptCPDialog = FALSE;

            if ( bNavboot )
                DialogSetFocus(lpDialog, &buNavBootAlertButtons);
            else
                DialogSetFocus(lpDialog, &buNavAlertButtons);
                                        // If the dialog was not canceled,
                                        // save options if necessary.
            if ( byAcceptCPDialog
#ifdef USE_PASSWORD    //&?
                 && !NeedPassword(PASSWORD_VIEWALERTOPT)
#endif
               )
                AlertAccept();
                                        // Free temp ALEROPT struct
            MemRelease(hBuffers);
            MemFree(hBuffers);

#ifdef USE_NETWORKALERTS   //&?
                                        // Be nice and enable the Ellipses
                                        // button since it's used by other
                                        // dialogs.
            if ( !bNavboot ) 
                ButtonEnable(&buEllipses, 0, TRUE);
#endif
            HyperHelpTopicSet(wOldHelp);

            return (byAcceptCPDialog ? ACCEPT_DIALOG : ABORT_DIALOG);
        } // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcAlert()


//************************************************************************
// LOCAL FUNCTIONS
//************************************************************************

UINT LOCAL PASCAL AlertMemory (VOID)
{
    extern TERec        teAlertMessage;
    extern TERec        teAlertNetworkUsers;
    extern TERec        teAlertRemoveAfter;
    auto   LPBYTE       lpBuffers;
    auto   WORD         wMemTotal;

    wMemTotal = teAlertMessage.max_len + 1;

    if ( !bNavboot ) 
        wMemTotal += teAlertNetworkUsers.max_len + 1;

    hBuffers  = MemAlloc(GHND,wMemTotal);
    
    lpBuffers = MemLock(hBuffers);
    if ((hBuffers == NULL) || (lpBuffers == NULL))
        return (ABORT_DIALOG);
                                        // Assign TERec string pointers
                                        // to the buffer we just locked.
    teAlertMessage.string      = lpBuffers;
#ifdef USE_NETWORKALERTS   //&?
    if ( !bNavboot ) 
        {
        teAlertNetworkUsers.string = teAlertMessage.string +
                                 teAlertMessage.max_len + 1;
        }
#endif
    return(EVENT_AVAIL);
}


//************************************************************************
// AlertCP_ENTERING()
//
// This routine handles the CP_ENTERING message for the Options
// Alert Control Panel entry.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Continue w/ dialog
//      ABORT_DIALOG                    Cancel dialog - couldn't allocate
//                                       memory for the temp ALERTOPT struct
//************************************************************************
// 3/12/93 DALLEE Function created.
//************************************************************************

UINT AlertCP_ENTERING (VOID)
{
    extern CheckBoxRec  cbAlertDisplayMessage;
    extern CheckBoxRec  cbAlertAudible;
    extern CheckBoxRec  cbAlertNetworkUsers;
    extern CheckBoxRec  cbAlertNetworkConsole;
    extern CheckBoxRec  cbAlertNLM;
    extern CheckBoxRec  cbAlertRemoveDialog;
    extern TERec        teAlertMessage;
    extern TERec        teAlertNetworkUsers;
    extern TERec        teAlertRemoveAfter;

    extern CheckBoxRec   cbAlertOthersKnownVirus;
    extern CheckBoxRec   cbAlertOthersUnknownVirus;
    extern CheckBoxRec   cbAlertOthersInocChange;
    extern CheckBoxRec   cbAlertOthersVirusLike;
    extern CheckBoxRec   cbAlertOthersStartEnd;
    extern CheckBoxRec   cbAlertOthersVirusList;

					// Enough spaces to hold a converted
    auto   char         szSeconds[7];   // UINT value


    UpdateCheckBox(&cbAlertDisplayMessage,  lpNavOptions->alert.bDispAlertMsg);
    UpdateCheckBox(&cbAlertAudible,         lpNavOptions->alert.bBeep);
    UpdateCheckBox(&cbAlertRemoveDialog,    lpNavOptions->alert.bRemoveAfter);

    NAVDLoadTextEdit(&teAlertMessage,       lpNavOptions->alert.szAlertMsg);

                                        // alert.uSeconds should be 
                                        // at least 1
    lpNavOptions->alert.uSeconds = lpNavOptions->alert.uSeconds ?
                                   lpNavOptions->alert.uSeconds : 1;

                                        // Convert with base 10 and no
                                        // thousands separator
    NAVDLoadTextEdit(&teAlertRemoveAfter,
            _ConvertWordToString(lpNavOptions->alert.uSeconds,szSeconds,10,0));


    _TextEditWrite(&teAlertMessage, TRUE, FALSE);
    _TextEditWrite(&teAlertRemoveAfter, TRUE, FALSE);

#ifdef USE_NETWORKALERTS   //&?
    if ( bNavboot ) 
        return (EVENT_AVAIL);
        
    UpdateCheckBox(&cbAlertNetworkUsers,    lpNavOptions->alert.bAlertNetUsr);
    UpdateCheckBox(&cbAlertNetworkConsole,  lpNavOptions->alert.bAlertNetCon);
    UpdateCheckBox(&cbAlertNLM,             lpNavOptions->alert.bAlertNLM);

    cbAlertOthersKnownVirus.value   = lpNavOptions->alert.bKnown;
    cbAlertOthersUnknownVirus.value = lpNavOptions->alert.bUnknown;
    cbAlertOthersInocChange.value   = lpNavOptions->alert.bInoculation;
    cbAlertOthersVirusLike.value    = lpNavOptions->alert.bVirusLike;
    cbAlertOthersStartEnd.value     = lpNavOptions->alert.bStartEnd;
    cbAlertOthersVirusList.value    = lpNavOptions->alert.bVirListChange;

    _TextEditWrite(&teAlertNetworkUsers, TRUE, FALSE);
#endif  // USE_NETWORKALERTS

    return (EVENT_AVAIL);
} // End AlertCP_ENTERING()


//************************************************************************
// AlertAccept()
//
// This routine updates the temporary ALERTOPT struct *lpOptAlert from the
// dialog controls, and if changes were made, saves them to the .INI file
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/12/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL AlertAccept (VOID)
{
    extern CheckBoxRec  cbAlertDisplayMessage;
    extern CheckBoxRec  cbAlertAudible;
    extern CheckBoxRec  cbAlertNetworkUsers;
    extern CheckBoxRec  cbAlertNetworkConsole;
    extern CheckBoxRec  cbAlertNLM;
    extern CheckBoxRec  cbAlertRemoveDialog;
    extern TERec        teAlertMessage;
    extern TERec        teAlertNetworkUsers;
    extern TERec        teAlertRemoveAfter;
    extern LPNAVOPTIONS lpNavOptions;

    extern CheckBoxRec   cbAlertOthersKnownVirus;
    extern CheckBoxRec   cbAlertOthersUnknownVirus;
    extern CheckBoxRec   cbAlertOthersInocChange;
    extern CheckBoxRec   cbAlertOthersVirusLike;
    extern CheckBoxRec   cbAlertOthersStartEnd;
    extern CheckBoxRec   cbAlertOthersVirusList;

    auto   WORD         wSeconds;
#ifdef USE_NETWORKALERTS   //&?
    auto   HGLOBAL	hUser;
#endif

    lpNavOptions->alert.bDispAlertMsg  = cbAlertDisplayMessage.value;
    lpNavOptions->alert.bBeep          = cbAlertAudible.value;
    lpNavOptions->alert.bRemoveAfter   = cbAlertRemoveDialog.value;

    STRCPY(lpNavOptions->alert.szAlertMsg, teAlertMessage.string);

    ConvertStringToWord(&wSeconds, teAlertRemoveAfter.string);
    lpNavOptions->alert.uSeconds = (BYTE) wSeconds;

                                        // alert.uSeconds should be 
                                        // at least 1
    lpNavOptions->alert.uSeconds = lpNavOptions->alert.uSeconds ?
                                   lpNavOptions->alert.uSeconds : 1;

    
#ifdef USE_NETWORKALERTS   //&?
    if ( bNavboot )
        return;

    lpNavOptions->alert.bAlertNetUsr   = cbAlertNetworkUsers.value;
    lpNavOptions->alert.bAlertNetCon   = cbAlertNetworkConsole.value;
    lpNavOptions->alert.bAlertNLM      = cbAlertNLM.value;

    lpNavOptions->alert.bKnown         =  cbAlertOthersKnownVirus.value;
    lpNavOptions->alert.bUnknown       =  cbAlertOthersUnknownVirus.value;
    lpNavOptions->alert.bInoculation   =  cbAlertOthersInocChange.value;
    lpNavOptions->alert.bVirusLike     =  cbAlertOthersVirusLike.value;
    lpNavOptions->alert.bStartEnd      =  cbAlertOthersStartEnd.value;
    lpNavOptions->alert.bVirListChange =  cbAlertOthersVirusList.value;


    /*----SKURTZ-----05-17-93 12:23:51pm---------------
     * Has teAlertNetworkUsers.string been edited directly by
     * user? If so, then parse and store its contents.
     *-------------------------------------------------*/

    StringToNetUsers(teAlertNetworkUsers.string, &lpNavOptions->netUser);

    /*----SKURTZ-----05-17-93 01:41:54pm---------------
     * See if NetUserItems has been allocated. If it has
     * then see if it is bigger then necessary.
     * Shrink it.
     *-------------------------------------------------*/

    if (bNetUserMemIsMaxSize &&
	lpNavOptions->netUser.uEntries < MAX_NET_OBJECTS)

	{
	MemUnlock(lpNavOptions->netUser.hNetUserItem,
                  lpNavOptions->netUser.lpNetUserItem);

    hUser = MemReAlloc(lpNavOptions->netUser.hNetUserItem,
               (lpNavOptions->netUser.uEntries + 1)  * sizeof(NETUSERITEM),
			    0);

	lpNavOptions->netUser.lpNetUserItem = MemLock(hUser);
	lpNavOptions->netUser.hNetUserItem = hUser;
	}
#endif  // USE_NETWORKALERTS

} // End AlertAccept()

//************************************************************************
// DisableAlertControls()
//
// This routine enables/disables the three alert text edits depending
// on the state of the corresponding checkboxes.
//      --  cbAlertDisplayMessage
//      --  cbAlertNetworkUsers
//      --  cbAlertRemoveDialog
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/14/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL DisableAlertControls (DIALOG_RECORD *lpdrDialog)
{
    extern CheckBoxRec  cbAlertDisplayMessage;
    extern CheckBoxRec  cbAlertNetworkUsers;
    extern CheckBoxRec  cbAlertRemoveDialog;

    extern TERec        teAlertMessage;
    extern TERec        teAlertNetworkUsers;
    extern TERec        teAlertRemoveAfter;

    extern ButtonsRec   buEllipses;

#ifdef USE_NETWORKALERTS   //&?
    if ( !bNavboot ) 
        {
        if (!bNormalProcess)
            {
            return;
            }
                                        // ButtonEnable only redraws if
                                        // necessary.
        ButtonEnable(&buEllipses, 0, cbAlertNetworkUsers.value);

        if (cbAlertNetworkUsers.value ==
            (BYTE)((teAlertNetworkUsers.flags & TE_DISABLED) ? TRUE : FALSE))
            {
            TextEditEnable(lpdrDialog, &teAlertNetworkUsers,
                           cbAlertNetworkUsers.value);
            }
        }        
#endif  // USE_NETWORKALERTS
                                        // If the CheckBox is TRUE,
                                        // the TE should be enabled.
                                        // Also, the inverse.
                                        // Update only if necessary.
    if (cbAlertDisplayMessage.value ==
            (BYTE)((teAlertMessage.flags & TE_DISABLED) ? TRUE : FALSE))
        {
        TextEditEnable(lpdrDialog, &teAlertMessage,
                       cbAlertDisplayMessage.value);
        }
    if (cbAlertRemoveDialog.value ==
            (BYTE)((teAlertRemoveAfter.flags & TE_DISABLED) ? TRUE : FALSE))
        {
        TextEditEnable(lpdrDialog, &teAlertRemoveAfter,
                       cbAlertRemoveDialog.value);
        }
} // End DisableAlertControls()


#ifdef USE_NETWORKALERTS   //&? Dead from here down...

//------------------------------------------------------------------------
// Alert Others
//------------------------------------------------------------------------

//************************************************************************
// OthersDialog()
//
// This routine displays the alert others settings dialog and updates the
// settings information if accepted.
//
// Parameters:
//      none
//
// Returns:
//      nothing
//************************************************************************
// 2/19/93 DALLEE Funtion created.
//************************************************************************

VOID LOCAL PASCAL OthersDialog(VOID)
{
                                        //  Declared in ALERSTR.C
    extern  CheckBoxRec     cbAlertOthersKnownVirus;
    extern  CheckBoxRec     cbAlertOthersUnknownVirus;
    extern  CheckBoxRec     cbAlertOthersInocChange;
    extern  CheckBoxRec     cbAlertOthersVirusLike;
    extern  CheckBoxRec     cbAlertOthersStartEnd;
    extern  CheckBoxRec     cbAlertOthersVirusList;
    extern  DIALOG          dlAlertOthers;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult;
    auto    WORD            wOldHelp;


    OthersInit();

    lpdrDialog = DialogOpen2( &dlAlertOthers,
                              &cbAlertOthersKnownVirus,
                              &cbAlertOthersUnknownVirus,
                              &cbAlertOthersInocChange,
                              &cbAlertOthersVirusLike,
                              &cbAlertOthersStartEnd,
                              &cbAlertOthersVirusList);

    if (lpdrDialog != NULL)
        {
        wOldHelp = HyperHelpTopicSet(HELP_DLG_ALERT_OTHERS);

        do
            {
            wResult = StdEventLoop(NULL,    // Menu
                                   lpdrDialog,
                                   OthersButtonsProc,
                                   NULL,    // Menu func
                                   NULL,    // Pre func
                                   NULL);   // Post func
            if (wResult == ACCEPT_DIALOG)
                {
                OthersAccept();
                break;
                }
            }
            while (wResult != ABORT_DIALOG);

        DialogClose(lpdrDialog, wResult);
        HyperHelpTopicSet(wOldHelp);
        }

} // End OthersDialog()


//************************************************************************
// OthersButtonsProc()
//
// This routine handles the button presses in the Alert others
// dialog.
//
// Parameters:
//      DIALOG_RECORD *lpdrDialog       Pointer to current dialog record
//
// Returns:
//      ACCEPT_DIALOG                   Ok button hit
//      ABORT_DIALOG                    Cancel hit
//      EVENT_AVAIL                     Help, or no button
//************************************************************************
// 3/12/93 DALLEE Function created.
//************************************************************************

int STATIC OthersButtonsProc (DIALOG_RECORD *lpdrDialog)
{
    BYTE                byValue;


    byValue = lpdrDialog->d.buttons->value;

    switch (byValue)                    // Current button selected
        {
        case ALERT_OTHERS_OK_BUTTON:
            return (ACCEPT_DIALOG);

        case ALERT_OTHERS_CANCEL_BUTTON:
            return (ABORT_DIALOG);

        case ALERT_OTHERS_HELP_BUTTON:
                                        // Call monitor startup help here
	    HyperHelpDisplay();
            return (EVENT_AVAIL);

        default:
            return (EVENT_AVAIL);
        }
} // End OthersButtonsProc()


//************************************************************************
// OthersInit()
//
// This routine sets up the alert others options dialog controls
// based on the values in the temporary ALERTOPT structure lpOptAlert.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/12/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL OthersInit (VOID)
{
    extern CheckBoxRec     cbAlertOthersKnownVirus;
    extern CheckBoxRec     cbAlertOthersUnknownVirus;
    extern CheckBoxRec     cbAlertOthersInocChange;
    extern CheckBoxRec     cbAlertOthersVirusLike;
    extern CheckBoxRec     cbAlertOthersStartEnd;
    extern CheckBoxRec     cbAlertOthersVirusList;


    cbAlertOthersKnownVirus.value   = lpNavOptions->alert.bKnown;
    cbAlertOthersUnknownVirus.value = lpNavOptions->alert.bUnknown;
    cbAlertOthersInocChange.value   = lpNavOptions->alert.bInoculation;
    cbAlertOthersVirusLike.value    = lpNavOptions->alert.bVirusLike;
    cbAlertOthersStartEnd.value     = lpNavOptions->alert.bStartEnd;
    cbAlertOthersVirusList.value    = lpNavOptions->alert.bVirListChange;

} // End OthersInit()


//************************************************************************
// OthersAccept()
//
// This routine is called if the alert others dialog has been accepted.
// It sets the values of the temporary ALERTOPTS structure *lpOptAlert to
// match the dialog controls.
//
// NOTE: The alert others options changes are not saved to the .INI file
//       unless and until the main alert options dialog is accepted.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 3/12/93 DALLEE Function created.
//************************************************************************

VOID LOCAL PASCAL OthersAccept (VOID)
{
    extern CheckBoxRec     cbAlertOthersKnownVirus;
    extern CheckBoxRec     cbAlertOthersUnknownVirus;
    extern CheckBoxRec     cbAlertOthersInocChange;
    extern CheckBoxRec     cbAlertOthersVirusLike;
    extern CheckBoxRec     cbAlertOthersStartEnd;
    extern CheckBoxRec     cbAlertOthersVirusList;

    lpNavOptions->alert.bKnown         = cbAlertOthersKnownVirus.value;
    lpNavOptions->alert.bUnknown       = cbAlertOthersUnknownVirus.value;
    lpNavOptions->alert.bInoculation   = cbAlertOthersInocChange.value;
    lpNavOptions->alert.bVirusLike     = cbAlertOthersVirusLike.value;
    lpNavOptions->alert.bStartEnd      = cbAlertOthersStartEnd.value;
    lpNavOptions->alert.bVirListChange = cbAlertOthersVirusList.value;

} // End OthersAccept()


//------------------------------------------------------------------------
// Alert Browse for Network Users
//------------------------------------------------------------------------


//************************************************************************
// NetworkUsersDlg()
//
// This routine displays the browse dialog for network users.
//
// Parameters:
//      none
//
// Returns:
//      nothing
//************************************************************************
// 2/23/93 BRAD Funtion created.
//************************************************************************

VOID LOCAL PASCAL NetworkUsersDlg(VOID)
{
    extern  TERec           	    teAlertNetworkUsers;
    extern  char                    FAR SZ_NETWORK_USERS_TITLE [];
    extern  LPNAVOPTIONS    	    lpNavOptions;

    auto    NETSELECTOBJECTSDOS     rSelectObjects;
    auto    HGLOBAL                 hSelected = NULL;
    auto    LPBYTE		    lpObjectName;
    auto    WORD                    wOldHelp;


    wOldHelp = HyperHelpTopicSet(HELP_DLG_ALERT_NETWORK_BROWSE);

    rSelectObjects.lpszTitle = SZ_NETWORK_USERS_TITLE;
    rSelectObjects.uSelectedMax = MAX_NET_OBJECTS;
                                        // Allocate memory for user names
    hSelected = MemAlloc (GHND, (DWORD) rSelectObjects.uSelectedMax *
                                                (NAVD_SERVER_USER_LENGTH + 1));
    if ( hSelected == NULL )
	goto NetExitOut;


    lpObjectName = rSelectObjects.rListOfObjects.lpSelected = MemLock(hSelected);

    if (lpObjectName == NULL)
	goto NetExitOut;

    /*---------------05-13-93 01:03:02pm-SKURTZ--------
     * Some objects have been selected.  If netUser struct
     * has not been filled in, allocate memory for it now.
     *-------------------------------------------------*/


    if (!AllocReAllocMemory(lpNavOptions))
	goto NetExitOut;

    StringToNetUsers(teAlertNetworkUsers.string, &lpNavOptions->netUser);

    // update number of entries.
    rSelectObjects.rListOfObjects.uSelectedCount = lpNavOptions->netUser.uEntries;

    if (lpNavOptions->netUser.uEntries > 0)
        NetUserToSelectedObjects(&rSelectObjects,&lpNavOptions->netUser);

    if (StdDlgSelectNetworkUsers(&rSelectObjects))
        {
                                        // Save changes.
    	SelectedObjectsToNetUser(&rSelectObjects,&lpNavOptions->netUser);
        }

NetExitOut:
    BuildAlertNetUsrString();

    HyperHelpTopicSet(wOldHelp);
    if (hSelected)
	{
    	MemUnlock(hSelected, lpObjectName );
    	MemFree(hSelected);
	}
}


/**************************************************************************
 * AllocReAllocMemory
 *
 * DESCRIPTION  : Verify that the memory used by LPNETUSERITEM
 * 		  has already been allocated. If it hasn't, then create
 * 		  that memory, now.  If it has but is too small, realloc
 *  		  it to the maximum size ever needed.
 *
 * 		  Note. When this control panel is exited, this memory will
 * 		  be put on a diet so that it is only as big as it needs to
 * 		  be.
 *
 * IN      :      lp to NavOptions
 *
 * RETURN VALUES:  Success or failure.
 *************************************************************************
 * SKURTZ : 05-17-93 at 12:55:06pm Function created
 ************************************************************************* */
BOOL LOCAL  PASCAL AllocReAllocMemory(LPNAVOPTIONS lpNavOptions)
{
    auto    HGLOBAL		    hUsers = NULL;
    auto    LPNETUSERITEM	    lpNetUserItem = NULL;


    if (lpNavOptions->netUser.hNetUserItem == NULL)
	{
	hUsers = MemAlloc(GHND,(MAX_NET_OBJECTS + 1) * sizeof(NETUSERITEM));
	if (hUsers == NULL)
	    return(FALSE);


        if ((lpNetUserItem = (LPNETUSERITEM) MemLock(hUsers)) == NULL)
	    {
            MemFree(hUsers);
	    return(FALSE);
	    }

	lpNavOptions->netUser.hNetUserItem  = hUsers;
	lpNavOptions->netUser.lpNetUserItem = lpNetUserItem;
	lpNavOptions->netUser.uEntries = 0;	// indicate the list is empty.
	bNetUserMemIsMaxSize = TRUE;
	}
    else
	{
	if (MemSize(lpNavOptions->netUser.hNetUserItem) <
		   (DWORD)(MAX_NET_OBJECTS + 1) * sizeof(NETUSERITEM))

	    {
	    MemUnlock(lpNavOptions->netUser.hNetUserItem,
                      lpNavOptions->netUser.lpNetUserItem);
	    hUsers = MemReAlloc(lpNavOptions->netUser.hNetUserItem,
			       (MAX_NET_OBJECTS + 1) * sizeof(NETUSERITEM),
			       0);

	    if (hUsers == NULL)
		return(FALSE);

	    lpNavOptions->netUser.lpNetUserItem = MemLock(hUsers);
	    lpNavOptions->netUser.hNetUserItem = hUsers;
	    bNetUserMemIsMaxSize = TRUE;
	    }
	}
    return(TRUE);
}

/**************************************************************************
 * BuildAlertNetUsrString
 *
 * DESCRIPTION  : Build the "teAlertNetwordkUserts.string" from the contents
 *  		  of lpNavOptions->netUser
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-14-93 at 03:42:04pm Function created
 ************************************************************************* */
VOID LOCAL PASCAL BuildAlertNetUsrString(VOID)
{
    extern  TERec           	    teAlertNetworkUsers;
    extern  LPNAVOPTIONS    	    lpNavOptions;

    auto    UINT		    uCount;
    auto    LPNETUSERITEM	    lpNetUserItem;
    auto    char		    szSpace[] = " ";



    teAlertNetworkUsers.string[0] = EOS;
    lpNetUserItem = lpNavOptions->netUser.lpNetUserItem;


    for (uCount = 0;
        uCount < lpNavOptions->netUser.uEntries;
	uCount++,
	lpNetUserItem++)

	{
	// if this is a group, then start out with "[" character
        if (!(lpNetUserItem->wFlags & NETUSER_FLAG_ISUSER))
            {
            STRCAT(teAlertNetworkUsers.string, SZ_GROUP_START);
            }

	// if there is a server involved, put that in now.

	if (*lpNetUserItem->szServer)
	    {
            STRCAT(teAlertNetworkUsers.string, lpNetUserItem->szServer);
            STRCAT(teAlertNetworkUsers.string, SZ_SEPS);
	    }

	// Add object
        STRCAT(teAlertNetworkUsers.string, lpNetUserItem->szObject);

	// if this is a group, then end with "]" character
        if (!(lpNetUserItem->wFlags & NETUSER_FLAG_ISUSER))
            {
            STRCAT(teAlertNetworkUsers.string, SZ_GROUP_END);
            }

	// and append the space.
	STRCAT(teAlertNetworkUsers.string, szSpace);

	}

	// Remove the trailing " " from this string.

    teAlertNetworkUsers.string[STRLEN(teAlertNetworkUsers.string)-1] = EOS;

    _TextEditWrite(&teAlertNetworkUsers, TRUE, FALSE);
}


/**************************************************************************
 * NetUserToSelectedObjects
 *
 * DESCRIPTION  : move Netuser items to SelectedObjects array
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-13-93 at 04:56:49pm Function created
 ************************************************************************* */

VOID LOCAL PASCAL NetUserToSelectedObjects(NETSELECTOBJECTSDOS * lprSelectObjs,
					   LPNETUSER lpNetUser)
{
    auto	UINT		uCounter;
    auto    	LPBYTE		lpObjectName;
    auto	LPNETUSERITEM	lpNetUserItem;


    lpObjectName = lprSelectObjs->rListOfObjects.lpSelected;
    lpNetUserItem = lpNetUser->lpNetUserItem;
    lprSelectObjs->rListOfObjects.uSelectedCount = lpNetUser->uEntries;


    for (uCounter = 0;
	 uCounter < lpNetUser->uEntries;
	 uCounter++,
         lpObjectName += NAVD_SERVER_USER_LENGTH + 1,
	 lpNetUserItem++)
	{

	*lpObjectName = EOS;

        if (!(lpNetUserItem->wFlags & NETUSER_FLAG_ISUSER))
            {
            STRCAT(lpObjectName,SZ_GROUP_START);
            }

	if (*lpNetUserItem->szServer)
	    {
	    STRCAT(lpObjectName,lpNetUserItem->szServer);
	    STRCAT(lpObjectName,SZ_SEPS);
	    }

	STRCAT(lpObjectName,lpNetUserItem->szObject);

        if (!(lpNetUserItem->wFlags & NETUSER_FLAG_ISUSER))
            {
            STRCAT(lpObjectName,SZ_GROUP_END);
            }
	}

}
/**************************************************************************
 * SelectedObjectsToNetUser
 *
 * DESCRIPTION  :Move SelectedObjects to NetUser
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-13-93 at 04:57:02pm Function created
 ************************************************************************* */
VOID LOCAL PASCAL SelectedObjectsToNetUser(NETSELECTOBJECTSDOS * lprSelectObjs,
					   LPNETUSER lpNetUser)

{
    auto	UINT		uCounter;
    auto    	LPBYTE		lpObjectName;
    auto	LPNETUSERITEM	lpNetUserItem;

    lpObjectName = lprSelectObjs->rListOfObjects.lpSelected;
    lpNetUser->uEntries = lprSelectObjs->rListOfObjects.uSelectedCount;
    lpNetUserItem = lpNetUser->lpNetUserItem;


    for (uCounter = 0;
    	uCounter < lpNetUser->uEntries;
	uCounter++,
	lpNetUserItem++,
        lpObjectName += NAVD_SERVER_USER_LENGTH + 1)
	{
        StringToSingleNetUser(lpObjectName,lpNetUserItem);
	}
    // Zero out the last entry,  needed by NetAlert() to locate end of list.
    // Yes, we are careful to always allocate  memory for
    // lpNetUser->uEntries + 1.
    MEMSET(lpNetUserItem, '\0', sizeof(*lpNetUserItem));
}


//************************************************************************
// StringToNetUsers
//
// Parameters:
//      LPSTR       lpszNetUsers        Text string of network users to notify.
//      LPNETUSER   lpNetUser           Network User info struct.
//
// Returns:
//      Nothing.
//************************************************************************
// 8/27/93 DALLEE, pulled this code out of AlertAccept().
//************************************************************************

VOID LOCAL PASCAL StringToNetUsers (LPSTR lpszNetUsers, LPNETUSER lpNetUser)
{
    auto    char            *lpStart;
    auto    char            *lpEnd;
    auto    char            *lpTotalEnd;
    auto    LPNETUSERITEM   lpNetUserItem;


    AllocReAllocMemory(lpNavOptions);

    StringRemoveTrailingSpaces(lpszNetUsers);
    lpTotalEnd = lpszNetUsers + STRLEN(lpszNetUsers);

    lpStart = StringSkipWhite(lpszNetUsers);
    lpEnd   = StringSkipToWhite(lpStart);
    *lpEnd  = EOS;

    lpNavOptions->netUser.uEntries = 0;
    lpNetUserItem = lpNetUser->lpNetUserItem;

    while ((*lpStart != EOS) && (lpNetUser->uEntries < MAX_NET_OBJECTS))
        {
        StringToSingleNetUser(lpStart, lpNetUserItem);

                                    // Make sure this entry doesn't already
                                    // exist in the list.
        if ( !NetUserAlreadyExists(lpNetUserItem, lpNetUser) )
            {
            lpNetUserItem++;
            lpNetUser->uEntries++;
            }
                                        // If we're at the end, done
        if ( lpEnd == lpTotalEnd )
            break;

        lpStart = StringSkipWhite(lpEnd + 1);
        lpEnd   = StringSkipToWhite(lpStart);
        *lpEnd  = EOS;
        }
    // Zero out the last entry,  needed by NetAlert() to locate end of list.
    // Yes, we're careful to always allocate  memry for lpNetUser->uEntries + 1.
    MEMSET(lpNetUserItem, '\0', sizeof(*lpNetUserItem));

} // End StringToNetUsers()


/**************************************************************************
 * StringToSingleNetUser
 *
 * DESCRIPTION  : Take a string and fill out a NETUSERITEM.
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-17-93 at 10:52:20am Function created
 ************************************************************************* */

VOID LOCAL PASCAL StringToSingleNetUser(char * szString,LPNETUSERITEM lpNetUserItem)
{
    auto    	LPSTR           lpszToken1;
    auto    	LPSTR           lpszToken2;
    auto	LPSTR		lpServer;
    auto	LPSTR		lpObject;
    auto	char		szTempString[NETMAXSERVERNAME + NETMAXUSERNAME + 5];


    STRNCPY(szTempString,szString, NETMAXSERVERNAME + NETMAXUSERNAME );

    // Get network server (if any)
    // Get person name.

    MEMSET(lpNetUserItem, '\0', sizeof(*lpNetUserItem));

    lpServer = lpNetUserItem->szServer;
    lpObject = lpNetUserItem->szObject;

    if (szTempString[0] != SZ_GROUP_START[0])
        {
        lpNetUserItem->wFlags |= NETUSER_FLAG_ISUSER; // an individual
        }
    else
	{	     // groups start with "[" and end with "]"
        lpNetUserItem->wFlags &= ~NETUSER_FLAG_ISUSER;
	// kill ending "]"
	szTempString[STRLEN(szTempString) - 1] = EOS;
	// kill beginning "["
	_fmemmove(szTempString,szTempString+1,STRLEN(szTempString)+1);
	}

    lpszToken1 = _fstrtok(&szTempString[0], SZ_SEPS);
    lpszToken2 = _fstrtok(NULL        , SZ_SEPS);

    // now parse the item up and put it in navOptions

    if (lpszToken2)		// if two tokens, then server was used
	{
        lpNetUserItem->wFlags |= NETUSER_FLAG_SHOWSERVER;
        STRNCPY(lpServer, lpszToken1, sizeof(lpNetUserItem->szServer) - 1);
	lpszToken1 = lpszToken2;
	}

    STRNCPY(lpObject, lpszToken1, sizeof(lpNetUserItem->szObject) - 1);

                                        // Uppercase both server and object
    STRUPR (lpServer);
    STRUPR (lpObject);
}


/**************************************************************************
 * TextEditValidation
 *
 * DESCRIPTION  : Validate input into the teAlertNetworkUsers dialog
 *                element.
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-14-93 at 05:13:37pm Function created
 ************************************************************************* */

UINT TEAlertNetworkUserValidate(TERec *te, UINT *c)
{
    *c = CharToUpper(*c);
    return(EVENT_AVAIL);
}
#endif  // USE_NETWORKALERTS

