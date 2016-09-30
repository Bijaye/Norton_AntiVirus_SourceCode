// Copyright (C) 1998 Symantec Corporation
//*****************************************************************************
// $Header:   S:/NAVWRAPC/VCS/NAVWRAPC.CPv   1.1   03 Aug 1998 16:37:56   jallee  $
//
// Description: This file contains the implementation for NAVWRAPC.DLL the
//              purpose of which is to provide a small amount of encapsualtion
//              of NAV options handling.
//
//              This DLL exports (for now) one function: "DisplayNAVOptionsDialog"
//              which displays the appropriate NAV property pages reading and
//              writing to a caller specified set of NAV options files.
//
//              In a way, this DLL is actually a subset of NAV and so requires
//              many of the same DLLs that NAV requires and that make up NAV. If
//              This DLL is installed with NAV, then all of the necessary DLLs
//              will automatically be present. If however, this DLL will be used
//              outside of NAV (as it will be with NSDU 3.0) the DLLs will need
//              to be shipped with it.
//
//*****************************************************************************
// $Log:   S:/NAVWRAPC/VCS/NAVWRAPC.CPv  $
// 
//    Rev 1.1   03 Aug 1998 16:37:56   jallee
// Fixed error handling so that n32opts.dll is freed when navopts can not be read.
// 
//    Rev 1.0   29 May 1998 14:34:56   DBuches
// Initial revision.
// 
//    Rev 1.3   07 May 1998 07:56:48   TSmith
// Fixed 114279 - Added code to initialize the Symantec standard help system so
// that Core based dialog help would work (e.g. Password Change).
// 
//    Rev 1.2   01 Apr 1998 22:07:58   TSmith
// Removed platform #ifdefs. Only build one platform now.
// 
//    Rev 1.1   01 Apr 1998 11:00:42   TSmith
// Changed 'DisplayNAVOptionsDialog' to load either n32opts.dll or xntopts.dll
// based on a compile time definition rather than a run time check.
// 
//    Rev 1.0   30 Mar 1998 15:59:54   TSmith
// Initial revision.
//*****************************************************************************
#include "platform.h"
#include "navsheet.h"
#include "navutil.h"
#include "options.h"
#include "navwrapc.h"



///////////////////////////////////////////////////////////////////////////////
// Type definitions

typedef int (PASCAL * PFNNAVOPTIONSSHEETEX )( HWND         hwnd, 
                                              LPNAVOPTIONS pNavOpts, 
                                              BOOL         *pbOptionsChanged,
                                              int          *iPanels, 
                                              int          iCount, 
                                              int          iActivePage, 
                                              BOOL         bIsNT );



///////////////////////////////////////////////////////////////////////////////
// Global variables

PFNNAVOPTIONSSHEETEX pfnNAVOptionsSheetEx = NULL;
HELPSTRUCT           g_HelpInfo;
HINSTANCE            g_hOptionsUI;
BOOL                 g_bIsNt;



///////////////////////////////////////////////////////////////////////////////
// Local function declarations

static BOOL OnLoadNAVOPTS(
    LPCSTR lpModuleName
    );

static BOOL OnFreeNAVOPTS(
    void
    );

static void InitStdHelp(
    HWND   hWnd,
    LPCSTR lpHelpFile
    );



/*****************************************************************************
FUNCTION:
    OnLoadNAVOPTS

DESCRIPTION:
    Loads the specified module.

PARAMETERS:
    LPCSTR lpModuleName - Pointer to the name of DLL to load.

RETURN VALUE:
    TRUE if module successfully loaded, else FALSE.
******************************************************************************
3/26/1998 TSmith  - Ported from Savanna.
*****************************************************************************/
static BOOL OnLoadNAVOPTS( LPCSTR lpModuleName )
    {
    DWORD dwError;
    BOOL  bResult = TRUE;

    SYM_ASSERT( lpModuleName );

    if( ( g_hOptionsUI = LoadLibrary( lpModuleName ) ) == NULL ) 
        {
        dwError = GetLastError( );
        bResult = FALSE ;
        }

    return bResult;
    }



/*****************************************************************************
FUNCTION:
    OnFreeNAVOPTS
    
DESCRIPTION:
    Unloads the specified module.

PARAMETERS:
    None.

RETURN VALUE:
    TRUE if the module was loaded, else FALSE.
******************************************************************************
3/26/1998 TSmith  - Ported from Savanna.
*****************************************************************************/
static BOOL OnFreeNAVOPTS( void ) 
    {
    BOOL bResult = FALSE;

    if ( g_hOptionsUI )
        {
        FreeLibrary( g_hOptionsUI );
        g_hOptionsUI = NULL;
        bResult = TRUE ;
        }

    return bResult;
    }



/*****************************************************************************
FUNCTION:
    DisplayNAVOptionsDialog
    
DESCRIPTION:
    Loads NAV options from the specifed source directory, displays the NAV
    property pages to allow modification of the options and writes the options
    out to the specified target directory. The source and target directories
    can, and most of the time will be, the same.
    
    Also registers the Symantec standard help system which is necessary for
    some of the help links in the options pages to work.

PARAMETERS:
    HWND  hWnd        - Handle to parent window.
    LPSTR lpHelpFile  - Fully qualified name of NAV help file.
    LPSTR lpSourceDir - Where to find the NAV options files to load from.
    LPSTR lpTargetDir - Where to save the NAV options files to.
    BOOL  bIsNT       - Show Windows NT related pages.

RETURN VALUE:
    TRUE if NAV options were successfully written, else FALSE.
******************************************************************************
3/26/1998 TSmith  - Ported from Savanna.
5/05/1998 TSmith  - Added hWnd and lpHelpFile parameters & call to standard
                    help initialization.
8/3/98 - JALLEE Now call OnFreeNAVOPTS() before exiting in error condition.
*****************************************************************************/
EXTERNC SYM_EXPORT BOOL WINAPI DisplayNAVOptionsDialog(
    HWND  hWnd,
    LPSTR lpHelpFile,
    LPSTR lpSourceDir,
    LPSTR lpTargetDir,
    BOOL  bIsNT
    )
    {
    LPSTR lpOptionsDll = "n32opts.dll";
    BOOL  bResult      = TRUE;
    BOOL  bOptionsChanged = FALSE;

    SYM_ASSERT( lpSourceDir );
    SYM_ASSERT( lpTargetDir );

    if ( TRUE == OnLoadNAVOPTS( lpOptionsDll ) )
        {
        pfnNAVOptionsSheetEx =
            ( PFNNAVOPTIONSSHEETEX )::GetProcAddress( g_hOptionsUI, "_NAVOptionsSheetEx@24" );

        if ( pfnNAVOptionsSheetEx )
            {
            NAVOPTIONS NavOpts;

            memset( &NavOpts, 0, sizeof( NavOpts ) );

            // Read the options from the NavOpts.dat file.
            if ( ( ERR == ReadNavBinFromDir( lpSourceDir, &NavOpts ) ) ||
                 ( ERR == ReadNavStartFromDir( lpSourceDir, &NavOpts ) ) ||
                 ( ERR == ExcludeInitSpecifyDir( &NavOpts.exclude, lpSourceDir ) ) )
                {
                bResult = FALSE ;
                OnFreeNAVOPTS();
                goto DisplayNAVOptionsDialog_Exit;
                }

            int i95Panels[ ] =
                {
                N32OPTS_SCANNER_PAGE,
                N32OPTS_AP_PAGE,
                N32OPTS_STARTUP_PAGE,
                N32OPTS_ALERT_PAGE,
                N32OPTS_LOG_PAGE,
                N32OPTS_EXCLUDE_PAGE,
                N32OPTS_GENERAL_PAGE,
                N32OPTS_PASSWORD_PAGE,
                N32OPTS_INOC_PAGE
                };

            int iNtPanels[ ] =
                {
                N32OPTS_SCANNER_PAGE,
                N32OPTS_AP_PAGE,
                N32OPTS_ALERT_PAGE,
                N32OPTS_LOG_PAGE,
                N32OPTS_EXCLUDE_PAGE,
                N32OPTS_GENERAL_PAGE,
                N32OPTS_PASSWORD_PAGE
                };

            // Unfortunately, ALERTUI does not support specifying what
            // directory to load NAVALERT.DAT from. So as a work around, pick
            // up the file from the source dir and copy it to the current
            // directory so that ALERTUI will be able to find it. Don't bother
            // error checking since sometimes the file will not be present
            // (like when workstation alerts are being configured).
            CString strNavAlertDat( lpSourceDir );
            strNavAlertDat += "\\navalert.dat";
            CopyFile( strNavAlertDat, "navalert.dat", FALSE );

            // Initialize the Symantec standard help system.
            InitStdHelp( hWnd, lpHelpFile );

            // Display the actual property pages.
            int nResult = pfnNAVOptionsSheetEx( NULL,
                                                &NavOpts,
                                                &bOptionsChanged,
                                                bIsNT ? iNtPanels : i95Panels,
                                                bIsNT ? 7 : 9,
                                                0,
                                                bIsNT );

            if ( ERROR_SUCCESS == nResult )
                {
                // Only write to the files when dialog succeeded.
                WriteNavBinToDir( lpTargetDir, &NavOpts );
                WriteNavStartToDir( lpTargetDir, &NavOpts );
                ExcludeTerminateSpecifyDir( &NavOpts.exclude,lpTargetDir, FALSE );

                // Even more unfortunately, ALERTUI does not support what
                // directory to save NAVALERT.DAT to. So as another work around,
                // move the file to the target directory.
                strNavAlertDat = lpTargetDir;
                strNavAlertDat += "\\navalert.dat";
                MoveFileEx( "navalert.dat", strNavAlertDat, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING );
                }
            else
                {
                // If the dialog was cancelled or there was a failure somewhere
                // inside of it, clean up the temporary file copied earlier.
                DeleteFile( "navalert.dat" );
                }
            }

        OnFreeNAVOPTS();
        }
    else
        {
        bResult = FALSE ;
        goto DisplayNAVOptionsDialog_Exit;
        }

DisplayNAVOptionsDialog_Exit:

    return bResult;
    }



/*****************************************************************************
FUNCTION:
    InitStdHelp
    
DESCRIPTION:
    Initializes the Symantec standard help system.

PARAMETERS:
    HWND   hWnd       - Handle to parent window.
    LPCSTR lpHelpFile - Fully qualified help file name such as:
                        "D:\Program Files\NSC\SnapIns\NAV\4.0\WRKPDLL\NAVWNT.HLP".

RETURN VALUE:
    None.
******************************************************************************
5/5/1998 TSmith  - Function created.
*****************************************************************************/
static void InitStdHelp(
    HWND   hWnd,
    LPCSTR lpHelpFile
    )
    {
    SYM_ASSERT( hWnd );
    SYM_ASSERT( lpHelpFile );

    g_HelpInfo.hinst = g_hOptionsUI;
    g_HelpInfo.hwnd  = hWnd;

    STRCPY( g_HelpInfo.helpFileName, lpHelpFile );
    StdHelpExInit( &g_HelpInfo );
    }



/*****************************************************************************
FUNCTION:
    DllMain
    
DESCRIPTION:
    Standard Windows DLL entry point.

PARAMETERS:
    HANDLE hInst              - Handle this modules instance.
    ULONG  ul_reason_for_call - Just what it says.
    LPVOID lpReserved         - Not for us.

RETURN VALUE:
    TRUE always.
******************************************************************************
3/26/1998 TSmith  - Function created.
*****************************************************************************/
#if 0
BOOL WINAPI DllMain(
    HANDLE hInst,
    ULONG  ul_reason_for_call,
    LPVOID lpReserved
    )
    {
    switch( ul_reason_for_call )
        {
        case  DLL_PROCESS_ATTACH:
            break;
        case  DLL_THREAD_ATTACH:
            break;
        case  DLL_THREAD_DETACH:
            break;
        case  DLL_PROCESS_DETACH:
            break;
        default:
            break;
        }

    return( TRUE );
    }
#endif
