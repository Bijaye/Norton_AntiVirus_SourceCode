/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*--------------------------------------------------------------------
   AvisServerConfig2.cpp : implementation file

   Property Page Tab Title: "Avis Web Communication"         

   Written by: Jim Hill                 
--------------------------------------------------------------------*/


#include "stdafx.h"	 
#include "resource.h"
#include "AvisConsole.h"
#include "AvisServerConfig2.h"
#include "InputRange.h"
#include "mmc.h"
#include "qscommon.h"
#include "vpstrutils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// HOW MANY SHEETS DOES THE USER HAVE UP IN THE SCOPE PANE?
static int g_ScopePropSheetTotalCount = 0;





// Persistant data
// Define
typedef struct tagAVISDATACONFIG2
{
	TCHAR	szGatewayAddress[WEB_DEFAULT_GATEWAY_ADDRESS_MAXLEN+1];
	long	iGatewayPort;
    long	iSSLPort;      
    long	iRetryLimit;   
    long	iTransactionTimeout;
    int     iRetryInterval;
    BOOL    bCheckSecureIgnoreHostname;
	BOOL	bCheckSecureDefDownload;
	BOOL	bCheckSecureSampleSubmission;
	BOOL	bSecureStatusQuery;
}   AVIS_DATA_CONFIG2;
// Declare
AVIS_DATA_CONFIG2  AvisConfig2={0};
static DWORD dwAvisConfig2DefaultsSet=FALSE;



/////////////////////////////////////////////////////////////////////////////
// CAvisServerConfig2 property page

IMPLEMENT_DYNCREATE(CAvisServerConfig2, CPropertyPage)

CAvisServerConfig2::CAvisServerConfig2() : CPropertyPage(CAvisServerConfig2::IDD)
{
	//{{AFX_DATA_INIT(CAvisServerConfig2)
	m_bCheckSecureDefDownload = FALSE;
	m_bCheckSecureSampleSubmission = FALSE;
	m_sGatewayAddress = _T("");
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_pSnapin       = NULL;
    m_dwRef         = 1;
    m_lParam        = 0;
    m_pSConfigData  = NULL;
    m_lpHelpIdArray = g_WebCommunicationHelpIdArray;  // LOAD THE HELP MAP  g_AvisServerConfig2HelpIdArray
    m_pAvisServerConfig          = NULL;
    m_pAvisServerDefPolicyConfig = NULL;
    m_pAvisServerConfig2         = NULL;
    //m_pAServerFirewallConfig2    = NULL;
    m_pAvisCustomerInfo          = NULL;
    m_pServerGeneralError        = NULL;
    m_pAlertingSetConfig         = NULL;
	m_bGatewayListModifed		 = FALSE;
}

CAvisServerConfig2::~CAvisServerConfig2()
{
  	fWidePrintString("CAvisServerConfig2 Destructor called. ObjectID= %d", m_iExtDataObjectID);

}


// DEFINES FOR VARIABLE LENGTHS IN INPUTRANGE.H
void CAvisServerConfig2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvisServerConfig2)
	DDX_Control(pDX, IDC_COMBO_GATEWAY_ADDRESS, m_GatewayCombo);
	DDX_Check(pDX, IDC_CHECK_SECURE_DEF_DOWNLOAD, m_bCheckSecureDefDownload);
	DDX_Check(pDX, IDC_CHECK_SECURE_SAMPLE_SUBMISSION, m_bCheckSecureSampleSubmission);
	DDX_CBString(pDX, IDC_COMBO_GATEWAY_ADDRESS, m_sGatewayAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAvisServerConfig2, CPropertyPage)
	//{{AFX_MSG_MAP(CAvisServerConfig2)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_CHECK_SECURE_DEF_DOWNLOAD, OnCheckSecureDefDownload)
	ON_BN_CLICKED(IDC_CHECK_SECURE_SAMPLE_SUBMISSION, OnCheckSecureSampleSubmission)
	ON_CBN_EDITCHANGE(IDC_COMBO_GATEWAY_ADDRESS, OnEditchangeComboGatewayAddress)
	ON_CBN_SELCHANGE(IDC_COMBO_GATEWAY_ADDRESS, OnSelchangeComboGatewayAddress)
	ON_CBN_EDITUPDATE(IDC_COMBO_GATEWAY_ADDRESS, OnEditupdateComboGatewayAddress)
	ON_CBN_SELENDOK(IDC_COMBO_GATEWAY_ADDRESS, OnSelendokComboGatewayAddress)
	ON_CBN_KILLFOCUS(IDC_COMBO_GATEWAY_ADDRESS, OnKillfocusComboGatewayAddress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvisServerConfig2 message handlers

// USED BY ROUTINES THAT FREE UP THE MFC CPROPERTYPAGE MEMORY
static LPFNPSPCALLBACK   pfnMmcCallback2 = NULL;

  
/*----------------------------------------------------------------------------
   myPropPageCallback2()
   Free up the MFC CPropertyPage memory

   Written by: Jim Hill
----------------------------------------------------------------------------*/
static UINT CALLBACK myPropPageCallback2(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
    // CALL ORIGINAL MMC CALLBACK. 
    if( pfnMmcCallback2 != NULL)
    {
        ppsp->pfnCallback = pfnMmcCallback2;  // restore original ptr
        (*pfnMmcCallback2)(hWnd, uMsg, ppsp);
    }


	if( uMsg == PSPCB_RELEASE )
    {
        fWidePrintString("CAvisServerConfig2 myPropPageCallback2 called with PSPCB_RELEASE");
        CAvisServerConfig2* pAvisServerConfig2 = (CAvisServerConfig2*) ppsp->lParam;
        if(IsAddressValid((void *)pAvisServerConfig2, sizeof(CAvisServerConfig2)))   
        {
            if(pAvisServerConfig2->VerifyCorrectPage(pAvisServerConfig2))
                pAvisServerConfig2->Release();
        }
        else
            fWidePrintString("myPropPageCallback2  pAvisServerConfig2 page is an invalid address ----------------");
    }

	return 1;
}


/*----------------------------------------------------------------------------
   CAvisServerConfig2::HookPropPageCallback()
   Free up the MFC CPropertyPage memory

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig2::HookPropPageCallback()   
{
    void *lpfnCall    = myPropPageCallback2;

    pfnMmcCallback2   = m_psp.pfnCallback;   // SAVE THE CALLBACK TO MMC
    m_psp.pfnCallback = myPropPageCallback2;  // SUBSTITUTE OUR CALLBACK


    return(TRUE);
}


/*----------------------------------------------------------------------------
   CAvisServerConfig2::VerifyCorrectPage()
   Free up the MFC CPropertyPage memory

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig2::VerifyCorrectPage(CAvisServerConfig2*  pAvisServerConfig2)
{ 
    if(pAvisServerConfig2 == NULL || m_pAvisServerConfig2== NULL)
        return(FALSE);
    if(pAvisServerConfig2 == m_pAvisServerConfig2)  
        return(TRUE);
    else
        return(FALSE);
}


/*----------------------------------------------------------------------------
   CAvisServerConfig2::Release()
   Free up the MFC CPropertyPage memory
   Also calls Release() in the other 2 Prop Pages.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
DWORD CAvisServerConfig2::Release() 
{ 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    
    --m_dwRef;

    // WERE WE CALLED FROM PostNcDestroy()
    if( m_dwRef == 1 )
        m_dwRef = 0;

    if( m_dwRef == 0 )
    {
        // DELETE CAvisServerConfig   "Avis Sample Policy"
        try
        {
            if( m_pAvisServerConfig )
            {
                if( m_pAvisServerConfig->Release() == 0)
                    m_pAvisServerConfig = NULL;
            }
        }
        catch(...) 
        {
            m_pAvisServerConfig= NULL;
        } 

        // DELETE CCustomerInfo 
        try
        {
            if( m_pAvisCustomerInfo )
            {
                if( m_pAvisCustomerInfo->Release() == 0)
                    m_pAvisCustomerInfo = NULL;
            }
        }
        catch(...) 
        {
            m_pAvisCustomerInfo= NULL;
        } 

        // DELETE CAvisServerDefPolicyConfig  12/11/99  "Avis Definition Policy" 
        try
        {
            if( m_pAvisServerDefPolicyConfig )
            {
                if( m_pAvisServerDefPolicyConfig->Release() == 0)
                    m_pAvisServerDefPolicyConfig = NULL;
            }
        }
        catch(...) 
        {
            m_pAvisServerDefPolicyConfig= NULL;
        } 

//        // DELETE CAServerFirewallConfig2  8/23/00  "Avis Firewall" jhill
        try
        {
            if( m_pAServerFirewallConfig2 )
            {
                if( m_pAServerFirewallConfig2->Release() == 0)
                    m_pAServerFirewallConfig2 = NULL;
            }
        }
        catch(...) 
        {
            m_pAServerFirewallConfig2= NULL;
        } 

        // DELETE   12/16/99  "Install Definitions" 
        try
        {
            if( m_pInstallDefinitions )
            {
                if( m_pInstallDefinitions->Release() == 0)
                    m_pInstallDefinitions= NULL;
            }
        }
        catch(...) 
        {
            m_pInstallDefinitions= NULL;
        } 

        // DELETE m_pAlertingSetConfig  1/8/00  "Alerting" Page
        try
        {
            if( m_pAlertingSetConfig )
            {
                if( m_pAlertingSetConfig->Release() == 0)
                    m_pAlertingSetConfig = NULL;
            }
        }
        catch(...) 
        {
            m_pAlertingSetConfig= NULL;
        } 

        // DELETE m_pServerGeneralError  12/11/99  "Attention" or "Errors" Page
        try
        {
            if( m_pServerGeneralError )
            {
                if( m_pServerGeneralError->Release() == 0)
                    m_pServerGeneralError = NULL;
            }
        }
        catch(...) 
        {
            m_pServerGeneralError= NULL;
        } 



        // TELL THE SERVER CONFIG OBJECT WE'RE NOT USING IT ANYMORE
        if( m_pSConfigData != NULL )
            m_pSConfigData->Release();

        // DELETE CAvisServerConfig2     "Web Communication"
        delete this;
        return 0;
    }

    return m_dwRef;
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig2::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::PostNcDestroy();

    Release();     

}


//#ifdef _UNICODE
//    #define  strtoul   wcstoul
//#endif


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    CString  szDisplayName;
    m_dwRef++; 
    m_bGatewayAddressModified  = FALSE;
    m_bFirewallAddressModified = FALSE;

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

	SetGatewayHistory(); //(tm)
    // INSERT DCOM CODE TO GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // hr = m_pSConfigData->ReadConfigDataFromServer();
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
			// check to see if string is in the history list (tm)
			int iIndex = m_GatewayCombo.SelectString(0,m_pSConfigData->m_szGatewayAddress);
            m_sGatewayAddress             =  m_pSConfigData->m_szGatewayAddress; //(tm)
			if (iIndex == CB_ERR) // if it is not up date string and save off the new address (tm)
			{
				UpdateGatewayHistoryList(m_sGatewayAddress);	//(tm)
				m_GatewayCombo.AddString(m_sGatewayAddress);	//(tm)
			}
			m_bCheckSecureSampleSubmission =  m_pSConfigData->m_bCheckSecureSampleSubmission;
			m_bCheckSecureDefDownload      =  m_pSConfigData->m_bCheckSecureDefDownload;
			// m_iRetryLimit                  =  m_pSConfigData->m_iRetryLimit;
			// m_iTransactionTimeout          =  m_pSConfigData->m_iTransactionTimeout;
			// m_iRetryInterval               =  m_pSConfigData->m_iRetryInterval;
			// m_iGatewayPort                 =  m_pSConfigData->m_iGatewayPort;
			// m_iSSLPort                     =  m_pSConfigData->m_iSSLPort;
			// m_bSecureStatusQuery           =  m_pSConfigData->m_bSecureStatusQuery;
            // m_bCheckSecureIgnoreHostname   =  m_pSConfigData->m_bCheckSecureIgnoreHostname;

            // UPDATE THE GLOBALS TO CHECK FOR CHANGES ON OK
            vpstrncpy(AvisConfig2.szGatewayAddress, m_sGatewayAddress, sizeof (AvisConfig2.szGatewayAddress)); //tm
            AvisConfig2.bCheckSecureDefDownload      = m_bCheckSecureDefDownload;     
            AvisConfig2.bCheckSecureSampleSubmission = m_bCheckSecureSampleSubmission;
            // AvisConfig2.iRetryLimit                  = m_iRetryLimit;                 
            // AvisConfig2.iTransactionTimeout          = m_iTransactionTimeout;         
            // AvisConfig2.iRetryInterval               = m_iRetryInterval;              
            // AvisConfig2.iGatewayPort                 = m_iGatewayPort;
            // AvisConfig2.iSSLPort                     = m_iSSLPort;                    
            // AvisConfig2.bSecureStatusQuery           = m_bSecureStatusQuery;          
            // AvisConfig2.bCheckSecureIgnoreHostname   = m_bCheckSecureIgnoreHostname;

        }
    }

    //UpdateData( FALSE );

//  	// Spin control for Gateway Port
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_GATEWAY_PORT))->SetRange32(WEB_MIN_SPIN_GATEWAY_PORT, WEB_MAX_SPIN_GATEWAY_PORT);
//     // Set the buddy
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_GATEWAY_PORT))->SetBuddy( GetDlgItem(IDC_EDIT_GATEWAY_PORT));
//     // Set initial pos
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_GATEWAY_PORT))->SetPos(m_iGatewayPort);
//  	// Spin control for m_SpinSSLPort 
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_SSL_PORT))->SetRange32( WEB_MIN_SPIN_SSL_PORT, WEB_MAX_SPIN_SSL_PORT );
//     // Set the buddy
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_SSL_PORT))->SetBuddy( GetDlgItem(IDC_EDIT_SSL_PORT));
//     // Set initial pos
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_SSL_PORT))->SetPos(m_iSSLPort);
// 
//  	// Spin control for Retry interval
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_RETRY_INTERVAL))->SetRange32(WEB_MIN_SPIN_RETRY_INTERVAL, WEB_MAX_SPIN_RETRY_INTERVAL);
//     // Set the buddy
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_RETRY_INTERVAL))->SetBuddy( GetDlgItem(IDC_EDIT_RETRY_INTERVAL));
//     // Set initial pos
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_RETRY_INTERVAL))->SetPos(m_iRetryInterval);
// 
//  	// Spin control for m_SpinTransactionTimeout 
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_TRANSACTION_TIMEOUT))->SetRange32( WEB_MIN_SPIN_TRANSACTION_TIMEOUT, WEB_MAX_SPIN_TRANSACTION_TIMEOUT );
//     // Set the buddy
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_TRANSACTION_TIMEOUT))->SetBuddy( GetDlgItem(IDC_EDIT_TRANSACTION_TIMEOUT));
//     // Set initial pos
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_TRANSACTION_TIMEOUT))->SetPos(m_iTransactionTimeout);
// 
//  	// Spin control for m_SpinRetryLimit 
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_RETRY_LIMIT))->SetRange32( WEB_MIN_SPIN_RETRY_LIMIT, WEB_MAX_SPIN_RETRY_LIMIT );
//     // Set the buddy                                                     
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_RETRY_LIMIT))->SetBuddy( GetDlgItem(IDC_EDIT_RETRY_LIMIT));
//     // Set initial pos
//     ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_RETRY_LIMIT))->SetPos(m_iRetryLimit);
         

    UpdateData( FALSE );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
                   

/*----------------------------------------------------------------------------
   CAvisServerConfig2::SaveChangedData
   Save data in remote registry for the AVIS client on the server
   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig2::SaveChangedData() 
{
	DWORD dwDataChanged=0;

    // Save the persistant data that has been changed

    if(m_sGatewayAddress.Compare((LPCTSTR) AvisConfig2.szGatewayAddress) != 0)
    {
        //fWidePrintString("DATA SAVED CAvisServerConfig2::m_szGatewayAddress  Old=%s  New=%s",
        //                 (LPCTSTR) AvisConfig2.szGatewayAddress,(LPCTSTR) m_szGatewayAddress);
        vpstrncpy(AvisConfig2.szGatewayAddress, m_sGatewayAddress, sizeof(AvisConfig2.szGatewayAddress));
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_szGatewayAddress = m_sGatewayAddress;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_GATEWAY_ADDRESS );
    }

//     if( AvisConfig2.iGatewayPort != m_iGatewayPort )
//     {
//         AvisConfig2.iGatewayPort = m_iGatewayPort;
//         dwDataChanged = TRUE;
//         // SAVE IT TO CServerConfigData
//         m_pSConfigData->m_iGatewayPort = m_iGatewayPort;
//         // SET FLAG FOR THE WRITE ROUTINE
//         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_GATEWAY_PORT );
//     }
//     // CAvisServerConfig2  m_iSSLPort  
//     if( AvisConfig2.iSSLPort != m_iSSLPort )
//     {
//         AvisConfig2.iSSLPort = m_iSSLPort;
//         dwDataChanged = TRUE;
//         // SAVE IT TO CServerConfigData2
//         m_pSConfigData->m_iSSLPort = m_iSSLPort;
//         // SET FLAG FOR THE WRITE ROUTINE
//         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_SSL_PORT );
//     }
//     // CAvisServerConfig2    m_bSecureStatusQuery      
//     if( AvisConfig2.bSecureStatusQuery != m_bSecureStatusQuery )
//     {
//         AvisConfig2.bSecureStatusQuery = m_bSecureStatusQuery;
//         dwDataChanged = TRUE;
//         // SAVE IT TO CServerConfigData2
//         m_pSConfigData->m_bSecureStatusQuery = m_bSecureStatusQuery;
//         // SET FLAG FOR THE WRITE ROUTINE
//         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_SECURE_STATUS_QUERY_CHECK );
//     }
// 
//     // CAvisServerConfig2    m_bCheckSecureIgnoreHostname  
//     if( AvisConfig2.bCheckSecureIgnoreHostname != m_bCheckSecureIgnoreHostname )
//     {
//         AvisConfig2.bCheckSecureIgnoreHostname = m_bCheckSecureIgnoreHostname;
//         dwDataChanged = TRUE;
//         // SAVE IT TO CServerConfigData2
//         m_pSConfigData->m_bCheckSecureIgnoreHostname = m_bCheckSecureIgnoreHostname;
//         // SET FLAG FOR THE WRITE ROUTINE
//         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_SECURE_IGNORE_HOST_NAME );
//     }
// 
//     // CAvisServerConfig2  m_iRetryInterval 
//     if( AvisConfig2.iRetryInterval != m_iRetryInterval )
//     {
//         AvisConfig2.iRetryInterval = m_iRetryInterval;
//         dwDataChanged = TRUE;
//         // SAVE IT TO CServerConfigData2
//         m_pSConfigData->m_iRetryInterval = m_iRetryInterval;
//         // SET FLAG FOR THE WRITE ROUTINE
//         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_RETRY_INTERVAL );
//     }
//     // CAvisServerConfig2   m_iRetryLimit  
//     if( AvisConfig2.iRetryLimit != m_iRetryLimit )
//     {
//         AvisConfig2.iRetryLimit = m_iRetryLimit;
//         dwDataChanged = TRUE;
//         // SAVE IT TO CServerConfigData2
//         m_pSConfigData->m_iRetryLimit = m_iRetryLimit;
//         // SET FLAG FOR THE WRITE ROUTINE
//         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_RETRY_LIMIT );
//     }
//     // CAvisServerConfig2   m_iTransactionTimeout    
//     if( AvisConfig2.iTransactionTimeout != m_iTransactionTimeout )
//     {
//         AvisConfig2.iTransactionTimeout = m_iTransactionTimeout;
//         dwDataChanged = TRUE;
//         // SAVE IT TO CServerConfigData2
//         m_pSConfigData->m_iTransactionTimeout = m_iTransactionTimeout;
//         // SET FLAG FOR THE WRITE ROUTINE
//         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_TRANSACTION_TIMEOUT );
//     }

    // CAvisServerConfig2    m_bCheckSecureSampleSubmission    
    if( AvisConfig2.bCheckSecureSampleSubmission != m_bCheckSecureSampleSubmission )
    {
        AvisConfig2.bCheckSecureSampleSubmission = m_bCheckSecureSampleSubmission;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData2
        m_pSConfigData->m_bCheckSecureSampleSubmission = m_bCheckSecureSampleSubmission;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_SECURE_SAMPLE_SUBMISSION_CHECK );
    }
    // CAvisServerConfig2     m_bCheckSecureDefDownload 
    if( AvisConfig2.bCheckSecureDefDownload != m_bCheckSecureDefDownload )
    {
        AvisConfig2.bCheckSecureDefDownload = m_bCheckSecureDefDownload;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData2
        m_pSConfigData->m_bCheckSecureDefDownload = m_bCheckSecureDefDownload;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_SECURE_DEF_DOWNLOAD_CHECK );
    }

	// save the history list off if it has been modified.
	if(m_bGatewayListModifed) // tm
    {
		SaveHistoryList();
        m_bGatewayAddressModified = TRUE;
    }

    // NEED TO WRITE IT?
    if(dwDataChanged)  
        m_pSConfigData->SetDirty(TRUE);

    return;
}


/*----------------------------------------------------------------------------
   OK button pressed
   Save data in remote registry for the AVIS client on the server
   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig2::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

	// fWidePrintString(" CAvisServerConfig2::OnOK called.  ObjectID= %d", m_iExtDataObjectID );

	CPropertyPage::OnOK();
}   // OnOk



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig2::OnApply() 
{
    HRESULT hr = 0;
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

    // Make sure data is good.
    UpdateData(TRUE);

    SaveChangedData(); 

    // WRITE CHANGES TO THE REMOTE REGISTRY
    if( m_pSConfigData )
    {
        if( m_pSConfigData->IsDirty() )   // 1/2/00 jhill
        {
        	fWidePrintString("CAvisServerConfig2::OnApply calling WriteConfigDataToServer.  ObjectID= %d", m_iExtDataObjectID );
            hr = m_pSConfigData->WriteConfigDataToServer();
	        fWidePrintString("CAvisServerConfig2::OnApply done writing data.  ObjectID= %d", m_iExtDataObjectID );
        }

        // DISABLE APPLY BUTTON IF WE SUCCEEDED
      	SetModified(FALSE);
    }

    // REDISPLAY 4/11 jhill
    if( m_bGatewayAddressModified )
    {
		// check to see if string is in the history list 
		int iIndex = m_GatewayCombo.SelectString(0,m_pSConfigData->m_szGatewayAddress);
		if (iIndex == CB_ERR) // if it is not up date string and save off the new address (tm)
		{
			UpdateGatewayHistoryList(m_sGatewayAddress);   
			m_GatewayCombo.AddString(m_sGatewayAddress);   
		}

        UpdateData( FALSE );
        m_bGatewayAddressModified = FALSE;
    }

	return CPropertyPage::OnApply();
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
   Code borrowed from QConsole    
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig2::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    BOOL   bRet  = TRUE;
    BOOL   bRet2 = TRUE;
    int    iRet  = 0;
    TCHAR  szAddr[512];
    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;

//     //  GATEWAY PORT  
//     long iGatewayPort = GetDlgItemInt( IDC_EDIT_GATEWAY_PORT, &bRet, FALSE );
//     if( !bRet || ( iGatewayPort < WEB_MIN_SPIN_GATEWAY_PORT || iGatewayPort > WEB_MAX_SPIN_GATEWAY_PORT ) )
//     {
//         // Inform the user 
//         sText.Format( IDS_BADFMT_GATEWAY_PORT, WEB_MIN_SPIN_GATEWAY_PORT, WEB_MAX_SPIN_GATEWAY_PORT );
//         MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
//         iControl = IDC_EDIT_GATEWAY_PORT;
//     }
//     // SSL PORT   
//     long iSSLPort = GetDlgItemInt( IDC_EDIT_SSL_PORT, &bRet, FALSE );
//     if( !bRet || ( iSSLPort < WEB_MIN_SPIN_SSL_PORT || iSSLPort > WEB_MAX_SPIN_SSL_PORT ) )
//     {
//         // Inform the user 
//         sText.Format( IDS_BADFMT_SSL_PORT, WEB_MIN_SPIN_SSL_PORT, WEB_MAX_SPIN_SSL_PORT );
//         MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
//         iControl = IDC_EDIT_SSL_PORT;
//     }
// 
//     //  RETRY LIMIT  
//     long iRetryLimit = GetDlgItemInt( IDC_EDIT_RETRY_LIMIT, &bRet, FALSE );
//     if( !bRet || ( iRetryLimit < WEB_MIN_SPIN_RETRY_LIMIT || iRetryLimit > WEB_MAX_SPIN_RETRY_LIMIT ) )
//     {
//         // Inform the user 
//         sText.Format( IDS_BADFMT_RETRY_LIMIT, WEB_MIN_SPIN_RETRY_LIMIT, WEB_MAX_SPIN_RETRY_LIMIT );
//         MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
//         iControl = IDC_EDIT_RETRY_LIMIT;
//     }
//     //  TRANSACTION TIMEOUT  
//     long iTransactionTimeout = GetDlgItemInt( IDC_EDIT_TRANSACTION_TIMEOUT, &bRet, FALSE );
//     if( !bRet || ( iTransactionTimeout < WEB_MIN_SPIN_TRANSACTION_TIMEOUT || iTransactionTimeout > WEB_MAX_SPIN_TRANSACTION_TIMEOUT ) )
//     {
//         // Inform the user 
//         sText.Format( IDS_BADFMT_TRANSACTION_TIMEOUT, WEB_MIN_SPIN_TRANSACTION_TIMEOUT, WEB_MAX_SPIN_TRANSACTION_TIMEOUT );
//         MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
//         iControl = IDC_EDIT_TRANSACTION_TIMEOUT;
//     }
//     //   RETRY INTERVAL 
//     long iRetryInterval = GetDlgItemInt( IDC_EDIT_RETRY_INTERVAL, &bRet, FALSE );
//     if( !bRet || ( iRetryInterval < WEB_MIN_SPIN_RETRY_INTERVAL || iRetryInterval > WEB_MAX_SPIN_RETRY_INTERVAL ) )
//     {
//         // Inform the user 
//         sText.Format( IDS_BADFMT_RETRY_INTERVAL, WEB_MIN_SPIN_RETRY_INTERVAL, WEB_MAX_SPIN_RETRY_INTERVAL );
//         MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
//         iControl = IDC_EDIT_RETRY_INTERVAL;
//     }
// 
// 
//     // RESET THE CONTROL
//     if( iControl )
//     {
//         GetDlgItem( iControl )->SetFocus();
//         ((CEdit*) GetDlgItem( iControl) )->SetSel( 0, -1 );
//         return FALSE;
//     }
// 

    // Now, get the text data
    UpdateData(TRUE);

    // 4/11/00
    if( m_sGatewayAddress.IsEmpty() )
    {
       sText.Format( IDS_ERROR_BLANK_GATEWAY_ADDR );
       MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
	   m_GatewayCombo.SetFocus();
	   m_GatewayCombo.SetEditSel(0,-1);
       return(FALSE);
    }

    //if(m_bGatewayAddressModified)
    if(m_sGatewayAddress.Compare((LPCTSTR) AvisConfig2.szGatewayAddress) != 0)
    {
        m_sGatewayAddress.TrimRight();
        m_sGatewayAddress.TrimLeft();
        vpstrncpy( szAddr, m_sGatewayAddress.GetBuffer(0), sizeof(szAddr));
        m_sGatewayAddress.ReleaseBuffer(-1);
        bRet = m_pSConfigData->ValidateIpAddress( szAddr ); 
        if( bRet == FALSE )
        {
            sText.Format( IDS_ERROR_IP_VALIDATE_GATEWAY_ADDR, szAddr ); 
            iRet = ShowErrorMessage( (LPCTSTR) sText, IDS_ERROR_CAPTION_IP_VALIDIDATE, MB_ICONERROR | MB_YESNO);
            if( iRet == IDOK )
            {
                m_bGatewayAddressModified = FALSE;
            }
            else
            {
        	    SetModified(TRUE);
				int iIndex = m_GatewayCombo.SelectString(0,m_sGatewayAddress);
				if (iIndex == CB_ERR)
					m_GatewayCombo.AddString(m_sGatewayAddress);

				m_GatewayCombo.SetFocus();
				m_GatewayCombo.SetEditSel(0,-1);
                UpdateData( FALSE );  // jhill 4/11

//				GetDlgItem( IDC_COMBO_GATEWAY_ADDRESS )->SetWindowText( m_szGatewayAddress );
//        		GetDlgItem( IDC_COMBO_GATEWAY_ADDRESS )->SetFocus();
//                ((CEdit*) GetDlgItem( IDC_COMBO_GATEWAY_ADDRESS ) )->SetSel( 0, -1 );
                return(FALSE);
            }
        }
        else
            m_bGatewayAddressModified = FALSE;
    }

	return CPropertyPage::OnKillActive();
}




/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
/* tm
void CAvisServerConfig2::OnChangeEditGatewayAddress() 
{
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
    m_bGatewayAddressModified = TRUE;

}
*/ 

/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
// void CAvisServerConfig2::OnChangeEditGatewayPort() 
// {
// 	// TODO: Add your control notification handler code here
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 	SetModified(TRUE);
// }
// void CAvisServerConfig2::OnChangeEditSslPort() 
// {
// 	// TODO: Add your control notification handler code here
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 	SetModified(TRUE);
// }
// void CAvisServerConfig2::OnCheckSecureIgnoreHostname() 
// {
// 	// TODO: Add your control notification handler code here
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 	SetModified(TRUE);
// }
// void CAvisServerConfig2::OnCheckSecureStatusQuery() 
// {
// 	// TODO: Add your control notification handler code here
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 	SetModified(TRUE);
// }
// 
// void CAvisServerConfig2::OnChangeEditRetryLimit() 
// {
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 	SetModified(TRUE);
// }
// 
// void CAvisServerConfig2::OnChangeEditTransactionTimeout() 
// {
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 	SetModified(TRUE);
// }
// 
// void CAvisServerConfig2::OnChangeEditRetryInterval() 
// {
//     AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
// 	SetModified(TRUE);
// }



void CAvisServerConfig2::OnCheckSecureDefDownload() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}

void CAvisServerConfig2::OnCheckSecureSampleSubmission() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
}



void CAvisServerConfig2::OnEditupdateComboGatewayAddress() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
	m_bGatewayListModifed = TRUE;
	
}
void CAvisServerConfig2::OnSelendokComboGatewayAddress() 
{
	SetModified(TRUE);
}

void CAvisServerConfig2::OnKillfocusComboGatewayAddress() 
{
    UpdateData(TRUE);      
	
	if(m_bGatewayListModifed )
	{
		UpdateGatewayHistoryList(m_sGatewayAddress);
	}
}

/*----------------------------------------------------------------------------
   CAvisServerConfig2::OnEditchangeComboGatewayAddress()

   Written by: Terry Marles
----------------------------------------------------------------------------*/

void CAvisServerConfig2::OnEditchangeComboGatewayAddress() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
	
}
/*----------------------------------------------------------------------------
   CAvisServerConfig2::OnSelchangeComboGatewayAddress()

   Written by: Terry Marles
----------------------------------------------------------------------------*/

void CAvisServerConfig2::OnSelchangeComboGatewayAddress() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	SetModified(TRUE);
	
}

/*----------------------------------------------------------------------------
   CAvisServerConfig2::SetGatewayHistory()

   Written by: Terry Marles
----------------------------------------------------------------------------*/

HRESULT  CAvisServerConfig2::SetGatewayHistory(void) 
{
	HRESULT hr = E_FAIL;
	CRegKey reg;
	LONG    lResult;
	TCHAR	szGateHistory[5*1024];
	DWORD dwBufferLen = sizeof(szGateHistory);

	// open local console key
	lResult = reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS == lResult )
	{
		lResult = reg.QueryStringValue(REGVALUE_GATEWAY_HISTORY,szGateHistory, &dwBufferLen);
		if(ERROR_SUCCESS == lResult )
		{
            ParseHistoryList(szGateHistory);
		}
	    reg.Close();
	}


	
	POSITION pos = m_GatewayAddressList.GetHeadPosition();
	while (pos)
	{
		CString s = m_GatewayAddressList.GetAt(pos);
		m_GatewayCombo.AddString(s);
		m_GatewayAddressList.GetNext(pos);
	}
	

	return hr;
}

/*----------------------------------------------------------------------------
   CAvisServerConfig2::ParseHistoryList()

   Written by: Terry Marles
----------------------------------------------------------------------------*/

HRESULT  CAvisServerConfig2::ParseHistoryList(LPTSTR szGateHistory)
{
	HRESULT hr = E_FAIL;

	TCHAR *p = _tcstok(szGateHistory, _T(";"));
	while (p!=NULL)
	{
		CString s = p;
		m_GatewayAddressList.AddTail(s);
		p = _tcstok(NULL, _T(";"));
	}

	return hr;
}


/*----------------------------------------------------------------------------
   CAvisServerConfig2::UpdateGatewayHistoryList()

   Written by: Terry Marles
----------------------------------------------------------------------------*/

HRESULT  CAvisServerConfig2::UpdateGatewayHistoryList(LPCTSTR psGateWayAddress)
{
	HRESULT hr = E_FAIL;

	if (psGateWayAddress == NULL)
		return hr;

	POSITION pos = m_GatewayAddressList.Find(psGateWayAddress);

	if (pos == NULL)
	{
		pos = m_GatewayAddressList.AddHead(psGateWayAddress);
		m_bGatewayListModifed = TRUE;
		if (pos != NULL)
			hr = S_OK;
	}
	else
		hr = S_OK;

	return hr;
}


/*----------------------------------------------------------------------------
   CAvisServerConfig2::SaveHistoryList()

   Written by: Terry Marles
----------------------------------------------------------------------------*/

HRESULT  CAvisServerConfig2::SaveHistoryList(void) 
{
	HRESULT hr = E_FAIL;
	CRegKey reg;
	LONG    lResult;
	CString s;
	int i = 0;
	
	POSITION pos = m_GatewayAddressList.GetHeadPosition();
	while (pos !=NULL && i++<=MAX_NO_HISTORY_ITEMS)
	{
		s += m_GatewayAddressList.GetAt(pos);
		s += _T(";");

		m_GatewayAddressList.GetNext(pos);
	}
	
	// open local console key
	lResult = reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS == lResult )
	{
		lResult = reg.SetStringValue(REGVALUE_GATEWAY_HISTORY, s);
	    reg.Close();
		m_bGatewayListModifed = FALSE;
		hr = S_OK;
	}


	return hr;
}
	











// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
/*----------------------------------------------------------------------------
   CAvisServerConfig2::OnHelpInfo()
   F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig2::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD_AVIS_WEB_COMM_PAGE, m_lpHelpIdArray, pHelpInfo );  // (AVIS_HELP_ID_STRUCT *) &m_HelpIdArray
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

// PROP SHEET HELP BUTTON
/*----------------------------------------------------------------------------
   CAvisServerConfig2::OnNotify()
   PROP SHEET HELP BUTTON
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig2::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	NMHDR* pNMHDR = (NMHDR*)lParam;
    DoHelpPropSheetHelpButton(IDD_AVIS_WEB_COMM_PAGE, m_lpHelpIdArray, pNMHDR );  // (AVIS_HELP_ID_STRUCT *)&m_HelpIdArray
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;

/*----------------------------------------------------------------------------
   CAvisServerConfig2::OnContextMenu()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig2::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());   
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD_AVIS_WEB_COMM_PAGE, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd ); // (AVIS_HELP_ID_STRUCT *)&m_HelpIdArray
    return;
}
#if 0
/*----------------------------------------------------------------------------
   CAvisServerConfig2::OnWhatsThisMenuitem()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig2::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR
    if( nResourceID == 0)
        nResourceID = IDD_AVIS_WEB_COMM_PAGE;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );  // (AVIS_HELP_ID_STRUCT *)&m_HelpIdArray
    pContextWnd = NULL;
    return;
}
#endif



