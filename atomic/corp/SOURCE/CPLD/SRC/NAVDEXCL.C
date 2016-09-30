// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/cpld/VCS/navdexcl.c_v   1.0   06 Feb 1997 20:50:20   RFULLER  $
//
// Description:
//      These are the support functions for the DOS NAV Exclusions options
//      configuration.
//
// Contains:
//      CPDialogProcExclude()
//
// See Also:
//      EXCLSTR.CPP for strings and control structures.
//      OPTSTR.CPP for gobal strings and control structures used in multiple
//          CP list dialogs.
//************************************************************************
// $Log:   S:/cpld/VCS/navdexcl.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:50:20   RFULLER
// Initial revision
// 
//    Rev 1.1   30 Sep 1996 14:38:24   JALLEE
// Port of DBCS changes from Nirvan Plus.
// 
//    Rev 1.0   30 Jan 1996 15:56:56   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:09:58   BARRY
// Initial revision.
// 
//    Rev 1.34   16 Jan 1995 14:56:56   DALLEE
// Removed calls to ExcludeXXXEx() functions.
// Passing EXCLUDEITEM structs around again (internal szTest now handles
// 260 byte filenames).
// 
//    Rev 1.33   06 Jan 1995 18:45:24   DALLEE
// Compile with Martin's changes to exclusions.
// Still need to fix:
//    1. Should be able to update by index.  ExcludeEditEx() can possibly
//       edit the wrong entry -- duplicate entries can be created with
//       prior calls to ExcludeEditEx().
//    2. ExcludeAdd() in NAVEXCLU does not preserve subdirs flag when
//       checking whether to merge entries.
// 
//    Rev 1.32   28 Dec 1994 11:44:02   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 15:48:46   DALLEE
    // Commented out password protection.
    //
    //    Rev 1.1   06 Dec 1994 20:48:16   DALLEE
    // CVT1 script.
    //
//    Rev 1.31   28 Dec 1994 11:34:36   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   29 Nov 1994 19:13:52   DALLEE
    // From Nirvana
    //
    //    Rev 1.28   12 Oct 1993 16:47:08   BARRY
    // New CP_LEAVING handling to fix Alt-F4 bug
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"
#include "dbcs.h"

#include "navdprot.h"
#include "navutil.h"

#include "options.h"

MODULE_NAME;

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************


#define EXCLUDE_OK_BUTTON           ( 0)
#define EXCLUDE_CANCEL_BUTTON       ( 1)
#define EXCLUDE_ADD_BUTTON          ( 2)
#define EXCLUDE_EDIT_BUTTON         ( 3)
#define EXCLUDE_DELETE_BUTTON       ( 4)
#define EXCLUDE_HELP_BUTTON         ( 5)

#define ADD_DIALOG_ELLIPSES_BUTTON  ( 0)
#define ADD_DIALOG_OK_BUTTON        ( 1)
#define ADD_DIALOG_CANCEL_BUTTON    ( 2)
#define ADD_DIALOG_HELP_BUTTON      ( 3)

#define EXCLUDE_ADD                 ( 1)
#define EXCLUDE_EDIT                ( 0)


#define ALT_RIGHT_ARROW		413



//************************************************************************
// EXTERNAL VARIABLES
//************************************************************************

extern ListRec lrExcludeItems;          // Declared in EXCLSTR.C
extern OutputFieldRec lpofExcludeFrom[];
extern OutputFieldRec ofExcludeSelectedItem;

extern  CheckBoxRec     cxScanSubDir;
extern  CheckBoxRec     cbExcludeAddKnownVirus;
extern  CheckBoxRec     cbExcludeAddUnknownVirus;
extern  CheckBoxRec     cbExcludeAddInocChange;
extern  CheckBoxRec     cbExcludeAddHardFormat;
extern  CheckBoxRec     cbExcludeAddHardBootRecords;
extern  CheckBoxRec     cbExcludeAddFloppyBootRecords;
extern  CheckBoxRec     cbExcludeAddWriteEXE;
extern  CheckBoxRec     cbExcludeAddAttributeChange;

extern LPNAVOPTIONS     lpNavOptions;


//************************************************************************
// LOCAL TYPEDEFS
//************************************************************************

typedef struct tagEXCLUDEDIALOGINFO
    {
    CheckBoxRec *lpCheckBox;
    WORD        wFlag;
    } EXCLUDEDIALOGINFO;

//************************************************************************
// STATIC VARIABLES
//************************************************************************

static BYTE    byPreviousState;

                                        // This array is used for updating
                                        // all the dialogs with the excluded
                                        // file information.
static EXCLUDEDIALOGINFO rExcludeDialogInfo [] =
  {
    {&cbExcludeAddKnownVirus,        excVIRUS_FOUND},
    {&cbExcludeAddUnknownVirus,      excFILE_WATCH},
    {&cbExcludeAddInocChange,        excINOC_CHANGE},
    {&cbExcludeAddHardFormat,        excLOW_FORMAT},
    {&cbExcludeAddHardBootRecords,   excWR_HBOOT},
    {&cbExcludeAddFloppyBootRecords, excWR_FBOOT},
    {&cbExcludeAddWriteEXE,          excWR_ATLIST},
    {&cbExcludeAddAttributeChange,   excRO_CHANGE},
    {NULL,                           NULL}
  };


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

UINT ExcludeCP_ENTERING       (VOID);
WORD LOCAL PASCAL ExcludeCP_LEAVING        (DIALOG_RECORD *lpDialog);
VOID LOCAL PASCAL AddEditDialog            (BOOL bAdd);
VOID LOCAL PASCAL AddEditDialogAccept      (LPEXCLUDEITEM lpExclude,
                                            BOOL bAdd);
VOID LOCAL PASCAL UpdateExcludeDialog      (DIALOG_RECORD *lpDialog);
VOID LOCAL PASCAL UpdateAddEditDialog      (LPEXCLUDEITEM lpExclude);
VOID LOCAL PASCAL DisableExcludeButtons    (VOID);
VOID LOCAL PASCAL AddEditGetSettings       (LPEXCLUDEITEM lpExclude);
WORD AddEditEventFilter ( DIALOG_RECORD *dr, WORD *wEvent );


//************************************************************************
// CPDialogProcExclude()
//
// This is the callback routine for the Options CP Exclusions Dialog.
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
//************************************************************************

UINT CPDialogProcExclude (
    LPCPL         lpCPLDialog,          // Pointer to related CPL structure
    UINT          uMessage,             // Message
    UINT          uParameter,           // Additional message parameter
    DIALOG_RECORD *lpDialog)            // Dialog record w/ internal info.
{
    extern      BYTE            byAcceptCPDialog;
    extern      ButtonsRec      buExcludeButtons;

    static      DWORD           dwOldListEntry;
    auto        WORD            wResult;
    auto        BYTE            byButton;
    auto        DIALOG_ITEM     *dItem;

#ifdef USE_PASSWORD    //&?
    static      BOOL            bNormalProcess;
#endif


    switch (uMessage)
        {
        case CP_PRE_EVENT:              // Event received, not yet processed
                                        // Unless the event is the ESC key,
                                        // set the save flag.
            byAcceptCPDialog = (BYTE)(uParameter != ESC);

                                        // Save the list entry so we can
                                        // check if it has changed.
            dwOldListEntry = lrExcludeItems.active_entry;
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_POST_EVENT:
                                        // If the list entry changed, update
                                        // the dialog.
            if (dwOldListEntry != lrExcludeItems.active_entry)
                {
                UpdateExcludeDialog(lpDialog);
                }
            return (EVENT_AVAIL);       // Return event avail for processing

        case CP_BUTTON_PRESSED:         // Button pressed
                                        // lpDialog->item is DIALOG_ITEM
                                        // DIALOG_ITEM->item is ptr to actual
                                        // item, buttons in this case
            byButton = buExcludeButtons.value;

            dItem = DialogGetFocus(lpDialog);
            if (dItem->item == &lrExcludeItems)
                {
                                        // Edit entry, if entry available
                byButton = (ButtonIsDisabled(&buExcludeButtons, EXCLUDE_EDIT_BUTTON)) ?
                                0xFF : EXCLUDE_EDIT_BUTTON; 
                }

            switch (byButton)
                {
                case EXCLUDE_OK_BUTTON:
                                        // Check data & update if necessary
                    wResult = ACCEPT_DIALOG;
                    break;

                case EXCLUDE_CANCEL_BUTTON:
                                        // Set save flag off.
                    byAcceptCPDialog = FALSE;
                    wResult = ABORT_DIALOG;
                    break;

                case EXCLUDE_ADD_BUTTON:
                case EXCLUDE_EDIT_BUTTON:
#ifdef USE_PASSWORD    //&?
		    if (AskForPassword(PASSWORD_MODIEXCLUSIONS))
#endif
			{

                                        // Same dialog, just pass a different
                                        // flag depending on which button
                                        // was hit.
                        AddEditDialog(buExcludeButtons.value == EXCLUDE_ADD_BUTTON ?
                                  	EXCLUDE_ADD : EXCLUDE_EDIT);
                        UpdateExcludeDialog(lpDialog);
	                HyperHelpTopicSet(HELP_DLG_CPL_EXCLUSIONS);   // reset help topic
			}
                    wResult = EVENT_AVAIL;

                    break;

                case EXCLUDE_DELETE_BUTTON:
#ifdef USE_PASSWORD    //&?
		    if (AskForPassword(PASSWORD_MODIEXCLUSIONS))
#endif
			{
                                        // Remove from the list and from
                                        // the exclusions buffer.
                    	ExcludeDeleteIndex(&lpNavOptions->exclude,
                                       	   (WORD) lrExcludeItems.active_entry);
                    	UpdateExcludeDialog(lpDialog);
			}
		    wResult = EVENT_AVAIL;
                    break;

                case EXCLUDE_HELP_BUTTON:
                                        // Call exclude settings help here
		    HyperHelpDisplay();
                    wResult = EVENT_AVAIL;
                    break;

                default:
                    wResult = EVENT_AVAIL;
                    break;
                }
            return (wResult);

        case CP_PRE_ENTERING:           // Haven't entered dialog yet
#ifndef SYM_DOSX                        // For DX, we need to reinit pointers
            break;                      
#else
            ExcludeCP_ENTERING();
            return (EVENT_AVAIL);           
#endif
        case CP_ENTERING:               // Dialog box now active box
                                        // Load OptExclude info from database
                                        //  and initialize dialog data
	    HyperHelpTopicSet(HELP_DLG_CPL_EXCLUSIONS);
#ifndef USE_PASSWORD //&?
            wResult = ExcludeCP_ENTERING();
#else
            wResult = ProcessPassword(lpDialog, 
	                              PASSWORD_VIEWEXCLOPT,
                                      ExcludeCP_ENTERING,
                                      &bNormalProcess);
            if (bNormalProcess)
#endif
                {
                UpdateExcludeDialog(lpDialog);
                }
            return (wResult);

        case CP_LEAVING:                // Dialog box is losing focus
                                        // uParameter contains ACCEPT/ABORT
                                        // Return CONTINUE_DIALOG to reject
                                        // the CP_LEAVING request.
            if ( uParameter != ACCEPT_DIALOG )
                byAcceptCPDialog = FALSE;

            return (ExcludeCP_LEAVING(lpDialog));
        } // End switch (uMessage)

    return (EVENT_AVAIL);               // Default return value
} // End CPDialogProcExclude()


//************************************************************************
// LOCAL/STATIC functions.
//************************************************************************

//************************************************************************
// ExcludeCP_ENTERING()
//
//      This routine reads the configuration information and initializes
//      the exclusions data structures and dialog controls
//
// Parameters:
//      *None*                          Should receive pointer to data
//                                          struct to load.
//
// Returns:
//      EVENT_AVAIL                     Ok, allow further processing of mesg.
//************************************************************************
// 2/17/93 DALLEE Function created.
//************************************************************************

UINT ExcludeCP_ENTERING(VOID)
{
                                        // Add all the exclusions text to
                                        // the list box.
    lrExcludeItems.active_entry = 0;
    lrExcludeItems.num          = ExcludeGetCount(&lpNavOptions->exclude);
    lrExcludeItems.entries      = &lpNavOptions->exclude;

    return (EVENT_AVAIL);
} // End ExcludeCP_ENTERING


//************************************************************************
// ExcludeCP_LEAVING()
//
// This routine copies the original exclusion information back to the temp
// structure if the Exclusions CP dialog was aborted.
//
// Parameters:
//      None
//
// Returns:
//      EVENT_AVAIL                     Ok, allow further processing of mesg.
//************************************************************************
// 2/17/93 DALLEE Function created.
//************************************************************************

WORD LOCAL PASCAL ExcludeCP_LEAVING (DIALOG_RECORD *lpDialog)
{
    extern  NAVOPTIONS  navOptions;
    extern  BYTE        byAcceptCPDialog;


                                        // Make sure scroll bar is enabled
    ListScrollBarEnable(lpDialog, &lrExcludeItems, TRUE);

                                        // If the user did not accept the
                                        // dialog, copy back the original
                                        // exclusions information.
    if (!byAcceptCPDialog)
        {
        ExcludeKillCopy(&lpNavOptions->exclude);
        ExcludeCreateCopy(&navOptions.exclude, &lpNavOptions->exclude);
        }

    return (ACCEPT_DIALOG);
} // End ExcludeCP_LEAVING()


//************************************************************************
// AddEditDialog()
//
// This routine displays the add exclusions dialog and updates
// the settings information if necessary.
//
// Parameters:
//      BOOL    bAdd                    EXCLUDE_ADD : Add new entry
//                                      EXCLUDE_EDIT: Edit current entry
//
// Returns:
//      nothing
//************************************************************************
// 2/18/93 DALLEE Funtion created.
//************************************************************************

VOID LOCAL PASCAL AddEditDialog(BOOL bAdd)
{
                                        // These externs are declared in
                                        // EXCLSTR.C
    extern      TERec           teExcludeAddItem;
    extern      DIALOG          dlExcludeAdd;
    extern      char            SZ_EXCLUDE_ADD_TITLE [];
    extern      char            SZ_EXCLUDE_EDIT_TITLE [];

                                        // These externs are declared in
                                        // OPTSTR.C
    extern      ButtonsRec      buEllipses;
    extern      char            SZ_STARS [];
    extern      char            SZ_EXCLUDE_FILE_TITLE [];
    auto        DIALOG_RECORD   *lpdrDialog;
    auto        WORD            wResult;
    auto        EXCLUDEITEM     rExclude;
    auto        BOOL            bSubDirs;
    auto        BOOL            bFinished = FALSE;
    auto        char            szAddItem[SYM_MAX_PATH];

    auto        WORD            wOldHelp1;
    auto        WORD            wOldHelp2;


    teExcludeAddItem.string = szAddItem;

                                        // Either clear the record, or fill
                                        // it with the information to edit.
    MEMSET(&rExclude, '\0', sizeof(rExclude));

    if (EXCLUDE_ADD != bAdd)
        {
        ExcludeGet(&lpNavOptions->exclude,
                   (WORD)lrExcludeItems.active_entry,
                   rExclude.szText,
                   &rExclude.wBits,
                   &bSubDirs);
        rExclude.bSubDirectory = (BYTE) bSubDirs;
        }
                                        // Load the text
    NAVDLoadTextEdit(&teExcludeAddItem, rExclude.szText);

                                        // Set up the dialog title
    dlExcludeAdd.title = (bAdd == EXCLUDE_ADD ?
                          SZ_EXCLUDE_ADD_TITLE : SZ_EXCLUDE_EDIT_TITLE);

    lpdrDialog = DialogOpen2(&dlExcludeAdd,
                             &teExcludeAddItem,
                             &buEllipses,
                             &cxScanSubDir,
                             &cbExcludeAddKnownVirus,
                             &cbExcludeAddUnknownVirus,
                             &cbExcludeAddInocChange,
                             &cbExcludeAddHardFormat,
                             &cbExcludeAddHardBootRecords,
                             &cbExcludeAddFloppyBootRecords,
                             &cbExcludeAddWriteEXE,
                             &cbExcludeAddAttributeChange);

    if (NULL != lpdrDialog)
        {
        wOldHelp1 = HyperHelpTopicSet(HELP_DLG_ADD_EXCLUSION);

        if (bAdd == EXCLUDE_ADD)
            {
                                            // Disable OK button
            ButtonEnable (lpdrDialog->d.buttons, 0, FALSE);
            }

        UpdateAddEditDialog(&rExclude);

        while (!bFinished)
            {
            wResult = DialogLoop(lpdrDialog, AddEditEventFilter);

            if ( wResult == ABORT_DIALOG )
                break;                      // User decided not to print

            if ( wResult != ACCEPT_DIALOG )
                continue;

            switch (DialogGetUniqueButtonNumber(lpdrDialog))
                {
                case ADD_DIALOG_OK_BUTTON:
    doOK:
                    AddEditGetSettings(&rExclude);
                    AddEditDialogAccept(&rExclude, bAdd);
                                            // Fall through to cancel

                case ADD_DIALOG_CANCEL_BUTTON:
                    bFinished = TRUE;
                    break;

                case ADD_DIALOG_ELLIPSES_BUTTON:
                    wOldHelp2=HyperHelpTopicSet(HELP_DLG_ADD_EXCLUSION_BROWSE);
                    TunnelSelectFile(&teExcludeAddItem,
                                     SZ_STARS,
                                     SZ_EXCLUDE_FILE_TITLE);
                    HyperHelpTopicSet(wOldHelp2);
                    break;

                case ADD_DIALOG_HELP_BUTTON:
                    HyperHelpDisplay();
                    break;

                default:                    // Not a button at all
                    goto doOK;
                }
            }
        DialogClose(lpdrDialog, wResult);

        HyperHelpTopicSet(wOldHelp1);
        }

} // End AddEditDialog()


/************************************************************************
 *                                                                      *
 * Description:                                                         *
// This routine enables or disables the OK in the ADD/EDIT dialog, based on
// whether there is:
//      1) text in the textEdit, and
//      2) At least 1 exclusion specified.
//
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/20/93 BRAD Function Created.                                      *
 ************************************************************************/

WORD AddEditEventFilter ( DIALOG_RECORD *dr, WORD *wEvent )
{
    extern      char            SZ_STARS [];
    extern      char            SZ_EXCLUDE_FILE_TITLE [];
    extern      TERec           teExcludeAddItem;
    extern      BYTE            byPreviousState;
    extern      ButtonsRec      buEllipses;
    auto        WORD            i;
    auto        BOOL            bExclusionSpecified;
    auto        BYTE            byEnableIt;

                                        // If one entry is enabled, then we
                                        // know the state
    for (i = 0, bExclusionSpecified = FALSE; rExcludeDialogInfo[i].wFlag; i++)
        {
        if (rExcludeDialogInfo[i].lpCheckBox->value)
            {
            bExclusionSpecified = TRUE;
            break;
            }
        }

    byEnableIt = (BYTE) ((bExclusionSpecified &&
                          teExcludeAddItem.string[0]) ? TRUE : FALSE);
    if ( byEnableIt != byPreviousState )
        {
                                        // Enable/disable OK, based on values
        ButtonEnable(dr->d.buttons, 0, byEnableIt);
        byPreviousState = byEnableIt;
        }

    if ((*wEvent == ALT_RIGHT_ARROW) &&
    	(dr->item->item == &teExcludeAddItem))
        {
        TunnelSelectFile(&teExcludeAddItem,
                         SZ_STARS,
                         SZ_EXCLUDE_FILE_TITLE);
	DialogSetFocus(dr, &teExcludeAddItem);
	return (EVENT_USED);
	}

    return ( EVENT_AVAIL );
}

//************************************************************************
// UpdateExcludeDialog()
//
// This routine redraws the list box and writes the actions from which
// the current list entry is excluded.
//
// Parameters:
//      none
//
// Returns:
//      nothing
//************************************************************************
// 5/6/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL UpdateExcludeDialog (DIALOG_RECORD *lpDialog)
{
    extern  char                SZ_EXCLUDE_SELECTED_FORMAT [];

    auto    EXCLUDEITEM         rExclude;
    auto    char                szBuffer [SYM_MAX_PATH];
    auto    WORD                i;
    auto    WORD                wCurrentField = 0;
    auto    LPSTR               lpSrc, lpDest;
    auto    BOOL                bSubDirs;

                                        // Reset the number of list entries,
                                        // and the active entry.
                                        // Write the updated list.
    lrExcludeItems.num = ExcludeGetCount(&lpNavOptions->exclude);

    if (lrExcludeItems.active_entry > lrExcludeItems.num)
        {
        lrExcludeItems.active_entry = lrExcludeItems.num;
        }

    ListWrite(&lrExcludeItems,
              TRUE,
              (BYTE) (lpDialog->item->item == (LPVOID)&lrExcludeItems));

                                        // If we have any exclusions, update
                                        // the dialog.  Otherwise, blank
                                        // all the fields.
    if (lrExcludeItems.num)
        {
        ExcludeGet(&lpNavOptions->exclude,
                   (WORD)lrExcludeItems.active_entry,
                   rExclude.szText,
                   &rExclude.wBits,
                   &bSubDirs);
        rExclude.bSubDirectory = (BYTE) bSubDirs;

                                        // Write out any types of activity
                                        // from which the current item is
                                        // excluded.
        for (i = 0; rExcludeDialogInfo[i].wFlag; i++)
            {
            if ( (rExclude.wBits & rExcludeDialogInfo[i].wFlag) ==
                 rExcludeDialogInfo[i].wFlag )
                {
                                        // Remove the hotkey character
                lpSrc = rExcludeDialogInfo[i].lpCheckBox->name;
                lpDest = szBuffer;
//                for (lpSrc = rExcludeDialogInfo[i].lpCheckBox->name,
//                     lpDest = szBuffer; *lpSrc; lpSrc++)
                while (*lpSrc)

                    {
                    if (DBCSIsLeadByte(*lpSrc))
                        {
                        *lpDest++ = *lpSrc++;
                        *lpDest++ = *lpSrc++;
                        }
                    else
                        {
                        if (*lpSrc != HOT_KEY_INDICATOR )
                            *lpDest++ = *lpSrc;
                        lpSrc++;
                        }
                    }
                *lpDest = EOS;

                DialogOutput(&lpofExcludeFrom[wCurrentField++],
                             szBuffer);
                }
            }

        FastStringPrint(szBuffer, SZ_EXCLUDE_SELECTED_FORMAT,
                        rExclude.szText);
        }
    else
        {
        szBuffer[0] = EOS;
        }

                                        // Blank the rest of the output fields.
    for ( ; rExcludeDialogInfo[wCurrentField].wFlag; wCurrentField++)
        {
        DialogOutput(&lpofExcludeFrom[wCurrentField], DIALOG_EMPTY_LINE);
        }
                                        // Display the selected exclusion item.
    DialogOutput(&ofExcludeSelectedItem, szBuffer);

    DisableExcludeButtons();
} // End UpdateExcludeDialog()


//************************************************************************
// UpdateAddEditDialog()
//
// This routine updates the status of the Add/Edit exclusion dialog
// checkboxes and textedit.
//
// Parameters:
//      LPEXCLUDEITEM   lpExclude       Exclude item whose settings to check.
//
// Returns:
//      nothing
//************************************************************************
// 5/6/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL UpdateAddEditDialog (LPEXCLUDEITEM lpExclude)
{
    extern  TERec   teExcludeAddItem;
    auto    WORD    i;


    CursorPush();
                                        // Update all the checkboxes depending
                                        // on what exclusions are set for
                                        // this item.
    for (i = 0; rExcludeDialogInfo[i].wFlag; i++)
        {
        UpdateCheckBox(rExcludeDialogInfo[i].lpCheckBox,
                       (lpExclude->wBits & rExcludeDialogInfo[i].wFlag) ==
                        rExcludeDialogInfo[i].wFlag);
        }

    UpdateCheckBox(&cxScanSubDir, lpExclude->bSubDirectory);

    byPreviousState = (BYTE) (teExcludeAddItem.string[0] ? TRUE : FALSE);

    CursorPop();

} // End UpdateAddEditDialog()


//************************************************************************
// AddEditGetSettings()
//
// This routine loads the EXCLUDEITEM structure with the settings of
// the Add/Edit dialog checkboxes and text edit control.
//
// Parameters:
//      LPEXCLUDEITEM   lpExclude       [out] EXCLUDEITEM to update
//
// Returns:
//      nothing
//************************************************************************
// 5/11/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL AddEditGetSettings (LPEXCLUDEITEM lpExclude)
{
    extern  TERec       teExcludeAddItem;

    auto    WORD        i;

    MEMSET(lpExclude, '\0', sizeof(*lpExclude));

    STRCPY(lpExclude->szText, teExcludeAddItem.string);
    lpExclude->bSubDirectory = cxScanSubDir.value;

                                        // Set the exclude action bits.
    for (i = 0; rExcludeDialogInfo[i].wFlag; i++)
        {
        if (rExcludeDialogInfo[i].lpCheckBox->value)
            {
            lpExclude->wBits |= rExcludeDialogInfo[i].wFlag;
            }
        }
} // End AddEditGetSettings()


//************************************************************************
// AddEditDialogAccept()
//
// This routine adds the EXCLUDEITEM to the list of exclusions, or
// modifies the entry, if it already exists.
//
// Parameters:
//      LPEXCLUDEITEM     lpExclude     ITEM to add or update.
//      BOOL              bAdd          EXCLUDE_ADD  : Add an exclusion.
//                                      EXCLUDE_EDIT : Replace the exclusion.
//
// Returns:
//      nothing
//************************************************************************
// 5/6/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL AddEditDialogAccept (LPEXCLUDEITEM lpExclude,
                                       BOOL          bAdd)
{
    auto    EXCLUDEITEM     rOldItem;
    auto    BOOL            bSubDirs;

                                        // If we are editing an entry,
                                        // replace it with the new information,
                                        // otherwise, add the new information.
    if (bAdd == EXCLUDE_EDIT)
        {
        ExcludeGet(&lpNavOptions->exclude,
                   (WORD)lrExcludeItems.active_entry,
                   rOldItem.szText,
                   &rOldItem.wBits,
                   &bSubDirs);
        rOldItem.bSubDirectory = (BYTE) bSubDirs;

        ExcludeEdit(&lpNavOptions->exclude,
                    &rOldItem,
                    lpExclude);
        }
    else
        {
        ExcludeAdd(&lpNavOptions->exclude, lpExclude);
        }
} // End AddEditDialogAccept()


//************************************************************************
// ExcludeListWriteEntry()
//
// This routine is the ListDisplay callback to write a single exclusions
// list entry.
//
// Parameters:
//      ListRec *  lplrList,
//      LPVOID     lpEntry,             LPEXCLUDE struct w/ all exclusions.
//      int        wAttr,               Attribute to write with
//      DWORD      dwEntry,             Number of entry in list
//
// Returns:
//      Nothing
//************************************************************************
// 5/6/93 DALLEE, Function created.
//************************************************************************

VOID ExcludeListWriteEntry (ListRec    *lplrList,
                            LPVOID      lpEntry,
                            int         iAttr,
                            DWORD       dwEntry)
{
    extern  char            SZ_EXCLUDE_LIST_FORMAT [];

    auto    char            szTemp [80 + 1];
    auto    EXCLUDEITEM     rExclude;
    auto    BOOL            bSubDirs;

    ColorSaveAll();
    ColorSetAttr(iAttr);

    if (lpEntry == NULL)
        {
        FastWriteSpaces(lplrList->column_width);
        }
    else
        {
        ExcludeGet((LPEXCLUDE)lpEntry,
                   (WORD)dwEntry,
                   rExclude.szText,
                   &rExclude.wBits,
                   &bSubDirs);

        rExclude.bSubDirectory = (BYTE) bSubDirs;

        FastStringPrint(szTemp, SZ_EXCLUDE_LIST_FORMAT, rExclude.szText);
        FastWriteString(szTemp);
        }
    ColorRestoreAll();
} // End ExcludeListWriteEntry()


//************************************************************************
// DisableExcludeButtons()
//
// This routine disable the Edit and Delete buttons based on the number
// of exclude items and the active list entry.
//
// Parameters:
//      none
//
// Returns:
//      nothing
//************************************************************************
// 5/11/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DisableExcludeButtons (VOID)
{
    extern  ButtonsRec  buExcludeButtons;

    auto    BYTE        bEnable;

    bEnable = lrExcludeItems.active_entry
              < ExcludeGetCount(&lpNavOptions->exclude);

    ButtonEnable(&buExcludeButtons, EXCLUDE_EDIT_BUTTON,   bEnable);
    ButtonEnable(&buExcludeButtons, EXCLUDE_DELETE_BUTTON, bEnable);
} // End DisableExcludeButtons()


