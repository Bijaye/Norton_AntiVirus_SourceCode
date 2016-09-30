/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#include "stdafx.h"
#include "QSConsoleComponent.h"
#include "MultiSelDataObject.h"
#include "qcresult.h"
#include "qscommon.h"

DWORD CQSConsoleComponent::m_dwNoOpenWindows = 0;
///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleComponent::QueryDataObject
//
// Description   : Handles multi-select chores.
//
// Return type   : STDMETHODIMP 
//
// Argument      : long cookie
// Argument      : DATA_OBJECT_TYPES type
// Argument      : LPDATAOBJECT  *ppDataObject
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleComponent::QueryDataObject(long cookie,
												  DATA_OBJECT_TYPES type,
												  LPDATAOBJECT  *ppDataObject)
{
    // 
    // If not multiselect, call base class
    // 
    if (cookie != MMC_MULTI_SELECT_COOKIE)
	{
        return IComponentImpl<CQSConsoleComponent>::QueryDataObject( cookie, type, ppDataObject );
	}
	
    HRESULT hr = S_OK;
	
    // 
    // Create multiselect data object
    // 
    CComObject<CMultiSelDataObject> *pObj;
    hr = CComObject<CMultiSelDataObject >::CreateInstance(&pObj);
    if( FAILED( hr ) )
        return hr;
	
    hr = pObj->QueryInterface( IID_IDataObject, (LPVOID*) ppDataObject );
    if( FAILED( hr ) )
        return hr;
	
    // 
    // Get pointer to result data. 
    // 
    CComPtr< IConsole > pc = m_spConsole;
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
    RESULTDATAITEM rdi;
    ZeroMemory( &rdi, sizeof( RESULTDATAITEM ) );
	
    // 
    // Need to enumerate items in results pane
    // 
    rdi.mask = RDI_STATE | RDI_PARAM;
    rdi.nIndex = -1;
    rdi.nState = LVIS_SELECTED;
    while( S_OK == spResultData->GetNextItem( &rdi ) )
	{
        // 
        // Save off pointer to item.
        // 
        pObj->m_ItemList.AddTail( (CSnapInItem*) rdi.lParam );
	}
	
    // 
    // Save off pointer to ourselves.
    //
	
    pObj->m_objectData.m_type = CCT_RESULT;
    pObj->m_objectData.m_pItem = pObj->m_ItemList.GetHead();
    pObj->m_pComponent = m_pComponentData;
	
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleComponent::CompareObjects
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     :  LPDATAOBJECT lpDataObjectA
// Argument     : LPDATAOBJECT lpDataObjectB
//
///////////////////////////////////////////////////////////////////////////////
// 3/17/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleComponent::CompareObjects( LPDATAOBJECT lpDataObjectA,
												 LPDATAOBJECT lpDataObjectB)
{
    // 
    // Get snapin items for each data object
    // 
    CSnapInItem* pA;
    CSnapInItem* pB;
    DATA_OBJECT_TYPES dtA, dtB;
    if( FAILED( CSnapInItem::GetDataClass( lpDataObjectA, &pA, &dtA ) ) ||
        FAILED( CSnapInItem::GetDataClass( lpDataObjectB, &pB, &dtB ) ) )
	{
        return S_FALSE;
	}
	
    // 
    // We only really care about result pane items.
    // 
    if( dtA != CCT_RESULT || dtB != CCT_RESULT )
	{
        return S_FALSE;
	}
	
    // 
    // Compare the item IDs for each item.
    // 
    CQSConsoleResultData* pDataA = (CQSConsoleResultData*) pA;
    CQSConsoleResultData* pDataB = (CQSConsoleResultData*) pB;
	
    if( pDataA->GetItemID() == pDataB->GetItemID() )
        return S_OK;
	
    return S_FALSE;
	
}

/*----------------------------------------------------------------------------
GetCurrentSelectedQserverVersion

  QsConsole writes the version of the currently selected
  Qserver here just before my ScopeExtData or AvisConExtData 
  objects are created.
  
	These registry entries are stored on the local console machine.
	
	  Written by: Jim Hill      1/3/00
----------------------------------------------------------------------------*/
DWORD GetCurrentSelectedQserverVersion( void)
{
    DWORD   dwQserverVersion = 1;
	CRegKey reg;
	LONG    lResult;
	
	
    //  GO RETRIEVE IT FROM LOCAL REGISTRY ENTRY
	lResult = reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS == lResult )
	{
		lResult = reg.QueryDWORDValue(REGVALUE_SELECTED_QSERVER_VERSION, dwQserverVersion);
		if(ERROR_SUCCESS != lResult )
		{
            dwQserverVersion = 1;
		}
		reg.Close();
	}
	
    return(dwQserverVersion);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleComponent::Compare
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     :  long lUserParam
// Argument     : long cookieA
// Argument     : long cookieB
// Argument     : int * pnResult
//
///////////////////////////////////////////////////////////////////////////////
// 3/17/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleComponent::Compare( long lUserParam,
										  long cookieA,
										  long cookieB,
										  int * pnResult )
{
	//
	// qserver version so we know which columns to compare
	//
	DWORD dwQSVersion = GetCurrentSelectedQserverVersion();
	
    // 
    // Get items to compare
    // 
    CQSConsoleResultData* pA = (CQSConsoleResultData*) cookieA;
    CQSConsoleResultData* pB = (CQSConsoleResultData*) cookieB;
    FILETIME ftA, ftB;
    ASSERT( pA );
    ASSERT( pB );
	
    if (dwQSVersion >=QSVERISON)
	{
		// 
		// Q: What column are we sorting?
		// 
		switch( *pnResult )
		{
		case RESULT_COL_FILENAME:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sFileName, pB->GetItemData()->m_sFileName);
			break;
		case RESULT_COL_USERNAME: 
			*pnResult = _tcsicmp( pA->GetItemData()->m_sUserName, pB->GetItemData()->m_sUserName);
			break;
		case RESULT_COL_MACHINENAME:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sMachineName, pB->GetItemData()->m_sMachineName);
			break;
		case RESULT_COL_COMPLEATE:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sComplete, pB->GetItemData()->m_sComplete);
			break;
		case RESULT_COL_AGE:
			// bug fix - need to compare actual age vs, string to sort correctly
//			*pnResult = _tcsicmp( pA->GetItemData()->m_sAge, pB->GetItemData()->m_sAge);
			*pnResult = (int)( pA->GetItemData()->m_cstAge.GetTotalSeconds() - pB->GetItemData()->m_cstAge.GetTotalSeconds() );
			break;
		case RESULT_COL_STATE:
			{
				USES_CONVERSION;
				CString sa = LookUpIcePackTokenString(T2W(pA->GetItemData()->m_sState.GetBuffer(0)),ICEPACK_TOKEN_STATE_TABLE);
				CString sb = LookUpIcePackTokenString(T2W(pB->GetItemData()->m_sState.GetBuffer(0)),ICEPACK_TOKEN_STATE_TABLE);
				if (sa.IsEmpty() || sb.IsEmpty())
				{
					*pnResult = _tcsicmp( pA->GetItemData()->m_sState.GetBuffer(0),pB->GetItemData()->m_sState.GetBuffer(0));
				}
				else
				{
					*pnResult = _tcsicmp( sa,sb);
				}
			}
			break;
		case RESULT_COL_STATUS:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sStatusText, pB->GetItemData()->m_sStatusText);
			break;
		case RESULT_COL_SEQ_NEEDED:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sDefSeqNeeded, pB->GetItemData()->m_sDefSeqNeeded);
			break;
		case RESULT_COL_VIRUS_NAME:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sVirusName, pB->GetItemData()->m_sVirusName);
			break;
		case RESULT_COL_ERROR:
			{
				USES_CONVERSION;

				CString sa = LookUpIcePackTokenString(T2W(pA->GetItemData()->m_sError.GetBuffer(0)),ICEPACK_TOKEN_ERROR_TABLE);
				CString sb = LookUpIcePackTokenString(T2W(pB->GetItemData()->m_sError.GetBuffer(0)),ICEPACK_TOKEN_ERROR_TABLE);
				if (sa.IsEmpty() || sb.IsEmpty())
				{
					*pnResult = _tcsicmp( pA->GetItemData()->m_sError.GetBuffer(0),pB->GetItemData()->m_sError.GetBuffer(0));
				}
				else
				{
					*pnResult = _tcsicmp( sa,sb);
				}
			}
		}
	}
	else
	{
		
		// 
		// Q: What column are we sorting?
		// 
		switch( *pnResult )
		{
		case RESULT_COL_FILENAME:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sFileName, pB->GetItemData()->m_sFileName);
			break;
		case RESULT_COL_USERNAME: 
			*pnResult = _tcsicmp( pA->GetItemData()->m_sUserName, pB->GetItemData()->m_sUserName);
			break;
		case RESULT_COL_MACHINENAME:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sMachineName, pB->GetItemData()->m_sMachineName);
			break;
		case RESULT_COL_DOMAINNAME:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sDomainName, pB->GetItemData()->m_sDomainName);
			break;
		case RESULT_COL_RECIEVED:
			SystemTimeToFileTime( &pA->GetItemData()->m_stRecieved, &ftA );
			SystemTimeToFileTime( &pB->GetItemData()->m_stRecieved, &ftB );
			*pnResult = CompareFileTime( &ftA, &ftB );
			break;
		case RESULT_COL_SUBMITTED: 
			SystemTimeToFileTime( &pA->GetItemData()->m_stSubmitted, &ftA );
			SystemTimeToFileTime( &pB->GetItemData()->m_stSubmitted, &ftB );
			*pnResult = CompareFileTime( &ftA, &ftB );
			break;
		case RESULT_COL_ERROR:
			if( pA->GetItemData()->GetError() <
				pB->GetItemData()->GetError() )
				*pnResult = -1;
			else if( pA->GetItemData()->GetError() >
				pB->GetItemData()->GetError() )
				*pnResult = 1;
			else 
				*pnResult = 0;
			break;
		case RESULT_COL_SUBMIT_BY:
			if( pA->GetItemData()->m_dwSubmittedBy <
				pB->GetItemData()->m_dwSubmittedBy )
				*pnResult = -1;
			else if( pA->GetItemData()->m_dwSubmittedBy >
				pB->GetItemData()->m_dwSubmittedBy )
				*pnResult = 1;
			else 
				*pnResult = 0;
			break;
			
		case RESULT_COL_STATUS:
			if( pA->GetItemData()->m_dwStatus <
				pB->GetItemData()->m_dwStatus )
				*pnResult = -1;
			else if( pA->GetItemData()->m_dwStatus >
				pB->GetItemData()->m_dwStatus )
				*pnResult = 1;
			else 
				*pnResult = 0;
			break;
			
		case RESULT_COL_VIRUS_NAME:
			*pnResult = _tcsicmp( pA->GetItemData()->m_sVirusName, pB->GetItemData()->m_sVirusName);
			break;
		}
	}
    return S_OK;    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleComponent::AddMenuItems
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : LPDATAOBJECT pDataObject
// Argument      : LPCONTEXTMENUCALLBACK piCallback
// Argument      : long *pInsertionAllowed
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleComponent::AddMenuItems(LPDATAOBJECT pDataObject,
											   LPCONTEXTMENUCALLBACK piCallback,
											   long *pInsertionAllowed)
{
    HRESULT hr = E_POINTER;	
    
    // 
    // Q: Are we multiSelect?
    // 
    CMultiSelDataObject* pData = CMultiSelDataObject::GetMultiSelectDataObject( pDataObject );
    if( pData == NULL )
	{
		
        return IExtendContextMenuImpl< CQSConsoleComponent >::AddMenuItems( pDataObject, piCallback, pInsertionAllowed );
	}
	
    // 
    // Add menu items.
    // 
    CSnapInItem* pItem = pData->m_ItemList.GetHead();
	//terrym 3/2/00 Changed to null to solve the fact multiselect was not calling the snap in
	hr = ((CQSConsoleResultData*)pItem)->MultiSelAddMenuItems(NULL/*pDataObject*/, piCallback, pInsertionAllowed );
	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleComponent::ControlbarNotify
//
// Description   : 
//
// Return type   : STDMETHODimp 
//
// Argument      : MMC_NOTIFY_TYPE event
// Argument      : long arg
// Argument      : long param
//
///////////////////////////////////////////////////////////////////////////////
// 3/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleComponent::ControlbarNotify(MMC_NOTIFY_TYPE event,
												   long arg,
												   long param)
{
    CMultiSelDataObject* pData;
    CSnapInItem* pItem;
	
    // 
    // Q: Are we multiSelect?
    //
    if( event == MMCN_SELECT ) 
	{ 
        pData = CMultiSelDataObject::GetMultiSelectDataObject( (LPDATAOBJECT) param );
        
        if( pData == NULL )
		{
            return IExtendControlbarImpl< CQSConsoleComponent >::ControlbarNotify( event, arg, param );
		}
        
        // 
        // Process notification
        // 
        pItem = pData->m_ItemList.GetHead();
        return pItem->ControlbarNotify(m_spControlbar, this, &(m_toolbarMap), event, arg, param, (CSnapInObjectRootBase*) this, CCT_RESULT);
	} 
    else if( event == MMCN_BTN_CLICK ) 
	{ 
        pData = CMultiSelDataObject::GetMultiSelectDataObject( (LPDATAOBJECT) arg );
		
        if( pData == NULL )
		{
            return IExtendControlbarImpl< CQSConsoleComponent >::ControlbarNotify( event, arg, param );
		}
		
        // 
        // Process notification
        // 
        pItem = pData->m_ItemList.GetHead();
        return ((CQSConsoleResultData*)pItem)->MultiSelCommand( param, pData );
	}
	
    return S_OK;
	
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleComponent::Notify
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : LPDATAOBJECT lpDataObject
// Argument      : MMC_NOTIFY_TYPE event
// Argument      : long arg
// Argument      : long param
//
///////////////////////////////////////////////////////////////////////////////
// 2/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleComponent::Notify(LPDATAOBJECT lpDataObject, 
                                         MMC_NOTIFY_TYPE event, long arg, long param)
{
    // 
    // Q: Is this a help request?
    // 
    if( event == MMCN_CONTEXTHELP )
	{
        return DoHelp( lpDataObject );
	}
	
    // 
    // Check for column click.   Sort items.
    // 
    if( event == MMCN_COLUMN_CLICK )
	{
        // 
        // Get pointer to result data. 
        // 
        CComQIPtr<IResultData, &IID_IResultData> spResultData(m_spConsole);
        return spResultData->Sort( arg, param, NULL );
	}
	
	
	if( event == MMCN_PROPERTY_CHANGE )
	{
        ((CQSConsoleResultData*)param)->DeletePropPages();    
        return S_OK;
	}
	
	if( event == MMCN_COLUMNS_CHANGED)
	{
		SetDirty();
		return S_OK;
	}
    // 
    // Q: Are we multiSelect?
    // 
    CMultiSelDataObject* pData = CMultiSelDataObject::GetMultiSelectDataObject( lpDataObject );
    if( pData == NULL )
	{
        return IComponentImpl<CQSConsoleComponent>::Notify(lpDataObject, event, arg, param);
	}
	
    // 
    // Pass notification to item's multiselect handler.
    // 
    CSnapInItem* pItem = pData->m_ItemList.GetHead();
	return ((CQSConsoleResultData*)pItem)->MultiSelNotify( event, arg, param, NULL, this, CCT_RESULT, pData );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleComponent::Command
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : long lCommandID
// Argument      : LPDATAOBJECT pDataObject
//
///////////////////////////////////////////////////////////////////////////////
// 3/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleComponent::Command( long lCommandID,
										  LPDATAOBJECT pDataObject)
{
    // 
    // Q: Are we multiSelect?
    // 
    CMultiSelDataObject* pData = CMultiSelDataObject::GetMultiSelectDataObject( pDataObject );
    if( pData == NULL )
	{
        return IExtendContextMenuImpl<CQSConsoleComponent>::Command( lCommandID, pDataObject );
	}
	
    // 
    // Pass notification to item's multiselect handler.
    // 
    CSnapInItem* pItem = pData->m_ItemList.GetHead();
    return ((CQSConsoleResultData*)pItem)->MultiSelCommand( lCommandID, pDataObject );
	
}




///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleComponent::DoHelp
//
// Description   : Fire off help for snapin.
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleComponent::DoHelp( LPDATAOBJECT lpDataObject )
{
    HRESULT hr;
    DWORD dwBytesRead;
    GUID nodeID;
	CSnapInItem* pItem;
	DATA_OBJECT_TYPES type;
    LPOLESTR pszTopic = NULL;
    CComPtr< IStream > pStream;
    CComQIPtr< IDisplayHelp > pDisplayHelp;
    STGMEDIUM stg = { TYMED_HGLOBAL, 0 };
    FORMATETC format = { CSnapInItem::m_CCF_NODETYPE,
		NULL,
		DVASPECT_CONTENT,
		-1,
		TYMED_HGLOBAL };
    // 
    // Get the snapin item from the data object.
    // 
    hr = m_pComponentData->GetDataClass(lpDataObject, &pItem, &type);
    if( FAILED( hr ) )
        return hr;
	
    // 
    // Figure out what sort of object the user is requesting help on.
    // 
    stg.hGlobal = GlobalAlloc( GHND, sizeof( GUID ) );
    if( stg.hGlobal == NULL )
        return E_OUTOFMEMORY;
    
    hr = lpDataObject->GetDataHere( &format, &stg );
    if( SUCCEEDED( hr ) )
	{
        hr = CreateStreamOnHGlobal( stg.hGlobal, FALSE, &pStream );
        if( SUCCEEDED( hr ) )
		{
            hr = pStream->Read( &nodeID, sizeof( GUID ), &dwBytesRead );
            if( SUCCEEDED( hr ) )
			{
                if( IsEqualGUID( nodeID, *CQSConsoleData::m_NODETYPE ) )
				{
                    pDisplayHelp = ((CQSConsoleData*)pItem)->m_pConsole;
                    pszTopic = L"AvisConsole.chm::/Quar_OverView.htm";
				}
                else if( IsEqualGUID( nodeID, *CQSConsoleResultData::m_NODETYPE ) )
				{
                    pDisplayHelp = ((CQSConsoleResultData*)pItem)->GetRootNode()->m_pConsole;
                    pszTopic = L"AvisConsole.chm::/Quar_Item.htm";
				}
			}
		}
	}
	
    // 
    // Cleanup
    // 
    GlobalFree( stg.hGlobal );
	
    // 
    // Display help topic.
    // 
    if( pDisplayHelp && pszTopic )
	{
		LPOLESTR p = (LPOLESTR) CoTaskMemAlloc( (wcslen( pszTopic ) + 1 ) * sizeof( WCHAR ) );
		if( p == NULL )
			return E_OUTOFMEMORY;
		wcscpy( p, pszTopic );
        hr = pDisplayHelp->ShowTopic( p );
	}
    else
        hr = E_FAIL;
	
	
    return hr;
}

STDMETHODIMP CQSConsoleComponent::Destroy(long cookie)
{
    m_dwNoOpenWindows--;
	return IComponentImpl<CQSConsoleComponent>::Destroy( cookie);
}
STDMETHODIMP CQSConsoleComponent::Initialize(LPCONSOLE lpConsole)
{
	m_dwNoOpenWindows++;
    return IComponentImpl<CQSConsoleComponent>::Initialize( lpConsole);
}
