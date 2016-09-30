/////////////////////////////////////////////////////////////////////
// NRESCONT.CPP - Created - 10/30/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/NRESCONT.CPv   1.1   07 Apr 1998 13:47:40   jallee  $
//
// Description:
//    Implementation file for the CNetResourceContainer class.  This
//    class will contain information for enumerating through network
//    resources.  This will entail a buffer to store the data, and
//    retrieval and sizing functions for the buffer.  It will also
//    have the ability to receive a NETRESOURCE struct from a current
//    enumeration, and set up a copy of that for tree control
//    maintenance.  An allocated instance of this class will be the
//    extra data for every item in a tree control to be able to
//    enumerate farther down in the tree.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/NRESCONT.CPv  $
//
//    Rev 1.1   07 Apr 1998 13:47:40   jallee
// islower() -> _istlower(). This caused enumeration bugs when we built unicode.
//
//    Rev 1.0   21 Nov 1997 11:51:56   KSACKIN
// Initial revision.
//
//    Rev 1.3   30 Jul 1997 16:15:16   KSACKIN
// Added support for a multi-select network browser.
//
//    Rev 1.2   15 Jun 1997 20:02:02   KSACKIN
// Removed the network provider layer of the network browser for Windows 95.
// The native Windows 95 network browser does not show this.  They simply
// skip the provider level and enumerate one level further.
//
//    Rev 1.1   13 Jun 1997 13:43:18   KSACKIN
// Updated the GetResourceType function to check for NULL strings.  Within
// Windows 95, the Network Enumeration functions give a NULL pointer for the
// resource name, and a string compare was failing.
//
//    Rev 1.0   18 Apr 1997 14:02:44   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:16:30   RFULLER
// Initial revision
//
//    Rev 1.6   24 Jan 1997 22:50:22   ksackin
// Fixed a bug where I was looking for the entire string of "Microsoft Windows
// Network" to determint if the network type was Microsoft.  Now I simply search
// for "Microsoft" as a substring of the provider to determine this.
//
//    Rev 1.5   18 Dec 1996 00:13:12   KSACKIN
// Added an increment of 100 bytes to the actual size that the network
// enumeration returns to me.  This is to fix a bug with NT 3.51 and the
// Novell 32 bit client.  The client will return to you that you need to
// increase the buffer size, and then it will give you a size.  When you
// allocate that size, and call it again, it will return to you the same size,
// and still say that it needs more.  This will fix any bug of this nature
// in the future because it will keep incrementing the buffer size until it
// has enough room for the data.
//
//    Rev 1.4   17 Dec 1996 23:43:14   ksackin
// Made the default enumeration size 16K.
//
//    Rev 1.3   16 Dec 1996 23:23:30   ksackin
// Made some pretty major changes to the way we enumerate the network.  This
// involved the way we handle the CNetResourceContainer class.  We are actually
// storing some string information in the class itself, instead of within a
// parent's enumeration data.  This is because we have to do multiple
// enumerations per level sometimes.
//
//    Rev 1.2   09 Dec 1996 23:48:52   ksackin
// Fixed a problem with only being able to add network containers.  Now we will
// be able to show shares, and not only network enumeratable containers.
//
//    Rev 1.1   05 Dec 1996 02:01:00   ksackin
// Added defines for the various Network Resource types to use with our
// screening of resources in our network tree.
//
//    Rev 1.0   20 Nov 1996 01:37:52   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// Include files

#include "stdafx.h"
#include "NResCont.h"
#include "NetBrStr.h"                   // For strings.
#include "DLLRCSta.h"                   // DLL Resource State class.
#include "Dispmsg.h"


/////////////////////////////////////////////////////////////////////
// Local function prototypes

#define SYM_SYSTEM_WIN_95           1
#define SYM_SYSTEM_WIN_NT           2
#define SYM_SYSTEM_WIN_32S          3
#define SYM_SYSTEM_WIN_31           4


                                        // Local version of this funciton.
                                        // This is because we are not linking
                                        // to SymNav or SymKrnl.
UINT WINAPI SystemGetWindowsType(VOID);

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::CNetResourceContainer
//
// Purpose:
//    Constructor for the CNetResourceContainer class.
//
// Parameters:
//    LPNETRESOURCE lpNetResource - [IN] Net Resource struct to copy
//                                       for our struct's reference.
//                                       (default = NULL)
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/30/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetResourceContainer::CNetResourceContainer(
    LPNETRESOURCE lpNetResource,             // = NULL
    CNetResourceContainer *lpParentContainer // = NULL
    )
{
   m_bEnumeratedOnce = FALSE;
   m_bNDSTreeItem = FALSE;

   InitContainer( lpNetResource, lpParentContainer );
}

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::~CNetResourceContainer
//
// Purpose:
//    Destructor for the CNetResourceContainer class.
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
// 10/30/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNetResourceContainer::~CNetResourceContainer( void )
{
}

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::InitContainer
//
// Purpose:
//    Function to initialize this container with a LPNETRESOURCE
//    template.  This will be passed most likely from the parent
//    branch of a network enumeration control to a child so that he
//    may know how to enumerate below himself.
//
// Parameters:
//    LPNETRESOURCE lpNetResource - [IN] Net resource for this
//                                       container.  This will most
//                                       likely be sent here from a
//                                       parent branch's enumeration.
//    CNetResourceContainer *lpParentContainer - [IN] This is to
//                                       determine parent heritage.
//
// Returns:
//    BOOLEAN success of initialization.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/30/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetResourceContainer::InitContainer(
    LPNETRESOURCE lpNetResource,
    CNetResourceContainer *lpParentContainer // = NULL
    )
{
BOOL bRetVal=TRUE;

                                        // If we got a valid net resource
                                        // pointer...
    if ( NULL != lpNetResource )
        {
                                        // Copy the caller's instance of
                                        // the NetResource structure to our own.
        m_NetResource = *lpNetResource;
        m_lpNetResource = &m_NetResource;
        }
    else
        {
                                        // If we did not get a valid resource,
                                        // set all items in the struct to 0.
        memset( &m_NetResource, 0, sizeof( m_NetResource ) );
        m_lpNetResource = NULL;
        }

                                        // Set parent contianer.
    m_lpParentContainer = lpParentContainer;

    if ( m_lpParentContainer )
        {
                                        // If our parent was an NDS tree item,
                                        // then we are as well.  This is for
                                        // display purposes.
        if ( m_lpParentContainer->IsNDSTreeItem() )
            {
                                        // Set our NDSTreeItem member.
            m_bNDSTreeItem = TRUE;
            }
        }
                                        // If we are an NDS Tree...
    if ( lpNetResource &&
         lpNetResource->dwDisplayType == RESOURCEDISPLAYTYPE_TREE )
        {
                                        // Set our NDSTreeItem member.
        m_bNDSTreeItem = TRUE;
        }

                                        // If we have a pointer to a net
                                        // resource struct...
    if ( NULL != m_lpNetResource )
        {
                                        // Make local copies of all of these
                                        // strings, and map the strings sent
                                        // to the local copy.
                                        // Needed to have these special function to
                                        // deal with things like NULL string pointers
                                        // and errors like that with CString objects.
        m_lpNetResource->lpLocalName  = MapStringPointerToCopy( &m_cszLocalName,  m_lpNetResource->lpLocalName  );
        m_lpNetResource->lpRemoteName = MapStringPointerToCopy( &m_cszRemoteName, m_lpNetResource->lpRemoteName );
        m_lpNetResource->lpComment    = MapStringPointerToCopy( &m_cszComment,    m_lpNetResource->lpComment    );
        m_lpNetResource->lpProvider   = MapStringPointerToCopy( &m_cszProvider,   m_lpNetResource->lpProvider   );
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// MapStringPointerToCopy
//
// Purpose:
//    Function to copy a string pointer to a CString object, and then
//    to return a pointer to the character data that we allocated to
//    the CString object.
//
// Parameters:
//    CString *lpcszDestStr         - [OUT] String to copy data into,
//                                          and then to return a
//                                          pointer to.
//    LPTSTR lpszStringPointerToMap - [IN]  String to copy into the
//                                          passed in CString object.
//
// Returns:
//    Pointer to the new string that now resides within the CString
//    object.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/30/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
LPTSTR MapStringPointerToCopy(
    CString *lpcszDestStr,
    LPTSTR lpszStringPointerToMap
    )
{
LPTSTR lpstrReturnString;

                                        // Check to see if the string that was
                                        // passed up to us is NULL
    if ( NULL == lpszStringPointerToMap )
        {
                                        // If this string pointer is NULL, we
                                        // need to also return a NULL, and make
                                        // sure that this CString object is empty.
        lpcszDestStr->Empty();
        lpstrReturnString = lpcszDestStr->GetBuffer(0);
        }
    else
        {
        try
            {
                                        // Make a copy of the string pointed to
                                        // by lpszStringPointerToMap.
            *lpcszDestStr = lpszStringPointerToMap;

                                        // Set the return pointer equal to the
                                        // buffer now in our local string.
            lpstrReturnString = lpcszDestStr->GetBuffer(0);
            }
        catch( CMemoryException )
            {
                                        // If a memory exception occurred, return
                                        // NULL;
            lpstrReturnString = NULL;
            lpcszDestStr->Empty();
            }
        }

    return lpstrReturnString;
}

/////////////////////////////////////////////////////////////////////
// TVCompareFunction
//
// Purpose:
//    Comparison function to sort items in our network tree control.
//    This function will evaluate the names, as well as the type of
//    item that it is.  If one item is a higher priority than
//    another, it will be sorted above the first.  For instance, if
//    you are listing the Netware Network, you will get trees as well
//    as servers.  We want to list the trees first, so if we are
//    comparing a tree to a server, return that the tree goes first,
//    even though the name may be greater than the server.
//
// Parameters:
//    None.
//
// Returns:
//    void * to the beginning of a writable buffer for network
//    enumeration.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/30/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
int CALLBACK TVCompareFunction(
    LPARAM lParam1,
    LPARAM lParam2,
    LPARAM lParamSort
    )
{
CNetResourceContainer *lpFirstContainer = (CNetResourceContainer *)lParam1;
CNetResourceContainer *lpSecondContainer = (CNetResourceContainer *)lParam2;
int nRetVal=0;

                                        // Only compare the names if they
                                        // are the same type.
    if ( lpFirstContainer->GetNetResourceStruct()->dwDisplayType ==
         lpSecondContainer->GetNetResourceStruct()->dwDisplayType )
        {
                                        // Items are the same type, so compare
                                        // the names.
        nRetVal = CompareString( LOCALE_USER_DEFAULT,
                                 NORM_IGNORESYMBOLS,
                                 lpFirstContainer->GetNetResourceStruct()->lpRemoteName,
                                 _tcslen(lpFirstContainer->GetNetResourceStruct()->lpRemoteName),
                                 lpSecondContainer->GetNetResourceStruct()->lpRemoteName,
                                 _tcslen(lpSecondContainer->GetNetResourceStruct()->lpRemoteName) );

                                        // Fixup return value to sort the item correctly.
                                        // This function returns 1, 2, or 3.
        nRetVal -= 2;
        }
    else
        {
                                        // Items are of a different type, so
                                        // we are going to return that the item
                                        // with the most importance is shown first.
                                        // To do this, we are going to get the image
                                        // ID's and subtract them.  This is because
                                        // we are going to sort our images by importance,
                                        // and we may not be able to count on their
                                        // ID's for importance.
        nRetVal = GetImageIDForResource( lpFirstContainer->GetNetResourceStruct() ) -
                  GetImageIDForResource( lpSecondContainer->GetNetResourceStruct() );
        }

    return nRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::EnumerateNetResourceBranch
//
// Purpose:
//    This function will enumerate a network resource branch, and
//    add each item to a branch of a tree control under a parent
//    branch specified.
//
// Parameters:
//    CNetworkTreeCtrl *lpNetTreeControl - [IN] Network Tree control to
//                                          enumerate in.
//    HTREEITEM hBranchToEnum  - [IN] Branch under that tree control.
//                                    to enumerate.
//
// Returns:
//    BOOLEAN as to the success of the enumeration.
//
// See Also:
//    CNetBrowsDlg class declaration for flag list.
//
/////////////////////////////////////////////////////////////////////
// 10/31/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetResourceContainer::EnumerateNetResourceBranch(
    CNetworkTreeCtrl *lpNetTreeControl,
    HTREEITEM hBranchToEnum
    )
{
CByteArray baNetResourceBuffer;        // Buffer for network enumeration.
LPNETRESOURCE lpnrLocal;               // pointer to enumerated structures
CNetResourceContainer * lpcTempContainer; // Temporary container.
DWORD dwNetResourceCount = 0xFFFFFFFF; // enumerate all possible entries
DWORD cbBuffer = 16384;                // 16K is a good size
DWORD dwResult;
DWORD dwResourceType;
DWORD dwEnumLoop;
TV_SORTCB rSortTVStruct;
HCURSOR hWaitCursor=NULL;
HCURSOR hOldCursor=NULL;
BOOL bRetVal = TRUE;

                                        // Only enumerate if we have not
                                        // already done this branch.
    if ( FALSE == m_bEnumeratedOnce )
        {
                                        // Set the wait cursor while
                                        // enumerating this branch.
        hWaitCursor = LoadCursor( NULL,
                                  IDC_WAIT );
        if ( hWaitCursor )
            {
                                        // Set cursor to the wait cursor.
            hOldCursor = SetCursor( hWaitCursor );
            }

                                        // Set the boolean flag that we have
                                        // enumerated this tree at least once.
        m_bEnumeratedOnce = TRUE;

                                        // Open the network enumeration for
                                        // this branch of the network.
        dwResult = WNetOpenEnum(RESOURCE_GLOBALNET,
            RESOURCETYPE_DISK,
            0,                    // Enumerate all resources
            m_lpNetResource,      // NULL first time this function is called
            &m_hNetEnumResource); // Handle to resource


                                        // If no error occurred...
        if (dwResult == NO_ERROR)
            {

                                        // This try is to catch all memory
                                        // allocation errors which could
                                        // occur when trying to enumerate
                                        // the network.
            try
                {
                                        // Allocate memory for NETRESOURCE
                                        // structures.
                baNetResourceBuffer.SetSize( cbBuffer );

                                        // Set up a local pointer to our network
                                        // resource byte array receive buffer.
                lpnrLocal = (LPNETRESOURCE) baNetResourceBuffer.GetData();

                                        // Begin enumeration loop.
                do
                    {
                                        // Set this to be the maximum number of
                                        // entries that will fit into our buffer.
                    dwNetResourceCount = 0xffffffff;

                                        // Call the net enumeration function.
                    dwResult = WNetEnumResource(
                        m_hNetEnumResource,      // resource handle
                        &dwNetResourceCount,     // defined locally as 0xFFFFFFFF
                        lpnrLocal,               // LPNETRESOURCE
                        &cbBuffer                // buffer size
                        );

                                         // Check for not enough buffer space first,
                                         // and if this is the problem, try again
                                         // before entering iterator loop.  We
                                         // should never have to do this more than
                                         // twice since the cbBuffer variable has
                                         // the correct size in it if this fails.
                    if ( dwResult == ERROR_MORE_DATA )
                        {
                                        // Hack for NT 3.51 Novell 32 bit
                                        // client.  Bogus return value (too
                                        // small of buffer size).  Also will
                                        // solve any loop where we get a size,
                                        // and it is just a bit too small, but
                                        // it returns the same amount to get.
                        cbBuffer += 100;

                                        // Allocate more memory for NETRESOURCE
                                        // structures, since the last call was
                                        // not sufficient. This should only happen
                                        // if there was not even enough room for one
                                        // item in our buffer.
                        baNetResourceBuffer.SetSize( cbBuffer );

                                        // Get a new buffer to our data to insure
                                        // that our buffer has not changed addresses.
                        lpnrLocal = (LPNETRESOURCE) baNetResourceBuffer.GetData();
                        }


                                        // If successfull enumeration...
                    if ( dwResult == NO_ERROR )
                        {
                                        // Loop through this round of resources.
                        for ( dwEnumLoop = 0; dwEnumLoop < dwNetResourceCount ; dwEnumLoop++)
                            {
                                        // Verify that this item is a valid
                                        // display item type.
                            if ( ! (lpnrLocal[dwEnumLoop].dwUsage & RESOURCEUSAGE_NOLOCALDEVICE) )
                                {
                                        // Get the resource type.
                                dwResourceType = MapNetResourceToType( &(lpnrLocal[dwEnumLoop]) );

                                if ( lpNetTreeControl->IsValidDisplayType( dwResourceType ) )
                                    {
                                    if ((SystemGetWindowsType() == SYM_SYSTEM_WIN_95)&&
                                        (dwResourceType & (RESOURCE_TYPE_NETWORK_MICROSOFT | RESOURCE_TYPE_NETWORK_NETWARE) ))
                                        {
                                        // If we are running on Windows 95,
                                        // skip adding networks, and directly
                                        // enumerate one level deeper.
                                        lpcTempContainer = new CNetResourceContainer( &(lpnrLocal[dwEnumLoop]), this );

                                        if ( NULL != lpcTempContainer )
                                            {
                                        // Enumerate next level.
                                            lpcTempContainer->EnumerateNetResourceBranch( lpNetTreeControl,
                                                                                          hBranchToEnum );

                                        // Delete temporary object.
                                            delete lpcTempContainer;
                                            }
                                        }
                                    else
                                        {
                                        // Insert this item into the tree,
                                        // as well as allocate a new class
                                        // of this type to handle that branch.
                                        InsertNetEnumItem( lpNetTreeControl,
                                                           hBranchToEnum,
                                                           &(lpnrLocal[dwEnumLoop]) );
                                        }
                                    }
                                }
                            }
                        }
                    }
                                        // Only quit for an error.  Do this enum until
                                        // either a memory allocation occurs, an
                                        // extended error occurred, or we got all of
                                        // the data.
                                        // ERROR_MORE_DATA is not an error, it simply
                                        // means that we had to resize the buffer.  We
                                        // should still continue.
                while ( (dwResult == NO_ERROR) ||
                        (dwResult == ERROR_MORE_DATA) );

                }
            catch( ... )
                {
                                        // This is to catch any memory
                                        // exceptions that may have occurred
                                        // when trying to enumerate the network.
                                        // If this happened, return FALSE for
                                        // a critical error.
                bRetVal = FALSE;
                }

                                        // Catch extended errors here.
            if ( dwResult != ERROR_NO_MORE_ITEMS )
                {
                                        // Display generic error message for
                                        // now...
                DisplayErrorMessage( lpNetTreeControl->GetParent()->m_hWnd,
                                     IDS_ERROR_ENUMERATING_NETWORK );
                }

                                        // Close this enumeration handle.
            WNetCloseEnum( m_hNetEnumResource );
            }
        else
            {
                                        // An error occurred when opening
                                        // this network enumeration.
            bRetVal = FALSE;
            }


                                        // If at least one item was added to
                                        // this branch of the tree...
        if ( lpNetTreeControl->GetChildItem( hBranchToEnum ) )
            {
                                        // Set up a sort structure to sort this
                                        // branch of the network browser
                                        // dialog.  We are using a custom sort
                                        // function for this because the
                                        // provided one is case insensitive.
                                        // Besides, we are going to do extra
                                        // processing here.
            rSortTVStruct.hParent = hBranchToEnum;
            rSortTVStruct.lpfnCompare = TVCompareFunction;
            rSortTVStruct.lParam = 0L;

                                        // Call to custom sort function.
            lpNetTreeControl->SortChildrenCB( &rSortTVStruct );
            }
        else
            {
                                        // No items were gotten from the enumerator,
                                        // therefore, we need to remove all
                                        // children counts from this item's data.
                                        // This needs to be done because of the way
                                        // we have faked out this item to still have
                                        // a [+] even though we have not enumerated
                                        // yet.
            SetItemChildrenCount( lpNetTreeControl, hBranchToEnum, 0 );
            }

                                        // If we were able to set the cursor
                                        // to the hour glass...
        if ( NULL != hOldCursor )
            {
                                        // Set it back to what it was before.
            SetCursor( hOldCursor );
            }
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::SetItemChildrenCount
//
// Purpose:
//    Set the children count of a particular branch to a value.
//
// Parameters:
//    CTreeCtrl *lpTreeControl - [IN] Tree control to operate on branch
//                                    item within.
//    HTREEITEM hBranchToEnum  - [IN] Branch under that tree control.
//                                    to change.
//    int       nNewcChildren  - [IN] New number of children for this
//                                    branch item.
//
// Returns:
//    BOOLEAN as to the success of the set.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 10/31/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetResourceContainer::SetItemChildrenCount(
    CTreeCtrl *lpTreeControl,
    HTREEITEM hBranchToChange,
    int nNewcChildren
    )
{
TV_ITEM tvItem;
BOOL bRetVal;

                                        // Set item to change, and value to
                                        // change it to.
    tvItem.mask = TVIF_CHILDREN;
    tvItem.hItem = hBranchToChange;
    tvItem.cChildren = nNewcChildren;

                                        // Set this items attributes.
    bRetVal = lpTreeControl->SetItem( &tvItem );

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// GetImageIDForResource
//
// Purpose:
//    To return an image offset for a particular net resource.  This
//    gives us one place to control the offsets into our CImageList
//    class for the image index of each type of network resource.
//
// Parameters:
//    LPNETRESOURCE lpNetResource - [IN] Net resource to retrieve an
//                                  image offset for.  This will be
//                                  returned from the network
//                                  enumeration.
//
// Returns:
//    Icon index into the CImageList class as to the bitmap index for
//    this particular resource type.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD GetImageIDForResource(
    LPNETRESOURCE lpNetResource
    )
{
DWORD dwRetVal=0;

                                        // If we got passed a NULL net
                                        // resource struct, return with
                                        // an index of 0.
   if ( NULL == lpNetResource )
       {
       return 0;
       }

                                        // Otherwise, switch on the display
                                        // type.
   switch( lpNetResource->dwDisplayType )
       {
       case RESOURCEDISPLAYTYPE_NETWORK:
            dwRetVal = 1;
            break;

       case RESOURCEDISPLAYTYPE_DOMAIN:
            dwRetVal = 2;
            break;

       case RESOURCEDISPLAYTYPE_TREE:
            dwRetVal = 3;
            break;

       case RESOURCEDISPLAYTYPE_NDSCONTAINER:
            dwRetVal = 4;
            break;

       case RESOURCEDISPLAYTYPE_SERVER:
            dwRetVal = 5;
            break;

       case RESOURCEDISPLAYTYPE_SHARE:
            dwRetVal = 7;
            break;

       case RESOURCEDISPLAYTYPE_GENERIC:

                                        // Check for the proveder name being the
                                        // same as the remote name.  If this is
                                        // the case, then we should display the
                                        // RESOURCEDISPLAYTYPE_NETWORK icon.  This
                                        // is for NT 3.51.  They return the network
                                        // providers as resource display type 0.
           if ( NULL == _tcsicmp( lpNetResource->lpProvider, lpNetResource->lpRemoteName ) )
                {
                dwRetVal = 1;
                }
            else
                {
                                        // If this is still not the case, return 0.
                dwRetVal = 0;
                }

           break;
       };

   return dwRetVal;
}

/////////////////////////////////////////////////////////////////////
// FormatResourceNameForDisplay
//
// Purpose:
//    Function to take a LPNETRESOURCE struct and format the string
//    values contained into a dispalyable name.  This name will then
//    be returned in the lpcszDisplayString parameter.
//
// Parameters:
//    LPNETRESOURCE lpNetResource      - [IN] Resource to format
//                                              into a display string.
//    CString      *lpcszDisplayString - [OUT] Display String. (For
//                                               Tree display view.
//
// Returns:
//    BOOLEAN as to whether the format succeeded.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetResourceContainer::FormatResourceNameForDisplay(
    CString                 *lpcszDisplayString
    )
{
DWORD dwProvider;
BOOL bRetVal=TRUE;

    dwProvider = GetProviderID( m_lpNetResource->lpProvider );

    if ( NETWORK_PROVIDER_NETWARE == dwProvider )
        {
                                        // Format string for Netware resources.
        FormatNetwareResourceNameForDisplay( lpcszDisplayString );
        }
    else
        {
                                        // INCLUSIVE of Microsoft  This is default.
                                        // Nothing special for this type, so send
                                        // it through the default parser.
        FormatStandardResourceNameForDisplay( lpcszDisplayString );
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::FormatStandardResourceNameForDisplay
//
// Purpose:
//    Function to take an LPNETRESOURCE struct and format the string
//    values contained into a standard dispalyable name.  This name
//    will then be returned in the lpcszDisplayString parameter.
//
// Parameters:
//    LPNETRESOURCE lpNetResource      - [IN]  Resource to format
//                                             into a display string.
//    CString      *lpcszDisplayString - [OUT] Display String. (For
//                                             Tree display view.
//
// Returns:
//    BOOLEAN as to whether the format succeeded.
//
// See Also:
//    FormatResourceNameForDisplay for the base function to call.
//
//    Also, FormatNetwareResourceNameForDisplay for different parsing
//       needs.  This function is default, but some providers
//       require extra processing, such as netware trees.
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetResourceContainer::FormatStandardResourceNameForDisplay(
    CString        *lpcszDisplayString
    )
{
LPTSTR szMovePtr;
BOOL bRetVal=TRUE;

                                        // Format the string to our liking.
                                        // First, search for the last '\\'
                                        // character.
    szMovePtr = _tcsrchr( m_lpNetResource->lpRemoteName, '\\' );

                                        // If we could find a '\\' in a reverse
                                        // search of the string...
    if ( szMovePtr )
        {
                                        // Found a '\\' character, so we are
                                        // going to increment this string, and
                                        // then format it.
        szMovePtr = _tcsinc( szMovePtr );

                                        // Copy this part of the string into
                                        // the return string class.
        *lpcszDisplayString = szMovePtr;
        }
    else
        {
                                        // Did not find a '\\', so we are just
                                        // going to use the display name.
        *lpcszDisplayString = m_lpNetResource->lpRemoteName;
        }

                                        // Make sure string is not empty.
    if ( 0 != lpcszDisplayString->GetLength() )
        {
                                        // Make string lower case.
        lpcszDisplayString->MakeLower();

                                        // If this character is a lower case letter...
        if ( _istlower( lpcszDisplayString->GetAt( 0 ) ) )
            {
                                        // Upcase first letter, leave the
                                        // rest in lower case.
            lpcszDisplayString->SetAt( 0, _totupper( lpcszDisplayString->GetAt(0) ) );
            }
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::FormatNetwareResourceNameForDisplay
//
// Purpose:
//    Function to take an LPNETRESOURCE struct and format the string
//    values contained into a Netware dispalyable name.  This name
//    will then be returned in the lpcszDisplayString parameter.
//
// Parameters:
//    LPNETRESOURCE lpNetResource      - [IN]  Resource to format
//                                             into a display string.
//    CString      *lpcszDisplayString - [OUT] Display String. (For
//                                             Tree display view.
//
// Returns:
//    BOOLEAN as to whether the format succeeded.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetResourceContainer::FormatNetwareResourceNameForDisplay(
    CString        *lpcszDisplayString
    )
{
int nStrPos;
BOOL bRetVal=TRUE;
                                        // Format type is Netware.  We need
                                        // to parse the strings differently for
                                        // this than for microsoft and
                                        // standard shares.

                                        // First, format as a standard resource
                                        // display name, then we will do extended
                                        // processing on it.
    if ( TRUE == FormatStandardResourceNameForDisplay( lpcszDisplayString ) )
        {

                                        // Trim the string up.
        lpcszDisplayString->TrimLeft();
        lpcszDisplayString->TrimRight();

                                        // If we are an NDS tree item...
        if ( m_bNDSTreeItem )
            {
                                        // Make string lower case since it
                                        // is, or lies within an NDS tree.
                                        // This is for Microsoft Explorer
                                        // look and feel.
            lpcszDisplayString->MakeLower();

                                        // For Netware trees, we only want to
                                        // display up to the first dot.
            nStrPos = lpcszDisplayString->Find( _T(".") );

                                        // If we found a '.' character, then
                                        // according to this format...
            if ( -1 != nStrPos )
                {
                                        // Chop it off here for the display
                                        // name.
                *lpcszDisplayString = lpcszDisplayString->Left( nStrPos );
                }
            }
        }
    else
        {
                                        // If standard formatting failed, we are going
                                        // to return FALSE as well.
        bRetVal = FALSE;
        }

    return bRetVal;
}

/////////////////////////////////////////////////////////////////////
// GetProviderID
//
// Purpose:
//    This function will return a DWORD flag representation of a
//    provider based upon the provider's name.
//
// Parameters:
//    LPCTSTR cszProviderToFind - [IN] Provider's name. (Probably
//                                gotten from a NETRESOURCE struct.)
//
// Returns:
//    Flag as to the provider identifier, 0 if not recognized.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD GetProviderID(
    LPCTSTR cszProviderToFind
    )
{
CString cszProviderResourceName;
DWORD dwReturn = 0;

                                        // Switch resource state to this
                                        // module for loading strings.
    CDLLResourceState switchState;

    try
        {
                                        // Microsoft Windows Network?
        if ( cszProviderResourceName.LoadString( IDS_PROVIDER_MICROSOFT ) )
            {
            if ( _tcsstr( cszProviderToFind, cszProviderResourceName ) )
                {
                dwReturn = NETWORK_PROVIDER_MICROSOFT;
                goto ProviderNameFound;
                }
            }

                                        // Netware Network?
        if ( cszProviderResourceName.LoadString( IDS_PROVIDER_NETWARE ) )
            {
            if ( _tcsstr( cszProviderToFind, cszProviderResourceName ) )
                {
                dwReturn = NETWORK_PROVIDER_NETWARE;
                goto ProviderNameFound;
                }
            }
        }
    catch( CMemoryException )
        {
                                        // If a memory exception occurred while
                                        // loading a string, return FALSE.
        dwReturn = FALSE;
        }

                                        // Label for jumping out of our string
                                        // resource loading section.
ProviderNameFound:

    return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::InsertNetEnumItem
//
// Purpose:
//    Function to insert a single network enumeration item into the
//    network browser tree.  This function does not check to see if
//    it is an acceptable type, but rather only adds the item  the
//    verification for correct container class should be done
//    previous to this function.
//
// Parameters:
//    CNetworkTreeCtrl *lpNetTreeControl - [IN] Network Tree Control.
//    HTREEITEM hParentBranch        - [IN] Parent Branch.
//    LPNETRESOURCE lpNetResource    - [IN] Net Resource Struct.
//
// Returns:
//    BOOLEAN value as to whether we could insert this item or not.
//
// See Also:
//    CNetResourceContainer::EnumerateNetResourceBranch funciton.
//    This function enumerates the list and calls this function to
//    add each item it enumerates.
//
/////////////////////////////////////////////////////////////////////
// 11/01/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNetResourceContainer::InsertNetEnumItem(
    CNetworkTreeCtrl *lpNetTreeControl,
    HTREEITEM hParentBranch,
    LPNETRESOURCE lpNetResource
    )
{
CNetResourceContainer * lpcNewNetResourceContainer;
HTREEITEM hNewSubTree;
DWORD dwImageID;
DWORD dwItemType;
BOOL bRetVal=TRUE;
CString cszStringToAdd;
TV_ITEM tvItem;
TV_INSERTSTRUCT tvIS;

                                        // Create a new resource container.
    lpcNewNetResourceContainer = new CNetResourceContainer( lpNetResource, this );

                                        // If we got one...
    if ( NULL != lpcNewNetResourceContainer )
        {
                                        // Get the image ID (index) for this
                                        // type of network resource.
        dwImageID = GetImageIDForResource( lpNetResource );

                                        // Get the item type.
        dwItemType = lpcNewNetResourceContainer->GetItemType();


                                        // Format this resources name for
                                        // displaying in our tree.
        lpcNewNetResourceContainer->FormatResourceNameForDisplay( &cszStringToAdd );

        tvIS.hParent = hParentBranch;
        tvIS.hInsertAfter = TVI_LAST;
        tvIS.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_CHILDREN | TVIF_SELECTEDIMAGE |TVIF_STATE;
        tvIS.item.stateMask = TVIS_USERMASK;
        tvIS.item.pszText = cszStringToAdd.GetBuffer(0);
        tvIS.item.cchTextMax = cszStringToAdd.GetLength();
        tvIS.item.iImage = dwImageID;
        tvIS.item.iSelectedImage = dwImageID;
        tvIS.item.cChildren = 0;
        tvIS.item.lParam = NULL;

                                        // If we have a parent, keep the selected
                                        // setting that our parent has.
        if ( NULL != hParentBranch )
            {
            tvIS.item.state = TVIS_EXPANDED | lpNetTreeControl->GetItemState( hParentBranch, TVIS_USERMASK);
            }
        else
            {
            tvIS.item.state = TVIS_EXPANDED | INDEXTOSTATEIMAGEMASK(NOT_CHECKED);
            }

                                        // Insert that item with a pointer to the created
                                        // CNetResourceContainer class.  This will be
                                        // used later for further enumeration.
        hNewSubTree = lpNetTreeControl->InsertItem( &tvIS );

/*
        hNewSubTree = lpNetTreeControl->InsertItem( lpNetResource->lpRemoteName,
                                                 dwImageID,
                                                 dwImageID,
                                                 hParentBranch );
*/

        if ( NULL != hNewSubTree )
            {
                                        // Add Extra item if resource type
                                        // if we are supposed to be able to
                                        // browse beneath this item.
            if ( lpNetTreeControl->IsValidBrowseBeneathType( dwItemType ) )
                {
                                        // Fake out windows to think that it has
                                        // a child, even though we do not know
                                        // if it really does yet.
                tvItem.mask = TVIF_CHILDREN;
                tvItem.cChildren = 1;
                tvItem.hItem = hNewSubTree;
                lpNetTreeControl->SetItem( &tvItem );
                }

                                        // Set the item data to the resource container
                                        // allocated for this item.
            lpNetTreeControl->SetItemData( hNewSubTree, (DWORD)lpcNewNetResourceContainer );
            }
        else
            {
                                        // Could not add item, return FALSE.
            bRetVal = FALSE;
            }
        }

    return bRetVal;
}


/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::GetItemName
//
// Purpose:
//    This function will extract the item name from this
//    CNetResourceContainer class.  It will inspect the lpNetResource
//    structure to determin this name.
//
// Parameters:
//    CString *lpcszItemName - [OUT] This item's name.
//
// Returns:
//    Pointer to the string that we just retrieved.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
LPCTSTR CNetResourceContainer::GetItemName(
    CString *lpcszItemName
    )
{
LPCTSTR lpcszReturnString = NULL;
                                        // Handle the NULL case.
    if ( NULL != lpcszItemName )
        {
                                        // Empty the string in case we fail
                                        // somewhere.  If we fail, we want to return
                                        // an empty string.
        lpcszItemName->Empty();

                                        // If there is actually a structure
                                        // associated with this item...
        if ( NULL != m_lpNetResource )
            {
                                        // Format the name from the resource.
            FormatResourceNameForDisplay( lpcszItemName );

            if ( RESOURCE_TYPE_SERVER == MapNetResourceToType( m_lpNetResource ) )
                {
                                        // If item is server, prepend a "\\" in
                                        // front.
                *lpcszItemName = _T("\\\\") + *lpcszItemName;
                }

                                        // Return a pointer to this string.
            lpcszReturnString = (LPCTSTR)*lpcszItemName;
            }
        else
            {
                                        // Return NULL that we did not get
                                        // the item name.
            lpcszReturnString = NULL;
            }
        }
    else
        {
                                        // Return NULL if string pointer was NULL;
        lpcszReturnString = NULL;
        }


    return lpcszReturnString;
}


/////////////////////////////////////////////////////////////////////
// CNetResourceContainer::GetItemType
//
// Purpose:
//    Function to retrieve this item's type information.  We are
//    going to do this by calling a function to map the display type
//    to a resource type, and then returning that value.
//
// Parameters:
//    None.
//
// Returns:
//    Pointer to the string that we just retrieved.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CNetResourceContainer::GetItemType( void )
{
DWORD dwItemType;

                                        // Check the NULL case.
    if ( NULL != m_lpNetResource )
        {
        dwItemType = MapNetResourceToType( m_lpNetResource );
        }
    else
        {
        dwItemType = 0;
        }

    return dwItemType;
}

/////////////////////////////////////////////////////////////////////
// MapNetResourceToType
//
// Purpose:
//    Function to map the information from a network resource
//    structure to a type to be used for identification.
//
// Parameters:
//    LPNETRESOURCE lpNetResource - [IN] Item to map to a resource type.
//
// Returns:
//    ResourceType.  See NResCont.h for definition of types.
//
// See Also:
//    Definition of type in NResCont.h
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD MapNetResourceToType(
    LPNETRESOURCE lpNetResource
    )
{
DWORD dwItemType = RESOURCEDISPLAYTYPE_GENERIC;

                                        // Otherwise, switch on the display
                                        // type.
    switch( lpNetResource->dwDisplayType )
        {
        case RESOURCEDISPLAYTYPE_NETWORK:
                                        // If item is network, find out
                                        // which one.
                switch( GetProviderID( lpNetResource->lpProvider ) )
                    {
                    case NETWORK_PROVIDER_MICROSOFT:
                        dwItemType = RESOURCE_TYPE_NETWORK_MICROSOFT;
                        break;

                    case NETWORK_PROVIDER_NETWARE:
                        dwItemType = RESOURCE_TYPE_NETWORK_NETWARE;
                        break;
                    }
            break;

        case RESOURCEDISPLAYTYPE_GENERIC:

                                        // Check for the proveder name being the
                                        // same as the remote name.  If this is
                                        // the case, then we have a network.
                                        // This is for NT 3.51.  They return the
                                        // network providers as resource display
                                        // type generic.
            if ( lpNetResource->lpProvider && lpNetResource->lpRemoteName )
                {
                if ( NULL == _tcsicmp( lpNetResource->lpProvider, lpNetResource->lpRemoteName ) )
                    {
                                        // If item is network, find out
                                        // which one.
                    switch( GetProviderID( lpNetResource->lpProvider ) )
                        {
                        case NETWORK_PROVIDER_MICROSOFT:
                            dwItemType = RESOURCE_TYPE_NETWORK_MICROSOFT;
                            break;

                        case NETWORK_PROVIDER_NETWARE:
                            dwItemType = RESOURCE_TYPE_NETWORK_NETWARE;
                            break;
                        }
                    }
                }
            else
                {
                                        // Item is not a provider, so set
                                        // the type truly to generic.
                dwItemType = RESOURCE_TYPE_GENERIC;
                }
            break;

        case RESOURCEDISPLAYTYPE_DOMAIN:
            dwItemType = RESOURCE_TYPE_DOMAIN;
            break;

        case RESOURCEDISPLAYTYPE_TREE:
            dwItemType = RESOURCE_TYPE_TREE;
            break;

        case RESOURCEDISPLAYTYPE_NDSCONTAINER:
            dwItemType = RESOURCE_TYPE_NDSCONTAINER;
            break;

        case RESOURCEDISPLAYTYPE_SERVER:
            dwItemType = RESOURCE_TYPE_SERVER;
            break;

        case RESOURCEDISPLAYTYPE_SHARE:
            dwItemType = RESOURCE_TYPE_SHARE;
            break;
        };

    return dwItemType;
}

/////////////////////////////////////////////////////////////////////
// UINT SYM_EXPORT WINAPI SystemGetWindowsType(VOID)
//
// Purpose:
//  Local copy of the SystemGetWindowsType function.  If this module
//  ever links to the SymKrnl dll, then we can remove this.
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 06-13-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
UINT WINAPI SystemGetWindowsType(VOID)
{
    DWORD   dwVersion = GetVersion();


    if (dwVersion < 0x80000000)
        {
        return(SYM_SYSTEM_WIN_NT);
        }
    else if (LOBYTE(LOWORD(dwVersion)) < 4)
        {
        return(SYM_SYSTEM_WIN_32S);
        }
    else
        return(SYM_SYSTEM_WIN_95);
}
