#include "platform.h"
#include "virusdbf.h"

#include "vlist.h"

extern HINSTANCE hInstance;

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI ResetVirusInfoContents ( // Reset contents of virus info
    HWND    hDlg )                              // handle to dialog box.
{
    auto    BOOL    bStatus = FALSE;

    SYM_ASSERT ( hDlg );
    LPVIRUSDBF lpVir = (LPVIRUSDBF)GetWindowLong ( hDlg, DWL_USER );
    SYM_ASSERT ( lpVir );

    if ( lpVir )
        {
        SetCursor ( LoadCursor ( NULL, IDC_WAIT ) );
        DoOnDestroy ( hDlg );

        lpVir->lpDeleted = new CStackObjects(sizeof(HVLVIRUSENTRY));
        SYM_ASSERT ( lpVir->lpDeleted );

        lpVir->hContext = InitVirusLibrary ( lpVir->VirList.szDatFile, 
                                             lpVir->VirList.szInfFile );

        if ( lpVir->hContext )
            {
            if ( lpVir->uActiveEntry != VIRUSTYPE_ALL )
                {
                ComboBox_SetCurSel ( GetDlgItem(hDlg, IDI_VL_COMBOX), 0 );
                lpVir->uActiveEntry = VIRUSTYPE_ALL;
                }

            if ( GetVirusHandles ( lpVir ) == TRUE )
                {

                                        // --------------------------------
                                        // Set the number of entries in this 
                                        // listbox
                                        // --------------------------------
                    TCHAR szWork    [ SYM_MAX_PATH ];
                    TCHAR szMessage [ SYM_MAX_PATH ];

                    LoadString (hInstance, IDS_VIRUSDEF, szMessage, sizeof(szMessage) );
                    SPRINTF ( szWork, szMessage, ACTIVELISTBOXENTRIES(lpVir) );
                    Static_SetText (GetDlgItem(hDlg, IDI_VL_TEXT1), szWork);

                    VListBox_SetCurSel( ACTIVELISTBOX(lpVir), 0 );
                    SetFocus ( ACTIVELISTBOX(lpVir) );

                                        // --------------------------------
                                        // Display definitions date.
                                        // --------------------------------
                    DisplayDefinitionDate ( hDlg, lpVir );

                    SetCursor ( LoadCursor ( NULL, IDC_ARROW ) );
                    bStatus = TRUE;
                }
            else
                {
                SetCursor ( LoadCursor ( NULL, IDC_ARROW ) );
                ErrorHandler ( hDlg, VERR_INTERNAL );
                }
            }
        else
            {
            SetCursor ( LoadCursor ( NULL, IDC_ARROW ) );
            ErrorHandler ( hDlg, VERR_DATAFILES );
            }
        }
    else
        ErrorHandler ( hDlg, VERR_WRONGPARENT );

    return ( bStatus );
}
