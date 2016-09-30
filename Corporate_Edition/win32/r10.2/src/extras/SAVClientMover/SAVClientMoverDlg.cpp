// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SAVClientMoverDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxtempl.h>
#include "MoveResults.h"
#include "vpcommon.h"
#include "SAVClientMover.h"
#include "SAVClientMoverDlg.h"
#include "DomainPasswordDlg.h"
#include "TransmanBase.h"
#include "ClientReg.h"
#include "InputDlg.h"
#include "SavHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIME_ID_UPDATE_CLIENT_LISTS                                                     777888
#define TIME_INTERVAL_UPDATE_CLIENT_LISTS_MILLISECONDS          60000   
#define CVPString       CString

CArray<CString,CString> gSourceClients;
CArray<CString,CString> gTargetClients;
CArray<CString,CString> gSourceClientsFullName;
CArray<CString,CString> gTargetClientsFullName;

CArray<CBA_Addr,CBA_Addr> gSourceClientsAddress;
CArray<CBA_Addr,CBA_Addr> gTargetClientsAddress;

const CString csNoValidPassword = "<No valid password>";

//////////////////////////////////////////////////////////////////////
// Created 2/2002 Rich Sutton
//
// Description:
//     IEnumClients wrappers for BuildServersClientList.
//
//////////////////////////////////////////////////////////////////////
void GetOptionInt( IEnumClientList* _client_enum, const char* _name, DWORD& _val, const DWORD& _default = 0 )
{
    DWORD _temp = 0;
    if ( _client_enum->GetOptionInt( const_cast<char*>(_name), &_temp, _default ) == ERROR_SUCCESS )
        _val = _temp;
    else
        _val = _default;
}

void GetOptionString( IEnumClientList* _client_enum, const char* _name, CString& _val, const char* _default = "" )
{
    char* _temp = NULL;
    if ( _client_enum->GetOptionString( const_cast<char*>(_name), &_temp, const_cast<char*>(_default) ) == ERROR_SUCCESS )
    {
        _val = _temp;
        CoTaskMemFree( _temp );
    }
    else
    {
        _val = _default;
    }
}

void GetOptionBinary( IEnumClientList* _client_enum, const char* _name, BYTE* _data, const DWORD& _len )
{
    BYTE* _temp = NULL;
    ULONG _actual_len = 0;
    if ( _client_enum->GetOptionBinary( const_cast<char*>(_name), &_temp, &_actual_len, NULL, 0 ) == ERROR_SUCCESS )
    {
        if ( _len == _actual_len )
            memcpy( _data, _temp, _len );
        else
            memset( _data, 0x00, _len );
        CoTaskMemFree( _temp );
    }
    else
    {
        memset( _data, 0x00, _len );
    }
}

void GetOptionAddress( IEnumClientList* _client_enum, const char* _name, CBA_Addr& _address )
{
    GetOptionBinary( _client_enum, _name, reinterpret_cast<BYTE*>(&_address), sizeof(_address) );
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum tagAbout
    {
        IDD = IDD_ABOUTBOX
    };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSAVClientMoverDlg dialog

CSAVClientMoverDlg::CSAVClientMoverDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSAVClientMoverDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSAVClientMoverDlg)
    m_csSourceParent = _T("");
    m_csTargetParent = _T("");
    m_csStatus = _T("");
    m_csFreshStatus = _T("");
	m_ctlMoveResultStatus = _T("");
	//}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSAVClientMoverDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSAVClientMoverDlg)
	DDX_Control(pDX, IDC_MOVE_RESULTS_VIEW, m_ctlMoveResults);
    DDX_Control(pDX, IDC_COMBO_TARGET_PARENT, m_TargetParentCombo);
    DDX_Control(pDX, IDC_COMBO_SOURCE_PARENT, m_SourceParentCombo);
    DDX_Control(pDX, IDC_CLIENT_LIST_TARGET, m_TargetClientList);
    DDX_Control(pDX, IDC_CLIENT_LIST_SOURCE, m_SourceClientList);
    DDX_CBString(pDX, IDC_COMBO_SOURCE_PARENT, m_csSourceParent);
    DDX_CBString(pDX, IDC_COMBO_TARGET_PARENT, m_csTargetParent);
    DDX_Text(pDX, IDC_STATIC_STATUS, m_csStatus);
    DDX_Text(pDX, IDC_STATIC_REFRESH_STATUS, m_csFreshStatus);
	DDX_Text(pDX, IDC_STATIC_STATUS2, m_ctlMoveResultStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSAVClientMoverDlg, CDialog)
//{{AFX_MSG_MAP(CSAVClientMoverDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_CBN_SELCHANGE(IDC_COMBO_SOURCE_PARENT, OnSelchangeComboSourceParent)
ON_LBN_SELCHANGE(IDC_CLIENT_LIST_SOURCE, OnSelchangeClientListSource)
ON_CBN_SELCHANGE(IDC_COMBO_TARGET_PARENT, OnSelchangeComboTargetParent)
ON_BN_CLICKED(IDC_MOVE, OnMove)
ON_BN_CLICKED(IDC_MOVE_BACK, OnMoveBack)
ON_LBN_SELCHANGE(IDC_CLIENT_LIST_TARGET, OnSelchangeClientListTarget)
ON_WM_TIMER()
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_LBN_SELCHANGE(IDC_MOVE_RESULTS_VIEW, OnSelchangeMoveResultsView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSAVClientMoverDlg message handlers

BOOL CSAVClientMoverDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if ( pSysMenu != NULL )
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if ( !strAboutMenu.IsEmpty() )
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);                 // Set big icon
    SetIcon(m_hIcon, FALSE);                // Set small icon

    DWORD dwIndex = 0;
    CString csAddressCache = "SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\AddressCache";
    CString csName;
    DWORD size = 1024;		// MAX_REGISTRY_KEY
    HKEY hAddressCacheKey;
    FILETIME ftLastWriteTime;

    if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, csAddressCache, 0, KEY_READ, &hAddressCacheKey ) && hAddressCacheKey)
    {
        while ( ERROR_SUCCESS == RegEnumKeyEx(
                                             hAddressCacheKey,                  // handle to key to enumerate
                                             dwIndex++,              // subkey index
                                             csName.GetBuffer(size),              // subkey name
                                             &size,            // size of subkey buffer
                                             0,         // reserved
                                             NULL,             // class string buffer
                                             NULL,           // size of class string buffer
                                             &ftLastWriteTime // last write time
                                             ) )
        {
            csName.ReleaseBuffer();
            m_SourceParentCombo.InsertString( -1, csName );
            m_TargetParentCombo.InsertString( -1, csName );
            size = 1000;
        }

        RegCloseKey( hAddressCacheKey );
    }

    m_csFreshStatus = "Refresh OFF";

    CString str;
    CSize   sz;
    int     dx=0;
    CDC*    pDC = m_SourceParentCombo.GetDC();
    for ( int i=0;i < m_SourceParentCombo.GetCount();i++ )
    {
        m_SourceParentCombo.GetLBText( i, str );
        sz = pDC->GetTextExtent(str);

        if ( sz.cx > dx )
            dx = sz.cx;
    }
    m_SourceParentCombo.ReleaseDC(pDC);
    // Adjust the width for the vertical scroll bar and the left and right border.
    dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);

    // If the width of the list box is too small, adjust it so that every
    // item is completely visible.
    if ( m_SourceParentCombo.GetDroppedWidth() < dx )
    {
        m_SourceParentCombo.SetDroppedWidth(dx);
        //ASSERT(m_SourceParentCombo.GetDroppedWidth() == dx);
    }
    m_SourceParentCombo;

    pDC = m_TargetParentCombo.GetDC();
    for ( i=0;i < m_TargetParentCombo.GetCount();i++ )
    {
        m_TargetParentCombo.GetLBText( i, str );
        sz = pDC->GetTextExtent(str);

        if ( sz.cx > dx )
            dx = sz.cx;
    }
    m_TargetParentCombo.ReleaseDC(pDC);
    // Adjust the width for the vertical scroll bar and the left and right border.
    dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2*::GetSystemMetrics(SM_CXEDGE);

    // If the width of the list box is too small, adjust it so that every
    // item is completely visible.
    if ( m_TargetParentCombo.GetDroppedWidth() < dx )
    {
        m_TargetParentCombo.SetDroppedWidth(dx);
        //ASSERT(m_TargetParentCombo.GetDroppedWidth() == dx);
    }

    //m_SourceParentCombo.SetDroppedWidth( 10 * m_SourceParentCombo.GetItemHeight( -1 ) );
    //m_TargetParentCombo.SetDroppedWidth( 10 * m_TargetParentCombo.GetItemHeight( -1 ) );

    DWORD ret;
    if ( ( ret = InitializeTransman() ) != ERROR_SUCCESS )
    {
        m_bTransmanInitialized = false;
        AfxMessageBox( "Transman fail to initialize." );
    }
    else
    {
        m_bTransmanInitialized = true;
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSAVClientMoverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ( (nID & 0xFFF0) == IDM_ABOUTBOX )
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSAVClientMoverDlg::OnPaint() 
{
    if ( IsIconic() )
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
HCURSOR CSAVClientMoverDlg::OnQueryDragIcon()
{
    return(HCURSOR) m_hIcon;
}



#define _CLSID_Transman             { 0xe381f1e0, 0x910e, 0x11d1, { 0xab, 0x1e, 0x0, 0xa0, 0xc9, 0xf, 0x8f, 0x6f } }
#define _IID_IVirusProtect          { 0xe381f1df, 0x910e, 0x11d1, { 0xab, 0x1e, 0x0, 0xa0, 0xc9, 0xf, 0x8f, 0x6f } }
#define _IID_IEnumClientList        { 0x0f405b8e, 0x1b59, 0x4597, { 0x9d, 0xfe, 0x5d, 0xd8, 0x4a, 0x9c, 0xbc, 0xc7 } }
#define _IID_IGenericConfig         { 0xe381f1de, 0x910e, 0x11d1, { 0xab, 0x1e, 0x0, 0xa0, 0xc9, 0xf, 0x8f, 0x6f } }

CLSID CLSID_Transman        = _CLSID_Transman;
IID IID_IVirusProtect            = _IID_IVirusProtect;
IID IID_IEnumClientList     = _IID_IEnumClientList;
IID IID_IGenericConfig     = _IID_IGenericConfig;

void CSAVClientMoverDlg::OnSelchangeComboSourceParent() 
{
    if ( false == m_bTransmanInitialized )
    {
        m_csStatus = "Transman not initialized.";
        UpdateData( FALSE );
        return;
    }

    UpdateData( TRUE );
    if ( m_csSourceParent.IsEmpty() )
    {
        return;
    }

	// clear the lists - since a server has been selected the old list
	// doesn't apply

    m_SourceClientList.ResetContent();
    m_csStatus.Empty();
    gSourceClients.RemoveAll();
    gSourceClientsAddress.RemoveAll();
    gSourceClientsFullName.RemoveAll();

    // get a working password for this server

    if ( m_PasswordCheck.GetPassword( m_csSourceParent ) != ERROR_SUCCESS )
    {
		m_SourceClientList.InsertString( -1, csNoValidPassword );
        return;
    }

    DWORD                   rv = 0;
    IEnumClientList         *_client_enum  = NULL;
    IVirusProtect           *vp = NULL;

    if ( S_OK != CoInitialize(NULL) )
    {
        return;
    }

    // Get a Virus Protect object.
    if ( (rv = CoCreateInstance(CLSID_Transman,NULL,CLSCTX_INPROC_SERVER,IID_IVirusProtect,(void**)&vp)) == S_OK )
    {
        // Crete a Client List enumeration object.
        if ( rv = vp->CreateByIID( IID_IEnumClientList,(void**)&_client_enum ) != S_OK )
        {
            vp->Release();
            m_csStatus = "IEnumClientList interface not available.";
            UpdateData( FALSE );
            CoUninitialize();
            return;
        }
        vp->Release();


        // Open the client list on the remote server.
        static const _client_count_per_iter = 500;
        DWORD _client_count = 0;

        m_csStatus.Format( "Contacting %s...", m_csSourceParent );
        UpdateData( FALSE );

        if ( ERROR_SUCCESS == _client_enum->Open( (char *)(const char *)m_csSourceParent, &_client_count ) )
        {
            DWORD i = 0;
            char *pszName = NULL, *pszAddress = NULL;
            CString csClientName;
            int index = 0;

            CBA_Addr address = {0};
            CVPString strProtocol;

            // This might be a long operation ... check each time to see if the service is coming down.
            while ( _client_enum->Next( &pszName, &i ) == S_OK )
            {
                csClientName = pszName;
                gSourceClientsFullName.Add( csClientName );
                index = csClientName.Find( "_::_" );
                csClientName = csClientName.Left( index );
                m_SourceClientList.InsertString( -1, csClientName );

                gSourceClients.Add( csClientName );


                // Protocol
                GetOptionString( _client_enum, "Protocol", strProtocol );
                // User Address - based on the value of Protocol
                GetOptionAddress( _client_enum, strProtocol, address );

                gSourceClientsAddress.Add( address );
            }

            m_csStatus.Format( "%s has %d clients.", m_csSourceParent, _client_count );
        }
        else
        {
            m_csStatus = "Cannot contact server: " + m_csSourceParent;
        }
    }
    else
    {
        m_csStatus = "Transman not registered.";
    }

    if ( _client_enum )
        _client_enum->Release();

    UpdateData( FALSE );

    CoUninitialize();

}

void CSAVClientMoverDlg::OnSelchangeComboTargetParent() 
{
    if ( false == m_bTransmanInitialized )
    {
        m_csStatus = "Transman not initialized.";
        UpdateData( FALSE );
        return;
    }

    UpdateData( TRUE );
    if ( m_csTargetParent.IsEmpty() )
    {
        return;
    }

    m_TargetClientList.ResetContent();
    m_csStatus.Empty();
    gTargetClients.RemoveAll();
    gTargetClientsAddress.RemoveAll();
    gTargetClientsFullName.RemoveAll();

    // get a working password for this server

    if ( m_PasswordCheck.GetPassword( m_csTargetParent ) != ERROR_SUCCESS )
    {
		m_TargetClientList.InsertString( -1, csNoValidPassword );
        return;
    }

    DWORD                   rv = 0;
    IEnumClientList         *_client_enum  = NULL;
    IVirusProtect           *vp = NULL;

    if ( S_OK != CoInitialize(NULL) )
    {
        return;
    }

    // Get a Virus Protect object.
    if ( (rv = CoCreateInstance(CLSID_Transman,NULL,CLSCTX_INPROC_SERVER,IID_IVirusProtect,(void**)&vp)) == S_OK )
    {
        // Crete a Client List enumeration object.
        if ( rv = vp->CreateByIID( IID_IEnumClientList,(void**)&_client_enum ) != S_OK )
        {
            vp->Release();
            m_csStatus = "IEnumClientList interface not available.";
            UpdateData( FALSE );
            CoUninitialize();
            return;
        }
        vp->Release();


        // Open the client list on the remote server.
        static const _client_count_per_iter = 500;
        DWORD _client_count = 0;

        m_csStatus.Format( "Contacting %s...", m_csTargetParent );
        UpdateData( FALSE );

        if ( ERROR_SUCCESS == _client_enum->Open( (char *)(const char *)m_csTargetParent, &_client_count ) )
        {
            DWORD i = 0;
            char *pszName = NULL, *pszAddress = NULL;
            CString csClientName;
            int index = 0;

            CBA_Addr address = {0};
            CVPString strProtocol;

            // This might be a long operation ... check each time to see if the service is coming down.
            while ( _client_enum->Next( &pszName, &i ) == S_OK )
            {
                csClientName = pszName;
                gTargetClientsFullName.Add( csClientName ); // this is GUID decorated - I need that to find the client on the parent, old or new

                index = csClientName.Find( "_::_" );
                csClientName = csClientName.Left( index );
                m_TargetClientList.InsertString( -1, csClientName );

                gTargetClients.Add( csClientName );

                // Protocol
                GetOptionString( _client_enum, "Protocol", strProtocol );
                // User Address - based on the value of Protocol
                GetOptionAddress( _client_enum, strProtocol, address );

                gTargetClientsAddress.Add( address );

            }

            m_csStatus.Format( "%s has %d clients.", m_csTargetParent, _client_count );

        }
        else
        {
            m_csStatus = "Cannot contact server: " + m_csTargetParent;
        }
    }
    else
    {
        m_csStatus = "Transman not registered.";
    }

    if ( _client_enum )
        _client_enum->Release();

    UpdateData( FALSE );

    CoUninitialize();
}

void CSAVClientMoverDlg::OnSelchangeClientListSource() 
{
    CString csTemp;
    int iSelectCount = 0;

	CString csText;

	// don't let them select the warning message
	m_SourceClientList.GetText( m_SourceClientList.GetCurSel(), csText );
	if ( csText == csNoValidPassword )
	{
		// this is a sleazy way to clear the selection - SetCurSel(-1) doesn't work on multisel list
		// since it is only one line it doesn't blink too much

		m_SourceClientList.ResetContent( );
		m_SourceClientList.InsertString( -1, csNoValidPassword );
	}

    iSelectCount = m_SourceClientList.GetSelCount();

	UpdateData( FALSE );

    //csTemp.Format( "Source Selection: %d items", iSelectCount );
    //AfxMessageBox( csTemp );      
}

void CSAVClientMoverDlg::OnSelchangeClientListTarget() 
{
    CString csTemp;
    int iSelectCount = 0;

	CString csText;

	// don't let them select the warning message
	m_TargetClientList.GetText( m_TargetClientList.GetCurSel(), csText );	
	if ( csText == csNoValidPassword )
	{
		// this is a sleazy way to clear the selection - SetCurSel(-1) doesn't work on multisel list
		// since it is only one line it doesn't blink too much

		m_TargetClientList.ResetContent( );
		m_TargetClientList.InsertString( -1, csNoValidPassword );
	}

    iSelectCount = m_TargetClientList.GetSelCount();

	UpdateData( FALSE );

    //csTemp.Format( "Target Selection: %d items", iSelectCount );
    //AfxMessageBox( csTemp );      
}



////////////////////////////////////////////////////////////////////////////////
// FUNTION NAME : CSAVClientMoverDlg::OnMove
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void CSAVClientMoverDlg::OnMove() 
{
	DoTheMove( m_SourceClientList, gSourceClients, gSourceClientsFullName, gSourceClientsAddress, m_csSourceParent, m_csTargetParent );
}

void CSAVClientMoverDlg::OnMoveBack()
{
	DoTheMove( m_TargetClientList, gTargetClients, gTargetClientsFullName, gTargetClientsAddress, m_csTargetParent, m_csSourceParent );
}

void CSAVClientMoverDlg::RefreshViewOfClientLists()
{
    OnSelchangeComboSourceParent();
    OnSelchangeComboTargetParent();
}

void CSAVClientMoverDlg::OnTimer(UINT nIDEvent)
{
    if ( TIME_ID_UPDATE_CLIENT_LISTS == nIDEvent )
    {
        RefreshViewOfClientLists();
    }

    CDialog::OnTimer(nIDEvent);
}

void CSAVClientMoverDlg::OnClose()
{
    if ( m_nTimer > 0 )
    {
        KillTimer(m_nTimer);
    }
    CDialog::OnClose();
}

void CSAVClientMoverDlg::OnButtonRefresh()
{
    static bool bOnRefresh = false;
    static UINT uintFreshRate = 30;

    if ( bOnRefresh == false )
    {
        RefreshViewOfClientLists();

        CInputDlg InDlg;
        InDlg.m_csPrompt = "Set refresh rate (seconds)";
        InDlg.m_uintNumberValue = uintFreshRate;
        int nResponse = InDlg.DoModal();
        if ( nResponse == IDOK )
        {
            m_nTimer = SetTimer( TIME_ID_UPDATE_CLIENT_LISTS, 1000 * InDlg.m_uintNumberValue, NULL );
            bOnRefresh = true;
            m_csFreshStatus.Format( "Every %d sec.", InDlg.m_uintNumberValue );
        }
        else if ( nResponse == IDCANCEL )
        {
            bOnRefresh = false;
            m_csFreshStatus = "Refresh OFF";
        }
    }
    else
    {
        KillTimer(m_nTimer);
        bOnRefresh = false;
        m_csFreshStatus = "Refresh OFF";
    }

    UpdateData( FALSE );
}


void CSAVClientMoverDlg::DoTheMove( CListBox &csClientList, CArray<CString,CString> &csSourceClients, CArray<CString,CString> &csSourceClientsFullName, CArray<CBA_Addr,CBA_Addr> &csSourceClientsAddress, const CString &csSourceParent, const CString &csTargetParent )
{
    //Check if Transman is initialized
    if ( false == m_bTransmanInitialized )
    {
        m_csStatus = "Transman not initialized.";
        UpdateData( FALSE );
        return;
    }
    //Check if client(s) have been selected for the move
    int iSelectCount = 0;
    CArray<int,int> SelectedClients;
    if ( 0 == ( iSelectCount = csClientList.GetSelCount() ) )
    {
        m_csStatus = "No client selected for move.";
        UpdateData( FALSE );
        return;
    }
    SelectedClients.SetSize(iSelectCount);
    csClientList.GetSelItems(iSelectCount, SelectedClients.GetData()); 
    //Check if we have a valid target parent to move client(s) to
    if ( csTargetParent.IsEmpty() || csSourceParent == csTargetParent )
    {
        m_csStatus = "Need to select a new target parent for move.";
        UpdateData( FALSE );
        return;
    }

    DWORD dwType = REG_SZ; 
    DWORD dwSize = 0;

    BYTE * pValue = NULL; 

    HRESULT hRetValue = 0;

    //COM declarations
    DWORD                   rv = 0;
    IGenericConfig          *pConfig  = NULL;
    IVirusProtect           *vp = NULL;

    //Initialize COM
    if ( S_OK != CoInitialize(NULL) )
    {
        m_csStatus = "Fail to initialize COM.";
        UpdateData( FALSE );

        return;
    }

    // Get a Virus Protect object.
    if ( (rv = CoCreateInstance(CLSID_Transman,NULL,CLSCTX_INPROC_SERVER,IID_IVirusProtect,(void**)&vp)) == S_OK )
    {
        // Create a Generic Configuration object.
        if ( rv = vp->CreateByIID( IID_IGenericConfig,(void**)&pConfig ) != S_OK )
        {
            vp->Release();
            m_csStatus = "IGenericConfig interface not available.";
            UpdateData( FALSE );
            CoUninitialize();
            return;
        }
        vp->Release();
    }

    //Variables for setting options on the client
    char sKey[256] = {0};
    char sName[1024] = {0};

    //Variable for moving client(s)
    CString csFailedClients;
    int iNumberClientsMoved = 0;
    CBA_Addr address = {0};

    int nRetries = 3;

    // Go through the selected clients

	// first clear all the status

	m_ctlMoveResultStatus = "";
	m_csStatus = "";

	while( m_ctlMoveResults.GetCount( ) > 0 )
	{
		m_ctlMoveResults.DeleteString( 0 );
	}

    for ( int index=0; index<iSelectCount; index++ )
    {
        DWORD dwRet;

		CString &csClient = csSourceClients[SelectedClients[index]];

		m_ctlMoveResultStatus.Format( "Client %d of %d is %s - beginning move ...", index+1, iSelectCount, csClient );

		UpdateData( FALSE );
		if ( KeepTheWindowMoving( ) != ERROR_SUCCESS )
			break;
        dwRet = MoveClient( csSourceClientsAddress[SelectedClients[index]],
                            csSourceClientsFullName[SelectedClients[index]],
                            csSourceClients[SelectedClients[index]],
                            csSourceParent,
                            csTargetParent,
                            m_csStatus );
        UpdateData( FALSE );
		if ( KeepTheWindowMoving( ) != ERROR_SUCCESS )
			break;

        if ( dwRet == ERROR_SUCCESS )
		{
			CString csClientResult;

			iNumberClientsMoved += 1;

			m_csStatus.Format( "Client %d of %d - %s moved OK.", index+1, iSelectCount, csSourceClients[SelectedClients[index]] );

			csClientResult.Format( "%s moved OK", csClient );
			m_ctlMoveResults.AddString( csClientResult );
		}
		else
        {
            //fail to move this client

			CString csClientResult;
			CString csResult = m_csStatus;

			m_csStatus.Format( "Client %d of %d - %s failed - %s.", index+1, iSelectCount, csClient, csResult );

			csClientResult.Format( "! %s failed - %s", csClient, csResult );
			m_ctlMoveResults.AddString( csClientResult );

            //add client name to list of clients
            if ( ! csFailedClients.IsEmpty() )
            {
                csFailedClients += ", ";
            }
            CString csClient;
            csClientList.GetText( SelectedClients[index], csClient );
            csFailedClients += csClient;
        }
    }

    //Update UI on move results
    if ( csFailedClients.IsEmpty() )
    {
        m_ctlMoveResultStatus.Format( "All %d clients moved OK.", iNumberClientsMoved );
    }
    else
    {
        m_ctlMoveResultStatus.Format( "%d client(s) moved OK, %d client(s) failed - highlight client in Move Results for details", 
			iNumberClientsMoved, iSelectCount - iNumberClientsMoved );
        m_ctlMoveResultStatus.AnsiToOem();
    }
    UpdateData( FALSE );

    //release Generic Configuration object
    if ( pConfig )
        pConfig->Release();

    //Deinitialize COM
    CoUninitialize();
}

void CSAVClientMoverDlg::OnSelchangeMoveResultsView() 
{
	int nIndex = m_ctlMoveResults.GetCurSel( );
	CString csSelectedStatus;

	if ( nIndex != LB_ERR )
	{
		m_ctlMoveResults.GetText( nIndex, csSelectedStatus );
		m_csStatus = csSelectedStatus;
	}
	else
	{
		m_csStatus = "";
	}

	UpdateData( FALSE );
}

HRESULT CSAVClientMoverDlg::KeepTheWindowMoving( )
{
    MSG msg;
	HRESULT hResult = ERROR_SUCCESS;

    while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
    { 
        if ( !AfxGetApp()->PumpMessage( ) ) 
        { 
            hResult = ERROR_CAN_NOT_COMPLETE; 
            ::PostQuitMessage( 0 ); 
            break; 
        } 
    } 

    // let MFC do its idle processing
    LONG lIdle = 0;
    while ( AfxGetApp()->OnIdle(lIdle++ ) )
        ;  

	return hResult;
}
