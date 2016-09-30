///////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/CALLBACK/VCS/infbxw32.cpv   1.0   18 Jun 1997 13:16:34   MKEATIN  $
///////////////////////////////////////////////////////////////////////////////////
//
// InfoBox.cpp - Contains implementation for generic NAVEXInfoBox() callbacks.
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/CALLBACK/VCS/infbxw32.cpv  $
// 
//    Rev 1.0   18 Jun 1997 13:16:34   MKEATIN
// Initial revision.
// 
//    Rev 1.0   17 Jun 1997 15:08:40   MKEATIN
// Initial revision.
// 
//    Rev 1.0   31 May 1997 15:52:44   MKEATIN
// Initial revision.
///////////////////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "windowsx.h"
#include "callbk15.h"
#include "callw32.h"
#include "resource.h"

extern HINSTANCE hInstance;

///////////////////////////////////////////////////////////////////////////////////
// Local Typedefs
typedef struct INFOBOXDATA {
                        LPTSTR  lpszText;
                        HWND hDlg;
                        DWORD dwResultFlag;
                     } INFOBOXDATA, *LPINFOBOXDATA;

///////////////////////////////////////////////////////////////////////////////////
// Local Functions
BOOL CALLBACK InfoBoxDlgProc( HWND hwndDlg,   // handle to dialog box 
                              UINT uMsg,      // message 
                              WPARAM wParam,  // first message parameter 
                              LPARAM lParam   // second message parameter 
                            );

static void OnInitDialog(HWND hDlg, LPARAM lParam);
static void OnCancel(HWND hDlg );

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

UINT WINAPI NAVEXInfoBoxInit32
(
    LPTSTR   lpszTitle,
    DWORD    dwButtonFlags,      // CANCEL only
    LPLPVOID lplpvInfoBoxInfo,
    LPVOID   lpvCookie)
{
    // get parent
    HWND hParent = (HWND)lpvCookie;

    // Allocate InfoBoxData Structure.
    LPINFOBOXDATA pData = new INFOBOXDATA;
    
    if( pData == NULL )
        return NAVEX_UI_ERROR;

    // Save off pointer to data
    *lplpvInfoBoxInfo = (LPVOID) pData;

    // Populate structure.
    ZeroMemory(pData, sizeof(INFOBOXDATA));
    pData->lpszText = lpszTitle;

    // Should we display a cancel button?
    int iDlgTemplate = (dwButtonFlags & BUTTON_FLAG_CANCEL) ? 
                        IDD_INFO_CANCEL_WINDOW : IDD_INFO_WINDOW;

    // Create modeless dialog.
    pData->hDlg = CreateDialogParam( hInstance,               // handle to application instance 
                            MAKEINTRESOURCE( iDlgTemplate ),   // identifies dialog box template 
                            hParent,                           // handle to owner window 
                            (DLGPROC)InfoBoxDlgProc,           // pointer to dialog box procedure 
                            (LPARAM) pData        
                            ); 

    // Need to disable parent window
    EnableWindow( hParent, FALSE );

    return NAVEX_UI_OK;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

UINT WINAPI NAVEXInfoBoxUpdate32
(
    LPVOID  lpvInfoBoxInfo,
    LPTSTR  lpszText,
    LPDWORD lpdwResultFlags,
    LPVOID  lpvCookie
)
{
    // Get pointer to data.
    LPINFOBOXDATA pData = (LPINFOBOXDATA) lpvInfoBoxInfo;
    if( pData == NULL )
        return NAVEX_UI_ERROR;

    // If the window hasn't been shown, show it!
    if(!IsWindowVisible(pData->hDlg))
        ShowWindow( pData->hDlg, SW_SHOW );


    // Update text in dialog
    HWND hCtrl = GetDlgItem( pData->hDlg, IDC_TEXT );
    SetWindowText(hCtrl, lpszText);

    // Get result flag.
    *lpdwResultFlags = pData->dwResultFlag;

    return NAVEX_UI_OK;       
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

UINT WINAPI NAVEXInfoBoxClose32
(
    LPVOID lpvInfoBoxInfo,
    LPVOID lpvCookie
)
{
    // Get pointer to data.
    LPINFOBOXDATA pData = (LPINFOBOXDATA) lpvInfoBoxInfo;
    if( pData == NULL )
        return NAVEX_UI_ERROR;

    // Need to re-enable parent window
    EnableWindow( GetParent( pData->hDlg ), TRUE );

    // Destroy our window.
    DestroyWindow( pData->hDlg );

    // De-allocate data.
    delete pData;

    return NAVEX_UI_OK;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK InfoBoxDlgProc( HWND hwndDlg,   // handle to dialog box 
                              UINT uMsg,      // message 
                              WPARAM wParam,  // first message parameter 
                              LPARAM lParam   // second message parameter 
                            ) 
{
    switch(uMsg)
        {
        case WM_INITDIALOG:
            OnInitDialog(hwndDlg, lParam);
            return TRUE;

        case WM_COMMAND:
            switch(GET_WM_COMMAND_ID(wParam, lParam))
                {
                case IDCANCEL:
                    OnCancel(hwndDlg);
                    return TRUE;
                }
            return FALSE;

        default:
            return FALSE;// Did not process this message       

        }
    
    return TRUE;
        
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void OnInitDialog(HWND hDlg, LPARAM lParam)
{
    // Save off pointer to init data.
    LPINFOBOXDATA pData = (LPINFOBOXDATA) lParam;
    SetWindowLong(hDlg, GWL_USERDATA, (LONG) pData);

    // Set up text control
    SetWindowText( hDlg, pData->lpszText );
    
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void OnCancel(HWND hDlg )
{
    // Save off pointer to init data.
    LPINFOBOXDATA pData = (LPINFOBOXDATA) GetWindowLong(hDlg, GWL_USERDATA);

    // Set cancel state.
    pData->dwResultFlag = BUTTON_FLAG_CANCEL;

}
