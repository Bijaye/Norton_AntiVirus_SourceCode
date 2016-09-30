// Copyright 1995 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/NPSUI/VCS/NPS_TRAY.CPv   1.0   19 May 1997 00:25:18   KSACKIN  $
//
// Description:
//
//
// Contains:
//
// See Also:
//
//***********************************************************************
// $Log:   S:/NPSUI/VCS/NPS_TRAY.CPv  $
// 
//    Rev 1.0   19 May 1997 00:25:18   KSACKIN
// Initial revision.
// 
//    Rev 1.5   22 Feb 1996 13:26:18   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
// 
//    Rev 1.4   11 Jan 1996 09:48:56   JWORDEN
// Port changes from Quake 8
//
//***********************************************************************
#include "platform.h"
#include "stdhelp.h"
#include "xapi.h"
#include "ngui.h"
#include "nps.h"
#include "npsres.h"
#include "npsstb.h"
#include "nps_hlp.h"


// ---------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// ---------------------------------------------------------------------------

VOID LOCAL MainDlgPopupTrayMenu (       // Build and display tray popup menu
    HWND        hDlg                    // [IN] Dialog box handle
    );

BOOL LOCAL SendTrayMessage (           // Setup for call to Shell_NotifyIcon
    HWND        hDlg,                   // [IN] Dialog hwnd owning tray icon
    DWORD       dwMessage,              // [IN] Message to send tray icon
    HICON       hIcon,                  // [IN] Icon to set tray icon to
    LPSTR       lpszTip                 // [IN] Tip to assign to tray icon
    );

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

VOID WINAPI ShowHideTrayIcon (          // Initialize tray with out icon
    HWND        hDlg,                   // [in] parent window
    BOOL        bShow                   // [in] true to show icon
    )
{
    static  HICON   hIcon = NULL;

    SYM_ASSERT ( hDlg );

                                        // -----------------------------------
                                        // Tray not supported in NT
                                        // -----------------------------------

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        return;

    if ( bShow == TRUE )
        {

        char    szMessage [ MAX_TRAY_MESSAGE ];
        LoadString ( ghInst, IDS_TRAY_TOOLTIP, szMessage, sizeof(szMessage) );

                                        // -----------------------------------
                                        // Load our tray icons.
                                        // -----------------------------------

        if (!hIcon)
            hIcon =  (HICON) LoadImage (
                                    ghInst, 
                                    MAKEINTRESOURCE (IDI_NPS_ICON),
                                    IMAGE_ICON,
                                    16,
                                    16,
                                    0
                                    );

        SYM_ASSERT ( hIcon );
                                        // -----------------------------------
                                        // Add ourselves to the tray.
                                        // -----------------------------------
        if ( hIcon )
            {
            SendTrayMessage ( hDlg, NIM_ADD, hIcon, szMessage );
            }
        }
    else
        {
                                        // -----------------------------------
                                        // Take ourselves out of the tray.
                                        // -----------------------------------

        SendTrayMessage ( hDlg, NIM_DELETE, NULL, NULL );
        }
}


//***************************************************************************
// MainDlgTrayNotify()
// 
// Description:
//      Handles the APP_MESSAGE_TRAY_NOTIFY message that our window
//      receives when the tray wants to talk to us.
//
// NOTES:
//      1)  Must perform the IsWindowEnabled() check to make sure that
//          our window is active and thier isn't a subdialog or error 
//          message being displayed.  To allow a tray popup menu to
//          be presented to the user, you can get in to serious trouble.
//      2)  There is a bug in that the popup menu doesn't close when you 
//          click outside of the area even though we pass NULL as the 
//          rectangle parameter to TrackPopupMenu()
// 
// Parameters:
//      hDlg            - [IN] Dialog box handle
//      wParam          - [IN] wParam of message
//      lParam          - [IN] lParam of message
// 
// Return Value: 
//      BOOL            - TRUE if we handled message; FALSE otherwise
// 
// See Also:
//      MainDlgPopupTrayMenu()
//
//***************************************************************************
// 12/07/1994 MARKL Function Created.                                   
//***************************************************************************

BOOL WINAPI MainDlgTrayNotify (         // Handles tray notifications
    HWND        hDlg,                   // [IN] Dialog box handle
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam                  // [IN] lParam of message
    )
{
    SYM_ASSERT (hDlg);

                                        // Make the tray item do what the
                                        // user is requesting it to do.
    switch (lParam)
        {
                                        // WM_RBUTTONDOWN:  The tray icon
                                        // wants us to show a menu.
        case WM_RBUTTONDOWN:
            if (wParam == ID_TRAY_ICON)
                {
                if (IsWindowEnabled (hDlg))
                    {
                    MainDlgPopupTrayMenu (hDlg);
                    }
                else
                    {
                    MessageBeep (0);
                    }
                }
            break;

                                        // WM_LBUTTONDBLCLK:  The user
                                        // wants us to show ourselves so
                                        // do so.
        case WM_LBUTTONDBLCLK:
            FORWARD_WM_COMMAND ( hDlg, ID_TRAY_MENU_RESTORE, 0, 0, PostMessage );
            break;

        default:
            return (FALSE);
        }

                                        // We handled it so return TRUE
    return (TRUE);
} // MainDlgTrayNotify()



//***************************************************************************
// MainDlgPopupTrayMenu()
// 
// Description:
//      Displays a popup menu for our tray item.  This allows the user to
//      Open, Enable/Disable, or Unload Auto-Protect.  Note that depending
//      on the configuration the menu may not display Enable/Disable or
//      Unload.
//
// NOTE:
//      If the user selects an option, it will be send as a WM_COMMAND
//      message to the calling dialog proc (in this case MainDlgProc).
// 
// Parameters:
//      hDlg            - [IN] Dialog box handle
// 
// Return Value: 
//      NONE
// 
//***************************************************************************
// 12/06/1994 MARKL Function Created.                                   
//***************************************************************************

VOID LOCAL MainDlgPopupTrayMenu (       // Build and display tray popup menu
    HWND        hDlg                    // [IN] Dialog box handle
    )
{
#define         MENU_TEXT_SIZE  40
    HMENU       hPopup = NULL;
    char        szMenuText[MENU_TEXT_SIZE];
    POINT       pMouse;

    SYM_ASSERT (hDlg);

                                        // Create the Popup menu
    if (hPopup = CreatePopupMenu ())
        {

                                        // Add the Restore item as the first
                                        // item.
        LoadString ( ghInst, IDS_TRAY_MENU_RESTORE,
                    szMenuText, sizeof (szMenuText));

        AppendMenu (hPopup, MF_STRING, ID_TRAY_MENU_RESTORE, szMenuText);
        AppendMenu (hPopup, MF_SEPARATOR, 0, NULL);

                                        // Add the Close item.
        LoadString ( ghInst, IDS_TRAY_MENU_CLOSE,
                    szMenuText, sizeof (szMenuText));
        AppendMenu (hPopup, MF_STRING, IDM_NPS_EVENTCLOSE, szMenuText);

                                        // Finally time to show the menu
                                        // to our user and allow him to
                                        // select an option.
        GetCursorPos (&pMouse);
        TrackPopupMenu (hPopup, TPM_TOPALIGN | TPM_RIGHTALIGN, 
                        pMouse.x, pMouse.y, 0, hDlg, NULL);
        }

                                        // Return to caller after doing 
                                        // cleanup
    SYM_ASSERT (hPopup);
    if (hPopup)
        {
        DestroyMenu (hPopup);
        }
} // MainDlgTrayMenu()



//***************************************************************************
// SendTrayMessage()
// 
// Description:
//      Processes the translation of various tray messages into the
// appropriate Shell_NotifyIcon() call by loading the NOTIFYICONDATA
// structure with the appropriate data.
// 
// Parameters:
//      hDlg            - [IN] Dialog handle owning tray item
//      dwMessage       - [IN] Message to send tray icon
//      hIcon           - [IN] Icon to set tray icon to
//      lpszTip         - [IN] Tip to assign to icon
// 
// Return Value: 
//      BOOL            - FALSE if failed; TRUE if okay
// 
// See Also: 
//      TRAYNOT sample program in Win95 SDK.
// 
//***************************************************************************
// 12/05/1994 MARKL Function Created.                                   
//***************************************************************************

BOOL LOCAL SendTrayMessage (           // Setup for call to Shell_NotifyIcon
    HWND        hDlg,                   // [IN] Dialog hwnd owning tray icon
    DWORD       dwMessage,              // [IN] Message to send tray icon
    HICON       hIcon,                  // [IN] Icon to set tray icon to
    LPSTR       lpszTip                 // [IN] Tip to assign to tray icon
    )
{
    auto    BOOL            bReturn = FALSE;
    auto    NOTIFYICONDATA  TrayIconData;

    SYM_ASSERT (hDlg);

                                        // -----------------------------------
                                        // Initialize the TrayIconData
                                        // structure.
                                        // -----------------------------------

    TrayIconData.cbSize                 = sizeof (NOTIFYICONDATA);
    TrayIconData.hWnd                   = hDlg;
    TrayIconData.uID                    = ID_TRAY_ICON;
    TrayIconData.uFlags                 = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    TrayIconData.uCallbackMessage       = APP_MESSAGE_TRAY_NOTIFY;
    TrayIconData.hIcon                  = hIcon;

                                        // -----------------------------------
                                        // Load the correct tip string.
                                        // -----------------------------------
    if (lpszTip)
        {
        STRNCPY (TrayIconData.szTip, lpszTip, sizeof (TrayIconData.szTip));
        }
    else
        {
        TrayIconData.szTip[0] = EOS;
        }

                                        // -----------------------------------
                                        // Tell the tray icon to do what
                                        // we just set up.
                                        // -----------------------------------

    bReturn = Shell_NotifyIcon (dwMessage, &TrayIconData);

    return ( bReturn );
}

