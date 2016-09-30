// Copyright 1998 Symantec Corporation.
//*************************************************************************
//
// $Header:   S:/NAVW/VCS/infodlg.cpv   1.7   14 Jul 1998 14:12:26   jtaylor  $
//
// Description:
//      Routines for the Info button.
//
// Contains:
//
//*************************************************************************
// $Log:   S:/NAVW/VCS/infodlg.cpv  $
// 
//    Rev 1.7   14 Jul 1998 14:12:26   jtaylor
// Changed the OEM version info dialog to display a string for old definitions that does not mention LiveUpdate.
//
//    Rev 1.6   10 Jul 1998 14:09:28   DBuches
// Fixed date formatting problems.
//
//    Rev 1.5   20 Jun 1998 13:36:12   tcashin
// Added icons, fixed the title bars, fixed a bug in opening several info
// dialogs at once, and other misc. fixes.
//
//    Rev 1.4   15 Jun 1998 18:12:46   DBuches
// Info dialog needs to respond to IDCANCEL command message.
//
//    Rev 1.3   14 Jun 1998 15:23:20   tcashin
// Make all print formatting use the same one (_stprintf()).
//
//    Rev 1.1   10 Jun 1998 22:33:54   tcashin
// Use total items in the quarantine status structure.
//
//    Rev 1.0   07 Jun 1998 15:38:48   tcashin
// Initial revision.
//
//*************************************************************************

#include "platform.h"
#include "ngui.h"                       // Norton look / feel
#include "navw.h"
#include <objbase.h>
#include "quar32.h"
#include "iquaran.h"
#include "defutils.h"
#include "navappids.h"
#include "navwstb.h"
#include "navwres.h"
#include "infodlg.h"
#include "ctl3d.h"
#include <time.h>
#include "n32behav.h"  // personality DLL

extern  HINSTANCE   ghInst;

/* **************************************************************************

@Name:
LRESULT WINAPI NavMainWndWM_COMMAND(    // Handles WM_COMMAND for main wnd
    HWND          hWnd,                 // [in] Window handle
    WPARAM        wParam,               // [in] wParam of WM_COMMAND message
    LPARAM        lParam,               // [in] lParam of WM_COMMAND message
    LPNAVMAIN     lpNav                 // [in/out] Main window data struct
    );

@Description:
This routine handles the WM_COMMAND message for the main dialog window
of the Norton AntiVirus for Windows.  Processing depends on the control
the user interacted with.

@Parameters:
$hWnd$            [in] Window handle of main dialog window
$wParam$          [in] wParam of WM_COMMAND message
$lParam$          [in] lParam of WM_COMMAND message
$lpNavMain$       [in/out] data structure for main window
$lpNavScanSpec$   [in/out] data structure for what to scan

@Returns:
      0L              message processed
      DefDlgProc()    message not processed.

************************************************************************** */
VOID SYM_EXPORT WINAPI InfoDlg (
    HWND            hDlg,
    LPINFOSTRUCT    lpInfo
    )
{
    EnableWindow( hDlg, FALSE);

    DialogBoxParam (
                   ghInst,
                   MAKEINTRESOURCE ( IDD_INFO ),
                   NULL,
                   &InfoDlgProc,
                   (LPARAM)lpInfo
                   );

    EnableWindow( hDlg, TRUE);
    SetActiveWindow(hDlg);
}


/* **************************************************************************

@Name:
BOOL CALLBACK InfoDlgProc (
    HWND          hWnd,                 // [in] Window handle
    WPARAM        wParam,               // [in] wParam of WM_COMMAND message
    LPARAM        lParam,               // [in] lParam of WM_COMMAND message
    LPNAVMAIN     lpNav                 // [in/out] Main window data struct
    );

@Description:
This routine handles the messages for the Info dialog.

@Parameters:
$hWnd$            [in] Window handle of main dialog window
$wParam$          [in] wParam of WM_COMMAND message
$lParam$          [in] lParam of WM_COMMAND message
$lpNavMain$       [in/out] data structure for main window

@Returns:
      TRUE        message processed
      FALSE       message not processed.

************************************************************************** */
BOOL CALLBACK InfoDlgProc (HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    static  BOOL            bPaintProcessed = TRUE;
    auto    BOOL            bReturn = FALSE;
    auto    LPINFOSTRUCT    lpInfo;
    auto    TCHAR           szFormat [1024];
    auto    TCHAR           szMsgText[1024];
    auto    TCHAR           szTitleText[1024];
    auto    HICON           hStatusIcon;
    auto    UINT            uIconID;
    auto    UINT            uStringID;


    switch(wMessage)
        {
        case WM_CTLCOLOR:

            bReturn = (BOOL)Ctl3dCtlColorEx(wMessage, wParam, lParam);
            break;

        case WM_INITDIALOG:

            lpInfo = (LPINFOSTRUCT)lParam;

            NG_SetFontsInDialog (hDlg, 0);
            NG_CenterDialog( hDlg, GetParent( hDlg ) );

            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);

            switch (lpInfo->dwInfoType)
            {
                case INFOVIRUSDEFS:
                    {
                    auto    CDefUtils   defUtils;
                    auto    LPCTSTR     lpszAppId;

                    auto    WORD        wYear      = 0;
                    auto    WORD        wMonth     = 0;
                    auto    WORD        wDay       = 0;
                    auto    DWORD       dwRevision = 0;

                    auto    CTime       *pDefDate;
                    auto    CTime       Today;
                    auto    CTimeSpan   DefAge;
                    auto    int         days;
                    auto    TCHAR       szDate[32];
                    auto    SYSTEMTIME  st;


                    lpszAppId = ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() ?
                                    g_szNavAppIdNavwNt : g_szNavAppIdNavw95 );

                    if ( defUtils.InitWindowsApp( lpszAppId ) )
                    {
                        if ( !defUtils.DefsAreNewest() )
                        {
                            defUtils.GetNewestDefsDate( &wYear, &wMonth, &wDay, &dwRevision );

                            pDefDate = new CTime( wYear, wMonth, wDay, 0, 0, 0);

                            ZeroMemory( &st, sizeof( SYSTEMTIME ) );
                            st.wYear = wYear;
                            st.wMonth = wMonth;
                            st.wDay = wDay;

                            GetDateFormat( LOCALE_SYSTEM_DEFAULT,
                                           DATE_SHORTDATE,
                                           &st,
                                           NULL,
                                           szDate,
                                           32 );

                            uStringID = IDS_INFO_DEFSNEW;
                            uIconID = IDI_STATUS_DEFINITIONS;
                        }
                        else
                        {
                            defUtils.GetCurrentDefsDate( &wYear, &wMonth, &wDay, &dwRevision );

                            pDefDate = new CTime( wYear, wMonth, wDay, 0, 0, 0);

                            ZeroMemory( &st, sizeof( SYSTEMTIME ) );
                            st.wYear = wYear;
                            st.wMonth = wMonth;
                            st.wDay = wDay;

                            GetDateFormat( LOCALE_SYSTEM_DEFAULT,
                                           DATE_SHORTDATE,
                                           &st,
                                           NULL,
                                           szDate,
                                           32 );

                            Today  =  CTime::GetCurrentTime();

                            DefAge = Today - *pDefDate;

                            days = DefAge.GetDays();

                            if ( abs(days) > 30 )
                            {
                                uIconID = IDI_STATUS_DEFSOLD;

                                // Load the appropriate string for old definitions.
                                if( !NAV_IS_SCANONLY() )
                                    uStringID = IDS_INFO_DEFSOLD;
                                else
                                    uStringID = IDS_INFO_DEFSOLDOEM;
                            }
                            else
                            {
                                uIconID = IDI_STATUS_DEFINITIONS;
                                uStringID = IDS_INFO_DEFSCURRENT;

                            }
                        }

                        LoadString(ghInst, uStringID, szFormat, sizeof(szFormat) );

                        _stprintf(szMsgText, szFormat, szDate);

                        hStatusIcon = ( HICON )LoadImage( ghInst,
                                                          MAKEINTRESOURCE( uIconID ),
                                                          IMAGE_ICON,
                                                          0, 0,
                                                          LR_DEFAULTCOLOR );

                        if ( hStatusIcon )
                             Static_SetIcon( GetDlgItem( hDlg, IDC_INFOICON ), hStatusIcon );

                        SetDlgItemText( hDlg, IDC_INFOTEXT, szMsgText );

                        LoadString(ghInst, IDS_INFOTITLE_DEFINITIONS, szTitleText, sizeof(szTitleText));
                        SetWindowText( hDlg, szTitleText );

                        delete pDefDate;
                    }

                    break;
                    }
                case INFOQUARANTINE:

                    if (lpInfo->lpNav->rQuarantineStatus.uTotalItems > 1)
                    {
                        LoadString(ghInst, IDS_INFO_QUARANTINE2, szFormat, sizeof(szFormat) );
                        _stprintf(szMsgText, szFormat, (int)lpInfo->lpNav->rQuarantineStatus.uTotalItems);
                    }
                    else
                        LoadString(ghInst, IDS_INFO_QUARANTINE1, szMsgText, sizeof(szMsgText) );


                    SetDlgItemText(hDlg, IDC_INFOTEXT, szMsgText);

                    uIconID = IDI_STATUS_QUARANTINE;

                    hStatusIcon = ( HICON )LoadImage( ghInst,
                                                      MAKEINTRESOURCE( uIconID ),
                                                      IMAGE_ICON,
                                                      0, 0,
                                                      LR_DEFAULTCOLOR );

                    if ( hStatusIcon )
                      Static_SetIcon( GetDlgItem( hDlg, IDC_INFOICON ), hStatusIcon );

                    LoadString(ghInst, IDS_INFOTITLE_QUARANTINE, szTitleText, sizeof(szTitleText));
                    SetWindowText( hDlg, szTitleText );

                    break;

                default:

                    break;
            }

            bReturn = TRUE;
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

        case WM_COMMAND:

            switch ( GET_WM_COMMAND_ID ( wParam, lParam ) )
                {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    break;
                }

            break;
        }

    return (bReturn);
}
