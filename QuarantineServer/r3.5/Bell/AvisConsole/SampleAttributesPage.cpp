/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// SampleAttributesPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AvisConsole.h"
#include "SampleAttributesPage.h"
#include "mmc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "AttributeAccess.h"
#include "GetAllAttributes.h"

// HOW MANY SHEETS DOES THE USER HAVE UP IN THE RESULT PANE?
static int g_ResultPropSheetTotalCount = 0;




/////////////////////////////////////////////////////////////////////////////
// CSampleAttributesPage property page

IMPLEMENT_DYNCREATE(CSampleAttributesPage, CPropertyPage)

CSampleAttributesPage::CSampleAttributesPage() : CPropertyPage(CSampleAttributesPage::IDD)
{
	//{{AFX_DATA_INIT(CSampleAttributesPage)
	m_sFilename = _T("");
	//}}AFX_DATA_INIT

    // INITIALIZE
    ++g_ResultPropSheetTotalCount;
    m_pSnapin       = NULL;
    m_dwRef         = 1;
    m_dwRegistered  = 0;
    m_lpHelpIdArray = g_SampleAttributesHelpIdArray;  // LOAD THE HELP MAP
    m_pQServerGetAllAttributes = NULL;
    m_pAttributeAccess         = NULL;
    m_pSampleAttributesPage    = NULL;
    m_pSampleActionsPage       = NULL;
    m_pSampleErrorPage         = NULL;

}


CSampleAttributesPage::~CSampleAttributesPage()
{
    --g_ResultPropSheetTotalCount;
  	fWidePrintString("CSampleAttributesPage destructor called. Caller's ObjectID= %d", m_iExtDataObjectID);
}

void CSampleAttributesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleAttributesPage)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_IconButton);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Text(pDX, IDC_NAME_EDIT, m_sFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSampleAttributesPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSampleAttributesPage)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleAttributesPage message handlers



// USED BY ROUTINES THAT FREE UP THE MFC CPROPERTYPAGE MEMORY
static LPFNPSPCALLBACK   pfnMmcCallback = NULL;
  
/*----------------------------------------------------------------------------
   myPropPageCallback()
   Free up the MFC CPropertyPage memory

   Written by: Jim Hill
----------------------------------------------------------------------------*/
static UINT CALLBACK myPropPageCallback(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
    // CALL ORIGINAL MMC CALLBACK. 
    if( pfnMmcCallback != NULL)
    {
        ppsp->pfnCallback = pfnMmcCallback;  // restore original ptr
        (*pfnMmcCallback)(hWnd, uMsg, ppsp);
    }


	if( uMsg == PSPCB_RELEASE )
    {
        fPrintString("CSampleAttributesPage myPropPageCallback called with PSPCB_RELEASE");
        CSampleAttributesPage* pPlatformAttributesObject = (CSampleAttributesPage*) ppsp->lParam;
        if(IsAddressValid((void *)pPlatformAttributesObject, sizeof(CSampleAttributesPage)))        // pPlatformAttributesObject == NULL
        {
            if(pPlatformAttributesObject->VerifyCorrectPage(pPlatformAttributesObject))
                pPlatformAttributesObject->Release();
        }
        else
            fPrintString("myPropPageCallback  pSampleAttributesPage is an invalid address ----------------");
    }

	return 1;
}

/*----------------------------------------------------------------------------
   CSampleAttributesPage::HookPropPageCallback()
   Free up the MFC CPropertyPage memory

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CSampleAttributesPage::HookPropPageCallback()   // CSampleAttributesPage* pSampleAttributesPage
{
    void *lpfnCall    = myPropPageCallback;

    pfnMmcCallback    = m_psp.pfnCallback;   // SAVE THE CALLBACK TO MMC
    m_psp.pfnCallback = myPropPageCallback;  // SUBSTITUTE OUR CALLBACK


    return(TRUE);
}

/*----------------------------------------------------------------------------
   CSampleAttributesPage::VerifyCorrectPage()
   Free up the MFC CPropertyPage memory

   Written by: Jim Hill
----------------------------------------------------------------------------*/
BOOL CSampleAttributesPage::VerifyCorrectPage(CSampleAttributesPage*  pSampleAttributesPage)
{ 
    if(pSampleAttributesPage == NULL || m_pSampleAttributesPage== NULL)
        return(FALSE);
    if(pSampleAttributesPage == m_pSampleAttributesPage)  
        return(TRUE);
    else
        return(FALSE);
}

/*----------------------------------------------------------------------------
   CSampleAttributesPage::Release()
   Free up the MFC CPropertyPage memory
   Also calls Release() in the other 2 Prop Pages.

   Written by: Jim Hill
----------------------------------------------------------------------------*/
DWORD CSampleAttributesPage::Release() 
{ 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    
    --m_dwRef;

    // WERE WE CALLED FROM PostNcDestroy()
    if( m_dwRef == 1 )
        m_dwRef = 0;

    if( m_dwRef == 0 )
    {
//-  DISABLE 5/24/99
//-         // DELETE CSampleAttributesPage 
//-        try
//-        {
//-    	    fWidePrintString("CSampleAttributesPage::Release called. Deleting ObjectID= %d", m_iExtDataObjectID);
//-
//-            if( m_pSampleAttributesPage )
//-            {
//-                if( m_pSampleAttributesPage->Release() == 0)
//-                {
//-                    m_pSampleAttributesPage = NULL;
//-                }
//-            }
//-        }
//-        catch(...) 
//-        {
//-            m_pSampleAttributesPage= NULL;
//-        } 

        // DELETE CSampleActionsPage 
        try
        {
            if( m_pSampleActionsPage )
            {
                if( m_pSampleActionsPage->Release() == 0)
                    m_pSampleActionsPage = NULL;
            }
        }
        catch(...) 
        {
            m_pSampleActionsPage= NULL;
        } 

        // DELETE m_pSampleErrorPage
        try
        {
            if( m_pSampleErrorPage )
            {
                if( m_pSampleErrorPage->Release() == 0)
                    m_pSampleErrorPage = NULL;
            }
        }
        catch(...) 
        {
            m_pSampleErrorPage= NULL;
        } 

        // TELL THE ACCESS ATTRIBUTE OBJECT WE'RE NOT USING IT ANYMORE
        if( m_pAttributeAccess )
        {
            m_pAttributeAccess->Release();
            m_pAttributeAccess = NULL;
        }

        // DELETE CSampleAttributesPage 
        delete this;
        return 0;
    }
    return m_dwRef;
}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSampleAttributesPage::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::PostNcDestroy();

    //fWidePrintString("CSampleAttributesPage::PostNcDestroy()  called");
    Release();       

}


/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/


//#include <locale.h>    // 7/15

/*----------------------------------------------------------------------------
    CSampleAttributesPage::OnInitDialog

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleAttributesPage::OnInitDialog() 
{
	// Call the base class
	CPropertyPage::OnInitDialog();

    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // INITIALIZE. FIRST PAGE TO CALL FROM OnInitDialog CAUSES THE READ
    if( m_pAttributeAccess )
    {
        m_pQServerGetAllAttributes = m_pAttributeAccess->Initialize();
        ++m_dwRegistered;
    }

    ++m_dwRef;
    DWORD dwflags=0;

    // SETUP LIST CTRL
    dwflags = LC_ENABLE_READONLY_LOOK;
    //dwflags = LC_ENABLE_TEXTSELECTION;
    //dwflags = LC_SELECT_ALL;
    m_ListCtrl.Initialize( dwflags, LVS_SORTASCENDING );     // ALPHA SORT LIST 
    //    m_ListCtrl.Initialize( dwflags, 0 );

	// INSERT TWO COLUMNS (REPORT MODE) AND MODIFY THE NEW HEADER ITEMS
    CString  s1,s2;
    s1.LoadString(IDS_COLUMN1_NAME);
    s2.LoadString(IDS_COLUMN2_NAME);
    m_ListCtrl.InsertTwoColumns((LPCTSTR)s1,(LPCTSTR)s2);         // "Attribute","Value"

    // TEST 7/16
    PrintLocaleInfo( "CSampleAttributesPage::OnInitDialog");


    // POPULATE THE LIST
    BOOL bRet = FillListWithData();
    if( bRet )
    {
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

    if(dwflags & LC_SELECT_ALL) {
        m_ListCtrl.SetFocus();   // Cause it to show all selected if LC_SELECT_ALL
        return(FALSE);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/*----------------------------------------------------------------------------
    CSampleAttributesPage::FillListWithData

    Written by: Jim Hill                               
 ----------------------------------------------------------------------------*/
BOOL CSampleAttributesPage::FillListWithData()
{
    BOOL bRet = 0;

    bRet = AddWildcardHeadersAndValuesToListCtrl(AVIS_WILDCARD_ALL_CATEGORY);
    //bRet = AddWildcardHeadersAndValuesToListCtrl(NULL);   // All, plus ones that don't start with X-

    return(bRet);
}


/*----------------------------------------------------------------------------
    CSampleAttributesPage::AddWildcardHeadersAndValuesToListCtrl

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSampleAttributesPage::AddWildcardHeadersAndValuesToListCtrl(LPCTSTR lpszWildcard)
{
    HRESULT hr          = S_OK;
    DWORD   dwIndex     = 0;
    DWORD   dwNumFields = 0;
    DWORD   dwFormat    = 0;
    VARIANT v, v2;
    CString sItem, sSubItem, sWildCard, sDisplayName;
    VariantInit( &v );
    VariantInit( &v2 );
    
    try
    {
        // VALIDATE
        if( m_pQServerGetAllAttributes == NULL )
            return(FALSE);
        if( !((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->IsDataAvailable() )
            return(FALSE);
    
        // INITIALIZE
        dwNumFields = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->GetNumFields();
        sWildCard = lpszWildcard;          

        // SEARCH
        for( ; dwIndex < dwNumFields ; dwIndex++ )
        {
            hr = ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->
                           GetValueByPartialAttributeName( &v, (LPCTSTR)sWildCard, &v2, &dwIndex );
            if( SUCCEEDED( hr ) )  
            {
                sItem.Empty();
                sSubItem.Empty();

                // DON'T DISPLAY DELETED ATTRIBUTES 4/7/00
                if( !IsAttributeDeleted( &v2 ) && ConvertVariantToString( sItem, &v, AVIS_ATTRIBUTE_FORMAT_USE_DEFAULT ) )
                {
                    sItem.TrimLeft();   // jhill 2/27/00   
                    dwFormat = GetFormatByAttributeText( (LPCTSTR) sItem ); 

                    if( dwFormat != AVIS_ATTRIBUTE_FORMAT_EXCLUDE && ConvertVariantToString( sSubItem, &v2, dwFormat ) )
                    {
                        // if( !sSubItem.IsEmpty() )
                        if( !sItem.IsEmpty() && !sSubItem.IsEmpty() )  // jhill 2/27/00   
                        {
                            // IS THIS THE STATUS FIELD?
                            if( dwFormat == AVIS_ATTRIBUTE_FORMAT_LOOKUP_SAMPLE_STATUS_TEXT )
                            {
                                TCHAR *endptr  = NULL;
                                DWORD dwStatus = STRTOUL( (LPCTSTR)sSubItem, &endptr, 10 );
                                ((CQServerGetAllAttributes*)m_pQServerGetAllAttributes)->GetStatusText( sSubItem, dwStatus );
                            }

                            sDisplayName = sItem;

                            // ADD TO THE LIST CTRL
                            m_ListCtrl.InsertItemAndSubItem( (LPTSTR)((LPCTSTR) sDisplayName), (LPTSTR)((LPCTSTR) sSubItem) );
                        }
                    }
                }
            }
			VariantClear(&v);
			VariantClear(&v2);
        }

    }
    catch(...)
    {
        return(FALSE);
    }
    return(TRUE);
}






//
// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
BOOL CSampleAttributesPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    DWORD dwResID = IDH_OVERVIEW_PLATFORM_ATTRIB_LIST;
    if( pHelpInfo->iCtrlId == IDC_LIST1 )             //  || 
    {
        pHelpInfo->iCtrlId = IDH_PLATFORM_ATTRIBS_LISTBOX;
        dwResID            = IDH_OVERVIEW_PLATFORM_ATTRIB_LIST;
    }
    else  if( pHelpInfo->iCtrlId == -1 )      
    {
        pHelpInfo->iCtrlId = IDH_OVERVIEW_PLATFORM_ATTRIB_LIST;
        dwResID            = IDH_PLATFORM_ATTRIBS_LISTBOX;      
    }

    DoHelpPropSheetF1( dwResID, m_lpHelpIdArray, pHelpInfo );  
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

// PROP SHEET HELP BUTTON
BOOL CSampleAttributesPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	NMHDR* pNMHDR = (NMHDR*)lParam;
    // WAS HELP BUTTON ON PROPERTY SHEET PRESSED?  Debugging only
    if( pNMHDR->code == PSN_HELP)   //********* in "Prsht.h"
        DoHelpPropSheetHelpButton( IDH_OVERVIEW_PLATFORM_ATTRIB_LIST, m_lpHelpIdArray, pNMHDR );  
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}



static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;
static TCHAR szAttributeName[259] = {0};

// GO BACK TO DISPLAYING A HELP PAGE FOR THE LIST BOX HERE. 7/7/99
#define USE_CONTEXTMENU_FOR_ATTRIBUTE_HELP  0

/*--------------------------------------------------------------------

   Written by: Jim Hill                 
--------------------------------------------------------------------*/
void CSampleAttributesPage::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    nResID      = 0;          
    pContextWnd = NULL;  
    hTargetWnd  = 0;      
    szAttributeName[0] = 0;

#if USE_CONTEXTMENU_FOR_ATTRIBUTE_HELP  
    // IS THIS AN ATTRIBUTE NAME INSIDE THE LISTBOX
    m_ListCtrl.GetSelectedAttributeName( szAttributeName );
    if( szAttributeName[0] == 0 )   // THIS WILL CAUSE IT TO DISPLAY ONLY ATTRIBUTE NAMES
        return;
#endif

    pContextWnd = pWnd;
    DWORD dwForcedDefault = 0;
    DWORD nID = pWnd->GetDlgCtrlID();
    if( nID == 0)                       // IT'S O WHEN IT'S THE DIALOG BOX ITSELF
    {
        nID = IDH_OVERVIEW_PLATFORM_ATTRIB_LIST; 
        dwForcedDefault = 0;
    }
    // else if( nID == IDC_LIST1 )          // IS IT POINTING TO THE LISTBOX OBJECT
    // {
    //     nID = IDH_PLATFORM_ATTRIBS_LISTBOX;
    //     dwForcedDefault = nID;
    // }

    DoHelpDisplayContextMenuWhatsThis( nID, &nResID, m_lpHelpIdArray, m_hWnd, 
                                       pWnd, point, dwForcedDefault, &hTargetWnd ); 
    return;
}
#if 0
/*--------------------------------------------------------------------

   Written by: Jim Hill                 
--------------------------------------------------------------------*/
void CSampleAttributesPage::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR
    //if( nResourceID == 0)
    //    nResourceID = IDH_OVERVIEW_PLATFORM_ATTRIB_LIST;

#if USE_CONTEXTMENU_FOR_ATTRIBUTE_HELP  
    if( szAttributeName[0] == 0 )
    {
        DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );  
    }
    if( szAttributeName[0] != 0 )
    {   
        // LOOKUP CONTEXT HELP FOR EACH ATTRIBUTE IN LIST
        DoHelpAttributeNameContextHelp( nResourceID, szAttributeName, pContextWnd, hWnd );  
    }
#else
    // 7/7/99 DO HELP ON THE LIST BOX ITSELF
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );  
#endif

    pContextWnd = NULL;
    return;
}
#endif