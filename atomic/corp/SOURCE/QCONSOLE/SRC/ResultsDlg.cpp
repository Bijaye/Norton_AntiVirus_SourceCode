/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/ResultsDlg.cpv   1.3   10 Apr 1998 14:14:06   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ResultsDlg.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/ResultsDlg.cpv  $
// 
//    Rev 1.3   10 Apr 1998 14:14:06   DBuches
// Added Compressed file type.
// 
//    Rev 1.2   31 Mar 1998 15:02:46   DBuches
// Added GetAction() method.
// 
//    Rev 1.1   13 Mar 1998 15:21:56   DBuches
// Added support for repair.
// 
//    Rev 1.0   11 Mar 1998 15:19:00   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "ResultsDlg.h"
#include "cdcos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CResultsDlg dialog



////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsDlg::CResultsDlg
//
// Description	    : Constructor
//
// Argument         : CWnd* pParent /*=NULL*/
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CResultsDlg::CResultsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResultsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResultsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_pImageList = NULL;
    m_iCaptionID = IDS_REPAIR_RESULTS;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsDlg::~CResultsDlg
//
// Description	    : Destructor
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CResultsDlg::~CResultsDlg()
{
    // Delete all items
    POSITION pos;
    for( pos = m_aItemList.GetHeadPosition(); pos != NULL; )
        {
        delete m_aItemList.GetNext(pos);
        }
}

////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsDlg::DoDataExchange
//
// Description	    : Handles DDX chores
//
// Return type		: void
//
// Argument         : CDataExchange* pDX
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CResultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultsDlg)
	DDX_Control(pDX, IDC_ITEM_LIST, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResultsDlg, CDialog)
	//{{AFX_MSG_MAP(CResultsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultsDlg message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsDlg::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG message.
//
// Return type		: BOOL
//
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CResultsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set correct caption
    CString sCaption;
    sCaption.LoadString( m_iCaptionID );
    SetWindowText( sCaption );
    
    // Set up list contorl
    SetupListCtrl();

    POSITION pos;
    for( pos = m_aItemList.GetHeadPosition(); pos != NULL; )
        {
        // Set up item to insert.
        LV_ITEM item;
        ZeroMemory( &item, sizeof( LV_ITEM ) );
        item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
        item.pszText = LPSTR_TEXTCALLBACK;
        item.iImage = I_IMAGECALLBACK;
        item.lParam = (LPARAM) m_aItemList.GetNext( pos );

        // Insert this record into the list
        m_ListCtrl.InsertItem( &item );
        }


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsDlg::SetupListCtrl
//
// Description	    :
//
// Return type		: void
//
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CResultsDlg::SetupListCtrl()
{
    // Get width of control
    CRect rect;
    m_ListCtrl.GetClientRect( &rect );

    // Add columns
    CString s;
    s.LoadString( IDS_FILE_NAME );
    m_ListCtrl.InsertColumn( RESULTS_FILE_NAME,
                             s,
                             LVCFMT_LEFT,
                             rect.Width() / 3 );

    s.LoadString( IDS_ACTION_TAKEN );
    m_ListCtrl.InsertColumn( RESULTS_ACTION,
                             s,
                             LVCFMT_LEFT,
                             (rect.Width() / 3) * 2 );

    // Set up image list
    m_ListCtrl.SetImageList( m_pImageList, LVSIL_SMALL );

    // Need to set up owner draw column sort stuff here.
    CHeaderCtrl *pHdr = (CHeaderCtrl*) m_ListCtrl.GetDlgItem(0);

    // Make each item owner draw
    HD_ITEM item;
    for(int i= RESULTS_FILE_NAME; i < RESULTS_MAX; i++)
        {
        item.mask = HDI_FORMAT;
        item.fmt = HDF_OWNERDRAW;
        pHdr->SetItem(i, &item );
        }
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsDlg::SetImageList
//
// Description	    : Sets the image list for our control
//
// Return type		: void
//
// Argument         : CImageList* pImageList
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CResultsDlg::SetImageList( CImageList* pImageList )
{
    // Set image list
    m_pImageList = pImageList;
}



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
// CResultsListCtrl

IMPLEMENT_DYNAMIC( CResultsListCtrl, CSortedListCtrl )

BEGIN_MESSAGE_MAP(CResultsListCtrl, CSortedListCtrl )
	//{{AFX_MSG_MAP(CResultsListCtrl)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsListCtrl::OnGetdispinfo
//
// Description	    : Handles getting display information.
//
// Return type		: void 
//
// Argument         : NMHDR* pNMHDR
// Argument         : LRESULT* pResult
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CResultsListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

    CResultsListItem* p = (CResultsListItem*) pDispInfo->item.lParam;

    // Get the text for this item
    if( pDispInfo->item.mask & LVIF_TEXT )
        {
        switch( pDispInfo->item.iSubItem )
            {
            case RESULTS_FILE_NAME:
                lstrcpyn( pDispInfo->item.pszText, p->m_sItemName, pDispInfo->item.cchTextMax );
                break;

            case RESULTS_ACTION:
                {
                CString s = p->m_Action.Format();
                lstrcpyn( pDispInfo->item.pszText, s, pDispInfo->item.cchTextMax );
                }
                break;
            }
        }
    
    // Get the image for this item.
    if( pDispInfo->item.mask & LVIF_IMAGE )
        {
        // Get icon index
        pDispInfo->item.iImage = p->m_iIconIndex;
        }

}

////////////////////////////////////////////////////////////////////////////
// Function name	: CResultsListCtrl::CompareFunc
//
// Description	    : Sort callback function for list control
//
// Return type		: int 
//
// Argument         : LPARAM lParam1
// Argument         : LPARAM lParam2
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CResultsListCtrl::CompareFunc( LPARAM lParam1, LPARAM lParam2 )
{
    CResultsListItem* p1 = (CResultsListItem*) lParam1;
    CResultsListItem* p2 = (CResultsListItem*) lParam2;

    switch( m_nSortColumn )
        {
        case RESULTS_FILE_NAME:
            if( m_bSortAscending )
                return p1->m_sItemName.Compare( p2->m_sItemName );
            else
                return p2->m_sItemName.Compare( p1->m_sItemName );
            break;

        case RESULTS_ACTION:
            if( m_bSortAscending )
                return p1->m_Action.Compare( &p2->m_Action );
            else
                return p2->m_Action.Compare( &p1->m_Action );
            break;
        }

    return 0;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CRepairAction


////////////////////////////////////////////////////////////////////////////
// Function name	: CRepairAction::CRepairAction
//
// Description	    : Constructor
//
// Argument         : int iAction - what type of action item this is
// Argument         : LPCTSTR szData - data to incorporate into format
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CRepairAction::CRepairAction()
{
    m_iRepairAction = RESULTS_ACTION_NO_ACTION;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CRepairAction::SetAction
//
// Description	    : Initializes this object
//
// Return type		: 
//
// Argument         : int iAction
// Argument         : LPCTSTR szData
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CRepairAction::SetAction(int iAction, LPCTSTR szData)
{

    // Save off action
    m_iRepairAction = iAction;

    // Save off data string
    m_sData = szData;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CRepairAction::Format
//
// Description	    : Creates a string containing details of the repair action
//
// Return type		: CString 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CString CRepairAction::Format()
{
    // Build correct string based on action
    int iStrID;
    CString s;
    switch( m_iRepairAction )
        {
        case RESULTS_ACTION_RESTORED:
            iStrID = IDS_RESULT_ACTION_RESTORED;
            break;
            
        case RESULTS_ACTION_REPAIRED:
            iStrID = IDS_RESULT_ACTION_REPAIRED;
            break;

        case RESULTS_ACTION_STILL_INFECTED:
            iStrID = IDS_RESULT_REPAIR_FAILED;
            break;
        
        case RESULTS_ACTION_COMPRESSED_ITEM:
            iStrID = IDS_RESULT_ACTION_COMPRESSED;
            break;
        
        case RESULTS_ACTION_NO_ACTION:


        default:
            s.LoadString( IDS_RESULT_ACTION_NOACTION );
            return s;
        }

    // Build string.
    s.Format( iStrID, m_sData );
    return s;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CRepairAction::Compare
//
// Description	    : See definition of CString::Compare for details
//
// Return type		: int 
//
// Argument         : CRepairAction *p
//
////////////////////////////////////////////////////////////////////////////
// 3/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CRepairAction::Compare( CRepairAction *p)
{
    if( m_iRepairAction < p->m_iRepairAction )
        return -1;
    else if( m_iRepairAction > p->m_iRepairAction )
        return 1;

    return 0;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CRepairAction::~CRepairAction
//
// Description	    : Destructor
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CRepairAction::~CRepairAction()
{
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CRepairAction::operator ==
//
// Description	    : Override of == operatior
//
// Return type		: BOOL  
//
// Argument         : int iAction
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CRepairAction::operator == ( int iAction )
{
    return m_iRepairAction == iAction;
}


