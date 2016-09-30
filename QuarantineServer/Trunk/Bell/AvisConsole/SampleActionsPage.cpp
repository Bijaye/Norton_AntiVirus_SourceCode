/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// SampleActionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AvisConsole.h"
#include "SampleActionsPage.h"
#include "InputRange.h"
#include "mmc.h"

#include "GetAllAttributes.h"
#include "IcePackTokens.h"
#include "avisregval.h"
#include "qscommon.h"

// Handle to main console window.
extern HWND g_hMainWnd;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Persistant data
// Define
// typedef struct tagSAMPLEACTIONS
// {
// 	DWORD	dwSubmissionPriority;
// 	BOOL	m_bAutoDelivery;
// 
// }   SAMPLE_ACTIONS_CONFIG;
// // Declare
// SAMPLE_ACTIONS_CONFIG  SampleActionsConfig={0};
// static DWORD dwSampleActionsDefaultsSet=FALSE;



#define ACTION_QUEUE_SAMPLE_FOR_SUBMISSION      1
#define ACTION_RESUBMIT_SAMPLE                  2
#define ACTION_REMOVE_FROM_SUBMISSION_QUEUE     3
#define ACTION_REMOVE_FROM_DELIVERY_QUEUE       4



/////////////////////////////////////////////////////////////////////////////
// CSampleActionsPage property page

IMPLEMENT_DYNCREATE(CSampleActionsPage, CPropertyPage)

CSampleActionsPage::CSampleActionsPage() : CPropertyPage(CSampleActionsPage::IDD)
{
	//{{AFX_DATA_INIT(CSampleActionsPage)
	m_szAnalysisState = _T("");
	m_szDefSequenceNeeded = _T("");
	m_VirusName = _T("");
	m_dwSubmissionPriority = 0;
	m_sFilename = _T("");
	m_sSampleStatus = _T("");
	m_sTrackingNumber = _T("");
	m_sStatusEx = _T("");
	//}}AFX_DATA_INIT

	// DEFAULT SETTINGS
	m_bAutoDelivery = FALSE;
	m_szAnalysisState.LoadString(IDS_STATUS_NOT_AVAILABLE);   
    m_szDefSequenceNeeded.LoadString(IDS_STATUS_NOT_AVAILABLE); 
    m_VirusName.LoadString(IDS_STATUS_UNKNOWN2); 
	m_sSampleStatus.LoadString(IDS_STATUS_NOT_AVAILABLE); 
	m_sTrackingNumber.LoadString(IDS_SAMPLE_NOT_SUBMITTED);     //        IDS_STATUS_NOT_AVAILABLE
    

    m_pSnapin       = NULL;
    m_dwRef         = 1;
    m_dwRegistered  = 0;
    m_lpHelpIdArray = g_SampleActionsHelpIdArray;  // LOAD THE HELP MAP
    m_pQServerGetAllAttributes = NULL;
    m_pAttributeAccess         = NULL;
    m_dwSignaturesPriority     = 0;
    m_dwStatus                 = 0;
    m_bIsXdateSubmitted        = FALSE;
    m_bOkToReSubmit            = FALSE;
    m_dwSubmissionCount        = 0;
    m_dwActionToTake           = 0;

    //dwSampleActionsDefaultsSet=FALSE;
    memset(&SampleActionsConfig , 0, sizeof(SampleActionsConfig));

    //fPrintString("CSampleActionsPage Constructor called");
}

CSampleActionsPage::~CSampleActionsPage()
{
    //fPrintString("CSampleActionsPage Destructor called");
  	//fWidePrintString("CSampleActionsPage destructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
}

void CSampleActionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleActionsPage)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_IconButton);
	DDX_Control(pDX, IDC_SPIN1, m_SpinSubmissionPriority);
	DDX_Text(pDX, IDC_ANALYSIS_STATE, m_szAnalysisState);
	DDX_Text(pDX, IDC_DEF_SEQUENCE_NEEDED, m_szDefSequenceNeeded);
	DDX_Text(pDX, IDC_VIRUS_NAME, m_VirusName);
	DDX_Text(pDX, IDC_SAMPLE_SUBMISSION_PRIORITY, m_dwSubmissionPriority);
	DDV_MinMaxDWord(pDX, m_dwSubmissionPriority, 0, 1000);
	DDX_Text(pDX, IDC_FILENAME_EDIT, m_sFilename);
	DDV_MaxChars(pDX, m_sFilename, 259);
	DDX_Text(pDX, IDC_SAMPLE_STATUS, m_sSampleStatus);
	DDX_Text(pDX, IDC_ISSUE_NUMBER, m_sTrackingNumber);
	DDX_Text(pDX, IDC_SAMPLE_STATUS_EX, m_sStatusEx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSampleActionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSampleActionsPage)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_CHECK_SAMPLE_AUTO_DELIVERY, OnCheckSampleAutoDelivery)
	ON_EN_CHANGE(IDC_SAMPLE_SUBMISSION_PRIORITY, OnChangeSampleSubmissionPriority)
	ON_BN_CLICKED(IDC_ACTIONS_SUBMIT_BUTTON, OnActionsSubmitButton)
	ON_BN_CLICKED(IDC_ACTIONS_DELIVER_BUTTON, OnActionsDeliverButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleActionsPage message handlers


LONG CSampleActionsPage::Release() 
{ 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());   

    try
    {   
        if( --m_dwRef == 0 )
        {
		    fWidePrintString("CSampleActionsPage::Release called. Deleting ObjectID= %d", m_iExtDataObjectID);
        
            // TELL THE ACCESS ATTRIBUTE OBJECT WE'RE NOT USING IT ANYMORE
            if( m_pAttributeAccess )
            {
                //if( IsPageRegistered() )
                m_pAttributeAccess->Release();
                m_pAttributeAccess = NULL;
            }
        
            delete this;
            return 0;
        }
    }
    catch(...) 
    {
    }                        

    return m_dwRef;
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::PostNcDestroy();

    //fPrintString("CSampleActionsPage::PostNcDestroy()  called");
    Release();      
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::FillAttributeData() 
{
    HRESULT hr       = S_OK;
    //TCHAR *endptr  = NULL;
    VARIANT v;
	USES_CONVERSION;
    try
    {
        if( m_pQServerGetAllAttributes == NULL )
            return(FALSE);
        if( !((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->IsDataAvailable() )
            return(FALSE);
    
        VariantInit( &v );
        VariantClear( &v );


        // HAS THIS SAMPLE BEEN SUBMITTED  3/27/00
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_X_DATE_SUBMITTED, &v );
        if( IsSuccessfullResult( hr ) && v.vt == VT_DATE)
        {
            m_bIsXdateSubmitted = (v.date != 0);
            VariantClear( &v );
        }

         
        // GET THE SAMPLE STATUS 3/27/00 
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_X_SAMPLE_STATUS, &v );
        if( IsSuccessfullResult( hr ) && v.vt == VT_UI4)
        {
            m_dwStatus = v.ulVal;
            ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetStatusText( m_sSampleStatus, m_dwStatus );
            VariantClear( &v );
        }


        // GET THE TRACKING NUMBER m_sTrackingNumber   3/27/00
        // ONLY GET THE TRACKING NUMBER IF THE SAMPLE HAS BEEN SUBMITTED
        //if( m_dwStatus != AVIS_SAMPLE_STATUS_QUARANTINED )
        if( m_bIsXdateSubmitted )
        {
            // NEXT THE MD5 CHECKSUM
            CString sMD5;
            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            GetValueByAttributeName( AVIS_X_SAMPLE_CHECKSUM, &v );
            if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)  
            {
                sMD5 = v.bstrVal;
                //m_sTrackingNumber += sMD5.Left(5);    
                m_sTrackingNumber = _T("");
                sMD5.MakeUpper();
                // MD5 IS 32 CHRS LONG
                for(int i = 0; i < 8; i++)
                {
                   m_sTrackingNumber += sMD5.Mid((i*4), 4 );
                   if( i < 7 )
                       m_sTrackingNumber += _T("-");

                }
                VariantClear( &v );
            }
        }


        // // GET THE TRACKING NUMBER m_sTrackingNumber   3/27/00
        // // ONLY GET THE TRACKING NUMBER IF THE SAMPLE HAS BEEN SUBMITTED
        // if( m_bIsXdateSubmitted )
        // {
        //     // // FIRST, THE CUSTOMER CONTACT EMAIL ADDRESS    
        //     // hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
        //     //                 GetValueByAttributeName( AVIS_X_CUSTOMER_CONTACT_EMAIL, &v );
        //     // if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        //     // {
        //     //     m_sTrackingNumber = v.bstrVal;
        //     //     m_sTrackingNumber.TrimLeft();
        //     //     m_sTrackingNumber.TrimRight();
        //     //     VariantClear( &v );
        //     // }
        // 
        //     // NEXT THE MD5 CHECKSUM
        //     CString sMD5;
        //     hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
        //                     GetValueByAttributeName( AVIS_X_SAMPLE_CHECKSUM, &v );
        //     if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        //     {
        //         sMD5 = v.bstrVal;
        //         m_sTrackingNumber += sMD5.Left(5);
        //         VariantClear( &v );
        //     }
        // }


        // // AVIS_SAMPLE_ACTIONS_GATEWAY_COOKIE    
        // hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
        //                 GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_GATEWAY_COOKIE, &v );
        // if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        // {
        //     m_szGatewayCookie = v.bstrVal;
        //     LeftTrimHexString( m_szGatewayCookie );
        //     VariantClear( &v );
        // }


        // AVIS_SAMPLE_ACTIONS_ANALYSIS_ISSUE    
        //m_szIssueNumber = m_sTrackingNumber;    // 2/12/00
        // hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
        //                 GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_ANALYSIS_ISSUE, &v );
        // if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        // {
        //     m_szIssueNumber = v.bstrVal;
        //     LeftTrimHexString( m_szIssueNumber );
        //     VariantClear( &v );
        // }

        
        // AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY 
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY, &v );
        if( IsSuccessfullResult( hr ) )
        {
            if( ConvertVariantToDword( &m_dwSubmissionPriority, &v) )
            {
                SampleActionsConfig.dwSubmissionPriority = m_dwSubmissionPriority; 
            }
            VariantClear( &v );
        }
    
        // AVIS_SAMPLE_ACTIONS_AUTO_DELIVERY_DEFS 
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY, &v );   // AVIS_SAMPLE_ACTIONS_AUTO_DELIVERY_DEFS
        if( IsSuccessfullResult( hr ))          // VT_UI4     // && v.vt == VT_BSTR
        {
            if( ConvertVariantToDword( &m_dwSignaturesPriority, &v) )
            {
                if( m_dwSignaturesPriority )
                    m_bAutoDelivery = TRUE;
                else
                    m_bAutoDelivery = FALSE;
                SampleActionsConfig.m_bAutoDelivery = m_bAutoDelivery;       
            }
            VariantClear( &v );
        }
    
        // AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED    
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_DEF_SEQ_NEEDED, &v );
        if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        {
            m_szDefSequenceNeeded = v.bstrVal;
			if (!m_szDefSequenceNeeded.IsEmpty())
			{
				if (atol(T2A(m_szDefSequenceNeeded.GetBuffer(0))) == 0)
					m_szDefSequenceNeeded.LoadString(IDS_BLANK);
				else
		            LeftTrimHexString( m_szDefSequenceNeeded );
			}
            VariantClear( &v );
        }
    
        // AVIS_SAMPLE_ACTIONS_VIRUS_NAME        
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_VIRUS_NAME, &v );
        if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        {
            m_VirusName= v.bstrVal;
            VariantClear( &v );
        }
        else
        {
            //  IF IT'S MISSING, USE DAVE'S FOR NOW
            // QSERVER_ITEM_INFO_VIRUSNAME
            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            GetValueByAttributeName( AVIS_X_SCAN_VIRUS_NAME, &v );  //AVIS_QSERVER_ITEM_INFO_VIRUSNAME
            if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
            {
                m_VirusName= v.bstrVal;
                VariantClear( &v );
            }
        }

        // AVIS_SAMPLE_ACTIONS_ANALYSIS_STATE    
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_SAMPLE_ACTIONS_ANALYSIS_STATE, &v );
        if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        {
            m_szAnalysisState = v.bstrVal;
            VariantClear( &v );

            // CONVERT THE TOKEN TO THE STRING
            ParseTokensToStrings();
        }

        // GET THE ICON AND DISPLAYNAME
        HICON hIcon;
        TCHAR szFilename[MAX_PATH];
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            GetFilenameAndIcon( szFilename, &hIcon );
        if( IsSuccessfullResult( hr ) )
        {
            m_IconButton.SetIcon( hIcon );
            m_sFilename = szFilename;
            //fWidePrintString( "Filename: %s  %s", (LPCTSTR) m_sFilename, szFilename );

            // 6/15/99 CHECK THAT WE GOT THE VIRUS NAME FROM AVIS_SAMPLE_ACTIONS_VIRUS_NAME
            // if( m_VirusName.IsEmpty() )
            //    m_VirusName = szFilename;

        }

        // AVIS_X_SUBMISSION_COUNT
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_X_SUBMISSION_COUNT, &v );   // 
        if( IsSuccessfullResult( hr ))          // VT_UI4     // && v.vt == VT_BSTR
        {
            ConvertVariantToDword( &m_dwSubmissionCount, &v); 
            VariantClear( &v );
        }

    }
    catch(...)
    {
        return(FALSE);
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
   CSampleActionsPage::ParseTokensToStrings

   m_szAnalysisState is the value of the X-Analysis-State attribute

   LookUpIcePackTokenString is in UtilPlus.cpp. Using the token, it looks 
   up the string in IcePackTokens.dll.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::ParseTokensToStrings()
{
    CString  s;
    WCHAR *  lpszDisplayString = NULL;
    WCHAR *  lpszToken         = NULL;
	USES_CONVERSION;
    // LookUpIcePackTokenString
    s                 = m_szAnalysisState;
    lpszToken         = T2W(s.GetBuffer(0));
    lpszDisplayString = LookUpIcePackTokenString( lpszToken, ICEPACK_TOKEN_STATE_TABLE );

    if( lpszDisplayString != NULL )
    {
        m_szAnalysisState =  lpszDisplayString;
    }
    

    s.ReleaseBuffer(-1);

    return TRUE;
}



/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    ++m_dwRef;	

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );


    // INITIALIZE. FIRST PAGE TO CALL FROM OnInitDialog CAUSES THE READ
    if( m_pAttributeAccess )
    {
        m_pQServerGetAllAttributes = m_pAttributeAccess->Initialize();
        ++m_dwRegistered;
    }

    // INSERT DCOM CODE TO GET LIVE SETTINGS FROM THE SERVER
    FillAttributeData();

    // IF IT'S RELEASED OR HELD, THEN IT HASN'T YET BEEN SENT jhill 4/29/00
    if( m_dwStatus == AVIS_SAMPLE_STATUS_RELEASED || m_dwStatus == AVIS_SAMPLE_STATUS_HELD )
    {
        m_bIsXdateSubmitted = FALSE;
        m_sTrackingNumber.LoadString( IDS_SAMPLE_NOT_SUBMITTED );    // DON'T DISPLAY UNLESS REALLY SUBMITTED
    }

    // IF THIS SAMPLE HAS ALREADY BEEN SUBMITTED, THEN GRAY OUT THE SUBMISSION PRIORITY FIELD 3/28/00 
    if( m_bIsXdateSubmitted )    
    {
        GetDlgItem( IDC_SAMPLE_SUBMISSION_PRIORITY )->EnableWindow( FALSE );
        GetDlgItem( IDC_SPIN1 )->ModifyStyle( WS_VISIBLE, 0, 0 );     // SWP_HIDEWINDOW
          
    }
    else
    {
        // Set up spin control for Submission priority.
        ((CSpinButtonCtrl*) GetDlgItem( IDC_SPIN1 ) )->SetRange32(ACTIONS_MIN_SPIN_SUBMISSION_PRIORITY, ACTIONS_MAX_SPIN_SUBMISSION_PRIORITY);
        // Set the buddy
        ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN1))->SetBuddy( GetDlgItem(IDC_SAMPLE_SUBMISSION_PRIORITY));    // IDC_SUBMISSION_PRIORITY
        // Set initial pos
        ((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN1))->SetPos(m_dwSubmissionPriority);
    }

   
    // CAN WE RE-SUBMIT?
    m_bOkToReSubmit = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->RestoreIsEligibleForReSubmission();
#ifdef MY_SYSTEM    
    //m_bOkToReSubmit = TRUE;    // RESUBMIT ANY
#endif
    if( m_bOkToReSubmit )                   
    {
        CString sNewText;
        sNewText.LoadString( IDS_BUTTON_RESUBMIT );
        GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->SetWindowText( sNewText );
        GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->EnableWindow( TRUE );
    }
    else
    {
        // ONLY MAKE SUBMIT BUTTON AVAILABLE IF SUBMIT PRIORITY IS 0.  AND THERE IS NO X-Date-Submitted    
        BOOL  bEnableSubmitButton = ( (m_dwSubmissionPriority == 0) && !m_bIsXdateSubmitted );

        GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->EnableWindow( bEnableSubmitButton );
    }

                                                                         
    // IF SAMPLE IS IN RELEASE or HELD STATE, THEN CHANGE BUTTON TO OFFER TO REMOVE IT FROM SUBMISSION QUEUE  3/28/00 
    // if( m_dwStatus == AVIS_SAMPLE_STATUS_RELEASED && m_dwSubmissionPriority != 0 )
    if( ( m_dwStatus == AVIS_SAMPLE_STATUS_RELEASED || m_dwStatus == AVIS_SAMPLE_STATUS_HELD ) && m_dwSubmissionPriority != 0 )
    {
        CString sNewText;
        sNewText.LoadString( IDS_BUTTON_REMOVE_SUBMISSION );   
        GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->SetWindowText( sNewText );
        GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->EnableWindow( TRUE );
        m_dwActionToTake = ACTION_REMOVE_FROM_SUBMISSION_QUEUE;
    }

    // ONLY MAKE DELIVERY BUTTON AVAILABLE IF STATUS IS "AVAILABLE"     jhill 3/28/00 
    // AND SIGNATURE PRIORITY IS 0   jhill 7/15/00
    if( m_dwStatus != AVIS_SAMPLE_STATUS_AVAILABLE || m_dwSignaturesPriority!= 0 )
    {
        GetDlgItem( IDC_ACTIONS_DELIVER_BUTTON )->EnableWindow( FALSE );
    }

    // SETUP THE EXTENDED STATUS FIELD       
    SetExtendedStatusText();

    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA

    // TEST  4/5/00
#ifdef MY_SYSTEM
    BOOL TestSampleErrorDection( void * pQServerGetAllAttributes );
    TestSampleErrorDection( m_pQServerGetAllAttributes );
#endif


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::SetExtendedStatusText() 
{

    // SETUP THE EXTENDED STATUS FIELD       
    if( m_dwSubmissionCount > 1 )
        m_sStatusEx.LoadString( IDS_SAMPLE_WAS_RESUBMITTED );    // X-Submission-Count
    else if( m_bIsXdateSubmitted )
        m_sStatusEx.LoadString( IDS_SAMPLE_WAS_SUBMITTED );
    else if( m_dwStatus != AVIS_SAMPLE_STATUS_HELD )
        m_sStatusEx.LoadString( IDS_SAMPLE_WAS_NOT_SUBMITTED );  

    if( m_dwStatus == AVIS_SAMPLE_STATUS_RELEASED && m_dwSubmissionPriority != 0 )
        m_sStatusEx.LoadString( IDS_SAMPLE_WAITING_FOR_SUBMISSION  );  
    else if( m_dwStatus == AVIS_SAMPLE_STATUS_HELD && m_dwSubmissionPriority == 0 )
        m_sStatusEx.LoadString( IDS_SAMPLE_HELD_FOR_MANUAL_SUBMISSION  );  
    

    // CHECK FOR NEW DEFS X-Signatures-Sequence
    if( !m_szDefSequenceNeeded.IsEmpty() )
    {
        //CString sActiveDefsSeqNum;    // REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM
       	CRegKey reg;
    	LONG    lResult;
        TCHAR   szActiveDefsSeqNum[100] = {0};
        DWORD   dwCount = sizeof(szActiveDefsSeqNum);
        //  GO RETRIEVE IT FROM LOCAL REGISTRY ENTRY
		lResult = reg.Open( HKEY_LOCAL_MACHINE, REGKEY_QSCONSOLE);
        if( lResult == ERROR_SUCCESS )
		{
			lResult = reg.QueryStringValue(REGVALUE_DEFS_ACTIVE_SEQUENCE_NUM, szActiveDefsSeqNum, &dwCount );  
			if( lResult == ERROR_SUCCESS )
			{
                TCHAR* endptr = NULL;
                DWORD  dwActiveDefsSeqNum  = STRTOUL( (LPCTSTR)szActiveDefsSeqNum, &endptr, 10 );
                DWORD  dwDefSequenceNeeded = STRTOUL( (LPCTSTR)m_szDefSequenceNeeded, &endptr, 10 );
                if( dwDefSequenceNeeded != 0 && dwActiveDefsSeqNum >= dwDefSequenceNeeded )
                    m_sStatusEx.LoadString( IDS_DEFINITIONS_HAVE_BEEN_RECIEVED );  
			}
		    reg.Close();
		}
    }

    return TRUE;
}



/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::OnOK() 
{
	CPropertyPage::OnOK();
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::OnApply() 
{
    // Make sure data is good.
    UpdateData(TRUE);

    SaveChangedData();

    // DISABLE APPLY BUTTON
	SetModified(FALSE);


    // ONLY MAKE SUBMIT BUTTON AVAILABLE IF SUBMIT PRIORITY IS 0    jhill 3/28/00  
    GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->EnableWindow( (m_dwSubmissionPriority == 0) );

    // ONLY MAKE DELIVERY BUTTON AVAILABLE IF STATUS IS "AVAILABLE" AND SIGNATURE PRIORITY IS 0   jhill 7/15/00
    if( m_dwStatus == AVIS_SAMPLE_STATUS_AVAILABLE && m_dwSignaturesPriority== 0 )
        GetDlgItem( IDC_ACTIONS_DELIVER_BUTTON )->EnableWindow( TRUE );
    else
        GetDlgItem( IDC_ACTIONS_DELIVER_BUTTON )->EnableWindow( FALSE );

    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA


	return CPropertyPage::OnApply();
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::SaveChangedData() 
{
	DWORD   dwDataChanged = 0;
    HRESULT hr = 0;
    VARIANT v;
    VariantInit( &v );

    // Save the persistant data that has been changed
    try
    {
        // GET THE INTERFACE
        if( m_pQServerGetAllAttributes == NULL )
            return;
        if( !((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->IsDataAvailable() )
            return;

        // AUTO SUBMIT SAMPLE
        if(SampleActionsConfig.dwSubmissionPriority != m_dwSubmissionPriority) 
        {
            fWidePrintString("DATA SAVED CSampleActionsPage::m_dwSubmissionPriority  Old=%d  New=%d",
                          SampleActionsConfig.dwSubmissionPriority,m_dwSubmissionPriority);
            SampleActionsConfig.dwSubmissionPriority = m_dwSubmissionPriority;

            // AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY == AVIS_X_SAMPLE_PRIORITY
            VariantClear( &v );
            v.vt    = VT_UI4;
            v.ulVal = m_dwSubmissionPriority;
            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            SetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY, &v );
            //if( SUCCEEDED( hr ) )


            dwDataChanged = TRUE;
        }
        
#if 0
        // AUTO DELIVERY OF DEFINITIONS
        if(SampleActionsConfig.m_bAutoDelivery != m_bAutoDelivery) 
        {
            fWidePrintString("DATA SAVED CSampleActionsPage::m_m_bAutoDelivery  Old=%d  New=%d",
                          SampleActionsConfig.m_bAutoDelivery,m_bAutoDelivery);
            SampleActionsConfig.m_bAutoDelivery = m_bAutoDelivery;
        
            // AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY == AVIS_X_SIGNATURES_PRIORITY
            VariantClear( &v );
            v.vt    = VT_UI4;

            if( m_bAutoDelivery )
            {
                if( m_dwSignaturesPriority )           // VALUE SAVED FROM  READ
                    v.ulVal = m_dwSignaturesPriority;
                else
                    v.ulVal = ENABLE_AUTO_DELIVERY_VALUE;
            }
            else
                v.ulVal = 0;

            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            SetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SIGNATURES_PRIORITY, &v );
            //if( SUCCEEDED( hr ) )
        
        
            dwDataChanged = TRUE;
        }
#endif

    }
    catch(...)
    {
        return;
    }
    
    // DID WE CHANGE ANYTHING?
    if( dwDataChanged )  
    {
        // FORCE IT TO REDISPLAY THE DATA
        UpdateData( FALSE );         
    }

    return;
}

    
/*----------------------------------------------------------------------------
    CSampleActionsPage::OnKillActive
    Validate entered data
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::OnKillActive() 
{
    DWORD  dwSubmissionPriority = 0;
    BOOL   bTrans=0;
    CString sText;   

    dwSubmissionPriority = GetDlgItemInt( IDC_SAMPLE_SUBMISSION_PRIORITY, &bTrans, FALSE ); 
    if( !bTrans || ( dwSubmissionPriority < ACTIONS_MIN_SPIN_SUBMISSION_PRIORITY || 
                     dwSubmissionPriority > ACTIONS_MAX_SPIN_SUBMISSION_PRIORITY ) )
    {
            sText.Format( IDS_BADFMT_SUBMISSION_PRIORITY, ACTIONS_MIN_SPIN_SUBMISSION_PRIORITY, ACTIONS_MAX_SPIN_SUBMISSION_PRIORITY ); 
            ShowErrorMessage( (LPCTSTR) sText, IDS_ERROR_CONFIG_SAMPLE_ACTIONS, MB_ICONERROR | MB_OK);


            GetDlgItem( IDC_SAMPLE_SUBMISSION_PRIORITY )->SetFocus();
            ((CEdit*) GetDlgItem( IDC_SAMPLE_SUBMISSION_PRIORITY) )->SetSel( 0, -1 );
            return(FALSE);
    }
 
 	// TODO: Add your specialized code here and/or call the base class
 	return CPropertyPage::OnKillActive();
}



/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::OnCheckSampleAutoDelivery() 
{
	SetModified(TRUE);
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::OnChangeSampleSubmissionPriority() 
{
	SetModified(TRUE);
}



/*----------------------------------------------------------------------------
    CSampleActionsPage::OnActionsSubmitButton()
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::OnActionsSubmitButton() 
{
    HRESULT  hr   = S_OK;
    BOOL     bRestored = FALSE;
    int      iRet = 0;
    CString  sText    = _T("");
    CString  sCaption = _T("");
    CString  sError   = _T("");
    CString  s;

    if( m_pQServerGetAllAttributes == NULL )
    	return;

    // RESUBMIT? 4/5/00
    if( m_bOkToReSubmit )
    {
        bRestored = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->RestoreAllSampleAttributes();
        if( bRestored )
        {
            sText.Format( IDS_ITEM_RESUBMITTED_FMT, 1 );    
            sCaption.LoadString( IDS_ITEM_RESUBMITTED_TITLE );             
            m_bOkToReSubmit = FALSE;
            GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->EnableWindow( FALSE );
            m_sStatusEx.LoadString( IDS_SAMPLE_WAS_RESUBMITTED );
            UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA
        }
        else
        {
            sText.LoadString( IDS_ITEM_RESUBMIT_FAILED );    
            sCaption.LoadString( IDS_ITEM_RESUBMITTED_TITLE );             
        }
        MessageBox( sText, sCaption, MB_OK | MB_ICONASTERISK);

   	   	return;
    }

    // REMOVE IT FROM THE SUBMISION QUEUE
    if( m_dwActionToTake == ACTION_REMOVE_FROM_SUBMISSION_QUEUE )    //jhill 4/29
    {
        VARIANT v;
        VariantInit( &v );

        if( !((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->IsDataAvailable() )
            return;
        if( m_dwSubmissionPriority == 0 )
            return;

        SampleActionsConfig.dwSubmissionPriority = 0;
        m_dwSubmissionPriority                   = 0;

        // AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY == AVIS_X_SAMPLE_PRIORITY
        VariantClear( &v );
        v.vt    = VT_UI4;
        v.ulVal = 0;
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        SetValueByAttributeName( AVIS_SAMPLE_ACTIONS_SUBMISSION_PRIORITY, &v );

        // 4/29/00 NOTE:  DOES IT ALSO NEED TO CHANGE THE STATUS OR DELETE THE SUBMITTED DATE?  jhill


        // RESET THE EXTENDED STATUS FIELD       
        // SetExtendedStatusText();
        m_sStatusEx.Empty();

        // DISABLE THE SUBMIT BUTTON 
        GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->EnableWindow( FALSE );

        // FORCE IT TO REDISPLAY THE DATA
        UpdateData( FALSE );         


        sCaption.LoadString( IDS_TEXT_REMOVE_SAMPLE_FROM_QUEUE );           
        sText.LoadString( IDS_TEXT_REMOVE_SAMPLE_FROM_QUEUE2 );    
        MessageBox( sText, sCaption, MB_OK | MB_ICONASTERISK);

   	   	return;
    }

    // AUTO SUBMIT SAMPLE MODIFIED?
    if(SampleActionsConfig.dwSubmissionPriority != m_dwSubmissionPriority) 
    {
        //if( g_hMainWnd != NULL )
        {
            sText.LoadString( IDS_SAVE_CHANGES_MSG );
            sCaption.LoadString( IDS_SAVE_CHANGES_CAPTION );
            iRet = MessageBox( sText, sCaption, MB_YESNO | MB_ICONQUESTION );  // g_hMainWnd, 
            if( iRet == IDYES )
                OnApply();
        }
    }


    // 6/15/00 CALC THE SUBMISSION PRIORITY. IF THE USER HAS MODIFIED THE SETTING, USE THE NEW SETTING. 
    DWORD  dwSubmissionPriority = 0;   
    BOOL   bTrans=0;
    if( (dwSubmissionPriority = m_dwSubmissionPriority) == 0 )
    {
        dwSubmissionPriority = GetDlgItemInt( IDC_SAMPLE_SUBMISSION_PRIORITY, &bTrans, FALSE ); 
        if( dwSubmissionPriority == 0 )
            dwSubmissionPriority = ENABLE_AUTO_SUBMISSION_VALUE;
    }

    hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->MarkSampleForSubmission( sError, sCaption, dwSubmissionPriority );
    if( IsSuccessfullResult( hr ) )
    {   
        m_dwSubmissionPriority = dwSubmissionPriority;             // ENABLE_AUTO_SUBMISSION_VALUE
        SampleActionsConfig.dwSubmissionPriority = m_dwSubmissionPriority;

        // ONLY MAKE SUBMIT BUTTON AVAILABLE IF SUBMIT PRIORITY IS 0    jhill 3/28/00  
        GetDlgItem( IDC_ACTIONS_SUBMIT_BUTTON )->EnableWindow( FALSE );

        sText.Format( IDS_ITEM_SUBMITTED_FMT2, 1 );    
        sCaption.LoadString( IDS_ITEMS_SUBMITTED_TITLE );             // _T("Selected Samples Released");  
        fWidePrintString("CSampleActionsPage::OnActionsSubmitButton  Item marked for auto submission. ObjectID= %d", m_iExtDataObjectID );

        // RESET THE EXTENDED STATUS FIELD       
        m_sStatusEx.Empty();

        // 7/7/00 DISABLE APPLY BUTTON
    	SetModified(FALSE);

        // THIS SAMPLE SUBMITTED, GRAY OUT THE SUBMISSION PRIORITY FIELD 7/7/00 
        GetDlgItem( IDC_SAMPLE_SUBMISSION_PRIORITY )->EnableWindow( FALSE );
        GetDlgItem( IDC_SPIN1 )->ModifyStyle( WS_VISIBLE, 0, 0 );     // SWP_HIDEWINDOW

        // FORCE IT TO REDISPLAY THE DATA
        UpdateData( FALSE );         
    }
    else
    {
        s.LoadString( IDS_MANUAL_SUBMIT_CRITERIA );
        sText = sError + s;
        //sCaption.LoadString( IDS_MANUAL_SUBMIT_ERROR );      9/2/99
        fWidePrintString("FAILED: CSampleActionsPage::OnActionsSubmitButton Failed to mark item for submission.  ObjectID= %d", m_iExtDataObjectID );
    }

    if( !sCaption.IsEmpty() )
        MessageBox( sText, sCaption, MB_OK | MB_ICONASTERISK);

	return;
}

/*----------------------------------------------------------------------------
    CSampleActionsPage::OnActionsDeliverButton()
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::OnActionsDeliverButton() 
{
    HRESULT  hr   = S_OK;
    int      iRet = 0;
    CString sText    = _T("");
    CString sCaption = _T("");
    CString sError   = _T("");
    CString s;

    if( m_pQServerGetAllAttributes == NULL )
    	return;

    // AUTO DELIVERY OF DEFINITIONS MODIFIED?
    if(SampleActionsConfig.m_bAutoDelivery != m_bAutoDelivery)  
    {
        //if( g_hMainWnd != NULL )
        {
            sText.LoadString( IDS_SAVE_CHANGES_MSG );
            sCaption.LoadString( IDS_SAVE_CHANGES_CAPTION );
            iRet = MessageBox( sText, sCaption, MB_YESNO | MB_ICONQUESTION );  // g_hMainWnd, 
            if( iRet == IDYES )
                OnApply();
        }
    }

    hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->MarkSampleForAutoDefDelivery( sError, sCaption );
    if( IsSuccessfullResult( hr ) )
    {
        m_dwSignaturesPriority = ENABLE_AUTO_DELIVERY_VALUE;
        SampleActionsConfig.m_bAutoDelivery = m_bAutoDelivery;
        m_bAutoDelivery        = TRUE;

        // ONLY MAKE DELIVERY BUTTON AVAILABLE IF STATUS IS "AVAILABLE" AND SIGNATURE PRIORITY IS 0   jhill 7/15/00
        GetDlgItem( IDC_ACTIONS_DELIVER_BUTTON )->EnableWindow( FALSE );

        sText.Format( IDS_ITEM_DELIVERED_FMT2, 1 );                   // %d item was set for distribution to the user
        sCaption.LoadString( IDS_ITEMS_DELIVERED_TITLE );             // _T("Selected items set for distribution");
        fWidePrintString("CSampleActionsPage::OnActionsDeliverButton  Item marked for auto delivery. ObjectID= %d", m_iExtDataObjectID );

        // FORCE IT TO REDISPLAY THE DATA
        UpdateData( FALSE );         
    }
    else
    {
        s.LoadString( IDS_MANUAL_DELIVERY_CRITERIA );
        sText = sError + s;
        //sCaption.LoadString( IDS_MANUAL_DELIVERY_ERROR );   9/2/99
        fWidePrintString("FAILED: CSampleActionsPage::OnActionsDeliverButton Failed to mark item for delivery.  ObjectID= %d", m_iExtDataObjectID );
    }

    // if( g_hMainWnd != NULL  && !sCaption.IsEmpty() )
    //     ::MessageBox( g_hMainWnd, sText, sCaption, MB_OK );
    if( !sCaption.IsEmpty() )
        MessageBox( sText, sCaption, MB_OK );

    return;
}

 
/*----------------------------------------------------------------------------
    F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD_SAMPLE_ACTIONS, m_lpHelpIdArray, pHelpInfo );  
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}


/*----------------------------------------------------------------------------
    PROP SHEET HELP BUTTON

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleActionsPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	NMHDR* pNMHDR = (NMHDR*)lParam;
    DoHelpPropSheetHelpButton( IDD_SAMPLE_ACTIONS, m_lpHelpIdArray, pNMHDR );  
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;

/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD_SAMPLE_ACTIONS, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd ); 
    return;
}

#if 0
/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleActionsPage::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR
    //if( nResourceID == 0)
    //    nResourceID = IDD_SAMPLE_ACTIONS;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );  
    pContextWnd = NULL;
    return;
}

#endif





