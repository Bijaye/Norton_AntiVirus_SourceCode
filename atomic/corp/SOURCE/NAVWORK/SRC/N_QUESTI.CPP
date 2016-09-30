// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_questi.cpv   1.17   28 Jul 1998 19:21:48   tcashin  $
//
// Description:
//      Routines for the ask question dialog for repair/delete/exclude...
//      one or all from Scan Results screen.
//      Also enumeration routine for list box if rep/del/exc... all files
//      is selected.
//
// Contains:
//      AskQuestion()
//      DoAll()
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_questi.cpv  $
// 
//    Rev 1.17   28 Jul 1998 19:21:48   tcashin
// Updated parameters to MarkCompressedFilesQuarantined() in
// ProcessItemFurther().
// 
//    Rev 1.16   21 Jul 1998 13:44:50   tcashin
// Displaying a wrong zip file name in a progress dialog when quarantining.
// 
//    Rev 1.15   13 Jul 1998 18:35:02   tcashin
// Removed extra calls to NetAlert().
// 
//    Rev 1.14   12 Jul 1998 17:34:40   tcashin
// Tweaked the file name being displayed in the progress bar when compressed
// files are being quarantined.
// 
//    Rev 1.13   10 Jul 1998 11:50:06   DBuches
// Removed unused help topics.
// 
//    Rev 1.12   01 Jul 1998 14:11:12   DBuches
// Removed How-To help.
// 
//    Rev 1.11   28 Jun 1998 14:04:02   tcashin
// Display the correct file name when putting zip files in quarantine.
// 
//    Rev 1.10   01 Jun 1998 16:08:40   tcashin
// Added support to quarantine compressed/zip files.
// 
//    Rev 1.9   21 May 1998 15:37:36   tcashin
// Added more quarantine help.
// 
//    Rev 1.8   12 May 1998 14:21:06   tcashin
// Count deleted and quarantined items individually.
// 
//    Rev 1.7   27 Apr 1998 17:59:46   TCASHIN
// Added quarantine alerts.
// 
//    Rev 1.6   03 Apr 1998 11:58:18   tcashin
// Added some of the new quarantine code.
// 
//    Rev 1.5   09 Mar 1998 23:43:30   DALLEE
// Changes for new (MFC-based) Repair Wizard.
// 
//    Rev 1.4   15 Jul 1997 14:36:58   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.3   06 Jun 1997 11:43:12   TIVANOV
// fix a bug - double notify message when quarantine all files
//
//    Rev 1.2   22 May 1997 16:20:56   BILL
// Alpha changes (removed ctl3d)
//
//    Rev 1.1   02 May 1997 12:00:16   TIVANOV
// new functionality to support the quarantine - AskQuestionDlgProc; DoInitDialo
// g; ProcessItems; PrecessItemFurther
//
//    Rev 1.0   06 Feb 1997 21:02:30   RFULLER
// Initial revision
//
//    Rev 1.15   23 Nov 1996 15:12:48   JBRENNA
// NameDisplayShortenedFileName(): Add a SYM_ASSERT to ensure that the caller
// provides a valid window.
//
//    Rev 1.14   08 Nov 1996 17:40:46   JBRENNA
// Move ShortenFileNameToWindow(), FormatFileName(), and CopyCharsThatFit() to
// NAVXUTIL\NavName.cpp. This is so that the functions can be used by multiple
// projects.
//
//    Rev 1.13   05 Nov 1996 17:45:22   GWESTER
// Added a parameter back to a call to SPRINTF so that the virus name,
// not garbage characters, wind up in a notification box.
//
//    Rev 1.12   30 Oct 1996 17:20:48   JBRENNA
// FormatFileName(): Change while condition to handle when a partial
// file name is copied.
//
//    Rev 1.11   30 Oct 1996 17:04:08   JBRENNA
// 1. Change AskQuestionDlgProc() to use ShortenFileNameToControl() and
//    FormatFileName() when building the file related question.
// 2. Rename NameShortenFileNameForControl() to ShortenFileNameToWindow().
// 3. Completely rewrite ShortenFileNameToWindow() to accurately calculate
//    the number of characters to change to "..."
// 4. Move adding CRLFs to new FormatFileName() function.
//
//    Rev 1.10   19 Sep 1996 15:10:08   DALLEE
// Blow off WM_HELP and WM_CONTEXTMENU in ConfirmDeleteDlg() so it doesn't
// get passed back and bring up main window help.
//
//    Rev 1.9   19 Aug 1996 14:01:00   GWESTER
// For Kirin, removed the "#ifdef NAVSCANW" statements around download
// and "GloballySaved_..." code, just like in LuigiPlus. This code will
// now get compiled whether you're compiling for Win95 or NT.
//
//    Rev 1.8   07 Aug 1996 15:40:12   JBRENNA
// LuigiPlus Port:
// DALLEE: Don't allow InocAll to act on repaired items -- according to the
//   file status structure, this operation isn't allowed. Also,
//   UpdateFileStatus() has no idea how to change REPAIRED based on results
//   from an Inoc attempt.
//
// JBRENNA: STS 58214 - Treat MBR and BR infections as a single infection
//   class when processing the Repair All button.
//
//
//    Rev 1.7   11 Jul 1996 08:03:10   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.6   28 Jun 1996 15:17:54   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.5   24 Jun 1996 21:28:28   RHAYES
// Add.'l OEM<->Ansi mod.'s (new and from Luigi).
//
//    Rev 1.4   24 May 1996 12:06:34   GWESTER
// Added confirmation flag on Do All dialog if user clicks Stop.
//
//    Rev 1.3   09 May 1996 17:58:32   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.2   22 Feb 1996 14:07:20   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.1   14 Feb 1996 17:02:48   JWORDEN
// Process internal file names in OEM mode to allow scan, and repair, and
// innoculation of file names containing certain non-ANSI translatable
// characters.
//
//    Rev 1.0   30 Jan 1996 15:55:24   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:26   BARRY
// Initial revision.
//
//    Rev 1.44   17 Jan 1996 22:35:42   YVANG
// For NAVSCANW: Modified to hide an ID_DOALL_FILLBAR_BOX control for 3-D frame
// with run-time check under Win95.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "ngui.h"
#include "stdhelp.h"

#include "ctl3d.h"

#include "nonav.h"
#include "symcfg.h"
#include "navutil.h"
#include "options.h"
#include "navw_hlp.h"
#include "cmbstr.h"
#include "nunavdlg.h"
#include "nunavstb.h"

#include "stub.h"
#include "virscan.h"
#include "inoc.h"
// #include "nonav.h"
#include "dbcs.h"


extern HINSTANCE hInstance;

// These are globally saved pointers to both kinds of info objects;
// they are only needed in an NT login/logout situation.
LPASKQUESTSTRUCT    GloballySaved_lpAsk =   NULL;
LPDOALLSTRUCT       GloballySaved_lpDo  =   NULL;

// ************************************************************************ //
// ****                       Local Defines                            **** //
// ************************************************************************ //

#define CONFIRMDELETE_CANCEL        0
#define CONFIRMDELETE_YES           1
#define CONFIRMDELETE_ALL           2
#define CONFIRMDELETE_NO            3

// ************************************************************************ //
// ****                      Local Prototypes                          **** //
// ************************************************************************ //

#ifdef SYM_WIN32

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
    (
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

#endif

UINT SYM_EXPORT WINAPI AskQuestionDlgProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

BOOL SYM_EXPORT WINAPI DoAllDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    );

VOID LOCAL MoveButtons (
    HWND    hDlg
    );

BOOL LOCAL ProcessItemFurther (
    HWND            hDlg, 
    LPDOALLSTRUCT   lpDo, 
    LPINFECTEDFILE  lpInfected,
    UINT            uIndex
    );

VOID LOCAL ProcessItem ( 
    HWND            hDlg, 
    LPDOALLSTRUCT   lpDo, 
    LPINFECTEDFILE  lpInfected,
    UINT            uIndex
    );

VOID LOCAL ProcessAllItems (
    HWND            hDlg,
    LPDOALLSTRUCT   lpDo
    );

VOID LOCAL DoInitDialog (
    HWND    hDlg,
    LPARAM  lParam
    );

UINT SYM_EXPORT WINAPI YesNoDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    );

LPDWORD LOCAL GetContextWhatsThisArray (
    UINT    uType
    );


UINT LOCAL GetContextHelpButton (       // Get correct help button ID
    UINT    uType
    );

int LOCAL ConfirmDelete (               // Confirm Delete All
    HWND    hParent,                    // Returns COMFIRMDELETE_xxx
    LPSTR   lpszFile );

BOOL CALLBACK ConfirmDeleteDlgProc (
    HWND    hWnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam );

// ************************************************************************ //

UINT SYM_EXPORT WINAPI AskQuestion (LPASKQUESTSTRUCT lpAsk)
{
    extern       HINSTANCE       hInstance;
    auto         DLGPROC         lpfn;
    auto         int             nStatus;


                                        // ---------------------------------
                                        // For NT logouts, save a handle:
                                        // ---------------------------------
    GloballySaved_lpAsk = lpAsk;

    lpfn = (DLGPROC) MakeProcInstance((FARPROC)AskQuestionDlgProc, hInstance);
    nStatus = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ASKQUESTION),
                                 lpAsk->hDlg, lpfn, (LPARAM)lpAsk);
    FreeProcInstance((FARPROC) lpfn);

                                        // ---------------------------------
                                        // For NT logouts, NULL the handle:
                                        // ---------------------------------
    GloballySaved_lpAsk = NULL;

    return((UINT) nStatus);
}


//************************************************************************
// AskQuestionDlgProc()
//
// Handles all the repair/delete/inoculate all files/one file dialogs
// from Problems Found.
//
// Parameters:
//
// Returns:
//
//************************************************************************
//  3/04/94 DALLEE, added strings/controls for registration exclude dlgs.
//************************************************************************

UINT SYM_EXPORT WINAPI AskQuestionDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    BOOL                bReturn = FALSE;
    auto    LPASKQUESTSTRUCT    lpAsk;
    auto    UINT                uQuestNum;
    auto    UINT                uMsgNum;
    auto    UINT                uWinTitle;
    auto    UINT                uButt1Txt;
    auto    UINT                uButt2Txt;
    auto    LPSTR               lpMsg;
    auto    LPSTR               lpMessage;
    auto    LPSTR               lpMessage2;
    auto    char                szFullPath [ SYM_MAX_PATH+1 ];

    switch(message)
        {
        case WM_CTLCOLOR:
            bReturn = (UINT)Ctl3dCtlColorEx(message, wParam, lParam);
            break;
        case WM_INITDIALOG:
            NAV_CenterDialog (hDlg);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, IDI_ASK_RAPFLD);
            lpAsk = (LPASKQUESTSTRUCT)lParam;
            LP_SET(hDlg, lpAsk);


            switch (lpAsk->uType)
                {
                case ASKQUESTION_REPAIR_INOCHANGED:
                    uMsgNum   = IDS_FILEINOCCHANGED_SHORT;
                    uQuestNum = IDS_ASK_REPAIR_INOCHANGE;
                    uWinTitle = IDS_TITLE_REPAIRFILE;
                    uButt1Txt = IDS_BUTTON_REPAIR;
                    uButt2Txt = IDS_BUTTON_REPAIRALL;
                    break;

                case ASKQUESTION_REPAIR_INFECTED:
                    uMsgNum   = IDS_VIRUSINFILE;
                    uQuestNum = IDS_ASK_REPAIR_INFECT;
                    uWinTitle = IDS_TITLE_REPAIRFILE;
                    uButt1Txt = IDS_BUTTON_REPAIR;
                    uButt2Txt = IDS_BUTTON_REPAIRALL;

                    break;

                case ASKQUESTION_REPAIR_BOOT:
                    uMsgNum   = IDS_VIRUSINBOOT;
                    uQuestNum = IDS_ASK_REPAIR_BOOT;
                    uWinTitle = IDS_TITLE_REPAIRBOOT;
                    uButt1Txt = IDS_BUTTON_REPAIR;
                    uButt2Txt = IDS_BUTTON_REPAIRALL;

                    break;

                case ASKQUESTION_REPAIR_MASTER_BOOT:
                    uMsgNum   = IDS_VIRUSINMASTERBOOT;
                    uQuestNum = IDS_ASK_REPAIR_MASTER_BOOT;
                    uWinTitle = IDS_TITLE_REPAIRMASTERBOOT;
                    uButt1Txt = IDS_BUTTON_REPAIR;
                    uButt2Txt = IDS_BUTTON_REPAIRALL;

                    break;

                case ASKQUESTION_DELETE_INFECTED:
                    uMsgNum   = IDS_VIRUSINFILE;
                    uQuestNum = IDS_ASK_DELETE_INFECT;
                    uWinTitle = IDS_TITLE_DELETEFILE;
                    uButt1Txt = IDS_BUTTON_DELETE;
                    uButt2Txt = IDS_BUTTON_DELETEALL;
                    break;

                case ASKQUESTION_DELETE_INOCHANGED:
                    uMsgNum   = IDS_FILEINOCCHANGED_SHORT;
                    uQuestNum = IDS_ASK_DELETE_INOCHANGE;
                    uWinTitle = IDS_TITLE_DELETEFILE;
                    uButt1Txt = IDS_BUTTON_DELETE;
                    uButt2Txt = IDS_BUTTON_DELETEALL;
                    break;

                case ASKQUESTION_DELETE_UNINOCULATED:
                    uMsgNum   = IDS_FILEISNOTINOCULATED;
                    uQuestNum = IDS_ASK_DELETE_UNINOC;
                    uWinTitle = IDS_TITLE_DELETEFILE;
                    uButt1Txt = IDS_BUTTON_DELETE;
                    uButt2Txt = IDS_BUTTON_DELETEALL;
                    break;

                case ASKQUESTION_DELETE_INOCULATERROR:
                    uMsgNum   = IDS_FILENOTINOCULATED;
                    uQuestNum = IDS_ASK_DELETE_INOCERROR;
                    uWinTitle = IDS_TITLE_DELETEFILE;
                    uButt1Txt = IDS_BUTTON_DELETE;
                    uButt2Txt = IDS_BUTTON_DELETEALL;
                    break;

                case ASKQUESTION_DELETE_ERROR:
                    uMsgNum   = IDS_FILEERROR_SHORT;
                    uQuestNum = IDS_ASK_DELETE_ERROR;
                    uWinTitle = IDS_TITLE_DELETEFILE;
                    uButt1Txt = IDS_BUTTON_DELETE;
                    uButt2Txt = IDS_BUTTON_DELETEALL;
                    break;

                case ASKQUESTION_DELETE_NOTREPAIRED:
                    uMsgNum   = IDS_FILENOTREPAIRED_SHORT;
                    uQuestNum = IDS_ASK_DELETE_NOREPAIRED;
                    uWinTitle = IDS_TITLE_DELETEFILE;
                    uButt1Txt = IDS_BUTTON_DELETE;
                    uButt2Txt = IDS_BUTTON_DELETEALL;
                    break;

                case ASKQUESTION_INOCULATE_INOCHANGED:
                    uMsgNum   = IDS_FILEINOCCHANGED_SHORT;
                    uQuestNum = IDS_ASK_INOC_INOCHANGE;
                    uWinTitle = IDS_TITLE_INOCULATEFILE;
                    uButt1Txt = IDS_BUTTON_INOCULATE;
                    uButt2Txt = IDS_BUTTON_INOCULATEALL;
                    break;

                case ASKQUESTION_INOCULATE_ALLCLEANFILES:
                    uMsgNum   = IDS_FILEISNOTINOCULATED;
                    uQuestNum = IDS_ASK_INOC_UNINOC;
                    uWinTitle = IDS_TITLE_INOCULATEFILE;
                    uButt1Txt = IDS_BUTTON_INOCULATE;
                    uButt2Txt = IDS_BUTTON_INOCULATEALL;
                    break;

                case ASKQUESTION_EXCLUDE_INOCHANGES:
                    uMsgNum   = IDS_FILEINOCCHANGED_SHORT;
                    uQuestNum = IDS_ASK_EXCLUDE_INOC;
                    uWinTitle = IDS_TITLE_EXCLUDEFILE;
                    uButt1Txt = IDS_BUTTON_EXCLUDE;
                    uButt2Txt = 0;
                    break;

                case ASKQUESTION_EXCLUDE_KNOWNVIRUS:
                    uMsgNum   = IDS_VIRUSINFILE;
                    uQuestNum = IDS_ASK_EXCLUDE_VIRSCHECK;
                    uWinTitle = IDS_TITLE_EXCLUDEFILE;
                    uButt1Txt = IDS_BUTTON_EXCLUDE;
                    uButt2Txt = 0;
                    break;

                case ASKQUESTION_EXCLUDE_NOTINOCULATED:
                    uMsgNum   = IDS_FILEISNOTINOCULATED;
                    uQuestNum = IDS_ASK_EXCLUDE_INOC;
                    uWinTitle = IDS_TITLE_EXCLUDEFILE;
                    uButt1Txt = IDS_BUTTON_EXCLUDE;
                    uButt2Txt = 0;
                    break;

                case ASKQUESTION_QUARANTINE_ALL_INFECTED:
                    uMsgNum   = IDS_VIRUSINFILE;
                    uQuestNum = IDS_ASK_MOVE_INFECT;
                    uWinTitle = IDS_TITLE_MOVEFILE;
                    uButt1Txt = IDS_BUTTON_MOVE;
                    uButt2Txt = IDS_BUTTON_MOVEALL;
                    break;

                default:
                    uMsgNum   = IDS_VIRUSINFILE;
                    uQuestNum = IDS_ASK_GIMMEABREAK;
                    uWinTitle = IDS_BUTTON_ERROR;
                    uButt1Txt = IDS_BUTTON_ERROR;
                    uButt2Txt = 0;
                    break;
                }
                                        // -------------------------------
                                        // Set the Window title and the
                                        // text to the push buttons.
                                        // -------------------------------
            SetWindowText (hDlg, String_Ptr(uWinTitle));
            SetWindowText (GetDlgItem(hDlg, IDI_ASK_ONLYONE),
                            String_Ptr(uButt1Txt));
            if (uButt2Txt)
                {
                SetWindowText (GetDlgItem(hDlg, IDI_ASK_ALL),
                               String_Ptr(uButt2Txt));
                }
            else
                {
                Button_Enable (GetDlgItem(hDlg, IDI_ASK_ALL), FALSE);
                ShowWindow    (GetDlgItem(hDlg, IDI_ASK_ALL), SW_HIDE);
                MoveButtons (hDlg);
                }

            lpMsg = lpMessage = NULL;

            if ((lpMsg      = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE)) != NULL &&
                (lpMessage  = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE * 2)) != NULL &&
                (lpMessage2 = (LPSTR)MemAllocPtr(GHND, SIZE_MESSAGE * 2)) != NULL)
                {
                String_Get (uMsgNum, lpMsg, SIZE_MESSAGE);

                if (uMsgNum == IDS_VIRUSINMASTERBOOT)
                    {
                    SPRINTF (szFullPath, "%d", *lpAsk->lpFileName);
                    SPRINTF (lpMessage, lpMsg, szFullPath, lpAsk->lpVirusName);
                    }
                else
                    {
                    NavNameShortenToWindow (GetDlgItem (hDlg, IDI_ASK_RAPFLD),
                                            lpAsk->lpFileName, szFullPath, 3, 0);

                                        // Manually put the filename into lpMsg
                    NavNameFormat (GetDlgItem (hDlg, IDI_ASK_RAPFLD), lpMsg,
                                   szFullPath, lpMessage);
                    }

                STRCAT (lpMessage, "\r\n\r\n");

                String_Get (uQuestNum, &lpMessage[STRLEN(lpMessage)],
                            SIZE_MESSAGE - STRLEN(lpMessage));

                SPRINTF (lpMessage2, lpMessage, lpAsk->lpVirusName);

                Static_SetText(GetDlgItem(hDlg, IDI_ASK_RAPFLD), lpMessage2);
                }

            if (lpMsg)
                MemFreePtr(lpMsg);
            if (lpMessage)
                MemFreePtr(lpMessage);

            bReturn = TRUE;
            break;

                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
                {
                StdHelpSetContext (HELP_DLG_ASK_ACTION, HELP_DLG_ASK_ACTION);
                }
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // or standard context sensitive help
                                        // ----------------------------------
            OnContextMenu ( hDlg, message, wParam, lParam );
            bReturn = TRUE;
            break;
#endif

        case WM_COMMAND:
            {
            LPASKQUESTSTRUCT    lpAsk = (LPASKQUESTSTRUCT)LP_GET(hDlg);

            if (!lpAsk)
                {LP_SET (hDlg, GloballySaved_lpAsk);
                 lpAsk = (LPASKQUESTSTRUCT)LP_GET(hDlg);
                 /*SYM_ASSERT (lpAsk);*/}
            SYM_ASSERT ( lpAsk );

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDI_ASK_ONLYONE:
                    EndDialog(hDlg, ASKQUESTION_ONLYONE);
                    break;

                case IDI_ASK_ALL:
                   EndDialog(hDlg, ASKQUESTION_ALL);
                   break;

                case IDCANCEL:
                   EndDialog(hDlg, ASKQUESTION_CANCEL);
                   break;

                case IDHELP:
                   StdHelpFromDialog (
                                hDlg,
                                GetContextHelpButton (lpAsk->uType)
                                );
                   break;
                }
            }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;
        }

    return ( bReturn );
}


VOID LOCAL MoveButtons (HWND hDlg)
{
    auto    RECT  RectAll;
    auto    RECT  RectCancel;
    auto    POINT ptUpperLeft;
    auto    POINT ptLowerRight;

    GetWindowRect (GetDlgItem(hDlg, IDI_ASK_ALL), &RectAll);
    GetWindowRect (GetDlgItem(hDlg, IDCANCEL), &RectCancel);

    ptUpperLeft.x  = RectAll.left;
    ptUpperLeft.y  = RectAll.top;
    ptLowerRight.x = RectAll.right;
    ptLowerRight.y = RectAll.bottom;

    ScreenToClient (hDlg, &ptUpperLeft );
    ScreenToClient (hDlg, &ptLowerRight);

    MoveWindow (GetDlgItem(hDlg, IDCANCEL), ptUpperLeft.x, ptUpperLeft.y,
                ptLowerRight.x - ptUpperLeft.x,
                ptLowerRight.y - ptUpperLeft.y, TRUE);

    ptUpperLeft.x  = RectCancel.left;
    ptUpperLeft.y  = RectCancel.top;
    ptLowerRight.x = RectCancel.right;
    ptLowerRight.y = RectCancel.bottom;

    ScreenToClient (hDlg, &ptUpperLeft );
    ScreenToClient (hDlg, &ptLowerRight);

    MoveWindow (GetDlgItem(hDlg, IDHELP), ptUpperLeft.x, ptUpperLeft.y,
                ptLowerRight.x - ptUpperLeft.x,
                ptLowerRight.y - ptUpperLeft.y, TRUE);
}


// ------------------------------------------------------------------------ //


VOID SYM_EXPORT WINAPI DoAll (
    HWND            hDlg,
    LPDOALLSTRUCT   lpDo
    )
{
    auto        DLGPROC         lpfn;
    auto        int             nStatus;

                                        // ---------------------------------
                                        // For NT logouts, save a handle:
                                        // ---------------------------------
    GloballySaved_lpDo = lpDo;

    lpfn = (DLGPROC) MakeProcInstance (
                                    (FARPROC)DoAllDlgProc,
                                    hInstance
                                    );
    SYM_ASSERT ( lpfn );

    nStatus = DialogBoxParam (
                            hInstance,
                            MAKEINTRESOURCE(IDD_DOALL),
                            hDlg,
                            lpfn,
                            (LPARAM)lpDo
                            );

    SYM_ASSERT ( nStatus != -1 );

    FreeProcInstance((FARPROC) lpfn);

    InvalidateRect ( hDlg, NULL, TRUE );
    UpdateWindow ( hDlg );

                                        // ---------------------------------
                                        // For NT logouts, free the handle:
                                        // ---------------------------------
    GloballySaved_lpDo = NULL;
}


// ************************************************************************ //

BOOL SYM_EXPORT WINAPI DoAllDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
                                        // lStopFlags stored in DWL_USER.
    #define STOP_TIMER_RUNNING  0x01    // Timer running (min time not elapsed)
    #define STOP_DOALL_DONE     0x02    // DoAll task is completed.

    auto    LONG            lStopFlags;

    auto    LPDOALLSTRUCT   lpDo;
    auto    BOOL            bReturn = FALSE;
    auto    char            Buffer1 [SYM_MAX_PATH];
    auto    char            Buffer2 [SYM_MAX_PATH];

    switch(message)
        {
        case WM_CTLCOLOR:
            bReturn = (BOOL)Ctl3dCtlColorEx(message, wParam, lParam);
            break;

        case WM_INITDIALOG:
            NG_SetFontsInDialog (hDlg, 0);
                                        // Timer to keep dialog on screen
                                        // for at least 1.5 seconds.
            lStopFlags = SetTimer(hDlg, 1, 1500, NULL) ?
                STOP_TIMER_RUNNING : 0;
                                        // Save whether timer started.
            SetWindowLong(hDlg, DWL_USER, lStopFlags);

            DoInitDialog(hDlg, lParam);
            NG_SetGroupBoxSunken(hDlg, ID_DOALL_BOX);

#ifdef NAVSCANW
            if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
                NG_SetGroupBoxSunken(hDlg,ID_DOALL_FILLBAR_BOX);
            else
                ShowWindow(GetDlgItem(hDlg, ID_DOALL_FILLBAR_BOX), SW_HIDE);
#endif
            ShowWindow ( hDlg, SW_SHOW );
            UpdateWindow ( hDlg );

            PostMessage(hDlg, WM_NAVWORKUSER, 0, 0L);
            bReturn = TRUE;
            break;

        case WM_TIMER:
                                        // Indicate minimum time elapsed,
                                        // and timer killed.
            SYM_ASSERT(KillTimer(hDlg, 1));
            lStopFlags = GetWindowLong(hDlg, DWL_USER) & ~STOP_TIMER_RUNNING;
            SetWindowLong(hDlg, DWL_USER, lStopFlags);

                                        // Check if still processing items...
            if (STOP_DOALL_DONE & lStopFlags)
                {
                EndDialog(hDlg, 0);     // Done, okay to exit now.
                }
            break;

        case WM_NAVWORKUSER:
            UpdateWindow(hDlg);

            lpDo = (LPDOALLSTRUCT)LP_GET(hDlg);
            if (!lpDo)
                {
                LP_SET (hDlg, GloballySaved_lpDo);
                lpDo = (LPDOALLSTRUCT)LP_GET(hDlg);
                SYM_ASSERT (lpDo);
                }

            ProcessAllItems(hDlg, lpDo);

                                        // Kill dialog if timer already
                                        // expired (or couldn't be started),
                                        // or if user cancelled.
            lStopFlags = GetWindowLong(hDlg, DWL_USER);

            if ( !(STOP_TIMER_RUNNING & lStopFlags)
                 || (TRUE == lpDo->bStop) )
                {
                EndDialog(hDlg, 0);
                }
            else                        // Otherwise, indicate processing done.
                {
                lStopFlags |= STOP_DOALL_DONE;
                SetWindowLong(hDlg, DWL_USER, lStopFlags);
                }
            break;

        case WM_DESTROY:
                                        // Make sure timer destroyed.
            if (STOP_TIMER_RUNNING & GetWindowLong(hDlg, DWL_USER))
                {
                KillTimer(hDlg, 1);
                }
            Fillbar_Close(GetDlgItem(hDlg, ID_DOALL_FILLBAR));
            LP_FREE(hDlg);
            break;

        case WM_COMMAND:
            lpDo = (LPDOALLSTRUCT)LP_GET(hDlg);
            if (!lpDo)
                {
                LP_SET (hDlg, GloballySaved_lpDo);
                lpDo = (LPDOALLSTRUCT)LP_GET(hDlg);
                SYM_ASSERT (lpDo);
                }

            switch ( GET_WM_COMMAND_ID (wParam,lParam) )
                {
                case IDCANCEL:
                    if (DA_CONFIRM_STOP & lpDo->uFlags)
                        {
                                        // Check for confirmation on Stop
                        LoadString (hInstance, IDS_DOALL_CONFIRM_STOP, Buffer1, sizeof(Buffer1));
                        LoadString (hInstance, IDS_NAV, Buffer2, sizeof(Buffer2));

                        if (IDYES == MessageBox (hDlg, Buffer1, Buffer2, MB_YESNO|MB_ICONQUESTION))
                            {
                            lpDo->bStop = TRUE;
                            }
                        }
                    else
                        {               // No confirmation. Stop automatically:
                        lpDo->bStop = TRUE;
                        }
                    break;
                }
            break;
        }

   return ( bReturn );
}

VOID LOCAL DoInitDialog (HWND hDlg, LPARAM lParam)
{
    auto    FILLBARSTYLE    fbStyle;
    auto    LPDOALLSTRUCT   lpDo;
    auto    HWND            hFillbar;
    auto    UINT            uTitle;
    auto    UINT            uMsg;


    Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
    NAV_CenterDialog (hDlg);
    NG_SetChildSmallFont (hDlg, ID_DOALL_FILE);

    lpDo = (LPDOALLSTRUCT)lParam;
    LP_SET (hDlg, lpDo);

    switch (lpDo->uType)
        {
        case ASKQUESTION_REPAIR_INOCHANGED:
        case ASKQUESTION_REPAIR_INFECTED:
            uMsg = IDS_DOALL_REPAIRINGFILES;
            uTitle = IDS_DOALL_TITLE_REPAIR;
            break;

        case ASKQUESTION_REPAIR_BOOT:
            uMsg = IDS_DOALL_REPAIRINGBOOT;
            uTitle = IDS_DOALL_TITLE_REPAIR;
            break;

        case ASKQUESTION_REPAIR_MASTER_BOOT:
            uMsg = IDS_DOALL_REPAIRINGMASTERBOOT;
            uTitle = IDS_DOALL_TITLE_REPAIR;
            break;

        case ASKQUESTION_REPAIR_ALL_INFECTED:
            uMsg = IDS_DOALL_REPAIRINGALL;
            uTitle = IDS_DOALL_TITLE_REPAIR;
            break;

        case ASKQUESTION_DELETE_INOCHANGED:
        case ASKQUESTION_DELETE_INFECTED:
        case ASKQUESTION_DELETE_UNINOCULATED:
        case ASKQUESTION_DELETE_ERROR:
        case ASKQUESTION_DELETE_NOTREPAIRED:
        case ASKQUESTION_DELETE_INOCULATERROR:
        case ASKQUESTION_DELETE_ALL_INFECTED:
            uMsg = IDS_DOALL_DELETINGFILES;
            uTitle = IDS_DOALL_TITLE_DELETE;
            break;

        case ASKQUESTION_INOCULATE_INOCHANGED:
        case ASKQUESTION_INOCULATE_ALLCLEANFILES:
            uMsg = IDS_DOALL_INOCINGFILES;
            uTitle = IDS_DOALL_TITLE_INOC;
            break;

#ifdef BANKERS
        case ASKQUESTION_REGISTER_NEW:
        case ASKQUESTION_REGISTER_CHANGED:
            uMsg   = IDS_DOALL_REGISTERINGFILES;
            uTitle = IDS_DOALL_TITLE_REGISTER;
            break;
#endif  // BANKERS

        case ASKQUESTION_QUARANTINE_ALL_INFECTED:
            uMsg   = IDS_DOALL_MOVINGFILES;
            uTitle = IDS_DOALL_TITLE_MOVE;
            break;

        default:
            uMsg = IDS_DOALL_REPAIRINGFILES;
            uTitle = IDS_DOALL_TITLE_REPAIR;
            break;
        }

    Static_SetText(hDlg, String_Ptr(uTitle));
    Static_SetText(GetDlgItem(hDlg, ID_DOALL_ACTION),
                   String_Ptr(uMsg));

    // ********************************************************* //
    // Initialize fillbar with all the currently required values //
    // ********************************************************* //

    hFillbar = GetDlgItem(hDlg, ID_DOALL_FILLBAR);

    fbStyle.nStruclen         = sizeof(fbStyle);
    fbStyle.b3D               = TRUE;
    fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
    fbStyle.nRightBottomWidth = FB_USEDEFAULT;
    fbStyle.crColor           = RGB(255, 0, 0);

    Fillbar_SetRange(hFillbar, 0, 100);
    Fillbar_Show(hFillbar, &fbStyle);

    lpDo->bStop = FALSE;
}


VOID LOCAL ProcessAllItems( HWND hDlg, LPDOALLSTRUCT lpDo )
{
    auto    LPINFARR        lpBlockList;
    auto    LPINFECTEDFILE  lpInfected;
    auto    UINT            uPercent;
    auto    UINT            uNumItems;
    auto    UINT            uCurrentItem;
    auto    UINT            i;

    auto    LPNONAV         lpNav;

    lpNav = lpDo->lpNav;

    // DALLEE 3/3/1998: Klugery to support a list view in the
    // new Repair Wizard - there is no list control with the infected
    // items available; use the full LPINFARR in lpNav for our loop.

    if ( NULL == lpDo->hListbox )
        {
        uNumItems = lpNav->Stats.Files.uInfected +
                    lpNav->Stats.BootRecs.uInfected +
                    lpNav->Stats.MasterBootRec.uInfected;

        // Loop through all blocks of problem items.

        for ( lpBlockList = lpNav->lpInfArr, uCurrentItem = 0;
              lpBlockList && FALSE == lpDo->bStop;
              lpBlockList = ( LPINFARR ) lpBlockList->lpNext )
            {
            lpInfected = ( LPINFECTEDFILE ) MemLock( lpBlockList->hInf );

            if ( lpInfected )
                {
                for ( i = 0; 
                      i < lpBlockList->uNumUsed && FALSE == lpDo->bStop;
                      i++, uCurrentItem++, lpInfected++ )
                    {
                    uPercent = (uCurrentItem * 100) / uNumItems;
                    Fillbar_SetLevel(GetDlgItem(hDlg,ID_DOALL_FILLBAR), uPercent);

                    YieldToWindows(hDlg);

                    ProcessItem( hDlg, lpDo, lpInfected, uCurrentItem );
                    }

                MemUnlock( lpBlockList->hInf );
                }
            else
                {
                // Unable to lock this block's entries, just exit.

                break;
                }
            }
        }
    else
        {
        // Old style: Use list box in Problems Found for iteration.

        SetWindowRedraw(lpDo->hListbox, FALSE);

        uNumItems = ListBox_GetCount(lpDo->hListbox);

        if (uNumItems != (UINT)LB_ERR)
            {
            for (i = 0; i < uNumItems && lpDo->bStop == FALSE; i++)
                {
                uPercent = (UINT)(((DWORD)i * 100) / uNumItems);
                Fillbar_SetLevel(GetDlgItem(hDlg,ID_DOALL_FILLBAR), uPercent);

                YieldToWindows(hDlg);

                lpInfected = (LPINFECTEDFILE)ListBox_GetItemData(lpDo->hListbox, i);

                if ((DWORD)lpInfected != (DWORD)LB_ERR)
                    {
                    ProcessItem( hDlg, lpDo, lpInfected, i );
                    }
                }
            }

        SetWindowRedraw ( lpDo->hListbox, TRUE );
        }

    SmartInocClose();
    lpNav->uInocOpenOK = SMARTOPEN_CLOSED;
} // ProcessAllItems();


VOID LOCAL ProcessItem( HWND            hDlg, 
                        LPDOALLSTRUCT   lpDo, 
                        LPINFECTEDFILE  lpInfected,
                        UINT            uIndex )
{
    auto    LPNONAV     lpNav;

    lpNav = lpDo->lpNav;

    switch (lpDo->uType)
        {
        case ASKQUESTION_REPAIR_INOCHANGED:
            if (lpInfected->wStatus == FILESTATUS_INOCULATEDCHANGED)
                {
                if (lpNav->Inoc.bUseInoc == TRUE)
                    {
                    lpNav->uInocOpenOK = SmartInocOpen( lpInfected->szFullPath,
                                                        FALSE );
                    }

                NameDisplayShortenedFileName( GetDlgItem( hDlg, ID_DOALL_FILE ),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_REPAIR_INFECTED:
            if (lpInfected->wStatus == FILESTATUS_INFECTED)
                {
                if (lpNav->Inoc.bUseInoc == TRUE)
                    {
                    lpNav->uInocOpenOK = SmartInocOpen( lpInfected->szFullPath,
                                                        FALSE );
                    }

                NameDisplayShortenedFileName( GetDlgItem( hDlg, ID_DOALL_FILE ),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_REPAIR_BOOT:
        case ASKQUESTION_REPAIR_MASTER_BOOT:
            if (lpInfected->wStatus == FILESTATUS_BOOTINFECTED)
                {
                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }

            else if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED)
                {
                char szPath[SYM_MAX_PATH + 1];
                SPRINTF( szPath,
                         String_Ptr( IDS_PHYS_DRIVE ),
                         *lpInfected->szFullPath );

                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              szPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_REPAIR_ALL_INFECTED:
            if ( IsMatchingStatusType(lpInfected->wStatus,
                                      ST_VIRUS | ST_CAN_REPAIR) )
                {
                char szPath[SYM_MAX_PATH + 1];

                if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED)
                    {
                    SPRINTF(szPath,
                            String_Ptr (IDS_PHYS_DRIVE),
                            *lpInfected->szFullPath);
                    }
                else
                    {
                    STRCPY(szPath, lpInfected->szFullPath);
                    }

                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              szPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_DELETE_INOCULATERROR:
            if (lpInfected->wStatus == FILESTATUS_INOCULATIONFAILED ||
                lpInfected->wStatus == FILESTATUS_REINOCULATIONFAILED)
                {
                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_DELETE_INOCHANGED:
            if (lpInfected->wStatus == FILESTATUS_INOCULATEDCHANGED)
                {
                if (lpNav->Inoc.bUseInoc == TRUE)
                    {
                    lpNav->uInocOpenOK = SmartInocOpen( lpInfected->szFullPath,
                                                        TRUE );
                    }

                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_DELETE_UNINOCULATED:
            if (lpInfected->wStatus == FILESTATUS_NOTINOCULATED)
                {
                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_DELETE_INFECTED:
            if (lpInfected->wStatus == FILESTATUS_INFECTED)
                {
                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_QUARANTINE_ALL_INFECTED:
            if (lpInfected->wStatus == FILESTATUS_INFECTED ||
                lpInfected->wStatus == FILESTATUS_NOTREPAIRED ||
                lpInfected->wStatus == FILESTATUS_ZIPINFECTED )
            {
                auto TCHAR   szZipFileName[SYM_MAX_PATH];

                if ( lpInfected->wStatus == FILESTATUS_ZIPINFECTED )
                {
                    MEMCPY(szZipFileName, lpInfected->szFullPath, _tcslen(lpInfected->szFullPath));
                    RemoveZipInfected ((LPSTR)&szZipFileName[0]);

                    NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                                  (LPSTR)&szZipFileName[0] );
                }
                else
                    NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                                  lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
            } 

            break;

        case ASKQUESTION_DELETE_ERROR:
            if (lpInfected->wStatus == FILESTATUS_ERROR)
                {
                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_DELETE_NOTREPAIRED:
            if (lpInfected->wStatus == FILESTATUS_NOTREPAIRED)
                {
                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_DELETE_ALL_INFECTED:
            if ( IsMatchingStatusType(lpInfected->wStatus,
                                      ST_VIRUS | ST_CAN_DELETE) )
                {
                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_INOCULATE_INOCHANGED:
            if (lpInfected->wStatus == FILESTATUS_INOCULATEDCHANGED)
                {
                if (lpNav->Inoc.bUseInoc == TRUE)
                    {
                    lpNav->uInocOpenOK = SmartInocOpen( lpInfected->szFullPath,
                                                        TRUE );
                    }

                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;

        case ASKQUESTION_INOCULATE_ALLCLEANFILES:
            if (lpInfected->wStatus == FILESTATUS_NOTINOCULATED)
                {
                if (lpNav->Inoc.bUseInoc == TRUE)
                    {
                    lpNav->uInocOpenOK = SmartInocOpen( lpInfected->szFullPath,
                                                        TRUE );
                    }

                NameDisplayShortenedFileName( GetDlgItem(hDlg, ID_DOALL_FILE),
                                              lpInfected->szFullPath );

                ProcessItemFurther(hDlg, lpDo, lpInfected, uIndex);
                }
            break;
        }
} // ProcessItem()

BOOL LOCAL ProcessItemFurther( HWND            hDlg, 
                               LPDOALLSTRUCT   lpDo, 
                               LPINFECTEDFILE  lpInfected,
                               UINT            uIndex )
{
    auto    LPNONAV             lpNav;
    auto    REPAIRFILESTRUCT    Repair;
    auto    BOOL                bStatus = FALSE;
    auto    UINT                uProcess;
    auto    NOTIFYSTRUCT        Notify;
    auto    BOOL                bSkipThisItem = FALSE;


    lpNav = lpDo->lpNav;

    MEMCPY( &lpNav->Infected, lpInfected, sizeof(INFECTEDFILE) );

    switch (lpDo->uType)
        {
        case ASKQUESTION_REPAIR_INOCHANGED:
        case ASKQUESTION_REPAIR_INFECTED:
        case ASKQUESTION_REPAIR_BOOT:
        case ASKQUESTION_REPAIR_MASTER_BOOT:
        case ASKQUESTION_REPAIR_ALL_INFECTED:
            uProcess = PROCESS_REPAIR;

            switch (lpInfected->wStatus)
                {
                case FILESTATUS_INOCULATEDCHANGED:
                case FILESTATUS_INFECTED:
                    MEMSET (&Repair, EOS, sizeof(Repair));

                    Repair.bUseInoc         = lpNav->Inoc.bUseInoc &&
                        !(NO_GENERIC & lpNav->Infected.Notes.wControl2);

                    Repair.bCreateBackup    = lpNav->Gen.bBackupRep;
                    Repair.uVirusSigIndex   = lpNav->Infected.uVirusSigIndex;
                    STRCPY (Repair.szFullPath, lpNav->Infected.szFullPath);
                    STRCPY (Repair.szFullOemPath, lpNav->Infected.szFullOemPath);
                    STRCPY (Repair.szBackExt, lpNav->Gen.szBackExt);

                    bStatus = RepairFile (&Repair);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uCleaned++;
                    else if (!(DA_NO_NOTIFY & lpDo->uFlags))
                        {
                        MEMSET (&Notify, EOS, sizeof(Notify));
                        Notify.hInstance    = hInstance;
                        Notify.hParent      = hDlg;
                        Notify.uMsgID       = IDS_FILENOTREPAIRED;
                        Notify.uTitleID     = IDS_NAV;
                        if ( GetCustomVersion() == VERSION_NETSCAPE )
                            {           // Title is "Internet Scanner"; set at runtime.
                            Notify.uTitleID     = IDS_NAV_INTERNET;
                            }
                        Notify.lpFileName   = lpNav->Infected.szFullPath;
                        Notify.lpVirusName  = lpNav->Infected.Notes.lpVirName;
                        Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                               lpNav->Alert.uSeconds : NULL;
                        Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                               lpNav->Alert.szAlertMsg : NULL;
                        NotifyUser (&Notify);
                        }
                    break;

                case FILESTATUS_BOOTINFECTED:
                case FILESTATUS_MASTERBOOTINFECTED:
                    bStatus = RepairBoot (lpNav, hDlg, FALSE);

                    if (bStatus == TRUE)
                        {
                        if (lpNav->Infected.wStatus == FILESTATUS_BOOTINFECTED)
                            lpNav->Stats.BootRecs.uCleaned++;
                        else
                            lpNav->Stats.MasterBootRec.uCleaned++;
                        }
                    else if (!(DA_NO_NOTIFY & lpDo->uFlags))
                        {
                        char szFileName[SYM_MAX_PATH + 1];

                        MEMSET (&Notify, EOS, sizeof(Notify));
                        Notify.hInstance    = hInstance;
                        Notify.hParent      = hDlg;
                        Notify.uTitleID     = IDS_NAV;
                        if ( GetCustomVersion() == VERSION_NETSCAPE )
                            {           // Title is "Internet Scanner"; set at runtime.
                        Notify.uTitleID     = IDS_NAV_INTERNET;
                            }
                        if (lpNav->Infected.wStatus == FILESTATUS_BOOTINFECTED)
                            {
                            Notify.uMsgID       = IDS_BOOTNOTREPAIRED;
                            STRCPY (szFileName,
                                    lpNav->Infected.szFullPath);
                            }
                        else
                            {
                            Notify.uMsgID       = IDS_MASTERBOOTNOTREPAIRED;
                            SPRINTF (szFileName,
                                     String_Ptr (IDS_PHYS_DRIVE),
                                     lpNav->Infected.szFullPath[0]);
                            }
                        Notify.lpFileName   = szFileName;
                        Notify.lpVirusName  = lpNav->Infected.Notes.lpVirName;
                        Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                               lpNav->Alert.uSeconds : NULL;
                        Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                               lpNav->Alert.szAlertMsg : NULL;
                        NotifyUser (&Notify);
                        }

                    break;
                }
            break;

        case ASKQUESTION_DELETE_INOCHANGED:
        case ASKQUESTION_DELETE_INFECTED:
        case ASKQUESTION_DELETE_UNINOCULATED:
        case ASKQUESTION_DELETE_ERROR:
        case ASKQUESTION_DELETE_NOTREPAIRED:
        case ASKQUESTION_DELETE_INOCULATERROR:
        case ASKQUESTION_DELETE_ALL_INFECTED:
                                        // Prompt before delete if needed.
            if (DA_CONFIRM_DELETE & lpDo->uFlags)
                {
                switch (ConfirmDelete(hDlg, lpNav->Infected.szFullPath))
                    {
                    case CONFIRMDELETE_YES:
                        break;

                    case CONFIRMDELETE_ALL:
                        lpDo->uFlags &= ~DA_CONFIRM_DELETE;
                        break;

                    case CONFIRMDELETE_NO:
                        bSkipThisItem = TRUE;
                        break;

                    default:
                        SYM_ASSERT(0);
                    case CONFIRMDELETE_CANCEL:
                        bSkipThisItem = TRUE;
                        lpDo->bStop = TRUE;
                        break;
                    }
                }

            if (!bSkipThisItem)
                {
                uProcess = PROCESS_DELETE;

                if (lpDo->uType == ASKQUESTION_DELETE_INOCHANGED)
                    bStatus = InocDeleteFile (lpNav, hDlg, FALSE);
                else
                {
                    bStatus = NavDeleteFile (lpNav, hDlg, FALSE);

                    if (bStatus == TRUE)
                        lpNav->Stats.Files.uDeleted++;
                }

                if ( (FALSE == bStatus) && !(DA_NO_NOTIFY & lpDo->uFlags) )
                    {
                    MEMSET (&Notify, EOS, sizeof(Notify));
                    Notify.hInstance    = hInstance;
                    Notify.hParent      = hDlg;
                    Notify.uMsgID       = IDS_FILENOTDELETED;
                    Notify.uTitleID     = IDS_NAV;
                    if ( GetCustomVersion() == VERSION_NETSCAPE )
                        {               // Title is "Internet Scanner"; set at runtime.
                        Notify.uTitleID     = IDS_NAV_INTERNET;
                        }
                    Notify.lpFileName   = lpNav->Infected.szFullPath;
                    Notify.lpVirusName  = lpNav->Infected.Notes.lpVirName;
                    Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                          lpNav->Alert.uSeconds : NULL;
                    Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                          lpNav->Alert.szAlertMsg : NULL;
                    NotifyUser (&Notify);
                    }
                }
            break;

        case ASKQUESTION_INOCULATE_INOCHANGED:
        case ASKQUESTION_INOCULATE_ALLCLEANFILES:
            uProcess = PROCESS_INOCULATE;
            bStatus = InocThisFile (lpNav, hDlg, FALSE);

            if ( (FALSE == bStatus) && !(DA_NO_NOTIFY & lpDo->uFlags) )
                {
                MEMSET (&Notify, EOS, sizeof(Notify));
                Notify.hInstance    = hInstance;
                Notify.hParent      = hDlg;
                Notify.uMsgID       = IDS_FILENOTINOCULATED;
                Notify.uTitleID     = IDS_NAV;
                if ( GetCustomVersion() == VERSION_NETSCAPE )
                    {                   // Title is "Internet Scanner"; set at runtime.
                Notify.uTitleID     = IDS_NAV_INTERNET;
                    }
                Notify.lpFileName   = lpNav->Infected.szFullPath;
                Notify.lpVirusName  = lpNav->Infected.Notes.lpVirName;
                Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                       lpNav->Alert.uSeconds : NULL;
                Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                       lpNav->Alert.szAlertMsg : NULL;
                NotifyUser (&Notify);
                }

            break;

        case ASKQUESTION_QUARANTINE_ALL_INFECTED:
            uProcess = PROCESS_QUARANTINE;

            bStatus = NavMoveFile (lpNav, hDlg, FALSE);

            if (bStatus == TRUE)
            {
                if (lpInfected->wStatus == FILESTATUS_ZIPINFECTED)
                {
                    MarkCompressedFilesQuarantined(lpNav, lpInfected, NULL, lpDo->hListbox, hDlg);
                    bSkipThisItem = TRUE;
                }
                else
                    lpNav->Stats.Files.uQuarantined++;
            }

            if ( (FALSE == bStatus) && !(DA_NO_NOTIFY & lpDo->uFlags) )
            {
                MEMSET (&Notify, EOS, sizeof(Notify));
                Notify.hInstance    = hInstance;
                Notify.hParent      = hDlg;
                Notify.uMsgID       = IDS_FILENOTMOVED;
                Notify.uTitleID     = IDS_NAV;

                if ( GetCustomVersion() == VERSION_NETSCAPE )
                {               // Title is "Internet Scanner"; set at runtime.
                    Notify.uTitleID     = IDS_NAV_INTERNET;
                }

                Notify.lpFileName   = lpNav->Infected.szFullPath;
                Notify.lpVirusName  = lpNav->Infected.Notes.lpVirName;
                Notify.uRemoveAfter = (lpNav->Alert.bRemoveAfter) ?
                                       lpNav->Alert.uSeconds : NULL;
                Notify.lpAlertMsg   = (lpNav->Alert.bDispAlertMsg) ?
                                       lpNav->Alert.szAlertMsg : NULL;
                NotifyUser (&Notify);
            }
            break;
        }

                                        // If user chose to skip processing
                                        // for this item, don't change
                                        // the state.
    if ( !bSkipThisItem )
        {
        lpInfected->wStatus = UpdateFileStatus (lpInfected->wStatus, uProcess,
                                                bStatus);

        if ( lpDo->hListbox )
            {
            ListBox_DeleteString(lpDo->hListbox, uIndex);
            ShowInfectFile(lpDo->hListbox, lpInfected, uIndex);
            }
        }

    return (bStatus);
}


//************************************************************************
// ConfirmDelete()
//
// int LOCAL ConfirmDelete (hParent, lpszName)
//  HWND    hParent
//  LPSTR   lpszFile        Candidate for deletion from ProcessAllItems()
//
// This routine asks the user if he really wants to delete the named file.
// Options are Yes, Yes to All, No, and Cancel.
//
// Returns:
//  CONFIRMDELETE_YES
//  CONFIRMDELETE_ALL
//  CONFIRMDELETE_NO
//  CONFIRMDELETE_CANCEL
//************************************************************************
// 04/18/96 DALLEE, created.
//************************************************************************

int LOCAL ConfirmDelete ( HWND hParent, LPSTR  lpszFile )
{
    auto    int     nResult;

    nResult = DialogBoxParam( hInstance,
                              MAKEINTRESOURCE(IDD_CONFIRM_DELETE),
                              hParent,
                              ConfirmDeleteDlgProc,
                              (LPARAM)lpszFile );

                                        // Treat error as cancel.
    return (-1 == nResult ? CONFIRMDELETE_CANCEL : nResult);
} // ConfirmDelete()

BOOL CALLBACK ConfirmDeleteDlgProc ( HWND   hWnd,
                                     UINT   uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam )
{
    auto    BOOL    bReturn = TRUE;

    switch(uMsg)
        {
        case WM_INITDIALOG:
            NAV_CenterDialog(hWnd);
            NG_SetFontsInDialog (hWnd, 0);

            NameDisplayShortenedFileName( GetDlgItem(hWnd, IDC_CD_FILE),
                                          (LPSTR)lParam,
                                          1 );
            break;

        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam,lParam))
                {
                case IDOK:
                    EndDialog(hWnd, CONFIRMDELETE_YES);
                    break;

                case IDC_CD_ALL:
                    EndDialog(hWnd, CONFIRMDELETE_ALL);
                    break;

                case IDC_CD_NO:
                    EndDialog(hWnd, CONFIRMDELETE_NO);
                    break;

                case IDCANCEL:
                    EndDialog(hWnd, CONFIRMDELETE_CANCEL);
                    break;

                default:
                    break;
                }
            break;

        case WM_HELP:
        case WM_CONTEXTMENU:
            break;

        default:
            bReturn = FALSE;
            break;
        }

    return (bReturn);
} // ConfirmDeleteDlgProc()


#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
            HWND    hDlg,
            UINT    uMsg
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU windows
message. The purpose of this function is to call the standard symantec help
routines that display an appropiate floating help menu.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
(
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
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
        0,              0
        };

    /*const*/   LPASKQUESTSTRUCT    lpAsk = (LPASKQUESTSTRUCT)LP_GET(hDlg);
    if (!lpAsk)
        {LP_SET (hDlg, GloballySaved_lpAsk);
        lpAsk = (LPASKQUESTSTRUCT)LP_GET(hDlg);
        /*SYM_ASSERT (lpAsk);*/}

    SYM_ASSERT ( lpAsk );

    NavDlgOnContextHelp (                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        GetContextWhatsThisArray ( lpAsk->uType ),
                        NULL, 
                        NULL,           /* This dlg. does not have any LB Ctrl*/
                        NULL            /* that's why these nulls             */
                        );
}

/* **************************************************************************
@Name:
LPDWORD LOCAL GetContextWhatsThisArray (
    UINT    uType
    );

@Description:
This function returns a pointer to the correct what's this array to use
depending on the contents of this dialog box.

@Parameters:
$uType$  ID number specifying the context that this dialog box should be.

@Returns:   Pointer to what's this array.

@Include: stdhelp.h
************************************************************************** */

LPDWORD LOCAL GetContextWhatsThisArray (
    UINT    uType
    )
{
    auto    LPDWORD     lpdwArray = NULL;


    static  DWORD       dwWhatsThisRepairMap[] =
        {
        IDD_ASKQUESTION,    IDH_NAVW_WHATHIS_REPAIR_FILE_DLG,
        IDI_ASK_ONLYONE,    IDH_NAVW_WHATHIS_REPAIR_FILE_DLG_REPAIR_BTN,
        IDI_ASK_ALL,        IDH_NAVW_WHATHIS_REPAIR_FILE_DLG_REPAIR_ALL_BTN,
        0,                  0
        };

    static  DWORD       dwWhatsThisDeleteMap[] =
        {
        IDD_ASKQUESTION,    IDH_NAVW_WHATHIS_DELETE_FILE_DLG,
        IDI_ASK_ONLYONE,    IDH_NAVW_WHATHIS_DELETE_FILE_DLG_DELETE_BTN,
        IDI_ASK_ALL,        IDH_NAVW_WHATHIS_DELETE_FILE_DLG_DELETE_ALL_BTN,
        0,              0
        };

    static  DWORD       dwWhatsThisInocMap[] =
        {
        IDD_ASKQUESTION,    IDH_NAVW_WHATHIS_INOCULATE_FILE_DLG,
        IDI_ASK_ONLYONE,    IDH_NAVW_WHATHIS_INOCULATE_FILE_DLG_INOCULATE_BTN,
        IDI_ASK_ALL,        IDH_NAVW_WHATHIS_INOCULATE_FILE_DLG_INOCULATE_ALL_BTN,
        0,              0
        };

    static  DWORD       dwWhatsThisExcludeMap[] =
        {
        IDD_ASKQUESTION,    IDH_NAVW_WHATHIS_EXCLUDE_FILE_DLG,
        IDI_ASK_ONLYONE,    IDH_NAVW_WHATHIS_EXCLUDE_FILE_DLG_EXCLUDE_BTN,
        0,              0
        };

    static  DWORD       dwWhatsThisQuarantineMap[] =
        {
        IDD_ASKQUESTION,    IDH_NAVW_QUARAN_DLG_HLP_BTN,
        IDI_ASK_ONLYONE,    IDH_NAVW_WHATHIS_QUARAN_DLG_QUARAN_BTN,
        IDI_ASK_ALL,        IDH_NAVW_WHATHIS_QUARAN_DLG_QUARAN_ALL_BTN,
        0,              0
        };

    switch ( uType )
        {
        case ASKQUESTION_REPAIR_INOCHANGED:
        case ASKQUESTION_REPAIR_INFECTED:
        case ASKQUESTION_REPAIR_BOOT:
            lpdwArray = dwWhatsThisRepairMap;
            break;

        case ASKQUESTION_DELETE_INFECTED:
        case ASKQUESTION_DELETE_INOCHANGED:
        case ASKQUESTION_DELETE_UNINOCULATED:
        case ASKQUESTION_DELETE_INOCULATERROR:
        case ASKQUESTION_DELETE_ERROR:
        case ASKQUESTION_DELETE_NOTREPAIRED:
            lpdwArray = dwWhatsThisDeleteMap;
            break;

        case ASKQUESTION_INOCULATE_INOCHANGED:
        case ASKQUESTION_INOCULATE_ALLCLEANFILES:
            lpdwArray = dwWhatsThisInocMap;
            break;

        case ASKQUESTION_EXCLUDE_INOCHANGES:
        case ASKQUESTION_EXCLUDE_KNOWNVIRUS:
        case ASKQUESTION_EXCLUDE_NOTINOCULATED:
            lpdwArray = dwWhatsThisExcludeMap;
            break;

        case ASKQUESTION_QUARANTINE_ALL_INFECTED:
            lpdwArray = dwWhatsThisQuarantineMap;
            break;

        default:
            SYM_ASSERT ( 0 );
            lpdwArray = dwWhatsThisRepairMap;
            break;
        }

    return ( lpdwArray );
}


/* **************************************************************************
@Name:
UINT LOCAL GetContextHelpButton (       // Get correct help button ID
    UINT    uType
    );

@Description:
This function returns the correct help button ID depending on context.

@Parameters:
$uType$  ID number specifying the context that this dialog box should be.

@Returns:   Help ID that should be displayed

@Include: stdhelp.h
************************************************************************** */

UINT LOCAL GetContextHelpButton (       // Get correct help button ID
    UINT    uType
    )
{
    auto    UINT    uHelp   =   0;

    switch ( uType )
        {
        case ASKQUESTION_REPAIR_INOCHANGED:
        case ASKQUESTION_REPAIR_INFECTED:
        case ASKQUESTION_REPAIR_BOOT:
            uHelp = IDH_NAVW_REPAIR_FILE_DLG_HELP_BTN;
            break;

        case ASKQUESTION_DELETE_INFECTED:
        case ASKQUESTION_DELETE_INOCHANGED:
        case ASKQUESTION_DELETE_UNINOCULATED:
        case ASKQUESTION_DELETE_INOCULATERROR:
        case ASKQUESTION_DELETE_ERROR:
        case ASKQUESTION_DELETE_NOTREPAIRED:
            uHelp = IDH_NAVW_DELETE_FILE_DLG_HELP_BTN;
            break;

        case ASKQUESTION_INOCULATE_INOCHANGED:
        case ASKQUESTION_INOCULATE_ALLCLEANFILES:
            uHelp = IDH_NAVW_INOCULATE_FILE_DLG_HELP_BTN;
            break;

        case ASKQUESTION_EXCLUDE_INOCHANGES:
        case ASKQUESTION_EXCLUDE_KNOWNVIRUS:
        case ASKQUESTION_EXCLUDE_NOTINOCULATED:
            uHelp = IDH_NAVW_EXCLUDE_FILE_DLG_HELP_BTN;
            break;

        case ASKQUESTION_QUARANTINE_ALL_INFECTED:
            uHelp = IDH_NAVW_QUARAN_DLG_HLP_BTN;
            break;

        default:
            SYM_ASSERT ( 0 );
            uHelp = IDH_NAVW_REPAIR_FILE_DLG_HELP_BTN;
            break;
        }

    return ( uHelp );
}

#endif

//***************************************************************************
// NameDisplayShortenedFileName()
//
// Description:
//   This function shortens the specified file name to the size of the
//   window specified. After shortening, this function sets the text of the
//   window to the shortened file name.
//
// Parameters:
//   hWnd      [in]  Window to size lpszName to. It is the callers
//                   responsibility to ensure that the window passed is
//                   valid (NULL is not a valid window). GPFs result for
//                   non-valid windows.
//   lpszName  [in]  File name may need shortening.
//   nLines    [in]  When shortening the file name, this determines the number
//                   lines the window will display.
//***************************************************************************
// 05/09/1995 KEITH MUND Function Created.
// 10/30/1996 Jim Brennan - Changed to use ShortenFileNameToWindow() &
//            FormatFileName().
// 11/23/1996 Jim Brennan - Added function comments & the SYM_ASSERT to ensure
//            the specified window is valid.
//***************************************************************************

VOID PASCAL NameDisplayShortenedFileName (
    HWND    hWnd,
    LPSTR   lpszName,
    int     nLines)
{
    SYM_ASSERT (::IsWindow (hWnd));

    auto   char    szFileName[SYM_MAX_PATH+1];
    auto   char    szMessage [SYM_MAX_PATH+1];

    NavNameShortenToWindow (hWnd, lpszName, szFileName, nLines, 0);
    NavNameFormat          (hWnd, "%s",     szFileName, szMessage);

    Static_SetText (hWnd, szMessage);
}
