#ifndef __QCRESULT_H_
#define __QCRESULT_H_
#include "resource.h"
#include <atlsnap.h>
#include "qsconsole.h"
#include "itemdata.h"
#include "GeneralItemPage.h"
#include "useritempage.h"
#include "viruspage.h"

// 
// This class is the data item for the results pane
// 

class CQSConsoleResultData : public CSnapInItemImpl<CQSConsoleResultData>
{
public:

    static const GUID* m_NODETYPE;
	static const OLECHAR* m_SZNODETYPE;
	static const OLECHAR* m_SZDISPLAY_NAME;
	static const CLSID* m_SNAPIN_CLASSID;
    static DWORD m_adwMultiSelMenuItemExcude[];

	BEGIN_SNAPINCOMMAND_MAP(CQSConsoleResultData, FALSE)
        SNAPINCOMMAND_ENTRY(ID_TASK_RESTOREITEM, OnRestoreItem )
        SNAPINCOMMAND_ENTRY(ID_TASK_REPAIRITEM, OnRepairItem )
        SNAPINCOMMAND_ENTRY(ID_TASK_SUBMITITEMTOSARC, OnSubmitItem )
	END_SNAPINCOMMAND_MAP()

	SNAPINMENUID(IDR_RESULT_MENU)

	BEGIN_SNAPINTOOLBARID_MAP(CQSConsoleResultData)
		// Create toolbar resources with button dimensions 16x16 
		// and add an entry to the MAP. You can add multiple toolbars
		// SNAPINTOOLBARID_ENTRY(Toolbar ID)
        SNAPINTOOLBARID_ENTRY(IDR_ITEM_TOOLBAR)
	END_SNAPINTOOLBARID_MAP()

    CQSConsoleResultData( CQSConsoleData* pRoot, CItemData* pItemData )
        {
        m_pRootNode = pRoot;
        m_pItemData = pItemData;
		// Image indexes may need to be modified depending on the images specific to 
		// the snapin.
		memset(&m_scopeDataItem, 0, sizeof(SCOPEDATAITEM));
		m_scopeDataItem.mask = SDI_STR | SDI_IMAGE | SDI_OPENIMAGE | SDI_PARAM;
		m_scopeDataItem.displayname = MMC_CALLBACK;
		m_scopeDataItem.lParam = (LPARAM) this;
		memset(&m_resultDataItem, 0, sizeof(RESULTDATAITEM));
		m_resultDataItem.mask = RDI_STR | RDI_IMAGE | RDI_PARAM;
		m_resultDataItem.str = MMC_CALLBACK;
		m_resultDataItem.nImage = pItemData->m_uImage;	    
		m_resultDataItem.lParam = (LPARAM) this;
        m_pVirusPage = NULL;
        m_pGenPage = NULL;
        m_pUserPage = NULL;
	}

	~CQSConsoleResultData()
	{
    }

	STDMETHOD(CreatePropertyPages)(LPPROPERTYSHEETCALLBACK lpProvider,
		long handle, 
		IUnknown* pUnk,
		DATA_OBJECT_TYPES type);

	STDMETHOD(QueryPagesFor)(DATA_OBJECT_TYPES type)
	{
		if (type == CCT_SCOPE || type == CCT_RESULT)
			return S_OK;
		return S_FALSE;
	}

	STDMETHOD(GetScopePaneInfo)(SCOPEDATAITEM *pScopeDataItem);

	STDMETHOD(GetResultPaneInfo)(RESULTDATAITEM *pResultDataItem);
    STDMETHOD(GetDataObject)(IDataObject** pDataObj, DATA_OBJECT_TYPES type);

	STDMETHOD(Notify)( MMC_NOTIFY_TYPE event,
		long arg,
		long param,
		IComponentData* pComponentData,
		IComponent* pComponent,
		DATA_OBJECT_TYPES type);

	LPOLESTR GetResultPaneColInfo(int nCol);

    CItemData* GetItemData() {return m_pItemData;}

    BOOL IsPropPageDisplayed() { return m_bPropPage; }
    void CancelPropPage() { m_bPropPage = FALSE; }

    STDMETHOD(ControlbarNotify)(IControlbar *pControlbar,
        IExtendControlbar *pExtendControlbar,
		CSimpleMap<UINT, IUnknown*>* pToolbarMap,
		MMC_NOTIFY_TYPE event,
        long arg, 
		long param,
		CSnapInObjectRootBase* pObj,
		DATA_OBJECT_TYPES type);


    // 
    // Multiselect methods.
    // 
    STDMETHOD(MultiSelAddMenuItems)( LPDATAOBJECT pDataObject,
                                     LPCONTEXTMENUCALLBACK piCallback,
                                     long *pInsertionAllowed );

	STDMETHOD(MultiSelNotify)( MMC_NOTIFY_TYPE event,
		long arg,
		long param,
		IComponentData* pComponentData,
		IComponent* pComponent,
		DATA_OBJECT_TYPES type,
        LPDATAOBJECT pData);

    STDMETHOD(MultiSelCommand)( long lCommandID,
                                LPDATAOBJECT pDataObject);

    STDMETHOD(AddMenuItems)(LPCONTEXTMENUCALLBACK piCallback,
        long  *pInsertionAllowed,
		DATA_OBJECT_TYPES type);


	BOOL UpdateToolbarButton(UINT id, BYTE fsState);


    void DeletePropPages();
    
    CQSConsoleData* GetRootNode() { return m_pRootNode; } 


    DWORD GetItemID() { return m_pItemData->m_dwItemID; }
// 
// Notification messages
// 
private:
    HRESULT OnNotifyAddImages( IImageList* pImageList );
    HRESULT OnNotifySelect( IComponent* pComponent, LONG arg, BOOL bMultiSel = FALSE );
    HRESULT OnNotifyDelete( IComponent* pComponent, LPDATAOBJECT pData = NULL );

    HRESULT AddPages(LPPROPERTYSHEETCALLBACK lpProvider, long handle );

    HRESULT OnRestoreItem( bool& bHandled, CSnapInObjectRootBase * pObj );
    HRESULT OnRepairItem( bool& bHandled, CSnapInObjectRootBase * pObj );
    HRESULT OnSubmitItem( bool& bHandled, CSnapInObjectRootBase * pObj );

    HRESULT BrowseForSaveLocation();

    HRESULT RestoreItems( CItemData** pItems, DWORD dwNumItems, BOOL bRepair = FALSE );
    HRESULT SubmitItemsToSARC( CItemData** pItems, DWORD dwNumItems );
    HRESULT RemoveItemFromServer( CItemData* pItem );
    HRESULT OnAccessDenied( CItemData* pItem ); 

// 
// Private data
// 
private:
    // 
    // Pointer to our root node.
    // 
    CQSConsoleData*         m_pRootNode;    

    // 
    // Item data for this object
    // 
    CItemData*              m_pItemData;

    // 
    // Flag indicating presence of a property sheet 
    // this item.
    // 
    BOOL                    m_bPropPage;

    CGeneralItemPage*       m_pGenPage;
    CUserItemPage*          m_pUserPage;
    CVirusPage*             m_pVirusPage;


    IComponent*             m_pComponent;

};


#endif