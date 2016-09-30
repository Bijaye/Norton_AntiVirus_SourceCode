// 
// QSConsoleData.h
// 

#ifndef _QSConsoleData_H_
#define _QSConsoleData_H_

#include "resource.h"
#include "QSConsoleComponent.h"
#import "qserver.tlb" no_namespace
#include "dcomobject.h" 

// 
// Foreward declarations.
// 
class CQSConsole;

class CQSConsoleData : public CSnapInItemImpl<CQSConsoleData>
{
public:
	static const GUID* m_NODETYPE;
	static const OLECHAR* m_SZNODETYPE;
	static const OLECHAR* m_SZDISPLAY_NAME;
	static const CLSID* m_SNAPIN_CLASSID;

	BEGIN_SNAPINCOMMAND_MAP(CQSConsoleData, FALSE)
        SNAPINCOMMAND_ENTRY(ID_TOP_ATTACHTOSERVER, OnAttachServer )
	END_SNAPINCOMMAND_MAP()

	SNAPINMENUID(IDR_SCOPE_MENU)

	BEGIN_SNAPINTOOLBARID_MAP(CQSConsoleData)
		// Create toolbar resources with button dimensions 16x16 
		// and add an entry to the MAP. You can add multiple toolbars
		// SNAPINTOOLBARID_ENTRY(Toolbar ID)
        SNAPINTOOLBARID_ENTRY( IDR_SCOPE_TOOLBAR )
	END_SNAPINTOOLBARID_MAP()

	CQSConsoleData()
	{
		// Image indexes may need to be modified depending on the images specific to 
		// the snapin.
		memset(&m_scopeDataItem, 0, sizeof(SCOPEDATAITEM));
		m_scopeDataItem.mask = SDI_STR | SDI_IMAGE | SDI_OPENIMAGE | SDI_PARAM | SDI_CHILDREN;
		m_scopeDataItem.displayname = MMC_CALLBACK;
		m_scopeDataItem.nImage = 0; 		// May need modification
		m_scopeDataItem.nOpenImage = 0; 	// May need modification
		m_scopeDataItem.lParam = (LPARAM) this;
		memset(&m_resultDataItem, 0, sizeof(RESULTDATAITEM));
		m_resultDataItem.mask = RDI_STR | RDI_IMAGE | RDI_PARAM;
		m_resultDataItem.str = MMC_CALLBACK;
		m_resultDataItem.nImage = 0;		// May need modification
		m_resultDataItem.lParam = (LPARAM) this;
        m_pComponentData = NULL;
        m_bPropPage = FALSE;
        m_bAttached = FALSE;
       
	}

	~CQSConsoleData()
	{
        DestroyData();
    }

    void InitDataClass(IDataObject* pDataObject, CSnapInItem* pDefault)
        {

        }

	STDMETHOD(CreatePropertyPages)(LPPROPERTYSHEETCALLBACK lpProvider,
		long handle, 
		IUnknown* pUnk,
		DATA_OBJECT_TYPES type);

	STDMETHOD(QueryPagesFor)(DATA_OBJECT_TYPES type)
	{
		if (type == CCT_SCOPE || type == CCT_SNAPIN_MANAGER)
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

    STDMETHOD(AddMenuItems)(LPCONTEXTMENUCALLBACK piCallback,
        long *pInsertionAllowed,
		DATA_OBJECT_TYPES type);

    STDMETHOD(GetResultViewType)(LPOLESTR *ppViewType,
        long *pViewOptions)
    {
        *pViewOptions = MMC_VIEW_OPTIONS_NOLISTVIEWS;
        return S_FALSE;
    }

	LPOLESTR GetResultPaneColInfo(int nCol);

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



   	STDMETHOD (FillData)(CLIPFORMAT cf, LPSTREAM pStream);

    
    HRESULT RemoveResultItem( LPVOID pItem );
    
    CQSConsole* GetComponentData() {return m_pComponentData;}
    void UpdateCaption( IConsole2* pc, BOOL bSel = TRUE );
    HSCOPEITEM GetNodeID() {return m_hStaticNodeID; }

    CString& GetServerName() { return m_sServer; }
    void SetServerName( LPCTSTR szServerName ); 
    CString& GetDomainName() { return m_sDomain; }
    void SetDomainName( LPCTSTR szDomainName );
    CString& GetUserName() { return m_sUser; }  
    void SetUserName( LPCTSTR psz ); 
    CString& GetPassword() { return m_sPassword; }
    void SetPassword( LPCTSTR psz ); 

    void SetAttached( BOOL bAttached = TRUE ) { m_bAttached = bAttached; }
    BOOL IsAttached() { return m_bAttached; }

    LPCTSTR GetDisplayName() { return m_sDisplayName; };
    void SetDisplayServerName( LPCTSTR szServer );

    HRESULT RefreshData( IConsole* pConsole );
    void DestroyData();
    HRESULT OnNotifyRefresh( IConsole* pConsole, IComponent* pComponent = NULL );



public:
    // 
    // Handle to main console window.
    // 
    HWND            m_hMainWnd;

    // 
    // List of result pane items.
    // 
    CMapPtrToPtr    m_Data;

    // 
    // Where restored files go
    // 
    CString         m_sRestoreLocation;

    // 
    // Pointer to console item for this node.
    // 
    CComPtr< IConsole2 > m_pConsole;

    // 
    // Pointer to component data for this node.
    // 
    CQSConsole*     m_pComponentData;

    // 
    // Pointer to our DCOM server object.
    // 
    DCOM_Object( QuarantineServer ) m_pQServer;

    // 
    // Server we are attatched to.
    // 
    CString         m_sServer;
    
    // 
    // Domain we are attached to
    // 
    CString         m_sDomain;

    // 
    // User name
    //
    CString         m_sUser;

    // 
    // Password 
    // 
    CString         m_sPassword;

    // 
    // Q: Are we attached to a server at this time?
    // 
    BOOL            m_bAttached;

    // 
    // Display name for status bar.
    // 
    CString         m_sDisplayName;
    
private:
    HRESULT AddServerPropPage( LPPROPERTYSHEETCALLBACK lpProvider, long handle );
    HRESULT AddServerWizard( LPPROPERTYSHEETCALLBACK lpProvider, long handle, IUnknown* pUnk );
    HRESULT AttachToServer( IConsole* pConsole,BOOL bReAttach = FALSE );
        
    // 
    // Notification messages
    // 
    HRESULT OnNotifyShow( IComponent* pComponent, LONG arg, BOOL bAddColumns = TRUE );
    HRESULT OnNotifyAddImages( IImageList* pImageList );
    HRESULT OnNotifySelect( IComponent* pComponent, LONG arg);
    HRESULT OnNotifyViewChange( IComponent* pComponent, long lData, long lHint );
    

    // 
    // Command handlers
    // 
    HRESULT OnAttachServer( bool& bHandled, CSnapInObjectRootBase * pObj );

private:
    // 
    // Description bar text.
    // 
    CString         m_sDescBarText;   
    
    // 
    // Static node ID for this node.
    // 
    HSCOPEITEM      m_hStaticNodeID;

    // 
    // Description column text
    //
    CComBSTR        m_bstrCol[4];

    // 
    // Is there a property page open for this node?
    // 
    BOOL            m_bPropPage;
};


#endif 