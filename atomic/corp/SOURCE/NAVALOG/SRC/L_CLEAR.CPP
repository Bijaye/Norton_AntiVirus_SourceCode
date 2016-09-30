//*********************************************************************
// Copyright 1993 Symantec, Peter Norton Group
//
// File: l_clear.c
//
// This file contains functions to display "clear" dialog box.
//
// See Also:
// Future Enhancements:
//*********************************************************************
// $Header:   S:/NAVALOG/VCS/l_clear.cpv   1.3   09 Jul 1998 17:51:26   DBuches  $
// 04/20/93 WRL is William R. Lee II (BLEE)
//            - Moved to Quake from NIRVANA:ACTILOG
//*********************************************************************
#include "actilogi.h"                   // Project specific dcls
#include "xapi.h"                       
#include "ctl3d.h"
#include "ngui.h"
#include "stdhelp.h"                    // IDHELP
#include "navutil.h"                    // HelpSetContext()
#include "navw_hlp.h"                   // Help ID's: HELP_DLG_ACTLOG_CLEAR
#include "actstb.h"
#include "actdlg.h"
#include "cmbstr.h"
#include "msgdisp.h"

//*********************************************************************
// - Local function prototypes
//*********************************************************************
BOOL SYM_EXPORT WINAPI ClearDlgProc (HWND, UINT, WPARAM, LPARAM);

#ifdef SYM_WIN32
VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(                                       
    HWND    hDlg,                       // [in] dialog that sent the ... 
    UINT    uMsg,						// [in] ... message
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
);                    
#endif

//** ClearLog() *******************************************************
//
// Future Enhancements:
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************
int SYM_EXPORT WINAPI ClearLog (HWND hDlg)
{
   int   nStatus;

   nStatus = DialogBox(hDLLInst, MAKEINTRESOURCE(IDD_CLEARLOG), hDlg,
                       ClearDlgProc);
   return (nStatus);
} // ClearLog()


//** ClearDlgProc() ***************************************************
//
// Future Enhancements:
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************

BOOL SYM_EXPORT WINAPI ClearDlgProc(
   HWND     hDlg,
   UINT     message,
   WPARAM   wParam,
   LPARAM   lParam
)
{
    switch(message)
        {
        case WM_CTLCOLOR:
            return ((BOOL)Ctl3dCtlColorEx(message, wParam, lParam));
        case WM_INITDIALOG:
            Ctl3dSubclassDlg(hDlg, CTL3D_ALL);
            NG_SetFontsInDialog (hDlg, 0);
#if defined(SYM_WIN32)
            if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
                {
                NG_SetChildSmallFont ( hDlg, IDYES );
                NG_SetChildSmallFont ( hDlg, IDNO );
                NG_SetChildSmallFont ( hDlg, 9 );
                }
#endif            
            NG_CenterDialog(hDlg, GetParent(hDlg));
            return (TRUE);

 /* 
        case WM_ACTIVATE:
            if ( GET_WM_ACTIVATE_STATE(wParam,lParam) )
                {
                StdHelpSetContext (HELP_DLG_ACTLOG_CLEAR,
                                    HELP_DLG_ACTLOG_CLEAR);
                }
            break;
  */

#if 0
												// --------------------------------	
												// bring up "WhatsThis" popup menu 
												// or invoke context sensitive help
												// --------------------------------
		case WM_HELP:							
		case WM_CONTEXTMENU:
			OnContextHelp(hDlg, message, wParam, lParam);	
			break;
#endif
        
        
        case WM_COMMAND:
            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
                {
                case IDYES:
                    EndDialog(hDlg, TRUE);
                    break;

                case IDNO:
                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    break;

                                        // -------------------------------------
										// Invoke the standard topic help
                                        // -------------------------------------
                case IDHELP:
                    StdHelpFromDialog ( hDlg, IDH_NAVW_CLEAR_ACTIVITY_LOG_DLG_HELP_BTN );                                    
                    break;
                }
            break;
        }

    return (FALSE);
} // ClearDlgProc()

//** ErrorHandler() ***************************************************
//
// Future Enhancements:
//*********************************************************************
// 04/20/93 WRL
//*********************************************************************
VOID ErrorHandler(HWND hDlg, UINT uStatus)
{
    static struct
        {
        UINT uError;
        UINT uString;
        } Error[] =
            {
                {0,                NULL}
            };

    UINT i;
    TCHAR szTitle [ SIZE_WORK+1 ];
    TCHAR szTemp  [ SIZE_WORK+1 ];

    for (i=0; Error[i].uError; i++)
        if (Error[i].uError == uStatus)
            break;

    if (!Error[i].uError)
        i = 0;

    LoadString ( hDLLInst, IDS_ERR_TITLE, szTitle, sizeof(szTitle) );
    LoadString ( hDLLInst, Error[i].uString, szTemp, sizeof(szTemp) );

    MessageDisplay (hDlg, szTemp, szTitle,
                    MB_ICONHAND | MB_OK, NULL, NULL);
}

#ifdef SYM_WIN32

/* **************************************************************************
@Name: VOID LOCAL OnContextHelp (
            HWND    hDlg,
            UINT    uMsg,
            WPARAM  wParam,
            LPARAM  lParam
            )

@Description:
This function is called when the dialog box receives a WM_CONTEXTMENU or WM_HELP
windows message. The purpose of this function is to call the standard symantec help
routines that display an appropiate: floating help menu or context sensitive help.

@Parameters:
$hDlg$  handle of the dialog box where the message was intercepted.

$uMsg$  actual message 

$wParam$    the wParam that was passed to WM_CONTEXTMENU message.

$lParam$    the lParam that was passed to WM_CONTEXTMENU message.

@Include: stdhelp.h
************************************************************************** */

VOID LOCAL OnContextHelp                // Respond to WM_CONTEXTMENU and WM_HELP
(                                       
    HWND    hDlg,                       // [in] dialog that sent the ... 
    UINT    uMsg,						// [in] ... message
    WPARAM  wParam,                     // [in] wParam from dialog
    LPARAM  lParam                      // [in] lParam from dialog
)                    
{

                                        // -----------------------------------
                                        // This array is a cross reference
                                        // between a control id number and a
                                        // help id number that should be
                                        // displayed when the user selects the
                                        // 'What's This' menu entry.
                                        // -----------------------------------
    static  DWORD       dwWhatsThisMap[] =
        {
        0,              0
        };

                                        // -----------------------------------
                                        // This array is a set of string id's
                                        // in our resources that are added to
                                        // the 'How To..." menu entry. When one
                                        // of the entries is selected by the
                                        // user the corresponding help is
                                        // displayed.
                                        // -----------------------------------

    static  DWORD      **pdwPickListMap = NULL;

    NavDlgOnContextHelp(                /* Invoke Common Context Help Handling     */
                        hDlg,                 
                        uMsg,
                        wParam,               
                        lParam,                
                        dwWhatsThisMap,       
                        NULL,
                        NULL,           /* This dlg. does not have any LB Ctrls -  */
                        NULL            /* - that's why these nulls ...            */ 
                       );
}
                 

#endif // ifdef w32 - only
