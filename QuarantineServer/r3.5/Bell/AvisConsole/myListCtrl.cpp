/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


/*----------------------------------------------------------------------------
    myListCtrl.cpp

    Class:      CMyListCtrl
    Base class: CListCtrl

    Functionality Added:
        Full row selection
        Can startup with all items initially selected
        Optional read only look, with selection disabled

    Limitations:
        Only handles two columns.
        Read only mode uses only Light Frey for the background.


    Public:
    CMyListCtrl::Initialize(DWORD dwFlags)
    CMyListCtrl::InsertTwoColumns(LPCTSTR lpszColumn1Name,LPCTSTR lpszColumn2Name)
    CMyListCtrl::InsertItemAndSubItem(LPTSTR lpszItem,LPTSTR lpszSubItem)
  
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/

#include "stdafx.h"
#include "myListCtrl.h"
#include "GetAllAttributes.h"      // 6/29/99 For Submit or Deliver partial readonly


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define  LT_GREY_BK     RGB(0xc0, 0xc0, 0xc0)


/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

IMPLEMENT_DYNCREATE(CMyListCtrl,CMyListCtrl)
            

CMyListCtrl::CMyListCtrl()
{
    m_dwAllowTextSelection  =0;
    m_bAllowMultiSelect     = TRUE;
    m_dwUseReadOnlyLook     =0; 
    m_dwInitialized         =FALSE;
    m_iLastItemInsertedPos  =0;
    m_iNextItemInsertPos    =0;
    m_dwSelectAllOnStart    =0;
	m_clrText               = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk             = ::GetSysColor(COLOR_WINDOW);
	m_clrBkgnd              = ::GetSysColor(COLOR_WINDOW);
	m_bFullRowSel           =TRUE;
	m_cxClient              =0;
    m_dwCol1WidthPcnt       =0;

}


CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, OnItemchanging)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRightClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
END_MESSAGE_MAP()




//void CMyListCtrl::Initialize2(DWORD dwFlags,FARPROC lpfnReturnResultItem)
//{
//    m_lpfnReturnResultItem=lpfnReturnResultItem;
//   Initialize(dwFlags);
//
//}

/*----------------------------------------------------------------------------
    CMyListCtrl::Initialize

    Must call before using.

    DWORD dwFlags     LC_ENABLE_TEXTSELECTION
                      LC_SELECT_ALL
                      LC_FULL_ROW_SELECTION
                      LC_ENABLE_READONLY_LOOK
                      LC_PARTIAL_READONLY_LOOK

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CMyListCtrl::Initialize( DWORD dwFlags, DWORD dwStyle )
{

    if(dwFlags & LC_SELECT_ALL)        // SELECT ALL PRESUPPOSES TEXTSELECTION
    {
        m_dwSelectAllOnStart  = TRUE;
        m_dwAllowTextSelection= TRUE;
        m_bFullRowSel         = TRUE;
        m_bAllowMultiSelect   = TRUE;
    }
    else
        m_dwAllowTextSelection   = dwFlags & LC_ENABLE_TEXTSELECTION;

    if(dwFlags & LC_FULL_ROW_SELECTION)  // DID WE SPECIFY FULL ROW?
        m_bFullRowSel=TRUE;

    if(!m_dwAllowTextSelection)          // IF SELECTION DISABLED, THIS SHOULD BE ALSO
        m_bFullRowSel=FALSE;

    m_dwUseReadOnlyLook   = dwFlags & LC_ENABLE_READONLY_LOOK;  
    m_dwInitialized       = TRUE;
  
    // IF IT'S THE READ ONLY LOOK, THEN DISABLE SELECTION
    if(m_dwUseReadOnlyLook) {
        m_dwAllowTextSelection= TRUE;    // FALSE  6/5/99
        m_bFullRowSel         = TRUE;    // FALSE  6/5/99
        m_dwSelectAllOnStart  = FALSE;
        m_bAllowMultiSelect   = FALSE;
        ModifyStyle(0L, LVS_SHOWSELALWAYS);
    }

    // IF IT'S THE READ ONLY LOOK, SET THE WIDTH OF COL1 TO 33%
    if(m_dwUseReadOnlyLook)
        m_dwCol1WidthPcnt= 45;     //33;
    else
        m_dwCol1WidthPcnt=40;    // 50


    // DO WE GREY OUT THE BACKGROUND?
    if(m_dwUseReadOnlyLook) {
        SetBkColor(LT_GREY_BK);        // RGB(0xc0, 0xc0, 0xc0)
        SetTextBkColor(LT_GREY_BK);
    }

    // ARE WE SELECTING THE ENTIRE LIST ON INITIAL SHOWING?
    if(m_dwSelectAllOnStart)
        ModifyStyle(0L, LVS_SHOWSELALWAYS);
  
    // IF FULL ROW SELECT, THEN SWITCH TO OWNER DRAW
    if(m_bFullRowSel)
	    ModifyStyle(0L, LVS_OWNERDRAWFIXED);

    // ARE WE MODIFYING THE STYLE TO LIST IN ALPHA ORDER. LVS_SORTASCENDING SORTS IN ALPHA ORDER
    if( dwStyle )
	    ModifyStyle( 0L, dwStyle );

    return;
}


/*----------------------------------------------------------------------------
    CMyListCtrl::InsertTwoColumns

    Only handles one or two columns

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/

void CMyListCtrl::InsertTwoColumns(LPCTSTR lpszColumn1Name,LPCTSTR lpszColumn2Name)
{
	// INSERT TWO COLUMNS (REPORT MODE) AND MODIFY THE NEW HEADER ITEMS
    int  rc = 0;
    CRect   rect;
	GetWindowRect(&rect);

    // VALIDATE
    if( lpszColumn1Name == NULL && lpszColumn2Name == NULL )
        return;

    // CALC THE COLUMN WIDTHS
    int iCol1WidthPcnt=0;
    int iCol1Width=0;
    int iCol2Width=0;

    if(m_dwCol1WidthPcnt<=0 || m_dwCol1WidthPcnt >=100)  // DEFAULT IS 33%
        m_dwCol1WidthPcnt=33;

    iCol1WidthPcnt=m_dwCol1WidthPcnt;
    iCol1Width=((rect.Width() * iCol1WidthPcnt) / 100 );
    iCol2Width=((rect.Width() * 1));           // or divide by (133 - m_dwCol1WidthPcnt)

    // HANDLE ONE COLUMN ONLY 12/28/99
    if( lpszColumn2Name == NULL )
    {
        iCol1Width = rect.Width();             // TAKE THE ENTIRE WIDTH
        iCol2Width = 0;
        lpszColumn2Name = _T("");
    }

    if( lpszColumn1Name != NULL )
    	rc = InsertColumn(0,lpszColumn1Name, LVCFMT_LEFT, iCol1Width, 0);

    if( lpszColumn2Name != NULL )
    	rc = InsertColumn(1,lpszColumn2Name, LVCFMT_LEFT, iCol2Width, 1);
    
    return;
}


/*----------------------------------------------------------------------------
    CMyListCtrl::InsertItemAndSubItem

    Only handles two columns
    With the Confirmation Dialog, LPARAM lParam holds a pointer to the Object.

    Modified 12/28/99 to handle a single column instead of two.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
int CMyListCtrl::InsertItemAndSubItem(LPTSTR lpszItem, LPTSTR lpszSubItem, LPARAM lParam)
{
	LVITEM ListItem;
    int    iActualItem=0;
    int    rc=-1;                               // ASSUME FAILURE
    UINT   dwSaveMask = 0;
    //CString s;

    // INITIALIZE
    memset(&ListItem,0,sizeof(ListItem));
    ListItem.mask = LVIF_TEXT ;

    // DO THE ITEM
    ListItem.iItem   = m_iNextItemInsertPos;      
    ListItem.iSubItem= 0; 
    ListItem.pszText = lpszItem; 
    if(m_dwSelectAllOnStart)  {                  // SET ALL TO SELECTED?
       ListItem.state     = LVIS_SELECTED;
       ListItem.stateMask = LVIS_SELECTED;
       ListItem.mask     |= LVIF_STATE; 
    }
    if( lParam != 0 )
    {
       dwSaveMask = ListItem.mask;
       ListItem.lParam  = lParam;
       ListItem.mask |= LVIF_PARAM;
    }


    //s = ListItem.pszText;

    iActualItem = InsertItem( &ListItem );      // ADD NEW ITEM
    // RESTORE
    if( lParam != 0 )
       ListItem.mask = dwSaveMask;


//     // IS THIS FOR A SINGLE COLUMN 12/28/99?
//     if( lpszSubItem == NULL)
//     {
//         m_iLastItemInsertedPos= iActualItem;
//         m_iNextItemInsertPos  = iActualItem+1;  // INSERT TO END OF LIST
//         ListItem.iItem   = iActualItem; 
//         ListItem.iSubItem= 0;
//    	    rc = SetItem(&ListItem);                // MODIFY NEW ITEM 
//         return(1);
//     }


    // DID WE SUCCEED?
    if(iActualItem>=0) {
        m_iLastItemInsertedPos= iActualItem;
        m_iNextItemInsertPos  = iActualItem+1;  // INSERT TO END OF LIST

        // DO THE SUBITEM
        //ListItem.mask    = LVIF_TEXT ;          // 4/3 IGNORE THE STATE AND STATEMASK FOR THE SUBITEMS
        ListItem.iItem   = iActualItem; 
        ListItem.iSubItem= 1; 
        ListItem.pszText = lpszSubItem;
        //ListItem.mask   ^= LVIF_PARAM;          // CLEAR THE BIT
   	    rc = SetItem(&ListItem);                // MODIFY NEW ITEM (THE SUB-ITEM TEXT)U
    }

    //s += _T("   ");
    //s += ListItem.pszText;
    //fWidePrintString( "%s ", (LPCTSTR) s );
    return(rc);
}






/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers


/*----------------------------------------------------------------------------
    CMyListCtrl::OnItemchanging

    Called before change applied.
    Prevent selection in read only mode

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // ADDED 6/29/99
    DWORD  dwIsDisabled = 0;
    CQServerGetAllAttributes* pGetAllAttributes = NULL;
    pGetAllAttributes = (CQServerGetAllAttributes*) pNMListView->lParam;
    if( pGetAllAttributes != NULL )
        dwIsDisabled = pGetAllAttributes->m_dwIsDisabled;
    // IS THIS A PARTIAL READONLY LIST, AND IS THIS ITEM DISABLED?
    if( dwIsDisabled )
    {
       *pResult = 1;                  // then prevent it
       return;
    }

    // ARE SELECTIONS ALLOWED?
    if( m_dwAllowTextSelection )      // && m_bAllowMultiSelect 
    {
  	  *pResult = 0;                  // allow all selections
       return;
    }
  
    // WILL THE NEW STATE SHOW SELECTED?
    if(pNMListView->uNewState & (LVIS_SELECTED | LVIS_FOCUSED))  
       *pResult = 1;                  // then prevent it
    else
  	  *pResult = 0;                  // allow the selection 
  
    return;
}


/*----------------------------------------------------------------------------
    CMyListCtrl::OnItemchanged

    Called after change applied.
    If m_bFullRowSel on, then set the subItem to selected also.

    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here LVIS_FOCUSED

    if(pNMListView->uNewState & (LVIS_SELECTED  ) && // IS IT SELECTED?
       pNMListView->iSubItem==0 &&                   // IS THIS AN ITEM, AND NOT A SUBITEM?
       m_bFullRowSel)                                // FULL ROW SELECTION
    {
	    LVITEM ListItem;
        memset(&ListItem,0,sizeof(ListItem));
    	ListItem.mask     = LVIF_STATE;
	    ListItem.iItem    = pNMListView->iItem;
	    ListItem.iSubItem = 1;                    // 2nd Column 
        ListItem.state    = LVIS_SELECTED;              
	    ListItem.stateMask= LVIS_SELECTED;    
        SetItem(&ListItem);                       // set state to selected

        //bSelected=TRUE;
    }

	*pResult = 0;
    return;
}


/*----------------------------------------------------------------------------
    CMyListCtrl::DrawItem

    This is only called if ModifyStyle has specified LVS_OWNERDRAWFIXED.
    This draws the highlight as a full row.
    m_bFullRowSel determines whether this is called or not.

    Written by: Jim Hill
----------------------------------------------------------------------------*/
void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	UINT uiFlags = ILD_TRANSPARENT;
	int nItem    = lpDrawItemStruct->itemID;
	BOOL bFocus  = (GetFocus() == this);
	COLORREF clrTextSave, clrBkSave;
    clrTextSave  = clrBkSave=0;
	COLORREF clrImage = m_clrBkgnd;
   	COLORREF dwclrTextBk = m_clrTextBk;
	static _TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText = szBuff;

    // GET ITEM DATA
	LV_ITEM lvi;
    memset( &lvi, 0, sizeof(LV_ITEM) );
	lvi.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM ;     //| LVIF_IMAGE 
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF;                 // get all state flags
	GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);


    // ADDED 6/29/99 FOR PARTIAL READONLY
    DWORD  dwIsDisabled = 0;
    CQServerGetAllAttributes* pGetAllAttributes = NULL;
    pGetAllAttributes = (CQServerGetAllAttributes*) lvi.lParam;
    if( pGetAllAttributes != NULL )
        dwIsDisabled = pGetAllAttributes->m_dwIsDisabled;
    // IS THIS A PARTIAL READONLY LIST, AND IS THIS ITEM DISABLED?
    if( dwIsDisabled )
    {
        bSelected   = FALSE;
        dwclrTextBk = LT_GREY_BK;
    }

	CRect rcAllLabels;
	GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	CRect rcLabel;
	GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;
	if (rcAllLabels.right<m_cxClient)       // m_bClientWidthSel && 
		rcAllLabels.right = m_cxClient;

	if (bSelected)
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave   = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect( &rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)) );
	}
	else
		pDC->FillRect( &rcAllLabels, &CBrush( dwclrTextBk ) );   // m_clrTextBk


    // DRAW ITEM LABEL
    GetItemRect(nItem, rcItem, LVIR_LABEL);
    // TRIM THE STRING
	pszText=MakeShortString(pDC,szBuff,rcItem.right-rcItem.left,0);  // 2*OFFSET_FIRST
				
	rcLabel = rcItem;
	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

    // DRAW LABELS FOR 2ND COLUMN
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
    GetColumn(1, &lvc);
	int nRetLen = GetItemText(nItem, 1, szBuff, sizeof(szBuff));
    if(nRetLen) {
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;
        // TRIM THE STRING
	    pszText=MakeShortString(pDC,szBuff,rcItem.right-rcItem.left,0);
	    rcLabel = rcItem;
		pDC->DrawText(pszText, -1, rcLabel,
			DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

    // SET ORIGINAL COLORS IF ITEM WAS SELECTED
	if (bSelected)
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}

}


/*----------------------------------------------------------------------------
    CMyListCtrl::MakeShortString
    Trim a copy of the strings from DrawItem to fit the currently 
        displayable column width.
 ----------------------------------------------------------------------------*/
LPCTSTR CMyListCtrl::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[] = _T("...");

	int nStringLen = lstrlen(lpszLong);

	if(nStringLen == 0 ||
		(pDC->GetTextExtent(lpszLong, nStringLen).cx + nOffset) <= nColumnLen)
	{
		return(lpszLong);
	}

	static _TCHAR szShort[300];

	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i = nStringLen-1; i > 0; i--)
	{
		szShort[i] = 0;
		if((pDC->GetTextExtent(szShort, i).cx + nOffset + nAddLen)
			<= nColumnLen)
		{
			break;
		}
	}

	lstrcat(szShort, szThreeDots);
	return(szShort);
}


/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
void CMyListCtrl::RepaintSelectedItems()
{
	//CListCtrl& ListCtrl = GetListCtrl();
	CRect rcItem, rcLabel;

// invalidate focused item so it can repaint properly

	int nItem = GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		GetItemRect(nItem, rcItem, LVIR_BOUNDS);
		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;

		InvalidateRect(rcItem, FALSE);
	}

// if selected items should not be preserved, invalidate them

	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = GetNextItem(nItem, LVNI_SELECTED))
		{
			GetItemRect(nItem, rcItem, LVIR_BOUNDS);
			GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcItem.left = rcLabel.left;

			InvalidateRect(rcItem, FALSE);
		}
	}

// update changes

	UpdateWindow();
}


/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);
	// TODO: Add your message handler code here

	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
   
}


/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
	
	// repaint items that should change appearance
	if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}


/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnPaint() 
{
	// CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
#if 0
    // DO WE WANT TO SELECT THE FULL ROW?
    if(m_bFullRowSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
    {
		CRect rcAllLabels;
		GetItemRect(0, rcAllLabels, LVIR_BOUNDS);

		if(rcAllLabels.right < m_cxClient)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left  = min(rcAllLabels.right-1, rcClip.left);
			rcClip.right = m_cxClient;

			InvalidateRect(rcClip, FALSE);
			// EndPaint will be called in CPaintDC d-tor    NULLREGION


		}

    }
#endif
	// Do not call CListCtrl::OnPaint() for painting messages
    CWnd::OnPaint();  
    return;
}


/*----------------------------------------------------------------------------
    CMyListCtrl::OnDblclk()
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    NMHDR* pNMHDRhdr = NULL;
    TCHAR  szAttributeName[259];
    HWND   hTargetWnd = NULL;
    CWnd*  pWnd = NULL;
    DWORD  dwControlID = 0;

    // ONLY DO IF READ ONLY
    if( !m_dwUseReadOnlyLook || pNMHDR == NULL )  
    {
    	*pResult = 0;
        return;
    }

    // GET THE ATTRIBUTE NAME
    memset( szAttributeName, 0, sizeof(szAttributeName) );
	int nRetLen = GetItemText(pNMListView->iItem, 0, szAttributeName, sizeof(szAttributeName));

    if( szAttributeName[0] != 0 )
    {
        // pNMHDRhdr =  &pNMListView->hdr;   //(NMHDR*)
        // if( pNMHDRhdr != NULL )
        //     hTargetWnd = pNMHDRhdr->hwndFrom;
        // if( hTargetWnd == NULL )
        // {
        //     //    hTargetWnd = GetActiveWindow();
        //     pWnd = GetActiveWindow();
        //     if( pWnd != NULL )
        //         hTargetWnd = pWnd->m_hWnd;
        // }

        dwControlID = GetDlgCtrlID();
        hTargetWnd = m_hWnd;

        if( hTargetWnd != NULL )
            DoHelpAttributeNameContextHelp( dwControlID, szAttributeName, NULL, hTargetWnd );  // IDC_LIST1
    }
    
	*pResult = 0;
}


/*----------------------------------------------------------------------------
    CMyListCtrl::OnRightClick
    Only do if Read Only
    Written by: Jim Hill
 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnRightClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    TCHAR szBuff[259];
    //DWORD dwTemp = 0;

    // ONLY DO IF READ ONLY
    if(!m_dwUseReadOnlyLook) 
    {
    	*pResult = 0;
        return;
    }

    //if(pNMListView->uNewState & (LVIS_SELECTED ) )  // IS IT SELECTED?
    //{
    //}

    // GET THE ATTRIBUTE NAME
    memset( szBuff, 0, sizeof(szBuff) );
	int nRetLen = GetItemText(pNMListView->iItem, 0, szBuff, sizeof(szBuff));

    // SAVE ATTRIBUTE NAME
    if( szBuff[0] != 0 )
    	sAttributeNameFromListbox = szBuff;

	*pResult = 0;
}


/*----------------------------------------------------------------------------
   CMyListCtrl::GetSelectedAttributeName
   Called from CPlatfromAttributes:
 ----------------------------------------------------------------------------*/
BOOL CMyListCtrl::GetSelectedAttributeName( TCHAR *lpszAttributeName )
{
    if( sAttributeNameFromListbox.IsEmpty() || lpszAttributeName == NULL )
        return(FALSE);

    // RETURN THE NAME
    STRCPY( lpszAttributeName, sAttributeNameFromListbox );

    // CLEAR IT. WE JUST RETURN IT ONCE.
    sAttributeNameFromListbox.Empty();

    return(TRUE);
}


/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
LRESULT CMyListCtrl::OnSetTextColor(WPARAM wParam, LPARAM lParam)
{
	m_clrText = (COLORREF)lParam;
	return(Default());
}

/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
LRESULT CMyListCtrl::OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrTextBk = (COLORREF)lParam;
	return(Default());
}

/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
LRESULT CMyListCtrl::OnSetBkColor(WPARAM wParam, LPARAM lParam)
{
	m_clrBkgnd = (COLORREF)lParam;
	return(Default());
}


/*----------------------------------------------------------------------------

 ----------------------------------------------------------------------------*/
void CMyListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	m_cxClient = cx;
}





// void CMyListCtrl::OnPaint() 
// {
// 	CPaintDC dc(this); // device context for painting
// 	
// 	// TODO: Add your message handler code here
// 	
// 	// Do not call CListCtrl::OnPaint() for painting messages
// }





void CMyListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListCtrl::OnLButtonDown(nFlags, point);
}
