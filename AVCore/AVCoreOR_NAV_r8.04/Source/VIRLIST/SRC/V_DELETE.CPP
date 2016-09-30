// Copyright 1996 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/VIRLIST/VCS/v_delete.cpv   1.17   20 May 1997 18:58:04   MKEATIN  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/VIRLIST/VCS/v_delete.cpv  $
// 
//    Rev 1.17   20 May 1997 18:58:04   MKEATIN
// We're now linking to an avenged api.
// 
//    Rev 1.16   23 Jul 1996 07:37:50   TIVANOV
// fix the dialog font initilization
//
//    Rev 1.15   27 Jun 1996 10:02:04   RPULINT
// Fixed defect 52487 - For "All Viruses" category always use uTotalDetected
// value. This value also needs to be reduced when a virus name is deleted
// apart from uNumEntries.
//
//    Rev 1.14   10 Jun 1996 15:47:48   PVENKAT
// Commented out call to avoid buttons being in Bold face.
//
//    Rev 1.13   01 Mar 1996 12:00:52   JWORDEN
// DBCS enable
//
//    Rev 1.11   24 Jan 1996 17:35:30   GDZIECI
// Include STDHELP.H for WIN16 platform.
//***************************************************************************

#include "platform.h"
#include <ctl3d.h>

#include "stdhelp.h"

#include "virusdbf.h"

#include "vlist.h"

extern  HINSTANCE   hInstance;

// --------------------------------------------------------------------------
//                          LOCAL STRUCTURES
// --------------------------------------------------------------------------

typedef struct tagDELINFO
{
    HWND    hListbox;                   // listbox in question
    UINT    uHelpID;                    // help id

} DELINFO, FAR * LPDELINFO;

// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

int LOCAL VirusDel (                    // Ask user if entry should be deleted
    HWND    hDlg,                       // [in] parent
    HWND    hListbox,                   // [in] listbox in question
    UINT    uHelpID );                  // [in] help id

BOOL SYM_EXPORT WINAPI VirusDelProc (   // Handle messages from delete dialog
    HWND    hDlg,                       // [in] handle to window
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam );                   // [in] lparam

VOID LOCAL DoInitDialog (               // Process WM_INITDIALOG
    HWND    hDlg,                       // [in] window handle
    LPARAM  lParam );                   // [in] user defined param

VOID LOCAL OnCommand (                  // Process WM_COMMAND
    HWND    hDlg,                       // [in] window handle
    int     nID,                        // [in] control id
    HWND    hwndCtl,                    // [in] control handle
    UINT    codeNotify );               // [in] code notify

// ******************************************************************** //
// ******************************************************************** //

VOID LOCAL RemoveSelectedEntry (HWND       hActiveBox,
                                LPVIRUSDBF lpVir,
                                int        nIndex)
{
    auto    WORD    wList, wCount, wDeleteID, wItems;
    auto    LPUINT  lpIndexList;

    wDeleteID = lpVir->ListboxArray[lpVir->uActiveEntry].lpuListBoxIndex[nIndex];

                                        // Cycle through the list.
    for (wList = 0; wList < NUM_LISTBOX; wList++)
        {
        lpIndexList = lpVir->ListboxArray[wList].lpuListBoxIndex;
        wItems      = lpVir->ListboxArray[wList].uNumEntries;

                                        // Cycle through the items.
        for (wCount = 0; wCount < wItems; wCount++)
            {
                                        // If an item is found, crunch the
                                        // rest of them over the top of it.
            if (lpIndexList[wCount] == wDeleteID)
                {
                   MEMCPY(&lpIndexList[wCount],
                          &lpIndexList[wCount+1],
                          ((wItems - wCount) - 1) * sizeof (UINT) );

                                        // And down the count.
                   lpVir->ListboxArray[wList].uNumEntries --;
                   break;
                }
            }

        }

    lpVir->uTotalDetected-- ;
}


// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID WINAPI DoDelete (                  // Do delete ritual
    HWND        hDlg,                   // [in] window handle
    LPVIRUSDBF  lpVir )                 // [in] virus database pointer
{
    SYM_ASSERT ( hDlg );
    SYM_ASSERT ( lpVir );

    int  nIndex  = VListBox_GetCurSel( ACTIVELISTBOX(lpVir) );

    if ( nIndex != LB_ERR )
        {
        BOOL bResult = TRUE;

        if ( lpVir->VirList.dwPassword )
            {
            PASSWORDSTRUCT Pw;

            MEMSET ( &Pw, EOS, sizeof(Pw) );
                                        // -----------------------------------
                                        // If the delete option should be
                                        // password protected ask password
                                        // from the user.
                                        // -----------------------------------
            Pw.dwSize               = sizeof (Pw);
            Pw.hParent              = hDlg;
            Pw.dwPassword           = lpVir->VirList.dwPassword;
            Pw.uMaxCharsToAccept    = lpVir->VirList.uMaxChars;
            Pw.bCaseSensitive       = lpVir->VirList.bCaseSensitive;
            Pw.bOemConvert          = FALSE;
            Pw.uHelpID              = 0;

            bResult = PasswordVerify ( &Pw );

            }

        if ( bResult == TRUE &&
             VirusDel ( hDlg, ACTIVELISTBOX(lpVir),
                        lpVir->VirList.uHelpVirdefDelete) )
            {
            HVLVIRUSENTRY hEntry = (HVLVIRUSENTRY)ListBox_GetItemData (
                                                        ACTIVELISTBOX(lpVir),
                                                        nIndex );
            SYM_ASSERT ( hEntry );
                                        // -----------------------------------
                                        // The user deleted the virus definition
                                        // so mark it deleted and indicate that
                                        // upon exit out database must be up-
                                        // dated.
                                        // -----------------------------------
            lpVir->lpDeleted->Push ( (LPSTR)&hEntry );
            RemoveSelectedEntry ( ACTIVELISTBOX(lpVir), lpVir, nIndex );
            VListBox_SetCurSel ( ACTIVELISTBOX(lpVir), nIndex );

                                        // -----------------------------------
                                        // decrement the number entries. If the
                                        // field that we're decrementing from
                                        // is not the all viruses field, make
                                        // sure to decrement from the all
                                        // viruses field too.
                                        // -----------------------------------

// This required count manipulation is already being done in RemoveSelectedEntry()
//            ACTIVELISTBOXENTRIES(lpVir) --;
//
//            if ( lpVir->uActiveEntry )
//                lpVir->ListboxArray[0].uNumEntries --;

            int nCount = VListBox_GetCount( ACTIVELISTBOX(lpVir) ) - 1;

            if ( nCount == nIndex )
                nIndex--;

            VListBox_SetCurSel ( ACTIVELISTBOX(lpVir), nIndex );
            SendMessage(ACTIVELISTBOX(lpVir), VLB_UPDATEPAGE, 0, 0L);

                                        // -----------------------------------
                                        // Finally display the number of virus
                                        // definition in this category.
                                        // -----------------------------------

            DisplayNumDefs ( hDlg,(lpVir->uActiveEntry == VIRUSTYPE_ALL) ?
                             lpVir->uTotalDetected : ACTIVELISTBOXENTRIES(lpVir) );
            }
        }
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

int LOCAL VirusDel (                    // Ask user if entry should be deleted
    HWND    hDlg,                       // [in] parent
    HWND    hListbox,                   // [in] listbox in question
    UINT    uHelpID )                   // [in] help id
{
    auto    int     nStatus;
    auto    DELINFO DelInfo = { 0 };

    DelInfo.hListbox = hListbox;
    DelInfo.uHelpID  = uHelpID;

#ifndef SYM_WIN32
    auto    DLGPROC lpfn;

    lpfn = (DLGPROC) MakeProcInstance((FARPROC)VirusDelProc, hInstance);

    nStatus = DialogBoxParam(
                            hInstance,
                            MAKEINTRESOURCE(IDD_DELVIRUS),
                            hDlg,
                            lpfn,
                            (LPARAM)&DelInfo );

    FreeProcInstance((FARPROC) lpfn);
#else
    nStatus = DialogBoxParam (
                             hInstance,
                             MAKEINTRESOURCE(IDD_DELVIRUS),
                             hDlg,
                             VirusDelProc,
                             (LPARAM)&DelInfo
                             );
#endif

    SYM_ASSERT ( nStatus != -1 );

    return ( nStatus );
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

BOOL SYM_EXPORT WINAPI VirusDelProc (   // Handle messages from delete dialog
    HWND    hDlg,                       // [in] handle to window
    UINT    uMessage,                   // [in] message to process
    WPARAM  wParam,                     // [in] wparam
    LPARAM  lParam )                    // [in] lparam
{
    auto    BOOL    bReturn = FALSE;

    switch ( uMessage )
        {
        case WM_INITDIALOG:
            DoInitDialog ( hDlg, lParam );
            break;

        case WM_ACTIVATE:
#ifdef SYM_WIN16
            if (GET_WM_ACTIVATE_STATE(wParam, lParam))
                {
                LPDELINFO lpDelInfo;

                lpDelInfo = (LPDELINFO)GetWindowLong ( hDlg, DWL_USER );
                StdHelpSetContext (lpDelInfo->uHelpID, lpDelInfo->uHelpID);
                }
#endif
            break;

        case WM_COMMAND:
            HANDLE_WM_COMMAND ( hDlg, wParam, lParam, OnCommand );
            break;
        }

    return ( bReturn );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL DoInitDialog (               // Process WM_INITDIALOG
    HWND    hDlg,                       // [in] window handle
    LPARAM  lParam )                    // [in] user defined param
{
    auto    LPDELINFO   lpDelInfo;

#ifdef SYM_WIN16
    Ctl3dSubclassDlgEx(hDlg, CTL3D_ALL);
#endif

    NG_CenterDialog (hDlg, GetParent(hDlg));

    NG_SetFontsInDialog (hDlg, 0);

    lpDelInfo = (LPDELINFO)lParam;
    SYM_ASSERT ( lpDelInfo );
    SetWindowLong ( hDlg, DWL_USER, (LONG)lpDelInfo );

    int nIndex = VListBox_GetCurSel ( lpDelInfo->hListbox );

    if ( nIndex != LB_ERR)
        {
        TCHAR   szWork [ SYM_MAX_PATH ];
        TCHAR   szMoreWork [ SYM_MAX_PATH ];
        TCHAR   szMessage [ SYM_MAX_PATH ];
        LPTSTR  lpszWork;

        ListBox_GetText( lpDelInfo->hListbox, nIndex, szWork );

        lpszWork = szWork;

        while (*lpszWork != EOS && *lpszWork != _T('\t'))
        {
            lpszWork = CharNext (lpszWork);
        }

        *lpszWork = EOS;

        LoadString ( hInstance, IDS_DELAREYOUSURE, szMoreWork,
                     sizeof(szMoreWork ) );

        SPRINTF ( szMessage, szMoreWork, (LPTSTR)szWork );
        Static_SetText ( GetDlgItem (hDlg, IDI_DV_RAP), szMessage );
        }
    else
        {
        EndDialog ( hDlg, 0 );
        }
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

VOID LOCAL OnCommand (                  // Process WM_COMMAND
    HWND    hDlg,                       // [in] window handle
    int     nID,                        // [in] control id
    HWND    hwndCtl,                    // [in] control handle
    UINT    codeNotify )                // [in] code notify
{

    switch ( nID )
        {
        case IDOK:
            EndDialog ( hDlg, TRUE );
            break;

        case IDCANCEL:
            EndDialog ( hDlg, FALSE );
            break;

        case IDHELP:
            {
            LPDELINFO lpDelInfo = (LPDELINFO)GetWindowLong ( hDlg, DWL_USER );
            SYM_ASSERT ( lpDelInfo );

            StdHelpFromDialog (hDlg, lpDelInfo->uHelpID);
            }
            break;
        }
}
