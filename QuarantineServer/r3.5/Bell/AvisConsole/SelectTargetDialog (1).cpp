/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// SelectTargetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "avisconsole.h"
#include "SelectTargetDialog.h"
#include "SelectNavServersDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectTargetDialog dialog


CSelectTargetDialog::CSelectTargetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectTargetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectTargetDialog)
	m_szHeading = _T("");
	//}}AFX_DATA_INIT
}


void CSelectTargetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectTargetDialog)
	DDX_Control(pDX, IDC_SELECT_TARGET_LIST, m_TargetListCtrl);
	DDX_Control(pDX, IDC_HELP_BUTTON_DIALOG2, m_HelpButtonControl);
	DDX_Text(pDX, IDC_EDIT1, m_szHeading);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectTargetDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectTargetDialog)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDADD, OnAdd)
	ON_BN_CLICKED(IDC_HELP_BUTTON_DIALOG2, OnHelpButtonDialog2)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectTargetDialog message handlers



/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectTargetDialog::Initialize(DWORD nIDTitle,
                                     DWORD nIDHeading,
                                     DWORD nIDCol1,
                                     CString& sTargetCommaList,
                                     int iItemCount,
                                     DWORD dwDefaultHelpID,
                                     AVIS_HELP_ID_STRUCT* pHelpIdArray )
{                                    
                                             
    m_szHeading.LoadString(nIDHeading);
    m_nIDTitle         = nIDTitle;
    m_nIDHeading       = nIDHeading;
    m_nIDCol1          = nIDCol1;
    m_iItemCount       = 0;        //iItemCount
    m_dwInitialized    = TRUE;
    m_sTargetStrings   = sTargetCommaList;
    m_bListModified    = FALSE;

    // HELP STUFF
    m_lpHelpIdArray            = pHelpIdArray;   // LOAD HELP MAP 
    m_dwDefaultOverviewHelpID  = dwDefaultHelpID;

    return;
}



/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
BOOL CSelectTargetDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // SET THE DEFAULT LANG FONT
    mySetDefaultFont( GetSafeHwnd() );

    // SETUP LIST CTRL
    m_dwListCtrlFlags = LC_ENABLE_TEXTSELECTION;            // LC_SELECT_ALL  LVS_SHOWSELALWAYS
    m_TargetListCtrl.Initialize(m_dwListCtrlFlags);
    m_TargetListCtrl.ModifyStyle(0L, LVS_SHOWSELALWAYS);


	// INSERT ONE COLUMN (REPORT MODE) AND MODIFY THE NEW HEADER ITEMS
    CString  s2;
    s2.LoadString(m_nIDCol1);
    m_TargetListCtrl.InsertTwoColumns((LPCTSTR)s2, NULL );     // (LPCTSTR)s3

    // POPULATE THE LIST
    FillListWithData();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

                                                     

/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
DWORD CSelectTargetDialog::FillListWithData()
{
    TCHAR     c = 0;
    CString   s;
    int       iTotalLength = 0;
    int       iCurrentPos  = 0;
    int       iLastComma   = 0;
    int       iStartSubString = 0;
    int       iSubStringLength = 0;
    BOOL      bParsingDone = FALSE;
    BOOL      bEndOfString = FALSE;

    // VALIDATE
    m_sTargetStrings.TrimLeft();
    m_sTargetStrings.TrimRight();
    if( m_sTargetStrings.IsEmpty() )
        return(0);
    iTotalLength = m_sTargetStrings.GetLength();

    for(;;)
    {
        if( iCurrentPos < iTotalLength )
            c = m_sTargetStrings.GetAt(iCurrentPos);
        else
        {
            bEndOfString = TRUE;
            c = ',';
        }

        // IS THIS A COMMA, OR END OF STRING?
        if( c == ','  || iCurrentPos >= iTotalLength )      
        {
            
            if( iSubStringLength > 0 ) 
            {
                // GET THE SUBSTRING
                s = m_sTargetStrings.Mid( iStartSubString, iSubStringLength );
                s.Remove( ',' );         // REMOVE ANY COMMAS WE COPIED
                s.TrimLeft();
                s.TrimRight();

                // ADD TO THE LIST CTRL
                m_TargetListCtrl.InsertItemAndSubItem( s.GetBuffer(iSubStringLength+1), NULL, 0 );
                s.ReleaseBuffer();
                //InsertNewItem(CString& s);      // Use CListCtrl
                ++m_iItemCount;

            
                // SETUP FOR NEXT ONE
                iStartSubString = iCurrentPos + 1;
                iSubStringLength= 0;
            }
            else
                ++iStartSubString;

            if( bEndOfString )
                break;
        }

        ++iCurrentPos;
        // DON'T COPY PAST END OF STRING
        //if( iCurrentPos < iTotalLength )
        if( iCurrentPos <= iTotalLength )
            ++iSubStringLength;

        // ARE WE DONE?
        if( iCurrentPos > iTotalLength )
            break;

    }

    return(S_OK);
}



/*----------------------------------------------------------------------------
    CSelectTargetDialog::OnAdd

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectTargetDialog::OnAdd() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CSelectNavServersDialog  dlg;

    dlg.Initialize(IDS_SELECT_NAV_SERVERS_TITLE,
                   IDS_SELECT_NAV_SERVERS_HEADING,
                   IDS_SELECT_NAV_SERVERS_COL1, 
                   &m_TargetListCtrl,              
                   0,              // dwDefaultHelpID
                   g_SelectTargetDialogHelpIdArray );         

    int nResponse = dlg.DoModal();

    // IF OK WAS PRESSED, CSelectNavServersDialog ADDED THE SELECTED ITEMS TO OUR LIST


    return;	
}




/*----------------------------------------------------------------------------
    CSelectTargetDialog::OnDelete()

    Written by: Jim Hill                 m_hWnd,
 ----------------------------------------------------------------------------*/
void CSelectTargetDialog::OnDelete() 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    int      i  = 0;
    int      rc = 0;
    BOOL     bConfirmMessageDisplayed = FALSE;
    POSITION pos_ListCtrl = 0;

    for( i = 0; ; i++)
    {   
        pos_ListCtrl = m_TargetListCtrl.GetFirstSelectedItemPosition();
        if( pos_ListCtrl == NULL )
            break;

        // MAKE USER CONFIRM THE DELETION
        if( !bConfirmMessageDisplayed)
        {
            CString sTitle;
            CString sText; 
            sTitle.LoadString(IDS_DELETE_WARNING_TARGET_LIST);
            sText.LoadString(IDS_DELETE_WARNING_TARGET_LIST_TEXT); 
            rc = MessageBox(  sText, sTitle, MB_ICONEXCLAMATION | MB_YESNO ); // MB_OK
            if( rc != IDYES )
                break;
            bConfirmMessageDisplayed = TRUE;
        }

        // GET THE INDEX
        int nItem = m_TargetListCtrl.GetNextSelectedItem(pos_ListCtrl);
        
        if( ListView_DeleteItem( m_TargetListCtrl.m_hWnd, nItem ) )
            --m_iItemCount;

        m_bListModified = TRUE;
    }


    return;	
}


/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectTargetDialog::OnOK() 
{
    // RETURN THE RESULTS
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    int      i = 0;
    int      rc = 0;
	LV_ITEM  lvi;
    int      nItem = 0;
    TCHAR    szBuff[512];
    CString  s;

    s.Empty();
    memset( szBuff, 0, sizeof(szBuff) );
    for( i = 0; ; i++)     
    {   
        szBuff[0] = 0;

        // RETRIEVE THE ITEM DATA
        memset( &lvi, 0, sizeof(LV_ITEM) );
	    lvi.mask = LVIF_TEXT;    
	    lvi.iItem = nItem;
	    lvi.iSubItem = 0;
	    lvi.pszText = szBuff;
	    lvi.cchTextMax = sizeof(szBuff);
	    rc = m_TargetListCtrl.GetItem(&lvi);

        // REBUILD THE COMMA DELIMITED LIST WE WERE PASSED
        if( szBuff[0] != 0 && rc != 0)
        {
            if( s.IsEmpty() )    // FIRST TIME THROUGH
               s = szBuff;
            else
            {
               s += _T(",");
               s += szBuff;
            }
        }

        // GET THE NEXT ITEM INDEX
        nItem = m_TargetListCtrl.GetNextItem( nItem, LVNI_ALL ); 
        if( nItem < 0 )
            break;

    }

    //m_iTotalReturnedItems = i;
    if( s.IsEmpty() )
    {
        m_iTotalReturnedItems = 0;
        //m_bListModified = FALSE;
    }
    else
    {
        m_iTotalReturnedItems = i+1;
        m_bListModified = TRUE;
    }

    // ANYTHING TO RETURN;
    m_sTargetStrings = s;


	CDialog::OnOK();
}












/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectTargetDialog::OnHelpButtonDialog2() 
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
BOOL CSelectTargetDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
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
void CSelectTargetDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    pContextWnd = pWnd;
    DoHelpDisplayContextMenuWhatsThis( IDD, &nResID, m_lpHelpIdArray, m_hWnd, pWnd, point, 0, &hTargetWnd );
}

#if 0
/*----------------------------------------------------------------------------
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CSelectTargetDialog::OnWhatsThisMenuitem() 
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








