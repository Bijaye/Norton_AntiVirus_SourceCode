// MultiSelDataObject.cpp: implementation of the CMultiSelDataObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MultiSelDataObject.h"
#include "qcresult.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CMultiSelDataObject::CMultiSelDataObject
//
// Description   : Constructor
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CMultiSelDataObject::CMultiSelDataObject()
{
    // 
    // Register clipboard formats
    // 
    m_CCF_MULTI_SELECT_SNAPINS = RegisterClipboardFormat( CCF_MULTI_SELECT_SNAPINS );
    m_CCF_OBJECT_TYPES_IN_MULTI_SELECT = RegisterClipboardFormat( CCF_OBJECT_TYPES_IN_MULTI_SELECT );

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CMultiSelDataObject::~CMultiSelDataObject
//
// Description   : Destructor
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CMultiSelDataObject::~CMultiSelDataObject()
{
    // 
    // Cleanup.
    // 
    m_ItemList.RemoveAll();
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CMultiSelDataObject::GetData
//
// Description   : 
//
// Return type   : 
//
// Argument      : FORMATETC *pformatetcIn
// Argument      : STGMEDIUM *pmedium
//
///////////////////////////////////////////////////////////////////////////////
// 2/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMultiSelDataObject::GetData(FORMATETC *pformatetcIn, STGMEDIUM *pmedium)
{
    HRESULT hr = DV_E_FORMATETC;
    DWORD dwAllocSize;
    LPBYTE pByte = NULL;
    POSITION pos = NULL;

    ATLTRACE2(atlTraceSnapin, 0, _T("CMultiSelDataObject::GetData\n"));
    // Make sure the type medium is HGLOBAL
	if( pmedium->tymed != TYMED_HGLOBAL )
        return DV_E_CLIPFORMAT;

    if( pformatetcIn->cfFormat == m_CCF_MULTI_SELECT_SNAPINS )
        {
        dwAllocSize = sizeof(DWORD) + ( sizeof( LPDATAOBJECT ) );
        pmedium->hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, dwAllocSize );
        if( pmedium->hGlobal == NULL )
            return E_OUTOFMEMORY;
        
        // 
        // Save off count
        // 
        pByte = (LPBYTE) GlobalLock( pmedium->hGlobal );
        *((DWORD*)pByte) = 1;

        // 
        // Get data objects for all selected items
        //
        LPDATAOBJECT pDataObject = (LPDATAOBJECT) (pByte + sizeof(DWORD));
        LPVOID p = this;
        CopyMemory( pDataObject, &p, sizeof( LPVOID ) );

        GlobalUnlock( pmedium->hGlobal );
        hr = S_OK;
        }
    else if( pformatetcIn->cfFormat == m_CCF_OBJECT_TYPES_IN_MULTI_SELECT )
        {
        dwAllocSize = sizeof(DWORD) + ( m_ItemList.GetCount() * sizeof( GUID ) );
        pmedium->hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, dwAllocSize );
        if( pmedium->hGlobal == NULL )
            return E_OUTOFMEMORY;
        
        // 
        // Save off count
        // 
        pByte = (LPBYTE) GlobalLock( pmedium->hGlobal );
        *((DWORD*)pByte) = m_ItemList.GetCount();

        // 
        // Get GUIDs for each selected object.
        // 
        POSITION pos = m_ItemList.GetHeadPosition();
        GUID* pGuid = (GUID*)(pByte + sizeof( DWORD ));
        while( pos != NULL )
            {
            CQSConsoleResultData* pData = (CQSConsoleResultData*)m_ItemList.GetNext( pos );
            CopyMemory( pGuid, pData->m_NODETYPE, sizeof( GUID ) );
            pGuid++;
            }

        GlobalUnlock( pmedium->hGlobal );
        hr = S_OK;
        }
    else if( pformatetcIn->cfFormat == CSnapInItem::m_CCF_MMC_MULTISELECT_DATAOBJECT )
        {
        pmedium->hGlobal = ::GlobalAlloc( GMEM_MOVEABLE, sizeof( DWORD ) );
        if( pmedium->hGlobal == NULL )
            return E_OUTOFMEMORY;

        LPDWORD pdw = (LPDWORD) GlobalLock( pmedium->hGlobal );
        *pdw = 1;
        GlobalUnlock( pmedium->hGlobal );
        hr = S_OK;
        }

    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CMultiSelDataObject::QueryGetData
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : FORMATETC* pformatetc
//
///////////////////////////////////////////////////////////////////////////////
// 2/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMultiSelDataObject::QueryGetData(FORMATETC* pformatetc)
{
    if( pformatetc->cfFormat == m_CCF_MULTI_SELECT_SNAPINS ||
        pformatetc->cfFormat == m_CCF_OBJECT_TYPES_IN_MULTI_SELECT ||
        pformatetc->cfFormat == CSnapInItem::m_CCF_MMC_MULTISELECT_DATAOBJECT )
        return S_OK;

    return DV_E_FORMATETC;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CMultiSelDataObject::GetDataHere
//
// Description   : Override of IDataObject::GetDataHere
//
// Return type   : STDMETHODIMP 
//
// Argument      : FORMATETC* pformatetc
// Argument      : STGMEDIUM* pmedium
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMultiSelDataObject::GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium)
{
    ATLTRACE2(atlTraceSnapin, 0, _T("CMultiSelDataObject::GetDataHere\n"));
	if (pmedium == NULL)
		return E_POINTER;

	HRESULT hr = DV_E_TYMED;
    ULONG uWritten;

	// Make sure the type medium is HGLOBAL
	if (pmedium->tymed == TYMED_HGLOBAL)
	    {
		// Create the stream on the hGlobal passed in
		CComPtr<IStream> spStream;
		hr = CreateStreamOnHGlobal(pmedium->hGlobal, FALSE, &spStream);
		if (SUCCEEDED(hr))
            {
            DWORD dwCount = m_ItemList.GetCount();  
            if( pformatetc->cfFormat == m_CCF_MULTI_SELECT_SNAPINS )
    			{
                // 
                // Save off count.
                //
                dwCount = 1;
                hr = spStream->Write( &dwCount, sizeof( DWORD), &uWritten );
                if( FAILED( hr ) )
                    return hr;

                // 
                // Get data objects for all selected items
                // 
                LPVOID p = this;
                hr = spStream->Write( &p, sizeof( LPDATAOBJECT ), &uWritten );
                if( FAILED( hr ) )
                    return hr;
	    		}
			else if( pformatetc->cfFormat == m_CCF_OBJECT_TYPES_IN_MULTI_SELECT )
                {
                // 
                // Save off count.
                // 
                hr = spStream->Write( &dwCount, sizeof( DWORD), &uWritten );
                if( FAILED( hr ) )
                    return hr;
                // 
                // Get GUIDs for each selected object.
                // 
                POSITION pos = m_ItemList.GetHeadPosition();
                while( pos != NULL )
                    {
                    CQSConsoleResultData* pData = (CQSConsoleResultData*)m_ItemList.GetNext( pos );
            
                    hr = spStream->Write( pData->m_NODETYPE, sizeof( GUID ), &uWritten );
                    if( FAILED( hr ) )
                        return hr;
                    }
                }
            else if( pformatetc->cfFormat == CSnapInItem::m_CCF_SNAPIN_GETOBJECTDATA )
                {
				hr = spStream->Write(&m_objectData, sizeof(CObjectData), &uWritten);
                }
            else if( pformatetc->cfFormat == CSnapInItem::m_CCF_NODETYPE )
                {
                hr = spStream->Write( CQSConsoleResultData::m_NODETYPE, sizeof(GUID), &uWritten );
                }
            }
	    }

    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CMultiSelDataObject::GetMultiSelectDataObject
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : LPDATAOBJECT pDataObj
// Argument      : LPDATAOBJECT* pDataObject
//
///////////////////////////////////////////////////////////////////////////////
// 2/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CMultiSelDataObject* CMultiSelDataObject::GetMultiSelectDataObject( LPDATAOBJECT pDataObject )
{
    // 
    // Check to see if this is indeed one of our multiselect data
    // objects
    //     
    FORMATETC format = { CMultiSelDataObject::m_CCF_MULTI_SELECT_SNAPINS,
                         NULL,
                         DVASPECT_CONTENT,
                         -1,
                         TYMED_HGLOBAL };
    // 
    // Check input.
    // 
    if( pDataObject == NULL )
        return NULL;

    // 
    // Make sure this is a multi-select object
    // 
    STGMEDIUM stg = { TYMED_HGLOBAL, 0 };
    HRESULT hr = pDataObject->QueryGetData( &format );
    if( FAILED( hr ) )
        return NULL;

    // 
    // Looks like it is.  Extract data.
    // 
    hr = pDataObject->GetData( &format, &stg );
    if( FAILED( hr ) )
        return NULL;

    SMMCDataObjects *pDo = (SMMCDataObjects *)::GlobalLock( stg.hGlobal );
    CMultiSelDataObject* pDataObj = (CMultiSelDataObject*)pDo->lpDataObject[0];
    ::GlobalUnlock( stg.hGlobal );
    ::GlobalFree( stg.hGlobal );
    
    return pDataObj;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CMultiSelDataObject::GetItemIDs
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : ULONG*  ulCount
// Argument     : ULONG*  aulItems
//
///////////////////////////////////////////////////////////////////////////////
// 4/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMultiSelDataObject::GetItemIDs( ULONG*  ulCount,
                                        ULONG** aulItems )
{
    // 
    // For single select items, there is only one item.
    // 
    *ulCount = m_ItemList.GetCount();
    if( *ulCount == 0 )
        return E_UNEXPECTED;

    // 
    // Allocate to hold item ids
    // 
    ULONG* pU = (ULONG*)CoTaskMemAlloc( *ulCount * sizeof( ULONG ) );
    if( pU == NULL )
        return E_OUTOFMEMORY;

    // 
    // Save off item IDs.
    // 
    int i = 0;
    POSITION pos = m_ItemList.GetHeadPosition();
    while( pos != NULL )
        {
        CQSConsoleResultData* pData = (CQSConsoleResultData*)m_ItemList.GetNext( pos );
        pU[i++] = pData->GetItemID();
        }

    // 
    // Save off pointer to data.
    // 
    *aulItems = pU;

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CMultiSelDataObject::GetQserverItem
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     :  ULONG ulItemID
// Argument     : IUnknown** pItem
//
///////////////////////////////////////////////////////////////////////////////
// 4/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMultiSelDataObject::GetQserverItem( ULONG ulItemID,
                                            IUnknown** pItem  )
{
    HRESULT hr = S_OK;
    CQSConsoleResultData* pResultItem = (CQSConsoleResultData*)m_objectData.m_pItem;
    IQuarantineServerItem *pQserverItem = NULL;

    try
        {
        // 
        // Try to get this item from the server
        // 
        pResultItem->GetRootNode()->m_pQServer->GetQuarantineItem( ulItemID, &pQserverItem );

        // 
        // Set the proxy blanket for this item
        // 
        hr = pResultItem->GetRootNode()->m_pQServer.DCO_SetProxyBlanket( pQserverItem );    
        if( FAILED( hr ) )
            {
            pQserverItem->Release();
            _com_issue_error( hr );
            }
        }
    catch( _com_error e )
        {
        hr = e.Error();
        }
    
    // 
    // Everything looks good.
    // 
    if( SUCCEEDED( hr ) )
        *pItem = pQserverItem;
    else
        *pItem = NULL;

    return hr;
}


