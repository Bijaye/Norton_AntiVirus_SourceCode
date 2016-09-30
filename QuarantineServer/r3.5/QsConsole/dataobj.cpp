/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


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
// Function name: CQSDataObject::GetQSVersion
//
// Description  : Gets the QS version for other extensions.
//
///////////////////////////////////////////////////////////////////////////////
// 12-21-99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////


STDMETHODIMP CQSDataObject::GetQSVersion(ULONG *ulVersion)
{
	HRESULT hr = S_OK;
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_ASSERTE(ulVersion);
	
	// check params
	if (ulVersion == NULL)
		return E_INVALIDARG;
	// Get the pointer to the node (in it is stored the qs version)
	CQSConsoleData* pNode = (CQSConsoleData*) m_objectData.m_pItem;
	
	if (pNode != NULL)
	{
		*ulVersion = pNode->m_dwQSVersion;
	}
	else
	{
		*ulVersion = 0;
		hr = E_FAIL;
	}
	
	
	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSDataObject::AvisConfigGetValue
//
// Description  : Returns the pointer to the avis server.
//
///////////////////////////////////////////////////////////////////////////////
// 12-21-99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CQSDataObject::AvisConfigGetValue(/*[in]*/ BSTR bstrFieldName, 
											   /*[in]*/ BSTR bstrKeyName,
											   /*[out]*/ VARIANT *v )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	HRESULT hr = S_OK;
	int i = 0;	
	// validate input
	if (bstrFieldName == NULL || v == NULL)
		return E_INVALIDARG;
	
	// Get the pointer to the node (in it is stored the qs version)
	CQSConsoleData* pNode = (CQSConsoleData*) m_objectData.m_pItem;
	if (pNode)
	{
		if(pNode->GetAttached()== CONNECTION_BROKEN)
		{
			hr = pNode->ReAttach();
			if (FAILED(hr))
				return E_FAIL;
		}
		while (TRUE)
		{
			try
			{
				_ASSERTE(pNode->m_cQSInterfaces);
				if (pNode->m_cQSInterfaces != NULL)
				{
					hr = pNode->m_cQSInterfaces->m_pAvisConfig->GetValue (bstrFieldName, bstrKeyName, v);
					if( FAILED( hr ) )
					{
					_com_issue_error( hr );
					}
					break;
				}
				else
				{
					hr = E_FAIL;
					_com_issue_error( hr );
				}
			}
			catch( _com_error e )
			{
				
				if (i++ >= 1)
				{
					hr = e.Error();
					break;
				}
				hr = pNode->ReAttach();
				if(FAILED(hr))
					break;
			}
		}
	}
	
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSDataObject::AvisConfigPutValue
//
// Description  : Returns the pointer to the avis server.
//
///////////////////////////////////////////////////////////////////////////////
// 12-21-99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CQSDataObject::AvisConfigSetValue(/*[in]*/ BSTR bstrFieldName, 
											   /*[in]*/ BSTR bstrKeyName,
											   /*[in]*/ VARIANT *v )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		
	HRESULT hr = S_OK;
	int i = 0;	
	// validate input
	if (bstrFieldName == NULL || v == NULL)
		return E_INVALIDARG;
	
	// Get the pointer to the node (in it is stored the qs version)
	CQSConsoleData* pNode = (CQSConsoleData*) m_objectData.m_pItem;

	if (pNode)
	{
		if(pNode->GetAttached()== CONNECTION_BROKEN)
		{
			hr = pNode->ReAttach();
			if (FAILED(hr))
				return E_FAIL;
		}
		while (TRUE)
		{
			try
			{
				_ASSERTE(pNode->m_cQSInterfaces);
				if (pNode->m_cQSInterfaces != NULL)
				{
					hr = pNode->m_cQSInterfaces->m_pAvisConfig->SetValue (bstrFieldName, bstrKeyName, v);
					if( FAILED( hr ) )
					{
					_com_issue_error( hr );
					}
					break;
				}
				else
				{
					hr = E_FAIL;
					_com_issue_error( hr );
				}
			}
			catch( _com_error e )
			{
				
				if (i++ >= 1)
				{
					hr = e.Error();
					break;
				}
				hr = pNode->ReAttach();
				if(FAILED(hr))
					break;
			}
		}
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSDataObject::GetQSDefVersion
//
// Description  : Gets the QS version for other extensions.
//
///////////////////////////////////////////////////////////////////////////////
// 9-24-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////


STDMETHODIMP CQSDataObject::GetQSDefVersion(BSTR *bstrQSVersion)
{
	HRESULT hr = S_OK;
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_ASSERTE(bstrQSVersion);
	
	// check params
	if (bstrQSVersion == NULL)
		return E_INVALIDARG;

	// Get the pointer to the node (in it is stored the qs version)
	CQSConsoleData* pNode = (CQSConsoleData*) m_objectData.m_pItem;
	
	if (pNode != NULL)
	{
		*bstrQSVersion = pNode->m_sCurrentDefVersion.AllocSysString();
	}
	else
	{
		*bstrQSVersion = 0;
		hr = E_FAIL;
	}
	
	
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSDataObject::GetQSDefDate
//
// Description  : Gets the QS version for other extensions.
//
///////////////////////////////////////////////////////////////////////////////
// 9-24-01 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////


STDMETHODIMP CQSDataObject::GetQSDefDate(BSTR *bstrQSDate)
{
	HRESULT hr = S_OK;
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		_ASSERTE(bstrQSDate);
	
	// check params
	if (bstrQSDate == NULL)
		return E_INVALIDARG;

	// Get the pointer to the node (in it is stored the qs version)
	CQSConsoleData* pNode = (CQSConsoleData*) m_objectData.m_pItem;
	
	if (pNode != NULL)
	{
		*bstrQSDate = pNode->m_sCurrentDefDate.AllocSysString();
	}
	else
	{
		*bstrQSDate = 0;
		hr = E_FAIL;
	}
	
	
	return hr;
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
												 IUnknown** pItem,  
												 ULONG *pIdentity)
{
    HRESULT hr = S_OK;
    CQSConsoleResultData* pResultItem = (CQSConsoleResultData*)m_objectData.m_pItem;
    IQuarantineServerItem *pQserverItem = NULL;
	int i=0;
	// Get the pointer to the node (in it is stored the qs version)
	CQSConsoleData* pNode = pResultItem->GetRootNode();
	if (pNode)
	{
		if(pNode->GetAttached()== CONNECTION_BROKEN)
		{
			hr = pNode->ReAttach();
			if (FAILED(hr))
				return E_FAIL;
		}
		while (TRUE)
		{
			try
			{
				_ASSERTE(pNode->m_cQSInterfaces);
				if (pNode->m_cQSInterfaces != NULL)
				{
					// 
					// Try to get this item from the server
					// 
					pNode->m_cQSInterfaces->m_pQServer->GetQuarantineItem( ulItemID, &pQserverItem );
					if( FAILED( hr ) )
					{
						_com_issue_error( hr );
					}
					
					// 
					// Set the proxy blanket for this item
					// 
					hr = pNode->m_cQSInterfaces->m_pQServer.DCO_SetProxyBlanket( pQserverItem );    
					if( FAILED( hr ) )
					{
						pQserverItem->Release();
						_com_issue_error( hr );
					}
					break;

				}
				else
				{
					hr = E_FAIL;
					_com_issue_error( hr );
				}
			}
			catch( _com_error e )
			{
				
				if (i++ >= 1)
				{
					hr = e.Error();
					break;
				}
				hr = pNode->ReAttach();
				if(FAILED(hr))
					break;
			}
		}
	}
    
    // 
    // Everything looks good.
    // 
    if( SUCCEEDED( hr ) )
	{
        *pItem = pQserverItem;
		if (pIdentity != NULL)
			*pIdentity = (ULONG)(LPVOID)pNode->m_cQSInterfaces->m_pQServer.DCO_GetIdentity();
	}
    else
        *pItem = NULL;
	
    return hr;
}




