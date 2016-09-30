/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/GenOptsPage.cpv   1.5   09 Jun 1998 23:10:22   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// GenOptsPage.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/GenOptsPage.cpv  $
// 
//    Rev 1.5   09 Jun 1998 23:10:22   DBuches
// Fixed problem with enabling browser button.
// 
//    Rev 1.4   21 May 1998 11:29:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.3   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.2   19 May 1998 13:40:58   DBuches
// Fixed problem with returning from browse dialog.  Added Call to UpdateData().
// 
//    Rev 1.1   08 May 1998 17:24:26   DBuches
// Added browse button.
// 
//    Rev 1.0   03 Apr 1998 13:30:06   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "GenOptsPage.h"
#include "password.h"
#include "netbrowsedlg.h"
#include "helpdlg.h"
#include "qconhlp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenOptsPage property page

// Help maps
DWORD CGenOptsPage::m_dwWhatsThisMap[] = {
    IDC_SCAN_PROMPT_CHECK,      IDH_QUARAN_OPTIONS_GENERAL_PROMPT_FOR_SCAN_CHKBX,   
    IDC_FORWARDING_CHECK,       IDH_QUARAN_OPTIONS_GENERAL_ENABLE_FORWARDING_CHKBX, 
    IDC_SERVER_NAME_STATIC,     IDH_QUARAN_OPTIONS_GENERAL_SERVER_NAME_TXTBX,
    IDC_BROWSE_BUTTON,          IDH_QUARAN_OPTIONS_GENERAL_BROWSER,
    IDC_PASSWORD_CHECK,         IDH_QUARAN_OPTIONS_GENERAL_ENABLE_PASSWORD_CHKBX,
    IDC_PASSWORD_BUTTON,        IDH_QUARAN_OPTIONS_GENERAL_SET_PASSWORD_BTN,
    0,0};

DWORD CGenOptsPage::m_dwHowToMap[] = { 0,0};

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGenOptsPage, CHelperPage)

CGenOptsPage::CGenOptsPage() : CHelperPage(CGenOptsPage::IDD)
{
	//{{AFX_DATA_INIT(CGenOptsPage)
	m_sServerName = _T("");
	m_bEnableForward = FALSE;
	m_bEnblePassword = FALSE;
	m_bScanPrompt = FALSE;
	//}}AFX_DATA_INIT

    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpHowToMap = m_dwHowToMap;
    m_dwHelpButtonHelpID = IDH_QUARAN_OPTIONS_GENERAL_HLPBTN;

    m_psp.dwFlags |= PSP_HASHELP;
}

CGenOptsPage::~CGenOptsPage()
{
}

void CGenOptsPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenOptsPage)
	DDX_Text(pDX, IDC_EDIT1, m_sServerName);
	DDV_MaxChars(pDX, m_sServerName, 32);
	DDX_Check(pDX, IDC_FORWARDING_CHECK, m_bEnableForward);
	DDX_Check(pDX, IDC_PASSWORD_CHECK, m_bEnblePassword);
	DDX_Check(pDX, IDC_SCAN_PROMPT_CHECK, m_bScanPrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenOptsPage, CHelperPage)
	//{{AFX_MSG_MAP(CGenOptsPage)
	ON_BN_CLICKED(IDC_FORWARDING_CHECK, OnForwardingCheck)
	ON_BN_CLICKED(IDC_PASSWORD_CHECK, OnPasswordCheck)
	ON_BN_CLICKED(IDC_PASSWORD_BUTTON, OnPasswordButton)
    ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenOptsPage message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CGenOptsPage::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG message
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CGenOptsPage::OnInitDialog() 
{
	CHelperPage::OnInitDialog();
	
	// Move data to controls
    m_pOpts->GetScanAtStartup( &m_bScanPrompt );
    m_pOpts->GetForwardingEnabled( &m_bEnableForward );
    m_pOpts->GetPasswordEnabled( &m_bEnblePassword );
    m_pOpts->GetPassword( m_sPassword.GetBuffer( 32 ), 32 );
    m_sPassword.ReleaseBuffer();
    m_pOpts->GetQuarantineServer( m_sServerName.GetBuffer(32), 32 );
    m_sServerName.ReleaseBuffer();

    UpdateData( FALSE );
    
    // Set up icon for browse button
    HICON hIcon = (HICON) LoadImage( AfxFindResourceHandle( MAKEINTRESOURCE(IDI_NETBROWSE), RT_GROUP_ICON ),
                             MAKEINTRESOURCE( IDI_NETBROWSE ),
                             IMAGE_ICON,
                             16,
                             16,
                             0 );
    ((CButton*)GetDlgItem(IDC_BROWSE_BUTTON))->SetIcon( hIcon );
    
    // Setup control states.
    if( m_bEnableForward == FALSE )
        {
        GetDlgItem( IDC_EDIT1 )->EnableWindow( FALSE );
        GetDlgItem( IDC_SERVER_NAME_STATIC )->EnableWindow( FALSE );
        GetDlgItem( IDC_BROWSE_BUTTON )->EnableWindow( FALSE );
        }

    if( m_bEnblePassword == FALSE )
        {
        GetDlgItem( IDC_PASSWORD_BUTTON )->EnableWindow( FALSE );
        }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CGenOptsPage::OnForwardingCheck
//
// Description	    : Enables controls when neccessary.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CGenOptsPage::OnForwardingCheck() 
{
	BOOL bEnabled = IsDlgButtonChecked( IDC_FORWARDING_CHECK );
    
    GetDlgItem( IDC_EDIT1 )->EnableWindow( bEnabled );
    GetDlgItem( IDC_SERVER_NAME_STATIC )->EnableWindow( bEnabled );
    GetDlgItem(IDC_BROWSE_BUTTON)->EnableWindow( bEnabled );
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CGenOptsPage::OnPasswordCheck
//
// Description	    : Enables controls when neccessary.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CGenOptsPage::OnPasswordCheck() 
{
	BOOL bEnabled = IsDlgButtonChecked( IDC_PASSWORD_CHECK );
    
    GetDlgItem( IDC_PASSWORD_BUTTON )->EnableWindow( bEnabled );
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CGenOptsPage::OnOK
// Description	    : Handles OK button click
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CGenOptsPage::OnOK() 
{
	// Get data from controls
    UpdateData();

    m_pOpts->SetScanAtStartup( m_bScanPrompt );
    m_pOpts->SetPasswordEnabled( m_bEnblePassword );
    m_pOpts->SetForwardingEnabled( m_bEnableForward );
    m_pOpts->SetQuarantineServer( m_sServerName.GetBuffer(0) );
	m_pOpts->SetPassword( m_sPassword.GetBuffer(0) );

	CHelperPage::OnOK();
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CGenOptsPage::OnKillActive
// Description	    : Performs data validation on server name 
// Return type		: BOOL 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CGenOptsPage::OnKillActive() 
{
    // Gather data.
    UpdateData();

    // If the user wants forwarding, make sure there is a server name
    if( m_bEnableForward && m_sServerName.GetLength() == 0 )
        {
        AfxMessageBox( IDS_ERROR_NO_SERVER_NAME, MB_ICONEXCLAMATION | MB_OK );
        GetDlgItem( IDC_EDIT1 )->SetFocus();
        return FALSE;
        }
	
	return CHelperPage::OnKillActive();
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CGenOptsPage::OnPasswordButton
// Description	    : Handles Password button click.
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CGenOptsPage::OnPasswordButton() 
{
	// TODO: Add your control notification handler code here
    CPasswordChangeDlg dlg;

    dlg.m_sPassword = m_sPassword;

    if( dlg.DoModal() != IDOK )
        return;

    // Save off password.
    m_sPassword = dlg.m_sPassword;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CGenOptsPage::OnBrowseButton
//
// Description	    : Handles Browse button click.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/8/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CGenOptsPage::OnBrowseButton() 
{
    // Fire off dialog
    CNetBrowseDlg dlg;
    if( dlg.DoModal() != IDOK )
        return;

    // Update window text.
    UpdateData( TRUE );
    m_sServerName = dlg.m_sServer;
    UpdateData( FALSE );
}


