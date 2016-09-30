///////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/VIRSCAN/VCS/edbxw32.cpv   1.0   31 May 1997 15:52:42   MKEATIN  $
///////////////////////////////////////////////////////////////////////////////////
//
// Editbox.cpp - Contains implementation for generic AVAPITextEditBox() callback.
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/VIRSCAN/VCS/edbxw32.cpv  $
// 
//    Rev 1.0   31 May 1997 15:52:42   MKEATIN
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

typedef struct TEXTDATA {
                        LPTSTR  lpszPrompt;
                        LPTSTR  lpszSeedText;
                        LPTSTR  lpszPutHere;
                        int     nMaxLength;
                     } TEXTDATA, *LPTEXTDATA;

///////////////////////////////////////////////////////////////////////////////////
// Local Functions

BOOL CALLBACK TextEditDlgProc( HWND hwndDlg,   // handle to dialog box 
                               UINT uMsg,      // message 
                               WPARAM wParam,  // first message parameter 
                               LPARAM lParam   // second message parameter 
                             ); 


static void OnInitDialog( HWND hDlg, LPARAM lParam );
static void OnOK( HWND hDlg );

///////////////////////////////////////////////////////////////////////////////////
// Local Data




///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

UINT WINAPI AVAPITextEditBox32 
(  
    DWORD  dwBoxID,
    LPTSTR lpszPrompt,
    LPTSTR lpszSeedText,
    LPTSTR lpszPutHere,
    UINT   nMaxLength,
    DWORD  dwMSTimeOut,
    LPVOID lpvCookie)
{
    // Validate input.
    if( (lpszPrompt  == NULL) ||
        (lpvCookie   == NULL) ||
        (lpszPutHere == NULL) )
        return NAVEX_UI_ERROR;

    // Save off parent window.  This will have to be changed.
    HWND hParent = (HWND)lpvCookie;
    
    // Pack up initialization data for dialog.
    TEXTDATA textData;
    ZeroMemory(&textData, sizeof(TEXTDATA));
    textData.lpszPrompt = lpszPrompt;
    textData.lpszSeedText = lpszSeedText;
    textData.lpszPutHere = lpszPutHere;
    textData.nMaxLength = nMaxLength;

    // Fire off the dialog
    DialogBoxParam( hInstance, 
                    MAKEINTRESOURCE(IDD_TEXT_EDIT_BOX),
                    hParent,
                    (DLGPROC)TextEditDlgProc,
                    (LPARAM) &textData );

    return NAVEX_UI_OK;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK TextEditDlgProc(    HWND hwndDlg,   // handle to dialog box 
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
                case IDOK:
                    OnOK(hwndDlg);
                    return TRUE;
                case IDCANCEL:
                    EndDialog(hwndDlg, 0);
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
    LPTEXTDATA pData = (LPTEXTDATA) lParam;
    SetWindowLong(hDlg, GWL_USERDATA, (LONG) pData);

    // Set message text.
    HWND hCtrl = GetDlgItem(hDlg, IDC_MESSAGE);
    SetWindowText(hCtrl, pData->lpszPrompt);

    // Set edit control text
    hCtrl = GetDlgItem(hDlg, IDC_EDIT);
    SetWindowText(hCtrl, pData->lpszSeedText);

    // Select all text
    Edit_SetSel(hCtrl, 0, -1);

}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void OnOK( HWND hDlg )
{
    // Get pointer to dialog data 
    LPTEXTDATA pData = (LPTEXTDATA) GetWindowLong(hDlg, GWL_USERDATA);

    // Get data from text control.
    HWND hCtrl = GetDlgItem(hDlg, IDC_EDIT);
    GetWindowText( hCtrl, pData->lpszPutHere, pData->nMaxLength );

    EndDialog(hDlg, 0);
}
