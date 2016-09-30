/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#if !defined(AVIS_SERVER_CONFIG_DATA_INCLUDED)
    #define AVIS_SERVER_CONFIG_DATA_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <comdef.h>
#include <comip.h>
#include <tchar.h>
#include "qscon.h"



//#define  USE_AVIS_SERVER   0


class CHiddenWndMmcThread;


// BYTE TABLE FOR SETTING INDIVIDUAL VARIABLES DIRTY
//
// CAvisServerConfig2  "Web Communication" 
#define FIELD_CHANGE_GATEWAY_ADDRESS                 0
#define FIELD_CHANGE_GATEWAY_PORT                    3 
#define FIELD_CHANGE_SECURE_DEF_DOWNLOAD_CHECK       19
#define FIELD_CHANGE_SECURE_SAMPLE_SUBMISSION_CHECK  20
#define FIELD_CHANGE_SECURE_STATUS_QUERY_CHECK       21
#define FIELD_CHANGE_SSL_PORT                        22
#define FIELD_CHANGE_RETRY_LIMIT                     23
#define FIELD_CHANGE_RETRY_INTERVAL                  24
#define FIELD_CHANGE_TRANSACTION_TIMEOUT             25
#define FIELD_CHANGE_SECURE_IGNORE_HOST_NAME         27

// CAServerFirewallConfig2  "AVIS Firewall"
#define FIELD_CHANGE_PROXY_FIREWALL                  1
#define FIELD_CHANGE_FIREWALL_PORT                   4 
#define FIELD_CHANGE_FIREWALL_USERNAME               17
#define FIELD_CHANGE_FIREWALL_PASSWORD               18

// CCustomerInfo                                  
#define FIELD_CHANGE_COMPANY_NAME                    12
#define FIELD_CHANGE_CONTACT_NAME                    13
#define FIELD_CHANGE_CONTACT_PHONE                   14
#define FIELD_CHANGE_CONTACT_EMAIL                   15
#define FIELD_CHANGE_CUSTOMER_ACCOUNT                16

// CAvisServerConfig    "Sample Policy"                           
#define FIELD_CHANGE_COMPRESS_USER_DATA              32 
#define FIELD_CHANGE_SCRAMBLE_USER_DATA              33 
#define FIELD_CHANGE_STRIP_USER_DATA                 7 
#define FIELD_CHANGE_QUEUECHECK_INTERVAL             8  
#define FIELD_CHANGE_MAX_PENDINGSAMPLES              10 
#define FIELD_CHANGE_INITIAL_SUBMISSIONPRIORITY      11
#define FIELD_CHANGE_STATUS_INTERVAL                 2

// CAvisServerDefPolicyConfig   "Definition Policy"  
//#define FIELD_CHANGE_DEFINITION_CHECK_INTERVAL       9  
#define FIELD_CHANGE_DEFLIBRARY_FOLDER               5 
#define FIELD_CHANGE_DEFS_HEURISTIC_LEVEL            26
#define FIELD_CHANGE_DEFS_UNPACK_TIMEOUT             31
#define FIELD_CHANGE_DEFS_BLESSED_INTERVAL           28
#define FIELD_CHANGE_DEFS_NEEDED_INTERVAL            29
#define FIELD_CHANGE_DEFS_DEFS_PRUNE                 30


// DEFINITIONS INSTALL PAGE
//#define FIELD_CHANGE_DEFINITION_DELIVERY_PRIORITY    6 
#define FIELD_CHANGE_DEF_DELIVERY_PRIORITY           6 
#define FIELD_CHANGE_DEF_DELIVERY_TIMEOUT            34
#define FIELD_CHANGE_DEF_DELIVERY_INTERVAL           35
#define FIELD_CHANGE_DEF_CHECK_BLESSED_BROADCAST     36
#define FIELD_CHANGE_DEF_CHECK_UNBLESSED_BROADCAST   37
#define FIELD_CHANGE_DEF_CHECK_UNBLESSED_NARROWCAST  38
#define FIELD_CHANGE_DEF_CHECK_UNBLESSED_POINTCAST   39
#define FIELD_CHANGE_DEF_BLESSED_TARGETS             40
#define FIELD_CHANGE_DEF_UNBLESSED_TARGETS           41  
#define FIELD_CHANGE_DEF_SECURE_USERNAME             46
#define FIELD_CHANGE_DEF_SECURE_PASSWORD             47

// ALERTING PAGE
#define FIELD_CHANGE_ENABLE_ALERTS                   42
#define FIELD_CHANGE_AMS_SERVER_ADDRESS              43
#define FIELD_CHANGE_ALERTING_INTERVAL               44
#define FIELD_CHANGE_ENABLE_NT_EVENT_LOG             45


// END
#define MAX_FIELD_CHANGE_VALUE                       48





// TEMP 12/15/99
//#define FIELD_CHANGE_AUTODELIVER_DEFINITIONS         FIELD_CHANGE_DEFINITION_DELIVERY_PRIORITY


class CServerConfigData  
{
public:
	CServerConfigData();
	~CServerConfigData();

    // HIDDEN WINDOW STUFF
    HWND     m_hwndManage;
    //void*    m_pHiddenWndMmcThread;
    //void*    m_pWnd;
    CHiddenWndMmcThread*  m_pWnd;
    BOOL                  m_bWriteDataRoutineDone;
    BOOL     CreateHiddenWndThread(CServerConfigData* pSConfigData);
    BOOL     ReleaseHiddenWndThread();

    HRESULT  WriteConfigDataToServer();
    HRESULT  WorkerWriteConfigDataToServer();



    HRESULT Initialize( IDataObject* pDataObject, TCHAR *lpszServerName, DWORD iExtDataObjectID = 0 )
    {
        HRESULT  hr = S_OK;
        m_pDataObject = pDataObject;
		BSTR bstrTemp = NULL;
		USES_CONVERSION;
        // 12/21/99
		// terrym sample on how to access the new interface to the qconsole.
		CComQIPtr<IQCScopeItem> pQCScopeItem( m_pDataObject );
        m_pQCScopeItem = pQCScopeItem;
		m_pQCScopeItem->GetQSVersion(&m_ulQServerVersion); 

		// 9-24-01 terrym added to get in qs def info
		m_pQCScopeItem->GetQSDefVersion(&bstrTemp); 
		if (bstrTemp)
		{
			m_sQSDefVersion = OLE2T(bstrTemp);
			SysFreeString( bstrTemp);
			bstrTemp = NULL;
		}
		m_pQCScopeItem->GetQSDefDate(&bstrTemp); 
		if (bstrTemp)
		{
			m_sQSDefDate = OLE2T(bstrTemp);
			SysFreeString( bstrTemp);
			bstrTemp = NULL;
		}

        // CREAT THE HIDDEN WINDOW TO HANDLE WRITING TO REMOTE REGISTRY
        m_hwndManage            = NULL;
        m_pWnd                  = NULL;
        m_bWriteDataRoutineDone = FALSE;
//        m_pHiddenWndMmcThread = NULL;
        CreateHiddenWndThread( this );

        hr = GetScopeItemServerName( lpszServerName ); // m_sServer = 
        m_iExtDataObjectID = iExtDataObjectID;         // HAS THE OBJECTID from caller
    	fWidePrintString("CServerConfigData constructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
        return(hr);
    }

    HRESULT  GetScopeItemServerName( TCHAR *pszServerName );

    HRESULT  ReadConfigDataFromServer();
    HRESULT  GetConfigInterface( BOOL bReAttach = FALSE );
    HRESULT  DetachAndReleaseConfigInterface();

    HRESULT  ClearServerDataMembers();
    BOOL     ContainsValidData() {return m_bDataReadSuccessfully;};
    BOOL     DataWrittenSuccessfully() {return m_bDataWrittenSuccessfully;};
    BOOL     IsDirty() {return m_bIsDirty;};
    BOOL     SetDirty(BOOL bSetDirty) {m_bIsDirty=bSetDirty; return m_bIsDirty;};
    HRESULT  ReleaseServerConfigObject();
    DWORD    Release(); 
    BOOL     IsValidFolderPath(CString* lpsFolder);
    //BOOL     IsValidAddressAndPort(CString* lpsAddrString, BOOL bIsHttpAddr);
    BOOL     ValidateIpAddress( LPTSTR lpsAddrString );

    DWORD    SetFieldDirty(DWORD dwFieldID);
    DWORD    IsFieldDirty(DWORD dwFieldID);
    DWORD    SetFieldClean(DWORD dwFieldID);

//  HRESULT  SetValue( _bstr_t bstrFieldName, const _variant_t & v );
    HRESULT  SetValue( _bstr_t bstrFieldName, VARIANT * v, BSTR pKeyName = NULL );
    HRESULT  GetValue( _bstr_t bstrFieldName, VARIANT * v, BSTR pKeyName = NULL);
    HRESULT  DetachInterface( ); 
    HRESULT  ReleaseInterface( ); 
    HRESULT  GetNewConfigInterface( BOOL bReAttach = FALSE );


    // ParseEventData.cpp
    BOOL     ReadAllEventConfigData();
    BOOL     WriteAllEventConfigData();
    void     SendTestEvent(); 



// DATA 
public:
    int        m_iExtDataObjectID;
    BOOL       m_bIsDirty;
    LPVOID     m_pSnapin;
    long       m_lParam;
    LPSTREAM   m_pServerStream;
    DWORD      m_lock;
    BOOL       m_bDataReadSuccessfully;
    BOOL       m_bDataWrittenSuccessfully;


public:
// Server Config Data 
    BYTE       szDirtyFields[100];
    int        m_iConfigChangeCounter;

// "General Attention Message"
    CString    m_sGeneralAttentionTokens;

// Server Config Data for CAvisServerConfig2   "Web Communication"
	CString	   m_szGatewayAddress;
	long	   m_iGatewayPort;
	BOOL	   m_bCheckSecureIgnoreHostname;
	BOOL	   m_bCheckSecureDefDownload;
	BOOL	   m_bCheckSecureSampleSubmission;
	BOOL	   m_bSecureStatusQuery;
	long	   m_iSSLPort;     
	long	   m_iRetryLimit;
    int        m_iRetryInterval;
	long	   m_iTransactionTimeout;


// Server Config Data for CAServerFirewallConfig2  "AVIS Firewall"
    CString    m_szProxyFirewall;
	long	   m_iFirewallPort;
    CString    m_sFirewallUsername;
    CString    m_sFirewallPassword;

// Server Config Data for CAvisServerConfig      "Sample Policy"
    DWORD      m_dwQueueCheckInterval;       
    DWORD      m_dwMaxPendingSamples;        
    DWORD      m_dwInitialSubmissionPriority;
	int		   m_iStatusInterval;
	BOOL	   m_bCompressUserData;
	BOOL	   m_bScrambleUserData;
    BOOL       m_bStripUserData;             

// Server Config Data for CAvisServerDefPolicyConfig   "Definition Policy"
    CString    m_szDefLibraryPath;           
	int		   m_iDefHeuristicLevel;
	int		   m_iUnpackTimeout;
	int		   m_iDefsBlessedInterval;
	int	 	   m_iDefsNeededInterval;
	int		   m_iPruneDefs;
    BOOL       m_bDefinitionActiveBlessed;  // ?
	CString	   m_sActiveDefsSeqNum;
	DWORD	   m_dwActiveDefsSeqNum;          // ?
	CString	   m_sCurrentBlessedDefsSeqNum;

// Server Config Data for Def Install Page
//  DWORD      m_dwDefinitionCheckInterval;
//  BOOL       m_bAutoDeliveryOfDefinitions; 
//  DWORD      m_iDefinitionDeliveryPriority;
	BOOL	   m_bCheckBlessedBroadcast;
	BOOL	   m_bCheckUnblessedBroadcast;
	BOOL	   m_bCheckUnblessedNarrowCast;
	BOOL	   m_bCheckUnblessedPointCast;
	int		   m_iDefDeliveryTimeout;
	int		   m_iDefDeliveryPriority;
	int		   m_iDefDeliveryInterval;
    CString	   m_sDefBlessedTargets;
    CString	   m_sDefUnblessedTargets;
    CString    m_sDefSecureUsername;
    CString    m_sDefSecurePassword;


// Server Config Data for CCustomerInfo
	CString	   m_szCompanyName;
	CString	   m_szContactEmail;
	CString	   m_szContactName;
	CString	   m_szContactPhone;
	CString	   m_szCustomerAccount;
    BOOL       m_bAllCustomerInfoEntered; // ???


// Server Config Data for Alerting Page
    CString    m_sTrackingNumber;
	BOOL	   m_bCheckEnableAlerts;
	CString	   m_sAmsServerAddress;
    long       m_iAlertCheckInterval;
    BOOL       m_bSendTestEvent;
    BOOL       m_bCheckNtEventLog;
    CString    m_sLastAlertString;
    CString    m_sLastAlertDate;


    // SERVER LOGON STUFF
    void SetAttached( BOOL bAttached = TRUE ) { m_bAttached = bAttached; }
    BOOL IsAttached() { return m_bAttached; }
    HRESULT AttachToServer( BOOL bReAttach = FALSE );

    CString& GetServerName() { return m_sServer; }
    void SetServerName( LPCTSTR szServerName ); 

    CString& GetUserID() { return m_sUser; }
    void SetUserID( LPCTSTR psz )  { m_sUser = psz; }

    CString& GetPassword() { return m_sPassword; }
    void SetPassword( LPCTSTR psz )  { m_sPassword = psz; }

    CString& GetDomainName() { return m_sDomain; }
    void SetDomainName( LPCTSTR psz )  { m_sDomain = psz; }

    //LPCTSTR GetDisplayName() { return m_sDisplayName; };
    //void    SetDisplayServerName( LPCTSTR szServer );
    LPCTSTR MakeTempDisplayName(); 

    HRESULT LoginToAvisConfigServer();
    HRESULT SecurityTest();  
    HRESULT mySetProxyBlanket( IUnknown* pUnk );

    CString& GetQSDefDate() { return m_sQSDefDate; }
    CString& GetQSDefVersion() { return m_sQSDefVersion; }

public:


    IQCScopeItem*          m_pQCScopeItem;
    ULONG                  m_ulQServerVersion;
    BOOL                   m_bLocal;
	IDataObject*           m_pDataObject;
//  IAvisConfig*           m_pConfig;
    CString                sRegKeyAvis;
    CString                sRegKeyAvisCurrent;
    CString                sRegKeyQServer;

private:

    // Q: Are we attached to a server at this time?
    BOOL            m_bAttached;
    // Server we are attatched to.
    CString         m_sServer;
    // User name
    CString         m_sUser;
    // Password 
    CString         m_sPassword;
    // Domain we are attached to
    CString         m_sDomain;
    // Display to show on Prop page
    CString         m_sDisplayName;

	// terrym 9-24-01 added so that qscon can give avisconsole virus def data
    // virus def version
    CString         m_sQSDefVersion;
    // virus def date
    CString         m_sQSDefDate;

private:
    DWORD           m_dwRef;

    // Dirty bit.
    BOOL            m_bDirty;

};


#endif







