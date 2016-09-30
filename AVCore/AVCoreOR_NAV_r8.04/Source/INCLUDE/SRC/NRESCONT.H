/////////////////////////////////////////////////////////////////////
// NRESCONT.H - Created - 11/01/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/NRESCONT.H_v   1.0   21 Nov 1997 11:53:04   KSACKIN  $
//
// Description:
//    Header file for the CNetResourceContainer class.  This class is
//    going to be used as the extra data item for each branch of the
//    network tree.  This class will store data about the current
//    network enumeration so that we may enumerate further levels
//    if we are so requested to do.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/NRESCONT.H_v  $
// 
//    Rev 1.0   21 Nov 1997 11:53:04   KSACKIN
// Initial revision.
// 
//    Rev 1.1   30 Jul 1997 16:11:10   KSACKIN
// Added defines for the checked state of our Network resources.
// 
//    Rev 1.0   24 Apr 1997 20:31:24   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:44   RFULLER
// Initial revision
// 
//    Rev 1.3   20 Dec 1996 20:17:14   ksackin
// Added the resource type of RESOURCE_TYPE_CUSTOM for when a user types in a 
// resource name, and we cannot verifiably detect what the resource type is.
// 
//    Rev 1.2   16 Dec 1996 23:46:14   ksackin
// Modified some of the data storage in this class so that we store our own
// copy of the LPNETRESOURCE structure and the string within.  This needed to be
// fixed so that we can do multiple enumerations on the same level of the
// network for large networks.
// 
//    Rev 1.1   05 Dec 1996 02:08:02   ksackin
// Added defines for Network Resource Types.  These are to be used for
// determining if a resource is a type that we wish to display, browse beneath,
// or eventually, select.
// 
//    Rev 1.0   20 Nov 1996 02:13:22   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__NRESCONT_H)
#define __NRESCONT_H

#include "NetTree.h"


/////////////////////////////////////////////////////////////////////////////
// #defines

                                        // Network Provider ID's
#define NETWORK_PROVIDER_MICROSOFT 0x00000001
#define NETWORK_PROVIDER_NETWARE   0x00000002
#define NETWORK_PROVIDER_ALL       0xffffffff


#define RESOURCE_TYPE_NETWORK_MICROSOFT     0x00000001
#define RESOURCE_TYPE_NETWORK_NETWARE       0x00000002
#define RESOURCE_TYPE_DOMAIN                0x00000004
#define RESOURCE_TYPE_GROUP                 0x00000008
#define RESOURCE_TYPE_TREE                  0x00000010
#define RESOURCE_TYPE_NDSCONTAINER          0x00000020
#define RESOURCE_TYPE_SERVER                0x00000040
#define RESOURCE_TYPE_SHARE                 0x00000080
#define RESOURCE_TYPE_FILE                  0x00000100
#define RESOURCE_TYPE_GENERIC               0x00000200

                                        // To be used when an item is entered
                                        // manually, and we may not know what
                                        // type of object it is.
#define RESOURCE_TYPE_CUSTOM                0xf0000000

#define RESOURCE_TYPE_ALL                   0xffffffff


#define NOT_CHECKED     0x00000001
#define CHECKED         0x00000002
#define GRAY_CHECKED    0x00000004


///////////////////////////////////////////////////////////////////
// Function Prototypes

                                        // Get an image index for a resource
                                        // type designated by the lpNetResource
                                        // structure.
DWORD GetImageIDForResource(
    LPNETRESOURCE lpNetResource
    );
                                        // Get a provider ID for a privider
                                        // string passed.
DWORD GetProviderID( 
    LPCTSTR cszProvider
    );

                                        // Function to map a net resource
                                        // struct to an item resource type.
DWORD MapNetResourceToType( 
    LPNETRESOURCE lpNetResource
    );

LPTSTR MapStringPointerToCopy( 
    CString *lpcszDestStr, 
    LPTSTR lpszStringPointerToMap 
    );


///////////////////////////////////////////////////////////////////
// CNetResourceContainer class.

class AFX_EXT_CLASS CNetResourceContainer
{
public:
	DWORD GetItemType( void );
    CNetResourceContainer( 
        LPNETRESOURCE lpNetResource = NULL,
        CNetResourceContainer *lpParentContainer = NULL
        );

    ~CNetResourceContainer( void );

                                        // Function to initialize a container.
    BOOL InitContainer( LPNETRESOURCE lpNetResource, CNetResourceContainer *lpParentContainer = NULL );
    
                                        // Member function to enumerate this
                                        // level of the network tree.
    EnumerateNetResourceBranch(
        CNetworkTreeCtrl *lpNetTreeControl,
        HTREEITEM hParentBranch
        );
                                        // Actually inserts an item based upon
                                        // an LPNETRESOURCE structure. (Only
                                        // adds types that this dialog has been
                                        // told to)
    BOOL InsertNetEnumItem(
        CNetworkTreeCtrl *lpNetTreeControl,
        HTREEITEM hParentBranch,
        LPNETRESOURCE lpNetResource
        );

                                        // Function to set the number of
                                        // children for a particular branch.
    BOOL SetItemChildrenCount( 
        CTreeCtrl *lpTreeControl, 
        HTREEITEM hBranchToChange, 
        int nNewcChildren 
        );

                                        // This is the default function.
                                        // the next functions are called
                                        // from this function.
    BOOL FormatResourceNameForDisplay(
        CString        *lpcszDisplayString
        );

                                        // This is a specific implementation 
                                        // for standard resource display types.
                                        // This should only be called from
                                        // FormatResourceNameForDisplay(). 
    BOOL FormatStandardResourceNameForDisplay(
        CString        *lpcszDisplayString
        );

                                        // This is a specific implementation 
                                        // for Netware resource display types.
                                        // This should only be called from
                                        // FormatResourceNameForDisplay(). 
    BOOL FormatNetwareResourceNameForDisplay(
        CString        *lpcszDisplayString
        );

                                        // Member data retrieval.
    LPNETRESOURCE GetNetResourceStruct( void ) {return m_lpNetResource;};

                                        // This function will return whether
                                        // this item resides within, or is, an
                                        // NDS tree.
    BOOL IsNDSTreeItem( void ) {return m_bNDSTreeItem;};

                                        // Gets this item's name, or returns
                                        // NULL if it cannot be determined.
	LPCTSTR GetItemName( CString *lpcszItemName );

private:
    CNetResourceContainer  *m_lpParentContainer;
    HANDLE                  m_hNetEnumResource;
    BOOL                    m_bEnumeratedOnce;
    BOOL                    m_bNDSTreeItem;

                                        // Local copy of network resource struct.
    NETRESOURCE             m_NetResource;
    LPNETRESOURCE           m_lpNetResource;

                                        // Local copies of strings because these
                                        // strings are no longer valid objects once
                                        // the network enumeration closes.
    CString                 m_cszLocalName;
    CString                 m_cszRemoteName;
    CString                 m_cszComment;
    CString                 m_cszProvider;
};


#endif // if !defined(__NRESCONT_H)
