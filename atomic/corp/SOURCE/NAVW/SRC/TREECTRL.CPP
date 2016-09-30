// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// TREECTRL.CPP - created 04/08/1998 1:13:46 PM
//
// $Header:   S:/NAVW/VCS/treectrl.cpv   1.0   20 Apr 1998 23:12:48   DALLEE  $
//
// Description:
//      Implementation of CNVTreeCtrl. Non-MFC generic tree control class.
//
// Contains:
//      All CNVTreeCtrl methods
//*************************************************************************
// $Log:   S:/NAVW/VCS/treectrl.cpv  $
// 
//    Rev 1.0   20 Apr 1998 23:12:48   DALLEE
// Initial revision.
//*************************************************************************

#include "platform.h"
#include "treectrl.h"


//*************************************************************************
// CNVTreeCtrl::CNVTreeCtrl()
//
// public
// CNVTreeCtrl::CNVTreeCtrl( )
//
// Constructor for CNVTreeCtrl - Non-MFC tree control class.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

CNVTreeCtrl::CNVTreeCtrl()
{
    m_hWnd = NULL;
} // CNVTreeCtrl::CNVTreeCtrl()


//*************************************************************************
// CNVTreeCtrl::~CNVTreeCtrl()
//
// public virtual
// CNVTreeCtrl::~CNVTreeCtrl( )
//
// Destructor for CNVTreeCtrl - Non-MFC toolbar control class.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

CNVTreeCtrl::~CNVTreeCtrl()
{
} // CNVTreeCtrl::~CNVTreeCtrl()


//*************************************************************************
// CNVTreeCtrl::Create()
//
// public
// BOOL CNVTreeCtrl::Create(
//      HINSTANCE  hInst,               [in] application instance
//      DWORD      dwStyle,             [in] win, commctrl, and toolbar styles
//      LPCRECT    pRect,               [in] initial window position - may be NULL
//      HWND       hParent,             [in] parent window
//      UINT       uID )                [in] toolbar control ID in parent
//
// This routine creates a toolbar window and attaches it to the CNVTreeCtrl
// class. dwStyle must specify WM_CHILD. Other common style bits are CCS_TOP, 
// TBSTYLE_TOOLTIPS, or TBSTYLE_FLAT.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::Create( HINSTANCE  hInst,
                          DWORD      dwStyle, 
                          LPCRECT    pRect, 
                          HWND       hParent, 
                          UINT       uID )
{
    SYM_ASSERT( pRect );

    m_hWnd = ::CreateWindowEx( 0,
                               WC_TREEVIEW,
                               NULL,
                               dwStyle,
                               pRect->left, pRect->top,
                               pRect->right - pRect->left, pRect->bottom - pRect->top,
                               hParent,
                               reinterpret_cast<HMENU>( uID ),
                               hInst,
                               this );

    return ( m_hWnd ? TRUE : FALSE );
} // CNVTreeCtrl::Create()


//*************************************************************************
// CNVTreeCtrl::operator HWND()
//
// public
// CNVTreeCtrl::operator HWND( ) const
//
// Conversion to HWND.
//
// Returns: 
//      Handle to associated toolbar control.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

CNVTreeCtrl::operator HWND() const
{ 
    return ( m_hWnd );
} // CNVTreeCtrl::operator HWND()


//*************************************************************************
// CNVTreeCtrl::Attach()
//
// public
// BOOL CNVTreeCtrl::Attach(
//      HWND hWnd )                     [in] Window handle to attach to class.
//
// This method associates a Windows tree view control with the class.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::Attach( HWND hWnd )
{
    auto    BOOL    bSuccess;

    bSuccess = FALSE;

#ifdef _DEBUG
    auto    TCHAR   szClass [ sizeof( WC_TREEVIEW ) / sizeof( TCHAR ) ];

    if ( GetClassName( hWnd, szClass, sizeof( szClass ) / sizeof( *szClass ) ) )
    {
        // Should only be attaching to tree views

        SYM_ASSERT( 0 == _tcsicmp( WC_TREEVIEW, szClass ) );
    }
#endif

    if ( ( NULL == m_hWnd ) && ::IsWindow( hWnd ) )
    {
        m_hWnd = hWnd;

        m_lOldUserData = SetWindowLong( m_hWnd, GWL_USERDATA, (LONG) this );

        bSuccess = TRUE;
    }
    else
    {
        SYM_ASSERT( FALSE );
        bSuccess = FALSE;
    }

    return ( bSuccess );
} // CNVTreeCtrl::Attach()


//*************************************************************************
// CNVTreeCtrl::Detach()
//
// public
// HWND CNVTreeCtrl::Detach( )
//
// This method detaches the class from its associated Windows tree view control.
//
// Returns: 
//      Handle of previously associated window.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

HWND CNVTreeCtrl::Detach()
{
    auto    HWND    hWndResult;

    if ( ::IsWindow( m_hWnd ) )
    {
        SetWindowLong( m_hWnd, GWL_USERDATA, m_lOldUserData );
    }

    hWndResult = m_hWnd;

    m_hWnd = NULL;

    return ( hWndResult );
} // CNVTreeCtrl::Detach()


//*************************************************************************
// CNVTreeCtrl::DestroyWindow()
//
// public virtual
// BOOL CNVTreeCtrl::DestroyWindow( )
//
// This method destroys the associated tree view window.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::DestroyWindow()
{
    auto    BOOL    bSuccess;

    bSuccess = FALSE;

    // Bail out early if no associated window.

    if ( NULL == m_hWnd )
    {
        goto BailOut;
    }

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    if ( ::DestroyWindow( m_hWnd ) )
    {
        m_hWnd   = NULL;
        bSuccess = TRUE;
    }

BailOut:
    return ( bSuccess );
} // CNVTreeCtrl::DestroyWindow()


//*************************************************************************
// CNVTreeCtrl::GetImageList()
//
// public const
// HIMAGELIST CNVTreeCtrl::GetImageList(
//      int nType )                     [in] type of image list to return.
//
// This method returns a handle to the selected image list.
// nType may be
//      TVSIL_NORMAL        for normal image list
//      TVSIL_STATE         for state image list
//
// Returns: 
//      On success, handle to selected image list. Otherwise, NULL.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVTreeCtrl::GetImageList( int nType ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetImageList( m_hWnd, nType ) );
} // CNVTreeCtrl::GetImageList()

HIMAGELIST CNVTreeCtrl::GetNormalImageList() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetImageList( m_hWnd, TVSIL_NORMAL ) );
} // CNVTreeCtrl::GetNormalImageList()

HIMAGELIST CNVTreeCtrl::GetStateImageList() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetImageList( m_hWnd, TVSIL_STATE ) );
} // CNVTreeCtrl::GetStateImageList()


//*************************************************************************
// CNVTreeCtrl::SetImageList()
//
// public
// HIMAGELIST CNVTreeCtrl::SetImageList(
//      HIMAGELIST  hImageList,         [in] handle of new image list
//      int         nType )             [in] which image list to set
//
// This method assigns a new image list to a control and redraws the control.
// If hImageList is NULL, all images are removed.
// nType indicates which images list to set. It may be either
//      TVSIL_NORMAL        for normal image list
//      TVSIL_STATE         for state image list
//
// Returns: 
//      Handle of the previous image list, or NULL if none.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

HIMAGELIST CNVTreeCtrl::SetImageList( HIMAGELIST hImageList, int nType )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_SetImageList( m_hWnd, hImageList, nType ) );
} // CNVTreeCtrl::SetImageList()

HIMAGELIST CNVTreeCtrl::SetNormalImageList( HIMAGELIST hImageList )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_SetImageList( m_hWnd, hImageList, TVSIL_NORMAL ) );
} // CNVTreeCtrl::SetNormalImageList()

HIMAGELIST CNVTreeCtrl::SetStateImageList( HIMAGELIST hImageList )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_SetImageList( m_hWnd, hImageList, TVSIL_STATE ) );
} // CNVTreeCtrl::SetStateImageList()


//*************************************************************************
// CNVTreeCtrl::GetItem()
//
// public const
// BOOL CNVTreeCtrl::GetItem(
//      LPTV_ITEM pItem )               [in/out] indicates information to 
//                                          query and receives information.
//
// This method returns information on a tree view item.
// The hItem and mask members of pItem must be set to the desired tree item
// and data to receive.
// On success, pItem will be filled in with the requested information.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::GetItem( LPTV_ITEM pItem ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( pItem );

    return ( TreeView_GetItem( m_hWnd, pItem ) );
} // CNVTreeCtrl::GetItem()


//*************************************************************************
// CNVTreeCtrl::SetItem()
//
// public
// BOOL CNVTreeCtrl::SetItem(
//      const TV_ITEM *pItem )          [in] info on tree item and data to set.
//
// This method sets some or all of the data associated with a tree view item.
// The hItem and mask members of pItem indicate the tree view item and the
// data to set.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::SetItem( const TV_ITEM *pItem )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( pItem );

    return ( TreeView_SetItem( m_hWnd, pItem ) );
} // CNVTreeCtrl::SetItem()


//*************************************************************************
// CNVTreeCtrl::GetItemData()
//
// public const
// LPARAM GetItemData(
//      HTREEITEM hItem )               [in] Retrieve data from this item.
//
// This method retrieves the specified tree item's user data.
//
// Returns: 
//      32-bit user data associated with tree item on success.
//      0 on error.
//*************************************************************************
// 04/10/1998 DALLEE, created - header added.
//*************************************************************************

LPARAM CNVTreeCtrl::GetItemData( HTREEITEM hItem ) const
{
    auto    TV_ITEM     rItem;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    memset( &rItem, 0, sizeof( rItem ) );

    rItem.mask      = TVIF_HANDLE | TVIF_PARAM;
    rItem.hItem     = hItem;

    return ( GetItem( &rItem ) ? rItem.lParam : 0 );
} // CNVTreeCtrl::GetItemData()


//*************************************************************************
// CNVTreeCtrl::SetItemData()
//
// public
// BOOL SetItemData(
//      HTREEITEM hItem,                [in] Set this item's user data.
//      LPARAM lData )                  [in] Data to set.
//
// This method sets the 32-bit user data of the specified tree item.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/10/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::SetItemData( HTREEITEM hItem, LPARAM lData )
{
    auto    TV_ITEM     rItem;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    memset( &rItem, 0, sizeof( rItem ) );

    rItem.mask      = TVIF_HANDLE | TVIF_PARAM;
    rItem.hItem     = hItem;
    rItem.lParam    = lData;

    return ( SetItem( &rItem ) );
} // CNVTreeCtrl::SetItemData()


//*************************************************************************
// CNVTreeCtrl::GetItemState()
//
// public const
// UINT GetItemState(
//      HTREEITEM   hItem               [in] Retrieve state from this item.
//      UINT        uMask )             [in] Mask of state bits to retrieve.
//
// This method retrieves the specified bits of the tree item's state.
//
// Returns: 
//      State bits of specified tree item on success.
//      0 on error.
//*************************************************************************
// 04/10/1998 DALLEE, created - header added.
//*************************************************************************

UINT CNVTreeCtrl::GetItemState( HTREEITEM hItem, UINT uMask ) const
{
    auto    TV_ITEM     rItem;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    memset( &rItem, 0, sizeof( rItem ) );

    rItem.mask      = TVIF_HANDLE | TVIF_STATE;
    rItem.hItem     = hItem;
    rItem.stateMask = uMask;

    return ( GetItem( &rItem ) ? ( rItem.state & uMask ) : 0 );
} // CNVTreeCtrl::GetItemState()


//*************************************************************************
// CNVTreeCtrl::SetItemState()
//
// public
// BOOL SetItemState(
//      HTREEITEM hItem,                [in] Set this item's user data.
//      UINT uState,                    [in] State info to set.
//      UINT uMask )                    [in] Mask of bits in uState which are valid.
//
// This method sets the specified bits of the tree item's state.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/10/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::SetItemState( HTREEITEM hItem, UINT uState, UINT uMask )
{
    auto    TV_ITEM     rItem;

    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    memset( &rItem, 0, sizeof( rItem ) );

    rItem.mask      = TVIF_HANDLE | TVIF_STATE;
    rItem.hItem     = hItem;
    rItem.state     = uState;
    rItem.stateMask = uMask;

    return ( SetItem( &rItem ) );
} // CNVTreeCtrl::SetItemState()


//*************************************************************************
// CNVTreeCtrl::GetNextItem()
//
// public const
// HTREEITEM CNVTreeCtrl::GetNextItem(
//      HTREEITEM   hItem,              [in] handle of base item
//      int         nRelation )         [in] relationship of desired item to 
//                                          base item
//
// This method returns the tree view item with the specified relation to
// the base item passed.
// For some relations, hItem may be NULL.
// nRelation may be one of:
//      TVGN_CARET              Currently selected item.
//      TVGN_CHILD              First child of hItem.
//      TVGN_DROPHILITE         Target item of current drag-and-drop operation.
//      TVGN_FIRSTVISIBLE       First visible item.
//      TVGN_NEXT               Next sibling item of hItem.
//      TVGN_NEXTVISIBLE        Next visible item following hItem. hItem must be visble.
//      TVGN_PARENT             Parent of hItem.
//      TVGN_PREVIOUS           Previous sibling item of hItem.
//      TVGN_PREVIOUSVISIBLE    First visible item preceding hItem. hItem must be visible.
//      TVGN_ROOT               First item of tree-view control.
//
// See also:
//      CNVTreeCtrl::GetChildItem()
//      CNVTreeCtrl::GetNextSiblingItem()
//      CNVTreeCtrl::GetParentItem()
//      CNVTreeCtrl::GetPrevSiblingItem()
//      CNVTreeCtrl::GetRootItem()
//      CNVTreeCtrl::GetSelectedItem()
//
// Returns: 
//      Handle of desired tree view item on success. Otherwise, NULL.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

HTREEITEM CNVTreeCtrl::GetNextItem( HTREEITEM hItem, int nRelation ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetNextItem( m_hWnd, hItem, nRelation ) );
} // CNVTreeCtrl::GetNextItem()

HTREEITEM CNVTreeCtrl::GetChildItem( HTREEITEM hItem ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetChild( m_hWnd, hItem ) );
} // CNVTreeCtrl::GetChildItem()

HTREEITEM CNVTreeCtrl::GetNextSiblingItem( HTREEITEM hItem ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetNextSibling( m_hWnd, hItem ) );
} // CNVTreeCtrl::GetNextSiblingItem()

HTREEITEM CNVTreeCtrl::GetParentItem( HTREEITEM hItem ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetParent( m_hWnd, hItem ) );
} // CNVTreeCtrl::GetParentItem()

HTREEITEM CNVTreeCtrl::GetPrevSiblingItem( HTREEITEM hItem ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetPrevSibling( m_hWnd, hItem ) );
} // CNVTreeCtrl::GetPrevSiblingItem()

HTREEITEM CNVTreeCtrl::GetRootItem() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetRoot( m_hWnd ) );
} // CNVTreeCtrl::GetRootItem()

HTREEITEM CNVTreeCtrl::GetSelectedItem() const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_GetSelection( m_hWnd ) );
} // CNVTreeCtrl::GetSelectedItem()


//*************************************************************************
// CNVTreeCtrl::GetItemRect()
//
// public const
// BOOL CNVTreeCtrl::GetItemRect(
//      HTREEITEM   hItem,              [in] tree item whose position to retrieve.
//      LPRECT      lpRect,             [out] receives position of tree item.
//      BOOL        bTextOnly )         [in] retrieve position of only text of item.
//
// This method retrieves to position of the specified tree view item relative
// to the upper left of the tree view control.
// If bTextOnly is TRUE, the bounding rectangle of the item's text is retrieved.
// If FALSE, the bounding rectangle of the item's entire line is retrieved.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::GetItemRect( HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( lpRect );

    return ( TreeView_GetItemRect( m_hWnd, hItem, lpRect, bTextOnly ) );
} // // CNVTreeCtrl::GetItemRect()


//*************************************************************************
// CNVTreeCtrl::HitTest()
//
// public const
// HTREEITEM CNVTreeCtrl::HitTest(
//      LPTV_HITTESTINFO pHitTestInfo ) [in/out] specifies point to test and
//                                          receives info on location.
//
// This method retrieves the location of a point in screen coordinates
// relative to the associated tree view control.
//
// On entry, the pt member of pHitTestInfo specifies a point in screen coordinates.
//
// On return, the hItem member of pHitTestInfo indicates the tree view item
// which contains the point, or NULL if not contained by any item.
//
// On return, the flags member of pHitTestInfo will be one or more of the 
// following values indicating the relation of the point to the tree control:
//
//      TVHT_ABOVE              Above the client area.
//      TVHT_BELOW              Below the client area.
//      TVHT_NOWHERE            In the client area, but below the last item.
//      TVHT_ONITEM             On the bitmap or label associated with an item.
//      TVHT_ONITEMBUTTON       On the button associated with an item.
//      TVHT_ONITEMICON         On the bitmap associated with an item.
//      TVHT_ONITEMINDENT       In the indentation associated with an item.
//      TVHT_ONITEMLABEL        On the label (string) associated with an item.
//      TVHT_ONITEMRIGHT        In the area to the right of an item.
//      TVHT_ONITEMSTATEICON    On the state icon for a tree-view item.
//      TVHT_TOLEFT             To the left of the client area.
//      TVHT_TORIGHT            To the right of the client area.
//
// Returns: 
//      Handle of tree view item which contains the specified point.
//      NULL if no item contains the point.
//*************************************************************************
// 04/08/1998 DALLEE, created - header added.
//*************************************************************************

HTREEITEM CNVTreeCtrl::HitTest( LPTV_HITTESTINFO pHitTestInfo ) const
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( pHitTestInfo );

    return ( TreeView_HitTest( m_hWnd, pHitTestInfo ) );
} // CNVTreeCtrl::HitTest()


//*************************************************************************
// CNVTreeCtrl::InsertItem()
//
// public
// HTREEITEM CNVTreeCtrl::InsertItem(
//      const TV_INSERTSTRUCT *pInsert )    [in] info on item to insert
//
// This method inserts an item into the tree control.
//
// See TV_INSERTSTRUCT, TV_ITEM.
//
// Returns: 
//      Handle to new item on success. Otherwise, NULL.
//*************************************************************************
// 04/09/1998 DALLEE, created - header added.
//*************************************************************************

HTREEITEM CNVTreeCtrl::InsertItem( const TV_INSERTSTRUCT *pInsert )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( pInsert );

    return ( TreeView_InsertItem( m_hWnd, pInsert ) );
} // CNVTreeCtrl::InsertItem()


//*************************************************************************
// CNVTreeCtrl::DeleteAllItems()
//
// public
// BOOL CNVTreeCtrl::DeleteAllItems( )
//
// This item removes all items from the tree control.
// The parent will receive a TVN_DELETITEM WM_NOTIFY message for each item.
// These messages must be forwarded to CNVTreeCtrl::OnNotify().
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/09/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::DeleteAllItems()
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_DeleteAllItems( m_hWnd ) );
} // CNVTreeCtrl::DeleteAllItems()


//*************************************************************************
// CNVTreeCtrl::DeleteItem()
//
// public
// BOOL CNVTreeCtrl::DeleteItem(
//      HTREEITEM hItem )               [in] Handle of item to delete.
//
// This method removes an item from the tree control.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/09/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::DeleteItem( HTREEITEM hItem )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_DeleteItem( m_hWnd, hItem ) );
} // CNVTreeCtrl::DeleteItem()


//*************************************************************************
// CNVTreeCtrl::Expand()
//
// public
// BOOL CNVTreeCtrl::Expand(
//      HTREEITEM   hItem,              [in] Item whose expanded state to change.
//      int         nAction )           [in] Expansion action to take.
//
// This method changes the expanded state of an item.
// nAction may be one or more of the following:
//      TVE_COLLAPSE        Collapses the item.
//      TVE_COLLAPSERESET   Collapses the item and removes the child items. 
//                              Note that TVE_COLLAPSE must also be specified.
//      TVE_EXPAND          Expands the item. 
//      TVE_TOGGLE          Collapses the list if it is currently expanded or 
//                              expands it if it is currently collapsed. 
//
// Note that the parent will not receive TVN_ITEMEXPANDING or TVN_ITEMEXPANDED
// messages as a result.
//
// Returns: 
//      TRUE if a change took place. Otherwise, FALSE.
//*************************************************************************
// 04/09/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::Expand( HTREEITEM hItem, int nAction )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( ( nAction & TVE_COLLAPSERESET ) ? ( nAction & TVE_COLLAPSE ) : 1 );

    return ( TreeView_Expand( m_hWnd, hItem, nAction ) );
} // CNVTreeCtrl::Expand()


//*************************************************************************
// CNVTreeCtrl::SortChildren()
//
// public
// BOOL CNVTreeCtrl::SortChildren(
//      HTREEITEM hItem )               [in] Item whose children to sort.
//
// This method sorts the specified tree item's child items alphabetically.
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/09/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::SortChildren( HTREEITEM hItem )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );

    return ( TreeView_SortChildren( m_hWnd, hItem, 0 ) );
} // CNVTreeCtrl::SortChildren()


//*************************************************************************
// CNVTreeCtrl::SortChildrenCB()
//
// BOOL CNVTreeCtrl::SortChildrenCB(
//      LPTV_SORTCB pSortInfo )         [in] Info on item whose children
//                                           to sort and how to sort.
//
// This method sorts the specified tree item's child items.
// pSortInfo specifies the parent tree item whose children will be sorted
// and additionally, a callback function to perform the comparison.
// 
// The callback will receive the LPARAM passed to this method and also the
// LPARAMs of each two tree items to be compared.
// The callback should return a negative value if the first item should
// precede the second, a positive value if the first should follow the second,
// and zero if the items are equivalent.
//
// typedef int (CALLBACK *PFNTVCOMPARE)(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
//
// typedef struct _TV_SORTCB {
//      HTREEITEM    hParent;
//      PFNTVCOMPARE lpfnCompare;
//      LPARAM       lParam;
// } TV_SORTCB, FAR *LPTV_SORTCB; 
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/09/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVTreeCtrl::SortChildrenCB( LPTV_SORTCB pSortInfo )
{
    SYM_ASSERT( ::IsWindow( m_hWnd ) );
    SYM_ASSERT( pSortInfo );
    SYM_ASSERT( pSortInfo->lpfnCompare );

    return ( TreeView_SortChildrenCB( m_hWnd, pSortInfo, 0 ) );
} // CNVTreeCtrl::SortChildrenCB()

