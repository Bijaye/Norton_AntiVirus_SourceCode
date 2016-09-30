/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/EnumQuarantineItems.cpv   1.5   12 Jun 1998 21:06:30   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// EnumQuarantineItems.cpp: implementation of the CEnumQuarantineItems class.
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/EnumQuarantineItems.cpv  $
// 
//    Rev 1.5   12 Jun 1998 21:06:30   DBuches
// Ooops.  Need to initialize members in the constructor.
// 
//    Rev 1.4   09 Jun 1998 18:30:02   DBuches
// Fixed memory leak in CEnumQuarantineItems::Next().
// 
//    Rev 1.3   27 Feb 1998 12:34:50   DBuches
// Fixed problem with Next() method.  Removed memory leaks.
//
//    Rev 1.2   25 Feb 1998 15:08:38   DBuches
// Fixed problems in testing.
//
//    Rev 1.1   25 Feb 1998 14:07:24   DBuches
// Added inclusion of ER.H
//
//    Rev 1.0   25 Feb 1998 14:00:30   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnumQuarantineItems.h"
#include "quarantineitem.h"
#include "er.h"

extern LONG g_dwObjs;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
// Function name	: CEnumQuarantineItems::CEnumQuarantineItems
//
// Description	    : Constructor
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CEnumQuarantineItems::CEnumQuarantineItems()
{
    // Bump global object count
    InterlockedIncrement( &g_dwObjs );

    m_dwRef = 0;
    m_pQuarantineFolderEnum = NULL;
    m_pMalloc = NULL;
    m_pFolder = NULL; 
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CEnumQuarantineItems::~CEnumQuarantineItems
//
// Description	    : Destructor
//
// Return type		:
//
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CEnumQuarantineItems::~CEnumQuarantineItems()
{
    // Release objects
    if( m_pQuarantineFolderEnum )
        {
        m_pQuarantineFolderEnum->Release();
        m_pQuarantineFolderEnum = NULL;
        }

    if( m_pFolder )
        {
        m_pFolder->Release();
        m_pFolder = NULL;
        }

    if( m_pMalloc )
        {
        m_pMalloc->Release();
        m_pMalloc = NULL;
        }

    InterlockedDecrement( &g_dwObjs );
}



///////////////////////////////////////////////////////////////////
// IUnknown implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CEnumQuarantineItems::QueryInterface
// Description	    : This function will return a requested COM interface
// Return type		: STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_EnumQuarantineItems) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CEnumQuarantineItems::AddRef()
// Description	    : Increments reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CEnumQuarantineItems::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CEnumQuarantineItems::Release()
// Description	    : Decrements reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CEnumQuarantineItems::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// IEnumQuarantineItems


////////////////////////////////////////////////////////////////////////////
// Function name	: CEnumQuarantineItems::Next
//
// Description	    : Advances enumeration object, returning items
//
// Return type		: HRESULT - NOERROR on success, S_FALSE if no more items or
//                    OLE defined error code.
//
// Argument         : ULONG celt - number of elements to fetch
// Argument         : IQuarantineItem * pItems - array of items to fetch
// Argument         : ULONG * pceltFetched - number actually fetched.
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Next( ULONG celt,
    IQuarantineItem ** pItems,
    ULONG * pceltFetched )
{
    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    // Fetch next item in IShellFolder enumeration
    STRRET str;
    ULONG uAttr;
    char szFileName[MAX_PATH];
    LPITEMIDLIST pidl;
    HRESULT hr;

    // Keep trying until there are no more files to iterate or we
    // successfully initialize a quarantine item.
    do
        {
        // Enumerate these objects
        hr = m_pQuarantineFolderEnum->Next( 1, &pidl, pceltFetched );
        if( hr == S_FALSE || FAILED( hr ) )
            return hr;

        // Get attributes of this item
        hr = m_pFolder->GetAttributesOf( 1, (LPCITEMIDLIST*)&pidl, &uAttr );
        if( FAILED( hr ) )
            {
            m_pMalloc->Free( pidl );
            return hr;
            }

        // Skip this guy if its a folder
        if( uAttr & SFGAO_FOLDER )
            {
            m_pMalloc->Free( pidl );
            continue;
            }

        // Get path for this item
        hr = m_pFolder->GetDisplayNameOf( pidl, SHGDN_FORPARSING, &str);
        if( FAILED( hr ) )
            {
            m_pMalloc->Free( pidl );
            return hr;
            }

        // Get string from display name
        StringFromStrRet(&str, pidl, szFileName );

        // We don't need the PIDL anymore, so delete it
        m_pMalloc->Free( pidl );

        // Now that we have the file name, create a quarantine item
        // object for this file.
        CQuarantineItem* p = new CQuarantineItem;
        if( p == NULL )
            return ResultFromScode( E_OUTOFMEMORY );

        // Make sure the interface is supported.
        hr = p->QueryInterface( IID_QuarantineItem, (LPVOID*)pItems );
        if( FAILED( hr ) )
            {
            delete p;
            return hr;
            }

        // Initialize our object.
        hr = p->Initialize( szFileName );
		if( FAILED( hr ) )
            {
			delete p;
			}
        }
    while( FAILED( hr ) );

    return NOERROR;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CEnumQuarantineItems::Skip
//
// Description	    : Advances Enumeration by celt objects
//
// Return type		: HRESULT - S_OK on success, S_FALSE otherwise
//
// Argument         : ULONG celt
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Skip( ULONG celt )
{
    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    return m_pQuarantineFolderEnum->Skip( celt );
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CEnumQuarantineItems::Reset
//
// Description	    : Resets this iteration object
//
// Return type		: HRESULT - S_OK on success, S_FALSE otherwise.
//
// Argument         : void
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Reset( void )
{
    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    return m_pQuarantineFolderEnum->Reset();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CEnumQuarantineItems::Clone
//
// Description	    : Creats a copy of this enumeration object
//
// Return type		: HRESULT - S_OK on success.  Can also return
//                    E_INVALIDARG, E_OUTOFMEMORY, and E_UNEXPECTED.
//
// Argument         : IEnumQuarantineItems * ppvOut
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Clone( IEnumQuarantineItems ** ppvOut )
{
    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    // Create new object
    CEnumQuarantineItems* p = new CEnumQuarantineItems;
    if( p == NULL )
        return E_OUTOFMEMORY;

    // Make sure the interface is supported.
    HRESULT hr = p->QueryInterface( IID_QuarantineItem, (LPVOID*) ppvOut );
    if( FAILED( hr ) )
        {
        delete p;
        return E_UNEXPECTED;
        }

    // Set up Allocator
    if( NOERROR != SHGetMalloc( &p->m_pMalloc ) )
        {
        delete p;
        return E_UNEXPECTED;
        }

    // Clone the state of our enumeration object
    m_pQuarantineFolderEnum->Clone( &p->m_pQuarantineFolderEnum );

    // Copy pointer to shell folder.
    p->m_pFolder = m_pFolder;

    // Bump reference count for this object
    m_pFolder->AddRef();

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////
// Private members
////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Function name	: CEnumQuarantineItems::Initialize
//
// Description	    : Initializes this enumeration object
//
// Return type		: HRESULT
//
// Argument         : LPSTR lpszFolder - folder to enumerate.
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
HRESULT CEnumQuarantineItems::Initialize( LPSTR lpszFolder )
{
    // Get pointer to Desktop IShellFolder interface.
    LPSHELLFOLDER pShellFolder;
    if( NOERROR != SHGetDesktopFolder( &pShellFolder ) )
        return E_UNEXPECTED;

    // Make sure we get released
    CEnsureRelease er( pShellFolder );

    // Get pointer to shells allocator object
    if( NOERROR != SHGetMalloc( &m_pMalloc ) )
        return E_UNEXPECTED;

    // Convert to Wide char for ParseDisplayName
    WCHAR szwItem[MAX_PATH];
    MultiByteToWideChar( CP_ACP,
                         MB_PRECOMPOSED,
                         lpszFolder,
                         -1,
                         szwItem,
                         MAX_PATH );

    // Get pidl for this item
    ULONG chEaten;
    DWORD dwAttr;
    LPITEMIDLIST pidl = NULL;
    if(!SUCCEEDED(pShellFolder->ParseDisplayName(
                                     NULL,
                                     NULL,
                                     szwItem,
                                     &chEaten,
                                     &pidl,
                                     &dwAttr)))
        {
        return E_UNEXPECTED;
        }

    // Bind to this object
    HRESULT hResult = pShellFolder->BindToObject( pidl, NULL, IID_IShellFolder, (LPVOID*) &m_pFolder );

    if( !SUCCEEDED(hResult) )
        {
        return E_UNEXPECTED;
        }


    // Get pointer to EnumIDList object
    hResult = m_pFolder->EnumObjects( NULL,
                        SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN,
                        &m_pQuarantineFolderEnum );

    if( !SUCCEEDED(hResult) )
        {
        return E_UNEXPECTED;
        }

    return S_OK;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name    : CFileIterator::StringFromStrRet
// Description      : Given an OLE STRRET structure, will return a LPSTR
//                    containing the text returned from an OLE-based Enum call
// Return type      : void
// Argument         : STRRET* p - STRRET structure to translate.
// Argument         : LPITEMIDLIST pidl - ITEMIDLIST used for offset computation
// Argument         : LPSTR szFileName - MAX_PATH length destination buffer
//
////////////////////////////////////////////////////////////////////////////////
// 8/30/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CEnumQuarantineItems::StringFromStrRet(STRRET* p, LPITEMIDLIST pidl, LPSTR szFileName )
{
    switch(p->uType)
        {
        case STRRET_CSTR:
            lstrcpyn(szFileName, p->cStr, MAX_PATH);
            break;

        case STRRET_WSTR:
            WideCharToMultiByte( CP_ACP,
                                 0,
                                 p->pOleStr,
                                 -1,
                                 szFileName,
                                 MAX_PATH,
                                 NULL,
                                 NULL);
            m_pMalloc->Free(p->pOleStr);
            break;

        case STRRET_OFFSET:
            {
            LPTSTR pStr;
            pStr = (LPTSTR) ((LPBYTE)pidl + p->uOffset);
            lstrcpyn(szFileName, pStr, MAX_PATH);
            }
            break;
        }
}

