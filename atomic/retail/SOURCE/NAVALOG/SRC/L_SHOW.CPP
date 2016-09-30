//*********************************************************************
// Copyright 1993-1995 Symantec, Peter Norton Group
//
// $Header:   S:/NAVALOG/VCS/l_show.cpv   1.6   01 Jul 1998 14:29:34   DBuches  $
// File: l_show.c
//
// Main dialog to build, display, clear the activity log.
//
// See Also:
// Future Enhancements:
//*********************************************************************
// $Log:   S:/NAVALOG/VCS/l_show.cpv  $
// 
//    Rev 1.6   01 Jul 1998 14:29:34   DBuches
// Fixed more help stuff.
// 
//    Rev 1.5   18 May 1998 17:37:32   DBuches
// Call correct time function for print dialog.
//
//    Rev 1.4   11 Feb 1998 16:32:54   DALLEE
// Added current date in short format to printed activity log header.
//
//    Rev 1.3   15 Jul 1997 13:44:32   DBUCHES
// Added Ctl3d support for DEC Alpha
//
//    Rev 1.2   18 Jun 1997 10:27:58   TIVANOV
// remove the default list selection on startup
//
//    Rev 1.1   21 May 1997 14:52:02   BILL
// Removed ctl3d for Alpha
//
//    Rev 1.0   06 Feb 1997 20:55:04   RFULLER
// Initial revision
//
//    Rev 1.9   04 Nov 1996 16:18:36   GWESTER
// Removed a hardcoded string ("Activity log:") which overwrites the
// caption set in the resource. Translated DBCS versions couldn't use it.
//
//    Rev 1.8   26 Oct 1996 14:10:10   JBRENNA
// Remove call to AnsiUpper() from UpdateLogWindow(). This had the effect of
// upper casing a double byte <a> activity log file name. The code the writes
// to the activity log does not AnsiUpper() the file name. Therefore, the
// writting code would write to DB <a> but the reading would try and read from
// DB <A> filename (which doesn't contain the data).
//
//    Rev 1.7   17 Sep 1996 09:44:20   JBRENNA
// STS 69170: Fix a problem were the title for the Print dialog was "Print %s".
// Used SPRINTF instead of STRCAT.
//
//    Rev 1.6   06 Sep 1996 16:27:10   PVENKAT
// Ported Gryphon changes.
//
//          Rev 1.5   20 Aug 1996 11:03:46   PVENKAT
//      Fixed #65101.  Remove 'Print' from 'Print Activity Log' - Title string.
//
//
//    Rev 1.5   06 Sep 1996 14:59:20   RPULINT
// Moved changes over from Gryphon: Fixed defect 63851: do not display
// activity log dialog if there are no entries (0-byte file size)
//
//    Rev 1.4   07 Aug 1996 13:55:30   YVANG
// Ported changes from LuigiPlus.
//
//    Rev 1.3   11 Jul 1996 08:05:12   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.2   10 Apr 1996 14:14:24   jworden
// Port DBCS Changes from Kirin
//
//    Rev 1.1   22 Feb 1996 14:12:18   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:52:30   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:14:46   BARRY
// Initial revision.
//
//    Rev 1.34   17 Jan 1996 22:31:40   YVANG
// Modified to hide the progress box with run-time check under NAVSCANW Win95.
//
//    Rev 1.33   14 Jan 1996 16:15:08   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//*********************************************************************

#include "platform.h"
#include "actilogi.h"                   // Project specific dcls
#include "xapi.h"                       // ConvertWordToString()
#include "file.h"                       // FileFindFirst()
#include "npt.h"                        // NptPrintAsciiFile()
#include "ngui.h"
#include "ctl3d.h"                  // Ctl3dCtlColorEx()

#ifdef SYM_WIN16
    #include "vwr.h"                        // View control
#else
    #include "richedit.h"
#endif

#include "navgraph.hpp"
#include "stack.hpp"

#include "stdhelp.h"                    // IDHELP
#include "msgdisp.h"
#include "hugeoem.h"
#include "navw_hlp.h"
#include "navutil.h"
#include "security.h"
#include "actdlg.h"                     // Dlg ID's:  IDD_* IDI_*
#include "actstb.h"                     // String tabl IDs:  IDS_*

extern  HINSTANCE       hDLLInst;

#define WM_UPDATEVIEWER (WM_USER + 200)


//*********************************************************************
// LOCAL PROTOTYPES
//*********************************************************************

VOID LOCAL UpdateLogWindow (
    HWND    hDlg );

VOID LOCAL DisplayLogError (
    HWND    hDlg,
    UINT    uMessageID,
    LPTSTR   lpLogFile
    );

BOOL SYM_EXPORT WINAPI ActDlgProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM );

VOID LOCAL PASCAL SetEntriesField (
    HWND    hDlg,
    UINT    uEntries
    );

VOID LOCAL MoveButton (
    HWND    hDlg,
    UINT    uDoomedButton,
    UINT    uButtonToMove
    );


HGLOBAL SYM_EXPORT WINAPI BuildVirusInfoForPrinting (
    LPPRINTDLGSTRUCT lpPrintDlgStruct
    );

BOOL LOCAL WINAPI BuildProgress (
    HWND        hDlg,
    LPRIVATE    lpPriv
    );

BOOL SYM_EXPORT WINAPI ProgressDlgProc (
    HWND        hDlg,
    UINT        message,
    WPARAM      wParam,
    LPARAM      lParam
    );

BOOL SYM_EXPORT WINAPI MyLogCallback (
    WORD        wNumProcessed,
    WORD        wNumToProcess,
    DWORD       dwUserData
    );

VOID LOCAL ConvertFileToAnsi (
    LPTSTR       lpLogFile
    );

#ifdef SYM_WIN32

VOID LOCAL DisplayTempFile (            // Display temporary file in rtf control
    HWND    hRtfControl,                // [in] control where to display file
    LPTSTR   lpszTempFile                // [in] file to display.
    );

VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(
    HWND    hDlg,                       // [in] dialog that sent the ...
    UINT    uMsg,                        // [in] ... message
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
);

#endif

//** ShowActivityLog() ************************************************
// Display activity log dialog
//
// Future Enhancements:
//*********************************************************************

int SYM_EXPORT WINAPI ShowActivityLog(  // Display activity log
   HWND         hParent,                // Parent window handle
   LPACTILOG    lpAct                   // Parameter block
   )
{
    auto    int         nStatus = 0;    // Return code
    auto    ALOGINIT    ALogInit;       // Init structure
    auto    PRIVATE     PrivateData;    // Private dialog data
    auto    BOOL        bNoIncl;        // Is pTypeIncl array in filter?
    auto    int         i;              // array is defined.
    auto    HINSTANCE   hRTFLib = NULL;

    MEMSET ( &PrivateData, EOS, sizeof(PrivateData) );

                                        // ----------------------------------
                                        // Check existence of Log file
                                        // ----------------------------------

    if ( !FileExists(lpAct->szLogFile) )
        {
        NOTIFYSTRUCT    Notify = { 0 };

        Notify.hParent       = hParent;
        Notify.uMsgID        = IDS_ERR_EXIST;
        Notify.uTitleID      = IDS_ERR_TITLE;
        Notify.hInstance     = hDLLInst;

        NotifyUser (&Notify);

        nStatus = ERR_FILEEXIST;
        }

#ifdef SYM_WIN32
                                        // ----------------------------------
                                        // We are using the RTF control when
                                        // building for W32 platform.
                                        // ----------------------------------

    if ( !nStatus )
        {
        hRTFLib = LoadLibrary("RICHED32.DLL");

        if ( !hRTFLib )
            {
            NOTIFYSTRUCT    Notify = { 0 };

            Notify.hParent       = hParent;
            Notify.uMsgID        = IDS_ERR_RTFLOAD;
            Notify.uTitleID      = IDS_ERR_TITLE;
            Notify.hInstance     = hDLLInst;

            NotifyUser (&Notify);

            nStatus = ERR_ALOGINIT;
            }
        }
#endif

    if ( !nStatus )
        {
        ALogInit.SSize = sizeof(ALOGINIT);// Consistency check
        ALogInit.nTypes = lpAct->nTypes;  // Number of output formats
        ALogInit.lpFormats = lpAct->pFormats;

        PrivateData.hALog = ALogStart(GetWindowInstance(hParent),&ALogInit);

        if ( !PrivateData.hALog )
            {
            ErrorHandler(hParent, ERR_ALOGINIT);
            nStatus = ERR_ALOGINIT;
            }
        }

    if ( !nStatus && !lpAct->lpLogFilter->pTypeIncl )
        {                               // Ensure checkbox inclusion
        bNoIncl = TRUE;                 // pTypeIncl not specified
        lpAct->lpLogFilter->pTypeIncl =
                        (LPINT)MemAllocPtr(GHND,lpAct->nTypes*sizeof(BOOL));

        for (i=0; i < lpAct->nTypes; i++) // Init array to include all
            lpAct->lpLogFilter->pTypeIncl[i] = TRUE;
        }
    else
        bNoIncl = FALSE;                // pTypeIncl already specified

    if ( !nStatus )
        {
        PrivateData.lpAct = lpAct;

        nStatus = DialogBoxParam (
                            hDLLInst,
                            MAKEINTRESOURCE(IDD_ACT_LOG),
                            hParent,
                            ActDlgProc,
                            (LPARAM)(LPRIVATE)&PrivateData
                            );
        }

    if ( bNoIncl )
        {
        MemFreePtr(lpAct->lpLogFilter->pTypeIncl);
        lpAct->lpLogFilter->pTypeIncl = NULL;
        }

    if ( PrivateData.hALog )
        ALogDone ( PrivateData.hALog );

                                        // ----------------------------------
                                        // There blocks of memory were allocated
                                        // by the calling program and are freed
                                        // here because if the calling programs
                                        // message handler is for a modeless
                                        // window, this information would be lost
                                        // ----------------------------------
    if ( lpAct && lpAct->lpLogFilter )
        MemFreePtr ( lpAct->lpLogFilter );

    if ( lpAct )
        MemFreePtr ( lpAct );

#ifdef SYM_WIN32
                                        // ----------------------------------
                                        // Free the library now that we're
                                        // done with it.
                                        // ----------------------------------
    if ( hRTFLib )
        {
        FreeLibrary ( hRTFLib );
        }
#endif

    return ( nStatus );
}


//** ActDlgProc() *****************************************************
//
// Future Enhancements:
//*********************************************************************

BOOL SYM_EXPORT WINAPI ActDlgProc(
   HWND     hDlg,                       // This dialog handle
   UINT     message,                    // Dialog Window message
   WPARAM   wParam,                     // The short stuff
   LPARAM   lParam                      // The big stuff
   )
{
#ifdef SYM_WIN16
    auto    VWRFILESPEC     FileSpec;
    auto    RECT            rect;
#endif

    auto    PRINTDLGSTRUCT  printDlgStruct;
    auto    CHECKSTRUCT     PwCheck;
    auto    LPRIVATE        lpPriv;
    auto    UINT            uStatus;
    auto    UINT            uEntries;
    auto    TCHAR           lpWork      [ SYM_MAX_PATH+1 ];
    auto    TCHAR           szOutputFile[ SYM_MAX_PATH+1 ];
    auto    TCHAR           szPrintTitle[ SYM_MAX_PATH+1 ];
    auto    HWND            hWnd;

    switch (message)
        {
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx(message, wParam, lParam));
        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_CenterDialog(hDlg, GetParent(hDlg));

            SetWindowLong(hDlg, DWL_USER, lParam); // Save private data

            lpPriv = (LPRIVATE)lParam;

#if 0                                   // Disable this code. It can't be used
                                        // in any translated versions:

                                        // ------------------------------------
                                        // If the title is something, overwrite
                                        // the default title
                                        // ------------------------------------

            if (lpPriv->lpAct->szTitle && *lpPriv->lpAct->szTitle != EOS)
                {
                STRCPY(lpWork, _T("&"));            // Underscore first character
                STRCAT(lpWork, lpPriv->lpAct->szTitle);
                STRCAT(lpWork, _T(":"));
                SetDlgItemText(hDlg, IDI_AL_ETITLE, lpWork);

                SetWindowText(hDlg, lpPriv->lpAct->szTitle);
                }
#endif
                                        // ------------------------------------
                                        // If we are not allowed to clear the
                                        // log file, hide button
                                        // ------------------------------------

            if (lpPriv->lpAct->bAllowClear == FALSE)
                MoveButton(hDlg, IDI_AL_CLEAR, IDHELP);

                                        // ------------------------------------
                                        // Read the specified log file, and
                                        // display to user
                                        // ------------------------------------

            lpPriv->TempFile[0] = EOS;
            lpPriv->bEndBuild = FALSE;

            uEntries = BuildProgress ( hDlg, lpPriv );

            // if there are no entries don't display the window.
            if ( 0 == uEntries )
            {
              EndDialog(hDlg, 0);
              break ;
            }

            SetEntriesField(hDlg, uEntries);

#ifdef SYM_WIN16
            hWnd = GetDlgItem (hDlg, IDI_AL_VIEW);
            GetWindowRect  (hWnd, &rect);
            ScreenToClient (hDlg,(LPPOINT)&rect);
            ScreenToClient (hDlg,(LPPOINT)&(rect.right));
            DestroyWindow  (hWnd);

            VwrCreate (hDlg, (LPHANDLE)&hWnd, IDI_AL_VIEW,
                        rect.left, rect.top,
                        rect.right-rect.left,
                        rect.bottom-rect.top);

            PostMessage (hDlg, WM_UPDATEVIEWER, 0, 0L);
#else
            DisplayTempFile (
                        GetDlgItem ( hDlg, IDI_AL_VIEW ),
                        lpPriv->TempFile
                        );
#endif
            return (TRUE);

/*
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam, lParam) )
                {
                lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

                StdHelpSetContext(lpPriv->lpAct->uHelpID, lpPriv->lpAct->uHelpID);
                }
            break;
*/

#ifdef SYM_WIN32
                                                // --------------------------------
                                                // bring up "WhatsThis" popup menu
                                                // or invoke context sensitive help
                                                // --------------------------------
        case WM_HELP:
        case WM_CONTEXTMENU:
            OnContextHelp(hDlg, message, wParam, lParam);
            break;
#endif

        case WM_DESTROY:
            lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

            if ( lpPriv && lpPriv->TempFile[0] ) // If file is not to be preserved
                FileDelete(lpPriv->TempFile);   // Delete temp file

            break;


        case WM_COMMAND:
            lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

            switch ( GET_WM_COMMAND_ID(wParam, lParam) )
                {
                case IDI_AL_VIEW:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_SETFOCUS)
                        SendDlgItemMessage(hDlg, IDI_AL_VIEW, EM_SETSEL, (WPARAM)(int)-1, (LPARAM)(int)-1);
                    break;

                case IDCANCEL:
                case IDI_AL_CLOSE:
                    EndDialog(hDlg, 0);
                    break;

                case IDI_AL_PRINT:
                    {
                    TCHAR       szTemp [ SYM_MAX_PATH ];
                    SYSTEMTIME  sysTime;

                    GetStartDir(hDLLInst, szOutputFile, sizeof(szOutputFile));

                    NameReturnLongName ( szOutputFile, lpWork, TRUE );
                    STRCPY ( szOutputFile, lpWork );

                    LoadString ( hDLLInst, IDS_DEF_ACTILOGFILE, szTemp, sizeof(szTemp) );

                    NameAppendFile (szOutputFile, szTemp );
#ifdef SYM_WIN16
                    STRLWR (szOutputFile);
#endif
                                        // Report title is "<Title> - <Date>"
                    GetLocalTime( &sysTime );
                    if ( 0 == GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE,
                                             &sysTime, NULL, szTemp, sizeof( szTemp ) ) )
                        {
                        *szTemp = EOS;
                        }

                    if ( STRLEN( lpPriv->lpAct->szTitle ) + 3 + STRLEN( szTemp ) < sizeof( printDlgStruct.szTitle ) )
                        {
                        SPRINTF( szPrintTitle, _T("%s - %s"),
                                 (LPCTSTR)lpPriv->lpAct->szTitle,
                                 (LPCTSTR)szTemp );
                        }
                    else if ( STRLEN( lpPriv->lpAct->szTitle ) < sizeof( printDlgStruct.szTitle ) )
                        {
                        STRCPY( szPrintTitle, lpPriv->lpAct->szTitle );
                        }
                    else
                        {
                        SYM_ASSERT( FALSE );
                        *szPrintTitle = EOS;
                        }

                                        // Fill in Print Dlg Struct
                    MEMSET(&printDlgStruct, 0, sizeof(printDlgStruct));

                    printDlgStruct.dwSize    = sizeof(printDlgStruct);
                    printDlgStruct.hParent   = hDlg;
                    printDlgStruct.lpfnBuild = BuildVirusInfoForPrinting;
                    printDlgStruct.bWordWrap = TRUE;
                    printDlgStruct.wHelpID   = HELP_DLG_PRINT;
                    printDlgStruct.wHelpIDOverwrite = HELP_DLG_OVERWRITE;
                    printDlgStruct.wHelpIDBrowse    = HELP_DLG_PRINT_BROWSE;

                    STRCPY ( printDlgStruct.szTitle, szPrintTitle );
                    STRCPY ( printDlgStruct.szFile, szOutputFile );

                    NavPrnDlgSetContextHelpIds( & printDlgStruct );

                    NPTPrintDialog (&printDlgStruct);
                    }
                    break;

                case IDI_AL_CLEAR:      // If clear, delete the activity log
                    MEMSET (&PwCheck, '\0', sizeof(PwCheck));
                    PwCheck.hWnd         = hDlg;
                    PwCheck.lpNavOptions = lpPriv->lpAct->lpNav;
                    PwCheck.uID          = PASSWORD_CLEARLOG;

                    uStatus = AskPassword (&PwCheck);

                    if (uStatus == PASSWORD_ASK)
                        {
                        uStatus = PasswordCheck(&PwCheck, FALSE, FALSE) ?
                                        TRUE : FALSE;
                        }
                    else if (uStatus == PASSWORD_SHOWCONTROL)
                        uStatus = TRUE;
                    else
                        uStatus = FALSE;

                    if (uStatus == TRUE && ClearLog(hDlg) == TRUE)
                        {
                                        // Just delete the file.
                        FileDelete(lpPriv->lpAct->szLogFile);

                                        // Setup up buttons now
                        hWnd = GetDlgItem(hDlg, IDI_AL_CLOSE);
                        SetFocus(hWnd);
                        Button_SetStyle(hWnd, BS_DEFPUSHBUTTON, TRUE);

                        hWnd = GetDlgItem(hDlg, IDI_AL_CLEAR);
                        Button_SetStyle(hWnd, BS_PUSHBUTTON, FALSE);
                        Button_Enable (hWnd, FALSE);

                                        // Disable rest of buttons in the
                                        // dialog.
                        Button_Enable (GetDlgItem(hDlg, IDI_AL_PRINT), FALSE);
                        Button_Enable (GetDlgItem(hDlg, IDI_AL_FILTER), FALSE);
#ifdef SYM_WIN16
                                        // Clear file display
                        SendDlgItemMessage (hDlg, IDI_AL_VIEW, VWR_FILE_SPEC,
                                            0, NULL);
#else
                        Edit_SetText ( GetDlgItem ( hDlg, IDI_AL_VIEW ), "" );
#endif
                        SetEntriesField(hDlg, 0);   // Reset entry count
                        }
                    break;

                case IDI_AL_FILTER:
                    if (FilterActivityLog (hDlg, lpPriv->lpAct->aFilterDesc,
                                           lpPriv) == TRUE)
                        {
                        lpPriv->bEndBuild = FALSE;

                        uEntries = BuildProgress (hDlg, lpPriv);

                        SetEntriesField ( hDlg, uEntries );
#ifdef SYM_WIN32
                        DisplayTempFile (
                                    GetDlgItem ( hDlg, IDI_AL_VIEW ),
                                    lpPriv->TempFile
                                    );
#else
                        PostMessage (hDlg, WM_UPDATEVIEWER, 0, 0L);
#endif
                        }
                    break;
                                        // -------------------------------------
                                        // Invoke the standard topic help
                                        // -------------------------------------
                case IDHELP:
                    StdHelpFromDialog (
                                hDlg,
                                IDH_NAVW_ACTIVITY_LOG_DLG_HELP_BTN
                                );
                    break;
                }
            break;

#if SYM_WIN16
        case WM_UPDATEVIEWER:
            lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

            UpdateWindow(hDlg);

            if ( FileExists(lpPriv->lpAct->szLogFile) )
                {
                lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

                MEMSET (&FileSpec, EOS, sizeof(FileSpec));

                FileSpec.wSize              = sizeof(VWRFILESPEC);
                FileSpec.gaFileSpec         = GlobalAddAtom(lpPriv->TempFile);
                FileSpec.gaFileExtOverride  = GlobalAddAtom(".TXT");
                FileSpec.gaParserOverride   = GlobalAddAtom("S32DOC7.PRS");
                FileSpec.dwDispStyles       = VWR_STY_DISABLE_SELECTION |
                                              VWR_STY_DISABLE_POPUP_MENU;
                FileSpec.lpDomain           = NULL;// No domain support

                SendDlgItemMessage (hDlg, IDI_AL_VIEW, VWR_FILE_SPEC,0,
                                        (LPARAM)(LPVWRFILESPEC)&FileSpec);

                NG_SetFontsInDialog (hDlg, 0);
                }

            break;
#endif
        }

    return (FALSE);
}


//*********************************************************************
//
//*********************************************************************
VOID LOCAL PASCAL SetEntriesField(HWND hDlg, UINT uEntries)
{
    auto    TCHAR    szWork[SIZE_WORK+1];
    auto    TCHAR    szWork2[SIZE_WORK+1];

    LoadString ( hDLLInst, (uEntries == 1) ? IDS_ENTRY : IDS_ENTRIES,
                 szWork2, sizeof(szWork2) );

    SPRINTF (szWork, szWork2, uEntries);

    SetDlgItemText(hDlg, IDI_AL_ENTRIES, szWork);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID LOCAL MoveButton (HWND hDlg, UINT uDoomedButton, UINT uButtonToMove)
{
    auto    RECT  Rect;
    auto    POINT ptUpperLeft;
    auto    POINT ptLowerRight;

    GetWindowRect (GetDlgItem(hDlg, uDoomedButton), &Rect);

    ptUpperLeft.x  = Rect.left;
    ptUpperLeft.y  = Rect.top;
    ptLowerRight.x = Rect.right;
    ptLowerRight.y = Rect.bottom;

    ScreenToClient (hDlg, &ptUpperLeft );
    ScreenToClient (hDlg, &ptLowerRight);

    MoveWindow (GetDlgItem(hDlg, uButtonToMove), ptUpperLeft.x, ptUpperLeft.y,
                ptLowerRight.x - ptUpperLeft.x,
                ptLowerRight.y - ptUpperLeft.y, TRUE);

    Button_Enable (GetDlgItem(hDlg, uDoomedButton), FALSE);
    ShowWindow    (GetDlgItem(hDlg, uDoomedButton), SW_HIDE);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

HGLOBAL SYM_EXPORT WINAPI BuildVirusInfoForPrinting(
    LPPRINTDLGSTRUCT lpPrintDlgStruct )
{
    auto    HGLOBAL     hBuffer = NULL;
    auto    DWORD       dwFileSize;
    auto    LPRIVATE    lpPriv;
    auto    HFILE       hFile;
    auto    HPSTR       hpStr;

    lpPriv = (LPRIVATE)GetWindowLong(lpPrintDlgStruct->hParent, DWL_USER);

    if ((hFile = FileOpen(lpPriv->TempFile,READ_ONLY_FILE)) != HFILE_ERROR)
        {
        dwFileSize = FileLength (hFile);
                                        // --------------------------------
                                        // The +10 is for safety incase the
                                        // value is on a paragraph boundary
                                        // --------------------------------
        if ((hBuffer = MemAlloc (GHND, dwFileSize+10)) != NULL)
            {
            hpStr = (HPSTR)MemLock (hBuffer);
            FileHugeRead (hFile, hpStr, dwFileSize);
            MemUnlock (hBuffer);
            }

        FileClose ( hFile );
        }

    return (hBuffer);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL LOCAL WINAPI BuildProgress (
    HWND        hDlg,
    LPRIVATE    lpPriv
    )
{
    auto    DLGPROC lpfn;
    auto    BOOL    bStatus;

    lpfn = (DLGPROC) MakeProcInstance((FARPROC)ProgressDlgProc, hDLLInst);

    bStatus = DialogBoxParam(hDLLInst,
                            MAKEINTRESOURCE(IDD_PROGRESS),
                            hDlg,
                            lpfn,
                            (LPARAM)lpPriv);

    FreeProcInstance((FARPROC) lpfn);

    return ((bStatus == -1) ? 0 : bStatus);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI ProgressDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    auto    FILLBARSTYLE fbStyle;
    auto    LPRIVATE    lpPriv;

    switch(message)
        {
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx(message, wParam, lParam));
        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_CenterDialog (hDlg, GetParent(hDlg));

#ifdef NAVSCANW
            if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
                NG_SetGroupBoxSunken(hDlg, ID_PROGRESS_BOX);
            else
                ShowWindow(GetDlgItem(hDlg, ID_PROGRESS_BOX), SW_HIDE);
#endif
                                        // --------------------------------
                                        // Initialize fillbar with all the
                                        // currently required values
                                        // --------------------------------

            fbStyle.nStruclen         = sizeof(FILLBARSTYLE);
            fbStyle.b3D               = TRUE;
            fbStyle.nLeftTopWidth     = FB_USEDEFAULT;
            fbStyle.nRightBottomWidth = FB_USEDEFAULT;
            fbStyle.crColor           = RGB(255, 0, 0);

            Fillbar_SetRange(GetDlgItem(hDlg, ID_PROGRESS_BAR), 0, 100);
            Fillbar_Show    (GetDlgItem(hDlg, ID_PROGRESS_BAR), &fbStyle);

            SetWindowLong(hDlg, DWL_USER, lParam); // Save private data
            lpPriv = (LPRIVATE)lParam;

            lpPriv->bReceivedPaint = FALSE;

            UpdateWindow(hDlg);


            return (TRUE);
            break;


        case WM_DESTROY:
            Fillbar_Close ( GetDlgItem ( hDlg, ID_PROGRESS_BAR ));
            break;

        case WM_COMMAND:
            lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDCANCEL:
                    lpPriv->bEndBuild = TRUE;
                    break;
                }
            break;

        case WM_QUERYNEWPALETTE:
        case WM_PALETTEISCHANGING:
        case WM_PALETTECHANGED:
            break;

        case WM_DRAWITEM:
            break;

        case WM_PAINT:
            lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

            if (lpPriv->bReceivedPaint == FALSE)
                {
                lpPriv->bReceivedPaint = TRUE;
                PostMessage(hDlg, WM_UPDATEVIEWER, 0, 0L);
                }

            break;

        case WM_UPDATEVIEWER:
            UpdateLogWindow (hDlg);
            break;
        }

    return (FALSE);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID LOCAL UpdateLogWindow (HWND hDlg)
{
    auto    LPRIVATE    lpPriv;
    auto    DWORD       dwKiloBytes;
    auto    DWORD       dwUserData;
    auto    int         nEntries;
    auto    UINT        uMessageID;

    lpPriv = (LPRIVATE)GetWindowLong(hDlg,DWL_USER);

    dwKiloBytes = (DWORD)lpPriv->lpAct->lpNav->activity.uKiloBytes
                  * 1024;

    dwUserData = (DWORD)GetDlgItem(hDlg, ID_PROGRESS_BAR);

    nEntries = ALogBuildFile (
                            lpPriv->hALog,
                            1, &lpPriv->lpAct->szLogFile,
                            NULL,
                            lpPriv->lpAct->lpLogFilter,
                            lpPriv->TempFile,
                            dwKiloBytes,
                            MyLogCallback,
                            dwUserData
                            );

    switch ( nEntries )
        {
        case ALOGERR_OPENLOG:
            uMessageID = IDS_ERR_OPENLOG;
            nEntries = 0;
            break;

        case ALOGERR_DISPFILE:
            uMessageID = IDS_ERR_DISPFILE;
            nEntries = 0;
            break;

        case ALOGERR_MEMORY:
            uMessageID = IDS_ERR_MEMORY;
            nEntries = 0;
            break;

        case ALOGERR_BADLOGFILE:
            uMessageID = IDS_ERR_BADLOGFILE;
            nEntries = 0;
            ConvertFileToAnsi (lpPriv->TempFile);
            break;

        default:
            if ( nEntries == 0 )
                {
                uMessageID = IDS_ERR_NOMATCH;
                }
            else
                {
                ConvertFileToAnsi (lpPriv->TempFile);
                uMessageID = 0;
                }

            break;
        }

    if ( uMessageID )
        {
        DisplayLogError(hDlg, uMessageID, lpPriv->lpAct->szLogFile);
        }

    EndDialog (hDlg, nEntries);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI MyLogCallback (
    WORD    wNumProcessed,
    WORD    wNumToProcess,
    DWORD   dwUserData
    )
{
    auto    DWORD       dwPercent;
    auto    HWND        hListBox;
    auto    MSG         msg;
    auto    LPRIVATE    lpPriv = NULL;

    hListBox = (HWND)dwUserData;
    SYM_ASSERT ( hListBox );

    lpPriv = (LPRIVATE)GetWindowLong(GetParent(hListBox), DWL_USER);
    SYM_ASSERT ( lpPriv );

    dwPercent = (((DWORD)wNumProcessed * 100) / wNumToProcess);

    if (!(dwPercent % 5))
        {
        Fillbar_SetLevel(hListBox, (UINT)(dwPercent));
        }


    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
        if (!IsDialogMessage(GetParent(hListBox), &msg))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }

    return ((lpPriv) ? lpPriv->bEndBuild : TRUE);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID LOCAL DisplayLogError (
    HWND    hDlg,
    UINT    uMessageID,
    LPTSTR   lpLogFile
    )
{
    auto    NOTIFYSTRUCT    Notify;

    MEMSET (&Notify, EOS, sizeof(Notify));
    Notify.hParent       = hDlg;
    Notify.uMsgID        = uMessageID;
    Notify.uTitleID      = IDS_ERR_TITLE;
    Notify.lpFileName    = lpLogFile;
    Notify.hInstance     = hDLLInst;

    NotifyUser (&Notify);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID LOCAL ConvertFileToAnsi (LPTSTR lpLogFile)
{
    auto    HFILE   hFile;
    auto    HPSTR   hpMem;
    auto    DWORD   dwFileSize;

    hFile = FileOpen(lpLogFile,READ_WRITE_FILE);

    if (hFile != HFILE_ERROR)
        {
        dwFileSize = FileLength (hFile);

        if ((hpMem = (HPSTR)MemAllocPtr (GHND, dwFileSize + 10)) != NULL)
            {
            if (FileHugeRead(hFile, hpMem, dwFileSize) == dwFileSize)
                {
                HugeOemToAnsiBuff (hpMem, hpMem, dwFileSize);

                FileSeek (hFile, 0L, SEEK_SET);

                FileHugeWrite(hFile, hpMem, dwFileSize);
                }

            MemFreePtr (hpMem);
            }

        FileClose (hFile);
        }
}

#ifdef SYM_WIN32

VOID LOCAL DisplayTempFile (            // Display temporary file in rtf control
    HWND    hRtfControl,                // [in] control where to display file
    LPTSTR   lpszTempFile                // [in] file to display.
    )
{
    SYM_ASSERT ( hRtfControl );
    SYM_ASSERT ( lpszTempFile );

    HFILE hFile = FileOpen ( lpszTempFile, READ_ONLY_FILE );

    if ( hFile != HFILE_ERROR )
        {
        DWORD dwFileSize = FileLength ( hFile );

                                        // --------------------------------
                                        // The +16 is for safety incase the
                                        // value is on a paragraph boundary
                                        // --------------------------------

        LPTSTR lpFile = (LPTSTR)MemAllocPtr ( GHND, dwFileSize+16 );

        if ( FileHugeRead ( hFile, lpFile, dwFileSize ) == dwFileSize )
            {
            SendMessage ( hRtfControl, EM_EXLIMITTEXT, 0, (LPARAM)dwFileSize );
            Edit_SetText ( hRtfControl, lpFile );
            }

        MemFreePtr ( lpFile );

        FileClose ( hFile );
        }
    PostMessage(hRtfControl, EM_SETSEL, (WPARAM)(int)-1, (LPARAM)(int)-1);
}

#endif


// ***************************************************************************

#ifdef SYM_WIN32

VOID LOCAL OnContextHelp                // WM_CONTEXTMENU and  WM_HELP handler
(
    HWND    hDlg,                       // [in] dialog that sent the ...
    UINT    uMsg,    // [in] "message"                    // [in] ... message
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
)
// ---------------------------------------------------------------------------
//
// This function is called when the dialog box receives a WM_CONTEXTMENU or
// WM_HELP windows message. The purpose of this function is to call the std.
// symantec help routines that display an appropiate: floating help menu or
// topic context sensitive help.
//
// Returns: Nothing.
//
// Depends: on "OnContextHelp()" defined in "CntxHelp.H"
//
// Portability: WIN32, only
//
// ---------------------------------------------------------------------------
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
        IDI_AL_VIEW,    IDH_NAVW_WHATHIS_ACTIVITY_LOG_DLG,
        IDI_AL_ETITLE,  IDH_NAVW_WHATHIS_ACTIVITY_LOG_DLG,
        IDD_ACT_LOG,    IDH_NAVW_WHATHIS_ACTIVITY_LOG_DLG,
//      IDI_AL_VIEW,    IDH_NAVW_WHATHIS_ACTIVITY_LOG_LIST_FILES_OF_TYPE_OPTION,
        IDI_AL_CLOSE,   IDH_NAVW_WHATHIS_ACTIVITY_LOG_CLOSE_BTN,
        IDI_AL_PRINT,   IDH_NAVW_WHATHIS_ACTIVITY_LOG_PRINT_BTN,
        IDI_AL_CLEAR,   IDH_NAVW_WHATHIS_ACTIVITY_LOG_CLEAR_BTN,
        IDI_AL_FILTER,  IDH_NAVW_WHATHIS_ACTIVITY_LOG_FILTER_BTN,
        0,              0
        };

    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        dwWhatsThisMap,
                        NULL,
                        NULL,           /* This dlg. does not have any LB Ctrl*/
                        NULL            /* - that's why these nulls ...       */
                       );
}


#endif // ifdef w32 - only

// ***************************************************************************
// ***************************************************************************
