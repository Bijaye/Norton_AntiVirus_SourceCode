// Copyright 1993 Symantec, Peter Norton Group
//*********************************************************************
// $Header:   S:/NAVALOG/VCS/l_filter.cpv   1.8   08 Oct 1998 19:40:06   tcashin  $
//
// File: l_filter.c
//
// Displays dialog to get filter information.
//
//Kedit:Tabs Incr 3
//Kedit:EditV SetF FExt CPP
//
// See Also:
// Future Enhancements:
//*********************************************************************
// 04/20/93 WRL is William R. Lee II (BLEE)
//            - Moved to Quake from NIRVANA:ACTILOG
//************************************************************************
// $Log:   S:/NAVALOG/VCS/l_filter.cpv  $
// 
//    Rev 1.8   08 Oct 1998 19:40:06   tcashin
// Added a new help ID for the access denied check box.
// 
//    Rev 1.7   22 Sep 1998 23:51:04   tcashin
// Added context help for access-denied filter checkbox.
// 
//    Rev 1.6   17 Jul 1998 18:42:52   jtaylor
// Updated dynamic filter creation to correctly create OEM and non-quarantine lists.
//
//    Rev 1.5   02 Jul 1998 11:55:34   DBuches
// Added help ID for quarantine checkbox.
//
//    Rev 1.4   01 Jul 1998 14:29:34   DBuches
// Fixed more help stuff.
//
//    Rev 1.3   25 Mar 1998 15:00:58   PVENKAT
// Moved changes from ROSWELL.  Disabling 'Innoculation' check box for OEM cases
// and uncheck the checkbox before disabling it.
//
//
//    Rev 1.2   15 Jul 1997 13:44:28   DBUCHES
// Added Ctl3d support for DEC Alpha
//
//    Rev 1.1   21 May 1997 14:54:42   BILL
// Removed ctl3d for Alpha
//
//    Rev 1.0   06 Feb 1997 20:55:02   RFULLER
// Initial revision
//
//    Rev 1.3   17 Jul 1996 15:10:42   PVENKAT
//  Fixed #59906.  (Refer to the Comment on WM_HELP).
//
//    Rev 1.2   10 Apr 1996 14:13:46   jworden
// Port DBCS Changes from Kirin
//
//    Rev 1.1   22 Feb 1996 14:12:14   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:52:22   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:14:34   BARRY
// Initial revision.
//
//    Rev 1.14   14 Jan 1996 16:15:06   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.13   05 Dec 1995 01:01:42   BARRY
// Fix What's This help for Known Virus and added VCS Log tracking
//*********************************************************************

#include "actilogi.h"                   // Project specific dcls
#include <windowsx.h>                   // ComboBox_AddString()
                                        // ComboBox_GetCurSel()
                                        // Button_GetTextLength()
                                        // Button_GetCheck() Button_Enable()
                                        // Button_SetCheck()
                                        // ComboBox_SetCurSel()
#include "symintl.h"                    //
#include "xapi.h"                       // DOSGetDate()
#include "ctl3d.h"                      // Ctl3dCtlColorEx() Ctl3dSubclassDlg()
#include "stdhelp.h"                    // IDHELP
#include "ngui.h"
#include "navutil.h"                    // HelpSetContext()
                                        // NG_SetChildSmallFont()
#include "actdlg.h"                     // Dlg ID's:  IDD_* IDI_*
#include "actstb.h"                     // String tabl IDs:  IDS_*
#include "navw_hlp.H"                   // help strings ids header
#include "n32behav.h"

#define MAX_FILTERS  9                  // Total number of filter
                                        // checkbox controls
typedef struct {
   LPRIVATE lpShowStruct;               //
   UINT FAR*aFilterDesc;                // String ID array
} PRIVFILTER, FAR *LPRIVFILTER;


static struct tagCombo {                // Associate
   UINT StrId;                          //    date select string
   WORD LogicMask;                      //    with date logic mask
} Combox[] =                            // These values are in the order
{                                       //    they appear in the combo box.
   {IDS_ON,          ALOG_EQ_START},
   {IDS_NOT_ON,      ALOG_LT_START|ALOG_GT_START},
   {IDS_BEFORE,      ALOG_LT_START},
   {IDS_BEFORE_OR_ON,ALOG_LT_START|ALOG_EQ_START},
   {IDS_AFTER,       ALOG_GT_START},
   {IDS_AFTER_OR_ON, ALOG_GT_START|ALOG_EQ_START},
   {IDS_BETWEEN,     ALOG_BETWEEN},
   {IDS_NOT_BETWEEN, ALOG_LT_START|ALOG_EQ_START|
                     ALOG_EQ_END|ALOG_GT_END}
};

    extern  HINSTANCE       hDLLInst;

//*********************************************************************
// - Local prototypes
//*********************************************************************
BOOL SYM_EXPORT WINAPI FilterDlgProc (HWND, UINT, WPARAM, LPARAM);

int LOCAL WmInitDialog(
   HWND hDlg,
   LPRIVFILTER lpFPriv);

VOID LOCAL DisplayError (
    HWND hDlg,
    UINT uMessageID);

#ifdef SYM_WIN32
VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the ...
    UINT    uMsg,						// [in] ... message
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
);
#endif

//*********************************************************************
// Possibly useful for general purpose.
//*********************************************************************
UINT SYM_EXPORT WINAPI MoveWindowToParent(
   HWND hWnd,                           // handle of window to move
   HWND hParent,                        // handle of parent window to contain
   int x,                               // horizontal position
   int y,                               // vertical position
   BOOL bRedraw)
{
   POINT pt;
   UINT rc;

   pt.x = x;
   pt.y = y;
   ScreenToClient(hParent, &pt);

   rc = SetWindowPos(hWnd, hParent, pt.x, pt.y, 0, 0,
                     SWP_NOSIZE | (bRedraw ? 0 : SWP_NOREDRAW) | SWP_NOZORDER);
   return rc;

} // MoveWindowToParent()


//** FilterActivityLog() **********************************************
// Display and process Filter Dialog
//
// Future Enhancements:
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************
int SYM_EXPORT WINAPI FilterActivityLog(    // Display filter dialog
   HWND     hParent,                    // The dude calling me
   UINT FAR *aFilterDesc,               // Array of String IDs to
                                        //    filter descriptions
   LPRIVATE lpShowStruct)               // Show structure information
{
   PRIVFILTER PrivFilter;               // Private data for Filter Dlg
   int      nStatus;                    // Dialog processing status


   PrivFilter.lpShowStruct = lpShowStruct; // Init private struct
   PrivFilter.aFilterDesc  = aFilterDesc;  // Init private struct

   nStatus = DialogBoxParam(hDLLInst, MAKEINTRESOURCE(IDD_ACT_FILTER),
                            hParent, (DLGPROC)FilterDlgProc,
                            (LPARAM)(LPRIVFILTER)&PrivFilter);
   return (nStatus);
} // FilterActivityLog()


//** FilterDlgProc() **************************************************
//
// Future Enhancements:
// - Resize dialog based on the number of active check box controls
//*********************************************************************
// 04/20/93 WRL
// 05/13/93 WRL Change CenterWindow to NG_CenterDialog();
//*********************************************************************
BOOL SYM_EXPORT WINAPI FilterDlgProc(       // Filter dialog box proc
   HWND hDlg,                           // Me!
   UINT message,                        // What!
   WPARAM wParam,                       //   and...
   LPARAM lParam)                       //   AND...
{
    auto    UINT    uIndex;
    auto    BOOL    bReturn = FALSE;

    switch ( message )
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(message, wParam, lParam);
            break;

        case WM_INITDIALOG:
            WmInitDialog(hDlg, (LPRIVFILTER)lParam);
            bReturn = TRUE;
            break;

#ifdef SYM_WIN32
												// --------------------------------
												// bring up "WhatsThis" popup menu
												// or invoke context sensitive help
												// --------------------------------
		case WM_CONTEXTMENU:
		case WM_HELP:
			OnContextHelp(hDlg, message, wParam, lParam);

			// As per SDK we need to send TRUE on WM_HELP and in all other
			// Dlgs/Windows without this it works, and some how here fails.
			if (message == WM_HELP)	// so, we se it to true here.
				bReturn = TRUE;		// If this is not set to TRUE, then WINDOWS passes
									// this message to the Dlg's parent and so on.
			break;

#endif

        case WM_COMMAND:
            switch ( GET_WM_COMMAND_ID(wParam, lParam) )
                {
                case IDI_AF_COMBO:
                    if ( GET_WM_COMMAND_CMD(wParam,lParam) == CBN_SELCHANGE )
                        {
                        uIndex = ComboBox_GetCurSel (GET_WM_COMMAND_HWND(wParam,lParam));

                                        // ---------------------------------
                                        // Since I wouldn't like to change
                                        // the logic, this ugly way of
                                        // enable/disable the second date
                                        // field is the only thing I camed
                                        // up with
                                        // ---------------------------------
                        if ((uIndex == 6 || uIndex == 7) &&
                            Button_GetCheck (GetDlgItem(hDlg, IDI_AF_DATED)))
                            {
                            uIndex = TRUE;
                            }
                        else
                            {
                            uIndex = FALSE;
                            }

                        EnableWindow(GetDlgItem(hDlg, IDI_AF_DATE2), uIndex);
                        }

                    break;

                case IDI_AF_DATED:
                    {
                    int i;

                    if ( GET_WM_COMMAND_CMD(wParam,lParam) == BN_CLICKED)
                        {
                        i = Button_GetCheck(GET_WM_COMMAND_HWND(wParam,lParam));

                        EnableWindow(GetDlgItem(hDlg, IDI_AF_COMBO), i);
                        EnableWindow(GetDlgItem(hDlg, IDI_AF_DATE1), i);

                        FORWARD_WM_COMMAND (
                                        hDlg,
                                        IDI_AF_COMBO,
                                        GetDlgItem(hDlg, IDI_AF_COMBO),
                                        CBN_SELCHANGE,
                                        PostMessage
                                        );
                        }
                    }
                    break;

                case IDOK:
                    {
                    int i;
                    LPRIVFILTER lpPriv;               // Dialog Proc private data
                    LPACTILOG   lpAct ;               // Used for convenience
                    lpPriv = (LPRIVFILTER)GetWindowLong(hDlg,DWL_USER);
                    lpAct  = lpPriv->lpShowStruct->lpAct;

                    for (i=0; i < lpAct->nTypes; i++)
                        {
                        if (Button_GetCheck(GetDlgItem(hDlg, i+IDI_AF_CHECKBASE) ))
                            lpAct->lpLogFilter->pTypeIncl[i] = TRUE;
                        else
                            lpAct->lpLogFilter->pTypeIncl[i] = FALSE;
                        }

                    if ( Button_GetCheck(GetDlgItem(hDlg, IDI_AF_DATED)))
                        {
                        UINT Mon, Day, Yr;
                        UINT uStatus;

                        DOSGetDate (&Mon, &Day, &Yr);

                        uStatus = NIntlGetValidDate(
                                            GetDlgItem(hDlg, IDI_AF_DATE1),
                                            &Mon, &Day, &Yr
                                            );

                        if ( uStatus == NOERR )
                            {
                            lpAct->lpLogFilter->StartDate =
                                        ConvertDateToEncoded(Mon, Day, Yr);

                            DOSGetDate (&Mon, &Day, &Yr);

                            uStatus = NIntlGetValidDate(
                                            GetDlgItem(hDlg, IDI_AF_DATE2),
                                            &Mon, &Day, &Yr
                                            );

                            if ( uStatus == NOERR )
                                {
                                lpAct->lpLogFilter->EndDate =
                                        ConvertDateToEncoded(Mon, Day, Yr);

                                Mon = ComboBox_GetCurSel(
                                            GetDlgItem(hDlg, IDI_AF_COMBO)
                                            );

                                lpAct->lpLogFilter->DateLogic =
                                            Combox[Mon].LogicMask;
                                }
                            }

                        if ( uStatus == ERR )
                            {
                            DisplayError (hDlg, IDS_WRONGDATE);
                            break;
                            }

                        }
                    else
                        {
                        lpAct->lpLogFilter->StartDate = 0;
                        lpAct->lpLogFilter->EndDate   = 0;
                        lpAct->lpLogFilter->DateLogic = 0;
                        }

                    EndDialog(hDlg, TRUE);
                    }
                    break;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    break;


                                        // -------------------------------------
										// Invoke the standard topic help
                                        // -------------------------------------
                case IDHELP:
                    StdHelpFromDialog (
                                hDlg,
                                IDH_NAVW_ACTIVITY_LOG_FILTER_HELP_BTN
                                );
                    break;


                } // switch
            break;
        }

    return ( bReturn );
}


//** WmInitDialog() ***************************************************
//
//*********************************************************************

int LOCAL WmInitDialog(
    HWND        hDlg,
    LPRIVFILTER lpFPriv )

{
    LPACTILOG   lpAct = lpFPriv->lpShowStruct->lpAct;
    int i;
    OSVERSIONINFO   osInfo ;
    BOOL            bIsNT ;

    ZeroMemory ( &osInfo, sizeof ( osInfo ) ) ;
    GetVersionEx ( &osInfo ) ;

    bIsNT = ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT ) ;

    SetWindowLong(hDlg, DWL_USER, (LONG)lpFPriv); // Save private data

    NG_SetFontsInDialog (hDlg, 0);
#if defined(SYM_WIN32)
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
        NG_SetChildSmallFont ( hDlg, IDOK );
        NG_SetChildSmallFont ( hDlg, IDCANCEL );
        NG_SetChildSmallFont ( hDlg, IDHELP );
        }
#endif
                                        // Overwrite the default title
    if ( lpAct->szTitle && *lpAct->szTitle )
        {
        TCHAR  szWork[SIZE_WORK+1];
        TCHAR  szWork2[SIZE_WORK+1];

        LoadString ( hDLLInst, IDS_FILTER, szWork2, sizeof(szWork2) );

        SPRINTF (szWork, szWork2, (LPTSTR) lpAct->szTitle);
        SetWindowText(hDlg, szWork);
        }

    if ( lpAct->nTypes < MAX_FILTERS )
        {                               // Readjust controls
        RECT Xrect, Yrect;
        HWND hWnd;
        GetWindowRect(GetDlgItem(hDlg,lpAct->nTypes+IDI_AF_CHECKBASE),
                    &Yrect);

        hWnd = GetDlgItem(hDlg, IDI_AF_DATED);
        GetWindowRect(hWnd, &Xrect);
        Yrect.top = Xrect.top - Yrect.top;
        MoveWindowToParent(hWnd, hDlg, Xrect.left, Xrect.top-Yrect.top, FALSE);

        hWnd = GetDlgItem(hDlg, IDI_AF_COMBO);
        GetWindowRect(hWnd, &Xrect);
        MoveWindowToParent(hWnd, hDlg, Xrect.left, Xrect.top-Yrect.top, FALSE);

        hWnd = GetDlgItem(hDlg, IDI_AF_DATE1);
        GetWindowRect(hWnd, &Xrect);
        MoveWindowToParent(hWnd, hDlg, Xrect.left, Xrect.top-Yrect.top, FALSE);

        hWnd = GetDlgItem(hDlg, IDI_AF_DATE2);
        GetWindowRect(hWnd, &Xrect);
        MoveWindowToParent(hWnd, hDlg, Xrect.left, Xrect.top-Yrect.top, FALSE);

        hWnd = GetDlgItem(hDlg, IDI_AF_GROUP);
        GetWindowRect(hWnd, &Xrect);
        Xrect.right = Xrect.right - Xrect.left;
        Xrect.bottom = Xrect.bottom - Yrect.top - Xrect.top;
        SetWindowPos(hWnd, hDlg, 0, 0, Xrect.right, Xrect.bottom,
                   SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);

        GetWindowRect(hDlg, &Xrect);
        Xrect.right = Xrect.right - Xrect.left;
        Xrect.bottom = Xrect.bottom - Yrect.top - Xrect.top;
        SetWindowPos(hDlg, NULL, 0, 0, Xrect.right, Xrect.bottom,
                   SWP_NOMOVE | SWP_NOREDRAW | SWP_NOZORDER);

        }

   //******************************************************************
   // Initialize all the required checkboxes.
   //******************************************************************
    for ( i=0; i < MAX_FILTERS; i++)
        {
        if (i < lpAct->nTypes)
            {
            TCHAR lpWork[SIZE_WORK+1];

            if (LoadString(GetWindowInstance(GetParent(GetParent(hDlg))),
                        lpFPriv->aFilterDesc[i], lpWork, SIZE_WORK))
                {
                SetDlgItemText(hDlg, i+IDI_AF_CHECKBASE, lpWork);

                Button_SetCheck(GetDlgItem(hDlg, i+IDI_AF_CHECKBASE),
                            lpAct->lpLogFilter->pTypeIncl[i]);
                }
            }
        else
            {                         // Disable control
            DestroyWindow(GetDlgItem(hDlg, i+IDI_AF_CHECKBASE));
//          Button_Enable(GetDlgItem(hDlg, i+IDI_AF_CHECKBASE), FALSE);
//          ShowWindow(GetDlgItem(hDlg, i+IDI_AF_CHECKBASE), FALSE);
            }
        }

// SpinButton_ResetContent(GetDlgItem(hDlg, IDI_AF_DATE1),
//                         SPN_TYPE_STRINGS, NULL);
// SpinButton_ResetContent(GetDlgItem(hDlg, IDI_AF_DATE2),
//                         SPN_TYPE_STRINGS, NULL);

    if ( !lpAct->lpLogFilter->StartDate)
        {                               // Start date not defined,
        TCHAR szDate[INTL_MAXLEN_sSHORTDATE+1]; //    so use current
        UINT uMonth, uDay, uYear;

        DOSGetDate(&uMonth, &uDay, &uYear);
        NSetIntlDate(szDate, uMonth, uDay, uYear);
        SetDlgItemText(hDlg, IDI_AF_DATE1, szDate);
        }
    else
        {
        TCHAR szDate[INTL_MAXLEN_sSHORTDATE+1]; // Use specified date
        UINT uMonth, uDay, uYear;
        ConvertEncodedDateToNormal(lpAct->lpLogFilter->StartDate,
                                 &uMonth, &uDay, &uYear);
        NSetIntlDate(szDate, uMonth, uDay, uYear);

        SetDlgItemText(hDlg, IDI_AF_DATE1, szDate);
        }

    if ( !lpAct->lpLogFilter->EndDate)
        {                               // End date not defined,
        TCHAR szDate[INTL_MAXLEN_sSHORTDATE+1]; //    so use current
        UINT uMonth, uDay, uYear;

        DOSGetDate(&uMonth, &uDay, &uYear);
        NSetIntlDate(szDate, uMonth, uDay, uYear);
        SetDlgItemText(hDlg, IDI_AF_DATE2, szDate);
        }
    else
        {
        TCHAR szDate[INTL_MAXLEN_sSHORTDATE+1]; // Use specified date
        UINT uMonth, uDay, uYear;
        ConvertEncodedDateToNormal(lpAct->lpLogFilter->EndDate,
                                 &uMonth, &uDay, &uYear);
        NSetIntlDate(szDate, uMonth, uDay, uYear);

        SetDlgItemText(hDlg, IDI_AF_DATE2, szDate);
        }

//  SpinButton_SetCurSel(GetDlgItem(hDlg, IDI_AF_DATE1), 0);
//  SpinButton_SetCurSel(GetDlgItem(hDlg, IDI_AF_DATE2), 0);

    if (lpAct->lpLogFilter->DateLogic)// Enable check box if logic set.
        Button_SetCheck(GetDlgItem(hDlg, IDI_AF_DATED), TRUE);
    else
        Button_SetCheck(GetDlgItem(hDlg, IDI_AF_DATED), FALSE);

                                     // Enable date fields
                                     // List of date inclusion options
    for ( i=0; i < sizeof(Combox)/sizeof(Combox[0]); i++)
        {
        TCHAR szTemp [ SYM_MAX_PATH ];

        LoadString ( hDLLInst, Combox[i].StrId, szTemp, sizeof(szTemp) );

        ComboBox_AddString(GetDlgItem(hDlg, IDI_AF_COMBO), szTemp );

        if (Combox[i].LogicMask == lpAct->lpLogFilter->DateLogic)
            ComboBox_SetCurSel(GetDlgItem (hDlg, IDI_AF_COMBO), i);
        }

    if ( !lpAct->lpLogFilter->DateLogic )
        ComboBox_SetCurSel(GetDlgItem (hDlg, IDI_AF_COMBO), 0);

    FORWARD_WM_COMMAND (
                    hDlg,
                    IDI_AF_DATED,
                    GetDlgItem(hDlg, IDI_AF_DATED),
                    BN_CLICKED,
                    SendMessage
                    );

    Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
    NG_CenterDialog(hDlg,GetParent(hDlg));

    return NOERR;
}


VOID LOCAL DisplayError (HWND hDlg, UINT uMessageID)
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));
    Notify.hParent       = hDlg;
    Notify.uMsgID        = uMessageID;
    Notify.uTitleID      = IDS_ERR_TITLE;
    Notify.hInstance     = hDLLInst;

    NotifyUser (&Notify);
}


#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextHelp (
            HWND    hDlg,
            UINT    uMsg,
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU or WM_HELP
windows message. The purpose of this function is to call the standard symantec help
routines that display an appropiate: floating help menu or context sensitive help.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$uMsg$  actual message

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the ...
    UINT    uMsg,						// [in] ... message
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
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
        IDI_AF_GROUP,       IDH_NAVW_ACTIVITY_LOG_FILTER_VIEW_FOLLOWING_EVENTS,
        IDI_AF_CHECKBASE,   IDH_NAVW_WHATHIS_KNOWN_VIRUS_DETECTIONS_CHKBOX,
        IDI_AF_CHECKBASE+1, IDH_NAVW_WHATHIS_INOCULATION_ACTIVITIES_CHKBOX,
        IDI_AF_CHECKBASE+2, IDH_NAVW_WHATHIS_VIRUS_LIKE_ACTIVITIES_CHKBOX,
        IDI_AF_CHECKBASE+3, IDH_NAVW_WHATHIS_COMPLETION_OF_SCANS_CHKBOX,
        IDI_AF_CHECKBASE+4, IDH_NAVW_WHATHIS_DENIED_ACCESS_CHKBOX,         // access failures
        IDI_AF_CHECKBASE+5, IDH_NAVW_WHATHIS_OPTIONS_ACTIVITY_LOG_QUARAN_CHKBX,
        IDI_AF_DATED,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATED_CHKBOX,
        IDI_AF_COMBO,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_ON_LSTBOX,
        IDI_AF_DATE1,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATE_TXTBOX,
        IDI_AF_DATE2,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATE_TXTBOX,
		0, 0
        };

    static  DWORD       dwWhatsThisMap95OEM[] =
        {
        IDI_AF_GROUP,       IDH_NAVW_ACTIVITY_LOG_FILTER_VIEW_FOLLOWING_EVENTS,
        IDI_AF_CHECKBASE,   IDH_NAVW_WHATHIS_KNOWN_VIRUS_DETECTIONS_CHKBOX,
        IDI_AF_CHECKBASE+1, IDH_NAVW_WHATHIS_VIRUS_LIKE_ACTIVITIES_CHKBOX,
        IDI_AF_CHECKBASE+2, IDH_NAVW_WHATHIS_COMPLETION_OF_SCANS_CHKBOX,
        IDI_AF_CHECKBASE+3, IDH_NAVW_WHATHIS_DENIED_ACCESS_CHKBOX,         // access failures
        IDI_AF_CHECKBASE+4, IDH_NAVW_WHATHIS_OPTIONS_ACTIVITY_LOG_QUARAN_CHKBX,
        IDI_AF_DATED,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATED_CHKBOX,
        IDI_AF_COMBO,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_ON_LSTBOX,
        IDI_AF_DATE1,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATE_TXTBOX,
        IDI_AF_DATE2,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATE_TXTBOX,
		0, 0
        };

    static  DWORD       dwWhatsThisMapNT[] =
        {
        IDI_AF_GROUP,       IDH_NAVW_ACTIVITY_LOG_FILTER_VIEW_FOLLOWING_EVENTS,
        IDI_AF_CHECKBASE,   IDH_NAVW_WHATHIS_KNOWN_VIRUS_DETECTIONS_CHKBOX,
        IDI_AF_CHECKBASE+1, IDH_NAVW_WHATHIS_COMPLETION_OF_SCANS_CHKBOX,
        IDI_AF_CHECKBASE+2, IDH_NAVW_WHATHIS_DENIED_ACCESS_CHKBOX,         // access failures
        IDI_AF_CHECKBASE+3, IDH_NAVW_WHATHIS_OPTIONS_ACTIVITY_LOG_QUARAN_CHKBX, // Quarantine
        IDI_AF_DATED,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATED_CHKBOX,
        IDI_AF_COMBO,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_ON_LSTBOX,
        IDI_AF_DATE1,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATE_TXTBOX,
        IDI_AF_DATE2,       IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_DATE_TXTBOX,
		0, 0
        };

    LPDWORD pHelpMap;

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
        pHelpMap = dwWhatsThisMapNT;
        }
    else
        {
        if( !NAV_IS_SCANONLY() )
            {
            pHelpMap = dwWhatsThisMap;
            }
        else
            {
            pHelpMap = dwWhatsThisMap95OEM;
            }
        }

	NavDlgOnContextHelp(                /* Invoke Common Context Help Handling     */
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        pHelpMap,
                        NULL,
                        NULL,           /* This dlg. does not have any LB Ctrls -  */
                        NULL            /* - that's why these nulls ...            */
                       );
}


#endif // ifdef w32 - only
