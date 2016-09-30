///////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/CALLBACK/VCS/msgbxw32.cpv   1.0   18 Jun 1997 13:16:36   MKEATIN  $
///////////////////////////////////////////////////////////////////////////////////
//
// Msgbox.cpp - Contains implementation for generic NAVEXMessageBox() callback.
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/CALLBACK/VCS/msgbxw32.cpv  $
// 
//    Rev 1.0   18 Jun 1997 13:16:36   MKEATIN
// Initial revision.
// 
//    Rev 1.0   17 Jun 1997 15:08:42   MKEATIN
// Initial revision.
// 
//    Rev 1.0   31 May 1997 15:52:46   MKEATIN
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

typedef struct INIT_DATA {
        LPTSTR                              lpszMessage;
        DWORD                               dwButtonFlags;
        LPDWORD                             lpdwResult;
        LPDWORD                             lpdwButtons;
        int                                 iButtonCount;
    } INITDATA, *LPINITDATA;

#define MAX_BUTTONS     8


///////////////////////////////////////////////////////////////////////////////////
// Local Functions

BOOL CALLBACK MessageBoxDlgProc(    HWND hwndDlg,   // handle to dialog box 
                                    UINT uMsg,      // message 
                                    WPARAM wParam,  // first message parameter 
                                    LPARAM lParam   // second message parameter 
                                ); 

static void OnInitDialog(HWND hDlg, LPARAM lParam);
static void HandleButtonPush(HWND hDlg, UINT uCommand);
static void SetupButtons(HWND hDlg, LPINITDATA pData);
static void RearrangeDialog( HWND hDlg, LPINITDATA pData );
static void CenterRow( HWND hDlg, int iStartCtrl, int iCount);

///////////////////////////////////////////////////////////////////////////////////
// Local Data

static DWORD aFlags[] = {   
                            BUTTON_FLAG_OK,        
                            BUTTON_FLAG_CANCEL,    
                            BUTTON_FLAG_CONTINUE,  
                            BUTTON_FLAG_YES,       
                            BUTTON_FLAG_NO,        
                            BUTTON_FLAG_PREVIOUS,  
                        };

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

UINT WINAPI NAVEXMessageBox32 
(
    DWORD   dwBoxID,
    LPTSTR  lpszMessage,
    DWORD   dwButtonFlags,
    LPDWORD lpdwResult,
    DWORD   dwMSTimeOut,
    LPVOID  lpvCookie
)
{
    // Validate input.
    if( (lpszMessage == NULL) ||
        (lpvCookie == NULL) ||
        (dwButtonFlags & BUTTON_INVALID_MASK) )
        return NAVEX_UI_ERROR;

    // Save off parent window.  This will have to be changed.
    HWND hParent = (HWND)lpvCookie;
    
        // Pack up initialization data for dialog.
    INITDATA initData;
    ZeroMemory(&initData, sizeof(INITDATA));
    initData.lpszMessage = lpszMessage;
    initData.dwButtonFlags = dwButtonFlags;
    initData.lpdwResult = lpdwResult;

    // Allocate array of DWORDs for button IDs and flags
    initData.lpdwButtons = new DWORD[MAX_BUTTONS * 2];
    ZeroMemory( initData.lpdwButtons, sizeof(DWORD) * MAX_BUTTONS * 2);
    
    // Fire off the dialog
    int iRet = DialogBoxParam( hInstance, 
                        MAKEINTRESOURCE(IDD_MESSAGE_BOX),
                        hParent,
                        (DLGPROC)MessageBoxDlgProc,
                        (LPARAM) &initData );

    // Cleanup
    delete [] initData.lpdwButtons;
    
    // Save off return value.
    *lpdwResult = iRet;
    return NAVEX_UI_OK;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK MessageBoxDlgProc(    HWND hwndDlg,   // handle to dialog box 
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
                case IDC_BUTTON1:
                case IDC_BUTTON2:
                case IDC_BUTTON3:
                case IDC_BUTTON4:
                case IDC_BUTTON5:
                case IDC_BUTTON6:
                case IDC_BUTTON7:
                case IDC_BUTTON8:
                    HandleButtonPush(hwndDlg, GET_WM_COMMAND_ID(wParam, lParam));
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
    LPINITDATA pData = (LPINITDATA) lParam;
    SetWindowLong(hDlg, GWL_USERDATA, (LONG) pData);

    // Set text in control to message text.
    HWND hCtrl = GetDlgItem(hDlg, IDC_TEXT);
    SetWindowText( hCtrl, pData->lpszMessage );

    // Figure out which buttons need to be displayed and
    // display them.
    SetupButtons( hDlg, pData );

    // Rearrange dialog.
    RearrangeDialog( hDlg, pData );

}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void SetupButtons(HWND hDlg, LPINITDATA pData)
{
    auto    int     i;
    auto    int     iButtonIndex = 0;
    auto    int     iOKPosition = -1;
    auto    int     iCancelPosition = -1;

    // How many buttons do we have?  Count the number of bits set
    pData->iButtonCount = 0;
    for( i = 0; i < sizeof(DWORD) * 8; i++)
        {
        if( pData->dwButtonFlags & (1 << i) )
            pData->iButtonCount++;
        }

    // Set up array.
    DWORD dwTempFlags = pData->dwButtonFlags;    
    for(i = 0; i < MAX_BUTTONS * 2; i += 2)
        {
        // Save off control ID.
        pData->lpdwButtons[i] = IDC_BUTTON1 + iButtonIndex++;

        // Look up value for this flag.
        for(int j = 0; j < MAX_BUTTONS; j++)
            {
            if( dwTempFlags & aFlags[j] )
                {
                // Rember position for cancel button.
                if( aFlags[j] == BUTTON_FLAG_CANCEL )
                    iCancelPosition = j;
                
                // Save off flag value.
                pData->lpdwButtons[i + 1] = aFlags[j];
                
                // Turn off this bit.
                dwTempFlags &= ~aFlags[j];
                break;
                }
            }
        
        }

    // Need to do some processing on array here.  CANCEL should
    // be the last button.
    if( iCancelPosition != -1 )
        {
        i = (int) pData->lpdwButtons[iCancelPosition * 2];
        pData->lpdwButtons[iCancelPosition * 2] = pData->lpdwButtons[(pData->iButtonCount-1) * 2];
        pData->lpdwButtons[(pData->iButtonCount -1) * 2 ] = i;
        }

    // Set text in buttons and show them.
    HWND hCtrl;
    TCHAR szButtonName[MAX_PATH];
    int iStringID;
    for(i = 0; i < MAX_BUTTONS * 2; i += 2)
        {
        // Get control
        hCtrl = GetDlgItem( hDlg, (int) pData->lpdwButtons[i] );

        // Set text in button.
        switch( pData->lpdwButtons[i+1])
            {
            case BUTTON_FLAG_OK:         iStringID = IDS_OK;        break; 
            case BUTTON_FLAG_CANCEL:     iStringID = IDS_CANCEL;    break; 
            case BUTTON_FLAG_CONTINUE:   iStringID = IDS_CONTINUE;  break; 
            case BUTTON_FLAG_YES:        iStringID = IDS_YES;       break; 
            case BUTTON_FLAG_NO:         iStringID = IDS_NO;        break; 
            case BUTTON_FLAG_PREVIOUS:   iStringID = IDS_PREVIOUS;  break; 
            default:
                iStringID = 0;
            }

        // If we found a valid flag, set up the button
        if(iStringID)
            {
            LoadString(hInstance, iStringID, szButtonName, MAX_PATH);
            SetWindowText( hCtrl, szButtonName );
            ShowWindow( hCtrl, SW_SHOW );
            }

        }

}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void HandleButtonPush(HWND hDlg, UINT uCommand)
{
    int iDialogRetValue = NAVEX_UI_ERROR;    

    // Get pointer to dialog data.
    LPINITDATA pData = (LPINITDATA) GetWindowLong(hDlg, GWL_USERDATA);

    // Look up command in button array, and return what happened.
    for(int i = 0; i < MAX_BUTTONS * 2; i += 2)
        {
        if( pData->lpdwButtons[i] == uCommand )
            {
            iDialogRetValue = (int) pData->lpdwButtons[i+1];         
            break;
            }
        }

    // Kill the dialog and return.
    EndDialog(hDlg, iDialogRetValue);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void RearrangeDialog( HWND hDlg, LPINITDATA pData )
{
    // If there are 8 buttons, our work is done.    
    if( pData->iButtonCount == 8 )
        return;

    // Figure out the size of a button.
    RECT rButtonSize;
    HWND hCtrl = GetDlgItem( hDlg, IDC_BUTTON5 );
    GetClientRect(hCtrl, &rButtonSize );

    // If we only have 1 row of buttons, shrink dialog
    RECT windowRect;
    GetWindowRect( hDlg, &windowRect );
    if(pData->iButtonCount < 5)
        {
        // Find the top of one of the lower row buttons, and resize window
        RECT buttonRect = rButtonSize;
        MapWindowPoints( hCtrl, NULL, (LPPOINT)&buttonRect, 2 );
        SetWindowPos( hDlg, NULL, 
                        0,
                        0, windowRect.right - windowRect.left,
                        buttonRect.top - windowRect.top + GetSystemMetrics( SM_CYDLGFRAME ), 
                        SWP_NOMOVE | SWP_NOZORDER );
        
        }

    // Center top row of buttons.
    CenterRow( hDlg, IDC_BUTTON1, pData->iButtonCount);
   
    // Center bottom row of buttons if necessary...
    if( pData->iButtonCount > 4 )
        {
        CenterRow( hDlg, IDC_BUTTON5, pData->iButtonCount - 4);
        }


}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

static void CenterRow( HWND hDlg, int iStartCtrl, int iCount)
{
    // Only do one row at a time
    if( iCount > 4 )
        iCount = 4;
    
    // Compute gap between buttons.  Should be 6 dialog units.
    int iGap = (6 * LOWORD(GetDialogBaseUnits())) / 4;

    // Get window width.
    RECT windowRect;
    GetWindowRect(hDlg, &windowRect);
    int iWindowWidth = windowRect.right - windowRect.left;

    // Get size of a button
    RECT rButton;
    HWND hCtrl = GetDlgItem( hDlg, iStartCtrl );
    GetWindowRect(hCtrl, &rButton );
    int iWidth = rButton.right - rButton.left;
    int iHeight = rButton.bottom - rButton.top;
                  
    // Compute width of smallest rect that can contain all 
    // of the buttons in this row, along with gaps.
    int iTotalWidth = (iWidth * iCount) +   // buttons
                      (iGap * (iCount -1)); // gaps
    
    // Find position of first button.
    int xPos = (iWindowWidth - iTotalWidth) / 2 + windowRect.left;
    
    // Need to convert this position to client coordinates.
    POINT p;
    p.x = xPos;
    p.y = rButton.top;
    ScreenToClient(hDlg, &p);
    xPos = p.x;

    // Move all buttons in this row
    for(int i = iStartCtrl; i < iStartCtrl + iCount; i++)
        {
        hCtrl = GetDlgItem(hDlg, i);
        MoveWindow( hCtrl, xPos, p.y, iWidth, iHeight, TRUE );
        xPos += iWidth + iGap;
        }

}
