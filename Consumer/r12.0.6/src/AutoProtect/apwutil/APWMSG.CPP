// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUTIL/VCS/Apwmsg.cpv   1.0   02 Jun 1997 02:48:44   DALLEE  $
//
// Description:
//      Source for NAVAPW message functions.
//
// Contains:
//      APWMessage()
//
// See Also:
//************************************************************************
// $Log:   S:/APWUTIL/VCS/Apwmsg.cpv  $
//
//    Rev 1.0   02 Jun 1997 02:48:44   DALLEE
// Initial revision.
//************************************************************************

#define STRICT
#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>

#include "apw.h"
#include "apwutil.h"
#include "resource.h"
#include "..\apwutilres\resource.h"
#include "..\apwuires\resource.h"
#include "AVRESBranding.h"
#include "APWRES.H"

bool _InitResources();


//************************************************************************
// External data.
//************************************************************************

#ifdef __cplusplus
extern  "C" {
#endif

extern HINSTANCE   g_hUtilInst;
#ifdef __cplusplus
}
#endif
extern CAtlBaseModule _Module;


//************************************************************************
// Local prototypes
//************************************************************************

static BOOL CALLBACK ApwMessageProc ( HWND    hWnd,
                                      UINT    uMsg,
                                      WPARAM  wParam,
                                      LPARAM  lParam );

static VOID ApwMessageWM_COMMAND ( HWND    hWnd,
                                   int     nID,
                                   HWND    hWndControl,
                                   UINT    uCode );

static BOOL ApwMessageWM_INITDIALOG( HWND hWnd, HWND hWndFocus, LPARAM lParam );


//************************************************************************
// NAVAPW notification message routines.
//************************************************************************

APWUTILAPI BOOL WINAPI ApwMessage( APWMESSAGEINFO *pMsgInfo )
{
	_InitResources();

	return ( -1 != DialogBoxParam( _Module.GetResourceInstance(),
                                   MAKEINTRESOURCE( IDD_APW_MESSAGE ),
                                   pMsgInfo->hParent,
                                   ApwMessageProc,
                                   ( LPARAM ) pMsgInfo ) );
} // ApwMessage()

static BOOL CALLBACK ApwMessageProc ( HWND    hWnd,
                                      UINT    uMessage,
                                      WPARAM  wParam,
                                      LPARAM  lParam )
{
    auto    BOOL    bHandled;

    // Initialize locals.

    bHandled = TRUE;

    // Handle message.

    switch ( uMessage )
    {
    case WM_INITDIALOG:
        bHandled = HANDLE_WM_INITDIALOG( hWnd, wParam, lParam,
                                         ApwMessageWM_INITDIALOG );
        break;

    case WM_COMMAND:
        HANDLE_WM_COMMAND( hWnd, wParam, lParam,
                           ApwMessageWM_COMMAND );
        break;

    default:
        bHandled = FALSE;
        break;
    }

    return ( bHandled );
} // ApwMessageProc()


//************************************************************************
// ApwMessageWM_COMMAND()
//
// Respond to WM_COMMAND for NAVAPW message window.
//
// Parameters:
//      HWND        hWnd                Handle of main window.
//      int         nID                 Control ID.
//      HWND        hWndControl         Window handle of control.
//      UINT        uCode               Notification code.
//
// Returns:
//      Nothing.
//************************************************************************
// 05/29/1997 DALLEE, created.
//************************************************************************

static VOID ApwMessageWM_COMMAND ( HWND    hWnd,
                                   int     nID,
                                   HWND    hWndControl,
                                   UINT    uCode )
{
    switch ( nID )
    {
    case IDOK:
        EndDialog( hWnd, IDOK );
        break;

    case IDCANCEL:
        EndDialog( hWnd, IDCANCEL );
        break;

    default:
        break;
    }
} // ApwMessageWM_COMMAND()


//************************************************************************
// ApwMessageWM_INITDIALOG()
//
// static BOOL ApwMessageWM_INITDIALOG (
//      HWND    hWnd,
//      HWND    hWndFocus,
//      WPARAM  wParam,
//      LPARAM  lParam );
//
// Dialog initialization. Set 3d controls and message text.
//
// Returns:
//      TRUE Always ( to let Windows set focus to default hWndFocus )
//************************************************************************
// 05/29/1997 DALLEE, created.
//************************************************************************

static BOOL ApwMessageWM_INITDIALOG( HWND hWnd, HWND hWndFocus, LPARAM lParam )
{
    #define MAX_FORMAT  300

    auto    APWMESSAGEINFO  *pMsgInfo;
    auto    LPTSTR          lpszMessage;
    auto    LPTSTR          lpszFormat;
    const   TCHAR           szEmpty [] = _T("");

    auto    LPCTSTR         lppszArgs [ 3 ];
	auto	CString			csFormat;

    // Initialize locals.

    pMsgInfo = ( APWMESSAGEINFO * ) lParam;
    lpszMessage = \
    lpszFormat  = NULL;

    assert( pMsgInfo );
    assert( pMsgInfo->uMessage );

    // Fix look of dialog.

    ApwCenterWindow( hWnd, pMsgInfo->hParent );
    ApwDlgSet3d( hWnd );

    // Set alert text.

    if ( pMsgInfo->lpszAlert )
    {
        SetDlgItemText( hWnd, IDC_MSG_ALERT, pMsgInfo->lpszAlert );
    }

    // Allocate buffer for loading messages.

    lpszFormat = ( LPTSTR ) GlobalAllocPtr( GHND, MAX_FORMAT * sizeof( TCHAR ) );

    if ( NULL == lpszFormat )
    {
        goto BailOut;
    }

    // Set title text.

	if(g_csProductName.IsEmpty())
	{
		CBrandingRes BrandRes;
		g_csProductName = BrandRes.ProductName();
	}

	SetWindowText( hWnd, g_csProductName );

    // Get message arguments.

    lppszArgs[ 0 ] = pMsgInfo->lpszString1 ? pMsgInfo->lpszString1 : szEmpty;
    lppszArgs[ 1 ] = pMsgInfo->lpszString2 ? pMsgInfo->lpszString2 : szEmpty;
    lppszArgs[ 2 ] = pMsgInfo->lpszString3 ? pMsgInfo->lpszString3 : szEmpty;

    // Set message text.

    csFormat.LoadString( pMsgInfo->hInstance, pMsgInfo->uMessage);

	if(pMsgInfo->uMessage == IDS_ERR_LOAD_DRIVER)
	{
		// Format with product name
		wsprintf(lpszFormat, csFormat, g_csProductName);
	}

    if ( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_FROM_STRING |
                          FORMAT_MESSAGE_ARGUMENT_ARRAY,
                        lpszFormat,
                        0,
                        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                        ( LPTSTR ) &lpszMessage,
                        0,
                        ( va_list * ) lppszArgs ) )
    {
        SetDlgItemText( hWnd, IDC_MSG_TEXT, lpszMessage );

        LocalFree( lpszMessage );
    }

BailOut:
    if ( lpszFormat )
        GlobalFreePtr( lpszFormat );

    return ( TRUE );
} // ApwMessageWM_INITDIALOG()

