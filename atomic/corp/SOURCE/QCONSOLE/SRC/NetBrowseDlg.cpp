/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/netbrowsedlg.cpv   1.2   21 May 1998 11:28:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// NetBrowseDlg.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/netbrowsedlg.cpv  $
// 
//    Rev 1.2   21 May 1998 11:28:24   DBuches
// Added help topics.
// 
//    Rev 1.1   20 May 1998 17:13:40   DBuches
// 1st pass at help.
// 
//    Rev 1.0   08 May 1998 17:22:32   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "NetBrowseDlg.h"
#include "qconhlp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNetBrowseDlg dialog

DWORD CNetBrowseDlg::m_dwWhatsThisMap[] = {
    IDC_TREE,       IDH_QUARAN_NETWORK_BROWSER,
    0,0};
DWORD CNetBrowseDlg::m_dwHowToMap[] = {0,0};

////////////////////////////////////////////////////////////////////////////
// Function name	: CNetBrowseDlg::CNetBrowseDlg
//
// Description	    : Constructor
//
// Argument         : CWnd* pParent /*=NULL*/
//
////////////////////////////////////////////////////////////////////////////
// 5/8/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CNetBrowseDlg::CNetBrowseDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CNetBrowseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetBrowseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    // Set up tree
    m_treeCtrl.SetDisplayItemFlags( m_treeCtrl.GetDisplayItemFlags() & ~RESOURCE_TYPE_NETWORK_NETWARE );
    m_treeCtrl.SetBrowseBeneathFlags( m_treeCtrl.GetBrowseBeneathFlags() & ~RESOURCE_TYPE_SERVER );
    m_treeCtrl.SetMultiSelectState( FALSE );

    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpHowToMap = m_dwHowToMap;
    m_dwHelpButtonHelpID = IDH_QUARAN_NETWORK_BROWSER_HLPBTN;


}



////////////////////////////////////////////////////////////////////////////
// Function name	: CNetBrowseDlg::DoDataExchange
//
// Description	    : Handles DDX chores
//
// Return type		: void 
//
// Argument         : CDataExchange* pDX
//
////////////////////////////////////////////////////////////////////////////
// 5/8/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CNetBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetBrowseDlg)
    DDX_Control(pDX, IDC_TREE, m_treeCtrl);
	    // NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetBrowseDlg, CHelperDialog)
	//{{AFX_MSG_MAP(CNetBrowseDlg)
    ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetBrowseDlg message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CNetBrowseDlg::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/8/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CNetBrowseDlg::OnInitDialog() 
{
	CHelperDialog::OnInitDialog();
	
    // Need to expand tree to MSNetwork
    HTREEITEM hMsNetwork = m_treeCtrl.GetChildItem( m_treeCtrl.GetRootItem() );
    m_treeCtrl.Expand( hMsNetwork, TVE_EXPAND );
    m_treeCtrl.EnsureVisible( m_treeCtrl.GetRootItem() );
	
    // Disalbe OK button for now
    GetDlgItem( IDOK )->EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CNetBrowseDlg::OnItemexpandingTree
//
// Description	    : Handles NM_ITEMEXPANDING tree notification
//
// Return type		: void 
//
// Argument         : NMHDR* pNMHDR
// Argument         : LRESULT* pResult
//
////////////////////////////////////////////////////////////////////////////
// 5/8/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CNetBrowseDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    CNetResourceContainer *lpNetResourceContainer;

    //
    // Get the item data for this branch. This will be the CNetResourceContainer
    // class that has been allocated for each branch
    //
    lpNetResourceContainer =
        (CNetResourceContainer *)m_treeCtrl.GetItemData( pNMTreeView->itemNew.hItem );

    //
    // Use this class to enumerate the next level of network resources
    //
    lpNetResourceContainer->EnumerateNetResourceBranch( &m_treeCtrl,
                                                        pNMTreeView->itemNew.hItem );

    *pResult = 0;
}



////////////////////////////////////////////////////////////////////////////
// Function name	: CNetBrowseDlg::OnOK
//
// Description	    : Handles OK buttun
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/8/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CNetBrowseDlg::OnOK() 
{
    // Get selected item
    HTREEITEM hSelected = m_treeCtrl.GetSelectedItem();
    if( hSelected == NULL )
        {
        MessageBeep(0);
        return;
        }

    //
    // Get the item data for this branch. This will be the CNetResourceContainer
    // class that has been allocated for each branch
    //
    CNetResourceContainer *lpNetResourceContainer;
    lpNetResourceContainer =
        (CNetResourceContainer *)m_treeCtrl.GetItemData( hSelected );

    // Make sure this is a server.
    if( RESOURCE_TYPE_SERVER != lpNetResourceContainer->GetItemType() )
        {
        MessageBeep(0);
        return;
        }

    // Save off server text    
    m_sServer = m_treeCtrl.GetItemText( hSelected );

    CHelperDialog::OnOK();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CNetBrowseDlg::OnSelchangedTree
//
// Description	    : Handles Tree selection change.
//
// Return type		: void 
//
// Argument         : NMHDR* pNMHDR
// Argument         : LRESULT* pResult
//
////////////////////////////////////////////////////////////////////////////
// 5/8/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CNetBrowseDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    CNetResourceContainer *lpNetResourceContainer;

    //
    // Get the item data for this branch. This will be the CNetResourceContainer
    // class that has been allocated for each branch
    //
    lpNetResourceContainer =
        (CNetResourceContainer *)m_treeCtrl.GetItemData( pNMTreeView->itemNew.hItem );

    // Make sure this is a server.
    if( RESOURCE_TYPE_SERVER != lpNetResourceContainer->GetItemType() )
        {                                        
        GetDlgItem( IDOK )->EnableWindow( FALSE );
        }
    else
        {
        GetDlgItem( IDOK )->EnableWindow( TRUE );
        }
	
	*pResult = 0;
}
