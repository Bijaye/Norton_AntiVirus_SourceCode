/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


// ConfirmationDialog.cpp : implementation file
//
// Old: MY_SYSTEM,_DEBUG,WIN32,_WINDOWS,_USRDLL,_UNICODE,UNICODE,SYM_WIN,SYM_WIN32,_WINDLL,_AFXDLL
// QSConsole:  _DEBUG,WIN32,_WINDOWS,_WINDLL,_AFXDLL,_USRDLL,_UNICODE,SYM_WIN,SYM_WIN32
// New: MY_SYSTEM,_DEBUG,WIN32,_WINDOWS,_WINDLL,_AFXDLL,_USRDLL,_UNICODE,UNICODE,SYM_WIN,SYM_WIN32
// ..\..\..\Core\Source\include\src

#include "stdafx.h"
#include "AvisConsole.h"
//#include "AvisCon.h"
#include "ConfirmationDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CConfirmationDialog dialog


CConfirmationDialog::CConfirmationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfirmationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfirmationDialog)
	m_szHeading = _T("");
	//}}AFX_DATA_INIT

    m_nIDTitle  =0;
    m_nIDHeading=0;
    m_nIDCol1   =0;
    m_nIDCol2   =0;
    m_dwDefaultHelpID = 0;
    m_lpItemList=NULL;
    m_dwInitialized =FALSE;
    m_dwListCtrlFlags=0;                                 // FLAGS PASSED TO myLISTCTRL
    m_iTotalSelectedItems = 0;
//    m_SelectedItemList;
}


void CConfirmationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfirmationDialog)
	DDX_Control(pDX, IDC_HELP_BUTTON_DIALOG2, m_HelpButtonCtrl);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_szHeading);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfirmationDialog, CDialog)
	//{{AFX_MSG_MAP(CConfirmationDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_HELP_BUTTON_DIALOG2, OnHelpButtonDialog2)
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmationDialog message handlers


//void Initialize(LPCTSTR lpszTitle,LPCTSTR lpszHeading,LPCTSTR lpszCol1,LPCTSTR lpszCol2)
/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CConfirmationDialog::Initialize(DWORD nIDTitle,DWORD nIDHeading,DWORD nIDCol1,
                                     DWORD nIDCol2, DWORD dwDefaultHelpID,
                                     LIST_DATA_STRUCT *lpItemList, int iItemCount,
                                     AVIS_HELP_ID_STRUCT* pHelpIdArray ) 
{

    m_szHeading.LoadString(nIDHeading);
    m_nIDTitle        = nIDTitle;
    m_nIDHeading      = nIDHeading;
    m_nIDCol1         = nIDCol1;
    m_nIDCol2         = nIDCol2;
    m_lpItemList      = lpItemList;
    m_iItemCount      = iItemCount;
    m_dwInitialized   = TRUE;

    m_dwDefaultHelpID = dwDefaultHelpID;
    if( dwDefaultHelpID          == IDH_SUBMIT_CONFIRM_LISTBOX )
        m_dwDefaultOverviewHelpID = IDH_OVERVIEW_SUBMIT_FILES_DIALOG;
    else if( dwDefaultHelpID     == IDH_DELIVER_CONFIRM_LISTBOX )
        m_dwDefaultOverviewHelpID = IDH_OVERVIEW_DELIVER_DEFS_DIALOG;
    else
        m_dwDefaultOverviewHelpID = dwDefaultHelpID;


    m_lpHelpIdArray   = pHelpIdArray;   // LOAD HELP MAP g_ConfirmationDialogHelpIdArray

    return;
}


#include <locale.h>    // 7/15

/*----------------------------------------------------------------------------

    Written by: Jim Hill               LVS_SHOWSELALWAYS
 ----------------------------------------------------------------------------*/
BOOL CConfirmationDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here

    // CHANGE THE WINDOW TITLE
    CString  s1;
    s1.LoadString(m_nIDTitle);
    SetWindowText(s1);

    // SETUP LIST CTRL
    m_dwListCtrlFlags = LC_SELECT_ALL;
    m_ListCtrl.Initialize(m_dwListCtrlFlags);

	// INSERT TWO COLUMNS (REPORT MODE) AND MODIFY THE NEW HEADER ITEMS
    CString  s2,s3;
    s2.LoadString(m_nIDCol1);
    s3.LoadString(m_nIDCol2);
    m_ListCtrl.InsertTwoColumns((LPCTSTR)s2,(LPCTSTR)s3);     // 


    // TEST 7/16
    TCHAR *lpszLocale = NULL;
    TCHAR szLocale[259];
    lpszLocale = _tsetlocale( LC_ALL, NULL );   // English  _T("Japanese")
    szLocale[0] = 0;
    if( lpszLocale != NULL )
    {
        //STRCPY( szLocale, lpszLocale );
        GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, sizeof(szLocale)); 
        fWidePrintString( "CConfirmationDialog::OnInitDialog  Locale= %s  %s ", lpszLocale, szLocale );
    }

    // POPULATE THE LIST
    FillListWithData();

    // CAUSE IT TO SHOW ALL SELECTED IF LC_SELECT_ALL IS SET
    if(m_dwListCtrlFlags & LC_SELECT_ALL) {
        m_ListCtrl.SetFocus();   
        return(FALSE);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CConfirmationDialog::OnOK() 
{
    // RETURN THE RESULTS
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HRESULT hr = S_OK;
    int      i = 0;
    POSITION pos_ListCtrl = 0;

    // DON'T RETURN RESULTS WHEN IT'S READ ONLY
    if((m_dwListCtrlFlags & LC_ENABLE_READONLY_LOOK) == 0)
    {
        pos_ListCtrl = m_ListCtrl.GetFirstSelectedItemPosition();
        for( i = 0; pos_ListCtrl != NULL; i++)
    	{   
            // GET THE INDEX
            int nItem = m_ListCtrl.GetNextSelectedItem(pos_ListCtrl);
            LPARAM lParam = m_ListCtrl.GetItemData(nItem);
            hr = ReturnSelectedItem( nItem, lParam );
        }
    }
    m_iTotalSelectedItems = i;
	CDialog::OnOK();
}


// SET THE ISSELECTED FLAG IN THE LIST ITEM PASSED TO US BY THE CALLER
HRESULT CConfirmationDialog::ReturnSelectedItem( int nItem, LPARAM lParam )
{
    LIST_DATA_STRUCT  *pItemList = m_lpItemList;

    if( m_lpItemList == NULL || lParam == 0 )
        return(E_FAIL);

    // LOOK IT UP BY VALUE OF LPARAM
    for( int i = 0; i < m_iItemCount ; i++, pItemList++ )
    {
        if( pItemList->lParam == lParam )
        {
            pItemList->iIsSelected = TRUE;
            return(S_OK);
        }
    }
    return(E_FAIL);
}






/*----------------------------------------------------------------------------
    CConfirmationDialog::FillListWithData

    By walking thru the list twice, the enabled items are at the top,
    and the disabled at the bottom.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD CConfirmationDialog::FillListWithData()
{
     LIST_DATA_STRUCT  *lpDataList = m_lpItemList;
     LPTSTR  lpszItem=NULL;
     LPTSTR  lpszSubItem=NULL;
     LPARAM  lParam = 0;

     // VALIDATE
     if(lpDataList==NULL)
        return(0);

     // WALK THROUGH THE DATA FOR ENABLED ITEMS FIRST
     for(int i = 0; i < m_iItemCount; i++) 
     {
        if( lpDataList->iIsDisabled == 0 )
        {
            lpszItem    = lpDataList->sItemStr;
            lpszSubItem = lpDataList->sSubItemStr;
            lParam      = lpDataList->lParam;
        
            // ARE WE DONE?
            //if(lpszItem==NULL || lpszSubItem==NULL)
            //    break;
        
            //m_ListCtrl.InsertItemAndSubItem( lpszItem, lpszSubItem, lParam );
            m_ListCtrl.InsertItemAndSubItem( lpszItem, lpszSubItem, lParam );
        }

        // POINT TO THE NEXT PAIR
        lpDataList++;
     }

     // NOW DO THE DISABLED ITEMS SO THEY APPEAR AT THE BOTTOM OF THE LIST GRAYED-OUT
     lpDataList = m_lpItemList;
     for(int j = 0; j < m_iItemCount; j++) 
     {
        if( lpDataList->iIsDisabled )
        {
            lpszItem    = lpDataList->sItemStr;
            lpszSubItem = lpDataList->sSubItemStr;
            lParam      = lpDataList->lParam;

            m_ListCtrl.InsertItemAndSubItem( lpszItem, lpszSubItem, lParam );
        }

        // POINT TO THE NEXT PAIR
        lpDataList++;
     }

     return(i+j);
}



    // m_ListCtrl.InsertItemAndSubItem(L"sam.morse.com"         ,L"c:\file1.doc");
    // m_ListCtrl.InsertItemAndSubItem(L"srwhite.watson.ibm.com",L"d:\file2.doc");
    // m_ListCtrl.InsertItemAndSubItem(L"morar.watson.ibm.com"  ,L"c:\file2.doc");
    // m_ListCtrl.InsertItemAndSubItem(L"pring.watson.ibm.com"  ,L"d:\file1.doc");




/*----------------------------------------------------------------------------

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CConfirmationDialog::OnDestroy() 
{

    CDialog::OnDestroy();
	// TODO: Add your message handler code here
	
}


/*----------------------------------------------------------------------------
    CConfirmationDialog::OnHelpButtonDialog2
    Provide Help when the help button pressed in the dialog.
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CConfirmationDialog::OnHelpButtonDialog2() 
{
	// TODO: Add your control notification handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    //DWORD dwResID = m_dwDefaultOverviewHelpID;
    NMHDR sNMHDR;
    memset(&sNMHDR, 0 ,sizeof(NMHDR));
    sNMHDR.code = PSN_HELP;
    DoHelpPropSheetHelpButton( m_dwDefaultOverviewHelpID, m_lpHelpIdArray, &sNMHDR ); // IDD_DIALOG2
    // CAUSE IT TO SHOW ALL SELECTED IF LC_SELECT_ALL IS SET
    if(m_dwListCtrlFlags) 
        m_ListCtrl.SetFocus();   
    return;
}


// F1 KEY PRESSED OR WHAT-IS ? CLICKED ON A FIELD
BOOL CConfirmationDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
    AFX_MANAGE_STATE(AfxGetStaticModuleState());           
    DWORD dwResID = m_dwDefaultOverviewHelpID;
    if( pHelpInfo->iCtrlId == IDC_LIST1 )             //  || 
    {
        pHelpInfo->iCtrlId = m_dwDefaultHelpID;
        dwResID            = m_dwDefaultOverviewHelpID;
    }
    else  if( pHelpInfo->iCtrlId == -1 )      // pHelpInfo->iCtrlId == IDD_DIALOG2
    {
        pHelpInfo->iCtrlId = m_dwDefaultOverviewHelpID;
        dwResID            = m_dwDefaultHelpID;      
    }
    DoHelpPropSheetF1( dwResID, m_lpHelpIdArray, pHelpInfo );     // 
	return CDialog::OnHelpInfo(pHelpInfo);
}

// PROP SHEET HELP BUTTON
BOOL CConfirmationDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    AFX_MANAGE_STATE(AfxGetStaticModuleState());	 
	NMHDR* pNMHDR = (NMHDR*)lParam;
    // WAS HELP BUTTON ON PROPERTY SHEET PRESSED?  Debugging only

    if( pNMHDR->code == PSN_HELP)   //********* in "Prsht.h"
        DoHelpPropSheetHelpButton(m_dwDefaultOverviewHelpID, m_lpHelpIdArray, pNMHDR );  // IDD_DIALOG2
	return CDialog::OnNotify(wParam, lParam, pResult);
}


static DWORD nResID = 0;
static CWnd* pContextWnd = NULL;
static DWORD hTargetWnd = 0;
// WHAT'S THIS CONTEXT MENU
void CConfirmationDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DWORD dwForcedDefault = 0;
    DWORD nID = pWnd->GetDlgCtrlID();
    if( nID == 0)                       // IT'S O WHEN IT'S THE DILOG BOX ITSELF
    {
        nID = m_dwDefaultOverviewHelpID; 
        dwForcedDefault = nID;
    }
    else if( nID == IDC_LIST1 )          // IS IT POINTING TO THE LISTBOX OBJECT
    {
        nID = m_dwDefaultHelpID;
        dwForcedDefault = nID;
    }

    DoHelpDisplayContextMenuWhatsThis( nID, &nResID, m_lpHelpIdArray, m_hWnd,   //  IDD_DIALOG2
                                       pWnd, point, dwForcedDefault, &hTargetWnd);

    // CAUSE IT TO SHOW ALL SELECTED IF LC_SELECT_ALL IS SET
    if(m_dwListCtrlFlags) 
        m_ListCtrl.SetFocus();   

	return;
}

#if 0
// WHAT'S THIS CONTEXT MENU SELECTED
void CConfirmationDialog::OnWhatsThisMenuitem() 
{
	// TODO: Add your command handler code here
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    HWND hWnd = (HWND) hTargetWnd;
    hTargetWnd= 0;
    int nResourceID = nResID;
    nResID = 0;                  // CLEAR     
    if( nResourceID == 0 || nResourceID == IDD_CONFIRMDIALOG)
        nResourceID = m_dwDefaultOverviewHelpID;                   // m_dwDefaultHelpID   
    DoHelpPropSheetContextHelp( nResourceID, m_lpHelpIdArray, pContextWnd, hWnd );
	return;
}

#endif
// void CConfirmationDialog::OnRButtonDown(UINT nFlags, CPoint point) 
// {
// 	// TODO: Add your message handler code here and/or call default
// 	if( nFlags | MK_RBUTTON )        // DON'T PASS RIGHT MOUSE CLICK IN HERE. IT CAUSES THE SELECTION TO BE LOST.
//     	return;
// 
// 	CDialog::OnRButtonDown(nFlags, point);
// 	return;
// }
