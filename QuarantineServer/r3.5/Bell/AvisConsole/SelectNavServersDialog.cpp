/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// SelectNavServersDialog.cpp : implementation file
//

#include "stdafx.h"
#include "avisconsole.h"
#include "SelectNavServersDialog.h"
#include "ListNavServers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectNavServersDialog dialog


CSelectNavServersDialog::CSelectNavServersDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectNavServersDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectNavServersDialog)
	m_szHeading = _T("");
	//}}AFX_DATA_INIT
}


void CSelectNavServersDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectNavServersDialog)
	DDX_Control(pDX, IDC_SELECT_NAV_SERVERS_LIST, m_NavServerListCtrl);
	DDX_Control(pDX, IDC_HELP_BUTTON_DIALOG2, m_HelpButtonControl);
	DDX_Text(pDX, IDC_EDIT_HEADING, m_szHeading);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectNavServersDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectNavServersDialog)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_HELP_BUTTON_DIALOG2, OnHelpButtonDialog2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectTargetDialog message handlers



/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectNavServersDialog::Initialize(DWORD nIDTitle,
                                     DWORD nIDHeading,
                                     DWORD nIDCol1,
                                     CMyListCtrl* pListCtrl,
                                     DWORD dwDefaultHelpID,
                                     AVIS_HELP_ID_STRUCT* pHelpIdArray )
{                                    
    m_pCallerListCtrl  = pListCtrl;                                
    m_szHeading.LoadString(nIDHeading);
    m_nIDTitle         = nIDTitle;
    m_nIDHeading       = nIDHeading;
    m_nIDCol1          = nIDCol1;
    m_dwInitialized    = TRUE;
    m_bListModified    = FALSE;
    m_iTotalReturnedItems= 0;
    //m_pSServerData     = NULL;


    // HELP STUFF
    m_lpHelpIdArray            = pHelpIdArray;   // LOAD HELP MAP 
    m_dwDefaultOverviewHelpID  = dwDefaultHelpID;

    return;
}



/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSelectNavServersDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // SETUP LIST CTRL
    m_dwListCtrlFlags = LC_ENABLE_TEXTSELECTION;            // LC_SELECT_ALL  LVS_SHOWSELALWAYS
    m_NavServerListCtrl.Initialize(m_dwListCtrlFlags);
    m_NavServerListCtrl.ModifyStyle(0L, LVS_SHOWSELALWAYS);


	// INSERT ONE COLUMN (REPORT MODE) AND MODIFY THE NEW HEADER ITEMS
    CString  s2;
    s2.LoadString(m_nIDCol1);
    m_NavServerListCtrl.InsertTwoColumns((LPCTSTR)s2, NULL );     // (LPCTSTR)s3

    // POPULATE THE LIST
    FillListWithData();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

                                                     

/*----------------------------------------------------------------------------
    CSelectNavServersDialog::FillListWithData()
    Populate our list with Terry's function to ask Topology service for 
    all the known NAV Servers.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD CSelectNavServersDialog::FillListWithData()
{
    SServerData *pSServerData = NULL;
	SServerData	*psdList      = NULL;
    long         dwNumRecords = 0;
    HRESULT      hr = 0;
	USES_CONVERSION;

    hr = ListNavServers(&pSServerData, &dwNumRecords);
    if( pSServerData != NULL )
    {
        psdList = pSServerData;
		if(SUCCEEDED(hr) && dwNumRecords > 0 )
		{
            for(int i = 0; i < dwNumRecords; i++, psdList++ )
            {
                // ADD TO THE LIST CTRL
                m_NavServerListCtrl.InsertItemAndSubItem( OLE2T(psdList->bstrName), NULL, 0 );
            }
        }

        // FREE IT
        delete [] pSServerData;
        return(S_OK);
    }

    // HANDLE NOT AVAIABLE
    if( pSServerData == NULL || !SUCCEEDED(hr) )
    {
        CString sTitle;
        CString sText; 
        sTitle.LoadString( IDS_ADD_NAV_SERVERS_ERROR_TITLE );
        sText.LoadString( IDS_ADD_NAV_SERVERS_ERROR_TEXT ); 
        MessageBox(  sText, sTitle, MB_ICONEXCLAMATION | MB_OK );  
    }

    return(S_OK);
}


//    // ADD TO THE LIST CTRL
//    m_NavServerListCtrl.InsertItemAndSubItem( _T("Test0"), NULL, 0 );



/*----------------------------------------------------------------------------
    CSelectNavServersDialog::OnOK()
    This will add the selections directly to the Caller's List Ctrl

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectNavServersDialog::OnOK() 
{
    // RETURN THE RESULTS
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    int      rc = 0;
    TCHAR    szBuff[512];
    BOOL     bIsDupe = FALSE;
	LV_ITEM  lvi;
    int      nItem = 0;
    POSITION pos_ListCtrl = 0;

    memset( szBuff, 0, sizeof(szBuff) );
    m_bListModified = FALSE;
    m_iTotalReturnedItems = 0;

    // CAN WE RETURN DATA?
    if( m_pCallerListCtrl )
    {
        pos_ListCtrl = m_NavServerListCtrl.GetFirstSelectedItemPosition();
        for( int i = 0; pos_ListCtrl != NULL; i++)
        {   
            // GET THE INDEX
            nItem = m_NavServerListCtrl.GetNextSelectedItem(pos_ListCtrl);
            szBuff[0] = 0;
        
            // RETRIEVE THE ITEM DATA
            memset( &lvi, 0, sizeof(LV_ITEM) );
		    lvi.mask = LVIF_TEXT;    
		    lvi.iItem = nItem;
		    lvi.iSubItem = 0;
		    lvi.pszText = szBuff;
		    lvi.cchTextMax = sizeof(szBuff);
		    rc = m_NavServerListCtrl.GetItem(&lvi);
        
            // DO WE HAVE A SERVER NAME?
            if( szBuff[0] != 0 )
            {
                //  DON'T ADD DUPLICATE NAMES
                bIsDupe = IsItemDuplicate( szBuff ); 
        
                // SHOULD WE ADD IT?
                if( !bIsDupe )
                {
                    rc = m_pCallerListCtrl->InsertItemAndSubItem( szBuff, NULL, 0 );
                    m_bListModified = TRUE;
                    ++m_iTotalReturnedItems;
                }
            }
        }
    }

	CDialog::OnOK();
}


/*----------------------------------------------------------------------------
    CSelectNavServersDialog::IsItemDuplicate
    Don't add duplicate names

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSelectNavServersDialog::IsItemDuplicate( LPTSTR lpszNewItem )
{
    int      i = 0;
    int      rc = 0;
	LV_ITEM  lvi;
    TCHAR    szBuff[512];
    CString  s1,s2;
    int      iTotalItems = 0;

    if( m_pCallerListCtrl )
         iTotalItems = m_pCallerListCtrl->GetItemCount();

    s1 = lpszNewItem;
    memset( szBuff, 0, sizeof(szBuff) );
    for( i = 0; i < iTotalItems ; i++)
    {   
         szBuff[0] = 0;

         // RETRIEVE THE ITEM DATA
         memset( &lvi, 0, sizeof(LV_ITEM) );
	     lvi.mask = LVIF_TEXT;    
	     lvi.iItem = i;
	     lvi.iSubItem = 0;
	     lvi.pszText = szBuff;
	     lvi.cchTextMax = sizeof(szBuff);
	     rc = m_pCallerListCtrl->GetItem(&lvi);
         if( rc == 0 )
             break;
         s2 = szBuff;

         if( s1 == s2 )
            return(TRUE);

    }

    return(FALSE);
}


















/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectNavServersDialog::OnHelpButtonDialog2() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    NMHDR sNMHDR;
    memset(&sNMHDR, 0 ,sizeof(NMHDR));
    sNMHDR.code = PSN_HELP;
    DoHelpPropSheetHelpButton( m_dwDefaultOverviewHelpID, m_lpHelpIdArray, &sNMHDR ); // IDD_DIALOG2
}


/*----------------------------------------------------------------------------
    F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSelectNavServersDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
    DoHelpPropSheetF1( IDD, m_lpHelpIdArray, pHelpInfo );
	return CDialog::OnHelpInfo(pHelpInfo);
}


static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;
/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectNavServersDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
}

#if 0
/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectNavServersDialog::OnWhatsThisMenuitem() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR
    if( nResourceID == 0)
        nResourceID = IDD;
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );
}

#endif








