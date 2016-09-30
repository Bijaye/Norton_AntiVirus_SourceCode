/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


// AvisServerConfig1.cpp : implementation file
//
/*--------------------------------------------------------------------
   AvisServerConfig1.cpp : implementation file

   Property Page Tab Title: "Sample Policies"

   Written by: Jim Hill                 
--------------------------------------------------------------------*/

#include "stdafx.h" 
#include "resource.h"
#include "AvisConsole.h"
#include "AvisServerConfig1.h"
#include "InputRange.h"
#include "mmc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





// Persistant data
// Define
typedef struct tagAVISDATACONFIG1
{
	// BOOL	bCompressUserData;
	// BOOL	bScrambleUserData;
	BOOL	bStripUserData;
	DWORD	dwQueueCheckInterval;
	// DWORD	dwMaxPendingSamples;
	DWORD	dwInitialSubmissionPriority;
    BOOL    bAutoSubmit;
    int     iStatusInterval;

}   AVIS_DATA_CONFIG1;
// Declare
AVIS_DATA_CONFIG1  AvisConfig1={0};
//static DWORD dwAvisConfig1DefaultsSet=FALSE;



/////////////////////////////////////////////////////////////////////////////
// CAvisServerConfig property page

IMPLEMENT_DYNCREATE(CAvisServerConfig, CPropertyPage)

CAvisServerConfig::CAvisServerConfig() : CPropertyPage(CAvisServerConfig::IDD)
{
	//{{AFX_DATA_INIT(CAvisServerConfig)
	m_bStripUserData = FALSE;
	m_dwQueueCheckInterval = 0;
	m_iStatusInterval = 0;
	m_bAutoSubmit = FALSE;
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_pSnapin            = NULL;
    m_dwRef              = 1;
    m_pSConfigData       = NULL;
    m_bCustEmailFilledIn = FALSE;
    m_lpHelpIdArray      = g_SamplePolicyHelpIdArray;  // LOAD THE HELP MAP  g_AvisServerConfigHelpIdArray



    // fWidePrintString("CAvisServerConfig Constructor called  ObjectID= %d", m_iExtDataObjectID);

}

CAvisServerConfig::~CAvisServerConfig()
{
    //fWidePrintString("CAvisServerConfig Destructor called  ObjectID= %d", m_iExtDataObjectID);
}


void CAvisServerConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvisServerConfig)
	DDX_Control(pDX, IDC_SPIN_STATUS_INTERVAL5, m_SpinStatusInterval);
	DDX_Control(pDX, IDC_SPIN1, m_SpinQueueCheckInterval);
	DDX_Check(pDX, IDC_CHECK_STRIP_USER_DATA, m_bStripUserData);
	DDX_Text(pDX, IDC_EDIT_QUEUE_CHECK_INTERVAL, m_dwQueueCheckInterval);
	DDX_Text(pDX, IDC_EDIT_STATUS_INTERVAL2, m_iStatusInterval);
	DDV_MinMaxInt(pDX, m_iStatusInterval, WEB_MIN_SPIN_STATUS_INTERVAL, WEB_MAX_SPIN_STATUS_INTERVAL);
	DDX_Check(pDX, IDC_CHECK_AUTO_SUBMIT, m_bAutoSubmit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAvisServerConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CAvisServerConfig)
	ON_EN_CHANGE(IDC_EDIT_QUEUE_CHECK_INTERVAL, OnChangeEditQueueCheckInterval)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_EN_CHANGE(IDC_EDIT_STATUS_INTERVAL2, OnChangeEditStatusInterval2)
	ON_BN_CLICKED(IDC_CHECK_AUTO_SUBMIT, OnCheckAutoSubmit)
	ON_BN_CLICKED(IDC_CHECK_STRIP_USER_DATA, OnCheckStripUserData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvisServerConfig message handlers



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::PostNcDestroy();

    Release();       
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
    CString  szDisplayName;
    m_dwRef++;	
   
    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // hr = m_pSConfigData->ReadConfigDataFromServer();
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
            m_bStripUserData             = m_pSConfigData->m_bStripUserData             ;
            m_dwQueueCheckInterval       = m_pSConfigData->m_dwQueueCheckInterval       ;
            m_iStatusInterval            = m_pSConfigData->m_iStatusInterval;
            m_bAutoSubmit                = (m_pSConfigData->m_dwInitialSubmissionPriority != 0);
            // m_dwInitialSubmissionPriority= m_pSConfigData->m_dwInitialSubmissionPriority;
            // m_dwMaxPendingSamples        = m_pSConfigData->m_dwMaxPendingSamples        ;
            // m_bCompressUserData          = m_pSConfigData->m_bCompressUserData          ;
            // m_bScrambleUserData          = m_pSConfigData->m_bScrambleUserData          ;

            // UPDATE THE GLOBALS TO CHECK FOR CHANGES ON OK
            AvisConfig1.bStripUserData              = m_bStripUserData             ; 
            AvisConfig1.dwQueueCheckInterval        = m_dwQueueCheckInterval       ; 
            AvisConfig1.iStatusInterval             = m_iStatusInterval;
            AvisConfig1.dwInitialSubmissionPriority = m_pSConfigData->m_dwInitialSubmissionPriority;
            AvisConfig1.bAutoSubmit                 = m_bAutoSubmit;
            // AvisConfig1.dwMaxPendingSamples         = m_dwMaxPendingSamples        ; 
            // AvisConfig1.bCompressUserData           = m_bCompressUserData             ; 
            // AvisConfig1.bScrambleUserData           = m_bScrambleUserData             ; 

            // VERIFY THAT CUSTOMER INFO IS FILLED IN 12/11/99
            m_bCustEmailFilledIn = !m_pSConfigData->m_szContactEmail.IsEmpty();

        }
    }


 	// Spin control for m_iStatusInterval 
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_STATUS_INTERVAL5))->SetRange32( WEB_MIN_SPIN_STATUS_INTERVAL, WEB_MAX_SPIN_STATUS_INTERVAL );
    // Set the buddy                                                     
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_STATUS_INTERVAL5))->SetBuddy( GetDlgItem(IDC_EDIT_STATUS_INTERVAL2));
    // Set initial pos
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_STATUS_INTERVAL5))->SetPos(m_iStatusInterval);

    // // SET UP SPIN CONTROL FOR INITIAL SUBMISSION PRIORITY.
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN2))->SetRange32(POLICY_MIN_SPIN_INITIAL_SUBMISSION_PRIORITY, POLICY_MAX_SPIN_INITIAL_SUBMISSION_PRIORITY);
    // // Set the buddy
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN2))->SetBuddy( GetDlgItem(IDC_EDIT_SUBMISSION_PRIORITY) );
    // // Set initial pos
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN2))->SetPos(m_dwInitialSubmissionPriority);
    // 
	// // FOR MAX PENDING SAMPLES
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN3))->SetRange32(POLICY_MIN_SPIN_PENDING_SAMPLES, POLICY_MAX_SPIN_PENDING_SAMPLES);
    // // Set the buddy
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN3))->SetBuddy( GetDlgItem(IDC_EDIT_MAX_PENDING_SAMPLES) );
    // // Set initial pos
    // ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN3))->SetPos(m_dwMaxPendingSamples);

	// FOR SAMPLE QUEUE CHECK INTERVAL
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN1))->SetRange32(POLICY_MIN_SPIN_QUEUE_CHECK_INTERVAL, POLICY_MAX_SPIN_QUEUE_CHECK_INTERVAL);
    // Set the buddy
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN1))->SetBuddy( GetDlgItem(IDC_EDIT_QUEUE_CHECK_INTERVAL) );
    // Set initial pos
    ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN1))->SetPos(m_dwQueueCheckInterval);


    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig::OnOK() 
{

	CPropertyPage::OnOK();
}  

 
/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig::SaveChangedData() 
{
	DWORD dwDataChanged=0;

    // SAVE THE PERSISTANT DATA THAT HAS BEEN CHANGED
    // //  m_bCompressUserData                                     "Sample Policy"
    // if(AvisConfig1.bCompressUserData != m_bCompressUserData) 
    // {
    //     AvisConfig1.bCompressUserData = m_bCompressUserData;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_bCompressUserData = m_bCompressUserData;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_COMPRESS_USER_DATA );
    // }
    // //   m_bScrambleUserData                                    "Sample Policy"
    // if(AvisConfig1.bScrambleUserData != m_bScrambleUserData) 
    // {
    //     AvisConfig1.bScrambleUserData = m_bScrambleUserData;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_bScrambleUserData = m_bScrambleUserData;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_SCRAMBLE_USER_DATA );
    // }
    // // MAX PENDING SAMPLES                                  "Sample Policy"
    // if(AvisConfig1.dwMaxPendingSamples != m_dwMaxPendingSamples) 
    // {
    //     fWidePrintString("DATA SAVED CAvisServerConfig::m_dwMaxPendingSamples  Old=%d  New=%d",
    //                   AvisConfig1.dwMaxPendingSamples,m_dwMaxPendingSamples);
    //     AvisConfig1.dwMaxPendingSamples = m_dwMaxPendingSamples;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData-> m_dwMaxPendingSamples= m_dwMaxPendingSamples;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_MAX_PENDINGSAMPLES );
    // }
    // // INITIAL SUBMISSION PRIORITY                          "Sample Policy"
    // if(AvisConfig1.dwInitialSubmissionPriority != m_dwInitialSubmissionPriority) 
    // {
    //     fWidePrintString("DATA SAVED CAvisServerConfig::m_dwInitialSubmissionPriority  Old=%d  New=%d",
    //                   AvisConfig1.dwInitialSubmissionPriority,m_dwInitialSubmissionPriority);
    //     AvisConfig1.dwInitialSubmissionPriority = m_dwInitialSubmissionPriority;
    //     dwDataChanged = TRUE;
    //     // SAVE IT TO CServerConfigData
    //     m_pSConfigData->m_dwInitialSubmissionPriority = m_dwInitialSubmissionPriority;
    //     // SET FLAG FOR THE WRITE ROUTINE
    //     m_pSConfigData->SetFieldDirty( FIELD_CHANGE_INITIAL_SUBMISSIONPRIORITY );
    // }


     // INITIAL SUBMISSION PRIORITY                          "Sample Policy"     
     if(AvisConfig1.bAutoSubmit != m_bAutoSubmit) 
     {
         AvisConfig1.bAutoSubmit = m_bAutoSubmit;
         dwDataChanged = TRUE;
         // SAVE IT TO CServerConfigData
         if( m_bAutoSubmit )
             m_pSConfigData->m_dwInitialSubmissionPriority = POLICY_DEFAULT_INITIAL_SUBMISSION_PRIORITY;
         else
             m_pSConfigData->m_dwInitialSubmissionPriority = 0;
         // SET FLAG FOR THE WRITE ROUTINE
         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_INITIAL_SUBMISSIONPRIORITY );
     }

     // STRIP USER DATA                                      "Sample Policy"
     if(AvisConfig1.bStripUserData != m_bStripUserData) 
     {
         AvisConfig1.bStripUserData = m_bStripUserData;
         dwDataChanged = TRUE;
         // SAVE IT TO CServerConfigData
         m_pSConfigData->m_bStripUserData = m_bStripUserData;
         // SET FLAG FOR THE WRITE ROUTINE
         m_pSConfigData->SetFieldDirty( FIELD_CHANGE_STRIP_USER_DATA );
     }

    // QUEUE CHECK INTERVAL                                 "Sample Policy"
    if(AvisConfig1.dwQueueCheckInterval != m_dwQueueCheckInterval) 
    {
        fWidePrintString("DATA SAVED CAvisServerConfig::m_dwQueueCheckInterval  Old=%d  New=%d",
                      AvisConfig1.dwQueueCheckInterval,m_dwQueueCheckInterval);
        AvisConfig1.dwQueueCheckInterval = m_dwQueueCheckInterval;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_dwQueueCheckInterval = m_dwQueueCheckInterval;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_QUEUECHECK_INTERVAL );
    }

    //  CAvisServerConfig2    m_iStatusInterval             "Sample Policy"
    if(AvisConfig1.iStatusInterval != m_iStatusInterval) 
    {
        AvisConfig1.iStatusInterval = m_iStatusInterval;
        dwDataChanged = TRUE;
        // SAVE IT TO CServerConfigData
        m_pSConfigData->m_iStatusInterval = m_iStatusInterval;
        // SET FLAG FOR THE WRITE ROUTINE
        m_pSConfigData->SetFieldDirty( FIELD_CHANGE_STATUS_INTERVAL );   
    }

    // NEED TO WRITE IT?
    if(dwDataChanged)  
        m_pSConfigData->SetDirty(TRUE);

    return;
}  




/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig::OnApply() 
{

    // Make sure data is good.
    UpdateData(TRUE);

    SaveChangedData(); 

    // 7/8/99
    // WRITE CHANGES TO THE REMOTE REGISTRY
    if( m_pSConfigData )
    {
        if( m_pSConfigData->IsDirty() )   // 1/2/00 jhill
        {
            HRESULT hr = m_pSConfigData->WriteConfigDataToServer();
        }

        // DISABLE APPLY BUTTON IF WE SUCCEEDED
      	SetModified(FALSE);
    }


	return CPropertyPage::OnApply();
}




/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 


    BOOL   bRet  = 0;
    CString sTitle, sText;
    sTitle.LoadString( IDS_CONFIG_ERROR );
    int iControl = 0;
    
    // INITIAL SUBMISSION PRIORITY
    // DWORD dwInitialSubmissionPriority = GetDlgItemInt( IDC_EDIT_SUBMISSION_PRIORITY, &bRet, FALSE );
    // if( !bRet || ( dwInitialSubmissionPriority < POLICY_MIN_SPIN_INITIAL_SUBMISSION_PRIORITY || dwInitialSubmissionPriority > POLICY_MAX_SPIN_INITIAL_SUBMISSION_PRIORITY ) )
    // {
    //     // Inform the user 
    //     sText.Format( IDS_BADFMT_INITIAL_SUBMIT_PRIORITY, POLICY_MIN_SPIN_INITIAL_SUBMISSION_PRIORITY, POLICY_MAX_SPIN_INITIAL_SUBMISSION_PRIORITY );
    //     MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //     iControl = IDC_EDIT_SUBMISSION_PRIORITY;
    // }
    // // VERIFY THAT CUSTOMER INFO IS FILLED IN BEFORE ALLOWING SUBMISSION PRIORITY TO BE SET 12/11/99
    // else 
    // {
    //     m_bCustEmailFilledIn = !m_pSConfigData->m_szContactEmail.IsEmpty();
    //     if( !m_bCustEmailFilledIn && dwInitialSubmissionPriority != 0 )
    //     {
    //         // Inform the user 
    //         sText.LoadString( IDS_CUST_INFO_BLANK_EMAIL2 );
    //         MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //         //iControl = IDC_EDIT_SUBMISSION_PRIORITY;          // DON'T RESET CONTROL 12/11/99
    //         iControl = IDC_EDIT_SUBMISSION_PRIORITY;
    //     
    //         // Clear the value  
    //         // if(AvisConfig1.dwInitialSubmissionPriority == 0 )   // they just tried to change it
    //         {
    //             m_dwInitialSubmissionPriority = 0;
    //             SetDlgItemInt( IDC_EDIT_SUBMISSION_PRIORITY, 0, FALSE );
    //         }
    //     
    //     }
    // }
    // MAXIMUM PENDING SAMPLES
    // DWORD dwMaxPendingSamples = GetDlgItemInt( IDC_EDIT_MAX_PENDING_SAMPLES, &bRet, FALSE );
    // if( !bRet || ( dwMaxPendingSamples < POLICY_MIN_SPIN_PENDING_SAMPLES || dwMaxPendingSamples > POLICY_MAX_SPIN_PENDING_SAMPLES ) )
    // {
    //     // Inform the user 
    //     sText.Format( IDS_BADFMT_MAX_PENDING_SAMPLES, POLICY_MIN_SPIN_PENDING_SAMPLES, POLICY_MAX_SPIN_PENDING_SAMPLES );
    //     MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    //     iControl = IDC_EDIT_MAX_PENDING_SAMPLES;
    // }

    // QUEUE CHECK INTERVAL
    DWORD dwQueueCheckInterval = GetDlgItemInt( IDC_EDIT_QUEUE_CHECK_INTERVAL, &bRet, FALSE );
    if( !bRet || ( dwQueueCheckInterval < POLICY_MIN_SPIN_QUEUE_CHECK_INTERVAL || dwQueueCheckInterval > POLICY_MAX_SPIN_QUEUE_CHECK_INTERVAL ) )
    {
        // Inform the user 
        sText.Format( IDS_BADFMT_QUEUE_CHECK_INTERVAL, POLICY_MIN_SPIN_QUEUE_CHECK_INTERVAL, POLICY_MAX_SPIN_QUEUE_CHECK_INTERVAL );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_QUEUE_CHECK_INTERVAL;
    }

    //  STATUS INTERVAL
    int iStatusInterval = GetDlgItemInt( IDC_EDIT_STATUS_INTERVAL2, &bRet, FALSE );
    if( !bRet || ( iStatusInterval < WEB_MIN_SPIN_STATUS_INTERVAL || iStatusInterval > WEB_MAX_SPIN_STATUS_INTERVAL ) )
    {
        // Inform the user 
        sText.Format( IDS_BADFMT_STATUS_INTERVAL, WEB_MIN_SPIN_STATUS_INTERVAL, WEB_MAX_SPIN_STATUS_INTERVAL );
        MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
        iControl = IDC_EDIT_STATUS_INTERVAL2;
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
void CAvisServerConfig::OnCheckStripUserData() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);

}

/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig::OnCheckAutoSubmit() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig::OnChangeEditQueueCheckInterval() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}


void CAvisServerConfig::OnChangeEditStatusInterval2() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}


// void CAvisServerConfig::OnChangeEditMaxPendingSamples() 
// {
// 	// TODO: Add your control notification handler code here
// 	SetModified(TRUE);
// }
// void CAvisServerConfig::OnChangeEditSubmissionPriority() 
// {
// 	// TODO: Add your control notification handler code here
// 	SetModified(TRUE);
// }
// void CAvisServerConfig::OnCheckCompressUserData() 
// {
// 	// TODO: Add your control notification handler code here
// 	SetModified(TRUE);
// }
// void CAvisServerConfig::OnCheckScrambleUserData() 
// {
// 	// TODO: Add your control notification handler code here
// 	SetModified(TRUE);
// }














// HELP HANDLERS AND ROUTINES 

/*----------------------------------------------------------------------------
   CAvisServerConfig::OnHelpInfo()
   F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    DoHelpPropSheetF1( IDD_AVIS_POLICY_PAGE, m_lpHelpIdArray, pHelpInfo );
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}


/*----------------------------------------------------------------------------
   CAvisServerConfig::OnNotify()
   PROP SHEET HELP BUTTON
   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CAvisServerConfig::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    NMHDR* pNMHDR = (NMHDR*)lParam;
    DoHelpPropSheetHelpButton(IDD_AVIS_POLICY_PAGE, m_lpHelpIdArray, pNMHDR );
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}


static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;

/*----------------------------------------------------------------------------
   CAvisServerConfig::OnContextMenu()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD_AVIS_POLICY_PAGE, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
    return;
}


#if 0
/*----------------------------------------------------------------------------
   CAvisServerConfig::OnWhatsThisMenuitem()

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CAvisServerConfig::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR IT
    if( nResourceID == 0)
        nResourceID = IDD_AVIS_POLICY_PAGE;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );
    pContextWnd = NULL;
    return;
}
#endif



