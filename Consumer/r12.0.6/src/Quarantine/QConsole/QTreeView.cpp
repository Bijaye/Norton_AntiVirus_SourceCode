//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QTreeView.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/QTreeView.cpv  $
// 
//    Rev 1.12   04 Jun 1998 21:56:56   DBuches
// Added function header comments.
// 
//    Rev 1.11   04 Jun 1998 15:37:44   DBuches
// Now, we emulate explorer in terms of right-clicking.
// 
//    Rev 1.10   28 May 1998 14:46:48   DBuches
// Fixed problem with updating main window title in corporate mode.
// 
//    Rev 1.9   21 May 1998 11:27:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.8   11 May 1998 08:24:28   DBuches
// Added new artwork.
// 
//    Rev 1.7   07 May 1998 15:08:40   DBuches
// Added support for status text.
// 
//    Rev 1.6   06 May 1998 13:24:20   DBuches
// Recalc item totals on refresh.
// 
//    Rev 1.5   03 Apr 1998 13:29:08   DBuches
// Added code to update titlebar of the main window when filters change.
// 
//    Rev 1.4   13 Mar 1998 15:23:08   DBuches
// Select quarantine filter by default.
// 
//    Rev 1.3   11 Mar 1998 15:17:04   DBuches
// Added OnUpdate handler for item deletes.
// 
//    Rev 1.2   06 Mar 1998 11:21:52   DBuches
// Checked in more work in progress.
// 
//    Rev 1.1   03 Mar 1998 17:05:02   DBuches
// Added quarantine items node.
// 
//    Rev 1.0   27 Feb 1998 15:09:00   DBuches
// Initial revision.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "QTreeView.h"
#include "qconhlp.h"
#include "Navhelplauncher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CQTreeView

IMPLEMENT_DYNCREATE(CQTreeView, CTreeView)

CQTreeView::CQTreeView()
{
	m_hRootExpandedThreats = NULL;
	m_hRootViralThreats = NULL;

	m_pOldSel = NULL;
}

CQTreeView::~CQTreeView()
{
}


BEGIN_MESSAGE_MAP(CQTreeView, CTreeView)
	//{{AFX_MSG_MAP(CQTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_CONTENTS, OnViewContents)
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////
// CQTreeView drawing

void CQTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


//////////////////////////////////////////////////////////////////////////////
// CQTreeView diagnostics

#ifdef _DEBUG
void CQTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CQTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CQconsoleDoc* CQTreeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CQconsoleDoc)));
	return (CQconsoleDoc*)m_pDocument;
}

#endif //_DEBUG


//////////////////////////////////////////////////////////////////////////////
// Override of CView::OnInitialUpdate()
// 2/20/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
}


//////////////////////////////////////////////////////////////////////////////
// Override for CWnd::Create()
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
BOOL CQTreeView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// Create window
	BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	// Set up tree control if successful.
	if(bRet)
		SetupTree();

	return bRet;
}


//////////////////////////////////////////////////////////////////////////////
// Sets up the tree control
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::SetupTree()
{
	CTreeCtrl& tree = GetTreeCtrl();

	// Set our styles
	tree.ModifyStyle(0, TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP);

	// Set up image list
	m_ImageList.Create(IDB_TREE_BITMAPS, 16, 1, RGB(255, 0, 255));
	tree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	// Create root items

	CString sTreeItem;
	bool bShowExpandedThreats = GetDocument()->GetExpandedThreatShowSetting();
	if(bShowExpandedThreats)
	{
		sTreeItem.LoadString(IDS_TREE_ROOT_EXPANDED_THREATS);
		m_hRootExpandedThreats = tree.InsertItem(sTreeItem, 4, 4);
		tree.SetItemData(m_hRootExpandedThreats, VIEW_MODE_EXPANDED_THREATS);
	}

	sTreeItem.LoadString(IDS_TREE_ROOT_VIRAL_THREATS);
	m_hRootViralThreats = tree.InsertItem(sTreeItem, 1, 1);
	tree.SetItemData(m_hRootViralThreats, VIEW_MODE_VIRAL_THREATS);

	// Select root node
	if(bShowExpandedThreats)
		tree.SelectItem(m_hRootExpandedThreats);
	else
		tree.SelectItem(m_hRootViralThreats);
}


//////////////////////////////////////////////////////////////////////////////
// Override of CView::OnUpdate
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch(lHint)
	{
	case SET_VIEW_FILTER:
		HandleFilterChange();
		break;
	case LISTVIEW_REFRESH:
	case LISTVIEW_DELETE_ITEMS:
		HandleRefresh();
		break;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Selectes the correct item in the tree.
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::HandleFilterChange()
{    
	// Get tree control.
	CTreeCtrl& tree = GetTreeCtrl();

	// Select the correct node
	HTREEITEM hItem = NULL;
	switch(GetDocument()->GetViewMode())
	{
	case VIEW_MODE_EXPANDED_THREATS:
		if(NULL != m_hRootExpandedThreats)
			hItem = m_hRootExpandedThreats;
		break;

	case VIEW_MODE_VIRAL_THREATS:
		hItem = m_hRootViralThreats;
		break;
	}

	// Select the correct item.
	tree.SelectItem(hItem);
}


//////////////////////////////////////////////////////////////////////////////
// Handles the refresh hint.
// 3/5/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::HandleRefresh()
{
	// Get pointer to document    
	CQconsoleDoc* pDoc = GetDocument();

	// Get tree control.
	CTreeCtrl& tree = GetTreeCtrl();

	// Recalc item totals
	pDoc->RecalcItemTypes();

	// Set the number of items for each node.
	CString sTreeItem;
	if(NULL != m_hRootExpandedThreats)
	{
		sTreeItem.Format(IDS_TREE_ROOT_EXPANDED_THREATS_FORMAT, pDoc->GetExpandedThreatItemCount());
		tree.SetItemText(m_hRootExpandedThreats, sTreeItem);
	}

	sTreeItem.Format(IDS_TREE_ROOT_VIRAL_THREATS_FORMAT, pDoc->GetViralItemCount());
	tree.SetItemText(m_hRootViralThreats, sTreeItem);
}


//////////////////////////////////////////////////////////////////////////////
// Handle selection changing in tree control
// 2/25/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// Get Tree control
	CTreeCtrl& tree = GetTreeCtrl();

	// Set bold state
	tree.SetItemState(pNMTreeView->itemOld.hItem, ~TVIS_BOLD, TVIS_BOLD);
	tree.SetItemState(pNMTreeView->itemNew.hItem, TVIS_BOLD, TVIS_BOLD);

	// Set new view mode.
	int iViewMode = (int) tree.GetItemData(pNMTreeView->itemNew.hItem);
	GetDocument()->SetViewMode(iViewMode);

	// Tell everyone what happened.
	GetDocument()->UpdateAllViews(this, SET_VIEW_FILTER, NULL);

	*pResult = 0;
}


//////////////////////////////////////////////////////////////////////////////
// Handles help contents.
// 5/21/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnViewContents() 
{
	NAVToolbox::CNAVHelpLauncher Help;
	Help.LaunchHelp(IDH_QUARAN_MAINWIN, GetSafeHwnd());
}


//////////////////////////////////////////////////////////////////////////////
// Handles WM_CONTEXTMENU - right mouse click.
// 2/27/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	UINT uFlags;
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	// Get point where user clicked in tree
	CPoint ptTree = point;
	treeCtrl.ScreenToClient(&ptTree);

	// Did we get an item?
	HTREEITEM htItem = treeCtrl.HitTest(ptTree, &uFlags);
	if((htItem != NULL) && (uFlags & TVHT_ONITEM))
	{
		ShowPopupMenu(point);
		treeCtrl.SetItemState(htItem, 0, TVIS_DROPHILITED);        
	}
	else
	{
		ShowPopupMenu(point);
	}

	// Preserve old selection
	if(m_pOldSel != NULL) 
	{
		treeCtrl.Select(m_pOldSel, TVGN_DROPHILITE);
		m_pOldSel = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Shows right click context menu.
// 6/4/98 DBUCHES, created - header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::ShowPopupMenu(CPoint pt)
{
	// Check for keystroke invocation
	if((pt.x == -1) && (pt.y == -1))
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		pt = rect.TopLeft();
		pt.Offset(5, 5);        
	}

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

	// Load correct popup menu
	CMenu* pPopup = menu.GetSubMenu(0);        
	ASSERT(pPopup);

	if(VIEW_MODE_EXPANDED_THREATS == GetDocument()->GetViewMode())
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

	// Fire off context menu
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x,
		pt.y,
		AfxGetMainWnd());
}


//////////////////////////////////////////////////////////////////////////////
// Handles right mouse click.  Needed to override this because
//	of the silly way CTreeView wraps the tree control.
// 6/4/98 DBUCHES, created - header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags;
	HTREEITEM htItem = GetTreeCtrl().HitTest(point, &uFlags);
	if((htItem != NULL) && (uFlags & TVHT_ONITEM))
		GetTreeCtrl().Select(htItem, TVGN_CARET); //DROPHILITE);
}


//////////////////////////////////////////////////////////////////////////////
// 6/4/98 DBUCHES, created - header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}


//////////////////////////////////////////////////////////////////////////////
// Handles left mouse click.  Needed to override this because
//	of the silly way CTreeView wraps the tree control.
// 6/4/98 DBUCHES, created - header added.
//////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags;
	HTREEITEM htItem = GetTreeCtrl().HitTest(point, &uFlags);

	if((htItem != NULL) && (uFlags & TVHT_ONITEM))
		GetTreeCtrl().Select(htItem,  TVGN_CARET); //DROPHILITE);

	CTreeView::OnLButtonDown(nFlags, point);
}
