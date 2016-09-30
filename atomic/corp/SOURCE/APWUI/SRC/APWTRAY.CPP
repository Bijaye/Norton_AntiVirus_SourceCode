// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUI/VCS/apwtray.cpv   1.5   02 Apr 1998 18:24:22   DALLEE  $
//
// Description:
//  Source for NAVAPW shell tray icon management.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/APWUI/VCS/apwtray.cpv  $
// 
//    Rev 1.5   02 Apr 1998 18:24:22   DALLEE
// Made double left click on tray icon launch NAVW.
// No longer restores the NAVAPW main dialog - we don't want to
// display it anymore.
// 
//    Rev 1.4   02 Apr 1998 18:08:02   DALLEE
// Set Open NAV to be default menu item on tray menu.
// 
//    Rev 1.3   13 Mar 1998 17:59:08   tcashin
// When we're about to put up the menu, check if we can put up the
// options dialog. If we're not, then make the Options menu item gray.
// 
//    Rev 1.2   17 Jun 1997 18:05:54   DALLEE
// Changed default status text to be disabled, rather than enabled on the
// NAVAPW tray icon.
// 
//    Rev 1.1   11 Jun 1997 12:18:42   DALLEE
// Set text of enable/disable menu item based on current state.
// Disable "disable" menu item if options say so.
// 
//    Rev 1.0   02 Jun 1997 13:56:36   DALLEE
// Initial revision.
//************************************************************************

#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "apw.h"
#include "apwutil.h"
#include "apwcfg.h"
#include "apwtray.h"
#include "resource.h"

//************************************************************************
// Local prototypes
//************************************************************************

static BOOL ApwTrayPopupMenu ( HWND hWnd );


//************************************************************************
// Tray icon functions
//************************************************************************

//************************************************************************
// ApwUpdateTrayIcon()
//
// BOOL ApwUpdateTrayIcon ()
//
// This routine checks the current state of our tray icon versus our
// current configuration and displays, hides, or updates the tray icon
// as necessary.
//
// Returns:
//  TRUE    on success.
//  FALSE   otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

BOOL ApwUpdateTrayIcon ()
{
    auto    NOTIFYICONDATA  rIconData;
    auto    DWORD           dwMessage;
    auto    UINT            uTrayTipID;

    auto    CApwConfig      *pConfig;
    auto    BOOL            bSuccess;

    // Initialize locals.

    bSuccess = FALSE;
    pConfig  = ApwGetConfig();

    if ( FALSE == ShellTrayPresent() )
    {
        // No tray is present. Can't update icon.

        pConfig->SetInTray( FALSE );

        bSuccess = FALSE;
        goto BailOut;
    }

    // Determine what kind of update (if any) is required.

    if ( FALSE == pConfig->GetShowIcon() )
    {
        // Options say don't display icon.

        if ( FALSE == pConfig->GetInTray() )
        {
            // We're not in the tray. No need to remove.

            bSuccess = TRUE;
            goto BailOut;
        }
        else
        {
            // Need to remove icon from tray.

            dwMessage = NIM_DELETE;
        }
    }
    else
    {
        // Options say display icon.

        if ( FALSE == pConfig->GetInTray() )
        {
            // Not currently in tray. Need to add icon.

            dwMessage = NIM_ADD;
        }
        else
        {
            // We're already in the tray.

            if ( pConfig->GetState() == pConfig->GetTrayState() )
            {
                // We're already in the tray with the correct state.
                // No need to update.

                bSuccess = TRUE;
                goto BailOut;
            }
            else
            {
                // Need to modify icon.

                dwMessage = NIM_MODIFY;
            }
        }
    }

    // Initialize tray notification structure.

    memset( &rIconData, 0, sizeof( rIconData ) );

    rIconData.cbSize            = sizeof( rIconData );
    rIconData.hWnd              = pConfig->GetMainWnd();
    rIconData.uID               = APW_TRAY_ICON;
    rIconData.uFlags            = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    rIconData.uCallbackMessage  = APWM_TRAY_NOTIFY;
    rIconData.hIcon             = pConfig->GetSmallIcon();
    
    switch ( pConfig->GetState() )
    {
    case AP_STATE_ENABLED:
        uTrayTipID = IDS_TRAY_ENABLED;
        break;

    case AP_STATE_DISABLED:
    default:
        uTrayTipID = IDS_TRAY_DISABLED;
        break;
    }
    LoadString( pConfig->GetInstance(),
                uTrayTipID,
                rIconData.szTip,
                sizeof( TCHAR ) * ( sizeof( rIconData.szTip ) - 1 ) );

    // Update the icon in the tray.

    bSuccess = Shell_NotifyIcon( dwMessage, &rIconData );

    // Remember state of icon

    if ( bSuccess )
    {
        pConfig->SetInTray( pConfig->GetShowIcon() );
        pConfig->SetTrayState( pConfig->GetState() );
    }

BailOut:
    return ( bSuccess );
} // ApwUpdateTrayIcon()


//************************************************************************
// ApwTrayNotify()
//
// LRESULT ApwTrayNotify (
//      HWND    hWnd,                   // Handle to NAVAPW main window.
//      WPARAM  wParam,                 // Taskbar icon ID.
//      LPARAM  lParam );               // Mouse message received.
//
// This routine is the shell tray callback for our icon.
// It handles the pop-up menu and restoring our main window.
//
// Returns:
//  1   If message processed.
//  0   Otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

LRESULT ApwTrayNotify ( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    auto    LRESULT     lResult;

    // Initialize locals.

    lResult = 1;

    // Switch on type of mouse message received.

    switch ( lParam )
    {
    case WM_RBUTTONUP:
        // Display our popup menu.

        ApwTrayPopupMenu( hWnd );
        break;

    case WM_LBUTTONDBLCLK:
        // Bring up NAVW main dialog.

        ApwActivateNavw();
        break;

    default:
        lResult = 0;
        break;
    }

    return ( lResult );
} // ApwTrayNotify()


//************************************************************************
// ApwTrayPopupMenu()
//
// LRESULT ApwTrayPopupMenu (
//      HWND    hWnd );                 // Handle to NAVAPW main window.
//
// This routine displays the tray icon's popup menu.
//
// Returns:
//  TRUE    if menu was displayed.
//  FALSE   otherwise.
//************************************************************************
// 05/21/1997 DALLEE, created.
//************************************************************************

static BOOL ApwTrayPopupMenu ( HWND hWnd )
{
    #define MAX_MENU_TEXT 80

    auto    CApwConfig  *pConfig;

    auto    HMENU       hMenu;
    auto    HMENU       hPopupMenu;
    auto    POINT       rMousePos;
                        
    auto    TCHAR       szEnableText [ MAX_MENU_TEXT + 1 ];
    auto    UINT        uEnableID;
    auto    UINT        uEnableState;
                        
    auto    BOOL        bSuccess;

    auto    HWND        hNavwWnd;
    auto    HWND        hCurrWnd;

    // Initialize locals.

    hMenu      = \
    hPopupMenu = NULL;

    bSuccess   = FALSE;
    pConfig    = ApwGetConfig();

    // Load our menu. Note the double menu load is intentional.
    // Need a popup style menu.

    hMenu = LoadMenu( pConfig->GetInstance(), MAKEINTRESOURCE( IDR_TRAY_MENU ) );
    hPopupMenu = GetSubMenu( hMenu, 0 );

    if ( NULL != hPopupMenu )
    {
        // Need to set text of Enable/Disable item.
        // Also disable if "Disable" and user not allowed to disable AP.

        switch ( pConfig->GetState() )
        {
        case AP_STATE_ENABLED:
            uEnableID = IDS_TRAY_DISABLE;
            uEnableState = pConfig->GetCanDisable() ? MF_ENABLED : MF_GRAYED;
            break;

        case AP_STATE_LOADING:
            uEnableID = IDS_TRAY_DISABLE;
            uEnableState = MF_GRAYED;
            break;

        case AP_STATE_DISABLED:
        default:
            uEnableID = IDS_TRAY_ENABLE;
            uEnableState = MF_ENABLED;
            break;
        }

        if ( 0 != LoadString( pConfig->GetInstance(),
                              uEnableID,
                              szEnableText,
                              sizeof( szEnableText ) / sizeof( TCHAR ) ) )
        {
            uEnableState |= MF_STRING;
        }

        ModifyMenu( hPopupMenu, 
                    IDM_TRAY_ENABLE, 
                    MF_BYCOMMAND | uEnableState,
                    IDM_TRAY_ENABLE,
                    szEnableText );

        // Disable the options menu item if the main UI 
        // is up and some other dialog box is open.

        if (::ApwGetCurrentNavWindow( (LPHANDLE)&hNavwWnd, (LPHANDLE)&hCurrWnd))
        {
            if ( hNavwWnd != hCurrWnd)
            {
                EnableMenuItem (hPopupMenu, 
                                IDM_TRAY_OPTIONS,
                                MF_BYCOMMAND | MF_GRAYED);
            }
        }

        // Set default item to open NAV. Doesn't do much here on right click
        // (menu item won't accept ENTER), but it gives a hint what the 
        // double left click behavior is.

        SetMenuDefaultItem( hPopupMenu, IDM_TRAY_OPEN, FALSE );

        // Kluge part 1: Put our Wnd in foreground before calling TrackPopupMenu
        // See notes in W32 SDK KBase --
        // "PRB: Menus for Notification Icons Don't Work Properly"

        SetForegroundWindow( hWnd );

        GetCursorPos( &rMousePos );
        bSuccess = TrackPopupMenu( hPopupMenu,
                                   TPM_LEFTALIGN | TPM_LEFTBUTTON,
                                   rMousePos.x,
                                   rMousePos.y,
                                   0,
                                   hWnd,
                                   NULL );

        // Kluge part 2: Bogon message to cause a task switch to us

        PostMessage( hWnd, WM_USER, 0, 0 );
    }

    if ( NULL != hMenu )
        DestroyMenu( hMenu );
    
    return ( bSuccess );
} // ApwTrayPopupMenu()

