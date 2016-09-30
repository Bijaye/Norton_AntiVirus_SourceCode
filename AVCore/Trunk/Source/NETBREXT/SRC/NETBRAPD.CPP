// NetBrAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetBrApp.h"
#include "NetBrApD.h"
#include "NetBrwsD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetBrAppDlg dialog

CNetBrAppDlg::CNetBrAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetBrAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetBrAppDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNetBrAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetBrAppDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNetBrAppDlg, CDialog)
	//{{AFX_MSG_MAP(CNetBrAppDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDB_BROWSE_SERVER, OnBrowseServer)
	ON_BN_CLICKED(IDC_BROWSER_TYPE_DEFAULT, OnBrowserTypeDefault)
	ON_BN_CLICKED(IDC_BROWSER_TYPE_NLM, OnBrowserTypeNlm)
	ON_BN_CLICKED(IDC_BROWSER_TYPE_FORWARD, OnBrowserTypeForward)
	ON_BN_CLICKED(IDC_BROWSER_TYPE_NET_MESSAGE, OnBrowserTypeNetMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetBrAppDlg message handlers

BOOL CNetBrAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

                                    // Set browser type default to be the
                                    // default;
    ((CButton *)GetDlgItem( IDC_BROWSER_TYPE_DEFAULT ))->SetCheck( TRUE );
    OnBrowserTypeDefault();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNetBrAppDlg::OnPaint()
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
HCURSOR CNetBrAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CNetBrAppDlg::OnBrowseServer()
{
    RetrieveSettings();

//    cBrowseDlg.SetDialogTitle( _T("Ken's Network Browser Dialog") );
//    cBrowseDlg.SetInitialBrowseItemName( _T("KEN_P590") );

	                                    // If we got the server name correctly...
    if ( IDOK == m_cNetBrowsDlg.DoModal() )
        {
                                        // Update our static text field.
        GetDlgItem( IDE_SERVER_NAME_TO_ADD )->SendMessage( WM_SETTEXT,
                                                           0,
                                                           (LPARAM)m_cNetBrowsDlg.GetSelectedItemName() );
        }
}

void CNetBrAppDlg::DisplaySettings( void )
{
DWORD   dwValidSelectionFlags;
DWORD   dwBrowseBeneathFlags;
DWORD   dwDisplayItemFlags;
CString cszDialogTitle;
CString cszDialogInformationText;

                                        // Get the current display item flags.
    dwDisplayItemFlags    = m_cNetBrowsDlg.GetDisplayItemFlags();
    dwBrowseBeneathFlags  = m_cNetBrowsDlg.GetBrowseBeneathFlags();
    dwValidSelectionFlags = m_cNetBrowsDlg.GetValidSelectionFlags();

                                        // Get the browser title.
    m_cNetBrowsDlg.GetDialogTitle( &cszDialogTitle );

                                        // Get the browser title.
    m_cNetBrowsDlg.GetDialogTitle( &cszDialogTitle );

                                        // Get the browser information text.
    m_cNetBrowsDlg.GetInitialBrowserInformationText( &cszDialogInformationText );

    GetDlgItem( IDE_BROWSER_TITLE )->SendMessage( WM_SETTEXT, NULL, (LPARAM)(LPCTSTR)cszDialogTitle );
    GetDlgItem( IDE_BROWSER_INFORMATION )->SendMessage( WM_SETTEXT, NULL, (LPARAM)(LPCTSTR)cszDialogInformationText );

    SetCheckIfFlag( IDC_DISPLAY_MICROSOFT_NETWORK , RESOURCE_TYPE_NETWORK_MICROSOFT  & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_NETWARE_NETWORK   , RESOURCE_TYPE_NETWORK_NETWARE    & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_DOMAIN            , RESOURCE_TYPE_DOMAIN             & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_GROUP             , RESOURCE_TYPE_GROUP              & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_TREE              , RESOURCE_TYPE_TREE               & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_NDS_CONTIANER     , RESOURCE_TYPE_NDSCONTAINER       & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_SERVER            , RESOURCE_TYPE_SERVER             & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_SHARE             , RESOURCE_TYPE_SHARE              & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_FILE              , RESOURCE_TYPE_FILE               & dwDisplayItemFlags );
    SetCheckIfFlag( IDC_DISPLAY_GENERIC           , RESOURCE_TYPE_GENERIC            & dwDisplayItemFlags );

    SetCheckIfFlag( IDC_BROWSE_BENEATH_MICROSOFT_NETWORK , RESOURCE_TYPE_NETWORK_MICROSOFT   & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_NETWARE_NETWORK   , RESOURCE_TYPE_NETWORK_NETWARE     & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_DOMAIN            , RESOURCE_TYPE_DOMAIN              & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_GROUP             , RESOURCE_TYPE_GROUP               & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_TREE              , RESOURCE_TYPE_TREE                & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_NDS_CONTIANER     , RESOURCE_TYPE_NDSCONTAINER        & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_SERVER            , RESOURCE_TYPE_SERVER              & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_SHARE             , RESOURCE_TYPE_SHARE               & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_FILE              , RESOURCE_TYPE_FILE                & dwBrowseBeneathFlags );
    SetCheckIfFlag( IDC_BROWSE_BENEATH_GENERIC           , RESOURCE_TYPE_GENERIC             & dwBrowseBeneathFlags );

    SetCheckIfFlag( IDC_VALID_SELECTION_MICROSOFT_NETWORK , RESOURCE_TYPE_NETWORK_MICROSOFT  & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_NETWARE_NETWORK   , RESOURCE_TYPE_NETWORK_NETWARE    & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_DOMAIN            , RESOURCE_TYPE_DOMAIN             & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_GROUP             , RESOURCE_TYPE_GROUP              & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_TREE              , RESOURCE_TYPE_TREE               & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_NDS_CONTIANER     , RESOURCE_TYPE_NDSCONTAINER       & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_SERVER            , RESOURCE_TYPE_SERVER             & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_SHARE             , RESOURCE_TYPE_SHARE              & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_FILE              , RESOURCE_TYPE_FILE               & dwValidSelectionFlags );
    SetCheckIfFlag( IDC_VALID_SELECTION_GENERIC           , RESOURCE_TYPE_GENERIC            & dwValidSelectionFlags );
}

void CNetBrAppDlg::RetrieveSettings( void )
{
DWORD   dwValidSelectionFlags=0;
DWORD   dwBrowseBeneathFlags=0;
DWORD   dwDisplayItemFlags=0;
CString cszDialogTitle;
CString cszDialogInformationText;

                                        // Set the browser title.
    GetDlgItemText( IDE_BROWSER_TITLE , cszDialogTitle );
    m_cNetBrowsDlg.SetDialogTitle( cszDialogTitle );

                                        // Set the browser information text.
    GetDlgItemText( IDE_BROWSER_INFORMATION , cszDialogInformationText );
    m_cNetBrowsDlg.SetInitialBrowserInformationText( cszDialogInformationText );

    GetCheckSetFlag( IDC_DISPLAY_MICROSOFT_NETWORK , RESOURCE_TYPE_NETWORK_MICROSOFT  ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_NETWARE_NETWORK   , RESOURCE_TYPE_NETWORK_NETWARE    ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_DOMAIN            , RESOURCE_TYPE_DOMAIN             ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_GROUP             , RESOURCE_TYPE_GROUP              ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_TREE              , RESOURCE_TYPE_TREE               ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_NDS_CONTIANER     , RESOURCE_TYPE_NDSCONTAINER       ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_SERVER            , RESOURCE_TYPE_SERVER             ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_SHARE             , RESOURCE_TYPE_SHARE              ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_FILE              , RESOURCE_TYPE_FILE               ,&dwDisplayItemFlags );
    GetCheckSetFlag( IDC_DISPLAY_GENERIC           , RESOURCE_TYPE_GENERIC            ,&dwDisplayItemFlags );

    GetCheckSetFlag( IDC_BROWSE_BENEATH_MICROSOFT_NETWORK , RESOURCE_TYPE_NETWORK_MICROSOFT   ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_NETWARE_NETWORK   , RESOURCE_TYPE_NETWORK_NETWARE     ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_DOMAIN            , RESOURCE_TYPE_DOMAIN              ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_GROUP             , RESOURCE_TYPE_GROUP               ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_TREE              , RESOURCE_TYPE_TREE                ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_NDS_CONTIANER     , RESOURCE_TYPE_NDSCONTAINER        ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_SERVER            , RESOURCE_TYPE_SERVER              ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_SHARE             , RESOURCE_TYPE_SHARE               ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_FILE              , RESOURCE_TYPE_FILE                ,&dwBrowseBeneathFlags );
    GetCheckSetFlag( IDC_BROWSE_BENEATH_GENERIC           , RESOURCE_TYPE_GENERIC             ,&dwBrowseBeneathFlags );

    GetCheckSetFlag( IDC_VALID_SELECTION_MICROSOFT_NETWORK , RESOURCE_TYPE_NETWORK_MICROSOFT  ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_NETWARE_NETWORK   , RESOURCE_TYPE_NETWORK_NETWARE    ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_DOMAIN            , RESOURCE_TYPE_DOMAIN             ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_GROUP             , RESOURCE_TYPE_GROUP              ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_TREE              , RESOURCE_TYPE_TREE               ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_NDS_CONTIANER     , RESOURCE_TYPE_NDSCONTAINER       ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_SERVER            , RESOURCE_TYPE_SERVER             ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_SHARE             , RESOURCE_TYPE_SHARE              ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_FILE              , RESOURCE_TYPE_FILE               ,&dwValidSelectionFlags );
    GetCheckSetFlag( IDC_VALID_SELECTION_GENERIC           , RESOURCE_TYPE_GENERIC            ,&dwValidSelectionFlags );

                                        // Setup the new display item flags.
    m_cNetBrowsDlg.SetDisplayItemFlags( dwDisplayItemFlags );
    m_cNetBrowsDlg.SetBrowseBeneathFlags( dwBrowseBeneathFlags );
    m_cNetBrowsDlg.SetValidSelectionFlags( dwValidSelectionFlags );
}


void CNetBrAppDlg::SetCheckIfFlag( UINT uControlID, DWORD dwFlag )
{
    if ( 0 != dwFlag )
        {
        ((CButton*)GetDlgItem( uControlID ))->SetCheck( TRUE );
        }
    else
        {
        ((CButton*)GetDlgItem( uControlID ))->SetCheck( FALSE );
        }

}

void CNetBrAppDlg::GetCheckSetFlag( UINT uControlID, DWORD dwFlag, DWORD *lpdwFlagList )
{
BOOL bSelected;

    bSelected = ((CButton*)GetDlgItem( uControlID ))->GetCheck();

    if ( TRUE == bSelected )
        {
                                        // Add this flag.
        *lpdwFlagList |= dwFlag;
        }
    else
        {
                                        // Remove this flag.
        *lpdwFlagList &= ~dwFlag;
        }

}

void CNetBrAppDlg::OnBrowserTypeDefault()
{
	m_cNetBrowsDlg.SetBrowserType( BROWSER_TYPE_DEFAULT );
    DisplaySettings();
}

void CNetBrAppDlg::OnBrowserTypeNlm() 
{
	m_cNetBrowsDlg.SetBrowserType( BROWSER_TYPE_NLM );
    DisplaySettings();
}

void CNetBrAppDlg::OnBrowserTypeForward() 
{
	m_cNetBrowsDlg.SetBrowserType( BROWSER_TYPE_FORWARD );
    DisplaySettings();
}

void CNetBrAppDlg::OnBrowserTypeNetMessage() 
{
	m_cNetBrowsDlg.SetBrowserType( BROWSER_TYPE_NETWORK_MESSAGE );
    DisplaySettings();
}
