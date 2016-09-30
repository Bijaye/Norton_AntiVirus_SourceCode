//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// ResultsDlg.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "ResultsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CResultsDlg dialog

//////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
CResultsDlg::CResultsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CResultsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResultsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pImageList = NULL;
	m_iCaptionID = IDS_REPAIR_RESULTS;
	m_bDetails = FALSE;
}


//////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
CResultsDlg::~CResultsDlg()
{
	if(m_bDetails)
		return;

	// Delete all items
	POSITION pos;
	for( pos = m_aItemList.GetHeadPosition(); pos != NULL; )
	{
		CResultsListItem *pItem = m_aItemList.GetNext(pos);
		POSITION pos1;
		for( pos1 = pItem->m_aSubActionList.GetHeadPosition(); pos1 != NULL; )
		{
			delete pItem->m_aSubActionList.GetNext(pos1);
		}            

		delete pItem;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Description	    : Handles DDX chores
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CResultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultsDlg)
	DDX_Control(pDX, IDC_ITEM_LIST, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResultsDlg, CDialog)
	//{{AFX_MSG_MAP(CResultsDlg)
	ON_BN_CLICKED(IDC_BTN_DETAILS, OnBnClickedBtnDetails)
	ON_NOTIFY(NM_DBLCLK , IDC_ITEM_LIST, OnListViewItemDoubleClick)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ITEM_LIST, OnLvnItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////
// CResultsDlg message handlers

void CResultsDlg::OnBnClickedBtnDetails()
{
	// If details view, don't do anything
	if(m_bDetails)
		return;

	CResultsListItem *pCurSel = GetCurSelItem();
	if(!pCurSel)
		return;

	// Setup status dialog.
	CResultsDlg dlg;
	dlg.SetImageList( m_pImageList );

	// Set details flag
	dlg.m_bDetails = TRUE;

	// setup caption
	dlg.SetCaption( IDS_RESULTS_DETAILS );

	// Add to list
	dlg.m_aItemList.AddTail( pCurSel );

	// Fire off results dialog.
	dlg.DoModal();
}


CResultsListItem * CResultsDlg::GetCurSelItem()
{
	POSITION posListCtrl = m_ListCtrl.GetFirstSelectedItemPosition();
	if (posListCtrl == NULL)
		return NULL;
	else
	{
		while (posListCtrl)
		{
			int nItem = m_ListCtrl.GetNextSelectedItem(posListCtrl);

			DWORD_PTR dwpData = m_ListCtrl.GetItemData(nItem);

			POSITION posItemList = m_aItemList.GetHeadPosition();
			while(posItemList)
			{
				DWORD_PTR dwpItem = (DWORD_PTR)&(m_aItemList.GetNext(posItemList)->m_Action);
				if(dwpData == dwpItem)
					return (CResultsListItem*) dwpItem;
			}

		}
	}

	return NULL;
}


void CResultsDlg::OnListViewItemDoubleClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedBtnDetails();   
}


void CResultsDlg::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	// If details view, don't do anything
	if(m_bDetails)
		return;

	CResultsListItem *pCurSel = GetCurSelItem();
	if(pCurSel)
	{
		DWORD dwCount = pCurSel->m_aSubActionList.GetCount();
		if(dwCount)
			GetDlgItem(IDC_BTN_DETAILS)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_BTN_DETAILS)->EnableWindow(FALSE);
	}
}


//////////////////////////////////////////////////////////////////////////////
// Description	    : Handles WM_INITDIALOG message.
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
BOOL CResultsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set correct caption
	CString sCaption;
	sCaption.LoadString( m_iCaptionID );
	SetWindowText( sCaption );

	// Set up list contorl
	SetupListCtrl();

	if(!m_bDetails)
	{
		POSITION pos;
		for( pos = m_aItemList.GetHeadPosition(); pos != NULL; )
		{
			// Set up item to insert.
			LV_ITEM item;
			ZeroMemory( &item, sizeof( LV_ITEM ) );
			item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.iImage = I_IMAGECALLBACK;
			item.lParam = (LPARAM) &(m_aItemList.GetNext( pos )->m_Action);

			// Insert this record into the list
			m_ListCtrl.InsertItem( &item );
		}

		GetDlgItem(IDC_BTN_DETAILS)->ShowWindow(TRUE);
		GetDlgItem(IDC_BTN_DETAILS)->EnableWindow(FALSE);
	}
	else
	{
		POSITION pos, posTemp;
		posTemp = m_aItemList.GetHeadPosition();
		CRepairActionList *pList = &(m_aItemList.GetNext(posTemp)->m_aSubActionList);
		for( pos = pList->GetHeadPosition(); pos != NULL; )
		{
			// Set up item to insert.
			LV_ITEM item;
			ZeroMemory( &item, sizeof( LV_ITEM ) );
			item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.iImage = I_IMAGECALLBACK;
			item.lParam = (LPARAM) pList->GetNext( pos );

			// Insert this record into the list
			m_ListCtrl.InsertItem( &item );
		}

		GetDlgItem(IDC_BTN_DETAILS)->ShowWindow(FALSE);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CResultsDlg::SetupListCtrl()
{
	// Get width of control
	CRect rect;
	m_ListCtrl.GetClientRect( &rect );

	// Add columns
	CString s;
	if( !m_bDetails ) 
		s.LoadString( IDS_COLNAME_THREAT_NAME );
	else
		s.LoadString( IDS_COLNAME_ITEM_NAME );

	m_ListCtrl.InsertColumn( RESULTS_FILE_NAME,
		s,
		LVCFMT_LEFT,
		rect.Width() / 2 );

	s.LoadString( IDS_COLNAME_RESULTS );
	m_ListCtrl.InsertColumn( RESULTS_ACTION,
		s,
		LVCFMT_LEFT,
		rect.Width() - (rect.Width() / 2));

	// Set up image list
	m_ListCtrl.SetImageList( m_pImageList, LVSIL_SMALL );

	DWORD dwExtendedStyle = m_ListCtrl.GetExtendedStyle();
	m_ListCtrl.SetExtendedStyle(dwExtendedStyle | LVS_EX_FULLROWSELECT);

}


//////////////////////////////////////////////////////////////////////////////
// Description	    : Sets the image list for our control
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CResultsDlg::SetImageList( CImageList* pImageList )
{
	// Set image list
	m_pImageList = pImageList;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// CResultsListCtrl

IMPLEMENT_DYNAMIC( CResultsListCtrl, CSortedListCtrl )

BEGIN_MESSAGE_MAP(CResultsListCtrl, CSortedListCtrl )
	//{{AFX_MSG_MAP(CResultsListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//////////////////////////////////////////////////////////////////////////////
// Description	    : Handles getting display information.
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CResultsListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	CRepairAction* p = (CRepairAction*) pDispInfo->item.lParam;

	// Get the text for this item
	if( pDispInfo->item.mask & LVIF_TEXT )
	{
		switch( pDispInfo->item.iSubItem )
		{
		case RESULTS_FILE_NAME:
			lstrcpyn( pDispInfo->item.pszText, p->GetName(), pDispInfo->item.cchTextMax );
			break;

		case RESULTS_ACTION:
			{
				CString s = p->Format();
				lstrcpyn( pDispInfo->item.pszText, s, pDispInfo->item.cchTextMax );
			}
			break;
		}
	}

	// Get the image for this item.
	if( pDispInfo->item.mask & LVIF_IMAGE )
	{
		// Get icon index
		pDispInfo->item.iImage = p->GetIconIndex();
	}
}


//////////////////////////////////////////////////////////////////////////////
// Description	    : Sort callback function for list control
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
int CResultsListCtrl::CompareFunc( LPARAM lParam1, LPARAM lParam2 )
{
	CRepairAction* p1 = (CRepairAction*) lParam1;
	CRepairAction* p2 = (CRepairAction*) lParam2;

	switch( m_nSortColumn )
	{
	case RESULTS_FILE_NAME:
		if( m_bSortAscending )
			return p1->GetName().Compare( p2->GetName() );
		else
			return p2->GetName().Compare( p1->GetName() );
		break;

	case RESULTS_ACTION:
		if( m_bSortAscending )
			return p1->Compare( p2 );
		else
			return p2->Compare( p1 );
		break;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// CRepairAction


//////////////////////////////////////////////////////////////////////////////
// Argument         : int iAction - what type of action item this is
// Argument         : LPCTSTR szData - data to incorporate into format
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
CRepairAction::CRepairAction()
{
	m_iRepairAction = RESULTS_ACTION_NO_ACTION;
	m_iIconIndex = NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Description	    : Initializes this object
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CRepairAction::SetAction(int iAction, LPCTSTR szData)
{
	// Save off action
	m_iRepairAction = iAction;

	// Save off data string
	m_sData = szData;
}


//////////////////////////////////////////////////////////////////////////////
// Description	    : Creates a string containing details of the repair action
// 3/11/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
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

	case RESULTS_ACTION_DELETED:
		iStrID = IDS_RESULT_ACTION_DELETED;
		break;

	case RESULTS_ACTION_ERROR:
		iStrID = IDS_RESULT_ACTION_ERROR;
		break;

	case RESULTS_ACTION_CANCELLED:
		iStrID = IDS_RESULT_ACTION_CANCELLED;
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


//////////////////////////////////////////////////////////////////////////////
// Description	    : See definition of CString::Compare for details
// 3/11/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
int CRepairAction::Compare( CRepairAction *p)
{
	if( m_iRepairAction < p->m_iRepairAction )
		return -1;
	else if( m_iRepairAction > p->m_iRepairAction )
		return 1;

	return 0;
}


//////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
CRepairAction::~CRepairAction()
{
}


//////////////////////////////////////////////////////////////////////////////
// Description	    : Override of == operatior
// 3/9/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
BOOL CRepairAction::operator == ( int iAction )
{
	return m_iRepairAction == iAction;
}
