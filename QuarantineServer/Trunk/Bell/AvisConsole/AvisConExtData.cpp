/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/



#include "stdafx.h"
#include "AvisConsole.h"
//#include "AvisCon.h"
#include "AvisConExtData.h"

#include "ConfirmationDialog.h"
#include "SubmitOrDeliver.h"




/////////////////////////////////////////////////////////////////////////////
// CAvisConExtData
static const GUID CAvisConExtGUID_NODETYPE = 
{ 0x1dae69de, 0xb621, 0x11d2, { 0x8f, 0x45, 0x30, 0x78, 0x30, 0x2c, 0x20, 0x30 } };
const GUID*  CAvisConExtData::m_NODETYPE = &CAvisConExtGUID_NODETYPE;
const OLECHAR* CAvisConExtData::m_SZNODETYPE = OLESTR("1DAE69DE-B621-11d2-8F45-3078302C2030");
const OLECHAR* CAvisConExtData::m_SZDISPLAY_NAME = OLESTR("Symantec AntiVirus Avis Console");
const CLSID* CAvisConExtData::m_SNAPIN_CLASSID = &CLSID_AvisCon;

// MAKE THE 1ST INSTANCE ACCESSABLE
static CAvisConExtData* pAvisConExtDataFirstInstance = NULL;
int g_ExtDataClassTotalCount = 0;
// Handle to main console window.
extern HWND g_hMainWnd;




/*----------------------------------------------------------------------------
    CAvisConExtData:: CAvisConExtData()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
CAvisConExtData::CAvisConExtData()
 {
     ++g_ExtDataClassTotalCount;
     m_iExtDataObjectID = GetNextObjectID();
     // ARE WE FIRST INSTANCE?
     if( g_ExtDataClassTotalCount == 1 && pAvisConExtDataFirstInstance == NULL )
     {
         m_bAreWeFirstInstance = TRUE;
         pAvisConExtDataFirstInstance = this;
     }
     else
         m_bAreWeFirstInstance = FALSE;

 	memset(&m_scopeDataItem, 0, sizeof(SCOPEDATAITEM));
 	memset(&m_resultDataItem, 0, sizeof(RESULTDATAITEM));
    g_hMainWnd = GetActiveWindow();
 	fWidePrintString("CAvisConExtData Constructor called. ObjectID= %d TotalCount= %d", 
                                      m_iExtDataObjectID, g_ExtDataClassTotalCount );

    // CAUSE IT TO READ FROM LOCAL REGISTRY   1/4/00
   	fWidePrintString("CAvisConExtData::CAvisConExtData calling SaveQServerVersion(), ObjectID= %d ", m_iExtDataObjectID );
    SaveQServerVersion(m_iExtDataObjectID);

 }


/*----------------------------------------------------------------------------
    CAvisConExtData:: ~CAvisConExtData()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
CAvisConExtData:: ~CAvisConExtData()
 {
     // DECREMENT OUR TOTAL INSTANCE COUNT
     --g_ExtDataClassTotalCount;
     fWidePrintString("CAvisConExtData Destructor called. ObjectID= %d TotalCount= %d", 
                                       m_iExtDataObjectID, g_ExtDataClassTotalCount );
     // IF WE WERE THE FIRST ONE, THEN CLEAR THE GLOBAL POINTER
     if( g_ExtDataClassTotalCount == 0 && pAvisConExtDataFirstInstance != NULL )
         pAvisConExtDataFirstInstance = NULL;  
 }




/*----------------------------------------------------------------------------
    CAvisConExtData::CreatePropertyPages

    pUnk is ptr to IExtendPropertySheetImpl,class CAvisConExtData.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAvisConExtData::CreatePropertyPages(LPPROPERTYSHEETCALLBACK lpProvider,
    long handle, 
	IUnknown* pUnk,
	DATA_OBJECT_TYPES type)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	
    HRESULT hr = 0;

    // IF ICEPACK NOT INSTALLED, THEN DON'T EXTEND ANYTHING 1/1/00
    if( !IsAvisIcePackSupported(m_iExtDataObjectID) )
		return S_OK;

    //  _ASSERTE(0 == "CAvisConExtData::CreatePropertyPages");
    fWidePrintString("CAvisConExtData::CreatePropertyPages  called. ObjectID= %d TotalCount= %d", 
                                       m_iExtDataObjectID, g_ExtDataClassTotalCount );

    // m_pDataObject IS A CQSDataObject FOR THE RESULT PANE
    if(m_pDataObject == NULL)
    {
        fWidePrintString("ERROR CAvisConExtData::CreatePropertyPages  m_pDataObject= NULL");
    	return E_UNEXPECTED;
    }

    // WHEN WE'RE AN EXT THE type PARM ARRIVES AS CCT_UNINITIALIZED
    //type = CCT_RESULT;
	////if (type == CCT_UNINITIALIZED || type == CCT_RESULT)
    try
	{
        // ACCESS TO ATTRIBUTES  
        CAttributeAccess* pAttributeAccess = new CAttributeAccess( m_pDataObject, m_iExtDataObjectID );
        if( pAttributeAccess == NULL ) 
            return E_OUTOFMEMORY;
        // DID WE SUCCEED AT GETTING THE INTERFACE FROM QSERVER?
        hr = pAttributeAccess->IsErrorGettingQserverInterface();
        if( hr != S_OK )
            return( hr );  // DON'T DISPLAY THE PROP PAGES

		//-----------------------------------------------------------
		// PAGE 1    3/20/99 jhill   "Sample Attributes"
		m_pSampleAttributesPage = new CSampleAttributesPage;
        if( m_pSampleAttributesPage == NULL ) 
            return E_OUTOFMEMORY;

        // SAVE OFF NOTIFICATION HANDLE.
        m_pSampleAttributesPage->m_pSnapin = this;
        // SAVE OFF TO USE WITH MMCN_PROPERTY_CHANGE. ONLY THIS PAGE 
        m_pSampleAttributesPage->m_lParam = (LPARAM) this;
        
        // PASS IN DATA FROM THE CURRENTLY SELECTED ITEM IN THE RESULT PANE
        //m_pSampleAttributesPage->Initialize( m_pDataObject, m_iExtDataObjectID );
        
        // ADD TO MMC PROPERTY SHEET.
        m_pSampleAttributesPage->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pSampleAttributesPage->m_psp );
        // HOOK THE CALLBACK FOR THIS PAGE ONLY TO DETECT WHEN THE PROP SHEET IS CLOSING
        m_pSampleAttributesPage->HookPropPageCallback();        
        HPROPSHEETPAGE hp1 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pSampleAttributesPage->m_psp );
        hr = lpProvider->AddPage( hp1 );

		//-----------------------------------------------------------
		// PAGE @   jhill   "Actions"
		m_pSampleActionsPage = new CSampleActionsPage;
		if(m_pSampleActionsPage==NULL) 
            return E_OUTOFMEMORY;
		    
        // SAVE OFF NOTIFICATION HANDLE.
        m_pSampleActionsPage->m_pSnapin = this;
		    
        // ADD TO MMC PROPERTY SHEET.
        m_pSampleActionsPage->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pSampleActionsPage->m_psp );
        HPROPSHEETPAGE hp3 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pSampleActionsPage->m_psp );
        hr = lpProvider->AddPage( hp3 );

		//-----------------------------------------------------------
		// PAGE 3   jhill   "Errors"
		m_pSampleErrorPage = new CSampleErrorPage;
		if(m_pSampleErrorPage==NULL) 
            return E_OUTOFMEMORY;
		    
        // SAVE OFF NOTIFICATION HANDLE.
        m_pSampleErrorPage->m_pSnapin = this;
		    
        // ADD TO MMC PROPERTY SHEET.
        m_pSampleErrorPage->m_psp.dwFlags |= PSP_HASHELP;  // ADD HELP BUTTON TO PROPSHEET
        MMCPropPageCallback( &m_pSampleErrorPage->m_psp );
        HPROPSHEETPAGE hp4 = ::CreatePropertySheetPage( (PROPSHEETPAGE*) &m_pSampleErrorPage->m_psp );
        hr = lpProvider->AddPage( hp4 );

		//-----------------------------------------------------------


        // NOW SAVE THE PAGE PTRS TO CSampleAttributesPage
        m_pSampleAttributesPage->m_pSampleAttributesPage     = m_pSampleAttributesPage;  
        m_pSampleAttributesPage->m_pSampleActionsPage        = m_pSampleActionsPage;     
        m_pSampleAttributesPage->m_pSampleErrorPage          = m_pSampleErrorPage;


        // ACCESS TO ATTRIBUTES THRU CACHE. 
        pAttributeAccess->m_pSampleAttributesPage = m_pSampleAttributesPage;
        pAttributeAccess->m_pSampleActionsPage    = m_pSampleActionsPage;     
        pAttributeAccess->m_pSampleErrorPage      = m_pSampleErrorPage;     

        // PASS IN DATA FROM THE CURRENTLY SELECTED ITEM IN THE RESULT PANE
        m_pSampleActionsPage->Initialize( pAttributeAccess, m_pDataObject, m_iExtDataObjectID );
        m_pSampleAttributesPage->Initialize( pAttributeAccess, m_pDataObject, m_iExtDataObjectID );
        m_pSampleErrorPage->Initialize( pAttributeAccess, m_pDataObject, m_iExtDataObjectID );

		return S_OK;
	}
    catch(...)
    {
        hr = E_UNEXPECTED;
    }
	return hr;
}



/*----------------------------------------------------------------------------
    CAvisConExtData::OnSubmitItem
    Called when Submit icon on toolbar pressed
    Also called when Submit is selected on the context menu

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAvisConExtData::OnSubmitItem( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());   
    fWidePrintString("CAvisConExtData::OnSubmitItem called. ObjectID= %d", m_iExtDataObjectID);

    HRESULT  hr = S_OK;
    if( !m_pDataObject )
        return(E_FAIL);

    // IF ICEPACK NOT INSTALLED, THEN DON'T EXTEND ANYTHING 1/1/00
    if( !IsAvisIcePackSupported(m_iExtDataObjectID) )
		return S_OK;

    CAvisResultItems* pSubmitAllItems = new CAvisResultItems( m_pDataObject, m_iExtDataObjectID, TRUE );
    if ( pSubmitAllItems == NULL )
        return(E_FAIL);

    hr = pSubmitAllItems->SubmitOrDeliverAllItems();

    pSubmitAllItems->Release();

    return hr;
}


/*----------------------------------------------------------------------------
    CAvisConExtData::OnDeliverItem
    Called when Deliver icon on toolbar pressed
    Also called when Deliver is selected on the context menu

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CAvisConExtData::OnDeliverItem( bool& bHandled, CSnapInObjectRootBase * pObj )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());   
    fWidePrintString("CAvisConExtData::OnDeliverItem called. ObjectID= %d", m_iExtDataObjectID);
    HRESULT  hr = S_OK;

    if( !m_pDataObject )
        return(E_FAIL);

    // IF ICEPACK NOT INSTALLED, THEN DON'T EXTEND ANYTHING 1/1/00
    if( !IsAvisIcePackSupported(m_iExtDataObjectID) )
		return S_OK;

    CAvisResultItems* pDeliverAllItems = new CAvisResultItems( m_pDataObject, m_iExtDataObjectID, FALSE );
    if ( pDeliverAllItems == NULL )
        return(E_FAIL);

    hr = pDeliverAllItems->SubmitOrDeliverAllItems();

    pDeliverAllItems->Release();

    return hr;
}

#if 0
/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
STDMETHODIMP CAvisConExtData::AddMenuItems(LPCONTEXTMENUCALLBACK piCallback,
                                  long  *pInsertionAllowed, DATA_OBJECT_TYPES type)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

    fPrintString("CAvisConExtData::AddMenuItems - result context menu added");

	SetMenuInsertionFlags(true, pInsertionAllowed);

	UINT menuID = GetMenuID();
	if (menuID == 0)
		return S_OK;

	HMENU hMenu = LoadMenu(_Module.GetResourceInstance(), MAKEINTRESOURCE(menuID));
	long insertionID;
	if (hMenu)
	{
		for (int i = 0; ; i++)
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
					insertionID = CCM_INSERTIONPOINTID_3RDPARTY_TASK;
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

			for (int j = 0; ; j++)
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
#endif






