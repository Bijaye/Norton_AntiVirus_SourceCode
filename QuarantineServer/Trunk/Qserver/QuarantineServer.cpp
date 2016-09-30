/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// QuarantineServer.cpp : Implementation of CQuarantineServer
#include "stdafx.h"
#include "Qserver.h"
#include "qdefs.h"
#include "QuarantineServer.h"
#include "EnumQserverItemsVariant.h"
#include "QuarantineServerItem.h"
#include "const.h"
#include "qspak.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////
// CQuarantineServer



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::CQuarantineServer
//
// Description: Constructor 
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CQuarantineServer::CQuarantineServer()
    : m_pEnumerator( NULL )
{
	if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_MED) == DEBUGLEVEL_MED)
	{
		fWidePrintString("CQuarantineServer::CQuarantineServer  this: 0x%lx",this );
	}
}                       






///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::EnumItems
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: IEnumQuarantineItems **pEnum
//
///////////////////////////////////////////////////////////////////////////////
// 12/1/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::EnumItems(IEnumQuarantineServerItems **pEnum)
{
    HRESULT hr = S_OK;
	int i = 0;

    while( i <  MAX_WAIT_FOR_QF_MOVE)
	{
        
        // 
        // Wait here for one if qfolder is being moved
		DWORD dwWait = WaitForSingleObject(_Module.m_hQFolderMoveEvent,1000);
        if( dwWait == WAIT_TIMEOUT  )
		{
            // 
            // We timed out check to see if long enough
            // 
			++i;
		}
		else
			break;
	}
    // 
    // Set up enumeration
    // 
    if( m_pEnumerator )
        delete m_pEnumerator;
    
    // 
    // Create enumerator helper object
    //
    TCHAR szQuarantineDir[ MAX_PATH + 1 ];
    _Module.GetQuarantineDir( szQuarantineDir );
    try
        {
        m_pEnumerator = new CEnumerator( szQuarantineDir );

        if( m_pEnumerator->Enumerate() == FALSE )
            hr = E_FAIL;
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }
    
	// 
    // Since this object should support the IEnumQuarantineServerItems, just bump the 
    // internal reference count of this object and return.
    // 
    return SUCCEEDED( hr ) ? QueryInterface( __uuidof( IEnumQuarantineServerItems ), (LPVOID*) pEnum ) : hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::GetCopyInterface
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: ICopyQuarantineItem** pEnum
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CQuarantineServer::GetCopyInterface( ICopyItemData** pCopy )
{
    HRESULT hr;
	int i = 0;

    while( i <  MAX_WAIT_FOR_QF_MOVE)
	{
        
        // 
        // Wait here for one if qfolder is being moved
		DWORD dwWait = WaitForSingleObject(_Module.m_hQFolderMoveEvent,1000);
        if( dwWait == WAIT_TIMEOUT  )
		{
            // 
            // We timed out check to see if long enough
            // 
			++i;
		}
		else
			break;
	}
    try
        {
        hr = CoCreateInstance( __uuidof( CopyItemData ),
                               NULL,
                               CLSCTX_ALL,
                               __uuidof( ICopyItemData ),
                               (LPVOID*) pCopy );
		if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_CONFIG_INTERFACE) == DEBUGLEVEL_CONFIG_INTERFACE	)
			fWidePrintString("CQuarantineServer::GetCopyInterface, pCopy  0x%x", *pCopy);
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::RemoveItem
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: LPSTR szItemFileName
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::RemoveItem( ULONG ulFileID )
{
	int i = 0;

    while( i <  MAX_WAIT_FOR_QF_MOVE)
	{
        
        // 
        // Wait here for one if qfolder is being moved
		DWORD dwWait = WaitForSingleObject(_Module.m_hQFolderMoveEvent,1000);
        if( dwWait == WAIT_TIMEOUT  )
		{
            // 
            // We timed out check to see if long enough
            // 
			++i;
		}
		else
			break;
	}
    // 
    // Check to see if someone is holding a pointer to this item
    // 
    if( _Module.IsItemOpen( ulFileID ) )
        return E_ACCESSDENIED;

    // 
    // Build full path to file.
    // 
    TCHAR szFilePath[ MAX_PATH + 1];
    TCHAR szQuarantineDir[ MAX_PATH + 1 ];
    _Module.GetQuarantineDir( szQuarantineDir );
    wsprintf( szFilePath, _T("%s\\%.8X"), szQuarantineDir, ulFileID );

    // 
    // Make sure the file exists.
    // 
    if( GetFileAttributes( szFilePath ) == 0xFFFFFFFF )
        return E_INVALIDARG;

    // 
    // Need to reduce total byte count of quarantine server by file size.
    //
    ULONG  ulSize = 0;
    HANDLE hFile = CreateFile( szFilePath, 
                               GENERIC_READ, 
                               FILE_SHARE_READ, 
                               NULL,
                               OPEN_EXISTING,
                               0,
                               NULL );
    if( hFile != INVALID_HANDLE_VALUE )
        {
        ulSize = (LONG) GetFileSize( hFile, NULL );
        CloseHandle( hFile );
        }

    // 
    // Blow the file away!
    // 
    BOOL bRet = DeleteFile( szFilePath );
    if( bRet )
        {
        // 
        // Decrement total file counter.
        // 
        InterlockedDecrement( (LONG*)&_Module.m_ulItemCount );
 
        // 
        // Decrement total byte count.
        // 
        if( ulSize != 0xFFFFFFFF )
            {
			if (ulSize < _Module.m_uQserverByteCount)
				InterlockedExchangeAdd( (LONG*)&_Module.m_uQserverByteCount, -((LONG)(ulSize)) );
			else
				InterlockedExchange((LONG*)&_Module.m_uQserverByteCount,0);
            }
        return S_OK;
        }

    // 
    // Figure out what happened.
    // 
    switch( GetLastError() )
        {
        case ERROR_ACCESS_DENIED:
            return E_ACCESSDENIED;
        }

    return E_FAIL;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::RemoveAllItems
//
// Description: 
//
// Return type: STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/3/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::RemoveAllItems()
{
	int i = 0;

    while( i <  MAX_WAIT_FOR_QF_MOVE)
	{
        
        // 
        // Wait here for one if qfolder is being moved
		DWORD dwWait = WaitForSingleObject(_Module.m_hQFolderMoveEvent,1000);
        if( dwWait == WAIT_TIMEOUT  )
		{
            // 
            // We timed out check to see if long enough
            // 
			++i;
		}
		else
			break;
	}
    // 
    // Nuke contents of quarantine folder.
    // 
    HRESULT hr = S_OK;
    CEnumerator* pEnum = NULL;
    TCHAR szQuarantineDir[ MAX_PATH + 1 ];
    TCHAR szFile[ MAX_PATH + 1];
    TCHAR szFullPath[ MAX_PATH + 1];

    _Module.GetQuarantineDir( szQuarantineDir );

    try
        {
        // 
        // Enumerate the quarantine folder.
        // 
        pEnum = new CEnumerator( szQuarantineDir );
        if( FALSE == pEnum->Enumerate() )
            throw(0);
        
        // 
        // Nuke all quarantine files.
        // 
        while( pEnum->GetNext( szFile ) )
            {
            wsprintf( szFullPath, _T("%s\\%s"), szQuarantineDir, szFile );
            DeleteFile( szFullPath );
            }

        // 
        // Set new size to 0.
        // 
        _Module.m_uQserverByteCount = 0;
        _Module.m_ulItemCount = 0;
        }
    catch(...)
        {
        hr=E_UNEXPECTED;
        }

    if( pEnum )
        delete pEnum;

	return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServer::EnumItemsVariant
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : VARIANT va
// Argument      : IEnumQserverItemsVariant** pEnum
//
///////////////////////////////////////////////////////////////////////////////
// 1/20/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::EnumItemsVariant( VARIANT* va, 
                                                  IEnumQserverItemsVariant** pEnum )
{
	int i = 0;

    while( i <  MAX_WAIT_FOR_QF_MOVE)
	{
        
        // 
        // Wait here for one if qfolder is being moved
		DWORD dwWait = WaitForSingleObject(_Module.m_hQFolderMoveEvent,1000);
        if( dwWait == WAIT_TIMEOUT  )
		{
            // 
            // We timed out check to see if long enough
            // 
			++i;
		}
		else
			break;
	}

    // 
    // First, create an object.
    // 
    IEnumQserverItemsVariant* pObject;
    HRESULT hr = CoCreateInstance( __uuidof( EnumQserverItemsVariant ),
                                   NULL,
                                   CLSCTX_ALL,
                                   __uuidof( IEnumQserverItemsVariant ),
                                   (LPVOID*) &pObject );

    if( FAILED( hr ) )
        return hr;

    *pEnum = pObject;

    // 
    // Initialize the object.
    // 
    CEnumQserverItemsVariant* pInternal = (CEnumQserverItemsVariant*) pObject;
    TCHAR szQuarantineDir[ MAX_PATH + 1 ];
    _Module.GetQuarantineDir( szQuarantineDir );
    hr = pInternal->Initialize( szQuarantineDir, *va );

	if (FAILED(hr))
	{
		pObject->Release();
		*pEnum = NULL;
	}

    return hr;
}
 

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServer::GetQuarantineItem
//
// Description   : This routine will fetch a Quarantine Server object for
//                 a given item ID.
//
// Return type   : STDMETHODIMP 
//
// Argument      : ULONG ulItem
// Argument      : IQuarantineServerItem** pItem
//
///////////////////////////////////////////////////////////////////////////////
// 1/22/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::GetQuarantineItem(ULONG ulItem,
                                                  IQuarantineServerItem** pItem )
{
    HRESULT hr;
	int i = 0;

    while( i <  MAX_WAIT_FOR_QF_MOVE)
	{
        
        // 
        // Wait here for one if qfolder is being moved
		DWORD dwWait = WaitForSingleObject(_Module.m_hQFolderMoveEvent,1000);
        if( dwWait == WAIT_TIMEOUT  )
		{
            // 
            // We timed out check to see if long enough
            // 
			++i;
		}
		else
			break;
	}

    try
        {
        // 
        // First, create an object.
        // 
        IQuarantineServerItem* pObject;
        hr = CoCreateInstance( __uuidof( QuarantineServerItem ),
                                       NULL,
                                       CLSCTX_ALL,
                                       __uuidof( IQuarantineServerItem ),
                                       (LPVOID*) &pObject );

        if( FAILED( hr ) )
            return hr;

        *pItem = pObject;

        // 
        // Initialize the object.
        // 
        CQuarantineServerItem* pInternal = (CQuarantineServerItem*) pObject;
        TCHAR szFilePath[ MAX_PATH + 1];
        TCHAR szQuarantineDir[ MAX_PATH + 1 ];
        _Module.GetQuarantineDir( szQuarantineDir );

        wsprintf( szFilePath, _T( "%s\\%.8X" ), szQuarantineDir, ulItem );
        hr = pInternal->Initialize( szFilePath, ulItem );
        if( SUCCEEDED( hr ) )
            {
            // 
            // Add this item to the list of open items.  
            // 
            _Module.AddOpenItem( ulItem, pInternal ); 
            }
		else
		{
			// tam 5-18-00 error condition clean up
			if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
			{
				fWidePrintString("CQuarantineServer::GetQuarantineItem INIT FAILED ItemName  %s, pQServerItemInterface: 0x%lx",
					szFilePath, pObject );
			}
			pInternal->Release();
			*pItem = NULL;
		}

        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }
    
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServer::Config
//
// Description   : Returns a pointer to a configuration object.  Also keeps
//                 track of the number of config objects.
//
// Return type   : STDMETHODIMP 
//
// Argument      : IQserverConfig ** pConfig
//
///////////////////////////////////////////////////////////////////////////////
// 1/25/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::Config( IQserverConfig ** pConfig )
{
    // 
    // Make sure there is no one else with a configuration object
    // hanging around.
    // 
    if( _Module.m_iConfigCount != 0 )
        return S_FALSE;
	_Module.m_dwTicktime = GetTickCount();
    // 
    // Create a configuration object.
    // 
    IQserverConfig* pObject;
    HRESULT hr = CoCreateInstance( __uuidof( QserverConfig ),
                                  NULL,
                                  CLSCTX_ALL,
                                  __uuidof( IQserverConfig ),
                                  (LPVOID *) &pObject );

    // 
    // If we got a configuration object, bump config user count.
    // 
    if( SUCCEEDED( hr ) )
        {
        InterlockedIncrement( &_Module.m_iConfigCount );
        *pConfig = pObject;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::GetItemCount
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : ULONG* pulCount
//
///////////////////////////////////////////////////////////////////////////////
// 4/7/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::GetItemCount( ULONG* pulCount )
{
    try
        {
        *pulCount = _Module.GetItemCount();
        }
    catch(...)
        {
        return E_INVALIDARG;
        }

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// IEnumQuarantineServerItems
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::Next
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG celt,                 
// Argument: LPQSERVERITEM* aQserverItems
// Argument: ULONG* pceltFetched
//
///////////////////////////////////////////////////////////////////////////////
// 12/1/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CQuarantineServer::Next( ULONG celt, 
			                          IQuarantineServerItem** aQserverItems, 
			                          ULONG* pceltFetched )
{
    ULONG ulNumReturned = 0;
         
    // 
    // Make sure caller knows what he is doing.
    // 
    if( pceltFetched == NULL && celt != 1 )
        return S_FALSE;
    else
        *pceltFetched = 0;

    // 
    // Make sure pointer is good
    // 
    if( aQserverItems == NULL )
        return E_POINTER;
    
    // 
    // Fetch the correct number of items.
    // 
    ULONG total = celt;
    TCHAR szItemName[ MAX_PATH + 1];
    HRESULT hr = S_OK;
    
    while( celt > 0 )
        {
        if( m_pEnumerator->GetNext( szItemName ) )
            {
            // 
            // Create and initialize a quarantine server object.
            // 
            hr = CreateQserverItem( szItemName, &aQserverItems[ total - celt ] );
            if( FAILED( hr ) )
                {
				continue;
                }

            ulNumReturned++;
            }
        else 
            break;
        
        celt--;
        }

    if( pceltFetched != NULL )
        *pceltFetched = ulNumReturned;

    if( ulNumReturned == 0 )
        return S_FALSE;

    // 
    // Success!
    // 
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::Skip
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: ULONG celt
//
///////////////////////////////////////////////////////////////////////////////
// 12/1/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::Skip( ULONG celt )
{
    // 
    // Make sure all is well
    // 
    if( m_pEnumerator == NULL )
        return E_UNEXPECTED;

    // 
    // Skip down the list.
    // 
    TCHAR szItem[ MAX_PATH + 1];
    while( celt > 0 && m_pEnumerator->GetNext( szItem ) )
        {
        celt --;
        }

    return S_OK;    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::Reset
//
// Description: 
//
// Return type: STDMETHODIMP 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/1/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::Reset()
{
    HRESULT hr = S_OK;

    // 
    // Make sure all is well
    // 
    if( m_pEnumerator == NULL )
        return E_UNEXPECTED;

    // 
    // Remove old enumerator.
    // 
    delete m_pEnumerator;

    // 
    // Create enumerator helper object
    //
    TCHAR szQuarantineDir[ MAX_PATH + 1 ];
    _Module.GetQuarantineDir( szQuarantineDir );

    try
        {
        m_pEnumerator = new CEnumerator( szQuarantineDir );
        if( m_pEnumerator->Enumerate() == FALSE )
            hr = E_FAIL;
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::Clone
//
// Description: 
//
// Return type: STDMETHODIMP 
//
// Argument: IEnumQuarantineServerItems** ppenum
//
///////////////////////////////////////////////////////////////////////////////
// 12/1/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::Clone( IEnumQuarantineServerItems** ppenum )
{
    HRESULT hr = E_NOTIMPL;
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ICopyQuarantineItem
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Overrrides
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQuarantineServer::FinalRelease
//
// Description   : 
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/2/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQuarantineServer::FinalRelease()
{
    // 
    // Cleanup
    // 
    if( m_pEnumerator )
        {
        delete m_pEnumerator;
        m_pEnumerator = NULL;
        }

	if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_MED) == DEBUGLEVEL_MED)
	{
		fWidePrintString("CQuarantineServer::FinalRelease  this: 0x%lx",this );
	}

    // 
    // Call base class implementation.
    // 
    CComObjectRoot::FinalRelease();
}

///////////////////////////////////////////////////////////////////////////////
// Helper routines


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQuarantineServer::CreateQserverItem
//
// Description  : This routine will create and initialze a Quarantine server 
//                item.
//
// Return type  : HRESULT
//
// Argument     : LPCTSTR pszItemName
// Argument     : IQuarantineServerItem* pItem
//
///////////////////////////////////////////////////////////////////////////////
// 1/14/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineServer::CreateQserverItem( LPCTSTR pszItemName, 
                                                  IQuarantineServerItem** ppItem )
{
	ULONG ulItemID = 0;
    // 
    // First, create an object.
    // 
    IQuarantineServerItem* pObject;
    HRESULT hr = CoCreateInstance( __uuidof( QuarantineServerItem ),
                                   NULL,
                                   CLSCTX_ALL,
                                   __uuidof( IQuarantineServerItem ),
                                   (LPVOID*) &pObject );

    if( FAILED( hr ) )
        return hr;

    *ppItem = pObject;
    
    // 
    // Initialize the object.
    // 
    CQuarantineServerItem *pInternal =(CQuarantineServerItem*) pObject;
    TCHAR szFilePath[ MAX_PATH + 1];
    TCHAR szQuarantineDir[ MAX_PATH + 1 ];
    _Module.GetQuarantineDir( szQuarantineDir );

    wsprintf( szFilePath, _T( "%s\\%s" ), szQuarantineDir, pszItemName );
	ulItemID = _tcstoul(pszItemName,NULL,16);
    hr = pInternal->Initialize( szFilePath, ulItemID  );
    if( SUCCEEDED( hr ) )
        {
        // 
        // Add this item to the list of open items.  
        // 
        _Module.AddOpenItem( ulItemID, pInternal ); 
        }
	else
	{
		//tam 5-18-00 error state release the object
		if ((_Module.m_ulItemInterfaceTrace & DEBUGLEVEL_LOW) == DEBUGLEVEL_LOW)
		{
			fWidePrintString("CQuarantineServer::CreateQserverItem  INIT FAILED ItemName  %s, pQServerItemInterface: 0x%lx",
				pszItemName, pObject );
		}
		pInternal->Release();
	}


    return hr;
}


