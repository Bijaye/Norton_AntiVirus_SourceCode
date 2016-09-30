/* Copyright 1991-1993 Symantec                                         */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPSUI/VCS/NPSADEDT.CPv   1.10   28 Aug 1998 17:19:46   JBRENNA  $ */
/*                                                                      */
/* Description:                                                         */
/*      This module contains the source directly related to the         */
/*      Add / Edit dialog box for the Norton Program Scheduler.         */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
//
// $Log:   S:/NPSUI/VCS/NPSADEDT.CPv  $
// 
//    Rev 1.10   28 Aug 1998 17:19:46   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.9   17 Apr 1998 12:19:06   mdunn
// Removed call to set an event's last-run time from NpsAddEditDlgDeInit().
//
//    Rev 1.8   30 Mar 1998 12:20:58   mdunn
// In NpsAddEditDlgDeInit(), an event's last-run time is set to the current
// time when the event is saved.
//
//    Rev 1.6   16 Jan 1998 21:48:18   vgloste
// Added code to prevent the RUN item from being added to the combo box
// if the user is not an administrator.
//
//    Rev 1.5   10 Sep 1997 16:29:46   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.4.1.2   14 Aug 1997 22:33:20   KSACKIN
// Added support for specifying in the schedule.ini whether or not to allow
// a blank action text, and if now, a custom error message per event type
// if you have a blank action text.
//
//    Rev 1.4.1.1   13 Aug 1997 22:28:22   KSACKIN
// Added support for default values for our Action Text and our Description
// field.
//
//    Rev 1.4.1.0   12 Aug 1997 22:55:50   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.4   15 Jul 1997 15:50:30   DBUCHES
// Added Ctl3d for DEC Alpha
//
//    Rev 1.3   12 Jul 1997 00:33:36   KSACKIN
// Fixed some bogus event year handling.  We now store the entire 4 number year
// instead of the 2 character shorthand year.
//
//    Rev 1.2   10 Jul 1997 11:25:48   KSACKIN
// Added a version check to make sure that we are running Windows major version
// 4.0 or greater to use the new OFN_EXPLORER switch with our file browser.
// Under NT3.51, this flag would cause the file browser dialog to not come up
// at all.
//
//    Rev 1.1   08 Jul 1997 14:07:12   KSACKIN
// Added a LimitText call to the NPS AddEdit dialog's Description edit text.
//
//    Rev 1.0   19 May 1997 00:25:02   KSACKIN
// Initial revision.
//
//    Rev 1.45   07 May 1997 18:51:46   BILL
// Removed ctl3d for Alpha platform
//
//    Rev 1.44   03 Dec 1996 16:01:30   JBRENNA
// Kirin (QAKF) port rev 1.43.1.1
//
//    Rev 1.43.1.1   22 Nov 1996 10:49:48   JBRENNA
// GetDlgItemText() does not correctly truncate DBCS strings. It removes
// trail bytes leaving lead bytes. I only fixed fetching the user's
// event description string in NpsAddEditDlgDeInit(). Really, a wrapper for
// GetDlgItemText() should be created that correctly truncates DBCS strings.
//
//    Rev 1.43   02 Oct 1996 18:32:24   GWESTER
// Fixed so that DBCS programs and directories are recognized by
// Scheduler when it validates command lines.
//
//    Rev 1.42   18 Sep 1996 10:16:38   JBRENNA
// Add NG_SetFontsInDialog() call when processing WM_INITDIALOG messages.
// This ensures that a DBCS font is used when necessary.
//
//    Rev 1.41   22 Feb 1996 13:17:38   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' and 'StdHelpOnContextHelp()' to
// 'StdHelpExOnContextHelp()' to use Quake's StdHelp merged with the NAV
// version.
//
//    Rev 1.40   11 Jan 1996 09:48:58   JWORDEN
// Port changes from Quake 8
//
//    Rev 1.39.1.1   30 Nov 1995 21:03:34   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.39   01 Jul 1995 16:33:50   MARKL
// I believe I found where all of the LFN processing was broken.  It turns out
// that the code had been ifdef 0-ed in previous areas to allow it to simply
// run instead of fixing all of the LFN issues.  Items that I am aware of
// fixing (although quickly for the build):
//   1)  When browsing, you cannot pass quoted file specs to the browse
//       dialogs.
//   2)  When returning from a browse, if the spec is a LFN with a space
//       in it, the spec needs to be surrounded by quotes.
//   3)  When parsing parameters, go after the first space on SFN but
//       after the quoted spec on an LFN with a space.
//
//    Rev 1.38   14 Jun 1995 20:14:38   GWESTER
// Removed Help button from "Browse" dlg.
//
//    Rev 1.37   05 May 1995 09:45:10   KEITH
// Fix the DBCS implementation.
//
//
/************************************************************************/

#include    "platform.h"
#include    "stdhelp.h"
#include    <commdlg.h>
#include    <ctl3d.h>
#include    "undoc.h"                  /* for extra typedefs           */
#include    "file.h"
#include    "xapi.h"
#include    "msgdisp.h"                    /* error message stuff          */
#include    "ngui.h"
#include    "process.h"

#ifdef SYM_WIN32
    #include "browsdlg.h"
#else
    #include "stddir.h"
#endif

#include    "symintl.h"
#include    "nps.h"                     /* scheduler includes           */
#include    "npsres.h"
#include    "npsstb.h"
#include    "nps_hlp.h"
#include    "dbcs.h"





//***************************************************************************
// Local constants
//***************************************************************************
#undef TRACE
#define TRACE           ERR_TRACE_ID (_NPSADEDT_C)
#define DISPLAY_AMPM    !(BOOL)GetProfileInt ("Intl", "iTime", 0)


//***************************************************************************
// Local Typedefs
//***************************************************************************
typedef struct NamedEventItemTag
    {
    char        szName[SSIZE];
    char        szPrompt[SSIZE];
    char        szCmdLine[MAX_CMDLINE];
    char        szStartupDir[SYM_MAX_PATH];
    char        szDefActionText[SSIZE];
    char        szDefDesc[SSIZE];
    char        szBlankActionTextMessage[SYM_MAX_PATH];
    WORD        wRunStyle;
    BOOL        bAllowBlankActionText;
    } NAMEDEVENTITEM, FAR * LPNAMEDEVENTITEM;


//***************************************************************************
// Local variables
//***************************************************************************
int    iPrompt1y;
int    iPrompt2y;
int    iPrompt3y;
int    iField1y;
int    iField2y;
int    iField3y;
int    iGroupBottom;


/************************************************************************/
/* Local component prototypes                                           */
/************************************************************************/
WORD LOCAL PASCAL NpsEventBrowse (      /* User browses for file        */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD LOCAL PASCAL NpsNamedEventListInit(        // Initializes action combo
    HWND        hDlg,                   // [in] Handle of dialog box
    WORD        wId,                    // [in] ID of action combo list box
    LPSTR       lpszNamed);             // [in] Named event name

WORD LOCAL PASCAL NpsNamedEventSelChange(       // Action type has changed
    HWND        hDlg,                   // [in] Handle of dialog box
    WORD        wId,                    // [in] ID of action combo list box
    BOOL        bInitializingDialog);   // [in] States whether we are initializing

WORD LOCAL PASCAL NpsNamedEventsLoad (  // Loads up the action name listbox
    HWND        hWnd);                  // [in] Handle to the action listbox

WORD LOCAL PASCAL NpsNamedEventsUnload (// Unloads the action name listbox
    HWND        hWnd);                  // [in] Handle to the action listbox

WORD LOCAL PASCAL NpsAddEditDlgDeInit ( /* Gets event from dlg cntrls   */
    HWND        hDlg);                  /* Handle to dialog box         */

WORD LOCAL PASCAL NpsValidateCommand (  /* Validate command             */
    HWND        hDlg,                   /* Handle to dialog box         */
    LPSTR       szCmd,                  /* Command to validate          */
    LPSTR       szStartupDir);          /* Optional startup directory   */

VOID LOCAL NpsFileBrowseFiltersGet(     // Returns browse file filter spec
    LPSTR       lpszzBuffer,            // [out] Receives filter specification
    WORD        wBufSize);              // [in] Size of buffer

WORD LOCAL NpsProgramExtsGet(           // Returns list of exe extensions
    LPSTR       lpszBuffer);            // [out] Buffer to receive ext list

WORD LOCAL PASCAL NpsAddEditControlsDisable ( /* Turns controls on/off  */
    HWND        hDlg,                   /* Handle to dialog box         */
    WORD        wFreq);                 /* Current frequency of event   */

WORD LOCAL PASCAL NpsStartupDirBrowse ( /* Browse for startup dir */
    HWND        hDlg);                  /* Handle to dailog box         */

static WORD NpsMoveControl(             // Move control within dialog box
    HWND    hDlg,                       // [in] Handle to dialog box
    WORD    wCtl,                       // [in] ID of control to move
    int     iOffsetY);                  // [in] New Y-position

static WORD NpsLimitDaysOfMonth(        // Set limit on days of the month
    HWND    hDlg);                      // [in] Handle to dialog box

BOOL PASCAL NpsVerifyAddEditDlgContent( /* Verify the contents of dialog*/
    HWND        hDlg                    /* Handle to dailog box         */
    );

VOID LOCAL OnContextHelp (              // Respond to WM_CONTEXTMENU and WM_HELP
    HWND        hDlg,                   // [in] dialog that sent the message.
    UINT        uMsg,                   // [in] either WM_HELP or WM_CONTEXTMENU
    WPARAM      wParam,                 // [in] wParam from dialog
    LPARAM      lParam                  // [in] lParam from dialog
    );


BOOL LOCAL IsAdminMode(void);           // Indicates if the current user is
                                        // running in "Admin" mode or not


/************************************************************************/
/* NPSADEDT routines                                                    */
/************************************************************************/




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This dialog procedure allows the user to enter or modify        */
/*      settings for an event.  This dialog box collects this           */
/*      information whenever the user wishes to Add a new event,        */
/*      Edit an existing event, or Copy and event.                      */
/*                                                                      */
/* NOTE:                                                                */
/*      Event[nCurEvent].wType must be set in real time as user         */
/*      interacts with controls for the NpsEvent? routines to work      */
/*      correctly.                                                      */
/*                                                                      */
/* BOOL SYM_EXPORT CALLBACK NpsAddEditDlgProc  (hDlg, -- dialog handle      */
/*                                       wMessage,                      */
/*                                       wParam,                        */
/*                                       lParam)                        */
/* Return values:                                                       */
/*      EndDialog (hDlg, FALSE)         -- Event is not to be processed */
/*                                         by dialog creator.           */
/*      EndDialog (hDlg, TRUE)          -- Event is to be processed by  */
/*                                         dialog creator.              */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/18/91 MSL Began cleanup pass.  Removed bGotEvent; results now     */
/*              returned as part of EndDialog() call.                   */
/*              Return(TRUE) and Return(FALSE) were backwards causing   */
/*              unpredictable results.  Also removed uncessary returns. */
/*              Fixed missing default processing on switch().           */
/* 03/05/91 MSL Resources now in NWRES.                                 */
/* 03/29/91 MSL Now calls ShadowDlgCreate() for that Norton Look.       */
/* 01/16/92 MSL Run Minized check box moved to common options dialog.   */
/*              Added NpsCommonOptionsDlgProc skeleton.                 */
/* 07/02/93 KDY Added BOOL typecast to return of Ctl3dColorEx.          */
/************************************************************************/
BOOL SYM_EXPORT CALLBACK NpsAddEditDlgProc( /* Add / Edit event settings    */
    HWND        hDlg,                   /* Handle to dialog box         */
    UINT        wMessage,               /* Message to process           */
    WPARAM      wParam,                 /* WPARAM data for message      */
    LPARAM      lParam)                 /* LPARAM data for message      */
{
    WORD                wFreq;
    extern      NPSEVNT Events[];       /* The actual events            */
    extern      HWND    hDlgModelessActive;
    static      WORD    wBrowseHelpID = 0;
    static      UINT    uMode = 0;
    static      BOOL    bInitializingDialog = TRUE;

    switch (wMessage)
        {
                                        /* WM_INITDIALOG:  Initialize   */
                                        /* the dialog box and return    */
                                        /* TRUE to let Windows set      */
                                       /* focus to our first control.  */
        case WM_INITDIALOG:
            bInitializingDialog = TRUE;
            NG_CenterDialog (hDlg, GetParent (hDlg));

            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            uMode = LOWORD(lParam) - IDS_NPS_EVENT_ADD;

                                        // Setup the Fonts. Finds the correct
                                        // DBCS font if DBCSInstalled().
            NG_SetFontsInDialog (hDlg, 0);

            NpsAddEditCtlsInit (hDlg, TRUE);
            NpsAddEditDlgInit (hDlg, LOWORD ((DWORD)lParam));
            NpsAddEditControlsDisable (hDlg, Events[nCurEvent].wFreq);
            SetFocus ( GetDlgItem ( hDlg, IDC_NPS_DESCRIPTION ));
            Edit_SetSel ( GetDlgItem ( hDlg, IDC_NPS_DESCRIPTION ), 0, -1 );
            return (FALSE);
            break;

                                        // WM_SYSCOLORCHANGE:  Needed for
                                        // CTL3D.DLL.
        case WM_SYSCOLORCHANGE:
            Ctl3dColorChange();
            break;
                                        // WM_CTLCOLOR:  Needed for CTL3D.DLL
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx (wMessage, wParam, lParam));

                                        /* WM_WININICHANGE:  A change   */
                                        /* in the internationalization  */
                                        /* stuff could screw us up so   */
                                        /* make adjustments.            */
        case WM_WININICHANGE:
            NpsInitTime();
            NpsAddEditCtlsInit (hDlg, FALSE);
           break;

#ifdef SYM_WIN16
                                        // WM_ACTIVATE:  Another help
                                        // kluge for F1.
        case WM_ACTIVATE:
            if (GET_WM_ACTIVATE_STATE(wParam, lParam))
                {
                StdHelpSetContext (NPS_HELP_DLG_ADDEDIT,
                                   NPS_HELP_DLG_ADDEDIT);
                }
            return (FALSE);
#endif

#ifdef SYM_WIN32
        case WM_HELP :                  // Fall through ...
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu
                                        // or standard context help:
                                        // ----------------------------------
            OnContextHelp ( hDlg, wMessage, wParam, lParam );
            break;
#endif

                                        /* WM_COMMAND:  User has        */
                                        /* interacted with a dialog box */
                                        /* control, process accordingly.*/
        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam))
                {
                                        // IDC_NPS_MONTH:  Limit the possible days of
                                        // the month based on the current month selected.
                case IDC_NPS_MONTH:
                    NpsLimitDaysOfMonth(hDlg);
                    break;
                                        /* IDCANCEL:  Get out of here.  */
                                        /* User wants to bug out.       */
                case IDCANCEL:
                    NpsNamedEventsUnload (
                        GetDlgItem (hDlg, IDC_NPS_ACTIONTYPE));
                    EndDialog (hDlg, FALSE);
                    break;

                                        /* IDOK:  User has finished     */
                                        /* updating settings for event. */
                case IDOK:
                                        // First, verify all content is OK.
                                        // This will do things like checking
                                        // for blank action text.  This will
                                        // return TRUE if everything is OK.
                    if ( TRUE == NpsVerifyAddEditDlgContent( hDlg ) )
                        {
                        if (NpsAddEditDlgDeInit (hDlg) == NOERR)
                            {
                            NpsNamedEventsUnload (
                                GetDlgItem (hDlg, IDC_NPS_ACTIONTYPE));
                            EndDialog (hDlg, TRUE);
                            }
                        }
                    break;

                                        // IDC_NPS_ACTIONTYPE:  The user is
                                        // changing the type of the action
                                        // so we'll need to enable/disable
                                        // controls appropriately.
                case IDC_NPS_ACTIONTYPE:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == CBN_SELCHANGE)
                        {
                                        // Our first change will be setting the
                                        // combo box position.  We don't want
                                        // to set default text for the initial
                                        // set, just all of the ones after this.
                        NpsNamedEventSelChange (hDlg, IDC_NPS_ACTIONTYPE, bInitializingDialog);
                        bInitializingDialog = FALSE;

                        }
                    break;

                                        /*  IDC_NPS_FREQ:  The frequency  */
                                        /* of the event is being changed*/
                                        /* so update the frequency.     */
                case  IDC_NPS_FREQ:
                    wFreq = ComboBox_GetCurSel (GetDlgItem (hDlg,  IDC_NPS_FREQ));
                    NpsAddEditControlsDisable (hDlg, wFreq);
                    break;

                                        /* IDC_NPS_CMD_TEXT: The user   */
                                        /* wishes to browse for a file  */
                                        /* to launch when the event gets*/
                                        /* triggered.                   */
                case  IDC_NPS_CMD_TEXT:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == TN_TUNNEL)
                        {
                        wBrowseHelpID = IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN;
                        NpsEventBrowse (hDlg);
                        }
                    break;

                                        /*  IDC_NPS_STARTUPDIR_TEXT:  The */
                                        /* user wishes to browse for a  */
                                        /* startup directory.           */
                case  IDC_NPS_STARTUPDIR_TEXT:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == TN_TUNNEL)
                        {
                        wBrowseHelpID = IDH_NAVW_SCHEDULER_WHATHIS_MAIN_WINDOW_UNKNOWN;
                        NpsStartupDirBrowse (hDlg);
                        }
                    break;

                                        /* IDHELP:  User wishes to get  */
                                        /* help on this dialog box.     */
                case IDC_NPS_HELP:
                    StdHelpFromDialog (hDlg,
                                       IDH_NAVW_SCHEDULER_ADD_EVENT_DLG_HELP_BTN);
                    break;

                                        /* default:  We didn't process  */
                                        /* the WM_COMMAND so Windows    */
                                        /* must do so.                  */
                default:
                    return (FALSE);
                }

                                        /* default:  We didn't process  */
                                        /* a message so let Windows     */
                                        /* handle it.                   */
        default:
            return (FALSE);
        }

                                        /* We processed the message so  */
                                        /* tell Windows not to bother   */
                                        /* with it.                     */
    return (TRUE);
} /* NpsAddEditDlgProc() */


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine verifies the contents of the Add / Edit Dialog box.*/
/*      This will be used before our OK message is processed.  If this  */
/*      function returns FALSE, we should not process the OK message.   */
/*                                                                      */
/* Return values:                                                       */
/*      TRUE   -- Contents are valid.                                   */
/*      FALSE  -- An error has occurred.
/*                                                                      */
/************************************************************************/
/* 08/14/97 ksackin Function created.                                       */
/************************************************************************/
BOOL PASCAL NpsVerifyAddEditDlgContent( /* Verify the contents of dialog*/
    HWND        hDlg                    /* Handle to dailog box         */
    )
{
LPNAMEDEVENTITEM    lpNamedEventItem = NULL;
HWND                hWndCombo = GetDlgItem (hDlg, IDC_NPS_ACTIONTYPE);
WORD                wIndex;
TCHAR               szActionText[262];
TCHAR               szMessageTitle[262];
BOOL                bRetVal = TRUE;

    wIndex = ComboBox_GetCurSel (hWndCombo);

                                        // If this is a named event...
    if ( wIndex + 1 >= EVENT_TYPE_NAMED )
        {
        lpNamedEventItem = (LPNAMEDEVENTITEM)ComboBox_GetItemData (hWndCombo, wIndex);

                                        // Only if we don't want to allow
                                        // blanks in the Action Text field...
        if ( FALSE == lpNamedEventItem->bAllowBlankActionText )
            {
                                        // Get the text from the edit control.
                                        // This also returns the number of
                                        // characters copied into the buffer.
                                        // Check for zero characters here.
            if ( 0 == GetDlgItemText( hDlg, IDC_NPS_ACTION_TEXT, szActionText, 262 ) )
                {
                                        // String length was zero, so put up
                                        // custom message saying that we can't
                                        // have this.
                LoadString( ghInst, IDS_NPS_TITLE, szMessageTitle, 262 );
                MessageBox( hDlg, lpNamedEventItem->szBlankActionTextMessage, szMessageTitle, MB_OK | MB_ICONEXCLAMATION );

                                        // Return FALSE so that we won't
                                        // process OK message.
                bRetVal = FALSE;
                }
            }
        }

    return bRetVal;
}

/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine initializes the Add / Edit Dialog box using the    */
/*      values found for the current event.                             */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Function completed okay.     */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/25/91 MSL Made cleanup pass.  Renamed to NpsAddModifyDlgInit().   */
/*              Interacts with the new spin buttons.                    */
/* 02/26/91 MSL Uses format string for minutes in SB_FINDNUM.           */
/* 03/01/91 MSL Title dynamically changes based on whether you are      */
/*              Adding, editing, or copying an event.                   */
/* 03/05/91 MSL Resources now in NWRES.                                 */
/* 04/22/91 MSL Now initializes Run Minimized check box.                */
/* 01/19/92 MSL Initializes lpEventOpts for options processing.         */
/* 04/19/93 MSL Dialog completely reworked.                             */
/* 06/10/93 KDY Added support for 'yearly' events and bSound flag       */
/************************************************************************/
WORD PASCAL NpsAddEditDlgInit(          /* Initializes Add / Edit Dlg   */
    HWND        hDlg,                   /* Handle to dailog box         */
    WORD        wCaptionStrID)          /* What caption to display      */
{
    auto        char        szCaption[BSIZE];
    auto        char        szDateTimeBuf[INTL_MAXLEN_sSHORTDATE];
    auto        RECT        crWnd;
    extern      NPSEVNT     Events[];
    extern      char        szTimeSep[];

                                        // Set y coordinate of frequency
                                        // prompts and fields and group
                                        // box so we can move them later.

    GetWindowRect(GetDlgItem(hDlg, IDC_NPS_DATE_PROMPT), &crWnd);
    NpsScreenToClient(hDlg, &crWnd);
    iPrompt1y = crWnd.top;
    GetWindowRect(GetDlgItem(hDlg, IDC_NPS_DATE), &crWnd);
    NpsScreenToClient(hDlg, &crWnd);
    iField1y = crWnd.top;

                                        // Schedule group box.
    iField2y = crWnd.left;
    iField3y = crWnd.bottom - crWnd.top;
    GetWindowRect(GetDlgItem(hDlg, IDC_NPS_SCHEDULE_GROUP), &crWnd);
    NpsScreenToClient(hDlg, &crWnd);
    iGroupBottom = iField3y + iField2y - crWnd.left;

    GetWindowRect(GetDlgItem(hDlg, IDC_NPS_TIME_PROMPT), &crWnd);
    NpsScreenToClient(hDlg, &crWnd);
    iPrompt2y = crWnd.top;
    GetWindowRect(GetDlgItem(hDlg, IDC_NPS_TIME), &crWnd);
    NpsScreenToClient(hDlg, &crWnd);
    iField2y = crWnd.top;

    GetWindowRect(GetDlgItem(hDlg, IDC_NPS_MONTH_PROMPT), &crWnd);
    NpsScreenToClient(hDlg, &crWnd);
    iPrompt3y = crWnd.top;
    GetWindowRect(GetDlgItem(hDlg, IDC_NPS_MONTH), &crWnd);
    NpsScreenToClient(hDlg, &crWnd);
    iField3y = crWnd.top;

                                        // Set the correct fields to small
                                        // font.
    NpsSmallFontSet (hDlg,  IDC_NPS_DESCRIPTION );
    NpsSmallFontSet (hDlg,  IDC_NPS_MSG_TEXT);
    NpsSmallFontSet (hDlg,  IDC_NPS_CMD_TEXT);
    NpsSmallFontSet (hDlg,  IDC_NPS_ACTION_TEXT);
    NpsSmallFontSet (hDlg,  IDC_NPS_STARTUPDIR_TEXT);
    NpsSmallFontSet (hDlg,  IDC_NPS_RUNSTYLE);
    NpsSmallFontSet (hDlg,  IDC_NPS_FREQ);
    NpsSmallFontSet (hDlg,  IDC_NPS_DATE);
    NpsSmallFontSet (hDlg,  IDC_NPS_TIME);
    NpsSmallFontSet (hDlg,  IDC_NPS_DAY);
    NpsSmallFontSet (hDlg,  IDC_NPS_MINUTES);
    NpsSmallFontSet (hDlg,  IDC_NPS_DAYOFMONTH);
    NpsSmallFontSet (hDlg,  IDC_NPS_MONTH);

                                        /* Get and set our caption      */
    if (!LoadString (ghInst, wCaptionStrID, szCaption, BSIZE))
        {
                STRCPY (szCaption, szTitle);
        }
    SetWindowText (hDlg, szCaption);

                                        /* Set if the event is enabled. */
    CheckDlgButton (hDlg,  IDC_NPS_ENABLEEVENT, Events[nCurEvent].bEnabled);

                                        /* Set if the sound is enabled. */
    CheckDlgButton (hDlg,  IDC_NPS_SOUND, Events[nCurEvent].bSound);

                                        /* Set the description for this */
                                        /* event.                       */
    Edit_LimitText (GetDlgItem (hDlg,  IDC_NPS_DESCRIPTION), MAX_DESCR - 1);
    SetDlgItemText (hDlg,  IDC_NPS_DESCRIPTION, Events[nCurEvent].szDescr);

    Edit_LimitText (GetDlgItem (hDlg,  IDC_NPS_MSG_TEXT), MAX_CMDLINE - 1);
    Edit_SetText (GetDlgItem (hDlg,  IDC_NPS_MSG_TEXT),
                  Events[nCurEvent].szMessage);
    Edit_LimitText (GetDlgItem (hDlg,  IDC_NPS_CMD_TEXT), MAX_CMDLINE - 1);
    Edit_SetText (GetDlgItem (hDlg,  IDC_NPS_CMD_TEXT),
                  Events[nCurEvent].szCmdLine);
    Tunnel_LimitText (GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT), SYM_MAX_PATH);
    Tunnel_SetText (GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT),
                    Events[nCurEvent].szStartupDir);
    Edit_LimitText (GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT), MAX_CMDLINE - 1);
    Edit_SetText (GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT),
                  Events[nCurEvent].szPromptParams);
    ComboBox_SetCurSel (GetDlgItem (hDlg,  IDC_NPS_RUNSTYLE),
                        Events[nCurEvent].wRunStyle);

                                        /* Select the correct frequency */
                                        /* based on our indexes being   */
                                        /* the same as the combo boxes'.*/
    ComboBox_SetCurSel (GetDlgItem (hDlg,  IDC_NPS_FREQ),
                        Events[nCurEvent].wFreq);

                                        // Set the current date.
    NSetIntlDate (szDateTimeBuf,
                  Events[nCurEvent].wMonth + 1,
                  Events[nCurEvent].wDate,
                  Events[nCurEvent].wYear);
    Edit_SetText (GetDlgItem (hDlg,  IDC_NPS_DATE), szDateTimeBuf);
    Edit_LimitText (GetDlgItem (hDlg,  IDC_NPS_DATE),
                    sizeof (szDateTimeBuf));

                                        // Set the current time.
    NSetIntlTime (szDateTimeBuf, Events[nCurEvent].wHour,
                  Events[nCurEvent].wMin, 0, FALSE, DISPLAY_AMPM);
    Edit_SetText (GetDlgItem (hDlg,  IDC_NPS_TIME), szDateTimeBuf);
    Edit_LimitText (GetDlgItem (hDlg,  IDC_NPS_TIME),
                    sizeof (szDateTimeBuf));

                                        // Set the current Day.
    SpinButton_SetCurSel (GetDlgItem (hDlg,  IDC_NPS_DAY),
                          Events[nCurEvent].wDay);

                                        // Set the current minutes.
    SpinButton_SetCurSel (GetDlgItem (hDlg,  IDC_NPS_MINUTES),
                          Events[nCurEvent].wMin);

                                        // Set the current Month.
    SpinButton_SetCurSel (GetDlgItem (hDlg,  IDC_NPS_MONTH),
                          Events[nCurEvent].wMonth);

                                        // Set the current Day of Month.
    SpinButton_SetCurSel (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH),
                          Events[nCurEvent].wDate-1);

                                        // Initialize the action type
                                        // combo box and all of its
                                        // related controls.  NOTE:  This
                                        // must be the last thing done!!!
    NpsNamedEventListInit (hDlg, IDC_NPS_ACTIONTYPE,
                           Events[nCurEvent].szNamed);

                                        // Return to caller
    return (NOERR);
}




//***************************************************************************
// NpsNamedEventListInit()
//
// Description:
//      This routine initializes the Action Type Combo List Box.  It loads
// the list box with the defaults (Display Message, Run Program) and also
// those entries found in the SCHEDULE.INI file.  Once the combobox is loaded,
// the action is named as per passed in the lpszNamed parameter.
//
// Parameters:
//      hDlg                            [in] Handle of dialog box
//      wId                             [in] ID of action combo list box
//      lpszNamed                       [in] Named event name
//
// Return Value:
//      NOERR                           Everything went fine
//      ERR                             There were problems
//
//***************************************************************************
// 09/28/1993 MARKL Function Created.
//***************************************************************************
WORD LOCAL PASCAL NpsNamedEventListInit(        // Initializes action combo
    HWND        hDlg,                   // [in] Handle of dialog box
    WORD        wId,                    // [in] ID of action combo list box
    LPSTR       lpszNamed)              // [in] Named event name
{
    auto        WORD            wRet             = NOERR;
    auto        HWND            hWnd             = GetDlgItem (hDlg, wId);
    auto        int             wIndex;
    extern      HINSTANCE       ghInst;
	auto		BOOL			bRunItem;


                                        // Find out if we're running in Admin
                                        // mode or not
    bRunItem = IsAdminMode();


                                        // At least put in our two defaults
                                        // in to the list.
    ComboBox_AddString (hWnd, szEventMessage);
										// Only add the "run" command if
										// the security allows
	if(bRunItem)
	    ComboBox_AddString (hWnd, szEventProgram);

                                        // Supply all of the Named Events
                                        // that are available to the user
                                        // to choose from.
    NpsNamedEventsLoad (hWnd);

                                        // Force our dialog to paint such
                                        // that the hiding/showing of various
                                        // controls will work smoothly.
    UpdateWindow (hDlg);

                                        // For the current event, select
                                        // the correct item in the combobox.
                                        // If there is not a match, default
                                        // to "Run Program".
    wIndex = ComboBox_FindStringExact (hWnd, -1, lpszNamed);

    if (wIndex == CB_ERR)
        {
        wIndex = ComboBox_FindStringExact (hWnd, -1, szEventProgram);
        }
    if (wIndex == CB_ERR)
        {
        wIndex = ComboBox_FindStringExact (hWnd, -1, szEventMessage);
        }

    if (wIndex != CB_ERR)
        {
        ComboBox_SetCurSel (hWnd, wIndex);
        ComboBox_SelectString (hWnd, -1, lpszNamed);
        PostMessage (hDlg,
                     WM_COMMAND,
                     GET_WM_COMMAND_MPS(wId, hWnd, CBN_SELCHANGE));
        }

                                        // return to caller
    return (wRet);
} // NpsNamedEventListInit()



//***************************************************************************
// NpsNamedEventSelChange()
//
// Description:
//      This routine is called when the user decides to change the type
// of the current event.
//
// Parameters:
//      hDlg                            [in] Handle of dialog box
//      wId                             [in] ID of action combo list box
//
// Return Value:
//      NOERR                           Everything went fine
//      ERR                             There were problems
//
//***************************************************************************
// 09/28/1993 MARKL Function Created.
//***************************************************************************
WORD LOCAL PASCAL NpsNamedEventSelChange(       // Action type has changed
    HWND        hDlg,                   // [in] Handle of dialog box
    WORD        wId,                    // [in] ID of action combo list box
    BOOL        bInitializingDialog)    // [in] States whether we are initializing
{
	auto		BOOL			bRunItem;
    auto        WORD            wRet             = NOERR;
    auto        WORD            wEventType;
    auto        HWND            hWnd             = GetDlgItem (hDlg, wId);
    auto        HWND            hWndCtl;
    auto        int             wIndex;
    auto        int             nItem;
    auto        LPNAMEDEVENTITEM lpNamedEventItem;
    extern      NPSEVNT         Events[];



                                        // Find out if we're running in Admin
                                        // mode or not
    bRunItem = IsAdminMode();

    wIndex = ComboBox_GetCurSel (hWnd);
    if (wIndex != CB_ERR)
        {
                                        // What is the name of this action
                                        // type?
        ComboBox_GetLBText (hWnd,
                            wIndex,
                            Events[nCurEvent].szNamed);

        wEventType = wIndex + 1;

        if ( wEventType > EVENT_TYPE_NAMED )
            {
            wEventType = EVENT_TYPE_NAMED;
            }

	if ( (wEventType == 2) && (!bRunItem) )
		wEventType = EVENT_TYPE_NAMED;

        switch ( wEventType )
            {
            case EVENT_TYPE_MESSAGE:

                                        // We need to change the event and
                                        // the appearance of the dialog
                                        // based on what the user has
                                        // selected as his new event type.
                                        // MESSAGE:  The user has chosen
                                        // to display a message.
                Events[nCurEvent].wType = EVENT_TYPE_MESSAGE;

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_CMD_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_CMD_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_RUNSTYLE_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_RUNSTYLE);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_MSG_PROMPT);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_MSG_TEXT);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                if ( FALSE == bInitializingDialog )
                    {
                    SetDlgItemText (hDlg,
                                    IDC_NPS_DESCRIPTION,
                                    _T(""));
                    SetDlgItemText (hDlg,
                                    IDC_NPS_ACTION_TEXT,
                                    _T(""));
                    }
                break;

            case EVENT_TYPE_PROGRAM:
                                        // PROGRAM:  The user has chosen
                                        // to launch a program event.
                Events[nCurEvent].wType = EVENT_TYPE_PROGRAM;

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_MSG_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_MSG_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_CMD_PROMPT);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_CMD_TEXT);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_PROMPT);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_RUNSTYLE_PROMPT);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_RUNSTYLE);
                ShowWindow (hWndCtl, SW_SHOW);
                Button_Enable (hWndCtl, TRUE);

                if ( FALSE == bInitializingDialog )
                    {
                    SetDlgItemText (hDlg,
                                    IDC_NPS_DESCRIPTION,
                                    _T(""));
                    SetDlgItemText (hDlg,
                                    IDC_NPS_ACTION_TEXT,
                                    _T(""));
                    }

                break;

            default:
                                        // NAMED:  The user has chosen to
                                        // use a named action/event category.
                Events[nCurEvent].wType = EVENT_TYPE_NAMED;

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_CMD_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_CMD_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_RUNSTYLE_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_RUNSTYLE);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_MSG_PROMPT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                hWndCtl = GetDlgItem (hDlg,  IDC_NPS_MSG_TEXT);
                ShowWindow (hWndCtl, SW_HIDE);
                Button_Enable (hWndCtl, FALSE);

                nItem = (int)ComboBox_GetItemData (hWnd, wIndex);
                lpNamedEventItem = (LPNAMEDEVENTITEM)nItem;

                if (lpNamedEventItem && nItem != CB_ERR )
                    {
                    SetDlgItemText (hDlg,
                                IDC_NPS_ACTION_PROMPT,
                                lpNamedEventItem->szPrompt);

                                        // Only set the description and action
                                        // text to the default values if we
                                        // are currently adding a new event.
                    if ( FALSE == bInitializingDialog )
                        {
                        SetDlgItemText (hDlg,
                                        IDC_NPS_DESCRIPTION,
                                        lpNamedEventItem->szDefDesc);
                        SetDlgItemText (hDlg,
                                        IDC_NPS_ACTION_TEXT,
                                        lpNamedEventItem->szDefActionText);
                        }

                    if (*(lpNamedEventItem->szPrompt) != EOS)
                        {
                        hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_PROMPT);
                        ShowWindow (hWndCtl, SW_SHOW);
                        Button_Enable (hWndCtl, TRUE);

                        hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT);
                        ShowWindow (hWndCtl, SW_SHOW);
                        Button_Enable (hWndCtl, TRUE);
                        }
                    else
                        {
                        hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_PROMPT);
                        ShowWindow (hWndCtl, SW_HIDE);
                        Button_Enable (hWndCtl, FALSE);

                        hWndCtl = GetDlgItem (hDlg,  IDC_NPS_ACTION_TEXT);
                        ShowWindow (hWndCtl, SW_HIDE);
                        Button_Enable (hWndCtl, FALSE);
                        }
                    }
                break;
            }
        }

                                        // return to caller
    return (wRet);
} // NpsNamedEventSelChange()


//***************************************************************************
// NpsNamedEventsLoad()
//
// Description:
//      This routine finds out all of the named events that are available to
// the user to choose from and builds up the action type list box.  The
// item data for each list box entry will contain a pointer to NAMEDEVENTITEM
// information that can be used later.
//
// Parameters:
//      hWnd                            [in] Handle to the action listbox
//
// Return Value:
//      NOERR                           Everything went okay
//      ERR                             Something blew chunks
//
//***************************************************************************
// 09/30/1993 MARKL Function Created.
//***************************************************************************
WORD LOCAL PASCAL NpsNamedEventsLoad (  // Loads up the action name listbox
    HWND        hWnd)                   // [in] Handle to the action listbox
{
    int         wIndex;
    WORD        wRet                    = NOERR;
    LPSTR       lpszzNamedEvents        = NULL;
    LPSTR       lpszzNextEvent          = NULL;
    LPNAMEDEVENTITEM lpNamedEventItem   = NULL;

                                        // Find out what "Named Events" are
                                        // to be provided for the user.
    if ((lpszzNamedEvents = (LPSTR)MemAllocPtr (GHND, 1024)) != NULL)
        {
        if (GetPrivateProfileString (
                szIniNamedEvents,
                NULL,
                "",
                lpszzNamedEvents,
                1024,
                szNortonIni))
            {

                                        // Loop through all of the events
                                        // in the list, looking them up,
                                        // and adding them to the combobox.
            lpszzNextEvent = lpszzNamedEvents;
            while (*lpszzNextEvent != EOS)
                {
                lpNamedEventItem =
                    (LPNAMEDEVENTITEM)MemAllocPtr (GHND, sizeof (NAMEDEVENTITEM));
                if (lpNamedEventItem)
                    {
                    GetPrivateProfileString (lpszzNextEvent,
                                             szIniName,
                                             "",
                                             lpNamedEventItem->szName,
                                             SSIZE,
                                             szNortonIni);
                    GetPrivateProfileString (lpszzNextEvent,
                                             szIniPrompt,
                                             "",
                                             lpNamedEventItem->szPrompt,
                                             SSIZE,
                                             szNortonIni);
                    GetPrivateProfileString (lpszzNextEvent,
                                             szIniCmdLine,
                                             "",
                                             lpNamedEventItem->szCmdLine,
                                             MAX_CMDLINE,
                                             szNortonIni);
                    GetPrivateProfileString (lpszzNextEvent,
                                             szIniStartupDir,
                                             "",
                                             lpNamedEventItem->szStartupDir,
                                             SYM_MAX_PATH,
                                             szNortonIni);
                    GetPrivateProfileString (lpszzNextEvent,
                                             szIniDefDesc,
                                             "",
                                             lpNamedEventItem->szDefDesc,
                                             SSIZE,
                                             szNortonIni);
                    GetPrivateProfileString (lpszzNextEvent,
                                             szIniDefActionText,
                                             "",
                                             lpNamedEventItem->szDefActionText,
                                             SSIZE,
                                             szNortonIni);
                    GetPrivateProfileString (lpszzNextEvent,
                                             szIniBlankActionTextMessage,
                                             "",
                                             lpNamedEventItem->szBlankActionTextMessage,
                                             SYM_MAX_PATH,
                                             szNortonIni);

                    lpNamedEventItem->bAllowBlankActionText = (BOOL)
                        GetPrivateProfileInt (lpszzNextEvent,
                                              szIniAllowBlankActionText,
                                              1,
                                              szNortonIni);
                    lpNamedEventItem->wRunStyle =
                        GetPrivateProfileInt (lpszzNextEvent,
                                              szIniRunStyle,
                                              0,
                                              szNortonIni);
                    wIndex = ComboBox_AddString (hWnd,
                                                 lpNamedEventItem->szName);
                    if (wIndex != CB_ERR)
                        {
                        if (ComboBox_SetItemData (hWnd,
                                                  wIndex,
                                                  lpNamedEventItem) == CB_ERR)
                            {
                            MemFreePtr (lpNamedEventItem);
                            }
                        }
                    else
                        {
                        MemFreePtr (lpNamedEventItem);
                        }
                    }

                                        // Skip on to the next event in
                                        // the list.
                while (*lpszzNextEvent != EOS)
                    {
                    lpszzNextEvent = AnsiNext (lpszzNextEvent);
                    }
                lpszzNextEvent++;
                }
            }
        MemFreePtr (lpszzNamedEvents);
        }

                                        // Return to caller
    return (wRet);
} // NpsNamedEventsLoad()


//***************************************************************************
// NpsNamedEventsUnload()
//
// Description:
//      This routine goes through all of the named events in the action
// combobox and frees up any item data space that may have been allocated
// to keep track of the NAMEDEVENTITEM information.
//
// Parameters:
//      hWnd                            [in] Handle to the action listbox
//
// Return Value:
//      NOERR                           Everything went okay
//      ERR                             Something blew chunks
//
//***************************************************************************
// 09/30/1993 MARKL Function Created.
//***************************************************************************
WORD LOCAL PASCAL NpsNamedEventsUnload (// Unloads the action name listbox
    HWND        hWnd)                   // [in] Handle to the action listbox
{
    int         wIndex;
    int         wNumItems;
    int         nItem;
    WORD        wRet                    = NOERR;
    LPSTR       lpszzNamedEvents        = NULL;
    LPSTR       lpszzNextEvent          = NULL;
    LPNAMEDEVENTITEM lpNamedEventItem   = NULL;

                                        // How many items do we have?
    wNumItems = ComboBox_GetCount (hWnd);
    if (wNumItems && (wNumItems != CB_ERR))
        {
        for (wIndex = 0; wIndex < wNumItems; wIndex++)
            {
            nItem = (int)ComboBox_GetItemData (hWnd, wIndex);
            lpNamedEventItem = (LPNAMEDEVENTITEM) nItem;
            if ((lpNamedEventItem != NULL) &&
                (nItem != CB_ERR) &&
                !IsBadWritePtr (lpNamedEventItem, sizeof (NAMEDEVENTITEM)))
                {
                MemFreePtr (lpNamedEventItem);
                }
            }
        }


                                        // Return to caller
    return (wRet);
} // NpsNamedEventsUnload()



/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine decides which controls should be enabled or        */
/*      disabled based on the current frequency of the event.           */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went fine.        */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/18/91 MSL EnableWindow() calls fixed.                             */
/* 02/25/91 MSL Made cleanup pass.  No longer sets disabled list items  */
/*              to -1, which prevented them from remembering their      */
/*              last setting.                                           */
/* 04/19/93 MSL Dialog control were completely reworked.                */
/* 06/09/93 KDY Added support for yearly events.                        */
/* 06/10/93 KDY Modified monthly and yearly events to properly set the  */
/*              range for 'day of month'.                               */
/************************************************************************/
WORD LOCAL PASCAL NpsAddEditControlsDisable ( /* Turns controls on/off  */
    HWND        hDlg,                   /* Handle to dialog box         */
    WORD        wFreq)                  /* Current frequency of event   */
{
    extern      BOOL    b24Hour;

                                        /* Based on the frequency of    */
                                        /* the envent, enable and       */
                                        /* disable the appropraite      */
                                        /* controls.                    */
    switch (wFreq)
        {
        case NPS_FREQ_HOURLY:
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), SW_HIDE);

            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), SW_HIDE);

            NpsMoveControl( hDlg, IDC_NPS_MINUTES_PROMPT, iPrompt1y );
            NpsMoveControl( hDlg, IDC_NPS_MINUTES, iField1y );
            NpsMoveControl( hDlg, IDC_NPS_SCHEDULE_GROUP, iField1y );
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), TRUE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), SW_NORMAL);
            break;

        case NPS_FREQ_DAILY:
        case NPS_FREQ_WEEKDAY:
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), SW_HIDE);

            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), SW_HIDE);

            NpsMoveControl( hDlg, IDC_NPS_TIME_PROMPT, iPrompt1y );
            NpsMoveControl( hDlg, IDC_NPS_TIME, iField1y );
            NpsMoveControl( hDlg, IDC_NPS_SCHEDULE_GROUP, iField1y );
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), TRUE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), SW_NORMAL);
            break;

        case NPS_FREQ_WEEKLY:
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), SW_HIDE);

            NpsMoveControl( hDlg, IDC_NPS_DAY_PROMPT, iPrompt1y );
            NpsMoveControl( hDlg, IDC_NPS_DAY, iField1y );
            NpsMoveControl( hDlg, IDC_NPS_TIME_PROMPT, iPrompt2y );
            NpsMoveControl( hDlg, IDC_NPS_TIME, iField2y );
            NpsMoveControl( hDlg, IDC_NPS_SCHEDULE_GROUP, iField2y );
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), TRUE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), SW_NORMAL);
            break;

    case NPS_FREQ_ONETIME:
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), SW_HIDE);

            NpsMoveControl( hDlg, IDC_NPS_DATE_PROMPT, iPrompt1y );
            NpsMoveControl( hDlg, IDC_NPS_DATE, iField1y );
            NpsMoveControl( hDlg, IDC_NPS_TIME_PROMPT, iPrompt2y );
            NpsMoveControl( hDlg, IDC_NPS_TIME, iField2y );
            NpsMoveControl( hDlg, IDC_NPS_SCHEDULE_GROUP, iField2y );
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), TRUE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), SW_NORMAL);
            break;

        case NPS_FREQ_MONTHLY:
            SpinButton_SetRange (GetDlgItem (hDlg, IDC_NPS_DAYOFMONTH), 1, 31);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), SW_HIDE);

            NpsMoveControl( hDlg, IDC_NPS_DAYOFMONTH_PROMPT, iPrompt1y );
            NpsMoveControl( hDlg, IDC_NPS_DAYOFMONTH, iField1y );
            NpsMoveControl( hDlg, IDC_NPS_TIME_PROMPT, iPrompt2y );
            NpsMoveControl( hDlg, IDC_NPS_TIME, iField2y );
            NpsMoveControl( hDlg, IDC_NPS_SCHEDULE_GROUP, iField2y );
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), TRUE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), SW_NORMAL);
            break;

        case NPS_FREQ_YEARLY:
            NpsLimitDaysOfMonth(hDlg);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), FALSE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), FALSE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAY), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DATE), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES_PROMPT), SW_HIDE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MINUTES), SW_HIDE);

            NpsMoveControl( hDlg, IDC_NPS_MONTH_PROMPT, iPrompt1y );
            NpsMoveControl( hDlg, IDC_NPS_MONTH, iField1y );
            NpsMoveControl( hDlg, IDC_NPS_DAYOFMONTH_PROMPT, iPrompt2y );
            NpsMoveControl( hDlg, IDC_NPS_DAYOFMONTH, iField2y );
            NpsMoveControl( hDlg, IDC_NPS_TIME_PROMPT, iPrompt3y );
            NpsMoveControl( hDlg, IDC_NPS_TIME, iField3y );
            NpsMoveControl( hDlg, IDC_NPS_SCHEDULE_GROUP, iField3y );
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), TRUE);
            EnableWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), TRUE);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_MONTH), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME_PROMPT), SW_NORMAL);
            ShowWindow (GetDlgItem (hDlg,  IDC_NPS_TIME), SW_NORMAL);
            break;
        }

                                        // Return to caller
    return (NOERR);
} /* NpsAddEditControlsDisable() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      Loads the current event based on the values of the controls in  */
/*      the Add/Edit Event Dialog Box.                                  */
/*                                                                      */
/* NOTE:                                                                */
/*      Event[nCurEvent].wType is set in the dialog box in real time    */
/*      so we can assume its value is set correctly in this routine.    */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went fine.        */
/*      NPS_PROMPT_COMMAND_LINE         -- The file given is invalid.   */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/18/91 MSL Made clean up pass.  Validates if file spec is valid.   */
/* 02/26/91 MSL Now obtains data from the new spin buttons.             */
/* 03/01/91 MSL Fixed bug for when its 12 am.  Was converting to 12 pm. */
/* 04/08/91 MSL WNameIsFile() changed to FileOpen with OF_EXIST flag.   */
/* 04/22/91 MSL Now handles Run Minized check box.                      */
/* 05/23/91 MSL Now calls NpsValidateCommmand().                        */
/* 01/19/92 MSL Now handles event options.                              */
/* 10/01/93 MSL Now handles NAMED event types.                          */
/************************************************************************/
WORD LOCAL PASCAL NpsAddEditDlgDeInit ( /* Gets event from dlg cntrls   */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    int                 wIndex;         /* Index into spin buttons      */
    int                 nItem;
    char                szBuf[MAX_CMDLINE];
    WORD                wRet;           /* Component return code        */
    HWND                hWndCmdMsg;
    UINT                wTempMonth;
    UINT                wTempDate;
    UINT                wTempYear;
    UINT                wTempHour;
    UINT                wTempMin;
    LPNAMEDEVENTITEM    lpNamedEventItem = NULL;
    HWND                hWndCombo = GetDlgItem (hDlg, IDC_NPS_ACTIONTYPE);
    UINT                uNumCharsCopied;
    extern      char    szHeaderEntry[];
    extern      NPSEVNT Events[];       /* The actual events            */

                                        /* Initialization               */
    wRet = NOERR;

    wIndex = ComboBox_GetCurSel (hWndCombo);


                                        /* Initialize event header.     */
    STRCPY (Events[nCurEvent].EntryHeader.szHeader, szHeaderEntry);
    Events[nCurEvent].EntryHeader.wVerMajor = NPS_ENTRY_VER_MAJOR;
    Events[nCurEvent].EntryHeader.wVerMinor = NPS_ENTRY_VER_MINOR;
    Events[nCurEvent].EntryHeader.wEntryID  = EVENT_ID_SCHEDULE;
    Events[nCurEvent].EntryHeader.wSizeEntry = sizeof (NPSEVNT);

                                        /* Is the event enabled?        */
    Events[nCurEvent].bEnabled = IsDlgButtonChecked (hDlg,  IDC_NPS_ENABLEEVENT);

                                        /* Is the sound enabled?        */
    Events[nCurEvent].bSound = IsDlgButtonChecked (hDlg,  IDC_NPS_SOUND);

                                        /* What is the description for  */
                                        /* this event?                  */
    uNumCharsCopied = GetDlgItemText (hDlg,
                                      IDC_NPS_DESCRIPTION,
                                      Events[nCurEvent].szDescr,
                                      MAX_DESCR);

    // GetDlgItemText truncates DBCS strings badly. It removes a trail byte
    // without removing the corresponding lead byte. Thus, when the buffer
    // is max-ed out, walk the string to ensure the last byte is not a lead
    // byte.
    if (DBCSInstalled() && MAX_DESCR == uNumCharsCopied + 1 /*EOS Char*/)
    {
        auto char *CurChar = Events[nCurEvent].szDescr;
        for (; *CurChar; CurChar = AnsiNext (CurChar))
        {
            if (DBCSIsLeadByte (CurChar[0]) && CurChar[1] == EOS)
                CurChar[0] = EOS;
        }
    }

                                        // What is the text/command for
                                        // the event?
    GetDlgItemText (hDlg,
                    IDC_NPS_MSG_TEXT,
                    Events[nCurEvent].szMessage,
                    MAX_CMDLINE);

    GetDlgItemText (hDlg,
                     IDC_NPS_CMD_TEXT,
                    Events[nCurEvent].szCmdLine,
                    MAX_CMDLINE);

    GetDlgItemText (hDlg,
                     IDC_NPS_STARTUPDIR_TEXT,
                    Events[nCurEvent].szStartupDir,
                    MAX_CMDLINE);

    Events[nCurEvent].wRunStyle =
        ComboBox_GetCurSel (GetDlgItem(hDlg, IDC_NPS_RUNSTYLE));

                                        // What type of event is this?
                                        // PROGRAM, MESSAGE, NAMED?  Get
                                        // the name for the event
                                        // regardless.  If it is a named
                                        // event, override the command line
                                        // and other related fields with what
                                        // the INI settings told us to do.
    Events[nCurEvent].wType = wIndex + 1;

    if ( Events[nCurEvent].wType > EVENT_TYPE_NAMED )
        {
        Events[nCurEvent].wType = EVENT_TYPE_NAMED;
        }

    ComboBox_GetLBText (hWndCombo,
                        wIndex,
                        Events[nCurEvent].szNamed);

    nItem = (int)ComboBox_GetItemData (hWndCombo, wIndex);
    lpNamedEventItem = (LPNAMEDEVENTITEM) nItem;

    if (lpNamedEventItem && (nItem != CB_ERR))
        {
        STRCPY (Events[nCurEvent].szCmdLine,
                lpNamedEventItem->szCmdLine);
        STRCPY (Events[nCurEvent].szStartupDir,
                lpNamedEventItem->szStartupDir);
        GetDlgItemText (hDlg, IDC_NPS_ACTION_TEXT,
                        Events[nCurEvent].szPromptParams,
                        MAX_CMDLINE);
        Events[nCurEvent].wRunStyle = lpNamedEventItem->wRunStyle;
        }

                                        /* If this event is going to    */
                                        /* execute a program, validate  */
                                        /* that the program is at least */
                                        /* a valid file spec.           */

    if ( Events[nCurEvent].wType == EVENT_TYPE_PROGRAM )
        {
        auto    LPSTR   lpCmd       =   NULL;
        auto    LPSTR   lpParams    =   NULL;

        lpCmd    = (LPSTR)MemAllocPtr ( GHND, (SYM_MAX_PATH*2) );
        lpParams = (LPSTR)MemAllocPtr ( GHND, SYM_MAX_PATH+1 );

        if ( lpCmd && lpParams )
            {
            STRCPY (szBuf, Events[nCurEvent].szCmdLine);

            NpsReturnCmdParams  (
                                szBuf,
                                lpCmd,
                                lpParams
                                );

            if ( wRet = NpsValidateCommand (
                                        hDlg,
                                        lpCmd,
                                        Events[nCurEvent].szStartupDir
                                        ) )
                {
                wRet = NPS_PROMPT_COMMAND_LINE;

                if ( NMsgBox ( ghInst,
                                hDlg,
                                wRet,
                                TRACE,
                                NULL,
                                MB_YESNO | MB_ICONQUESTION,
                                NULL
                                ) == IDYES )
                    {
                    if ( lpCmd )
                        MemFreePtr ( lpCmd );

                    if ( lpParams )
                        MemFreePtr ( lpParams );

                    goto NpsAddEditDlgDeInitExit;
                    }
                }
            wRet = NOERR;
            }

        if ( lpCmd )
            MemFreePtr ( lpCmd );

        if ( lpParams )
            MemFreePtr ( lpParams );
        }

                                        /* What is the frequency?       */

    Events[nCurEvent].wFreq = ComboBox_GetCurSel (
                                            GetDlgItem (hDlg, IDC_NPS_FREQ)
                                            );

                                        // What is the date/time?
    wTempMonth = Events[nCurEvent].wMonth + 1;
    wTempDate  = Events[nCurEvent].wDate;
    wTempYear  = Events[nCurEvent].wYear;
    if ((NIntlGetValidDate (GetDlgItem (hDlg,  IDC_NPS_DATE),
                            &wTempMonth,
                            &wTempDate,
                            &wTempYear) != NOERR) ||
        (NIntlGetValidTime (GetDlgItem (hDlg,  IDC_NPS_TIME),
                            &wTempHour,
                            &wTempMin,
                            DISPLAY_AMPM) != NOERR))
        {
                wRet = NPS_INVALID_DATETIME;
                NMsgBox (ghInst,hDlg, wRet, TRACE, NULL, -1, NULL);
                goto NpsAddEditDlgDeInitExit;
        }
    Events[nCurEvent].wMonth = (WORD)wTempMonth - 1;
    Events[nCurEvent].wDate  = (WORD)wTempDate;
    Events[nCurEvent].wYear  = (WORD)wTempYear + (wTempYear<80 ? 2000 : 1900);
    Events[nCurEvent].wHour  = (WORD)wTempHour;
    Events[nCurEvent].wMin   = (WORD)wTempMin;

                                        // Are there any minutes involved?
    if (Events[nCurEvent].wFreq == NPS_FREQ_HOURLY)
        {
        wIndex = SpinButton_GetCurSel (GetDlgItem (hDlg,  IDC_NPS_MINUTES));
        if (wIndex != CB_ERR)
            {
            Events[nCurEvent].wMin =
                SpinButton_GetSpnNum (GetDlgItem (hDlg,  IDC_NPS_MINUTES),
                                      wIndex);
            }
        }
    else if (Events[nCurEvent].wFreq == NPS_FREQ_WEEKLY)
        {
        Events[nCurEvent].wDay = SpinButton_GetCurSel (
                                     GetDlgItem (hDlg,  IDC_NPS_DAY));
        }
    else if (Events[nCurEvent].wFreq == NPS_FREQ_MONTHLY)
        {
        Events[nCurEvent].wDate = SpinButton_GetCurSel (
                                     GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH)) + 1;
        }
    else if (Events[nCurEvent].wFreq == NPS_FREQ_YEARLY)
        {
        Events[nCurEvent].wMonth = SpinButton_GetCurSel (
                                     GetDlgItem (hDlg,  IDC_NPS_MONTH));
        Events[nCurEvent].wDate = SpinButton_GetCurSel (
                                     GetDlgItem (hDlg,  IDC_NPS_DAYOFMONTH)) + 1;
        }

                                        /* Return to caller             */
NpsAddEditDlgDeInitExit:
    if (wRet != NOERR)
        {
        if (Events[nCurEvent].wType == EVENT_TYPE_PROGRAM )
            {
            hWndCmdMsg = GetDlgItem (hDlg,  IDC_NPS_MSG_TEXT);
            }
        else
            {
            hWndCmdMsg = GetDlgItem (hDlg,  IDC_NPS_CMD_TEXT);
            }
        SetFocus (hWndCmdMsg);
        Edit_SetSel (hWndCmdMsg, 0, -1);
        }
    return (wRet);
} /* NpsAddEditDlgDeInit() */




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine allows the user to browse for a file to launch     */
/*      when an event is triggered.  The file is placed in the          */
/*       IDC_NPS_CMD_TEXT field.  We also want to remember the last drive */
/*      and path given by the user so we can default back to it on      */
/*      subsequent calls to standard browse.  We also do the same thing */
/*      with the file extension, memorizing it to use as a filter on    */
/*      subsequent calls.                                               */
/*                                                                      */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 02/18/91 MSL Function created.                                       */
/* 02/26/91 MSL StdBrowse now can accept a default path and filter.     */
/* 04/01/91 MSL Checks for wildcard in path spec.                       */
/* 01/10/92 MSL Uses new standard browse.                               */
/* 03/16/93 MSL Now uses common dialog.                                 */
/* 04/19/93 MSL Now uses ID_NPS_CMD_TEXT.                               */
/************************************************************************/
WORD LOCAL PASCAL NpsEventBrowse (      /* User browses for file        */
    HWND        hDlg)                   /* Handle to dialog box         */
{
    WORD                        wRet = (WORD)ERR;
    OPENFILENAME                OFName;
    char                        szDialogTitle[60];
    char                        szzFilterSpec[ SYM_MAX_PATH + 1];
    char                        szFileSpec[ SYM_MAX_PATH + 1];
    char                        szFileSpecOut[ SYM_MAX_PATH + 1];
    extern      HINSTANCE       ghInst;

                                        // Load our dialog title.
    LoadString (ghInst,
                IDS_NPS_FILEBROWSE_TITLE,
                szDialogTitle,
                sizeof (szDialogTitle));

                                        // Our file specification cannot
                                        // have any command line options
                                        // our common dialog will blow
                                        // chunks.
    GetDlgItemText (hDlg,  IDC_NPS_CMD_TEXT, szFileSpec, sizeof (szFileSpec));
    NpsReturnCmdParams (szFileSpec, szFileSpec, NULL);

                                        // If the command line is quoted,
                                        // we need to remove the quotes or
                                        // name return short name will choke.
    if ( *szFileSpec == '\"' )
        {
        LPSTR lpszFrom = CharNext ( szFileSpec );
        LPSTR lpszTo   = szFileSpec;
        while ( (*lpszFrom != '\"') && (*lpszFrom != EOS) )
            {
            *lpszTo  = *lpszFrom;
            if (DBCSIsLeadByte(*lpszFrom))
                {*(lpszTo+1)  = *(lpszFrom+1);}

            lpszTo   = CharNext ( lpszTo );
            lpszFrom = CharNext ( lpszFrom );
            }
        *lpszTo = EOS;
        }

                                        // Set up our Browse Filters.
    NpsFileBrowseFiltersGet (szzFilterSpec, sizeof (szzFilterSpec));

                                        // Set up the OFName structure
                                        // such that common dialog may
                                        // prompt for the file to launch.
    MEMSET (&OFName, 0, sizeof (OFName));
    OFName.lStructSize          = sizeof (OFName);
    OFName.hwndOwner            = hDlg;
    OFName.hInstance            = ghInst;
    OFName.lpstrFilter          = szzFilterSpec;
    OFName.lpstrFile            = szFileSpec;
    OFName.nMaxFile             = sizeof (szFileSpec);
    OFName.lpstrTitle           = szDialogTitle;

    OFName.Flags                = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
                                  OFN_ENABLEHOOK; /* | OFN_SHOWHELP */

                                        // Only give the EXPLORER flag if we
                                        // are in NT4 or Win95.
    if ( LOBYTE( GetVersion() ) >= 4 )
        {
        OFName.Flags            |= OFN_EXPLORER;
        }
                                  ;
    OFName.lpstrDefExt          = szzFilterSpec;
    OFName.lpfnHook             = NpsFileBrowseHookProc;

                                        // Prompt the user for the file
    if (GetOpenFileName (&OFName))
        {
                                        // If this looks like an LFN with a
                                        // space in it, we need to surround
                                        // it by quotes.
        if ( STRSTR ( OFName.lpstrFile, " " ) != NULL )
            {
            STRCPY ( szFileSpecOut, "\"" );
            STRCAT ( szFileSpecOut, OFName.lpstrFile );
            STRCAT ( szFileSpecOut, "\"" );
            }
        else
            STRCPY ( szFileSpecOut, OFName.lpstrFile );

                                        // Update the  IDC_NPS_EDIT with
                                        // the gathered file spec.
        SetDlgItemText ( hDlg,  IDC_NPS_CMD_TEXT, szFileSpecOut );
        }

                                        // Return to caller.
    return (wRet);
} /* NpsEventBrowse() */



//***************************************************************************
// NpsFileBrowseFiltersGet()
//
// Description:
//      This routine builds a filter specification for our common browse
// dialog.  A typical string would look as follows:
//              "Programs|*.EXE *.COM *.BAT|All (*.*)|*.*|"
// where the "|" characters are really EOS markers.
//
// Parameters:
//      lpszzBuffer              [out] Buffer to receive filter specification.
//      wBufSize                 [in] Size of buffer
//
// Return Value:
//      nothing
//***************************************************************************
// 03/16/1993 MARKL Function Created.
//***************************************************************************
VOID LOCAL NpsFileBrowseFiltersGet(     // Returns browse file filter spec
    LPSTR       lpszzBuffer,            // [out] Receives filter specification
    WORD        wBufSize)               // [in] Size of buffer
{
    char        szProgramExts[80];
    char        szAllFiles[80];
    LPSTR       lpTemp;
    extern      HINSTANCE       ghInst;

                                        // Set up our Browse Filters.  We
                                        // First see if we can say
                                        // Programs and use the list of
                                        // extensions in the win.ini file.
    *lpszzBuffer = EOS;
    if (NpsProgramExtsGet (szProgramExts) == NOERR)
        {
        LoadString (ghInst,
                    IDS_NPS_FILTER_TYPE_PROGRAMS,
                    lpszzBuffer,
                    wBufSize);
        STRCAT (lpszzBuffer, szProgramExts);
        }


                                        // Now set up ALL FILES.
    LoadString (ghInst,
                IDS_NPS_FILTER_TYPE_ALL,
                szAllFiles,
                sizeof (szAllFiles));
    STRCAT (lpszzBuffer, szAllFiles);

                                        // Finally, convert all "|"s to
                                        // EOS markers such that we have
                                        // a true list of NULL terminated
                                        // strings.
    lpTemp = lpszzBuffer;
    while (*lpTemp != EOS)
        {
        if (*lpTemp == '|')
            {
            *lpTemp = EOS;
            lpTemp++;
            }
        else
            {
            lpTemp = AnsiNext (lpTemp);
            }
        }

                                        // Return to caller.
} // NpsFileBrowseFiltersGet()



//***************************************************************************
// NpsProgramExtsGet()
//
// Description:
//      This routine finds out the program file extensions from the win.ini
// file.  It returns the extensions to the buffer in the form
// "*.XXX *.XXX *.XXX|" even though they are in the ini file as
// "XXX XXX XXX".
//
// Parameters:
//      lpszBuffer              [out] Buffer to receive extension list.
//
// Return Value:
//      NOERR                   Extensions are in list.
//      ERR                     No extensions in list.
//***************************************************************************
// 03/16/1993 MARKL Function Created.
// 05/04/95   KLM Fixed DBCS implementation and replaced spaces with semicolons
//***************************************************************************
WORD LOCAL NpsProgramExtsGet(           // Returns list of exe extensions
    LPSTR       lpszBuffer)             // [out] Buffer to receive ext list
{
    char        szProgramExts[80];
    LPSTR       lpszCurExt;
    WORD        wRet = (WORD)ERR;
    BOOL        bFirstExt = TRUE;
                                        // If there is a list of program
                                        // file extensions in the Win.Ini
                                        // file, then format them correctly.
    *lpszBuffer = EOS;
    if (GetProfileString ("windows",
                          "programs",
                          "",
                          szProgramExts,
                          sizeof (szProgramExts)))
        {
        wRet = NOERR;
        lpszCurExt = szProgramExts;

                                        // Skip leading blanks
        while (*lpszCurExt == ' ')
            {
            lpszCurExt++;
            }
                                        // Loop through each extension,
                                        // building a "*.XXX;" for each
                                        // one found, last is *.XXX|.
        while (*lpszCurExt != EOS)
            {
                                        // Put a ';' seperator between
                                        // multiple extensions.
            if ( bFirstExt )
                bFirstExt = FALSE;
            else
                {
                *lpszBuffer = ';';
                lpszBuffer++;
                }

                                        // Set up "*." at beginning of
                                        // specification.
            *lpszBuffer = '*';
            lpszBuffer++;
            *lpszBuffer = '.';
            lpszBuffer++;

                                        // Copy extension
            while (*lpszCurExt != EOS && *lpszCurExt != ' ')
                {
                if ( DBCSIsLeadByte(*lpszCurExt) )
                    *lpszBuffer++ = *lpszCurExt++;

                *lpszBuffer++ = *lpszCurExt++;
                }
                                        // Skip trailing blanks
            while (*lpszCurExt == ' ')
                {
                lpszCurExt++;
                }

            }
        *lpszBuffer++ = '|';
        *lpszBuffer = EOS;
        }

                                        // Return to caller
    return (wRet);
} // NpsProgramExtsGet()


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      Validates that the given command line looks semi valid and is   */
/*      at least launchable.                                            */
/*                                                                      */
/* NOTE:                                                                */
/*      Assumption is that parameters have been removed from the        */
/*      command line!!!                                                 */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Everything went fine.        */
/*      NPS_PROMPT_COMMAND_LINE         -- The file given is invalid.   */
/************************************************************************/
/* 05/23/91 MSL Function created.                                       */
/************************************************************************/
WORD LOCAL PASCAL NpsValidateCommand (  /* Validate command             */
    HWND        hDlg,                   /* Handle to dialog box         */
    LPSTR       szCmd,                  /* Command to validate          */
    LPSTR       szStartupDir)           /* Optional startup directory   */
{
    auto    WORD        wRet = NOERR;
    auto    char        szShortCmd      [ SYM_MAX_PATH + 1 ];
    auto    char        szShortStartDir [ SYM_MAX_PATH + 1 ];
    auto    STATUS      Status;
    auto    HCURSOR     hCursorOld;

                                        // Tell the user to hold on.
    hCursorOld = SetCursor ( LoadCursor ( NULL, IDC_WAIT ) );
    ShowCursor (TRUE);

                                        // If the command line is quoted,
                                        // we need to remove the quotes or
                                        // name return short name will choke.
    if ( *szCmd == '\"' )
        {
        LPSTR lpszFrom = CharNext ( szCmd );
        LPSTR lpszTo   = szShortCmd;
        while ( (*lpszFrom != '\"') && (*lpszFrom != EOS) )
            {
            *lpszTo  = *lpszFrom;
            if (DBCSIsLeadByte (*lpszFrom))
                {*(lpszTo+1)  = *(lpszFrom+1);}

            lpszTo   = CharNext ( lpszTo );
            lpszFrom = CharNext ( lpszFrom );
            }
        *lpszTo = EOS;
        }
    else
        STRCPY ( szShortCmd, szCmd );

                                        // Get a valid short name for the
                                        // command if we can.
    Status = NameReturnShortName ( szShortCmd, szShortCmd );
    if ( Status != NOERR )
        STRCPY ( szShortCmd, szCmd );

                                        // If there is a startup directory
                                        // we need to work with it as well.
    if ( *szStartupDir )
        {
                                        // If the startup dir is quoted,
                                        // we need to remove the quotes or
                                        // name return short name will choke.
        if ( *szStartupDir == '\"' )
            {
            LPSTR lpszFrom = CharNext ( szStartupDir );
            LPSTR lpszTo   = szShortStartDir;
            while ( (*lpszFrom != '\"') && (*lpszFrom != EOS) )
                {
                *lpszTo  = *lpszFrom;
                if (DBCSIsLeadByte (*lpszFrom))
                    {*(lpszTo+1)  = *(lpszFrom+1);}

                lpszTo   = CharNext ( lpszTo );
                lpszFrom = CharNext ( lpszFrom );
                }
            *lpszTo = EOS;
            }
        else
            STRCPY ( szShortStartDir, szStartupDir );

                                        // See if the startup dir has a
                                        // short equivalent.
        Status = NameReturnShortName ( szShortStartDir, szShortStartDir );
        if ( Status != NOERR )
            {
            STRCPY ( szShortStartDir, szStartupDir );
            }
        }
    else
        {
        *szShortStartDir = EOS;
        }

                                        // FINALLY:  We can now validate that
                                        // the thing is launchable.  It is
                                        // MUCH EASIER to just launch the
                                        // darn thing than going through all
                                        // of this!!!
    wRet = NameIsExecutableInitDir ( szShortCmd, szShortStartDir, NULL);

                                        // Tell the user we are back.
    if ( hCursorOld )
        {
        ShowCursor (FALSE);
        SetCursor (hCursorOld);
        }

                                        // Return to caller.
    return ( wRet );
}



/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This calls up the standard directory browse box such that the   */
/*      user may browse for a startup directory.                        */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Function completed okay.     */
/*                                                                      */
/************************************************************************/
/* 01/28/92 MSL Function created.                                       */
/* 03/18/93 MSL Uses new standard directory select and highlights dir   */
/*              on return from dialog.                                  */
/************************************************************************/
WORD LOCAL PASCAL NpsStartupDirBrowse ( /* Browse for startup dir */
    HWND        hDlg)                   /* Handle to dailog box         */
{
    extern      HINSTANCE       ghInst;

#ifdef SYM_WIN32
    auto    NG_BROWSEINFO   StdDir;
    auto    char            szDirBufferOut[SYM_MAX_PATH+1];

    MEMSET ( &StdDir, EOS, sizeof(StdDir) );

    StdDir.dwSize           =   sizeof(StdDir);
    StdDir.hwndParent       =   hDlg;
    StdDir.hInstance        =   ghInst;
    StdDir.pszTitle         =   MAKEINTRESOURCE(IDS_NPS_DIRBROWSE_TITLE);
    StdDir.pszDescription   =   NULL;
    StdDir.pszOKText        =   NULL;
    StdDir.bSubfolder       =   TRUE;
    StdDir.pfnCallback      =   NULL;
    StdDir.lParam           =   0;
    StdDir.pidlRoot         =   NULL;
    StdDir.dwFlags          =   NGBIF_RETURNONLYFSDIRS |
                                NGBIF_USETITLE;

                                        // ----------------------------------
                                        // Show folder select dialog box and
                                        // check if the user pressed the OK
                                        // button.
                                        // ----------------------------------

    if ( NG_BrowseForFolder ( &StdDir ) == TRUE )
        {
                                        // If this looks like an LFN with a
                                        // space in it, we need to surround
                                        // it by quotes.
        if ( STRSTR ( StdDir.szPath, " " ) != NULL )
            {
            STRCPY ( szDirBufferOut, "\"" );
            STRCAT ( szDirBufferOut, StdDir.szPath );
            STRCAT ( szDirBufferOut, "\"" );
            }
        else
            STRCPY ( szDirBufferOut, StdDir.szPath );

        Tunnel_SetText ( GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT),
                         szDirBufferOut );
        }
                                        // ----------------------------------
                                        // Hilight the directory in the
                                        // edit field.
                                        // ----------------------------------
    Tunnel_SetSel ( GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT), 0, -1);

#else

    char                szDirBuffer[SYM_MAX_PATH+1];
    char                szDirBufferOut[SYM_MAX_PATH+1];
    char                szTitle[60];
    HWND                hWndDirectory;
    STDDIRSTRUCT        stdDir;


                                        // Initialization
    hWndDirectory = GetDlgItem (hDlg,  IDC_NPS_STARTUPDIR_TEXT);
    Tunnel_GetText (hWndDirectory,
                    szDirBuffer,
                    sizeof (szDirBuffer));
    LoadString (ghInst,
                IDS_NPS_DIRBROWSE_TITLE,
                szTitle,
                sizeof (szTitle));

                                        // Allow the user to browse for
                                        // a directory.  If he selects
                                        // one, save it.
    MEMSET(&stdDir, 0, sizeof(stdDir));
    stdDir.dwSize       = sizeof(stdDir);
    stdDir.hWnd         = hDlg;
    stdDir.lpszTitle    = szTitle;
    stdDir.lpszDir      = szDirBuffer;
    stdDir.wMaxDir      = sizeof(szDirBuffer);
    stdDir.uHelpID      = NPS_HELP_DLG_BROWSESTARTUPDIR;

    if (StdDirSelect (&stdDir) )
        {
                                        // If this looks like an LFN with a
                                        // space in it, we need to surround
                                        // it by quotes.
        if ( STRSTR ( szDirBuffer, " " ) != NULL )
            {
            STRCPY ( szDirBufferOut, "\"" );
            STRCAT ( szDirBufferOut, szDirBuffer );
            STRCAT ( szDirBufferOut, "\"" );
            }
        else
            STRCPY ( szDirBufferOut, szDirBuffer );

        Tunnel_SetText (hWndDirectory, szDirBufferOut);
        }

                                        // Hilight the directory in the
                                        // edit field.
    Tunnel_SetSel (hWndDirectory, 0, -1);

#endif

    return (NOERR);
} /* NpsStartupDirBrowse() */


//***************************************************************************
// NpsFileBrowseHookProc()
//
// Description:
//      Thisw procedure is used for hooking the common dialog File Open
// Dialog Box.  The purpose of this hook is to provide a 3D look via
// CTL3D.
//
// Parameters:
//      hWnd            [in] Window handle
//      wMessage        [in] Message to process
//      wParam          [in] wParam of message
//      lParam          [in] lParam of message
//
// Return Value:
//      FALSE           Didn't process message
//      TRUE            Processed message
//
//***************************************************************************
// 03/16/93 MSL Function Created.
// 07/02/93 KDY Added UINT typecast to return of Ctl3dColorEx.
//***************************************************************************
UINT SYM_EXPORT CALLBACK NpsFileBrowseHookProc (    // For handling File Browse
    HWND        hWnd,                           // [in] Window handle
    UINT        wMessage,                       // [in] Message to process
    WPARAM      wParam,                         // [in] wParam of message
    LPARAM      lParam)                         // [in] lParam of message
{
    switch (wMessage)
        {
                                                // WM_INITDIALOG:  Set up
                                                // for 3D look/feel.
        case WM_INITDIALOG:
            NG_CenterDialog (hWnd, GetParent (hWnd));

            Ctl3dSubclassDlg (hWnd, CTL3D_ALL);
            return (FALSE);
                                                // WM_CTLCOLOR:  Used to give
                                                // control 3D look.
        case WM_CTLCOLOR:
            return ((UINT)Ctl3dCtlColorEx (wMessage, wParam, lParam));

#if !defined(SYM_WIN32)
                                        // WM_ACTIVATE:  Another help
                                        // kluge for F1.
        case WM_ACTIVATE:
            if (GET_WM_ACTIVATE_STATE(wParam, lParam))
                {
                StdHelpSetContext (NPS_HELP_DLG_BROWSECOMMAND,
                                   NPS_HELP_DLG_BROWSECOMMAND);
                }
            return (FALSE);
#endif
                                                // default:  Let COMMDLG.DLL
                                                // handle it.
        default:
            return (FALSE);
        }
                                                // Return to caller
    return (TRUE);
} // NpsFileBrowseHookProc()



//***************************************************************************
// NpsMoveControl()
//
// Description:
//      Moves control hCtl to y-offset with hDlg dialog box.
//
//
// Return Value:
//      NOERR                           -- Everything went fine.
//
//***************************************************************************
// 06/09/93 KDY Function Created.
//***************************************************************************
static WORD NpsMoveControl(                                     // Move control within dialog box
        HWND                    hDlg,                                   // [in] Handle to dialog box
        WORD                    wCtl,                   // [in] ID of control to move
        int                     iOffsetY)               // [in] New Y-position
{
    HWND                        hCtl;
    RECT                        crWnd;


        hCtl = GetDlgItem(hDlg, wCtl);
        GetWindowRect(hCtl, &crWnd);
        NpsScreenToClient(hDlg, &crWnd);
                                                                                // Special case for groupbox.
        if (wCtl == IDC_NPS_SCHEDULE_GROUP)
        {
                        crWnd.bottom = iGroupBottom + iOffsetY;
                        iOffsetY = crWnd.top;
        }

        MoveWindow(hCtl, crWnd.left, iOffsetY, crWnd.right-crWnd.left,
                                crWnd.bottom - crWnd.top, TRUE);

        return(NOERR);
} //  NpsMoveControl()



//***************************************************************************
// NpsLimitDaysOfMonth()
//
// Description:
//      Resets the limit of the IDC_NPS_DAYOFMONTH spin button to coincide
//      with that of the currently selected month in IDC_NPS_MONTH.  Currently,
//      this is only used by 'Yearly' events.
//
//
// Return Value:
//      NOERR                           -- Everything went fine.
//
//***************************************************************************
// 06/09/93 KDY Function Created.
//***************************************************************************
static WORD NpsLimitDaysOfMonth(        // Set limit on days of the month
    HWND    hDlg)                       // [in] Handle to dialog box
{
    WORD    wMonth;
    WORD    wDays;

    wMonth = SpinButton_GetCurSel(GetDlgItem (hDlg, IDC_NPS_MONTH));
    switch(wMonth)
        {
        case 0:         // Jan
        case 2:         // Mar
        case 4:         // May
        case 6:         // Jul
        case 7:         // Aug
        case 9:         // Oct
        case 11:        // Dec
            wDays = 31;
            break;

        case 3:         // Apr
        case 5:         // Jun
        case 8:         // Sep
        case 10:        // Nov
            wDays = 30;
            break;

        case 1:         // Feb
                                        // Note:  Don't bother with leap years!  Does it
                                        // make sense to have a 'yearly' event occur once
                                        // every four years?
            wDays = 28;
            break;
        }

    SpinButton_SetRange (GetDlgItem (hDlg, IDC_NPS_DAYOFMONTH), 1, wDays);

        return(NOERR);
} //  NpsLimitDaysOfMonth()

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
        IDD_NPS_EVENT,
        IDH_UNKNOWN,

        IDC_NPS_ENABLEEVENT,
        IDH_NAVW_SCHEDULER_WHATHIS_ENABLE_THIS_EVENT_CHKBX,

        IDC_NPS_SOUND,
        IDH_NAVW_SCHEDULER_WHATHIS_AUDIBLE_ALARM_CHKBX,

        IDC_NPS_ACTIONTYPE_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_TYPE_OF_EVENT_LSTBX,

        IDC_NPS_ACTIONTYPE,
        IDH_NAVW_SCHEDULER_WHATHIS_TYPE_OF_EVENT_LSTBX,

        IDC_NPS_DESCRIPTION,
        IDH_NAVW_SCHEDULER_WHATHIS_DESCRIPTION,

        IDC_NPS_DESCRIPTION_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_DESCRIPTION,

        IDC_NPS_MSG_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_MESSAGE_TO_DISPLAY,

        IDC_NPS_MSG_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_MESSAGE_TO_DISPLAY,

        IDC_NPS_ACTION_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_WHAT_TO_SCAN,

        IDC_NPS_ACTION_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_WHAT_TO_SCAN,

        IDC_NPS_CMD_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_COMMAND_LINE_TO_RUN_LSTBX,

        IDC_NPS_CMD_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_COMMAND_LINE_TO_RUN_LSTBX,

        IDC_NPS_STARTUPDIR_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_STARTUP_FOLDER_LSTBX,

        IDC_NPS_STARTUPDIR_TEXT,
        IDH_NAVW_SCHEDULER_WHATHIS_STARTUP_FOLDER_LSTBX,

        IDC_NPS_RUNSTYLE_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_RUN_STYLE_LSTBX,

        IDC_NPS_RUNSTYLE,
        IDH_NAVW_SCHEDULER_WHATHIS_RUN_STYLE_LSTBX,

        IDC_NPS_SCHEDULE_GROUP,
        IDH_NAVW_SCHEDULER_WHATHIS_SCHEDULE_INFORMATION_GRPBX,

        IDC_NPS_FREQ_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_FREQUENCY_LSTBX,

        IDC_NPS_FREQ,
        IDH_NAVW_SCHEDULER_WHATHIS_FREQUENCY_LSTBX,

        IDC_NPS_DATE_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_DATE_LSTBX,

        IDC_NPS_DATE,
        IDH_NAVW_SCHEDULER_WHATHIS_DATE_LSTBX,

        IDC_NPS_DAY_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_DAY_OF_WEEK_LSTBX,

        IDC_NPS_DAY,
        IDH_NAVW_SCHEDULER_WHATHIS_DAY_OF_WEEK_LSTBX,

        IDC_NPS_MONTH_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_MONTH_LSTBX,

        IDC_NPS_MONTH,
        IDH_NAVW_SCHEDULER_WHATHIS_MONTH_LSTBX,

        IDC_NPS_DAYOFMONTH_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_DAY_OF_MONTH_LSTBX,

        IDC_NPS_DAYOFMONTH,
        IDH_NAVW_SCHEDULER_WHATHIS_DAY_OF_MONTH_LSTBX,

        IDC_NPS_MINUTES_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_MINUTES_LSTBX,

        IDC_NPS_MINUTES,
        IDH_NAVW_SCHEDULER_WHATHIS_MINUTES_LSTBX,

        IDC_NPS_TIME_PROMPT,
        IDH_NAVW_SCHEDULER_WHATHIS_TIME_LSTBX,

        IDC_NPS_TIME,
        IDH_NAVW_SCHEDULER_WHATHIS_TIME_LSTBX,

/*
        IDC_NPS_TEST,
        0,

        IDC_NPS_TEXT,
        0,

        IDC_NPS_SCHEDTIME,
        0,
*/

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








//***************************************************************************
// IsAdminMode()
//
// Description:
//      This function checks to see if the current user is running in "Admin"
//      mode or not.
//
//  NOTE: This code added to plug a security hole.  It first tests if we are
//        running under NT If so it gets an access token and tests for the
//        right to do scheduling.  If this right is not present, the run item
//        is excluded when the combo box is filled afterward.
//
// Return Value:
//      TRUE if the current user is running in "Admin" mode, FALSE if not
//
//***************************************************************************
// 07/09/98 SEDWARD Function Created.
//***************************************************************************

BOOL LOCAL IsAdminMode(void)
{
    auto    BOOL                        bRunItem = FALSE;
    auto    DWORD                       dwSize = 0;
    auto    int                         i;
    auto    int                         pid;
    auto    HANDLE                      ph;
    auto    HANDLE                      token;
    auto    PSID                        pSid = NULL;
    auto    PTOKEN_GROUPS               pGroupInfo;
    auto    SID_IDENTIFIER_AUTHORITY    SIDAuth = SECURITY_NT_AUTHORITY;

    ph = NULL;
    pGroupInfo = NULL;

    // Get the current process ID
    pid = _getpid();

    // Get the process handle from the ID
    ph = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if(ph == NULL)
        {
        goto security_exit;
        }

    // Get the Access Token corresponding to our process
    if(!OpenProcessToken(ph, TOKEN_READ, &token))
        {
        goto security_exit;
        }

    // Get the security info; first get the number of groups
    GetTokenInformation(token, TokenGroups, NULL, dwSize, &dwSize);

    // Allocate a buffer for the groups
    pGroupInfo = (PTOKEN_GROUPS) GlobalAlloc(GPTR, dwSize);
    if(!pGroupInfo)
        {
        goto security_exit;
        }

    // Get the group information
    if(!GetTokenInformation(token, TokenGroups, pGroupInfo, dwSize, &dwSize))
        {
        goto security_exit;
        }

    // Create a security object (SID) for the administrator group
    if(!AllocateAndInitializeSid(&SIDAuth
                                    , 2
                                    , SECURITY_BUILTIN_DOMAIN_RID
                                    , DOMAIN_ALIAS_RID_ADMINS
                                    , 0
                                    , 0
                                    , 0
                                    , 0
                                    , 0
                                    , 0
                                    , &pSid))
        {
        goto security_exit;
        }

    // Iterate through the groups that our process belongs to, and compare each
    // to the administrator group; if we find a match, enable the "Run" Item
    for(i = 0; i < pGroupInfo->GroupCount; i++)
        {
        if(EqualSid(pSid, pGroupInfo->Groups[i].Sid))
            {
            bRunItem = TRUE;
            goto security_exit;
            }
        }


security_exit:
    if(ph)
        {
        CloseHandle(ph);
        }

    if(pGroupInfo)
        {
        GlobalFree(pGroupInfo);
        }

    if(pSid)
        {
        FreeSid(pSid);
        }

    return (bRunItem);

}  // end of "IsAdminMode"


