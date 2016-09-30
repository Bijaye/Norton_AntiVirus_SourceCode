// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVWORK/VCS/n_info.cpv   1.4   01 Jul 1998 14:11:10   DBuches  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVWORK/VCS/n_info.cpv  $
// 
//    Rev 1.4   01 Jul 1998 14:11:10   DBuches
// Removed How-To help.
// 
//    Rev 1.3   24 Jun 1998 12:53:58   DBuches
// Added shortened quarantine status message.
// 
//    Rev 1.2   15 Jul 1997 14:36:44   DBUCHES
// Added Ctl3d support for DEC Alpha
// 
//    Rev 1.1   22 May 1997 16:20:44   BILL
// Alpha changes (removed ctl3d)
// 
//    Rev 1.0   06 Feb 1997 21:02:48   RFULLER
// Initial revision
// 
//    Rev 1.7   23 Nov 1996 14:56:34   JBRENNA
// Calling NameDisplayShortenedFileName() with a NULL window id from
// FillVirusInfo() results in a GPF on Win95 Japanese (silently fails on
// Win95 English). So, do not call NameDisplay...() with NULL.
// 
//    Rev 1.6   07 Aug 1996 15:44:42   JBRENNA
// LuigiPlus Port:
//   DALLEE: Bullet proof routines to get filestatus status and help strings.
//   Returns "" instead of trying to load string ID 0.
// 
// 
//    Rev 1.5   11 Jul 1996 08:03:52   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.4   09 Jul 1996 17:57:32   DJESCH
// Made the "Virus Info" update the listbox selection from the "Problems
// Found" dialog when using the Next/Prev buttons.  (STS 57123)
//
//    Rev 1.3   28 Jun 1996 15:23:16   GWESTER
// Ported in fixes from Luigi Plus.
//
//    Rev 1.2   09 May 1996 18:00:44   GWESTER
// Ported in Repair Wizard from Luigi Plus.
//
//    Rev 1.1   22 Feb 1996 14:08:12   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
//
//    Rev 1.0   30 Jan 1996 15:55:48   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:50:50   BARRY
// Initial revision.
//
//    Rev 1.47   14 Jan 1996 16:50:08   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
//
//    Rev 1.46   30 Nov 1995 22:42:06   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
//
//    Rev 1.45   21 Nov 1995 12:48:46   GWESTER
// Under NT, fixed problem of GPFs that occur when NAVNT is running and the
// user logs in and logs out.
//
//    Rev 1.44   01 Nov 1995 12:23:52   JWORDEN
// Fix filename for MBR on info dialog
//
//    Rev 1.43   01 Jul 1995 14:37:58   KEITH
// Fixed a buffer overwrite problem relating to ZIP file
// information display. This should fix STS 39138.
//
//    Rev 1.42   29 Jun 1995 15:21:36   MARKL
// Fixed defect #39110 by providing bitmap masks and rewriting code.
//
//    Rev 1.41   16 Jun 1995 21:40:52   GWESTER
// Modified ContextHelp links.
//
//    Rev 1.40   14 Jun 1995 17:35:32   GWESTER
// Corrected Help button links.
//
//    Rev 1.39   08 Jun 1995 11:17:10   GWESTER
// Modified How To... help links.
//
//    Rev 1.38   09 May 1995 10:30:46   KEITH
// Correct some nouns
//
//    Rev 1.37   09 May 1995 09:50:46   KEITH
// Use a consistent method for shortening filenames on dialogs.
// Set the width dynamically relative to the control width.
// Allow long names to wrap if the control allows.
//
//************************************************************************

#include "platform.h"
#include <windowsx.h>
#include "ngui.h"
#include "xapi.h"
#include "file.h"
#include "npt.h"
#include "hstring.h"
    #include "ctl3d.h"
#ifdef SYM_WIN16
    #include "syminteg.h"
#endif

#include "navgraph.hpp"
#include "stdhelp.h"
#include "ctsn.h"
#include "symcfg.h"

#include "navutil.h"
#include "options.h"
#include "navw_hlp.h"
#include "cmbstr.h"
#include "nunavdlg.h"
#include "nunavstb.h"

#include "stub.h"
#include "virlist.h"
#include "virscan.h"
#include "nonav.h"

extern   HINSTANCE   hInstance;

WORD    gwForPrintHack = 0;


// ************************************************************************ //
// ****                         Local Prototypes                       **** //
// ************************************************************************ //

#ifdef SYM_WIN32

VOID LOCAL OnContextMenu (              // Respond to WM_CONTEXTMENU message
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam,                     // [in] lParam from dialog
    BOOL    bVirusFoundContext          // [in] flag TRUE is virus found dlg
    );

#endif

VOID LOCAL FillVirusInfo (
    HWND    hDlg,
    LPVOID  lpVoid,
    LPSTR   lpFullPath,
    WORD    wStatus
    );

UINT LOCAL GetIndexIntoArray (
    HWND    hListBox,
    LPUINT  lpArray,
    UINT    uArrayEntries
    );

LPUINT LOCAL BuildVirusInfoArray (
    HWND    hListbox,
    LPUINT  lpArray
    );

BOOL SYM_EXPORT WINAPI ScanInfoDlgProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

BOOL SYM_EXPORT WINAPI FoundVirusProc (
    HWND,
    UINT,
    WPARAM,
    LPARAM
    );

VOID LOCAL EnableDisablePrevNext (
    HWND        hDlg,
    WORD        wIndex,
    WORD        wNumItems
    );

HGLOBAL SYM_EXPORT CALLBACK BuildVirusInfoForPrinting (
    LPPRINTDLGSTRUCT lpPrintDlgStruct
    );

VOID LOCAL AddVirInfo (                 // Initialize checkboxes'n stuff
    HWND        hDlg,                   // [in] dialog box handle
    LPN30NOTES  lpN30Notes,             // [in] source of information
    LPSTR       lpFile                  // [in] path to the INF file
    );

VOID LOCAL BuildInfectsString (
    HPN30NOTES  hpDefInMemory,
    LPSTR       szBuffer,
    UINT        uListType
    );


BOOL LOCAL OnDrawItem (                 // A bitmap should be drawn in
    HWND        hDlg,                   // [in] our main dialog box.
    LPDRAWITEMSTRUCT lpDrawItem         // [in] pointer to DRAWITEMSTRUCT
    );

VOID LOCAL OnDestroy (                  // Clean house
    HWND        hDlg
    );

// ************************************************************************ //
// ****                         Local Typedefs                         **** //
// ************************************************************************ //

typedef struct tagINFOPARAMS
{
    UINT                uNumItems;      // Number of infected items
    UINT                uIndex;         // index into this number of items.
    HWND                hListBox;       // listbox containing items
    LPNONAV             lpNav;          // our good ol' pointer.
    LPUINT              lpArray;        // array of indexes.
    N30NOTES            N30Notes;       // Item being displayed
    HBITMAP             hBmpCheck;      // checkmark bitmap handle
    HBITMAP             hBmpCheckMask;  // mask for checkmark bitmap
    HBITMAP             hBmpNoWay;      // checkmark bitmap handle
    HBITMAP             hBmpNoWayMask;  // mask for checkmark bitmap handle
} INFOPARAMS, FAR * LPINFOPARAMS;


#ifdef NAVSCANW
// This is a globally saved pointer to the Info dialog's INFOPARAMS object.
LPINFOPARAMS    GloballySaved_InfoParams   =    NULL;
#endif

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****     04/25/96 GWESTER Added parameter bShowFileInfo. If you're  **** //
// ****         launching this from anywhere but the Repair Wizard,    **** //
// ****         this parameter is TRUE. From the Repair Wizard Info    **** //
// ****         panel, it is FALSE. (The Info Panel will use a dlg     **** //
// ****         resource that has the file info hidden, so that this   **** //
// ****         dialog can represent a virus, NOT a virus-infected     **** //
// ****         file.)                                                 **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI ShowScanInfoDlg (
    HWND    hDlg,
    HWND    hListBox,
    LPNONAV lpNav,
    BOOL    bShowFileInfo
    )
{
    extern  HINSTANCE       hInstance;
    auto    DLGPROC         lpfn;
    auto    int             nStatus = -1;
    auto    INFOPARAMS      InfoParams;

    MEMSET (&InfoParams, EOS, sizeof(InfoParams));

#ifdef NAVSCANW
                                        // ---------------------------------
                                        // For NT logouts, save a handle:
                                        // ---------------------------------
    GloballySaved_InfoParams = &InfoParams;
#endif

    InfoParams.hListBox = hListBox;
    InfoParams.lpNav    = lpNav;

    lpfn = (DLGPROC) MakeProcInstance(
                                (FARPROC)ScanInfoDlgProc,
                                hInstance
                                );
    if ( lpfn )
        {
        nStatus = DialogBoxParam (
                                hInstance,
                                MAKEINTRESOURCE ( (bShowFileInfo) ?
                                                  IDD_SCANINFODLG : IDD_VIRUSONLYINFODLG),
                                hDlg,
                                lpfn,
                                (LPARAM)(LPINFOPARAMS)&InfoParams
                                );

        FreeProcInstance((FARPROC) lpfn);
        }

    SYM_ASSERT ( nStatus != -1 );

#ifdef NAVSCANW
                                        // ---------------------------------
                                        // For NT logouts, NULL the handle:
                                        // ---------------------------------
    GloballySaved_InfoParams = NULL;
#endif
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI ScanInfoDlgProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam )
{
    auto    BOOL            bReturn = FALSE;
    auto    LPINFECTEDFILE  lpInfected;
    auto    LPINFOPARAMS    lpInfo;
    auto    char            szDefFile [ SYM_MAX_PATH+1 ];
    auto    char            szWork    [ SYM_MAX_PATH+1 ];
    auto    PRINTDLGSTRUCT  printDlgStruct;
    auto    NOTIFYSTRUCT    Notify;

    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = ((BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam));
            break;
        case WM_INITDIALOG:
            NAV_CenterDialog (hDlg);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, IDI_VI_FNAME);
            NG_SetChildSmallFont (hDlg, IDI_VI_STATUS);
            NG_SetChildSmallFont (hDlg, IDI_VI_VNAME);
            NG_SetChildSmallFont (hDlg, IDI_VI_ALIAS);
            NG_SetChildSmallFont (hDlg, IDI_VI_INFECTS);
            NG_SetChildSmallFont (hDlg, IDI_VI_LIKE);
            NG_SetChildSmallFont (hDlg, IDI_VI_LENGTH);
            NG_SetChildSmallFont (hDlg, IDI_VI_MEMRESTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_OVERWRTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_COMMENT);
            NG_SetChildSmallFont (hDlg, IDI_VI_STEALTHTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_COMPTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_SLFENCRTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_PARASTXT);

            lpInfo = (LPINFOPARAMS)lParam;
            LP_SET(hDlg, lpInfo);

                                        // -----------------------------------
                                        // Initialize our bitmaps.
                                        // -----------------------------------
            lpInfo->hBmpCheck     = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_CHECKMARK) );
            lpInfo->hBmpCheckMask = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_CHECKMARK_MASK) );
            lpInfo->hBmpNoWay     = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_NOWAYJOSE) );
            lpInfo->hBmpNoWayMask = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_NOWAYJOSE_MASK) );
            SYM_ASSERT ( lpInfo->hBmpCheck );
            SYM_ASSERT ( lpInfo->hBmpCheckMask );
            SYM_ASSERT ( lpInfo->hBmpNoWay );
            SYM_ASSERT ( lpInfo->hBmpNoWayMask );

            lpInfo->lpArray = BuildVirusInfoArray (
                                        lpInfo->hListBox,
                                        &lpInfo->uNumItems
                                        );

            lpInfo->uIndex = GetIndexIntoArray (
                                        lpInfo->hListBox,
                                        lpInfo->lpArray,
                                        lpInfo->uNumItems
                                        );

            EnableDisablePrevNext ( hDlg, lpInfo->uIndex,
                                    lpInfo->uNumItems);

            FORWARD_WM_COMMAND ( hDlg, IDI_VI_SHOWNEXT,
                                 GetDlgItem ( hDlg, IDI_VI_SHOWNEXT ),
                                 BN_CLICKED, PostMessage );

            bReturn = TRUE;
            break;
                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
                {
                StdHelpSetContext (HELP_DLG_INFO,
                                   HELP_DLG_INFO);
                }
            break;

        case WM_DESTROY:
            lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpInfo if this is NT
            if (!lpInfo)
                {LP_SET (hDlg, GloballySaved_InfoParams);
                 lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
                 SYM_ASSERT (lpInfo);}
#endif

            if (lpInfo->lpArray)
                MemFreePtr (lpInfo->lpArray);

                                        // ----------------------------------
                                        // Zap all bitmap related stuff.
                                        // ----------------------------------
            OnDestroy ( hDlg );

            LP_FREE(hDlg);
            break;

        case WM_DRAWITEM:
            OnDrawItem ( hDlg, (LPDRAWITEMSTRUCT)lParam );
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // ----------------------------------
            OnContextMenu ( hDlg, wMessage, wParam, lParam, FALSE );
            break;
#endif
        case WM_COMMAND:
            lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpInfo if this is NT
            if (!lpInfo)
                {LP_SET (hDlg, GloballySaved_InfoParams);
                 lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
                 /*SYM_ASSERT (lpInfo);*/}
#endif
            SYM_ASSERT ( lpInfo );

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDI_VI_PREV:
                    lpInfo->uIndex --;

                    EnableDisablePrevNext(hDlg, lpInfo->uIndex, lpInfo->uNumItems);
                    FORWARD_WM_COMMAND ( hDlg, IDI_VI_SHOWNEXT,
                                         GetDlgItem ( hDlg, IDI_VI_SHOWNEXT ),
                                         BN_CLICKED, PostMessage );
                    SendDlgItemMessage(GetParent(hDlg), ID_SR_LISTBOX, LB_SETCURSEL, lpInfo->uIndex, 0);
                    break;

                case IDI_VI_NEXT:
                    lpInfo->uIndex ++;

                    EnableDisablePrevNext(hDlg, lpInfo->uIndex, lpInfo->uNumItems);
                    FORWARD_WM_COMMAND ( hDlg, IDI_VI_SHOWNEXT,
                                         GetDlgItem ( hDlg, IDI_VI_SHOWNEXT ),
                                         BN_CLICKED, PostMessage );
                    SendDlgItemMessage(GetParent(hDlg), ID_SR_LISTBOX, LB_SETCURSEL, lpInfo->uIndex, 0);
                    break;

                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    break;

                case IDI_VI_PRINT:
                    {
                                        // ---------------------------------
                                        // Build a full path to the default
                                        // print file.
                                        // ---------------------------------

                    GetStartDir(hInstance, szDefFile, sizeof(szDefFile) );

                    NameReturnLongName ( szDefFile, szWork, TRUE );
                    STRCPY ( szDefFile, szWork );

                    LoadString (hInstance, IDS_DEF_PRINTFILE, szWork,
                                sizeof(szWork));

                    NameAppendFile(szDefFile, szWork);
#ifdef SYM_WIN16
                    STRLWR (szDefFile);
#endif
                    LoadString (hInstance, IDS_PRINTVIRUS, szWork, sizeof(szWork));

                                        // Fill in Print Dlg Struct
                    MEMSET(&printDlgStruct, 0, sizeof(printDlgStruct));
                    printDlgStruct.dwSize   = sizeof(printDlgStruct);
                    printDlgStruct.hParent  = hDlg;
                    printDlgStruct.lpfnBuild= BuildVirusInfoForPrinting;
                    printDlgStruct.wHelpID  = HELP_DLG_PRINT;
                    printDlgStruct.wHelpIDOverwrite = HELP_DLG_OVERWRITE;
                    printDlgStruct.wHelpIDBrowse    = HELP_DLG_PRINT_BROWSE;

                    STRCPY ( printDlgStruct.szTitle, szWork );
                    STRCPY ( printDlgStruct.szFile, szDefFile );

                                        // ---------------------------------
                                        // Set Printer dialog context Help
                                        // controls ids:
                                        // ---------------------------------
                    NavPrnDlgSetContextHelpIds( &printDlgStruct );

                                        // ---------------------------------
                                        // See how user wants the output
                                        // directed and do so.
                                        // ---------------------------------

                    gwForPrintHack = lpInfo->N30Notes.wInfo;

                    PRINTERR Status = NPTPrintDialog (&printDlgStruct);

                    if ( Status != PRINTERR_OK && Status != PRINTERR_CANCEL )
                        {
                        MEMSET (&Notify, '\0', sizeof(Notify));
                        Notify.hParent       = hDlg;
                        Notify.uMsgID        = IDS_PRINTNOK;
                        Notify.uTitleID      = IDS_NAV;
                        if ( GetCustomVersion() == VERSION_NETSCAPE )
                            {           // Title is "Internet Scanner"; set at runtime.
                        Notify.uTitleID      = IDS_NAV_INTERNET;
                            }
                        Notify.lpFileName    = lpInfo->lpNav->Infected.szFullPath;
                        Notify.lpVirusName   = lpInfo->lpNav->Infected.Notes.lpVirName;
                        Notify.lpAlertMsg    = (lpInfo->lpNav->Alert.bDispAlertMsg) ?
                                                lpInfo->lpNav->Alert.szAlertMsg : NULL;
                        Notify.uRemoveAfter  = (lpInfo->lpNav->Alert.bRemoveAfter) ?
                                                lpInfo->lpNav->Alert.uSeconds : 0;
                        Notify.hInstance     = hInstance;
                        NotifyUser (&Notify);
                        }
                    }
                    break;

                case IDHELP:
                    StdHelpFromDialog (
                                hDlg,
                                IDH_NAVW_VIRUS_FOUND_INFORMATION_DLG_HELP_BTN
                                );
                    break;

                case IDI_VI_SHOWNEXT:
                    lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (
                                        lpInfo->hListBox,
                                        lpInfo->lpArray[lpInfo->uIndex]
                                        );

                    SYM_ASSERT ( lpInfected );

                    if ( lpInfected )
                        {
                        MEMCPY ( &lpInfo->N30Notes, &lpInfected->Notes,
                                 sizeof(lpInfected->Notes) );

                        FillVirusInfo (
                                    hDlg,
                                    &lpInfected->Notes,
                                    lpInfected->szFullPath,
                                    lpInfected->wStatus
                                    );
                        }
                    break;
                }
            break;
        }

    return ( bReturn );
}

//************************************************************************
// EnableDisablePrevNext()
//
// Enables/Disables Prev/Next Contro;.
//
// Parameters:
//
// Returns:
//  nothing
//************************************************************************
// 2/10/93 BRAD Function created.
//************************************************************************

VOID LOCAL EnableDisablePrevNext (
    HWND    hDlg,
    WORD    wIndex,
    WORD    wNumItems )
{
   EnableWindow(GetDlgItem(hDlg, IDI_VI_PREV), (wIndex != 0) ? TRUE : FALSE);
   EnableWindow(GetDlgItem(hDlg, IDI_VI_NEXT), (wIndex+1 != wNumItems) ? TRUE :
                                                                       FALSE);
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  IMPORTANT NOTE:                                               **** //
// ****                                                                **** //
// ****     This functions is called from two very different places.   **** //
// ****     The reason for this is code reusability. The second place  **** //
// ****     its called from is from when a virus is found during scan  **** //
// ****     and the user would like more info about it.                **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID LOCAL FillVirusInfo (
    HWND    hDlg,
    LPVOID  lpVoid,
    LPSTR   lpFullPath,
    WORD    wStatus )
{
    static UINT uLongMessageToShortMessage[] =
        {
        IDS_RAP_NOTREPAIRED,
        IDS_RAP_NOTREPAIRED_SHORT,

        IDS_RAP_DELETED,
        IDS_RAP_DELETED_SHORT,

        IDS_RAP_NOTDELETED,
        IDS_RAP_NOTDELETED_SHORT,

        IDS_RAP_INFECTED,
        IDS_RAP_INFECTED_SHORT,

        IDS_RAP_ZIPINFECTED,
        IDS_RAP_ZIPINFECTED_SHORT,

        IDS_RAP_BOOTINFECTED,
        IDS_RAP_BOOTINFECTED_SHORT,

        IDS_RAP_BOOTNOTREPAIRED,
        IDS_RAP_BOOTNOTREPAIRED_SHORT,

        IDS_RAP_MASTERBOOTINFECTED,
        IDS_RAP_MASTERBOOTINFECTED_SHORT,

        IDS_RAP_MASTERBOOTREPAIRED,
        IDS_RAP_MASTERBOOTREPAIRED_SHORT,

        IDS_RAP_MASTERBOOTNOTREPAIRED,
        IDS_RAP_MASTERBOOTNOTREPAIRED_SHORT,

        IDS_FILEINOCCHANGED,
        IDS_FILEINOCCHANGED_SHORT,

        IDS_FILEERROR,
        IDS_FILEERROR_SHORT,

        IDS_FILENOTREPAIRED,
        IDS_FILENOTREPAIRED_SHORT,

        IDS_RAP_INOCNOTDELETED,
        IDS_RAP_INOCNOTDELETED_SHORT,

        IDS_RAP_INOCREPAIRFAILED,
        IDS_RAP_INOCREPAIRFAILED_SHORT,

        IDS_RAP_EXCLUDE_FAILED,
        IDS_RAP_EXCLUDE_FAILED_SHORT,

        IDS_RAP_INOCREPAIRED,
        IDS_RAP_INOCREPAIRED_SHORT,

        IDS_RAP_INOCDELETED,
        IDS_RAP_INOCDELETED_SHORT,

        IDS_RAP_INOCULATEDCHANGED,
        IDS_RAP_INOCULATEDCHANGED_SHORT,

        IDS_RAP_ERROR,
        IDS_RAP_ERROR_SHORT,

        IDS_RAP_ERRORDELETED,
        IDS_RAP_ERROR_SHORT,

        IDS_RAP_ERRORNOTDELETED,
        IDS_RAP_ERROR_SHORT,
        
        IDS_RAP_QUARANTINE,
        IDS_RAP_QUARANTINE_SHORT,

        0, 0
        };


    extern  FILESTATUSSTRUCT    FileStatus    [];
    auto    char                szZipFilePath [ SYM_MAX_PATH+1 ];
    auto    char                szFileName    [ SYM_MAX_PATH+1 ];

    if ( wStatus == FILESTATUS_ZIPINFECTED )
        {
                                        // Handle 2 file names + a string!
        LPSTR   lpBuff = (LPSTR)MemAllocPtr ( GHND,
                                              SYM_MAX_PATH*2+SIZE_WORK+1 );
        if ( lpBuff != NULL )
            {
            ParseZipFileName (lpFullPath, szZipFilePath, szFileName);

            NameReturnFile (szZipFilePath, lpBuff);
            STRCPY (szZipFilePath, lpBuff);

            SPRINTF (lpBuff, String_Ptr(IDS_PRINTZIPFILE),
                    (LPSTR)szFileName, (LPSTR)szZipFilePath);
            Static_SetText (GetDlgItem(hDlg, IDI_VI_FNAME), lpBuff);

            MemFreePtr ( lpBuff );
            }
        }
    else
        {
        if (wStatus == FILESTATUS_MASTERBOOTINFECTED ||
            wStatus == FILESTATUS_MASTERBOOTREPAIRED ||
            wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
            {
            SPRINTF (szFileName, String_Ptr (IDS_PHYS_DRIVE), *lpFullPath);
            }
        else
            {
            STRCPY (szFileName, lpFullPath);
            }

        HWND hFileNameWnd = GetDlgItem (hDlg, IDI_VI_FNAME);
        if (hFileNameWnd)
            NameDisplayShortenedFileName (hFileNameWnd, szFileName, 2);
        }
                                        // -----------------------------------
                                        // Check for special messages that has
                                        // very long follow help. These
                                        // messages are substituted for a
                                        // shorter version.
                                        // -----------------------------------

    UINT uStatusMessage = FileStatus[wStatus].uFollowHelp;

    for ( UINT uIndex = 0; uLongMessageToShortMessage[uIndex]; uIndex += 2 )
        {
        if ( uStatusMessage == uLongMessageToShortMessage[uIndex] )
            {
            uStatusMessage = uLongMessageToShortMessage[uIndex+1];
            SYM_ASSERT ( uStatusMessage );
            break;
            }
        }

    Static_SetText ( GetDlgItem ( hDlg, IDI_VI_STATUS ),
                     uStatusMessage ? String_Ptr ( uStatusMessage ) : "" );

                                        // -----------------------------------
                                        // Add the rest of the info
                                        // -----------------------------------

    VirusScanGetInfFile ( szFileName, sizeof(szFileName) );

    AddVirInfo ( hDlg, (LPN30NOTES)lpVoid, szFileName );
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI FoundVirusInfo(HWND hDlg, LPNONAV lpNav)
{
    auto    DLGPROC         lpfn;
    auto    int             nStatus = -1;
    auto    INFOPARAMS      InfoParams;

    MEMSET (&InfoParams, EOS, sizeof(InfoParams));

    InfoParams.lpNav = lpNav;


    lpfn = (DLGPROC) MakeProcInstance (
                                    (FARPROC)FoundVirusProc,
                                    hInstance
                                    );

    if ( lpfn )
        {
        nStatus = DialogBoxParam (
                                hInstance,
                                MAKEINTRESOURCE(IDD_VIRUSINFODLG),
                                hDlg,
                                lpfn,
                                (LPARAM)(LPINFOPARAMS)&InfoParams
                                );

        FreeProcInstance((FARPROC) lpfn);
        }

    SYM_ASSERT ( nStatus != -1 );
}

// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

BOOL SYM_EXPORT WINAPI FoundVirusProc (
    HWND    hDlg,
    UINT    wMessage,
    WPARAM  wParam,
    LPARAM  lParam )
{
    auto    BOOL            bReturn = FALSE;
    auto    char            szDefFile[SYM_MAX_PATH+1];
    auto    LPINFOPARAMS    lpInfo;


    switch(wMessage)
        {
        case WM_CTLCOLOR:
            bReturn = ((BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam));
            break;
        case WM_INITDIALOG:
            {
            NAV_CenterDialog (hDlg);
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
            NG_SetChildSmallFont (hDlg, IDI_VI_VNAME);
            NG_SetChildSmallFont (hDlg, IDI_VI_FNAME);
            NG_SetChildSmallFont (hDlg, IDI_VI_INFECTS);
            NG_SetChildSmallFont (hDlg, IDI_VI_LENGTH);
            NG_SetChildSmallFont (hDlg, IDI_VI_OVERWRTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_ALIAS);
            NG_SetChildSmallFont (hDlg, IDI_VI_LIKE);
            NG_SetChildSmallFont (hDlg, IDI_VI_MEMRESTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_COMPTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_COMMENT);
            NG_SetChildSmallFont (hDlg, IDI_VI_SLFENCRTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_PARASTXT);
            NG_SetChildSmallFont (hDlg, IDI_VI_STEALTHTXT);

            lpInfo = (LPINFOPARAMS)lParam;

            SYM_ASSERT ( lpInfo );
            LP_SET ( hDlg, lpInfo );

                                        // -----------------------------------
                                        // Initialize our bitmaps.
                                        // -----------------------------------
            lpInfo->hBmpCheck     = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_CHECKMARK) );
            lpInfo->hBmpCheckMask = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_CHECKMARK_MASK) );
            lpInfo->hBmpNoWay     = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_NOWAYJOSE) );
            lpInfo->hBmpNoWayMask = LoadBitmap ( hInstance,
                                                 MAKEINTRESOURCE (IDB_NOWAYJOSE_MASK) );
            SYM_ASSERT ( lpInfo->hBmpCheck );
            SYM_ASSERT ( lpInfo->hBmpCheckMask );
            SYM_ASSERT ( lpInfo->hBmpNoWay );
            SYM_ASSERT ( lpInfo->hBmpNoWayMask );

                                        // ----------------------------------
                                        // This is required for when we paint
                                        // the checkmark bitmaps.
                                        // ----------------------------------
            MEMCPY ( &lpInfo->N30Notes,
                     &lpInfo->lpNav->Infected.Notes,
                     sizeof(lpInfo->lpNav->Infected.Notes)
                     );

            FillVirusInfo (
                        hDlg,
                        &lpInfo->lpNav->Infected.Notes,
                        lpInfo->lpNav->Infected.szFullPath,
                        FILESTATUS_INFECTED
                        );
            }

            bReturn = TRUE;
            break;

        case WM_DRAWITEM:
            OnDrawItem ( hDlg, (LPDRAWITEMSTRUCT)lParam );
            break;

        case WM_DESTROY:
                                        // ----------------------------------
                                        // Free bitmap related stuff.
                                        // ----------------------------------
            OnDestroy ( hDlg );
            LP_FREE(hDlg);
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // ----------------------------------
            OnContextMenu ( hDlg, wMessage, wParam, lParam, TRUE );
            break;
#endif

                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE ( wParam, lParam ) )
                {
                StdHelpSetContext (HELP_DLG_VIRUS_FOUND,
                                   HELP_DLG_VIRUS_FOUND);
                }
            break;

        case WM_COMMAND:
            {
            lpInfo = (LPINFOPARAMS)LP_GET ( hDlg );
#ifdef NAVSCANW                           // Verify lpInfo if this is NT
            if (!lpInfo)
                {LP_SET (hDlg, GloballySaved_InfoParams);
                 lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
                 /*SYM_ASSERT (lpInfo);*/}
#endif
            SYM_ASSERT ( lpInfo );

            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    break;

                case IDHELP:
                    StdHelpFromDialog
                        (
                        hDlg,
                        IDH_NAVW_VIRUS_FOUND_INFORMATION_DLG_HELP_BTN
                        // HELP_DLG_VIRUS_FOUND // Old Help Id
                        );
                    break;

                case IDI_VI_PRINT:
                    {
                    auto    PRINTDLGSTRUCT          printDlgStruct;
                    auto    NOTIFYSTRUCT            Notify;
                    auto    char                    szWork [ SYM_MAX_PATH+1 ];


                                        // ---------------------------------
                                        // Build a full path to the default
                                        // print file.
                                        // ---------------------------------
                    GetStartDir(hInstance, szDefFile, sizeof(szDefFile) );

                    NameReturnLongName ( szDefFile, szWork, TRUE );
                    STRCPY ( szDefFile, szWork );

                    LoadString ( hInstance, IDS_DEF_PRINTFILE, szWork,
                                 sizeof(szWork));

                    NameAppendFile (szDefFile, szWork);
#ifdef SYM_WIN16
                    STRLWR(szDefFile);
#endif
                    LoadString (hInstance, IDS_PRINTVIRUS, szWork, sizeof(szWork));

                                        // Fill in Print Dlg Struct
                    MEMSET(&printDlgStruct, 0, sizeof(printDlgStruct));
                    printDlgStruct.dwSize   = sizeof(printDlgStruct);
                    printDlgStruct.hParent  = hDlg;
                    printDlgStruct.lpfnBuild= BuildVirusInfoForPrinting;
                    printDlgStruct.wHelpID  = HELP_DLG_PRINT;
                    printDlgStruct.wHelpIDOverwrite = HELP_DLG_OVERWRITE;
                    printDlgStruct.wHelpIDBrowse    = HELP_DLG_PRINT_BROWSE;

                    STRCPY ( printDlgStruct.szTitle, szWork );
                    STRCPY ( printDlgStruct.szFile,  szDefFile );

                                        // ---------------------------------
                                        // Set Printer dialog context Help
                                        // controls ids:
                                        // ---------------------------------
                    NavPrnDlgSetContextHelpIds( &printDlgStruct );

                                        // See how user wants the output
                                        // directed and do so.

                    gwForPrintHack = lpInfo->N30Notes.wInfo;

                    PRINTERR Status = NPTPrintDialog (&printDlgStruct);

                    if ( Status != PRINTERR_OK && Status != PRINTERR_CANCEL )
                        {
                        MEMSET (&Notify, '\0', sizeof(Notify));
                        Notify.hParent       = hDlg;
                        Notify.uMsgID        = IDS_PRINTNOK;
                        Notify.uTitleID      = IDS_NAV;
                        if ( GetCustomVersion() == VERSION_NETSCAPE )
                            {           // Title is "Internet Scanner"; set at runtime.
                        Notify.uTitleID      = IDS_NAV_INTERNET;
                            }
                        Notify.lpFileName    = lpInfo->lpNav->Infected.szFullPath;
                        Notify.lpVirusName   = lpInfo->lpNav->Infected.Notes.lpVirName;
                        Notify.lpAlertMsg    = (lpInfo->lpNav->Alert.bDispAlertMsg) ?
                                                lpInfo->lpNav->Alert.szAlertMsg : NULL;
                        Notify.uRemoveAfter  = (lpInfo->lpNav->Alert.bRemoveAfter) ?
                                                lpInfo->lpNav->Alert.uSeconds : 0;
                        Notify.hInstance     = hInstance;
                        NotifyUser (&Notify);
                        }
                    }
                    break;
                }
            }
            break;
        }

   return ( bReturn );
}

//********************************************************************

HGLOBAL SYM_EXPORT CALLBACK BuildVirusInfoForPrinting (
    LPPRINTDLGSTRUCT    lpPrintDlgStruct )
{
    auto    HGLOBAL     hBuffer;
    auto    BOOL        bValue;

    bValue = (BOOL)LOWORD(lpPrintDlgStruct->dwUserData);

    hBuffer = BuildNavVirusInfo (lpPrintDlgStruct->hParent,
                                 (BOOL)bValue);
    return(hBuffer);
}

// **************************************************************************
//                  (C) Copyright Symantec Corp. 1993
// **************************************************************************
// This function builds an array with index numbers that correspond to the
// listbox entry numbers of infected files that the user can obtain informa-
// tion for. This array is used by the << Prev and Next >> keys.
//
// INPUT:   HWND    hListbox:   Listbox window handle
//          LPSTR LPSTR lpArray:    Far pointer to a far pointer that will
//                                  contain the array.
// OUTPUT:  UINT number of indexes in the array.
//
// **************************************************************************
// 06/25/93 Martin, Function created
// **************************************************************************

LPUINT LOCAL BuildVirusInfoArray (
    HWND    hListbox,
    LPUINT  uNumInfo )
{
    auto    LPINFECTEDFILE  lpInfected;
    auto    LPUINT          lpArray = NULL;
    auto    UINT            uEntries = 0;
    auto    UINT            uIndex;

    *uNumInfo = 0;

    if (((uEntries = ListBox_GetCount (hListbox)) != LB_ERR) &&
        ((lpArray = (LPUINT)MemAllocPtr (GHND, (uEntries * sizeof(UINT))))) != NULL)
        {
        for (uIndex = 0; uIndex < uEntries; uIndex++)
            {
            lpInfected = (LPINFECTEDFILE)ListBox_GetItemData (hListbox, uIndex);

            if (OkToShowFileInfo(lpInfected->wStatus) == TRUE)
                {
                lpArray[*uNumInfo] = uIndex;
                (*uNumInfo)++;
                }
            }

        if (!*uNumInfo)
            {
            MemFreePtr (lpArray);
            lpArray = NULL;
            }
        }

    return (lpArray);
}

// **************************************************************************
//                      (C) Copyright Symantec Corp. 1993
// **************************************************************************
// This function obtains the index of the currently selected item in the
// array if listbox indexes.
//
// INPUT:   HWND    hListbox:   Listbox window handle
//          LPSTR lpArray:      Far pointer to array of indexes.
// OUTPUT:  UINT index into the index array.
//
// **************************************************************************
// 06/25/93 Martin, Function created
// **************************************************************************

UINT LOCAL GetIndexIntoArray (
    HWND    hListBox,
    LPUINT  lpArray,
    UINT    uArrayEntries )
{
    auto    UINT    uIndex          = 0;
    auto    UINT    uListboxIndex   = 0;

    uListboxIndex = ListBox_GetCurSel (hListBox);

    if (uListboxIndex != LB_ERR)
        {
        for (uIndex = 0; lpArray[uIndex] != uListboxIndex &&
                         uIndex < uArrayEntries; uIndex++)
            ;

        if (lpArray[uIndex] != uListboxIndex)
            uIndex = 0;
        }

    return (uIndex);
}

VOID LOCAL AddVirInfo (                 // Initialize checkboxes'n stuff
    HWND        hDlg,                   // [in] dialog box handle
    LPN30NOTES  lpN30Notes,             // [in] source of information
    LPSTR       lpFile                  // [in] path to the INF file
    )
{
    SYM_ASSERT ( lpFile );
    SYM_ASSERT ( lpN30Notes );

    auto    char        szWork    [SIZE_WORK+1]     = { 0 };
    auto    char        szMoreWork[SIZE_WORK+1]     = { 0 };
    auto    char        szAlias   [SIZE_ALIAS+1]    = { 0 };
    auto    char        szComment [SIZE_COMMENT+1]  = { 0 };

    // ************************ //
    // ***    Virus Name    *** //
    // ************************ //
    Static_SetText(GetDlgItem(hDlg, IDI_VI_VNAME), lpN30Notes->lpVirName);

    // ********************** //
    // What the virus infects //
    // ********************** //

    BuildInfectsString(lpN30Notes, szWork, 0);

    Static_SetText(GetDlgItem(hDlg, IDI_VI_INFECTS), szWork);

    LoadString (hInstance, (lpN30Notes->wInfo & bitINFO_INTHEWILD) ?
               IDS_INPD : IDS_NOTINPD, szWork, SIZE_WORK+1);

    Static_SetText(GetDlgItem(hDlg, IDI_VI_LIKE), szWork);

                                        // -----------------------------------
                                        // If virus is of boot type, default
                                        // to a virus length of 512 bytes.
                                        // -----------------------------------

    LoadString (hInstance, IDS_BYTES, szMoreWork, SIZE_WORK);

    SPRINTF (szWork, szMoreWork, (lpN30Notes->wType == BOOtype) ? 512 :
                                                        lpN30Notes->wSize);

                                        // if virus is a MAC virus, length
                                        // doesn't apply
    if (lpN30Notes->wType == MACtype)
       {
       LoadString (hInstance, IDS_NA_BYTES, szWork, SIZE_WORK);
       }

    Static_SetText(GetDlgItem(hDlg, IDI_VI_LENGTH), szWork);

                                        // -----------------------------------
                                        // Show / Hide Controls Based on
                                        // Virus Type
                                        // -----------------------------------
    if (lpN30Notes->wType == MACtype)
        {
        ShowWindow (GetDlgItem (hDlg, IDI_VI_MEMRES),       SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_MEMRESTXT),    SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_SIZESTLTH),    SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_STEALTHTXT),   SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_FULLSTLTH),    SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_SLFENCRTXT),   SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_TRIGGER),      SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_OVERWRTXT),    SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_ENCRYPT),      SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_COMPTXT),      SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_POLYMORHP),    SW_HIDE);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_PARASTXT),     SW_HIDE);
        }
    else
        {
        ShowWindow (GetDlgItem (hDlg, IDI_VI_MEMRES),       SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_MEMRESTXT),    SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_SIZESTLTH),    SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_STEALTHTXT),   SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_FULLSTLTH),    SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_SLFENCRTXT),   SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_TRIGGER),      SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_OVERWRTXT),    SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_ENCRYPT),      SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_COMPTXT),      SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_POLYMORHP),    SW_SHOW);
        ShowWindow (GetDlgItem (hDlg, IDI_VI_PARASTXT),     SW_SHOW);
        }

    InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_MEMRES ),    NULL, FALSE );
    InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_SIZESTLTH ), NULL, FALSE );
    InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_FULLSTLTH ), NULL, FALSE );
    InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_TRIGGER ),   NULL, FALSE );
    InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_ENCRYPT ),   NULL, FALSE );
    InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_POLYMORHP ), NULL, FALSE );

    VirusScanDatFileGetComment (
                            lpFile,
                            lpN30Notes->wVirID,
                            szAlias,
                            szComment
                            );

    Static_SetText (GetDlgItem(hDlg, IDI_VI_ALIAS), szAlias);
    Static_SetText (GetDlgItem(hDlg, IDI_VI_COMMENT), szComment);
}

//************************************************************************
// BuildInfectsString()
//
// Updates the Virus Info.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 2/10/93 BRAD Function created.
//************************************************************************

VOID LOCAL BuildInfectsString (
    HPN30NOTES  hpDefInMemory,
    LPSTR       szBuffer,
    UINT        uListType )
{
// *********************************************** //
// Different type of viruses that can be displayed //
// *********************************************** //

    enum {
         ALL_VIRUS = 0,
         COMMON_VIRUS,
         FILE_VIRUS,
         BOOT_VIRUS,
         STEALTH_VIRUS,
         POLY_VIRUS,
         MULTIPARTY_VIRUS,
         MAC_VIRUS
         };

    #define     SIZE_SMALLSTRING (SYM_MAX_PATH + 1)

    typedef struct tagARRAYPTR
    {
        LPSTR lpBuffer;
    } ARRAYPTR;

    static      UINT            uInfectsFilesString[] =
                                       {
                                       0,
                                       IDS_1FILE,
                                       IDS_2FILE,
                                       IDS_3FILE
                                       };

    static      UINT            uBootRecordString[] =
                                       {
                                       0,
                                       IDS_1BOOT,
                                       IDS_2BOOT,
                                       IDS_3BOOT
                                       };

    auto        UINT            uConditions;
    auto        UINT            uStringID;
    auto        WORD            wInfo;
    auto        char            szWork [SIZE_WORK+1];
    auto        char            pStr1[SIZE_SMALLSTRING+1];
    auto        char            pStr2[SIZE_SMALLSTRING+1];
    auto        char            pStr3[SIZE_SMALLSTRING+1];
    auto        BOOL            bBootType;

    auto        ARRAYPTR        pStr[] =
                                    {
                                    pStr1,
                                    pStr2,
                                    pStr3
                                    };
    uConditions = 0;

    switch (hpDefInMemory->wType)
        {
        case CRCtype:
        case ALGtype:
        case MACtype:
            if (uListType == BOOT_VIRUS)
                bBootType = TRUE;
            else
                bBootType = FALSE;
            break;

        case BOOtype:
            if (uListType == FILE_VIRUS)
                bBootType = FALSE;
            else
                bBootType = TRUE;
            break;

        default:
            bBootType = FALSE;
            break;
        }

    wInfo = hpDefInMemory->wInfo;
    if ( bBootType == TRUE )
        {
                                        // Boot Record viruses
        if ( wInfo & bitINFO_FLOPBOOT )
            LoadString(hInstance, IDS_FLOPPY, pStr[uConditions++].lpBuffer,
                       SIZE_SMALLSTRING);

        if ( wInfo & bitINFO_HDISKBOOT )
            LoadString(hInstance, IDS_HARDDISK, pStr[uConditions++].lpBuffer,
                       SIZE_SMALLSTRING);

        if ( wInfo & bitINFO_MBOOTREC )
            LoadString(hInstance, IDS_MASTER, pStr[uConditions++].lpBuffer,
                       SIZE_SMALLSTRING);

        uStringID = uBootRecordString[uConditions];
        }
    else
        {
                                        // Program File viruses
        if ( wInfo & bitINFO_COMFILE )
            LoadString (hInstance, IDS_COM, pStr[uConditions++].lpBuffer,
                        SIZE_SMALLSTRING);

        if ( wInfo & bitINFO_EXEFILE )
            LoadString (hInstance, IDS_EXE, pStr[uConditions++].lpBuffer,
                        SIZE_SMALLSTRING);

        if ( wInfo & bitINFO_SYSFILE )
            LoadString (hInstance, IDS_SYS, pStr[uConditions++].lpBuffer,
                        SIZE_SMALLSTRING);

        uStringID = uInfectsFilesString[uConditions];
        }

    if ( uStringID )
        {
        LoadString (hInstance, uStringID, szWork, sizeof(szWork));

        SPRINTF (szBuffer, szWork,
                (LPSTR) pStr[0].lpBuffer,
                (LPSTR) pStr[1].lpBuffer,
                (LPSTR) pStr[2].lpBuffer);
        }
    else
        *szBuffer = EOS;
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
            HWND    hDlg,
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

VOID LOCAL OnContextMenu (              // Respond to WM_CONTEXTMENU message
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam,                     // [in] lParam from dialog
    BOOL    bVirusFoundContext )        // [in] flag TRUE is virus found dlg
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
        IDI_VI_TYPEGRP,     IDH_NAVW_WHATHIS_VIRUS_FOUND_CHARACTERISTICS,
        IDI_VI_COMMGRP,     IDH_NAVW_WHATHIS_VIRUS_FOUND_DETAILS,
        IDOK,               IDH_NAVW_WHATHIS_VIRUS_FOUND_INFORMATION_CLOSE_BTN,
        IDI_VI_PRINT,       IDH_NAVW_WHATHIS_SCAN_RESULTS_PRINT_BTN,
        0,          0
        };

    static  DWORD       dwWhatsVirusThisMap[] =
        {
        IDI_VI_TYPEGRP,     IDH_NAVW_WHATHIS_VIRUS_FOUND_CHARACTERISTICS,
        IDI_VI_COMMGRP,     IDH_NAVW_WHATHIS_VIRUS_FOUND_DETAILS,
        IDOK,               IDH_NAVW_WHATHIS_VIRUS_FOUND_INFORMATION_CLOSE_BTN,
        IDI_VI_PRINT,       IDH_NAVW_WHATHIS_SCAN_RESULTS_PRINT_BTN,
        // IDD_VIRUSINFODLG,   IDH_NAVW_WHATHIS_VIRUS_FOUND_INFORMATION_DLG,
        // IDOK,               IDH_NAVW_WHATHIS_VIRUS_FOUND_INFORMATION_CLOSE_BTN,
        // IDI_VI_VNAME,       IDH_NAVW_WHATHIS_VIRUS_FOUND_VIRUS_NAME,
        // IDI_VI_TYPEGRP,     IDH_NAVW_WHATHIS_VIRUS_FOUND_CHARACTERISTICS,
        0,                  0
        };
                                        // -----------------------------------
                                        // This array is a set of string id's
                                        // in our resources that are added to
                                        // the 'How To..." menu entry. When one
                                        // of the entries is selected by the
                                        // user the corresponding help is
                                        // displayed.
                                        // -----------------------------------


    auto    LPDWORD     lpdwWhatsThisArray;

    if ( bVirusFoundContext == TRUE )
        lpdwWhatsThisArray = dwWhatsThisMap;
    else
        lpdwWhatsThisArray = dwWhatsVirusThisMap;

    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        lpdwWhatsThisArray,
                        NULL,
                        NULL,           /* This dlg. does not have any LB Ctrl*/
                        NULL            /* that's why these nulls             */
                       );


}


#endif

/*@API:*************************************************************************
@Declaration:
VOID LOCAL OnDrawItem ( HWND hDlg, LPDRAWITEMSTRUCT lpDrawItem )

@Description:
This function processes the Norton AntiVirus Express general options
WM_DRAWITEM message. This is used to draw the bitmap inside the dialog box.

@Parameters:
$hDlg$ Norton AntiVirus Express general options dialog box window handle.

$lpDrawItem$ pointer to the DRAWITEMSTRUCT structure.

@Returns:
TRUE if we've processed the message, FALSE to allow windows to process the
message.

@Compatibility: Win16 and Win32
*******************************************************************************/

BOOL LOCAL OnDrawItem (                 // A bitmap should be drawn in
    HWND    hDlg,                       // [in] our main dialog box.
    LPDRAWITEMSTRUCT lpDrawItem         // [in] pointer to DRAWITEMSTRUCT
    )
{
    auto    BOOL            bReturn = FALSE;
    /*const*/ LPINFOPARAMS    lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
    auto    HBITMAP         hBmp;
    auto    HBITMAP         hBmpMask;
    auto    BOOL            bUseCheckMark = FALSE;


#ifdef NAVSCANW                           // Verify lpInfo if this is NT
    if (!lpInfo)
        {LP_SET (hDlg, GloballySaved_InfoParams);
         lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
         /*SYM_ASSERT (lpInfo);*/}
#endif
    SYM_ASSERT ( lpInfo );
    SYM_ASSERT ( lpDrawItem );

    if ( lpDrawItem->itemAction == ODA_DRAWENTIRE ||
        (lpDrawItem->itemAction &  ODA_SELECT) )
        {
        switch ( lpDrawItem->CtlID )
            {
            case IDI_VI_MEMRES:
                if ( lpInfo->N30Notes.wInfo & bitINFO_RESIDENT )
                    bReturn = TRUE;
                break;

            case IDI_VI_SIZESTLTH:
                if ( lpInfo->N30Notes.wInfo & bitINFO_SIZESTLTH )
                    bReturn = TRUE;
                break;

            case IDI_VI_FULLSTLTH:
                if ( lpInfo->N30Notes.wInfo & bitINFO_FULLSTLTH )
                    bReturn = TRUE;
                break;

            case IDI_VI_TRIGGER:
                if ( lpInfo->N30Notes.wInfo & bitINFO_TRIGGERED )
                    bReturn = TRUE;
                break;

            case IDI_VI_ENCRYPT:
                if ( lpInfo->N30Notes.wInfo & bitINFO_ENCRYPT )
                    bReturn = TRUE;
                break;

            case IDI_VI_POLYMORHP:
                if ( lpInfo->N30Notes.wInfo & bitINFO_POLYMORPH )
                    bReturn = TRUE;
                break;

#ifdef SYM_DEBUG
            default:
                SYM_ASSERT ( 0 );
                break;
#endif
            }

         hBmp     = (bReturn) ?  (lpInfo->hBmpCheck) :  (lpInfo->hBmpNoWay);
         hBmpMask = (bReturn) ?  (lpInfo->hBmpCheckMask) :  (lpInfo->hBmpNoWayMask);

                                        // Erase what was already there
                                        // since we are using transparent
                                        // masks.
            ExtTextOut ( lpDrawItem->hDC, 0, 0, ETO_OPAQUE,
                        &lpDrawItem->rcItem, "", 0, NULL );

                                        // Draw the correct bitmap with its
                                        // mask.
        if ( hBmp )
            {
            NGDrawMaskedBitmap ( lpDrawItem->hDC,
                                 hBmp,
                                 hBmpMask,
                                 lpDrawItem->rcItem.left,
                                 lpDrawItem->rcItem.top );
            }
        }

    return ( bReturn );
}

/*@API:*************************************************************************
@Declaration:
VOID LOCAL OnDestroy ( HWND hDlg )

@Description:
This function processes the Norton AntiVirus
WM_DESTROY message.

@Parameters:
$hDlg$ Norton AntiVirus Express general options dialog box window handle.

@Returns:
Nothing.

@Compatibility: Win16 and Win32
*******************************************************************************/

VOID LOCAL OnDestroy (                  // Clean house
    HWND    hDlg
    )
{
    /*const*/LPINFOPARAMS    lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
#ifdef NAVSCANW                           // Verify lpInfo if this is NT
    if (!lpInfo)
        {LP_SET (hDlg, GloballySaved_InfoParams);
         lpInfo = (LPINFOPARAMS)LP_GET(hDlg);
         /*SYM_ASSERT (lpInfo);*/}
#endif

    SYM_ASSERT ( lpInfo );

    if ( lpInfo->hBmpCheck )
        DeleteObject ( lpInfo->hBmpCheck );

    if ( lpInfo->hBmpCheckMask )
        DeleteObject ( lpInfo->hBmpCheckMask );

    if ( lpInfo->hBmpNoWay )
        DeleteObject ( lpInfo->hBmpNoWay );

    if ( lpInfo->hBmpNoWayMask )
        DeleteObject ( lpInfo->hBmpNoWayMask );

    lpInfo->hBmpCheck     = NULL;
    lpInfo->hBmpCheckMask = NULL;
    lpInfo->hBmpNoWay     = NULL;
    lpInfo->hBmpNoWayMask = NULL;
}


