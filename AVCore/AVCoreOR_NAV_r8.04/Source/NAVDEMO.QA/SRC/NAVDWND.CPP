//////////////////////////////////////////////////////////////////////////
// Copyright 1996 Symantec Corp. / Peter Norton Product Group
//////////////////////////////////////////////////////////////////////////
//************************************************************************
//
// File:
//      navdwnd.cpp
//
// Description:
//      Window-related functions and dialog box procedures for the NAV SDK
//      3.1 Demo.
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Include files

#include "windows.h"
#include "windowsx.h"

#include "navsdk.h"

#include "navdemo.h"
#include "navdwnd.h"
#include "resource.h"

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
// External declarations for global variables

extern HINSTANCE g_hInstance;           // the application's instance
extern HWND      g_hMainWnd;            // handle of the main window
extern HWND      g_hOutputListbox;      // handle of the output listbox
extern HWND      g_hWndProgressDlg;     // handle of the memory scanning
                                        // progress dialog

extern HVIRUSSCANNER g_hScannerHandle;  // NAV scanner handle


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoCreateOutputListbox ( HWND hWnd )
//
// Description:
//      Creates a listbox in the main window, where the demo app's output
//      messages will be displayed.
//
// Input:
//      hWnd:  Handle of the app's main window.
//
// Returns:
//      Nothing.
//
//////////////////////////////////////////////////////////////////////////

void NavDemoCreateOutputListbox ( HWND hWnd )
{
    RECT rClient;

    GetClientRect ( hWnd, &rClient );

    g_hOutputListbox = CreateWindow ( "LISTBOX",
                                      "",
                                      WS_CHILD |
                                        WS_VISIBLE |
                                        WS_VSCROLL |
                                        WS_HSCROLL |
                                        LBS_NOINTEGRALHEIGHT |
                                        LBS_HASSTRINGS |
                                        LBS_NOTIFY,
                                      rClient.left,
                                      rClient.top,
                                      rClient.right - rClient.left,
                                      rClient.bottom - rClient.top,
                                      hWnd,
                                      (HMENU) IDL_OUTPUT_LISTBOX,
                                      g_hInstance,
                                      NULL);

}   // NavDemoCreateOutputListbox()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoResizeOutputListbox ( void )
//
// Description:
//      Resizes the output listbox so that it always fills the main
//      window's client area.
//
// Input:
//      Nothing.
//
// Returns:
//      Nothing.
//
//////////////////////////////////////////////////////////////////////////

void NavDemoResizeOutputListbox ( void )
{
    RECT    rClient;
    LRESULT lCount;

    GetClientRect ( g_hMainWnd, &rClient );

    SetWindowPos ( g_hOutputListbox,
                   NULL,
                   rClient.left,
                   rClient.top,
                   rClient.right - rClient.left,
                   rClient.bottom - rClient.top,
                   SWP_NOZORDER | SWP_SHOWWINDOW );

                                        // scroll the listbox all the
                                        // way to the bottom
    lCount = SendMessage ( g_hOutputListbox,
                           LB_GETCOUNT,
                           0,
                           0L );

    if ( lCount > 0 )
        {
        SendMessage ( g_hOutputListbox,
                      LB_SETTOPINDEX,
                      (WPARAM) ( lCount - 1 ),
                      0L );
        }

}   // NavDemoResizeOutputListbox()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      NavDemoHandleInfectionDlgProc ()
//
// Description:
//      Dialog procedure for the NAVDEMO_HANDLE_INFECTION dialog box.
//      Use DialogBoxParam() when invoking this dialog, and send the
//      name of the infected file as the extra parameter.
//
// NAV SDK functions used:
//      VirusLibraryDeleteFile()
//      VirusLibraryGetLastVirusInfo()
//
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK NavDemoHandleInfectionDlgProc ( HWND   hWndDlg,
                                              UINT   uMsg,
                                              WPARAM wParam,
                                              LPARAM lParam )
{
static LPSTR s_szFile;                  // the file that will be operated on

BOOL bRetVal;


    switch ( uMsg )
        {
        case WM_INITDIALOG:
            {
                                        // The lParam to this message is the
                                        // name of the infected file, which
                                        // we'll save for later use.
            s_szFile = (LPSTR) lParam;

            SetDlgItemText ( hWndDlg, IDT_FILENAME, s_szFile );

            bRetVal = TRUE;             // Let windows set the initial focus
            }
        break;


        case WM_COMMAND:
            {
            switch ( GET_WM_COMMAND_ID ( wParam, lParam ) )
                {
                case IDB_DELETE:
                    {
                    switch ( VirusLibraryDeleteFile ( g_hScannerHandle,
                                                      s_szFile ) )
                        {
                        case VIRLIB_OK:
                            {
                            MessageOutput ( "The file was successfully deleted." );
                            }
                        break;

                        case VIRLIB_ERROR:
                            {
                            MessageOutput ( "An error occured while deleting the file." );
                            }
                        break;

                        case VIRLIB_INFECTED:
                            {
                            MessageOutput ( "This file cannot be safely deleted." );
                            }
                        break;

                        default:
                            {
                            MessageOutput ( "Unknown value returned by VirusLibraryDeleteFile()." );
                            }
                        break;
                        }   // end switch

                    bRetVal = TRUE;     // we processed this message

                    EndDialog ( hWndDlg, 0 );
                    }   // end case IDB_DELETE
                break;


                case IDB_VIEWINFO:
                    {
                    VIRUSINFO rVirusInfo;

                    if ( VIRLIB_OK == VirusLibraryGetLastVirusInfo (
                                          g_hScannerHandle,
                                          (LPVIRUSINFO) &rVirusInfo ) )
                        {
                                        // Display the virus info screen.
                        DialogBoxParam ( g_hInstance,
                                         "NAVDEMO_VIRUS_INFO",
                                         hWndDlg,
                                         (DLGPROC) NavDemoVirusInfoDlgProc,
                                         (LPARAM) &rVirusInfo );
                        }   // end if
                    else
                        {
                        MessageBox ( hWndDlg,
                                     "Error getting virus information.",
                                     "NAV SDK 2.0 Demo",
                                     MB_ICONEXCLAMATION | MB_OK );
                        }

                    bRetVal = TRUE;     // we processed this message
                    }   // end case IDB_VIEWINFO
                break;

                case IDB_NOACTION:
                    {
                    bRetVal = TRUE;     // we processed this message

                    EndDialog ( hWndDlg, 0 );
                    }
                break;  // end case IDB_NOACTION

                default:
                    {
                    bRetVal = FALSE;    // a message that we don't process
                    }
                break;
                }   // end switch

            }   // end case WM_COMMAND
        break;

        default:
            {
            bRetVal = FALSE;            // we won't process this message
            }
        break;

        }   // end switch

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
// This macro is used to set text fields in the virus info dialog box.

#define SetVirusCharacteristic(n) \
    SetDlgItemText ( hWndDlg, nResourceIDs[(n)],  \
                     ( prInfo->dwVirusFlags & (wVirinfoFlags[(n)]) ) ? "Yes" : "No" );


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      NavDemoVirusInfoDlgProc ()
//
// Description:
//      Dialog procedure for the NAVDEMO_VIRUS_INFO dialog box.
//      Use DialogBoxParam() when invoking this dialog, and send the
//      address of a VIRUSINFO struct as the extra parameter.
//
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK NavDemoVirusInfoDlgProc ( HWND hWndDlg, UINT uMsg,
                                        WPARAM wParam, LPARAM lParam )
{
BOOL bRetVal = FALSE;

    switch ( uMsg )
        {
        case WM_INITDIALOG:
            {
            LPVIRUSINFO prInfo;          // virus info is in this struct
            int        nFlag;
            char       szWork [ 8 ];

            // Element #i in this array is the ID of the yes/no field that
            // corresponds to the VIRINFO_INFECT_* flag contained in
            // wVirinfoFlags[i].

            int nResourceIDs[] =
            {
                IDT_VIRINFO_FILES,
                IDT_VIRINFO_EXE,
                IDT_VIRINFO_COM,
                IDT_VIRINFO_SYS,
                IDT_VIRINFO_MBR,
                IDT_VIRINFO_FLOPPIES,
                IDT_VIRINFO_HDBOOT,
                IDT_VIRINFO_INWILD,
                IDT_VIRINFO_MEMRESIDENT,
                IDT_VIRINFO_SIZESTEALTH,
                IDT_VIRINFO_FULLSTEALTH,
                IDT_VIRINFO_TRIGGER,
                IDT_VIRINFO_ENCRYPTING,
                IDT_VIRINFO_POLYMORPHIC,
                IDT_VIRINFO_MULTIPARTITE
            };

            WORD wVirinfoFlags[] =
            {
                VIRINFO_INFECT_FILES,
                VIRINFO_INFECT_EXE,
                VIRINFO_INFECT_COM,
                VIRINFO_INFECT_SYS,
                VIRINFO_INFECT_MBR,
                VIRINFO_INFECT_FLOPPY,
                VIRINFO_INFECT_HD_BOOT,
                VIRINFO_INFECT_WILD,
                VIRINFO_INFECT_MEM_RES,
                VIRINFO_INFECT_SIZE_ST,
                VIRINFO_INFECT_FULL_ST,
                VIRINFO_INFECT_TRIGGER,
                VIRINFO_INFECT_ENCRYPT,
                VIRINFO_INFECT_POLYMOR,
                VIRINFO_INFECT_MULTIPRT
            };

            prInfo = (LPVIRUSINFO) lParam;


            SetDlgItemText ( hWndDlg, IDT_VIRINFO_VIRNAME,
                             prInfo->szVirusName );

            SetDlgItemText ( hWndDlg, IDT_VIRINFO_FILE,
                             prInfo->szInfectedFileName );


            sprintf ( szWork, "%#x", (int) prInfo->byInfectedPhysicalDrive );
            SetDlgItemText ( hWndDlg, IDT_VIRINFO_PHYSDRIVE,
                             szWork );

            if ( prInfo->byInfectedPartitionNumber == 0xFF )
                {
                SetDlgItemText ( hWndDlg, IDT_VIRINFO_PARTITION,
                                 "MBR" );
                }
            else
                {
                sprintf ( szWork, "%#x", prInfo->byInfectedPartitionNumber );
                SetDlgItemText ( hWndDlg, IDT_VIRINFO_PARTITION,
                                 szWork );
                }

            *szWork = (char) prInfo->byInfectedLogicalDrive;
            szWork[1] = '\0';

            SetDlgItemText ( hWndDlg, IDT_VIRINFO_LOGICAL_DRIVE,
                             szWork );

            SetDlgItemText ( hWndDlg, IDT_VIRINFO_SIZE, prInfo->szVirusSize );

            SetDlgItemText ( hWndDlg, IDT_VIRINFO_REPAIRABLE,
                             ( prInfo->dwVirusFlags & VIRINFO_CAN_REPAIR ) ?
                                 "This virus can be repaired." :
                                 "This virus cannot be repaired." );

            SetDlgItemText ( hWndDlg, IDT_VIRINFO_DESCRIPTION,
                             prInfo->szVirusDescription );

            for ( nFlag = 0; nFlag < 15; nFlag++ )
                {
                SetVirusCharacteristic( nFlag );
                }

            bRetVal = TRUE;
            }
        break;


        case WM_COMMAND:
            {
            switch ( GET_WM_COMMAND_ID ( wParam, lParam ) )
                {
                case IDOK:
                    {
                    EndDialog ( hWndDlg, TRUE );
                    bRetVal = TRUE;
                    }
                break;
                }
            }
        break;

        case WM_CLOSE:
            {
            EndDialog ( hWndDlg, TRUE );
            bRetVal = TRUE;
            }
        break;
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      NavDemoMemScanProgressDlgProc ()
//
// Description:
//      Dialog procedure for the NAVDEMO_MEMSCAN_PROGRESS dialog box.
//
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK NavDemoMemScanProgressDlgProc ( HWND   hWndDlg,
                                              UINT   uMsg,
                                              WPARAM wParam,
                                              LPARAM lParam )
{
BOOL bRetVal = FALSE;

    switch ( uMsg )
        {
        case WM_INITDIALOG:
            {
            SetDlgItemInt ( hWndDlg,
                            IDT_PROGRESS_PERCENT,
                            0,
                            FALSE );

            bRetVal = TRUE;             // Let windows set the initial focus
            }
        break;
        }

    return bRetVal;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//      NavDemoSysAreaPromptDlgProc ()
//
// Description:
//      Dialog procedure for the NAVDEMO_SYSAREA_PROMPT dialog box.
//      Use DialogBoxParam() when invoking this dialog, and send a
//      pointer to a SYSAREA_OPTS struct as the extra parameter.
//      This function returns TRUE to the code that created the dialog
//      if the user selects OK, or false if they select Cancel.
//
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK NavDemoSysAreaPromptDlgProc ( HWND   hWndDlg,
                                            UINT   uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
static LPSYSAREA_OPTS pOpts = NULL;
BOOL bRetVal = FALSE;


    switch ( uMsg )
        {
        case WM_INITDIALOG:
            {
            char    szWorkStr[] = " :\\", szDisplayStr[] = " :";
            int     nDrive;
            LRESULT lDefaultDriveIndex;
            UINT    uDriveType;

                                        // Store the address of our
                                        // options struct.
            pOpts = (LPSYSAREA_OPTS) lParam;


                                        // Initialize the combobox with
                                        // a list of local drives.
            for ( nDrive = 0; nDrive < 26; nDrive++ )
                {
                                        // Check if the next drive is either
                                        // a hard disk or a removable drive.
                *szWorkStr = nDrive + 'A';
                uDriveType = GetDriveType ( szWorkStr );

                if ( uDriveType == DRIVE_REMOVABLE ||
                     uDriveType == DRIVE_FIXED )
                    {
                    *szDisplayStr = nDrive + 'A';

                                        // Add the drive to the list of
                                        // drives.

                    SendDlgItemMessage ( hWndDlg,
                                         IDCB_DRIVES,
                                         CB_ADDSTRING,
                                         0,
                                         (LPARAM)(LPCSTR) szDisplayStr );
                    }
                }   // end for

        
                                        // Set the default drive to C:
            lDefaultDriveIndex = SendDlgItemMessage ( hWndDlg,
                                                      IDCB_DRIVES,
                                                      CB_FINDSTRING,
                                                      0,
                                                      (LPARAM)(LPCSTR) "C" );

            SendDlgItemMessage ( hWndDlg,
                                 IDCB_DRIVES,
                                 CB_SETCURSEL,
                                 (WPARAM) lDefaultDriveIndex,
                                 0L );

                                        // Set the default states of the
                                        // checkboxes & radio buttons.

            CheckDlgButton ( hWndDlg, IDC_ALL_FIXED, TRUE );
            CheckDlgButton ( hWndDlg, IDC_ALL_REMOVABLE, FALSE );
            CheckRadioButton ( hWndDlg, IDR_ONE_DRIVE,
                               IDR_MULTIPLE_DRIVES, IDR_ONE_DRIVE );

            EnableWindow ( GetDlgItem ( hWndDlg, IDC_ALL_FIXED ),
                           FALSE );
            EnableWindow ( GetDlgItem ( hWndDlg, IDC_ALL_REMOVABLE ),
                           FALSE );

            bRetVal = TRUE;
            }   // end case WM_INITDIALOG
        break;

        case WM_COMMAND:
            {
            switch ( GET_WM_COMMAND_ID ( wParam, lParam ) )
                {
                case IDR_ONE_DRIVE:
                    {
                                        // Grey out the multiple drive
                                        // scanning controls
                    EnableWindow ( GetDlgItem ( hWndDlg, IDC_ALL_FIXED ),
                                   FALSE );
                    EnableWindow ( GetDlgItem ( hWndDlg, IDC_ALL_REMOVABLE ),
                                   FALSE );

                                        // Enable the single drive scanning
                                        // controls.
                    EnableWindow ( GetDlgItem ( hWndDlg, IDT_ONE_DRIVE_TAG ),
                                   TRUE );
                    EnableWindow ( GetDlgItem ( hWndDlg, IDCB_DRIVES ),
                                   TRUE );

                                        // Always enable OK if the scan-
                                        // one-drive radio button is selected.
                    EnableWindow ( GetDlgItem ( hWndDlg, IDOK ),
                                   TRUE );


                    bRetVal = TRUE;
                    }
                break;

                case IDR_MULTIPLE_DRIVES:
                    {
                                        // Enable the multiple drive
                                        // scanning controls
                    EnableWindow ( GetDlgItem ( hWndDlg, IDC_ALL_FIXED ),
                                   TRUE );
                    EnableWindow ( GetDlgItem ( hWndDlg, IDC_ALL_REMOVABLE ),
                                   TRUE );

                                        // Grey out the single drive
                                        // scanning controls.
                    EnableWindow ( GetDlgItem ( hWndDlg, IDT_ONE_DRIVE_TAG ),
                                   FALSE );
                    EnableWindow ( GetDlgItem ( hWndDlg, IDCB_DRIVES ),
                                   FALSE );


                                        // Check if we need to disable
                                        // the OK button.  It will be
                                        // disabled if neither checkbox
                                        // is checked.

                    if ( IsDlgButtonChecked ( hWndDlg, IDC_ALL_FIXED ) ||
                         IsDlgButtonChecked ( hWndDlg, IDC_ALL_REMOVABLE ) )
                        {
                        EnableWindow ( GetDlgItem ( hWndDlg, IDOK ),
                                       TRUE );
                        }
                    else
                        {
                        EnableWindow ( GetDlgItem ( hWndDlg, IDOK ),
                                       FALSE );
                        }


                    bRetVal = TRUE;
                    }
                break;

                case IDC_ALL_FIXED:
                case IDC_ALL_REMOVABLE:
                    {
                                        // Disable the OK button if both
                                        // checkboxes are unchecked.
                    if ( IsDlgButtonChecked ( hWndDlg, IDC_ALL_FIXED ) ||
                         IsDlgButtonChecked ( hWndDlg, IDC_ALL_REMOVABLE ) )
                        {
                        EnableWindow ( GetDlgItem ( hWndDlg, IDOK ), TRUE );
                        }
                    else
                        {
                        EnableWindow ( GetDlgItem ( hWndDlg, IDOK ), FALSE );
                        }
                    }
                break;

                case IDCANCEL:
                    {
                    EndDialog ( hWndDlg, FALSE );
                    bRetVal = TRUE;
                    }
                break;

                case IDOK:
                    {
                    LRESULT lSelection;
                    char    szDrive [ 4 ];

                                        // Set the options flag that tells
                                        // the calling function whether
                                        // the user chose to scan one
                                        // drive or multiple drives.
                    pOpts->bOneDrive = IsDlgButtonChecked ( hWndDlg,
                                                            IDR_ONE_DRIVE );

                    if ( pOpts->bOneDrive )
                        {
                                        // Store the selected drive in
                                        // the options struct.
                        lSelection = SendDlgItemMessage ( hWndDlg,
                                                          IDCB_DRIVES,
                                                          CB_GETCURSEL,
                                                          0,
                                                          0L );

                        SendDlgItemMessage ( hWndDlg,
                                             IDCB_DRIVES,
                                             CB_GETLBTEXT,
                                             (WPARAM) lSelection,
                                             (LPARAM)(LPCSTR) szDrive );

                                        // Store the drive letter in the
                                        // struct. The calling function
                                        // will be responsible for storing
                                        // this in a BYTE.
                        pOpts->wDrive = *szDrive;
                        }
                    else
                        {
                                        // Store the appropriate
                                        // VIRLIB_*_FLAG value(s) in the
                                        // options struct, which the
                                        // calling function can pass on
                                        // to VirusLibraryScanSysArea().
                        pOpts->wDrive = 0;

                        if ( IsDlgButtonChecked ( hWndDlg,
                                                  IDC_ALL_FIXED ) )
                            {
                            pOpts->wDrive |= VIRLIB_FIXED_FLAG;
                            }

                        if ( IsDlgButtonChecked ( hWndDlg,
                                                  IDC_ALL_REMOVABLE ) )
                            {
                            pOpts->wDrive |= VIRLIB_REMOVABLE_FLAG;
                            }
                        }

                    bRetVal = TRUE;

                    EndDialog ( hWndDlg, TRUE);

                    }   // end case IDOK
                break;
                
                }   // end switch
            }   // end case WM_COMMAND
        break;

        }   // end switch


    return bRetVal;
}
