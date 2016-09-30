//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// qconsoleView.cpp : implementation of the CQconsoleView class
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/qconsoleView.cpv  $
// 
//    Rev 1.29   30 Jun 1998 22:23:56   jtaylor
// Enhanced the error reporting for Scan and Deliver.
// 
//    Rev 1.28   26 Jun 1998 18:25:40   jtaylor
// Prevent an aborted repair from leaving a temporary file.
// 
//    Rev 1.27   26 Jun 1998 17:09:52   jtaylor
// Prevent an aborted repair from updating the file status.
// 
//    Rev 1.26   11 Jun 1998 15:48:28   DBuches
// Added support for trial ware.
// 
//    Rev 1.25   04 Jun 1998 15:37:24   DBuches
// Changed to work with GMT times.
// 
//    Rev 1.24   29 May 1998 15:28:52   DBuches
// Fixed problem with repairing from the backup folder.
// 
//    Rev 1.23   26 May 1998 18:03:56   DBuches
// Fixed problem responding to the Arrange Items by... menu items
// 
//    Rev 1.22   21 May 1998 11:27:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.21   18 May 1998 13:33:46   DBuches
// 1st pass at corporate user directory creation.
// 
//    Rev 1.20   12 May 1998 15:20:52   DBuches
// Added code for NT security support.
// 
//    Rev 1.19   11 May 1998 15:45:50   DBuches
// Hooked up corporate mode repair prompt.
// 
//    Rev 1.18   08 May 1998 15:15:58   DBuches
// Replaced call to MoveFileEx() with CopyFile() and DeleteFile().
// 
//    Rev 1.17   07 May 1998 18:18:54   DBuches
// Added code to enforce operaion restrictions in All Items view mode.
// 
//    Rev 1.16   07 May 1998 15:08:12   DBuches
// Added support for status text.
// 
//    Rev 1.15   28 Apr 1998 15:14:00   DBuches
// Changed message prompts to Yes/No dialogs.
// 
//    Rev 1.14   27 Apr 1998 16:11:12   DBuches
// Fixed a few minor bugs, and hooked up corporate mode stuff.
// 
//    Rev 1.13   21 Apr 1998 17:17:46   DBuches
// Added warning dialogs.
//
//    Rev 1.12   14 Apr 1998 19:48:32   JTaylor
// Implemented call to Scan and Deliver.
//
//    Rev 1.11   10 Apr 1998 14:14:54   DBuches
// Added extra error checking.
//
//    Rev 1.10   07 Apr 1998 16:49:40   DBuches
// 1st pass at startup scanning.
//
//    Rev 1.9   03 Apr 1998 13:28:50   DBuches
// 1st pass at options.
//
//    Rev 1.8   31 Mar 1998 15:02:18   DBuches
// Switched to using MFC data maps for keeping track of quarantine items.
//
//    Rev 1.7   13 Mar 1998 17:25:26   DBuches
// Now, sort by virus name and status.
//
//    Rev 1.6   13 Mar 1998 15:22:46   DBuches
// Hooked up item repair.
//
//    Rev 1.5   11 Mar 1998 15:16:40   DBuches
// 1st pass at Restore item.
//
//    Rev 1.4   06 Mar 1998 17:17:18   DBuches
// Hooked up delete menu action.
//
//    Rev 1.3   06 Mar 1998 11:21:52   DBuches
// Checked in more work in progress.
//
//    Rev 1.2   03 Mar 1998 17:04:48   DBuches
// 1st pass at callback driven list.
//
//    Rev 1.1   02 Mar 1998 15:35:30   DBuches
// Checked in work in progress.
//
//    Rev 1.0   27 Feb 1998 15:09:00   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SIMON.h"
#include "qconsole.h"
#include "platform.h"
#include "qconsoleDoc.h"
#include "qconsoleView.h"

#include "ItemPropSheet.h"
#include "cdcos.h"
#include "xapi.h"
#include "progdlg.h"
#include "qconhlp.h"
#include "AVRESBranding.h"
#include "NAVDefutilsLoader.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepClientBase.h"
#include "cltPepConstants.h"

#include "qconres.h"
#include "NAVHelpLauncher.h"

#include "IScanDeliver.h"

// Include to get the name of the category string in the qspak item
#include "qsfields.h"
#include "QItemEZ.h"

#include "RestoreDlg.h"

#include "AVccModuleID.h"
#include "AvEvents.h"
#include "AllNavEvents.h"
#include "NAVEventFactoryLoader.h"
#include "OSInfo.h"
#include "NAVVersion.h"

#include "ccSymStringImpl.h"	// ccSym::CStringImpl

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Index of generic executable icon in system image list.
#define EXE_IMAGE_INDEX 2

// Defines for the sort up and sort down arrows in Common Control 6 used to draw
// the column headers in our listview
// We have to do this becuase if we define _WIN32_WINNT=0x501, The CFileDialog
// destructor causes a crash

#ifndef HDF_SORTUP
#define HDF_SORTUP              0x0400
#endif

#ifndef HDF_SORTDOWN
#define HDF_SORTDOWN            0x0200
#endif

CString CQconsoleView::m_sProductName;
CString CQconsoleView::m_sAppName;


/////////////////////////////////////////////////////////////////////////////
// CQconsoleView

IMPLEMENT_DYNCREATE(CQconsoleView, CListView)

BEGIN_MESSAGE_MAP(CQconsoleView, CListView)
	//{{AFX_MSG_MAP(CQconsoleView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ACTION_REPAIR, OnActionRepair)
	ON_COMMAND(ID_ACTION_DELETEITEM, OnActionDeleteItem)
	ON_COMMAND(ID_ACTION_RESTORE, OnActionRestore)
	ON_COMMAND(ID_ACTION_PROPERTIES, OnActionProperties)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_ACTION_SUBMITTOSARC, OnActionSubmitToSarc)
	ON_WM_DRAWITEM()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_UPDATE_COMMAND_UI(ID_ACTION_PROPERTIES, OnUpdateProperties)
	ON_UPDATE_COMMAND_UI(ID_ACTION_REPAIR, OnUpdateSelectedItem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_UPDATE_COMMAND_UI(ID_ACTION_DELETEITEM, OnUpdateSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_ACTION_RESTORE, OnUpdateSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SUBMITTOSARC, OnUpdateSelectedItem)
	ON_COMMAND(ID_VIEW_CONTENTS, OnViewContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CQconsoleView construction/destruction

CQconsoleView::CQconsoleView()
{
	m_bSortAscending = FALSE;
	m_bExclusionsInitialized = false;
	m_bUnsavedExclusions = false;

	if(m_sProductName.IsEmpty())
	{
		CBrandingRes BrandRes;
		m_sProductName = BrandRes.ProductName();
		m_sAppName = BrandRes.GetString(IDS_QUARANTINE_APP_NAME);
	}
}

CQconsoleView::~CQconsoleView()
{
	if(m_bExclusionsInitialized && (m_spExclusions != NULL))
	{
		NavExclusions::ExResult res = NavExclusions::Success;
		res = m_spExclusions->uninitialize(false);
		if(NavExclusions::Success != res)
		{
			CCTRACEW(_T("%s - Error while uninitializing exclusion manager. ExResult = %d"), __FUNCTION__, res);
		}

		m_spExclusions.Release();
	}
}


BOOL CQconsoleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CQconsoleView drawing

void CQconsoleView::OnDraw(CDC* pDC)
{
	// TODO: add draw code for native data here
}


/////////////////////////////////////////////////////////////////////////////
// CQconsoleView diagnostics

#ifdef _DEBUG
void CQconsoleView::AssertValid() const
{
	CListView::AssertValid();
}

void CQconsoleView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CQconsoleDoc* CQconsoleView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CQconsoleDoc)));
	return (CQconsoleDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CQconsoleView message handlers

////////////////////////////////////////////////////////////////////////////////
// Override of CView::OnInitialUpdate().
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	// Build list of objects
	pDoc->RebuildItemArrays();

	// Update our views.
	pDoc->UpdateAllViews(NULL, LISTVIEW_REFRESH, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// Override for CWnd::Create().  Adjust window style here.
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// Create the control
	BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);
	if(pDoc->GetExpandedThreatShowSetting())
		m_iLastViewMode = VIEW_MODE_EXPANDED_THREATS;
	else
		m_iLastViewMode = VIEW_MODE_VIRAL_THREATS;


	// Set up correct list view style.
	if(bRet)
	{
		CListCtrl& ListCtrl = GetListCtrl();

		// Get saved style and set up.
		DWORD dwViewStyle = GetDocument()->GetListViewStyle();
		ListCtrl.ModifyStyle(0, dwViewStyle | LVS_SHAREIMAGELISTS);

		ListView_SetExtendedListViewStyle(ListCtrl, LVS_EX_FULLROWSELECT);

		// Set up our list control.
		SetupListCtrl();
	}

	return bRet;
}


////////////////////////////////////////////////////////////////////////////////
// Set up essentials for our list control.
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::SetupListCtrl()
{
	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Get system image lists for our use here.  This is done by making calls
	// to SHGetFileInfo().  See SDK docs for more details
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);

	// Get small image list
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	DWORD dwImageListHandle = SHGetFileInfo(szPath,
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_SMALLICON
		);

	ASSERT(dwImageListHandle);
	ListView_SetImageList(ListCtrl.GetSafeHwnd(), dwImageListHandle, LVSIL_SMALL);

	// Get large image list
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	dwImageListHandle = SHGetFileInfo(szPath,
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_LARGEICON
		);

	ASSERT(dwImageListHandle);
	ListView_SetImageList(ListCtrl.GetSafeHwnd(), dwImageListHandle, LVSIL_NORMAL);

	// Get executable images
	GetExeImages();

	UpdateListCtrlColumns();
}


// NAV 2006
// Reset columns based on view
void CQconsoleView::UpdateListCtrlColumns()
{
	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Delete all columns and re-add
	// DON'T start enumerating columns from 0 and go to max columns. When you
	// delete a column, the columns with indices that are higher than it are
	// decremented by one.
	int iColCount = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for(int iCol = 0; iCol < iColCount; iCol++)
		ListCtrl.DeleteColumn(0);

	// Get column widths
	int* aColumnWidths = new int[GetColumnCount()];
	if(NULL == aColumnWidths)
    {
        CCTRACEE(_T("%s - Failed to allocate aColumnWidths buffer"), __FUNCTION__);
		ASSERT(NULL != aColumnWidths);
    }
	GetColumnWidths(aColumnWidths, GetColumnCount());

	// Add list control columns/column headers
	CString sColumnHeader;
	sColumnHeader.LoadString(IDS_THREAT_NAME);
	ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_THREAT_NAME), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_THREAT_NAME)]);

	sColumnHeader.LoadString(IDS_QUARANTINE_DATE);
	ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_QDATE), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_QDATE)]);

	sColumnHeader.LoadString(IDS_CATEGORY_NAME);
	ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_CATEGORY), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_CATEGORY)]);

	//if(VIEW_MODE_EXPANDED_THREATS == m_iLastViewMode)
	//{
		sColumnHeader.LoadString(IDS_RISK_LEVEL);
		ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_RISK_LEVEL), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_RISK_LEVEL)]);
	//}
	//else // VIEW_MODE_VIRAL_THREATS
	//{
	//	sColumnHeader.LoadString(IDS_ORIG_LOCATION);
	//	ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_LOCATION), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_LOCATION)]);
	//}

	if(VIEW_MODE_EXPANDED_THREATS == m_iLastViewMode)
	{
		sColumnHeader.LoadString(IDS_DEPENDENCIES);
		ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_DEPENDENCIES), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_DEPENDENCIES)]);
	}
	//else // VIEW_MODE_VIRAL_THREATS
	//{
	//	sColumnHeader.LoadString(IDS_FILE_NAME);
	//	ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_FILE_NAME), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_FILE_NAME)]);
	//}

	sColumnHeader.LoadString(IDS_SUBMITTED_DATE);
	ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_SDATE), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_SDATE)]);

	sColumnHeader.LoadString(IDS_STATUS);
	ListCtrl.InsertColumn(pDoc->MapColumnIDToIndex(COLUMN_STATUS), sColumnHeader, LVCFMT_LEFT, aColumnWidths[pDoc->MapColumnIDToIndex(COLUMN_STATUS)]);

	delete[] aColumnWidths;

	// On non-windows XP machines, we need to owner draw the header controls.
	if(FALSE == GetDocument()->IsWinXP())
	{
		// Need to set up owner draw column sort stuff here.
		CHeaderCtrl* pHdr = (CHeaderCtrl*)ListCtrl.GetWindow(GW_CHILD);

		// Make each item owner draw
		HD_ITEM item;
		for(int iCol = 0; iCol < iColCount; iCol++)
		{
			item.mask = HDI_FORMAT;
			item.fmt = HDF_OWNERDRAW;
			pHdr->SetItem(iCol, &item);
		}
	}
}


int GetSystemIconIndexFromExtension(LPCSTR szExtension, int iIconSize = SHGFI_SMALLICON)
{
	// Build dummy file name
	CString cszTempFileName;
	cszTempFileName.Format(_T("qcontest.%s"), szExtension);

	return QItemEZ::GetSystemIconIndex(cszTempFileName, iIconSize);
}


////////////////////////////////////////////////////////////////////////////
// Make sure the system image list has the proper image for an executable.
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetExeImages()
{
	// Get large icon
	m_iLargeExeImage = GetSystemIconIndexFromExtension(_T("COM"), SHGFI_LARGEICON);

	// Get small icon
	m_iSmallExeImage = GetSystemIconIndexFromExtension(_T("COM"), SHGFI_SMALLICON);
}


////////////////////////////////////////////////////////////////////////////////
// Handles UpdateAllViews
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch(lHint)
	{
	case LISTVIEW_REFRESH:
		HandleRefresh();
		break;

	case SET_VIEW_FILTER:
		HandleFilterChange();
		break;

	case LISTVIEW_SORTITEMS:
		// Old sort column is sent in pHint
		if((int) pHint == GetDocument()->GetSortCol())
			m_bSortAscending = !m_bSortAscending;
		else
			m_bSortAscending = TRUE;

		HandleSortChange();
		break;

	case LISTVIEW_NEW_DEFS:
		HandleNewDefsScan();
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Sets the correct list view style.
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::HandleStyleChange()
{
	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Turn off redraw for now.
	ListCtrl.SetRedraw(FALSE);

	// If we are in report view now, save column widths
	if((ListCtrl.GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		SaveColumnWidths();

	// Set correct style
	ListCtrl.ModifyStyle(LVS_REPORT | LVS_ICON | LVS_LIST | LVS_SMALLICON,
		GetDocument()->GetListViewStyle());

	ListCtrl.Arrange(LVA_DEFAULT);

	// Reset redraw
	ListCtrl.SetRedraw(TRUE);
}


////////////////////////////////////////////////////////////////////////////////
// Repopulates the list with items
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::HandleFilterChange()
{
	// Get pointer to document
	CQconsoleDoc* pDoc = GetDocument();

	// Get the current view mode and see if we are
	// already in this mode.  If so, bail out.
	int iCurrentMode = pDoc->GetViewMode();
	if(iCurrentMode == m_iLastViewMode)
		return;

	SaveColumnWidths();

	// Save off current mode.
	m_iLastViewMode = iCurrentMode;

	// Set the Info Text
	CFrameWnd* pFrameWnd = (CFrameWnd*)AfxGetMainWnd();
	pFrameWnd->SetMessageText(AFX_IDS_IDLEMESSAGE);

	UpdateListCtrlColumns();

	// Repopulate list
	PopulateListCtrl();

	HMENU hMenu = ::GetMenu(pFrameWnd->m_hWnd);
	if(VIEW_MODE_EXPANDED_THREATS == m_iLastViewMode)
	{
		// Enable expanded threat columns and disable viral columns in "Sort Items" menu
		::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYDEPENDENCIES, MF_BYCOMMAND | MF_ENABLED);
		//::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYRISK_LEVEL, MF_BYCOMMAND | MF_ENABLED);

		//::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYNAME, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		//::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYLOCATION, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
	}
	else // VIEW_MODE_VIRAL_THREATS
	{
		// Enable viral columns and disable expanded threat columns in "Sort Items" menu
		//::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYNAME, MF_BYCOMMAND | MF_ENABLED);
		//::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYLOCATION, MF_BYCOMMAND | MF_ENABLED);

		::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYDEPENDENCIES, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		//::EnableMenuItem(hMenu, ID_VIEW_ARRANGEICONS_BYRISK_LEVEL, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
	}

	::DrawMenuBar(pFrameWnd->m_hWnd);
}


////////////////////////////////////////////////////////////////////////////////
// Handle right mouse click
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Load context menu for view
	CMenu menu;
	if(GetDocument()->GetExpandedThreatShowSetting())
	{
		if(FALSE == menu.LoadMenu(IDR_VIEW_CONTEXT))
			return;
	}
	else
	{
		if(FALSE == menu.LoadMenu(IDR_VIEW_CONTEXT_NO_EXPANDED_THREAT))
			return;
	}

	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Get mouse position
	CPoint pt;
	GetCursorPos(&pt);

	// Get client coordinates for hit test.
	CPoint ptClient(pt);
	ScreenToClient(&ptClient);

	// If the user did not click on an item, remove the properties item.
	int iPopup = VIEW_POPUP_ONITEM;
	if(ListCtrl.HitTest(ptClient) == -1)
		iPopup = VIEW_POPUP;

	// Load correct popup menu
	CMenu* pPopup = menu.GetSubMenu(iPopup);
	ASSERT(pPopup);

	if(VIEW_MODE_EXPANDED_THREATS == m_iLastViewMode)
	{
		// Enable expanded threat columns and disable viral columns in "Sort Items" menu
		pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYDEPENDENCIES, MF_BYCOMMAND | MF_ENABLED);
		//pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYRISK_LEVEL, MF_BYCOMMAND | MF_ENABLED);

		//pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYNAME, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		//pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYLOCATION, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
	}
	else // VIEW_MODE_VIRAL_THREATS
	{
		// Enable viral columns and disable expanded threat columns in "Sort Items" menu
		//pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYNAME, MF_BYCOMMAND | MF_ENABLED);
		//pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYLOCATION, MF_BYCOMMAND | MF_ENABLED);

		pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYDEPENDENCIES, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
		//pPopup->EnableMenuItem(ID_VIEW_ARRANGEICONS_BYRISK_LEVEL, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
	}

	DrawMenuBar();

	// If we are on an item, set the Details item to BOLD
	if(iPopup == VIEW_POPUP_ONITEM)
	{
		MENUITEMINFO mif;
		ZeroMemory(&mif, sizeof(MENUITEMINFO));
		mif.cbSize = sizeof(MENUITEMINFO);
		mif.fMask = MIIM_STATE;
		GetMenuItemInfo(pPopup->m_hMenu, 0, TRUE, &mif);

		// Set this item to the default state
		mif.fMask = MIIM_STATE;
		mif.fState |= MFS_DEFAULT;
		SetMenuItemInfo(pPopup->m_hMenu, 0, TRUE, &mif);
	}

	// Fire off context menu
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x,
		pt.y,
		AfxGetMainWnd());
}


////////////////////////////////////////////////////////////////////////////////
// Enables a menu item if something is selected in the list
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnUpdateSelectedItem(CCmdUI* pCmdUI)
{
	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Only enable if an item is selected.
	if(ListCtrl.GetSelectedCount() > 0)
	{
		CQconsoleDoc* pDoc = GetDocument();
		ASSERT(NULL != pDoc);

		// Trial version only allows restore operations
		if((FALSE == pDoc->IsQConsoleEnabled()) && (pCmdUI->m_nID != ID_ACTION_RESTORE))
		{
			pCmdUI->Enable(FALSE);
			return;
		}

		// Make sure this operation is availiable to items in this mode
		//int iIndex;
		//int iViewMode = pDoc->GetViewMode();
		//switch(iViewMode)
		//{
		//	case VIEW_MODE_EXPANDED_THREATS:
		//		iIndex = QFILEOPS_INDEX_THREAT_BACKUP_FILE;
		//		break;
		//	case VIEW_MODE_VIRAL_THREATS:
		//		iIndex = QFILEOPS_INDEX_QUARANTINED;
		//		break;
		//}

		// Get file option flags
		//DWORD dwFileOps = 0;
		//pDoc->m_pOpts->GetValidFileOperations(&dwFileOps, iIndex);

		EnableActionsForSelected(pCmdUI);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		((CFrameWnd*)AfxGetMainWnd())->SetMessageText(AFX_IDS_IDLEMESSAGE);
	}
}


////////////////////////////////////////////////////////////////////////////
// This ugly function decides if a given control should be enabled based on 
//	options for each action.
// 5/7/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::EnableActionsForSelected(CCmdUI* pCmdUI)
{
	// Get reference to list control
	//CListCtrl& ListCtrl = GetListCtrl();

	// Get Document
	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	// How many elements are selected?
	//int iCount = ListCtrl.GetSelectedCount();    

	// Get file option flags
	DWORD dwQuarFileOps;
	pDoc->m_pOpts->GetValidFileOperations(&dwQuarFileOps, QFILEOPS_INDEX_QUARANTINED);

	DWORD dwBackupFileOps;
	pDoc->m_pOpts->GetValidFileOperations(&dwBackupFileOps, QFILEOPS_INDEX_BACKUP_FILE);

	DWORD dwSubFileOps;
	pDoc->m_pOpts->GetValidFileOperations(&dwSubFileOps, QFILEOPS_INDEX_SUBMITTED);

	DWORD dwThreatBackupFileOps;
	pDoc->m_pOpts->GetValidFileOperations(&dwThreatBackupFileOps, QFILEOPS_INDEX_THREAT_BACKUP_FILE);

	bool bQuarItems = AnyOfStatusSelected(QFILE_STATUS_QUARANTINED);
	bool bBackupItems = AnyOfStatusSelected(QFILE_STATUS_BACKUP_FILE);
	bool bThreatBackupItems = AnyOfStatusSelected(QFILE_STATUS_THREAT_BACKUP);
	bool bSubmittedItems = AnyOfStatusSelected(QFILE_STATUS_SUBMITTED);

	// Classify each item.  
	//int iItem = -1;
	//int iQuar = 0;
	//int iSubmitted = 0;
	//int iBackup = 0;
	//int iThreatBackup = 0;

	//while((iItem = ListCtrl.GetNextItem(iItem, LVNI_ALL | LVNI_SELECTED)) != -1)
	//{
	//	// Get item from list box extra data
	//	IQuarantineItem* pQuarItem = (IQuarantineItem*) ListCtrl.GetItemData(iItem);
	//	ASSERT(pQuarItem);

	//	// Get status for this item
	//	DWORD dwStatus;
	//	if(FAILED(pQuarItem->GetFileStatus(&dwStatus)))
	//		continue;

	//	// Keep track of how many of each item type we have.
	//	switch(dwStatus)
	//	{
	//	case QFILE_STATUS_QUARANTINED: iQuar++; break;
	//	case QFILE_STATUS_SUBMITTED:   
	//		// If a non-viral item is in Submitted, treat it like Threat Backup
	//		QItemEZ::THREAT_NON_VIRAL == QItemEZ::IsViral(pQuarItem) ? iThreatBackup++ : iSubmitted++;
	//		break;
	//	case QFILE_STATUS_BACKUP_FILE: iBackup ++; break;
	//	case QFILE_STATUS_THREAT_BACKUP: iThreatBackup ++; break;
	//	}

	//	// Optimization.  If we already have one of each item, bail from loop.
	//	if(iQuar && iSubmitted && iBackup && iThreatBackup)
	//		break;
	//}

	// Enable control based on user options.  This is ugly, but has to be
	// done.
	BOOL bEnable = TRUE;
	switch(pCmdUI->m_nID)
	{
	case ID_ACTION_REPAIR:
		if(bQuarItems)
			bEnable = ((dwQuarFileOps & QUAR_OPTS_FILEOP_REPAIR) > 0);
		if(bSubmittedItems)
			bEnable &= ((dwSubFileOps & QUAR_OPTS_FILEOP_REPAIR) > 0);
		if(bBackupItems)            
			bEnable &= ((dwBackupFileOps & QUAR_OPTS_FILEOP_REPAIR) > 0);
		if(bThreatBackupItems)            
			bEnable &= ((dwThreatBackupFileOps & QUAR_OPTS_FILEOP_REPAIR) > 0);
		break;

	case ID_ACTION_RESTORE:
		if(bQuarItems)
			bEnable = ((dwQuarFileOps & QUAR_OPTS_FILEOP_PUT_BACK) > 0);
		if(bSubmittedItems)
			bEnable &= ((dwSubFileOps & QUAR_OPTS_FILEOP_PUT_BACK) > 0);
		if(bBackupItems)            
			bEnable &= ((dwBackupFileOps & QUAR_OPTS_FILEOP_PUT_BACK) > 0);
		if(bThreatBackupItems)            
			bEnable &= ((dwThreatBackupFileOps & QUAR_OPTS_FILEOP_PUT_BACK) > 0);
		break;

	case ID_ACTION_DELETEITEM:
		if(bQuarItems)
			bEnable = ((dwQuarFileOps & QUAR_OPTS_FILEOP_DELETE) > 0);
		if(bSubmittedItems)
			bEnable &= ((dwSubFileOps & QUAR_OPTS_FILEOP_DELETE) > 0);
		if(bBackupItems)            
			bEnable &= ((dwBackupFileOps & QUAR_OPTS_FILEOP_DELETE) > 0);
		if(bThreatBackupItems)            
			bEnable &= ((dwThreatBackupFileOps & QUAR_OPTS_FILEOP_DELETE) > 0);
		break;

	case ID_ACTION_SUBMITTOSARC:
		if(bQuarItems)
			bEnable = ((dwQuarFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC) > 0);
		if(bSubmittedItems)
			bEnable &= ((dwSubFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC) > 0);
		if(bBackupItems)            
			bEnable &= ((dwBackupFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC) > 0);
		if(bThreatBackupItems)            
			bEnable &= ((dwThreatBackupFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC) > 0);
		break;
	}

	pCmdUI->Enable(bEnable);   
}


// Added for NAV 2006
// Dropping view based file action options.
// Most options will be based on setting in default options, 
// but submit needs to be blocked if items have been submitted already.
//void CQconsoleView::EnableActionsPerViewMode(CCmdUI* pCmdUI, DWORD dwFileOps)
//{
//	// Get reference to list control
//	CListCtrl& ListCtrl = GetListCtrl();
//
//	BOOL bEnable = TRUE;
//	switch(pCmdUI->m_nID)
//	{
//	case ID_ACTION_REPAIR:
//		bEnable = ((dwFileOps & QUAR_OPTS_FILEOP_REPAIR) > 0);
//		bEnable = bEnable && !AnyOfStatusSelected(QFILE_STATUS_BACKUP_FILE);
//		break;
//	case ID_ACTION_RESTORE:
//		bEnable = ((dwFileOps & QUAR_OPTS_FILEOP_PUT_BACK) > 0);
//		break;
//	case ID_ACTION_DELETEITEM:
//		bEnable = ((dwFileOps & QUAR_OPTS_FILEOP_DELETE) > 0);
//		break;
//	case ID_ACTION_SUBMITTOSARC:
//		bEnable = ((dwFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC) > 0);
//		bEnable = bEnable && !AnyOfStatusSelected(QFILE_STATUS_SUBMITTED);
//		break;
//	}
//
//	pCmdUI->Enable(bEnable);   
//}


////////////////////////////////////////////////////////////////////////////////
// Handles CMDUI for Properties action.  Only allow if a single item is selected.
// 3/5/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnUpdateProperties(CCmdUI* pCmdUI)
{
	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Only enable if a single item is selected.
	if(ListCtrl.GetSelectedCount() == 1)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}


////////////////////////////////////////////////////////////////////////////////
// Handles Action/Repair menu item
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionRepair()
{
	// Get list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Get count.
	int iItemCount = ListCtrl.GetSelectedCount();

	// Build warning message.
	CString sMessage;
	if(iItemCount > 1)
		sMessage.Format(IDS_MULTIPLE_REPAIR_WARNING, iItemCount);
	else
		sMessage.LoadString(IDS_REPAIR_WARNING);

	// Warn user about what he is about to do.
	if(MessageBox(sMessage, m_sAppName, MB_ICONQUESTION | MB_YESNO) != IDYES)
		return;

	// Perform repair operation
	DoRepair(true);

	// Recompute item totals
	GetDocument()->RecalcItemTypes();

	// Force a refresh of the tree pane.
	GetDocument()->UpdateAllViews(this, LISTVIEW_DELETE_ITEMS, NULL);
}


////////////////////////////////////////////////////////////////////////////
// Handles Action/Delete menu item
// 3/6/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionDeleteItem()
{
	// Get list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Get count.
	int iItemCount = ListCtrl.GetSelectedCount();

	// Build warning message.
	CString sMessage;
	if(iItemCount > 1)
		sMessage.LoadString(IDS_MULTIPLE_DELETE_WARNING);
	else
		sMessage.LoadString(IDS_DELETE_WARNING);

	// Warn user about what he is about to do.
	if(MessageBox(sMessage, m_sAppName, MB_ICONEXCLAMATION | MB_YESNO) != IDYES)
		return;

	// Could take some time
	CWaitCursor wait;
	CQconsoleDoc* pDoc = GetDocument();
	IQuarantineItem* pItem;
	int iItem;

	// Disable drawing for now
	ListCtrl.SetRedraw(FALSE);

	// Delete all selected items
	while((iItem = ListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED)) != -1)
	{
		// Get object from user data field.
		pItem = (IQuarantineItem*) ListCtrl.GetItemData(iItem);
		ASSERT(pItem);

		// Delete the item in question.
		pItem->DeleteItem();

		// Delete the item from the list
		ListCtrl.DeleteItem(iItem);

		// Delete the item from the array
		pDoc->DeleteItemFromLists(pItem);
	}

	// Turn redraw back on
	ListCtrl.SetRedraw(TRUE);

	// Tell other views what happened.
	pDoc->UpdateAllViews(this, LISTVIEW_DELETE_ITEMS, NULL);
}


////////////////////////////////////////////////////////////////////////////////
// Handles Action/Properties menu item
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionProperties()
{
	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Get index of selected item.
	int iIndex = ListCtrl.GetNextItem(-1, LVNI_SELECTED);
	if(iIndex == -1)
		return;

	// Get item.
	LV_ITEM item;
	TCHAR szBuffer[MAX_PATH];
	ZeroMemory(&item, sizeof(LV_ITEM));
	item.iItem = iIndex;
	item.pszText = szBuffer;
	item.cchTextMax = MAX_PATH;
	item.mask = LVIF_IMAGE | LVIF_TEXT;
	if(ListCtrl.GetItem(&item) == FALSE)
		return;

	// Set up property sheet.
	CString s;
	s.Format(IDS_PROP_TITLE_FORMAT, item.pszText);
	CItemPropSheet dlg(s);
	dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Fire off dialog.
	dlg.DisplayProperties((IQuarantineItem*) ListCtrl.GetItemData(iIndex),
		ListCtrl.GetImageList(LVSIL_NORMAL),
		item.iImage);
}


////////////////////////////////////////////////////////////////////////////////
// Handle Action/Restore menu item
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionRestore()
{
	// Get list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Get count.
	int iItemCount = ListCtrl.GetSelectedCount();

	bool bKnownExpandedThreats = AnyOfThreatSelected(false);

	CRestoreDlg dlgRestore(bKnownExpandedThreats, iItemCount, this);
	dlgRestore.SetDocument(GetDocument());
	if(IDYES != dlgRestore.DoModal())
		return;

	if(dlgRestore.ShouldExcludeSelected())
	{
		// Handle excluding all selected items that are known expanded threats
		ExcludeSelected();
	}

	// Perform restore operation
	DoRepair(false);

	// Tell other views what happened.
	GetDocument()->UpdateAllViews(this, LISTVIEW_DELETE_ITEMS, NULL);
}


////////////////////////////////////////////////////////////////////////////////
// Performs repair and restore operations
// Argument: BOOL bRepair - true if repair is to be done, false if only 
//				restore is required
// 3/12/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::DoRepair(bool bRepair)
{
	// Get list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Create progress dialog
	CProgressDlg progress;
	progress.Create();
	progress.SetRange(0, ListCtrl.GetSelectedCount());

	// Get document
	CQconsoleDoc* pDoc = GetDocument();

	int iItem = -1;
	int iCounter = 0;
	LV_ITEM listItem;
	IQuarantineItem* pItem;
	CResultsListItem* pResultItem;

	// Setup status dialog.
	CResultsDlg dlg;
	dlg.SetImageList(GetListCtrl().GetImageList(LVSIL_SMALL));
	if(!bRepair)
		dlg.SetCaption(IDS_RESTORE_RESULTS);

	// Restore all selected items
	while((iItem = ListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED)) != -1)
	{
		// Setup new results item.
		pResultItem = new CResultsListItem;

		// Get data for this object
		ZeroMemory(&listItem, sizeof(LV_ITEM));
		listItem.iItem = iItem;
		listItem.mask = LVIF_IMAGE | LVIF_PARAM;

		// Get item data
		ListCtrl.GetItem(&listItem);

		// de-select this item.
		ListCtrl.SetItemState(iItem, ~LVIS_SELECTED, LVIS_SELECTED);

		// Get object from user data field.
		pItem = (IQuarantineItem*) listItem.lParam;
		ASSERT(pItem);

		// Save off item info
		pDoc->GetThreatName(pItem, pResultItem->m_sItemName);
		pResultItem->m_iListItem = iItem;

		// Save off icon index
		pResultItem->m_Action.SetIconIndex(listItem.iImage);

		// Add to list
		dlg.m_aItemList.AddTail(pResultItem);

		// Attempt repair / restore operation.
		int iResult = bRepair ? RepairItem(pItem, pResultItem) :
								RestoreItem(pItem, pResultItem);

		if((iResult == -1) || progress.CheckCancelButton())
			return;

		// If Quar32 repaired this item for us and did not delete it, 
		// it changed the status from Quarantine to Backup

		// Advance progress dialog
		progress.SetPos(++iCounter);
	}

	// Remove progress window
	progress.DestroyWindow();

	// Fire off results dialog.
	dlg.DoModal();
}


////////////////////////////////////////////////////////////////////////////
// Attempt to restore an item from quarantine.
// Return type: int 0 to continue, -1 to cancel
// 3/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CQconsoleView::RestoreItem(IQuarantineItem* pItem, CResultsListItem* pResult)
{
	// Psuedocode:
	//   Restore Main File, prompt for new location if necessary
	//   Restore all Remediation Actions, prompt for new location on file remediations
	//   return

	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	CString cszThreatName;
	pDoc->GetThreatName(pItem, cszThreatName);

	// Init results item (OPTIMISTIC)
	pResult->m_Action.SetAction(RESULTS_ACTION_RESTORED, cszThreatName);

	// Restore main file (RemediationIndex = 0)
	bool bMainFileIsDummy = false;
	HRESULT hr;
	if(SUCCEEDED(pItem->InitialDummyFile(&bMainFileIsDummy)) && bMainFileIsDummy)
	{
		CCTRACEI(_T("%s - Initial file is the dummy implementation. Nothing to do for the main file restore."), __FUNCTION__);
	}
	else
	{
		hr = RestoreFileFromItem(pItem, pResult, 0);
	}

	// Restore RemediationActions
	hr = RestoreRemediationActions(pItem, pResult);

	// Log even if there are failed operations, since part of the item may have been restored.
	LogRestore(pItem, pResult);

	if(pResult->m_dwFailedOperations)
	{
		pResult->m_Action.SetAction(RESULTS_ACTION_ERROR, cszThreatName);
	}
	else
	{
		// Since we've successfully restored the item
		//  remove it from the lists, and delete it from Quarantine

		// Get list control
		CListCtrl& ListCtrl = GetListCtrl();

		// Remove item in question from list
		ListCtrl.DeleteItem(pResult->m_iListItem);

		// Delete the item in question.
		pItem->DeleteItem();

		// Delete the item from the array
		GetDocument()->DeleteItemFromLists(pItem);
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////
// Attempt to restore remediation actions for item from quarantine.
// Returns: S_OK if function complets
//          E_FAIL upon unexpected failure
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::RestoreRemediationActions(IQuarantineItem* pItem, CResultsListItem* pResult, BOOL bSkipInfectionRemediations)
{
	HRESULT hr, hrReturn = S_OK;
	DWORD dwRemediationCount = NULL;

	hr = pItem->GetRemediationCount(&dwRemediationCount);
	if(FAILED(hr))
		return hr;

	if(0 == dwRemediationCount)
		return S_OK;

	DWORD dwRemediationIndex = dwRemediationCount;
	for(dwRemediationIndex = dwRemediationCount; dwRemediationIndex > 0; dwRemediationIndex--)
	{
		CString cszProperty;

		DWORD dwType;
		cszProperty.Format(QSERVER_ITEM_REMEDIATION_ACTION_TYPE, dwRemediationIndex);
		hr = pItem->GetDWORD((LPSTR)(LPCSTR)cszProperty, &dwType);
		if(FAILED(hr))
			break;

		if(ccEraser::InfectionRemediationActionType == dwType || 
			ccEraser::FileRemediationActionType      == dwType)
		{
			// May not want to restore infected remediations
			if(bSkipInfectionRemediations 
				&& (ccEraser::InfectionRemediationActionType == dwType))
			{
				continue;
			}

			hr = RestoreFileFromItem(pItem, pResult, dwRemediationIndex);
		}
		else
		{
			hr = RestoreNonFileBasedRemediation(pItem, pResult, dwRemediationIndex);
		}
	}

	return hrReturn;
}


////////////////////////////////////////////////////////////////////////////
// Attempt to restore a Remediation from an item in quarantine.
// Argument: DWORD dwIndex  (dwIndex == 0 denotes "Main File", 1-n denotes Remediation
// Returns : S_OK if restore was successful
//           E_FAIL upon unexpected failure, restore failed
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::RestoreNonFileBasedRemediation(IQuarantineItem* pItem, CResultsListItem* pResult, DWORD dwIndex)
{
	HRESULT hrReturn = E_FAIL;

	HRESULT hr;
	ccEraser::eResult eRestoreResult;

	CString cszRemediationDescription;
	if(!QItemEZ::GetRemediationDescription(pItem, cszRemediationDescription, dwIndex))
		return E_FAIL;

	hr = pItem->RestoreRemediation(dwIndex, &eRestoreResult);
	if(SUCCEEDED(hr))
	{
		// convert ccEraser return to usable HRESULT
		hr = ccEraser::Succeeded(eRestoreResult) ? S_OK : E_FAIL;
	}
	else if(E_UNAVAILABLE_REMEDIATION_UNDO == hr)
	{
		// Original remediation was unsuccessful, or undo file not available.
		// Skip.
		return S_FALSE;
	}

	CRepairAction *pRepairAction = new CRepairAction;
	pResult->m_aSubActionList.AddTail(pRepairAction);
	pRepairAction->SetIconIndex(QItemEZ::GetSystemIconIndex(pItem, SHGFI_SMALLICON, dwIndex));

	if(SUCCEEDED(hr))
	{
		pResult->m_dwSuccessfulOperations++;
		pRepairAction->SetAction(RESULTS_ACTION_RESTORED, cszRemediationDescription);
	}
	else
	{
		pResult->m_dwFailedOperations++;
		pRepairAction->SetAction(RESULTS_ACTION_ERROR, cszRemediationDescription);
	}

	hrReturn = hr;

	return hrReturn;
}


////////////////////////////////////////////////////////////////////////////
// Attempt to restore an item from quarantine.
// Argument: DWORD dwIndex  (dwIndex == 0 denotes "Main File", 1-n denotes Remediation
// Returns : S_OK if restore was successful
//           E_FAIL upon unexpected failure, restore failed
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::RestoreFileFromItem(IQuarantineItem* pItem, CResultsListItem* pResult, DWORD dwIndex)
{
	HRESULT hrReturn = E_FAIL;

	if(dwIndex > 0)
	{
		bool bRemediationSucceeded = false;
		bool bUndoAvailable = false;
		HRESULT hResRemediation = pItem->GetRemediationActionSucceeded(dwIndex, &bRemediationSucceeded);
		HRESULT hResUndoAvail = pItem->GetRemediationUndoAvailable(dwIndex, &bUndoAvailable);

		if(FAILED(hResRemediation) || FAILED(hResUndoAvail) || !bRemediationSucceeded || !bUndoAvailable)
			return S_FALSE;
	}

	// Get full original file name
	CString sOriginalFileName;
	GetDocument()->GetFullANSIFilePath(pItem, sOriginalFileName, dwIndex);

	// Get location
	CString sFilePath;
	GetDocument()->GetANSIFilePathOnly(pItem, sFilePath, dwIndex);

	// Get filename
	CString sFileName;
	GetDocument()->GetANSIFileNameOnly(pItem, sFileName, dwIndex);

	CString cszRemediationDescription = sOriginalFileName;

	// create a repair action for reporting
	CRepairAction *pRepairAction = new CRepairAction;
	pResult->m_aSubActionList.AddTail(pRepairAction);
	pRepairAction->SetIconIndex(QItemEZ::GetSystemIconIndex(pItem, SHGFI_SMALLICON, dwIndex));

	// Get save location.
	if(GetSaveLocation(sFilePath, sOriginalFileName, sFileName) != FALSE)
	{
		// Perform restore
		CString sUnpackedFile;
		HRESULT hr;
		// If 0==dwIndex, then do "Main File", otherwise, handle remediation
		if(0 == dwIndex)
		{
			// Restore "Main File"
			hr = pItem->UnpackageMainFile((LPSTR)(LPCSTR)sFilePath, (LPSTR)(LPCSTR)sFileName, NULL, 0, TRUE, FALSE);

			// Restore ccGSER side effects, if any
			pItem->RestoreCCGserSideEffects((LPSTR)(LPCSTR)sOriginalFileName);

			// Set description
			cszRemediationDescription = sOriginalFileName;

		}
		else
		{
			if(!QItemEZ::GetRemediationDescription(pItem, cszRemediationDescription, dwIndex))
				return E_FAIL;

			// Restore Remediation
			ccEraser::eResult eRestoreResult;
			hr = pItem->RestoreFileRemediationToLocation(dwIndex, (LPSTR)(LPCSTR)sFilePath, (LPSTR)(LPCSTR)sFileName, &eRestoreResult);
			if(SUCCEEDED(hr))
			{
				// convert ccEraser return to usable HRESULT
				hr = ccEraser::Succeeded(eRestoreResult) ? S_OK : E_FAIL;
			}
		}

		if(SUCCEEDED(hr))
		{
			pResult->m_dwSuccessfulOperations++;
			pRepairAction->SetAction(RESULTS_ACTION_RESTORED, cszRemediationDescription);
		}
		else
		{
			pResult->m_dwFailedOperations++;
			pRepairAction->SetAction(RESULTS_ACTION_ERROR, cszRemediationDescription);
		}

		hrReturn = hr;
	}
	else
	{
		pResult->m_dwFailedOperations++;
		pRepairAction->SetAction(RESULTS_ACTION_CANCELLED, cszRemediationDescription);
		hrReturn = E_FAIL;
	}

	return hrReturn;
}


////////////////////////////////////////////////////////////////////////////
// Attempt to repair and restore all infected files in a Quarantine item.
// Returns: S_OK if all files successfully repaired
//          E_FAIL if any file fails to repair
// Notes: If any infected file fails to repair successfully, 
//                    function will cleanup and terminate.
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::RepairInfectedFiles(IQuarantineItem* pItem, CResultsListItem* pResult)
{
	HRESULT hr = S_OK;

	// Repair infected files
	MAP_ITEM_TO_LOCATION mapRepairLocations;
	CString cszRepairLocation;
	BOOL bAllFilesRepaired = TRUE;
	CString cszOriginalFileName;

	// Repair "Main File" unless it's a dummy file implementation
	bool bMainFileIsDummy = false;
	if(SUCCEEDED(pItem->InitialDummyFile(&bMainFileIsDummy)) && bMainFileIsDummy)
	{
		CCTRACEI(_T("CQconsoleView::RepairInfectedFiles() - Initial file is the dummy implementation. Nothing to do for the main file restore."));
		hr = S_OK;
	}
	else
		hr = RepairInfectedFile(pItem, pResult, cszRepairLocation, 0);

	if(SUCCEEDED(hr) && S_NO_VIRUS_FOUND != hr && S_UNDELETABLE_VIRUS_FOUND != hr)
	{
		// Add "Main File" to list of repaired files as long as it is not a dummy file
		if(!bMainFileIsDummy)
		mapRepairLocations[0] = cszRepairLocation;

		// Continue repairing
		DWORD dwRemediationCount = NULL;
		hr = pItem->GetRemediationCount(&dwRemediationCount);
		if(SUCCEEDED(hr) && dwRemediationCount)
		{
			DWORD dwIndex = 1;
			for(dwIndex = 1; dwIndex <= dwRemediationCount; dwIndex++)
			{
				DWORD dwRemediationType = NULL;
				BOOL bRet = QItemEZ::GetRemediationType(pItem, dwRemediationType, dwIndex);
				if(bRet && ccEraser::InfectionRemediationActionType == dwRemediationType)
				{
					hr = RepairInfectedFile(pItem, pResult, cszRepairLocation, dwIndex);

					if(SUCCEEDED(hr) && (S_FALSE != hr))
						mapRepairLocations[dwIndex] = cszRepairLocation;
					else if(FAILED(hr))
						bAllFilesRepaired = FALSE;
				}
			}
		}
	}
	else
	{
		bAllFilesRepaired = FALSE;
	}

	// All files were not repaired, perform cleanup
	if(!bAllFilesRepaired)
	{
		MAP_ITEM_TO_LOCATION::iterator itLocations;
		for(itLocations = mapRepairLocations.begin(); itLocations != mapRepairLocations.end(); itLocations++)
		{
			// Delete any files that might have been repaired
			::DeleteFile((*itLocations).second);
		}

		return E_FAIL;
	}

	// We're looking good, all infected files were repaired
	MAP_ITEM_TO_LOCATION::iterator itLocations;
	for(itLocations = mapRepairLocations.begin(); itLocations != mapRepairLocations.end(); itLocations++)
	{
		// Store off index of subitem and repaired file location
		DWORD dwIndex = (*itLocations).first;
		cszRepairLocation = (*itLocations).second;

		if(0 == cszRepairLocation.GetLength())
		{
			// This "fix" for this file was probably to delete it
			continue;
		}

		// Get original full file path for item
		CString cszOriginalFileName;
		QItemEZ::GetFullANSIFilePath(pItem, cszOriginalFileName, dwIndex);

		CRepairAction *pRepairAction = new CRepairAction;
		pResult->m_aSubActionList.AddTail(pRepairAction);
		pRepairAction->SetIconIndex(QItemEZ::GetSystemIconIndex(pItem, SHGFI_SMALLICON, dwIndex));

		// Split path
		CString cszFilePath, cszFileName;
		NameReturnPath(cszOriginalFileName, cszFilePath.GetBuffer(MAX_PATH));
		cszFilePath.ReleaseBuffer();

		// Get save location.
		if(GetSaveLocation(cszFilePath, cszOriginalFileName, cszFileName) != FALSE)
		{
			// Restore this item.
			if(::CopyFile(cszRepairLocation, cszOriginalFileName, FALSE))
			{
				// Get security descriptor for repaired file
				LPBYTE  securityDesc = NULL;
				if(SUCCEEDED(GetFileSecurityDesc((LPSTR)(LPCSTR)cszRepairLocation, &securityDesc))) 
				{
					// Set new file security.  Ignore result.
					SetFileSecurityDesc((LPSTR)(LPCSTR)cszOriginalFileName, securityDesc);

					// Cleanup
					if(securityDesc)
						delete[] securityDesc;

					// if "Main File", deal with ccGSER side effects
					if(0 == dwIndex)
					{
						// Restore ccGSER side effects, if any
						pItem->RestoreCCGserSideEffects((LPSTR)(LPCSTR)cszOriginalFileName);
					}
				}

				pResult->m_dwSuccessfulOperations++;
				pRepairAction->SetAction(RESULTS_ACTION_RESTORED, cszOriginalFileName);
			}
			else
			{
				// Display error message to user
				auto    LPVOID      lpMsgBuf;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
					, NULL
					, GetLastError()
					, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
					, (LPTSTR) &lpMsgBuf
					, 0
					, NULL);
				MessageBox((CHAR *)lpMsgBuf, m_sAppName, MB_OK| MB_ICONSTOP);
				LocalFree(lpMsgBuf);

				ASSERT(FALSE);
				pResult->m_dwFailedOperations++;
				pRepairAction->SetAction(RESULTS_ACTION_ERROR, cszOriginalFileName);
			}
		}
		else
		{
			pRepairAction->SetAction(RESULTS_ACTION_NO_ACTION, cszOriginalFileName);
		}

		// Delete temp file
		::DeleteFile(cszRepairLocation);
	}

	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// Attempt to repair an infected file from an item in quarantine.
// Returns: S_OK if file is successfully repaired (filename may be
//               zero-length if file was deleted as part of repair.
//          E_FAIL if file could not be successfully repaired.
// 12/1/2004 APIERCE - Function created
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::RepairInfectedFile(IQuarantineItem* pItem, CResultsListItem* pResult, CString& cszRepairedFile, DWORD dwIndex)
{
	if(dwIndex > 0)
	{
		bool bRemediationSucceeded = false;
		bool bUndoAvailable = false;
		HRESULT hResRemediation = pItem->GetRemediationActionSucceeded(dwIndex, &bRemediationSucceeded);
		HRESULT hResUndoAvail = pItem->GetRemediationUndoAvailable(dwIndex, &bUndoAvailable);

		if(FAILED(hResRemediation) || FAILED(hResUndoAvail) || !bRemediationSucceeded || !bUndoAvailable)
			return S_FALSE;
	}

	// Get full original file name
	CString cszOriginalFileName;
	QItemEZ::GetFullANSIFilePath(pItem, cszOriginalFileName, dwIndex);

	// create a repair action for reporting
	CRepairAction *pRepairAction = new CRepairAction;
	pResult->m_aSubActionList.AddTail(pRepairAction);
	pRepairAction->SetIconIndex(QItemEZ::GetSystemIconIndex(pItem, SHGFI_SMALLICON, dwIndex));

	HRESULT hr;
	if(0 == dwIndex)
	{
		hr = pItem->RepairAndRestoreMainFileToTemp(cszRepairedFile.GetBuffer(MAX_PATH),
			MAX_PATH);
		cszRepairedFile.ReleaseBuffer();
	}
	else
	{
		CString cszDestFolder, cszDestFilename;
		DWORD dwDestFolderSize = MAX_PATH, dwDestFilenameSize = MAX_PATH;
		hr = pItem->RepairAndRestoreFileRemediationToTemp(dwIndex, 
			cszDestFolder.GetBuffer(dwDestFolderSize), 
			&dwDestFolderSize, 
			cszDestFilename.GetBuffer(dwDestFilenameSize), 
			&dwDestFilenameSize);
		cszDestFolder.ReleaseBuffer();
		cszDestFilename.ReleaseBuffer();
		if(SUCCEEDED(hr))
		{
			DWORD dwSize = cszDestFolder.GetLength() + cszDestFilename.GetLength() + 2;
			PathCombine(cszRepairedFile.GetBuffer(dwSize), cszDestFolder, cszDestFilename);
			cszRepairedFile.ReleaseBuffer();
		}
	}

	if(SUCCEEDED(hr) && S_NO_VIRUS_FOUND != hr && S_UNDELETABLE_VIRUS_FOUND != hr)
	{
		// Make sure the file was not deleted on the repair
		// This will happen if the destroy flag is set for this virus
		if(GetFileAttributes(cszRepairedFile) == INVALID_FILE_ATTRIBUTES)
		{
			cszRepairedFile.Empty();
			pRepairAction->SetAction(RESULTS_ACTION_DELETED, cszOriginalFileName);
		}
		else
		{
			pRepairAction->SetAction(RESULTS_ACTION_REPAIRED, cszOriginalFileName);
		}

		return S_OK;
	}

	pResult->m_dwFailedOperations++;
	pRepairAction->SetAction(RESULTS_ACTION_STILL_INFECTED, cszOriginalFileName);

	//
	// Handle repair failures
	CString cszError;
	switch(hr)
	{
	case E_FILE_CREATE_FAILED:
		cszError.Format(IDS_FILE_IN_USE, cszOriginalFileName); 
		break;

	case E_FILE_WRITE_FAILED:
		{
			TCHAR szFolder[MAX_PATH];
			GetDocument()->m_pQuarantine->GetQuarantineFolder(szFolder, MAX_PATH);
			cszError.Format(IDS_ERROR_DISK_SPACE, szFolder[0]);
		}
		break;

	default:
		break;
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Attempt to repair and restore an item from quarantine.
// Return type: int 0 to continue, -1 to cancel
// 3/11/98 DBUCHES - Function created / Header added.
// 6/26/98 JTAYLOR - Restore the original file status if the repair fails.
// 6/26/98 JTAYLOR - Added a delete for the temporary file if repair fails.
////////////////////////////////////////////////////////////////////////////
int CQconsoleView::RepairItem(IQuarantineItem* pItem, CResultsListItem* pResult)
{
	// Pseudocode:
	//  Repair all infected files
	//  Restore all repaired files
	//  Restore all non-infected file Remediations

	CString cszThreatName;
	GetDocument()->GetThreatName(pItem, cszThreatName);

	// Init results item (OPTIMISTIC)
	pResult->m_Action.SetAction(RESULTS_ACTION_REPAIRED, cszThreatName);

	// Repair and restore infected files
	HRESULT hr = RepairInfectedFiles(pItem, pResult);
	if(FAILED(hr))
	{
		pResult->m_Action.SetAction(RESULTS_ACTION_STILL_INFECTED, cszThreatName);
		return 0; // continue 
	}
	else
	{
		// Set the item to a backup item since the infected items have been repaired
		pItem->SetFileStatus(QFILE_STATUS_BACKUP_FILE);
		pItem->SaveItem();
	}

	// Restore the rest of the remediations
	hr = RestoreRemediationActions(pItem, pResult, TRUE);

	if(pResult->m_dwFailedOperations)
		pResult->m_Action.SetAction(RESULTS_ACTION_ERROR, cszThreatName);

	return 0; // continue
}


////////////////////////////////////////////////////////////////////////////
// Argument         : IQuarantineItem* pItem - item in question
// Argument         : CString& sDestFileName - reference to final locaiton of file
// 5/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
//BOOL CQconsoleView::CreateItemUserDirectory(IQuarantineItem* pItem, CString& sFilePath, CString& sDestFileName, CString& sFileName)
//{
//    // Get user name.
//    CString sUserName;
//	DWORD dwBufferSize = MAX_PATH;
//    pItem->GetOriginalOwnerName(sUserName.GetBuffer(MAX_PATH), &dwBufferSize);
//    sUserName.ReleaseBuffer();
//
// Get filename
//    LPTSTR p = sFileName.GetBuffer(MAX_PATH);
//    NameReturnFile(sDestFileName, p);
//    sFileName.ReleaseBuffer();
//    
//    // Build save path.
//    sFilePath = GetDocument()->GetRepairFolder();
//    sFilePath += _T("\\");
//    sFilePath += sUserName;
//
//    sDestFileName = sFilePath;
//    sDestFileName += _T("\\");
//    sDestFileName += sFileName;
//
//    // Attempt to create directory.
//    DWORD dwAttr = GetFileAttributes(sFilePath);
//    if(dwAttr == 0xFFFFFFFF || ! (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
//        {
//        if(::CreateDirectory(sFilePath, NULL) == FALSE)
//            {
//            // If we can't create the directory, prompt user for save location
//            return GetSaveLocation(GetDocument()->GetRepairFolder(), sDestFileName, sFileName);
//            }
//        }
//
//    // Try saving here.
//    return GetSaveLocation(sFilePath, sDestFileName, sFileName);
//}

////////////////////////////////////////////////////////////////////////////
// Prompts user for save location
// 5/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleView::GetSaveLocation(CString& sFilePath, 
									CString& sOriginalFileName, 
									CString& sFileName)
{
	// Get filename
	LPTSTR p = NULL;

	// Make sure original directory actually exists.  Allways prompt for destination
	// folder if in corporate mode
	DWORD dwAttr = GetFileAttributes(sFilePath);

	if(sOriginalFileName.GetLength() > MAX_PATH || dwAttr == 0xFFFFFFFF || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		// Ask user where file should be saved

		// Add the temp file path to it
		CString sTempPath;
		::GetTempPath(MAX_PATH, sTempPath.GetBuffer(MAX_PATH));
		sTempPath.ReleaseBuffer();
		sTempPath += sFileName;
		CString sFilter((LPCTSTR) IDS_ADD_FILTER_STRING);
		CFileDialog dlg(FALSE,                 // Save
			NULL,                  // No default extension
			sTempPath,             // Initial file name
			OFN_FILEMUSTEXIST |    // Flags
			OFN_HIDEREADONLY |
			OFN_PATHMUSTEXIST,
			sFilter,               // Filter string
			this);                // Parent window

		// Fire off dialog
		if(dlg.DoModal() == IDCANCEL)
			return FALSE;


		// Save off file name and path
		sFileName = dlg.GetFileName();
		sOriginalFileName = dlg.GetPathName();
		p = sFilePath.GetBuffer(MAX_PATH);
		NameReturnPath(sOriginalFileName, p);
		sFilePath.ReleaseBuffer();
	}

	// If the file already exists, ask the user if he wants to
	// overwrite the file.
	dwAttr = GetFileAttributes(sOriginalFileName);

	while(dwAttr != 0xFFFFFFFF)
	{
		// Prompt the user for overwrite
		CString sTitle, sText;
		sTitle.LoadString(IDS_RESTORE_OVERWRITE_TITLE);
		sText.Format(IDS_RESTORE_OVERWRITE_FORMAT, sOriginalFileName);
		if(MessageBox(sText, sTitle, MB_ICONWARNING | MB_YESNO) != IDYES)
		{
			CString sFilter((LPCTSTR) IDS_ADD_FILTER_STRING);
			CFileDialog dlg(FALSE,                 // Save
				NULL,                  // No default extension
				sOriginalFileName,     // Initial file name
				OFN_FILEMUSTEXIST |    // Flags
				OFN_HIDEREADONLY |
				OFN_PATHMUSTEXIST,
				sFilter,               // Filter string
				this);                // Parent window

			// Fire off dialog
			if(dlg.DoModal() == IDCANCEL)
				return FALSE;


			// Save off file name and path
			sFileName = dlg.GetFileName();
			sOriginalFileName = dlg.GetPathName();
			p = sFilePath.GetBuffer(MAX_PATH);
			NameReturnPath(sOriginalFileName, p);
			sFilePath.ReleaseBuffer();
		}
		else
		{
			// User wants to overwrite this file, so delete it first.
			::DeleteFile(sOriginalFileName);
			break;
		}

		// Get file attributes of this file
		dwAttr = GetFileAttributes(sOriginalFileName);
	}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// Handles Refresh hint
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::HandleRefresh()
{
	// Simply repopulate the list, forcing a rebuild of the main list.
	PopulateListCtrl();
}


////////////////////////////////////////////////////////////////////////////////
// Populates the list control
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::PopulateListCtrl(BOOL bRebuild /* = FALSE */)
{
	// This could take a while.
	CWaitCursor wait;

	// Get list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Get pointer to document
	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	// Turn off redraw and delete all items from list
	ListCtrl.SetRedraw(FALSE);
	ListCtrl.DeleteAllItems();

	// Rebuild all item arrays
	if(bRebuild)
		pDoc->RebuildItemArrays();

	// Get currently selected array.
	CMapPtrToPtr *pList;
	pList = pDoc->GetItemList();

	// Enum the list and add our items.
	POSITION pos;
	LPVOID p1, p2;
	for(pos = pList->GetStartPosition(); pos != NULL;)
	{
		// Get item to add to list
		pList->GetNextAssoc(pos, p1, p2);
		ASSERT(p2);

		// Set up item to insert.
		LV_ITEM item;
		ZeroMemory(&item, sizeof(LV_ITEM));
		item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.iImage = I_IMAGECALLBACK;
		item.lParam = (LPARAM) p2;

		// Insert this record into the list
		ListCtrl.InsertItem(&item);
	}

	HandleSortChange();

	// Select first item in the list
	//ListCtrl.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

	// Turn drawing back on and force a repaint
	ListCtrl.SetRedraw(TRUE);
	ListCtrl.UpdateWindow();
}


////////////////////////////////////////////////////////////////////////////////
// Handles left button double click
// 2/16/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// Call base class.
	CListView::OnLButtonDblClk(nFlags, point);

	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// If the user did not click on an item, bail out
	if(ListCtrl.HitTest(point) == -1)
		return;

	// Call properties handler
	OnActionProperties();
}


////////////////////////////////////////////////////////////////////////////////
// Returns TRUE if this file is an executable type (.EXE or .COM)
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleView::IsExecutable(CString& sFileName)
{
	CString s(sFileName);
	s.MakeUpper();

	if(s.Right(4) == _T(".EXE") ||
		s.Right(4) == _T(".COM"))
		return TRUE;

	return FALSE;
}


int CQconsoleView::OkToScanAndDeliver()
{
	int iRet = NO_STATUS;

	// First, check to see if virus defintions are less than 2
	// weeks old.  If they are out of date, we want the user to 
	// get new defs (via LiveUpdate or whatever) first.  

	// Get the date of the defs we are using.
	WORD wYear = 0, wMonth = 0, wDay = 0;
	DWORD dwRev;

	DefUtilsLoader DefUtilsLoader;

	{ // Scope the object
		IDefUtilsPtr pIDefUtils;
		if(SYM_SUCCEEDED(DefUtilsLoader.CreateObject(pIDefUtils.m_p)))
		{
			if(pIDefUtils->InitWindowsApp(_T(QUARANTINE_APP_ID)))
			{
				// Try to use the newest defs.  If this fails, it's probably because
				// there is a new set of defs on the machine that has not been integrated
				// yet, but the current process doesn't have rights to the hawkings tree.
				// In this case, just use the current defs.
				char szDefsDir[MAX_PATH];
				if(pIDefUtils->UseNewestDefs(szDefsDir, MAX_PATH) == FALSE)
				{
					pIDefUtils->GetCurrentDefs(szDefsDir, MAX_PATH);
				}

				pIDefUtils->GetCurrentDefsDate(&wYear, &wMonth, &wDay, &dwRev);
				CTime defTime(wYear, wMonth, wDay, 0, 0, 0);

				// Get the current date.
				CTime currentTime = CTime::GetCurrentTime();
				CTimeSpan span = currentTime - defTime;

				// Check for stale defs.
				if(span.GetDays() > 14)
				{
					pIDefUtils.Release();
					CCTRACEI(_T("%s - Old defs"), __FUNCTION__);
					return DEFS_ARE_OLD;
				}
			}
		}
	} // End scope

	// We need to skip certain functionality in safe mode
	bool bIsSafeMode = false;
	if(0 != GetSystemMetrics(SM_CLEANBOOT))
	{
		bIsSafeMode = true;
		CCTRACEI(_T("%s - Running in safe mode."), __FUNCTION__);
	}

	// Next, check for expired virus definitions subscription.
	// License checking done higher in tree, so this should only be called if we already passed a trial/rental valid check.
	// So now just check if this is rental, if it is, there is no subscription to expire, so we're good to proceed.

	if(bIsSafeMode)
	{
		return SAFE_MODE;
	}
	else
	{
#ifndef _DEBUG
		STAHLSOFT_HRX_TRY(hr)
		{
			// Getting licensing and subscription properties needed to enable product features
			CPEPClientBase pepBase;
			hrx << pepBase.Init();
			pepBase.SetContextGuid(clt::pep::CLTGUID);
			pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
			pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
			pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);

			hrx << pepBase.QueryPolicy();

			DJSMAR00_LicenseState licState = DJSMAR00_LicenseState_Violated;
			hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD&)licState, DJSMAR00_LicenseState_Violated);
			CCTRACEI(_T("%s - License state=%d"), __FUNCTION__, licState);

			switch(licState)
			{
			case DJSMAR00_LicenseState_Violated:
			case DJSMAR00_LicenseState_EXPIRED:
				break;

			case DJSMAR00_LicenseState_TRIAL:
				{
					iRet = ALL_GOOD;
					CCTRACEI(_T("%s - Trial license state, don't check subscription, return ALL_GOOD"), __FUNCTION__);
					break;
				}
			case DJSMAR00_LicenseState_PURCHASED:
				{
					DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;
					hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD&)licType, DJSMAR_LicenseType_Violated);
					CCTRACEI(_T("%s - License type=%d"), __FUNCTION__, licType);

					if((DJSMAR_LicenseType_Rental == licType) || (DJSMAR_LicenseType_TryDie == licType))
					{
						iRet = ALL_GOOD;
						CCTRACEI(_T("%s - Purchased license state, Rental or TryDie type, don't check subscription, return ALL_GOOD"), __FUNCTION__);
						break;
					}

					pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_RECORD);
					pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);
					hrx << pepBase.QueryPolicy();

					// Get the subscription disposition
					DWORD dwSubDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;
					hrx << pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, (DWORD&)dwSubDisposition, SUBSCRIPTION_STATIC::DISPOSITION_NONE);
					CCTRACEI(_T("%s - Subscription disposition=0x%08X"), __FUNCTION__, dwSubDisposition);

					if(DWORD_HAS_FLAG(dwSubDisposition, SUBSCRIPTION_STATIC::DISPOSITION_SUBSCRIPTION_VALID))
					{
						iRet = ALL_GOOD;
						CCTRACEI(_T("%s - Purchased license state, Subscription is good"), __FUNCTION__);
					}
					else
					{
						iRet = SUBSCRIPTION_EXPIRED;
						CCTRACEI(_T("%s - Purchased license state, Subscription is expired"), __FUNCTION__);
					}

					break;
				}
			}
		}
		STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

#else
		iRet = ALL_GOOD;
		CCTRACEI(_T("%s - Running in Debug. Skipped license/subscription check and returning ALL_GOOD"), __FUNCTION__);
#endif
	}

	return iRet;
}


////////////////////////////////////////////////////////////////////////////////
// Handles Action/Submit menu item
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionSubmitToSarc()
{
	// First, make sure we are allowed to Scan & Deliver.
	CString sText;
	CString sMessage;

	switch(OkToScanAndDeliver())
	{
	case ALL_GOOD:
		break;

	case DEFS_ARE_OLD:
		sMessage.LoadString(IDS_ERR_DEFS_OLD);
		MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return;

	case SUBSCRIPTION_EXPIRED:
		sMessage.LoadString(IDS_ERR_SUBSCRIPTION_EXPIRED);
		MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return;

	case SAFE_MODE:
		sMessage.LoadString(IDS_ERR_SAFEMODE_STARTING_SCANANDDELIVER);
		MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return;

	default:
		sMessage.Format(IDS_ERROR_STARTING_SCANANDDELIVER, m_sProductName);
		MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		return;
	}

	// Get pointer to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Allocate array of item pointers.
	IQuarantineItem ** pItems;
	pItems = new IQuarantineItem*[ListCtrl.GetSelectedCount()];

	int iItem = -1;
	int iCounter = 0;

	// Get all selected items
	while((iItem = ListCtrl.GetNextItem(iItem, LVNI_ALL | LVNI_SELECTED)) != -1)
	{
		// Get object from user data field.
		pItems[iCounter++] = (IQuarantineItem*) ListCtrl.GetItemData(iItem);
	}

	// Initialize the Scan and Deliver interface
	IScanDeliverDLL* pScanDeliver;
	HRESULT hr = CoCreateInstance(CLSID_ScanDeliverDLL,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ScanDeliverDLL,
		(LPVOID*)&pScanDeliver);

	if(FAILED(hr))
	{
		CCTRACEE(_T("%s - Failed to initialize Scan and Deliver interface. hr=0x%X"), __FUNCTION__, hr);

		CString sMessage;
		sMessage.Format(IDS_ERROR_STARTING_SCANANDDELIVER, m_sProductName);
		MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
	}
	else
	{
		// Deliver files to SARC
		hr = pScanDeliver->DeliverFilesToSarc(pItems, ListCtrl.GetSelectedCount());

		if(E_UNABLE_TO_INITIALIZE == hr)
		{
			CCTRACEE(_T("%s - Failed pScanDeliver->DeliverFilesToSarc() hr=0x%X"), __FUNCTION__, hr);

			CString sMessage;
			sMessage.Format(IDS_ERROR_STARTING_SCANANDDELIVER, m_sProductName);
			MessageBox(sMessage, m_sAppName, MB_ICONSTOP | MB_OK);
		}

		pScanDeliver->Release();
	}


	// Cleanup.
	delete [] pItems;

	// tell everyone what happened.
	GetDocument()->UpdateAllViews(NULL, LISTVIEW_REFRESH, NULL);
}


const int TRIANGLE_PADDING = 20;

////////////////////////////////////////////////////////////////////////////////
// Handles WM_DRAWITEM messages.
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(lpDrawItemStruct->CtlType != ODT_HEADER)
	{
		CListView::OnDrawItem(nIDCtl, lpDrawItemStruct);
		return;
	}

	// Get item text and width.
	HD_ITEM item;
	TCHAR szBuffer[100];
	item.mask = HDI_TEXT | HDI_WIDTH | HDI_LPARAM;
	item.pszText = szBuffer;
	item.cchTextMax = 100;
	CRect WindowRect;
	CHeaderCtrl *pHdr = (CHeaderCtrl*) CWnd::FromHandle(lpDrawItemStruct->hwndItem);
	pHdr->GetClientRect(&WindowRect);
	pHdr->GetItem(lpDrawItemStruct->itemID, &item);

	// Get rectangle for this item
	CRect rect(lpDrawItemStruct->rcItem);

	// Set up DC
	CDCOS dc(CDC::FromHandle(lpDrawItemStruct->hDC), WindowRect, rect);

	// Fill rect.
	CBrush brush(GetSysColor(COLOR_BTNFACE));
	dc.FillRect(&rect, &brush);

	// Set up DC
	CFont *pOldFont = dc.SelectObject(pHdr->GetFont());
	COLORREF oldBkColor = dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
	COLORREF oldTextColor = dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));

	// Compute size of text
	CString s(szBuffer);
	CSize size = dc.GetOutputTextExtent(s);

	// Compute text rectangle
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	CRect textRect;
	CRect sizeRect;
	int iOffset = (lpDrawItemStruct->itemState & ODS_SELECTED) ? 1 : 0;

	textRect.right = rect.right - TRIANGLE_PADDING;
	textRect.left = rect.left + tm.tmAveCharWidth + iOffset;
	textRect.top = rect.top + (rect.Height() - size.cy) / 2 + iOffset;
	textRect.bottom = rect.bottom;
	dc.DrawText(szBuffer, &sizeRect, DT_LEFT | DT_CALCRECT);

	// Draw text to screen.
	dc.DrawText(szBuffer, &textRect, DT_LEFT | DT_END_ELLIPSIS);

	// Get top of triangle.
	int iTop = (rect.Height() - 7) / 2 + iOffset;
	int iLeft;

	// Compute correct X-coordinate for triangle
	if(sizeRect.Width()  + tm.tmAveCharWidth + iOffset< textRect.Width())
		iLeft = rect.left + sizeRect.Width() + 15 + iOffset;
	else
		iLeft = rect.right - 15 + iOffset;

	// Draw search direction triangle.  Only draw if we have room.
	if(GetDocument()->GetSortCol() == (int)lpDrawItemStruct->itemID &&
		iLeft >= textRect.left)
	{
		DrawArrow(&dc, iLeft, iTop, m_bSortAscending);
	}

	// Cleanup
	dc.SelectObject(pOldFont);
	dc.SetTextColor(oldTextColor);
	dc.SetBkColor(oldBkColor);
}


////////////////////////////////////////////////////////////////////////////
// 12/1/97 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::DrawArrow(CDC* pDC, int x, int y, BOOL bUp)
{
	// Create pens needed
	CPen whitePen;
	whitePen.CreateStockObject(WHITE_PEN);
	CPen grayPen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));

	int iTop = y;
	int iBottom = iTop + 6;

	CPen * oldPen = pDC->SelectObject(&grayPen);
	if(bUp)
	{
		// Draw bottom line
		pDC->SelectObject(&whitePen);
		pDC->MoveTo(x, iBottom);
		pDC->LineTo(x + 7, iBottom);

		// Draw left diagonal
		pDC->SelectObject(&grayPen);
		pDC->MoveTo(x + 3, iTop);
		pDC->LineTo(x , iBottom);

		// Draw right diagonal
		pDC->MoveTo(x + 4, iTop);
		pDC->SelectObject(&whitePen);
		pDC->LineTo(x + 8, iBottom +1);
	}
	else
	{
		// Draws downward pointing triangle

		// Draw top line
		pDC->MoveTo(x, iTop);
		pDC->LineTo(x + 7, iTop);

		// Draw left diagonal
		pDC->MoveTo(x + 3, iBottom);
		pDC->LineTo(x , iTop);

		// Draw right diagonal
		pDC->MoveTo(x + 4, iBottom);
		pDC->SelectObject(&whitePen);
		pDC->LineTo(x + 8, iTop -1);
	}

	// Cleanup
	pDC->SelectObject(oldPen);
}


////////////////////////////////////////////////////////////////////////////////
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Get pointer to list control
	CListCtrl& ListCtrl = GetListCtrl();
	CWaitCursor wait;

	// If we've recently clicked this guy, reverse sort order.
	if(pNMListView->iSubItem == GetDocument()->GetSortCol())
	{
		m_bSortAscending = !m_bSortAscending;
	}
	else
	{
		// Save off last clicked column
		GetDocument()->SetSortCol(pNMListView->iSubItem);
		m_bSortAscending = TRUE;
	}

	HandleSortChange();

	*pResult = 0;
}


////////////////////////////////////////////////////////////////////////////
// Saves off column widths
// 1/19/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::SaveColumnWidths()
{
	// Get list control
	CListCtrl& ListCtrl = GetListCtrl();

	// If we are in report mode, save off list view columns
	if((ListCtrl.GetStyle() & LVS_TYPEMASK) != LVS_REPORT)
		return;

	// Build string
	CString sWidths;
	int iColCount = ListCtrl.GetHeaderCtrl()->GetItemCount();
	for(int iCol = 0; iCol < iColCount; iCol++)
	{
		CString sTemp;
		sTemp.Format("%d;", ListCtrl.GetColumnWidth(iCol));
		sWidths += sTemp;
	}

	if(VIEW_MODE_EXPANDED_THREATS == m_iLastViewMode)
	{
		// Save to registry
		AfxGetApp()->WriteProfileString(REGKEY_UI_SETTINGS,
			REGKEY_VALUE_COLUMNS_EXPANDED,
			sWidths);
	}
	else // VIEW_MODE_VIRAL_THREATS
	{
		AfxGetApp()->WriteProfileString(REGKEY_UI_SETTINGS,
			REGKEY_VALUE_COLUMNS_VIRAL,
			sWidths);
	}
}


////////////////////////////////////////////////////////////////////////////
// Retrieve the column widths from the registry
// 1/19/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetColumnWidths(int *pColumns, int nCount)
{
	CString sWidths;

	if(VIEW_MODE_EXPANDED_THREATS == m_iLastViewMode)
	{
		// Get string from registry.
		sWidths = AfxGetApp()->GetProfileString(REGKEY_UI_SETTINGS,
			REGKEY_VALUE_COLUMNS_EXPANDED,
			DEFAULT_COLUMN_WIDTHS);
	}
	else // VIEW_MODE_VIRAL_THREATS
	{
		sWidths = AfxGetApp()->GetProfileString(REGKEY_UI_SETTINGS,
			REGKEY_VALUE_COLUMNS_VIRAL,
			DEFAULT_COLUMN_WIDTHS);
	}

	// Get the correct value from the string.
	CString s;
	for(int i = 0; i < nCount; i++)
	{
		// Search for ;
		int iPos = sWidths.Find(_T(';'));
		if(iPos == -1)
			break;

		// Number is left of ;
		s = sWidths.Left(iPos);
		pColumns[i] = atol(s);

		// Move to next chunk 'o string
		sWidths = sWidths.Right(sWidths.GetLength() - iPos -1);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Handles WM_DESTROY message.
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnDestroy()
{
	// Save off listview column widths.
	SaveColumnWidths();

	// Call base class.
	CListView::OnDestroy();
}


////////////////////////////////////////////////////////////////////////////////
// Handles WM_SIZE message
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnSize(UINT nType, int cx, int cy)
{
	// Call base class
	CListView::OnSize(nType, cx, cy);

	// Get list control
	CListCtrl& ListCtrl = GetListCtrl();

	// Make sure items stay arranged.
	ListCtrl.Arrange(LVA_DEFAULT);
}


////////////////////////////////////////////////////////////////////////////
// Handles LVN_GETDISPINFO notification message
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	// Get the text for this item
	if(pDispInfo->item.mask & LVIF_TEXT)
		GetItemText(&pDispInfo->item);

	// Get the image for this item.
	if(pDispInfo->item.mask & LVIF_IMAGE)
		GetItemImage(&pDispInfo->item);

	*pResult = 0;
}


////////////////////////////////////////////////////////////////////////////
// This routine returns the correct image for a given item
// Argument: LV_ITEM * pItem - the iImage member of this struct
//                              recieves the image index.
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetItemImage(LV_ITEM * pItem)
{
	CString s;
	IQuarantineItem* p;
	CString sRealFileName;

	// Get pointer to document
	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	// Figure out which style icon we need.
	UINT uIconFlag = (GetListCtrl().GetStyle() & LVS_ICON) ? SHGFI_LARGEICON : SHGFI_SMALLICON;


	// Get this item
	p = (IQuarantineItem*) pItem->lParam;
	ASSERT(p);

	GetDocument()->GetFullANSIFilePath(p, sRealFileName);

	// If this is an executable file, just return the icon index
	// we know and love for generic executables.
	if(IsExecutable(sRealFileName))
	{
		pItem->iImage = (uIconFlag == SHGFI_LARGEICON) ? m_iLargeExeImage : m_iSmallExeImage;
		return;
	}

	// Get icon for this file
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	SHGetFileInfo(sRealFileName,
		0,
		&sfi,
		sizeof(SHFILEINFO),
		SHGFI_SYSICONINDEX | uIconFlag | SHGFI_USEFILEATTRIBUTES
		);

	// Save off icon index.
	pItem->iImage = sfi.iIcon;
}


////////////////////////////////////////////////////////////////////////////
// This routine returns the text for a given sub-item.
// Argument: LV_ITEM * pItem - the pszText and cchTextMax fields are used as 
//				the location and size of the text.
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetItemText(LV_ITEM * pItem)
{
	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	// Get this item
	IQuarantineItem* pQuarItem;
	pQuarItem = (IQuarantineItem*) pItem->lParam;
	ASSERT(pQuarItem);
	CString sRealFileName;

	int iColumnID = pDoc->MapColumnIndexToID(pItem->iSubItem);
	switch(iColumnID)
	{
	//case COLUMN_FILE_NAME:
	//	// Get the filename and add it to our path
	//	pDoc->GetANSIFileNameOnly(pQuarItem, sRealFileName);
	//	_tcsncpy(pItem->pszText, sRealFileName, (pItem->cchTextMax - 1));
	//	break;

	//case COLUMN_LOCATION:
	//	pDoc->GetANSIFilePathOnly(pQuarItem, sRealFileName);
	//	// If the path is too big use some clever elipses action at the end
	//	if(_tcslen(sRealFileName) > pItem->cchTextMax)
	//	{
	//		sRealFileName = sRealFileName.Left(pItem->cchTextMax - 3);

	//		// Cut it off at the previous backslash so it kinda shows the original path
	//		int nLastSlash = sRealFileName.ReverseFind(_TCHAR('\\'));
	//		sRealFileName = sRealFileName.Left(nLastSlash+1);
	//		sRealFileName += _T("...");
	//	}
	//	_tcsncpy(pItem->pszText, sRealFileName, pItem->cchTextMax-1);
	//	break;

	case COLUMN_QDATE:
		{
			TCHAR szTime[30];
			SYSTEMTIME tm;
			pQuarItem->GetDateQuarantined(&tm);

			// Convert to local time
			FILETIME ft, lft;
			SystemTimeToFileTime(&tm, &ft);
			FileTimeToLocalFileTime(&ft, &lft);
			FileTimeToSystemTime(&lft, &tm);

			GetDateFormat(NULL,
				DATE_SHORTDATE,//LONGDATE,
				&tm,
				NULL,
				pItem->pszText,
				pItem->cchTextMax);

			GetTimeFormat(NULL,
				0,
				&tm,
				NULL,
				szTime,
				30);

			lstrcat(pItem->pszText, _T(" "));
			lstrcat(pItem->pszText, szTime);
		}
		break;

	case COLUMN_DEPENDENCIES:
		{
			DWORD dwHasDependencies = 0;
			CString sDependencies;

			pQuarItem->GetAnomalyDependencyFlag(&dwHasDependencies);
			if(0 == dwHasDependencies)
				sDependencies.LoadString(IDS_DEPENDENCIES_NO);
			else
				sDependencies.LoadString(IDS_DEPENDENCIES_YES);

			_tcsncpy(pItem->pszText, sDependencies, pItem->cchTextMax-1);
		}
		break;

	case COLUMN_RISK_LEVEL:
		{
			DWORD dwRiskLevel = pDoc->GetRiskLevelAverage(pQuarItem);

			CString sRiskLevel;
			bool bGotText = pDoc->GetRiskLevelText(dwRiskLevel, sRiskLevel);
			_tcsncpy(pItem->pszText, sRiskLevel, pItem->cchTextMax-1);
		}
		break;

	case COLUMN_SDATE:
		{
			TCHAR szTime[30];
			SYSTEMTIME tm;
			pQuarItem->GetDateSubmittedToSARC(&tm);

			// Check to make sure this was actually submitted.
			if(tm.wYear == 0)
			{
				CString s;
				s.LoadString(IDS_NOT_SUBMITTED);
				lstrcpyn(pItem->pszText, s, pItem->cchTextMax);
				break;
			}
			else
			{
				// Convert to local time
				FILETIME   ft, lft;
				SystemTimeToFileTime(&tm, &ft);
				FileTimeToLocalFileTime(&ft, &lft);
				FileTimeToSystemTime(&lft, &tm);
			}

			GetDateFormat(NULL,
				DATE_SHORTDATE,//LONGDATE,
				&tm,
				NULL,
				pItem->pszText,
				pItem->cchTextMax);

			GetTimeFormat(NULL,
				0,
				&tm,
				NULL,
				szTime,
				30);

			lstrcat(pItem->pszText, _T(" "));
			lstrcat(pItem->pszText, szTime);
		}
		break;

	case COLUMN_STATUS:
		{
			int iStrID;
			DWORD dwStatus;
			if(FAILED(pQuarItem->GetFileStatus(&dwStatus)))
				return;

			if(dwStatus & QFILE_STATUS_QUARANTINED)
				iStrID = IDS_STATUS_QUARANTINED;
			else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
				iStrID = IDS_STATUS_BACKUP;
			else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
				iStrID = IDS_STATUS_THREAT_BACKUP;
			//else if(dwStatus & QFILE_STATUS_SUBMITTED)
			//	iStrID = IDS_STATUS_SUBMITTED;

			// Load and copy string
			CString sStatus;
			sStatus.LoadString(iStrID);
			lstrcpyn(pItem->pszText, sStatus, pItem->cchTextMax);
		}
		break;

	case COLUMN_THREAT_NAME:
		{
			CString sThreatName;
			BOOL bRet = pDoc->GetThreatName(pQuarItem, sThreatName);

			// Get virus name from struct.
			if(bRet && (sThreatName.GetLength() > 0))
			{
				lstrcpyn(pItem->pszText, sThreatName, pItem->cchTextMax);
			}
			else
			{
				// If no Virus, display "Unknown"
				CString s, cszDefault, cszFileName;
				cszDefault.LoadString(IDS_UNKNOWN_THREAT_NAME);
				QItemEZ::GetANSIFileNameOnly(pQuarItem, cszFileName);

				s.FormatMessage(cszDefault, cszFileName);

				// Is this file from WarChild?
				DWORD dwFileType;
				pQuarItem ->GetFileType (&dwFileType);

				if(QFILE_TYPE_WARCHILD & dwFileType)
					s.LoadString (IDS_WARCHILD_SOURCE);

				if(QFILE_TYPE_OEH & dwFileType)
					s.LoadString (IDS_OEH_SOURCE);

				lstrcpyn(pItem->pszText, s, pItem->cchTextMax);
			}
		}
		break;

	case COLUMN_CATEGORY:
		{
			// Get categories from the item.
			CString cszThreatCategoryList;
			BOOL bRet = QItemEZ::GetThreatCategoryList(pQuarItem, cszThreatCategoryList);

			// If no Categories, display default categories string "Unknown"
			if(cszThreatCategoryList.IsEmpty())
				cszThreatCategoryList.LoadString(IDS_UNKNOWN_THREAT_CATEGORY);

			lstrcpyn(pItem->pszText, cszThreatCategoryList, pItem->cchTextMax);
		}
		break;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Sort ascending sort callback function
// 3/5/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
int CQconsoleView::SortAscendCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	IQuarantineItem* pQuarItem1 = (IQuarantineItem*)lParam1;
	IQuarantineItem* pQuarItem2 = (IQuarantineItem*)lParam2;
	CQconsoleView* pThis = (CQconsoleView*)lParamSort;
	CString sBuf1;
	CString sBuf2;
	SYSTEMTIME tm1, tm2;
	FILETIME ft1, ft2;

	int iColumnID = pThis->GetDocument()->MapColumnIndexToID(pThis->GetDocument()->GetSortCol());
	switch(iColumnID)
	{
	//case COLUMN_FILE_NAME:
	//	pThis->GetDocument()->GetANSIFileNameOnly(pQuarItem1, sBuf1);
	//	pThis->GetDocument()->GetANSIFileNameOnly(pQuarItem2, sBuf2);
	//	return lstrcmpi(sBuf1, sBuf2);

	//case COLUMN_LOCATION:
	//	pThis->GetDocument()->GetANSIFilePathOnly(pQuarItem1, sBuf1);
	//	pThis->GetDocument()->GetANSIFilePathOnly(pQuarItem2, sBuf2);
	//	return lstrcmpi(sBuf1, sBuf2);

	case COLUMN_QDATE:
		pQuarItem1->GetDateQuarantined(&tm1);
		SystemTimeToFileTime(&tm1, &ft1);
		pQuarItem2->GetDateQuarantined(&tm2);
		SystemTimeToFileTime(&tm2, &ft2);
		return CompareFileTime(&ft1, &ft2);

	case COLUMN_DEPENDENCIES:
		{
			DWORD dwHasDependencies1 = 0;
			DWORD dwHasDependencies2 = 0;

			if(FAILED(pQuarItem1->GetAnomalyDependencyFlag(&dwHasDependencies1)))
				dwHasDependencies1 = 0;

			if(FAILED(pQuarItem2->GetAnomalyDependencyFlag(&dwHasDependencies2)))
				dwHasDependencies2 = 0;

			if(dwHasDependencies1 > dwHasDependencies2)
				return -1;
			else if(dwHasDependencies1 < dwHasDependencies2)
				return 1;
			else
				return 0;
		}

	case COLUMN_RISK_LEVEL:
		{
			DWORD dwRiskLevel1 = pThis->GetDocument()->GetRiskLevelAverage(pQuarItem1);
			DWORD dwRiskLevel2 = pThis->GetDocument()->GetRiskLevelAverage(pQuarItem2);

			if(dwRiskLevel1 > dwRiskLevel2)
				return -1;
			else if(dwRiskLevel1 < dwRiskLevel2)
				return 1;
			else
				return 0;
		}

	case COLUMN_SDATE:
		{
			pQuarItem1->GetDateSubmittedToSARC(&tm1);
			BOOL bTimeConverted1 = SystemTimeToFileTime(&tm1, &ft1);

			pQuarItem2->GetDateSubmittedToSARC(&tm2);
			BOOL bTimeConverted2 = SystemTimeToFileTime(&tm2, &ft2);

			if(!bTimeConverted1 && !bTimeConverted2)
				return 0;
			else if(!bTimeConverted1)
				return -1;
			else if(!bTimeConverted2)
				return 1;
			else
				return CompareFileTime(&ft1, &ft2);
		}

	case COLUMN_STATUS:
		{
			int iStrID1, iStrID2;
			DWORD dwStatus1, dwStatus2;
			if(FAILED(pQuarItem1->GetFileStatus(&dwStatus1)) || 
				FAILED(pQuarItem2->GetFileStatus(&dwStatus2)))
			{
				return 0;
			}

			if(dwStatus1 & QFILE_STATUS_QUARANTINED)
				iStrID1 = IDS_STATUS_QUARANTINED;
			else if(dwStatus1 & QFILE_STATUS_BACKUP_FILE)
				iStrID1 = IDS_STATUS_BACKUP;
			else if(dwStatus1 & QFILE_STATUS_THREAT_BACKUP)
				iStrID1 = IDS_STATUS_THREAT_BACKUP;

			CString sStatus1;
			sStatus1.LoadString(iStrID1);

			if(dwStatus2 & QFILE_STATUS_QUARANTINED)
				iStrID2 = IDS_STATUS_QUARANTINED;
			else if(dwStatus2 & QFILE_STATUS_BACKUP_FILE)
				iStrID2 = IDS_STATUS_BACKUP;
			else if(dwStatus2 & QFILE_STATUS_THREAT_BACKUP)
				iStrID2 = IDS_STATUS_THREAT_BACKUP;

			CString sStatus2;
			sStatus2.LoadString(iStrID2);

			return lstrcmpi(sStatus1, sStatus2);
		}
		break;

	case COLUMN_THREAT_NAME:
		{
			CString sThreatName1;
			pThis->GetDocument()->GetThreatName(pQuarItem1, sThreatName1);

			CString sThreatName2;
			pThis->GetDocument()->GetThreatName(pQuarItem2, sThreatName2);

			int iRet = sThreatName1.CompareNoCase(sThreatName2);
			return iRet;
		}

	case COLUMN_CATEGORY:
		{
			// Get categories from the item.
			CString cszThreatCategoryList1;
			CString cszThreatCategoryList2;
			BOOL bRet = QItemEZ::GetThreatCategoryList(pQuarItem1, cszThreatCategoryList1);
			bRet = QItemEZ::GetThreatCategoryList(pQuarItem2, cszThreatCategoryList2);

			bool bUnknown1 = false;
			bool bUnknown2 = false;

			// If it's empty set default categories string
			if(cszThreatCategoryList1.IsEmpty())
			{
				bUnknown1 = true;
				cszThreatCategoryList1.LoadString(IDS_UNKNOWN_THREAT_CATEGORY);
			}

			if(cszThreatCategoryList2.IsEmpty())
			{
				bUnknown2 = true;
				cszThreatCategoryList2.LoadString(IDS_UNKNOWN_THREAT_CATEGORY);
			}

			if(bUnknown1 && bUnknown2)
				return 0;
			else if(bUnknown1)
				return 1;
			else if(bUnknown2)
				return -1;
			else
				return cszThreatCategoryList1.CompareNoCase(cszThreatCategoryList2);
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Sort descending sort callback function
// 3/5/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
int CQconsoleView::SortDescendCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	IQuarantineItem* pQuarItem1 = (IQuarantineItem*) lParam1;
	IQuarantineItem* pQuarItem2 = (IQuarantineItem*) lParam2;
	CQconsoleView* pThis = (CQconsoleView*) lParamSort;
	//TCHAR szBuffer1[MAX_PATH];
	//TCHAR szBuffer2[MAX_PATH];
	CString sBuf1;
	CString sBuf2;
	SYSTEMTIME tm1, tm2;
	FILETIME ft1, ft2;

	int iColumnID = pThis->GetDocument()->MapColumnIndexToID(pThis->GetDocument()->GetSortCol());
	switch(iColumnID)
	{
	//case COLUMN_FILE_NAME:
	//	pThis->GetDocument()->GetANSIFileNameOnly(pQuarItem1, sBuf1);
	//	pThis->GetDocument()->GetANSIFileNameOnly(pQuarItem2, sBuf2);
	//	return -lstrcmpi(sBuf1, sBuf2);

	//case COLUMN_LOCATION:
	//	pThis->GetDocument()->GetANSIFilePathOnly(pQuarItem1, sBuf1);
	//	pThis->GetDocument()->GetANSIFilePathOnly(pQuarItem2, sBuf2);
	//	return -lstrcmpi(sBuf1, sBuf2);

	case COLUMN_QDATE:
		pQuarItem1->GetDateQuarantined(&tm1);
		SystemTimeToFileTime(&tm1, &ft1);
		pQuarItem2->GetDateQuarantined(&tm2);
		SystemTimeToFileTime(&tm2, &ft2);
		return -CompareFileTime(&ft1, &ft2);

	case COLUMN_DEPENDENCIES:
		{
			DWORD dwHasDependencies1 = 0;
			DWORD dwHasDependencies2 = 0;

			if(FAILED(pQuarItem1->GetAnomalyDependencyFlag(&dwHasDependencies1)))
				dwHasDependencies1 = 0;

			if(FAILED(pQuarItem2->GetAnomalyDependencyFlag(&dwHasDependencies2)))
				dwHasDependencies2 = 0;

			if(dwHasDependencies1 > dwHasDependencies2)
				return 1;
			else if(dwHasDependencies1 < dwHasDependencies2)
				return -1;
			else
				return 0;
		}

	case COLUMN_RISK_LEVEL:
		{
			DWORD dwRiskLevel1 = pThis->GetDocument()->GetRiskLevelAverage(pQuarItem1);
			DWORD dwRiskLevel2 = pThis->GetDocument()->GetRiskLevelAverage(pQuarItem2);

			if(dwRiskLevel1 > dwRiskLevel2)
				return 1;
			else if(dwRiskLevel1 < dwRiskLevel2)
				return -1;
			else
				return 0;
		}

	case COLUMN_SDATE:
		{
			pQuarItem1->GetDateSubmittedToSARC(&tm1);
			BOOL bTimeConverted1 = SystemTimeToFileTime(&tm1, &ft1);

			pQuarItem2->GetDateSubmittedToSARC(&tm2);
			BOOL bTimeConverted2 = SystemTimeToFileTime(&tm2, &ft2);

			if(!bTimeConverted1 && !bTimeConverted2)
				return 0;
			else if(!bTimeConverted1)
				return 1;
			else if(!bTimeConverted2)
				return -1;
			else
				return -CompareFileTime(&ft1, &ft2);
		}

	case COLUMN_STATUS:
		{
			int iStrID1, iStrID2;
			DWORD dwStatus1, dwStatus2;
			if(FAILED(pQuarItem1->GetFileStatus(&dwStatus1)) || 
				FAILED(pQuarItem2->GetFileStatus(&dwStatus2)))
			{
				return 0;
			}

			if(dwStatus1 & QFILE_STATUS_QUARANTINED)
				iStrID1 = IDS_STATUS_QUARANTINED;
			else if(dwStatus1 & QFILE_STATUS_BACKUP_FILE)
				iStrID1 = IDS_STATUS_BACKUP;
			else if(dwStatus1 & QFILE_STATUS_THREAT_BACKUP)
				iStrID1 = IDS_STATUS_THREAT_BACKUP;

			CString sStatus1;
			sStatus1.LoadString(iStrID1);

			if(dwStatus2 & QFILE_STATUS_QUARANTINED)
				iStrID2 = IDS_STATUS_QUARANTINED;
			else if(dwStatus2 & QFILE_STATUS_BACKUP_FILE)
				iStrID2 = IDS_STATUS_BACKUP;
			else if(dwStatus2 & QFILE_STATUS_THREAT_BACKUP)
				iStrID2 = IDS_STATUS_THREAT_BACKUP;

			CString sStatus2;
			sStatus2.LoadString(iStrID2);

			return -lstrcmpi(sStatus1, sStatus2);
		}
		break;

	case COLUMN_THREAT_NAME:
		{
			CString sThreatName1;
			pThis->GetDocument()->GetThreatName(pQuarItem1, sThreatName1);

			CString sThreatName2;
			pThis->GetDocument()->GetThreatName(pQuarItem2, sThreatName2);

			int iRet = -(sThreatName1.CompareNoCase(sThreatName2));

			return iRet;
		}

	case COLUMN_CATEGORY:
		{
			// Get categories from the item.
			CString cszThreatCategoryList1;
			CString cszThreatCategoryList2;
			BOOL bRet = QItemEZ::GetThreatCategoryList(pQuarItem1, cszThreatCategoryList1);
			bRet = QItemEZ::GetThreatCategoryList(pQuarItem2, cszThreatCategoryList2);

			bool bUnknown1 = false;
			bool bUnknown2 = false;

			// If it's empty set default categories string
			if(cszThreatCategoryList1.IsEmpty())
			{
				bUnknown1 = true;
				cszThreatCategoryList1.LoadString(IDS_UNKNOWN_THREAT_CATEGORY);
			}

			if(cszThreatCategoryList2.IsEmpty())
			{
				bUnknown2 = true;
				cszThreatCategoryList2.LoadString(IDS_UNKNOWN_THREAT_CATEGORY);
			}

			if(bUnknown1 && bUnknown2)
				return 0;
			else if(bUnknown1)
				return -1;
			else if(bUnknown2)
				return 1;
			else
				return -(cszThreatCategoryList1.CompareNoCase(cszThreatCategoryList2));
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////
// Handles scan of all non-backup quarantine items
// 4/7/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::HandleNewDefsScan()
{
	int iCounter = 0;
	IQuarantineItem* pItem1;
	IQuarantineItem* pItem2;
	CResultsListItem* pResultItem;

	// Get document
	CQconsoleDoc* pDoc = GetDocument();

	// Get pointer to master list of items
	CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>    *pAllItems;
	CMapPtrToPtr *pI;
	pI = pDoc->GetItemList(VIEW_MODE_ALL);
	pAllItems = reinterpret_cast< CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>* > (pI);
	ASSERT(pAllItems);

	// Create progress dialog
	CProgressDlg progress;
	progress.Create();
	progress.SetRange(0, pAllItems->GetCount());

	// Setup status dialog.
	CResultsDlg dlg;
	dlg.SetImageList(GetListCtrl().GetImageList(LVSIL_SMALL));

	// Enum items in list
	POSITION pos;
	DWORD dwStatus;
	for(pos = pAllItems->GetStartPosition(); pos != NULL;)
	{
		pAllItems->GetNextAssoc(pos, pItem1, pItem2);

		// Get file status
		pItem1->GetFileStatus(&dwStatus);

		// Skip backup files
		if((dwStatus & QFILE_STATUS_THREAT_BACKUP) || (dwStatus & QFILE_STATUS_BACKUP_FILE))
			continue;

		// Setup new results item.
		pResultItem = new CResultsListItem;

		// Save off item name
		GetDocument()->GetThreatName(pItem1, pResultItem->m_sItemName);

		// Get the filename and add it to our path
		CString sRealFileName;
		pDoc->GetANSIFileNameOnly(pItem1, sRealFileName);


		if(IsExecutable(sRealFileName))
		{
			pResultItem->m_Action.SetIconIndex(m_iSmallExeImage);
		}
		else
		{
			SHFILEINFO sfi;
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo(sRealFileName,
				0,
				&sfi,
				sizeof(SHFILEINFO),
				SHGFI_SYSICONINDEX | SHGFI_SMALLICON	| SHGFI_USEFILEATTRIBUTES
				);

			// Save off icon index.
			pResultItem->m_Action.SetIconIndex(sfi.iIcon);
		}


		// Add to list
		dlg.m_aItemList.AddTail(pResultItem);

		// Attempt repair / restore operation.
		int iResult = RepairItem(pItem1, pResultItem);
		if(iResult == -1 || progress.CheckCancelButton())
			return;

		// Advance progress dialog
		progress.SetPos(++iCounter);
	}

	// Remove progress window
	progress.DestroyWindow();

	// Fire off results dialog.
	dlg.DoModal();

	// Build list of objects
	GetDocument()->RebuildItemArrays();

	// Recompute item totals.
	pDoc->RecalcItemTypes();

	// Refresh all views.
	pDoc->UpdateAllViews(NULL, LISTVIEW_REFRESH, NULL);
}


void CQconsoleView::HandleSortChange()
{
	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	// Get reference to list control
	CListCtrl& ListCtrl = GetListCtrl();

	// On XP, set the proper style for the sort arrow.
	if(pDoc->IsWinXP())
	{
		CHeaderCtrl* pHeader = ListCtrl.GetHeaderCtrl();
		HD_ITEM item;
		int iColCount = pHeader->GetItemCount();
		for(int iCol = 0; iCol < iColCount; iCol++)
		{
			item.mask = HDI_FORMAT;
			pHeader->GetItem(iCol, &item);

			// Clear the sort item
			if(item.fmt &(HDF_SORTDOWN | HDF_SORTUP))
				item.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);

			pHeader->SetItem(iCol, &item);
		}

		// Set the correct sort arrow.
		item.mask = HDI_FORMAT;
		pHeader->GetItem(pDoc->GetSortCol(), &item);
		item.fmt |= m_bSortAscending ? HDF_SORTUP : HDF_SORTDOWN;
		pHeader->SetItem(pDoc->GetSortCol(), &item);
	}

	// Sort items in list
	if(m_bSortAscending)
		ListCtrl.SortItems(CQconsoleView::SortAscendCompareFunc, (DWORD)this);
	else
		ListCtrl.SortItems(CQconsoleView::SortDescendCompareFunc, (DWORD)this);
}


////////////////////////////////////////////////////////////////////////////
// Called when an item in the list view changes state.  Here I set the main 
// frame status text based on the state of the last selected item.
// 5/7/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Is an item selected?
	if(pNMListView->iItem == -1 || ! (pNMListView->uNewState & LVIS_SELECTED))
		return;

	// Get pointer to item
	IQuarantineItem* pQuarItem = (IQuarantineItem*) GetListCtrl().GetItemData(pNMListView->iItem);
	ASSERT(pQuarItem);

	// Classify this item
	DWORD dwStatus;
	if(FAILED(pQuarItem->GetFileStatus(&dwStatus)))
		return;

	// Set correct hint text.
	if(dwStatus & QFILE_STATUS_QUARANTINED)
	{
		((CFrameWnd*)AfxGetMainWnd())->SetMessageText(IDS_STATUS_TEXT_QUARANTINED);
	}
	else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
	{
		((CFrameWnd*)AfxGetMainWnd())->SetMessageText(IDS_STATUS_TEXT_BACKUP);
	}
	else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
	{
		((CFrameWnd*)AfxGetMainWnd())->SetMessageText(IDS_STATUS_TEXT_THREAT_BACKUP);
	}

	*pResult = 0;
}


////////////////////////////////////////////////////////////////////////////
// Gets the security descriptor for a given file
// Argument: LPSTR lpszSourceFileName - file to get security attributes from
//			 LPBYTE *pDesc - pointer to buffer pointer.  Note that this function
//			  allocates this buffer.  It must be freed by the calling routine.
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::GetFileSecurityDesc(LPSTR lpszSourceFileName, LPBYTE *pDesc)
{
	// Do nothing of on NT.
	if(GetDocument()->IsNT() == FALSE)
		return S_OK;

	// Get security attributes of original file.
	SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION | 
		GROUP_SECURITY_INFORMATION | 
		DACL_SECURITY_INFORMATION;

	// This call should fail, returning the length needed for the 
	// security descriptor.
	DWORD dwLengthNeeded = 0;
	GetFileSecurity(lpszSourceFileName,
		si,
		(PSECURITY_DESCRIPTOR) *pDesc,
		0,
		&dwLengthNeeded);

	// Allocate space needed for call.
	*pDesc = new BYTE[dwLengthNeeded];
	if(*pDesc == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// This should get the actual security descriptor.
	if(FALSE == GetFileSecurity(lpszSourceFileName,
		si,
		(PSECURITY_DESCRIPTOR) *pDesc,
		dwLengthNeeded,
		&dwLengthNeeded))
	{
		return E_FAIL;
	}

	return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// Set file security for a filesystem object
// Argument: LPSTR szFileName - File to modify
// Argument: LPBYTE pDesc     - descriptor to set
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::SetFileSecurityDesc(LPSTR szFileName, LPBYTE pDesc)
{
	// Do nothing of on NT.
	if(GetDocument()->IsNT() == FALSE)
		return S_OK;

	BOOL bRet = FALSE;

	if(pDesc != NULL)
	{
		SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION | 
			GROUP_SECURITY_INFORMATION | 
			DACL_SECURITY_INFORMATION;

		// Save return result.
		bRet = SetFileSecurity(szFileName, si, pDesc);
	}

	return (bRet ? S_OK : E_FAIL);
}


////////////////////////////////////////////////////////////////////////////
// Handles help contents.
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnViewContents() 
{
	NAVToolbox::CNAVHelpLauncher Help;
	Help.LaunchHelp(IDH_QUARAN_MAINWIN, GetSafeHwnd());
}


// Added for NAV 2006.
// If you pass in a bit OR-ed status, this function checks if ANY of those
// statuses are selected in the list, not if ALL statuses are present in at
// least one item.  It also does an extra check of the submitted date to
// make sure it has a (somewhat) valid submitted date.
bool CQconsoleView::AnyOfStatusSelected(DWORD dwStatusToCheck)
{
	bool bLookForQuarantined = false;
	bool bLookForBackup = false;
	bool bLookForThreatBackup = false;
	bool bLookForSubmitted = false;

	if(dwStatusToCheck & QFILE_STATUS_QUARANTINED)
		bLookForQuarantined = true;

	if(dwStatusToCheck & QFILE_STATUS_BACKUP_FILE)
		bLookForBackup = true;

	if(dwStatusToCheck & QFILE_STATUS_THREAT_BACKUP)
		bLookForThreatBackup = true;

	if(dwStatusToCheck & QFILE_STATUS_SUBMITTED)
		bLookForSubmitted = true;

	CListCtrl& ListCtrl = GetListCtrl();

	int iItem = -1;
	bool bItemsOfTypeSelected = false;
	while(!bItemsOfTypeSelected && ((iItem = ListCtrl.GetNextItem(iItem, LVNI_ALL | LVNI_SELECTED)) != -1))
	{
		// Get item from list box extra data
		IQuarantineItem* pQuarItem = (IQuarantineItem*) ListCtrl.GetItemData(iItem);
		ASSERT(pQuarItem);

		// Classify this item
		DWORD dwStatus;
		if(FAILED(pQuarItem->GetFileStatus(&dwStatus)))
			continue;

		if(bLookForQuarantined && (dwStatus & QFILE_STATUS_QUARANTINED))
			bItemsOfTypeSelected = true;

		if(bLookForBackup && (dwStatus & QFILE_STATUS_BACKUP_FILE))
			bItemsOfTypeSelected = true;

		if(bLookForThreatBackup && (dwStatus & QFILE_STATUS_THREAT_BACKUP))
			bItemsOfTypeSelected = true;

		if(bLookForSubmitted && (dwStatus & QFILE_STATUS_SUBMITTED))
		{
			SYSTEMTIME tm;
			if(FAILED(pQuarItem->GetDateSubmittedToSARC(&tm)))
				continue;

			// Check to make sure this was actually submitted.
			if(0 != tm.wYear) // Has a submitted date
				bItemsOfTypeSelected = true;
		}
	}

	return bItemsOfTypeSelected;
}


bool CQconsoleView::AnyOfThreatSelected(bool bViral)
{
	CListCtrl& ListCtrl = GetListCtrl();

	int iItem = -1;
	bool bItemsOfThreatSelected = false;
	while(!bItemsOfThreatSelected && ((iItem = ListCtrl.GetNextItem(iItem, LVNI_ALL | LVNI_SELECTED)) != -1))
	{
		// Get item from list box extra data
		IQuarantineItem* pQuarItem = (IQuarantineItem*) ListCtrl.GetItemData(iItem);
		ASSERT(pQuarItem);

		DWORD dwThreat = QItemEZ::THREAT_UNKNOWN;
		dwThreat = QItemEZ::IsViral(pQuarItem);

		if(bViral && (QItemEZ::THREAT_VIRAL == dwThreat))
			bItemsOfThreatSelected = true;
		else if(!bViral && (QItemEZ::THREAT_NON_VIRAL == dwThreat))
			bItemsOfThreatSelected = true;
	}

	return bItemsOfThreatSelected;
}


bool CQconsoleView::InitializeExclusions()
{
	if(m_bExclusionsInitialized)
	{
		if(NavExclusions::Success == m_spExclusions->reloadState())
			return true;
		else
			return false;
	}

	SYMRESULT symRes = m_ExclusionManagerLoader.Initialize();
	if(!SYM_SUCCEEDED(symRes))
	{
		CCTRACEE(_T("%s - Failed to load Exclusion Manager loader. SYMRESULT = 0x%X"), __FUNCTION__, symRes);
		return false;
	}

	if(!SYM_SUCCEEDED(symRes = m_ExclusionManagerLoader.CreateObject(m_spExclusions)))
	{
		CCTRACEE(_T("%s - Failed to create Exclusion Manager. SYMRESULT = 0x%X"), __FUNCTION__, symRes);
		return false;
	}

	if(!m_spExclusions)
	{
		CCTRACEE(_T("%s - Exclusion Manager is invalid."), __FUNCTION__);
		return false;
	}

	NavExclusions::ExResult exRes = m_spExclusions->initialize(false);
	if(exRes != NavExclusions::Success)
	{
		CCTRACEE(_T("%s - Failed to initialize exclusions. ExResult = %d"), __FUNCTION__, exRes);
		m_spExclusions.Release();
		return false;
	}

	m_bExclusionsInitialized = true;
	return m_bExclusionsInitialized;
}


// Added NAV 2006
bool CQconsoleView::AddExpandedThreatExclusion(IQuarantineItem* pQuarItem)
{
	if(NULL == pQuarItem)
	{
		CCTRACEE(_T("%s - NULL quarantine item parameter. Automatically returning false."), __FUNCTION__);
		return false;
	}

	if(!m_bExclusionsInitialized)
	{
		CCTRACEE(_T("%s - Called while uninitialized. Automatically returning false."), __FUNCTION__);
		return false;
	}

	if(!m_spExclusions)
	{
		CCTRACEE(_T("%s - Called while exclusion manager is null - returning false."), __FUNCTION__);
		return false;
	}

	// We only allow non-viral exclusion adds, don't add viral items
	if(QItemEZ::THREAT_NON_VIRAL != QItemEZ::IsViral(pQuarItem))
		return false;

	ULONG ulVid = _INVALID_VID;
	DWORD dwVirID = 0;
	if(FAILED(pQuarItem->GetVirusID(&dwVirID)) || (0 == dwVirID))
		CCTRACEW(_T("%s - Could not retrieve VID from quarantine item."), __FUNCTION__);
	else
		ulVid = static_cast<ULONG>(dwVirID);

	// ????????????
	// We only want to add the GUID if this is a specific anomaly
	// otherwise this was generated and the Anomaly ID is a random GUID

	CString sAnomalyID;
	cc::IStringPtr spAnomalyID = NULL;
	if((FALSE == QItemEZ::GetAnomalyID(pQuarItem, sAnomalyID)) 
		|| sAnomalyID.IsEmpty())
	{
		// We don't need both VID and anomaly ID, just one
		CCTRACEW(_T("%s - Can't retrieve anomaly ID from quarantine item."), __FUNCTION__);
	}
	else
	{
		spAnomalyID.Attach(ccSym::CStringImpl::CreateStringImpl(sAnomalyID));
		if(spAnomalyID == NULL)
			CCTRACEW(_T("%s - Could not set anomaly ID."), __FUNCTION__);
	}

	CString sThreatName;
	cc::IStringPtr spThreatName = NULL;
	if((FALSE == QItemEZ::GetThreatName(pQuarItem, sThreatName)) 
		|| sThreatName.IsEmpty())
	{
		CCTRACEE(_T("%s - Can't retrieve threat name from quarantine item. Returning false."), __FUNCTION__);
		return false;
	}
	else
	{
		spThreatName.Attach(ccSym::CStringImpl::CreateStringImpl(sThreatName));
		if(spThreatName == NULL)
		{
			CCTRACEE(_T("%s - Could not set threat name. Returning false."), __FUNCTION__);
			return false;
		}
	}

	if((0 == ulVid) && (spAnomalyID == NULL))
	{
		CCTRACEE(_T("%s - Could not retrieve VID or GUID from quarantine item %s; no criteria to add with. Returning false."), spThreatName->GetStringA());
		return false;
	}

	if(_INVALID_ID != m_spExclusions->addExclusionAnomaly(ulVid, spAnomalyID, 0, spThreatName, 
		(NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL)))
	{
		m_bUnsavedExclusions = true;
		return true;
	}

	return false;
}


// Handle excluding all selected items that are known expanded threats
void CQconsoleView::ExcludeSelected()
{
	if(!CQconsoleView::InitializeExclusions())
		return;

	int iItem = -1;
	IQuarantineItem* pQuarItem;
	CListCtrl& ListCtrl = GetListCtrl();

	while(-1 != (iItem = ListCtrl.GetNextItem(iItem, LVNI_ALL | LVNI_SELECTED)))
	{
		pQuarItem = (IQuarantineItem*)ListCtrl.GetItemData(iItem);
		ASSERT(pQuarItem);

		AddExpandedThreatExclusion(pQuarItem);
	}

	if(m_bUnsavedExclusions)
	{
		NavExclusions::ExResult res = NavExclusions::Fail;
		res = m_spExclusions->saveState();
		if(NavExclusions::Success != res)
			CCTRACEE(_T("%s - Could not save exclusion manager state! Possible loss of data. ExResult = %d"), __FUNCTION__, res);

		m_bUnsavedExclusions = false;
	}
}


// Returns the number of columns for the current view
int CQconsoleView::GetColumnCount()
{
	if(VIEW_MODE_VIRAL_THREATS == m_iLastViewMode)
		return ColumnCount_ViralView;
	else
		return ColumnCount_ExpandedThreatView;
}


void CQconsoleView::LogRestore(IQuarantineItem* pItem, CResultsListItem* pResult)
{
	// Attempt to initialize COM
	HRESULT hrCOMInit = CoInitialize(NULL);
	bool bCOMInitialized = false;
	bool bSkipLogging = false;

	if(S_FALSE != hrCOMInit) // already init
	{
		if(S_OK == hrCOMInit)
		{
			bCOMInitialized = true;
		}
		else
		{
			if(FAILED(hrCOMInit))
			{
				CCTRACEE(_T("%s - failed to initialize COM"));
				return;
			}
		}
	}

	CQconsoleDoc* pDoc = GetDocument();
	ASSERT(NULL != pDoc);

	AV::IAvEventFactoryPtr pLogger;
	if(pLogger == NULL)
	{
		SYMRESULT symRes = AV::NAVEventFactory_IAVEventFactory::CreateObject(GETMODULEMGR(), &pLogger);
		DWORD dwError = ::GetLastError();
		if(SYM_FAILED(symRes) || (NULL == pLogger.m_p))
		{
			CCTRACEE(_T("%s - Could not create IAvFactory object. - %d"), __FUNCTION__, dwError);
			pLogger = NULL;
		}
	}

	CEventData eventData;
	eventData.SetData(AV::Event_Base_propSessionID, COSInfo::GetSessionID());

	// What version of our packet are we sending?
	eventData.SetData(AV::Event_Base_propVersion, THREAT_PACKET_VERSION);

	eventData.SetData(AV::Event_Base_propType, AV::Event_ID_Threat);
	eventData.SetData(AV::Event_Threat_propFeature, AV_MODULE_ID_QUARANTINE);

	TCHAR szDefsDate[13] = {0};
	if(pDoc->GetDefsDate(szDefsDate))
		eventData.SetData(AV::Event_Threat_propDefsRevision, szDefsDate);

	CNAVVersion Version;
	eventData.SetData(AV::Event_Threat_propProductVersion, Version.GetPublicRevision());

	eventData.SetData(AV::Event_Threat_propEventSubType, AV::Event_Threat);
	eventData.SetData(AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File);

	// SUBFILE DATA
	CEventData SubFile;
	SubFile.SetData(AV::Event_ThreatSubFile_propFileName, m_sAppName);

	// Make a temp threat
	CEventData threat;

	DWORD dwVirID = 0;
	if(SUCCEEDED(pItem->GetVirusID(&dwVirID)) || (0 != dwVirID))
		threat.SetData(AV::Event_ThreatEntry_propVirusID, dwVirID);

	CString sThreatName;
	if((FALSE != QItemEZ::GetThreatName(pItem, sThreatName)) 
		&& !sThreatName.IsEmpty())
	{
		ccLib::CString strVirusName = sThreatName;
		threat.SetData(AV::Event_ThreatEntry_propVirusName, (LPCSTR)strVirusName);
	}

	// Action 
	BYTE byAction = AV::Event_Action_Restored;
	threat.SetData(AV::Event_ThreatEntry_propActionCount, 1);
	threat.SetData(AV::Event_ThreatEntry_propActionData, &byAction, 1);

	// Fill in the threat categories
	CString sThreatCategories;
	int iCategoryCount = 0;
	if(FALSE != QItemEZ::GetThreatCategoryList(pItem, sThreatCategories, FALSE))
	{
		sThreatCategories.Trim();

		int iStart = 0;
		int iIndex = -1;

		do
		{
			sThreatCategories.Find(_T(' '), iStart);
			if(-1 != iIndex)
				iCategoryCount++;

			iStart = iIndex;
		} while(-1 != iIndex);

		threat.SetData(AV::Event_ThreatEntry_propThreatCatCount, iCategoryCount);
		threat.SetData(AV::Event_ThreatEntry_propThreatCategories, (LPCTSTR)sThreatCategories);
	}

	CString sDescription;
	sDescription.LoadString(IDS_LOG_DESCRIPTION_HEADER); // "Affected Areas:"

	POSITION pos;
	CRepairActionList *pList = &(pResult->m_aSubActionList);
	for(pos = pList->GetHeadPosition(); pos != NULL; pList->GetNext(pos))
	{
		sDescription += _T("\n");
		sDescription += (pResult->m_aSubActionList).GetAt(pos)->GetName(); // Name of item
		sDescription += _T(" - ");
		sDescription += (pResult->m_aSubActionList).GetAt(pos)->Format();  // Formatted action text
	}

	SubFile.SetData(AV::Event_ThreatSubFile_propFileDescription, (LPCTSTR)sDescription);


	// Save
	CEventData AllThreats;
	AllThreats.SetNode(0, threat);

	SubFile.SetNode(AV::Event_ThreatSubFile_propThreatData, AllThreats);

	CEventData AllFiles;
	AllFiles.SetNode(0, SubFile);
	eventData.SetNode(AV::Event_Threat_propSubFileData, AllFiles);

	if(pLogger != NULL)
	{
		SYMRESULT result = pLogger->BroadcastAvEvent(eventData);
		if(SYM_FAILED(result))
		{
			CCTRACEE(_T("%s - BroadcastAvEvent failed - %d"), __FUNCTION__, result);
		}
	}

	if(bCOMInitialized)
		CoUninitialize();
}
