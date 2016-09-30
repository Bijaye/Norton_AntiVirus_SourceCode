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
#include "stdafx.h"

#define STRICT

#define ISOLATION_AWARE_ENABLED 1
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <crtdbg.h>
#include "shellapi.h"
#include "apw.h"
#include "apwutil.h"
#include "apwcfg.h"
#include "apwtray.h"
#include "resource.h"
#include "..\apwuires\resource.h"
#include "osinfo.h"
#include "ccEvent.h"

#include "NAVOptions.h"
#include "NAVSnoozeSettings.h"

#include "navapnotify.h" // for tray icon event
ccLib::CEvent g_eventTrayIcon;
extern CAtlBaseModule _Module;
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

BOOL ApwUpdateTrayIcon (bool refreshIcon)
{
    auto    NOTIFYICONDATA  rIconData;
    auto    DWORD           dwMessage;
    auto    UINT            uTrayTipID;

    auto    CApwConfig      *pConfig;
    auto    BOOL            bSuccess;
	auto    CString csBuffer;

    // Initialize locals.

	CCTRACEI("UpdateTrayIcon");

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

            if ( !refreshIcon && pConfig->GetState() == pConfig->GetTrayState() )
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
	csBuffer.LoadString(_Module.GetResourceInstance(), uTrayTipID);
	wsprintf(rIconData.szTip, csBuffer, g_csProductName);

    // Update the icon in the tray.

	CCTRACEI("Update icon 0x%X : %s", dwMessage, rIconData.szTip);

    bSuccess = Shell_NotifyIcon( dwMessage, &rIconData );

	// If we tried to add an icon that was already present,
	// we'll get an error.  In this case, perform a MODIFY action.
	// If this fails, then we truly have issues.
	if( bSuccess == false && dwMessage == NIM_ADD )
	{
		bSuccess = Shell_NotifyIcon( NIM_MODIFY, &rIconData );
	}

    // Remember state of icon

    if ( bSuccess )
    {
        pConfig->SetInTray( pConfig->GetShowIcon() );
        pConfig->SetTrayState( pConfig->GetState() );
    }

BailOut:
    if ( !g_eventTrayIcon.SetEvent ())
        CCTRACEE ("ApwUpdateTrayIcon - failed to set tray icon event!");

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
    LRESULT lResult = 0;

	if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONDBLCLK)
	{
		if (!IsSnoozeAlertDialogShown())
		{
			// Do something only if the snooze alert is not displayed
			if (lParam == WM_RBUTTONUP)
			{
		        // Display our popup menu.
		        ApwTrayPopupMenu( hWnd );
			}
			else
			{
		        // Bring up NAVW main dialog.
		        ApwActivateNavw();
			}

		    lResult = 1;
		}
	}

    return lResult;
}


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
	auto	CApwCmd		*pCmd;

	auto    HMENU       hMenu;
	auto    HMENU       hPopupMenu;
	auto    POINT       rMousePos;

	auto    TCHAR       szEnableText [ MAX_MENU_TEXT + 1 ];
	auto    UINT        uEnableID;
	auto    UINT        uEnableState;

	auto    BOOL        bSuccess;

	//    auto    HWND        hNavwWnd;
	//    auto    HWND        hCurrWnd;
	auto	COSInfo		osInfo;

	static  bool		bAdmin = COSInfo::IsAdminProcess();
	auto CString csBuffer;
	auto CString csOptions;
	auto CString csOpen;

	// Initialize locals.

	hMenu      = \
		hPopupMenu = NULL;

	bSuccess   = FALSE;
	pConfig    = ApwGetConfig();
	pCmd = pConfig->GetCmd();

    // Load our menu. Note the double menu load is intentional.
    // Need a popup style menu.

    hMenu = LoadMenu( _Module.GetResourceInstance(), MAKEINTRESOURCE( IDR_TRAY_MENU ) );
    hPopupMenu = GetSubMenu( hMenu, 0 );

    if ( NULL != hPopupMenu )
    {
        // Need to set text of Enable/Disable item.
        // Also disable if "Disable" and user not allowed to disable AP.

		BOOL bApFeatureEnabled = pCmd->FeatureEnabled();

        switch ( pConfig->GetState() )
        {
        case AP_STATE_ENABLED:
			uEnableID = IDS_TRAY_DISABLE;
			if(bApFeatureEnabled)
			{
				uEnableState = pConfig->GetCanDisable() && bAdmin ? MF_ENABLED : MF_GRAYED;
			}
			else
			{
				// License is expired, disable the AP
				//  and gray out "Config Norton AntiVirus" and "Enable Auto-Protect".

				if( pCmd->Init( NULL ) )
					pCmd->DisableDriver();

				uEnableState = MF_GRAYED;
				EnableMenuItem ( hPopupMenu, IDM_TRAY_OPTIONS, MF_BYCOMMAND | MF_GRAYED );
			}
            break;

        case AP_STATE_LOADING:
            uEnableID = IDS_TRAY_DISABLE;
            uEnableState = MF_GRAYED;
            break;

        case AP_STATE_DISABLED:
        default:
            uEnableID = IDS_TRAY_ENABLE;
			if(bApFeatureEnabled)
			{
				uEnableState = bAdmin ? MF_ENABLED : MF_GRAYED;
			}
			else
			{
				// License is expired,
				//  gray out "Config Norton AntiVirus" and "Enable Auto-Protect".

				uEnableState = MF_GRAYED;
				EnableMenuItem ( hPopupMenu, IDM_TRAY_OPTIONS, MF_BYCOMMAND | MF_GRAYED );
			}
			break;
        }

		csBuffer.LoadString(_Module.GetResourceInstance(), uEnableID);
		wsprintf(szEnableText, csBuffer, g_csProductName);
        uEnableState |= MF_STRING;

        ModifyMenu( hPopupMenu,
			IDM_TRAY_ENABLE,
			MF_BYCOMMAND | uEnableState,
			IDM_TRAY_ENABLE,
			szEnableText );

		// Format product name in IDM_TRAY_OPTIONS menu item
		csBuffer.LoadString(_Module.GetResourceInstance(), IDS_OPTIONS);
		csOptions.Format(csBuffer, g_csProductName);
		ModifyMenu(hPopupMenu,
			IDM_TRAY_OPTIONS,
			MF_BYCOMMAND | MF_STRING,
			IDM_TRAY_OPTIONS,
			csOptions);

		//
		// Only Administrators are allowed to launch options.  Gray it out
		// for non-admins.
		//
        BOOL bConfigWizFinished = TRUE;
        IsCfgWizFinished(bConfigWizFinished);

		if( bAdmin == false || FALSE == bConfigWizFinished )
		{
			EnableMenuItem( hPopupMenu, IDM_TRAY_OPTIONS,
				MF_BYCOMMAND | MF_GRAYED);
		}

		// Format product name in IDM_TRAY_OPEN menu item
		csBuffer.LoadString(_Module.GetResourceInstance(), IDS_OPEN_NAVUI);
		csOpen.Format(csBuffer, g_csProductName);
		ModifyMenu(hPopupMenu,
			IDM_TRAY_OPEN,
			MF_BYCOMMAND | MF_STRING,
			IDM_TRAY_OPEN,
			csOpen);

        // Check if the NAV integrator plugin is running.  If it is, then
        // disable the Open and Configure menu items.
        // Note that we don't have control over the integrator to pop up
        // NAV's options, so that's why Configure is disabled as well.

		HANDLE hMutex;
		BOOL   bRunning = FALSE;

        hMutex = CreateMutex ( NULL, TRUE, _T("NAV_Integrator_Plugin_Mutex"));

        if ( NULL != hMutex )
		{
            // If the mutex already existed, then the integrator plugin
            // is running.

            if ( ERROR_ALREADY_EXISTS == GetLastError() )
			{
                bRunning = TRUE;
			}

            CloseHandle ( hMutex );
		}

        // Disable menu items if the plugin is running.

        if ( bRunning )
		{
            EnableMenuItem ( hPopupMenu, IDM_TRAY_OPEN,
				MF_BYCOMMAND | MF_GRAYED );
		}


        // Check if the options dialog is already open (because we showed
        // it from an earlier context menu command).  If so, disable the
        // Configure menu option.

		

        hMutex = CreateMutex ( NULL, TRUE, _T("CNAVOptionsObj_{72ED3226-4C58-4290-93D4-7F15AE02203F}") );

        if ( NULL != hMutex )
		{
            if ( ERROR_ALREADY_EXISTS == GetLastError() )
			{
                EnableMenuItem ( hPopupMenu, IDM_TRAY_OPTIONS,
					MF_BYCOMMAND | MF_GRAYED );
			}

            CloseHandle ( hMutex );
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

