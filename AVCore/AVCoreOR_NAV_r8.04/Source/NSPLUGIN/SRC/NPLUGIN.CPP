//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/NSPLUGIN/VCS/Nplugin.cpv   1.2   28 Jul 1997 13:52:40   DALLEE  $
//
// Description: Source file for the application class and command line
//              parsing class.
//
//************************************************************************
//  $Log:   S:/NSPLUGIN/VCS/Nplugin.cpv  $
// 
//    Rev 1.2   28 Jul 1997 13:52:40   DALLEE
// Added OS version checking and use SFN w/ no quotes for helper app
// command line if major version less than 4.
// Kluge around problem w/ Netscape Communicator on NT 3.51 dropping
// command line parameters when quotes are present.
// 
//    Rev 1.1   09 Jun 1997 13:59:18   MDUNN
// Made the various state variables global, instead of having them be member
// variables of the app class.  This makes the code way more readable and
// intuitive.
// Added the /r and /n switches, with supporting code.
// Added in logic to handle the case where NAV isn't installed (the only
// thing we can do is uninstall NAV as a plug-in).
// 
//    Rev 1.0   21 May 1997 16:34:04   MDUNN
// Initial revision.
//************************************************************************

#include "stdafx.h"
#include "NPlugIn.h"
#include "NPlugDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNPlugInApp

BEGIN_MESSAGE_MAP(CNPlugInApp, CWinApp)
    //{{AFX_MSG_MAP(CNPlugInApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPlugInApp construction

CNPlugInApp::CNPlugInApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance

//    AfxEnableMemoryTracking ( TRUE );
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNPlugInApp object

CNPlugInApp theApp;


/////////////////////////////////////////////////////////////////////////////
// Global variables

BOOL    g_bOSIsNT;                      // Operating system info, set in
DWORD   g_dwOSMajor;                    // CNPlugInApp::InitInstance()
DWORD   g_dwOSMinor;

                                        // The following variables are set
                                        // by command-line switches.
BOOL    g_bSilentMode;
BOOL    g_bInstallMode;
BOOL    g_bUninstallMode;
BOOL    g_bForcedUninstallMode;
BOOL    g_bForcedSilentUninstallMode;
BOOL    g_bNAVInstalling;

CString g_cstrINIFilename;
CString g_cstrOutputKey;

// ** NOTE **  The g_bForcedSilentUninstallMode is needed because
// normal forced uninstall mode sets g_bSilentMode to keep the UI from
// popping up.  g_bForcedSilentUninstallMode is set only if the /u /s
// switches are on the command line.

/////////////////////////////////////////////////////////////////////////////
// CNPlugInApp initialization

BOOL CNPlugInApp::InitInstance()
{
    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    // ******* NOTE **********
    // The following calls are commented out for the time being.
    // The Enable3dControlsStatic() results in linker errors, but the
    // Enable3dCOntrols() works OK.  Trouble is that now we can't
    // build a version of the app that's statically linked to MFC.
    // Not calling these functions results in an oldshell-looking
    // checklistbox on 3.51.  Everything else in the dialog has a 3D look.

#ifdef _AFXDLL
//    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
//    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    // Initialize the global OS flag.
OSVERSIONINFO rOSInfo;

    rOSInfo.dwOSVersionInfoSize = sizeof ( rOSInfo );
    GetVersionEx ( &rOSInfo );

    g_bOSIsNT   = ( rOSInfo.dwPlatformId == VER_PLATFORM_WIN32_NT );
    g_dwOSMajor = rOSInfo.dwMajorVersion;
    g_dwOSMinor = rOSInfo.dwMinorVersion;

    // Parse command line & set the app's flags based on any command
    // line switches that may be present.
CNPICommandLineInfo cli;

    ParseCommandLine ( cli );

    // Special check here.  If no /f switch was on the command line, set
    // NPLUGIN.INI (in the same dir as the app) as the INI filename.  This
    // lets users run the app on their own by double-clicking it.
    // Note:: NPLUGIN.INI can be changed as it's a string resource.

    if ( g_cstrINIFilename.IsEmpty() )
        {
        TCHAR  szPath [ MAX_PATH ];
        LPTSTR szName;
        CString cstrINIDefaultName;

        cstrINIDefaultName.LoadString ( IDS_INI_DEFAULT_FILENAME );

        GetModuleFileName ( NULL, szPath, sizeof(szPath) / sizeof(TCHAR) );

        if ( szName = _tcsinc ( _tcsrchr ( szPath, '\\' ) ) )
            {
            // There'd _better_ be a backslash!
            _tcscpy ( szName, cstrINIDefaultName );

            g_cstrINIFilename = szPath;
            }
        }


CNPlugInDlg dlg;
int         nDlgRet;
DWORD       dwDisposition;              // not used, needed by RegCreateKeyEx
HKEY        hkeyOutput;

    m_pMainWnd = &dlg;

    nDlgRet = dlg.DoModal();

    if ( nDlgRet == -1 )
        {
        // -1 means the DoModal() call failed and the dlg was
        // never shown.
        // 
        // Some kind of message here??
        }
    else if ( !g_cstrOutputKey.IsEmpty() )
        {
        // Write the return value to the registry.

        if ( ERROR_SUCCESS == RegCreateKeyEx
                                  ( HKEY_LOCAL_MACHINE,
                                    g_cstrOutputKey,
                                    0, NULL, REG_OPTION_NON_VOLATILE,
                                    KEY_WRITE,
                                    NULL, &hkeyOutput, &dwDisposition ) )
            {
            if ( ERROR_SUCCESS != RegSetValueEx
                                      ( hkeyOutput,
                                        _T("NSPlugIn"),
                                        0, REG_DWORD,
                                        (const BYTE*) (DWORD*) &nDlgRet,
                                        sizeof ( DWORD ) ) )
                {
                // Error writing the return code!
                nDlgRet = (DWORD) -1;
                }

            RegCloseKey ( hkeyOutput );
            }
        else
            {
            // Error creating/opening the reg key!
            nDlgRet = (DWORD) -1;
            }
        }   // end if

    // I was thinking about returning nDlgRet, as in
    //    ExitProcess ( (UINT) nDlgRet );
    // but I don't know if calling ExitProcess() from deep within the
    // MFC framework will work properly and not leak resources.  For now
    // I'll just write the reg value and that's it.  If install needs a
    // return value from the app I can try it & see what happens.

                            
    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CNPICommandLineInfo functions

CNPICommandLineInfo::CNPICommandLineInfo() : CCommandLineInfo()
{
}

// Note that this function takes an LPCSTR.  The MFC docs are wrong in saying
// that it taken an LPCTSTR.  So everything in this fn is single-byte
// only.
// I'm unsure if this will hose us if the user enters a DB character
// on the command line.  Probably will.
void CNPICommandLineInfo::ParseParam ( LPCSTR lpszParam,
                                       BOOL bFlag, BOOL bLast )
{
CString cstrParam = lpszParam;

    if ( bFlag  &&  !cstrParam.IsEmpty() )
        {
        switch ( _totlower ( cstrParam[0]) )
            {
            case 's':                   // silent mode
                {
                g_bSilentMode = TRUE;
                }
            break;

            case 'i':                   // install mode
                {
                g_bInstallMode = TRUE;
                g_bUninstallMode = FALSE;
                }
            break;

            case 'u':                   // uninstall mode
                {
                g_bUninstallMode = TRUE;
                g_bInstallMode = FALSE;
                }
            break;

            case 'n':                   // NAV's install launched us, and we
                {                       // should stay always on top.
                g_bNAVInstalling = TRUE;
                }
            break;

            case 'f':                   // name of config file
                {
                // Check if the 2nd char is a colon, and skip it if so.  This
                // lets the switch be entered as "/f:file.ini" or "/ffile.ini".
                if ( cstrParam[1] == ':' )
                    {
                    g_cstrINIFilename = cstrParam.Mid(2);
                    }
                else
                    {
                    g_cstrINIFilename = cstrParam.Mid(1);
                    }
                }
            break;

            case 'r':                   // name of output registry key
                {
                // Similar check to the /f switch above.
                if ( cstrParam[1] == ':' )
                    {
                    g_cstrOutputKey = cstrParam.Mid(2);
                    }
                else
                    {
                    g_cstrOutputKey = cstrParam.Mid(1);
                    }
                }
            break;

            // Just ignore any other switch characters
            }   // end switch
        }   // end if ( bFlag )

    // We'll ignore any command line parameters that aren't switches
}
