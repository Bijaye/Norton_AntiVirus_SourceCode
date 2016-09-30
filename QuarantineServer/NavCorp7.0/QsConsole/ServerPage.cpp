// ServerPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ServerPage.h"
#include "qsregval.h"
#include "qscon.h"
#include "qsconsole.h"
#include "mmc.h"
#include "qshelp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD CServerPage::m_dwWhatsThisMap[] = { 
	IDC_FOLDER_STATIC, IDH_Quar_Server_Folder,
    IDC_Q_FOLDER, IDH_Quar_Server_Folder, 
	IDC_MAX_STATIC, IDH_Quar_Server_Max_Size,
	IDC_SPIN1, IDH_Quar_Server_Max_Size,
	IDC_QFOLDER_SIZE, IDH_Quar_Server_Max_Size,
	IDC_MEGABYTES_STATIC, IDH_Quar_Server_Max_Size,
	IDC_IP_STATIC, IDH_Quar_Server_PortIP,
	IDC_IP_PORT, IDH_Quar_Server_PortIP,
	IDC_IP_CHECK, 	IDH_Quar_Server_ListenIP,
	IDC_IPX_CHECK, IDH_Quar_Server_ListenSPX,
	IDC_IPX_STATIC, IDH_Quar_Server_PortSPX,
	IDC_SPX_PORT, IDH_Quar_Server_PortSPX,
	0,0 };


/////////////////////////////////////////////////////////////////////////////
// CServerPage property page

IMPLEMENT_DYNCREATE(CServerPage, CQPropPage)

CServerPage::CServerPage() : CQPropPage(CServerPage::IDD)
{
	//{{AFX_DATA_INIT(CServerPage)
	m_bListenIP = FALSE;
	m_bListenSPX = FALSE;
	m_iIPPort = 4185;
	m_sFolder = _T("");
	m_uMaxSize = 10;
	m_iSPXPort = 4186;
	//}}AFX_DATA_INIT

    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
}

CServerPage::~CServerPage()
{
}

void CServerPage::DoDataExchange(CDataExchange* pDX)
{
	CQPropPage::DoDataExchange(pDX);

	//
	// Make sure number text fields have data.
	//
	CString s;
	GetDlgItemText( IDC_QFOLDER_SIZE, s );
	if( s.GetLength() == 0 )
		GetDlgItem( IDC_QFOLDER_SIZE)->SetWindowText(_T("0"));
	GetDlgItemText( IDC_IP_PORT, s );
	if( s.GetLength() == 0 )
		GetDlgItem( IDC_IP_PORT)->SetWindowText(_T("0"));
	GetDlgItemText( IDC_SPX_PORT, s );
	if( s.GetLength() == 0 )
		GetDlgItem( IDC_SPX_PORT)->SetWindowText(_T("0"));

	//{{AFX_DATA_MAP(CServerPage)
	DDX_Check(pDX, IDC_IP_CHECK, m_bListenIP);
	DDX_Check(pDX, IDC_IPX_CHECK, m_bListenSPX);
	DDX_Text(pDX, IDC_IP_PORT, m_iIPPort);
	DDX_Text(pDX, IDC_Q_FOLDER, m_sFolder);
	DDV_MaxChars(pDX, m_sFolder,  MAX_PATH );
	DDX_Text(pDX, IDC_QFOLDER_SIZE, m_uMaxSize);
	DDX_Text(pDX, IDC_SPX_PORT, m_iSPXPort);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CServerPage, CQPropPage)
	//{{AFX_MSG_MAP(CServerPage)
	ON_BN_CLICKED(IDC_IP_CHECK, OnIpCheck)
	ON_BN_CLICKED(IDC_IPX_CHECK, OnIpxCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerPage message handlers


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnIpCheck
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::OnIpCheck() 
{
	BOOL bEnable = IsDlgButtonChecked( IDC_IP_CHECK );

    GetDlgItem( IDC_IP_STATIC )->EnableWindow( bEnable );
    GetDlgItem( IDC_IP_PORT )->EnableWindow( bEnable );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnIpxCheck
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::OnIpxCheck() 
{
	BOOL bEnable = IsDlgButtonChecked( IDC_IPX_CHECK );

    GetDlgItem( IDC_IPX_STATIC )->EnableWindow( bEnable );
    GetDlgItem( IDC_SPX_PORT )->EnableWindow( bEnable );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnInitDialog
//
// Description   : 
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerPage::OnInitDialog() 
{
    CQPropPage::OnInitDialog();

    // 
    // Unmarshal server interface.
    // 
    CoGetInterfaceAndReleaseStream( m_pServerStream, __uuidof( IQserverConfig ), (LPVOID*)&m_pConfig );
    ((CQSConsoleData*)m_pSnapin)->m_pQServer.DCO_SetProxyBlanket( m_pConfig );


	//
	// Set text limits
	//
	((CEdit*)GetDlgItem( IDC_QFOLDER_SIZE ))->SetLimitText( 4 );
	((CEdit*)GetDlgItem( IDC_IP_PORT ))->SetLimitText( 5 );
	((CEdit*)GetDlgItem( IDC_SPX_PORT ))->SetLimitText( 5 );
    
    // 
    // Set up data members.
    // 
    try
        {
        VARIANT v;
        CString s;
        
        VariantInit( &v );
        s = REGVALUE_QUARANTINE_FOLDER;
        if( SUCCEEDED( m_pConfig->GetValue( s.AllocSysString(), &v ) ) )
            {
            m_sFolder = v.bstrVal;
            m_sOrigQFolder = m_sFolder;
            VariantClear( &v );
            }

        VariantInit( &v );
        s = REGVALUE_PORT;
        if( SUCCEEDED( m_pConfig->GetValue( s.AllocSysString(), &v ) ) )
            {
            m_iIPPort = (int) v.ulVal;
            }

        VariantInit( &v );
        s = REGVALUE_SPXSOCKET;
        if( SUCCEEDED( m_pConfig->GetValue( s.AllocSysString(), &v ) ) )
            {
            m_iSPXPort = (int) v.ulVal;
            }

        VariantInit( &v );
        s = REGVALUE_LISTEN_IP;
        if( SUCCEEDED( m_pConfig->GetValue( s.AllocSysString(), &v ) ) )
            {
            m_bListenIP = (BOOL) (v.ulVal > 0 );
            }

        VariantInit( &v );
        s = REGVALUE_LISTEN_SPX;
        if( SUCCEEDED( m_pConfig->GetValue( s.AllocSysString(), &v ) ) )
            {
            m_bListenSPX = (BOOL) (v.ulVal > 0 );
            }

        VariantInit( &v );
        s = REGVALUE_MAX_SIZE;
        if( SUCCEEDED( m_pConfig->GetValue( s.AllocSysString(), &v ) ) )
            {
            m_uMaxSize = v.ulVal;
            }
        }
    catch(...)
        {
        // 
        // Tell the user that something bad happened.
        // 
        CString sError( (LPCTSTR) IDS_ERROR );
        CString sText( (LPCTSTR) IDS_CONFIG_ERROR );
        MessageBox( sText, sError, MB_ICONSTOP | MB_OK );
        }


    UpdateData( FALSE );

	// 
    // Set states of controls correctly
    // 
    OnIpxCheck();
    OnIpCheck();
	
    // 
    // Set up spin control.
    // 
    ((CSpinButtonCtrl*) GetDlgItem( IDC_SPIN1 ) )->SetRange(1, 4095 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnOK
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::OnOK()
{
    // 
    // Save values to server
    //
    CString s; 

    try
        {
        HRESULT hr;
        VARIANT v;

        // 
        // Trim leading and trailing spaces
        // 
        m_sFolder.TrimLeft();
        m_sFolder.TrimRight();

        // 
        // Strip off trailing backslashes
        // 
        m_sFolder.TrimRight( _T('\\') );    


        VariantInit( &v );
        v.vt = VT_BSTR;
        v.bstrVal = m_sFolder.AllocSysString();
        s = REGVALUE_QUARANTINE_FOLDER;
        hr = m_pConfig->SetValue( s.AllocSysString(), v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantInit( &v );
        v.vt = VT_UI4;
        v.ulVal = m_iIPPort;
        s = REGVALUE_PORT;
        hr = m_pConfig->SetValue( s.AllocSysString(), v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantInit( &v );
        v.vt = VT_UI4;
        v.ulVal = m_iSPXPort;
        s = REGVALUE_SPXSOCKET;
        hr = m_pConfig->SetValue( s.AllocSysString(), v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantInit( &v );
        v.vt = VT_UI4;
        v.ulVal = m_bListenIP;
        s = REGVALUE_LISTEN_IP;
        hr = m_pConfig->SetValue( s.AllocSysString(), v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantInit( &v );
        v.vt = VT_UI4;
        v.ulVal = m_bListenSPX;
        s = REGVALUE_LISTEN_SPX;
        hr = m_pConfig->SetValue( s.AllocSysString(), v );
        if( FAILED( hr ) )            
            {
            _com_issue_error( hr );
            }

        VariantInit( &v );
        v.vt = VT_UI4;
        v.ulVal = m_uMaxSize;
        s = REGVALUE_MAX_SIZE;
        hr = m_pConfig->SetValue( s.AllocSysString(), v );
        if( FAILED( hr ) )
            {
            _com_issue_error( hr );
            }

        }
    catch( _com_error e )
        {
        // 
        // Tell user what happened.
        //
        s.LoadString( IDS_ERROR );
        MessageBox( e.ErrorMessage(), s, MB_ICONSTOP | MB_OK ); 
        }



}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::PostNcDestroy
//
// Description   : Need to delete this now.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerPage::PostNcDestroy() 
{
	CQPropPage::PostNcDestroy();

    // 
    // Tell main snapin that the page is gone.
    // 
    ((CQSConsoleData*)m_pSnapin)->CancelPropPage();

    // 
    // Release configuration interface
    // 
    m_pConfig->Detach();
    m_pConfig->Release();

    // 
    // If quarantine folder has changed, notify caller.
    //
    if( m_sOrigQFolder != m_sFolder )   
        {
        MMCPropertyChangeNotify( m_lNotifyHandle, (long) m_pSnapin );
        }

    // 
    // This is the only real opportunity to delete this property page.
    // 
    delete this; 
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::OnKillActive
//
// Description   : 
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerPage::OnKillActive() 
{
	// 
    // Make sure data is good.
    // 
    if( CQPropPage::OnKillActive() == FALSE )
        return FALSE;

    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;

    // 
    // Check quarantine folder field.
    // 
    if( IsValidQserverFolder() == FALSE )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_QSERVER_FOLDER );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_Q_FOLDER;
        }
    else if( m_bListenIP && ( m_iIPPort < 1 || m_iIPPort > 65535 ) )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_PORT );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_IP_PORT;
        }
    else if( m_bListenSPX && ( m_iSPXPort < 1 || m_iSPXPort > 65535 ) )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_PORT );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_SPX_PORT;
        }
    else if( m_uMaxSize < 1 || m_uMaxSize > 4095 )
        {
        // 
        // Inform the user of the errors of his ways.
        // 
        sText.LoadString( IDS_BAD_MAX_SIZE );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_QFOLDER_SIZE;
        }

    // 
    // Select the offending control.
    // 
    if( iControl )
        {
        GetDlgItem( iControl )->SetFocus();
        ((CEdit*) GetDlgItem( iControl ) )->SetSel( 0, -1 );
        return FALSE;
        }

    // 
    // Looks good.  Ok to bail out now.
    // 
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CServerPage::IsValidQserverFolder
//
// Description   : Makes sure the quarantine folder path looks valid.
//
// Return type   : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerPage::IsValidQserverFolder()
{
    // 
    // Check the length
    //     
    if( m_sFolder.GetLength() < 3 || m_sFolder.IsEmpty() )
        return FALSE;

    // 
    // Convert forward slashes.
    // 
    m_sFolder.Replace( _T('/'), _T( '\\' ) );

    // 
    // Trim leading and trailing spaces
    // 
    m_sFolder.TrimLeft();
    m_sFolder.TrimRight();

    // 
    // Strip off trailing backslashes
    // 
    m_sFolder.TrimRight( _T('\\') );    

    // 
    // Make sure there are no wildcard characters.
    // 
    if( -1 != m_sFolder.FindOneOf( _T("*?") ) )
        return FALSE;

    // 
    // Make sure the folder is in the form X:\...
    // 
    CString sTemp = m_sFolder.Left( 3 );
    if( _T( ':' ) == sTemp.GetAt( 1 ) && 
        _T( '\\' ) == sTemp.GetAt( 2 ) )
        {
        return TRUE;
        }

    return FALSE;
}


