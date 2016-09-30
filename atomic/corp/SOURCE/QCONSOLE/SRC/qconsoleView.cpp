/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/qconsoleView.cpv   1.29   30 Jun 1998 22:23:56   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// qconsoleView.cpp : implementation of the CQconsoleView class
//
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
#include "qconsole.h"
#include "platform.h"
#include "qconsoleDoc.h"
#include "qconsoleView.h"
#include "shellapi.h"
#include "ItemPropSheet.h"
#include "cdcos.h"
#include "const.h"
#include "xapi.h"
#include "progdlg.h"
#include "CorpRepairDlg.h"
#include "qconhlp.h"


#define INITGUID
#include "iscandeliver.h"
// bugbug - should not need this
#include "quaradd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Index of generic executable icon in system image list.
#define EXE_IMAGE_INDEX 2

/////////////////////////////////////////////////////////////////////////////
// CQconsoleView

IMPLEMENT_DYNCREATE(CQconsoleView, CListView)

BEGIN_MESSAGE_MAP(CQconsoleView, CListView)
	//{{AFX_MSG_MAP(CQconsoleView)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ACTION_REPAIR, OnActionRepair)
    ON_COMMAND(ID_ACTION_DELETEITEM, OnActionDeleteitem)
	ON_COMMAND(ID_ACTION_RESTORE, OnActionRestore)
	ON_COMMAND(ID_ACTION_PROPERTIES, OnActionProperties)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_ACTION_SUBMITTOSARC, OnActionSubmittosarc)
	ON_WM_DRAWITEM()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_DESTROY()
	ON_WM_SIZE()
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_UPDATE_COMMAND_UI(ID_ACTION_PROPERTIES, OnUpdateProperties)
	ON_UPDATE_COMMAND_UI(ID_ACTION_REPAIR, OnUpdateSelectedItem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_COMMAND(ID_VIEW_WHATSTHIS, OnWhatsThis)
	ON_UPDATE_COMMAND_UI(ID_ACTION_DELETEITEM, OnUpdateSelectedItem)
    ON_UPDATE_COMMAND_UI(ID_ACTION_RESTORE, OnUpdateSelectedItem)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SUBMITTOSARC, OnUpdateSelectedItem)
	ON_COMMAND(ID_VIEW_CONTENTS, OnViewContents)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQconsoleView construction/destruction

CQconsoleView::CQconsoleView()
{
	// TODO: add construction code here
    //m_iLastSortCol = COLUMN_FILE_NAME;
    m_iLastViewMode = VIEW_MODE_QUARANTINE;
    m_bSortAscending = TRUE;
}

CQconsoleView::~CQconsoleView()
{
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
	CQconsoleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CQconsoleView printing

BOOL CQconsoleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}


void CQconsoleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
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
//
// Function name	: CQconsoleView::OnInitialUpdate
// Description	    : Override of CView::OnInitialUpdate().
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

    // Build list of objects
    GetDocument()->RebuildItemArrays();

    // Update our views.
    GetDocument()->UpdateAllViews( NULL, LISTVIEW_REFRESH, NULL );

    // Sort items in list
    m_bSortAscending = TRUE;
    GetListCtrl().SortItems( CQconsoleView::SortAscendCompareFunc, (DWORD) this );

}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::Create
// Description	    : Override for CWnd::Create().  Adjust window style here.
// Return type		: BOOL
// Argument         : LPCTSTR lpszClassName
// Argument         : LPCTSTR lpszWindowName
// Argument         : DWORD dwStyle
// Argument         : const RECT& rect
// Argument         : CWnd* pParentWnd
// Argument         : UINT nID
// Argument         : CCreateContext* pContext
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// Create the control
    BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

    // Set up correct list view style.
    if( bRet )
        {
        CListCtrl & ListCtrl = GetListCtrl();

        // Get saved style and set up.
        DWORD dwViewStyle = GetDocument()->GetListViewStyle();
        ListCtrl.ModifyStyle( 0, dwViewStyle | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS);

        ListView_SetExtendedListViewStyle( ListCtrl, LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT );

        // Set up our list control.
        SetupListCtrl();
        }

    return bRet;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::SetupListCtrl
// Description	    : Set up essentials for our list control.
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::SetupListCtrl()
{
    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

    // Get column widths
    int aColumns[COLUMN_MAX];
    GetColumnWidths( aColumns, COLUMN_MAX );

    // Add list control column headers
    CString s;

    s.LoadString( IDS_FILE_NAME );
    ListCtrl.InsertColumn( COLUMN_FILE_NAME, s, LVCFMT_LEFT, aColumns[COLUMN_FILE_NAME] );

    // If running on server, 2nd column should display user name.
    if( GetDocument()->GetCorpMode() )
        s.LoadString( IDS_USERNAME );
    else
        s.LoadString( IDS_ORIG_LOCATION );

    ListCtrl.InsertColumn( COLUMN_LOCATION, s, LVCFMT_LEFT, aColumns[COLUMN_LOCATION] );

    s.LoadString( IDS_QUARANTINE_DATE );
    ListCtrl.InsertColumn( COLUMN_QDATE, s, LVCFMT_LEFT, aColumns[COLUMN_QDATE] );

    s.LoadString( IDS_SUBMITTED_DATE );
    ListCtrl.InsertColumn( COLUMN_SDATE, s, LVCFMT_LEFT, aColumns[COLUMN_SDATE] );

//  s.LoadString( IDS_QUARANTINE_TYPE );
//  ListCtrl.InsertColumn( COLUMN_TYPE, s, LVCFMT_LEFT, aColumns[COLUMN_TYPE] );

    s.LoadString( IDS_STATUS );
    ListCtrl.InsertColumn( COLUMN_STATUS, s, LVCFMT_LEFT, aColumns[COLUMN_STATUS] );

    s.LoadString( IDS_VIRUS_NAME );
    ListCtrl.InsertColumn( COLUMN_VIRUS_NAME, s, LVCFMT_LEFT, aColumns[COLUMN_VIRUS_NAME] );


    // Get system image lists for our use here.  This is
    // done by making calls to SHGetFileInfo().  See SDK docs
    // for more details
    TCHAR szPath[MAX_PATH];
    SHFILEINFO sfi;
    GetModuleFileName( NULL, szPath, MAX_PATH );

    // Get small image list
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    DWORD dwImageListHandle = SHGetFileInfo( szPath,
                            0,
                            &sfi,
                            sizeof( SHFILEINFO ),
                            SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_SMALLICON
                            );

    ASSERT( dwImageListHandle );
    ListView_SetImageList( ListCtrl.GetSafeHwnd(), dwImageListHandle, LVSIL_SMALL );

    // Get large image list
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    dwImageListHandle = SHGetFileInfo( szPath,
                            0,
                            &sfi,
                            sizeof( SHFILEINFO ),
                            SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_LARGEICON
                            );

    ASSERT( dwImageListHandle );
    ListView_SetImageList( ListCtrl.GetSafeHwnd(), dwImageListHandle, LVSIL_NORMAL );

    // Get executable images
    GetExeImages( );

    // Need to set up owner draw column sort stuff here.
    CHeaderCtrl *pHdr = (CHeaderCtrl*) ListCtrl.GetWindow( GW_CHILD );

    // Make each item owner draw
    HD_ITEM item;
    for(int i= COLUMN_FILE_NAME; i < COLUMN_MAX; i++)
        {
        item.mask = HDI_FORMAT;
        item.fmt = HDF_OWNERDRAW;
        pHdr->SetItem(i, &item );
        }

}

////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::GetExeImages
//
// Description	    : Make sure the system image list has the proper image
//                    for an executable.
//
// Return type		: void
//
//
////////////////////////////////////////////////////////////////////////////
// 3/9/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetExeImages()
{
    // Create bogus test file.
    CFile file( _T("QTESTICO.COM"), CFile::modeCreate );

    // Save off name
    CString sFileName = file.GetFileName();
    file.Close();

    // Get icon for this file
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    SHGetFileInfo( sFileName,
                   0,
                   &sfi,
                   sizeof( SHFILEINFO ),
                   SHGFI_SYSICONINDEX | SHGFI_LARGEICON
                   );
    m_iLargeExeImage = sfi.iIcon;

    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    SHGetFileInfo( sFileName,
                   0,
                   &sfi,
                   sizeof( SHFILEINFO ),
                   SHGFI_SYSICONINDEX | SHGFI_SMALLICON
                   );
    m_iSmallExeImage = sfi.iIcon;

    // Delete test file.
    CFile::Remove( sFileName );
}




////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnUpdate
// Description	    : Handles UpdateAllViews
// Return type		: void
// Argument         : CView* pSender
// Argument         : LPARAM lHint
// Argument         : CObject* pHint
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch( lHint )
        {
        case LISTVIEW_STYLE_CHANGE:
            HandleStyleChange();
            break;

        case LISTVIEW_REFRESH:
            HandleRefresh();
            break;

        case SET_VIEW_FILTER:
            HandleFilterChange();
            break;

        case LISTVIEW_SORTITEMS:
            // Old sort column is sent in pHint
            if( (int) pHint == GetDocument()->GetSortCol() )
                m_bSortAscending = !m_bSortAscending;
            else
                m_bSortAscending = TRUE;
            
            if( m_bSortAscending )
                GetListCtrl().SortItems( CQconsoleView::SortAscendCompareFunc, (DWORD) this );
            else            
                GetListCtrl().SortItems( CQconsoleView::SortDescendCompareFunc, (DWORD) this );
            return;

        case LISTVIEW_NEW_DEFS:
            HandleNewDefsScan();
            break;

        }
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::HandleStyleChange
// Description	    : Sets the correct list view style.
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::HandleStyleChange()
{
    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

    // Turn off redraw for now.
    ListCtrl.SetRedraw( FALSE );

    // If we are in report view now, save column widths
    if( (ListCtrl.GetStyle() & LVS_TYPEMASK) == LVS_REPORT )
        SaveColumnWidths();

    // Set correct style
    ListCtrl.ModifyStyle( LVS_REPORT | LVS_ICON | LVS_LIST | LVS_SMALLICON,
                          GetDocument()->GetListViewStyle() );

    ListCtrl.Arrange( LVA_DEFAULT );

    // Reset redraw
    ListCtrl.SetRedraw( TRUE );

}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::HandleFilterChange
// Description	    : Repopulates the list with items
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::HandleFilterChange()
{
    // Get pointer to document
    CQconsoleDoc* pDoc = GetDocument();

    // Get the current view mode and see if we are
    // already in this mode.  If so, bail out.
    int iCurrentMode = pDoc->GetViewMode();
    if( iCurrentMode == m_iLastViewMode )
        return;

    // Save off current mode.
    m_iLastViewMode = iCurrentMode;

    // Repopulate list
    PopulateListCtrl();

}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnContextMenu
// Description	    : Handle right mouse click
// Return type		: void
// Argument         : CWnd* pWnd
// Argument         : CPoint point
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnContextMenu(CWnd* pWnd, CPoint point)
{
    // Load context menu for view
    CMenu menu;
    if( menu.LoadMenu( IDR_VIEW_CONTEXT ) == FALSE )
        return;

    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

    // Get mouse position
    CPoint pt;
    GetCursorPos( &pt );

    // Get client coordinates for hit test.
    CPoint ptClient( pt );
    ScreenToClient( &ptClient );

    // If the user did not click on an item, remove the properties item.
    int iPopup = VIEW_POPUP_ONITEM;
    if( ListCtrl.HitTest( ptClient ) == -1 )
        {
        iPopup = VIEW_POPUP;
        }

    // Load correct popup menu
    CMenu* pPopup = menu.GetSubMenu(iPopup);
    ASSERT( pPopup );

    // If we are on an item, set the properties item to BOLD
    if( iPopup == VIEW_POPUP_ONITEM )
        {
        MENUITEMINFO mif;
        ZeroMemory( &mif, sizeof( MENUITEMINFO ) );
        mif.cbSize = sizeof( MENUITEMINFO );
        mif.fMask = MIIM_STATE;
        GetMenuItemInfo( pPopup->m_hMenu,
                         0,
                         TRUE,
                         &mif
                        );

        // Set this item to the default state
        mif.fMask = MIIM_STATE;
        mif.fState |= MFS_DEFAULT;
        SetMenuItemInfo( pPopup->m_hMenu,
                         0,
                         TRUE,
                         &mif
                        );
        }

    // Fire off context menu
    pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                            pt.x,
                            pt.y,
                            AfxGetMainWnd() );

}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnUpdateSelectedItem
// Description	    : Enables a menu item if something is selected in the list
// Return type		: void
// Argument         : CCmdUI* pCmdUI
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnUpdateSelectedItem(CCmdUI* pCmdUI)
{
    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

	// Only enable if an item is selected.
    if( ListCtrl.GetSelectedCount() )
        {
        CQconsoleDoc* pDoc = GetDocument();
        ASSERT( pDoc );

        // Trial version only allows restore operations
        if( pDoc->IsTrialValid() == FALSE && pCmdUI->m_nID != ID_ACTION_RESTORE )
            {
            pCmdUI->Enable( FALSE );
            return;
            }

        // Make sure this operation is availiable to items in this mode
        int iViewMode = pDoc->GetViewMode();
        int iIndex;

        switch( iViewMode )
            {
            case VIEW_MODE_ALL:
                EnableForViewModeAll( pCmdUI );
                return;
            case VIEW_MODE_QUARANTINE:
                iIndex = QFILEOPS_INDEX_QUARANTINED;
                break;
            case VIEW_MODE_BACKUP:
                iIndex = QFILEOPS_INDEX_BACKUP_FILE;
                break;
            case VIEW_MODE_SUBMITTED:
                iIndex = QFILEOPS_INDEX_SUBMITTED;
                break;
            }

        // Get file option flags
        DWORD dwFileOps = 0;
        pDoc->m_pOpts->GetValidFileOperations( &dwFileOps, iIndex );

        // Enable control
        switch( pCmdUI->m_nID )
            {
            case ID_ACTION_REPAIR:
                pCmdUI->Enable( dwFileOps & QUAR_OPTS_FILEOP_REPAIR );
                break;
            case ID_ACTION_RESTORE:
                pCmdUI->Enable( dwFileOps & QUAR_OPTS_FILEOP_PUT_BACK );
                break;
            case ID_ACTION_DELETEITEM:
                pCmdUI->Enable( dwFileOps & QUAR_OPTS_FILEOP_DELETE );
                break;
            case ID_ACTION_SUBMITTOSARC:
                pCmdUI->Enable( dwFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC );
                break;
            }
        }
    else
        {
        pCmdUI->Enable( FALSE );
        ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( AFX_IDS_IDLEMESSAGE );
        }

}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::EnableForViewModeAll
//
// Description	    : This ugly function decides if a given control should
//                    be enabled based on the user options.
//
// Return type		: void 
//
// Argument         : CCmdUI* pCmdUI
//
////////////////////////////////////////////////////////////////////////////
// 5/7/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::EnableForViewModeAll(CCmdUI* pCmdUI)
{
    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

    // Get Document
    CQconsoleDoc* pDoc = GetDocument();
    ASSERT( pDoc );

    // How many elements are selected?
    int iCount = ListCtrl.GetSelectedCount();    

    // Get file option flags
    DWORD dwQuarFileOps;
    pDoc->m_pOpts->GetValidFileOperations( &dwQuarFileOps, QFILEOPS_INDEX_QUARANTINED );

    DWORD dwBackupFileOps;
    pDoc->m_pOpts->GetValidFileOperations( &dwBackupFileOps, QFILEOPS_INDEX_BACKUP_FILE );

    DWORD dwSubFileOps;
    pDoc->m_pOpts->GetValidFileOperations( &dwSubFileOps, QFILEOPS_INDEX_SUBMITTED );

    // Classify each item.  
    BOOL bEnable = TRUE;
    int iItem = -1;
    int iQuar = 0;
    int iSubmitted = 0;
    int iBackup = 0;

    while( (iItem = ListCtrl.GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED )) != -1 )
        {
        // Get item from list box extra data
        IQuarantineItem* p = (IQuarantineItem*) ListCtrl.GetItemData( iItem );
        ASSERT( p );

        // Get status for this item
        DWORD dwStatus;
        if( FAILED( p->GetFileStatus( &dwStatus ) ) )
            continue;

        // Keep track of how many of each item type we have.
        switch( dwStatus )
            {
            case QFILE_STATUS_QUARANTINED: iQuar ++; break;
            case QFILE_STATUS_SUBMITTED:   iSubmitted ++; break;
            case QFILE_STATUS_BACKUP_FILE: iBackup ++; break;
            }

        // Optimization.  If we already have one of each item, bail from loop.
        if( iQuar && iSubmitted && iBackup )
            break;
        }

    // Enable control based on user options.  This is ugly, but has to be
    // done. 
    switch( pCmdUI->m_nID )
        {
        case ID_ACTION_REPAIR:
            if( iQuar )
                bEnable = (( dwQuarFileOps & QUAR_OPTS_FILEOP_REPAIR ) > 0);
            if( iSubmitted )
                bEnable &= (( dwSubFileOps & QUAR_OPTS_FILEOP_REPAIR ) > 0);
            if( iBackup )            
                bEnable &= (( dwBackupFileOps & QUAR_OPTS_FILEOP_REPAIR ) > 0);
            break;

        case ID_ACTION_RESTORE:
            if( iQuar )
                bEnable = (( dwQuarFileOps & QUAR_OPTS_FILEOP_PUT_BACK ) > 0);
            if( iSubmitted )
                bEnable &= (( dwSubFileOps & QUAR_OPTS_FILEOP_PUT_BACK ) > 0);
            if( iBackup )            
                bEnable &= (( dwBackupFileOps & QUAR_OPTS_FILEOP_PUT_BACK ) > 0);
            break;

        case ID_ACTION_DELETEITEM:
            if( iQuar )
                bEnable = (( dwQuarFileOps & QUAR_OPTS_FILEOP_DELETE ) > 0);
            if( iSubmitted )
                bEnable &= (( dwSubFileOps & QUAR_OPTS_FILEOP_DELETE ) > 0);
            if( iBackup )            
                bEnable &= (( dwBackupFileOps & QUAR_OPTS_FILEOP_DELETE ) > 0);
            break;

        case ID_ACTION_SUBMITTOSARC:
            if( iQuar )
                bEnable = (( dwQuarFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC ) > 0);
            if( iSubmitted )
                bEnable &= (( dwSubFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC ) > 0);
            if( iBackup )            
                bEnable &= (( dwBackupFileOps & QUAR_OPTS_FILEOP_SUBMIT_TO_SARC ) > 0);
            break;
        }

    pCmdUI->Enable( bEnable );   

}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnUpdateProperties
// Description	    : Handles CMDUI for Properties action.  Only allow if a
//                    single item is selected.
// Return type		: void
// Argument         : CCmdUI* pCmdUI
//
////////////////////////////////////////////////////////////////////////////////
// 3/5/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnUpdateProperties(CCmdUI* pCmdUI)
{
    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

	// Only enable if a single item is selected.
    if( ListCtrl.GetSelectedCount() == 1 )
        pCmdUI->Enable( TRUE );
    else
        pCmdUI->Enable( FALSE );
 
}

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnActionRepair
// Description	    : Handles Action/Repair menu item
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionRepair()
{
    // Get list control
    CListCtrl &List = GetListCtrl();

    // Special case for corporate customers.
    if( GetDocument()->GetCorpMode() )
        {
        // Give a more corporate friendly message.
        CCorpRepairDlg dlg( this, TRUE );
        if( dlg.DoModal() != IDOK )
            return;
        }
    else
        {
        // Get count.
        int iItemCount = List.GetSelectedCount();

        // Build warning message.
        CString sMessage;
        if( iItemCount > 1 )
            {
            // Build repair warning string.
            sMessage.Format( IDS_MULTIPLE_REPAIR_WARNING, iItemCount );
            }
        else
            {
            sMessage.LoadString( IDS_REPAIR_WARNING );
            }

        // Warn user about what he is about to do.
        if( AfxMessageBox( sMessage, MB_ICONQUESTION | MB_YESNO ) != IDYES )
            return;
        }

    // Perform repair operation
    DoRepair( TRUE );

    // Recompute item totals
    GetDocument()->RecalcItemTypes();

    // Force a refresh of the tree pane.
    GetDocument()->UpdateAllViews( this, LISTVIEW_DELETE_ITEMS, NULL );

}



////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::OnActionDeleteitem
//
// Description	    : Handles Action/Delete menu item
//
// Return type		: void
//
//
////////////////////////////////////////////////////////////////////////////
// 3/6/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionDeleteitem()
{
    // Get list control
    CListCtrl &List = GetListCtrl();

    // Get count.
    int iItemCount = List.GetSelectedCount();

    // Build warning message.
    CString sMessage;
    if( iItemCount > 1 )
        {
        // Build delete warning string.
        sMessage.Format( IDS_MULTIPLE_DELETE_WARNING, iItemCount );
        }
    else
        {
        sMessage.LoadString( IDS_DELETE_WARNING );
        }

    // Warn user about what he is about to do.
    if( AfxMessageBox( sMessage, MB_ICONEXCLAMATION | MB_YESNO ) != IDYES )
        return;

    // Could take some time
    CWaitCursor wait;
    CQconsoleDoc* pDoc = GetDocument();
    IQuarantineItem* pItem;
    int iItem;

    // Disable drawing for now
    List.SetRedraw( FALSE );

    // Delete all selected items
    while( (iItem = List.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED )) != -1 )
        {
        // Get object from user data field.
        pItem = (IQuarantineItem*) List.GetItemData( iItem );
        ASSERT( pItem );

        // Delete the item in question.
        pItem->DeleteItem();

        // Delete the item from the list
        List.DeleteItem( iItem );

        // Delete the item from the array
        pDoc->DeleteItemFromLists( pItem );
        }

    // Turn redraw back on
    List.SetRedraw( TRUE );

    // Tell other views what happened.
    pDoc->UpdateAllViews( this, LISTVIEW_DELETE_ITEMS, NULL );

}




////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnActionProperties
// Description	    : Handles Action/Properties menu item
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionProperties()
{
    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

	// Get index of selected item.
    int iIndex = ListCtrl.GetNextItem( -1, LVNI_SELECTED );
    if( iIndex == -1 )
        return;

    // Get item.
    LV_ITEM item;
    TCHAR szBuffer[MAX_PATH];
    ZeroMemory( &item, sizeof( LV_ITEM ) );
    item.iItem = iIndex;
    item.pszText = szBuffer;
    item.cchTextMax = MAX_PATH;
    item.mask = LVIF_IMAGE | LVIF_TEXT;
    if( ListCtrl.GetItem( &item ) == FALSE )
        return;

    // Set up property sheet.
    CString s;
    s.Format( IDS_PROP_TITLE_FORMAT, item.pszText );
    CItemPropSheet dlg(s);
    dlg.m_psh.dwFlags |= PSH_NOAPPLYNOW;

    // Fire off dialog.
    dlg.DisplayProperties( (IQuarantineItem*) ListCtrl.GetItemData( iIndex ),
                           ListCtrl.GetImageList( LVSIL_NORMAL ),
                           item.iImage );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnActionRestore
// Description	    : Handle Action/Restore menu item
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionRestore()
{
    // Get list control
    CListCtrl &List = GetListCtrl();

    // Get count.
    int iItemCount = List.GetSelectedCount();

    // Special case for corporate customers.
    if( GetDocument()->GetCorpMode() )
        {
        // Give a more corporate friendly message.
        CCorpRepairDlg dlg;
        if( dlg.DoModal() != IDOK )
            return;
        }
    else
        {
        // Build warning message.
        CString sMessage;
        if( iItemCount > 1 )
            {
            // Build repair warning string.
            sMessage.Format( IDS_MULTIPLE_RESTORE_WARNING, iItemCount );
            }
        else
            {
            sMessage.LoadString( IDS_RESTORE_WARNING );
            }

        // Warn user about what he is about to do.
        if( AfxMessageBox( sMessage, MB_ICONEXCLAMATION | MB_YESNO ) != IDYES )
            return;
        }
    

    // Perform restore operation
    DoRepair( FALSE );

    // Tell other views what happened.
    GetDocument()->UpdateAllViews( this, LISTVIEW_DELETE_ITEMS, NULL );
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::DoRepair
// Description	    : Performs repair and restore operations
// Return type		: void
// Argument         : BOOL bRepair - TRUE if repair is to be done,
//                                   FALSE if only restore is required
//
////////////////////////////////////////////////////////////////////////////////
// 3/12/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::DoRepair( BOOL bRepair )
{
    // Get list control
    CListCtrl &List = GetListCtrl();

    // Create progress dialog
    CProgressDlg progress;
    progress.Create();
    progress.SetRange(0, List.GetSelectedCount() );

    // Get document
    CQconsoleDoc* pDoc = GetDocument();

    int iItem = -1;
    int iCounter = 0;
    LV_ITEM listItem;
    IQuarantineItem* pItem;
    CResultsListItem* pResultItem;

    // Setup status dialog.
    CResultsDlg dlg;
    dlg.SetImageList( GetListCtrl().GetImageList( LVSIL_SMALL ) );
    if( bRepair == FALSE )
        dlg.SetCaption( IDS_RESTORE_RESULTS );

    // Restore all selected items
    while( (iItem = List.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED )) != -1 )
        {
        // Setup new results item.
        pResultItem = new CResultsListItem;

        // Get data for this object
        ZeroMemory( &listItem, sizeof( LV_ITEM ) );
        listItem.iItem = iItem;
        listItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;
        listItem.pszText = pResultItem->m_sItemName.GetBuffer( MAX_PATH );
        listItem.cchTextMax = MAX_PATH;

        // Get item data
        List.GetItem( &listItem );

        // de-select this item.
        List.SetItemState( iItem, ~LVIS_SELECTED, LVIS_SELECTED );

        // Release buffer memory
        pResultItem->m_sItemName.ReleaseBuffer();
        pResultItem->m_iListItem = iItem;

        // Get object from user data field.
        pItem = (IQuarantineItem*) listItem.lParam;
        ASSERT( pItem );

        // Save off icon index
        pResultItem->m_iIconIndex = listItem.iImage;

        // Add to list
        dlg.m_aItemList.AddTail( pResultItem );

        // Attempt repair / restore operation.
        int iResult = bRepair ? RepairItem( pItem, pResultItem ) :
            RestoreItem( pItem, pResultItem );

        if( iResult == -1 || progress.CheckCancelButton() )
            return;

        // If we have repaired this item, we want it move to a
        // different view.
        int iViewMode = pDoc->GetViewMode();
        if( bRepair && pResultItem->m_Action.GetAction() == RESULTS_ACTION_REPAIRED && 
            iViewMode != VIEW_MODE_ALL && iViewMode != VIEW_MODE_BACKUP )
            {
            List.DeleteItem( iItem );
            iItem = -1;
            }

        // Advance progress dialog
        progress.SetPos( ++iCounter );
        }

    // Remove progress window
    progress.DestroyWindow();

    // Fire off results dialog.
    dlg.DoModal();

}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::RestoreItem
//
// Description	    : Attempt to restore an item from quarantine.
//
// Return type		: int 0 to continue, -1 to cancel
//
// Argument         : IQuarantineItem* pItem
// Argument         : CResultsListItem* pResult
//
////////////////////////////////////////////////////////////////////////////
// 3/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CQconsoleView::RestoreItem( IQuarantineItem* pItem, CResultsListItem* pResult )
{
    // Get full original file name
    CString sOriginalFileName;
    LPTSTR p = sOriginalFileName.GetBuffer(MAX_PATH);
    pItem->GetOriginalAnsiFilename( p, MAX_PATH );
    sOriginalFileName.ReleaseBuffer();

    // Get location
    CString sFilePath;
    p = sFilePath.GetBuffer(MAX_PATH);
    NameReturnPath( sOriginalFileName, p );
    sFilePath.ReleaseBuffer();

    // Get filename
    CString sFileName;
    p = sFileName.GetBuffer(MAX_PATH);
    NameReturnFile( sOriginalFileName, p );
    sFileName.ReleaseBuffer();

    // If running on a server machine, prompt for creation of 
    // directory named after the user whom the infected item belongs.
    if( GetDocument()->GetCorpMode() )
        {
        if( CreateItemUserDirectory( pItem, sFilePath, sOriginalFileName, sFileName ) == FALSE )
            {
            pResult->m_Action.SetAction( RESULTS_ACTION_NO_ACTION, NULL );
            return -1;
            }
        }
    else
        {
        // Get save location.
        if( GetSaveLocation( sFilePath, sOriginalFileName, sFileName ) == FALSE )
            {
            pResult->m_Action.SetAction( RESULTS_ACTION_NO_ACTION, NULL );
            return -1;
            }
        }


    // Restore this item.
    HRESULT hr = pItem->UnpackageItem( sFilePath.GetBuffer(0), sFileName.GetBuffer(0), NULL, 0, TRUE, FALSE );
    if( SUCCEEDED( hr ) )
        {
        // Set action
        pResult->m_Action.SetAction( RESULTS_ACTION_RESTORED, sOriginalFileName );

        // Get list control
        CListCtrl &List = GetListCtrl();

        // Remove item in question from list
        List.DeleteItem( pResult->m_iListItem );

        // Delete the item in question.
        pItem->DeleteItem();

        // Delete the item from the array
        GetDocument()->DeleteItemFromLists( pItem );
        }
    else
        {
        CString s;    
        
        // Display error message.
        switch( hr )
            {
            case E_FILE_WRITE_FAILED:
                s.Format( IDS_ERROR_DISK_SPACE, sFilePath[0]);
                AfxMessageBox( s, MB_ICONSTOP | MB_OK );
                return -1;
        
            case E_FILE_CREATE_FAILED:
                s.Format( IDS_FILE_IN_USE, sOriginalFileName ); 
                AfxMessageBox( s, MB_ICONSTOP | MB_OK );
                return -1;
            }
        }

    // Success
    return 0;
}

////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::RepairItem
//
// Description	    : Attempt to repair and restore an item from quarantine.
//
// Return type		: int 0 to continue, -1 to cancel
//
// Argument         : IQuarantineItem* pItem
// Argument         : CResultsListItem* pResult
//
////////////////////////////////////////////////////////////////////////////
// 3/11/98 DBUCHES - Function created / Header added.
// 6/26/98 JTAYLOR - Restore the original file status if the repair fails.
// 6/26/98 JTAYLOR - Added a delete for the temporary file if repair fails.
////////////////////////////////////////////////////////////////////////////
int CQconsoleView::RepairItem( IQuarantineItem* pItem, CResultsListItem* pResult )
{

    CString sFileName;

    // Get full original file name
    CString sOriginalFileName;
    LPTSTR p = sOriginalFileName.GetBuffer(MAX_PATH);
    pItem->GetOriginalAnsiFilename( p, MAX_PATH );
    sOriginalFileName.ReleaseBuffer();

    // save the original file status
    DWORD dwFileStatus = 0;
    pItem->GetFileStatus( &dwFileStatus );

    // Attempt repair of this item.
    CString sRepairedFile;
    HRESULT hr = pItem->RepairItem( GetDocument()->m_pQuarantine,
        sRepairedFile.GetBuffer( MAX_PATH ),
        MAX_PATH );
    sRepairedFile.ReleaseBuffer();

    // bugbug - need to handle repair of un-infected item.
    CString s;

    // If the repair did not succeed, then restore the file status
    if( S_OK != hr )
        {
        pItem->SetFileStatus( dwFileStatus );
        pItem->SaveItem( );
        }

    // Q: did we repair it?
    switch( hr )
        {
        case S_OK:
            break;

        case E_FILE_CREATE_FAILED:
            s.Format( IDS_FILE_IN_USE, sOriginalFileName ); 
            AfxMessageBox( s, MB_ICONSTOP | MB_OK );
            return -1;

        case E_FILE_WRITE_FAILED:
            {
            TCHAR szFolder[MAX_PATH];
            GetDocument()->m_pQuarantine->GetQuarantineFolder( szFolder, MAX_PATH );
            s.Format( IDS_ERROR_DISK_SPACE, szFolder[0]);
            AfxMessageBox( s, MB_ICONSTOP | MB_OK );
            return -1;
            }

        default:
            pResult->m_Action.SetAction( RESULTS_ACTION_STILL_INFECTED, sOriginalFileName );
            return 0;
        }

    // Get location
    CString sFilePath;
    p = sFilePath.GetBuffer(MAX_PATH);
    NameReturnPath( sOriginalFileName, p );
    sFilePath.ReleaseBuffer();

    // If running on a server machine, prompt for creation of 
    // directory named after the user whom the infected item belongs.
    if( GetDocument()->GetCorpMode() )
        {
        if( CreateItemUserDirectory( pItem, sFilePath, sOriginalFileName, sFileName ) == FALSE )
            {
            pResult->m_Action.SetAction( RESULTS_ACTION_NO_ACTION, NULL );
            pItem->SetFileStatus( dwFileStatus );
            pItem->SaveItem( );

            // Delete repaired file.
            ::DeleteFile( sRepairedFile );
            
            return -1;
            }
        }
    else
        {
        // Get save location.
        if( GetSaveLocation( sFilePath, sOriginalFileName, sFileName ) == FALSE )
            {
            pResult->m_Action.SetAction( RESULTS_ACTION_NO_ACTION, NULL );
            pItem->SetFileStatus( dwFileStatus );
            pItem->SaveItem( );

            // Delete repaired file.
            ::DeleteFile( sRepairedFile );
            
            return -1;
            }
        }

    // Restore this item.
    if( ::CopyFile( sRepairedFile, sOriginalFileName, FALSE ) )
        {
        // Get security descriptor for repaired file
        LPBYTE  securityDesc = NULL;
        if( SUCCEEDED( GetFileSecurityDesc( sRepairedFile.GetBuffer(0), &securityDesc ) ) ) 
            {
            // Set new file security.  Ignore result.
            SetFileSecurityDesc( sOriginalFileName.GetBuffer(0), securityDesc );
        
            // Cleanup
            if( securityDesc )
                {
                delete[] securityDesc;
                }
            }

        // Set action
        pResult->m_Action.SetAction( RESULTS_ACTION_REPAIRED, sOriginalFileName );
        }
    else
        {
        // Display error message to user
        auto    LPVOID      lpMsgBuf;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                                            , NULL
                                            , GetLastError()
                                            , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
                                            , (LPTSTR) &lpMsgBuf
                                            , 0
                                            , NULL );
        AfxMessageBox( (CHAR *)lpMsgBuf, MB_OK| MB_ICONSTOP );
        LocalFree( lpMsgBuf );

        ASSERT(FALSE);
        }


    // Delete repaired file.
    ::DeleteFile( sRepairedFile );

    return 0;

}


////////////////////////////////////////////////////////////////////////////
// Function name	: CreateItemUserDirectory
//
// Description	    : 
//
// Return type		: BOOL 
//
// Argument         : IQuarantineItem* pItem - item in question
// Argument         : CString& sDestFileName - reference to final locaiton of file
//
////////////////////////////////////////////////////////////////////////////
// 5/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleView::CreateItemUserDirectory( IQuarantineItem* pItem, CString& sFilePath, CString& sDestFileName, CString& sFileName )
{
    // Get user name.
    CString sUserName;
    pItem->GetOriginalOwnerName( sUserName.GetBuffer(MAX_PATH), MAX_PATH );
    sUserName.ReleaseBuffer();

    // Get filename
    LPTSTR p = sFileName.GetBuffer(MAX_PATH);
    NameReturnFile( sDestFileName, p );
    sFileName.ReleaseBuffer();
    
    // Build save path.
    sFilePath = GetDocument()->GetRepairFolder();
    sFilePath += _T("\\");
    sFilePath += sUserName;

    sDestFileName = sFilePath;
    sDestFileName += _T("\\");
    sDestFileName += sFileName;

    // Attempt to create directory.
    DWORD dwAttr = GetFileAttributes( sFilePath );
    if( dwAttr == 0xFFFFFFFF || ! (dwAttr & FILE_ATTRIBUTE_DIRECTORY) )
        {
        if( ::CreateDirectory( sFilePath, NULL ) == FALSE )
            {
            // If we can't create the directory, prompt user for save location
            return GetSaveLocation( GetDocument()->GetRepairFolder(), sDestFileName, sFileName );
            }
        }

    // Try saving here.
    return GetSaveLocation( sFilePath, sDestFileName, sFileName );
}

////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleview::GetSaveLocation
//
// Description	    : Prompts user for save location
//
// Return type		: BOOL 
//
// Argument         : CString& sFilePath
// Argument         : CString& sOriginalFileName
// Argument         : CString& sFileName
//
////////////////////////////////////////////////////////////////////////////
// 5/13/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleView::GetSaveLocation( CString& sFilePath, 
                                     CString& sOriginalFileName, 
                                     CString& sFileName )
{
    // Get filename
    LPTSTR p;
    p = sFileName.GetBuffer(MAX_PATH);
    NameReturnFile( sOriginalFileName, p );
    sFileName.ReleaseBuffer();

    // Make sure original directory actually exists.  Allways prompt for destination
    // folder if in corporate mode
    DWORD dwAttr = GetFileAttributes( sFilePath );
    if( dwAttr == 0xFFFFFFFF || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY) )
        
        {
        // Ask user where file should be saved
        CString sFilter((LPCTSTR) IDS_ADD_FILTER_STRING);
        CFileDialog dlg( FALSE,                 // Save
                         NULL,                  // No default extension
                         sFileName,             // Initial file name
                         OFN_FILEMUSTEXIST |    // Flags
                         OFN_HIDEREADONLY |
                         OFN_PATHMUSTEXIST,
                         sFilter,               // Filter string
                         this );                // Parent window

        // Fire off dialog
        if( dlg.DoModal() == IDCANCEL )
            {
            return FALSE;
            }

        // Save off file name and path
        sFileName = dlg.GetFileName();
        sOriginalFileName = dlg.GetPathName();
        p = sFilePath.GetBuffer(MAX_PATH);
        NameReturnPath( sOriginalFileName, p );
        sFilePath.ReleaseBuffer();
        }

    // If the file already exists, ask the user if he wants to
    // overwrite the file.
    dwAttr = GetFileAttributes( sOriginalFileName );
    while( dwAttr != 0xFFFFFFFF )
        {
        // Prompt the user for overwrite
        CString sTitle, sText;
        sTitle.LoadString( IDS_RESTORE_OVERWRITE_TITLE );
        sText.Format( IDS_RESTORE_OVERWRITE_FORMAT, sOriginalFileName );
        if( MessageBox( sText, sTitle, MB_ICONWARNING | MB_YESNO ) != IDYES )
            {
            CString sFilter((LPCTSTR) IDS_ADD_FILTER_STRING);
            CFileDialog dlg( FALSE,                 // Save
                             NULL,                  // No default extension
                             sOriginalFileName,     // Initial file name
                             OFN_FILEMUSTEXIST |    // Flags
                             OFN_HIDEREADONLY |
                             OFN_PATHMUSTEXIST,
                             sFilter,               // Filter string
                             this );                // Parent window

            // Fire off dialog
            if( dlg.DoModal() == IDCANCEL )
                {
                return FALSE;
                }

            // Save off file name and path
            sFileName = dlg.GetFileName();
            sOriginalFileName = dlg.GetPathName();
            p = sFilePath.GetBuffer(MAX_PATH);
            NameReturnPath( sOriginalFileName, p );
            sFilePath.ReleaseBuffer();
            }
        else
            {
            // User wants to overwrite this file, so delete it first.
            ::DeleteFile( sOriginalFileName );
            break;
            }

        // Get file attributes of this file
        dwAttr = GetFileAttributes( sOriginalFileName );
        }

    return TRUE;

}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::HandleRefresh
// Description	    : Handles Refresh hint
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/15/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::HandleRefresh()
{
    // Simply repopulate the list, forcing a rebuild of the
    // main list.
    PopulateListCtrl();
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::PopulateListCtrl
// Description	    : Populates the list control
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::PopulateListCtrl( BOOL bRebuild /* = FALSE */)
{
    // This could take a while.
    CWaitCursor wait;

    // Get list control
    CListCtrl & ListCtrl = GetListCtrl();

    // Get pointer to document
    CQconsoleDoc* pDoc = (CQconsoleDoc*) GetDocument();
    ASSERT( pDoc );

    // Turn off redraw and delete all items from list
    ListCtrl.SetRedraw(FALSE);
    ListCtrl.DeleteAllItems();

    // Rebuild all item arrays
    if( bRebuild )
        pDoc->RebuildItemArrays();

    // Get currently selected array.
    CMapPtrToPtr *pList;
    pList = pDoc->GetItemList( );

    // Enum the list and add our items.
    POSITION pos;
    LPVOID p1, p2;
    for( pos = pList->GetStartPosition(); pos != NULL; )
        {
        // Get item to add to list
        pList->GetNextAssoc( pos, p1, p2 );
        ASSERT( p2 );

        // Set up item to insert.
        LV_ITEM item;
        ZeroMemory( &item, sizeof( LV_ITEM ) );
        item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
        item.pszText = LPSTR_TEXTCALLBACK;
        item.iImage = I_IMAGECALLBACK;
        item.lParam = (LPARAM) p2;

        // Insert this record into the list
        ListCtrl.InsertItem( &item );
        }

    // Sort items in list
    if( m_bSortAscending )
        ListCtrl.SortItems( CQconsoleView::SortAscendCompareFunc, (DWORD) this );
    else
        ListCtrl.SortItems( CQconsoleView::SortDescendCompareFunc, (DWORD) this );
    
    // Turn drawing back on and force a repaint
    ListCtrl.SetRedraw(TRUE);
    ListCtrl.UpdateWindow();

    // Select first item in the list
    ListCtrl.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED );

}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnLButtonDblClk
// Description	    : Handles left button double click
// Return type		: void
// Argument         : UINT nFlags
// Argument         : CPoint point
//
////////////////////////////////////////////////////////////////////////////////
// 2/16/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // Call base class.
	CListView::OnLButtonDblClk(nFlags, point);

    // Get reference to list control
    CListCtrl & ListCtrl = GetListCtrl();

    // If the user did not click on an item, bail out
    if( ListCtrl.HitTest( point ) == -1 )
        return;

    // Call properties handler
    OnActionProperties();

}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::IsExecutable
// Description	    : Returns TRUE if this file is an executable type (.EXE or .COM )
// Return type		: BOOL
//
////////////////////////////////////////////////////////////////////////////////
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CQconsoleView::IsExecutable(CString& sFileName)
{
    CString s(sFileName);
    s.MakeUpper();

    if( s.Right( 4 ) == _T(".EXE") ||
        s.Right( 4 ) == _T(".COM") )
        return TRUE;

    return FALSE;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnActionSubmittosarc
// Description	    : Handles Action/Submit menu item
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnActionSubmittosarc()
{
    // Get pointer to list control
    CListCtrl &List = GetListCtrl();

    // Allocate array of item pointers.
    IQuarantineItem ** pItems;
    pItems = new IQuarantineItem*[List.GetSelectedCount()];

    int iItem = -1;
    int iCounter = 0;

    // Get all selected items
    while( (iItem = List.GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED )) != -1 )
        {
        // Get object from user data field.
        pItems[iCounter++] = (IQuarantineItem*) List.GetItemData( iItem );
        }

    // Initialize the Scan and Deliver interface
    HRESULT hr;

    IScanDeliverDLL* pScanDeliver;
    hr = CoCreateInstance( CLSID_ScanDeliverDLL,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_ScanDeliverDLL,
                           (LPVOID*)&pScanDeliver);

    if( FAILED( hr ) )
        {
        AfxMessageBox( IDS_ERROR_STARTING_SCANANDDELIVER, MB_ICONSTOP | MB_OK );
        }
    else
        {
        // Deliver files to SARC
        hr = pScanDeliver->DeliverFilesToSarc( pItems, List.GetSelectedCount() );
        
        if( E_UNABLE_TO_INITIALIZE == hr )
            AfxMessageBox( IDS_ERROR_STARTING_SCANANDDELIVER, MB_ICONSTOP | MB_OK );

        pScanDeliver->Release();
        }


    // Cleanup.
    delete [] pItems;

    // tell everyone what happened.
    GetDocument()->UpdateAllViews( NULL, LISTVIEW_REFRESH, NULL );

}

#define TRIANGLE_PADDING 20

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnDrawItem
// Description	    : Handles WM_DRAWITEM messages.
// Return type		: void
// Argument         : int nIDCtl
// Argument         : LPDRAWITEMSTRUCT lpDrawItemStruct
//
////////////////////////////////////////////////////////////////////////////////
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
    if( lpDrawItemStruct->CtlType != ODT_HEADER )
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
    CHeaderCtrl *pHdr = (CHeaderCtrl*) CWnd::FromHandle( lpDrawItemStruct->hwndItem );
    pHdr->GetClientRect( &WindowRect );
    pHdr->GetItem(lpDrawItemStruct->itemID, &item );

    // Get rectangle for this item
    CRect rect( lpDrawItemStruct->rcItem );

    // Set up DC
    CDCOS dc( CDC::FromHandle( lpDrawItemStruct->hDC ), WindowRect, rect );

    // Fill rect.
    CBrush brush( GetSysColor( COLOR_BTNFACE ) );
    dc.FillRect( &rect, &brush );

    // Set up DC
    CFont *pOldFont = dc.SelectObject( pHdr->GetFont() );
    COLORREF oldBkColor = dc.SetBkColor( GetSysColor( COLOR_BTNFACE ) );
    COLORREF oldTextColor = dc.SetTextColor( GetSysColor( COLOR_BTNTEXT ) );

    // Compute size of text
    CString s(szBuffer);
    CSize size = dc.GetOutputTextExtent( s );

    // Compute text rectangle
    TEXTMETRIC tm;
    dc.GetTextMetrics( &tm );

    CRect textRect;
    CRect sizeRect;
    int iOffset = (lpDrawItemStruct->itemState & ODS_SELECTED) ? 1 : 0;

    textRect.right = rect.right - TRIANGLE_PADDING;
    textRect.left = rect.left + tm.tmAveCharWidth + iOffset;
    textRect.top = rect.top + (rect.Height() - size.cy) / 2 + iOffset;
    textRect.bottom = rect.bottom;
    dc.DrawText( szBuffer, &sizeRect, DT_LEFT | DT_CALCRECT );

    // Draw text to screen.
    dc.DrawText( szBuffer, &textRect, DT_LEFT | DT_END_ELLIPSIS  );

    // Get top of triangle.
    int iTop = ( rect.Height() - 7 ) / 2 + iOffset;
    int iLeft;

    // Compute correct X-coordinate for triangle
    if( sizeRect.Width()  + tm.tmAveCharWidth + iOffset< textRect.Width())
        iLeft = rect.left + sizeRect.Width() + 15 + iOffset;
    else
        iLeft = rect.right - 15 + iOffset;

    // Draw search direction triangle.  Only draw if we have room.
    if( GetDocument()->GetSortCol() == (int)lpDrawItemStruct->itemID &&
        iLeft >= textRect.left )
        {
        DrawArrow( &dc, iLeft, iTop, m_bSortAscending );
        }

    // Cleanup
    dc.SelectObject( pOldFont );
    dc.SetTextColor( oldTextColor );
    dc.SetBkColor( oldBkColor );

}



////////////////////////////////////////////////////////////////////////////
// Function name	: CJobTargetView::DrawArrow
//
// Description	    :
//
// Return type		: void
//
// Argument         :  CDC* pDC
// Argument         : int x
// Argument         : int y
// Argument         : BOOL bUp
//
////////////////////////////////////////////////////////////////////////////
// 12/1/97 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::DrawArrow( CDC* pDC, int x, int y, BOOL bUp)
{
    // Create pens needed
    CPen whitePen;
    whitePen.CreateStockObject( WHITE_PEN );
    CPen grayPen( PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));

    int iTop = y;
    int iBottom = iTop + 6;


    CPen * oldPen = pDC->SelectObject( &grayPen );
    if( bUp )
        {
        // Draw bottom line
        pDC->SelectObject( &whitePen );
        pDC->MoveTo( x, iBottom );
        pDC->LineTo( x + 7, iBottom );

        // Draw left diagonal
        pDC->SelectObject( &grayPen );
        pDC->MoveTo( x + 3, iTop );
        pDC->LineTo( x , iBottom );

        // Draw right diagonal
        pDC->MoveTo( x + 4, iTop );
        pDC->SelectObject( &whitePen );
        pDC->LineTo( x + 8, iBottom +1);
        }
    else
        {
        // Draws downward pointing triangle

        // Draw top line
        pDC->MoveTo( x, iTop );
        pDC->LineTo( x + 7, iTop );

        // Draw left diagonal
        pDC->MoveTo( x + 3, iBottom );
        pDC->LineTo( x , iTop );

        // Draw right diagonal
        pDC->MoveTo( x + 4, iBottom );
        pDC->SelectObject( &whitePen );
        pDC->LineTo( x + 8, iTop -1);
        }

    // Cleanup
    pDC->SelectObject( oldPen );
}




////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnColumnclick
// Description	    :
// Return type		: void
// Argument         : NMHDR* pNMHDR
// Argument         : LRESULT* pResult
//
////////////////////////////////////////////////////////////////////////////////
// 2/20/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // Get pointer to list control
    CListCtrl &List = GetListCtrl();
    CWaitCursor wait;

    // If we've recently clicked this guy, reverse sort order.
    if( pNMListView->iSubItem == GetDocument()->GetSortCol() )
        {
        m_bSortAscending = !m_bSortAscending;
        }
    else
        {
        // Save off last clicked column
        GetDocument()->SetSortCol( pNMListView->iSubItem );
        m_bSortAscending = TRUE;
        }

    // Sort item in list
    if( m_bSortAscending )
        List.SortItems( CQconsoleView::SortAscendCompareFunc, (DWORD) this );
    else
        List.SortItems( CQconsoleView::SortDescendCompareFunc, (DWORD) this );

	*pResult = 0;
}

////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::SaveColumnWidths
//
// Description	    : Saves off column widths
//
// Return type		: void
//
//
////////////////////////////////////////////////////////////////////////////
// 1/19/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::SaveColumnWidths()
{
    // Get list control
    CListCtrl &List = GetListCtrl();

	// If we are in report mode, save off list view columns
    if( (List.GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
        return;

    // Build string
    CString sWidths;
    sWidths.Format( _T("%d;%d;%d;%d;%d;%d;%d;"),
                    List.GetColumnWidth( COLUMN_FILE_NAME  ),
                    List.GetColumnWidth( COLUMN_LOCATION   ),
                    List.GetColumnWidth( COLUMN_QDATE      ),
                    List.GetColumnWidth( COLUMN_SDATE ),
                    List.GetColumnWidth( COLUMN_STATUS ),
                    //List.GetColumnWidth( COLUMN_TYPE   ),
                    List.GetColumnWidth( COLUMN_VIRUS_NAME   )
                     );


    // Save to registry
    AfxGetApp()->WriteProfileString( REGKEY_UI_SETTINGS,
                                REGKEY_VALUE_COLUMNS,
                                sWidths );

}

////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::GetColumnWidths
//
// Description	    : Retrieve the column widths from the registry
//
// Return type      : void
//
////////////////////////////////////////////////////////////////////////////
// 1/19/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetColumnWidths(int *pColumns, int nCount)
{
    // Get string from registry.
    CString sWidths = AfxGetApp()->GetProfileString( REGKEY_UI_SETTINGS,
                                REGKEY_VALUE_COLUMNS,
                                DEFAULT_COLUMN_WIDTHS );

    // Get the correct value from the string.
    CString s;
    for( int i = 0; i < nCount; i++ )
        {
        // Search for ;
        int iPos = sWidths.Find( _T(';') );
        if( iPos == -1 )
            break;

        // Number is left of ;
        s = sWidths.Left( iPos );
        pColumns[i] = atol( s );

        // Move to next chunk 'o string
        sWidths = sWidths.Right( sWidths.GetLength() - iPos -1);
        }
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::OnDestroy
// Description	    : Handles WM_DESTROY message.
// Return type		: void
//
////////////////////////////////////////////////////////////////////////////////
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
//
// Function name	: CQconsoleView::OnSize
// Description	    : Handles WM_SIZE message
// Return type		: void
// Argument         : UINT nType
// Argument         : int cx
// Argument         : int cy
//
////////////////////////////////////////////////////////////////////////////////
// 2/26/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnSize(UINT nType, int cx, int cy)
{
	// Call bass class
    CListView::OnSize(nType, cx, cy);

    // Get list control
    CListCtrl &List = GetListCtrl();

    // Make sure items stay arranged.
    List.Arrange( LVA_DEFAULT );


}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::OnGetdispinfo
//
// Description	    : Handles LVN_GETDISPINFO notification message
//
// Return type		: void
//
// Argument         : NMHDR* pNMHDR
// Argument         : LRESULT* pResult
//
////////////////////////////////////////////////////////////////////////////
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

    // Get the text for this item
    if( pDispInfo->item.mask & LVIF_TEXT )
        {
        GetItemText( &pDispInfo->item );
        }

    // Get the image for this item.
    if( pDispInfo->item.mask & LVIF_IMAGE )
        {
        GetItemImage( &pDispInfo->item );
        }

    *pResult = 0;

}



////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::GetItemImage
//
// Description	    : This routine returns the correct image for a given item
//
// Return type		: void
//
// Argument         : LV_ITEM * pItem - the iImage member of this struct
//                              recieves the image index.
//
////////////////////////////////////////////////////////////////////////////
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetItemImage(LV_ITEM * pItem)
{
    CString s;
    IQuarantineItem* p;
    CString sRealFileName;

    // Get pointer to document
    CQconsoleDoc* pDoc = (CQconsoleDoc*) GetDocument();
    ASSERT( pDoc );

    // Figure out which style icon we need.
    UINT uIconFlag = ( GetListCtrl().GetStyle() & LVS_ICON ) ? SHGFI_LARGEICON : SHGFI_SMALLICON;


    // Get this item
    p = (IQuarantineItem*) pItem->lParam;
    ASSERT( p );

    // Get the filename and add it to our path
    p->GetCurrentFilename( sRealFileName.GetBuffer(MAX_PATH), MAX_PATH );
    sRealFileName.ReleaseBuffer();

    // If this is an executable file, just return the icon index
    // we know and love for generic executables.
    if( IsExecutable( sRealFileName ) )
        {
        pItem->iImage = (uIconFlag == SHGFI_LARGEICON) ? m_iLargeExeImage : m_iSmallExeImage;
        return;
        }

    // Start with quarantine folder
    s = pDoc->GetQuarantineFolder();
    s += _T( '\\' );
    s += sRealFileName;

    // Get icon for this file
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    SHGetFileInfo( s,
                   0,
                   &sfi,
                   sizeof( SHFILEINFO ),
                   SHGFI_SYSICONINDEX | uIconFlag
                   );

    // Save off icon index.
    pItem->iImage = sfi.iIcon;
}



////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::GetItemText
//
// Description	    : This routine returns the text for a given sub-item.
//
// Return type		: void
//
// Argument         : LV_ITEM * pItem - the pszText and cchTextMax fields
//                              are used as the location and size of the
//                              text.
//
////////////////////////////////////////////////////////////////////////////
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::GetItemText(LV_ITEM * pItem)
{
    // Get this item
    IQuarantineItem* p;
    p = (IQuarantineItem*) pItem->lParam;
    ASSERT( p );

    switch( pItem->iSubItem )
        {
        case COLUMN_FILE_NAME:
            p->GetOriginalAnsiFilename( pItem->pszText, pItem->cchTextMax );
            NameReturnFile( pItem->pszText, pItem->pszText );
            break;

        case COLUMN_LOCATION:
            if( GetDocument()->GetCorpMode() )
                {
                p->GetOriginalOwnerName( pItem->pszText, pItem->cchTextMax );
                }
            else
                {
                p->GetOriginalAnsiFilename( pItem->pszText, pItem->cchTextMax );
                NameReturnPath( pItem->pszText, pItem->pszText );
                }
            break;

        case COLUMN_QDATE:
            {
            TCHAR szTime[30];
            SYSTEMTIME tm;
            p->GetDateQuarantined( &tm );

            // Convert to local time
            FILETIME   ft, lft;
            SystemTimeToFileTime( &tm, &ft );
            FileTimeToLocalFileTime( &ft, &lft );
            FileTimeToSystemTime( &lft, &tm );

            GetDateFormat( NULL,
                           DATE_SHORTDATE,//LONGDATE,
                           &tm,
                           NULL,
                           pItem->pszText,
                           pItem->cchTextMax );

            GetTimeFormat( NULL,
                           0,
                           &tm,
                           NULL,
                           szTime,
                           30 );

            lstrcat( pItem->pszText, _T(" ") );
            lstrcat( pItem->pszText, szTime );
            }
            break;

        case COLUMN_SDATE:
            {
            TCHAR szTime[30];
            SYSTEMTIME tm;
            p->GetDateSubmittedToSARC( &tm );
            
            // Check to make sure this was actually submitted.
            if( tm.wYear == 0 )
                {
                CString s;
                s.LoadString( IDS_NOT_SUBMITTED );
                lstrcpyn( pItem->pszText, s, pItem->cchTextMax );
                break;
                }
            else
                {
                // Convert to local time
                FILETIME   ft, lft;
                SystemTimeToFileTime( &tm, &ft );
                FileTimeToLocalFileTime( &ft, &lft );
                FileTimeToSystemTime( &lft, &tm );
                }

            GetDateFormat( NULL,
                           DATE_SHORTDATE,//LONGDATE,
                           &tm,
                           NULL,
                           pItem->pszText,
                           pItem->cchTextMax );

            GetTimeFormat( NULL,
                           0,
                           &tm,
                           NULL,
                           szTime,
                           30 );

            lstrcat( pItem->pszText, _T(" ") );
            lstrcat( pItem->pszText, szTime );
            }
            break;

        case COLUMN_STATUS:
            {
            // Get status text
            CString s;
            int iStrID;
            DWORD dwStatus;
            if( FAILED( p->GetFileStatus( &dwStatus ) ) )
                return;

            switch( dwStatus )
                {
                case QFILE_STATUS_QUARANTINED: iStrID = IDS_STATUS_QUARANTINED; break;
                case QFILE_STATUS_SUBMITTED:   iStrID = IDS_STATUS_SUBMITTED;   break;
                case QFILE_STATUS_BACKUP_FILE: iStrID = IDS_STATUS_BACKUP;      break;
                }

            // Load and copy string
            s.LoadString( iStrID );
            lstrcpyn( pItem->pszText, s, pItem->cchTextMax );
            }
            break;

        case COLUMN_VIRUS_NAME:
            {
            // Get virus info struct from item.
            N30* pVirInfo;
            p->GetN30StructPointer( &pVirInfo );
            ASSERT( pVirInfo );

            // Get virus name from struct.
            if( lstrlen( pVirInfo->lpVName ) )
                lstrcpyn( pItem->pszText, pVirInfo->lpVName, pItem->cchTextMax );
            else
                {
                // If no Virus, display "Unknown"
                CString s;
                s.LoadString( IDS_UNKNOWN );
                lstrcpyn( pItem->pszText, s, pItem->cchTextMax );
                }
            }
            break;
        }

}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::SortAscendCompareFunc
// Description	    : Sort ascending sort callback function
// Return type		: int
// Argument         : LPARAM lParam1
// Argument         : LPARAM lParam2
// Argument         : LPARAM lParamSort
//
////////////////////////////////////////////////////////////////////////////////
// 3/5/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
int CQconsoleView::SortAscendCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
    IQuarantineItem* p1 = (IQuarantineItem*) lParam1;
    IQuarantineItem* p2 = (IQuarantineItem*) lParam2;
    CQconsoleView* pThis = (CQconsoleView*) lParamSort;
    TCHAR szBuffer1[MAX_PATH];
    TCHAR szBuffer2[MAX_PATH];
    SYSTEMTIME tm1, tm2;
    FILETIME ft1, ft2;

    switch( pThis->GetDocument()->GetSortCol() )
        {
        case COLUMN_FILE_NAME:
            p1->GetOriginalAnsiFilename( szBuffer1, MAX_PATH );
            NameReturnFile( szBuffer1, szBuffer1 );
            p2->GetOriginalAnsiFilename( szBuffer2, MAX_PATH );
            NameReturnFile( szBuffer2, szBuffer2 );
            return lstrcmp( szBuffer1, szBuffer2 );

        case COLUMN_LOCATION:
            if( pThis->GetDocument()->GetCorpMode() )
                {
                p1->GetOriginalOwnerName( szBuffer1, MAX_PATH );
                p2->GetOriginalOwnerName( szBuffer2, MAX_PATH );
                }
            else
                {
                p1->GetOriginalAnsiFilename( szBuffer1, MAX_PATH );
                NameReturnPath( szBuffer1, szBuffer1 );
                p2->GetOriginalAnsiFilename( szBuffer2, MAX_PATH );
                NameReturnPath( szBuffer2, szBuffer2 );
                }
            return lstrcmp( szBuffer1, szBuffer2 );

        case COLUMN_QDATE:
            p1->GetDateQuarantined( &tm1 );
            SystemTimeToFileTime( &tm1, &ft1 );
            p2->GetDateQuarantined( &tm2 );
            SystemTimeToFileTime( &tm2, &ft2 );
            return CompareFileTime( &ft1, &ft2 );

        case COLUMN_SDATE:
            p1->GetDateSubmittedToSARC( &tm1 );
            SystemTimeToFileTime( &tm1, &ft1 );
            p2->GetDateSubmittedToSARC( &tm2 );
            SystemTimeToFileTime( &tm2, &ft2 );
            return CompareFileTime( &ft1, &ft2 );

        case COLUMN_STATUS:
            {
            DWORD dwStatus1, dwStatus2;
            if( FAILED( p1->GetFileStatus( &dwStatus1 ) ) ||
                FAILED( p2->GetFileStatus( &dwStatus2 ) ))
                return 0;

            if( dwStatus1 < dwStatus2 )
                return -1;
            else if( dwStatus1 > dwStatus2 )
                return 1;
            else
                return 0;
            }
            break;

        case COLUMN_VIRUS_NAME:
            {
            // Get virus info struct from item.
            N30* pVirInfo1;
            N30* pVirInfo2;
            p1->GetN30StructPointer( &pVirInfo1 );
            p2->GetN30StructPointer( &pVirInfo2 );

            // Get virus name from struct.
            return lstrcmp( pVirInfo1->lpVName, pVirInfo2->lpVName );
            }
        }

    return 0;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQconsoleView::SortDescendCompareFunc
// Description	    : Sort descending sort callback function
// Return type		: int
// Argument         : LPARAM lParam1
// Argument         : LPARAM lParam2
// Argument         : LPARAM lParamSort
//
////////////////////////////////////////////////////////////////////////////////
// 3/5/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
int CQconsoleView::SortDescendCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
    IQuarantineItem* p1 = (IQuarantineItem*) lParam1;
    IQuarantineItem* p2 = (IQuarantineItem*) lParam2;
    CQconsoleView* pThis = (CQconsoleView*) lParamSort;
    TCHAR szBuffer1[MAX_PATH];
    TCHAR szBuffer2[MAX_PATH];
    SYSTEMTIME tm1, tm2;
    FILETIME ft1, ft2;

    switch( pThis->GetDocument()->GetSortCol() )
        {
        case COLUMN_FILE_NAME:
            p1->GetOriginalAnsiFilename( szBuffer1, MAX_PATH );
            NameReturnFile( szBuffer1, szBuffer1 );
            p2->GetOriginalAnsiFilename( szBuffer2, MAX_PATH );
            NameReturnFile( szBuffer2, szBuffer2 );
            return -lstrcmp( szBuffer1, szBuffer2 );

        case COLUMN_LOCATION:
            if( pThis->GetDocument()->GetCorpMode() )
                {
                p1->GetOriginalOwnerName( szBuffer1, MAX_PATH );
                p2->GetOriginalOwnerName( szBuffer2, MAX_PATH );
                }
            else
                {
                p1->GetOriginalAnsiFilename( szBuffer1, MAX_PATH );
                NameReturnPath( szBuffer1, szBuffer1 );
                p2->GetOriginalAnsiFilename( szBuffer2, MAX_PATH );
                NameReturnPath( szBuffer2, szBuffer2 );
                }
            return -lstrcmp( szBuffer1, szBuffer2 );

        case COLUMN_QDATE:
            p1->GetDateQuarantined( &tm1 );
            SystemTimeToFileTime( &tm1, &ft1 );
            p2->GetDateQuarantined( &tm2 );
            SystemTimeToFileTime( &tm2, &ft2 );
            return -CompareFileTime( &ft1, &ft2 );

        case COLUMN_SDATE:
            p1->GetDateSubmittedToSARC( &tm1 );
            SystemTimeToFileTime( &tm1, &ft1 );
            p2->GetDateSubmittedToSARC( &tm2 );
            SystemTimeToFileTime( &tm2, &ft2 );
            return -CompareFileTime( &ft1, &ft2 );

        case COLUMN_STATUS:
            {
            DWORD dwStatus1, dwStatus2;
            if( FAILED( p1->GetFileStatus( &dwStatus1 ) ) ||
                FAILED( p2->GetFileStatus( &dwStatus2 ) ))
                return 0;

            if( dwStatus1 > dwStatus2 )
                return -1;
            else if( dwStatus1 < dwStatus2 )
                return 1;
            else
                return 0;
            }
            break;

        case COLUMN_VIRUS_NAME:
            {
            // Get virus info struct from item.
            N30* pVirInfo1;
            N30* pVirInfo2;
            p1->GetN30StructPointer( &pVirInfo1 );
            p2->GetN30StructPointer( &pVirInfo2 );

            // Get virus name from struct.
            return -lstrcmp( pVirInfo1->lpVName, pVirInfo2->lpVName );
            }
        }

    return 0;
}



////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::HandleNewDefsScan
//
// Description	    : Handles scan of all non-backup quarantine items
//
// Return type		: void
//
//
////////////////////////////////////////////////////////////////////////////
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
    pI = pDoc->GetItemList( VIEW_MODE_ALL );
    pAllItems = reinterpret_cast< CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>* > (pI);
    ASSERT( pAllItems );

    // Create progress dialog
    CProgressDlg progress;
    progress.Create();
    progress.SetRange(0, pAllItems->GetCount() );

    // Setup status dialog.
    CResultsDlg dlg;
    dlg.SetImageList( GetListCtrl().GetImageList( LVSIL_SMALL ) );

    // Enum items in list
    POSITION pos;
    DWORD dwStatus;
    for( pos = pAllItems->GetStartPosition(); pos != NULL; )
        {
        pAllItems->GetNextAssoc(pos, pItem1, pItem2);

        // Get file status
        pItem1->GetFileStatus( &dwStatus );

        // Skip backup files
        if( dwStatus == QFILE_STATUS_BACKUP_FILE )
            continue;

        // Setup new results item.
        pResultItem = new CResultsListItem;

        // Save off item name
        TCHAR *p = pResultItem->m_sItemName.GetBuffer( MAX_PATH );
        pItem1->GetOriginalAnsiFilename( p, MAX_PATH );
        NameReturnFile( p, p );
        pResultItem->m_sItemName.ReleaseBuffer();

        // Get the filename and add it to our path
        CString sRealFileName;
        pItem1->GetCurrentFilename( sRealFileName.GetBuffer(MAX_PATH), MAX_PATH );
        sRealFileName.ReleaseBuffer();


        if( IsExecutable( sRealFileName ) )
            {
            pResultItem->m_iIconIndex = m_iSmallExeImage;
            }
        else
            {
            // Build path to this item
            CString s;

            // Start with quarantine folder
            s = pDoc->GetQuarantineFolder();
            s += _T( '\\' );
            s += sRealFileName;

            SHFILEINFO sfi;
            ZeroMemory( &sfi, sizeof(SHFILEINFO) );
            SHGetFileInfo( sRealFileName,
                           0,
                           &sfi,
                           sizeof( SHFILEINFO ),
                           SHGFI_SYSICONINDEX | SHGFI_SMALLICON
                           );

            // Save off icon index.
            pResultItem->m_iIconIndex = sfi.iIcon;
            }


        // Add to list
        dlg.m_aItemList.AddTail( pResultItem );

        // Attempt repair / restore operation.
        int iResult = RepairItem( pItem1, pResultItem );
        if( iResult == -1 || progress.CheckCancelButton() )
            return;

        // Advance progress dialog
        progress.SetPos( ++iCounter );
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
    pDoc->UpdateAllViews( NULL, LISTVIEW_REFRESH, NULL );

}



////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::OnItemchanged
//
// Description	    : Called when an item in the list view changes state.  
//                    Here I set the main frame status text based on the 
//                    state of the last selected item.
//
// Return type		: void 
//
// Argument         : NMHDR* pNMHDR
// Argument         : LRESULT* pResult
//
////////////////////////////////////////////////////////////////////////////
// 5/7/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
    // Is an item selected?
    if( pNMListView->iItem == -1 || ! (pNMListView->uNewState & LVIS_SELECTED) )
        return;

    // Get pointer to item
    IQuarantineItem* p = (IQuarantineItem*) GetListCtrl().GetItemData( pNMListView->iItem );
    ASSERT( p );
    
    // Classify this item
    DWORD dwStatus;
    if( FAILED( p->GetFileStatus( &dwStatus ) ) )
        return;

    // Set correct hint text.
    switch( dwStatus )
        {
        case QFILE_STATUS_QUARANTINED: 
            ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( IDS_STATUS_TEXT_QUARANTINED );
            break;
        
        case QFILE_STATUS_SUBMITTED:   
            {
            // Extract date text
            SYSTEMTIME tm;
            p->GetDateSubmittedToSARC( &tm );

            TCHAR szDateText[32];
            GetDateFormat( NULL,
                           DATE_SHORTDATE,
                           &tm,
                           NULL,
                           szDateText,
                           32 );

            // Build formatted string
            CString s;
            s.Format( IDS_STATUS_TEXT_SUBMITTED, szDateText );
            ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( s );
            }
            break;

        case QFILE_STATUS_BACKUP_FILE: 
            ((CFrameWnd*)AfxGetMainWnd())->SetMessageText( IDS_STATUS_TEXT_BACKUP );
            break;
        }

	*pResult = 0;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::GetOriginalFileSecurity
//
// Description	    : Gets the security descriptor for a given file
//
// Return type		: HRESULT 
//
// Argument         : LPSTR lpszSourceFileName - file to get security attributes from
// Argument         : LPBYTE *pDesc - pointer to buffer pointer.  Note that this
//                    function allocates this buffer.  It must be freed by the
//                    calling routine.
//
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::GetFileSecurityDesc( LPSTR lpszSourceFileName, LPBYTE *pDesc )
{
    // Do nothing of on NT.
    if( GetDocument()->IsNT() == FALSE )
        return S_OK;

    // Get security attributes of original file.
    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION | 
                              GROUP_SECURITY_INFORMATION | 
                              DACL_SECURITY_INFORMATION;

    // This call should fail, returning the length needed for the 
    // security descriptor.
    DWORD dwLengthNeeded = 0;
    GetFileSecurity( lpszSourceFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     0,
                     &dwLengthNeeded );

    // Allocate space needed for call.
    *pDesc = new BYTE[dwLengthNeeded];
    if( *pDesc == NULL )
        {
        return E_OUTOFMEMORY;
        }

    // This should get the actual security descriptor.
    if( FALSE == GetFileSecurity( lpszSourceFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     dwLengthNeeded,
                     &dwLengthNeeded ) )
        {
        return E_FAIL;
        }

    return S_OK;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::SetFileSecurityDesc
//
// Description	    : Set file security for a filesystem object
//
// Return type		: HRESULT 
//
// Argument         : LPSTR szFileName - File to modify
// Argument         : LPBYTE pDesc     - descriptor to set
//
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CQconsoleView::SetFileSecurityDesc( LPSTR szFileName, LPBYTE pDesc )
{
    // Do nothing of on NT.
    if( GetDocument()->IsNT() == FALSE )
        return S_OK;

    BOOL bRet = FALSE;

    if( pDesc != NULL )
        {
        SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION | 
                                  GROUP_SECURITY_INFORMATION | 
                                  DACL_SECURITY_INFORMATION;

        // Save return result.
        bRet = SetFileSecurity( szFileName, 
                         si,
                         pDesc );
        }

    return bRet ? S_OK : E_FAIL;
}



////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::OnWhatsThis
//
// Description	    : Handles right-click whats this help.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnWhatsThis() 
{
	// Invoke help system
    ::WinHelp( GetSafeHwnd(), 
               AfxGetApp()->m_pszHelpFilePath,
               HELP_CONTEXTPOPUP,
               IDH_QUARAN_MAINWIN );
	
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQconsoleView::OnViewContents
//
// Description	    : Handles help contents.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/21/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CQconsoleView::OnViewContents() 
{
		// Invoke help system
    ::WinHelp( GetSafeHwnd(), 
               AfxGetApp()->m_pszHelpFilePath,
               HELP_FINDER,
               0 );
}
