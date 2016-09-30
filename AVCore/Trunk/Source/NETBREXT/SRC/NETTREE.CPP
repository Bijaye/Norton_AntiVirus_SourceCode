/////////////////////////////////////////////////////////////////////
// NETTREE.CPP - Created - 11/20/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/nettree.cpv   1.2   07 Jan 1998 15:50:50   DBuches  $
//
// Description:
//      Network Browser Tree Contol class implementation file.
//    This class is a class derived from a CTreeCtrl.  It will be
//    responsable for enumerating the network and filling the tree
//    control with those enumerated item.  This was made into it's
//    own class so that it would not be tied into the dialog that
//    uses it.  In this fashion, we can drop this control into
//    any other dialog seamlessly without having to worry about the
//    ties to the current one.
//      This class is, however, dependent upon another class.  That
//    class is the CNetResrouceContainer class.  This class is used
//    in all of the branches of this Network Tree Class.  Each branch
//    has one of these classes allocated as the extra data member.
//    this design was neccessary in order to do just-in-time network
//    enumeration only when you expand a particular branch.  The
//    Branch container saves such state information as the current
//    network enumeration position, and the buffer for the Network
//    enumeration API call.
//
//    See Also : NResCont.h for definition of the Netowrk Resoruce
//                   container class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/nettree.cpv  $
// 
//    Rev 1.2   07 Jan 1998 15:50:50   DBuches
// Added OnChar handler so Spacebar now checks and unchecks items.
//
//    Rev 1.1   07 Jan 1998 15:44:02   DBuches
// Added OnChar handler so Spacebar now checks and unchecks items.
//
//    Rev 1.0   21 Nov 1997 11:51:58   KSACKIN
// Initial revision.
//
//    Rev 1.4   08 Aug 1997 12:46:54   KSACKIN
// Set the default for using the network browser tree to be FALSE.
//
//    Rev 1.3   31 Jul 1997 12:53:40   KSACKIN
// Turned off the redraw for the network browser window while retrieving the
// selected selections.
//
//    Rev 1.2   30 Jul 1997 16:09:30   KSACKIN
// Added support for a Multi-Select network browsing tree.
//
//    Rev 1.1   14 May 1997 16:11:50   KSACKIN
// Set the defaults for the CNetworkTreeControl class to browse into everything
// .  Before, it defaulted to browse into nothing, so you would get an empty
// tree.
//
//    Rev 1.0   18 Apr 1997 14:02:56   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:16:30   RFULLER
// Initial revision
//
//    Rev 1.3   16 Dec 1996 23:22:14   ksackin
// Modified some functionality for the new Novell 32 bit client.  This involved
// verifying some strings a little differently and handling another level of
// indirection.
//
//    Rev 1.2   09 Dec 1996 23:44:14   ksackin
// Changed a function dealing with enumerating a resource branch.  We had a
// stipulation that every item that we added needed to be a resource container
// when in actuality, we may wish to show such things as shares.
//
//    Rev 1.1   05 Dec 1996 01:59:14   ksackin
// Added defines for network resoruce types, and now reference a DWORD of flags
// to determine if items should be displayed, browsed beneath, or are valid
// selection items.
//
//    Rev 1.0   20 Nov 1996 01:49:10   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "NetBrStr.h"                   // String Resources
#include "NetTree.h"                    // CNetTreeCtrl Class definition.
#include "NResCont.h"                   // for the CNetResourceContainer class.
#include "DllRCSta.h"                   // DLL Resource State Switcher Class

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::CNetworkTreeCtrl
//
// Purpose:
//    Constructor for the CNetworkTreeCtrl class.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/05/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetworkTreeCtrl::CNetworkTreeCtrl()
{
    SetValidSelectionFlags( RESOURCE_TYPE_ALL );
    SetBrowseBeneathFlags( RESOURCE_TYPE_ALL );
    SetDisplayItemFlags( RESOURCE_TYPE_ALL );

    m_bUseMultiSelect = FALSE;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::CNetworkTreeCtrl
//
// Purpose:
//    Destructor for the CNetworkTreeCtrl class.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/05/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetworkTreeCtrl::~CNetworkTreeCtrl()
{
                                        // Most memory for this tree will be
                                        // released when the WM_DESTROY message
                                        // is sent.  This can be for any handling
                                        // when this class is destroyed.  The
                                        // reason for this is that the base tree
                                        // class for this tree is alredy destroyed
                                        // at this time, where as at the WM_DESTROY
                                        // message, it is not.
}

/////////////////////////////////////////////////////////////////////
// Afx Message Map Declaration.

BEGIN_MESSAGE_MAP(CNetworkTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CNetworkTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
    ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::InitTreeControl
//
// Purpose:
//    Function to initialize the tree control member of this dialog.
//    This funciton will create the first branch of the tree control,
//    and then enumerate the first level of the network.  This
//    function will also create the image list for the tree control.
//
// Parameters:
//    None.
//
// Returns:
//    BOOLEAN as to whether the tree could be initialized or not.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::InitTreeControl( void )
{
CNetResourceContainer *lpcEntireNetwork;
CImageList *lpImageList;
HTREEITEM hEntireNetworkBranch;
CString cszTempStr;
BOOL bRetVal = TRUE;
TV_INSERTSTRUCT tvIS;

	                                    // Switch resource state to this module.
    CDLLResourceState switchState;

                                        // Create a new image list class.
    lpImageList = new CImageList();


                                        // If we were able to create the image
                                        // list...
    if ( lpImageList )
        {
                                        // Now we are going to load our
                                        // tree control's bitmap for it's
                                        // image list.
        if ( lpImageList->Create( IDBM_NETWORK_TREE_IMAGE_LIST,
                                  16,        // width of each item.
                                  10,        // Numer of items this list can grow.
                                  RGB( 255, 255, 255 ) ) )  // Transparent color.
            {
                                        // Set this image list as the tree
                                        // control's image list.
            SetImageList( lpImageList, TVSIL_NORMAL  );
            }
        }

                                        // If we wish this to be a multi-select
                                        // network browser, set up the selected
                                        // image state list.
    if ( TRUE == m_bUseMultiSelect )
        {
        if ( m_xCheckImages.Create( IDB_CHECKS, 16, 16*5, RGB(255,0,255)) )
            {
            SetImageList( &m_xCheckImages, TVSIL_STATE  );
            }
        }

                                        // Create the first resource container
                                        // that is going to encase the "Entire
                                        // Network" item.
    lpcEntireNetwork = new CNetResourceContainer( NULL );

                                        // If we got one...
    if ( NULL != lpcEntireNetwork )
        {
                                        // Attempt to load the string for the
                                        // entire network.
        if ( cszTempStr.LoadString( IDT_ENTIRE_NETWORK ) )
            {
                                        // Add that string to the tree as the
                                        // base item.
            tvIS.hParent = NULL;
            tvIS.hInsertAfter = TVI_ROOT;
            tvIS.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_CHILDREN | TVIF_SELECTEDIMAGE |TVIF_STATE;
            tvIS.item.stateMask = TVIS_USERMASK;
            tvIS.item.state = TVIS_EXPANDED|INDEXTOSTATEIMAGEMASK(NOT_CHECKED);
            tvIS.item.pszText = cszTempStr.GetBuffer(0);
            tvIS.item.cchTextMax = cszTempStr.GetLength();
            tvIS.item.iImage = 0;
            tvIS.item.iSelectedImage = 0;
            tvIS.item.cChildren = 1;
            tvIS.item.lParam = NULL;
                                        // Insert that item with a pointer to the created
                                        // CNetResourceContainer class.  This will be
                                        // used later for further enumeration.
            hEntireNetworkBranch = InsertItem( &tvIS );
//            hEntireNetworkBranch = InsertItem( cszTempStr,
//                                               0,
//                                               0,
//                                               TVI_ROOT );

                                        // Set this item's data to the class
                                        // that we created for it.
            SetItemData( hEntireNetworkBranch, (DWORD) lpcEntireNetwork );

                                        // Enumerate this level to create the
                                        // first children item.
            lpcEntireNetwork->EnumerateNetResourceBranch( this,
                                                          hEntireNetworkBranch );

                                        // Expand this item to begin
                                        // with.
            Expand( hEntireNetworkBranch, TVE_EXPAND );

                                        // If we only asked for one network
                                        // provider or the other, expand that
                                        // provider by default.
            if ( GetCount() == 2 )
                {
                                        // Expand the network branch.
                Expand( GetChildItem( hEntireNetworkBranch ), TVE_EXPAND );

                                        // Set the entire network as the
                                        // first viewable item.
                Select( hEntireNetworkBranch, TVGN_FIRSTVISIBLE );
                }
            }
        }


    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::DestroyNetworkBranch
//
// Purpose:
//    Destroys a network branch.  Actually, it destroys all of the
//    lparam extra data classes that have been allocated for this
//    branch.
//
// Parameters:
//    HTREEITEM hBranchToDelete - [IN] Branch to delete all data items
//                                     beneath.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::DestroyNetworkBranch(
    HTREEITEM hBranchToDelete
    )
{
HTREEITEM hCurrentItem=hBranchToDelete;
CNetResourceContainer *lpcNetResourceContainer;

                                        // If list is empty, return;
    if ( NULL == hBranchToDelete )
        return;

                                        // First we need to kill all of this
                                        // item's children.
                                        // If it has children, iterate through
                                        // them and destroy them all.
    if( ItemHasChildren( hBranchToDelete ) )
        {
                                        // Get the first child item.
        hCurrentItem = GetChildItem( hBranchToDelete );

                                        // Loop to destroy all children.
        while ( NULL != hCurrentItem )
            {
                                        // Recursive call to destroy all
                                        // children of this branch.
            DestroyNetworkBranch( hCurrentItem );

                                        // Get the next child.
            hCurrentItem = GetNextSiblingItem( hCurrentItem );
            }
        }

                                        // Retrieve the extra data of this tree
                                        // item to free the memory of the class
                                        // associated with it.
    lpcNetResourceContainer = (CNetResourceContainer *)GetItemData( hBranchToDelete );

                                        // Make sure that there was a class
                                        // allocated to this tree item.
    if ( NULL != lpcNetResourceContainer )
        {
                                        // Free the memory for this item.
        delete ( lpcNetResourceContainer );
        }
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::DestroyNetworkTree
//
// Purpose:
//    Main function to delete the entire network tree and all items
//    associated with it. (Including the ImageList associated with
//    the tree)
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::DestroyNetworkTree( void )
{
CImageList *lpImageList;

                                        // Get the image list that we allocated
                                        // earlier.
    lpImageList = GetImageList( TVSIL_NORMAL );

                                        // If we got one.
    if ( NULL != lpImageList )
        {
                                        // Delete it.
        delete lpImageList;
        }

                                        // Destroy all of the container classes
                                        // associated with the branches of this
                                        // tree.
    DestroyNetworkBranch( GetRootItem() );
}


/////////////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl message handlers

void CNetworkTreeCtrl::PreSubclassWindow()
{
                                        // Initialize our tree control here,
                                        // because if we are not created by a
                                        // create call, (attaching to an
                                        // existing control, we are going to have
                                        // to fill it up with the initial
                                        // information.
    InitTreeControl();

	CTreeCtrl::PreSubclassWindow();
}


/////////////////////////////////////////////////////////////////////
// BOOL CNetworkTreeCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
//
// Purpose:
//    Create method of the CNetworkTreeCtrl class.
//
// Parameters:
//
// Returns:
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::Create(
    LPCTSTR lpszClassName,
    LPCTSTR lpszWindowName,
    DWORD dwStyle,
    const RECT& rect,
    CWnd* pParentWnd,
    UINT nID,
    CCreateContext* pContext
    )
{
DWORD dwRetVal;

	                                    // Switch resource state to this module.
    CDLLResourceState switchState;

	dwRetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

                                        // Initialize the tree control during
                                        // the create member if created this way.
                                        // If the item is subclassing another window,
                                        // we will initialize it when we subclass.
    InitTreeControl();


    return dwRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::GetSelectedItemName
//
// Purpose:
//    This function will return the name of the selected item from
//    this tree control.  It will return this in the form of a UNC
//    value for this item.  This function also accepts an optional
//    parameter for a resource type, if you wish.
//
// Parameters:
//    CString *lpcszSelectedItemName - [OUT] Name of the resource
//                                           selected.
//    DWORD *lpdwResourceType        - [OUT] Type of the resource
//                                           selected.
//
// Returns:
//    BOOLEAN as to the success of the get.  If there is no item
//    associated with that tree, or if something was in error,
//    this function will return FALSE.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::GetSelectedItemInformation(
    CString *lpcszSelectedItemName,
    DWORD *lpdwResourceType  // = NULL (If you do not wish to get this param)
    )
{
CNetResourceContainer *lpcNetResourceContainer;
HTREEITEM hSelectedItem;
BOOL bRetVal=TRUE;

                                        // Get the selected item from this tree
                                        // control.
    hSelectedItem = GetSelectedItem();

                                        // If an item is currently selected...
    if ( NULL != hSelectedItem )
        {
        lpcNetResourceContainer = (CNetResourceContainer *)GetItemData( hSelectedItem );

                                        // If we got an actual pointer for the
                                        // extra data, then we are going to
                                        // extract the data.
        if ( NULL != lpcNetResourceContainer )
            {
                                        // Now that we have a pointer to the
                                        // resource, we will ask it about it's
                                        // name and type.
            if ( NULL == lpcNetResourceContainer->GetItemName( lpcszSelectedItemName ) )
                {
                                        // Could not get the item name by the
                                        // net resource struct, so just return
                                        // the name of the branch, and TRUE;
                *lpcszSelectedItemName = GetItemText( hSelectedItem );
                bRetVal = TRUE;
                }

                                        // Only ask for type of we want to get
                                        // this information.
                                        // This parameter is optional.
            if ( NULL != lpdwResourceType )
                {
                *lpdwResourceType = lpcNetResourceContainer->GetItemType();
                }
            }
        else
            {
                                        // Extra data was NULL, so there was no
                                        // item associated with it.  This should
                                        // not happen.  There should be a pointer
                                        // for every item in the list. (except the
                                        // first one).
                                        // If this is NULL, just put the text as the
                                        // name of the item.  Not as secure, but it
                                        // stops errors when getting the type of the
                                        // first couple of items.
            *lpcszSelectedItemName = GetItemText( hSelectedItem );
            bRetVal = TRUE;
            }
        }
    else
        {
                                        // No item was selected, so return FALSE.
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::GetSelectedItemName
//
// Purpose:
//    This function will return the name of the selected item(s) from
//    this tree control.  This will add all items to a passed in
//    CObArray to retrieve the selections.
//
// Parameters:
//
//
// Returns:
//    BOOLEAN as to the success of the get.  If there is no item
//    associated with that tree, or if something was in error,
//    this function will return FALSE.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::GetSelectedItemInformation(
    CObArray *lpcSelectedItemArray
    )
{
BOOL bRetVal=TRUE;

                                        // This could take a little while, and
                                        // we don't want to see the tree control
                                        // enumerating the entire tree control.
    GetParent()->SetRedraw( FALSE );

    bRetVal = AddSelectedItems( GetRootItem(), lpcSelectedItemArray );

                                        // Turn the redraw back on for this control.
    GetParent()->SetRedraw( TRUE );

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// BOOL CNetworkTreeCtrl::AddSelectedItems(
//
// Purpose:
//    This function will add all selected items from a multi-select
//    implementation of this browser and put it into a CObArray for
//    return.
//
// Parameters:
//    HTREEITEM hBranchToAdd - [IN] Branch to Add.
//    CObArray *lpcSelectedItemArray - [OUT] List of selected items.
//
// Returns:
//    TRUE  - Added successfully.
//    FALSE - Failure during add.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::AddSelectedItems(
    HTREEITEM hBranchToAdd,
    CObArray *lpcSelectedItemArray
    )
{
CNetResourceContainer  *lpcNetResourceContainer;
CNetworkSelection      *lpcSelectedItem;
HTREEITEM               hChildItem;
CString                 cszItemName;
BOOL                    bRetVal = FALSE;

    if ( NULL != hBranchToAdd )
        {
                                        // If this item is selected, add it.
        if ( GetItemState( hBranchToAdd, TVIS_USERMASK) & INDEXTOSTATEIMAGEMASK(CHECKED) )
            {
            lpcNetResourceContainer = (CNetResourceContainer *)GetItemData( hBranchToAdd );

                                        // If we got an actual pointer for the
                                        // extra data, then we are going to
                                        // extract the data.
            if ( NULL != lpcNetResourceContainer )
                {
                                        // Now that we have a pointer to the
                                        // resource, we will ask it about it's
                                        // name and type.
                lpcSelectedItem = new CNetworkSelection(
                    lpcNetResourceContainer->GetItemName( &cszItemName ),
                    lpcNetResourceContainer->GetItemType()
                    );

                if ( NULL != lpcSelectedItem )
                    {
                                        // Add this item to our selected item
                                        // array.
                    lpcSelectedItemArray->Add( lpcSelectedItem );
                    bRetVal = TRUE;
                    }
                }
            }

                                        // Only add children if this check is at
                                        // least grey or checked.
        if ( FALSE == (GetItemState( hBranchToAdd, TVIS_USERMASK) & INDEXTOSTATEIMAGEMASK(NOT_CHECKED)) )
            {
                                        // Make sure that this branch is
                                        // expanded.  If this is not, it is
                                        // possible that it has children that
                                        // we don't know about.
            Expand( hBranchToAdd, TVE_EXPAND );
            }
                                        // Add all children of this item.
        hChildItem = GetChildItem( hBranchToAdd );

        while ( NULL != hChildItem )
            {
                                        // Recursively call to add the child
                                        // item and all branches of it.
            AddSelectedItems( hChildItem, lpcSelectedItemArray );

                                        // Retrieve the next sibling.
            hChildItem = GetNextItem( hChildItem, TVGN_NEXT );
            }
        }
    else
        {
                                        // Item sent was NULL.  Return False.
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::OnDestroy
//
// Purpose:
//    This function will destroy all data associated with the
//    branches of this tree control, as well as the CImageList that
//    has been allocated for it.  The reason why this must be done
//    here, instead of in the destructor is so that the tree control
//    will not be destroyed before we get a chance to free the memory
//    that we have allocated for it.  Every branch of this tree will
//    have a class allocated for the management of that particular
//    branch.  These have to be deallocated here.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::OnDestroy()
{
                                        // Free all memory that is associated
                                        // with the tree control.  This will
                                        // involve freeing up the image list,
                                        // as well as all of the
                                        // CNetResourceContainer classes as the
                                        // extra data member for every branch.
    DestroyNetworkTree();

                                        // Call through to the CTreeCtrl's Destroy
                                        // member.
	CTreeCtrl::OnDestroy();
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::SetValidSelectionFlags
//
// Purpose:
//    This function will set the valid selection item flags for this
//    tree control.  This will be used in querying if the item
//    selected is actually one that we could select, and not just an
//    enumerated item.
//
// Parameters:
//    DWORD dwValidSelectionFlags - [IN] New Valid Selection Flags.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::SetValidSelectionFlags(
    DWORD dwValidSelectionFlags
    )
{
                                        // Set this tree control's valid
                                        // selection flags to the flags that
                                        // you sent up.
    m_dwValidSelectionFlags = dwValidSelectionFlags;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::SetBrowseBeneathFlags
//
// Purpose:
//    This function will set the valid Browse Beneath item flags for
//    this tree control.  This will be used in determining whether
//    to offer a [+] next to each network resource item.  We are
//    going to set this to be everything except for those items which
//    are bottom level items.
//
// Parameters:
//    DWORD dwBrowseBeneathFlags - [IN] New Browse beneath item Flags
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::SetBrowseBeneathFlags(
    DWORD dwBrowseBeneathFlags
    )
{
                                        // Set this tree control's browse
                                        // beneath flags to the flags that
                                        // you sent up.
    m_dwBrowseBeneathFlags = dwBrowseBeneathFlags;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::SetDisplayItemFlags
//
// Purpose:
//    This function will set the display item flags for this
//    tree control.  This will be used in determining whether an item
//    that was retrieved from a network enumeration should be shown
//    or not.  This is useful for when different types of objects can
//    be grouped within one section.  These items would be like an
//    NDS Container, or the entire Netware Network where you can
//    have trees and servers in the same level.
//
// Parameters:
//    DWORD dwDisplayItemFlags - [IN] New Display Item Flags.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::SetDisplayItemFlags(
    DWORD dwDisplayItemFlags
    )
{
                                        // Set this tree control's Display
                                        // item flags.  These flags will
                                        // be used by the network resource
                                        // container classes to determine
                                        // which items should be shown,
                                        // and which should not.
    m_dwDisplayItemFlags = dwDisplayItemFlags;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::IsValidSelection
//
// Purpose:
//    This function will check to see if the item you send is a valid
//    selectable item according to how this network browser window
//    was set up.
//
// Parameters:
//    DWORD dwFlagToCheck - [IN] Flag to check against a valid
//                               selection flag list.
//
// Returns:
//    TRUE  - Valid selection type.
//    FLASE - Invalid selection type.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::IsValidSelectionType(
    DWORD dwFlagToCheck
    )
{
BOOL bRetVal;

                                        // Check to see if the flag you sent
                                        // up is a valid selection flag in
                                        // our selection flag list.
    if ( dwFlagToCheck & m_dwValidSelectionFlags )
        {
        bRetVal = TRUE;
        }
    else
        {
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::IsValidBrowseBeneathType
//
// Purpose:
//    This function will check to see if the item you send is a valid
//    Browse Beneath Item.
//
// Parameters:
//    DWORD dwType - [IN] Flag to check against a valid Browse
//                        Beneath flag list.
//
// Returns:
//    TRUE  - Valid selection type.
//    FLASE - Invalid selection type.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::IsValidBrowseBeneathType(
    DWORD dwType
    )
{
BOOL bRetVal;

                                        // Check to see if the flag you sent
                                        // up is a valid Browse Beneath flag
                                        // in our Browse Beneath flag list.
    if ( dwType & m_dwBrowseBeneathFlags )
        {
        bRetVal = TRUE;
        }
    else
        {
        bRetVal = FALSE;
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::IsValidDisplayType
//
// Purpose:
//    This function will check to see if the item you send is a valid
//    Display Type Item.
//
// Parameters:
//    DWORD dwType - [IN] Flag to check against a valid Display
//                        Type flag list.
//
// Returns:
//    TRUE  - Valid selection type.
//    FLASE - Invalid selection type.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetworkTreeCtrl::IsValidDisplayType(
    DWORD dwType
    )
{
BOOL bRetVal;

                                        // Check to see if the flag you sent
                                        // up is a valid Display Item flag
                                        // in our Display Item flag list.
    if ( dwType & m_dwDisplayItemFlags )
        {
        bRetVal = TRUE;
        }
    else
        {
        bRetVal = FALSE;
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl::SetMultiSelectState
//
// Purpose:
//    This function is used to set the state of this tree control to
//    multi-state before creating it.  Note that this must be set up
//    before the tree control is created.
//
// Parameters:
//    BOOL bUseMultiSelect - [IN] Specify to use or not use a multi-
//                                select network browser.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::SetMultiSelectState(
    BOOL bUseMultiSelect
    )
{
                                        // Set our internal multi-select
                                        // variable to the callers request.
    m_bUseMultiSelect = bUseMultiSelect;
}

void CNetworkTreeCtrl::CheckItems(HTREEITEM hItem, BOOL bChecked)
{
    HTREEITEM hParent;
    int iChecked = bChecked ? CHECKED : NOT_CHECKED;

    // Check this item.
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(iChecked), TVIS_USERMASK);

    // Check any child items.
    HTREEITEM hChildItem = hItem;
    if((hChildItem = GetNextItem(hChildItem, TVGN_CHILD)) != NULL)
        CheckChildren(hChildItem, iChecked);

    hParent = hItem;

    // Set the correct state of parent nodes.
    while ( hParent = GetParentItem(hParent) )
        {
        UpdateBranchSelectionState( hParent );
        }
}

void CNetworkTreeCtrl::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    // Space bar toggles check state
    if( nChar == VK_SPACE )
        {
        HTREEITEM hItem = GetSelectedItem();

        if( hItem )
            {
            // Toggle check state.
            if(GetItemState(hItem, TVIS_USERMASK) & INDEXTOSTATEIMAGEMASK(CHECKED))
                {
                CheckItems(hItem, FALSE);
                }
            else{
                CheckItems(hItem, TRUE);
                }
            }
        }
    else
        CTreeCtrl::OnChar( nChar, nRepCnt, nFlags );


}

/////////////////////////////////////////////////////////////////////
// void CNetworkTreeCtrl::UpdateBranchSelectionState(
//
// Purpose:
//    This function will go through a branch's children, and determine
//    it's selection state based on that of it's parents.
//
// Parameters:
//    HTREEITEM hBranchToUpdate - [IN] The branch to update.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::UpdateBranchSelectionState(
    HTREEITEM hBranchToUpdate
    )
{
    HTREEITEM   hChildItem;
    BOOL        bAllChecked = TRUE;
    BOOL        bAtLeastOneChecked = FALSE;

                                        // Only adjust the state if the item
                                        // Has children.
    if ( ItemHasChildren( hBranchToUpdate ) )
		{
        hChildItem = GetChildItem( hBranchToUpdate );

        do
            {
		    UINT uState;
			uState =GetItemState(hChildItem, TVIS_USERMASK);

			if( uState & INDEXTOSTATEIMAGEMASK(CHECKED) ||
                uState & INDEXTOSTATEIMAGEMASK(GRAY_CHECKED) )
                {
                                        // Note that we have at least one checked.
                bAtLeastOneChecked = TRUE;
                }

            if( uState & INDEXTOSTATEIMAGEMASK(NOT_CHECKED) ||
                uState & INDEXTOSTATEIMAGEMASK(GRAY_CHECKED) )
                {
                                        // We have had at least one item that is
                                        // not checked.
                bAllChecked = FALSE;
                }

                                        // Go to the next child item.
            hChildItem = GetNextSiblingItem( hChildItem );
            }
        while ( NULL != hChildItem );

        if ( TRUE == bAllChecked )
			SetItemState( hBranchToUpdate , INDEXTOSTATEIMAGEMASK(CHECKED), TVIS_USERMASK);
        else if ( TRUE == bAtLeastOneChecked )
			SetItemState( hBranchToUpdate , INDEXTOSTATEIMAGEMASK(GRAY_CHECKED), TVIS_USERMASK);
        else
			SetItemState( hBranchToUpdate , INDEXTOSTATEIMAGEMASK(NOT_CHECKED), TVIS_USERMASK);
        }
}

/////////////////////////////////////////////////////////////////////
// void CNetworkTreeCtrl::CheckChildren(HTREEITEM hItem, int iChecked)
//
// Purpose:
//    This function will check all of the children under a tree item.
//
// Parameters:
//    HTREEITEM hItem - [IN] Tree item to check along with all of the
//                           children of this tree item.
//    int iChecked    - [IN] State to set all of the children.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::CheckChildren(
    HTREEITEM hItem,
    int iChecked
    )
{
    // Check this item.
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(iChecked), TVIS_USERMASK);

    // Recursivly check all children.
    HTREEITEM hChildItem;
    if((hChildItem = GetNextItem(hItem, TVGN_CHILD)) != NULL)
        {
        CheckChildren(hChildItem, iChecked);
        }

    // Recursivly check all children, then check siblings.
    HTREEITEM hNextItem = hItem;
    while((hNextItem = GetNextSiblingItem( hNextItem )) != NULL)
        {
        if((hChildItem = GetNextItem(hNextItem, TVGN_CHILD)) != NULL)
            {
            CheckChildren(hChildItem, iChecked);
            }
        // Set this guy too!
        SetItemState(hNextItem, INDEXTOSTATEIMAGEMASK(iChecked), TVIS_USERMASK);
        }

}

/////////////////////////////////////////////////////////////////////
// void CNetworkTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
//
// Purpose:
//    Mouse left button down handler.
//
// Parameters:
//    UINT nFlags  - [IN] Flags for the button click.
//    CPoint point - [IN] Point where button click occurred.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::OnLButtonDown(
    UINT nFlags,
    CPoint point
    )
{
    if ( TRUE == m_bUseMultiSelect )
        {
        // Check to see if the user clicked on the check box.
        UINT uFlags;
        HTREEITEM hItem = HitTest(point, &uFlags);
        if(hItem && (uFlags & TVHT_ONITEMSTATEICON))
            {
            // Toggle check state.
            if(GetItemState(hItem, TVIS_USERMASK) & INDEXTOSTATEIMAGEMASK(CHECKED))
                {
                CheckItems(hItem, FALSE);
                }
            else{
                CheckItems(hItem, TRUE);
                }
            }
        else    // TODO: Add your message handler code here and/or call default
            CTreeCtrl::OnLButtonDown(nFlags, point);
        }
    else
        {
        CTreeCtrl::OnLButtonDown(nFlags, point);
        }
}

/////////////////////////////////////////////////////////////////////
// void CNetworkTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
//
// Purpose:
//    Double click handler for our Network tree control.
//
// Parameters:
//    UINT nFlags  - [IN] Flags for the button click.
//    CPoint point - [IN] Point where button click occurred.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkTreeCtrl::OnLButtonDblClk(
    UINT nFlags,
    CPoint point
    )
{
    // Only call default method if double click is *NOT*
    // on the check box.  Users like to double click checkbox items
    // which is not cool in this case.
    UINT uFlags;
    HTREEITEM hItem = HitTest(point, &uFlags);
    if(!hItem || !(uFlags & TVHT_ONITEMSTATEICON))
        {
        CTreeCtrl::OnLButtonDblClk(nFlags, point);
        }
}

/////////////////////////////////////////////////////////////////////////////
// CNetworkSelection

/////////////////////////////////////////////////////////////////////
// CNetworkSelection::CNetworkSelection(
//
// Purpose:
//    Network Selection class constructor.
//
// Parameters:
//    None.
//
// Returns:
//    A constructed class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetworkSelection::CNetworkSelection(
    LPCTSTR cszResourceName,
    DWORD   dwResourceType
    )
{
    SetResourceName( cszResourceName );
    SetResourceType( dwResourceType );
}

/////////////////////////////////////////////////////////////////////
// void CNetworkSelection::SetResourceName(
//
// Purpose:
//  Access function to set the resource name.
//
// Parameters:
//    LPCTSTR cszResourceName - [IN] New resource name.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkSelection::SetResourceName(
    LPCTSTR cszResourceName
    )
{
    try
        {
        m_cszResourceName = cszResourceName;
        }
    catch ( CMemoryException )
        {
        }
}

/////////////////////////////////////////////////////////////////////
// void CNetworkSelection::SetResourceType(
//
// Purpose:
//  Access function to set the resource type of a Network Selection
//  class.
//
// Parameters:
//    DWORD dwResourceType - [IN] New resource type.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 07-30-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNetworkSelection::SetResourceType(
    DWORD dwResourceType
    )
{
    m_dwResourceType = dwResourceType;
}
