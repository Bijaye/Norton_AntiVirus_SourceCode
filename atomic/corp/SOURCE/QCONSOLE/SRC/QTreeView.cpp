/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/QTreeView.cpv   1.12   04 Jun 1998 21:56:56   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// QTreeView.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "QTreeView.h"
#include "qconhlp.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQTreeView

IMPLEMENT_DYNCREATE(CQTreeView, CTreeView)

CQTreeView::CQTreeView()
{
    m_hRoot = m_hBackup = 
    m_hSubmitted = NULL;

    m_pOldSel = NULL;
}

CQTreeView::~CQTreeView()
{
}


BEGIN_MESSAGE_MAP(CQTreeView, CTreeView)
	//{{AFX_MSG_MAP(CQTreeView)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_CONTEXTMENU()
  	ON_COMMAND(ID_VIEW_WHATSTHIS, OnWhatsThis)
    ON_COMMAND(ID_VIEW_CONTENTS, OnViewContents)
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQTreeView drawing

void CQTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
// CQconsoleView printing

BOOL CQTreeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}


void CQTreeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CQTreeView message handlers



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQTreeView::OnFilePrint
// Description	    : Handles Print menu item.  Just passes a hint to other view
//                    for printing 
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQTreeView::OnFilePrintPreview
// Description	    : Same as OnFilePrint.  Passes hint
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnFilePrintPreview() 
{
	// TODO: Add your command handler code here
	
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQTreeView::OnInitialUpdate
// Description	    : Override of CView::OnInitialUpdate()
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();

}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQTreeView::Create
//
// Description	    : Override for CWnd::Create()
//
// Return type		: BOOL 
//
// Argument         : LPCTSTR lpszClassName
// Argument         : LPCTSTR lpszWindowName
// Argument         : DWORD dwStyle
// Argument         : const RECT& rect
// Argument         : CWnd* pParentWnd
// Argument         : UINT nID
// Argument         : CCreateContext* pContext
//
////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQTreeView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
    // Create window
	BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
    
    // Set up tree control if successful.
    if( bRet )
        SetupTree();

    return bRet;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQTreeView::SetupTree
//
// Description	    : Sets up the tree control
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQTreeView::SetupTree()
{
    CTreeCtrl& tree = GetTreeCtrl();
    
	// Set our styles
    tree.ModifyStyle( 0, TVS_HASLINES | TVS_HASBUTTONS );

    // Set up image list
    m_ImageList.Create( IDB_TREE_BITMAPS, 16, 1, RGB( 255, 0, 255 ) );
    tree.SetImageList( &m_ImageList, TVSIL_NORMAL );

    // Create root item.
    CString s;
    s.LoadString( IDS_TREE_ROOT_TEXT );
    m_hRoot = tree.InsertItem( s, 0, 0 );
    tree.SetItemData( m_hRoot, VIEW_MODE_ALL );

    // Add children
    s.LoadString( IDS_QUARANTINE_ITEMS );
    m_hQuarantine = tree.InsertItem( s, 1, 1, m_hRoot );
    tree.SetItemData( m_hQuarantine, VIEW_MODE_QUARANTINE );

    s.LoadString( IDS_TREE_BACKUP_ITEMS );
    m_hBackup = tree.InsertItem( s, 2, 2, m_hRoot );
    tree.SetItemData( m_hBackup, VIEW_MODE_BACKUP );
    
    s.LoadString( IDS_TREE_SUBMITTED_ITEMS );
    m_hSubmitted = tree.InsertItem( s, 3, 3, m_hRoot );
    tree.SetItemData( m_hSubmitted, VIEW_MODE_SUBMITTED );
   
    // Expand root node.
    tree.Expand( m_hRoot, TVE_EXPAND );

    // Select root node
    tree.SelectItem( m_hQuarantine );

}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQTreeView::OnUpdate
//
// Description	    : Override of CView::OnUpdate
//
// Return type		: void 
//
// Argument         : CView* pSender
// Argument         : LPARAM lHint
// Argument         : CObject* pHint
//
////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch( lHint )
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


////////////////////////////////////////////////////////////////////////////
// Function name	: CQTreeView::HandleFilterChange
//
// Description	    : Selectes the correct item in the tree.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQTreeView::HandleFilterChange()
{    
    // Get tree control.
    CTreeCtrl & tree = GetTreeCtrl();

    // Select the correct node
    HTREEITEM hItem = NULL;
    switch( GetDocument()->GetViewMode() )
        {
        case VIEW_MODE_ALL:
            hItem = m_hRoot;
            break;

        case VIEW_MODE_QUARANTINE:
            hItem = m_hQuarantine;
            break;

        case VIEW_MODE_BACKUP:
            hItem = m_hBackup;
            break;

        case VIEW_MODE_SUBMITTED:
            hItem = m_hSubmitted;
            break;
        }

    // Select the correct item.
    tree.SelectItem( hItem );

}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQTreeView::HandleRefresh
// Description	    : Handles the refresh hint.
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 3/5/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQTreeView::HandleRefresh()
{
    // Get pointer to document    
    CQconsoleDoc* pDoc = (CQconsoleDoc*) GetDocument();

    // Get tree control.
    CTreeCtrl & tree = GetTreeCtrl();

    // Recalc item totals
    pDoc->RecalcItemTypes();
    
    // Set the number of items for each node.
    CString s;
    s.Format( IDS_TREE_ALL_FORMAT, pDoc->GetItemCount() );
    tree.SetItemText( m_hRoot, s );
    s.Format( IDS_TREE_QUARANTINE_FORMAT, pDoc->GetQuarantineItemCount() );
    tree.SetItemText( m_hQuarantine, s );
    s.Format( IDS_TREE_BACKUP_FORMAT, pDoc->GetBackupItemCount() );
    tree.SetItemText( m_hBackup, s );
    s.Format( IDS_TREE_SUBMITTED_FORMAT, pDoc->GetSubmittedItemCount() );
    tree.SetItemText( m_hSubmitted, s );

    // Update main frame window text
    HTREEITEM hItem = NULL;
    switch( GetDocument()->GetViewMode() )
        {
        case VIEW_MODE_ALL:
            hItem = m_hRoot;
            break;

        case VIEW_MODE_QUARANTINE:
            hItem = m_hQuarantine;
            break;

        case VIEW_MODE_BACKUP:
            hItem = m_hBackup;
            break;

        case VIEW_MODE_SUBMITTED:
            hItem = m_hSubmitted;
            break;
        }

    // Update main window text with view mode
    if( GetDocument()->GetCorpMode() )
        s.Format( IDS_MAINFRM_TITLE_SERVER_FMT, tree.GetItemText( hItem ) );
    else
        s.Format( IDS_MAINFRM_TITLE_FMT, tree.GetItemText( hItem ) );
    
    if( IsWindow( AfxGetMainWnd()->GetSafeHwnd()) )
        AfxGetMainWnd()->SetWindowText(s);

}

////////////////////////////////////////////////////////////////////////////
// Function name	: CQTreeView::OnSelchanged
//
// Description	    : Handle selection changing in tree control
//
// Return type		: void 
//
// Argument         : NMHDR* pNMHDR
// Argument         : LRESULT* pResult
//
////////////////////////////////////////////////////////////////////////////
// 2/25/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Get Tree control
    CTreeCtrl & tree = GetTreeCtrl();

    // Set bold state
    tree.SetItemState( pNMTreeView->itemOld.hItem, ~TVIS_BOLD, TVIS_BOLD );
    tree.SetItemState( pNMTreeView->itemNew.hItem, TVIS_BOLD, TVIS_BOLD );

    // Set new view mode.
    int iViewMode = (int) tree.GetItemData( pNMTreeView->itemNew.hItem );
    GetDocument()->SetViewMode( iViewMode );

    // Update main window text with view mode
    CString s;

    // Update main window text with view mode
    if( GetDocument()->GetCorpMode() )
        s.Format( IDS_MAINFRM_TITLE_SERVER_FMT, tree.GetItemText( pNMTreeView->itemNew.hItem ) );
    else
        s.Format( IDS_MAINFRM_TITLE_FMT, tree.GetItemText( pNMTreeView->itemNew.hItem ) );
    
    if( IsWindow( AfxGetMainWnd()->GetSafeHwnd()) )
        {
        AfxGetMainWnd()->SetWindowText(s);
#if 0
        // Set status bar text.
        switch( iViewMode )
            {
            case VIEW_MODE_ALL:
                ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( IDS_STATUS_TEXT_ALL_VIEW );
                break;

            case VIEW_MODE_QUARANTINE:
                ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( IDS_STATUS_TEXT_QUARANTINED_VIEW );
                break;

            case VIEW_MODE_BACKUP:
                ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( IDS_STATUS_TEXT_BACKUP_VIEW );
                break;

            case VIEW_MODE_SUBMITTED:
                ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( IDS_STATUS_TEXT_SUBMITTED_VIEW );
                break;
            }
#endif
        }

    // Tell everyone what happened.
    GetDocument()->UpdateAllViews( this, SET_VIEW_FILTER, NULL );
	
	*pResult = 0;
}





////////////////////////////////////////////////////////////////////////////
// Function name	: CQTreeView::OnWhatsThis
//
// Description	    : Handles right-click whats this help.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnWhatsThis() 
{
	// Invoke help system
    ::WinHelp( GetSafeHwnd(), 
               AfxGetApp()->m_pszHelpFilePath,
               HELP_CONTEXTPOPUP,
               IDH_QUARAN_MAINWIN );
	
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQTreeView::OnViewContents
//
// Description	    : Handles help contents.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnViewContents() 
{
		// Invoke help system
    ::WinHelp( GetSafeHwnd(), 
               AfxGetApp()->m_pszHelpFilePath,
               HELP_FINDER,
               0 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQTreeView::OnContextMenu
// Description	    : Handles WM_CONTEXTMENU - right mouse click.
// Return type		: void 
// Argument         : CWnd* pWnd
// Argument         : CPoint point
//
////////////////////////////////////////////////////////////////////////////////
// 2/27/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQTreeView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    UINT uFlags;
    CTreeCtrl& treeCtrl = GetTreeCtrl();
   
    // Get point where user clicked in tree
    CPoint ptTree = point;
    treeCtrl.ScreenToClient(&ptTree);

    // Did we get an item?
    HTREEITEM htItem = treeCtrl.HitTest(ptTree, &uFlags);
    if ((htItem != NULL) && (uFlags & TVHT_ONITEM))
        {
        ShowPopupMenu( point );
        treeCtrl.SetItemState(htItem, 0, TVIS_DROPHILITED);        
        }
    else
        ShowPopupMenu( point );
    
    // Preserve old selection
    if (m_pOldSel != NULL) 
        {
        treeCtrl.Select(m_pOldSel, TVGN_DROPHILITE);
        m_pOldSel = NULL;
        }
}

//*************************************************************************
// CQTreeView::ShowPopupMenu()
//
// CQTreeView::ShowPopupMenu(
//      CPoint pt )
//
// Description: Shows right click context menu.
//
// Returns: void 
//
//*************************************************************************
// 6/4/98 DBUCHES, created - header added.
//*************************************************************************

void CQTreeView::ShowPopupMenu( CPoint pt )
{
    // Check for keystroke invocation
    if (pt.x == -1 && pt.y == -1)
        {
        CRect rect;
        GetClientRect(rect);
        ClientToScreen(rect);
        pt = rect.TopLeft();
        pt.Offset(5, 5);        
        }
    
    // Load context menu for view
    CMenu menu;
    if( menu.LoadMenu( IDR_VIEW_CONTEXT ) == FALSE )
        return;

    // Load correct popup menu
    CMenu* pPopup = menu.GetSubMenu( 0 );        
    ASSERT( pPopup );

    // Fire off context menu
    pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                            pt.x,
                            pt.y,
                            AfxGetMainWnd() );
}


//*************************************************************************
// CQTreeView::OnRButtonDown()
//
// CQTreeView::OnRButtonDown(
//      UINT nFlags
//      CPoint point )
//
// Description: Handles right mouse click.  Needed to override this because
//              of the silly way CTreeView wraps the tree control.
//
// Returns: void 
//
//*************************************************************************
// 6/4/98 DBUCHES, created - header added.
//*************************************************************************

void CQTreeView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    UINT uFlags;
    HTREEITEM htItem = GetTreeCtrl().HitTest(point, &uFlags);
    if ((htItem != NULL) && (uFlags & TVHT_ONITEM))
        {
        //m_pOldSel = GetTreeCtrl().GetSelectedItem();
        GetTreeCtrl().Select(htItem, TVGN_CARET );//DROPHILITE);
        }

	//CTreeView::OnRButtonDown(nFlags, point);
}

//*************************************************************************
// CQTreeView::OnKeyDown()
//
// CQTreeView::OnKeyDown(
//      UINT nChar
//      UINT nRepCnt
//      UINT nFlags )
//
// Description:
//
// Returns: void 
//
//*************************************************************************
// 6/4/98 DBUCHES, created - header added.
//*************************************************************************

void CQTreeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//*************************************************************************
// CQTreeView::OnLButtonDown()
//
// CQTreeView::OnLButtonDown(
//      UINT nFlags
//      CPoint point )
//
// Description: Handles left mouse click.  Needed to override this because
//              of the silly way CTreeView wraps the tree control.
//
// Returns: void 
//
//*************************************************************************
// 6/4/98 DBUCHES, created - header added.
//*************************************************************************

void CQTreeView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    UINT uFlags;
    HTREEITEM htItem = GetTreeCtrl().HitTest(point, &uFlags);
    
    if( (htItem != NULL) && (uFlags & TVHT_ONITEM) )
        GetTreeCtrl().Select(htItem,  TVGN_CARET );//DROPHILITE);
    
    CTreeView::OnLButtonDown(nFlags, point);
}
