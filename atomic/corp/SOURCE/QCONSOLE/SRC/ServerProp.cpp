/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/serverprop.cpv   1.4   20 May 1998 17:15:14   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ServerProp.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/serverprop.cpv  $
// 
//    Rev 1.4   20 May 1998 17:15:14   DBuches
// 1st pass at help
// 
//    Rev 1.3   07 May 1998 15:07:46   DBuches
// Added help button.
// 
//    Rev 1.2   10 Apr 1998 14:14:34   DBuches
// Added N/A text to domain name field if domain is not known.
// 
//    Rev 1.1   06 Mar 1998 11:20:26   DBuches
// Updated to use IQuarantineItem pointer to update data.
// 
//    Rev 1.0   27 Feb 1998 15:09:00   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "platform.h"
#include "qconsole.h"
#include "ServerProp.h"
#include "xapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerProp property page

IMPLEMENT_DYNCREATE(CServerProp, CItemPage)

CServerProp::CServerProp() : CItemPage(CServerProp::IDD)
{
	//{{AFX_DATA_INIT(CServerProp)
	m_sItemName = _T("");
	m_sUserName = _T("");
	m_sDomainName = _T("");
	m_sComputerName = _T("");
	//}}AFX_DATA_INIT
	m_sUserName.LoadString( IDS_NA );// = _T("");
	m_sDomainName.LoadString( IDS_NA );// = _T("");
	m_sComputerName.LoadString( IDS_NA );// = _T("");

}

CServerProp::~CServerProp()
{
}

void CServerProp::DoDataExchange(CDataExchange* pDX)
{
	CItemPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerProp)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_ImageButton);
	DDX_Text(pDX, IDC_NAME_EDIT, m_sItemName);
	DDX_Text(pDX, IDC_USERNAME_EDIT, m_sUserName);
	DDX_Text(pDX, IDC_DOMAIN_EDIT, m_sDomainName);
	DDX_Text(pDX, IDC_COMPUTER_EDIT, m_sComputerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerProp, CItemPage)
	//{{AFX_MSG_MAP(CServerProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerProp message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CServerProp::OnInitDialog
//
// Description	    : Handle WM_INITDIALOG message
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/4/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CServerProp::OnInitDialog() 
{
	CItemPage::OnInitDialog();
	
    // Populate controls.
    LPTSTR p = m_sItemName.GetBuffer( MAX_PATH );
    m_pItem->GetOriginalAnsiFilename( p, MAX_PATH );
    NameReturnFile( p, p );
    m_sItemName.ReleaseBuffer();

    m_pItem->GetOriginalOwnerName( m_sUserName.GetBuffer( 32 ), 32 );
    m_sUserName.ReleaseBuffer();
	
    m_pItem->GetOriginalMachineName( m_sComputerName.GetBuffer( MAX_COMPUTERNAME_LENGTH + 1 ),
                                    MAX_COMPUTERNAME_LENGTH + 1 );
    m_sComputerName.ReleaseBuffer();
   
    m_pItem->GetOriginalMachineDomain( m_sDomainName.GetBuffer(MAX_PATH), MAX_PATH );
    m_sDomainName.ReleaseBuffer();

    // If no domain name, display not applicable text
    if( m_sDomainName.GetLength() == 0 )
        m_sDomainName.LoadString( IDS_NA );


    // Blast data to controls
    UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
