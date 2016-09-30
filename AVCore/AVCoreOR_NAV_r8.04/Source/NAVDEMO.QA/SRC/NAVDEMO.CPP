///////////////////////////////////////////////////////////////////////////////
// Copyright 1996-1997 Symantec Corp. / Peter Norton Product Group
///////////////////////////////////////////////////////////////////////////////
//************************************************************************
//
// File:
//      navdemo.cpp
//
// Description:
//      Sample application to demonstrate use of Virus Scanning API and
//      Virus Definitions API.
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Include files

#include <windows.h>
#include <windowsx.h>

#include "navsdk.h"
#include "navdefs.h"

#include "navdemo.h"
#include "navdwnd.h"
#include "resource.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////////
// Global variables

HINSTANCE g_hInstance;                  // the application's instance

HWND      g_hMainWnd;                   // handle of the main window
HWND      g_hOutputListbox;             // handle of the output listbox
HWND      g_hWndProgressDlg;            // handle of the memory scanning
                                        // progress dialog

HCURSOR   g_hArrowCursor;               // the normal arrow cursor
HCURSOR   g_hWaitCursor;                // the wait cursor

char      g_szAppID[MAX_PATH];          // Application ID string

HVIRUSSCANNER g_hScannerHandle;         // NAV scanner handle



//////////////////////////////////////////////////////////////////////////
// Local Function Prototypes

WORD NavDemoFakeInstall     (LPSTR  lpAppID, LPSTR  lpVirusDefDir);
BOOL NavDemoFakeUninstall   (LPSTR  lpAppID);



//////////////////////////////////////////////////////////////////////////
//
// Function:
//      WinMain
//
// Description:
//      Main entry point of the NAVDEMO program.  This program demonstrates
//      how to hook up scanning to your product.
//
//////////////////////////////////////////////////////////////////////////

int PASCAL WinMain ( HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpszCmdLine,
                     int       nCmdShow )
{
static char szAppName[] = "NavDemo";
static char szMenu[]    = "NavDemoMenu";
static char szIcon[]    = "NavDemoIcon";

HWND        hWnd;
MSG         msg;
WNDCLASS    rNavDemoClass;
HICON       hIcon;
int         nResult = 0;


                                        // Initialize global variables
    g_hInstance = NULL;
    g_hMainWnd = NULL;
    g_hOutputListbox = NULL;
    g_hWndProgressDlg = NULL;

    g_hScannerHandle = NULL;

    g_hArrowCursor = LoadCursor ( NULL, IDC_ARROW );
    g_hWaitCursor = LoadCursor ( NULL, IDC_WAIT );


    if ( !hPrevInstance )
        {
                                        // Register the window class used
                                        // by our application.
        hIcon = LoadIcon ( hInstance, szIcon );

        rNavDemoClass.style         = CS_HREDRAW | CS_VREDRAW;
        rNavDemoClass.lpfnWndProc   = MainWndProc;
        rNavDemoClass.cbClsExtra    = 0;
        rNavDemoClass.cbWndExtra    = 0;
        rNavDemoClass.hInstance     = hInstance;
        rNavDemoClass.hIcon         = hIcon;
        rNavDemoClass.hCursor       = NULL;
        rNavDemoClass.hbrBackground = (HBRUSH) GetStockObject ( WHITE_BRUSH );
        rNavDemoClass.lpszMenuName  = szMenu;
        rNavDemoClass.lpszClassName = szAppName;

        RegisterClass ( &rNavDemoClass ) ;


                                        // Create the window for our
                                        // demo application.  The virus
                                        // library is initialized in the
                                        // WM_CREATE handler that this
                                        // call generates.

        g_hInstance = hInstance;

        hWnd = CreateWindow ( szAppName,
                              "Norton AntiVirus SDK Version 3.1 Demo",
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              NULL,
                              NULL,
                              hInstance,
                              NULL );

        if ( hWnd == NULL )
            {
                                        // We could not create the window
                                        // and/or init the library.
            MessageBox ( GetDesktopWindow(),
                         "Could not initialize the application.\nVerify that all NAVSDK files are installed correctly\nand are in the same directory as the demo app.",
                         "NAV SDK 3.1 Demo",
                         MB_OK | MB_ICONSTOP );

            return 0;
            }

                                        // Display the window as we were
                                        // told to upon startup.
        ShowWindow ( hWnd, nCmdShow );
        ShowWindow ( g_hOutputListbox, SW_MAXIMIZE );
        UpdateWindow(g_hOutputListbox);
        UpdateWindow ( hWnd );


        g_hMainWnd = hWnd;


        MessageOutput ( "NAV SDK 3.1 has been initialized." );


                                        // Process all messages for this
                                        // application.
        while ( GetMessage ( &msg, NULL, 0, 0 ) )
            {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
            }

                                        // Clean up before exiting.
        if ( NULL != hIcon )
            {
            DestroyIcon ( hIcon );
            }

        nResult = msg.wParam;
        }

    return nResult;

}   // WinMain()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      LRESULT WINAPI MainWndProc ( HWND   hWnd,
//                                   UINT   uMessage,
//                                   WPARAM wParam,
//                                   LPARAM lParam   )
//
// Description:
//      This is the main window procedure for NAVDEMO.
//
// Input:
//      hWnd:     Window handle
//      uMessage: Message to process
//      wParam:   wParam of message
//      lParam:   lParam of message
//
// Returns:
//      LRESULT based on message handling.
//
//////////////////////////////////////////////////////////////////////////

LRESULT WINAPI MainWndProc ( HWND   hWnd,
                             UINT   uMessage,
                             WPARAM wParam,
                             LPARAM lParam   )
{
LRESULT lResult;
                                        // Handle messages for the main
                                        // program window.
    switch ( uMessage )
        {
                                        // WM_CREATE:  The window has been
                                        // created, initialize our virus
                                        // engine component.
        case WM_CREATE:
            {
            lResult = HANDLE_WM_CREATE ( hWnd,
                                         wParam,
                                         lParam,
                                         NavDemoWM_CREATE );

            NavDemoResizeOutputListbox();
            }
        break;

                                        // WM_COMMAND:  Handle user commands.
        case WM_COMMAND:
            {
            HANDLE_WM_COMMAND ( hWnd,
                                wParam,
                                lParam,
                                NavDemoWM_COMMAND );


            lResult = 0;                // Tell Win that we processed the
                                        // command message.
            }
        break;

                                        // WM_SIZE:  Resize the output
                                        // listbox as well.
        case WM_SIZE:
            {
            NavDemoResizeOutputListbox ();

            lResult = 0;                // Tell Win that we processed the
                                        // command message.
            }
        break;


                                        // WM_DESTROY:  Clean up and close
                                        // the virus engine.
        case WM_DESTROY:
            {
            lResult = HANDLE_WM_DESTROY ( hWnd,
                                          wParam,
                                          lParam,
                                          NavDemoWM_DESTROY );
            }
        break;

                                        // default:  Let windows handle it.
        default:
            {
            lResult = DefWindowProc ( hWnd, uMessage, wParam, lParam );
            }
        break;
        }

    return lResult;

}   // MainWndProc()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      BOOL NavDemoWM_CREATE ( HWND hWnd,
//                              CREATESTRUCT FAR* lpCreate )
//
// Description:
//      This routine handles the WM_CREATE message for the NAVDEMO program.
//      It initializes the virus library so that the demo application can
//      scan for viruses.  The virus definition files are installed from the
//      VIRUSDEF directory immediately below the directory where NAVDEMO resides.
//
// Input:
//      hWnd:      Handle of the window being created.
//      lpCreate:  Create structure.
//
// Returns:
//      TRUE  if the virus library was initialized.
//      FALSE if there was an error initializing library.  The app
//            cannot continue if the library could not be initialized.
//
//
// NAV SDK functions used:
//      VirusLibraryInit()
//
//////////////////////////////////////////////////////////////////////////

BOOL NavDemoWM_CREATE ( HWND              hWnd,
                        CREATESTRUCT FAR* lpCreate )
{
BOOL          bRetVal;
VIRUSLIBINIT  rInit;
char          szVirusDefDirectory [ MAX_PATH ] = {0}, *szDirPtr;

                                        // Initialize the virus library
                                        // initialization struct.

    GetModuleFileName ( g_hInstance,
                        szVirusDefDirectory,
                        sizeof ( szVirusDefDirectory ) );

    szDirPtr = strrchr ( szVirusDefDirectory, '\\' );

    if ( szDirPtr == NULL )
        return FALSE;

                                        // The we'll look for the virus def in
                                        // defs subdirectory below where NAVDEMO is
                                        // running from.
    strcpy ( szDirPtr,"\\VIRUSDEF\\" );

                                        // Install the virus definition files.

    if ( NavDemoFakeInstall ( &rInit.szAppID[0], szVirusDefDirectory ) != 0 )
        return FALSE;

    strcpy ( &g_szAppID[0], &rInit.szAppID[0] );

    rInit.Progress = NavDemoMemScanCallback;
	rInit.bDisableNAVAPBySession = TRUE;
    rInit.bManageVirusDefs = TRUE;
    rInit.dwInterfaceVersion = VIRLIB_INTERFACE_VERSION;

    if ( VirusLibraryInit ( (LPVIRUSLIBINIT) &rInit ) == VIRLIB_OK )

        {
                                        // Store the virus engine context
                                        // for later reference.

        g_hScannerHandle = rInit.hScannerHandle;

                                        // Set the initial shape of the
                                        // cursor.
        SetCursor ( g_hArrowCursor );

        bRetVal = TRUE;                 // Everything was OK.
        }
    else
        {
        bRetVal = FALSE;                // The virus library could not be
                                        // initialized, so return FALSE so
                                        // that WinMain() exits.
        }


                                        // Create a listbox in the main
                                        // window, where the demo app's
                                        // output messages will be displayed
    NavDemoCreateOutputListbox ( hWnd );


    return bRetVal;

}   // NavDemoWM_CREATE()



//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoWM_DESTROY ( HWND hWnd )
//
// Description:
//      Handles the WM_DESTROY message for the NAVDEMO application.  This
//      function also closes our scanner handle.
//
// Input:
//      hWnd: Handle of window being destroyed.
//
// Returns:
//      Nothing.
//
//
// NAV SDK functions used:
//      VirusLibraryClose()
//
//////////////////////////////////////////////////////////////////////////

void NavDemoWM_DESTROY ( HWND hWnd )
{
                                        // Get our context that we stored
                                        // when we initialized the library.
                                        // If it's a valid handle, deinit
                                        // and unload.
    if ( g_hScannerHandle != NULL )
        {
        VirusLibraryClose ( g_hScannerHandle );
        }

                                        // Uninstall NAVDEMO as a user of
                                        // the virus defs.

    NavDemoFakeUninstall ( &g_szAppID[0] );

                                        // Terminate the application.
    PostQuitMessage ( 0 );

}    // NavDemoWM_DESTROY()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//       void NavDemoWM_COMMAND ( HWND hWnd,
//                                int  nID,
//                                HWND hWndCtl,
//                                UINT uNotify )
//
// Description:
//      This routine handles the WM_COMMAND message for the NAVDEMO app.
//      The user may select to specify a file to be scanned or to quit
//      the application.
//
// Input:
//      hWnd:     Main window handle
//      nID:      Control or menu ID
//      hWndCtl:  Control window's handle
//      uNotify:  Control notification code
//
// Returns:
//      Nothing.
//
//////////////////////////////////////////////////////////////////////////

void NavDemoWM_COMMAND ( HWND        hWnd,
                         int         nID,
                         HWND        hWndCtl,
                         UINT        uNotify )
{
                                        // Determine which control notified
                                        // us.
    switch ( nID )
        {
                                        // IDM_SCAN_FILE: Select a file to
                                        // be scanned.
        case IDM_SCAN_FILE:
            {
            char szFile [ MAX_PATH ];

            if ( NavDemoSelectFile ( hWnd,
                                     szFile,
                                     "Executable Files (*.exe, *.com)\0*.exe;*.com\0Word Documents (*.doc, *.dot)\0*.doc;*.dot\0All Program Files\0*.386;*.bin;*.cla;*.com;*.cpl;*.dll;*.doc;*.dot;*.drv;*.exe;*.ncp;*.ned;*.nnl;*.ocx;*.ov?;*.scr;*.sys;*.vbx;*.vxd\0All Files (*.*)\0*.*\0" ) )
                {
                                        // Scan the file specified by the
                                        // user.
                NavDemoScanFile ( hWnd, szFile );
                }
            }
        break;


                                        // IDM_SCAN_MEMORY_640: Scan
                                        // conventional memory.
        case IDM_SCAN_MEMORY_640:
            {
            NavDemoScanMem ( FALSE );
            }
        break;


                                        // IDM_SCAN_MEMORY_UPPER: Scan
                                        // conventional & upper memory.
        case IDM_SCAN_MEMORY_UPPER:
            {
            NavDemoScanMem ( TRUE );
            }
        break;


                                        // IDM_SCAN_SYSAREA: Scan the system
                                        // areas of a drive or drives.
        case IDM_SCAN_SYSAREA:
            {
            SYSAREA_OPTS rOpts;

            DWORD   dwVersion = GetVersion();

            if (dwVersion < 0x80000000)    // OS is NT
            {
               MessageBox(hWnd,
                "This demo doesn't use physcial\ndrives. Physcial drive information\nis required for NT.",
                  "Navdemo Scan System Area",MB_ICONINFORMATION);
            }
            else   // OS is Win95
               if( NavDemoSelectDrive ( &rOpts ) )
               {
                  NavDemoScanSysArea ( &rOpts );
               }
            }
        break;


                                        // IDM_SELF_TEST: CRC-check an
                                        // executable file.
        case IDM_SELF_TEST:
            {
            char szFile [ MAX_PATH ];

            if ( NavDemoSelectFile ( hWnd,
                                     szFile,
                                     "*.exe, *.dll, *.vxd\0*.exe;*.dll;*.vxd\0All Files (*.*)\0*.*\0" ) )
                {
                                        // Scan the file specified by the 
                                        // user.
                NavDemoSelfTest ( hWnd, szFile );
                }

            }
        break;


                                        // IDM_EXIT: Terminate the
                                        // application.
        case IDM_EXIT:
            {
            DestroyWindow ( hWnd );
            }
        break;

        }   // end switch
}    // NavDemoWM_COMMAND()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      BOOL NavDemoSelectFile ( HWND   hWnd,
//                               LPSTR  szFile,
//                               LPCSTR szFilters )
//
// Description:
//      Uses the common browse dialog to prompt the user for the name of
//      the file to scan.
// 
// Input:
//      hWnd:      Parent window handle.
//      szFile:    Buffer which receives the file name if the user selects
//                 a file.
//      szFilters: The filter string to be displayed in the browse dialog. 
// 
// Returns: 
//      TRUE if the user selects a file.
//      FALSE if the user cancels the browse operation.
//
//      If TRUE is returned, the name of the selected file is stored
//        in szFile.
//
//////////////////////////////////////////////////////////////////////////

BOOL NavDemoSelectFile ( HWND   hWnd,
                         LPSTR  szFile,
                         LPCSTR szFilters )
{
OPENFILENAME ofn;

                                        // Prepare our common dialog with
                                        // the information needed to prompt
                                        // for a file.
    memset ( (void*) &ofn, 0, sizeof ( ofn ) );

    *szFile = '\0';
    ofn.lStructSize = sizeof ( ofn );
    ofn.hwndOwner   = hWnd;
    ofn.lpstrFilter = szFilters;
    ofn.lpstrFile   = szFile;
    ofn.nMaxFile    = MAX_PATH;
    ofn.lpstrTitle  = "Select File to Scan";
    ofn.Flags       = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

                                        // Prompt the user for a file.
    return GetOpenFileName ( &ofn );

}   // NavDemoSelectFile()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoScanFile ( HWND   hWnd,
//                             LPSTR  szFile )
// Description:
//      This routine scans the specified file for known viruses.  If it
//      is infected, the user is asked what s/he wants to do with the file.
// 
// Input:
//      hWnd:    Parent window handle.
//      szFile:  Fully-qualified path of the file to scan.
// 
// Returns: 
//      Nothing.
//
//
// NAV SDK functions used:
//      VirusLibraryScanFile()
//
//////////////////////////////////////////////////////////////////////////

void NavDemoScanFile ( HWND   hWnd,
                       LPSTR  szFile )
{
char szVirusName [ VIRLIB_VIRUS_NAME_SIZE ] = "",
     szMessage [ 300 ] = "";
WORD wScanResult;

    if ( g_hScannerHandle != NULL )
        {
        sprintf ( szMessage, "Scanned \"%s\" -- ", szFile );

        wScanResult = VirusLibraryScanFile ( g_hScannerHandle,
                                             szFile,
                                             szVirusName );

        switch ( wScanResult )                                                
            {
            case VIRLIB_CLEAN:
                {
                strcat ( szMessage, "file is clean." );

                MessageOutput ( szMessage );
                }
            break;


            case VIRLIB_INFECTED:
                {
                strcat ( szMessage, "file is infected with " );
                strcat ( szMessage, szVirusName );
                strcat ( szMessage, "." );

                MessageOutput ( szMessage );

                                        // ask the user what to do
                                        // with the infected file
                NavDemoHandleFileInfection ( szFile );
                }
            break;


            case VIRLIB_ERROR:
                {
                strcat ( szMessage, "error during scanning." );

                MessageOutput ( szMessage );
                }
            break;


            default:
                {
                strcat ( szMessage, "unknown value returned from the scanner." );
                sprintf ( strchr ( szMessage, '\0' ),
                          " (%#x)", wScanResult );

                MessageOutput ( szMessage );
                }
            break;

            }   // end switch


        }   // end if
}   // NavDemoScanFile()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoHandleFileInfection ( LPSTR szFile )
//
// Description:
//      Prompts the user when an infected file is found.  The user can
//      choose to delete the file, view information about the virus,
//      or take no action and leave the file alone.
//      The code that actually operates on the file is in the dialog
//      box procedure (see navdwnd.cpp).
//
// Input:
//      szFile: The fully qualified name of the infected file.
//
// Returns:
//      Nothing.
//
//////////////////////////////////////////////////////////////////////////

void NavDemoHandleFileInfection ( LPSTR szFile )
{
    DialogBoxParam ( g_hInstance,
                     "NAVDEMO_HANDLE_INFECTION",
                     g_hMainWnd,
                     (DLGPROC) NavDemoHandleInfectionDlgProc,
                     (LPARAM) szFile );

}   // NavDemoHandleFileInfection()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoScanMem ( BOOL bScanUpperMem )
//
// Description:
//      This function handles memory scanning and displays the results
//      of the scan.
//
// Input:
//      bScanUpperMem: If this param is TRUE, the entire lowest 1 MB of
//                     memory is scanned.  If it's false, just the lowest
//                     640K is scanned.
//
// Returns:
//      Nothing.
//
//
// NAV SDK functions used:
//      VirusLibraryScanMem()
//
//////////////////////////////////////////////////////////////////////////

void NavDemoScanMem ( BOOL bScanUpperMem )
{
char szVirusName [ VIRLIB_VIRUS_NAME_SIZE ], szMessage [ 128 ];
WORD wScanResult;

                                        // Display a dialog that will
                                        // show the memory scan's
                                        // progress.

    g_hWndProgressDlg = CreateDialog ( g_hInstance,
                                       "NAVDEMO_MEMSCAN_PROGRESS",
                                       g_hMainWnd,
                                       (DLGPROC) NavDemoMemScanProgressDlgProc );

    if ( g_hWndProgressDlg )
        {
        ShowWindow ( g_hWndProgressDlg, SW_SHOW );
//        InvalidateRect ( g_hWndProgressDlg, NULL, FALSE );
//        UpdateWindow ( g_hWndProgressDlg );
        }


    if ( bScanUpperMem )
        {
        strcpy ( szMessage, "Scanned conventional and upper memory -- " );
        }
    else
        {
        strcpy ( szMessage, "Scanned conventional memory -- " );
        }

                                        // Display the wait cursor while
                                        // the scan is in progress.
    SetCursor ( g_hWaitCursor );

    wScanResult = VirusLibraryScanMem ( g_hScannerHandle,
                                        bScanUpperMem ? VIRLIB_UPPER_FLAG : 0,
                                        NULL,
                                        szVirusName );

    switch ( wScanResult )
        {
        case VIRLIB_CLEAN:
            {
            strcat ( szMessage, "memory is clean." );
            }
        break;


        case VIRLIB_INFECTED:
            {
            strcat ( szMessage, "memory is infected with " );
            strcat ( szMessage, szVirusName );
            strcat ( szMessage, "." );
            }
        break;


        case VIRLIB_ERROR:
            {
            strcat ( szMessage, "error during scanning." );
            }
        break;


        case VIRLIB_NOT_ENAB:
            {
            strcat ( szMessage, "memory scanning is disabled on this platform." );
            }
        break;


        default:
            {                          
            strcat ( szMessage, "unknown value returned from the scanner." );
            sprintf ( strchr ( szMessage, '\0' ), " (%#x)", wScanResult );
            }
        break;

        }   // end switch

    MessageOutput ( szMessage );

                                        // Remove the progress dialog.
    if ( g_hWndProgressDlg )
        {
        DestroyWindow ( g_hWndProgressDlg );
        g_hWndProgressDlg = NULL;

        }
                                        // Restore the normal arrow cursor.        
    SetCursor ( g_hArrowCursor );
}


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      BOOL NavDemoSelectDrive ( LPSYSAREA_OPTS prOpts )
//
// Description:
//      This function prompts the user for which drive(s) to scan.
//
// Input:
//      prOpts: Pointer to a SYSAREA_OPTS struct into which this function
//              will store the user's selection if the user chooses OK from
//              the prompt dialog.  This struct can then be passed directly
//              to NavDemoScanSysArea().
//
// Returns:
//      TRUE if the user chooses OK from the prompt, or FALSE if the user
//      chooses Cancel.
//
//      prOpts: See above.
//
//////////////////////////////////////////////////////////////////////////

BOOL NavDemoSelectDrive ( LPSYSAREA_OPTS prOpts )
{
BOOL bRetVal;

    bRetVal = DialogBoxParam ( g_hInstance,
                               "NAVDEMO_SYSAREA_PROMPT",
                               g_hMainWnd,
                               (DLGPROC) NavDemoSysAreaPromptDlgProc,
                               (LPARAM) (LPSYSAREA_OPTS) prOpts );

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoScanSysArea ( LPSYSAREA_OPTS prOpts )
//
// Description:
//      Translates the contents of the passed-in struct into parameters
//      for VirusLibraryScanSysArea() and then calls VLScanSysArea() to
//      do the actual scan.  Also displays the results of the scan.
//
// Input:
//      prOpts: Pointer to a SYSAREA_OPTS struct which is assumed to 
//              contain valid information.  The contents are translated
//              into the corresponding parameters for VLScanSysArea().
//
// Returns:
//      Nothing.
//
//
// NAV SDK functions used:
//      VirusLibraryScanSysArea()
//
//////////////////////////////////////////////////////////////////////////

void NavDemoScanSysArea ( LPSYSAREA_OPTS prOpts )
{
WORD wScanResult;
BYTE byDrive;
char szVirusName [ VIRLIB_VIRUS_NAME_SIZE ], szMessage [ 128 ];

                                        // Set up the parameters - they
                                        // will differ depending on whether
                                        // we're scanning one drive or
                                        // multiple drives
    if ( prOpts->bOneDrive )
        {
        byDrive = (BYTE) prOpts->wDrive;

        wScanResult = VirusLibraryScanSysArea ( g_hScannerHandle,
                                                VIRLIB_SCAN_DRIVE_FLAG,
                                                (LPBYTE) &byDrive,
                                                szVirusName );

        sprintf ( szMessage, "Scanned drive %c: -- ", byDrive );

        switch ( wScanResult )
            {
            case VIRLIB_CLEAN:
                {
                strcat ( szMessage, "drive is clean." );
                }
            break;
    

            case VIRLIB_INFECTED:
                {
                strcat ( szMessage, "drive is infected with " );
                strcat ( szMessage, szVirusName );
                strcat ( szMessage, "." );
                }
            break;


            case VIRLIB_ERROR:
                {
                strcat ( szMessage, "error during scanning." );
                }
            break;


            case VIRLIB_NO_LOGICAL:
                {
                strcat ( szMessage,
                         "scanning of logical drives is not supported on this platform." );
                }
            break;


            case VIRLIB_NOT_BOOT_DEVICE:
                {
                strcat ( szMessage,
                         "drive is not a bootable device and cannot be scanned." );
                }
            break;


            default:
                {
                strcat ( szMessage, "unknown value returned from the scanner." );
                sprintf ( strchr ( szMessage, '\0' ),
                          " (%#x)", wScanResult );
                }
            break;
            }   // end switch
        }   // end if
    else
        {
                                        // We're scanning multiple drives.

                                        // Determine which types of drives
                                        // were selected and make the
                                        // corresponding status message.
        switch ( prOpts->wDrive )
            {
            case VIRLIB_FIXED_FLAG:
                {
                strcpy ( szMessage, "Scanned all fixed drives -- " );
                }
            break;


            case VIRLIB_REMOVABLE_FLAG:
                {
                strcpy ( szMessage, "Scanned all removable drives -- " );
                }
            break;


            case VIRLIB_FIXED_FLAG | VIRLIB_REMOVABLE_FLAG:
                {
                strcpy ( szMessage, 
                         "Scanned all fixed drives and all removable drives -- " );
                }
            break;
            }   // end switch

        wScanResult = VirusLibraryScanSysArea ( g_hScannerHandle,
                                                prOpts->wDrive,
                                                (LPBYTE) &byDrive,
                                                szVirusName );

        switch ( wScanResult )
            {
            case VIRLIB_CLEAN:
                {
                strcat ( szMessage, "drives are clean." );
                }
            break;


            case VIRLIB_INFECTED:
                {
                sprintf ( strchr ( szMessage, '\0' ),
                          "drive %c: is infected with %s.",
                          (char) byDrive,
                          szVirusName );
                }
            break;


            case VIRLIB_ERROR:
                {
                strcat ( szMessage, "error during scanning." );
                }
            break;


            case VIRLIB_NO_LOGICAL:
                {
                strcat ( szMessage,
                         "scanning of logical drives is not supported on this platform." );
                }
            break;


            case VIRLIB_NOT_BOOT_DEVICE:
                {
                strcat ( szMessage,
                         "drive is not a bootable device and cannot be scanned." );
                }
            break;


            default:
                {
                strcat ( szMessage, 
                         "unknown value returned from the scanner." );
                sprintf ( strchr ( szMessage, '\0' ),
                          " (%#x)", wScanResult );
                }
            break;
            }   // end switch
        }   // end else

    MessageOutput ( szMessage );


    if ( wScanResult == VIRLIB_INFECTED )
        {
        NavDemoHandleSysAreaInfection ();
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoHandleSysAreaInfection ( void )
//
// Description:
//      This routine prompts the user to view info about a virus that
//      was found in a system area.
// 
// Input:
//      Nothing.
//
// Returns:
//      Nothing.
//
//
// NAV SDK functions used:
//      VirusLibraryGetLastVirusInfo()
//
//////////////////////////////////////////////////////////////////////////

void NavDemoHandleSysAreaInfection ( void )
{
    if ( IDYES == MessageBox ( g_hMainWnd,
                               "A virus was found in a system area.\nWould you like to see information about this virus?",
                               "NAV SDK 3.1 Demo",
                               MB_YESNO | MB_ICONQUESTION ) )
        {
        VIRUSINFO rVirusInfo;

        if ( VIRLIB_OK == VirusLibraryGetLastVirusInfo (
                              g_hScannerHandle,
                              (LPVIRUSINFO) &rVirusInfo ) )
            {
                                        // Display the virus info screen.

            DialogBoxParam ( g_hInstance,
                             "NAVDEMO_VIRUS_INFO",
                             g_hMainWnd,
                             (DLGPROC) NavDemoVirusInfoDlgProc,
                             (LPARAM) &rVirusInfo );
            }   // end if
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void NavDemoSelfTest ( HWND   hWnd,
//                             LPSTR  szFile )
// Description:
//      This routine checks the CRC of the specified file.
// 
// Input:
//      hWnd:    Parent's window handle.
//      szFile:  Fully-qualified path of the file to check.
// 
// Returns: 
//      Nothing.
//
//
// NAV SDK functions used:
//      VirusLibrarySelfTest()
//
//////////////////////////////////////////////////////////////////////////

void NavDemoSelfTest ( HWND   hWnd,
                       LPSTR  szFile )
{
char szMessage [ 300 ] = "";
WORD wScanResult;

                                        // CRC check the file.

    sprintf ( szMessage, "Checked \"%s\" -- ", szFile );

    wScanResult = VirusLibrarySelfTest ( szFile );

    switch ( wScanResult )
        {
        case VIRLIB_SELFTEST_OK:
            {
            strcat ( szMessage, "CRC is correct." );
            }
        break;


        case VIRLIB_SELFTEST_NO_CRC:
            {
            strcat ( szMessage, "file has no CRC." );
            }
        break;


        case VIRLIB_SELFTEST_FAIL:
            {
            strcat ( szMessage, "CRC is incorrect." );
            }
        break;


        case VIRLIB_SELFTEST_ERROR:
            {
            strcat ( szMessage, "error during scanning." );
            }
        break;


        default:
            {
            strcat ( szMessage, "unknown value returned from the scanner." );
            sprintf ( strchr ( szMessage, '\0' ),
                      " (%#x)", wScanResult );
            }
        break;

        }   // end switch

    MessageOutput ( szMessage );

}   // NavDemoSelfTest()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      void MessageOutput ( LPCSTR szMessage )
//
// Description:
//      Adds the specified message to the end of the main window's output
//      listbox.
//
// Input:
//      szMessage: The text to display
//
// Returns:
//      Nothing.
//
//////////////////////////////////////////////////////////////////////////

void MessageOutput ( LPCSTR szMessage )
{
HDC  hDC;
SIZE rSize;


    while ( LB_ERRSPACE == SendMessage ( g_hOutputListbox,
                                         LB_ADDSTRING,
                                         0,
                                         (LPARAM) szMessage ) )
        {
                                        // If the listbox ran out of memory,
                                        // delete the top line from the
                                        // listbox and try to add szMessage
                                        // again.
        SendMessage ( g_hOutputListbox,
                      LB_DELETESTRING,
                      0,                // Delete the first item.
                      0L );
        }


                                        // Set the horizontal extent of the
                                        // listbox if necessary, to
                                        // accomodate long strings.

        hDC = GetDC ( g_hOutputListbox );

        if ( hDC != NULL )
            {
            if ( GetTextExtentPoint32 (hDC,
                                      (LPCTSTR) szMessage,
                                      strlen ( szMessage ),
                                      (LPSIZE) &rSize ) )
                {
                if ( rSize.cx > SendMessage ( g_hOutputListbox,
                                              LB_GETHORIZONTALEXTENT,
                                              0,
                                              0L ) )
                    {
                    SendMessage ( g_hOutputListbox,
                                  LB_SETHORIZONTALEXTENT,
                                  (WPARAM) rSize.cx,
                                  0L );
                    }
                }

            ReleaseDC ( g_hOutputListbox, hDC );
            }


}   // MessageOutput()


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      BOOL WINAPI EXPORT NavDemoMemScanCallback ( DWORD  dwNumDone,
//                                                  DWORD  dwNumTotal,
//                                                  LPVOID lpUserData )
//
// Description:
//      This is the callback function that VirusLibraryScanMem() calls.
//      This function is responsible for displaying a progress message.
//      The message is displayed in a modeless dialog box that this
//      function creates and destroys.
//
// Input:
//      dwNumDone:  A counter that indicates how far VirusLibraryScanMem()
//                  has progressed.
//      dwNumTotal: The value that wNumDone will equal when the scan of
//                  the current memory buffer is complete.
//
//      lpUserData: Pointer to any extra data.
//
// Returns:
//      Always returns TRUE.
//
// Note:
//      This routine must be exported in the 16-bit version since it is used
//      by the navoem.dll.
//
//////////////////////////////////////////////////////////////////////////

BOOL WINAPI EXPORT NavDemoMemScanCallback ( DWORD dwNumDone, DWORD dwNumTotal, LPVOID lpUserData )
{
int  nMemPercent;                       // This percentage is calculated and
                                        // displayed in the modeless dlg.


// For Win32, wNumDone progresses from 0 to dwNumTotal once for 
// conventional memory, then again for upper memory.  We can therefore
// display the percent scanned based on the value of wNumDone.  Note that
// the counter will reach 100% when conventional memory scanning is 
// finished, then reset to 0% when upper memory scanning is started.



    if ( g_hWndProgressDlg )
        {            
        nMemPercent = (int) ( 100 * ( dwNumDone / dwNumTotal ) );

        SetDlgItemInt ( g_hWndProgressDlg,
                        IDT_PROGRESS_PERCENT,
                        nMemPercent,
                        FALSE );
        }
 
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:
//      WORD NavDemoFakeInstall ( LPSTR  lpAppID,
//                                LPSTR  lpVirusDefDir )
// Description:
//      This routine call the Virus Definition Library's
//      Install function to register NAVDEMO as a virus definition
//      user.
// 
// Input:
//      lpAppID:        Buffer to hold the NULL terminated Application Identifier
//      lpVirusDefDir:  Directory containing virus definition files to install
// 
// Returns: 
//      0:        if the install/registration was successful
//      non-zero: failure
//
// NAV SDK functions used:
//      VirusDefInit()
//      VirusDefInstall()
//      VirusDefClose()
//
//////////////////////////////////////////////////////////////////////////

WORD NavDemoFakeInstall ( LPSTR  lpAppID,
                          LPSTR  lpVirusDefDir )
{
    HVIRUSDEF hVirusDef;
    BOOL      bRetValue;

    if ( VirusDefInit(&hVirusDef) == FALSE )
        return (1);

    bRetValue = VirusDefInstall(hVirusDef, lpAppID, MAX_PATH,
                                NULL, lpVirusDefDir);

    VirusDefClose(hVirusDef);

    return (bRetValue);
}



//////////////////////////////////////////////////////////////////////////
//
// Function:
//      BOOL NavDemoFakeUninstall ( LPSTR  lpAppID )
//
// Description:
//      This routine call the Virus Definition Library's
//      Uninstall function to remove NAVDEMO as a virus definition
//      user.
// 
// Input:
//      lpAppID: NULL terminated Application Identifier.
// 
// Returns: 
//      TRUE     if the uninstall was sucessful
//
//
// NAV SDK functions used:
//      VirusDefInit()
//      VirusDefUninstall()
//      VirusDefClose()
//
//////////////////////////////////////////////////////////////////////////

BOOL NavDemoFakeUninstall ( LPSTR  lpAppID )
{
    HVIRUSDEF hVirusDef;
    BOOL      bRetValue;

    if ( VirusDefInit(&hVirusDef) == FALSE )
        return (FALSE);

    bRetValue = VirusDefUninstall(hVirusDef, lpAppID );

    VirusDefClose(hVirusDef);

    return (bRetValue);
}