#include "platform.h"

#ifdef SYM_WIN
#include "ngui.h"
#include "ctl3d.h"
#include "navutil.h"
#include "resexts.h"

// ********************************************************************** //
// ****               LOCAL FUNCTION PROTOTYPES                      **** //
// ********************************************************************** //

BOOL SYM_EXPORT WINAPI YesNoDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    );

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
    (
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
    );

// ********************************************************************** //

BOOL SYM_EXPORT WINAPI AskYesNo (LPYESNOSTRUCT lpYesNo)
{
    extern      HINSTANCE       hInstance;
    auto        DLGPROC         lpfn;
    auto        int             nStatus;

    lpfn = (DLGPROC) MakeProcInstance((FARPROC)YesNoDlgProc, hInstance);
    nStatus = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_YESNO),
                             lpYesNo->hParent, lpfn, (LPARAM)lpYesNo);
    FreeProcInstance((FARPROC) lpfn);

    return((BOOL) nStatus);
}


// ************************************************************************ //

BOOL SYM_EXPORT WINAPI YesNoDlgProc (
    HWND    hDlg,
    UINT    message,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    #define MAX_MESSAGE_SIZE    256
    auto    LPYESNOSTRUCT   lpYesNo;
    auto    char            szMessage [ MAX_MESSAGE_SIZE ];
    auto    BOOL            bReturn = FALSE;

    switch(message)
        {
        case WM_CTLCOLOR:
            bReturn = ((BOOL)Ctl3dCtlColorEx(message, wParam, lParam));
            break;
        case WM_INITDIALOG:
            lpYesNo = (LPYESNOSTRUCT)lParam;
            LP_SET ( hDlg, lpYesNo );

            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_CenterDialog (hDlg, lpYesNo->hParent);

            NG_SetFontsInDialog (hDlg, 0);
            NG_SetGroupBoxSunken(hDlg, ID_YESNO_BOX);

#ifdef NAVSCANW
            NG_SetChildSmallFont (hDlg, IDOK);
            NG_SetChildSmallFont (hDlg, IDCANCEL);
            NG_SetChildSmallFont (hDlg, IDHELP);
#endif                                  // NAVSCANW

                                        // ---------------------------------
                                        // Set Windows caption bar
                                        // ---------------------------------
            if ( lpYesNo->uTitleID )
                {
                LoadString (lpYesNo->hInstance, lpYesNo->uTitleID,
                            szMessage, sizeof(szMessage));

                SetWindowText (hDlg, szMessage);
                }

                                        // ---------------------------------
                                        // Set YES Button
                                        // ---------------------------------
            if ( lpYesNo->uYesButtonID )
                {
                LoadString (lpYesNo->hInstance, lpYesNo->uYesButtonID,
                            szMessage, sizeof(szMessage));

                Button_SetText (GetDlgItem (hDlg, IDOK), szMessage);
                }

                                        // ---------------------------------
                                        // Set NO Button
                                        // ---------------------------------
            if ( lpYesNo->uNoButtonID )
                {
                LoadString (lpYesNo->hInstance, lpYesNo->uNoButtonID,
                            szMessage, sizeof(szMessage));

                Button_SetText (GetDlgItem (hDlg, IDCANCEL), szMessage);
                }

                                        // ---------------------------------
                                        // Enable/disable buttons
                                        // ---------------------------------
            Button_Enable(GetDlgItem(hDlg,IDOK),!(lpYesNo->bYesDisabled));

                                        // Don't allow both to be disabled
            if (lpYesNo->bNoDisabled && !(lpYesNo->bYesDisabled))
                Button_Enable(GetDlgItem(hDlg,IDCANCEL), FALSE);
            else
                Button_Enable(GetDlgItem(hDlg,IDCANCEL), TRUE);

                                        // ---------------------------------
                                        // Set Static Message
                                        // ---------------------------------
            if ( lpYesNo->uQuestionID )
                {
                LoadString (lpYesNo->hInstance, lpYesNo->uQuestionID,
                            szMessage, sizeof(szMessage));

                Static_SetText (GetDlgItem (hDlg, ID_YESNO_MSG), szMessage);
                }

            bReturn = TRUE;
            break;

        case WM_COMMAND:
            {
            lpYesNo = (LPYESNOSTRUCT)LP_GET(hDlg);
            SYM_ASSERT ( lpYesNo );

            switch (GET_WM_COMMAND_ID(wParam,lParam))
                {
                case IDOK:
                    EndDialog(hDlg, TRUE);
                    break;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    break;

                case IDHELP:
                    if ( lpYesNo->uHelpBtnID )
                        {
                        StdHelpFromDialog(
                                        hDlg,
                                        lpYesNo->uHelpBtnID
                                        );
                        }
                    break;
                }
            }
            break;

        case WM_DESTROY:
            LP_FREE(hDlg);
            break;

#ifdef SYM_WIN32
        case WM_HELP:
        case WM_CONTEXTMENU:
                                        // ----------------------------------
                                        // Bring up right click help menu.
                                        // or standard context sensitive help
                                        // ----------------------------------
            OnContextMenu ( hDlg, message, wParam, lParam );
            bReturn = TRUE;
            break;
#endif
        }

    return ( bReturn );
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextMenu (
            HWND    hDlg,
            UINT    uMsg
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU windows
message. The purpose of this function is to call the standard symantec help
routines that display an appropiate floating help menu.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextMenu                // Respond to WM_CONTEXTMENU message
(
    HWND    hDlg,                       // [in] dialog that sent the message.
    UINT    uMsg,
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
)
{
    auto    LPYESNOSTRUCT   lpYesNo = (LPYESNOSTRUCT)LP_GET(hDlg);

    SYM_ASSERT ( lpYesNo );

    if ( lpYesNo && lpYesNo->lpdwWhatsThisArray )
        {
        NavDlgOnContextHelp(                /* Invoke Common Context Help Handling*/
                        hDlg,
                        uMsg,
                        wParam,
                        lParam,
                        lpYesNo->lpdwWhatsThisArray,
                        NULL,
                        NULL,           /* This dlg. does not have any LB Ctrl*/
                        NULL            /* that's why these nulls             */
                        );                                                     
        }
}

#endif


#endif
