////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************
//
// $Header:   S:/NAVW/VCS/nvbrowse.cpv   1.5   22 Oct 1998 19:39:54   CEATON  $
//
// Description:
//      NAV Multiple-Selection Folder browse control.
//
// Contains:
//      CNVBrowse
//      CNVBrowseNode
//      CNVBrowseItem
//*************************************************************************
// $Log:   S:/NAVW/VCS/nvbrowse.cpv  $
// 
//    Rev 1.5   22 Oct 1998 19:39:54   CEATON
// Added member function to return number of selected items.
// 
// 
//    Rev 1.4   30 Apr 1998 19:28:44   DALLEE
// Oops... Problem retrieving correct buffer size in GetName().
// Caused unpredictable behavior in NT - no crash, but bogus names
// returned.
// 
//    Rev 1.3   26 Apr 1998 15:50:32   DALLEE
// Put Desktop back to root of treee - Now we show both My Computer and
// Network Neighborhood.
// Select and expand My Computer on initialization.
// Only display checkboxes next to scanable (filesystem) objects.
// 
//    Rev 1.2   23 Apr 1998 14:17:36   DALLEE
// Argh! Left out a very important "CallWindowProc(" when calling
// the treeview proc. Thought for sure I had that in there...
// 
//    Rev 1.1   22 Apr 1998 16:07:04   DALLEE
// Changes to compile for Alpha.
// Had to implement operators for = != > < for CNVBrowseItem and
// CIconID which are stored in STL collections.
// 
//    Rev 1.0   20 Apr 1998 23:12:12   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include <shlobj.h>
#include "NVBrowse.h"
#include "resource.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////

static void StrretToLPTSTR ( const STRRET& rStrRet, LPTSTR str,
                             LPCITEMIDLIST pidl, LPMALLOC pMalloc )
{
    ASSERT ( NULL != pidl );
    ASSERT ( NULL != pMalloc );

#ifndef _UNICODE
    switch ( rStrRet.uType )
        {
        case STRRET_CSTR:
            _tcsncpy( str, rStrRet.cStr, MAX_PATH - 1 );
        break;

        case STRRET_OFFSET:
            _tcsncpy( str, 
                      LPCSTR( (LPBYTE)pidl + rStrRet.uOffset ),
                      MAX_PATH - 1 );
        break;

        case STRRET_WSTR:
            WideCharToMultiByte( CP_ACP, 0, rStrRet.pOleStr,
                                 -1, str, MAX_PATH - 1, NULL, NULL );

            pMalloc->Free( rStrRet.pOleStr );
        break;
            
        DEFAULT_UNREACHABLE;
        }
#else   // _UNICODE
    switch ( rStrRet.uType )
        {
        case STRRET_CSTR:
            MultiByteToWideChar( CP_ACP, 0,rStrRet.cStr,
                                 -1, str, MAX_PATH - 1 );
        break;

        case STRRET_OFFSET:
            MultiByteToWideChar( CP_ACP, 0,
                                 LPCSTR( (LPBYTE)pidl + rStrRet.uOffset ),
                                 -1, str, MAX_PATH - 1 );
        break;

        case STRRET_WSTR:
            _tcsncpy( str, rStrRet.pOleStr, MAX_PATH - 1 );

            pMalloc->Free( rStrRet.pOleStr );
        break;

        DEFAULT_UNREACHABLE;
        }
#endif  // _UNICODE
}

/////////////////////////////////////////////////////////////////////////////
// CNVBrowse

CNVBrowse::CNVBrowse()
{
    m_hImageList      = NULL;
    m_hStateImageList = NULL;
    m_pfnOrigWndProc  = NULL;
    m_pValidateSink   = NULL;
}


CNVBrowse::~CNVBrowse()
{
    if ( ::IsWindow( m_hWnd ) &&
         m_pfnOrigWndProc )
    {
        SetWindowLong( m_hWnd, GWL_WNDPROC, ( LONG )m_pfnOrigWndProc );
    }

    if ( m_hImageList )
        ImageList_Destroy( m_hImageList );

    if ( m_hStateImageList )
        ImageList_Destroy( m_hStateImageList );
}


BOOL CNVBrowse::Attach( HWND hWnd )
{
    auto    BOOL    bSuccess;

    bSuccess = CNVTreeCtrl::Attach( hWnd );

    if ( bSuccess )
    {
        m_pfnOrigWndProc = ( WNDPROC )GetWindowLong( m_hWnd, GWL_WNDPROC );
        SetWindowLong( m_hWnd, GWL_WNDPROC, ( LONG )SubClassWndProc );
    }

    return ( bSuccess );
}


HWND CNVBrowse::Detach()
{
    if ( ::IsWindow( m_hWnd ) &&
         m_pfnOrigWndProc )
    {
        SetWindowLong( m_hWnd, GWL_WNDPROC, ( LONG )m_pfnOrigWndProc );
        m_pfnOrigWndProc = NULL;
    }

    return ( CNVTreeCtrl::Detach() );
}

//*************************************************************************
// CNVBrowse::SubClassWndProc()
//
// LRESULT CALLBACK CNVBrowse::SubClassWndProc(
//      HWND       hWnd,
//      UINT       uMsg,
//      WPARAM     wParam,
//      LPARAM     lParam )
//
// Custom window procedure for the browse control.
// Prevents double clicks on the state icon from expanding tree item.
// Traps VK_SPACE and toggles checked status of currently selected item.
//
// Returns: 
//      Dependent on message.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

LRESULT CALLBACK CNVBrowse::SubClassWndProc( HWND       hWnd,
                                             UINT       uMsg,
                                             WPARAM     wParam,
                                             LPARAM     lParam )
{
    auto    LRESULT         lResult;
    auto    TV_HITTESTINFO  rHitInfo;
    auto    HTREEITEM       hItem;
    auto    CNVBrowse       *pThis;

    lResult = 0;

    pThis = ( CNVBrowse * )GetWindowLong( hWnd, GWL_USERDATA );

    if ( pThis )
    {
        switch ( uMsg )
        {
        case WM_LBUTTONDBLCLK:
            memset( &rHitInfo, 0, sizeof( rHitInfo ) );
            rHitInfo.pt.x = LOWORD( lParam );
            rHitInfo.pt.y = HIWORD( lParam );

            pThis->HitTest( &rHitInfo );

            if ( 0 == ( rHitInfo.flags & TVHT_ONITEMSTATEICON ) )
            {
                lResult = CallWindowProc( pThis->m_pfnOrigWndProc, hWnd, uMsg, wParam, lParam );
            }
            break;

        case WM_CHAR:
            if ( VK_SPACE == wParam )
            {
                hItem = pThis->GetSelectedItem();
                
                if ( hItem )
                {
                    if( pThis->GetItemState( hItem, TVIS_STATEIMAGEMASK ) == INDEXTOSTATEIMAGEMASK( CHECKED ) )
                    {
                        pThis->CheckItem( hItem, UNCHECKED );
                    }
                    else
                    {
                        pThis->CheckItem( hItem, CHECKED );
                    }
                }
            }
            else
            {
                lResult = CallWindowProc( pThis->m_pfnOrigWndProc, hWnd, uMsg, wParam, lParam );
            }
            break;

        default:
            lResult = CallWindowProc( pThis->m_pfnOrigWndProc, hWnd, uMsg, wParam, lParam );
            break;
        }
    }

    return ( lResult );
}

LRESULT CNVBrowse::OnNotify( WPARAM wParam, LPARAM lParam )
{
    auto    LPNMHDR     pNMHDR;
    auto    LRESULT     lResult;

    pNMHDR = reinterpret_cast<LPNMHDR>(lParam);

    ASSERT( pNMHDR->hwndFrom == m_hWnd );

    switch ( pNMHDR->code )
    {
    case TVN_ITEMEXPANDING:
        OnItemExpanding( pNMHDR, &lResult );
        break;

    case TVN_DELETEITEM:
        OnDeleteItem( pNMHDR, &lResult );
        break;

    case NM_CLICK:
        OnLClick( pNMHDR, &lResult );
        break;

    default:
        lResult = 0;
        break;
    }

    return ( lResult );
}            


void CNVBrowse::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
    auto    CNVBrowseNode   *pNode;
	auto    NM_TREEVIEW     *pNMTreeView = (NM_TREEVIEW *)pNMHDR;

    // Check for first time expansion and add children
    if ( ( TVE_EXPAND == pNMTreeView->action ) &&
         !( TVIS_EXPANDEDONCE & pNMTreeView->itemNew.state ) )
    {
        if ( NULL != ( pNode = (CNVBrowseNode *)pNMTreeView->itemNew.lParam ) )
            pNode->AddChildren();
    }

	*pResult = 0;
}


//***************************************************************************
// CNVBrowse::OnDeleteItem()
//
// void CNVBrowse::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
// 
// This routine is called for each tree item deleted.
// Node cleanup is handled here.
//***************************************************************************
// 01/08/1997 DALLEE created
//***************************************************************************

void CNVBrowse::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
    auto    CNVBrowseNode     *pNode;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Free node class associated with this item.

    pNode = (CNVBrowseNode *)pNMTreeView->itemOld.lParam;

    if ( NULL != pNode )
        delete pNode;

	*pResult = 0;
}


void CNVBrowse::OnLClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    auto    HTREEITEM   hItem;
    auto    DWORD       dwMessagePos;

    auto    TV_HITTESTINFO  rHitInfo;

    memset( &rHitInfo, 0, sizeof( rHitInfo ) );

    dwMessagePos = ::GetMessagePos();
    rHitInfo.pt.x = LOWORD( dwMessagePos );
    rHitInfo.pt.y = HIWORD( dwMessagePos );
    ScreenToClient( m_hWnd, &rHitInfo.pt );

    // If user clicked on the state icon, toggled checked status.

    hItem = HitTest( &rHitInfo );

    if ( ( NULL != hItem ) &&
         ( rHitInfo.flags & TVHT_ONITEMSTATEICON ) ||
         ( rHitInfo.flags & TVHT_ONITEMLABEL ) ||
         ( rHitInfo.flags & TVHT_ONITEMICON ))
    {
        if( GetItemState( hItem, TVIS_STATEIMAGEMASK ) == INDEXTOSTATEIMAGEMASK( CHECKED ) )
        {
            CheckItem( hItem, UNCHECKED );
        }
        else
        {
            CheckItem( hItem, CHECKED );
        }
    }

    *pResult = 0;
}


int CNVBrowse::Init( HINSTANCE hResInst )
{
    auto    int             nReturn;
    auto    TV_INSERTSTRUCT rTreeInsert;
    auto    IMalloc         *pSHMalloc;
    auto    CNVBrowseNode   *pRootNode;
    auto    ULONG           ulAttr;

    auto    STRRET          rStrRet;
    auto    TCHAR           szDisplayName [ MAX_PATH ];

    auto    int             nImage;
    auto    int             nSelectedImage;

    auto    HTREEITEM       hChildItem;
    auto    LPITEMIDLIST    pidlMyComputer;
    auto    CNVBrowseNode   *pChildNode;
    auto    HRESULT         hResult;
    auto    BOOL            bHWIsNEC;   // NEC hardware?

    memset( &rStrRet, 0, sizeof( rStrRet ) );
    pSHMalloc       = NULL;
    pRootNode       = NULL;
    pidlMyComputer  = NULL;
    nReturn         = -1;
    bHWIsNEC        = ( GetKeyboardType(0) == 7 &&
                        GetKeyboardType(1) >= 0x0D01 );

    // Create and attach initial image lists

    m_hStateImageList = ImageList_LoadImage( hResInst,
                                             MAKEINTRESOURCE( IDB_SCAN_FOLDER_CHECKBOXES ),
                                             16,
                                             0,
                                             RGB( 255, 0, 255 ),
                                             IMAGE_BITMAP,
                                             LR_LOADMAP3DCOLORS );

    if ( NULL == m_hStateImageList )
        goto BailOut;

    SetImageList( m_hStateImageList, TVSIL_STATE );

    m_hImageList = ImageList_Create( GetSystemMetrics( SM_CXSMICON ),
                                     GetSystemMetrics( SM_CYSMICON ),
									 ILC_MASK | ILC_COLOR32,
                                     10, 30 );

    if ( NULL == m_hImageList )
        goto BailOut;

    SetImageList( m_hImageList, TVSIL_NORMAL );

    // Get shell's malloc
    if ( FAILED( ::SHGetMalloc( &pSHMalloc ) ) )
        goto BailOut;

    // Allocate node class for desktop at root of tree
    if ( NULL == ( pRootNode = new CNVBrowseNode ) )
        goto BailOut;

    pRootNode->m_pTree = this;

    // Get ITEMIDLIST for desktop
    if ( FAILED( ::SHGetSpecialFolderLocation( m_hWnd,
                                               CSIDL_DESKTOP,
                                               &pRootNode->m_pIDL ) ) )
        goto BailOut;

    // Get IShellFolder interface for desktop
    if ( FAILED( ::SHGetDesktopFolder( &pRootNode->m_pParentISF ) ) )
        goto BailOut;

    pRootNode->m_pISF = pRootNode->m_pParentISF;
    pRootNode->m_pISF->AddRef();

    // Get folder attributes (find out if it has children)
    //ulAttr = SFGAO_HASSUBFOLDER;
    ulAttr = (UINT)-1;
    if ( FAILED( pRootNode->m_pParentISF->GetAttributesOf( 1, (LPCITEMIDLIST *)&pRootNode->m_pIDL, &ulAttr ) ) )
        goto BailOut;

    pRootNode->m_ulAttr = ulAttr;

    // Get display name of desktop
    if ( FAILED( pRootNode->m_pParentISF->GetDisplayNameOf( pRootNode->m_pIDL,
                                                      SHGDN_INFOLDER,
                                                      &rStrRet ) ) )
        {
        goto BailOut;
        }

    szDisplayName[ 0 ] = _T('\0');
    szDisplayName[ MAX_PATH - 1 ] = _T('\0');

    StrretToLPTSTR ( rStrRet, szDisplayName, pRootNode->m_pIDL,
                     pSHMalloc );

    // Get images for this node and register them with the image list.

    nImage         = pRootNode->RegisterImage( pRootNode->m_pIDL, GIL_FORSHELL );
    nSelectedImage = pRootNode->RegisterImage( pRootNode->m_pIDL, GIL_FORSHELL | GIL_OPENICON );

    if ( -1 == nImage )
        nImage = 0;
    if ( -1 == nSelectedImage )
        nSelectedImage = 0;

    // Insert desktop at root of tree

    memset( &rTreeInsert, 0, sizeof( rTreeInsert ) );
    rTreeInsert.hParent             = TVI_ROOT;
    rTreeInsert.hInsertAfter        = TVI_SORT;
    rTreeInsert.item.mask           = TVIF_PARAM | TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    rTreeInsert.item.pszText        = szDisplayName;
    rTreeInsert.item.iImage         = nImage;
    rTreeInsert.item.iSelectedImage = nSelectedImage;
    rTreeInsert.item.cChildren      = ( ulAttr & SFGAO_HASSUBFOLDER ) ? 1 : 0;
    rTreeInsert.item.lParam         = (LPARAM)pRootNode;

    pRootNode->m_hItem = InsertItem( &rTreeInsert );

    if ( NULL != pRootNode->m_hItem )
    {
        Expand( pRootNode->m_hItem, TVE_EXPAND );

        // Go through children and select and expand My Computer.

        if ( SUCCEEDED( ::SHGetSpecialFolderLocation( m_hWnd, CSIDL_DRIVES, &pidlMyComputer ) ) )
        {
            for ( hChildItem = GetChildItem( pRootNode->m_hItem );
                  hChildItem != NULL;
                  hChildItem = GetNextSiblingItem( hChildItem ) )
            {
                pChildNode = ( CNVBrowseNode * )GetItemData( hChildItem );

                ASSERT( pChildNode );

                hResult = pChildNode->m_pParentISF->CompareIDs( 0, pChildNode->m_pIDL, pidlMyComputer );

                if ( SUCCEEDED( hResult ) && 
                     ( 0 == SCODE_CODE( hResult ) ) )
                {
                    Expand( pChildNode->m_hItem, TVE_EXPAND );
                    TreeView_SelectItem( m_hWnd, pChildNode->m_hItem );

                    // Find the first hard drive (C: for ATs, A: for NECs) 
                    // and expand & select it.  'Tis a usability improvement -
                    // novice users don't know what to do if all they see are
                    // the drives.  (They don't understand how to use the
                    // tree control, or even what the tree is showing them.)
                    // Expanding the drive will show them some 
                    // folders, which'll make it easier for them to grasp what 
                    // they're looking at, since they are "scanning folders"
                    // when they get to this dialog.

                    BOOL bDriveFound = FALSE;
                    LPCTSTR szDriveToExpand;

                    // NEC code removed since we no longer support them.
                    //
                    szDriveToExpand = _T("c:\\");

                    for ( hChildItem = GetChildItem ( hChildItem );
                          hChildItem != NULL;
                          hChildItem = GetNextSiblingItem ( hChildItem ))
                        {
                        pChildNode = ( CNVBrowseNode * )GetItemData( hChildItem );
                        ASSERT( NULL != pChildNode );

                        // Get the path name of the next
                        hResult = pChildNode->m_pParentISF->GetDisplayNameOf (
                                      pChildNode->m_pIDL, SHGDN_FORPARSING,
                                      &rStrRet );
                        if ( SUCCEEDED(hResult) )
                            {
                            StrretToLPTSTR ( rStrRet, szDisplayName,
                                             pChildNode->m_pIDL, pSHMalloc );

                            // Is is the drive we want to expand?
                            if ( 0 == lstrcmpi ( szDisplayName, 
                                                 szDriveToExpand ))
                                {
                                bDriveFound = TRUE;
                                break;
                                }
                            }
                        }

                    if ( bDriveFound )
                        {
                        Expand( pChildNode->m_hItem, TVE_EXPAND );
                        TreeView_SelectItem( m_hWnd, pChildNode->m_hItem );
                        }

                    break;
                }
            }

            pSHMalloc->Free( pidlMyComputer );
        }

        // Success, so prevent pRootNode from being deleted
        pRootNode = NULL;
        nReturn = 0;
    }

BailOut:
    if ( NULL != pRootNode )
        delete pRootNode;

    if ( NULL != pSHMalloc )
        pSHMalloc->Release();

    return ( nReturn );
}


//*************************************************************************
// CNVBrowse::UpdateSelectedList()
//
// public
// int CNVBrowse::UpdateSelectedList( )
//
// This method updates the selected items stored in m_listSelectedItems.
//
// Returns: 
//      Number of selected items.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

int CNVBrowse::UpdateSelectedList()
{
    ASSERT( ::IsWindow( m_hWnd ) );

    m_listSelectedItems.clear();

    AddSelectedItems( GetRootItem() );

    return ( m_listSelectedItems.size() );
} // CNVBrowse::UpdateSelectedList()


//*************************************************************************
// CNVBrowse::AddSelectedItems()
//
// protected
// void CNVBrowse::AddSelectedItems(
//      HTREEITEM hItem )               [in] Item whose subtree to check.
//
// This method adds all checked items from the subtree identified by hItem  
// to the list of selected items.
// Calls itself recursively to get all checked items.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

void CNVBrowse::AddSelectedItems( HTREEITEM hItem )
{
    auto    CNVBrowseItem   browseItem;
    auto    CNVBrowseNode   *pNode;
    auto    LPTSTR          lpszName;
    auto    UINT            uState;
    auto    DWORD           dwNameLength;

    if ( NULL == hItem )
        return;

    // Scan all children for checked items.

    for ( hItem = GetChildItem( hItem );
          NULL != hItem;
          hItem = GetNextSiblingItem( hItem ) )
    {
        // Add item if checked or gray checked.

        uState = GetItemState( hItem, TVIS_STATEIMAGEMASK );
        
        if( ( INDEXTOSTATEIMAGEMASK( CHECKED ) == uState ) ||
            ( INDEXTOSTATEIMAGEMASK( GRAYCHECKED ) == uState ) )
        {
            pNode = (CNVBrowseNode *)GetItemData( hItem );

            dwNameLength = pNode->GetName( NULL, 0 );

            if ( dwNameLength )
            {
                lpszName = new TCHAR [ dwNameLength ];

                if ( lpszName )
                {
                    pNode->GetName( lpszName, dwNameLength );

                    browseItem.SetName( lpszName );
                    browseItem.SetSubDirs( INDEXTOSTATEIMAGEMASK( CHECKED ) == uState );

                    m_listSelectedItems.push_back( browseItem );

                    delete [] lpszName;
                }
            }
        }

        // If item was unchecked, or only gray-checked, we need to look at
        // its children.

        if ( INDEXTOSTATEIMAGEMASK( CHECKED ) != uState )
        {
            AddSelectedItems( hItem );
        }
    }
} // CNVBrowse::AddSelectedItems()


//***************************************************************************
// CNVBrowse::LocateImage()
//
// int CNVBrowse::LocateImage(const CIconID& iconID)
//  
// This routine finds the index of the specified icon in the tree's image list.
//
// Returns:
//      0 based index if image is found.
//      -1  if image is not currently in the tree's image list.
//***************************************************************************
// 01/08/1997 DALLEE created
//***************************************************************************

int CNVBrowse::LocateImage( CIconID& iconID )
{
    auto    list<CIconID>::iterator itIcons;

    auto    int     nIndex;
    auto    int     nImage;

    nImage   = -1;

    for ( itIcons = m_listIcons.begin(), nIndex = 0; 
          itIcons != m_listIcons.end();
          itIcons++, nIndex++ )
    {
        if ( iconID == *itIcons )
        {
            nImage = nIndex;
            break;
        }
    }

    return ( nImage );
}


//***************************************************************************
// CNVBrowse::AddImage()
//
// int CNVBrowse::AddImage( HICON hIcon, const CIconID& iconID )
//
// This routine adds an image to the tree control's image list and adds
// its identifier to the control's list of images.
//
// If the image is already in the image list, then a second copy will be added.
// The LocateImage() method should be called to ensure that duplicate images
// are not added to the image list.
// 
// Returns:
//      0 based index if image was successfully added.
//      -1 on failure.
//***************************************************************************
// 01/08/1997 DALLEE created
//***************************************************************************

int CNVBrowse::AddImage( HICON hIcon, CIconID& iconID )
{
    auto    int     nImageIndex;

    nImageIndex = ImageList_AddIcon( m_hImageList, hIcon );

    if ( -1 != nImageIndex )
    {
        m_listIcons.push_back( iconID );
    }

    return ( nImageIndex );
}


int CALLBACK CNVBrowse::CompareNodes(LPARAM lNode1, LPARAM lNode2, LPARAM lISF)
{
    auto    IShellFolder    *pISF;
    auto    CNVBrowseNode   *pNode1;
    auto    CNVBrowseNode   *pNode2;
    auto    HRESULT         hResult;
    auto    int             nReturn;

    pISF   = (IShellFolder *) lISF;
    pNode1 = (CNVBrowseNode *) lNode1;
    pNode2 = (CNVBrowseNode *) lNode2;

    hResult = pISF->CompareIDs( 0, pNode1->m_pIDL, pNode2->m_pIDL );

    if ( SUCCEEDED( hResult ) )
    {
        nReturn = (short int) SCODE_CODE( hResult );
    }
    else
    {
        // On error, return equivalent since we have no idea of sort order.
        nReturn = 0;
    }

    return ( nReturn );
}


//*************************************************************************
// CNVBrowse::CheckItem()
//
// protected
// void CNVBrowse::CheckItem(
//      HTREEITEM  hItem,               [in] Tree item to check.
//      STATEINDEX nCheckIndex )        [in] Checked state to set.
//
// This method checks or unchecks the specified tree item and all child
// items.
// Note: Only SGAO_FILESYSTEM type objects will be checked. Other objects
// will not display checkboxes - they're not scanable objects.
//
// nCheckIndex may be one of:
//      NONE,
//      UNCHECKED,
//      CHECKED,
//      GRAYCHECKED
//
// This method will also set the state of the item's parent based on
// the new state of the specified item and the state of the item's siblings.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 04/13/1998 DALLEE, created - header added.
//*************************************************************************

void CNVBrowse::CheckItem( HTREEITEM hItem, int nCheckIndex )
{
    auto    HTREEITEM       hParent;
    auto    HTREEITEM       hChild;
    auto    BOOL            bAllChildrenChecked;
    auto    UINT            uParentImageBits;
    auto    CNVBrowseNode   *pNode;

    pNode = ( CNVBrowseNode * ) GetItemData( hItem );

    ASSERT( pNode );

    if ( ( 0 == ( SFGAO_FILESYSTEM & pNode->m_ulAttr ) ) ||
         ( GetRootItem() == hItem ) )
    {
        return;
    }

    CheckItemAndChildren( hItem, nCheckIndex );

    // Now set correct check state of parent.

    hParent = GetParentItem( hItem );

    while ( NULL != hParent )
    {
        uParentImageBits = GetItemState( hParent, TVIS_STATEIMAGEMASK );

        if ( ( uParentImageBits == INDEXTOSTATEIMAGEMASK( CHECKED ) ) &&
             ( nCheckIndex != CHECKED ) )
        {
            // Parent is checked and we're unchecking or partially unchecking
            // the item. The parent should be changed to gray checked.

            SetItemState( hParent,
                          INDEXTOSTATEIMAGEMASK( GRAYCHECKED ),
                          TVIS_STATEIMAGEMASK );
        }
        else if ( ( uParentImageBits == INDEXTOSTATEIMAGEMASK( GRAYCHECKED ) ) &&
                  ( nCheckIndex == CHECKED ) )
        {
            // Parent is gray checked and we're checking the item. If all
            // siblings are also checked, parent should be checked.

            bAllChildrenChecked = TRUE;

            for ( hChild = GetChildItem( hParent );
                  hChild != NULL;
                  hChild = GetNextSiblingItem( hChild ) )
            {
                if ( GetItemState( hChild, TVIS_STATEIMAGEMASK ) != INDEXTOSTATEIMAGEMASK( CHECKED ) )
                {
                    bAllChildrenChecked = FALSE;
                    break;
                }
            }

            if ( bAllChildrenChecked )
            {
                SetItemState( hParent,
                              INDEXTOSTATEIMAGEMASK( CHECKED ),
                              TVIS_STATEIMAGEMASK );
            }
        }
        else
        {
            // No change to parent, so no need to check changes to parent's parent.

            break;
        }

        hParent = GetParentItem( hParent );
    }

    Validate ();
    
} // CNVBrowse::CheckItem()


//*************************************************************************
// CNVBrowse::CheckItemAndChildren()
//
// protected
// void CNVBrowse::CheckItemAndChildren(
//      HTREEITEM  hItem,               [in] Tree item to check.
//      STATEINDEX nCheckIndex )        [in] Checked state to set.
//
// This method checks or unchecks the specified tree item and recursively
// checks all child items.
//
// Note: This method should only be called by CheckItem(). CheckItem() should
// be used to set the checked state of an item - it will correctly set
// the item, the item's children, and the state of the item's parent.
//
// Note 2: Only SGAO_FILESYSTEM type objects will be checked. Other objects
// will not display checkboxes - they're not scanable objects.
//
// nCheckIndex may be one of:
//      NONE,
//      UNCHECKED,
//      CHECKED,
//      GRAYCHECKED
//
// Returns: 
//      Nothing.
//*************************************************************************
// 04/13/1998 DALLEE, created - header added.
//*************************************************************************

void CNVBrowse::CheckItemAndChildren( HTREEITEM hItem, int nCheckIndex )
{
    auto    HTREEITEM       hChild;
    auto    CNVBrowseNode   *pNode;

    // Check item.

    pNode = ( CNVBrowseNode * ) GetItemData( hItem );

    ASSERT( pNode );

    if ( SFGAO_FILESYSTEM & pNode->m_ulAttr )
    {
        SetItemState( hItem, 
                      INDEXTOSTATEIMAGEMASK( nCheckIndex ),
                      TVIS_STATEIMAGEMASK );
    }

    // Recursively check all children.

    for ( hChild = GetChildItem( hItem ); 
          hChild != NULL;
          hChild = GetNextSiblingItem( hChild ) )
    {
        CheckItemAndChildren( hChild, nCheckIndex );
    }
} // CNVBrowse::CheckItemAndChildren()


CNVBrowse::GetTotalSelectedItems()
{
    return ( m_listSelectedItems.size() );
}

/////////////////////////////////////////////////////////////////////////////
// CNVBrowseNode

CNVBrowseNode::CNVBrowseNode()
{
    m_hItem      = NULL;
    m_pISF       = NULL;
    m_pParentISF = NULL;
    m_pTree      = NULL;
    m_pIDL       = NULL;
    m_ulAttr     = 0;
}

CNVBrowseNode::~CNVBrowseNode()
{
    auto    IMalloc     *pSHMalloc;

    if ( ( NULL != m_pIDL ) &&
         SUCCEEDED( ::SHGetMalloc( &pSHMalloc ) ) )
    {
        pSHMalloc->Free( m_pIDL );
        pSHMalloc->Release();
    }

    if ( NULL != m_pISF )
        m_pISF->Release();

    if ( NULL != m_pParentISF )
        m_pParentISF->Release();
}

int CNVBrowseNode::AddChildren()
{
    auto    int             nReturn;
    auto    IEnumIDList     *pEnum;
    auto    LPITEMIDLIST    pIDL;
    auto    CNVBrowseNode   *pNode;
    auto    IMalloc         *pSHMalloc;
    auto    HCURSOR         hOldCursor;
    auto    BOOL            bHasChildren;

    auto    STRRET          rStrRet;
    auto    TCHAR           szDisplayName [ MAX_PATH ];

    auto    int             nImage;
    auto    int             nSelectedImage;
    auto    int             nStateImage;
            bool            bIsOSXP = false;

    pSHMalloc    = NULL;
    nReturn      = -1;
    pEnum        = NULL;
    pNode        = NULL;
    pIDL         = NULL;
    hOldCursor   = NULL;
    bHasChildren = FALSE;

	// Get windows version info.
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &vi );
	
	if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion == 5 && vi.dwMinorVersion >= 1 )
        bIsOSXP = true;

    // Get shell's malloc
    if ( FAILED( ::SHGetMalloc( &pSHMalloc ) ) )
        goto BailOut;
    
    if ( ( NULL == m_pISF ) || ( NULL == m_pTree ) )
        goto BailOut;

    if ( FAILED ( m_pISF->EnumObjects( m_pTree->m_hWnd,
                                       SHCONTF_FOLDERS | SHCONTF_INCLUDEHIDDEN,
                                       &pEnum ) ) )
        goto BailOut;

    if ( NULL == ( hOldCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) ) ) )
        hOldCursor = LoadCursor( NULL, IDC_ARROW );

    // Get state image for children.

    nStateImage = ( INDEXTOSTATEIMAGEMASK( CNVBrowse::CHECKED ) ==
                    m_pTree->GetItemState( m_hItem, TVIS_STATEIMAGEMASK )
                    ? CNVBrowse::CHECKED : CNVBrowse::UNCHECKED );

    while ( NOERROR == pEnum->Next( 1, &pIDL, NULL ) ) 
    {
        auto    TV_INSERTSTRUCT rTreeInsert;

        memset( &rTreeInsert, 0, sizeof( rTreeInsert ) );
        memset( &rStrRet, 0, sizeof( rStrRet ) );

        if ( NULL == ( pNode = new CNVBrowseNode ) )
            goto BailOut;

        pNode->m_pIDL = pIDL;
        pIDL = NULL;

        pNode->m_pTree      = m_pTree;

        pNode->m_pParentISF = m_pISF;
        m_pISF->AddRef();

        if ( FAILED( m_pISF->BindToObject( pNode->m_pIDL, 
                                           NULL, 
                                           IID_IShellFolder, 
                                           (LPVOID *)&pNode->m_pISF ) ) )
            goto BailOut;

        // Get folder attributes (find out if it is a filesystem object and
        // whether or not it has children).
        // We used to not check for SFGAO_READONLY since this will ping the device
        // (floppy/CD-ROM). However we really want to do that now. It gives the user 
        // access to floppy sub folders if they put a disk in after they launch
        // the dialog.

        pNode->m_ulAttr = (UINT)-1; // & ~SFGAO_READONLY;

        if ( FAILED( m_pISF->GetAttributesOf( 1, (LPCITEMIDLIST *)&pNode->m_pIDL, &pNode->m_ulAttr ) ) )
            pNode->m_ulAttr = 0;

        if ( 0 == ( ( SFGAO_FILESYSANCESTOR | SFGAO_FILESYSTEM) & pNode->m_ulAttr ) )
        {

            // On Windows XP if a removable drive (floppy, CD-ROM on laptops, ZIP drives) doesn't
            // have a disk in it the attributes are 0. Since we want to display this device
            // even though it doesn't have a disk in it, we look for this case and allow it.
            //
            if ( 0 == pNode->m_ulAttr && bIsOSXP )
            {
                pNode->m_ulAttr = ( SFGAO_FILESYSTEM |
                                    SFGAO_REMOVABLE |
                                    SFGAO_FILESYSANCESTOR |
                                    SFGAO_HASSUBFOLDER );

            }
            else
            {
                delete pNode;
                pNode = NULL;
                continue;
            }
        }

        // Get display name of node

        if ( FAILED( m_pISF->GetDisplayNameOf( pNode->m_pIDL,
                                               SHGDN_INFOLDER,
                                               &rStrRet ) ) )
        {
            delete pNode;
            pNode = NULL;
            continue;
        }

        szDisplayName[ 0 ] = _T('\0');
        szDisplayName[ MAX_PATH - 1 ] = _T('\0');

        StrretToLPTSTR ( rStrRet, szDisplayName, pNode->m_pIDL,
                         pSHMalloc );

        // Get images for this node and register them with the associated
        // tree control's image list.

        nImage         = pNode->RegisterImage( pNode->m_pIDL, GIL_FORSHELL );
        nSelectedImage = pNode->RegisterImage( pNode->m_pIDL, GIL_FORSHELL | GIL_OPENICON );

        if ( -1 == nImage )
            nImage = 0;
        if ( -1 == nSelectedImage )
            nSelectedImage = 0;

        // Insert node into tree control.
        // Note: only adding checkbox state image for filesystem objects.

        rTreeInsert.hParent             = m_hItem;
        rTreeInsert.hInsertAfter        = TVI_LAST;
        rTreeInsert.item.mask           = TVIF_PARAM | TVIF_TEXT | TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE;
        rTreeInsert.item.state          = INDEXTOSTATEIMAGEMASK( ( pNode->m_ulAttr & SFGAO_FILESYSTEM ) ? nStateImage : 0 );
        rTreeInsert.item.stateMask      = TVIS_STATEIMAGEMASK;
        rTreeInsert.item.pszText        = szDisplayName;
        rTreeInsert.item.iImage         = nImage;
        rTreeInsert.item.iSelectedImage = nSelectedImage;
        rTreeInsert.item.cChildren      = ( pNode->m_ulAttr & SFGAO_HASSUBFOLDER ) ? 1 : 0;
        rTreeInsert.item.lParam         = (LPARAM)pNode;

        if ( NULL != ( pNode->m_hItem = m_pTree->InsertItem( &rTreeInsert ) ) )
        {
            // Success, so prevent pNode from being deleted.
            pNode = NULL;
            nReturn = 0;

            // Remeber if at least one child really exists.
            bHasChildren = TRUE;
        }
    } 

BailOut:
    if ( bHasChildren )
    {
        // If we added children, sort them.
        auto    TV_SORTCB   rSort;

        rSort.hParent     = m_hItem;
		rSort.lpfnCompare = &CNVBrowse::CompareNodes;
		rSort.lParam      = (LPARAM) m_pISF;

        m_pTree->SortChildrenCB( &rSort );
    }
    else
    {
        // If no children, then remove child setting for network drives.
        //
        // IShellFolder.GetAttributesOf automatically returns SGFAO_HASCHILDREN
        // for network folders and removable media to avoid pinging the device.
        //
        // Not doing this for removable drives, since user may want to insert
        // a disk after pinging once.

        if ( !( SFGAO_REMOVABLE & m_ulAttr ) )
        {
            auto    TV_ITEM     rTreeItem;

            memset( &rTreeItem, 0, sizeof( rTreeItem ) );

            rTreeItem.hItem     = m_hItem;
            rTreeItem.mask      = TVIF_CHILDREN;
            rTreeItem.cChildren = 0;

            m_pTree->SetItem( &rTreeItem );
        }
    }

    if ( NULL != hOldCursor )
        SetCursor( hOldCursor );

    if ( NULL != pSHMalloc )
    {
        if ( NULL != pIDL )
            pSHMalloc->Free( pIDL );

        pSHMalloc->Release();
    }

    if ( NULL != pEnum )
        pEnum->Release();

    if ( NULL != pNode )
        delete pNode;

    return ( nReturn );
}

//***************************************************************************
// CNVBrowseNode::RegisterImage()
//
// int CNVBrowseNode::RegisterImage( LPITEMIDLIST pIDL )
//
// This routine gets the location of an item's images,
// ensures it is added to the associated tree control's image list
// and returns the index of the item in the image list.
//
// Parameters:
//      LPITEMIDLIST    pIDL    Identifier of shell item
//      UINT            uFlags  Flags to indicate image to retrieve. 
//                              A combination of:  GIL_FORSHELL
//                                                 GIL_OPENICON
// 
// Returns:
//      0 based index if image was successfully added.
//      -1 on failure.
//***************************************************************************
// 01/08/1997 DALLEE created
//***************************************************************************

int CNVBrowseNode::RegisterImage( LPITEMIDLIST pIDL,  UINT uFlags )
{
    auto    IShellIcon      *pISI;
    auto    IExtractIcon    *pIEI;
    auto    CIconID         iconID;
    auto    UINT            uImageFlags;
    auto    int             nImageIndex;
    auto    HICON           hLargeIcon;
    auto    HICON           hIcon;
    auto    BOOL            bSystemImage;

    auto    TCHAR           szIconSource [ MAX_PATH ];
    auto    int             nIconIndex;

    pISI            = NULL;
    pIEI            = NULL;
    nImageIndex     = -1;
    bSystemImage    = FALSE;
    szIconSource[0] = _T('\0');

    // Get icon location.
    // Use IShellIcon to check for icon in system image list first. 
    // If not available, then use parent's IExtractIcon.

    if ( SUCCEEDED( m_pISF->QueryInterface( IID_IShellIcon, (LPVOID *)&pISI ) ) &&
         ( NOERROR == pISI->GetIconOf( pIDL, uFlags, &nIconIndex ) ) )
    {
        _tcscpy( szIconSource, _T("System Image\0IPE") );
        bSystemImage = TRUE;
    }
    else if ( SUCCEEDED( m_pParentISF->GetUIObjectOf( m_pTree->m_hWnd,
                                                      1,
                                                      (LPCITEMIDLIST *)&pIDL,
                                                      IID_IExtractIcon,
                                                      NULL,
                                                      (LPVOID *)&pIEI ) ) )
    {
        pIEI->GetIconLocation( uFlags,
                               szIconSource,
                               MAX_PATH,
                               &nIconIndex,
                               &uImageFlags );
        //&? Check HRESULT! ^^^
    }
    else
    {
        goto BailOut;
    }

    iconID.SetID( szIconSource, nIconIndex );

    // Check if image has already been loaded.

    nImageIndex = m_pTree->LocateImage( iconID );

    // If the image was not in the list, we need to extract it and add it.

    if ( -1 == nImageIndex )
    {
        if ( bSystemImage )
        {
            auto    SHFILEINFO      rSHInfo;
            auto    UINT            uSHFlags;

            memset( &rSHInfo, 0, sizeof( rSHInfo ) );

            uSHFlags = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_PIDL;
            if ( GIL_OPENICON & uFlags )
                uSHFlags |= SHGFI_OPENICON;

            ::SHGetFileInfo( (LPCTSTR)pIDL, FILE_ATTRIBUTE_DIRECTORY, &rSHInfo, sizeof( rSHInfo ), uSHFlags );
            hIcon = rSHInfo.hIcon;
        }
        else if ( GIL_NOTFILENAME & uImageFlags )
        {
            pIEI->Extract( iconID.m_pszSource, 
                           iconID.m_nIndex, 
                           &hLargeIcon, 
                           &hIcon,  
                           MAKELONG( GetSystemMetrics( SM_CXICON ),
                                     GetSystemMetrics( SM_CXSMICON ) ) );
        }
        else
        {
            ::ExtractIconEx( iconID.m_pszSource, iconID.m_nIndex, NULL, &hIcon, 1 );
        }

        nImageIndex = m_pTree->AddImage( hIcon, iconID );
        ::DestroyIcon ( hIcon );
    }

BailOut:
    if ( NULL != pISI )
        pISI->Release();

    if ( NULL != pIEI )
        pIEI->Release();

    return ( nImageIndex );
}


//*************************************************************************
// CNVBrowseNode::GetName()
//
// DWORD CNVBrowseNode::GetName(
//      LPTSTR  lpszName,               [out] Buffer to receive name.
//      DWORD   cchBuffer )             [in] Size of lpszName in characters.
//
// This method returns the parsing name for a give tree node.
// This function may be called with cchBuffer == 0 and lpszName == NULL
// to determine the size of the buffer required for lpszLong.
//
// Returns: 
//  On success:
//      The number of characters copied to lpszName excluding EOS.
//  On failure:
//      If the buffer is too small, the return value is the size of the
//      buffer required in characters including the EOS.
//      The return value is 0 for all other errors.
//*************************************************************************
// 04/17/1998 DALLEE, created - header added.
//*************************************************************************

DWORD CNVBrowseNode::GetName( LPTSTR lpszName, DWORD dwBufferSize )
{
    auto    STRRET  rStrRet;
    auto    IMalloc *pSHMalloc;
    auto    DWORD   dwLength;

    ASSERT( dwBufferSize ? NULL != lpszName : TRUE );

    memset( &rStrRet, 0, sizeof( rStrRet ) );
    pSHMalloc = NULL;
    dwLength  = 0;

    // Get shell's malloc
    if ( FAILED( ::SHGetMalloc( &pSHMalloc ) ) )
        goto BailOut;

    if ( FAILED( m_pParentISF->GetDisplayNameOf( m_pIDL,
                                                 SHGDN_FORPARSING,
                                                 &rStrRet ) ) )
        goto BailOut;

#ifndef _UNICODE
    switch ( rStrRet.uType )
    {
    case STRRET_CSTR:
        dwLength = _tcslen( rStrRet.cStr );

        if ( dwLength < dwBufferSize )
        {
            _tcscpy( lpszName, rStrRet.cStr );
        }
        else
        {
            // Buffer not large enough. Add one to string length to
            // get size of buffer required 

            dwLength++;
        }
        break;

    case STRRET_OFFSET:
        dwLength = _tcslen( ( LPCSTR )( (LPBYTE)m_pIDL + rStrRet.uOffset ) );

        if ( dwLength < dwBufferSize )
        {
            _tcscpy( lpszName, ( LPCSTR )( (LPBYTE)m_pIDL + rStrRet.uOffset ) );
        }
        else
        {
            // Buffer not large enough. Add one to string length to
            // get size of buffer required 

            dwLength++;
        }
        break;

    case STRRET_WSTR:
        dwLength = WideCharToMultiByte( CP_ACP, 0,
                                        rStrRet.pOleStr,
                                        -1,
                                        lpszName,
                                        dwBufferSize,
                                        NULL, NULL );

        if ( 0 == dwBufferSize )
        {
            // This was a request to get required buffer size.
            // No need to do anything more.
        }
        else if ( dwLength )
        {
            // Success... but WideCharToMultiByte()'s return value includes
            // EOS, this function doesn't count EOS on success.

            dwLength--;
        }
        else if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
        {
            dwLength = WideCharToMultiByte( CP_ACP, 0,
                                            rStrRet.pOleStr,
                                            -1,
                                            lpszName,
                                            0,
                                            NULL, NULL );
        }

        pSHMalloc->Free( rStrRet.pOleStr );
        break;
    }
#else
    switch ( rStrRet.uType )
    {
    case STRRET_CSTR:
        dwLength = MultiByteToWideChar( CP_ACP, 0,
                                        rStrRet.cStr,
                                        -1,
                                        lpszName,
                                        dwBufferSize );

        if ( 0 == dwBufferSize )
        {
            // This was a request to get required buffer size.
            // No need to do anything more.
        }
        else if ( dwLength )
        {
            // MultiByteToWideChar()'s return value includes EOS, this function
            // doesn't count EOS on success.

            dwLength--;
        }
        else if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
        {
            dwLength = MultiByteToWideChar( CP_ACP, 0,
                                            rStrRet.cStr,
                                            -1,
                                            lpszName,
                                            0 );
        }
        break;

    case STRRET_OFFSET:
        dwLength = MultiByteToWideChar( CP_ACP, 0,
                                        ( LPCSTR )( (LPBYTE)m_pIDL + rStrRet.uOffset ),
                                        -1,
                                        lpszName,
                                        dwBufferSize );

        if ( 0 == dwBufferSize )
        {
            // This was a request to get required buffer size.
            // No need to do anything more.
        }
        else if ( dwLength )
        {
            // MultiByteToWideChar()'s return value includes EOS, this function
            // doesn't count EOS on success.

            dwLength--;
        }
        else if ( ERROR_INSUFFICIENT_BUFFER == GetLastError() )
        {
            dwLength = MultiByteToWideChar( CP_ACP, 0,
                                            ( LPCSTR )( (LPBYTE)m_pIDL + rStrRet.uOffset ),
                                            -1,
                                            lpszName,
                                            0 );
        }
        break;

        break;

    case STRRET_WSTR:
        dwLength = _tcslen( rStrRet.pOleStr );

        if ( dwLength < dwBufferSize )
        {
            _tcscpy( lpszName, rStrRet.pOleStr );
        }
        else
        {
            // Buffer not large enough. Add one to string length to
            // get size of buffer required 

            dwLength++;
        }

        pSHMalloc->Free( rStrRet.pOleStr );
        break;
    }
#endif

BailOut:
    if ( NULL != pSHMalloc)
        pSHMalloc->Release();

    return ( dwLength );
}

/////////////////////////////////////////////////////////////////////////////
// CNVBrowseItem - selected item from browse control.

//*************************************************************************
// CNVBrowseItem::CNVBrowseItem()
//
// CNVBrowseItem::CNVBrowseItem( )
//
// Returns: 
//      Nothing.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

CNVBrowseItem::CNVBrowseItem()
{
    m_lpszName = NULL;
    m_bSubDirs = FALSE;
} // CNVBrowseItem::CNVBrowseItem()

CNVBrowseItem::CNVBrowseItem(const CNVBrowseItem &item)
{
    m_lpszName = NULL;
    m_bSubDirs = FALSE;

    SetName( item.GetName() );
    SetSubDirs( item.GetSubDirs() );
} // CNVBrowseItem::CNVBrowseItem()


//*************************************************************************
// CNVBrowseItem::~CNVBrowseItem()
//
// public virtual
// CNVBrowseItem::~CNVBrowseItem( )
//
// Returns: 
//      Nothing.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

CNVBrowseItem::~CNVBrowseItem()
{
    if ( m_lpszName )
    {
        delete [] m_lpszName;
        m_lpszName = NULL;
    }
} // CNVBrowseItem::~CNVBrowseItem()

//*************************************************************************
// CNVBrowseItem::SetName()
//
// public
// BOOL CNVBrowseItem::SetName(
//      LPCTSTR lpszName )
//
// This method sets the name of the item. The contents of lpszName are
// copied to internal storage. lpszName can be 
//
// Returns: 
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVBrowseItem::SetName( LPCTSTR lpszName )
{
    auto    BOOL    bSuccess;

    ASSERT( lpszName );

    if ( m_lpszName )
    {
        delete [] m_lpszName;
    }

    m_lpszName = new TCHAR [ _tcslen( lpszName ) + 1 ];

    if ( m_lpszName )
    {
        _tcscpy( m_lpszName, lpszName );
        bSuccess = TRUE;
    }
    else
    {
        bSuccess = FALSE;
    }

    return ( bSuccess );
} // CNVBrowseItem::SetName()


//*************************************************************************
// CNVBrowseItem::GetName()
//
// public const
// LPCTSTR CNVBrowseItem::GetName( )
//
// This method retrieves a pointer to the name of the item.
//
// Returns: 
//      Pointer to name of item on success. NULL on error.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

LPCTSTR CNVBrowseItem::GetName() const
{
    return ( m_lpszName );
} // CNVBrowseItem::GetName()


//*************************************************************************
// CNVBrowseItem::SetSubDirs()
//
// BOOL CNVBrowseItem::SetSubDirs(
//      BOOL bSubDirs )
//
// This method sets the include subdirectories flag of the item.
//
// Returns: 
//      Old setting of subdirectories flag.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVBrowseItem::SetSubDirs( BOOL bSubDirs )
{
    auto    BOOL    bOldSubDirs;

    bOldSubDirs = m_bSubDirs;
    m_bSubDirs  = bSubDirs;

    return ( bOldSubDirs );
} // CNVBrowseItem::SetSubDirs()


//*************************************************************************
// CNVBrowseItem::GetSubDirs()
//
// BOOL CNVBrowseItem::GetSubDirs( )
//
// This method retrieves the setting of the include subdirectories flag.
//
// Returns: 
//      Setting of include subdirectories flag.
//*************************************************************************
// 04/20/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CNVBrowseItem::GetSubDirs() const
{
    return ( m_bSubDirs );
} // CNVBrowseItem::GetSubDirs()

bool CNVBrowseItem::operator==(const CNVBrowseItem& item2) const
{
    return ( ( GetSubDirs() == item2.GetSubDirs() ) &&
             ( GetName() ) &&
             ( item2.GetName() ) &&
             ( 0 == _tcsicmp( GetName(), item2.GetName() ) ) );
}

bool CNVBrowseItem::operator!=(const CNVBrowseItem& item2) const
{
    return ( !operator==( item2 ) );
}

bool CNVBrowseItem::operator>(const CNVBrowseItem& item2) const
{
    // Relation of two objects. 
    // 0 is equal.
    // <0 if *this < iconID2
    // >0 if *this > iconID2

    auto    int     nRelation;

    // First check names.

    if ( GetName() && item2.GetName() )
    {
        nRelation = _tcsicmp( GetName(), item2.GetName() );
    }
    else if ( GetName() )
    {
        nRelation = 1;
    }
    else if ( item2.GetName() )
    {
        nRelation = -1;
    }
    else
    {
        nRelation = 0;
    }

    // If equivalent so far, item with subdirs flag will be greater than
    // item w/o.

    if ( 0 == nRelation )
    {
        nRelation = GetSubDirs() - item2.GetSubDirs();
    }

    return ( nRelation > 0 );
}

bool CNVBrowseItem::operator<(const CNVBrowseItem& item2) const
{
    // Relation of two objects. 
    // 0 is equal.
    // <0 if *this < iconID2
    // >0 if *this > iconID2

    auto    int     nRelation;

    // First check names.

    if ( GetName() && item2.GetName() )
    {
        nRelation = _tcsicmp( GetName(), item2.GetName() );
    }
    else if ( GetName() )
    {
        nRelation = 1;
    }
    else if ( item2.GetName() )
    {
        nRelation = -1;
    }
    else
    {
        nRelation = 0;
    }

    // If equivalent so far, item with subdirs flag will be greater than
    // item w/o.

    if ( 0 == nRelation )
    {
        nRelation = GetSubDirs() - item2.GetSubDirs();
    }

    return ( nRelation < 0 );
}


/////////////////////////////////////////////////////////////////////////////
// CIconID

CIconID::CIconID()
{
    m_nIndex    = 0;
    m_pszSource = NULL;
}

CIconID::CIconID(const CIconID& iconID)
{
    m_nIndex    = 0;
    m_pszSource = NULL;

    SetID( iconID.m_pszSource, iconID.m_nIndex );
}

CIconID::CIconID(LPCTSTR pszSource, int nIndex)
{
    m_nIndex    = 0;
    m_pszSource = NULL;

    SetID( pszSource, nIndex );
}

CIconID::~CIconID()
{
    if ( m_pszSource )
    {
        delete [] m_pszSource;
        m_pszSource = NULL;
    }
}

int CIconID::SetID( LPCTSTR pszSource, int nIndex )
{
    ASSERT( pszSource );

    // Remove old ID, if any.

    if ( m_pszSource )
        delete [] m_pszSource;

    // Set new source and icon index.

    m_pszSource = new TCHAR [ _tcslen( pszSource ) + 1 ];

    if ( m_pszSource )
    {
        _tcscpy( m_pszSource, pszSource );
    }

    m_nIndex = nIndex;

    return ( 0 );
}

const CIconID& CIconID::operator=(const CIconID& iconIDSrc)
{
    SetID( iconIDSrc.m_pszSource, iconIDSrc.m_nIndex );

    return ( *this );
}

bool CIconID::operator==(const CIconID& iconID2) const
{
    return ( ( m_nIndex == iconID2.m_nIndex ) &&
             ( m_pszSource ) &&
             ( iconID2.m_pszSource ) &&
             ( 0 == _tcsicmp( m_pszSource, iconID2.m_pszSource ) ) );
}

bool CIconID::operator!=(const CIconID& iconID2) const
{
    return ( !operator==( iconID2 ) );
}

bool CIconID::operator>(const CIconID& iconID2) const
{
    // Relation of two objects. 
    // 0 is equal.
    // <0 if *this < iconID2
    // >0 if *this > iconID2

    auto    int     nRelation;

    // First check sources.

    if ( m_pszSource && iconID2.m_pszSource )
    {
        nRelation = _tcsicmp( m_pszSource, iconID2.m_pszSource );
    }
    else if ( m_pszSource )
    {
        nRelation = 1;
    }
    else if ( iconID2.m_pszSource )
    {
        nRelation = -1;
    }
    else
    {
        nRelation = 0;
    }

    // If equivalent so far, check icon indexes.

    if ( 0 == nRelation )
    {
        if ( m_nIndex < iconID2.m_nIndex )
        {
            nRelation = -1;
        }
        else if ( m_nIndex > iconID2.m_nIndex )
        {
            nRelation = 1;
        }
    }

    return ( nRelation > 0 );
}

bool CIconID::operator<(const CIconID& iconID2) const
{
    // Relationship of two objects. 
    // 0 is equal.
    // <0 if *this < iconID2
    // >0 if *this > iconID2

    auto    int     nRelation;

    // First check image sources.

    if ( m_pszSource && iconID2.m_pszSource )
    {
        nRelation = _tcsicmp( m_pszSource, iconID2.m_pszSource );
    }
    else if ( m_pszSource )
    {
        nRelation = 1;
    }
    else if ( iconID2.m_pszSource )
    {
        nRelation = -1;
    }
    else
    {
        nRelation = 0;
    }

    // If equivalent so far, check icon indexes.

    if ( 0 == nRelation )
    {
        if ( m_nIndex < iconID2.m_nIndex )
        {
            nRelation = -1;
        }
        else if ( m_nIndex > iconID2.m_nIndex )
        {
            nRelation = 1;
        }
    }

    return ( nRelation < 0 );
}


void CNVBrowse::SetValidatingFunction( IValidate* pValidateSink )
{
    if ( pValidateSink )
        m_pValidateSink = pValidateSink;
}

void CNVBrowse::Validate()
{
    if ( m_pValidateSink )
        m_pValidateSink -> OnItemChanged ();       
}
