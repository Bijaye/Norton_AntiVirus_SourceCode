/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*--------------------------------------------------------------------
   SampleErrorPage.cpp : implementation file

   Property Page Tab Title: "Errors"

   Written by: Jim Hill                 
--------------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "AvisConsole.h"
#include "SampleErrorPage.h"
#include "mmc.h"
#include "GetAllAttributes.h"
#include "ParseEventData.h"
//#include "AvisEventDetection.h"   // in QuarantineServer\Include 
//#include "IcePackTokens.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif






/////////////////////////////////////////////////////////////////////////////
// CSampleErrorPage property page

IMPLEMENT_DYNCREATE(CSampleErrorPage, CPropertyPage)

CSampleErrorPage::CSampleErrorPage() : CPropertyPage(CSampleErrorPage::IDD)
{
	//{{AFX_DATA_INIT(CSampleErrorPage)
	m_sSampleErrorString = _T("");
	m_sFilename = _T("");
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_dwRef                    = 1;
    m_pDataObject              = NULL;  
    m_pQServerGetAllAttributes = NULL;
    m_pAttributeAccess         = NULL;
    m_sSampleErrorTokens       = _T("");

    m_lpHelpIdArray = g_SampleErrorPageHelpIdArray;  // LOAD HELP MAP

    //fWidePrintString("CSampleErrorPage Constructor called  ObjectID= %d", m_iExtDataObjectID);

}

CSampleErrorPage::~CSampleErrorPage()
{
    //fWidePrintString("CSampleErrorPage Destructor called  ObjectID= %d", m_iExtDataObjectID);
}


// DEFINES FOR VARIABLE LENGTHS IN INPUTRANGE.H
void CSampleErrorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleErrorPage)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_IconButton);
	DDX_Text(pDX, IDC_SAMPLE_ERROR_EDIT, m_sSampleErrorString);
	DDX_Text(pDX, IDC_NAME_EDIT, m_sFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSampleErrorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSampleErrorPage)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleErrorPage message handlers



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CSampleErrorPage::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::PostNcDestroy();
    Release();     
    //fWidePrintString("CSampleErrorPage::PostNcDestroy()  ObjectID= %d", m_iExtDataObjectID);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CSampleErrorPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    m_dwRef++; 

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // INITIALIZE. FIRST PAGE TO CALL FROM OnInitDialog CAUSES THE READ
    if( m_pAttributeAccess )
    {
        m_pQServerGetAllAttributes = m_pAttributeAccess->Initialize();
        ++m_dwRegistered;
    }

    

    FillAttributeData();

    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleErrorPage::FillAttributeData() 
{
    HRESULT hr     = S_OK;
    //TCHAR *endptr = NULL;
    CString sAttention          = _T("");
    CString sSampleErrorString  = _T("");
    CString sLastAlertString    = _T("");
    CString sLastAlertDate      = _T("");
    CString sEventDefaultMsg    = _T("");
    CString sEventName          = _T("");
    CString sSampleStatus       = _T("");
    CString sAlertStatus        = _T("");
    CString sResultToken        = _T("");
    CString s                   = _T("");
    CString sToken              = _T("");
    CString sHeading            = _T("");
    CString sTemp               = _T("");
    CString sTimeInStateErrorMsg= _T("");
    CString sNote               = _T("");
    CString sAnalysisState      = _T("");
    VARIANT v;
    int     iStatusID          = 0;


    try
    {
        if( m_pQServerGetAllAttributes == NULL )
            return(FALSE);
        if( !((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->IsDataAvailable() )
            return(FALSE);
    
        VariantInit( &v );
        VariantClear( &v );


        // GET SAMPLE STATUS?
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( QSITEMINFO_X_SAMPLE_STATUS, &v );  
        if( IsSuccessfullResult( hr ) && v.vt == VT_UI4)
        {
            iStatusID = (int) v.ulVal;
            VariantClear( &v );

            CString sStatusID;
            sStatusID.Format( _T("%d"), iStatusID );
            if( GetStatusToken( s, sStatusID ) )
                sSampleStatus = s;

            // GET X-Scan-Result
            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            GetValueByAttributeName( QSITEMINFO_X_SCAN_RESULT, &v );   
            if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR )
            {
                sResultToken = v.bstrVal;
                VariantClear( &v );
            }
            
			// GET X-Analysis-State
            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            GetValueByAttributeName( QSITEMINFO_X_ANALYSIS_STATE, &v );   
            if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR )
            {
                sAnalysisState = v.bstrVal;
                VariantClear( &v );
            }
        }
        else
            iStatusID = -1;



        // GET THE ATTENTION STRING FIRST              
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_X_SAMPLE_ERROR_ATTENTION, &v );  // AVIS_X_SAMPLE_ERROR_ATTENTION
        if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        {
            s = v.bstrVal;
            VariantClear( &v );
            if( !s.IsEmpty() )
            {
                sHeading.LoadString( IDS_HEADING_ATTENTION );
                sAttention = sHeading + _T("\r\n") + s + _T("\r\n\r\n");
            }
        }

        // GET THE LAST ALERT ERROR TOKEN    
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( QSITEMINFO_X_ALERT_STATUS, &v );  // 
        if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR)
        {
            sToken = v.bstrVal;
            VariantClear( &v );                                    
            if( !StrLookUpIcePackTokenString( sToken, sLastAlertString, ICEPACK_TOKEN_ERROR_TABLE ) )   
                StrLookUpIcePackTokenString( sToken, sLastAlertString, ICEPACK_TOKEN_STATUS_TABLE );   

            // IS THIS A TIME IN STATE ALERT?
            if( IsSampleTimeInStateAlert( sSampleStatus, sToken ) )
            {
                sAlertStatus = sToken;
                StrTableLoadString( IDSTABLE_SAMPLE_INSTATE_TOO_LONG, s );     
                sTimeInStateErrorMsg.Format( s, sToken );
                // MessageBox( sTimeInStateErrorMsg, _T("Time In State Alert"), MB_OK ); 
            }

            // IS THIS A LOCAL QUARANTINE ERROR CONDITION?
            if( iStatusID >= 0 && IsLocalQuarantineSampleAlert( sSampleStatus, &iStatusID, sResultToken,sAnalysisState ))
            {
                // IF SO, SUBSTITUTE TOKEN
                sToken = TOKEN_NOTREPAIRED;
                StrLookUpIcePackTokenString( sToken, sLastAlertString, ICEPACK_TOKEN_RESULT_TABLE );
            }
        
            // GET THE LAST ALERT DATE    
            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            GetValueByAttributeName( QSITEMINFO_X_DATE_ALERT, &v );  // 
            if( IsSuccessfullResult( hr ) && v.vt != VT_EMPTY )
                ConvertVariantToString( sLastAlertDate, &v, AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT );  

            // GET THE DEFAULT MESSAGE
            GetEventNameAndDefaultMsg( sToken, sEventDefaultMsg, sEventName, sNote, EVENT_TOKEN_SAMPLE_INTERVENTION_TABLE );

            // IF THIS IS A "NOT INSTALLED" EVENT, FIXUP THE DEFAULT MSG
            if( sSampleStatus == TOKEN_NOTINSTALLED )           // iStatusID == STATUS_NOTINSTALLED
            {
                CString  sDefSequenceNeeded;
                VariantClear( &v );                                    
                hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                                GetValueByAttributeName( QSITEMINFO_X_SIGNATURES_SEQUENCE, &v );  //  AVIS_X_SIGNATURES_SEQUENCE
                if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR )
                {
                    sDefSequenceNeeded = v.bstrVal;
                    sDefSequenceNeeded.TrimLeft( _T(" 0") );  // TRIM LEADING ZEROS AND SPACES
                    VariantClear( &v );
                    if( !sDefSequenceNeeded.IsEmpty() && !sEventDefaultMsg.IsEmpty() )
                    {
                        // ADD EVENT NAME HEADING
                        sHeading.LoadString( IDS_HEADING_EVENT_NAME );

                        // ADD EVENT NAME
                        if( !sEventName.IsEmpty() )
                            sEventDefaultMsg = sHeading + sEventName + _T("\r\n") + sEventDefaultMsg;

                        sEventDefaultMsg += sDefSequenceNeeded;
                    }
                }
            }

            // IS THERE ALSO A NOTE FOR THIS EVENT
            if( !sNote.IsEmpty() )  
            {
                sEventDefaultMsg += _T("\r\n") + sNote;
            }

            // IS THIS A TIME IN STATE ALERT?
            if( !sTimeInStateErrorMsg.IsEmpty() )
            {
                // ONLY ADD IT ON IF WE'RE MISSING THE DEFAULT MSG
                if( sEventDefaultMsg.IsEmpty() )
                    sEventDefaultMsg  = sTimeInStateErrorMsg;
                //else
                //    sEventDefaultMsg += _T("\r\n") + sTimeInStateErrorMsg;
            }


            // ADD EVENT NAME HEADING
            sHeading.LoadString( IDS_HEADING_EVENT_NAME );

            // ADD EVENT NAME
            if( sEventName.IsEmpty() )
                s = sLastAlertString;
            else
                s = sHeading + sEventName + _T("\r\n") + sLastAlertString;

            // ADD HEADING
            sHeading.LoadString( IDS_HEADING_LAST_ALERT );
            sHeading += sLastAlertDate;
            sLastAlertString = sHeading + _T("\r\n") + s + _T("\r\n") + sEventDefaultMsg;
        }

        // GET THE ERROR TOKEN    
        hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                        GetValueByAttributeName( AVIS_X_SAMPLE_ERROR, &v );  // 
        if( IsSuccessfullResult( hr ) && v.vt == VT_BSTR )
        {
            sToken = v.bstrVal;
            VariantClear( &v );
            StrLookUpIcePackTokenString( sToken, s, ICEPACK_TOKEN_ERROR_TABLE );

            // ADD HEADING
            sHeading.LoadString( IDS_HEADING_ERROR );
            sSampleErrorString = sHeading + _T("\r\n") + s + _T("\r\n\r\n");
        }

        // IS THIS A TIME-IN-STATE CONDITION?      2/27/00
        if( !sTimeInStateErrorMsg.IsEmpty() && sSampleStatus == sAlertStatus )
        {
            // ADD HEADING
            sHeading.LoadString( IDS_HEADING_ERROR );
            sSampleErrorString += sHeading + _T("\r\n") + sTimeInStateErrorMsg + _T("\r\n\r\n");
        }

        // IF THIS IS A "NOT INSTALLED" EVENT, DISPLAY AS AN ERROR
        if( sSampleStatus == TOKEN_NOTINSTALLED )           // iStatusID == STATUS_NOTINSTALLED
        {
            // ADD HEADING
            sHeading.LoadString( IDS_HEADING_ERROR );
            sSampleErrorString += sHeading + _T("\r\n") + sEventDefaultMsg + _T("\r\n\r\n");
        }


        // IS THIS A LOCAL QUARANTINE ERROR CONDITION?
        if( iStatusID >= 0 )
        {
                
            if( IsLocalQuarantineSampleAlert( sSampleStatus, &iStatusID, sResultToken,sAnalysisState ) )
            {
                sToken = TOKEN_NOTREPAIRED;
                StrLookUpIcePackTokenString( sToken, s, ICEPACK_TOKEN_RESULT_TABLE );
            
                // ADD HEADING
                sHeading.LoadString( IDS_HEADING_ERROR );
                sSampleErrorString += sHeading + _T("\r\n") + s + _T("\r\n\r\n");
            }
        }


        // COMBINE ALL STRINGS
        m_sSampleErrorString = sSampleErrorString; 


        // ADD TEXT FROM X-ATTENTION
        if( !sAttention.IsEmpty() )
            m_sSampleErrorString += sAttention; 

        // IF EMPTY ERRORS STRING, DISPLAY "No Errors"
        if( m_sSampleErrorString.IsEmpty() )
        {
            m_sSampleErrorString.LoadString( IDS_NO_ERROR_STRING );
            m_sSampleErrorString += _T("\r\n\r\n");
        }

        // ADD TEXT FOR LAST ALERT
        if( !sLastAlertString.IsEmpty() )
            m_sSampleErrorString += sLastAlertString; 


#if 0
        // IF EMPTY STRING, DISPLAY "No Errors"
        if( m_sSampleErrorTokens.IsEmpty() )
        {
            if( sAttention.IsEmpty() )
                m_sSampleErrorString.LoadString( IDS_NO_ERROR_STRING );
        }
        else
        {
            // PARSE TOKENS TO STRINGS IN RESOURCE DLL
            m_sSampleErrorTokens = sSampleErrorString;
            ParseTokensToStrings();

            // ADD HEADING
            s = m_sSampleErrorString;
            sHeading.LoadString( IDS_HEADING_ERROR );
            m_sSampleErrorString = sHeading + _T("\r\n") + s;
        }

        // ADD TEXT FROM X-ATTENTION
        if( !sAttention.IsEmpty() )
            m_sSampleErrorString += sAttention + _T("\r\n"); 
#endif


        // GET THE FILENAME AND ICON 
        HICON hIcon;
        TCHAR szFilename[MAX_PATH];
        HRESULT hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                            GetFilenameAndIcon( szFilename, &hIcon );
        if( IsSuccessfullResult( hr ) )
        {
            m_sFilename = szFilename;
            fWidePrintString( "Filename: %s  %s", (LPCTSTR) m_sFilename, szFilename );
            m_IconButton.SetIcon( hIcon );
            UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA
        }


    }
    catch(...)
    {
        return(FALSE);
    }
    return(TRUE);
}


/*----------------------------------------------------------------------------
   CSampleErrorPage::ParseTokensToStrings

???   m_sSampleErrorTokens contains a comma delimted list of numbers.
   Format the resulting strings in m_sSampleErrorString

   m_sSampleErrorTokens is the value of the X-Error attribute

   LookUpIcePackTokenString is in UtilPlus.cpp. Using the token, it looks 
   up the string in IcePackTokens.dll.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CSampleErrorPage::ParseTokensToStrings()
{
    WCHAR *  lpszDisplayString = NULL;
    WCHAR *  lpszToken         = NULL;
	USES_CONVERSION;

    // LookUpIcePackTokenString
    lpszToken = T2W(m_sSampleErrorTokens.GetBuffer(0));
    lpszDisplayString = LookUpIcePackTokenString( lpszToken, ICEPACK_TOKEN_ERROR_TABLE );

    if( lpszDisplayString == NULL )
        m_sSampleErrorString = m_sSampleErrorTokens;
    else
        m_sSampleErrorString =  lpszDisplayString;

    // APPEND CRLF TO THE RETURNED STRING
    m_sSampleErrorString += _T("\r\n\r\n") ;

    m_sSampleErrorTokens.ReleaseBuffer(-1);
    return TRUE;
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
LONG CSampleErrorPage::Release() 
{ 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());   

    try
    {   
        if( --m_dwRef == 0 )
        {
		    fWidePrintString("CSampleErrorPage::Release called. Deleting ObjectID= %d", m_iExtDataObjectID);
        
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









// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
BOOL CSampleErrorPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD, m_lpHelpIdArray, pHelpInfo );
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}



// PROP SHEET HELP BUTTON
BOOL CSampleErrorPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void CSampleErrorPage::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
    return;
}

#if 0
void CSampleErrorPage::OnWhatsThisMenuitem() 
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

