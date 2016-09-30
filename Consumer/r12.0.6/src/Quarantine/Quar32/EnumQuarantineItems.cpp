//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// EnumQuarantineItems.cpp: implementation of the CEnumQuarantineItems class.
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// 2/24/98 DBUCHES - Function created / Header added.
CEnumQuarantineItems::CEnumQuarantineItems()
{
    m_pQuarantineFolderEnum = NULL;
    m_pMalloc = NULL;
    m_pFolder = NULL; 
}


// 2/24/98 DBUCHES - Function created / Header added.
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
}


////////////////////////////////////////////////////////////////////////////////
// IEnumQuarantineItems


// ==== Next ==========================================================
// Advances enumeration object, returning items
//
// Input: ULONG celt - number of elements to fetch
//		  IQuarantineItem * pItems - array of items to fetch
//		  ULONG* pceltFetched - number actually fetched.
//
// Output: HRESULT - NOERROR on success, S_FALSE if no more items or OLE
//						defined error code.
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Next( ULONG celt,
    IQuarantineItem ** pItems,
    ULONG * pceltFetched )
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    // Fetch next item in IShellFolder enumeration
    STRRET str;
    ULONG uAttr;
    char szQuarantineItemPath[MAX_PATH];
    LPITEMIDLIST pidl;
    HRESULT hr;
	DWORD dwFileAttribute;

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

        // Get path for this item
        hr = m_pFolder->GetDisplayNameOf( pidl, SHGDN_FORPARSING, &str);
        if( FAILED( hr ) )
            {
            m_pMalloc->Free( pidl );
            return hr;
            }

        // Get string from display name
        StringFromStrRet(&str, pidl, szQuarantineItemPath );

        // Skip this guy if its a folder
		// Defect# 341029. On Win2K, GetAttributesOf sets the SFGAO_FOLDER
		// flag for CAB files. Therefore causing us to think CAB files 
		// are folders. Switched to use GetFileAttributes().
		dwFileAttribute = GetFileAttributes(szQuarantineItemPath);
		if((INVALID_FILE_ATTRIBUTES != dwFileAttribute) && ((dwFileAttribute & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY))
		{
			m_pMalloc->Free( pidl );
			continue;
		}

        // We don't need the PIDL anymore, so delete it
        m_pMalloc->Free( pidl );

        // Now that we have the file name, create a quarantine item
        // object for this file.
        (*pItems) = new CQuarantineItem;
        if( (*pItems) == NULL )
            return ResultFromScode( E_OUTOFMEMORY );

        // Reference the object
        (*pItems)->AddRef();

        // Initialize our object.
        hr = (*pItems)->Initialize(szQuarantineItemPath);
		if( FAILED( hr ) )
            {
            CCTRACEE(_T("CEnumQuarantineItems::Next() - Failed to initialize the quarantine item: %s"), szQuarantineItemPath);
			(*pItems)->Release();
            *pItems = NULL;
			}
        }
    while( FAILED( hr ) );

    return NOERROR;
}


// ==== Skip ==========================================================
// Advances Enumeration by celt objects
// Input: ULONG celt
// Output: HRESULT - S_OK on success, S_FALSE otherwise
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Skip( ULONG celt )
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    return m_pQuarantineFolderEnum->Skip( celt );
}


// ==== Reset ==========================================================
// Resets this iteration object
// Output: HRESULT - S_OK on success, S_FALSE otherwise.
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Reset( void )
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    return m_pQuarantineFolderEnum->Reset();
}


// ==== Clone ==========================================================
// Creats a copy of this enumeration object
//
// Input: IEnumQuarantineItems * ppvOut
// Output: HRESULT - S_OK on success.  
//						Can also return E_INVALIDARG, E_OUTOFMEMORY, and E_UNEXPECTED.
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CEnumQuarantineItems::Clone( IEnumQuarantineItems ** ppvOut )
{
    ccLib::CSingleLock Lock(&m_critSec, INFINITE, FALSE);

    if( m_pQuarantineFolderEnum == NULL )
        return E_UNEXPECTED;

    // Create new object
    CEnumQuarantineItems* p = new CEnumQuarantineItems;
    if( p == NULL )
        return E_OUTOFMEMORY;

    // Assign the interface
    *ppvOut = p;
    p->AddRef();

    // Set up Allocator
    if( NOERROR != SHGetMalloc( &p->m_pMalloc ) )
        {
        CCTRACEE(_T("CEnumQuarantineItems::Clone() - Failed to set up the allocator"));
        p->Release();
        p = NULL;
        *ppvOut = NULL;
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
////////////////////////////////////////////////////////////////////////////
// Private members
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// ==== Initialize ==========================================================
// Initializes this enumeration object
// Input: LPSTR lpszFolder - folder to enumerate.
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


// ==== StringFromStrRet =====================================================
// Given an OLE STRRET structure, will return a LPSTR containing the text
// returned from an OLE-based Enum call
// Input: STRRET* p			- STRRET structure to translate.
//		  LPITEMIDLIST pidl - ITEMIDLIST used for offset computation
//		  LPSTR szFileName	- MAX_PATH length destination buffer
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
