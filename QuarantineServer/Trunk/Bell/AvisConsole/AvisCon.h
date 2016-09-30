/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#ifndef __AVISCON_H_
#define __AVISCON_H_
#include "resource.h"
#include <atlsnap.h>
#include "AvisConExtData.h"
#include "ScopeExtData.h"


static int g_CAvisConClassCount = 0;

class CAvisCon : public CComObjectRootEx<CComSingleThreadModel>,
public CSnapInObjectRoot<0, CAvisCon>,
	public IExtendControlbarImpl<CAvisCon>,
	public IExtendPropertySheetImpl<CAvisCon>,
	public IExtendContextMenuImpl<CAvisCon>,
    public ISnapinHelp,                // for HTML Help  4/5/99
	public CComCoClass<CAvisCon, &CLSID_AvisCon>
{
public:
    int m_iExtObjectID;


public:
	CAvisCon()
	{
        //++g_CAvisConClassCount;
        m_iExtObjectID = GetNextObjectID();
        fWidePrintString("CAvisCon Constructor called  ObjectID= %d", m_iExtObjectID);
		m_pComponentData = this;
		m_CCF_MULTI_SELECT_SNAPINS = RegisterClipboardFormat( _T("CCF_MULTI_SELECT_SNAPINS") );
	}

	~CAvisCon()
	{
        fWidePrintString("CAvisCon Destructor called  ObjectID= %d", m_iExtObjectID);
	}



EXTENSION_SNAPIN_DATACLASS(CAvisConExtData)
EXTENSION_SNAPIN_DATACLASS(CScopeExtData)

    // Dave 5/28
	HRESULT GetDataClass(IDataObject* pDataObject, CSnapInItem** ppItem, DATA_OBJECT_TYPES* pType) \
	{ 
		if (ppItem == NULL) 
			return E_POINTER; 
		if (pType == NULL) 
			return E_POINTER; 

        *ppItem = NULL; 

		*pType = CCT_UNINITIALIZED; 

        LPDATAOBJECT pRealDataObj = pDataObject;

        STGMEDIUM stgmedium = { TYMED_HGLOBAL, NULL }; 
		FORMATETC formatetc = { m_CCF_MULTI_SELECT_SNAPINS, 
			NULL, 
			DVASPECT_CONTENT, 
			-1, 
			TYMED_HGLOBAL 
		    }; 

        // 
        // Q: Is this a multi-select item?
        // 
        HRESULT hr = pDataObject->GetData(&formatetc, &stgmedium); 
		if (SUCCEEDED( hr )) 
		    {
           	fWidePrintString("CAvisCon::GetDataClass Multi-Select. ObjectID= %d", m_iExtObjectID );
                                      
            // 
            // Looks like it is.  Extract the real data object.
            // 
            SMMCDataObjects* pMSItems = (SMMCDataObjects*)GlobalLock( stgmedium.hGlobal );
            pRealDataObj = pMSItems->lpDataObject[0];
            GlobalUnlock( stgmedium.hGlobal); 
			GlobalFree(stgmedium.hGlobal); 
		    } 
        
        stgmedium.tymed = TYMED_HGLOBAL;
		formatetc.cfFormat = CSnapInItem::m_CCF_NODETYPE;
        formatetc.ptd = NULL;
		formatetc.dwAspect = DVASPECT_CONTENT;
        formatetc.lindex = -1;
        formatetc.tymed = TYMED_HGLOBAL;

		stgmedium.hGlobal = GlobalAlloc(0, sizeof(GUID)); 
		if (stgmedium.hGlobal == NULL) 
			return E_OUTOFMEMORY; 

		hr = pRealDataObj->GetDataHere(&formatetc, &stgmedium); 
		if (FAILED(hr)) 
		{ 
			GlobalFree(stgmedium.hGlobal); 
			return hr; 
		} 

        // THIS VARIABLE IS USED BY THE EXTENSION_SNAPIN_NODEINFO_ENTRY MACRO.
        GUID guid; 
		memcpy(&guid, stgmedium.hGlobal, sizeof(GUID)); 

		GlobalFree(stgmedium.hGlobal); 
		hr = S_OK;

        // PUT EXTENSION STUFF HERE.
        EXTENSION_SNAPIN_NODEINFO_ENTRY(CAvisConExtData)
    	EXTENSION_SNAPIN_NODEINFO_ENTRY(CScopeExtData)

		return CSnapInItem::GetDataClass(pDataObject, ppItem, pType); 
	};

#if 0
BEGIN_EXTENSION_SNAPIN_NODEINFO_MAP(CAvisCon)       // 
	EXTENSION_SNAPIN_NODEINFO_ENTRY(CScopeExtData)
//	EXTENSION_SNAPIN_NODEINFO_ENTRY(CAvisConExtData)  // DaveB 5/28 DON'T NEED
END_EXTENSION_SNAPIN_NODEINFO_MAP()
#endif

BEGIN_COM_MAP(CAvisCon)
	COM_INTERFACE_ENTRY(IExtendControlbar)
    COM_INTERFACE_ENTRY(IExtendPropertySheet)
    COM_INTERFACE_ENTRY(IExtendContextMenu)
    COM_INTERFACE_ENTRY(ISnapinHelp)        // for HTML Help 4/5/99
END_COM_MAP()

DECLARE_REGISTRY_RESOURCEID(IDR_AVISCON)

DECLARE_NOT_AGGREGATABLE(CAvisCon)



    // DAVE BUCHES 5/28/99
    STDMETHOD(ControlbarNotify)(MMC_NOTIFY_TYPE event, long arg, long param)
	{
		ATLTRACE2(atlTraceSnapin, 0, _T("CAvisCon::ControlbarNotify\n"));


        // IF ICEPACK NOT INSTALLED, THEN DON'T EXTEND ANYTHING 1/1/00 jhill
        if( !IsAvisIcePackSupported(m_iExtObjectID) )
		    return S_OK;


		CSnapInItem* pItem = NULL;
		DATA_OBJECT_TYPES type;
		HRESULT hr = S_OK;

		BOOL bSelect = (BOOL) HIWORD (arg);
		BOOL bScope = (BOOL) LOWORD(arg); 
        LPDATAOBJECT pDataObject = (event == MMCN_BTN_CLICK) ? (LPDATAOBJECT) arg : (LPDATAOBJECT) param;
		STGMEDIUM stgmedium = { TYMED_HGLOBAL, NULL }; 
		FORMATETC formatetc = { m_CCF_MULTI_SELECT_SNAPINS, 
			NULL, 
			DVASPECT_CONTENT, 
			-1, 
			TYMED_HGLOBAL 
		    }; 

        // Q: IS THIS A MULTI-SELECT ITEM?
        hr = pDataObject->GetData(&formatetc, &stgmedium); 
		if (SUCCEEDED( hr )) 
		    {
           	fWidePrintString("CAvisCon::ControlbarNotify Multi-Select. ObjectID= %d  event= 0x%X", m_iExtObjectID, event );
                                      

            // LOOKS LIKE IT IS.  EXTRACT THE REAL DATA OBJECT.          
            SMMCDataObjects* pMSItems = (SMMCDataObjects*)GlobalLock( stgmedium.hGlobal );
            pItem = m_CAvisConExtData.GetExtNodeObject(pDataObject, &m_CAvisConExtData);
            pItem->InitDataClass( pMSItems->lpDataObject[0], pItem );
            
            GlobalUnlock( stgmedium.hGlobal); 
			GlobalFree(stgmedium.hGlobal); 
		    } 
        else
            {
           	fWidePrintString("CAvisCon::ControlbarNotify Single-Select. ObjectID= %d  event= 0x%X", m_iExtObjectID, event );
                                      

            // SINGLE SELECION.  
    		hr = m_pComponentData->GetDataClass(pDataObject, &pItem, &type);
            }
		
        if (event == MMCN_SELECT)
		    {
			if (bSelect)
			    {
				int n = m_toolbarMap.GetSize();
				for (int i = 0; i < n; i++)
    				{
					IToolbar* pToolbar = (IToolbar*)m_toolbarMap.GetValueAt(i);
					 if (pToolbar != NULL)
						m_spControlbar->Detach(pToolbar);
	    			}
			    }
		    }

        // PROCESS NOTIFICATION MESSAGE.
		if (SUCCEEDED(hr) && pItem )
			hr = pItem->ControlbarNotify( m_spControlbar, this, &(m_toolbarMap), event, arg, param, (CSnapInObjectRootBase*) this, type);

		return hr;
	}


    // DAVE BUCHES 5/28/99
    STDMETHOD(AddMenuItems)(LPDATAOBJECT pDataObject,
        LPCONTEXTMENUCALLBACK piCallback,
        long *pInsertionAllowed)
	{
     	fWidePrintString("CAvisCon::AddMenuItems called. ObjectID= %d", m_iExtObjectID );

        // IF ICEPACK NOT INSTALLED, THEN DON'T EXTEND ANYTHING 1/1/00 jhill
        if( !IsAvisIcePackSupported(m_iExtObjectID) )
		    return S_OK;


		HRESULT hr = E_POINTER;

		ATLASSERT(pDataObject != NULL);
		if (pDataObject == NULL)
         	fWidePrintString("CAvisCon::AddMenuItems called with pDataObject==NULL. ObjectID= %d", m_iExtObjectID );
		else
		    {
            DATA_OBJECT_TYPES type;
            CSnapInItem* pItem = NULL;
		    STGMEDIUM stgmedium = { TYMED_HGLOBAL, NULL }; 
		    FORMATETC formatetc = { m_CCF_MULTI_SELECT_SNAPINS, 
			    NULL, 
			    DVASPECT_CONTENT, 
			    -1, 
			    TYMED_HGLOBAL 
		        }; 

            // Q: IS THIS A MULTI-SELECT ITEM?
            hr = pDataObject->GetData(&formatetc, &stgmedium); 
		    if (SUCCEEDED( hr )) 
		        {
             	fWidePrintString("CAvisCon::AddMenuItems Multi-Select. ObjectID= %d", m_iExtObjectID );
                                      
                // LOOKS LIKE IT IS.  EXTRACT THE REAL DATA OBJECT.
                SMMCDataObjects* pMSItems = (SMMCDataObjects*)GlobalLock( stgmedium.hGlobal );
                pItem = m_CAvisConExtData.GetExtNodeObject(pDataObject, &m_CAvisConExtData);
                pItem->InitDataClass( pMSItems->lpDataObject[0], pItem );
            
                GlobalUnlock( stgmedium.hGlobal); 
			    GlobalFree(stgmedium.hGlobal); 
		        } 
            else
            {           
             	fWidePrintString("CAvisCon::AddMenuItems Single-Select. ObjectID= %d",  m_iExtObjectID );
                                     

                // SINGLE SELECION.  
    		    hr = m_pComponentData->GetDataClass(pDataObject, &pItem, &type);
                }
            
			if (SUCCEEDED(hr))
				hr = pItem->AddMenuItems(piCallback, pInsertionAllowed, type);
		    }
		return hr;
	}

    // DAVE BUCHES 5/28/99
    STDMETHOD(Command)(long lCommandID, LPDATAOBJECT pDataObject)
	{
     	fWidePrintString("CAvisCon::Command called. IExtendContextMenu. ObjectID= %d", m_iExtObjectID );

		HRESULT hr = E_POINTER;

		ATLASSERT(pDataObject != NULL);
		if (pDataObject == NULL)
			ATLTRACE2(atlTraceSnapin, 0, _T("IExtendContextMenu::Command called with pDataObject==NULL\n"));
		else
		{
            DATA_OBJECT_TYPES type;
            CSnapInItem* pItem = NULL;
		    STGMEDIUM stgmedium = { TYMED_HGLOBAL, NULL }; 
		    FORMATETC formatetc = { m_CCF_MULTI_SELECT_SNAPINS, 
			    NULL, 
			    DVASPECT_CONTENT, 
			    -1, 
			    TYMED_HGLOBAL 
		        }; 

            // Q: IS THIS A MULTI-SELECT ITEM?
            hr = pDataObject->GetData(&formatetc, &stgmedium); 
		    if (SUCCEEDED( hr )) 
		        {
                // LOOKS LIKE IT IS.  EXTRACT THE REAL DATA OBJECT.
                SMMCDataObjects* pMSItems = (SMMCDataObjects*)GlobalLock( stgmedium.hGlobal );
                pItem = m_CAvisConExtData.GetExtNodeObject(pDataObject, &m_CAvisConExtData);
                pItem->InitDataClass( pMSItems->lpDataObject[0], pItem );
            
                GlobalUnlock( stgmedium.hGlobal); 
			    GlobalFree(stgmedium.hGlobal); 
		        } 
            else
                {
                // SINGLE SELECION.  
    		    hr = m_pComponentData->GetDataClass(pDataObject, &pItem, &type);
                }
			
			if (SUCCEEDED(hr))
				hr = pItem->Command(lCommandID, (CSnapInObjectRootBase*)this, type);
		}
		return hr;
	}





	// STDMETHOD(GetClassID)(CLSID *pClassID)
	// {
    //     fPrintString("CAvisCon::GetClassID");
	// }	

	static void WINAPI ObjectMain(bool bStarting)
	{
		if (bStarting)
			CSnapInItem::Init();
	}

    // pointer to path of compiled HTMLHelp file
	STDMETHOD(GetHelpTopic)( LPOLESTR* lpCompiledHelpFile );   
	//{
    //    fPrintString("CAvisCon::GetHelpTopic ------------------");
    //	return E_NOTIMPL;  // S_OK
	//}

	IDataObject* m_pDataObject;
	virtual void InitDataClass(IDataObject* pDataObject, CSnapInItem* pDefault)
	{
		m_pDataObject = pDataObject;
		// The default code stores off the pointer to the Dataobject the class is wrapping
		// at the time. 
		// Alternatively you could convert the dataobject to the internal format
		// it represents and store that information
    	//fPrintString("CAvisCon::InitDataClass called");
	}
	CSnapInItem* GetExtNodeObject(IDataObject* pDataObject, CSnapInItem* pDefault)
	{
		// Modify to return a different CSnapInItem* pointer.
    	//fPrintString("CAvisCon::GetExtNodeObject  called");
		return pDefault;
	}


    STDMETHOD(CreatePropertyPages)(LPPROPERTYSHEETCALLBACK lpProvider,
            long handle,
            LPDATAOBJECT pDataObject);




private:
    CLIPFORMAT   m_CCF_MULTI_SELECT_SNAPINS;

};






class ATL_NO_VTABLE CAvisConAbout : public ISnapinAbout,
	public CComObjectRoot,
	public CComCoClass< CAvisConAbout, &CLSID_AvisConAbout>
{
public:
//	DECLARE_REGISTRY(CAvisConAbout, _T("AvisConAbout.1"), _T("AvisConAbout.1"), IDS_AVISCON_DESC, THREADFLAGS_BOTH);
	DECLARE_REGISTRY_RESOURCEID(IDR_AVISCONABOUT)

	BEGIN_COM_MAP(CAvisConAbout)
		COM_INTERFACE_ENTRY(ISnapinAbout)
	END_COM_MAP()

	STDMETHOD(GetSnapinDescription)(LPOLESTR *lpDescription)
	{
		USES_CONVERSION;
		TCHAR szBuf[256];
		if (::LoadString(_Module.GetResourceInstance(), IDS_AVISCON_DESC, szBuf, 256) == 0)
			return E_FAIL;

		*lpDescription = (LPOLESTR)CoTaskMemAlloc((lstrlen(szBuf) + 1) * sizeof(OLECHAR));
		if (*lpDescription == NULL)
			return E_OUTOFMEMORY;

		ocscpy(*lpDescription, T2OLE(szBuf));

		return S_OK;
	}

	STDMETHOD(GetProvider)(LPOLESTR *lpName)
	{
		USES_CONVERSION;
		TCHAR szBuf[256];
		if (::LoadString(_Module.GetResourceInstance(), IDS_AVISCON_PROVIDER, szBuf, 256) == 0)
			return E_FAIL;

		*lpName = (LPOLESTR)CoTaskMemAlloc((lstrlen(szBuf) + 1) * sizeof(OLECHAR));
		if (*lpName == NULL)
			return E_OUTOFMEMORY;

		ocscpy(*lpName, T2OLE(szBuf));

		return S_OK;
	}

	STDMETHOD(GetSnapinVersion)(LPOLESTR *lpVersion)
	{
		USES_CONVERSION;
		TCHAR szBuf[256];
		if (::LoadString(_Module.GetResourceInstance(), IDS_AVISCON_VERSION, szBuf, 256) == 0)
			return E_FAIL;

		*lpVersion = (LPOLESTR)CoTaskMemAlloc((lstrlen(szBuf) + 1) * sizeof(OLECHAR));
		if (*lpVersion == NULL)
			return E_OUTOFMEMORY;

		ocscpy(*lpVersion, T2OLE(szBuf));

		return S_OK;
	}

	STDMETHOD(GetSnapinImage)(HICON *hAppIcon)
	{
		*hAppIcon = NULL;
		return S_OK;
	}

	STDMETHOD(GetStaticFolderImage)(HBITMAP *hSmallImage,
		HBITMAP *hSmallImageOpen,
		HBITMAP *hLargeImage,
		COLORREF *cMask)
	{
		*hSmallImageOpen = *hLargeImage = *hLargeImage = 0;
		return S_OK;
	}
};

#endif
