/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// 
// QSConsoleData.h
// 

#ifndef _QSConsoleData_H_
#define _QSConsoleData_H_

#include "resource.h"
#include "QSConsoleComponent.h"
#import "qserver.tlb" no_namespace
#include "dcomobject.h" 

#define READ_NO_SAMPLES	10
#define INVALID_THREAD 0xFFFFFFFF
#define CONNECTION_BROKEN -1

class QServerSAVInfo
{

public:
    // 
    // Pointer to our DCOM Qserversavinfo interface object.
    // 
    DCOM_Object( QServerSavInfo ) m_pQServerSavInfo;

};

class QServerInterfaces
{

public:
    // 
    // Pointer to our DCOM server object.
    // 
    DCOM_Object( QuarantineServer ) m_pQServer;

    // 
    // Pointer to our DCOM QserverII interface object.
    // 
    DCOM_Object( QuarantineServerII ) m_pQServerII;

    // 
    // Pointer to our Avis Config interface.  We need this now to export for extensions. (security issues)
    // 
    DCOM_Object( AvisConfig ) m_pAvisConfig;


};
// 
// Foreward declarations.
// 
class CQSConsole;
class CItemData;
class CManageWnd;
class CSampleRead;
class CQSConsoleData : public CSnapInItemImpl<CQSConsoleData>
{
public:
	static const GUID* m_NODETYPE;
	static const TCHAR* m_SZNODETYPE;
	static const TCHAR* m_SZDISPLAY_NAME;
	static const CLSID* m_SNAPIN_CLASSID;

	BEGIN_SNAPINCOMMAND_MAP(CQSConsoleData, FALSE)
        SNAPINCOMMAND_ENTRY(ID_TOP_ATTACHTOSERVER, OnAttachServer )
        SNAPINCOMMAND_ENTRY(IDM_VIEW_AMS_LOG, OnViewAMSLog )
        SNAPINCOMMAND_ENTRY(IDM_CONFIGURE_AMS, OnConfigureAMS )
		SNAPINCOMMAND_ENTRY(IDM_DISPLAY_VIRUS_LIST, OnDisplayVList )
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
		m_resultDataItem.nImage = MMC_IMAGECALLBACK ;		// May need modification
		m_resultDataItem.lParam = (LPARAM) this;
        m_pComponentData = NULL;
        m_bPropPage = FALSE;
		m_bFullRefesh = FALSE;
        m_bAttached = FALSE;
		m_dwQSVersion = 0;
       
		m_hwndManage = NULL;
		m_pHiddenWnd = NULL;
		m_dwRefreshInProgress=FALSE;
		m_dwThread = INVALID_THREAD;
		m_pSRead = NULL;
		m_pListofDataList = NULL;
        InitializeCriticalSection( &csThread );
		m_cSAVInfo = NULL;
		m_cQSInterfaces = new QServerInterfaces;

	}

	~CQSConsoleData()
	{
        DestroyData(&m_Data); // Destroy the result pane item list.
		DeleteCriticalSection(&csThread );
		if (m_cSAVInfo != NULL)
		{
			delete m_cSAVInfo;
			m_cSAVInfo = NULL;
		}
		if (m_cQSInterfaces != NULL)
		{
			delete m_cQSInterfaces;
			m_cSAVInfo = NULL;
		}
    }

    void InitDataClass(IDataObject* pDataObject, CSnapInItem* pDefault)
        {

        }

    STDMETHOD(Command)(long lCommandID,
		CSnapInObjectRootBase* pObj,
		DATA_OBJECT_TYPES type)
	{
    // 
    // Call base class implememtation.
    //     
		HRESULT hr = CSnapInItemImpl< CQSConsoleData >::Command(lCommandID,
														pObj,
														type);
		if (m_pConsole)
		{
			UpdateCaption(m_pConsole);
		}
		
		return hr;
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

	BOOL UpdateToolbarButton(UINT id, BYTE fsState); // added 5-31-00 tm

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

	HRESULT ReAttach(void);
    void SetAttached( BOOL bAttached = TRUE ) { m_bAttached = bAttached; }
    int GetAttached( void ) { return m_bAttached; }
    BOOL IsAttached() { return m_bAttached; }
	void WriteQServerVersion(long dwVersion);
    LPCTSTR GetDisplayName() { return m_sDisplayName; };
    void SetDisplayServerName( LPCTSTR szServer );
	HRESULT DeleteColumnHeaders( IComponent* pComponent);
	HRESULT InsertColumnHeaders( IComponent* pComponent);

    HRESULT RefreshData( IConsole* pConsole );
    void DestroyData(CMapPtrToPtr *DataList);
    HRESULT OnNotifyRefresh( IConsole* pConsole, IComponent* pComponent = NULL );
	HRESULT InitResultsPane (IComponent* pComponent);
	HRESULT ReadQListFromQServer(IConsole2 *pc);
    HRESULT OnPropertyChange( IConsole* pConsole, IComponent* pComponent = NULL );


	//
	// hidden window functions
	//
	BOOL	CreateManageWnd(CQSConsoleComponent* pCQSConsoleComponent);
	HWND	m_hwndManage;
	CManageWnd* m_pHiddenWnd;
	void ChangeIcon(IConsole* pConsole, DWORD dwIconIndex );
    static  UINT    m_msgDisplayProgressText;
    static  UINT    m_msgDisplayRecordText;
    static  UINT    m_msgRefresh;
	static  UINT    m_msgReadFinished;

	BOOL NeedAttention(void);
	CString GetStatusSampleCountText(void){return m_sStatusBarSampleCountText;}
	void SetStatusSampleCountText(CString &sText){m_sStatusBarSampleCountText = sText;}
	void SetStatusSampleCountText(ULONG ulNoSamples)
		{
			m_sStatusBarSampleCountText.Format(IDS_STATBAR_SAMPLE_COUNT_FORMAT,ulNoSamples); 
		}
	void SetStatus(void)
		{
			USES_CONVERSION;
			CString sStatus = m_sStatusBarVirusDefDate + m_sStatusBarSampleCountText ;
			m_pConsole->SetStatusText(T2OLE(sStatus.GetBuffer(0)));
		}


	CString GetStatusBarVirusDefDateText(void){return m_sStatusBarVirusDefDate;}
	void SetStatusBarVirusDefDateText(CString &sText){m_sStatusBarVirusDefDate= sText;}

	CString GetStatusBarProgress(void){return m_sStatusBarProgress;}
	void SetStatusBarProgress(CString& sText){m_sStatusBarProgress= sText;}

	HRESULT SyncDataLists(CMapPtrToPtr *pDataList);
    HRESULT UpdateResultItems ( IConsole* pConsole,IComponent* pComponent);
	HRESULT PurgeMarkedItems( IConsole* pConsole,IComponent* pComponent  );
    HRESULT OnNotifyAddImages( IImageList* pImageList );
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
    // Current Qserver Def Version
    // 
    CString         m_sCurrentDefVersion;
    
	// 
    // Current Qserver Def Date
    // 
    CString         m_sCurrentDefDate;

    // 
    // Version of the QS we are attached to.
    // 
    DWORD m_dwQSVersion;

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
    int            m_bAttached;

    // 
    // Display name for status bar.
    // 
    CString         m_sDisplayName;
    
	LONG		m_dwRefreshInProgress;

    // 
    // Handle to thread if background thread exists
    // 
    DWORD           m_dwThread;

	CSampleRead		*m_pSRead ;

    CPtrList             *m_pListofDataList;
    
	QServerSAVInfo		 *m_cSAVInfo;

	QServerInterfaces	 *m_cQSInterfaces;
private:
    HRESULT AddServerPropPage( LPPROPERTYSHEETCALLBACK lpProvider, long handle );
    HRESULT AddServerWizard( LPPROPERTYSHEETCALLBACK lpProvider, long handle, IUnknown* pUnk );
    HRESULT AttachToServer( IConsole* pConsole,BOOL bReAttach = FALSE );
	HRESULT GetSAVInfo(void);
        
    // 
    // Notification messages
    // 
    HRESULT OnNotifyShow( IComponent* pComponent, LONG arg, BOOL bAddColumns = TRUE );
    HRESULT OnNotifySelect( IComponent* pComponent, LONG arg);
    HRESULT OnNotifyViewChange( IComponent* pComponent, long lData, long lHint );
    

    // 
    // Command handlers
    // 
    HRESULT OnAttachServer( bool& bHandled, CSnapInObjectRootBase * pObj );
    HRESULT OnViewAMSLog( bool& bHandled, CSnapInObjectRootBase * pObj );
    HRESULT OnConfigureAMS ( bool& bHandled, CSnapInObjectRootBase * pObj );
    HRESULT OnDisplayVList ( bool& bHandled, CSnapInObjectRootBase * pObj );
	void GetAMSServer(CString& s);

	// 
	// data list functions
	//
	HRESULT ClearState(CMapPtrToPtr *DataList );
	HRESULT FindItemByFileID(CMapPtrToPtr *DataList, CItemData *pItemData, CItemData **pItemDataFound );


private:
    // 
    // Description bar text.
    // 
    CString         m_sDescBarText;   
    
    
	// 
    // Status bar Samples in Quarantine count text.
    // 
    CString         m_sStatusBarSampleCountText;

	// 
    // Status bar Virus defination date text.
    // 
    CString         m_sStatusBarVirusDefDate;

	// 
    // Status bar Virus defination date text.
    // 
    CString         m_sStatusBarProgress;

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
	//
	// String for ams server
	//
	CString m_sAMSServer;
	//
	// String for ams Product name
	//
	CString m_sAMSProductName;

	BOOL		m_bFullRefesh;

	// critical section for starting the refresh thread.
	CRITICAL_SECTION csThread;

};


#endif 