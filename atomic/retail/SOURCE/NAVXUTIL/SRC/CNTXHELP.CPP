// **************************************************************************
//
// FileName: CntxtHelp.Cpp
//
// Purpose : Implementation of default  handling of Context Sensitive Help
//           messages (WM_HELP and WM_CONTEXTMENU) sent to any NavW Dialog
//
// Originaly created :                 02-10-95,               by A. Matic
//
// Exports:
//
//    NavDlgOnContextHelp               (...)
//
//    NavPrnDlgSetContextHelpIds        (...)
//
// **************************************************************************


#include "platform.h"
#include "stdhelp.h"

#include "navutil.h"
#include "navw_hlp.h"
#include "navwstb.h"



#ifdef SYM_WIN32

// ***************************************************************************

VOID SYM_EXPORT WINAPI NavDlgOnContextHelp (
    HWND    hDlg,                       
    UINT    uMsg,    
    WPARAM  wParam,                     
    LPARAM  lParam,                     
    DWORD  *pdwarrWhatsThisMap,         
    DWORD  *pdwarrHowToMap,             
    DWORD  *pdwarrLBItemsHelpStrings,   
    LB_ITEMSELECTIONPROC lpfnGetSelItem
    )
{
    
    if ( pdwarrHowToMap )
        {
        HINSTANCE hResInstance = (HINSTANCE)GetWindowLong(hDlg, GWL_HINSTANCE);
        CHAR szBuffer[256];
        if (   LoadString( 
                          hResInstance, 
                          pdwarrHowToMap[0],  
                          szBuffer,
                          sizeof(szBuffer)
                         )
           )
            {                           // Res. is in inst where hwnd is made:
            HELPSTRUCT HelpInfoPrev, HelpInfo; 
            StdHelpExGetHelpInfo( &HelpInfoPrev );
            HelpInfo       = HelpInfoPrev;
            HelpInfo.hinst = hResInstance;
            HelpInfo.hwnd  = hDlg;
            StdHelpExInit ( &HelpInfo );
            StdHelpExOnContextHelp (hDlg,uMsg,wParam, lParam, pdwarrWhatsThisMap, 
                    pdwarrHowToMap, pdwarrLBItemsHelpStrings, lpfnGetSelItem );
            StdHelpExInit ( &HelpInfoPrev );
            }
        else                            // App is responsible - res. is in TASK
            {
            StdHelpExOnContextHelp (hDlg,uMsg,wParam, lParam, pdwarrWhatsThisMap, 
                    pdwarrHowToMap, pdwarrLBItemsHelpStrings, lpfnGetSelItem );
            }
        }
    else                                // Whatever ...                                
        {
        StdHelpExOnContextHelp (hDlg,uMsg,wParam, lParam, pdwarrWhatsThisMap, 
                    pdwarrHowToMap, pdwarrLBItemsHelpStrings, lpfnGetSelItem );
        }
}  
  


/* **************************************************************************
@Name: NavPrnDlgSetContextHelpIds( LPPRINTDLGSTRUCT  pPrintDlgStruct )

@Description:
Does NAV Common Printer dialog context sensitive help string settings.

$pPrintDlgStruct$ - addres of struct which describes pronter dialog properties.

@Returns:
NOTHING

@Include: quake's <npt.h>
***************************************************************************** */

VOID SYM_EXPORT NavPrnDlgSetContextHelpIds (
    LPPRINTDLGSTRUCT lpPrintDlgStruct
    )
{
#if 0
    lpPrintDlgStruct->wHelpID          = IDH_NAVW_PRINT_DLG_HELP_BTN; 
    
    lpPrintDlgStruct->wHelpIDBrowse    = IDH_NAVW_PRINT_TO_FILE_DLG_HELP_BTN;

                                        //
                                        // Printer's chk box and tunnel btn.
                                        //

    lpPrintDlgStruct->uHelpRightClickPrintPrinterButton      =  \
        lpPrintDlgStruct->uHelpRightClickPrintPrinterTunnel  =  \
                         IDH_NAVW_WHATHIS_PRINT_DLG_PRINTER_BTN;

                                        //
                                        // File's chk box and tunnel btn.
                                        //

    lpPrintDlgStruct->uHelpRightClickPrintFileButton         =  \
        lpPrintDlgStruct->uHelpRightClickPrintFileTunnel     =  \
                   IDH_NAVW_WHATHIS_PRINT_DLG_PRINT_TO_FILE_BTN;


                                        //
                                        // Send to user chk box
                                        //

    lpPrintDlgStruct->uHelpRightClickSendUserButton          =  \
            IDH_NAVW_WHATHIS_WHATHIS_PRINT_DLG_MAIL_TO_USER_BTN;
#endif

    lpPrintDlgStruct->wHelpID                           =
    lpPrintDlgStruct->wHelpIDBrowse                     =
    lpPrintDlgStruct->uHelpRightClickPrintPrinterButton =
    lpPrintDlgStruct->uHelpRightClickPrintPrinterTunnel =
    lpPrintDlgStruct->uHelpRightClickPrintFileButton    =
    lpPrintDlgStruct->uHelpRightClickPrintFileTunnel    =
    lpPrintDlgStruct->uHelpRightClickSendUserButton     = 0;

}
// ***************************************************************************

    #undef ResInstance

// ***************************************************************************

#endif // W32 only


// .eof.

