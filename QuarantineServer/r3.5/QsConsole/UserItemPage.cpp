/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// UserItemPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "UserItemPage.h"
#include "winsock2.h"
#include "macros.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserItemPage property page

IMPLEMENT_DYNCREATE(CUserItemPage, CQPropPage)

CUserItemPage::CUserItemPage() : CQPropPage(CUserItemPage::IDD)
{
	//{{AFX_DATA_INIT(CUserItemPage)
	m_sComputer = _T("");
	m_sDomain = _T("");
	m_sFileName = _T("");
	m_sUserName = _T("");
	m_sIPAddress = _T("");
	m_sNACAddress = _T("");
	m_sPlatform = _T("");
	m_sProduct = _T("");
	//}}AFX_DATA_INIT
    m_dwRef = 1;
}

CUserItemPage::~CUserItemPage()
{
}

void CUserItemPage::DoDataExchange(CDataExchange* pDX)
{
	CQPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserItemPage)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_iconBtn);
	DDX_Text(pDX, IDC_COMPUTER_EDIT, m_sComputer);
	DDX_Text(pDX, IDC_DOMAIN_EDIT, m_sDomain);
	DDX_Text(pDX, IDC_NAME_EDIT, m_sFileName);
	DDX_Text(pDX, IDC_USERNAME_EDIT, m_sUserName);
	DDX_Text(pDX, IDC_IPADDRESS_EDIT, m_sIPAddress);
	DDX_Text(pDX, IDC_NACADDRESS_EDIT, m_sNACAddress);
	DDX_Text(pDX, IDC_PLATFORM_EDIT, m_sPlatform);
	DDX_Text(pDX, IDC_PRODUCT_EDIT, m_sProduct);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserItemPage, CQPropPage)
	//{{AFX_MSG_MAP(CUserItemPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserItemPage message handlers



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CUserItemPage::Initialize
//
// Description   : Sets up values for this dialog
//
// Return type   : HRESULT 
//
// Argument      : CItemData* pItemData
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CUserItemPage::Initialize( CItemData* pItemData )
{
    HRESULT hr = S_OK;
    
    // 
    // Get information.
    // 
    m_sFileName = pItemData->m_sFileName;
    m_sUserName = pItemData->m_sUserName;
    m_sComputer = pItemData->m_sMachineName;
    m_sPlatform = pItemData->m_sPlatform;
    m_sProduct = pItemData->m_sProduct;

    // 
    // Need to handle addresses.
    // 
    GetAddressFields( pItemData->m_sAddress );

    
    if( pItemData->m_sDomainName.GetLength() )
        m_sDomain = pItemData->m_sDomainName;
    else
        m_sDomain.LoadString( IDS_NA );

    // 
    // Set up proper icon.
    // 
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    SHGetFileInfo( m_sFileName, 
                   FILE_ATTRIBUTE_NORMAL,
                   &sfi, 
                   sizeof( SHFILEINFO ), 
                   SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES 
                   );

    m_iconBtn.SetIcon( sfi.hIcon );
    
    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CUserItemPage::GetAddressFields
//
// Description  : 
//
// Return type  : void 
//
// Argument     : LPCTSTR szAddress
//
///////////////////////////////////////////////////////////////////////////////
// 3/30/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CUserItemPage::GetAddressFields( LPCTSTR szAddress )
{
USES_CONVERSION;

    LPTSTR pszTemp;
    LPTSTR pszToken;

    // 
    // Set to unknown for now.
    // 
    m_sIPAddress.LoadString( IDS_UNKNOWN );
    m_sNACAddress.LoadString( IDS_UNKNOWN );

    // 
    // Check to see if we have work to do here.
    // 
    if( lstrlen( szAddress ) == 0 )
        return;

    try
        {
        // 
        // Copy to temp buffer.
        // 
        pszTemp = new TCHAR[lstrlen( szAddress ) + 1];
        lstrcpy( pszTemp, szAddress );

        pszToken = _tcstok( pszTemp, _T( " " ) ); 
        while( pszToken )
            {
            // 
            // Look for the IP address.
            // 
            if( inet_addr( T2A(pszToken) ) == INADDR_NONE )
                m_sNACAddress = pszToken;
            else
                m_sIPAddress = pszToken;

            pszToken = _tcstok( NULL, _T( " " ) );
            }

        delete [] pszTemp;

        }
    catch( CMemoryException *e )
        {
        e->Delete();
        }
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CUserItemPage::PostNcDestroy
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CUserItemPage::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CQPropPage::PostNcDestroy();

    Release();
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CUserItemPage::OnInitDialog
//
// Description  : 
//
// Return type  : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 4/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CUserItemPage::OnInitDialog() 
{
	CQPropPage::OnInitDialog();
	
    m_dwRef++;	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL CUserItemPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    char szPath[512];
    CString sTopic;
    // INITIALIZE
    HWND hHelpWin = NULL;
    sTopic = _T(" ");

	AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    NMHDR* pNMHDR = (NMHDR*)lParam;

	if( pNMHDR->code != PSN_HELP)   //********* in "Prsht.h"
    return(S_FALSE);

     CWnd *pwnd = GetActiveWindow();
    sTopic.Format( _T("%s::/%s"), m_sHelpFile, QCONSOLE_HELPTOPIC_SAMP_USER_PROP);
#ifdef _UNICODE
     int iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTopic.GetBuffer(0),-1,
                                   (LPSTR)szPath,sizeof(szPath),NULL,NULL);
     sTopic.ReleaseBuffer(-1);
     if( iRet == FALSE )
        return(FALSE);
#else
    vpstrncpy( szPath, sTopic.GetBuffer(0), sizeof(szPath) );
    sTopic.ReleaseBuffer(-1);
#endif

    hHelpWin =HtmlHelpA(
                    pwnd->m_hWnd,
                    szPath,                // (LPCTSTR)sTopic,    // "c:\\Help.chm::/Intro.htm"
                    HH_DISPLAY_TOPIC,         // 
                    NULL) ;
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}
