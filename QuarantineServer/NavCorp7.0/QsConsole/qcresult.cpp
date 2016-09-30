#include "stdafx.h"
#include "Qscon.h"
#include "QSConsole.h"
#include "Qcresult.h"
#include "macros.h"
#include "rdialog.h"
#include "MultiSelDataObject.h"
#include "FetchFileDlg.h"
#include "snditem.h"
#include "qscommon.h"
#define INITGUID
#include "iScandeliver.h"
// {1DAE69DE-B621-11d2-8F45-3078302C2030}
static const GUID CQSConsoleResultsGUID_NODETYPE = 
{ 0x1dae69de, 0xb621, 0x11d2, { 0x8f, 0x45, 0x30, 0x78, 0x30, 0x2c, 0x20, 0x30 } };

const GUID*  CQSConsoleResultData::m_NODETYPE = &CQSConsoleResultsGUID_NODETYPE;
const OLECHAR* CQSConsoleResultData::m_SZNODETYPE = OLESTR("1DAE69DE-B621-11d2-8F45-3078302C2030");
const OLECHAR* CQSConsoleResultData::m_SZDISPLAY_NAME = OLESTR("QSConsole Result Item");
const CLSID* CQSConsoleResultData::m_SNAPIN_CLASSID = &CLSID_QSConsole;


// 
// Add items to this list to remove items from 
// context menus when multiple items are selected.
// 
DWORD CQSConsoleResultData::m_adwMultiSelMenuItemExcude[] = {  
                                                              0xFFFFFFFF };



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::CreatePropertyPages
//
// Description   : Called by MMC framework when property pages need to be
//                 for this item.
//
// Return type   : HRESULT 
//
// Argument      : LPPROPERTYSHEETCALLBACK lpProvider
// Argument      : long handle
// Argument      : IUnknown* pUnk
// Argument      : DATA_OBJECT_TYPES type
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::CreatePropertyPages(LPPROPERTYSHEETCALLBACK lpProvider,
    long handle, 
    IUnknown* pUnk,
    DATA_OBJECT_TYPES type)
{
    HRESULT hr;    

    if (type == CCT_RESULT)
    {
        // 
        // Add our pages.
        // 
        hr = AddPages( lpProvider, handle );
        if( FAILED( hr ) )
            return hr;

        // 
        // Add other pages here.
        // 

        return hr;
    }
    return E_UNEXPECTED;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::AddGeneralPage
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : LPPROPERTYSHEETCALLBACK lpProvider
// Argument      : long handle
//
///////////////////////////////////////////////////////////////////////////////
// 2/9/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::AddPages(LPPROPERTYSHEETCALLBACK lpProvider, long handle )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;
    try
        {
        // 
        // Create the General page
        // 
        m_pGenPage = new CGeneralItemPage;
        if( m_pGenPage == NULL )
            return E_OUTOFMEMORY;

        // 
        // Save off notification handle.
        // 
        m_pGenPage->m_lNotifyHandle = handle;
        m_pGenPage->m_lParam = (LPARAM) this;
        hr = m_pGenPage->Initialize( m_pItemData );
        if( FAILED( hr ) )
            {
            return hr;
            }
        
        // 
        // Add to MMC property sheet.
        // 
        MMCPropPageCallback( &m_pGenPage->m_psp );
        HPROPSHEETPAGE hp = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pGenPage->m_psp );
        hr = lpProvider->AddPage( hp );

        // 
        // Add user page.
        // 
        m_pUserPage = new CUserItemPage;
        if( m_pUserPage == NULL )
            return E_OUTOFMEMORY;

        hr = m_pUserPage->Initialize( m_pItemData );
        if( FAILED( hr ) )
            {
            return hr;
            }
    
        MMCPropPageCallback( &m_pUserPage->m_psp );
        hp = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pUserPage->m_psp );
        hr = lpProvider->AddPage( hp );

        // 
        // Add virus info page if neccessary.
        // 
        if( m_pItemData->m_dwVirusID )
            {
            m_pVirusPage = new CVirusPage;
            if( m_pVirusPage == NULL )
                return E_OUTOFMEMORY;
        
            if( SUCCEEDED( m_pVirusPage->Initialize( m_pItemData->m_dwVirusID, m_pRootNode->GetComponentData()->GetNAVAPI() ) ) )
                {
                MMCPropPageCallback( &m_pVirusPage->m_psp );
                hp = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pVirusPage->m_psp );
                hr = lpProvider->AddPage( hp );
                }
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
// Function name : CQSConsoleResultData::GetScopePaneInfo
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : SCOPEDATAITEM *pScopeDataItem
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::GetScopePaneInfo(SCOPEDATAITEM *pScopeDataItem)
{
    if (pScopeDataItem->mask & SDI_STR)
        pScopeDataItem->displayname = m_bstrDisplayName;
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
// Function name : CQSConsoleResultData::GetResultPaneInfo
//
// Description   : Callback for list item data.
//
// Return type   : HRESULT 
//
// Argument      : RESULTDATAITEM *pResultDataItem
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::GetResultPaneInfo(RESULTDATAITEM *pResultDataItem)
{
    CString s;    

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
        switch( pResultDataItem->nCol )
            {
            case RESULT_COL_FILENAME:
                pResultDataItem->str = m_pItemData->m_sFileName.GetBuffer(0);
                break;
            case RESULT_COL_USERNAME:     
                pResultDataItem->str = m_pItemData->m_sUserName.GetBuffer(0);
                break;
            case RESULT_COL_DOMAINNAME:   
                pResultDataItem->str = m_pItemData->m_sDomainName.GetBuffer(0);
                break;
            case RESULT_COL_MACHINENAME:  
                pResultDataItem->str = m_pItemData->m_sMachineName.GetBuffer(0);
                break;
            case RESULT_COL_RECIEVED:
                pResultDataItem->str = m_pItemData->m_sRecievedDateStr.GetBuffer(0);
                break;
            case RESULT_COL_SUBMITTED:
                pResultDataItem->str = m_pItemData->m_sSubmittedDateStr.GetBuffer(0);
                break;
            case RESULT_COL_STATUS:
                pResultDataItem->str = m_pItemData->m_sStatusText.GetBuffer(0);
                break;
            case RESULT_COL_SUBMIT_BY:
                pResultDataItem->str = m_pItemData->m_sSubmittedBy.GetBuffer(0);
                break;
            case RESULT_COL_VIRUS_NAME:
                pResultDataItem->str = m_pItemData->m_sVirusName.GetBuffer(0);
                break;
            }
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
HRESULT CQSConsoleResultData::Notify( MMC_NOTIFY_TYPE event,
    long arg,
    long param,
    IComponentData* pComponentData,
    IComponent* pComponent,
    DATA_OBJECT_TYPES type)
{
    // Add code to handle the different notifications.
    // Handle MMCN_SHOW and MMCN_EXPAND to enumerate children items.
    // In response to MMCN_SHOW you have to enumerate both the scope
    // and result pane items.
    // For MMCN_EXPAND you only need to enumerate the scope items
    // Use IConsoleNameSpace::InsertItem to insert scope pane items
    // Use IResultData::InsertItem to insert result pane item.
    HRESULT hr = E_NOTIMPL;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    
    _ASSERTE(pComponentData != NULL || pComponent != NULL);

    CComPtr<IConsole> spConsole;
    CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> spHeader;
    if (pComponentData != NULL)
        spConsole = ((CQSConsole*)pComponentData)->m_spConsole;
    else
    {
        spConsole = ((CQSConsoleComponent*)pComponent)->m_spConsole;
        spHeader = spConsole;
    }

    switch (event)
        {
        case MMCN_DBLCLICK:
            hr = S_FALSE;
            break;
        
        case MMCN_SHOW:
            hr = S_OK;
            break;

        case MMCN_SELECT:
            m_pComponent = pComponent;
            hr = OnNotifySelect( pComponent, arg );
            break;

        case MMCN_DELETE:
            hr = OnNotifyDelete( pComponent );
            break;

        case MMCN_ADD_IMAGES:
            hr = OnNotifyAddImages( (IImageList*) arg );
            break;

        case MMCN_VIEW_CHANGE:
            ATLTRACE( "Foobar" );
            break;
        }

    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::OnNotifyDelete
//
// Description   : Delete this item from the result pane.
//
// Return type   : HRESULT 
//
// Argument      : IConsole* pConsole
//
///////////////////////////////////////////////////////////////////////////////
// 2/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::OnNotifyDelete( IComponent* pComponent, LPDATAOBJECT pData )
{
    HRESULT hr;
    CComPtr< IConsole > pc = ((CQSConsoleComponent*)pComponent)->m_spConsole;
    CString sTitle((LPCTSTR) IDS_TITLE );
    CString sText;
    IDataObject* pDataObj;
    CComPtr<IDataObject> pNodeDataObj;
    CQSConsoleComponent* pComp = (CQSConsoleComponent*)pComponent;
    
    // 
    // Get multisel data.
    // 
    CMultiSelDataObject* pMultiSel = CMultiSelDataObject::GetMultiSelectDataObject(pData); 
    if( pMultiSel == NULL )
        {
        sText.LoadString( IDS_DELETE_WARNING );
        }
    else
        {
        int iCount = pMultiSel->m_ItemList.GetCount();
        sText.Format( IDS_MULTI_DELETE_WARNING, iCount );        
        }
    
    // 
    // Check to see if there is a property page for any of the selected
    // items.
    // 
    CComQIPtr< IPropertySheetProvider, &IID_IPropertySheetProvider > pPropProvider(pc);
    if( pPropProvider == NULL )
        return E_FAIL;
  
    // 
    // Prompt user
    // 
    int iRet;
    pc->MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_YESNO, &iRet );
    if( iRet != IDYES )
        return S_OK;

    // 
    // Get pointer to ourselves.
    // 
    RESULTDATAITEM* prdi;
    hr = GetResultData( &prdi );
    if( FAILED( hr ) )
        return hr;
    
    // 
    // Set wait cursor.
    // 
    HCURSOR hCursor = ::SetCursor( ::LoadCursor( NULL, IDC_WAIT ) );
    
    m_pRootNode->GetDataObject( &pNodeDataObj, CCT_RESULT );
        
    if( pMultiSel )
        {
        CQSConsoleResultData* p;

        // 
        // Make sure none of these items exist before we fire off the delete.
        // 
        POSITION pos = pMultiSel->m_ItemList.GetHeadPosition();
        while( pos != NULL )
            {
            p = (CQSConsoleResultData*) pMultiSel->m_ItemList.GetNext( pos );
            if( SUCCEEDED( p->GetDataObject( &pDataObj, CCT_RESULT ) ) )
                {
                if( S_OK == pPropProvider->FindPropertySheet( (long)p, pComponent, pDataObj ) )
                    {
                    sText.LoadString( IDS_DELETE_PROPERTY_WARNING );
                    ::MessageBox( m_pRootNode->m_hMainWnd , sText, sTitle, MB_ICONSTOP | MB_OK );
                    pPropProvider->FindPropertySheet( (long)this, pComponent, pDataObj );
                    pDataObj->Release();
                    return S_OK;
                    }
                pDataObj->Release();
                }
            }

        // 
        // We need to nuke all items here.
        // 
        pos = pMultiSel->m_ItemList.GetHeadPosition();
        while( pos != NULL )
            {
            CQSConsoleResultData* p = (CQSConsoleResultData*) pMultiSel->m_ItemList.GetNext( pos );

            // 
            // Remove item from quaratine server
            // 
            if( FAILED( RemoveItemFromServer( p->m_pItemData ) ) )
                continue;
            
            // 
            // Notify all views that this item is now history
            // 
            hr = pc->UpdateAllViews( pNodeDataObj, (LONG) p->m_pItemData, VIEW_HINT_DELETE_ITEM );
            if( FAILED( hr ) )
                _com_issue_error( hr );

            // 
            // Remove this item completly
            // 
            m_pRootNode->m_Data.RemoveKey( p->m_pItemData );
            pComp->m_dataMap.RemoveKey( p->m_pItemData ); 
            delete p->m_pItemData;
            
            // 
            // Don't want to delete ourselves just yet.  Do so when loop terminates
            // 
            if( p != this )
                delete p;

            }

        // 
        // Delete ourselves.
        // 
        delete this;
        }
    else
        {
        // 
        // If there is a property page for this item, bring it to the 
        // front.
        // 
        if( SUCCEEDED( GetDataObject( &pDataObj, CCT_RESULT ) ) )
            {
            if( S_OK == pPropProvider->FindPropertySheet( (long)this, pComponent, pDataObj ) )
                {
                sText.LoadString( IDS_DELETE_PROPERTY_WARNING );
                ::MessageBox( m_pRootNode->m_hMainWnd , sText, sTitle, MB_ICONSTOP | MB_OK );
                pPropProvider->FindPropertySheet( (long)this, pComponent, pDataObj );
                pDataObj->Release();
                return S_OK;
                }
            pDataObj->Release();
            }

        // 
        // Remove item from quaratine server
        // 
        if( SUCCEEDED( RemoveItemFromServer( m_pItemData ) ) )
            {
            // 
            // Notify all views that this item is now history
            // 
            hr = pc->UpdateAllViews( pNodeDataObj, (LONG) m_pItemData, VIEW_HINT_DELETE_ITEM );
    
            // 
            // Remove this item completly
            // 
            m_pRootNode->m_Data.RemoveKey( m_pItemData );
            pComp->m_dataMap.RemoveKey( m_pItemData );
            delete m_pItemData;
            delete this;
            }
        }

    // 
    // Restore cursor.
    // 
    ::SetCursor( hCursor );

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::RemoveItemFromServer
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : DWORD dwItemID
//
///////////////////////////////////////////////////////////////////////////////
// 3/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::RemoveItemFromServer( CItemData* pItem )
{
    HRESULT hr = S_OK;
    CString sTitle, sText;
    

    try
        {
        m_pRootNode->m_pQServer->RemoveItem( pItem->m_dwItemID );
        }
    catch( _com_error e )
        {
        // 
        // Save off error return.
        // 
        hr = e.Error();
        switch( e.Error() )
            {
            case E_ACCESSDENIED:
                // 
                // Someone else has the file in use.  Need to display error dialog here.
                // 
                OnAccessDenied( pItem );
                break;
            
            case E_INVALIDARG:
                // 
                // Invalid arg means that the item in question doesn't
                // exist on the server.  This means that it was deleted by 
                // someone else.
                // 
                hr = S_OK;
                break;
            }
        }
    
    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::OnNotifySelect
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
HRESULT CQSConsoleResultData::OnNotifySelect( IComponent* pComponent, LONG arg, BOOL bMultiSel )
{
    CComQIPtr< IConsole2 > pc = (((CQSConsoleComponent*)pComponent)->m_spConsole);
    CComPtr< IConsoleVerb > pConsoleVerb;
    CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> spHeader;
    spHeader = pc;

    // 
    // If scope pane, ignore.
    // 
    if( LOWORD( arg ) )
        return S_OK;

    // 
    // Set the correct caption.
    // 
    m_pRootNode->UpdateCaption( pc );

    // 
    // Enable correct menu items.
    // 
    if( SUCCEEDED( pc->QueryConsoleVerb( &pConsoleVerb ) ) )
        {
        pConsoleVerb->SetDefaultVerb( MMC_VERB_NONE );
        pConsoleVerb->SetVerbState( MMC_VERB_DELETE, ENABLED, TRUE );
        
        // 
        // Only enable these verbs if we are not in multiselect mode
        // 
        if( !bMultiSel )
            {
            pConsoleVerb->SetVerbState( MMC_VERB_PROPERTIES, ENABLED, TRUE );
            pConsoleVerb->SetDefaultVerb( MMC_VERB_PROPERTIES );
            }

        return S_OK;
        }

    return E_FAIL;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::OnNotifyAddImages
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
HRESULT CQSConsoleResultData::OnNotifyAddImages( IImageList* pImageList )
{
    HRESULT hr = S_OK;
    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::GetResultPaneColInfo
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
LPOLESTR CQSConsoleResultData::GetResultPaneColInfo(int nCol)
{
    if (nCol == 0)
        return m_bstrDisplayName;

    // TODO : Return the text for other columns
    return OLESTR("Override GetResultPaneColInfo");
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::OnRestoreItem
//
// Description   : Handles Restore Menu Item.
//
// Return type   : HRESULT 
//
// Argument      : bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::OnRestoreItem( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = S_OK;

    bHandled = TRUE;

    hr = RestoreItems( &m_pItemData, 1 );
        
    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::OnRestoreItem
//
// Description   : Handles Repair Menu Item.
//
// Return type   : HRESULT 
//
// Argument      : bool& bHandled
// Argument      : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::OnRepairItem( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = S_OK;

    bHandled = TRUE;

    hr = RestoreItems( &m_pItemData, 1, TRUE );

    // 
    // Force repaint of this item.
    // 
    CComPtr< IDataObject > pData;
    m_pRootNode->GetDataObject( &pData, CCT_RESULT );
    m_pRootNode->GetComponentData()->m_spConsole->UpdateAllViews( pData, (long)m_pItemData, VIEW_HINT_REFRESH_ITEM );

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::RestoreItems
//
// Description   : Fires off restore dialog.
//
// Return type   : HRESULT 
//
// Argument      :  CItemData** pItems
// Argument      : DWORD dwNumItems
//
///////////////////////////////////////////////////////////////////////////////
// 3/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::RestoreItems( CItemData** pItems, DWORD dwNumItems, BOOL bRepair /* = FALSE */ )
{
    HRESULT hr = S_OK;
    
    // 
    // Make sure the user knows what he is doing here.
    // 
    CString sTitle, sText;
    DWORD dwFormat;
    if( bRepair )
        {
        sTitle.LoadString( IDS_ERROR_TITLE );
        if( dwNumItems > 1 )
            {
            sText.Format( IDS_MULTI_REPAIR_ITEM, dwNumItems );
            }
        else
            {
            sText.LoadString( IDS_REPAIR_ITEM );
            }
        dwFormat = MB_ICONQUESTION | MB_YESNO;
        }
    else
        {
        sTitle.LoadString( IDS_ERROR_TITLE );
        sText.LoadString( IDS_RESTORE_WARNING );
        dwFormat = MB_ICONWARNING | MB_YESNO;
        }

    if( IDYES != ::MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, dwFormat ) )
        return S_OK;

    // 
    // Prompt for save location
    // 
    if( S_OK != BrowseForSaveLocation() )
        return hr;
    
    // 
    // Need to disable AP for this process.
    // 
    DisableAP();

    CComPtr< ICopyItemData > pCopy;
    try
        {
        // 
        // Get copy interface from quarntine server.
        // 
        m_pRootNode->m_pQServer->GetCopyInterface( &pCopy );
        m_pRootNode->m_pQServer.DCO_SetProxyBlanket( pCopy );

        // 
        // Fire off dialog.
        // 
        CRDialog dlg( CWnd::FromHandle( m_pRootNode->m_hMainWnd ) );
        dlg.Initialize( pItems, dwNumItems, m_pRootNode->m_sRestoreLocation, pCopy, m_pRootNode, bRepair ? m_pRootNode->GetComponentData()->GetNAVAPI() : NULL );
        dlg.DoModal();
        }
    catch(...)
        {
        // 
        // bugbug: need error dialog here.
        // 
        }

    // 
    // Need to turn AP back on.
    // 
    EnableAP();

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::BrowseForSaveLocation
//
// Description   : 
//
// Return type   : HRESULT 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/24/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::BrowseForSaveLocation()
{
    HRESULT hr = S_OK;

    // 
    // Get shells IMalloc object for so we can free memory allocated by 
    // SHBrowseForFolder().
    // 
    CComPtr< IMalloc > pMalloc;
    hr = SHGetMalloc( &pMalloc );
    if( FAILED( hr ) )
        return hr;
    
    // 
    // Prompt user for restore location.
    // 
    BROWSEINFO b;
    TCHAR szBuffer[ MAX_PATH ];
    ZeroMemory( &b, sizeof( BROWSEINFO ) );
    CString s((LPCTSTR) IDS_BROWSE_FOR_SAVE_LOCATION );
    b.hwndOwner = m_pRootNode->m_hMainWnd;
    b.pszDisplayName = szBuffer;
    b.lpszTitle = s.GetBuffer(0);
    b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
    LPITEMIDLIST pidl = SHBrowseForFolder( &b );    

    // 
    // pidl == NULL when cancel button is clicked.
    // 
    if( pidl == NULL )
        return S_FALSE;
    
    // 
    // Get actual path
    // 
	SHGetPathFromIDList( pidl, m_pRootNode->m_sRestoreLocation.GetBuffer( MAX_PATH ) );
	m_pRootNode->m_sRestoreLocation.ReleaseBuffer();

    // 
    // Free shell allocation.
    // 
    pMalloc->Free( pidl );

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : STDMETHODIMPCQSConsoleResultData::MultiSelAddMenuItems
//
// Description   : This routine will enable the correct menu items when
//                 multiple items are selected in the result pane.  Most of this
//                 code was copied from ATLSNAP.H.                
//         
// Return type   : HRESULT
//
// Argument      : LPDATAOBJECT pDataObject
// Argument      : LPCONTEXTMENUCALLBACK piCallback
// Argument      : long *pInsertionAllowed
//
///////////////////////////////////////////////////////////////////////////////
// 3/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleResultData::MultiSelAddMenuItems( LPDATAOBJECT pDataObject,
                                 LPCONTEXTMENUCALLBACK piCallback,
                                 long *pInsertionAllowed )
{
    DWORD* pdwIndex;

    ATLTRACE2(atlTraceSnapin, 0, _T("CQSConsoleResultData::MultiSelAddMenuItemss\n"));

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

                // 
                // Check to see if this menu item needs to be 
                // excluded from the context menu.
                // 
                pdwIndex = m_adwMultiSelMenuItemExcude;
                BOOL bExclude = FALSE;
                while( *pdwIndex != 0xFFFFFFFF)
                    {
                    if( *pdwIndex == menuItemInfo.wID )
                        {
                        bExclude = TRUE;
                        break;
                        }
                    pdwIndex++;
                    }

                // 
                // If no virus definitions are installed, disable repair and submit.
                // 
                if( FAILED( m_pRootNode->GetComponentData()->GetNAVAPI()->Initialize()) )
                    {
                    if( menuItemInfo.wID == ID_TASK_REPAIRITEM ||
                        menuItemInfo.wID == ID_TASK_SUBMITITEMTOSARC )
                        {
                        bExclude = TRUE;
                        }
                    }

                if( bExclude )
                    continue;
                
                if (menuItemInfo.fType != MFT_STRING)
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

    // 
    // Get around bug in MMC 1.1 where context menus are not properly added 
    // when multiselecting. 
    // BUGBUG: Need to determing if we are running on MMC 1.2.  Do this by 
    // looking for interface IHeaderCtrl2 from IConsole object.
    // 
	IContextMenuProvider* pProvider = NULL;
	HRESULT hr = piCallback->QueryInterface( IID_IContextMenuProvider, (void**)&pProvider );
	if (SUCCEEDED(hr))
		{
		hr = pProvider->AddThirdPartyExtensionItems( pDataObject );

		pProvider->Release();
		}

	SetMenuInsertionFlags(true, pInsertionAllowed);

	return hr;
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
STDMETHODIMP CQSConsoleResultData::MultiSelNotify( MMC_NOTIFY_TYPE event,
    long arg,
    long param,
    IComponentData* pComponentData,
    IComponent* pComponent,
    DATA_OBJECT_TYPES type,
    LPDATAOBJECT pData )
{
    HRESULT hr = E_NOTIMPL;
    
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // 
    // Get multisel data.
    // 
    CMultiSelDataObject* pMultiSel = CMultiSelDataObject::GetMultiSelectDataObject( pData );
    if( pMultiSel == NULL )
        {
        return E_UNEXPECTED;
        }

    _ASSERTE(pComponentData != NULL || pComponent != NULL);

    CComPtr<IConsole> spConsole;
    CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> spHeader;
    if (pComponentData != NULL)
        spConsole = ((CQSConsole*)pComponentData)->m_spConsole;
    else
    {
        spConsole = ((CQSConsoleComponent*)pComponent)->m_spConsole;
        spHeader = spConsole;
    }

    switch (event)
        {
        case MMCN_SELECT:
            hr = OnNotifySelect( pComponent, arg, TRUE );
            break;

        case MMCN_DELETE:
            hr = OnNotifyDelete( pComponent, pData );
            break;

        default:
            return Notify( event, arg, param, pComponentData, pComponent, type );
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CQSConsoleResultData::MultiSelCommand
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
STDMETHODIMP CQSConsoleResultData::MultiSelCommand( long lCommandID,
                                                    LPDATAOBJECT pDataObject)
{
    HRESULT hr = S_OK;
    CItemData** pItems = NULL;
    int i = 0;
    POSITION pos;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // 
    // Get multisel data.
    // 
    CMultiSelDataObject* pMultiSel = CMultiSelDataObject::GetMultiSelectDataObject( pDataObject );
    if( pMultiSel == NULL )
        {
        return E_UNEXPECTED;
        }

    try
        {
        //        
        // Alloc array of items.
        // 
        pItems = new CItemData*[ pMultiSel->m_ItemList.GetCount() ];
        pos = pMultiSel->m_ItemList.GetHeadPosition();
        while( pos != NULL )
            {
            pItems[i++] = ((CQSConsoleResultData*)pMultiSel->m_ItemList.GetNext( pos ))->m_pItemData;
            }

        // 
        // Execute command hander
        //
        switch( lCommandID )
            {
            case ID_TASK_REPAIRITEM:
                hr = RestoreItems( pItems, pMultiSel->m_ItemList.GetCount(), TRUE );
                break;

            case ID_TASK_RESTOREITEM:
                hr = RestoreItems( pItems, pMultiSel->m_ItemList.GetCount(), FALSE ); 
                break;

            case ID_TASK_SUBMITITEMTOSARC:
                hr = SubmitItemsToSARC( pItems, pMultiSel->m_ItemList.GetCount() );
                break;
                
            }
        
        // 
        // Force repaint of selected items.
        // 
        CComPtr< IDataObject > pData;
        CItemData *pItemData;
        m_pRootNode->GetDataObject( &pData, CCT_RESULT );
        pos = pMultiSel->m_ItemList.GetHeadPosition();
        while( pos != NULL )
            {
            pItemData = ((CQSConsoleResultData*)pMultiSel->m_ItemList.GetNext( pos ))->m_pItemData;
            m_pRootNode->GetComponentData()->m_spConsole->UpdateAllViews( pData,
                                                                        (long)pItemData,
                                                                        VIEW_HINT_REFRESH_ITEM );
            }
        }
    catch(...)
        {
        hr = E_UNEXPECTED;
        }

    // 
    // Cleanup.
    // 
    if( pItems )
        delete [] pItems;

    return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::OnSubmitItem
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     :  bool& bHandled
// Argument     : CSnapInObjectRootBase * pObj
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::OnSubmitItem( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());   
    // 
    // Set handled flag.
    // 
    bHandled = TRUE;

    HRESULT hr = SubmitItemsToSARC( &m_pItemData, 1 );

    // 
    // Force repaint of this item.
    // 
    if( SUCCEEDED( hr ) ) 
        {
        CComPtr< IDataObject > pData;
        m_pRootNode->GetDataObject( &pData, CCT_RESULT );
        m_pRootNode->GetComponentData()->m_spConsole->UpdateAllViews( pData, (long)m_pItemData, VIEW_HINT_REFRESH_ITEM );
        }

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::SubmitItemsToSARC
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     :  CItemData** pItems
// Argument     : DWORD dwNumItems
//
///////////////////////////////////////////////////////////////////////////////
// 3/16/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::SubmitItemsToSARC( CItemData** pItems, DWORD dwNumItems ) 
{
    CSndItem** pSndItems = NULL;
    CRegKey reg;
    CString sInstallDir;
    CString sLanguage;
    LONG lResult;
    DWORD dwCount;

    // 
    // Ask for confirmation.
    //         
    CString sTitle;
    CString sText;
    sTitle.LoadString( IDS_ERROR_TITLE );
    if( dwNumItems > 1 )
        sText.Format( IDS_SUBMIT_PROMPT_FORMAT, dwNumItems );
    else
        sText.LoadString( IDS_SUBMIT_PROMPT );
    if( IDYES != MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONQUESTION | MB_YESNO ) )
        return S_OK;

    CComPtr< ICopyItemData > pCopy;
    CFetchFileDlg dlg( CWnd::FromHandle( m_pRootNode->m_hMainWnd ) );
    try
        {
        // 
        // Get copy interface from quarntine server.
        // 
        m_pRootNode->m_pQServer->GetCopyInterface( &pCopy );
        m_pRootNode->m_pQServer.DCO_SetProxyBlanket( pCopy );
        }
    catch( _com_error e )
        {
        return e.Error();
        }

    // 
    // Initialize file fetch dialog.
    // 
    if( FAILED( dlg.Initialize( m_pRootNode, pItems, dwNumItems, pCopy ) ) )
        {
        sTitle.LoadString( IDS_ERROR_TITLE );
        sText.LoadString( IDS_ERROR_INITIALIZING_SND );
        MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONSTOP | MB_OK );
        return E_FAIL;
        }

    // 
    // Fetch files from the server.
    // 
    if( dlg.DoModal() != IDOK )
        return S_OK;


    // 
    // Get the real count of files grabbed.
    // 
    DWORD dwNumFilesFetched = dlg.GetNumFilesFetched();

    // 
    // Check to see if any of these files have been blown away.
    // 
    if( dwNumFilesFetched != dwNumItems )
        {
        // 
        // Inform the user.
        // 
        sTitle.LoadString( IDS_ERROR_TITLE );
        sText.Format( dwNumItems == 0 ? IDS_ERROR_ITEM_GONE : IDS_ERROR_ITEMS_GONE, dwNumItems - dwNumFilesFetched );
        MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        
        // 
        // Refresh the result pane.
        // 
        CComPtr< IConsole > pc = ((CQSConsoleComponent*)m_pComponent)->m_spConsole;
        m_pRootNode->OnNotifyRefresh( pc );
        return S_OK;
        }
    
    // 
    // Submit files to SARC
    // 
    IScanDeliverDLL2* pSnd = NULL;
    if( FAILED( CoCreateInstance( CLSID_ScanDeliverDLL, NULL, CLSCTX_ALL, IID_ScanDeliverDLL2, (PVOID*) &pSnd ) ) )
        {
        sTitle.LoadString( IDS_ERROR_TITLE );
        sText.LoadString( IDS_ERROR_INITIALIZING_SND );
        MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONSTOP | MB_OK );
        return S_OK;
        }

    try
        {
        USES_CONVERSION;

        // 
        // Initialize scan and deliver.
        // 
        SYSTEMTIME st;
        m_pRootNode->GetComponentData()->GetNAVAPI()->GetVirusDefsDate( &st );
        pSnd->SetVirusDefinitionsDate( &st );
        pSnd->SetDefinitionsDir( T2A( m_pRootNode->GetComponentData()->GetNAVAPI()->GetVirusDefsDir()) );
        
        // 
        // Read in values from registry
        // 
        lResult = reg.Open( HKEY_LOCAL_MACHINE,
                            REGKEY_QSCONSOLE,
                            KEY_READ );
        if( ERROR_SUCCESS != lResult )
            throw E_FAIL;

        dwCount = MAX_PATH * sizeof( TCHAR );
        lResult = reg.QueryValue( sInstallDir.GetBuffer( dwCount ), 
                                  REGVAL_INSTALL_DIR,
                                  &dwCount );
        sInstallDir.ReleaseBuffer();
        if( ERROR_SUCCESS != lResult )
            throw E_FAIL;

        pSnd->SetStartupDir( T2A( sInstallDir ) );
        pSnd->SetConfigDir( T2A( sInstallDir ) );

        dwCount =  MAX_PATH * sizeof( TCHAR );

        sLanguage.LoadString ( IDS_LOCAL_LANGUAGE );

        pSnd->SetLanguage( T2A( sLanguage ) );

        // 
        // Allocate array of our pseudo SND items.
        // 
        pSndItems = new CSndItem* [ dwNumFilesFetched ];
        ZeroMemory( pSndItems, dwNumFilesFetched * sizeof( CSndItem* ) );
        for(DWORD i = 0; i < dwNumFilesFetched; i++)
            {
            // 
            // Allocate item
            // 
            pSndItems[i] = new CSndItem( pItems[i], 
                                         dlg.m_aFileDataArray[i]->sFileName, 
                                         m_pRootNode
                                         );
            }
        
        // 
        // Invoke scan and deliver
        // 
        DisableAP();
        pSnd->DeliverFilesToSarc( reinterpret_cast< IQuarantineItem **> (pSndItems), dwNumFilesFetched );
        EnableAP();
        }
    catch( HRESULT e )
        {
        e;
        sTitle.LoadString( IDS_ERROR_TITLE );
        sText.LoadString( IDS_ERROR_INITIALIZING_SND );
        MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONSTOP | MB_OK );
        }
    catch( _com_error e )
        {
        sTitle.LoadString( IDS_ERROR_TITLE );
        sText.LoadString( IDS_ERROR_INITIALIZING_SND );
        MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONSTOP | MB_OK );
        }
    catch( CMemoryException e )
        {
        sTitle.LoadString( IDS_ERROR_TITLE );
        sText.LoadString( IDS_ERROR_OUTOFMEMORY );
        MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONSTOP | MB_OK );
        }

    // 
    // Cleanup.
    // 
    pSnd->Release();
    if( pSndItems )
        {
        for( DWORD i = 0; i < dwNumItems; i++ )
            if( pSndItems[i] )
                delete pSndItems[i];
        delete[] pSndItems;
        }

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::ControlbarNotify
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
// 3/21/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleResultData::ControlbarNotify(IControlbar *pControlbar,
                                    IExtendControlbar *pExtendControlbar,
                                    CSimpleMap<UINT, IUnknown*>* pToolbarMap,
                                    MMC_NOTIFY_TYPE event,
                                    long arg, 
                                    long param,
                                    CSnapInObjectRootBase* pObj,
                                    DATA_OBJECT_TYPES type)
{
    
    // 
    // Figure out if we need to really be here.
    // 
    if( HIWORD (arg) == FALSE )
        return S_OK;

    

    HRESULT hr = CSnapInItemImpl< CQSConsoleResultData >::ControlbarNotify(pControlbar,
                                    pExtendControlbar,
                                    pToolbarMap,
                                    event,
                                    arg, 
                                    param,
                                    pObj,
                                    type );
    
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
BOOL CQSConsoleResultData::UpdateToolbarButton(UINT id, BYTE fsState)
{
	// 
    // Only enable repair and submit buttons if we have 
    // valid virus defs installed.
    // 
    BOOL bEnabled = SUCCEEDED( m_pRootNode->GetComponentData()->GetNAVAPI()->Initialize() );
    if( fsState == ENABLED )
        {
        if( (id == ID_TASK_REPAIRITEM || id == ID_TASK_SUBMITITEMTOSARC ) && !bEnabled )
            return FALSE;
        return TRUE;
        }

    if( fsState == INDETERMINATE )
        {
        if( (id == ID_TASK_REPAIRITEM || id == ID_TASK_SUBMITITEMTOSARC ) && !bEnabled )
            return TRUE;
        }

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::AddMenuItems
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : LPCONTEXTMENUCALLBACK piCallback
// Argument     : long  *pInsertionAllowed
// Argument     : DATA_OBJECT_TYPES type
//
///////////////////////////////////////////////////////////////////////////////
// 3/29/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQSConsoleResultData::AddMenuItems(LPCONTEXTMENUCALLBACK piCallback,
                                                long  *pInsertionAllowed,
                                                DATA_OBJECT_TYPES type)
{
    return MultiSelAddMenuItems( NULL, piCallback, pInsertionAllowed ); 
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::OnAccessDenied
//
// Description  : 
//
// Return type  : HRESULT 
//
// Argument     : CItemData* pItem
//
///////////////////////////////////////////////////////////////////////////////
// 3/25/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CQSConsoleResultData::OnAccessDenied( CItemData* pItem )
{
    CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
    CString sText;

    // 
    // Tell user that the item in question is in use.
    // 
    sText.Format( IDS_ERROR_ACCESS_DENIED, pItem->m_sFileName );
    MessageBox( m_pRootNode->m_hMainWnd, sText, sTitle, MB_ICONEXCLAMATION | MB_OK );

    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::GetDataObject
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
STDMETHODIMP CQSConsoleResultData::GetDataObject(IDataObject** pDataObj, DATA_OBJECT_TYPES type)
{
    ATLTRACE2(atlTraceSnapin, 0, _T("CQSConsoleData::GetDataObject\n"));	

    CComObject<CQSResultDataObject>* pData;
	HRESULT hr = CComObject<CQSResultDataObject>::CreateInstance(&pData);
	if (FAILED(hr))
		return hr;

	pData->m_objectData.m_pItem = this;
	pData->m_objectData.m_type = type;
    pData->m_pComponent = m_pRootNode->GetComponentData();

	hr = pData->QueryInterface(IID_IDataObject, (void**)(pDataObj));
	return hr;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: CQSConsoleResultData::DeletePropPages
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 4/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CQSConsoleResultData::DeletePropPages()
    {
    if( m_pGenPage )
        {
        if( m_pGenPage->Release() == 0)
            m_pGenPage = NULL;
        }

    if( m_pUserPage )
        {
        if( m_pUserPage->Release() == 0)
            m_pUserPage = NULL;
        }

    if( m_pVirusPage )
        {
        if( m_pVirusPage->Release() == 0)
            m_pVirusPage = NULL;
        }
    }

