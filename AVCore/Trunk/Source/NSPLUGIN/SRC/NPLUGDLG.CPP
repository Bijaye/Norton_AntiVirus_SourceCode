//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/NSPLUGIN/VCS/nplugdlg.cpv   1.11   28 Aug 1998 17:27:06   JBRENNA  $
//
// Description: Source file for the main dialog class and its member
//              functions.
//
//************************************************************************
//  $Log:   S:/NSPLUGIN/VCS/nplugdlg.cpv  $
// 
//    Rev 1.11   28 Aug 1998 17:27:06   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.10.1.1   29 Jul 1998 15:54:28   mdunn
//    Changed CheckNetscapeInstalled() to look for a version string starting
//    with "4." instead of expecting exactly "4.0"  This makes us work on the
//    Netscape 4.5 beta.
// 
//    Rev 1.10   26 May 1998 14:41:28   mdunn
// Updated InitArraysWithHardCodedStrings() for Atomic.
// 
//    Rev 1.9   09 Mar 1998 10:35:46   mdunn
// Minor changes to fix the tab order and hotkeys.
// 
//    Rev 1.8   10 Sep 1997 17:10:34   KSACKIN
// Ported changes from QAKG.
// 
//    Rev 1.7.1.2   23 Aug 1997 21:44:14   KSACKIN
// On Install, if you choose not to install NAV as a plugin, we will not process
// our OK message and write all of our MIME types out to disk.  Also, if you
// don't have NAV as a plugin, and you launch it at a later time and don't
// decide to install NAV, we also will not process the OK message.  In all
// other cases, even if you don't change the state of the plugin information,
// you will get the options written down to disk, and will get a message box
// saying that your changes were made successfully.
//
//    Rev 1.7.1.1   22 Aug 1997 00:28:16   KSACKIN
// Added a check box for whether to use NAV as a Netscape plugin or not,
// reworded some text, and added a frame around the advanced section.
//
//    Rev 1.7.1.0   12 Aug 1997 22:57:18   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.7   29 Jul 1997 17:12:02   BGERHAR
// Sync program extensions with NAV app
//
//    Rev 1.6   28 Jul 1997 13:50:48   DALLEE
// Added OS version checking and use SFN w/ no quotes for helper app
// command line if major version less than 4.
// Kluge around problem w/ Netscape Communicator on NT 3.51 dropping
// command line parameters when quotes are present.
//
//    Rev 1.5   28 Jul 1997 10:41:18   DALLEE
// Added Netscape 4.0 detection.
//
//    Rev 1.4   23 Jun 1997 15:50:48   mdunn
// Fixed another case covered by defect 85772.  Now I don't check the
// return from RegDeleteValue(), since if the Suffixes key doesn't exist
// and the NAV reg entries are being removed, RDV() will return an error but
// it's not an error condition as far as we're concerned.
//
//    Rev 1.3   09 Jun 1997 14:32:48   MDUNN
// Updated the code that handles the case where the app is run in silent-
// install mode but NAV isn't installed.  Now the app bails and sets bit
// 3 in the return value (to indicate a generic error).
//
//    Rev 1.2   09 Jun 1997 13:57:40   MDUNN
// Made the various state variables global, instead of having them be member
// variables of the app class.  This makes the code way more readable and
// intuitive.
// Added the /r and /n switches, with supporting code.
// Added in logic to handle the case where NAV isn't installed (the only
// thing we can do is uninstall NAV as a plug-in).
//
//    Rev 1.1   06 Jun 1997 12:23:00   MDUNN
// The app now creates the Viewers and/or Suffixes keys if necessary.
// Fixes defect #85772.
//
//    Rev 1.0   21 May 1997 16:32:02   MDUNN
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
// Global variables

extern BOOL    g_bOSIsNT;
extern DWORD   g_dwOSMajor;
extern DWORD   g_dwOSMinor;
extern BOOL    g_bSilentMode;
extern BOOL    g_bInstallMode;
extern BOOL    g_bUninstallMode;
extern BOOL    g_bForcedUninstallMode;
extern BOOL    g_bForcedSilentUninstallMode;
extern BOOL    g_bNAVInstalling;

extern CString g_cstrINIFilename;
extern CString g_cstrOutputKey;

BOOL   g_bOriginalUseNavAsPluginState;


/////////////////////////////////////////////////////////////////////////////
// CNPlugInDlg dialog

CNPlugInDlg::CNPlugInDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNPlugInDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNPlugInDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNPlugInDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNPlugInDlg)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNPlugInDlg, CDialog)
    //{{AFX_MSG_MAP(CNPlugInDlg)
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckAll)
    ON_BN_CLICKED(IDC_UNCHECK_ALL, OnUncheckAll)
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_WHATS_THIS, OnWhatsThis)
    ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_USE_NAV_AS_PLUGIN, OnUseNavAsPlugin)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPlugInDlg message handlers

BOOL CNPlugInDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ( g_bNAVInstalling )
        {
        // If the NAV install launched us, make the dialog topmost, since
        // we don't want the user to click on the install background
        // (which takes up the whole screen) and hide our window.  Also
        // remove the minimize button and the restore/minimize/maximize/size
        // commands from the system menu.

        SetWindowPos ( &wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

        DWORD dwStyle = ::GetWindowLong ( GetSafeHwnd(), GWL_STYLE );

        ::SetWindowLong ( GetSafeHwnd(), GWL_STYLE,
                          dwStyle & ~WS_MINIMIZEBOX );

        CMenu* pSysMenu = GetSystemMenu ( FALSE );

        pSysMenu->DeleteMenu ( SC_MINIMIZE, MF_BYCOMMAND );
        pSysMenu->DeleteMenu ( SC_MAXIMIZE, MF_BYCOMMAND );
        pSysMenu->DeleteMenu ( SC_RESTORE,  MF_BYCOMMAND );
        pSysMenu->DeleteMenu ( SC_SIZE,     MF_BYCOMMAND );
        pSysMenu->DeleteMenu ( 1,           MF_BYPOSITION );

        // That last DeleteMenu() call removes the leftover separator
        // between Move and Close.
        }
    else
        {
        // We were launched manually, so all we have to do is remove
        // Maximize and Size from the system menu.
        CMenu* pSysMenu = GetSystemMenu ( FALSE );

        pSysMenu->DeleteMenu ( SC_MAXIMIZE, MF_BYCOMMAND );
        pSysMenu->DeleteMenu ( SC_SIZE,     MF_BYCOMMAND );
        }


    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon


    // First do a basic check to verify that Netscape is installed, and
    // bail out if it isn't.

    if ( ! CheckNetscapeInstalled() )
        {
        if ( ! g_bSilentMode )
            {
            AfxMessageBox ( IDP_NETSCAPE_NOT_INSTALLED,
                            MB_OK | MB_ICONSTOP );
            }

            EndDialog ( NSP_ERROR_SAVING );
            return FALSE;
        }

    // Next check that NAV is installed.  If it isn't, we can't install
    // NAV as a helper app, so prompt the user to remove all the NAV
    // download scanner reg entries.

    if ( ! CheckNAVInstalled() )
        {
        if ( g_bUninstallMode  &&  g_bSilentMode )
            {
            // In this case, do nothing special, since we were launched
            // specifically to remove NAV as a helper app.  The following
            // code will handle removing the reg entries.

            g_bForcedUninstallMode       = TRUE;
            g_bForcedSilentUninstallMode = TRUE;
            }
        else if ( g_bInstallMode  &&  g_bSilentMode )
            {
            // This is a weird case.  We're in silent mode, but can't
            // do the install.  Scott and I agreed that it's enough
            // to just return the generic error code (bit 3 == 1 in the
            // reutrn value) and bail.

            EndDialog ( NSP_ERROR_SAVING );
            return FALSE;
            }
        else
            {
            // Otherwise, we enter a forced-uninstall mode.  Ask the
            // user if they want to remove all the NAV plug in reg entries.

            int nResponse;

            nResponse = AfxMessageBox ( IDP_FORCED_UNINSTALL,
                                        MB_YESNO | MB_ICONEXCLAMATION );

            if ( nResponse == IDNO )
                {
                // Don't do anything.
                EndDialog ( NSP_CANCEL );
                return FALSE;
                }
            else
                {
                // Set the app to silent mode and uninstall mode.

                g_bInstallMode         = FALSE;
                g_bUninstallMode       = TRUE;
                g_bSilentMode          = TRUE;
                g_bForcedUninstallMode = TRUE;
                }
            }
        }


    // Initialize the check list box
    m_checklist.SubclassDlgItem ( IDC_MIME_LIST, this );
    m_checklist.SetCheckStyle ( BS_AUTOCHECKBOX );

    // Init member arrays.
//&^    InitHelpIDArray();

    if ( ! InitMIMETypesArrays() )
        {
        if ( ! g_bSilentMode )
            {
            AfxMessageBox ( IDS_HOSED_INI_FILE, MB_ICONSTOP | MB_OK );
            }

            EndDialog ( NSP_ERROR_SAVING );
            return TRUE;
        }

    // Initialize the listbox.
    if ( ! FillListbox() )
        {
        EndDialog ( NSP_ERROR_SAVING );
        return TRUE;
        }

                                        // Turn on check if at least one
                                        // item is currently registered.
    UpdateUseNAVCheckStatus();

                                        // Save the original state of this
                                        // check box for later use.
    g_bOriginalUseNavAsPluginState = ((CButton *)GetDlgItem( IDC_USE_NAV_AS_PLUGIN ))->GetCheck();

    // Set the focus to the listbox.
//    GotoDlgCtrl ( GetDlgItem ( IDC_MIME_LIST ) );

    // NEW::
    // Act on any switches that were on the command line.
    // If the /s switch was there, call EndDialog() after the work is done
    // so we'll bail w/o showing the dialog.

    // Hide the HELP button if we're in install or uninstall mode.

//&^    if ( g_bUninstallMode || g_bInstallMode )
//&^ **NOTE** that the Help button is always hidden now, since help
//&^          is disabled entirely.
        {
        GetDlgItem ( ID_HELP ) -> ShowWindow ( SW_HIDE );
        }

    if ( g_bUninstallMode )
        {
        OnUncheckAll();
        }

    if ( g_bSilentMode )
        {
        OnOK();                         // OnOK() calls EndDialog() so
                                        // the app will exit after OnOK()
                                        // does its work.
        }

    return FALSE;  // return TRUE  unless you set the focus to a control
}

void CNPlugInDlg::OnDestroy()
{
//&^    WinHelp(0L, HELP_QUIT);
    CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNPlugInDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNPlugInDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}


/////////////////////////////////////////////////////////////////////////////
// CNPlugInDlg helper functions used by CNPlugInDlg::OnInitDialog()

//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::CheckNAVInstalled()
//
// Description:
//  Verifies that NAV is installed on the system.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if NAV is installed.
//  FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 2/24/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::CheckNAVInstalled()
{
BOOL bRetVal = FALSE;
HKEY hkeyInstalledApps;

    // First open the InstalledApps key.

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("Software\\Symantec\\InstalledApps"),
                                         0, KEY_READ,
                                         &hkeyInstalledApps ) )
        {
        // Next check for the NAV95/NAVNT value.  If it's there, then NAV
        // really is installed.

        if ( ERROR_SUCCESS == RegQueryValueEx ( hkeyInstalledApps,
                                                g_bOSIsNT ? _T("NAVNT")
                                                          : _T("NAV95"),
                                                0, NULL, NULL, NULL ) )
            {
            bRetVal = TRUE;
            }

        RegCloseKey ( hkeyInstalledApps );
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::CheckNetscapeInstalled()
//
// Description:
//  Verifies that Netscape version 2 to 4 is installed on the system.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if Netscape is installed.
//  FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 2/24/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::CheckNetscapeInstalled()
{
BOOL  bRetVal = FALSE;
HKEY  hkeyNetscapeMain;
HKEY  hkeyNetscapeCurrentVersion;
TCHAR szNetscapeVersion [ 80 ];
DWORD dwBufferSize;


    // First check for version 4.0
    // Open main Netscape key under local machine.

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("Software\\Netscape\\Netscape Navigator"),
                                         0, KEY_READ,
                                         &hkeyNetscapeMain ) )
        {
        // Get current installed Netscape version ( works for 4.x ).

        dwBufferSize = sizeof ( szNetscapeVersion );

        if ( ERROR_SUCCESS == RegQueryValueEx ( hkeyNetscapeMain,
                                                _T("CurrentVersion"),
                                                NULL, NULL,
                                                (LPBYTE) szNetscapeVersion,
                                                &dwBufferSize ) )
            {
            // Ensure major version is 4.

            if ( 0 == _tcsnicmp ( szNetscapeVersion, _T("4."), 2 ) )
                {
                // Double check that key for this version is here.

                if ( ERROR_SUCCESS == RegOpenKeyEx ( hkeyNetscapeMain,
                                                     szNetscapeVersion,
                                                     0, KEY_READ,
                                                     &hkeyNetscapeCurrentVersion ) )
                    {
                    bRetVal = TRUE;

                    RegCloseKey ( hkeyNetscapeCurrentVersion );
                    }
                }
            }

        RegCloseKey ( hkeyNetscapeMain );
        }

    // If version 4.0 is not installed, then do combined check for
    // versions 2.0 and 3.0

    if ( FALSE == bRetVal )
        {
        // First open the main Netscape info key under current user.

        if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_CURRENT_USER,
                                             _T("Software\\Netscape\\Netscape Navigator\\Main"),
                                             0, KEY_READ,
                                             &hkeyNetscapeMain ) )
            {
            // Next check the Netscape version

            dwBufferSize = sizeof ( szNetscapeVersion );

            if ( ERROR_SUCCESS == RegQueryValueEx ( hkeyNetscapeMain,
                                                    _T("Mozilla"),
                                                    0, NULL,
                                                    (LPBYTE) szNetscapeVersion,
                                                    &dwBufferSize ) )
                {
                // First check for the signature "Good-"

                if ( 0 == _tcsnicmp ( szNetscapeVersion, _T("Good-"), 5 ) )
                    {
                    // After "Good-" comes the version number.  Check that the
                    // next character is between '2' and '3'.

                    if ( szNetscapeVersion[5] >= '2'  &&
                         szNetscapeVersion[5] <= '3'     )
                        {
                        bRetVal = TRUE;
                        }
                    }
                }

            RegCloseKey ( hkeyNetscapeMain );
            }
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::InitHelpIDArray()
//
// Description:
//  This fn initializes the member array m_auContextHelpCtrlIDs[] with the
//  control IDs of all controls that have a right-click menu and can
//  respond to a context help request (from the user using the [?] caption
//  button or picking What's This? from the right-click menu).
//
// Input:
//  Nothing.
//
// Returns:
//  Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void CNPlugInDlg::InitHelpIDArray()
{
    m_anContextHelpCtrlIDs[0] = IDOK;
    m_anContextHelpCtrlIDs[1] = IDCANCEL;
    m_anContextHelpCtrlIDs[2] = IDC_MIME_LIST;
    m_anContextHelpCtrlIDs[3] = IDC_CHECK_ALL;
    m_anContextHelpCtrlIDs[4] = IDC_UNCHECK_ALL;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::InitMIMETypesArrays()
//
// Description:
//  This fn initializes the member array m_acstrMIMETypes[] with the
//  names of the MIME types that NAV can scan.  These names are hard-
//  coded in the function, but MIME types aren't translated so that's
//  not an issue for internationalization.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if everything is OK.
//  FALSE if not.  This will happen if the INI file is not present or is
//      totally jacked.  The app cannot continue if this fn returns FALSE.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::InitMIMETypesArrays()
{
TCHAR   szBigBuffer [4096];             // should be big enough
TCHAR   szKeyName [256];                // holds the full key name
TCHAR   szMIMEType [128];               // holds just the MIME type name
TCHAR   szExtensions [128];             // holds just the list of extensions
TCHAR   szValue [8];                    // holds YES/NO value
BOOL    bHasExtensions = FALSE;         // TRUE if the current MIME type has
                                        // a list of extensions
LPTSTR  szNextKey;
LPTSTR  szExtList;
CString cstrINISectionName;
CString cstrYes;

    // Init strings.
    cstrINISectionName.LoadString ( IDS_INI_SECTION );
    cstrYes.LoadString ( IDS_YES );

    // First grab a list of all keys in the INI section.  IOW, we'll grab
    // all of the strings on the LEFT side of the = signs.

    GetPrivateProfileString ( cstrINISectionName,
                              NULL,     // to grab all the keys
                              _T("**"), // sentinel
                              szBigBuffer,
                              sizeof (szBigBuffer) / sizeof(TCHAR),
                              g_cstrINIFilename );

    if ( 0 == _tcscmp ( _T("**"), szBigBuffer ) )
        {
        // If GPPS() returns 0, then the INI file is way hosed or doesn't
        // exist.  In this case, fall back to hard-coded strings.
        return InitArraysWithHardCodedStrings();
        }

    szNextKey = szBigBuffer;

    while ( *szNextKey != '\0' )
        {
        // Grab the key name so we can get its associated value later.
        _tcscpy ( szKeyName, szNextKey );

        // See if there is a semicolon in the key.  If there is, then the
        // current key is a MIME type with extensions.
        if ( NULL != ( szExtList = _tcschr ( szNextKey, ';' ) ) )
            {
            // There was a semicolon.  Pick off the MIME type and the
            // extensions.
            bHasExtensions = TRUE;
            *szExtList = '\0';
            szExtList = _tcsinc ( szExtList );

            _tcscpy ( szMIMEType, szNextKey );
            _tcscpy ( szExtensions, szExtList );
            }
        else
            {
            _tcscpy ( szMIMEType, szNextKey );
            }

        // Now go back and grab the YES/NO value from the INI file.
        GetPrivateProfileString ( cstrINISectionName,
                                  szKeyName,
                                  _T("**"),    // sentinel
                                  szValue,
                                  sizeof(szValue) / sizeof(TCHAR),
                                  g_cstrINIFilename );

        if ( 0 == _tcscmp ( szValue, _T("**") ) )
            {
            // Something's way hosed in the INI file, since GPPS() didn't
            // find anything.
            return FALSE;
            }

        try
            {
            // Add the MIME type to the list of MIME types.
            m_acstrMIMETypes.Add ( szMIMEType );

            if ( bHasExtensions )
                {
                m_acstrMIMETypesWithExtensions.Add ( szMIMEType );
                m_acstrExtensions.Add ( szExtList );
                }

            if ( g_bInstallMode )
                {
                // Now set the default check value for install mode
                if ( 0 == cstrYes.CompareNoCase ( szValue ) )
                    {
                    // This MIME type will be checked by default.

                    m_abyDefaults.Add ( TRUE );
                    }
                else
                    {
                    // This MIME type will be unchecked by default.

                    m_abyDefaults.Add ( FALSE );
                    }
                }
            }
        catch ( CMemoryException* e )
            {
            AfxMessageBox ( IDS_MEM_ALLOC_ERROR );
            e->Delete();
            return FALSE;
            }

        // Set variables for next iteration
        szNextKey = _tcsinc ( _tcschr ( szNextKey, '\0' ) );

        if ( bHasExtensions )
            {
            // We need to do another seek past '\0' again since if there was
            // an extension list, a null was inserted where the semicolon was.

            szNextKey = _tcsinc ( _tcschr ( szNextKey, '\0' ) );
            }

        bHasExtensions = FALSE;
        }   // end while

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::InitArraysWithHardCodedStrings()
//
// Description:
//  This fn is a fallback in case the config file is missing or corrupted.
//  It initializes the member CStringArray objects with hard-coded
//  values.
//
// Input:
//  Nothing (operates on member variables).
//
// Returns:
//  TRUE if everything went OK, FALSE if an error occured.  If this fn
//  returns FALSE, that's bad and the app should not continue.
//
// Notes:
//  The values below will have to be changed if we ever change the MIME
//  types and/or extensions that NAV scans.
//
//////////////////////////////////////////////////////////////////////////
// 5/14/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::InitArraysWithHardCodedStrings()
{
BOOL bRetVal = TRUE;
int  nIndex;
int  nMaxIndex;

    try
        {
        m_acstrMIMETypes.Add ( _T("application/binary") );
        m_acstrMIMETypes.Add ( _T("application/excel") );
        m_acstrMIMETypes.Add ( _T("application/msexcel") );
        m_acstrMIMETypes.Add ( _T("application/msword") );
        m_acstrMIMETypes.Add ( _T("application/octet-stream") );
        m_acstrMIMETypes.Add ( _T("application/octet-string") );
        m_acstrMIMETypes.Add ( _T("application/word") );
        m_acstrMIMETypes.Add ( _T("application/x-binary") );
        m_acstrMIMETypes.Add ( _T("application/x-compressed") );
        m_acstrMIMETypes.Add ( _T("application/x-excel") );
        m_acstrMIMETypes.Add ( _T("application/x-lha") );
        m_acstrMIMETypes.Add ( _T("application/x-lha-compressed") );
        m_acstrMIMETypes.Add ( _T("application/x-lzh") );
        m_acstrMIMETypes.Add ( _T("application/x-lzh-compressed") );
        m_acstrMIMETypes.Add ( _T("application/x-msdownload") );
        m_acstrMIMETypes.Add ( _T("application/x-msexcel") );
        m_acstrMIMETypes.Add ( _T("application/x-msword") );
        m_acstrMIMETypes.Add ( _T("application/x-winexe") );
        m_acstrMIMETypes.Add ( _T("application/x-word") );
        m_acstrMIMETypes.Add ( _T("application/x-zip") );
        m_acstrMIMETypes.Add ( _T("application/x-zip-compressed") );
        m_acstrMIMETypes.Add ( _T("application/zip") );
        m_acstrMIMETypes.Add ( _T("file/executable") );

        // Init defaults array to TRUE for all the above types.

        nMaxIndex = m_acstrMIMETypes.GetUpperBound();

        for ( nIndex = 0; nIndex <= nMaxIndex; nIndex++ )
            {
            m_abyDefaults.Add ( TRUE );
            }

        m_acstrMIMETypesWithExtensions.Add ( _T("application/msexcel") );
        m_acstrMIMETypesWithExtensions.Add ( _T("application/msword") );
        m_acstrMIMETypesWithExtensions.Add ( _T("application/octet-stream") );
        m_acstrMIMETypesWithExtensions.Add ( _T("application/zip") );

        m_acstrExtensions.Add ( _T("xlb,xlm,xls,xlt,xlw") );
        m_acstrExtensions.Add ( _T("doc,dot") );
        m_acstrExtensions.Add ( _T("386,adt,bin,cbt,cla,com,cpl,dll,drv,exe,ovl,ppt,scr,sys") );
        m_acstrExtensions.Add ( _T("zip,lha,lzh,arj,mim,mme,uue") );
        }
    catch ( CMemoryException )
        {
        bRetVal = FALSE;
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::FillListbox()
//
// Description:
//  This fn fills the MIME type listbox with the types that NAV can
//  scan.  For each type, the state of its checkbox is also set -- checked
//  if NAV is currently set to scan the type, or unchecked if not (unless
//  overridden by command-line switches).
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if everything went OK.
//  FALSE if not.  If this fn returns FALSE, something very bad occured
//      and the app should not continue.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::FillListbox()
{
BOOL bRetVal = TRUE;
int  nCurrIndex;
int  nMaxIndex;
int  nItemState;
int  nResult;

    nMaxIndex = m_acstrMIMETypes.GetUpperBound();

    if ( nMaxIndex == -1 )
        {
        if ( ! g_bSilentMode  &&
             ! g_bUninstallMode  )
            {
            AfxMessageBox ( IDS_NO_MIME_TYPES_IN_ARRAY,
                            MB_ICONSTOP | MB_OK );

            return FALSE;
            }
        }

    for ( nCurrIndex = 0;
          bRetVal  &&  nCurrIndex <= nMaxIndex;
          nCurrIndex++ )
        {
        // First, add in the next MIME type to the listbox.
        nResult = m_checklist.AddString ( m_acstrMIMETypes [ nCurrIndex ] );

        if ( nResult != LB_ERR  &&  nResult != LB_ERRSPACE )
            {
            // Next, check if NAV is associated with the current MIME
            // type, and set the state of the checkbox accordingly.
            // The status of the item is stored in the DWORD provided
            // by the listbox control (CListBox::SetItemData()).
            nItemState = GetPluginItemState ( m_acstrMIMETypes [ nCurrIndex ] );

            // Check for an error return from GetPluginItemState.  HA_ERROR
            // indicates that the Netscape helper app section of the
            // registry is hosed.
            if ( nItemState == HA_ERROR )
                {
                AfxMessageBox ( IDP_GET_PLUGIN_STATE_ERROR,
                                MB_OK | MB_ICONSTOP );

                bRetVal = FALSE;
                }
            else
                {
                // Set the check state.  If we're in install mode, get this
                // value from the array of defaults.  Otherwise, make it
                // checked iff NAV is the helper app.
                if ( g_bInstallMode )
                    {
                    m_checklist.SetCheck ( nCurrIndex,
                                           m_abyDefaults[nCurrIndex] );
                    }
                else
                    {
                    m_checklist.SetCheck ( nCurrIndex,
                                           ( nItemState == HA_NAV ) ? 1 : 0 );
                    }

                m_checklist.SetItemData ( nCurrIndex, (DWORD) nItemState );
                }
            }
        else
            {
            // The AddString() call failed, which is a pretty
            // catastrophic error.
            bRetVal = FALSE;
            }
        }   // end for

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::GetPluginItemState()
//
// Description:
//  This fn checks the Netscape plug-in key for a given MIME type and
//  returns a constant from the HA_* enum indicating the status of
//  that MIME type's Netscape helper app.
//
// Input:
//  szMIMEType : The name of the MIME type.
//
// Returns:
//  A constant from the HA_* enum indicating the status of the helper
//  app for the passed-in MIME type.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

int CNPlugInDlg::GetPluginItemState ( LPCTSTR szMIMEType )
{
int   nRetVal = HA_ERROR;
int   nGetHelperAppRet;
TCHAR szCurrHelper [ MAX_PATH + 32 ];   // +32 to allow for switches

    // Get the current helper app associated with the specified MIME type.
    nGetHelperAppRet = GetHelperAppString ( szMIMEType, szCurrHelper );

    if ( nGetHelperAppRet == GHA_OK )
        {
        // Lowercase the string for easier searching.
        _tcslwr ( szCurrHelper );

        if ( _tcslen ( szCurrHelper ) == 0 )
            {
            nRetVal = HA_EMPTY;
            }
        else
            {
            // Search for our "/download" switch.  If it's there, NAV is
            // the helper app.  Note that I'm not checking for a situation
            // where some values are set to, say, NAVWNT.EXE and others to
            // N32SCANW.EXE.  That'll never happen under normal circumstances
            // anyway.

            if ( NULL != _tcsstr ( szCurrHelper, _T("/download") ) )
                {
                nRetVal = HA_NAV;
                }
            else
                {
                nRetVal = HA_OTHER;
                }
            }
        }
    else if ( nGetHelperAppRet == GHA_NO_VIEWER_VALUE )
        {
        // There was no viewer assigned to the MIME type.
        nRetVal = HA_NONE;
        }
    else if ( nGetHelperAppRet == GHA_NO_VIEWERS_KEY )
        {
        // If there's no Viewers key, the Netscape install is munged or
        // it was badly uninstalled.  There isn't much we can do about
        // it though, except return an error.
        nRetVal = HA_ERROR;
        }
    else
        {
        ASSERT ( FALSE );               // Bad return from
                                        // GetHelperAppString()
        }

    ASSERT ( nRetVal >= HA_ERROR  &&  nRetVal <= HA_EMPTY );

    return nRetVal;
}


/////////////////////////////////////////////////////////////////////////////
// Other CNPlugInDlg helper functions


//////////////////////////////////////////////////////////////////////////
//
// Function:    GetNetscapeViewersKey()
//
// Description:
//  Returns an HKEY opened to the Netscape Viewers key.
//
// Input:
//  bWriteAccess: Flag specifying whether the calling function wants to
//                open the key with write access.
//                  TRUE for write and read access,
//                  FALSE for only read access.
//
// Returns:
//  An HKEY that the calling function can use to access the Viewers data.
//      The calling code is responsible for closing this HKEY.
//  NULL if the Viewers key could not be opened.
//
//////////////////////////////////////////////////////////////////////////
// 2/24/97  Mike Dunn -- function created
// 6/6/97  Mike Dunn -- changed to use RegCreateKeyEx() so that the key
//                      will be created if it isn't already in the registry.
//////////////////////////////////////////////////////////////////////////

HKEY CNPlugInDlg::GetNetscapeViewersKey ( BOOL bWriteAccess )
{
HKEY  hkeyRetVal;
DWORD dwDisposition;                    // not used, but needed by
                                        // RegCreateKeyEx()

    if ( ERROR_SUCCESS == RegCreateKeyEx
                              ( HKEY_CURRENT_USER,
                                _T("Software\\Netscape\\Netscape Navigator\\Viewers"),
                                0, _T(""),
                                REG_OPTION_NON_VOLATILE,
                                bWriteAccess ? KEY_READ | KEY_WRITE : KEY_READ,
                                NULL, &hkeyRetVal, &dwDisposition ) )
        {
        return hkeyRetVal;
        }
    else
        {
        return NULL;
        }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::GetNAVInstallDir()
//
// Description:
//  This fn reads the NAV install dir from the registry and copies it
//  into a supplied buffer.
//
// Input:
//  szNAVDir: A TCHAR buffer of at least MAX_PATH characters.  If the
//            fn succeedes, the NAV install dir is copied into this buffer.
//
// Returns:
//  TRUE if the directory was read and copied into the buffer.
//  FALSE if the directory could not be read from the registry.  In this
//      case, the buffer is not changed.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
// 6/9/97  Mike Dunn -- changed to return TRUE immediately if the app is
//                      in forced-uninstall mode.
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::GetNAVInstallDir ( LPTSTR szNAVDir )
{
BOOL  bRetVal = FALSE;
HKEY  hkeyInstalledApps;
TCHAR szBuffer [ MAX_PATH ];
DWORD dwBufferSize = sizeof ( szBuffer );

    if ( g_bForcedUninstallMode )
        {
        // We don't need to do anything here in forced uninstall mode
        // because the plug-in reg entries are being removed.

        return TRUE;
        }


    // First open the InstalledApps key.
    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("Software\\Symantec\\InstalledApps"),
                                         0, KEY_READ,
                                         &hkeyInstalledApps ) )
        {
        // Next query the NAVNT/NAV95 reg value.  This holds the install
        // directory.
        if ( ERROR_SUCCESS == RegQueryValueEx ( hkeyInstalledApps,
                                                g_bOSIsNT ? _T("NAVNT")
                                                          : _T("NAV95"),
                                                0, NULL,
                                                (LPBYTE) szBuffer,
                                                &dwBufferSize ) )
            {
            // Copy the install dir into the supplied buffer.
            _tcscpy ( szNAVDir, szBuffer );

            bRetVal = TRUE;
            }
        else
            {
            // The NAVNT/NAV95 value couldn't be read.
            }

        RegCloseKey ( hkeyInstalledApps );
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::GetHelperAppString()
//
// Description:
//  This fn retrieves the Netscape helper app string associated with
//  the specified MIME type.
//
// Input:
//  szMIMEType: The name of the MIME type.
//  szHelperAppString: A TCHAR buffer of at least MAX_PATH characters,
//                     which will receive the helper app string.
//
// Returns:
//  A member of the GHA_* enum indicating success or failure.  On success,
//      the current string is written to szHelperAppString.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

int CNPlugInDlg::GetHelperAppString ( LPCTSTR szMIMEType,
                                      LPTSTR  szHelperAppString )
{
int   nRetVal = GHA_OK;
HKEY  hkeyHelperApps;
TCHAR szBuffer [ MAX_PATH + 32 ];
DWORD dwBufferSize = sizeof ( szBuffer );

    // First open the Viewers (helper apps) key.

    if ( hkeyHelperApps = GetNetscapeViewersKey ( FALSE ) )
        {
        // Next query the MIME type value.  This value conveniently has
        // the same name as the MIME type.

        if ( ERROR_SUCCESS == RegQueryValueEx ( hkeyHelperApps,
                                                szMIMEType,
                                                0, NULL,
                                                (LPBYTE) szBuffer,
                                                &dwBufferSize ) )
            {
            // Success!  Copy the string back to the supplied buffer.
            _tcscpy ( szHelperAppString, szBuffer );
            }
        else
            {
            // Couldn't read the string reg value.
            nRetVal = GHA_NO_VIEWER_VALUE;
            }

        RegCloseKey ( hkeyHelperApps );
        }
    else
        {
        // Couldn't open the Viewers key.
        nRetVal = GHA_NO_VIEWERS_KEY;
        }

    ASSERT ( nRetVal >= GHA_OK  &&  nRetVal <= GHA_NO_VIEWER_VALUE );

    return nRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::SetHelperAppString()
//
// Description:
//  Sets a Netscape helper app string to a specified string.
//
// Input:
//  szMIMEType: The name of the MIME type whose helper app will be set.
//  szNewString: The string to be stored in the registry.
//
// Returns:
//  TRUE if the value was set OK.
//  FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::SetHelperAppString ( LPCTSTR szMIMEType,
                                       LPCTSTR szNewString )
{
BOOL  bRetVal = FALSE;
HKEY  hkeyHelperApps;
DWORD dwStringSize = sizeof ( TCHAR ) * ( 1 + _tcslen ( szNewString ) );

    if ( hkeyHelperApps = GetNetscapeViewersKey ( TRUE ) )
        {
        // Next set the MIME type helper app string value.  This value
        // conveniently has the same name as the MIME type.

        if ( ERROR_SUCCESS == RegSetValueEx ( hkeyHelperApps,
                                              szMIMEType,
                                              0,
                                              REG_SZ,
                                              (LPBYTE) szNewString,
                                              dwStringSize ) )
            {
            // Success!
            bRetVal = TRUE;
            }

        RegCloseKey ( hkeyHelperApps );
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::SetTrustedAppValue()
//
// Description:
//  Sets NAV as a Netscape trusted external application.
//  This removes the ReallyOpenUsingNAV? prompt in version 4.0x.
//
// Input:
//  szNavCmdLine: Exact command line NAV registers as a helper app.
//
// Returns:
//  TRUE if the value was set OK.
//  FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 7/25/1997  DALLEE -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::SetTrustedAppValue ( LPCTSTR szNavCmdLine )
{
BOOL  bRetVal = FALSE;
HKEY  hkeyTrustedApps;
DWORD dwDisposition;
DWORD dwStringSize;

    if ( ERROR_SUCCESS == RegCreateKeyEx
                              ( HKEY_CURRENT_USER,
                                _T("Software\\Netscape\\Netscape Navigator\\User Trusted External Applications"),
                                0, _T(""),
                                REG_OPTION_NON_VOLATILE,
                                KEY_WRITE, NULL,
                                &hkeyTrustedApps,
                                &dwDisposition ) )
        {
        // Value name for trusted apps must be the exact command line.
        // Data should be "Yes".

        dwStringSize = sizeof ( _T("Yes") );

        if ( ERROR_SUCCESS == RegSetValueEx ( hkeyTrustedApps,
                                              szNavCmdLine,
                                              0,
                                              REG_SZ,
                                              (LPBYTE) _T("Yes"),
                                              dwStringSize ) )
            {
            // Success!
            bRetVal = TRUE;
            }

        RegCloseKey ( hkeyTrustedApps );
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::SetExtensionString()
//
// Description:
//  Sets or deletes a Netscape extension list string.
//
// Input:
//  szMIMEType: The name of the MIME type whose extension list will be set.
//  bAddExtensions:  Flag - TRUE means add the extensions list to the
//                          registry,
//                          FALSE means remove the list.
//
// Returns:
//  TRUE if the value was set or deleted OK.
//  FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::SetExtensionString ( LPCTSTR szMIMEType,
                                       BOOL bAddExtensions )
{
BOOL  bRetVal = FALSE;
BOOL  bMatchFound = FALSE;
int   nIndex;
int   nMaxIndex;
HKEY  hkeySuffixes;
DWORD dwDisposition;                    // not used, but needed by
                                        // RegCreateKeyEx()

    nMaxIndex = m_acstrMIMETypesWithExtensions.GetUpperBound();

    // Search through the list of MIME types that have extensions.  If
    // szMIMEType is in the list, get the list of extensions and write
    // it to the registry.

    for ( nIndex = 0;
          !bRetVal  &&  nIndex <= nMaxIndex;
          nIndex++ )
        {
        // Compare szMIMEType against the next one in the member array
        // m_acstrMIMETypesWithExtensions[]

        if ( 0 == _tcsicmp ( szMIMEType,
                             m_acstrMIMETypesWithExtensions [ nIndex ] ) )
            {
            // Match!
            bMatchFound = TRUE;

            // Fix for STS 85772 -- call RegCreateKeyEx() instead of
            // RegOpenKeyEx() so that the Suffixes key will be created
            // if it doesn't already exist.  This case can happen if there
            // are no MIME types with associated extensions in the user's
            // list of helper apps.  I did a similar change to
            // GetNetscapeViewersKey().

            if ( ERROR_SUCCESS == RegCreateKeyEx
                                      ( HKEY_CURRENT_USER,
                                        _T("Software\\Netscape\\Netscape Navigator\\Suffixes"),
                                        0, _T(""),
                                        REG_OPTION_NON_VOLATILE,
                                        KEY_WRITE, NULL,
                                        &hkeySuffixes,
                                        &dwDisposition ) )
                {
                if ( bAddExtensions )
                    {
                    // Create the list of extensions.
                    if ( ERROR_SUCCESS == RegSetValueEx
                                              ( hkeySuffixes,
                                                szMIMEType,
                                                0, REG_SZ,
                                                (LPBYTE) (LPCTSTR) m_acstrExtensions [ nIndex ],
                                                sizeof(TCHAR) * ( 1 + _tcslen ( m_acstrExtensions [ nIndex ] ) ) ) )
                        {
                        bRetVal = TRUE;
                        }
                    }
                else
                    {
                    // Delete the string that holds the extension list.
                    // I don't check the return value since it's possible that
                    // the suffixes key or the value might not exist.  If that's
                    // the case, RDV() will return an error but for us it's not
                    // an error condition.  (fixes 85772)
                    RegDeleteValue ( hkeySuffixes, szMIMEType );

                    bRetVal = TRUE;      // assume all's well
                    }

                RegCloseKey ( hkeySuffixes );
                }
            }
        }   // end for

    // One last check here... if szMIMEType didn't have an extension
    // list, then we need to return success, since we didn't have
    // anything to do.
    if ( ! bMatchFound )
        {
        bRetVal = TRUE;
        }

    return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::CtrlHasContextHelp()
//
// Description:
//  Checks the passed-in control ID and returns whether that control
//  can give context help.
//
// Input:
//  nCtrlID: The ID of the control to check.
//
// Returns:
//  TRUE if the control can give context help.
//  FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// 2/21/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::CtrlHasContextHelp ( int nCtrlID )
{
int nIndex;

    // Check the dialog's array of controls that can give context help.
    // If a match is found, exit immediately.

    for ( nIndex = 0; nIndex < NUM_CONTEXT_HELP_CTRLS; nIndex++ )
        {
        if ( nCtrlID == m_anContextHelpCtrlIDs [ nIndex ] )
            {
            return TRUE;
            }
        }

    // If we get here, the ID wasn't in the member array.

    return FALSE;
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::CreateNetscapeViewerValues()
//
// Description:
//  Creates string values corresponding to the specified MIME type.
//  This fn can create either the MIME type string, the TYPExx string,
//  or both.
//
// Input:
//  szMIMEType:            The name of the MIME type.
//  bCreateMIMETypeString: Flag specifying whether to create a string value
//                         with the same name as the MIME type.
//  bCreateTYPExxString:   Flag specifying whether to create a new TYPExx
//                         string with data equal to the MIME type name.
//
// Returns:
//  TRUE if the values were created OK.
//  FALSE if an error occured while creating the values.
//
// Notes:
//  * This fn does not check that the MIME type value already exists -- if
//      it exists, it is overwritten.
//  * Passing FALSE for both BOOL parameters will make this fn return TRUE.
//
//////////////////////////////////////////////////////////////////////////
// 2/24/97  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::CreateNetscapeViewerValues ( LPCTSTR szMIMEType,
                                               BOOL    bCreateMIMETypeString,
                                               BOOL    bCreateTYPExxString )
{
BOOL  bMIMETypeStringCreated = FALSE;
BOOL  bTYPExxStringCreated   = FALSE;
HKEY  hkeyViewers;
TCHAR szValueName [ 64 ];
TCHAR szValueData [ MAX_PATH + 32 ];
DWORD dwValueNameSize;
DWORD dwValueDataSize;
DWORD dwValueType;
DWORD dwIndex = 0;
int   nTypeNumber;

    if ( hkeyViewers = GetNetscapeViewersKey ( TRUE ) )
        {
        if ( bCreateMIMETypeString )
            {
            // Create an empty string value.
            if ( ERROR_SUCCESS == RegSetValueEx ( hkeyViewers,
                                                  szMIMEType,
                                                  0, REG_SZ,
                                                  (LPBYTE) _T(""),
                                                  sizeof (TCHAR) ) )
                {
                bMIMETypeStringCreated = TRUE;
                }
            }
        else
            {
            bMIMETypeStringCreated = TRUE;
                                        // Return success if the user
                                        // didn't request that we create a
                                        // MIME type value.
            }

        if ( bCreateTYPExxString )
            {
            // Enumerate the TYPExx values and find the first unused
            // TYPExx value.
            // If we find a TYPExx along the way whose string data matches
            // the current MIME type, we can stop and return success.

            // Reset variables.
            nTypeNumber = 0;
            dwValueNameSize = 64;
            dwValueDataSize = sizeof ( szValueData );

            for (;;)                    // using break statements
                                        // to end this loop
                {
                _stprintf ( szValueName, _T("TYPE%d"), nTypeNumber );

                // Check if a value with this name exists. If not, we
                // can use it.  If it does exist, check if its string
                // data matches the MIME type.  If they do match, we
                // can quit and return success.

                if ( ERROR_SUCCESS == RegQueryValueEx ( hkeyViewers,
                                                        szValueName,
                                                        0, &dwValueType,
                                                        (LPBYTE) szValueData,
                                                        &dwValueDataSize ) )
                    {
                    // The value existed, so check its contents against
                    // szMIMEType.

                    if ( 0 == _tcsicmp ( szValueData, szMIMEType ) )
                        {
                        // Match!
                        bTYPExxStringCreated = TRUE;
                        break;          // exit the for loop
                        }
                    }
                else
                    {
                    // We found an unused TYPExx value.  Store the current
                    // MIME type in the value.
                    if ( ERROR_SUCCESS == RegSetValueEx ( hkeyViewers,
                                                          szValueName,
                                                          0, REG_SZ,
                                                          (LPBYTE) szMIMEType,
                                                          sizeof(TCHAR) * ( 1 + _tcslen ( szMIMEType ) ) ) )
                        {
                        bTYPExxStringCreated = TRUE;
                        break;          // exit the for loop
                        }
                    else
                        {
                        break;          // couldn't set the reg value, so
                                        // bail out of the for loop with
                                        // bTYPExxStringCreated still FALSE.
                        }
                    }

                // Reset variables for next iteration.
                nTypeNumber++;
                dwValueNameSize = 64;
                dwValueDataSize = sizeof ( szValueData );
                }   // end for (;;)
            }   // end if ( bCreateTYPExxString )

        RegCloseKey ( hkeyViewers );
        }   // end if ( opened Viewers key )

    return bMIMETypeStringCreated && bTYPExxStringCreated;
}


/////////////////////////////////////////////////////////////////////////////
// CNPlugInDlg command message handlers

void CNPlugInDlg::OnCheckAll()
{
int nIndex;
int nMaxIndex;

    nMaxIndex = m_acstrMIMETypes.GetUpperBound();

    for ( nIndex = 0; nIndex <= nMaxIndex; nIndex++ )
        {
        m_checklist.SetCheck ( nIndex, 1 );
        }
}

void CNPlugInDlg::OnUncheckAll()
{
int nIndex;
int nMaxIndex;

    nMaxIndex = m_acstrMIMETypes.GetUpperBound();

    for ( nIndex = 0; nIndex <= nMaxIndex; nIndex++ )
        {
        m_checklist.SetCheck ( nIndex, 0 );
        }
}

// Handler for WM_CONTEXTMENU, which is sent to the dialog when the user
// right-clicks in the dialog window.
void CNPlugInDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
CPoint ptClient = point;
CWnd*  pwndRightClickedCtrl;
int    nRightClickedCtrlID;
CMenu  cmenuContextMenu;

//&^ Help is disabled for right now...
CDialog::OnContextMenu(pWnd, point);
return;

    // Determine if the click occured over a control window, and if so,
    // whether that control can give context help.

    ScreenToClient ( &ptClient );

    pwndRightClickedCtrl = ChildWindowFromPoint ( ptClient );

    if ( pwndRightClickedCtrl != NULL )
        {
        nRightClickedCtrlID = pwndRightClickedCtrl -> GetDlgCtrlID();

        if ( CtrlHasContextHelp ( nRightClickedCtrlID ) )
            {
            // Save the control ID for the handler for the What's This?
            // menu item.
            m_nRightClickedCtrlID = nRightClickedCtrlID;

            // Display the popup menu.
            if ( cmenuContextMenu.LoadMenu ( IDR_CONTEXT_MENU ) )
                {
                cmenuContextMenu.GetSubMenu(0) ->
                        TrackPopupMenu ( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                         point.x, point.y, this );
                }
            }
        else
            {
            // The control does not have context help, so pass the
            // message on to the CDialog handler.
            CDialog::OnContextMenu ( pWnd, point );
            }
        }
    else
        {
        // The mouse clicked happend outside the client area of the
        // main dialog.  Pass the message on to the CDialog handler.
        CDialog::OnContextMenu ( pWnd, point );
        }
}


void CNPlugInDlg::OnWhatsThis()
{
    AfxGetApp() -> WinHelp ( HID_BASE_CONTROL + m_nRightClickedCtrlID,
                             HELP_CONTEXTPOPUP );
}


// Handler for the Help pushbutton.
LRESULT CNPlugInDlg::OnCommandHelp ( WPARAM wParam, LPARAM lParam )
{
    AfxGetApp() -> WinHelp ( HID_BASE_RESOURCE + this->IDD );

    return TRUE;                        // TRUE indicates that this fn called
                                        // WinHelp()
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    CNPlugInDlg::OnHelpInfo()
//
// Description:
//  This fn is called by the framework when the user presses F1 or clicks
//  the [?] caption button, then clicks in the dialog window.  This fn
//  determines if the window that the cursor is over can give context
//  help, and if so, displays the help (just like in OnWhatsThis()).
//
// Input:
//  pHelpInfo: Ptr to a struct with various info, the coolest of which being
//             the ID of the control that the user is requesting help on.
//
// Returns:
//  TRUE.  The MSVC docs don't describe the meaning of the BOOL return,
//      but TRUE seems to work fine.
//
//////////////////////////////////////////////////////////////////////////

BOOL CNPlugInDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
//&^ Help is disabled for right now...
return TRUE;

    // Determine if the mouse was over a control window, and if so,
    // whether the control can give context help.

    // Check against HELPINFO_WINDOW to prevent us from doing anything if
    // a menu is up when the user hits F1.  Otherwise they'd get a nasty
    // "No help topic is associated with this item" message.

    if ( pHelpInfo->iContextType == HELPINFO_WINDOW  &&
         CtrlHasContextHelp ( pHelpInfo->iCtrlId )      )
            {
            // Display the help popup.
            AfxGetApp() -> WinHelp ( HID_BASE_CONTROL + pHelpInfo->iCtrlId,
                                     HELP_CONTEXTPOPUP );
            }

    return TRUE;                        // I'm not sure what the return value
                                        // indicates -- it's not described
                                        // in the MSVC docs!  TRUE seems to
                                        // work fine, though.
}


// Handler for the "Save" button.  This does all the work of setting and
// clearing the Netscape helper app keys
void CNPlugInDlg::OnOK()
{
BOOL  bStatus = TRUE;                   // TRUE unless an error occurs while
                                        // this fn is doing its work.

TCHAR szNAVDir [ MAX_PATH ] = _T("");
TCHAR szNAVCommandLine [ MAX_PATH + 32 ];
BOOL  bUseNavAsPluginState;
int   nDlgRetVal = NSP_SAVE;
int   nCurrIndex;
int   nMaxIndex;
int   nItemStatus;
int   nIDSReason;                       // ID of the error message to be
                                        // displayed if an error occurs.

                                        // Get the new checked state of the
                                        // bUseNavAsPlugin.
    bUseNavAsPluginState = ((CButton *)GetDlgItem( IDC_USE_NAV_AS_PLUGIN ))->GetCheck();

                                        // If we are in install mode...
    if ( TRUE == g_bInstallMode )
        {
                                        // If our UseNavAsPlugin is not
                                        // checked...
        if ( FALSE == bUseNavAsPluginState )
            {
                                        // We don't want to process anything
                                        // and add the keys to the registry.
            goto ExitOnOK;
            }
        }

                                        // If we started with the
                                        // UseNavAsPlugin state at false, and
                                        // we ended that way, we should not
                                        // process this OK, and should exit.
    if ( (bUseNavAsPluginState == FALSE) &&
         (g_bOriginalUseNavAsPluginState == FALSE) )
        {
        goto ExitOnOK;
        }

    nMaxIndex = m_checklist.GetCount() - 1;

    ASSERT ( nMaxIndex >= 0 );

    // Build the NAV command line, which is the path to NAVW (in quotes)
    // followed by the "/download" switch.

    if ( GetNAVInstallDir ( szNAVDir ) )
        {
        if ( g_dwOSMajor >= 4 )
            {
            _tcscpy ( szNAVCommandLine, _T("\"") );
            _tcscat ( szNAVCommandLine, szNAVDir );
            _tcscat ( szNAVCommandLine,
                      g_bOSIsNT ? _T("\\navwnt.exe") : _T("\\navw32.exe") );
            _tcscat ( szNAVCommandLine, _T("\" /download") );
            }
        else
            {
            // Klugery for Netscape 4.0 and NT 3.51, Netscape drops parameters
            // when launching helper apps with LFN's in quotes. Use SFN.
            if ( GetShortPathName ( szNAVDir, szNAVCommandLine, MAX_PATH ) )
                {
                _tcscat ( szNAVCommandLine,
                          g_bOSIsNT ? _T("\\navwnt.exe") : _T("\\navw32.exe") );
                _tcscat ( szNAVCommandLine, _T(" /download") );
                }
            else
                {
                bStatus = FALSE;
                nIDSReason = IDP_GET_INSTALL_DIR_ERROR;
                }
            }

        // Loop through the items and set the Netscape reg settings
        // accordingly. If an error occurs along the way we stop immediately.
        // (bStatus is set to FALSE on an error.)

        for ( nCurrIndex = 0;
              bStatus  &&  nCurrIndex <= nMaxIndex;
              nCurrIndex++ )
            {
            nItemStatus = (int) m_checklist.GetItemData ( nCurrIndex );

            switch ( nItemStatus )
                {
                case HA_NONE:
                    {
                    if ( m_checklist.GetCheck ( nCurrIndex ) )
                        {
                        bStatus = CreateNetscapeViewerValues
                                      ( m_acstrMIMETypes [ nCurrIndex ],
                                        TRUE, TRUE );

                        if ( bStatus )
                            {
                            // The string values are in place, now set NAV as
                            // the helper app.
                            bStatus = SetHelperAppString
                                          ( m_acstrMIMETypes [ nCurrIndex ],
                                            szNAVCommandLine );

                            if ( bStatus )
                                {
                                // If this MIME type has extensions, set them
                                // as well.
                                bStatus =
                                    SetExtensionString
                                            ( m_acstrMIMETypes [ nCurrIndex ],
                                              TRUE );

                                if ( bStatus )
                                    {
                                    // Set the item's status data to reflect
                                    // the change.
                                    m_checklist.SetItemData ( nCurrIndex,
                                                              (DWORD) HA_NAV );
                                    }
                                }
                            }

                        if ( !bStatus )
                            {
                            nIDSReason = IDP_NO_REG_WRITE_ACCESS;
                            }
                        }   // end if

                    // If the item was not checked, we have nothing to do.
                    }
                break;

                case HA_NAV:
                    {
                    if ( ! m_checklist.GetCheck ( nCurrIndex ) )
                        {
                        // Clear the helper app string.
                        bStatus = SetHelperAppString
                                          ( m_acstrMIMETypes [ nCurrIndex ],
                                            _T("") );

                        if ( bStatus )
                            {
                            // If this MIME type has extensions, delete
                            // the extensions reg value as well.
                            bStatus =
                                SetExtensionString
                                        ( m_acstrMIMETypes [ nCurrIndex ],
                                          FALSE );

                            if ( bStatus )
                                {
                                // Set the item's status data to reflect the
                                // change.
                                m_checklist.SetItemData ( nCurrIndex,
                                                          (DWORD) HA_EMPTY );
                                }
                            }

                        if ( ! bStatus )
                            {
                            nIDSReason = IDP_NO_REG_WRITE_ACCESS;
                            }
                        }

                    // If the item was checked, there's nothing to do since
                    // NAV was already the helper app for the associated
                    // MIME type.
                    }
                break;

                case HA_OTHER:
                case HA_EMPTY:
                    {
                    if ( m_checklist.GetCheck ( nCurrIndex ) )
                        {
                        // The item was checked, so set NAV as the helper for
                        // the corresponding MIME type.

                        // First create the TYPExx and MIME type string
                        // values if necessary.  If they already exist,
                        // CreateNetscapeViewerValues() will leave them
                        // alone and return success.
                        bStatus = CreateNetscapeViewerValues
                                      ( m_acstrMIMETypes [ nCurrIndex ],
                                        TRUE, TRUE );

                        if ( bStatus )
                            {
                            // The item was checked, so set NAV as the helper for
                            // the corresponding MIME type.
                            bStatus = SetHelperAppString
                                              ( m_acstrMIMETypes [ nCurrIndex ],
                                                szNAVCommandLine );

                            if ( bStatus )
                                {
                                // If this MIME type has extensions, set them
                                // as well.
                                bStatus =
                                    SetExtensionString
                                            ( m_acstrMIMETypes [ nCurrIndex ],
                                              TRUE );

                                if ( bStatus )
                                    {
                                    // Set the item's status data to reflect the
                                    // change.
                                    m_checklist.SetItemData ( nCurrIndex,
                                                              (DWORD) HA_NAV );
                                    }
                                }
                            }

                        if ( ! bStatus )
                            {
                            nIDSReason = IDP_NO_REG_WRITE_ACCESS;
                            }
                        }

                    // NOTE:: I might want to add an "Are you sure?" type
                    // of message if the status is HA_OTHER, so the user
                    // can confirm that s/he wants to clobber the
                    // existing helper app.  It'd be similar to what
                    // our install does, but would list the current helper
                    // app and have a nice "Yes to all" button.

                    // If the item was uncheked, we leave it alone.
                    }
                break;

                default:
                    {
                    ASSERT ( FALSE );   // A bad status value was stored in
                                        // the listbox data item, or the status
                                        // was HA_ERROR.  We should't be
                                        // here in either case.
                    }
                break;
                }   // end switch ( nItemStatus )
            }   // end for
        }   // end if ( GetNavInstallDir succeeded )
    else
        {
        // Error getting the NAV install dir.
        bStatus = FALSE;
        nIDSReason = IDP_GET_INSTALL_DIR_ERROR;
        }

    if ( bStatus )
        {
        int  nIndex;
        int  nNumItemsChecked = 0;

        // Display a msg indicating success if the switches allow us
        // to display UI.

        if ( !g_bSilentMode ||
             ( g_bForcedUninstallMode && !g_bForcedSilentUninstallMode ) )
            {
            AfxMessageBox ( IDP_CHANGES_SAVED,
                            MB_OK | MB_ICONINFORMATION );
            }

        // Construct the dlg's return value.  We loop thru all the
        // items here & see what was checked.

        for ( nIndex = 0; nIndex <= nMaxIndex; nIndex++ )
            {
            if ( m_checklist.GetCheck ( nIndex ) )
                {
                nNumItemsChecked++;
                }
            }

        if ( nNumItemsChecked == 0 )
            {
            nDlgRetVal |= NSP_NONE_CHECKED;
            }
        else
            {
            // If NAV is handling one or types, set NAV as a
            // User Trusted External Application.
            bStatus = SetTrustedAppValue ( szNAVCommandLine );

            nDlgRetVal |= NSP_SOME_OR_ALL_CHECKED;

            if ( nNumItemsChecked == nMaxIndex + 1 )
                {
                nDlgRetVal |= NSP_ALL_CHECKED;
                }
            else
                {
                nDlgRetVal |= NSP_SOME_CHECKED;
                }
            }

        EndDialog ( nDlgRetVal );
        }
    else
        {
        if ( ! g_bSilentMode )
            {
            AfxMessageBox ( nIDSReason, MB_OK | MB_ICONSTOP );
            }

        EndDialog ( NSP_ERROR_SAVING );
        }

ExitOnOK:

    EndDialog ( nDlgRetVal );
}

void CNPlugInDlg::OnCancel()
{
    EndDialog ( NSP_CANCEL );
}

void CNPlugInDlg::OnUseNavAsPlugin()
{
BOOL bUseNAVChecked;

    bUseNAVChecked = ((CButton *)GetDlgItem(IDC_USE_NAV_AS_PLUGIN))->GetCheck();

    if ( TRUE == bUseNAVChecked )
        {
                                        // If we set the check box to on,
                                        // select all mime types.
        OnCheckAll();
        GetDlgItem( IDC_MIME_LIST )->EnableWindow( TRUE );
        GetDlgItem( IDC_CHECK_ALL )->EnableWindow( TRUE );
        GetDlgItem( IDC_UNCHECK_ALL )->EnableWindow( TRUE );
        }
    else
        {
                                        // If we set the check box to off,
                                        // unselect all mime types.
        OnUncheckAll();
        GetDlgItem( IDC_MIME_LIST )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHECK_ALL )->EnableWindow( FALSE );
        GetDlgItem( IDC_UNCHECK_ALL )->EnableWindow( FALSE );
        }
}

void CNPlugInDlg::UpdateUseNAVCheckStatus()
{
BOOL    bAtLeastOneChecked=FALSE;
int     nLoop;
int     nMimeTypeCount;
CCheckListBox *lpcCheckListControl;

    lpcCheckListControl = (CCheckListBox *)GetDlgItem( IDC_MIME_LIST );

    nMimeTypeCount = lpcCheckListControl->GetCount();
    for ( nLoop=0; nLoop < nMimeTypeCount; nLoop++ )
        {
        if ( lpcCheckListControl->GetCheck( nLoop ) )
            {
                                        // If at least one is set, set
                                        // our flag to true.
            bAtLeastOneChecked = TRUE;
            }
        }

    if ( bAtLeastOneChecked == TRUE )
        {
        ((CButton *)GetDlgItem(IDC_USE_NAV_AS_PLUGIN))->SetCheck( TRUE );
        }
    else
        {
                                        // If nothing is checked, disable
                                        // the list.
        GetDlgItem( IDC_MIME_LIST )->EnableWindow( FALSE );
        GetDlgItem( IDC_CHECK_ALL )->EnableWindow( FALSE );
        GetDlgItem( IDC_UNCHECK_ALL )->EnableWindow( FALSE );
        }
}
