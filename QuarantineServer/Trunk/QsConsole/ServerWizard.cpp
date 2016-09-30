/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ServerWizard.cpp : implementation file
//

#include "stdafx.h"
#include "qscon.h"
#include "ServerWizard.h"
#include "mmc.h"
#include "DCOMObject.h"
#import "qserver.tlb" no_namespace

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerWizard property page

IMPLEMENT_DYNCREATE(CServerWizard, CQPropPage)

CServerWizard::CServerWizard() : CQPropPage(CServerWizard::IDD)
{
	//{{AFX_DATA_INIT(CServerWizard)
	m_iComputer = 0;
	m_sDomain = _T("");
	m_sPassword = _T("");
	m_sUser = _T("");
	m_sServerName = _T("");
	//}}AFX_DATA_INIT
}

CServerWizard::~CServerWizard()
{
}

void CServerWizard::DoDataExchange(CDataExchange* pDX)
{
	CQPropPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerWizard)
	DDX_Radio(pDX, IDC_CHOOSER_RADIO_LOCAL_MACHINE, m_iComputer);
	DDX_Text(pDX, IDC_DOMAIN, m_sDomain);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_USER, m_sUser);
	DDX_Text(pDX, IDC_CHOOSER_EDIT_MACHINE_NAME, m_sServerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerWizard, CQPropPage)
	//{{AFX_MSG_MAP(CServerWizard)
	ON_BN_CLICKED(IDC_CHOOSER_RADIO_LOCAL_MACHINE, OnChooserRadioLocalMachine)
	ON_EN_CHANGE(IDC_CHOOSER_EDIT_MACHINE_NAME, OnChangeChooserEditMachineName)
    ON_BN_CLICKED(IDC_CHOOSER_RADIO_SPECIFIC_MACHINE, OnChooserRadioLocalMachine)
	ON_BN_CLICKED(IDC_CHOOSER_BUTTON_BROWSE_MACHINENAMES, OnChooserButtonBrowseMachinenames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerWizard message handlers


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerWizard::PostNcDestroy
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/18/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerWizard::PostNcDestroy() 
{
	CQPropPage::PostNcDestroy();

    // 
    // Notify the world that the page is gone.
    // 
    MMCPropertyChangeNotify( m_lNotifyHandle, 0 );

    delete this;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerWizard::OnInitDialog
//
// Description  : 
//
// Return type  : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/18/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerWizard::OnInitDialog() 
{
	CQPropPage::OnInitDialog();
	
    // 
    // Enable the finish button.
    // 
    ((CPropertySheet*) GetParent() )->SetWizardButtons( PSWIZB_FINISH );
    OnChooserRadioLocalMachine();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerWizard::OnChooserRadioLocalMachine
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/18/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerWizard::OnChooserRadioLocalMachine() 
{
    static int iControls[] = { IDC_CHOOSER_EDIT_MACHINE_NAME,
                               IDC_CHOOSER_BUTTON_BROWSE_MACHINENAMES,
                               IDC_USERNAME_STATIC,
                               IDC_USER,
                               IDC_PASSWORD,
                               IDC_PASSWORD_STATIC,
                               IDC_DOMAIN_STATIC,
                               IDC_DOMAIN,
                               0
        };

    // 
    // Get state of contols
    // 
    UpdateData();

    // 
    // Enable/disable controls.
    // 
    int i = 0;
    while( iControls[i] )
        {
        GetDlgItem( iControls[i++] )->EnableWindow( (BOOL) m_iComputer );
        }

    // 
    // Check to make sure that there is text in the server field.
    // if not, then we need to disbale the finish button.
    // 
    DWORD dwFlags = PSWIZB_FINISH;
    if( m_iComputer && m_sServerName.IsEmpty() )
        dwFlags = 0;

    ((CPropertySheet*) GetParent() )->SetWizardButtons( dwFlags );    
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerWizard::OnChangeChooserEditMachineName
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/18/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerWizard::OnChangeChooserEditMachineName() 
{
    // 
    // Disable finish button if remote computer selection has
    // no text.
    // 
    if( m_iComputer )
        {
        CString s;
        GetDlgItem( IDC_CHOOSER_EDIT_MACHINE_NAME )->GetWindowText( s );
    
        DWORD dwFlags = PSWIZB_FINISH;
	    if( s.IsEmpty() )
            dwFlags = 0;

        ((CPropertySheet*) GetParent() )->SetWizardButtons( dwFlags );    
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerWizard::OnChooserButtonBrowseMachinenames
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/18/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CServerWizard::OnChooserButtonBrowseMachinenames() 
{
	// Fall back to IE-style browser
	BROWSEINFO bi;
	LPITEMIDLIST lpItemIdList;
	CComPtr< IMalloc > pMalloc;
    CString sCaption;
    TCHAR szServer[ MAX_PATH ] = {0};
    
    sCaption.LoadString( IDS_COMPUTER_BROWSER_CAPTION );

	if (SUCCEEDED(SHGetSpecialFolderLocation(GetSafeHwnd(), CSIDL_NETWORK, &lpItemIdList)))
	    {
		bi.hwndOwner = GetSafeHwnd(); 
		bi.pidlRoot = lpItemIdList; 
		bi.pszDisplayName = szServer; 
		bi.lpszTitle = sCaption.GetBuffer(0); 
		bi.ulFlags = BIF_BROWSEFORCOMPUTER; 
		bi.lpfn = NULL; 
		bi.lParam = NULL; 
		bi.iImage = NULL; 

		if (SHBrowseForFolder(&bi) != NULL) 
		    {
			if ( szServer[0] != '\0') 
			    {
				GetDlgItem( IDC_CHOOSER_EDIT_MACHINE_NAME )->SetWindowText( szServer );
			    }
		    }

		if (SUCCEEDED(SHGetMalloc(&pMalloc))) 
		    {
			pMalloc->Free(lpItemIdList);
		    }
	    }
    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerWizard::OnWizardFinish
//
// Description  : Attempts connection to specified Quarantine server.
//
// Return type  : BOOL 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerWizard::OnWizardFinish() 
{
    BOOL bConnected;
    CQSConsoleData* pData = (CQSConsoleData*) m_pRootNode;

    // 
    // Fetch Data from Controls.
    // 	
    UpdateData();
    m_sServerName.TrimLeft(_T(" \\/") );
    m_sServerName.TrimRight( _T(" \\/") );
    m_sServerName.MakeUpper();
    m_sDomain.TrimLeft();
    m_sDomain.TrimRight();

    // 
    // Try connecting to remote computer.
    // 
    if( m_iComputer )
        bConnected = AttemptConnection( m_sServerName, m_sUser, m_sPassword, m_sDomain );
    else
        {
        bConnected = AttemptConnection();
        if( bConnected )
            {
            DWORD dwLen = MAX_PATH; 
            GetComputerName( m_sServerName.GetBuffer( MAX_PATH ), &dwLen );
            m_sServerName.ReleaseBuffer();
            }
        }

    // 
    // If we can connect, then all is well, bail out now.
    // 
    if( bConnected )
        {
        pData->SetServerName( m_sServerName );
        pData->SetDomainName( m_sDomain );
        pData->SetPassword( m_sPassword );
        pData->SetUserName( m_sUser );
        pData->SetDisplayServerName( m_sServerName );
        return CQPropPage::OnWizardFinish();
        }

    // 
    // Can't connect, so inform user and return failure.
    //
    CString sTitle, sText;
    sTitle.LoadString( IDS_ERROR_TITLE );
    sText.Format( IDS_CONNECT_FAILIRE_FMT, m_sServerName );
    MessageBox( sText, sTitle, MB_ICONSTOP | MB_OK );
    UpdateData( FALSE );
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CServerWizard::AttemptConnection
//
// Description  : This routine will attempt to contact the quarantine server
//                on the specified machine.
//
// Return type  : BOOL 
//
// Argument     : LPCTSTR pszServer
// Argument     : LPCTSTR pszUser
// Argument     : LPCTSTR pszPassword
// Argument     : LPCTSTR pszDomain
//
///////////////////////////////////////////////////////////////////////////////
// 3/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CServerWizard::AttemptConnection( LPCTSTR pszServer,
                                       LPCTSTR pszUser,
                                       LPCTSTR pszPassword,
                                       LPCTSTR pszDomain )
{
    CString sServer;
    BOOL bRet = TRUE;

	_ASSERTE(((CQSConsoleData*)m_pRootNode)->m_cQSInterfaces);
	if (((CQSConsoleData*)m_pRootNode)->m_cQSInterfaces == NULL)
	{
		return FALSE;
	}
    try
        {
        // 
        // If no server name is specified, assume local machine.
        // 
        if( pszServer == NULL )
            {
            DWORD dwLen = MAX_PATH; 
            GetComputerName( sServer.GetBuffer( MAX_PATH ), &dwLen );
            sServer.ReleaseBuffer();
            }
        else
            sServer = pszServer;

        // 
        // Set up user information.
        // 
        ((CQSConsoleData*)m_pRootNode)->m_cQSInterfaces->m_pQServer.DCO_SetServer( sServer );
        ((CQSConsoleData*)m_pRootNode)->m_cQSInterfaces->m_pQServer.DCO_SetIdentity( pszUser,
                                                          pszPassword,
                                                          pszDomain );

        // 
        // Attempt connection.
        // 
        HRESULT hr = ((CQSConsoleData*)m_pRootNode)->m_cQSInterfaces->m_pQServer.DCO_CreateObject();
        if( FAILED( hr ) )
            _com_issue_error( hr );
        }
    catch( _com_error e )
        {
        // 
        // Error connecting to server.
        // 
        bRet = FALSE;
        }

    return bRet;
}