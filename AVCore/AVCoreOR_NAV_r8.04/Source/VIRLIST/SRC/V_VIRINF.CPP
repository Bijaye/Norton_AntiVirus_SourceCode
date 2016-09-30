// Copyright 1996 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/VIRLIST/VCS/v_virinf.cpv   1.28   28 Aug 1998 17:36:14   JBRENNA  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/VIRLIST/VCS/v_virinf.cpv  $
// 
//    Rev 1.28   28 Aug 1998 17:36:14   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.27.2.3   09 Jul 1998 17:23:12   DBuches
//    Removed HowTo menus.
// 2. Rev 1.27.2.2   02 Jul 1998 13:25:50   mdunn
//    Commented out reference to IDH_NAVW_WHATHIS_VIRUS_FOUND_INFORMATION_DLG
//    (that ID is commented out in navwhlp5.h)
// 3. Rev 1.27.2.1   26 Jun 1998 18:19:32   mdunn
//    Now #includeing navwhlp5.h
// 
//    Rev 1.27   20 May 1997 18:58:16   MKEATIN
// We're now linking to an avenged api.
// 
//    Rev 1.26   02 Aug 1996 12:47:02   PVENKAT
// Fixed #58149.  HelpIDs for Virus Info Dlg.
// 
//    Rev 1.25   23 Jul 1996 07:37:54   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.24   21 Jun 1996 14:58:14   PVENKAT
// Remove #ifdef SYM_WIN16 to enable Help processing.
//
//    Rev 1.23   11 Jun 1996 14:54:12   DJESCH
// Made the "Virus Info" update the listbox selection from the "Virus
// List" dialog.  (STS 57123)
//
//    Rev 1.22   01 Mar 1996 11:58:32   JWORDEN
// DBCS enable
// Bring in the bitmaped checkbox code from QAK8
//
//    Rev 1.20   24 Jan 1996 17:36:00   GDZIECI
// Include STDHELP.H for WIN16 platform.
//***************************************************************************

#include "platform.h"
#include "ngui.h"
#include "xapi.h"
#include "npt.h"
#include <ctl3d.h>

#include "stdhelp.h"

#include "virusdbf.h"

#include "vlist.h"
#include "navwhlp5.h"

extern   HINSTANCE   hInstance;
TCHAR            g_sNAVDir1[SYM_MAX_PATH+1];
BOOL               g_bOSIsNT1;

int  gwForPrintHack;

// --------------------------------------------------------------------------
//                              LOCAL STRUCTURES
// --------------------------------------------------------------------------

typedef struct tagINFO
{
    UINT                uIndex;         // Index in moma listbox
    UINT                uNumItems;      // Number of items in moma listbox
    HVLVIRUSENTRY         hEntry;         // entry currently displayed
    LPVIRINFO           lpVirInfo;      // init structure from moma window
    HBITMAP             hBmpCheck;      // checkmark bitmap handle
    HBITMAP             hBmpCheckMask;  // mask for checkmark bitmap
    HBITMAP             hBmpNoWay;      // checkmark bitmap handle
    HBITMAP             hBmpNoWayMask;  // mask for checkmark bitmap handle

} INFO, FAR * LPINFO;

// ************************************************************************ //
// ****                       Local Protos                             **** //
// ************************************************************************ //

VOID LOCAL PASCAL EnableDisablePrevNext(
    HWND    hDlg,
    WORD    wIndex,
    WORD    wItems );

HGLOBAL SYM_EXPORT WINAPI BuildVirusInfoForPrinting(
    LPPRINTDLGSTRUCT lpPrintDlgStruct);

VOID LOCAL SetFocusTo (                 // Forces the focus to be set
    HWND    hDlg,                       // [in] window handle
    UINT    uActive,                    // [in] active control ID
    UINT    uPasive );                  // [in] pasive control ID

VOID LOCAL PASCAL BuildInfectsString(   // Build the infected string
    HVLVIRUSENTRY hEntry,                 // [in] virus handle
    LPSTR       lpBuffer );             // [out] message built here

VOID LOCAL DoPrint (                    // Process printing.
    HWND    hDlg,                       // [in] from this dialog box
    LPINFO  lpInfo );                   // [in] virus list information

VOID LOCAL DoInitDialog (               // Process the WM_INITDIALOG message
    HWND    hDlg,                       // [in] window handle
    LPARAM  lParam );                   // [in] user defined value

VOID OnCommand (                        // Handle WM_COMMAND
    HWND    hwnd,                       // [in] from this window
    int     nID,                        // [in] control ID
    HWND    hwndCtl,                    // [in] control handle
    UINT    codeNotify );               // [in] notification code

		BOOL InitDir() ;

BOOL SYM_EXPORT WINAPI VirusInfoProc (  // Handle message to info window
    HWND    hDlg,                       // [in] info window handle
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam );                   // [in] lparam

VOID LOCAL AddVirInfo (                 // Fill controls with virus info
    HWND        hDlg,                   // [in] window that has controls.
    HVLVIRUSENTRY hEntry );               // [in] virus entry handle

BOOL LOCAL OnDrawItem (                 // A bitmap should be drawn in
    HWND    hDlg,                       // [in] our main dialog box.
    LPDRAWITEMSTRUCT lpDrawItem         // [in] pointer to DRAWITEMSTRUCT
    );

VOID LOCAL OnDestroy (                  // Clean house
    HWND    hDlg
    );

#ifdef SYM_WIN32
VOID LOCAL OnContextHelp (              // Help on right mouse button click
    HWND    hWnd,                       // [in] window that needs help
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam
    );                   // [in] mouse coordinates
#endif

// ************************************************************************ //

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

VOID WINAPI VirusInfo (                 // Display virus information
    LPVIRINFO   lpVirInfo )             // [in] initialization structure
{
    auto    INFO    Info = {0};

    SYM_ASSERT ( lpVirInfo );
    Info.lpVirInfo = lpVirInfo;

#ifndef SYM_WIN32
    auto        DLGPROC        lpfn;

    lpfn = (DLGPROC) MakeProcInstance((FARPROC)VirusInfoProc, hInstance);

    int nStatus = DialogBoxParam (
                             hInstance,
                             MAKEINTRESOURCE(IDD_VIRINFO),
                             lpVirInfo->hParent,
                             lpfn,
                             (LPARAM)&Info
                             );

    FreeProcInstance((FARPROC) lpfn);
#else
    int nStatus = DialogBoxParam (
                             hInstance,
                             MAKEINTRESOURCE(IDD_VIRINFO),
                             lpVirInfo->hParent,
                             VirusInfoProc,
                             (LPARAM)&Info
                             );
#endif
    SYM_ASSERT ( nStatus != -1 );
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

VOID LOCAL PASCAL EnableDisablePrevNext(HWND hDlg, WORD wIndex, WORD wNumItems)
{
   EnableWindow(GetDlgItem(hDlg, IDI_VI_PREV), (wIndex) ? TRUE : FALSE);
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

BOOL SYM_EXPORT WINAPI VirusInfoProc (  // Handle message to info window
    HWND    hDlg,                       // [in] info window handle
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam )                    // [in] lparam
{
    auto    BOOL    bReturn = FALSE;

    switch ( uMessage )
        {
        case WM_INITDIALOG:
            DoInitDialog ( hDlg, lParam );
            bReturn = TRUE;
            break;

#ifdef SYM_WIN32
                                        // ----------------------------------
                                        // Bring up "WhatsThis" or
                                        // standard context sensitive help
                                        // ----------------------------------

        case WM_HELP:
        case WM_CONTEXTMENU:
            OnContextHelp ( hDlg, uMessage, wParam, lParam );
            bReturn = TRUE;

#endif

        case WM_COMMAND:
            HANDLE_WM_COMMAND ( hDlg, wParam, lParam, OnCommand );
            bReturn = TRUE;
            break;

        case WM_DESTROY:
            OnDestroy ( hDlg );
            bReturn = TRUE;
            break;

        case WM_DRAWITEM:
            OnDrawItem ( hDlg, (LPDRAWITEMSTRUCT)lParam );
            bReturn = TRUE;
            break;
        }

   return ( bReturn );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL DoInitDialog (               // Process the WM_INITDIALOG message
    HWND    hDlg,                       // [in] window handle
    LPARAM  lParam )                    // [in] user defined value
{
#ifdef SYM_WIN16
    Ctl3dSubclassDlgEx ( hDlg, CTL3D_ALL );
#endif
    InitDir() ;
    NG_SetFontsInDialog (hDlg, 0);
    NG_CenterDialog ( hDlg, GetParent(hDlg) );

    NG_SetChildSmallFont (hDlg, IDI_VI_VNAME);
    NG_SetChildSmallFont (hDlg, IDI_VI_ALIAS);
    NG_SetChildSmallFont (hDlg, IDI_VI_INFECTS);
    NG_SetChildSmallFont (hDlg, IDI_VI_LIKE);
    NG_SetChildSmallFont (hDlg, IDI_VI_LENGTH);
    NG_SetChildSmallFont (hDlg, IDI_VI_MACVIRTXT);
    NG_SetChildSmallFont (hDlg, IDI_VI_MEMRESTXT);
    NG_SetChildSmallFont (hDlg, IDI_VI_STEALTHTXT);
    NG_SetChildSmallFont (hDlg, IDI_VI_SLFENCRTXT);
    NG_SetChildSmallFont (hDlg, IDI_VI_OVERWRTXT);
    NG_SetChildSmallFont (hDlg, IDI_VI_COMPTXT);
    NG_SetChildSmallFont (hDlg, IDI_VI_PARASTXT);
    NG_SetChildSmallFont (hDlg, IDI_VI_COMMENT);

    LPINFO lpInfo = (LPINFO)lParam;
    SYM_ASSERT ( lpInfo );

    SetWindowLong ( hDlg, DWL_USER, (LONG)lpInfo );

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

                                        // -----------------------------------
                                        // Initialize dialog box control based
                                        // upon the highlighted listbox item
                                        // in moma window.
                                        // -----------------------------------

    SYM_ASSERT ( lpInfo->lpVirInfo->hListBox );
    lpInfo->uIndex = VListBox_GetCurSel ( lpInfo->lpVirInfo->hListBox );
    SYM_ASSERT ( lpInfo->uIndex != LB_ERR );

    lpInfo->hEntry = (HVLVIRUSENTRY)ListBox_GetItemData(
                                                lpInfo->lpVirInfo->hListBox,
                                                lpInfo->uIndex
                                                );
    SYM_ASSERT ( lpInfo->hEntry );
    AddVirInfo ( hDlg, lpInfo->hEntry );
    lpInfo->uNumItems = VListBox_GetCount( lpInfo->lpVirInfo->hListBox );

    EnableDisablePrevNext ( hDlg, lpInfo->uIndex, lpInfo->uNumItems );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID OnCommand (                        // Handle WM_COMMAND
    HWND    hDlg,                       // [in] from this window
    int     nID,                        // [in] control ID
    HWND    hwndCtl,                    // [in] control handle
    UINT    codeNotify )                // [in] notification code
{

    LPINFO lpInfo = (LPINFO)GetWindowLong ( hDlg, DWL_USER );
    SYM_ASSERT ( lpInfo );

    switch ( nID )
        {
        case IDOK:
        case IDCANCEL:
            EndDialog ( hDlg, lpInfo->uIndex );
            break;

        case IDI_VI_PREV:
            if ( codeNotify == BN_CLICKED )
                {
                lpInfo->uIndex--;

                EnableDisablePrevNext ( hDlg, lpInfo->uIndex,
                                              lpInfo->uNumItems);

                lpInfo->hEntry = (HVLVIRUSENTRY)ListBox_GetItemData(
                                                lpInfo->lpVirInfo->hListBox,
                                                lpInfo->uIndex);
                SendMessage(lpInfo->lpVirInfo->hListBox, LB_SETCURSEL, lpInfo->uIndex, 0);
                SYM_ASSERT ( lpInfo->hEntry );
                AddVirInfo ( hDlg, lpInfo->hEntry );

                if ( !lpInfo->uIndex )
                    SetFocusTo ( hDlg, IDI_VI_NEXT, IDI_VI_PREV );
                else
                    SetFocusTo ( hDlg, IDI_VI_PREV, IDI_VI_NEXT );
                }
            break;

        case IDI_VI_NEXT:
            if ( codeNotify == BN_CLICKED )
                {
                lpInfo->uIndex++;

                EnableDisablePrevNext ( hDlg, lpInfo->uIndex,
                                              lpInfo->uNumItems);

                lpInfo->hEntry = (HVLVIRUSENTRY)ListBox_GetItemData(
                                                lpInfo->lpVirInfo->hListBox,
                                                lpInfo->uIndex);
                SendMessage(lpInfo->lpVirInfo->hListBox, LB_SETCURSEL, lpInfo->uIndex, 0);
                SYM_ASSERT ( lpInfo->hEntry );
                AddVirInfo ( hDlg, lpInfo->hEntry );

                if ( lpInfo->uIndex+1 == lpInfo->uNumItems )
                    SetFocusTo ( hDlg, IDI_VI_PREV, IDI_VI_NEXT );
                else
                    SetFocusTo ( hDlg, IDI_VI_NEXT, IDI_VI_PREV );
                }

            break;

        case IDI_VI_PRINT:
			{
				auto TCHAR szLogFile [SYM_MAX_PATH+1] ;
	           	auto BOOL bRetVal ;
				HINSTANCE   hinstNavalog = NULL;
				typedef int (CALLBACK* LPFN_TEMPACTIVITYLOG)( HWND, LPSTR ) ;
				LPFN_TEMPACTIVITYLOG  lpfnTempActivityLog;

                wsprintf ( szLogFile, _T("%s\\%s"), 
                           g_sNAVDir1, _T("activity.log") );

                hinstNavalog = LoadLibrary(  g_bOSIsNT1 ? _T("xntalog.dll")
                                                        : _T("n32alog.dll") );


				if ( NULL == hinstNavalog )
				{
					
					goto bailout;
				}

				 lpfnTempActivityLog = (LPFN_TEMPACTIVITYLOG)(
				GetProcAddress ( hinstNavalog, _T("_TempActivityLog@8")) );

				if ( NULL == lpfnTempActivityLog )
				{
				MessageBox( hDlg,"tempActivitylog","test",MB_OK) ;
				goto bailout;
				 }

				// TempActivityLog() returns 0 on success.
				bRetVal = ( 0 == lpfnTempActivityLog ( hDlg, szLogFile ) ) ;

bailout:
    
    if ( NULL != hinstNavalog )
        FreeLibrary ( hinstNavalog );

            DoPrint ( hDlg, lpInfo );
			}
            break;

        case IDHELP:
            StdHelpFromDialog ( hDlg, lpInfo->lpVirInfo->uHelpID );
            break;
            }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL DoPrint (                    // Process printing.
    HWND        hDlg,                   // [in] from this dialog box
    LPINFO      lpInfo )                // [in] virus list information
{
    auto    PRINTDLGSTRUCT  printDlgStruct = {0};
    auto    TCHAR           szTitle [ SYM_MAX_PATH ];
    auto    TCHAR           szFile  [ SYM_MAX_PATH ];

                                        // ---------------------------------
                                        // Build a full path to the default
                                        // print file.
                                        // ---------------------------------
    LoadString  (
                hInstance,
                IDS_DEF_INFOFILE,
                szTitle,
                sizeof(szTitle)
                );

    GetModuleFileName(
                     hInstance,
                     szFile,
                     sizeof(szFile)
                     );

    NameStripFile ( szFile );
    NameAppendFile( szFile, szTitle );
    STRLWR( szFile );

    LoadString (hInstance, IDS_PRINTVIRUS,
                szTitle,
                sizeof(szTitle) );

    STRCPY ( printDlgStruct.szTitle, szTitle );
    STRCPY ( printDlgStruct.szFile,  szFile  );

    printDlgStruct.dwSize           = sizeof(printDlgStruct);
    printDlgStruct.hParent          = hDlg;
    printDlgStruct.lpfnBuild        = BuildVirusInfoForPrinting;
    printDlgStruct.wHelpID          = lpInfo->lpVirInfo->uHelpPrint;
    printDlgStruct.wHelpIDOverwrite = lpInfo->lpVirInfo->uHelpPrintOverwrite;
    printDlgStruct.wHelpIDBrowse    = lpInfo->lpVirInfo->uHelpPrintBrowse;

    printDlgStruct.uHelpRightClickPrintFileButton =
            lpInfo->lpVirInfo->uHelpPrintRightClickPrintFileButton;

    printDlgStruct.uHelpRightClickPrintPrinterButton =
            lpInfo->lpVirInfo->uHelpPrintRightClickPrintPrinterButton;

    printDlgStruct.uHelpRightClickSendUserButton =
            lpInfo->lpVirInfo->uHelpPrintRightClickSendUserButton;

    printDlgStruct.uHelpRightClickPrintFileTunnel =
            lpInfo->lpVirInfo->uHelpPrintRightClickPrintFileTunnel;

    printDlgStruct.uHelpRightClickPrintPrinterTunnel =
            lpInfo->lpVirInfo->uHelpPrintRightClickPrintPrinterTunnel;

            printDlgStruct.lpdwHelpHowToPrintArray = NULL;

    LPN30NOTES lpN30Notes = (LPN30NOTES)ListBox_GetItemData(
                                            lpInfo->lpVirInfo->hListBox,
                                            lpInfo->uIndex);

    if (lpN30Notes->wType == MACtype)
        printDlgStruct.dwUserData = MAKELPARAM(TRUE, FALSE);

                                        // -----------------------------------
                                        // See how user wants the output
                                        // directed and do so.
                                        // -----------------------------------

    PRINTERR Status = NPTPrintDialog (&printDlgStruct);

    if ( Status != PRINTERR_OK && Status != PRINTERR_CANCEL )
        {
        LoadString (hInstance, IDS_NAVDBF,
                    szTitle,
                    sizeof(szTitle) );

        LoadString (hInstance, IDS_PRINTNOTOK,
                    szFile,
                    sizeof(szFile) );

        MessageBox ( hDlg, szFile,
                     szTitle, MB_OK | MB_ICONINFORMATION );
        }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL PASCAL BuildInfectsString(   // Build the infected string
    HVLVIRUSENTRY hEntry,                 // [in] virus handle
    LPSTR       lpBuffer )              // [out] message built here
{
    static      UINT            uInfectsFilesString[] =
                                       {
                                       IDS_NA_BYTES,
                                       IDS_1FILE,
                                       IDS_2FILE,
                                       IDS_3FILE
                                       };

    static      UINT            uBootRecordString[] =
                                       {
                                       IDS_NA_BYTES,
                                       IDS_1BOOT,
                                       IDS_2BOOT,
                                       IDS_3BOOT
                                       };

    auto        UINT            uConditions;
    auto        UINT            uStringID;
    auto        TCHAR           szWork  [ SYM_MAX_PATH ];
    auto        TCHAR           pStr1   [ SYM_MAX_PATH ];
    auto        TCHAR           pStr2   [ SYM_MAX_PATH ];
    auto        TCHAR           pStr3   [ SYM_MAX_PATH ];
    auto        BOOL            bBootType;

    struct tagARRAYPTR
        {
        LPSTR   lpBuffer;
        } pStr[] =
            {
            pStr1,
            pStr2,
            pStr3
            };

    uConditions = 0;

    bBootType = !VLDefInfo ( hEntry, VI_BOOL_INFECT_FILES, szWork );

    if ( bBootType == TRUE )
        {
                                        // Boot Record viruses
        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_FLOPPY_BOOT, szWork ) )
            LoadString ( hInstance, IDS_FLOPPY, pStr[uConditions++].lpBuffer,
                         SYM_MAX_PATH );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_HD_BOOT, szWork ) )
            LoadString ( hInstance, IDS_HARDDISK, pStr[uConditions++].lpBuffer,
                         SYM_MAX_PATH );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_MBR, szWork ) )
            LoadString ( hInstance, IDS_MASTER, pStr[uConditions++].lpBuffer,
                         SYM_MAX_PATH );

        uStringID = uBootRecordString[uConditions];
        }
    else
        {
                                        // Program File viruses
        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_COM, szWork ) )
            LoadString ( hInstance, IDS_COM, pStr[uConditions++].lpBuffer,
                         SYM_MAX_PATH );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_EXE, szWork ) )
            LoadString ( hInstance, IDS_EXE, pStr[uConditions++].lpBuffer,
                         SYM_MAX_PATH );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_SYS, szWork ) )
            LoadString ( hInstance, IDS_SYS, pStr[uConditions++].lpBuffer,
                         SYM_MAX_PATH );

        uStringID = uInfectsFilesString[uConditions];
        }

    if ( uStringID )
        {
        LoadString (hInstance, uStringID, szWork, sizeof(szWork));

        SPRINTF (lpBuffer, szWork,
                (LPSTR) pStr[0].lpBuffer,
                (LPSTR) pStr[1].lpBuffer,
                (LPSTR) pStr[2].lpBuffer);
        }
    else
        *lpBuffer = EOS;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL AddVirInfo (                 // Fill controls with virus info
    HWND        hDlg,                   // [in] window that has controls.
    HVLVIRUSENTRY hEntry )                // [in] virus entry handle
{
    const   UINT    uBufferSize = ( SYM_MAX_PATH * 2 );
    auto    LPSTR   lpWork;
    auto    LPSTR   lpMoreWork;

    lpWork      = ( LPSTR )MemAllocPtr ( GHND, uBufferSize );
    lpMoreWork  = ( LPSTR )MemAllocPtr ( GHND, uBufferSize );

    if ( lpWork && lpMoreWork )
        {
                                        // ----------------------------------
                                        // Virus Name
                                        // ----------------------------------

        BOOL bResult = VLDefInfo( hEntry, VI_VIRUS_NAME, lpWork );
        SYM_ASSERT ( bResult );
        SYM_ASSERT ( *lpWork );

        Static_SetText( GetDlgItem ( hDlg, IDI_VI_VNAME ), lpWork );

                                        // ----------------------------------
                                        // What the virus infects
                                        // ----------------------------------

        BuildInfectsString ( hEntry, lpWork );

        Static_SetText ( GetDlgItem(hDlg, IDI_VI_INFECTS), lpWork );

        bResult = VLDefInfo( hEntry, VI_BOOL_INFECT_WILD, lpWork );

        LoadString (hInstance, ( bResult ) ?
                   IDS_INPD : IDS_NOTINPD, lpWork, uBufferSize );

        Static_SetText ( GetDlgItem(hDlg, IDI_VI_LIKE), lpWork );

                                        // -----------------------------------
                                        // If virus is of boot type, default
                                        // to a virus length of 512 bytes.
                                        // -----------------------------------

        LoadString ( hInstance, IDS_BYTES, lpMoreWork, uBufferSize );

        if ( VLDefInfo ( hEntry, VI_BOOL_INFECT_FILES, lpWork ) == FALSE )
            SPRINTF ( lpWork, lpMoreWork, uBufferSize );
        else
            {
            bResult = VLDefInfo ( hEntry, VI_VIRUS_SIZE, lpWork );
            SYM_ASSERT ( bResult );
            SYM_ASSERT ( *lpWork );

            UINT uSize = 0;
            ConvertStringToWord ( &uSize, lpWork );

            SPRINTF ( lpWork, lpMoreWork, uSize );
            }

                                        // -----------------------------------
                                        // if virus is a MAC virus, length
                                        // doesn't apply
                                        // -----------------------------------

        if ( VLDefInfo ( hEntry, VI_BOOL_MAC_VIRUS, lpWork ) == TRUE )
           {
           LoadString ( hInstance, IDS_NA_BYTES, lpWork, uBufferSize );
           }

        Static_SetText ( GetDlgItem(hDlg, IDI_VI_LENGTH), lpWork );

                                        // -----------------------------------
                                        // Show / Hide Controls Based on Virus
                                        // type
                                        // -----------------------------------
        if ( VLDefInfo ( hEntry, VI_BOOL_MAC_VIRUS, lpWork ) == TRUE )
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
            ShowWindow (GetDlgItem (hDlg, IDI_VI_MACVIR),       SW_SHOW);
            ShowWindow (GetDlgItem (hDlg, IDI_VI_MACVIRTXT),    SW_SHOW);
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
            ShowWindow (GetDlgItem (hDlg, IDI_VI_MACVIR),       SW_HIDE);
            ShowWindow (GetDlgItem (hDlg, IDI_VI_MACVIRTXT),    SW_HIDE);
            }

        bResult = VLDefInfo ( hEntry, VI_VIRUS_ALIAS, lpWork );
        OemToAnsi (lpWork, lpWork);
        Static_SetText (GetDlgItem(hDlg, IDI_VI_ALIAS), lpWork);

        bResult = VLDefInfo ( hEntry, VI_VIRUS_INFO, lpWork );
        OemToAnsi (lpWork, lpWork);
        Static_SetText (GetDlgItem(hDlg, IDI_VI_COMMENT), lpWork);


        InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_MACVIR ),    NULL, FALSE );
        InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_MEMRES ),    NULL, FALSE );
        InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_SIZESTLTH ), NULL, FALSE );
        InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_FULLSTLTH ), NULL, FALSE );
        InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_TRIGGER ),   NULL, FALSE );
        InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_ENCRYPT ),   NULL, FALSE );
        InvalidateRect ( GetDlgItem ( hDlg, IDI_VI_POLYMORHP ), NULL, FALSE );
        }

    if ( lpWork )
        MemFreePtr ( lpWork );

    if ( lpMoreWork )
        MemFreePtr ( lpMoreWork );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL SetFocusTo (                 // Forces the focus to be set
    HWND    hDlg,                       // [in] window handle
    UINT    uActive,                    // [in] active control ID
    UINT    uPasive )                   // [in] pasive control ID
{
    SetFocus (GetDlgItem(hDlg, uActive));

    Button_SetStyle(GetDlgItem(hDlg, uActive), BS_DEFPUSHBUTTON, TRUE);
    Button_SetStyle(GetDlgItem(hDlg, uPasive), BS_PUSHBUTTON, TRUE);

    Button_SetStyle(GetDlgItem(hDlg, IDOK), BS_PUSHBUTTON, TRUE);
    Button_SetStyle(GetDlgItem(hDlg, IDHELP), BS_PUSHBUTTON, TRUE);
    Button_SetStyle(GetDlgItem(hDlg, IDI_VI_PRINT), BS_PUSHBUTTON, TRUE);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

HGLOBAL SYM_EXPORT WINAPI BuildVirusInfoForPrinting(
    LPPRINTDLGSTRUCT lpPrintDlgStruct )
{
    auto        HGLOBAL         hBuffer = NULL;

    hBuffer = BuildVirusInfo (
                            lpPrintDlgStruct->hParent,
                            (BOOL)LOWORD(lpPrintDlgStruct->dwUserData)
                            );
    return (hBuffer);
}

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
    auto    char            szWork [ SYM_MAX_PATH ];
    const   LPINFO          lpInfo = (LPINFO)GetWindowLong ( hDlg, DWL_USER );
    auto    HBITMAP         hBmp;
    auto    HBITMAP         hBmpMask;
    auto    BOOL            bUseCheckMark = FALSE;

    SYM_ASSERT ( lpInfo );
    SYM_ASSERT ( lpDrawItem );

    if ( lpDrawItem->itemAction == ODA_DRAWENTIRE ||
        (lpDrawItem->itemAction &  ODA_SELECT) )
        {
        switch ( lpDrawItem->CtlID )
            {
            case IDI_VI_MACVIR:
                if ( VLDefInfo ( lpInfo->hEntry, VI_BOOL_MAC_VIRUS, szWork ) )
                    {
                    bUseCheckMark = TRUE;
                    // This might not be the right bit , but as long as
                    // it matches the print routine
                    gwForPrintHack = gwForPrintHack | bitINFO_INTHEWILD;
                    }
                    gwForPrintHack = gwForPrintHack & ~bitINFO_INTHEWILD;
                break;

            case IDI_VI_MEMRES:
                if ( VLDefInfo ( lpInfo->hEntry, VI_BOOL_MEMORY_RES, szWork ) )
                    {
                    bUseCheckMark = TRUE;
                    gwForPrintHack = gwForPrintHack | bitINFO_RESIDENT;
                    }
                else
                    gwForPrintHack = gwForPrintHack & ~bitINFO_RESIDENT;
                break;

            case IDI_VI_SIZESTLTH:
                if ( VLDefInfo ( lpInfo->hEntry, VI_BOOL_SIZE_STEALTH, szWork ) )
                    {
                    bUseCheckMark = TRUE;
                    gwForPrintHack = gwForPrintHack | bitINFO_SIZESTLTH;
                    }
                else
                    gwForPrintHack = gwForPrintHack & ~bitINFO_SIZESTLTH;
                break;

            case IDI_VI_FULLSTLTH:
                if ( VLDefInfo ( lpInfo->hEntry, VI_BOOL_FULL_STEALTH, szWork ) )
                    {
                    bUseCheckMark = TRUE;
                    gwForPrintHack = gwForPrintHack | bitINFO_FULLSTLTH;
                    }
                else
                    gwForPrintHack = gwForPrintHack & ~bitINFO_FULLSTLTH;
                break;

            case IDI_VI_TRIGGER:
                if ( VLDefInfo ( lpInfo->hEntry, VI_BOOL_TRIGGER, szWork ) )
                    {
                    bUseCheckMark = TRUE;
                    gwForPrintHack = gwForPrintHack | bitINFO_TRIGGERED;
                    }
                else
                    gwForPrintHack = gwForPrintHack & ~bitINFO_TRIGGERED;
                break;

            case IDI_VI_ENCRYPT:
                if ( VLDefInfo ( lpInfo->hEntry, VI_BOOL_ENCRYPTING, szWork ) )
                    {
                    bUseCheckMark = TRUE;
                    gwForPrintHack = gwForPrintHack | bitINFO_ENCRYPT;
                    }
                else
                    gwForPrintHack = gwForPrintHack & ~bitINFO_ENCRYPT;
                break;

            case IDI_VI_POLYMORHP:
                if ( VLDefInfo ( lpInfo->hEntry, VI_BOOL_POLYMORPHIC, szWork ) )
                    {
                    bUseCheckMark = TRUE;
                    gwForPrintHack = gwForPrintHack | bitINFO_POLYMORPH;
                    }
                else
                    gwForPrintHack = gwForPrintHack & ~bitINFO_POLYMORPH;
                break;

            default:
                SYM_ASSERT ( 0 );
                break;
            }

        if ( bUseCheckMark )
            {
            hBmp     = lpInfo->hBmpCheck;
            hBmpMask = lpInfo->hBmpCheckMask;
            }
        else
            {
            hBmp     = lpInfo->hBmpNoWay;
            hBmpMask = lpInfo->hBmpNoWayMask;
            }

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

    return ( ( hBmp ) ? TRUE : FALSE );
}

/*@API:*************************************************************************
@Declaration:
VOID LOCAL OnDestroy ( HWND hDlg )

@Description:
This function processes the Norton AntiVirus Express general options
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
    const   LPINFO      lpInfo = (LPINFO)GetWindowLong ( hDlg, DWL_USER );

    SYM_ASSERT ( lpInfo );

    if ( lpInfo )
        {
        if ( lpInfo->hBmpCheck )
            DeleteObject ( lpInfo->hBmpCheck );

        if ( lpInfo->hBmpCheckMask )
            DeleteObject ( lpInfo->hBmpCheckMask );

        if ( lpInfo->hBmpNoWay )
            DeleteObject ( lpInfo->hBmpNoWay );

        if ( lpInfo->hBmpNoWayMask )
            DeleteObject ( lpInfo->hBmpNoWayMask );
        }
}

#ifdef SYM_WIN32

/*@API:*************************************************************************
@Declaration:
VOID LOCAL OnContextMenu ( HWND hWnd, LPARAM lParam )

@Description:
This function processes all right mouse button click help message requests.

@Parameters:
$hWnd$ handle to window that requests help
$lParam$ mouse coordinates where click occured.

@Returns:
Nothing.

@Compatibility: Win32
*******************************************************************************/

VOID LOCAL OnContextHelp                // Help on right mouse button click
(
    HWND    hWnd,                       // [in] window that needs help
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam                      // [in] mouse coordinates
)
{
#if 0
    LPINFO     lpInfo    = (LPINFO)GetWindowLong ( hWnd, DWL_USER );
    LPVIRINFO  lpVirInfo = lpInfo->lpVirInfo;
    SYM_ASSERT ( (lpInfo && lpVirInfo) );

    static struct tagWHATSTHIS
        {
        DWORD   dwControlID;
        DWORD   dwHelpID;
        } aWhatsThis[3] = { 0 };

    int nIndex = 0;

        aWhatsThis[nIndex].dwControlID = IDD_VIRINFO;
        aWhatsThis[nIndex].dwHelpID    = lpVirInfo->uHelpRightClickThisDlg;
        nIndex++;

        aWhatsThis[nIndex].dwControlID = IDI_VI_COMMGRP;
        aWhatsThis[nIndex].dwHelpID    = lpVirInfo->uHelpRightClickComments;
        nIndex++;

        aWhatsThis[nIndex].dwControlID = IDOK;
        aWhatsThis[nIndex].dwHelpID    = lpVirInfo->uHelpVirusInfoRightClickCloseBtn;
        nIndex++;

        aWhatsThis[nIndex].dwControlID = IDI_VI_PRINT;
        aWhatsThis[nIndex].dwHelpID    = lpVirInfo->uHelpVirusInfoRightClickPrintBtn;
        nIndex++;

        aWhatsThis[nIndex].dwControlID = IDI_VI_TYPEGRP;
        aWhatsThis[nIndex].dwHelpID    = lpVirInfo->uHelpVirusInfoRightClickCharacteristics;


        StdHelpExOnContextHelp( hWnd,
                                uMsg,
                                wParam,
                                lParam,
                                (LPDWORD)(aWhatsThis),
                                lpVirInfo->lpdwHelpHowToArray,
                                NULL,
                                NULL
                               );
#else
    static struct tagWHATSTHIS
        {
        DWORD   dwControlID;
        DWORD   dwHelpID;
        } aWhatsThis[] = { 
                IDI_VI_COMMGRP  , IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_DLG_COMMENTS_OPTION,
                IDI_VI_PRINT    , IDH_NAVW_WHATHIS_VIRUS_LIST_INFO_PRINT_BTN,
//                IDI_VI_TYPEGRP  , IDH_NAVW_WHATHIS_VIRUS_FOUND_INFORMATION_DLG,
                IDOK            , IDH_NAVW_WHATHIS_VIRUS_LIST_CLOSE_BTN
                };

    LPINFO     lpInfo    = (LPINFO)GetWindowLong ( hWnd, DWL_USER );
    LPVIRINFO  lpVirInfo = lpInfo->lpVirInfo;

    SYM_ASSERT ( (lpInfo && lpVirInfo) );

    StdHelpExOnContextHelp( hWnd,
                            uMsg,
                            wParam,
                            lParam,
                            (LPDWORD)(aWhatsThis),
                            NULL,
                            NULL,
                            NULL
                            );

#endif
}

BOOL InitDir()
{
BOOL  bRet = FALSE;
TCHAR szWork [SYM_MAX_PATH+1];
HKEY  hkey;
DWORD dwType, dwBuffSize = sizeof(szWork);

    g_bOSIsNT1 = ( GetVersion() & 0x80000000UL ) ? FALSE : TRUE;

    
    // Read the NAV install directory.    
    if ( ERROR_SUCCESS ==
         RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                        _T("software\\symantec\\InstalledApps"),
                        0, KEY_QUERY_VALUE, &hkey ) )
        {
        if ( ERROR_SUCCESS == 
             RegQueryValueEx ( hkey, _T("NAV"), NULL, &dwType,
                               (LPBYTE) szWork, &dwBuffSize ) )
            {
            if ( REG_SZ == dwType )
                {
				   bRet = TRUE;
                   lstrcpy( g_sNAVDir1, szWork ) ;
                
				 }

			RegCloseKey ( hkey );
			}

       
	}
    return bRet;
}


#endif // #if W32, only

