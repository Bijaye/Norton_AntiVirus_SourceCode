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

// 
// Initialize our node type information here.
// 
static const GUID CQSConsoleGUID_NODETYPE = 
{ 0x26140c87, 0xb554, 0x11d2, { 0x8f, 0x45, 0x30, 0x78, 0x30, 0x2c, 0x20, 0x30 } };
const GUID*  CQSConsoleData::m_NODETYPE = &CQSConsoleGUID_NODETYPE;
const OLECHAR* CQSConsoleData::m_SZNODETYPE = OLESTR("26140C87-B554-11D2-8F45-3078302C2030");
const OLECHAR* CQSConsoleData::m_SZDISPLAY_NAME = OLESTR("Norton AntiVirus Quarantine");
const CLSID* CQSConsoleData::m_SNAPIN_CLASSID = &CLSID_QSConsole;


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

    try
        {
        // 
        // Create the new page
        // 
        CServerPage* pPage = new CServerPage;
        if( pPage == NULL )
            return E_OUTOFMEMORY;

        // 
        // Save off notification handle.
        // 
        pPage->m_lNotifyHandle = handle;
        pPage->m_pSnapin = this;

        // 
        // Get the configuration interface from the server.
        // 
        CComPtr< IQserverConfig > pConfig;
        if( S_FALSE != m_pQServer->Config( &pConfig ) )
            {
            // 
            // Need to set proxy blanket for this interface.
            // 
            m_pQServer.DCO_SetProxyBlanket( pConfig );

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
        }
    catch( _com_error e )
        {
        hr = e.Error();
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
    if( IsPropPageDisplayed() == FALSE && GetComponentData()->IsAttachExplicit() )
        return CSnapInItemImpl<CQSConsoleData>::AddMenuItems( piCallback, pInsertionAllowed, type );
    
    if( IsPropPageDisplayed() == FALSE && !GetComponentData()->IsExtension() )
        return CSnapInItemImpl<CQSConsoleData>::AddMenuItems( piCallback, pInsertionAllowed, type );

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
                if( menuItemInfo.wID == ID_TOP_ATTACHTOSERVER )
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

	if (pScopeDataItem->mask & SDI_STR)
        {
        if (GetComponentData()->IsAttachExplicit() && !m_sDisplayName.IsEmpty())
            {
            m_bstrDisplayName = m_sDisplayName;
            }
            else
            {
            CString s((LPCTSTR) IDS_NODE_NAME );
            m_bstrDisplayName = (LPCOLESTR) (LPCTSTR)s;
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
            else
                m_hStaticNodeID = (HSCOPEITEM) NULL;
            break;

        case MMCN_SELECT:
            hr = OnNotifySelect( pComponent, arg );
            break;

	    case MMCN_EXPAND:
            m_hStaticNodeID = (HSCOPEITEM) param;
            hr = RefreshData( spConsole );
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
                hr = OnNotifyRefresh( spConsole, pComponent );
            break;

        case MMCN_REMOVE_CHILDREN:
            {
            CComQIPtr<IResultData, &IID_IResultData> spResultData(spConsole);
            hr = spResultData->DeleteAllRsltItems();
            DestroyData();
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
    // Tell views that we are done refreshing.
    // 
    hr = pc->UpdateAllViews( pData, (long) pComponent, VIEW_HINT_REFRESH_END );

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

    switch( lHint )
        {
        case VIEW_HINT_REFRESH_BEGIN:
            {
            // 
            // Remove all items from result pane.
            // 
            hr = spResultData->DeleteAllRsltItems();
            }
            break;

        case VIEW_HINT_REFRESH_END:
            {
            // 
            // Select our node.  This will force a repaint of the list contorl.
            //
            if( pComponent == (IComponent*) lData || m_hStaticNodeID != NULL )
                OnNotifyShow( pComponent, TRUE, FALSE );//pc->SelectScopeItem( m_hStaticNodeID );
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
                // Update this item.
                // 
                RESULTDATAITEM* prdi;
                ((CQSConsoleResultData*) pv)->GetResultData( &prdi );
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
                                         
    // 
    // Copy small system image list here.
    // 
    SHFILEINFO sfi;
    ZeroMemory( &sfi, sizeof(SHFILEINFO) );
    HIMAGELIST hSmallImageList = (HIMAGELIST)SHGetFileInfo( _T("Foo.EXE"), 
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


    int iCount = ImageList_GetImageCount( hSmallImageList );
    for( int i = 0; i < iCount; i++ )
        {
        hIcon = ImageList_GetIcon( hSmallImageList, i, ILD_NORMAL );
        hr = pImageList->ImageListSetIcon( (long*)hIcon , i + 1 ); 
        DestroyIcon( hIcon );
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
        }
    else
        {
        // 
        // Add column headers.
        // 
        CString s;
        if( bAddColumns )
            {
            s.LoadString( IDS_COL_FILENAME );
            ph->InsertColumn( RESULT_COL_FILENAME, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_FILENAME ] );

            s.LoadString( IDS_COL_USERNAME );
            ph->InsertColumn( RESULT_COL_USERNAME, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_USERNAME ] );

            s.LoadString( IDS_COL_MACHINE_NAME );
            ph->InsertColumn( RESULT_COL_MACHINENAME, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_MACHINENAME ] );

            s.LoadString( IDS_COL_DOMAIN_NAME );
            ph->InsertColumn( RESULT_COL_DOMAINNAME, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_DOMAINNAME ] );

            s.LoadString( IDS_COL_RECIEVED );
            ph->InsertColumn( RESULT_COL_RECIEVED, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_RECIEVED ] );

            s.LoadString( IDS_COL_SUBMITTED );
            ph->InsertColumn( RESULT_COL_SUBMITTED, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_SUBMITTED ] );

            s.LoadString( IDS_COL_SUBMIT_BY );
            ph->InsertColumn( RESULT_COL_SUBMIT_BY, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_SUBMIT_BY ] );

            s.LoadString( IDS_COL_STATUS );
            ph->InsertColumn( RESULT_COL_STATUS, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_STATUS ] );

            s.LoadString( IDS_COL_VIRUS_NAME );
            ph->InsertColumn( RESULT_COL_VIRUS_NAME, s, 
                              LVCFMT_LEFT, pComp->m_colInfo.m_aColumnWidths[ RESULT_COL_VIRUS_NAME ] );
            }

        // 
        // User is selecting our node, so populate result pane with items
        // 
        RESULTDATAITEM* prdi;
        POSITION pos;
        LPVOID pv;
        CQSConsoleResultData* pResultItem;

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
            // Allocate new result pane item
            // 
            pResultItem = new CQSConsoleResultData( this, (CItemData*)pv );
            
            // 
            // Insert item into list.
            // 
            pResultItem->GetResultData( &prdi );

            // 
            // Add to map
            // 
            ((CQSConsoleComponent*)pComponent)->m_dataMap.SetAt( pv, pResultItem );            

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
                m_sDescBarText.Format( IDS_DESCRIPTION_FORMAT, GetServerName() );
            }
        
        // 
        // Update Status text.
        // 
        if( m_pComponentData && SUCCEEDED( m_pComponentData->GetNAVAPI()->Initialize()) )
            {
            SYSTEMTIME st = { 0 };
            TCHAR szDate[32];
            m_pComponentData->GetNAVAPI()->GetVirusDefsDate( &st );
            GetDateFormat( NULL, DATE_SHORTDATE, &st, NULL, szDate, 32 );
            s.Format( IDS_STATUS_BAR_FORMAT, szDate );
            }
        else
            {
            s.LoadString( IDS_STATUS_BAR_FORMAT_NODEFS );
            }

        // 
        // This function will fail when we are running on MMC1.1.  So just add the
        // text to the description bar.
        // 
        if( FAILED( pc->SetStatusText( s.GetBuffer(0) ) ) )
            {
            m_sDescBarText += _T(" - " );
            m_sDescBarText += s;
            };
        }
    else
        {
        m_sDescBarText.Empty();
        }
    
    spResultData->SetDescBarText( m_sDescBarText.GetBuffer(0) );
    
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

    // 
    // Reattach to server.
    // 
    HRESULT hr = AttachToServer( m_pComponentData->m_spConsole, TRUE );
    if( SUCCEEDED( hr ) ) 
        {
        hr = OnNotifyRefresh( m_pComponentData->m_spConsole );
        }

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

    // 
    // If we are already attached, then we don't need to do this.
    // 
    if( IsAttached() && bReAttach == FALSE )
        return S_OK;

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

        LPDATAOBJECT    pDataObject = 0;

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

            if (SUCCEEDED(hr))
                GetComponentData()->SetAttachExplicit( TRUE );
        }
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

        if( bReAttach )
            {        
            if( dlg.DoModal() == IDCANCEL )
                {
                hr = E_FAIL;
                break;
                }
            s = dlg.m_sServer;
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
                DWORD dwLen = MAX_PATH; 
                GetComputerName( s.GetBuffer( MAX_PATH ), &dwLen );
                s.ReleaseBuffer();
                }

            // 
            // Server name should be uppercase
            // 
            s.MakeUpper();


            m_pQServer.DCO_SetServer( s );
            m_pQServer.DCO_SetIdentity( dlg.m_sUser,
                                        dlg.m_sPassword,
                                        dlg.m_sDomain );

            // 
            // Create remote object.
            // 
            hr = m_pQServer.DCO_CreateObject();
            if( FAILED( hr ) )
                _com_issue_error( hr );

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
            sdi.displayname = m_sDisplayName.GetBuffer(0);
            GetComponentData()->GetNameSpace()->SetItem( &sdi );
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
            pc->MessageBox( sText, sTitle, MB_ICONSTOP | MB_OK, &iRet ); 
            
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
    CComPtr<IEnumQserverItemsVariant> pEnum; 
    CComPtr< IConsole > pc = pConsole;

    // 
    // First, remove all old data items.
    // 
    CComQIPtr<IResultData, &IID_IResultData> spResultData(pc);
    DestroyData();
    
    // 
    // Get the array of attributes required.
    // 
    SAFEARRAY* psa = CItemData::GetDataMemberArray();
    SafeArrayLock( psa );
    if( psa == NULL )
        {
        return E_OUTOFMEMORY;
        }

    try
        {
        // 
        // Connect to server.
        // 
        if( FAILED( AttachToServer( pc ) ) )
            {
            SafeArrayUnlock( psa );
            return S_OK;
            }

        // 
        // Set up variant array
        // 
        VARIANT vArray;
        VariantInit( &vArray );
        vArray.vt = VT_ARRAY;
        vArray.parray = psa;
        
        // 
        // Get the enumeration interface.
        // 
        m_pQServer->EnumItemsVariant( &vArray, &pEnum );
        m_pQServer.DCO_SetProxyBlanket( pEnum );

        // 
        // Begin enumeration.
        // 
        VARIANT items[10];
        ULONG fetched = 0;
        while( pEnum->Next( 10, items, &fetched ) == S_OK )
            {
            for( ULONG i = 0; i < fetched; i++ )
                {
                // 
                // Add elements.
                // 
                CItemData* pData = new CItemData;
                
                // 
                // Initialize safe array construct
                // 
                hr = pData->Initialize( items[i].parray );
                if( FAILED( hr ) )
                    {
                    delete pData;
                    continue;
                    }

                // 
                // Add to list
                // 
                m_Data.SetAt( pData, pData );

                // 
                // Clean up returned safe array array.
                //
                VariantClear( &items[i] );
                }
            fetched = 0;
            }
        }
    catch( _com_error e )
        {
        hr = e.Error();
        }
    catch( CMemoryException e )
        {
        e.Delete();
        hr = E_OUTOFMEMORY;
        }

    // 
    // Clean up allocated safe array.
    // 
    SafeArrayUnlock( psa );
    SafeArrayDestroy( psa );    

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
void CQSConsoleData::DestroyData()
{
    // 
    // Remove all list elements
    // 
    if( !m_Data.IsEmpty() )
        {
        POSITION pos;
        LPVOID pv, pv2;
        
        // 
        // Add data elements to list.
        // 
        for( pos = m_Data.GetStartPosition(); pos != NULL; )
            {
            m_Data.GetNextAssoc( pos, pv, pv2 );
            delete (CItemData*) pv;
            }
        m_Data.RemoveAll();
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
