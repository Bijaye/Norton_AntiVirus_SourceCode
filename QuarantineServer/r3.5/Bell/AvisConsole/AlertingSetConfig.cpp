/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/



/*--------------------------------------------------------------------
   AlertingSetConfig.cpp : implementation file

   Property Page Tab Title: "Alerting"

   Use m_pSConfigData->m_dwQueueCheckInterval in CServerConfigData
   for how frequently to walk through the sample queue.







   Written by: Jim Hill                 
--------------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "AvisConsole.h"
#include "AlertingSetConfig.h"
#include "mmc.h"
#include "ParseEventData.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if 0
// Persistant data
// Define
typedef struct 
{
	TCHAR   szAmsServerAddress[255];
    BOOL    m_bCheckEnableAlerts;

}   AVIS_ALERTING_CONFIG;
// Declare
static AVIS_ALERTING_CONFIG  AvisAlertingConfig = {0};
#endif


//extern HWND g_hMainWnd;


extern HRESULT ConfigureAMS ( LPSTR szAppName, LPSTR szHostName, LPSTR szCaption);

/////////////////////////////////////////////////////////////////////////////
// CAlertingSetConfig property page

IMPLEMENT_DYNCREATE(CAlertingSetConfig, CPropertyPage)

CAlertingSetConfig::CAlertingSetConfig() : CPropertyPage(CAlertingSetConfig::IDD)
{
	//{{AFX_DATA_INIT(CAlertingSetConfig)
	m_bCheckEnableAlerts = FALSE;
	m_sAmsServerAddress = _T("");
	m_iAlertCheckInterval = 0;
	m_bCheckNtEventLog = FALSE;
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_bSavedCheckEnableAlerts = FALSE;
    m_sSavedAmsServerAddress  = _T("");
    m_iSavedAlertCheckInterval=0;
    m_pSnapin         = NULL;
    m_dwRef           = 1;
    m_pSConfigData    = NULL;
    m_iTotalListItems = 0;
    m_lpHelpIdArray   = g_AlertingSetConfigPageHelpIdArray;  // LOAD HELP MAP


    //fWidePrintString("CAlertingSetConfig Constructor called  ObjectID= %d", m_iExtDataObjectID);

}

CAlertingSetConfig::~CAlertingSetConfig()
{
    //fWidePrintString("CAlertingSetConfig Destructor called  ObjectID= %d", m_iExtDataObjectID);
}


// DEFINES FOR VARIABLE LENGTHS IN INPUTRANGE.H
void CAlertingSetConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlertingSetConfig)
	DDX_Control(pDX, IDC_SPIN_ALERT_CHECK_INTERVAL1, m_SpinAlertInterval);
	DDX_Control(pDX, IDC_EVENT_LIST1, m_EventList);
	DDX_Control(pDX, IDC_TEST_BUTTON2, m_TestButton1);
	DDX_Check(pDX, IDC_CHECK_ENABLE_ALERTS1, m_bCheckEnableAlerts);
	DDX_Text(pDX, IDC_EDIT_AMS_SERVER_ADDRESS1, m_sAmsServerAddress);
	DDV_MaxChars(pDX, m_sAmsServerAddress, 64);
	DDX_Text(pDX, IDC_EDIT_ALERT_CHECK_INTERVAL1, m_iAlertCheckInterval);
	DDX_Check(pDX, IDC_CHECK_NT_EVENT_LOG, m_bCheckNtEventLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlertingSetConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CAlertingSetConfig)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_TEST_BUTTON2, OnTestButton1)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_ALERTS1, OnCheckEnableAlerts)
	ON_EN_CHANGE(IDC_EDIT_AMS_SERVER_ADDRESS1, OnChangeEditAmsServerAddress)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_ALERT_CHECK_INTERVAL1, OnChangeEditAlertCheckInterval)
	ON_BN_CLICKED(IDC_CHECK_NT_EVENT_LOG, OnCheckNtEventLog)
	ON_BN_CLICKED(IDC_CONFIG_BUTTON, OnConfigButton)
	//}}AFX_MSG_MAP
ON_MESSAGE( WM_USER_INFO_DIRTY, OnListDirty )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlertingSetConfig message handlers



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::PostNcDestroy() 
{
	CPropertyPage::PostNcDestroy();
    Release();     
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAlertingSetConfig::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DWORD dwIndex = 0;
    m_dwRef++; 

	// SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );
    //m_EventList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

    fWidePrintString("CAlertingSetConfig Set up spin control.  ObjectID= %d", m_iExtDataObjectID);
    m_iAlertCheckInterval = ALERTING_MIN_SPIN_ALERT_INTERVAL;
    // SET UP SPIN CONTROL FOR   m_iAlertCheckInterval
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_ALERT_CHECK_INTERVAL1))->SetRange32(ALERTING_MIN_SPIN_ALERT_INTERVAL, ALERTING_MAX_SPIN_ALERT_INTERVAL);
    // Set the buddy
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_ALERT_CHECK_INTERVAL1))->SetBuddy( GetDlgItem(IDC_EDIT_ALERT_CHECK_INTERVAL1) );
    // Set initial pos
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_ALERT_CHECK_INTERVAL1))->SetPos(m_iAlertCheckInterval);  


    // GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
            m_bCheckEnableAlerts  = m_pSConfigData->m_bCheckEnableAlerts;
            m_sAmsServerAddress   = m_pSConfigData->m_sAmsServerAddress;
            m_iAlertCheckInterval = m_pSConfigData->m_iAlertCheckInterval;
            m_bCheckNtEventLog    = m_pSConfigData->m_bCheckNtEventLog;

            // UPDATE THE GLOBALS TO CHECK FOR CHANGES ON OK
            m_bSavedCheckEnableAlerts  = m_bCheckEnableAlerts;
            m_sSavedAmsServerAddress   = m_sAmsServerAddress;
            m_iSavedAlertCheckInterval = m_iAlertCheckInterval;
            m_bSavedCheckNtEventLog    = m_bCheckNtEventLog;

            // SETUP LIST CTRL
            m_iTotalListItems = 0;
            m_EventList.Initialize();
            
            // DO THE SAMPLE ERROR EVENTS FIRST, SO THEY WILL BE LAST IN THE LISTBOX
            for( dwIndex = 0 ;; )
            {
				CEventObject *pEventObj = new CEventObject;
                if( GetEventDataByIndex( EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE, pEventObj, &dwIndex ) )
                {
        			m_EventList.InsertEventItem(pEventObj);
                }
                else
                    delete pEventObj;
        
                // ARE WE AT THE END OF THE TABLE?
                if( dwIndex == 0xFFFFFFFF )
                {
                    break;
                }

            }

            // DO THE GENERAL ATTENTION EVENTS
            for( dwIndex = 0 ;; )
            {
				CEventObject *pEventObj = new CEventObject;
                if( GetEventDataByIndex( EVENT_TOKEN_GENERAL_ATTENTION_TABLE, pEventObj, &dwIndex ) )
                {
        			m_EventList.InsertEventItem(pEventObj);
                }
                else
                    delete pEventObj;
        
                // ARE WE AT THE END OF THE TABLE?
                if( dwIndex == 0xFFFFFFFF )
                {
                    break;
                }

            }

            // SELECT THE FIRST ENTRY
            m_EventList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
            m_iTotalListItems = m_EventList.GetItemCount();

        }
    }


    // FORCE IT TO REDISPLAY THE DATA
    UpdateData( FALSE );         

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAlertingSetConfig::OnApply() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    // Make sure data is good.
    UpdateData(TRUE);

    SaveChangedData(); 

    // WRITE CHANGES TO THE REMOTE REGISTRY
    if( m_pSConfigData )
    {
        // UPDATE THE GLOBAL DATA
        SaveEventTableTableData();

        if( m_pSConfigData->IsDirty() )   
        {
            HRESULT hr = m_pSConfigData->WriteConfigDataToServer();
        }

        // DISABLE APPLY BUTTON IF WE SUCCEEDED
      	SetModified(FALSE);
    }

	return CPropertyPage::OnApply();
}

/*----------------------------------------------------------------------------
   SaveEventTableTableData()
   Update the global Event Table data

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAlertingSetConfig::SaveEventTableTableData()
{
    CEventObject *pEventObj      = NULL;
    LPARAM        lParam         = 0;
    DWORD         dwChangedCount = 0;

    try
    {
        m_iTotalListItems = m_EventList.GetItemCount();
        if( m_iTotalListItems == 0 )   
            return(TRUE);

        for( int i = 0; i < m_iTotalListItems ; i++)
    	{   
            // GET THE LPARAM DATA
            lParam = m_EventList.GetItemData(i); 
            if( lParam != LB_ERR  && lParam != 0)
            {
                pEventObj = (CEventObject*)lParam;
                PutEventDataByIndex( pEventObj, i, &dwChangedCount );
            }
        }

        // NEED TO WRITE IT?
        if(dwChangedCount)  
            m_pSConfigData->SetDirty(TRUE);

    }
    catch(...)
    {
        return(FALSE);
    }
    return(TRUE);
}

/*----------------------------------------------------------------------------
   FreeDataList()
   Free each CEventObject in the list control

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAlertingSetConfig::FreeDataList()
{
    CEventObject *pEventObj    = NULL;
    LPARAM        lParam       = 0;

    try
    {
        m_iTotalListItems = m_EventList.GetItemCount();
        if( m_iTotalListItems == 0 )   
            return(TRUE);

        for( int i = 0; i < m_iTotalListItems ; i++)
    	{   
            // GET THE LPARAM DATA
            lParam = m_EventList.GetItemData(i); 
            if( lParam != LB_ERR  && lParam != 0)
            {
                pEventObj = (CEventObject*)lParam;
                delete pEventObj;
            }
        }

    }
    catch(...)
    {
        return(FALSE);
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
   SaveChangedData()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::SaveChangedData() 
{
	DWORD dwDataChanged=0;



    // SAVE THE PERSISTANT DATA THAT HAS BEEN CHANGED
    if( m_bCheckEnableAlerts != m_bSavedCheckEnableAlerts )
    {
        m_bSavedCheckEnableAlerts  = m_bCheckEnableAlerts;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_bCheckEnableAlerts = m_bCheckEnableAlerts;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_ENABLE_ALERTS );
    }

    if( m_sAmsServerAddress != m_sSavedAmsServerAddress )
    {
        m_sSavedAmsServerAddress   = m_sAmsServerAddress;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_sAmsServerAddress = m_sAmsServerAddress;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_AMS_SERVER_ADDRESS );
    }

    if( m_iAlertCheckInterval != m_iSavedAlertCheckInterval )
    {
        m_iSavedAlertCheckInterval = m_iAlertCheckInterval;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_iAlertCheckInterval = m_iAlertCheckInterval;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_ALERTING_INTERVAL );
    }

    if( m_bCheckNtEventLog != m_bSavedCheckNtEventLog )
    {
        m_bSavedCheckNtEventLog = m_bCheckNtEventLog; // RESET
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_bCheckNtEventLog = m_bCheckNtEventLog;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_ENABLE_NT_EVENT_LOG );
    }


    // NEED TO WRITE IT?
    if(dwDataChanged)  
        m_pSConfigData->SetDirty(TRUE);

    return;
}  


/*----------------------------------------------------------------------------

   Written by: Jim Hill                               
----------------------------------------------------------------------------*/
BOOL CAlertingSetConfig::OnKillActive() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    BOOL   bRet  = TRUE;
    BOOL   bRet2 = TRUE;
    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;

    // Now, get the data
    UpdateData(TRUE);

    // ENABLE AND AMS NAME
    // BOOL	bCurrentCheckEnableAlerts  = GetDlgItemInt( IDC_CHECK_ENABLE_ALERTS1, &bRet, FALSE );
    // CString	sCurrentAmsServerAddress   = GetDlgItemInt( IDC_EDIT_AMS_SERVER_ADDRESS1, &bRet2, FALSE );
    // if( !bRet || !bRet2 || (bCurrentCheckEnableAlerts && sCurrentAmsServerAddress.IsEmpty()) )

    // ENABLE AND AMS NAME
    if( m_bCheckEnableAlerts && m_sAmsServerAddress.IsEmpty() )
    {
        // Inform the user 
        sText.LoadString( IDS_BADFMT_ENABLE_AMS );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_CHECK_ENABLE_ALERTS1;
    }

    // ALERT INTERVAL
    long iCurrentAlertCheckInterval = GetDlgItemInt( IDC_EDIT_ALERT_CHECK_INTERVAL1, &bRet, FALSE );
    if( !bRet || ( iCurrentAlertCheckInterval < ALERTING_MIN_SPIN_ALERT_INTERVAL || iCurrentAlertCheckInterval > ALERTING_MAX_SPIN_ALERT_INTERVAL ) )
    {
        // Inform the user 
        sText.Format( IDS_BADFMT_ALERTING_INTERVAL, ALERTING_MIN_SPIN_ALERT_INTERVAL, ALERTING_MAX_SPIN_ALERT_INTERVAL );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_ALERT_CHECK_INTERVAL1;
    }

    // RESET THE CONTROL
    if( iControl )
    {
        GetDlgItem( iControl )->SetFocus();
        ((CEdit*) GetDlgItem( iControl) )->SetSel( 0, -1 );
        return FALSE;
    }

	return CPropertyPage::OnKillActive();
}



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::OnDestroy() 
{
	CPropertyPage::OnDestroy();
	
    FreeDataList();
	
}


/*----------------------------------------------------------------------------
   Written by: Terry Marles
----------------------------------------------------------------------------*/
afx_msg LRESULT CAlertingSetConfig::OnListDirty(WPARAM wParam, LPARAM lParam)
{
	SetModified(TRUE);	
	return S_OK;
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::OnOK() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	
//    FreeDataList();

	CPropertyPage::OnOK();
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::OnCancel() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	
//    FreeDataList();
   
	CPropertyPage::OnCancel();
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::OnCheckEnableAlerts() 
{
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::OnChangeEditAmsServerAddress() 
{
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::OnChangeEditAlertCheckInterval() 
{
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAlertingSetConfig::OnCheckNtEventLog() 
{
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------
    SendTestEvent()

    Send a test event to the AMS server to validate the system.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CAlertingSetConfig::SendTestEvent() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = 0;

    try
    {
        if( m_pSConfigData )
        {
            m_pSConfigData->m_bSendTestEvent = TRUE;
            m_pSConfigData->SetDirty(TRUE);
            hr = m_pSConfigData->WriteConfigDataToServer();  // TELL THE HIDDEN WIN TO WRITE IT

            CString  sTitle;      // _T("Send Test Alert");
            CString  sText;       // _T("A test alert will be sent to the AMS server, the next scheuled time Quarantine Server is configured to look for events.");
            StrTableLoadString( IDSTABLE_SEND_TEST_EVENTNAME, sTitle );   
            sText.LoadString( IDS_SEND_TEST_EVENT_MSG );
            MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );    // 
        }
    }
    catch(...) 
    {
    }
    return;
}



#ifdef MY_SYSTEM

#include "AvisEventDetection.h"   // in QuarantineServer\Include
#include "EventObject.h"
void CAlertingSetConfig::OnTestButton1() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

    SendTestEvent(); 
    // CEventDetect::TestDetectAllEvents();
    return;
}

#else

void CAlertingSetConfig::OnTestButton1() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    SendTestEvent(); 

    return;
}

#endif





#if 0
#ifdef MY_SYSTEM
    #include "AvisEventDetection.h"   // in QuarantineServer\Include
#endif

void CAlertingSetConfig::OnTestButton1() 
{
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 

#ifdef MY_SYSTEM
    //SendTestMail();
    //CEventDetect::DetectGeneralErrors();
    CEventDetect::TestDetectAllEvents();
#endif


    return;
}
#endif




// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
BOOL CAlertingSetConfig::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD, m_lpHelpIdArray, pHelpInfo );
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}



// PROP SHEET HELP BUTTON
BOOL CAlertingSetConfig::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	NMHDR* pNMHDR = (NMHDR*)lParam;
    DoHelpPropSheetHelpButton( IDD, m_lpHelpIdArray, pNMHDR );
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}


static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;

void CAlertingSetConfig::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
    return;
}

#if 0
void CAlertingSetConfig::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR
    if( nResourceID == 0)
        nResourceID = IDD;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );
    return;
}
#endif

void CAlertingSetConfig::OnConfigButton() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
USES_CONVERSION;    
	CString sCaption;
	CString sAMSProductName;
    sCaption.LoadString(IDS_AMS_CONFIG_CAPTION);
	
	if(m_sAmsServerAddress.IsEmpty())
	{
		CString sTitle((LPCTSTR) IDS_ERROR_TITLE );
		CString sText((LPCTSTR) IDS_NO_AMS_SERVER_CONFIGURED );
		
		// 
		// There is no AMS server.
		// 
		MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
		return ;
	}
	sAMSProductName.LoadString(IDS_QSEVENT_PRODUCT_NAME);
	m_sAmsServerAddress.MakeUpper();
	ConfigureAMS(T2A(sAMSProductName.GetBuffer(0)),T2A(m_sAmsServerAddress.GetBuffer(0)),T2A(sCaption.GetBuffer(0)));

    return;
}
