/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/*--------------------------------------------------------------------
   ServerGeneralError.cpp : implementation file

   Property Page Tab Title: "Attention"

   Written by: Jim Hill                 
--------------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "AvisConsole.h"
#include "ServerGeneralError.h"
#include "mmc.h"

#include "ParseEventData.h"
//#include "IcePackTokens.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif






/////////////////////////////////////////////////////////////////////////////
// CServerGeneralError property page

IMPLEMENT_DYNCREATE(CServerGeneralError, CPropertyPage)

CServerGeneralError::CServerGeneralError() : CPropertyPage(CServerGeneralError::IDD)
{
	//{{AFX_DATA_INIT(CServerGeneralError)
	m_sGeneralErrorString = _T("");
	//}}AFX_DATA_INIT

    // INITIALIZE
    m_pSnapin       = NULL;
    m_dwRef         = 1;
    m_pSConfigData  = NULL;
    m_lpHelpIdArray = g_AtttentionPageHelpIdArray;  // LOAD HELP MAP


    fWidePrintString("CServerGeneralError Constructor called  ObjectID= %d", m_iExtDataObjectID);

}

CServerGeneralError::~CServerGeneralError()
{
    fWidePrintString("CServerGeneralError Destructor called  ObjectID= %d", m_iExtDataObjectID);
}


// DEFINES FOR VARIABLE LENGTHS IN INPUTRANGE.H
void CServerGeneralError::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerGeneralError)
	DDX_Text(pDX, IDC_GENERAL_ERROR_EDIT, m_sGeneralErrorString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerGeneralError, CPropertyPage)
	//{{AFX_MSG_MAP(CServerGeneralError)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerGeneralError message handlers



/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
void CServerGeneralError::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::PostNcDestroy();
    Release();     
    //fWidePrintString("CServerGeneralError::PostNcDestroy()  ObjectID= %d", m_iExtDataObjectID);
}


/*----------------------------------------------------------------------------

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CServerGeneralError::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    CString s                   = _T("");
    CString sToken              = _T("");
    CString sHeading            = _T("");
    m_dwRef++; 

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // GET LIVE SETTINGS FROM THE SERVER
    if(m_pSConfigData!=NULL) 
    {
        // DID WE READ VALID DATA FROM THE SERVER?
        if( m_pSConfigData->ContainsValidData() )
        {
            m_sGeneralAttentionTokens =  m_pSConfigData->m_sGeneralAttentionTokens;

            // IF EMPTY STRING, DISPLAY "No Errors"
            if( m_sGeneralAttentionTokens.IsEmpty() )
            {
                m_sGeneralErrorString.LoadString( IDS_BLANK );
                m_sGeneralErrorString += _T("\r\n\r\n"); 
            }
            else
            {
                m_sGeneralErrorString.LoadString( IDS_HEADING_ATTENTION );
                m_sGeneralErrorString += _T("\r\n"); 

                // PARSE MULTIPLE ERROR TOKENS TO STRINGS IN RESOURCE DLL
                ParseTokensToStrings();
            }

            // GET THE LAST ALERT ERROR TOKEN    
            CString  sLastAlertString   = _T("");
            CString  sLastAlertDate     = _T("");
            CString  sEventDefaultMsg   = _T("");
            CString  sEventName         = _T("");
            CString  sKey               = REGKEY_QSERVER;
            TCHAR*   endptr             = NULL;
            time_t   tAlertDateTime     = 0;
            HRESULT  hr                 = 0;
            VARIANT  v;

            VariantInit( &v );

            // GET LIVE DATA
            m_sLastAlertString = m_pSConfigData->m_sLastAlertString;
            m_sLastAlertDate   = m_pSConfigData->m_sLastAlertDate;  
            if( !m_sLastAlertString.IsEmpty() )
            {
                sToken = m_sLastAlertString;
                StrLookUpIcePackTokenString( sToken, sLastAlertString, ICEPACK_TOKEN_ATTENTION_TABLE );   
                if( !sLastAlertString.IsEmpty() )
                {
                    // GET THE LAST ALERT DATE    
                    if( !m_sLastAlertDate.IsEmpty() )
                    {
                        // SET UP TO FORMAT THE TIME STRING AS GMT
                        tAlertDateTime = STRTOUL( (LPCTSTR)m_sLastAlertDate, &endptr, 10 );
                        if( tAlertDateTime != 0 )
                        {
                            SYSTEMTIME timeDest;
                            CTime ctTime( tAlertDateTime );
                            ctTime.GetAsSystemTime( timeDest ); 
                   
                            // CONVERT AND FORMAT TIME STRING
                            VariantClear( &v );
                            v.vt = VT_DATE;
                            SystemTimeToVariantTime( &timeDest, &v.date );
                            ConvertVariantToString( sLastAlertDate, &v, AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT );
                            VariantClear( &v );
                        }
                    }

                    // IS THIS A GATEWAY COMM EVENT?
                    CString sTemp = GENERAL_ATTENTION_GATEWAY_QUERY_TOKEN_LIST;
                    if( sTemp.Find( sToken ) >= 0 )
                        sToken = TOKEN_GATEWAYCOMM;  // _T("gatewayComm") 
                    else
                        sToken = m_sLastAlertString;

                    // GET THE DEFAULT MESSAGE
                    GetEventNameAndDefaultMsg( sToken, sEventDefaultMsg, sEventName, s, EVENT_TOKEN_GENERAL_ATTENTION_TABLE );


                    // ADD EVENT NAME HEADING
                    sHeading.LoadString( IDS_HEADING_EVENT_NAME );

                    // ADD EVENT NAME
                    if( sLastAlertString == m_sLastAlertString)  // IF JUST THE TOKEN, DON'T DISPLAY
                        s = sHeading + sEventName;
                    else  if( sEventName.IsEmpty() )
                        s = sLastAlertString;
                    else
                        s = sHeading + sEventName + _T("\r\n") + sLastAlertString;

                    // ADD HEADING
                    sHeading.LoadString( IDS_HEADING_LAST_ALERT );
                    sHeading += sLastAlertDate;
                    sLastAlertString = sHeading + _T("\r\n") + s + _T("\r\n") + sEventDefaultMsg;
                    
                }
            }

            // ADD TEXT FOR LAST ALERT
            if( !sLastAlertString.IsEmpty() )
                m_sGeneralErrorString += sLastAlertString; 

        }
    }

    UpdateData( FALSE );         // FORCE IT TO REDISPLAY THE DATA

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*----------------------------------------------------------------------------
   CServerGeneralError::ParseTokensToStrings

???   m_sGeneralAttentionTokens contains a comma delimted list of numbers.
   Format the resulting strings in m_sGeneralErrorString

   m_sGeneralAttentionTokens is the value from the Registry entry "attention".

   LookUpIcePackTokenString is in UtilPlus.cpp. Using the token, it looks 
   up the string in IcePackTokens.dll.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CServerGeneralError::ParseTokensToStrings()
{
    WCHAR *  lpszDisplayString = NULL;
    WCHAR *  lpszToken         = NULL;
    WCHAR    commaDelimiter[10],szAttentionTokens[512];   // token
	USES_CONVERSION;

    // 12/11/99 FOR NOW, JUST SHOW THE Tokens  · 
    // m_sGeneralErrorString = _T("*   ") + m_sGeneralAttentionTokens + _T("\r\n\r\n") ;
    wcscpy( commaDelimiter, L"," );
    wcscpy( szAttentionTokens, T2W(m_sGeneralAttentionTokens.GetBuffer(0) ));
    m_sGeneralAttentionTokens.ReleaseBuffer(-1);

    if( (lpszToken = wcstok(szAttentionTokens,commaDelimiter)) == NULL )   
        return(TRUE);

    for(;;)
    {
        if( lpszToken == NULL )
            break;

        // LookUpIcePackTokenString
        //lpszToken = m_sGeneralAttentionTokens.GetBuffer(0);
        lpszDisplayString = LookUpIcePackTokenString( lpszToken, ICEPACK_TOKEN_ATTENTION_TABLE );
        
        if( lpszDisplayString == NULL || *lpszDisplayString == 0 )
            m_sGeneralErrorString += lpszToken;
        else
            m_sGeneralErrorString += lpszDisplayString;
        
        // APPEND CRLF TO THE RETURNED STRING
        m_sGeneralErrorString += _T("\r\n\r\n") ;

        // GET NEXT TOKEN
        lpszToken = wcstok(NULL,commaDelimiter);  
        
    }

    // TRIM OFF THE SPACE WE ADDED
    m_sGeneralErrorString.TrimLeft();

    return TRUE;
}



#if 0
    // LookUpIcePackTokenString
    lpszToken = m_sGeneralAttentionTokens.GetBuffer(0);
    lpszDisplayString = LookUpIcePackTokenString( lpszToken, ICEPACK_TOKEN_ATTENTION_TABLE );

    if( lpszDisplayString == NULL )
        m_sGeneralErrorString = m_sGeneralAttentionTokens;
    else
        m_sGeneralErrorString =  lpszDisplayString;

    // APPEND CRLF TO THE RETURNED STRING
    m_sGeneralErrorString += _T("\r\n\r\n") ;

    m_sGeneralAttentionTokens.ReleaseBuffer(-1);

#endif


// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
BOOL CServerGeneralError::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD, m_lpHelpIdArray, pHelpInfo );
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}



// PROP SHEET HELP BUTTON
BOOL CServerGeneralError::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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

void CServerGeneralError::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
    return;
}

#if 0
void CServerGeneralError::OnWhatsThisMenuitem() 
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
