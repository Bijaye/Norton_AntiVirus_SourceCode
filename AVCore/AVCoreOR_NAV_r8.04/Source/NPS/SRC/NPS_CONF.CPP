// Copyright 1995 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/NPS/VCS/nps_conf.cpv   1.17   28 Aug 1998 16:55:48   JBRENNA  $
//
// Description:
//      This module contains the code for the confirmation dialog box
//
// Contains:
//
// See Also:
//
//***********************************************************************
// $Log:   S:/NPS/VCS/nps_conf.cpv  $
// 
//    Rev 1.17   28 Aug 1998 16:55:48   JBRENNA
// Port changes from QAKN branch to trunk.
// 
//    Rev 1.16   15 Jul 1997 15:13:24   DBUCHES
// Added Ctl3d Support for DEC Alpha
// 
//    Rev 1.15   07 May 1997 18:52:44   BILL
// Removed ctl3d for Alpha platform
// 
//    Rev 1.14   18 Sep 1996 10:16:40   JBRENNA
// Add NG_SetFontsInDialog() call when processing WM_INITDIALOG messages.
// This ensures that a DBCS font is used when necessary.
// 
//    Rev 1.13   22 Feb 1996 13:16:24   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' and 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()' to use Quake's StdHelp merged with the NAV
// version.
// 
//    Rev 1.12   11 Jan 1996 09:48:52   JWORDEN
// Port changes from Quake 8
// 
//***********************************************************************
#include "platform.h"
#include "stdhelp.h"
#include    <ctl3d.h>
#include "ngui.h"
#include "nps.h"
#include "npsres.h"
#include "npsstb.h"
#include "nps_hlp.h"

// ---------------------------------------------------------------------------
// Local variables
// ---------------------------------------------------------------------------
static  WORD    DONOTBUG    =   0x8000;

static	UINT 	ConfirmationBoxType;	// Set in OnInitDialog().  This is a
										// cache which holds the type of the
										// confirmation box just lauched.  The
										// value is used in 
										// OnCommand::IDC_NPS_HELP to launch 
										// the proper help screen.


// ---------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------

BOOL CALLBACK ConfirmDlgProc (          // Process messages to confirm box
    HWND    hDlg,                       // [in] dialog box window handle
    UINT    uMessage,                   // [in[ message to process
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
    );

BOOL LOCAL OnInitDialog (               // Process WM_INITDIALOG message
    HWND    hDlg,                       // [in] from this window
    LPARAM  lParam                      // [in] app defined value
    );

void LOCAL OnCommand (                  // Process the WM_COMMAND message
    HWND    hDlg,                       // [in] from this window
    int     nID,                        // [in] control ID that notifies
    HWND    hwndCtl,                    // [in] control hwnd that notified
    UINT    codeNotify                  // [in] notify code
    );

VOID LOCAL OnContextHelp (              // Respond to WM_CONTEXTMENU and WM_HELP
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,                       // [in] either WM_HELP or WM_CONTEXTMENU
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

VOID LOCAL NonExitDialogLayout (        // Adjust controls inside dialog
    HWND    hDlg                        // [in] dialog box to adjust
    );

VOID LOCAL ResizeDialog (               // Resize the dialog box
    HWND    hDlg,                       // [in] handle to the dialog box
    UINT    uUnits                      // [in] units to shrink with
    );

VOID LOCAL MoveCtrlUp (                 // Move the control up in dialog
    HWND    hCtrl,                      // [in] handle to control
    UINT    uUnits                      // [in] units to move up
    );

VOID LOCAL MoveButton (                 // Move button to new rect.
    HWND    hButton,                    // [in] handle to button to move
    LPRECT  lpTarget                    // [in] target
    );

/* **************************************************************************
@Name:
CONFSTATUS WINAPI AskConfirmation (     // Ask for confirmation
    HWND            hDlg,               // [in] moma window
    CONFIRM         Event,              // [in] CONFIRM_xxx
    LPNPSOPTIONS    lpOptions           // [in] pointer to options
    )

@Description:
This function creates the ask confirmation dialog box. The calling application
passes in an event identifyer that directs the logic to display the correct
message and dialog layout. 

@Parameters:
$hDlg$      handle of the parent of this dialog box.
$Event$     event identifier that we're to notify about.

            CONFIRM_EXIT    : About to exit
            CONFIRM_DELETE  : About to delete an item
            CONFIRM_SAVE    : About to save an item
            CONFIRM_EDIT    : About to save changes to an item

$lpOptions$ pointer to scheduler options that contain the logic of what
            we're to do.

@Returns:
            CONFSTATUS_ALLOW    : User specified to go ahead
            CONFSTATUS_DISALLOW : Do not allow this event to continue
            CONFSTATUS_MINIMIZE : Application should minimize.

@Include: none
************************************************************************** */

CONFSTATUS WINAPI AskConfirmation (     // Ask for confirmation
    HWND            hDlg,               // [in] moma window
    CONFIRM         Event,              // [in] CONFIRM_xxx
    LPNPSOPTIONS    lpOptions           // [in] pointer to options
    )
{
    auto    CONFSTATUS  Status      =   CONFSTATUS_ALLOW;

    SYM_ASSERT ( hDlg );
    SYM_ASSERT ( lpOptions );

    if ( lpOptions )
        {
                                        // -----------------------------------
                                        // Check to see if we're to ask for
                                        // confirmation. I thought it was 
                                        // convenient to add this check here
                                        // instead of peppering the code with
                                        // checks everywhere.
                                        // -----------------------------------

        BOOL bDisplayConfirmation;

        switch ( Event )
            {
            case CONFIRM_EXIT:
                bDisplayConfirmation = lpOptions->Confirm.bOnExit;
                break;

            case CONFIRM_DELETE:
                bDisplayConfirmation = lpOptions->Confirm.bOnDelete;
                break;

            case CONFIRM_SAVE:
                bDisplayConfirmation = lpOptions->Confirm.bOnSave;
                break;

            case CONFIRM_EDIT:
                bDisplayConfirmation = lpOptions->Confirm.bOnEdit;
                break;

            default:
                SYM_ASSERT ( 0 );
                bDisplayConfirmation = FALSE;
                break;
            }

        if ( bDisplayConfirmation == TRUE )
            {
            Status = (CONFSTATUS)DialogBoxParam (
                                    ghInst,
                                    MAKEINTRESOURCE(IDD_ASKCONFIRMATION),
                                    hDlg,
                                    ConfirmDlgProc,
                                    (LPARAM)Event
                                    );

            SYM_ASSERT ( Status != -1 );
                                        // -----------------------------------
                                        // If high order bit is set then we're
                                        // not to ask this confirmation again.
                                        // -----------------------------------
            if ( Status & DONOTBUG )
                {
                                        // -----------------------------------
                                        // Clear it so we don't affect boolean
                                        // return value.
                                        // -----------------------------------
                Status &= ~DONOTBUG;

                switch ( Event )
                    {
                    case CONFIRM_EXIT:
                        lpOptions->Confirm.bOnExit = FALSE;
                        break;

                    case CONFIRM_DELETE:
                        lpOptions->Confirm.bOnDelete = FALSE;
                        break;

                    case CONFIRM_SAVE:
                        lpOptions->Confirm.bOnSave = FALSE;
                        break;

                    case CONFIRM_EDIT:
                        lpOptions->Confirm.bOnEdit = FALSE;
                        break;

                    default:
                        SYM_ASSERT ( 0 );
                        break;
                    }
                }
            }
        }

    return ( Status );
}

/* **************************************************************************
@Name:
BOOL CALLBACK ConfirmDlgProc (          // Process messages to confirm box
    HWND    hDlg,                       // [in] dialog box window handle
    UINT    uMessage,                   // [in[ message to process
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
    )

@Description:
This function processes all the messages from the confirmation dialog box.

@Parameters:
$hDlg$      handle of the confirmation dialog box.
$uMessage$  message identification number
$wParam$    depends on the message
$lParam$    depends on the message

@Include: none
************************************************************************** */

BOOL CALLBACK ConfirmDlgProc (          // Process messages to confirm box
    HWND    hDlg,                       // [in] dialog box window handle
    UINT    uMessage,                   // [in[ message to process
    WPARAM  wParam,                     // [in] ?
    LPARAM  lParam                      // [in] ??
    )
{
    auto    BOOL    bReturn = FALSE;

    switch ( uMessage )
        {
        case WM_INITDIALOG:
            bReturn = OnInitDialog ( hDlg, lParam );
            break;

        case WM_COMMAND:
            HANDLE_WM_COMMAND ( hDlg, wParam, lParam, OnCommand );
            bReturn = TRUE;
            break;

                                        // WM_SYSCOLORCHANGE:  For 3D
                                        // effect.
        case WM_SYSCOLORCHANGE:
            Ctl3dColorChange();
            bReturn = TRUE;
            break;
                                        // WM_CTLCOLOR:  For 3D effect.
        case WM_CTLCOLOR:
            bReturn = ((BOOL)Ctl3dCtlColorEx ( uMessage, wParam, lParam));
            break;

#ifdef SYM_WIN32
        case WM_HELP :                  // Fall through ...
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu
                                        // or standard context help:
                                        // ----------------------------------
            OnContextHelp ( hDlg, uMessage, wParam, lParam );
            break;
#endif
        }

    return ( bReturn );
}


/* **************************************************************************
@Name:
BOOL LOCAL OnInitDialog (               // Process WM_INITDIALOG message
    HWND    hDlg,                       // [in] from this window
    LPARAM  lParam                      // [in] app defined value
    )

@Description:
This function processes the WM_INITDIALOG message.

@Parameters:
$hDlg$      handle of the dialog that received the WM_INITDIALOG message
$lParam$    application define DWORD value

@Include: none
************************************************************************** */

BOOL LOCAL OnInitDialog (               // Process WM_INITDIALOG message
    HWND    hDlg,                       // [in] from this window
    LPARAM  lParam                      // [in] app defined value
    )
{
    const   CONFIRM     Event = (CONFIRM)lParam;
    auto    char        szMessage [ SYM_MAX_PATH ];
    auto    UINT        uMessageID = 0;
    auto    UINT        uCaptionID = 0;



    Ctl3dSubclassDlg ( hDlg, CTL3D_ALL );
                                        // -----------------------------------
                                        // If this is not a Exit confirmation
                                        // dialog box then adjust layout.
                                        // -----------------------------------
    if ( Event != CONFIRM_EXIT )
        NonExitDialogLayout ( hDlg );

    NG_CenterDialog ( hDlg, GetParent ( hDlg ) );


                                        // Setup the Fonts. Finds the correct
                                        // DBCS font if DBCSInstalled().
    NG_SetFontsInDialog (hDlg, 0);

                                        // -----------------------------------
                                        // ALter checkbox contents depending
                                        // on confirmation event.
                                        // Also set proper caption:
                                        // -----------------------------------
    switch ( Event )
        {
        case CONFIRM_EXIT:
            uMessageID = IDS_NPS_CONFIRM_EXIT;
            uCaptionID = IDS_NPS_CONFIRM_EXIT_CAPTION;
            break;

        case CONFIRM_DELETE:
            uMessageID = IDS_NPS_CONFIRM_DELETE;
            uCaptionID = IDS_NPS_CONFIRM_DEL_CAPTION;
            break;

        case CONFIRM_SAVE:
            uMessageID = IDS_NPS_CONFIRM_SAVE;
            uCaptionID = IDS_NPS_CONFIRM_ADD_CAPTION;
            break;

        case CONFIRM_EDIT:
            uMessageID = IDS_NPS_CONFIRM_EDIT;
            uCaptionID = IDS_NPS_CONFIRM_EDIT_CAPTION;
            break;

        default:
            SYM_ASSERT ( 0 );
            break;
        }

	ConfirmationBoxType = uMessageID;
										// -----------------------------------
                                        // ConfirmationBoxType will last beyond 
										// the scope of this function.
										// -----------------------------------
                                        
    *szMessage = EOS;
    LoadString ( ghInst, uMessageID, szMessage, sizeof(szMessage) );
    SYM_ASSERT ( *szMessage );

    Button_SetText ( GetDlgItem ( hDlg, IDC_CONFIRM_CHECKBOX ), szMessage );

                                        // -----------------------------------
                                        // The message to the user is always
                                        // uMessageID + 1
                                        // -----------------------------------
    *szMessage = EOS;
    LoadString ( ghInst, uMessageID + 1, szMessage, sizeof(szMessage) );
    SYM_ASSERT ( *szMessage );

    Static_SetText ( GetDlgItem ( hDlg, IDC_CONFIRM_MESSAGE ), szMessage );


										// -----------------------------------
                                        // Set the caption:
										// -----------------------------------
    *szMessage = EOS;
    LoadString ( ghInst, uCaptionID, szMessage, sizeof(szMessage) );
    SYM_ASSERT ( *szMessage );
    SetWindowText (hDlg, szMessage);

    return ( TRUE );
}

/* **************************************************************************
@Name:
VOID LOCAL OnCommand (                  // Process the WM_COMMAND message
    HWND    hDlg,                       // [in] from this window
    int     nID,                        // [in] control ID that notifies
    HWND    hwndCtl,                    // [in] control hwnd that notified
    UINT    codeNotify                  // [in] notify code
    )

@Description:
This function processes all the WM_COMMAND messages. The message cracker
provides all the arguments to this function.

@Parameters:
$hDlg$      handle of the dialog that received the WM_COMMAND message
$nID$       control ID that sent the message
$hwndCtl$   controls window handle
$codeNotify$ notification code that the control is sending.

@Include: none
************************************************************************** */

VOID LOCAL OnCommand (                  // Process the WM_COMMAND message
    HWND    hDlg,                       // [in] from this window
    int     nID,                        // [in] control ID that notifies
    HWND    hwndCtl,                    // [in] control hwnd that notified
    UINT    codeNotify                  // [in] notify code
    )
{
    if ( codeNotify == BN_CLICKED )
        {
        switch ( nID )
            {
            case IDOK:
            case IDCANCEL:
            case IDC_CONFIRM_MINIMIZE:
                {
                CONFSTATUS  Status = CONFSTATUS_DISALLOW;

                if ( nID == IDOK )
                    Status = CONFSTATUS_ALLOW;
                else if ( nID == IDC_CONFIRM_MINIMIZE )
                    Status = CONFSTATUS_MINIMIZE;

                if ( Button_GetCheck ( GetDlgItem (hDlg, IDC_CONFIRM_CHECKBOX) ) )
                    Status |= DONOTBUG;

                EndDialog ( hDlg, (int)Status );
                }
                break;

            case IDC_NPS_HELP:
                                        // -----------------------------------
										// Determine which kind of confirmation
										// this is.
                                        // -----------------------------------
                                        // -----------------------------------
										// For right now, all four kinds of help
										// will go into the same topic.  However,
										// to route them into different topics
										// change the IDH_ in the case below.
                                        // -----------------------------------
			    switch ( ConfirmationBoxType )
        			{
			        case IDS_NPS_CONFIRM_EXIT:
		                StdHelpFromDialog (hDlg,
                		              IDH_NAVW_SCHEDULER_CONFIRM_DLG_HELP_BTN);
                		break;
        			case IDS_NPS_CONFIRM_DELETE:
		                StdHelpFromDialog (hDlg,
                		              IDH_NAVW_SCHEDULER_CONFIRM_DLG_HELP_BTN);
                		break;
        			case IDS_NPS_CONFIRM_SAVE:
		                StdHelpFromDialog (hDlg,
                		              IDH_NAVW_SCHEDULER_CONFIRM_DLG_HELP_BTN);
                		break;
        			case IDS_NPS_CONFIRM_EDIT:
		                StdHelpFromDialog (hDlg,
                		              IDH_NAVW_SCHEDULER_CONFIRM_DLG_HELP_BTN);
                		break;

                                        // -----------------------------------
										// Non-matching ConfirmationBoxType will fall 
										// through.
										// -----------------------------------

					}					// End switch ( ConfirmationBoxType )
                break;
            }
        }
}

/* **************************************************************************
@Name:
VOID LOCAL NonExitDialogLayout (        // Adjust controls inside dialog
    HWND    hDlg                        // [in] dialog box to adjust
    )

@Description:
This function adjust the controls inside the confirmation dialog box. All
Exit confirmation features are removed.

@Parameters:
$hDlg$  handle of the dialog box to fix.

@Include: none
************************************************************************** */

VOID LOCAL NonExitDialogLayout (        // Adjust controls inside dialog
    HWND    hDlg                        // [in] dialog box to adjust
    )
{
    auto    RECT    rOldMinimize;
    auto    RECT    rOldCancel;
    auto    char    szOkText [ SYM_MAX_PATH ];

    GetWindowRect ( GetDlgItem ( hDlg, IDC_CONFIRM_MINIMIZE ), &rOldMinimize );
    GetWindowRect ( GetDlgItem ( hDlg, IDCANCEL ), &rOldCancel );

    const   UINT    uMoveUpUnits = rOldMinimize.bottom - rOldMinimize.top;

    DestroyWindow ( GetDlgItem ( hDlg, IDC_CONFIRM_MINIMIZE ) );

    MoveButton ( GetDlgItem ( hDlg, IDCANCEL ),           &rOldMinimize );
    MoveButton ( GetDlgItem ( hDlg, IDC_NPS_HELP ),       &rOldCancel   );
    MoveCtrlUp ( GetDlgItem ( hDlg, IDC_CONFIRM_CHECKBOX), uMoveUpUnits );

    ResizeDialog ( hDlg, uMoveUpUnits );

    LoadString ( ghInst, IDC_CONFIRM_OK_BUTTON, szOkText, sizeof(szOkText) );
    Button_SetText ( GetDlgItem ( hDlg, IDOK ), szOkText );
}

/* **************************************************************************
@Name:
VOID LOCAL MoveButton (                 // Move button to new rect.
    HWND    hButton,                    // [in] handle to button to move
    LPRECT  lpTarget                    // [in] target
    )

@Description:
This function moves a child control to the specified rectangular area.

@Parameters:
$hButton$  handle of the button to move
$lpTarget$  pointer to a rect structure where the control should be moved.

@Include: none
************************************************************************** */

VOID LOCAL MoveButton (                 // Move button to new rect.
    HWND    hButton,                    // [in] handle to button to move
    LPRECT  lpTarget                    // [in] target
    )
{
    SYM_ASSERT ( hButton  );
    SYM_ASSERT ( lpTarget );

    ScreenToClient ( GetParent ( hButton ), (LPPOINT)lpTarget );
    ScreenToClient ( GetParent ( hButton ), (LPPOINT)&lpTarget->right );

    MoveWindow ( hButton, lpTarget->left, lpTarget->top,
                 lpTarget->right - lpTarget->left,
                 lpTarget->bottom - lpTarget->top, FALSE );
}

/* **************************************************************************
@Name:
VOID LOCAL MoveCtrlUp (                 // Move the control up in dialog
    HWND    hCtrl,                      // [in] handle to control
    UINT    uUnits                      // [in] units to move up
    )

@Description:
This function moves a child control up with the specified number of units.

@Parameters:
$hCtrl$     handle of the control to move
$uUnits$    number of units to move the control up.

@Include: none
************************************************************************** */

VOID LOCAL MoveCtrlUp (                 // Move the control up in dialog
    HWND    hCtrl,                      // [in] handle to control
    UINT    uUnits                      // [in] units to move up
    )
{
    SYM_ASSERT ( hCtrl );
    SYM_ASSERT ( uUnits );

    auto    RECT    Rect;

    GetClientRect ( hCtrl, &Rect );

    ClientToScreen ( hCtrl, (LPPOINT)&Rect );
    ClientToScreen ( hCtrl, (LPPOINT)&Rect.right );

    ScreenToClient ( GetParent ( hCtrl ), (LPPOINT)&Rect );
    ScreenToClient ( GetParent ( hCtrl ), (LPPOINT)&Rect.right );

    MoveWindow ( hCtrl, Rect.left, Rect.top - uUnits,
                 Rect.right - Rect.left,
                 Rect.bottom - Rect.top, FALSE );
}

/* **************************************************************************
@Name:
VOID LOCAL ResizeDialog (               // Resize the dialog box
    HWND    hDlg,                       // [in] handle to the dialog box
    UINT    uUnits                      // [in] units to shrink with
    )

@Description:
This function shrinks the dialog box with the specified amount of units.

@Parameters:
$hDlg$      handle of the dialog to shrink
$uUnits$    number of units to shrink with

@Include: none
************************************************************************** */

VOID LOCAL ResizeDialog (               // Resize the dialog box
    HWND    hDlg,                       // [in] handle to the dialog box
    UINT    uUnits                      // [in] units to shrink with
    )
{
    SYM_ASSERT ( hDlg );
    SYM_ASSERT ( uUnits );

    auto    RECT    Rect;

    GetWindowRect ( hDlg, &Rect );

    MoveWindow ( hDlg, Rect.left, Rect.top,
                 Rect.right - Rect.left,
                 Rect.bottom - Rect.top - uUnits, FALSE );
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextHelp (
            HWND    hDlg,
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU and WM_HELP
windows message. The purpose of this function is to call the standard symantec help
routines that display an appropiate: floating help menu or context sensitive help

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextHelp (              // Respond to WM_CONTEXTMENU and WM_HELP
    HWND        hDlg,                   // [in] dialog that sent the message.
    UINT        uMsg,                   // [in] either WM_HELP or WM_CONTEXTMENU
    WPARAM      wParam,                 // [in] wParam from dialog
    LPARAM      lParam                  // [in] lParam from dialog
    )
{
                                        // -----------------------------------
                                        // This array is a cross reference
                                        // between a control id number and a
                                        // help id number that should be
                                        // displayed when the user selects the
                                        // 'What's This' menu entry.
                                        // -----------------------------------
    static  DWORD       dwWhatsThisMap[] =
        {
        IDC_CONFIRM_MESSAGE,
        IDH_NAVW_SCHEDULER_WHATHIS_CONFIRM_MESSAGE,

        IDC_CONFIRM_CHECKBOX,
        IDH_NAVW_SCHEDULER_WHATHIS_CONFIRM_CHECKBOX,

        0,
        0
        };



                                        // -----------------------------------
                                        // Invoke cmn Nav context sensitive help
                                        // -----------------------------------
    StdHelpExOnContextHelp (
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        dwWhatsThisMap,
                        NULL,
                        NULL,           //  This dlg. doesn't have any  ...
                        NULL            //  ... List Box controls - so NULL here
                        );
}

#endif // #if w32 only
