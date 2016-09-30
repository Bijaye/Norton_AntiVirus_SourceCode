/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#include "stdafx.h"
#include "qscon.h"
#include <atlsnap.h>
#include "QSConsoleData.h"
#include "QSConsole.h"
#include "ServerWizard.h"
#include "serverpage.h"
#include "qcresult.h"
#include "dataobj.h"
#include "serverdlg.h"
#include "qscommon.h"
#include "managewnd.h"
#include "qsregval.h"
#include "qsregval.h"
#include "samplereadthread.h"
#include "VirusListView.h"
// 
// Initialize our node type information here.
// 
static const GUID CQSConsoleGUID_NODETYPE = 
{ 0x26140c87, 0xb554, 0x11d2, { 0x8f, 0x45, 0x30, 0x78, 0x30, 0x2c, 0x20, 0x30 } };
const GUID*  CQSConsoleData::m_NODETYPE = &CQSConsoleGUID_NODETYPE;
const TCHAR* CQSConsoleData::m_SZNODETYPE = _T("26140C87-B554-11D2-8F45-3078302C2030");
const TCHAR* CQSConsoleData::m_SZDISPLAY_NAME = _T("Symantec Central Quarantine");
const CLSID* CQSConsoleData::m_SNAPIN_CLASSID = &CLSID_QSConsole;
extern HRESULT ViewAMSLog( LPSTR szAppName, LPSTR szHostName, LPSTR szCaption );
extern HRESULT ConfigureAMS ( LPSTR szAppName, LPSTR szHostName, LPSTR szCaption);

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::CreatePropertyPages
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : LPPROPERTYSHEETCALLBACK lpProvider
// Argument      : long handle
// Argument      : IUnknown* pUnk                    
// Argument      : DATA_OBJECT_TYPES type
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::CreatePropertyPages( LPPROPERTYSHEETCALLBACK lpProvider,
											long handle,
											IUnknown* pUnk,
											DATA_OBJECT_TYPES type )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	switch(type)
	{
	case CCT_SCOPE:
		return AddServerPropPage( lpProvider, handle );
	case CCT_SNAPIN_MANAGER:
		return AddServerWizard( lpProvider, handle, pUnk );
	}
	return E_UNEXPECTED;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::AddServerWizard
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     :  LPPROPERTYSHEETCALLBACK lpProvider
// Argument     : long handle
//
///////////////////////////////////////////////////////////////////////////////
// 3/18/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::AddServerWizard( LPPROPERTYSHEETCALLBACK lpProvider, long handle, IUnknown* pUnk )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
    // 
    // Create the new page
    // 
    CServerWizard* pPage = new CServerWizard;
    if( pPage == NULL )
        return E_OUTOFMEMORY;
    
    pPage->m_lNotifyHandle = handle;
    pPage->m_pRootNode = this;
	
    MMCPropPageCallback( &pPage->m_psp );
    HPROPSHEETPAGE hp = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &pPage->m_psp );
    return lpProvider->AddPage( hp );
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::AddServerPropPage
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : LPPROPERTYSHEETCALLBACK lpProvider
// Argument      : long handle
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::AddServerPropPage( LPPROPERTYSHEETCALLBACK lpProvider, long handle )
{
    HRESULT hr;
	int i = 0;
	USES_CONVERSION;

	if (!m_bAttached)
	{
        CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
        CString sText((LPCTSTR) IDS_PROPERTY_PAGE_NO_QS_DEFINED);
		int iRet;

        m_pConsole->MessageBox( T2W(sText.GetBuffer(0)), T2W(sTitle.GetBuffer(0)), MB_ICONSTOP | MB_OK, &iRet ); 
		
		return E_FAIL;
	}
	if(GetAttached()== CONNECTION_BROKEN)
	{
		hr = ReAttach();
		if (FAILED(hr))
			return E_FAIL;
	}

	UpdateCaption(m_pConsole);
    while (TRUE)
	{
		// 
		// Create the new page
		// 
		CServerPage* pPage = new CServerPage;
		if( pPage == NULL )
			return E_OUTOFMEMORY;
		try
		{
			// get SAV info			
			if (SUCCEEDED(GetSAVInfo()))
			{
				if (m_cSAVInfo != NULL)
				{
					delete m_cSAVInfo;
					m_cSAVInfo = NULL;
				}
			}
			// 
			// Save off notification handle.
			// 
			pPage->m_lNotifyHandle = handle;
			pPage->m_pSnapin = this;
			
			_ASSERTE(m_cQSInterfaces);
			if (m_cQSInterfaces != NULL)
			{
				// 
				// Get the configuration interface from the server.
				// 
				CComPtr< IQserverConfig > pConfig;
				hr = m_cQSInterfaces->m_pQServer->Config( &pConfig );
				if( S_FALSE != hr )
				{
					// 
					// Need to set proxy blanket for this interface.
					// 
					hr = m_cQSInterfaces->m_pQServer.DCO_SetProxyBlanket( pConfig );
					if( FAILED( hr ) )
					{
						_com_issue_error( hr );
					}
					
					// 
					// Need to marshal this interface over to the property page, since
					// it runs in a seperate thread.
					// 
					hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IQserverConfig ), pConfig, &pPage->m_pServerStream );
					if( FAILED( hr ) )
						_com_issue_error( hr );
					
					// 
					// Add to MMC property sheet.
					// 
					MMCPropPageCallback( &pPage->m_psp );
					pPage->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
					HPROPSHEETPAGE hp = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &pPage->m_psp );
					hr = lpProvider->AddPage( hp );
					m_bPropPage = TRUE;

				}
				else
				{
					// 
					// Need to stub out page.
					// 
					CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
					CString sText((LPCTSTR) IDS_ERROR_MULTI_CONFIG );
					
					// 
					// Tell user that the item in question is in use.
					// 
					HWND hWnd;
					GetComponentData()->m_spConsole->GetMainWindow( &hWnd );
					MessageBox( hWnd, sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
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
			
			delete pPage;
			if (i++ >= 1)
			{
				hr = e.Error();
				break;
			}
			hr = ReAttach();
			if(FAILED(hr))
				break;
		}
		catch (...)
		{
			return E_FAIL;
		}
		
	}
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::AddMenuItems
//
// Description   : 
//
// Return type   : STDMETHODIMP 
//
// Argument      : LPCONTEXTMENUCALLBACK piCallback
// Argument      : long *pInsertionAllowed
// Argument      : DATA_OBJECT_TYPES type
//
///////////////////////////////////////////////////////////////////////////////
// 3/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleData::AddMenuItems( LPCONTEXTMENUCALLBACK piCallback,
										  long *pInsertionAllowed,
										  DATA_OBJECT_TYPES type )
{
    // 
    // Perform normal operation if property page is displayed.
    // 
//    if( IsPropPageDisplayed() == FALSE && GetComponentData()->IsAttachExplicit() )
//        return CSnapInItemImpl<CQSConsoleData>::AddMenuItems( piCallback, pInsertionAllowed, type );
    
//    if( IsPropPageDisplayed() == FALSE && !GetComponentData()->IsExtension() )
//        return CSnapInItemImpl<CQSConsoleData>::AddMenuItems( piCallback, pInsertionAllowed, type );
	
    // 
    // Need to add menu items, however I also need to prevent users from changing
    // connected servers while the server property page is up.
    // 
	ATLTRACE2(atlTraceSnapin, 0, _T("CQSConsoleData::AddMenuItems\n"));
	
	SetMenuInsertionFlags(true, pInsertionAllowed);
	
	UINT menuID = GetMenuID();
	if (menuID == 0)
		return S_OK;
	
	HMENU hMenu = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(menuID));
	long insertionID;
	if (hMenu)
	{
		for (int i = 0; 1; i++)
		{
			HMENU hSubMenu = GetSubMenu(hMenu, i);
			if (hSubMenu == NULL)
				break;
			
			MENUITEMINFO menuItemInfo;
			memset(&menuItemInfo, 0, sizeof(menuItemInfo));
			menuItemInfo.cbSize = sizeof(menuItemInfo);
			
			switch (i)
			{
			case 0:
				if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_TOP) )
					continue;
				insertionID = CCM_INSERTIONPOINTID_PRIMARY_TOP;
				break;
				
			case 1:
				if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_NEW) )
					continue;
				insertionID = CCM_INSERTIONPOINTID_PRIMARY_NEW;
				break;
				
			case 2:;
				if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_TASK) )
					continue;
				insertionID = CCM_INSERTIONPOINTID_PRIMARY_TASK;
				break;
			case 3:;
				if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_VIEW) )
					continue;
				insertionID = CCM_INSERTIONPOINTID_PRIMARY_VIEW;
				break;
			default:
				{
					insertionID = 0;
					continue;
				}
				break;
			}
			
			menuItemInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
			menuItemInfo.fType = MFT_STRING;
			TCHAR szMenuText[128];
			
			for (int j = 0; 1; j++)
			{
				menuItemInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
				menuItemInfo.fType = MFT_STRING;
				menuItemInfo.cch = 128;
				menuItemInfo.dwTypeData = szMenuText;
				TCHAR szStatusBar[256];
				
				if (!GetMenuItemInfo(hSubMenu, j, TRUE, &menuItemInfo))
					break;
				if (menuItemInfo.fType != MFT_STRING)
					continue;
				
                // 
                // Do not show Attach menu item
                // 
//                if( menuItemInfo.wID == ID_TOP_ATTACHTOSERVER )
//                    continue;
				
				if(((m_dwQSVersion < QSVERSION2X) && // terrym 9-21-01 version 3.0 support
					(menuItemInfo.wID == IDM_VIEW_AMS_LOG ||
					menuItemInfo.wID == IDM_CONFIGURE_AMS ||
					menuItemInfo.wID == IDM_DISPLAY_VIRUS_LIST))||
					((m_dwQSVersion < QSVERISON)&&(menuItemInfo.wID == IDM_DISPLAY_VIRUS_LIST)))
					continue;

				UpdateMenuState(menuItemInfo.wID, szMenuText, &menuItemInfo.fState);
				LoadString(_Module.GetResourceInstance(), menuItemInfo.wID, szStatusBar, 256);
				
				OLECHAR wszStatusBar[256];
				OLECHAR wszMenuText[128];
				USES_CONVERSION;
				ocscpy(wszMenuText, T2OLE(szMenuText));
				ocscpy(wszStatusBar, T2OLE(szStatusBar));
				
				CONTEXTMENUITEM contextMenuItem;
				contextMenuItem.strName = wszMenuText;
				contextMenuItem.strStatusBarText = wszStatusBar;
				contextMenuItem.lCommandID = menuItemInfo.wID;
				contextMenuItem.lInsertionPointID = insertionID;
				contextMenuItem.fFlags = menuItemInfo.fState;
				contextMenuItem.fSpecialFlags = 0;
				
				HRESULT hr = piCallback->AddItem(&contextMenuItem);
				ATLASSERT(SUCCEEDED(hr));
			}
		}
		DestroyMenu(hMenu);
	}
	
    SetMenuInsertionFlags(true, pInsertionAllowed);
	
	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::ControlbarNotify
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : IControlbar *pControlbar
// Argument     : IExtendControlbar *pExtendControlbar
// Argument     : CSimpleMap<UINT
// Argument     : IUnknown*>* pToolbarMap
// Argument     : MMC_NOTIFY_TYPE event
// Argument     : long arg
// Argument     : long param
// Argument     : CSnapInObjectRootBase* pObj
// Argument     : DATA_OBJECT_TYPES type
//
///////////////////////////////////////////////////////////////////////////////
// 3/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleData::ControlbarNotify(IControlbar *pControlbar,
											  IExtendControlbar *pExtendControlbar,
											  CSimpleMap<UINT, IUnknown*>* pToolbarMap,
											  MMC_NOTIFY_TYPE event,
											  long arg, 
											  long param,
											  CSnapInObjectRootBase* pObj,
											  DATA_OBJECT_TYPES type)
{
    // 
    // If we are an extension, then we don't want to show the 
    // Attach toolbar button.  Since this is the only button being displayed,
    // do not show the toobar at all.  Explicitly attached extensions are exempt.
    //
    if( GetComponentData()->IsExtension() && !GetComponentData()->IsAttachExplicit() )
        return S_OK;
	
    // 
    // Call base class implememtation.
    //     
    return CSnapInItemImpl< CQSConsoleData >::ControlbarNotify(pControlbar,
		pExtendControlbar,
		pToolbarMap,
		event,
		arg, 
		param,
		pObj,
		type );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::GetScopePaneInfo
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : SCOPEDATAITEM *pScopeDataItem
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::GetScopePaneInfo(SCOPEDATAITEM *pScopeDataItem)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;

	if (pScopeDataItem->mask & SDI_STR)
	{
        if (GetComponentData()->IsAttachExplicit() && !m_sDisplayName.IsEmpty())
		{
            m_bstrDisplayName = m_sDisplayName;
		}
		else
		{
            CString s((LPCTSTR) IDS_NODE_NAME );
            m_bstrDisplayName = T2OLE(s.GetBuffer(0));
		}
		pScopeDataItem->displayname = m_bstrDisplayName;
	}
	if (pScopeDataItem->mask & SDI_IMAGE)
		pScopeDataItem->nImage = m_scopeDataItem.nImage;
	if (pScopeDataItem->mask & SDI_OPENIMAGE)
		pScopeDataItem->nOpenImage = m_scopeDataItem.nOpenImage;
	if (pScopeDataItem->mask & SDI_PARAM)
		pScopeDataItem->lParam = m_scopeDataItem.lParam;
	if (pScopeDataItem->mask & SDI_STATE )
		pScopeDataItem->nState = m_scopeDataItem.nState;
	
	// TODO : Add code for SDI_CHILDREN 
	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::GetResultPaneInfo
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : RESULTDATAITEM *pResultDataItem
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::GetResultPaneInfo(RESULTDATAITEM *pResultDataItem)
{
	
    if (pResultDataItem->bScopeItem)
	{
		if (pResultDataItem->mask & RDI_STR)
		{
			pResultDataItem->str = GetResultPaneColInfo(pResultDataItem->nCol);
		}
		if (pResultDataItem->mask & RDI_IMAGE)
		{
			pResultDataItem->nImage = m_scopeDataItem.nImage;
		}
		if (pResultDataItem->mask & RDI_PARAM)
		{
			pResultDataItem->lParam = m_scopeDataItem.lParam;
		}
		
		return S_OK;
	}
	
	if (pResultDataItem->mask & RDI_STR)
	{
		
    }
	if (pResultDataItem->mask & RDI_IMAGE)
	{
		pResultDataItem->nImage = m_resultDataItem.nImage;
	}
	if (pResultDataItem->mask & RDI_PARAM)
	{
		pResultDataItem->lParam = m_resultDataItem.lParam;
	}
	if (pResultDataItem->mask & RDI_INDEX)
	{
		pResultDataItem->nIndex = m_resultDataItem.nIndex;
	}
	
	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::Notify
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : MMC_NOTIFY_TYPE event
// Argument      : long arg
// Argument      : long param                    
// Argument      : IComponentData* pComponentData
// Argument      : IComponent* pComponent        
// Argument      : DATA_OBJECT_TYPES type        
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::Notify( MMC_NOTIFY_TYPE event,
							   long arg,
							   long param,
							   IComponentData* pComponentData,
							   IComponent* pComponent,
							   DATA_OBJECT_TYPES type)
{
    // 
    // Need to do this since this routine could call subroutines which
    // use MFC.
    // 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	
	// Add code to handle the different notifications.
	// Handle MMCN_SHOW and MMCN_EXPAND to enumerate children items.
	// In response to MMCN_SHOW you have to enumerate both the scope
	// and result pane items.
	// For MMCN_EXPAND you only need to enumerate the scope items
	// Use IConsoleNameSpace::InsertItem to insert scope pane items
	// Use IResultData::InsertItem to insert result pane item.
	HRESULT hr = E_NOTIMPL;
	
	
	_ASSERTE(pComponentData != NULL || pComponent != NULL);
	
	CComQIPtr<IConsole2> spConsole;
	CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> spHeader;
	if (pComponentData != NULL)
	{
		spConsole = ((CQSConsole*)pComponentData)->m_spConsole;
	}
	else
	{
		spConsole = ((CQSConsoleComponent*)pComponent)->m_spConsole;
		spHeader = spConsole;
        
        // 
        // Save off pointer to console.
        // 
        if( m_pConsole == NULL )
            m_pConsole = spConsole;
	}
    
    // 
    // Fetch handle to main window.
    // 
    // (Note: call fails during MMCN_REMOVE_CHILDREN. PAGBABI)
    if (event != MMCN_REMOVE_CHILDREN)
	{
        spConsole->GetMainWindow( &m_hMainWnd );
	}
	
	switch (event)
	{
	case MMCN_DBLCLICK:
		hr = S_FALSE;//spConsole->SelectScopeItem( m_hStaticNodeID );
		break;
		
	case MMCN_SHOW:
		hr = OnNotifyShow( pComponent, arg);
		if( arg )
			m_hStaticNodeID = (HSCOPEITEM) param;
//		else
//			m_hStaticNodeID = (HSCOPEITEM) NULL;
		break;
		
	case MMCN_SELECT:
		hr = OnNotifySelect( pComponent, arg );
		break;
		
	case MMCN_EXPAND:
		{
			m_hStaticNodeID = (HSCOPEITEM) param;
//			hr = RefreshData( spConsole );
		}
		break;
		
	case MMCN_VIEW_CHANGE:
		hr = OnNotifyViewChange( pComponent, arg, param );
		break;
		
	case MMCN_ADD_IMAGES:
		hr = OnNotifyAddImages( (IImageList*) arg );
		break;
		
	case MMCN_REFRESH:
		hr = OnNotifyRefresh( spConsole, pComponent );
		break;
        
	case MMCN_PROPERTY_CHANGE:
		if( arg == TRUE )
			hr = OnPropertyChange( spConsole, pComponent );
		break;
		
	case MMCN_REMOVE_CHILDREN:
		{
            CComQIPtr<IResultData, &IID_IResultData> spResultData(spConsole);
            hr = spResultData->DeleteAllRsltItems();
            DestroyData(&m_Data);
            m_bAttached = FALSE;
		}
		break;
		
		
	}
	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::OnNotifyRefresh
//
// Description   : Handles standard Refresh verb.
//
// Return type   : HRESULT 
//
// Argument      : IConsole* pConsole
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnNotifyRefresh( IConsole* pConsole, IComponent* pComponent )
{
    HRESULT hr;
    CComPtr< IConsole > pc = pConsole;
	
    // 
    // Tell views what is about to happen.
    // 
    CComPtr<IDataObject> pData;
    GetDataObject( &pData, CCT_SCOPE );
    hr = pc->UpdateAllViews( pData, 0, VIEW_HINT_REFRESH_BEGIN );
    if( FAILED( hr ) ) 
        return hr;
    
    // 
    // Rebuild data 
    // 
    hr = RefreshData( pc );
    if( FAILED( hr ) )
        return hr;
	//
	// Update Result Items from new datalist
	//
//	hr = UpdateResultItems (pc, pComponent);
	
    // 
    // Tell views that we are done refreshing.
    // 
    hr = pc->UpdateAllViews( pData, (long) pComponent, VIEW_HINT_REFRESH_END );
	
    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::OnNotifyRefresh
//
// Description   : Handles standard Refresh verb.
//
// Return type   : HRESULT 
//
// Argument      : IConsole* pConsole
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnPropertyChange( IConsole* pConsole, IComponent* pComponent )
{
    HRESULT hr;
    CComPtr< IConsole > pc = pConsole;
	
    // 
    // Tell views what is about to happen.
    // 
    CComPtr<IDataObject> pData;
    GetDataObject( &pData, CCT_SCOPE );
    hr = pc->UpdateAllViews( pData, 0, VIEW_HINT_FULL_REFRESH_BEGIN );
    if( FAILED( hr ) ) 
        return hr;
    
    // 
    // Rebuild data 
    // 
    hr = RefreshData( pc );
    if( FAILED( hr ) )
        return hr;
	
    // 
    // Tell views that we are done refreshing.
    // 
    hr = pc->UpdateAllViews( pData, (long) pComponent, VIEW_HINT_FULL_REFRESH_END );
	
    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::OnNotifyViewChange
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : IComponent* pComponent
// Argument      : long lData
// Argument      : long lHint
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnNotifyViewChange( IComponent* pComponent, long lData, long lHint )
{
    HRESULT hr = E_NOTIMPL;
    
    CComPtr< IConsole > pc = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
    CQSConsoleComponent* pComp = (CQSConsoleComponent*)pComponent;
    CComQIPtr< IConsole2 > pc2 = (((CQSConsoleComponent*)pComponent)->m_spConsole);
	
    ASSERT(spResultData);
	switch( lHint )
	{
	case VIEW_HINT_SERVER_ATTACH_BEGIN:
		{
		}
		break;
	case VIEW_HINT_SERVER_ATTACH_FINISH:
		{
            // 
            // Remove all items from result pane.
            // 
			hr = spResultData->DeleteAllRsltItems();
			if( pComponent == (IComponent*) lData || m_hStaticNodeID != NULL )
			{
			    OnNotifyShow( pComponent, FALSE, FALSE );//pc->SelectScopeItem( m_hStaticNodeID );
			}
            // 
            // Select our node.  This will force a repaint of the list contorl.
            //
			if(m_dwRefreshInProgress == 0)
				ReadQListFromQServer(pc2);
			if( pComponent == (IComponent*) lData || m_hStaticNodeID != NULL )
			    OnNotifyShow( pComponent, TRUE, TRUE );//pc->SelectScopeItem( m_hStaticNodeID );
		}
		break;
	case VIEW_HINT_REFRESH_BEGIN:
		{
            // 
            // Remove all items from result pane.
            // 
			//            hr = spResultData->DeleteAllRsltItems();
		}
		break;

	case VIEW_HINT_REFRESH_END:
		{
            // 
            // Select our node.  This will force a repaint of the list contorl.
            //
			//            if( pComponent == (IComponent*) lData || m_hStaticNodeID != NULL )
			//                OnNotifyShow( pComponent, TRUE, FALSE );//pc->SelectScopeItem( m_hStaticNodeID );
		}
		break;
		
	case VIEW_HINT_FULL_REFRESH_BEGIN:
		{
            // 
            // Remove all items from result pane.
            // 
			DestroyData(&m_Data);
            hr = spResultData->DeleteAllRsltItems();
			// 
			// Remove all result pane items.
			//
			POSITION pos;
			for( pos = pComp->m_dataMap.GetStartPosition(); pos != NULL; )
			{
				LPVOID pv, pv2;
				pComp->m_dataMap.GetNextAssoc( pos, pv, pv2 );
				delete (CQSConsoleResultData*) pv2;
			}
			// 
			// Remove all elements from the map.
			// 
			pComp->m_dataMap.RemoveAll();


		}
		break;
		
	case VIEW_HINT_FULL_REFRESH_END:
		{
			//
			// Update Result Items from new datalist
			//
            // 
            // Select our node.  This will force a repaint of the list contorl.
            //
            if( pComponent == NULL || m_hStaticNodeID != NULL )
			{
//				hr = UpdateResultItems (pc, pComponent);
	            OnNotifyShow( pComponent, TRUE, FALSE );//pc->SelectScopeItem( m_hStaticNodeID );
			}
		}
		break;
		
	case VIEW_HINT_DELETE_ITEM:
		{
            // 
            // Remove this item
            // 
            CItemData* pItemData = (CItemData*) lData; 
            if( pItemData == NULL )
                return E_UNEXPECTED;
            // 
            // Get result data item and delete from result pane.
            // 
            LPVOID pv = NULL;
            ((CQSConsoleComponent*)pComponent)->m_dataMap.Lookup( pItemData, pv );
            if( pv == NULL )
			{
                ASSERT( FALSE );
                return E_UNEXPECTED;
			}
            else
			{
                // 
                // Remove the item from this view.
                // 
                RESULTDATAITEM* prdi;
                ((CQSConsoleResultData*) pv)->GetResultData( &prdi );
                hr = spResultData->DeleteItem( prdi->itemID, 0 );
                if( FAILED( hr ) )
                    return hr;
			}
		}
		break;
		
	case VIEW_HINT_REFRESH_ITEM:
		{
            // 
            // Refresh this item.
            // 
            CItemData* pItemData = (CItemData*) lData; 
            if( pItemData == NULL )
                return E_UNEXPECTED;
			
			CQSConsoleResultData * pResD= NULL;
			
			// 
            // Get result data item and delete from result pane.
            // 
            LPVOID pv = NULL;
            ((CQSConsoleComponent*)pComponent)->m_dataMap.Lookup( pItemData, pv );
            if( pv == NULL )
			{
                ASSERT( FALSE );
                return E_UNEXPECTED;
			}
            else
			{
				pResD = (CQSConsoleResultData*) pv;
                // 
                // Update this item.
                // 
                RESULTDATAITEM* prdi;
                pResD->GetResultData( &prdi );
				// Check its icon state;
//		hr = spResultData->FindItemByLParam((LPARAM)pv,&prdi->itemID);

				// Is there a error state
//				pItemData->SetError( pItemData->IsSampleErrorCondition());

				// update icon index in the resultdataitem
//				pResD->m_resultDataItem.nImage = pItemData->m_uImage;
	
                hr = spResultData->UpdateItem( prdi->itemID );
                if( FAILED( hr ) )
                    return hr;
			}
		}
		break;
	}
    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::OnNotifySelect
//
// Description   : Enables standard verbs for the scope pane
//
// Return type   : HRESULT 
//
// Argument      : IConsole* pConsole
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnNotifySelect( IComponent* pComponent, LONG arg )
{
    CComQIPtr< IConsole2 > pc = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CComPtr< IConsoleVerb > pConsoleVerb;
	
    // 
    // If not scope pane, ignore.
    // 
    if( LOWORD( arg ) == 0 )
        return S_OK;
	
    UpdateCaption( pc, HIWORD( arg ) );
	
    // 
    // This call will check dirty flag, as well as save column widths.
    // 
    ((CQSConsoleComponent*)pComponent)->IsDirty();
	
    if( SUCCEEDED( pc->QueryConsoleVerb( &pConsoleVerb ) ) )
	{
        pConsoleVerb->SetDefaultVerb( MMC_VERB_NONE );
        pConsoleVerb->SetVerbState( MMC_VERB_REFRESH, ENABLED, TRUE );
        pConsoleVerb->SetVerbState( MMC_VERB_PROPERTIES, ENABLED, TRUE );
        //pConsoleVerb->SetVerbState( MMC_VERB_OPEN, ENABLED, TRUE );
        pConsoleVerb->SetDefaultVerb( MMC_VERB_OPEN );
        return S_OK;
	}
	
    
    return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::OnNotifyAddImages
//
// Description   : Handles MMCN_ADD_IMAGES
//
// Return type   : HRESULT 
//
// Argument      : IImageList* pImageList
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnNotifyAddImages( IImageList* pImageList )
{
    HRESULT hr = S_OK;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	ATLTRACE(_T("IImageList::ImageListSetStrip started\n"));
	HIMAGELIST hSmallImageList = NULL;

    
	// 
    // Copy small system image list here.
    // 
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    hSmallImageList = (HIMAGELIST)SHGetFileInfo( _T("Foo.EXE"), 
		FILE_ATTRIBUTE_NORMAL,
		&sfi, 
		sizeof( SHFILEINFO ), 
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES 
		);
	
    // 
    // Set first icon to our main icon.
    // 
    HICON hIcon = ::LoadIcon( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_MAIN_ICON ) );
    hr = pImageList->ImageListSetIcon( (long*)hIcon , 0 ); 
	
    // 
    // Set second icon to our error icon.
    // 
    hIcon = ::LoadIcon( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_SAMPLE_ERROR ) );
    hr = pImageList->ImageListSetIcon( (long*)hIcon , 1 ); 
	
	if (hSmallImageList != NULL)
	{
		int iCount = ImageList_GetImageCount( hSmallImageList );
		for( int i = 0; i < iCount; i++ )
		{
			hIcon = ImageList_GetIcon( hSmallImageList, i, ILD_NORMAL );
			hr = pImageList->ImageListSetIcon( (long*)hIcon , i + 2 ); 
			DestroyIcon( hIcon );
		}
	}
	if (FAILED(hr))
		ATLTRACE(_T("IImageList::ImageListSetStrip failed\n"));
	

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::OnNotifyShow
//
// Description   : Handles the MMCN_SHOW notification message.
//
// Return type   : HRESULT 
//
// Argument      :  IHeaderCtrl* pHeader
// Argument      : IConsole* pConsole
// Argument      : LONG arg
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnNotifyShow( IComponent* pComponent, LONG arg, BOOL bAddColumns /* = TRUE */ )
{
    HRESULT hr = S_OK;
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
    // 
    // Make sure we keep track of references.
    // 
    CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> ph;
    ph = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CComPtr< IConsole > pc = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CQSConsoleComponent* pComp = (CQSConsoleComponent*)pComponent;
	
    // 
    // Get pointer to result data. 
    // 
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
	
    spResultData->ModifyViewStyle( (MMC_RESULT_VIEW_STYLE)0, MMC_SINGLESEL );
	
	// 
    // Test for show state. 0 == de-selecting.
    // 
    if( arg == 0 )
	{
		if (m_dwRefreshInProgress)
		{
			m_pSRead->Abort();
			m_dwRefreshInProgress = 0;
		}
		DeleteColumnHeaders( pComponent);
		if (m_hwndManage)
		{
			::DestroyWindow( m_hwndManage );
			m_hwndManage = 0;
		}
	}
    else
	{
        // 
        // Add column headers.
        // 
        if( bAddColumns )
        {
			// create the hidden communcation window.
			CreateManageWnd((CQSConsoleComponent*)pComponent);
			OnNotifyRefresh( pc, pComponent );
			InsertColumnHeaders( pComponent);
        }
		
	}
	
	return hr;
}








///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::RemoveResultItem
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : LPVOID pItem
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::RemoveResultItem( LPVOID pItem )
{
    // 
    // Remove this item from our data map.
    // 
    return m_Data.RemoveKey( pItem ) ? S_OK : E_FAIL;   
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::UpdateCaption
//
// Description  : 
//
// Return type  : void 
//
// Argument     : IConsole* pc
//
///////////////////////////////////////////////////////////////////////////////
// 3/21/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleData::UpdateCaption( IConsole2* pc, BOOL bSel )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString s;
	USES_CONVERSION;
	
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
    if( bSel )
	{
        // 
        // Update description bar.
        //      
        if( m_pComponentData && GetServerName().IsEmpty() == FALSE )
		{
            // 
            // If we are pointing at the local machine, 
            // 
            DWORD dwLen = MAX_PATH; 
            GetComputerName( s.GetBuffer( MAX_PATH ), &dwLen );
            s.ReleaseBuffer();
			
            if( lstrcmpi( s, GetServerName() ) == 0 )
			{
                s.LoadString( IDS_LOCAL );
                m_sDescBarText.Format( IDS_DESCRIPTION_FORMAT, s );
			}
            else
                m_sDescBarText.Format( IDS_DESCRIPTION_FORMAT, GetServerName());
		}
        
        // 
        // Update Status text.
        // 
		s.Empty();
//        if( m_pComponentData && SUCCEEDED( m_pComponentData->GetNAVAPI()->Initialize()) )
//		{
//            SYSTEMTIME st = { 0 };
//            TCHAR szDate[32];
//            m_pComponentData->GetNAVAPI()->GetVirusDefsDate( &st );
//            GetDateFormat( NULL, DATE_SHORTDATE, &st, NULL, szDate, 32 );
//            s.Format( IDS_STATUS_BAR_FORMAT, szDate );
//		}
//        else
//		{
//            s.LoadString( IDS_STATUS_BAR_FORMAT_NODEFS );
//		}
		// save off the virus date text
		m_sStatusBarVirusDefDate = s;
		if (!m_sStatusBarSampleCountText.IsEmpty())
		{
			s = m_sStatusBarVirusDefDate + m_sStatusBarSampleCountText;
		}

        // 
        // This function will fail when we are running on MMC1.1.  So just add the
        // text to the description bar.
        // 

        if( FAILED( pc->SetStatusText( T2W(s.GetBuffer(0)) ) ) )
		{
            m_sDescBarText += _T(" - " );
            m_sDescBarText += s;
		};
	}
    else
	{
        m_sDescBarText.Empty();
	}
    
    spResultData->SetDescBarText(T2W( m_sDescBarText.GetBuffer(0)) );
    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::GetResultPaneColInfo
//
// Description   : 
//
// Return type   : LPOLESTR 
//
// Argument      : int nCol
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
LPOLESTR CQSConsoleData::GetResultPaneColInfo(int nCol)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
    CString s;
	
    // 
    // All nodes expect column 0 to be display name.
    // 
    if (nCol == 0)
	{
        s.LoadString( IDS_NODE_NAME );
        m_bstrDisplayName = (LPCOLESTR) (LPCTSTR)s;
        return m_bstrDisplayName;
	}
    else
	{
        // 
        // Check to see if we are extending the Computer Manager Node
        // 
        if( IS_EXTENSION_PRESENT( GetComponentData(), CExtendComputerManagement ) ) 
		{
            if( nCol < 5 )
			{
                nCol --;
                switch( nCol )
				{
				case 0:
					if( m_bstrCol[ nCol ].Length() == 0 )
						m_bstrCol[ nCol ].LoadString( AfxGetResourceHandle(), IDS_EXTENSION_SNAPIN );
					break;
					
				case 1:
					if( m_bstrCol[ nCol ].Length() == 0 )
						m_bstrCol[ nCol ].LoadString( AfxGetResourceHandle(), IDS_NODE_DESC );
					break;
				}
                return m_bstrCol[ nCol ];
			}
		}
        else if( IS_EXTENSION_PRESENT( GetComponentData(), CExtendNSCStatic ) ) 
		{
            if( nCol < 5 )
			{
                nCol --;
                switch( nCol )
				{
				case 0:
					if( m_bstrCol[ nCol ].Length() == 0 )
						m_bstrCol[ nCol ].LoadString( AfxGetResourceHandle(), IDS_NODE_DESC );
					break;
				}
                return m_bstrCol[ nCol ];
			}
		}
	}
	
    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : OnAttachServer
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      :  bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnAttachServer( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	SCOPEDATAITEM sdi = {0};
	HRESULT hr = S_OK;
	HRESULT lhr;
	
    if (!GetComponentData()->IsExtension())
	{
        sdi.ID = GetNodeID();
	}
    else
	{
        LPSCOPEDATAITEM psdi;
        GetScopeData( &psdi );
        sdi.ID = psdi->ID;
	}

	// We need to see if this node has been expanded before (we have attached to a server)
	sdi.mask = SDI_STATE;
    lhr = GetComponentData()->GetNameSpace()->GetItem( &sdi );

    // 
    // attach to server.
    // 
    if (sdi.nState == 0 && m_bAttached == 0)
		hr = AttachToServer( m_pComponentData->m_spConsole, FALSE);
	else
		hr = AttachToServer( m_pComponentData->m_spConsole, TRUE );
    if( SUCCEEDED( hr ) ) 
	{
		m_pComponentData->SetDirty();
		lhr = m_pConsole->SelectScopeItem(sdi.ID);
	}
    return hr;
}

void GetAMSServer(CString& s)
{
	CRegKey reg;
	TCHAR szAmsServerName[MAX_PATH]={0};
	DWORD dwSize = sizeof(szAmsServerName);

    if( ERROR_SUCCESS != reg.Create( HKEY_LOCAL_MACHINE, REGKEY_QSERVER, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ ) )
        return;
	reg.QueryStringValue(REGVALUE_AMS_SERVER_ADDRESS, szAmsServerName,  &dwSize);
    
	s = szAmsServerName;
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : OnConfigureAMS 
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      :  bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 2-11-00 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnConfigureAMS ( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hr = S_OK;	
USES_CONVERSION;    
	CString sCaption;
	
    sCaption.LoadString(IDS_AMS_CONFIG_CAPTION);
	GetAMSServer(m_sAMSServer);
	
	if(m_sAMSServer.IsEmpty())
	{
		CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
		CString sText((LPCTSTR) IDS_NO_AMS_SERVER_CONFIGURED );
		
		// 
		// There is no AMS server.
		// 
		HWND hWnd;
		GetComponentData()->m_spConsole->GetMainWindow( &hWnd );
		MessageBox( hWnd, sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
		return hr;
	}
	m_sAMSProductName.LoadString(IDS_QSEVENT_PRODUCT_NAME);
	m_sAMSServer.MakeUpper();
	ConfigureAMS(T2A(m_sAMSProductName.GetBuffer(0)),T2A(m_sAMSServer.GetBuffer(0)),T2A(sCaption.GetBuffer(0)));

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : OnDisplayVList
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      :  bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 7-10-00 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnDisplayVList( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hr = S_OK;	
USES_CONVERSION;    
	CString sCaption;
    sCaption.LoadString(IDS_DISPLAY_VIRUS_LIST);
    CVirusListView dlg;
	int i = 0;
	
	CString sTemp;
	sTemp.LoadString(IDS_NA);
	if(GetAttached()== CONNECTION_BROKEN)
	{
		hr = ReAttach();
		if (FAILED(hr))
			return E_FAIL;
	}
	UpdateCaption(m_pConsole);
    while (TRUE)
	{
		try
		{

			if (SUCCEEDED(GetSAVInfo()))
			{
				dlg.m_pSnapin = this;
				// 
				// Get the Virus enum interface from the server.
				// 
				CComPtr< IEnumSavVirusInfo > pEnum;
				hr = m_cSAVInfo->m_pQServerSavInfo->GetEnumVirusInfo( &pEnum );
				if( S_FALSE != hr )
				{
					// 
					// Need to set proxy blanket for this interface.
					// 
					hr = m_cSAVInfo->m_pQServerSavInfo.DCO_SetProxyBlanket( pEnum );
					if( FAILED( hr ) )
					{
						_com_issue_error( hr );
					}
					
					// 
					// Need to marshal this interface over to the property page, since
					// it runs in a seperate thread.
					// 
					hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IEnumSavVirusInfo ), pEnum, &dlg.m_pServerStream );
					if( FAILED( hr ) )
						_com_issue_error( hr );
				
					dlg.m_strPatternFile = m_sCurrentDefDate;
					dlg.DoModal();	
					if (m_cSAVInfo != NULL)
					{
						delete (m_cSAVInfo);
						m_cSAVInfo = NULL;
					}
				}
			}				
			else
			{
				int iRet;
				CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
				CString sText((LPCTSTR) IDS_VIRUS_LIST_LOAD_ERROR );
				m_pComponentData->m_spConsole->MessageBox( sText, sTitle, MB_ICONSTOP | MB_OK, &iRet ); 
		}
			break;
		}
		catch( _com_error e )
		{
			
			if (i++ >= 1)
			{
				hr = e.Error();
				break;
			}
			hr = ReAttach();
			if(FAILED(hr))
				break;
		}
		catch (...)
		{
			return E_FAIL;
		}
	}
	
	
    return hr;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name : OnViewAMSLog
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      :  bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 2-11-00 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::OnViewAMSLog( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hr = S_OK;	
USES_CONVERSION;    
    CString sCaption;

    sCaption.LoadString(IDS_AMS_LOG_CAPTION);
	GetAMSServer(m_sAMSServer);
    
	if(m_sAMSServer.IsEmpty())
	{
		CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
		CString sText((LPCTSTR) IDS_NO_AMS_SERVER_CONFIGURED );
		
		// 
		// There is no AMS server.
		// 
		HWND hWnd;
		GetComponentData()->m_spConsole->GetMainWindow( &hWnd );
		MessageBox( hWnd, sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
		return hr;
	}
	m_sAMSProductName.LoadString(IDS_QSEVENT_PRODUCT_NAME);
	m_sAMSServer.MakeUpper();
	ViewAMSLog(T2A(m_sAMSProductName.GetBuffer(0)),T2A(m_sAMSServer.GetBuffer(0)),T2A(sCaption.GetBuffer(0)));

	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::FillData
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : CLIPFORMAT cf
// Argument     : LPSTREAM pStream
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleData::FillData( CLIPFORMAT cf, LPSTREAM pStream )
{
    ULONG uWritten;
	
    // 
    // We only care about the display name.
    // 
    if( cf != m_CCF_DISPLAY_NAME )
        return CSnapInItemImpl< CQSConsoleData >::FillData( cf, pStream );
	
    // 
    // Return the real display name.
    // 
    int iLen = lstrlen( GetDisplayName() );
    return pStream->Write( GetDisplayName(), 
		iLen * sizeof(OLECHAR), 
		&uWritten);
	
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::GetDataObject
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : IDataObject** pDataObj
// Argument     : DATA_OBJECT_TYPES type
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleData::GetDataObject(IDataObject** pDataObj, DATA_OBJECT_TYPES type)
{
    ATLTRACE2(atlTraceSnapin, 0, _T("CQSConsoleData::GetDataObject\n"));	
	
    CComObject<CQSDataObject>* pData;
	HRESULT hr = CComObject<CQSDataObject>::CreateInstance(&pData);
	if (FAILED(hr))
		return hr;
	
	pData->m_objectData.m_pItem = this;
	pData->m_objectData.m_type = type;
	
	hr = pData->QueryInterface(IID_IDataObject, (void**)(pDataObj));
	return hr;
}




///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::AttachToServer
//
// Description   : This routine will attempt to attach to a quarantine server.
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::AttachToServer( IConsole* pConsole, BOOL bReAttach )
{
    HRESULT hr;
    CString s;
    HWND hWndParent;
    CComPtr< IConsole > pc = pConsole;
    LPDATAOBJECT    pDataObject = 0;
    CComQIPtr< IConsole2 > pc2 = pc;
	CString sStatus;
    DWORD dwLen = MAX_PATH; 
	TCHAR szLocalMachineName[MAX_PATH];
	USES_CONVERSION;
    // 
    // If we are already attached, then we don't need to do this.
    // 
    if( (IsAttached()== TRUE || GetAttached()== CONNECTION_BROKEN) && bReAttach == FALSE )
	{
		// we need to re-write the server version incase there are several 
		//nodes installed of different versions
		WriteQServerVersion (m_dwQSVersion);
        return S_OK;
	}
    //
    // Bring up the wizard if this is the first time we attach
    //
    s = GetServerName();
	
    if (s.IsEmpty())
    {
		
        CComQIPtr<IPropertySheetProvider> spProvider;
		
        spProvider = pConsole;
		
        if (!spProvider)
            return E_NOINTERFACE;
		
        CComQIPtr<IPropertySheetCallback> spCallback;
		
        spCallback = pConsole;
		
        if (!spCallback)
            return E_NOINTERFACE;
		
        //
        // must pass a non-null name - cannot be blank
        //
        CComBSTR bstrTitle;
		
        //
        // this name will not display when wizard created
        //
        bstrTitle.LoadString( IDS_NODE_NAME );
		
        //
        // create a data object for this node.
        // making it a snapin manager type will run the snap-in's
        // insertion wizard.
        //
        hr = GetDataObject( &pDataObject, CCT_SNAPIN_MANAGER );
		
        hr = spProvider->CreatePropertySheet( bstrTitle,
			false,
			1,
			pDataObject,
			0 );
		
        HWND hWnd;
        pConsole->GetMainWindow( &hWnd );
		
        if (SUCCEEDED(hr))
        {
            hr = spProvider->AddPrimaryPages( static_cast<IComponentData*>
				(GetComponentData()),
				TRUE,
				hWnd,
				TRUE );
			
            if (SUCCEEDED(hr))
                hr = spProvider->Show( (long)hWnd, 0 );
            else
                hr = spProvider->Show( -1, 0 );
			
            if (SUCCEEDED(hr)&& !m_sServer.IsEmpty())
                GetComponentData()->SetAttachExplicit( TRUE );
			else
			{
				// we canceled out of the dialog
				hr = E_FAIL;
				if(pDataObject)
					pDataObject->Release();
				return hr;
			}
        }
    }
    
	hr = GetDataObject( &pDataObject, CCT_SCOPE);

	if (bReAttach)
	{
	    hr = pc->UpdateAllViews( pDataObject, 0, VIEW_HINT_SERVER_ATTACH_BEGIN);
		
	}
	
    pc->GetMainWindow( &hWndParent );
    CServerDlg dlg( CWnd::FromHandle( hWndParent ) );
    while( TRUE )
	{
        // 
        // Ask user for server to attach to.
        //
        s = GetServerName();
		dlg.m_sServer = s;
        dlg.m_sDomain = GetDomainName();
        dlg.m_sUser = GetUserName();
        dlg.m_sPassword = GetPassword();
		
        GetComputerName( szLocalMachineName, &dwLen );
        if( bReAttach )
		{        
            if( dlg.DoModal() == IDCANCEL )
			{
				// put it back the way it was 
				m_cQSInterfaces->m_pQServer.DCO_SetServer( GetServerName() );
				m_cQSInterfaces->m_pQServer.DCO_SetIdentity( GetUserName(),
					GetPassword(),
					GetDomainName() );
				
				// 
				// Create remote object.
				// 
				hr = m_cQSInterfaces->m_pQServer.DCO_CreateObject();
                hr = E_FAIL;
                break;
			}
            s = dlg.m_sServer;
			// bug fix (JS) - if server is not local, name and password are required
			// but since a null password is allowed, only require username
			if( !(s.IsEmpty()) && (_tcsicmp (s,szLocalMachineName) != 0) && 
				dlg.m_sUser.IsEmpty())
			{
				int iRet;
				CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
				CString sText((LPCTSTR) IDS_NAMEPASSWORD_REQUIRED );
				pc->MessageBox( sText, sTitle, MB_ICONSTOP | MB_OK, &iRet ); 
				continue;
			}

		}
		
        // 
        // Try attaching to server.
        // 
        try
		{
			// 
            // Set up connection to server.
            //
            if( s.IsEmpty() )
			{
				s = szLocalMachineName;
			}
			else if ((_tcsicmp (s,szLocalMachineName) != 0) && bReAttach == FALSE && m_sPassword.IsEmpty())
			{
				bReAttach = TRUE;
				continue;
			}
			
			sStatus.LoadString(IDS_ATTACH_STATUS);
			pc2->SetStatusText(T2OLE(sStatus.GetBuffer(0)));
			// 
            // Server name should be uppercase
            // 
            s.MakeUpper();
			
			
            m_cQSInterfaces->m_pQServer.DCO_SetServer( s );
            m_cQSInterfaces->m_pQServer.DCO_SetIdentity( dlg.m_sUser,
				dlg.m_sPassword,
				dlg.m_sDomain );
			
            // 
            // Create remote object.
            // 
            hr = m_cQSInterfaces->m_pQServer.DCO_CreateObject();
            if( FAILED( hr ) )
                _com_issue_error( hr );
			
			m_dwQSVersion = 0;
	
			// Added by TerryM 
			//
			// Check to see if IQuarantineServerII exists.
			//
            m_cQSInterfaces->m_pQServerII.DCO_SetServer( s );
            m_cQSInterfaces->m_pQServerII.DCO_SetIdentity( dlg.m_sUser,
				dlg.m_sPassword,
				dlg.m_sDomain );
			
            // 
            // Create remote object II.
            // 
            hr = m_cQSInterfaces->m_pQServerII.DCO_CreateObject();
			
			//
			// If Succedded get Server version
			//
			if (SUCCEEDED(hr))
			{
				
				hr = m_cQSInterfaces->m_pQServerII->GetVersion(&m_dwQSVersion);
				
				// Added by TerryM to connect to the avis server config interface
				//
				// Check to see if Avis SErver exists.
				//
				m_cQSInterfaces->m_pAvisConfig.DCO_SetServer( s );
				m_cQSInterfaces->m_pAvisConfig.DCO_SetIdentity( dlg.m_sUser,
					dlg.m_sPassword,
					dlg.m_sDomain );
				
				// 
				// Create remote object II.
				// 
				hr = m_cQSInterfaces->m_pAvisConfig.DCO_CreateObject();
				// terrym 9-24-01 added to support securtiy for virus def info
				if (m_dwQSVersion > QSVERSION2X)
				{
					if (SUCCEEDED(GetSAVInfo()))
					{
						if (m_cSAVInfo != NULL)
						{
							delete (m_cSAVInfo);
							m_cSAVInfo = NULL;
						}
					}
					
				}
			}
			else
				hr = S_OK;
			WriteQServerVersion (m_dwQSVersion);
			m_pComponentData->SetDirty();

			
			
			//
			// Save user data
			//
			m_sUser = dlg.m_sUser;
			m_sPassword = dlg.m_sPassword;
			
            // 
            // Save server data.
            // 
            SetServerName( s );
            dlg.m_sDomain.MakeUpper();
            SetDomainName( dlg.m_sDomain );
            
			
            // 
            // Set server name
            // 
            SetDisplayServerName( s );
            SCOPEDATAITEM sdi;
            ZeroMemory( &sdi, sizeof ( SCOPEDATAITEM ) );
            sdi.mask = SDI_STR;
            if (!GetComponentData()->IsExtension())
			{
                sdi.ID = GetNodeID();
			}
            else
			{
                LPSCOPEDATAITEM psdi;
                GetScopeData( &psdi );
                sdi.ID = psdi->ID;
			}
            sdi.displayname = T2OLE(m_sDisplayName.GetBuffer(0));
            GetComponentData()->GetNameSpace()->SetItem( &sdi );
			
			if( bReAttach )
			{        
				DestroyData(&m_Data);
				hr = pc->UpdateAllViews( pDataObject, 0, VIEW_HINT_SERVER_ATTACH_FINISH);
			}

		}
        catch( _com_error e )
		{
            // 
            // Tell user what happened.
            //
            int iRet;
            CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
            CString sText;
            sText.Format( IDS_CONNECT_FAILIRE_FMT, s );
            pc->MessageBox( T2W(sText.GetBuffer(0)), T2W(sTitle.GetBuffer(0)), MB_ICONSTOP | MB_OK, &iRet ); 
            
            // 
            // Save off error results.
            // 
            hr = e.Error();
			
            // 
            // Want to prompt user for machine info.
            // 
            bReAttach = TRUE;
		}
		
        // 
        // If were are here, then things look good, bail out now.
        // 
        if( hr == S_OK )
		{
            SetAttached();
            break;
		}
        }
		if (pDataObject != NULL)
			pDataObject->Release();


		sStatus=CLEAR_STATUS_TEXT;
		pc2->SetStatusText(T2OLE(sStatus.GetBuffer(0)));
		return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::RefreshData
//
// Description   : 
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/27/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::RefreshData( IConsole* pConsole )
{
    
    HRESULT hr = S_OK;
	if (m_dwRefreshInProgress != 0)
		return hr;  // refresh is in progress but no error

    CComPtr< IConsole > pc = pConsole;
    CComQIPtr< IConsole2 > pc2 = pConsole;
	int i = 0;
    // 
    // First, remove all old data items.
    // 
	
	while (TRUE)
	{
		try
		{
			// 
			// Connect to server.
			// 
			if( FAILED( AttachToServer( pc ) ) )
			{
				return S_OK;
			}
			hr = ReadQListFromQServer(pc2)	;
			break;
		}
		catch( _com_error e )
		{
			
			if (i++ >= 1)
			{
				hr = e.Error();
				break;
			}
			hr = ReAttach();
		}
		catch( CMemoryException *e )
		{
			e->Delete();
			hr = E_OUTOFMEMORY;
		}
	}

	
//	if (NeedAttention())
//	{
//		ChangeIcon(pConsole, 1);
//	}
	
	
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::DestroyData
//
// Description   : Removes all data from our list.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 1/27/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleData::DestroyData(CMapPtrToPtr *DataList )
{
    // 
    // Remove all list elements
    // 
    if( !DataList->IsEmpty() )
	{
        POSITION pos;
        LPVOID pv, pv2;
        
        // 
        // Add data elements to list.
        // 
        for( pos = DataList->GetStartPosition(); pos != NULL; )
		{
            DataList->GetNextAssoc( pos, pv, pv2 );
            delete (CItemData*) pv;
			pv = NULL;
		}
        DataList->RemoveAll();
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::SetDisplayServerName
//
// Description  : 
//
// Return type  : void 
//
// Argument     : LPCTSTR szServer
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleData::SetDisplayServerName( LPCTSTR szServer ) 
{ 
    // 
    // Check to see if we are pointing to ourselves.
    //     
    CString sComputerName;
    DWORD dwLen = MAX_PATH; 
    GetComputerName( sComputerName.GetBuffer( MAX_PATH ), &dwLen );
    sComputerName.ReleaseBuffer();
	
    if( lstrcmpi( sComputerName, szServer ) == 0 )
	{
        m_sDisplayName.LoadString( IDS_LOCAL_NODE_NAME );
	}
    else
        m_sDisplayName.Format( IDS_NODE_NAME_FMT, szServer ); 
}




///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::SetServerName
//
// Description  : 
//
// Return type  : void 
//
// Argument     : LPCTSTR szServerName
//
///////////////////////////////////////////////////////////////////////////////
// 4/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleData::SetServerName( LPCTSTR szServerName ) 
{ 
    if( m_sServer != szServerName )
	{
        m_sServer = szServerName; 
        m_pComponentData->SetDirty();
	}    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::SetDomainName
//
// Description  : 
//
// Return type  : void 
//
// Argument     : LPCTSTR szDomainName
//
///////////////////////////////////////////////////////////////////////////////
// 4/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleData::SetDomainName( LPCTSTR szDomainName ) 
{ 
    if( m_sDomain != szDomainName )
	{
        m_sDomain = szDomainName; 
        m_pComponentData->SetDirty();
	}    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::SetUserName
//
// Description  : 
//
// Return type  : void 
//
// Argument     : LPCTSTR psz
//
///////////////////////////////////////////////////////////////////////////////
// 4/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleData::SetUserName( LPCTSTR psz ) 
{ 
    if( m_sUser != psz )
	{
        m_sUser = psz; 
        m_pComponentData->SetDirty();
	}    
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::SetPassword
//
// Description  : 
//
// Return type  : void 
//
// Argument     : LPCTSTR psz
//
///////////////////////////////////////////////////////////////////////////////
// 4/19/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleData::SetPassword( LPCTSTR psz ) 
{ 
    if( m_sPassword != psz )
	{
        m_sPassword = psz; 
	}    
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::SyncDataLists
//
// Description   : 
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/14/99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::SyncDataLists(CMapPtrToPtr *pDataList)
{
    
    HRESULT hr = S_OK;
    POSITION pos;
    LPVOID pv;
	CItemData *pidMemory, *pidResult;

	int i = 0;
	int j = 0;
	int iIndex = 0;
	
	// check to see if the main list that reflects the result pane is empty.  
	// If it is we don't need to continue.
	if (m_Data.IsEmpty() && pDataList->IsEmpty())
		return hr;
	i = pDataList->GetCount();
	j = m_Data.GetCount();
	for( pos = pDataList->GetStartPosition(); pos != NULL; )
    {
		// setup for next item
		pidMemory = NULL;
		pidResult = NULL;
        ++ iIndex;
		// 
        // Fetch next item from data map
        // 
        pDataList->GetNextAssoc( pos, pv, pv );
		pidMemory = (CItemData*)pv;
		
		// find the item data represented in the results pane
		FindItemByFileID(&m_Data, pidMemory, &pidResult);
		
		// if it doesnt exist we need to add it to the data list
		if (pidResult == NULL)
		{
			pidResult = new CItemData;
			pidResult->Initialize(pidMemory);
			pidResult->SetItemDataState(ITEM_DATA_STATE_NEEDS_ADDED);
			
            // 
            // Add to list
            // 
            m_Data.SetAt( pidResult, pidResult);
			
		}
		else // it does exist so we get to compare it.
		{
//			ATLTRACE(_T("Comparing Data in item 0x%x  \n"),pidResult);
			if (! (pidMemory->Compare(*pidResult))) // compare the two item data 
			{ // they match the state is unchanged.
				pidResult->SetItemDataState(ITEM_DATA_STATE_UNCHANGED);
			}
			else
			{// they don't match we need to update the UI
				// copy the new data over
//				ATLTRACE(_T("Syncing Data in item 0x%x  \n"),pidResult);
				pidResult->Initialize(pidMemory);
				// set the right state
				pidResult->SetItemDataState(ITEM_DATA_STATE_NEEDS_UPDATE);
			}
			
		}
		if (pidMemory)
		{
			delete pidMemory;
			pidMemory = NULL;
		}
	}// end for scanning the new memory list
	pDataList->RemoveAll();
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::ClearState
//
// Description   : 
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/14/99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::ClearState(CMapPtrToPtr *DataList )
{
	HRESULT hr=S_OK;
    POSITION pos;
    LPVOID pv;
	CItemData *pidItem = NULL;
	for( pos = DataList->GetStartPosition(); pos != NULL; )
    {
        DataList->GetNextAssoc( pos, pv, pv );
		pidItem = (CItemData*) pv;
		pidItem->SetItemDataState(ITEM_DATA_STATE_UNKNOWN );
	}	
	return hr;
	
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::FindItemByFileID
//
// Description   : 
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/14/99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::FindItemByFileID(CMapPtrToPtr *pDataList, CItemData *pItemData, CItemData **pItemDataFound )
{
	HRESULT hr = E_FAIL;
    POSITION pos;
    LPVOID pv;
	CItemData *pidItem = NULL;
	
	// Check params
	if (pDataList == NULL || pItemData == NULL || pItemDataFound == NULL)
		return E_INVALIDARG;
	
	// make sure we break the calling function if they assume it is always found.
	*pItemDataFound = NULL;
	
	// start scanning the list
	for( pos = pDataList->GetStartPosition(); pos != NULL; )
    {
        pDataList->GetNextAssoc( pos, pv, pv );
		pidItem = (CItemData*) pv;
		
		// check to see if we have the same id
		if(pidItem->m_dwItemID == pItemData->m_dwItemID)
		{
			hr = S_OK;
			*pItemDataFound = pidItem;
			break;
		}
	}	
	
	
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::UpdateResultItems
//
// Description   : 
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/14/99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::UpdateResultItems( IConsole* pConsole,IComponent* pComponent)
{
    HRESULT hr = S_OK;
    POSITION pos;
    LPVOID pv;
	CItemData *pidItemData=NULL;
    CQSConsoleResultData* pResultItem;
    RESULTDATAITEM* prdi;
	int noAdded=0;
	
    CComPtr< IDataObject > pData = NULL;
    CComPtr< IConsole > pc = pConsole;
	int iDisplayList = m_Data.GetCount();
	
    // 
    // Get pointer to result data. 
    // 
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
	
	// scan down data list and see what needs to be updated
	for( pos = m_Data.GetStartPosition(); pos != NULL; )
    {
		m_Data.GetNextAssoc( pos, pv, pv );
		pidItemData = (CItemData*)pv;
		// Does the displayed record need to be updated.
		if (pidItemData->GetItemDataState() == ITEM_DATA_STATE_NEEDS_UPDATE)
		{
			if (pData == NULL)
				GetDataObject( &pData, CCT_SCOPE/*CCT_RESULT*/ );

//			ATLTRACE (_T("Updating item 0x%x \n"),pidItemData);
			// update view
			pc->UpdateAllViews( pData,(long)pidItemData,
				VIEW_HINT_REFRESH_ITEM );		
			// need to change the state so it won't be updated next time around
			pidItemData->SetItemDataState(ITEM_DATA_STATE_UNCHANGED);
		}
		// new record needs to be added to the resultes pane
		else if (pidItemData->GetItemDataState() == ITEM_DATA_STATE_NEEDS_ADDED)
		{
			// need to change the state if we don't it will be added again next time
			pidItemData->SetItemDataState(ITEM_DATA_STATE_UNCHANGED);
            // 
            // Allocate new result pane item
            // 
            pResultItem = new CQSConsoleResultData( this, pidItemData);
			noAdded++;
			int size = sizeof(CQSConsoleResultData); 
            // 
            // Insert item into list.
            // 
            pResultItem->GetResultData( &prdi );
			
            // 
            // Add to map
            // 
            ((CQSConsoleComponent*)pComponent)->m_dataMap.SetAt( pidItemData, pResultItem );            
			
            // 
            // Add to results pane
            // 
            hr = spResultData->InsertItem( prdi );
		}
		
	}	
	
	return hr;
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::ReAttach
//
// Description   : This routine will attempt to re-attach to the quarantine server.
//					that had the attachment.  used in case DCom looses the connection
//					(it happens offten)
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/21/99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::ReAttach(void)
{
    HRESULT hr;
	
    // 
    // If we were already attached, if not then don't do this
    // 
    if( IsAttached() == FALSE)
        return E_FAIL;
	
	if(GetAttached()== CONNECTION_BROKEN)
	{
		int iRc = 0;
		// 
		// Tell the user that something bad happened.
		// 
		CString sError( (LPCTSTR) IDS_ERROR );
		CString sText;
		sText.Format(IDS_ERROR_QS_SERVER_DIED, GetServerName());
		HWND hWnd;
		GetComponentData()->m_spConsole->GetMainWindow( &hWnd );
		iRc = MessageBox( hWnd, sText, sError, MB_ICONEXCLAMATION | MB_YESNO);
		if (iRc != IDYES)
		{
	        return E_FAIL;
		}
	}
	if (m_cQSInterfaces != NULL)
	{
		delete m_cQSInterfaces;
		m_cQSInterfaces = NULL;
	}

	m_cQSInterfaces = new QServerInterfaces;
	if (m_cQSInterfaces == NULL)
		return E_OUTOFMEMORY;
	
	while (TRUE)
	{
		// 
		// Try attaching to server.
		// 
		try
		{
			
			m_cQSInterfaces->m_pQServer.DCO_SetServer( m_sServer);
			m_cQSInterfaces->m_pQServer.DCO_SetIdentity( m_sUser,
				m_sPassword,
				m_sDomain);
			
			// 
			// Create remote object.
			// 
			hr = m_cQSInterfaces->m_pQServer.DCO_CreateObject();
			if( FAILED( hr ) )
				_com_issue_error( hr );
			
			// Added by TerryM 
			//
			// Check to see if IQuarantineServerII exists.
			//
			m_cQSInterfaces->m_pQServerII.DCO_SetServer( m_sServer);
			m_cQSInterfaces->m_pQServerII.DCO_SetIdentity( m_sUser,
				m_sPassword,
				m_sDomain);
			
			// 
			// Create remote object II.
			// 
			hr = m_cQSInterfaces->m_pQServerII.DCO_CreateObject();
			
			if( FAILED( hr ) )
				_com_issue_error( hr );
			//
			// If Succedded get Server version
			//
			if (SUCCEEDED(hr))
			{
				hr = m_cQSInterfaces->m_pQServerII->GetVersion(&m_dwQSVersion);
				// if there was a dcom connection drop we need to reset the counter on the other side
				hr = m_cQSInterfaces->m_pQServerII->ResetQuarantineConfigInterface();
			}
			
			// Added by TerryM to connect to the avis server config interface
			//
			// Check to see if Avis SErver exists.
			//
			m_cQSInterfaces->m_pAvisConfig.DCO_SetServer( m_sServer);
			m_cQSInterfaces->m_pAvisConfig.DCO_SetIdentity( m_sUser,
				m_sPassword,
				m_sDomain );
			
			hr = m_cQSInterfaces->m_pAvisConfig.DCO_CreateObject();
			
			if( FAILED( hr ) )
				_com_issue_error( hr );

			if (SUCCEEDED(GetSAVInfo()))
			{
				if (m_cSAVInfo != NULL)
				{
					delete m_cSAVInfo;
					m_cSAVInfo = NULL;
				}
			}
		}
		catch( _com_error e )
		{
//			if (hr == 0x800706ba)
//			{
				int iRc = 0;
				// 
				// Tell the user that something bad happened.
				// 
				CString sError( (LPCTSTR) IDS_ERROR );
				CString sText;
				sText.Format(IDS_ERROR_QS_SERVER_DIED, GetServerName());
				HWND hWnd;
				GetComponentData()->m_spConsole->GetMainWindow( &hWnd );
				iRc = MessageBox( hWnd, sText, sError, MB_ICONEXCLAMATION | MB_YESNO);
				if (iRc == IDYES)
				{
					continue;
				}
				else
				{
					hr = e.Error();
					SetAttached(CONNECTION_BROKEN);
					break;
				}

		}
		catch(...)
		{
			return E_FAIL;
		}
		if( hr == S_OK )
		{
			SetAttached();
			break;
		}
	}
		
    // 
    // If were are here, then things look good, bail out now.
    // 
    
    return hr;
}

void CQSConsoleData::WriteQServerVersion(long dwVersion)
{
	CRegKey reg;
	LONG lResult;
	// 
	// Read in values from registry
	// 
	lResult = reg.Open( HKEY_LOCAL_MACHINE,
		REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS != lResult )
	{
		lResult = reg.Create( HKEY_LOCAL_MACHINE,
			REGKEY_QSCONSOLE);
	}
	
	reg.SetDWORDValue (REGVALUE_SELECTED_QSERVER_VERSION, dwVersion);
	reg.Close();
}
void CQSConsoleData::ChangeIcon(IConsole* pConsole, DWORD dwIconIndex )
{
    SCOPEDATAITEM sdi = {0};
    if (!GetComponentData()->IsExtension())
	{
        sdi.ID = GetNodeID();
	}
    else
	{
        LPSCOPEDATAITEM psdi;
        GetScopeData( &psdi );
        sdi.ID = psdi->ID;
	}
	sdi.nImage = dwIconIndex;
	sdi.nOpenImage = dwIconIndex;
	sdi.mask = SDI_IMAGE | SDI_OPENIMAGE;
	m_scopeDataItem.nImage = dwIconIndex;
	m_scopeDataItem.nOpenImage = dwIconIndex;
	GetComponentData()->GetNameSpace()->SetItem( &sdi );
}

 
BOOL CQSConsoleData::NeedAttention(void)
{
	BOOL bRc = FALSE;
	CString sAttention;
	DWORD dwCount = MAX_PATH;
	HRESULT hr = S_OK;
	int iRetry=0;
    VARIANT v;
    BSTR bstrKey;
    BSTR bstrValue;
	USES_CONVERSION;
	
    VariantInit( &v );
//    bstrKey = REGKEY_QSERVER;
//    bstrValue = REGVALUE_ATTENTION;
	// 
	// Read in values from quaratine server
	// 
	if(GetAttached()== CONNECTION_BROKEN)
	{
		hr = ReAttach();
		if (FAILED(hr))
			return E_FAIL;
	}
	while(TRUE)
	{

		try
		{
			_ASSERTE(m_cQSInterfaces);
			if (m_cQSInterfaces != NULL)
			{
				if (m_dwQSVersion >= QSVERSION2X) // terrym 9-21-01 3.0 version support
				{
					bstrKey = SysAllocString(T2COLE(REGKEY_QSERVER));
					bstrValue = SysAllocString(T2COLE(REGVALUE_ATTENTION));
					if( SUCCEEDED(m_cQSInterfaces->m_pAvisConfig->GetValue( bstrValue, bstrKey, &v ) ) )
					{
					sAttention = v.bstrVal;
					if (!sAttention.IsEmpty())
						bRc = TRUE;
					VariantClear( &v );
					}
					SysFreeString(bstrKey);
					SysFreeString(bstrValue);

				}
				break;
			}
			else
			{
				hr = E_FAIL;
				_com_issue_error( hr );
			}
		}
		catch(_com_error e)
		{
			if (++iRetry > 1)
			{
				hr = e.Error();
				break;
			}
			hr = ReAttach();
			if (FAILED(hr))
				break;
		}
		catch (...)
		{
			// 
			// Tell the user that something bad happened.
			// 
			CString sError( (LPCTSTR) IDS_ERROR );
			CString sText( (LPCTSTR) IDS_QSERVER_ATTENTION_ERROR );
			HWND hWnd;
			GetComponentData()->m_spConsole->GetMainWindow( &hWnd );
			MessageBox( hWnd, sText, sError, MB_ICONEXCLAMATION | MB_OK );
			break;
		}
	}
	return bRc;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::DeleteColumnHeaders
//
// Description   : Handles the MMCN_SHOW notification message.
//
// Return type   : HRESULT 
//
// Argument      :  IHeaderCtrl* pHeader
// Argument      : IConsole* pConsole
// Argument      : LONG arg
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::DeleteColumnHeaders( IComponent* pComponent)
{
    HRESULT hr = S_OK;
	
    // 
    // Make sure we keep track of references.
    // 
    CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> ph;
    ph = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CComPtr< IConsole > pc = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CQSConsoleComponent* pComp = (CQSConsoleComponent*)pComponent;
	
    // 
    // Get pointer to result data. 
    // 
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
	// if we are attached to a blue ice Qserver need to support a new set of displayed data.
//	if (m_dwQSVersion>=QSVERISON)
//	{
		
		for (int i=MAX_COLUMNS-1; i>=0; --i)
			hr = ph->DeleteColumn( i);
	
//	}
//	else
//	{	// we are attached to an old Qserver and we need to support the old columns
		
//		for (int i=MAX_RESULT_COLUMNS-1; i>=0; --i)
//			hr = ph->DeleteColumn( i);

//	}
//	DestroyData(&m_Data);

	// 
	// Remove all items from result pane.
	// 
	hr = spResultData->DeleteAllRsltItems();
	
	// 
	// Remove all result pane items.
	//
	POSITION pos;
	for( pos = pComp->m_dataMap.GetStartPosition(); pos != NULL; )
	{
		LPVOID pv, pv2;
		pComp->m_dataMap.GetNextAssoc( pos, pv, pv2 );
		delete (CQSConsoleResultData*) pv2;
	}
	
	// 
	// Remove all elements from the map.
	// 
	pComp->m_dataMap.RemoveAll();
	
	// 
	// Save column widths
	//
	pComp->SaveColumnWidths( ph );
	// destroy the Manage window: it will delete its CWnd object
	return hr;	
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::InsertColumnHeaders
//
// Description   : Handles the MMCN_SHOW notification message.
//
// Return type   : HRESULT 
//
// Argument      :  IHeaderCtrl* pHeader
// Argument      : IConsole* pConsole
// Argument      : LONG arg
//
///////////////////////////////////////////////////////////////////////////////
// 2/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::InsertColumnHeaders( IComponent* pComponent)
{
    HRESULT hr = S_OK;
    // 
    // Make sure we keep track of references.
    // 
    CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> ph;
    ph = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CComPtr< IConsole > pc = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CQSConsoleComponent* pComp = (CQSConsoleComponent*)pComponent;
	CString s;
	int dwWidth;
    // 
    // Get pointer to result data. 
    // 
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
	
	// see if we can get the column width of the first column.  If we succeed then there are already columes
	// installed and we should skip.
	hr = ph->GetColumnWidth(0,&dwWidth); 
	if (FAILED(hr))
	{
		USES_CONVERSION;
		// if we are attached to a blue ice Qserver need to support a new set of displayed data.
		if (m_dwQSVersion>=QSVERSION2X) // terrym 9-21-01 added 3.0 support
		{

			if (pComp->m_colInfo.m_dwfInit == FALSE || pComp->m_colInfo.m_iNoColumns != MAX_BI_RESULT_COLUMNS)
				pComp->m_colInfo.Init(MAX_BI_RESULT_COLUMNS);
			
			s.LoadString( IDS_COL_FILENAME );
			ph->InsertColumn( RESULT_COL_FILENAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_FILENAME ] );
			
			s.LoadString( IDS_COL_USERNAME );
			ph->InsertColumn( RESULT_COL_USERNAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_USERNAME ] );
			
			s.LoadString( IDS_COL_MACHINE_NAME );
			ph->InsertColumn( RESULT_COL_MACHINENAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_MACHINENAME ] );
			
			s.LoadString( IDS_COL_COMPLETED );
			ph->InsertColumn( RESULT_COL_COMPLEATE, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_COMPLEATE ] );
			
			s.LoadString( IDS_COL_AGE);
			ph->InsertColumn( RESULT_COL_AGE, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_AGE] );
			
			s.LoadString( IDS_COL_SAMPLE_STATE);
			ph->InsertColumn( RESULT_COL_STATE, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_STATE] );
			
			s.LoadString( IDS_COL_MIM_SEQ_NEEDED);
			ph->InsertColumn( RESULT_COL_SEQ_NEEDED, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_SEQ_NEEDED] );
			
			s.LoadString( IDS_COL_STATUS );
			ph->InsertColumn( RESULT_COL_STATUS, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_STATUS ] );
			
			s.LoadString( IDS_COL_VIRUS_NAME );
			ph->InsertColumn( RESULT_COL_VIRUS_NAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_VIRUS_NAME ] );
			
			s.LoadString( IDS_COL_ERROR);
			ph->InsertColumn( RESULT_COL_ERROR, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_ERROR] );
			
		}
		else
		{	// we are attached to an old Qserver and we need to support the old columns
			if (pComp->m_colInfo.m_dwfInit == FALSE || pComp->m_colInfo.m_iNoColumns != MAX_RESULT_COLUMNS)
				pComp->m_colInfo.Init(MAX_RESULT_COLUMNS);
			
			s.LoadString( IDS_COL_FILENAME );
			ph->InsertColumn( RESULT_COL_FILENAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_FILENAME ] );
			
			s.LoadString( IDS_COL_USERNAME );
	 		ph->InsertColumn( RESULT_COL_USERNAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_USERNAME ] );
			
			s.LoadString( IDS_COL_MACHINE_NAME );
			ph->InsertColumn( RESULT_COL_MACHINENAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_MACHINENAME ] );
			
			s.LoadString( IDS_COL_DOMAIN_NAME );
			ph->InsertColumn( RESULT_COL_DOMAINNAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_DOMAINNAME ] );
			
			s.LoadString( IDS_COL_RECIEVED );
			ph->InsertColumn( RESULT_COL_RECIEVED, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_RECIEVED ] );
			
			s.LoadString( IDS_COL_SUBMITTED );
			ph->InsertColumn( RESULT_COL_SUBMITTED, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_SUBMITTED ] );
			
			s.LoadString( IDS_COL_SUBMIT_BY );
			ph->InsertColumn( RESULT_COL_SUBMIT_BY, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_SUBMIT_BY ] );
			
			s.LoadString( IDS_COL_STATUS );
			ph->InsertColumn( RESULT_COL_STATUS, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_STATUS ] );
			
			s.LoadString( IDS_COL_VIRUS_NAME );
			ph->InsertColumn( RESULT_COL_VIRUS_NAME, T2W(s.GetBuffer(0)), 
				LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_VIRUS_NAME ] );
		}
		InitResultsPane(pComponent);
	}
	
	return hr;	
}


HRESULT CQSConsoleData::InitResultsPane (IComponent* pComponent)
{
    HRESULT hr = S_OK;
    // 
    // User is selecting our node, so populate result pane with items
    // 
    RESULTDATAITEM* prdi;
    POSITION pos;
    LPVOID pv;
    CQSConsoleResultData* pResultItem;
    CComPtr< IConsole > pc = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    // 
    // Get pointer to result data. 
    // 
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
	
	int iNoRec = m_Data.GetCount();
    CItemData* pData = NULL;
    
	// 
    // Add data elements to list.
    // 
    for( pos = m_Data.GetStartPosition(); pos != NULL && SUCCEEDED( hr ); )
	{
        // 
        // Fetch next item from data map
        // 
        m_Data.GetNextAssoc( pos, pv, pv );
		
        
        // 
        // Add elements to the list array the represents the results pane
        // 
        pData = (CItemData*)pv;
        
		ASSERT(pData);
		
        // 
        // Allocate new result pane item
        // 
        pResultItem = new CQSConsoleResultData( this, pData);
		
        // 
        // Insert item into list.
        // 
        pResultItem->GetResultData( &prdi );
		
        // 
        // Add to map
        // 
        ((CQSConsoleComponent*)pComponent)->m_dataMap.SetAt( pData, pResultItem );            
        // 
        // Add to results pane
        // 
        hr = spResultData->InsertItem( prdi );
	}
	return hr;
}


HRESULT CQSConsoleData::ReadQListFromQServer(IConsole2 *pc)
{
	HRESULT hr = S_OK;    
	EnterCriticalSection(&csThread);

	if (m_dwRefreshInProgress == 0)
	{
		InterlockedIncrement(&m_dwRefreshInProgress);
	}
	else
		return hr;


	// allocate thread class.  We won't delete it.  The thread will delete the class as it cleans up.
	m_pSRead = new CSampleRead;

		try
		{
			// spin off thread to read the samples
			//
			m_pSRead->ReadAllSamples(this);
			
		
		}
		catch( _com_error e )
		{
		}
	
	LeaveCriticalSection(&csThread);
	return hr;
}
void CQSConsoleData::GetAMSServer(CString& sAMSServer)
{

    VARIANT v;
    BSTR bstrKey;
    BSTR bstrValue;
	USES_CONVERSION;
    VariantInit( &v );

	try
	{
		_ASSERTE(m_cQSInterfaces);
		if (m_cQSInterfaces != NULL)
		{
			if (m_dwQSVersion >= QSVERSION2X)// terrym 9-21-01 added support for 3.0 version
			{
				bstrKey = SysAllocString(T2COLE(REGKEY_QSERVER));
				bstrValue = SysAllocString(T2COLE(REGVALUE_AMS_SERVER_ADDRESS));
				if( SUCCEEDED( m_cQSInterfaces ->m_pAvisConfig->GetValue( bstrValue, bstrKey, &v ) ) )
				{
					sAMSServer = v.bstrVal;
					VariantClear( &v );
				}
				else
					sAMSServer.Empty();
				SysFreeString(bstrKey);
				SysFreeString(bstrValue);
			}
		}
		else
		{
			_com_issue_error( E_FAIL );
		}
	}
	catch (...)
	{
        // 
        // Tell the user that something bad happened.
        // 
        CString sError( (LPCTSTR) IDS_ERROR );
        CString sText( (LPCTSTR) IDS_AMS_SERVER_CONFIG_ERROR );
		HWND hWnd;
		GetComponentData()->m_spConsole->GetMainWindow( &hWnd );
		MessageBox( hWnd, sText, sError, MB_ICONEXCLAMATION | MB_OK );
	}
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleData::UpdateResultItems
//
// Description   : 
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/14/99 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::PurgeMarkedItems( IConsole* pConsole,IComponent* pComponent  )
{
    HRESULT hr = S_OK;
    POSITION pos;
    LPVOID pv;
	CItemData *pidItemData=NULL;
    CQSConsoleResultData* pResultItem;
	
    CComPtr< IDataObject > pData;
    CComPtr< IConsole > pc = pConsole;
	
    // 
    // Get pointer to result data. 
    // 
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
	GetDataObject( &pData, CCT_RESULT );
	
	// scan down data list and see what needs to be updated
	for( pos = m_Data.GetStartPosition(); pos != NULL; )
    {
        m_Data.GetNextAssoc( pos, pv, pv );
		pidItemData = (CItemData*)pv;
		if (pidItemData->GetItemDataState() == ITEM_DATA_STATE_UNKNOWN || 
			pidItemData->GetItemDataState() == ITEM_DATA_STATE_NEEDS_DELETE)
		{
//			ATLTRACE (_T("Deleting --->>> item 0x%x \n"),pidItemData);
			// delete it from results pane
			pc->UpdateAllViews( pData,(long)pidItemData,
				VIEW_HINT_DELETE_ITEM);		
			
            // find the result object
			((CQSConsoleComponent*)pComponent)->m_dataMap.Lookup( pidItemData, pv );
            if( pv == NULL )
            {
                ASSERT( FALSE );
                return E_UNEXPECTED;
            }
            else
            {
				pResultItem = (CQSConsoleResultData*) pv;
			}
			// 
            // Remove this item completly
            // 
            m_Data.RemoveKey( pidItemData);
            ((CQSConsoleComponent*)pComponent)->m_dataMap.RemoveKey( pidItemData); 
            delete pidItemData;
			delete pResultItem;
		}
		
	}	
	ClearState(&m_Data);
	
	return hr;
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::UpdateToolbarButton
//
// Description  : 
//
// Return type  : BOOL 
//
// Argument     : UINT id
// Argument     : BYTE fsState
//
///////////////////////////////////////////////////////////////////////////////
// 3/29/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CQSConsoleData::UpdateToolbarButton(UINT id, BYTE fsState)
{
	// If this is version 2 of	qserver then hide the submit button
	if ((id== IDM_VIEW_AMS_LOG || id== IDM_CONFIGURE_AMS)&& 
		fsState == ENABLED  && m_dwQSVersion >= QSVERSION2X) // terrym 9-21-01 added support for version 3.0
		return TRUE;
	else if ((id== IDM_VIEW_AMS_LOG || id== IDM_CONFIGURE_AMS)&& 
		fsState == HIDDEN  && m_dwQSVersion < QSVERSION2X)
		return TRUE;
	
	else if (fsState == ENABLED)
		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleData::UpdateToolbarButton
//
// Description  : This function will login and obtain an interface to the SAVInterface
//				  on QServer.  It uses the login information stored in the node.  the
//				  node must be attached to a server before this function can be called.
//				  The calling function should then release the interface.
//				  This funciton must be called before
//				  every use of the savinterface variable in the node.
//
// Return type  : HRESULT
//
//
///////////////////////////////////////////////////////////////////////////////
// 3-24-02 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleData::GetSAVInfo(void)
{
	HRESULT hr = S_OK;
	
	m_cSAVInfo = new QServerSAVInfo;

	if (m_cSAVInfo == NULL)
		return E_OUTOFMEMORY;

	m_sCurrentDefDate.LoadString(IDS_NA);
	m_sCurrentDefVersion.LoadString(IDS_NA);

	try
	{
		if (m_dwQSVersion > QSVERSION2X)
		{
			m_cSAVInfo->m_pQServerSavInfo.DCO_SetServer( m_sServer );
			m_cSAVInfo->m_pQServerSavInfo.DCO_SetIdentity( m_sUser,
				m_sPassword,
				m_sDomain);
			
			// 
			// Create remote object II.
			// 
			hr = m_cSAVInfo->m_pQServerSavInfo.DCO_CreateObject();
			// IF successful extract the virus def info
			if(SUCCEEDED(hr))
			{
				VARIANT *pv = NULL;
				pv = new VARIANT;
				VariantInit (pv);
				try 
				{
					
					hr = m_cSAVInfo->m_pQServerSavInfo->GetSavDefVersion(pv);
					if (pv->vt == VT_BSTR && SUCCEEDED(hr) )
						m_sCurrentDefVersion= OLE2T(pv->bstrVal);
					
					VariantClear (pv);
					hr = m_cSAVInfo->m_pQServerSavInfo->GetSavDefDate(pv);
					if (pv->vt == VT_BSTR && SUCCEEDED(hr) )
						m_sCurrentDefDate = OLE2T(pv->bstrVal);
				}
				catch (_com_error e )
				{
					if (e.Error() != E_PENDING)
						_com_issue_error( hr );
					
				}
				delete pv;
				
			}
			
		}
		else
			hr = E_FAIL;
	}
	catch (...)
	{
		hr = E_FAIL;
	}

	return hr;
}

