
#include "stdafx.h"
#include "dataobj.h"
#include "qscon.h"
#include "qsconsole.h"
#include "qsconsoledata.h"
#include "qcresult.h"


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSDataObject::GetDataHere
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : FORMATETC* pformatetc
// Argument     : STGMEDIUM* pmedium
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSDataObject::GetDataHere( FORMATETC* pformatetc, STGMEDIUM* pmedium )
{
    // 
    // Validate input
    // 
    HRESULT hr = DV_E_TYMED;
    ATLTRACE2(atlTraceSnapin, 0, _T("CQSDataObject::GetDataHere\n"));
    if (pmedium == NULL)
        return E_POINTER;
    
    // Make sure the type medium is HGLOBAL
    if (pmedium->tymed != TYMED_HGLOBAL)
        return DV_E_TYMED;

    // Create the stream on the hGlobal passed in
    CComPtr<IStream> pStream;
    hr = CreateStreamOnHGlobal(pmedium->hGlobal, FALSE, &pStream);
    if( FAILED( hr ) )
        return hr;

    // 
    // Figure out what clipboard the client wants.
    // 
    if( pformatetc->cfFormat == CQSDataObject::m_CCF_COMPUTER_NAME )
        {
        if( m_objectData.m_type != CCT_SCOPE )
            return DV_E_CLIPFORMAT;
        
        return GetMachineName( pStream );
        }
    else
        {
        return CSnapInDataObjectImpl::GetDataHere( pformatetc, pmedium );
        }

    // 
    // Should never get here.
    // 
    return DV_E_CLIPFORMAT;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSDataObject::GetMachineName
//
// Description  : This routine will retrieve the machine name (Computer Name) we
//                are currently pointing at.
//
// Return type  : HRESULT 
//
// Argument     : LPSTREAM pStream
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSDataObject::GetMachineName(LPSTREAM pStream)
{
    ULONG uWritten;

    // 
    // Fetch machine name from component data.
    // 
    CQSConsoleData* pNode = (CQSConsoleData*) m_objectData.m_pItem;
    ASSERT( pNode );
    CString s = pNode->GetServerName();
    
    return pStream->Write( s.GetBuffer(0), (s.GetLength() + 2) * sizeof( WCHAR ), &uWritten);
}






///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSResultDataObject::~CQSResultDataObject
//
// Description  : Destructor
//
///////////////////////////////////////////////////////////////////////////////
// 4/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CQSResultDataObject::~CQSResultDataObject()
{

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSResultDataObject::GetItemIDs
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
STDMETHODIMP CQSResultDataObject::GetItemIDs( ULONG*  ulCount,
                                        ULONG** aulItems )
{
    // 
    // For single select items, there is only one item.
    // 
    *ulCount = 1;

    // 
    // Allocate to hold item ids
    // 
    *aulItems = (ULONG*)CoTaskMemAlloc( sizeof( ULONG ) );
    if( *aulItems == NULL )
        return E_OUTOFMEMORY;

    // 
    // Save off item ID.
    // 
    *aulItems[0] = ((CQSConsoleResultData*) m_objectData.m_pItem)->GetItemID();

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSResultDataObject::GetQserverItem
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
STDMETHODIMP CQSResultDataObject::GetQserverItem( ULONG ulItemID,
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



