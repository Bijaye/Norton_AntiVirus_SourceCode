/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*
    ScopeExtData.h

    Extend the scope menu when running as an extension to Q Console.

    Written by: Jim Hill
*/


#ifndef SCOPE_EXTENSION_H_DEFINED 
#define SCOPE_EXTENSION_H_DEFINED


#include "stdafx.h"
#include "resource.h"
//#include "ServerConfigData.h"
#include "AvisServerConfig2.h"
// #include "qscon.h"


extern int  g_ScopeExtDataClassTotalCount;
// Handle to main console window.
extern HWND g_hMainWnd;

class CScopeExtData : public CSnapInItemImpl<CScopeExtData, TRUE>
{
public:
	static const GUID*    m_NODETYPE;
	static const OLECHAR* m_SZNODETYPE;
	static const OLECHAR* m_SZDISPLAY_NAME;
	static const CLSID*   m_SNAPIN_CLASSID;

	BEGIN_SNAPINCOMMAND_MAP(CScopeExtData, FALSE)
	END_SNAPINCOMMAND_MAP()

	//SNAPINMENUID(IDR_SNAPPER2_MENU)

	BEGIN_SNAPINTOOLBARID_MAP(CScopeExtData)
	END_SNAPINTOOLBARID_MAP()

	CScopeExtData()
	{
        ++g_ScopeExtDataClassTotalCount;
        m_iExtDataObjectID = GetNextObjectID();
        fWidePrintString("CScopeExtData Constructor called. ObjectID= %d TotalCount= %d", 
                                       m_iExtDataObjectID, g_ScopeExtDataClassTotalCount );
		memset(&m_scopeDataItem, 0, sizeof(SCOPEDATAITEM));
		memset(&m_resultDataItem, 0, sizeof(RESULTDATAITEM));
        g_hMainWnd = GetActiveWindow();
        m_bPropPage                  = 0;
        m_pSConfigData               = NULL;
        m_pAvisServerConfig          = NULL;
        m_pAvisServerDefPolicyConfig = NULL;
        m_pAvisServerConfig2         = NULL;
        //m_pAServerFirewallConfig2    = NULL;
        m_pAvisCustomerInfo          = NULL;
        m_pServerGeneralError        = NULL;
        m_pAlertingSetConfig         = NULL;

         

        // CAUSE IT TO READ FROM LOCAL REGISTRY   1/4/00
        SaveQServerVersion(m_iExtDataObjectID);
       	fWidePrintString("CScopeExtData::CScopeExtData calling SaveQServerVersion(), ObjectID= %d ", m_iExtDataObjectID );

	}


	~CScopeExtData()
	{
        --g_ScopeExtDataClassTotalCount;
        fWidePrintString("CScopeExtData Destructor called. ObjectID= %d TotalCount= %d", 
                                       m_iExtDataObjectID, g_ScopeExtDataClassTotalCount );
	}

	STDMETHOD(CreatePropertyPages)(LPPROPERTYSHEETCALLBACK lpProvider,
		long handle, 
		IUnknown* pUnk,
		DATA_OBJECT_TYPES type);

	STDMETHOD(QueryPagesFor)(DATA_OBJECT_TYPES type)
	{
		if(type == CCT_SCOPE)
			return S_OK;
		return S_FALSE;
	}

	IDataObject* m_pDataObject;
	virtual void InitDataClass(IDataObject* pDataObject, CSnapInItem* pDefault)
	{
        ULONG    ulVersion = 0;
        HRESULT  hr = 0;
		m_pDataObject = pDataObject;
		// The default code stores off the pointer to the Dataobject the class is wrapping
		// at the time. 
		// Alternatively you could convert the dataobject to the internal format
		// it represents and store that information

        // 12/21/99
		// terrym sample on how to access the new interface to the qconsole.
// 		CComQIPtr<IQCScopeItem> m_pQCScopeItem( m_pDataObject );
// 		m_pQCScopeItem->GetQSVersion(&ulVersion); 

        
        fWidePrintString("CScopeExtData::InitDataClass called  ObjectID= %d", m_iExtDataObjectID);
	}

	CSnapInItem* GetExtNodeObject(IDataObject* pDataObject, CSnapInItem* pDefault)
	{
		// Modify to return a different CSnapInItem* pointer.
    	//fWidePrintString("CScopeExtData::GetExtNodeObject  called  ObjectID= %d", m_iExtDataObjectID);
		return pDefault;
	}



public:
// #if ! (USE_AVIS_SERVER)
//     IQCScopeItem*        m_pQCScopeItem;
// #endif
//     ULONG                m_ulVersion;
    int                  m_iExtDataObjectID;
    // Pointer to console item for this node.
    // CComPtr< IConsole > m_pConsole;


private:
    // Is there a property page open for this node?
    BOOL                         m_bPropPage;

    CAvisServerConfig2*          m_pAvisServerConfig2;
    CAServerFirewallConfig2*     m_pAServerFirewallConfig2;
    CAvisServerConfig*           m_pAvisServerConfig;
    CAvisServerDefPolicyConfig*  m_pAvisServerDefPolicyConfig;
    CInstallDefinitions*         m_pInstallDefinitions;
    CCustomerInfo*               m_pAvisCustomerInfo;
    CServerGeneralError*         m_pServerGeneralError;
    CAlertingSetConfig*          m_pAlertingSetConfig;


    // CServerConfigData  Object
    CServerConfigData*   m_pSConfigData;



};








#endif  // SCOPE_EXTENSION_H_DEFINED


