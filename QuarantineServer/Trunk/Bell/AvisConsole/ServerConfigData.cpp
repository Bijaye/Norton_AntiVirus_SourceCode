/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*----------------------------------------------------------------------------
  TO DO

  12/11 add to Firewall Page
      m_sFirewallUsername 
      m_sFirewallPassword 
  12/11 add to Web Comm Page 
      m_bCheckSecureDefDownload;         
      m_bCheckSecureSampleSubmission;    
      m_bSecureStatusQuery; 
      m_bCheckSecureIgnoreHostname;
      m_iSSLPort;
      m_iRetryInterval;
      m_iRetryLimit;                     
      m_iTransactionTimeout;             
      m_iDefsNeededInterval;
      m_iStatusInterval;


 ----------------------------------------------------------------------------*/



#include "stdafx.h"	 
#include "resource.h"
#include "AvisConsole.h"
#include "ServerConfigData.h"
#include "InputRange.h"
#include "mmc.h"
#include "qsregval.h"
#include "avisregval.h"
#include "AvisEventDetection.h"   // in QuarantineServer\Include 
#include "password.h"
#include "vpstrutils.h"



extern HWND g_hMainWnd;

// Computer name clipboard format.
static CLIPFORMAT    m_CCF_COMPUTER_NAME;

// WINSOCK STUFF
#include  <winsock.h>
BOOL IsIpAddress( const char * pszAddress );
static WSADATA WinsockData;
static BOOL bWsockOpen = FALSE;




/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
CServerConfigData::CServerConfigData()
{
    m_iExtDataObjectID = 0;
    m_bAttached        = 0;
    m_bIsDirty      = FALSE;
    m_pSnapin       = NULL;
    m_lParam        = 0;
    m_pServerStream = NULL;
    m_lock          = 0;
    ClearServerDataMembers();
    m_bDataReadSuccessfully    = 0;
    m_bDataWrittenSuccessfully = 0;
    m_dwRef = 1;

    m_bLocal = 0;
    m_pDataObject = NULL;
    m_pQCScopeItem  = NULL;
    m_ulQServerVersion  = 1;

    sRegKeyQServer      = REGKEY_QSERVER;
    sRegKeyAvis         = sRegKeyQServer + REGKEY_ASERVER_SUBKEY;
    sRegKeyAvisCurrent  = sRegKeyAvis + REGKEY_ICEPACK_CURRENT_SUBKEY; 

    // INITIALIZE WINSOCK
    WORD wVersion = 0x0101; /* Version 1.1 */
    bWsockOpen    = FALSE;
    int iRet = WSAStartup(wVersion,&WinsockData);
    if( iRet == 0 )  
        bWsockOpen = TRUE;

    // if( bWsockOpen )
    // {
    //     char szHostname[512];
    //     gethostname( szHostname, sizeof(szHostname) );
    // }
};

///
CServerConfigData::~CServerConfigData()
{

    if( bWsockOpen )
        WSACleanup();

    Release();
    m_szGatewayAddress.Empty();
    m_szProxyFirewall.Empty();
    m_szDefLibraryPath.Empty();
    m_sFirewallUsername.Empty();
    m_sFirewallPassword.Empty();

  	fWidePrintString("CServerConfigData destructor called. ObjectID= %d", m_iExtDataObjectID);
}


// WRAPPERS
    // USE NEW QSERVER INTERFACE 12/27/99
    HRESULT CServerConfigData::GetValue( _bstr_t bstrFieldName, VARIANT * v,  BSTR pKeyName ) 
    {
        HRESULT _hr  = -1;
		USES_CONVERSION;
        BSTR sKey = NULL ;   // REGKEY_ASERVER
		//CComQIPtr<IQCScopeItem> pQCScopeItem( m_pDataObject );
        //m_pQCScopeItem = pQCScopeItem;

        // // NEED TO MARSHAL THIS INTERFACE 
        // CoMarshalInterThreadInterfaceInStream( __uuidof( IQCScopeItem ), pQCScopeItem, &m_pServerStream );
        // // Unmarshal server interface.
        // CoGetInterfaceAndReleaseStream( m_pServerStream, __uuidof( IQCScopeItem ), (LPVOID*)&m_pQCScopeItem );
        // _hr = m_pQCScopeItem->AvisConfigGetValue(bstrFieldName, sKey.AllocSysString(), v);   //  NULL


        // USE TO PASS IN A DIFFERENT KEY THAN THE DEFAULT
        if( pKeyName != NULL )
            sKey = pKeyName;
		else
			sKey = SysAllocString(T2COLE( sRegKeyAvis));

        if( m_pQCScopeItem != NULL )
            _hr = m_pQCScopeItem->AvisConfigGetValue(bstrFieldName, sKey, v);   // tam 5-25-00 bstr change
        else
            _hr = RPC_S_INTERFACE_NOT_FOUND;

        //if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
		if (pKeyName == NULL)
			SysFreeString(sKey);
        return _hr;                      
    }

    HRESULT CServerConfigData::SetValue( _bstr_t bstrFieldName, VARIANT * v, BSTR pKeyName )    //const _variant_t & v
    {   
        HRESULT _hr  = -1;
        BSTR sKey = NULL;
		USES_CONVERSION;
		//CComQIPtr<IQCScopeItem> m_pQCScopeItem( m_pDataObject );

        // USE TO PASS IN A DIFFERENT KEY THAN THE DEFAULT
        if( pKeyName != NULL )
            sKey = pKeyName;
		else
			sKey = SysAllocString( T2COLE(sRegKeyAvis));

        if( m_pQCScopeItem != NULL )
            _hr = m_pQCScopeItem->AvisConfigSetValue(bstrFieldName, sKey, v);// tam 5-25-00 bstr change
        else
            _hr = RPC_S_INTERFACE_NOT_FOUND;
	
		if (pKeyName == NULL)
			SysFreeString (sKey);
        return _hr;
    }





HRESULT  CServerConfigData::GetNewConfigInterface( BOOL bReAttach )
{
    HRESULT hr = 0;

    return hr;
}



/*----------------------------------------------------------------------------
    CServerConfigData::ReadConfigDataFromServer()



    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT  CServerConfigData::ReadConfigDataFromServer()
{
    HRESULT hr    = S_OK;
    HRESULT comhr = S_OK;
	USES_CONVERSION;
    BSTR	s=NULL; 
	CString sErrorMsg;
    DWORD   dwWriteDefaults = FALSE;
	BSTR    bstrRegKeyQServer = SysAllocString (T2COLE(sRegKeyQServer));
	BSTR    bstrRegKeyAvisCurrent = SysAllocString (T2COLE(sRegKeyAvisCurrent));
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // GET SETTINGS FROM THE SERVER
    try
    {

 		fWidePrintString(" CServerConfigData::ReadConfigDataFromServer Preparing to Read data.  ObjectID= %d", m_iExtDataObjectID );

        // GET THE CONFIGURATION INTERFACE FROM THE SERVER.
        hr = GetNewConfigInterface( FALSE );
        if( hr != S_OK)
             _com_issue_error( hr );


        fWidePrintString( "TIMING START CServerConfigData::ReadConfigDataFromServer called." );

        
        // DEBUG Check current thread 12/29/99
        SaveCurrentThreadId();

        
        // START READING DATA
        VARIANT v;
        CString strNum;
        TCHAR *endptr=NULL;
        
        // GET THE CHANGE COUNTER
        VariantInit( &v );
        s = SysAllocString (T2COLE(REGVALUE_CONFIG_CHANGE_COUNTER));
        hr = GetValue( s, &v );// tam 5-25-00 bstr change
        if( SUCCEEDED( hr ) )
        {
            if( hr == 2 || v.vt == VT_EMPTY )   
            {
                m_iConfigChangeCounter = 0;
            }
            else
            {
                strNum                = v.bstrVal;
                m_iConfigChangeCounter = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
            }
            VariantClear( &v );
        }

        // "General Attention Message"
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_GENERAL_ATTENTION_MESSAGE));
        //hr = GetValue( s.AllocSysString(), &v, sRegKeyQServer.AllocSysString() );  // GET FROM QSERVER'S KEY
        hr = GetValue( s, &v );  // 4/19/00 changed to get from Avis key jhill // tam 5-25-00 bstr change
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_sGeneralAttentionTokens = v.bstrVal;
                m_sGeneralAttentionTokens.Remove( ' ' );
                VariantClear( &v );
            }
        }
  
        // READ SERVER CONFIG DATA FOR CAVISSERVERCONFIG2      "Web Communication"  
        //
        // CAVISSERVERCONFIG2 READ GATEWAY ADDRESS             "Web Communication"
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_GATEWAY_ADDRESS));
//        hr = m_pConfig->GetValue( s.AllocSysString(), &v );
        hr = GetValue( s, &v );// tam 5-25-00 bstr change
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_szGatewayAddress = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_szGatewayAddress = WEB_DEFAULT_GATEWAY_ADDRESS;
                dwWriteDefaults = TRUE;
                //SetFieldDirty(FIELD_CHANGE_GATEWAY_ADDRESS);
            }
        }

        // REMOVED jhill 3/27/00
        // CAVISSERVERCONFIG2 READ m_iRetryInterval   "Web Communication"    
        // VariantInit( &v );
        // s =  REGVALUE_RETRY_INTERVAL;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum            = v.bstrVal;
        //         m_iRetryInterval  = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iRetryInterval = WEB_DEFAULT_RETRY_INTERVAL;           
        //         dwWriteDefaults  = TRUE;
        //     }
        // }
        // 
        // // CAVISSERVERCONFIG2 READ m_iRetryLimit     "Web Communication" 
        // VariantInit( &v );
        // s =  REGVALUE_RETRY_LIMIT;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum            = v.bstrVal;
        //         m_iRetryLimit     = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iRetryLimit   = WEB_DEFAULT_RETRY_LIMIT;           
        //         dwWriteDefaults = TRUE;
        //     }
        // }
        // 
        // // CAVISSERVERCONFIG2 READ m_iTransactionTimeout     "Web Communication" 
        // VariantInit( &v );
        // s =  REGVALUE_TRANSACTION_TIMEOUT;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum                = v.bstrVal;
        //         m_iTransactionTimeout = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iTransactionTimeout = WEB_DEFAULT_TRANSACTION_TIMEOUT;           
        //         dwWriteDefaults       = TRUE;
        //     }
        // }


        // CAVISSERVERCONFIG2 READ m_bCheckSecureSampleSubmission     "Web Communication" 
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_CHECK_SECURE_SUBMISSION));
        hr = GetValue( s, &v );// tam 5-25-00 bstr change
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                = v.bstrVal;
                DWORD dwTemp = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                m_bCheckSecureSampleSubmission = dwTemp != 0;  
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckSecureSampleSubmission = WEB_DEFAULT_CHECK_SECURE_SAMPLE_SUBMISSION;           
                dwWriteDefaults                = TRUE;
            }
        }

        // CAVISSERVERCONFIG2 READ m_bCheckSecureDefDownload         "Web Communication" 
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_CHECK_SECURE_DEF_DOWNLOAD));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                = v.bstrVal;
                DWORD dwTemp = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                m_bCheckSecureDefDownload = dwTemp != 0;  
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckSecureDefDownload = WEB_DEFAULT_CHECK_SECURE_DEF_DOWNLOAD;           
                dwWriteDefaults           = TRUE;
            }
        }


        // // CAVISSERVERCONFIG2 READ GATEWAY ADDRESS PORT / HTTP PORT    "Web Communication"
        // VariantInit( &v );
        // s =  REGVALUE_GATEWAY_ADDRESS_PORT;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum            = v.bstrVal;
        //         m_iGatewayPort = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iGatewayPort  = WEB_DEFAULT_GATEWAY_PORT;           
        //         dwWriteDefaults = TRUE;
        //     }
        // }
        // // CAVISSERVERCONFIG2 READ SSL PORT   "Web Communication"
        // VariantInit( &v );
        // s =  REGVALUE_SSL_PORT;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum            = v.bstrVal;
        //         m_iSSLPort = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iSSLPort      = WEB_DEFAULT_SSL_PORT;           
        //         dwWriteDefaults = TRUE;
        //     }
        // }
        // // CAVISSERVERCONFIG2 READ  m_bSecureStatusQuery             "Web Communication" 
        // VariantInit( &v );
        // s =  REGVALUE_CHECK_SECURE_STATUS_QUERY;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum                = v.bstrVal;
        //         DWORD dwTemp = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         m_bSecureStatusQuery = dwTemp != 0;  
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_bSecureStatusQuery  = WEB_DEFAULT_CHECK_SECURE_STATUS_QUERY;           
        //         dwWriteDefaults       = TRUE;
        //     }
        // }
        // // CAVISSERVERCONFIG2 READ  m_bCheckSecureIgnoreHostname     "Web Communication" 
        // VariantInit( &v );
        // s =  REGVALUE_CHECK_SECURE_IGNORE_HOSTNAME;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum                = v.bstrVal;
        //         DWORD dwTemp = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         m_bCheckSecureIgnoreHostname = dwTemp != 0;  
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_bCheckSecureIgnoreHostname = WEB_DEFAULT_CHECK_SECURE_IGNORE_HOSTNAME;           
        //         dwWriteDefaults              = TRUE;
        //     }
        // }
        // 







        // SERVER CONFIG DATA FOR CAServerFirewallConfig2
        //
        // CAServerFirewallConfig2 READ PROXY FIREWALL
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_PROXY_FIREWALL));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_szProxyFirewall = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }
        // CAServerFirewallConfig2 READ PROXY FIREWALL PORT
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_PROXY_FIREWALL_PORT));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum             = v.bstrVal;
                m_iFirewallPort = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_iFirewallPort = WEB_DEFAULT_FIREWALL_PORT;           
                dwWriteDefaults = TRUE;
            }
            if( m_iFirewallPort == 0 )
            {
                // SET DEFAULT
                m_iFirewallPort = WEB_DEFAULT_FIREWALL_PORT;           
                dwWriteDefaults = TRUE;
            }
        }
        // CAServerFirewallConfig2 READ  m_sFirewallUsername   
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_FIREWALL_USERNAME));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_sFirewallUsername = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }

        // CAServerFirewallConfig2 READ m_sFirewallPassword  
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_FIREWALL_PASSWORD));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_sFirewallPassword = v.bstrVal;
                // DECRYPT IT 1/3/00
                EncryptDecryptPassword(m_sFirewallPassword);
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }



        // SERVER CONFIG DATA FOR CAvisServerDefPolicyConfig       "Def Policy"
        //
        // // CAvisServerDefPolicyConfig READ DEFINITION LIBRARY FOLDER
        // VariantInit( &v );
        // s = REGVALUE_DEFLIBRARY_FOLDER;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         m_szDefLibraryPath = v.bstrVal;
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         dwWriteDefaults = TRUE;
        //         //m_szDefLibraryPath.Format(IDS_CONFIG_DEFAULT_DEFLIBRARY_FOLDER);  
        //     }
        // }
        // 
        //  // CAvisServerDefPolicyConfig READ  m_iDefsNeededInterval    12/15/99 
        // VariantInit( &v );
        // s = REGVALUE_NEEDED_INTERVAL;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum                = v.bstrVal;
        //         m_iDefsNeededInterval = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iDefsNeededInterval = WEB_DEFAULT_DEFS_NEEDED_INTERVAL;           
        //         dwWriteDefaults       = TRUE;
        //     }
        // }
        // 
        // // CAvisServerDefPolicyConfig READ  m_iPruneDefs        12/15/99
        // VariantInit( &v );
        // s =  REGVALUE_DEFS_PRUNE;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum             = v.bstrVal;
        //         m_iPruneDefs       = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iPruneDefs    = DEF_DEFAULT_DEFS_PRUNE;           
        //         dwWriteDefaults = TRUE;
        //     }
        // }
        // 
        // // CAvisServerDefPolicyConfig READ   m_iDefHeuristicLevel    12/15/99 
        // VariantInit( &v );
        // s =  REGVALUE_DEFS_HEURISTIC_LEVEL;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum               = v.bstrVal;
        //         m_iDefHeuristicLevel = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iDefHeuristicLevel = DEF_DEFAULT_DEFS_HEURISTIC_LEVEL;           
        //         dwWriteDefaults      = TRUE;
        //     }
        // }
        // 
        // // CAvisServerDefPolicyConfig READ  m_iUnpackTimeout    12/15/99 
        // VariantInit( &v );
        // s =  REGVALUE_DEFS_UNPACK_TIMEOUT;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum             = v.bstrVal;
        //         m_iUnpackTimeout = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iUnpackTimeout = DEF_DEFAULT_UNPACK_TIMEOUT;           
        //         dwWriteDefaults  = TRUE;
        //     }
        // }
        // 
        // // CAvisServerDefPolicyConfig READ  m_sCurrentBlessedDefsSeqNum  12/30/99
        // // READ FROM AVIS\CURRENT KEY
        // // THIS IS READ ONLY. IT IS NOT WRITTEN.  
        // VariantInit( &v );
        // s =  REGVALUE_DEFS_BLESSED_SEQUENCE_NUM;
        // hr = GetValue( s.AllocSysString(), &v, sRegKeyAvisCurrent.AllocSysString() ); // GET FROM current
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         m_sCurrentBlessedDefsSeqNum = v.bstrVal;
        //         VariantClear( &v );
        //     }
        // }





        // CAvisServerDefPolicyConfig READ m_dwActiveDefsSeqNum      12/27/99
        // READ FROM AVIS\CURRENT KEY
        // THIS IS READ ONLY. IT IS NOT WRITTEN.
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM));
        hr = GetValue( s, &v, bstrRegKeyAvisCurrent );  // GET FROM current
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum               = v.bstrVal;
                m_dwActiveDefsSeqNum = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
        }

        // CAvisServerDefPolicyConfig READ  m_bDefinitionActiveBlessed  12/27/99
        // READ FROM  AVIS\CURRENT KEY                           // QSERVER DIR, Not
        // THIS IS READ ONLY. IT IS NOT WRITTEN.
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEFS_ACTIVE_BLESSED));
        hr = GetValue( s, &v, bstrRegKeyAvisCurrent ); // GET FROM current
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                     = v.bstrVal;
                m_bDefinitionActiveBlessed = ( STRTOUL( (LPCTSTR)strNum, &endptr, 10 ) != 0 );
                VariantClear( &v );
            }
        }

        // CREATE THE STRING FOR ACTIVE DEF SEQUENCE
        // THIS IS NOT WRITTEN.
        if( m_dwActiveDefsSeqNum ) 
        {
            int iSeqNumTextID = IDS_UNBLESSED_TEXT;
            if( m_bDefinitionActiveBlessed )
                iSeqNumTextID = IDS_BLESSED_TEXT;
            CString sTempString;
            sTempString.LoadString(iSeqNumTextID);

            m_sActiveDefsSeqNum.Format(IDS_FMT_ACTIVE_DEFS_SEQ_NUM, m_dwActiveDefsSeqNum);
            m_sActiveDefsSeqNum += "  " + sTempString;

        }

        // CAvisServerDefPolicyConfig READ  m_iDefsBlessedInterval   12/15/99  
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEFS_BLESSED_INTERVAL));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum             = v.bstrVal;
                m_iDefsBlessedInterval = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_iDefsBlessedInterval = DEF_DEFAULT_DEFS_BLESSED_INTERVAL;           
                dwWriteDefaults = TRUE;
            }
        }





        // SERVER CONFIG DATA FOR DEF INSTALL PAGE
        //
        // CInstallDefinitions READ  m_bCheckBlessedBroadcast  12/16/99
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_BLESSED_BROADCAST));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                   = v.bstrVal;
                m_bCheckBlessedBroadcast = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckBlessedBroadcast = DEF_DEFAULT_CHECK_BLESSED_BROADCAST;           
                dwWriteDefaults          = TRUE;
            }
        }
         // CInstallDefinitions READ  m_bCheckUnblessedBroadcast  12/16/99
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_UNBLESSED_BROADCAST));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                     = v.bstrVal;
                m_bCheckUnblessedBroadcast = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckUnblessedBroadcast = DEF_DEFAULT_CHECK_UNBLESSED_BROADCAST;           
                dwWriteDefaults            = TRUE;
            }
        }
        // CInstallDefinitions READ m_bCheckUnblessedNarrowCast   12/16/99
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_UNBLESSED_NARROWCAST));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                      = v.bstrVal;
                m_bCheckUnblessedNarrowCast = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckUnblessedNarrowCast = DEF_DEFAULT_CHECK_UNBLESSED_NARROWCAST;           
                dwWriteDefaults             = TRUE;
            }
        }
        // CInstallDefinitions READ  m_bCheckUnblessedPointCast  12/16/99
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_UNBLESSED_POINTCAST));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                     = v.bstrVal;
                m_bCheckUnblessedPointCast = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckUnblessedPointCast = DEF_DEFAULT_CHECK_UNBLESSED_POINTCAST;           
                dwWriteDefaults            = TRUE;
            }
        }
        // CInstallDefinitions READ  m_sDefSecureUsername
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_SECURE_USERNAME));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
				USES_CONVERSION;
				char szEncryptedUserName[PASS_MAX_CIPHER_TEXT_BYTES]= {0};
				char szUnencryptedUsername[PASS_MAX_PLAIN_TEXT_BYTES+1]={0};
				ssStrnCpy(szEncryptedUserName, OLE2A(v.bstrVal), sizeof (szEncryptedUserName));
				UnMakeEP(szEncryptedUserName, sizeof (szEncryptedUserName), PASS_KEY7, PASS_KEY8, szUnencryptedUsername, sizeof(szUnencryptedUsername));
                m_sDefSecureUsername = szUnencryptedUsername;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }
        // CInstallDefinitions READ  m_sDefSecurePassword
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_SECURE_PASSWORD));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
				USES_CONVERSION;
				char szEncryptedPassword[PASS_MAX_CIPHER_TEXT_BYTES]= {0};
				char szUnencryptedPassword[PASS_MAX_PLAIN_TEXT_BYTES+1]={0};
				ssStrnCpy(szEncryptedPassword, OLE2A(v.bstrVal), sizeof (szEncryptedPassword));
				UnMakeEP(szEncryptedPassword, sizeof (szEncryptedPassword), PASS_KEY5, PASS_KEY6, szUnencryptedPassword, sizeof(szUnencryptedPassword));
				m_sDefSecurePassword = szUnencryptedPassword;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }
        // // CInstallDefinitions READ  m_iDefDeliveryTimeout  12/16/99
        // VariantInit( &v );
        // s = REGVALUE_DEF_DELIVERY_TIMEOUT;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum                = v.bstrVal;
        //         m_iDefDeliveryTimeout = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_iDefDeliveryTimeout  = DEF_DEFAULT_DELIVERY_TIMEOUT;           
        //         dwWriteDefaults        = TRUE;
        //     }
        // }


        // CInstallDefinitions READ  m_iDefDeliveryInterval  12/16/99
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_DELIVERY_INTERVAL));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                 = v.bstrVal;
                m_iDefDeliveryInterval = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_iDefDeliveryInterval = DEF_DEFAULT_DELIVERY_INTERVAL;           
                dwWriteDefaults        = TRUE;
            }
        }
        // CInstallDefinitions READ  m_sDefBlessedTargets    
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_BLESSED_TARGETS));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_sDefBlessedTargets = v.bstrVal;
                VariantClear( &v );
            }
        }
        // CInstallDefinitions READ m_sDefUnblessedTargets    
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_DEF_UNBLESSED_TARGETS));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_sDefUnblessedTargets = v.bstrVal;
                VariantClear( &v );
            }
        }



        // SERVER CONFIG DATA FOR CAVISSERVERCONFIG             "Sample Policy"    
        //
        // CAVISSERVERCONFIG READ STRIP USER DATA  m_bStripUserData  "Sample Policy" 
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_STRIP_USER_DATA));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum            = v.bstrVal;
                //m_bStripUserData  = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                m_bStripUserData  = (STRTOUL( (LPCTSTR)strNum, &endptr, 10 )) != 0;
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bStripUserData      = POLICY_DEFAULT_STRIP_USER_DATA;           
                dwWriteDefaults       = TRUE;
            }
        }

        // CAVISSERVERCONFIG READ   m_bCompressUserData       "Sample Policy" 
        // VariantInit( &v );
        // s = REGVALUE_COMPRESS_USER_DATA;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum              = v.bstrVal;
        //         m_bCompressUserData = (STRTOUL( (LPCTSTR)strNum, &endptr, 10 )) != 0;
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_bCompressUserData   = POLICY_DEFAULT_COMPRESS_USER_DATA;           
        //         dwWriteDefaults       = TRUE;
        //     }
        // }
        // // CAVISSERVERCONFIG READ   m_bScrambleUserData        "Sample Policy" 
        // VariantInit( &v );
        // s = REGVALUE_SCRAMBLE_USER_DATA;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum            = v.bstrVal;
        //         m_bScrambleUserData  = (STRTOUL( (LPCTSTR)strNum, &endptr, 10 )) != 0;
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_bScrambleUserData   = POLICY_DEFAULT_SCRAMBLE_USER_DATA;           
        //         dwWriteDefaults       = TRUE;
        //     }
        // }
        // // CAVISSERVERCONFIG READ MAX PENDING SAMPLES
        // VariantInit( &v );
        // s = REGVALUE_MAX_PENDINGSAMPLES;
        // hr = GetValue( s.AllocSysString(), &v );
        // if( SUCCEEDED( hr ) )
        // {
        //     if( v.vt != VT_EMPTY )
        //     {
        //         strNum                = v.bstrVal;
        //         m_dwMaxPendingSamples = STRTOUL( (LPCTSTR)strNum, &endptr, 10 ); 
        //         VariantClear( &v );
        //     }
        //     if( hr == 2 )   
        //     {
        //         // SET DEFAULT
        //         m_dwMaxPendingSamples = POLICY_DEFAULT_MAX_PENDING_SAMPLES;           
        //         dwWriteDefaults       = TRUE;
        //     }
        // }


        
        // CAVISSERVERCONFIG READ INITIAL SUBMISSION PRIORITY
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_INITIAL_SUBMISSIONPRIORITY));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                        = v.bstrVal;
                m_dwInitialSubmissionPriority = STRTOUL( (LPCTSTR)strNum, &endptr, 10 ); 
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_dwInitialSubmissionPriority = POLICY_DEFAULT_INITIAL_SUBMISSION_PRIORITY;           
                dwWriteDefaults               = TRUE;
            }
        }

        // CAVISSERVERCONFIG READ SAMPLE QUEUE CHECK INTERVAL   "Sample Policy" 
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_QUEUECHECK_INTERVAL));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                 = v.bstrVal;
                m_dwQueueCheckInterval = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_dwQueueCheckInterval= POLICY_DEFAULT_QUEUE_CHECK_INTERVAL;           
                dwWriteDefaults       = TRUE;
            }
        }

        // CAVISSERVERCONFIG2 READ STATUS INTERVAL  m_iStatusInterval
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_STATUS_INTERVAL));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum            = v.bstrVal;
                m_iStatusInterval = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_iStatusInterval     = WEB_DEFAULT_STATUS_INTERVAL;           
                dwWriteDefaults       = TRUE;
            }
        }








        // SERVER CONFIG  CCUSTOMERINFO DATA
        //
        // CCUSTOMERINFO READ COMPANY NAME
        VariantInit( &v );
        SysReAllocString(&s, T2COLE(REGVALUE_COMPANY_NAME));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_szCompanyName = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }
        // CCUSTOMERINFO READ CONTACT NAME
        SysReAllocString(&s, T2COLE(REGVALUE_CONTACT_NAME));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_szContactName = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }
        // CCUSTOMERINFO READ CONTACT PHONE
        SysReAllocString(&s, T2COLE(REGVALUE_CONTACT_PHONE));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_szContactPhone = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }
        // CCUSTOMERINFO READ CONTACT EMAIL
        SysReAllocString(&s, T2COLE(REGVALUE_CONTACT_EMAIL));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_szContactEmail = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }
        // CCUSTOMERINFO READ CUSTOMER ACCOUNT
        SysReAllocString(&s, T2COLE(REGVALUE_CUSTOMER_ACCOUNT));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_szCustomerAccount = v.bstrVal;
                VariantClear( &v );
            }
            if( hr == 2 )   dwWriteDefaults = TRUE;
        }


        // ALERTING
        // CAlertingSetConfig   m_bCheckEnableAlerts  
        SysReAllocString(&s, T2COLE(REGVALUE_ALERTING_ENABLED));
        hr = GetValue( s, &v, bstrRegKeyQServer );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum               = v.bstrVal;
                m_bCheckEnableAlerts = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckEnableAlerts = ALERTING_DEFAULT_ENABLE;           
                dwWriteDefaults      = TRUE;
            }
        }

        // CAlertingSetConfig  m_bCheckNtEventLog
        SysReAllocString(&s, T2COLE(REGVALUE_WRITE_TO_NT_EVENT_LOG));
        hr = GetValue( s, &v, bstrRegKeyQServer );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum             = v.bstrVal;
                m_bCheckNtEventLog = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_bCheckNtEventLog = ALERTING_DEFAULT_NT_EVENT_LOG;           
                dwWriteDefaults    = TRUE;
            }
        }

        // CAlertingSetConfig  m_sAmsServerAddress  
        SysReAllocString(&s, T2COLE(REGVALUE_AMS_SERVER_ADDRESS));
        hr = GetValue( s, &v, bstrRegKeyQServer );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                m_sAmsServerAddress = v.bstrVal;
                VariantClear( &v );
            }
        }

        // CAlertingSetConfig m_sAlertCheckInterval  STORED AS DWORD, NOT STRING 
        SysReAllocString(&s, T2COLE(REGVALUE_ALERTINTERVAL));
        hr = GetValue( s, &v, bstrRegKeyQServer);
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                if( v.vt == VT_BSTR )
                {
                    strNum                 = v.bstrVal;
                    m_iAlertCheckInterval  = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                }
                else if( v.vt == VT_UI4 )
                {
                    m_iAlertCheckInterval  = v.ulVal;
                }
                if( m_iAlertCheckInterval <= 0 )
                {
                    m_iAlertCheckInterval = 1;
                    dwWriteDefaults       = TRUE;
                }
                VariantClear( &v );
            }
            if( hr == 2 )   
            {
                // SET DEFAULT
                m_iAlertCheckInterval = ALERTING_DEFAULT_ALERT_INTERVAL;           
                dwWriteDefaults       = TRUE;
            }
        }

        // CServerGeneralError m_sLastAlertString  
        // GET THE LAST ALERT ERROR TOKEN    READ ONLY
        SysReAllocString(&s, T2COLE(REGVALUE_ALERT_ICEPACK_ATTENTION_LAST_ALERT));
        hr = GetValue( s, &v, bstrRegKeyQServer);
        if( SUCCEEDED( hr ) && v.vt == VT_BSTR )      
        {
            m_sLastAlertString = v.bstrVal; 
            VariantClear( &v );
        }

        // CServerGeneralError m_sLastAlertDate    
        // GET THE LAST ALERT DATE    READ ONLY
        SysReAllocString(&s, T2COLE(REGVALUE_ALERT_ICEPACK_ATTENTION_LAST_ALERT_DATE));
        hr = GetValue( s, &v, bstrRegKeyQServer );
        if( SUCCEEDED( hr ) && v.vt == VT_BSTR )      
        {
            m_sLastAlertDate = v.bstrVal; 
            VariantClear( &v );
        }


        // ParseEventData.cpp
        ReadAllEventConfigData();




        // SET THE FLAG
        m_bDataReadSuccessfully = TRUE;

    }

    catch( _com_error e )
    {
        hr = comhr = e.Error();
        // TELL THE USER 
        if( s==NULL)
            sErrorMsg.Format( IDS_REGISTRY_READ_ERROR_FMT, e.ErrorMessage() );
        else
            sErrorMsg.Format( IDS_REGISTRY_READ_ERROR2_FMT, s, e.ErrorMessage() );
        ShowErrorMessage(sErrorMsg, IDS_CONFIG_READ_ERROR, 0, comhr, g_ServerDisplayName);
    }
    catch(...)
    {
        hr = E_FAIL;
        // TELL THE USER 
        sErrorMsg.Format( IDS_REGISTRY_READ_ERROR3_FMT, s );         // _T("Error reading value name [%s] from remote registry.\r\n")
        ShowErrorMessage(sErrorMsg, IDS_CONFIG_READ_ERROR, 0, comhr, g_ServerDisplayName);
    }

    // ARE SOME OF THE ENTRIES MISSING ON THE SERVER? IF SO CREATE THEM,
    if( dwWriteDefaults )
    {
        memset( szDirtyFields, 1, MAX_FIELD_CHANGE_VALUE); // SET ALL, TO WRITE OUT EVERYTHING
        SetDirty(TRUE);                                    // NEED THIS SET TO WRITE ANYTHING AT ALL
    }

    // RELEASE CONFIGURATION INTERFACE      jhill 4/19/99  
    // DetachAndReleaseConfigInterface();    // if USE_AVISSERVER, this writes out the defaults



    fWidePrintString( "TIMING STOP CServerConfigData::ReadConfigDataFromServer done." );
	SysFreeString(bstrRegKeyQServer);
	SysFreeString(bstrRegKeyAvisCurrent);
	SysFreeString(s);

    return(hr);
}





/*----------------------------------------------------------------------------
    CServerConfigData::WriteConfigDataToServer()

    Only write fields that have been changed.
    12/29/99 Only called from the hidden window.
    The property pages call CServerConfigData::WriteConfigDataToServer().
    It sends a message to the hidden window, which then calls this routine.

    Written by: Jim Hill                WriteConfigDataToServer()
 ----------------------------------------------------------------------------*/
HRESULT  CServerConfigData::WorkerWriteConfigDataToServer() 
{
    DWORD   iConfigChangeCounter = -1;
    HRESULT hr    = S_OK;
    HRESULT comhr = S_OK;
    CString sErrorMsg;
	BSTR	s=NULL;
	USES_CONVERSION;
	BSTR    bstrRegKeyQServer = SysAllocString (T2COLE(sRegKeyQServer));

    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if( !IsDirty() )
        return(S_OK);

    // SET THE FLAG    1/2/00 jhill
    SetDirty(FALSE);

    // WRITE SETTINGS TO THE SERVER
    try
    {

 		//fWidePrintString(" CServerConfigData::WorkerWriteConfigDataToServer Preparing to Write data.  ObjectID= %d", m_iExtDataObjectID );
        fWidePrintString( "TIMING START CServerConfigData::WorkerWriteConfigDataToServer called." );



        // GET THE CONFIGURATION INTERFACE FROM THE SERVER.
//        s.Empty();

        // FORMATTING
        // Trim leading and trailing spaces
        m_szDefLibraryPath.TrimLeft();
        m_szDefLibraryPath.TrimRight();
        // Strip off trailing backslashes
        m_szDefLibraryPath.TrimRight( _T('\\') );    


        // DEBUG Check current thread 12/29/99
        SaveCurrentThreadId();


        // WRITE SERVER CONFIG DATA TO REMOTE REGISTRY
        VARIANT v;
        CString strNum;
        TCHAR *endptr=NULL;
        
        // GET THE CHANGE COUNTER
        VariantInit( &v );
        //VariantClear( &v );
        s = SysAllocString(T2COLE(REGVALUE_CONFIG_CHANGE_COUNTER));
        hr = GetValue( s, &v );
        if( SUCCEEDED( hr ) )
        {
            if( v.vt != VT_EMPTY )
            {
                strNum                = v.bstrVal;
                iConfigChangeCounter = STRTOUL( (LPCTSTR)strNum, &endptr, 10 );
                VariantClear( &v );
            }
            else
                iConfigChangeCounter = 1;
        }

        // HAS ANOTHER CONSOLE WRITTEN TO THE SERVER?
        // if(iConfigChangeCounter != m_iConfigChangeCounter)



        // WRITE SERVER CONFIG DATA FOR CAVISSERVERCONFIG2
        // CAVISSERVERCONFIG2 WRITE GATEWAY ADDRESS
        if( IsFieldDirty( FIELD_CHANGE_GATEWAY_ADDRESS ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_szGatewayAddress.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_GATEWAY_ADDRESS));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
			VariantClear(&v);
            SetFieldClean( FIELD_CHANGE_GATEWAY_ADDRESS );
        }
        // if( IsFieldDirty( FIELD_CHANGE_GATEWAY_PORT ) ) 
        // {
        //     // CAVISSERVERCONFIG2 WRITE GATEWAY ADDRESS PORT  / HTTP PORT
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iGatewayPort);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_GATEWAY_ADDRESS_PORT;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_GATEWAY_PORT );
        // }
        // // CAVISSERVERCONFIG2 WRITE  m_iSSLPort   
        // if( IsFieldDirty( FIELD_CHANGE_SSL_PORT ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iSSLPort);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_SSL_PORT;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_SSL_PORT );
        // }
        // CAVISSERVERCONFIG2 WRITE  m_bSecureStatusQuery       
        // if( IsFieldDirty( FIELD_CHANGE_SECURE_STATUS_QUERY_CHECK ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_bSecureStatusQuery);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_CHECK_SECURE_STATUS_QUERY;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_SECURE_STATUS_QUERY_CHECK );
        // }
        // // CAVISSERVERCONFIG2 WRITE m_bCheckSecureIgnoreHostname    
        // if( IsFieldDirty( FIELD_CHANGE_SECURE_IGNORE_HOST_NAME ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_bCheckSecureIgnoreHostname);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_CHECK_SECURE_IGNORE_HOSTNAME;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_SECURE_IGNORE_HOST_NAME );
        // }
        // 
        // // CAVISSERVERCONFIG2 WRITE m_iRetryInterval  
        // if( IsFieldDirty( FIELD_CHANGE_RETRY_INTERVAL ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iRetryInterval);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_RETRY_INTERVAL;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_RETRY_INTERVAL );
        // }
        // // CAVISSERVERCONFIG2 WRITE m_iRetryLimit            
        // if( IsFieldDirty( FIELD_CHANGE_RETRY_LIMIT ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iRetryLimit);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_RETRY_LIMIT;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_RETRY_LIMIT );
        // }
        // // CAVISSERVERCONFIG2 WRITE  m_iTransactionTimeout     
        // if( IsFieldDirty( FIELD_CHANGE_TRANSACTION_TIMEOUT ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iTransactionTimeout);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_TRANSACTION_TIMEOUT;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_TRANSACTION_TIMEOUT );
        // }


        // CAVISSERVERCONFIG2 WRITE m_bCheckSecureSampleSubmission   
        if( IsFieldDirty( FIELD_CHANGE_SECURE_SAMPLE_SUBMISSION_CHECK ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckSecureSampleSubmission);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_CHECK_SECURE_SUBMISSION));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_SECURE_SAMPLE_SUBMISSION_CHECK );
			VariantClear(&v);
        }
        // CAVISSERVERCONFIG2 WRITE m_bCheckSecureDefDownload    
        if( IsFieldDirty( FIELD_CHANGE_SECURE_DEF_DOWNLOAD_CHECK ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckSecureDefDownload);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_CHECK_SECURE_DEF_DOWNLOAD));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_SECURE_DEF_DOWNLOAD_CHECK );
			VariantClear(&v);
        }


        // SERVER CONFIG DATA FOR CAServerFirewallConfig2
        //
        // CAVISSERVERCONFIG2 WRITE PROXY FIREWALL
        if( IsFieldDirty( FIELD_CHANGE_PROXY_FIREWALL ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_szProxyFirewall.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_PROXY_FIREWALL));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_PROXY_FIREWALL );
			VariantClear(&v);
        }
        // CAVISSERVERCONFIG2 WRITE PROXY FIREWALL PORT
        if( IsFieldDirty( FIELD_CHANGE_FIREWALL_PORT ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_iFirewallPort);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_PROXY_FIREWALL_PORT));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_FIREWALL_PORT );
			VariantClear(&v);
        }
        // CAVISSERVERCONFIG2 WRITE  m_sFirewallUsername  
        if( IsFieldDirty( FIELD_CHANGE_FIREWALL_USERNAME ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_sFirewallUsername.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_FIREWALL_USERNAME));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_FIREWALL_USERNAME );
			VariantClear(&v);
        }
        // CAVISSERVERCONFIG2 WRITE m_sFirewallPassword  
        if( IsFieldDirty( FIELD_CHANGE_FIREWALL_PASSWORD ) ) 
        {
            // ENCRYPT IT   1/3/00 jhill
            EncryptDecryptPassword(m_sFirewallPassword);

            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_sFirewallPassword.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_FIREWALL_PASSWORD));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_FIREWALL_PASSWORD );
			VariantClear(&v);
        }



        // SERVER CONFIG DATA FOR CAvisServerDefPolicyConfig  "Def Policy"
        //
        // // CAvisServerDefPolicyConfig WRITE DEFINITION LIBRARY FOLDER
        // if( IsFieldDirty( FIELD_CHANGE_DEFLIBRARY_FOLDER ) )
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     v.bstrVal = m_szDefLibraryPath.AllocSysString();
        //     s = T2COLE(REGVALUE_DEFLIBRARY_FOLDER;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_DEFLIBRARY_FOLDER );
        // }
        // 
        // // CAvisServerDefPolicyConfig WRITE   m_iDefHeuristicLevel    12/15/99 
        // if( IsFieldDirty( FIELD_CHANGE_DEFS_HEURISTIC_LEVEL ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iDefHeuristicLevel);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_DEFS_HEURISTIC_LEVEL;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_DEFS_HEURISTIC_LEVEL );
        // }
        // 
        // // CAvisServerDefPolicyConfig WRITE  m_iUnpackTimeout    12/15/99 
        // if( IsFieldDirty( FIELD_CHANGE_DEFS_UNPACK_TIMEOUT ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iUnpackTimeout);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_DEFS_UNPACK_TIMEOUT;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_DEFS_UNPACK_TIMEOUT );
        // }                  
        // 
        // // CAvisServerDefPolicyConfig WRITE  m_iDefsNeededInterval  12/15/99
        // if( IsFieldDirty( FIELD_CHANGE_DEFS_NEEDED_INTERVAL ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iDefsNeededInterval);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_NEEDED_INTERVAL;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_DEFS_NEEDED_INTERVAL );
        // }
        // 
        // // CAvisServerDefPolicyConfig WRITE  m_iPruneDefs        12/15/99
        // if( IsFieldDirty( FIELD_CHANGE_DEFS_DEFS_PRUNE ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iPruneDefs);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_DEFS_PRUNE;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_DEFS_DEFS_PRUNE );
        // }

        // CAvisServerDefPolicyConfig WRITE  m_iDefsBlessedInterval   12/15/99  
        if( IsFieldDirty( FIELD_CHANGE_DEFS_BLESSED_INTERVAL ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_iDefsBlessedInterval);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEFS_BLESSED_INTERVAL));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEFS_BLESSED_INTERVAL );
			VariantClear(&v);
        }



        // SERVER CONFIG DATA FOR DEF INSTALL PAGE
        //
        //  CInstallDefinitions  WRITE  m_bCheckBlessedBroadcast  12/16/99
        if( IsFieldDirty( FIELD_CHANGE_DEF_CHECK_BLESSED_BROADCAST ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckBlessedBroadcast);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_BLESSED_BROADCAST));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_CHECK_BLESSED_BROADCAST );
			VariantClear(&v);
        }
        //  CInstallDefinitions  WRITE  m_bCheckUnblessedBroadcast  12/16/99
        if( IsFieldDirty( FIELD_CHANGE_DEF_CHECK_UNBLESSED_BROADCAST ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckUnblessedBroadcast);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_UNBLESSED_BROADCAST));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_CHECK_UNBLESSED_BROADCAST );
			VariantClear(&v);
        }
        //  CInstallDefinitions  WRITE  m_bCheckUnblessedNarrowCast  12/16/99
        if( IsFieldDirty( FIELD_CHANGE_DEF_CHECK_UNBLESSED_NARROWCAST ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckUnblessedNarrowCast);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_UNBLESSED_NARROWCAST));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_CHECK_UNBLESSED_NARROWCAST );
			VariantClear(&v);
        }
        //  CInstallDefinitions  WRITE  m_bCheckUnblessedPointCast  12/16/99
        if( IsFieldDirty( FIELD_CHANGE_DEF_CHECK_UNBLESSED_POINTCAST ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckUnblessedPointCast);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_UNBLESSED_POINTCAST));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_CHECK_UNBLESSED_POINTCAST );
			VariantClear(&v);
        }

        if( IsFieldDirty( FIELD_CHANGE_DEF_SECURE_USERNAME ) ) 
        {
			USES_CONVERSION;
			char szEncryptedUsername[PASS_MAX_CIPHER_TEXT_BYTES]={0};
			char szUnencryptedUsername[PASS_MAX_PLAIN_TEXT_BYTES]={0};
			ssStrnCpy(szUnencryptedUsername, T2A((LPCTSTR)m_sDefSecureUsername), sizeof(szUnencryptedUsername));
			MakeEP(szEncryptedUsername, sizeof(szEncryptedUsername), PASS_KEY7, PASS_KEY8, szUnencryptedUsername,sizeof(szUnencryptedUsername));
            VariantInit( &v );
            v.vt = VT_BSTR;
			v.bstrVal = ::SysAllocString(A2OLE(szEncryptedUsername));
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_SECURE_USERNAME));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_SECURE_USERNAME );
			VariantClear(&v);
        }

		if( IsFieldDirty( FIELD_CHANGE_DEF_SECURE_PASSWORD ) ) 
        {
			USES_CONVERSION;
			char szEncryptedPassword[PASS_MAX_CIPHER_TEXT_BYTES]={0};
			char szUnencryptedPassword[PASS_MAX_PLAIN_TEXT_BYTES]={0};
			ssStrnCpy(szUnencryptedPassword, T2A((LPCTSTR)m_sDefSecurePassword), sizeof(szUnencryptedPassword));
			MakeEP(szEncryptedPassword, sizeof(szEncryptedPassword), PASS_KEY5, PASS_KEY6, szUnencryptedPassword,sizeof(szUnencryptedPassword));
			VariantInit( &v );
            v.vt = VT_BSTR;
			v.bstrVal = ::SysAllocString(A2OLE(szEncryptedPassword));
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_SECURE_PASSWORD));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_SECURE_PASSWORD );
			VariantClear(&v);
        }

		// //  CInstallDefinitions  WRITE  m_iDefDeliveryTimeout  12/16/99
        // if( IsFieldDirty( FIELD_CHANGE_DEF_DELIVERY_TIMEOUT ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_iDefDeliveryTimeout);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_DEF_DELIVERY_TIMEOUT;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_DEF_DELIVERY_TIMEOUT );
        // }

        //  CInstallDefinitions  WRITE  m_iDefDeliveryInterval  12/16/99
        if( IsFieldDirty( FIELD_CHANGE_DEF_DELIVERY_INTERVAL ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_iDefDeliveryInterval);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_DELIVERY_INTERVAL));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_DELIVERY_INTERVAL );
			VariantClear(&v);
        }
        // CInstallDefinitions WRITE  m_sDefBlessedTargets     12/16/99
        if( IsFieldDirty( FIELD_CHANGE_DEF_BLESSED_TARGETS ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_sDefBlessedTargets.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_BLESSED_TARGETS));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_BLESSED_TARGETS );
			VariantClear(&v);
        }
         // CInstallDefinitions WRITE  m_sDefUnblessedTargets  12/16/99
        if( IsFieldDirty( FIELD_CHANGE_DEF_UNBLESSED_TARGETS ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_sDefUnblessedTargets.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_DEF_UNBLESSED_TARGETS));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_DEF_UNBLESSED_TARGETS );
			VariantClear(&v);
        }


        // SERVER CONFIG DATA FOR CAVISSERVERCONFIG   "Sample Policy"   
        //
        // CAVISSERVERCONFIG WRITE STRIP USER DATA    "Sample Policy" 
        if( IsFieldDirty( FIELD_CHANGE_STRIP_USER_DATA ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bStripUserData);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_STRIP_USER_DATA));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_STRIP_USER_DATA );
			VariantClear(&v);
        }

        // CAVISSERVERCONFIG WRITE   m_bCompressUserData  "Sample Policy" 
        // if( IsFieldDirty( FIELD_CHANGE_COMPRESS_USER_DATA ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_bCompressUserData);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_COMPRESS_USER_DATA;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_COMPRESS_USER_DATA );
        // }
        // // CAVISSERVERCONFIG WRITE  m_bScrambleUserData   "Sample Policy" 
        // if( IsFieldDirty( FIELD_CHANGE_SCRAMBLE_USER_DATA ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_bScrambleUserData);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_SCRAMBLE_USER_DATA;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_SCRAMBLE_USER_DATA );
        // }
        // // CAVISSERVERCONFIG WRITE MAX PENDING SAMPLES    "Sample Policy" 
        // if( IsFieldDirty( FIELD_CHANGE_MAX_PENDINGSAMPLES ) ) 
        // {
        //     VariantInit( &v );
        //     v.vt = VT_BSTR;
        //     strNum.Format(_T("%d"),m_dwMaxPendingSamples);
        //     v.bstrVal = strNum.AllocSysString();
        //     s = T2COLE(REGVALUE_MAX_PENDINGSAMPLES;
        //     hr = SetValue( s.AllocSysString(), &v );
        //     if( FAILED( hr ) )  { _com_issue_error( hr ); }
        //     SetFieldClean( FIELD_CHANGE_MAX_PENDINGSAMPLES );
        // }


        // CAVISSERVERCONFIG WRITE QUEUE CHECK INTERVAL   "Sample Policy" 
        if( IsFieldDirty( FIELD_CHANGE_QUEUECHECK_INTERVAL ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_dwQueueCheckInterval);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_QUEUECHECK_INTERVAL));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_QUEUECHECK_INTERVAL );
			VariantClear(&v);
        }
        // CAVISSERVERCONFIG WRITE INITIAL SUBMISSION PRIORITY        "Sample Policy" 
        if( IsFieldDirty( FIELD_CHANGE_INITIAL_SUBMISSIONPRIORITY ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_dwInitialSubmissionPriority);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_INITIAL_SUBMISSIONPRIORITY));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_INITIAL_SUBMISSIONPRIORITY );
			VariantClear(&v);
        }

        // CAVISSERVERCONFIG WRITE STATUS INTERVAL m_iStatusInterval  "Sample Policy" 
        if( IsFieldDirty( FIELD_CHANGE_STATUS_INTERVAL ) ) 
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_iStatusInterval);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_STATUS_INTERVAL));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_STATUS_INTERVAL );
			VariantClear(&v);
        }


        // SERVER CONFIG  CCUSTOMERINFO DATA
        //
        // CCUSTOMERINFO WRITE COMPANY NAME
        if( IsFieldDirty( FIELD_CHANGE_COMPANY_NAME ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_szCompanyName.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_COMPANY_NAME));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_COMPANY_NAME );
			VariantClear(&v);
        }
        // CCUSTOMERINFO WRITE CONTACT NAME
        if( IsFieldDirty( FIELD_CHANGE_CONTACT_NAME ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_szContactName.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_CONTACT_NAME));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_CONTACT_NAME );
			VariantClear(&v);
        }
        // CCUSTOMERINFO WRITE CONTACT PHONE
        if( IsFieldDirty( FIELD_CHANGE_CONTACT_PHONE ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_szContactPhone.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_CONTACT_PHONE));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_CONTACT_PHONE );
			VariantClear(&v);
        }
        // CCUSTOMERINFO WRITE CONTACT EMAIL
        if( IsFieldDirty( FIELD_CHANGE_CONTACT_EMAIL ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_szContactEmail.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_CONTACT_EMAIL));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_CONTACT_EMAIL );
			VariantClear(&v);
        }
        // CCUSTOMERINFO WRITE CUSTOMER ACCOUNT
        if( IsFieldDirty( FIELD_CHANGE_CUSTOMER_ACCOUNT ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_szCustomerAccount.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_CUSTOMER_ACCOUNT));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_CUSTOMER_ACCOUNT );
			VariantClear(&v);
        }

        // ALERTING
        // CAlertingSetConfig   m_bCheckEnableAlerts
        if( IsFieldDirty( FIELD_CHANGE_ENABLE_ALERTS ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckEnableAlerts);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_ALERTING_ENABLED));
            hr = SetValue( s, &v, bstrRegKeyQServer);
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_ENABLE_ALERTS );
			VariantClear(&v);
        }

        // CAlertingSetConfig  m_bCheckNtEventLog
        if( IsFieldDirty( FIELD_CHANGE_ENABLE_NT_EVENT_LOG ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            strNum.Format(_T("%d"),m_bCheckNtEventLog);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_WRITE_TO_NT_EVENT_LOG));
            hr = SetValue( s, &v, bstrRegKeyQServer );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_ENABLE_NT_EVENT_LOG );
			VariantClear(&v);
        }

        // CAlertingSetConfig  m_sAmsServerAddress
        if( IsFieldDirty( FIELD_CHANGE_AMS_SERVER_ADDRESS ) )
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            v.bstrVal = m_sAmsServerAddress.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_AMS_SERVER_ADDRESS));
            hr = SetValue( s, &v, bstrRegKeyQServer);
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_AMS_SERVER_ADDRESS );
			VariantClear (&v);
        }

        // CAlertingSetConfig m_sAlertCheckInterval  STORED AS DWORD, NOT STRING
        if( IsFieldDirty( FIELD_CHANGE_ALERTING_INTERVAL ) )
        {
            VariantInit( &v );
            v.vt = VT_UI4;
            v.ulVal = m_iAlertCheckInterval;
            SysReAllocString(&s,T2COLE(REGVALUE_ALERTINTERVAL));
            hr = SetValue( s, &v, bstrRegKeyQServer );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
            SetFieldClean( FIELD_CHANGE_ALERTING_INTERVAL );
			VariantClear(&v);
        }

        // In ParseEventData.cpp
        WriteAllEventConfigData();



        // SET THE CHANGE COUNTER
        if(iConfigChangeCounter >= 0 )             // IF IT'S STILL -1, WE FAILED TO RETRIEVE IT.
        {
            VariantInit( &v );
            v.vt = VT_BSTR;
            ++iConfigChangeCounter;
            strNum.Format(_T("%d"),iConfigChangeCounter);
            v.bstrVal = strNum.AllocSysString();
            SysReAllocString(&s,T2COLE(REGVALUE_CONFIG_CHANGE_COUNTER));
            hr = SetValue( s, &v );
            if( FAILED( hr ) )  { _com_issue_error( hr ); }
			VariantClear(&v);
        }

        // SET THE FLAG
        m_bDataWrittenSuccessfully = TRUE;
        SetDirty(FALSE);
        fPrintString("CServerConfigData::WorkerWriteConfigDataToServer  DATA SAVED TO SERVER");
    }
    //catch(...)
    catch( _com_error e )
    {
        hr = E_FAIL;
        comhr = e.Error();
        if( s==NULL)
            sErrorMsg.Format( IDS_REGISTRY_WRITE_ERROR_FMT, e.ErrorMessage());
        else
            sErrorMsg.Format( IDS_REGISTRY_WRITE_ERROR2_FMT, s, e.ErrorMessage());
        //sErrorMsg.Format(_T("COM Error writing value name [%s] to remote registry. Data not saved.\r\n%s"), s, e.ErrorMessage());
        ShowErrorMessage(sErrorMsg, IDS_CONFIG_WRITE_ERROR, 0, comhr, g_ServerDisplayName);
    }
    catch(...)
    {
        hr = E_FAIL;
        // TELL THE USER            
        //sErrorMsg.Format(_T("Error writing value name [%s] to remote registry.\r\n"), s);
        sErrorMsg.Format( IDS_REGISTRY_WRITE_ERROR3_FMT, s);
        ShowErrorMessage(sErrorMsg, IDS_CONFIG_WRITE_ERROR, 0, comhr, g_ServerDisplayName);
    }


    fWidePrintString( "TIMING STOP CServerConfigData::WorkerWriteConfigDataToServer done." );
	//fWidePrintString(" CServerConfigData::WriteConfigDataToServer done writing data.  ObjectID= %d", m_iExtDataObjectID );

    // RELEASE CONFIGURATION INTERFACE
    // DetachAndReleaseConfigInterface();
	SysFreeString(bstrRegKeyQServer );
	SysFreeString(s);

    return(hr);
}


/*----------------------------------------------------------------------------
    CServerConfigData::Release()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD CServerConfigData::Release() 
{ 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    fPrintString("CServerConfigData::Release called");

    if( m_dwRef == 0 )
        return 0;

    if( --m_dwRef == 0 )
    {
        //Sleep(3000);
        ReleaseServerConfigObject();
        delete this;
        return 0;
    }
    return m_dwRef;

    return 0;
}



/*----------------------------------------------------------------------------
    CServerConfigData::ReleaseServerConfigObject()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT  CServerConfigData::ReleaseServerConfigObject()
{
    HRESULT hr          = S_OK;
    BOOL    bIsAttached = 0;

    // WRITE CHANGES TO THE REMOTE REGISTRY
//    WriteConfigDataToServer();   //12/29/99

    // RELEASE CONFIGURATION INTERFACE
    // DetachAndReleaseConfigInterface();

    // DESTROY THE HIDDEN WINDOW 12/29/00
    ReleaseHiddenWndThread();

    fPrintString("CServerConfigData::ReleaseServerConfigObject  m_pConfig Detach and Release");
    return(hr);
}


/*----------------------------------------------------------------------------
    CServerConfigData::ClearServerDataMembers()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
HRESULT CServerConfigData::ClearServerDataMembers()
{

    // Config data
    _ASSERTE( MAX_FIELD_CHANGE_VALUE < sizeof(szDirtyFields));
    memset( szDirtyFields, 0, sizeof(szDirtyFields));

    // "General Attention Message"
    m_sGeneralAttentionTokens = _T("");

    // CAvisServerConfig2       "Web Communication" 
	m_szGatewayAddress = _T("");
	m_iGatewayPort = 0;
	m_bCheckSecureDefDownload = FALSE;
	m_bCheckSecureSampleSubmission = FALSE;
	m_bSecureStatusQuery = FALSE;
    m_bCheckSecureIgnoreHostname = FALSE;
	m_iSSLPort = 0;
    m_iRetryInterval = 0;
	m_iRetryLimit = 0;
	m_iTransactionTimeout = 0;

    // CAServerFirewallConfig2  "AVIS Firewall"
    m_szProxyFirewall   = _T(""); 
    m_iFirewallPort     = 0;
    m_sFirewallUsername = _T("");
    m_sFirewallPassword = _T("");

    // CAvisServerConfig             "Sample Policy"  
	m_iStatusInterval = 0;
	m_bStripUserData = FALSE;
	m_dwQueueCheckInterval = 0;
	m_dwMaxPendingSamples = 0;
	m_dwInitialSubmissionPriority = 0;

    // CAvisServerDefPolicyConfig   "Definition Policy"  
	m_szDefLibraryPath     = _T("");
    m_sActiveDefsSeqNum    = _T("");
    m_dwActiveDefsSeqNum   = 0;  
    m_iDefHeuristicLevel   = 0;  
    m_iUnpackTimeout       = 0;      
    m_iDefsBlessedInterval = 0;
    m_iDefsNeededInterval  = 0;
    m_iPruneDefs           = 0;          


// Server Config Data for Def Install Page
//  m_dwDefinitionCheckInterval = 0;
//	m_bAutoDeliveryOfDefinitions = FALSE;
//  m_iDefinitionDeliveryPriority = 0;
    m_bCheckBlessedBroadcast    = 0;
    m_bCheckUnblessedBroadcast  = 0;
    m_bCheckUnblessedNarrowCast = 0;
    m_bCheckUnblessedPointCast  = 0;
    m_iDefDeliveryTimeout       = 0;
    m_iDefDeliveryPriority      = 0;
    m_iDefDeliveryInterval      = 0;
    m_sDefBlessedTargets        = _T("");
    m_sDefUnblessedTargets      = _T("");
    m_sDefSecureUsername        = _T("");
    m_sDefSecurePassword        = _T("");


    // CCustomerInfo
	m_szCompanyName      = _T("");   
	m_szContactEmail     = _T("");
	m_szContactName      = _T("");
	m_szContactPhone     = _T("");
	m_szCustomerAccount  = _T("");


// Server Config Data for Alerting Page
    m_bCheckEnableAlerts  = FALSE;
    m_sAmsServerAddress   = _T(""); 
    m_iAlertCheckInterval = 0; 
    m_bSendTestEvent      = 0;
    m_sLastAlertString    = _T("");
    m_sLastAlertDate      = _T("");

    m_iConfigChangeCounter = 0;

    return(S_OK);
}


//
BOOL CServerConfigData::IsValidFolderPath(CString* lpsFolder)
{
    BOOL bPathOk = FALSE;

    // Check the length
    if(lpsFolder==NULL || lpsFolder->GetLength() < 3 || lpsFolder->IsEmpty() )
        return FALSE;

    // Convert forward slashes.
    lpsFolder->Replace( _T('/'), _T( '\\' ) );

    // Trim leading and trailing spaces
    lpsFolder->TrimLeft();
    lpsFolder->TrimRight();

    // Strip off trailing backslashes
    lpsFolder->TrimRight( _T('\\') );    

    // Make sure there are no wildcard characters.
    if( -1 != lpsFolder->FindOneOf( _T("*?") ) )
        return FALSE;

//#ifdef REQUIRE_UNC_PATHS
    // ALLOW EITHER 12/21/99
    // MINIMAL VALIDATION
    // ALLOW ONLY UNC PATHS
    if( lpsFolder->GetAt(0) == '\\' && lpsFolder->GetAt(1) == '\\' )
    {
        // ALLOW UNC PATHS
        return(TRUE);
    }
  
    // THIS ALLOWS DRIVE LETTER PATHS
    if( lpsFolder->GetAt(1) == ':' && lpsFolder->GetAt(2) == '\\' )
    {
        return(TRUE);
    }



    return(FALSE);
}



/*----------------------------------------------------------------------------
    CServerConfigData::SetFieldDirty()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD  CServerConfigData::SetFieldDirty(DWORD dwFieldID)
{

    if(dwFieldID > MAX_FIELD_CHANGE_VALUE)
        return(FALSE);

    szDirtyFields[dwFieldID] = TRUE;

    return(TRUE);
}



/*----------------------------------------------------------------------------
    CServerConfigData::IsFieldDirty()

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD  CServerConfigData::IsFieldDirty(DWORD dwFieldID)
{
    if(dwFieldID > MAX_FIELD_CHANGE_VALUE)
        return(FALSE);

    if( szDirtyFields[dwFieldID] != 0 )
        return(TRUE);
    else
        return(FALSE);
}


/*----------------------------------------------------------------------------
    CServerConfigData::SetFieldClean()
    Clear the dirty bit.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD  CServerConfigData::SetFieldClean(DWORD dwFieldID)
{

    if(dwFieldID > MAX_FIELD_CHANGE_VALUE)
        return(FALSE);

    szDirtyFields[dwFieldID] = FALSE;

    return(TRUE);
}



/*----------------------------------------------------------------------------
   ValidateIpAddress()

----------------------------------------------------------------------------*/
BOOL CServerConfigData::ValidateIpAddress( LPTSTR lpsAddrString )
{
    char   szAddr[259];
    int    iRet = TRUE;

    if( lpsAddrString == NULL )
        return(FALSE);

#ifdef _UNICODE
     iRet = WideCharToMultiByte(CP_ACP,0,(LPCWSTR)lpsAddrString,-1,
                                (LPSTR)szAddr,sizeof(szAddr),NULL,NULL);
     if( iRet == FALSE )
        return(FALSE);
#else
    vpstrncpy( szAddr, lpsAddrString, sizeof(szAddr) );
#endif

    DWORD dwWsockError = 0;
    sockaddr saDestAddr;
    sockaddr_in* pAddr = (sockaddr_in*) &saDestAddr;
    memset( &saDestAddr, 0, sizeof( sockaddr ) );
    pAddr->sin_family = PF_INET;

    if( IsIpAddress( szAddr ) )
    {
        // Convert x.x.x.x address to IP address
        pAddr->sin_addr.S_un.S_addr = inet_addr( szAddr );
        if( pAddr->sin_addr.S_un.S_addr == INADDR_NONE )
        {
            return(FALSE);
        }
    }
    else
    {
        // Try looking up server using DNS.
        struct hostent *hp = gethostbyname( szAddr );
        dwWsockError = WSAGetLastError();
        if( hp == NULL )
        {
            return(FALSE);
        }
    }

    return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : IsIpAddress
//
// Description   : Returns TRUE if the specified address looks like a valid IP 
//                 address. (xxx.xxx.xxx.xxx).
//
// Return type   : BOOL 
//
// Argument      : const char * pszAddress
//
///////////////////////////////////////////////////////////////////////////////
// 12/28/98 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL IsIpAddress( const char * pszAddress )
{
    char szAddress[ 17 ];
    int iAddressComponents = 4;
    int iLength, i;

    // 
    // Make sure no one is messing with us.
    // 
    if( strstr( pszAddress, ".." ) != NULL ||
        strlen( pszAddress ) > 15 )
        return FALSE;
    
    // 
    // Parse string
    // 
	vpstrncpy(szAddress,pszAddress,sizeof(szAddress));
    char *p = strtok( szAddress, "." );

    while( p != NULL && iAddressComponents )
        {
        // 
        // Check length.
        // 
        iLength = strlen( p );
        if( iLength > 3 )
            return FALSE;
        
        // 
        // Make sure all characters are digits
        // 
        for( i = 0; i < iLength; i++ )
            {
            if( !isdigit( p[i] ) )
                return FALSE;
            }
        
        // 
        // Make sure ranges are correct.
        // 
        if( atoi( p ) > 255 )
            return FALSE;
        
        // 
        // Get next token.
        // 
        iAddressComponents --;        
        p = strtok( NULL, "." );
        }

    // 
    // If we have found 4 address components, and there are no more
    // we have a valid IP address.
    // 
    if( iAddressComponents == 0 && p == NULL )
        return TRUE;

    // 
    // Valid IP address not found.
    // 
    return FALSE;
    
}




//
// SERVER LOGON STUFF --------------------------------------------------------------------------------------
//


/*--------------------------------------------------------------------
   CServerConfigData::MakeTempDisplayName  

----------------------------------------------------------------------*/
LPCTSTR CServerConfigData::MakeTempDisplayName() 
{ 
    CString  sDisplayName;

    if( m_bLocal )
        sDisplayName.LoadString( IDS_LOCAL_NODE_NAME );
    else
        sDisplayName.Format( IDS_NODE_NAME_FMT, m_sServer ); 

    if( !m_sServer.IsEmpty() )
    {
        vpstrncpy( g_ServerDisplayName, (LPCTSTR) sDisplayName, sizeof(g_ServerDisplayName) );
        return(g_ServerDisplayName);
    }

    return(NULL);
}


/*--------------------------------------------------------------------
   CServerConfigData::GetScopeItemName  

----------------------------------------------------------------------*/
HRESULT CServerConfigData::GetScopeItemServerName( TCHAR *lpszServerName )
{
    HRESULT   hr = S_OK;
    DWORD     dwBytesRead = 0;

    // m_pDataObject is a CQSDataObject. Use it to retrieve the server name.
    m_CCF_COMPUTER_NAME = (CLIPFORMAT) RegisterClipboardFormat( _T("MMC_SNAPIN_MACHINE_NAME" ) );
    if(m_pDataObject == NULL || lpszServerName == NULL )
    	return E_UNEXPECTED;

    CComPtr< IStream > pStream;
    STGMEDIUM stg    = { TYMED_HGLOBAL, 0 };
    FORMATETC format = { m_CCF_COMPUTER_NAME,
                         NULL,
                         DVASPECT_CONTENT,
                         -1,
                         TYMED_HGLOBAL };

    *lpszServerName = '\0';

    stg.hGlobal = GlobalAlloc( GHND, sizeof( MAX_PATH + 1 ) );
    if( stg.hGlobal == NULL )
        return E_OUTOFMEMORY;
    
    hr = m_pDataObject->GetDataHere( &format, &stg );
    if( SUCCEEDED( hr ) )
    {
        hr = CreateStreamOnHGlobal( stg.hGlobal, FALSE, &pStream );
        if( SUCCEEDED( hr ) )
        {
            hr = pStream->Read( lpszServerName, MAX_PATH, &dwBytesRead );
            if( FAILED( hr ) )
            {
                *lpszServerName = '\0';
            }
        }
    }

    GlobalFree( stg.hGlobal );

    // NOW SET m_sServer
    if( SUCCEEDED( hr ) )
        SetServerName( lpszServerName );

    return(hr);
}



/*--------------------------------------------------------------------
   CServerConfigData::SetServerName  

----------------------------------------------------------------------*/
void CServerConfigData::SetServerName( LPCTSTR lpszServerName )  
{ 
    if( lpszServerName == NULL || *lpszServerName == '\0')
    {
        m_bLocal = TRUE;
    }
    else
    {
        m_bLocal = FALSE;
        TCHAR szMachineName[MAX_PATH];
        DWORD dwBufSize = sizeof(szMachineName);
        if( GetComputerName(szMachineName, &dwBufSize) != 0 )
        {
            CString s = lpszServerName;
            s.TrimRight();
            s.TrimLeft();
            s.TrimLeft(_T('\\'));
            s.TrimLeft();
        
            if( STRCMP( (LPCTSTR) s, szMachineName) == 0 )
            {
                m_bLocal = TRUE;
                //return;
            }
        }

        m_sServer = lpszServerName; 
    }
}




















#if 0
/*----------------------------------------------------------------------------
   CServerConfigData::IsValidAddressAndPort()

   Validates that an http address has the proper form and a port number

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CServerConfigData::IsValidAddressAndPort(CString* lpsAddrString, BOOL bIsHttpAddr)
{
    CString  sAddr, s;
    DWORD    dwPort = 0;
    int      iLen   = 0;
    int      iIndex = 0;
    int      nCount = 0;
    int      iRet   = 0;
    TCHAR    szUrl[512];
    TCHAR    szErrorMsg[512];

    // INITIALIZE
    memset(szUrl,0,sizeof(szUrl));
    memset(szErrorMsg,0,sizeof(szErrorMsg));

    lpsAddrString->TrimLeft();
    vpstrncpy( szUrl, lpsAddrString->GetBuffer(0), sizeof(szURL));
    lpsAddrString->ReleaseBuffer(-1);

    bIsHttpAddr = FALSE;  // BYPASS HTTP TEST
    if( bIsHttpAddr )     // GATEWAY ADDRESS
    {
        iRet = ValidateHttpAddress(szUrl, &dwPort, szErrorMsg);
        
        if( iRet == IDABORT)
            return(FALSE);
        
        if( iRet == IDOK )
        {
            *lpsAddrString = szUrl;    
            return(TRUE);
        }
        
        if( iRet == IDCANCEL)
        {
            *lpsAddrString = szUrl;    // TEMP FOR DEBUGGING         (LPCTSTR)
            return(FALSE);
        }
        return(FALSE);
    }
    else                // FIREWALL ADDRESS
    {
        return( ValidateHostnameOrIpAddress( szUrl ) );
    }

    return TRUE;
}
#endif









