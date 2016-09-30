/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ACStateListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "avisconsole.h"
#include "listctrlex.h"
#include "ACStateListCtrl.h"
#include "smartedit.h"
#include "vpstrutils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define SUBITEM_NAME                            0
#define SUBITEM_TIME                            1
#define COLUMN_TIME                                     1
#define COLUMN_COUNT                            COLUMN_WHEN + 1

/////////////////////////////////////////////////////////////////////////////
// CACStateListCtrl

CACStateListCtrl::CACStateListCtrl()
{
	m_pEdit = NULL;
}

CACStateListCtrl::~CACStateListCtrl()
{
	if (m_pEdit)
		delete m_pEdit;
}


BEGIN_MESSAGE_MAP(CACStateListCtrl, CListCtrlEx)
//{{AFX_MSG_MAP(CACStateListCtrl)
ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
ON_WM_CHAR()
ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
ON_MESSAGE( WM_USER_UPDATE_VALUE, OnUpdateValue )
ON_MESSAGE( WM_USER_EDIT_CLOSING, OnEditClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACStateListCtrl message handlers

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
	  void CACStateListCtrl::Initialize( void)
{
	
    CString text;
    CRect rect;
    BOOL rc = FALSE;
	
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
    if ( m_images.Create( IDB_STATE_ARRAY, 16, 10, RGB( 0x00, 0xff, 0x00 ) ) )
        SetImageList( &m_images, LVSIL_SMALL );
	
    GetClientRect( &rect );
	
    VERIFY( text.LoadString( IDS_EVENT_LIST_COL_1) );
    InsertColumn( 0, text, LVCFMT_LEFT,     ((rect.right - rect.left) * 3/4) - 10);
    VERIFY( text.LoadString( IDS_EVENT_LIST_COL2) );
    InsertColumn( 1, text, LVCFMT_LEFT,     ((rect.right - rect.left) * 4/4) - 10, 1);
    ModifyStyle(0L, LVS_SHOWSELALWAYS|LVS_REPORT);
	ModifyStyleEx(0, LVS_EX_CHECKBOXES,0);
	
    return;
}

/*----------------------------------------------------------------------------
CACStateListCtrl::InsertItemAndSubItem

  CEventObject* pEvent holds a pointer to the Object.
  
	returns the index of the item in the list box. -1 if failure.

	  Written by: TMARLES
----------------------------------------------------------------------------*/
int CACStateListCtrl::InsertEventItem(CEventObject* pEvent)
{
	LVITEM ListItem;
    int    iActualItem=0;
    UINT   dwSaveMask = 0;
	
	if (pEvent == NULL)
		return -1;

    // INITIALIZE
    memset(&ListItem,0,sizeof(ListItem));
	
    // DO THE ITEM
    ListItem.iItem   = 0;      
    ListItem.iSubItem= 0; 
    ListItem.pszText = LPSTR_TEXTCALLBACK; 
	ListItem.state     = LVIS_SELECTED;
	ListItem.stateMask = LVIS_SELECTED;
	ListItem.mask     = LVIF_STATE | LVIF_TEXT | LVIF_PARAM; 
	dwSaveMask = ListItem.mask;
	ListItem.lParam  = (LPARAM)pEvent;
	
    //s = ListItem.pszText;
	
    iActualItem = InsertItem( &ListItem );      // ADD NEW ITEM
	pEvent->m_iIndex = iActualItem;
	
	
    return(iActualItem );
}


void CACStateListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
    CPoint                  pt( GetMessagePos() );
    UINT                    flags;
    int                             item;
    CRect                   rect;
	//    BOOL                    bEnabled;
	CEventObject			*pEvent;
	CWnd					*pWndParent = NULL;
    ScreenToClient( &pt );
    item = HitTest( pt, &flags );
    if ( ( item != -1 ) && ( flags & LVHT_ONITEMICON ) )
	{
        GetItemRect( item, &rect, LVIR_BOUNDS );
        rect.right = rect.left + 16;
        if ( rect.PtInRect( pt ) )
		{
            VERIFY(pEvent = (CEventObject *)GetItemData(item));
			GetOwner()->PostMessage(WM_USER_INFO_DIRTY, 0,0);
			
            if ( pEvent->m_bEnabled )
                pEvent->m_bEnabled = FALSE;
            else
                pEvent->m_bEnabled = TRUE;
            InvalidateRect( &rect, TRUE );
			//            pGenConfig->SetSubOption("");
		}
		else
			OnLButtonDblClk(flags, pt);

	}
	*pResult = 0;
}

void CACStateListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
    LV_DISPINFO             *pDispInfo = (LV_DISPINFO*)pNMHDR;
    LV_ITEM                 *pItem = &(pDispInfo->item);
    CEventObject			*pEvent;
    CString                 sText;
	
    pItem->mask |= LVIF_TEXT | LVIF_IMAGE;
	
	VERIFY(pEvent = (CEventObject *)GetItemData(pItem->iItem));
	
    if ( pItem->iSubItem == SUBITEM_NAME )
	{
        sText = pEvent->m_sEventName;
        vpstrncpy( pItem->pszText, sText, pItem->cchTextMax );
        if ( pEvent->m_bEnabled )
            pItem->iImage = 2;
        else
            pItem->iImage = 1;
	}
    else if ( pItem->iSubItem == SUBITEM_TIME )
	{
        sText=pEvent->m_sTime;
       vpstrncpy( pItem->pszText, sText, pItem->cchTextMax );
 	}
	
    if ( pResult )
        *pResult = 0;
	
	*pResult = 0;
}

void CACStateListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CACStateListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    int                             item;
    CRect                   rect;
	CEventObject			*pEvent;
	CWnd					*pWndParent = NULL;
	item = GetNextItem(-1, LVNI_SELECTED);
	if (nChar == VK_SPACE)
	{
        GetItemRect( item, &rect, LVIR_BOUNDS );
        VERIFY(pEvent = (CEventObject *)GetItemData(item));
		GetOwner()->PostMessage(WM_USER_INFO_DIRTY, 0,0);
		
        if ( pEvent->m_bEnabled )
            pEvent->m_bEnabled = FALSE;
        else
            pEvent->m_bEnabled = TRUE;
        InvalidateRect( &rect, TRUE );
	}
	CListCtrlEx::OnChar(nChar, nRepCnt, nFlags);
}

void CACStateListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    CPoint          pt( GetMessagePos() );
	CString			string;
	CEventObject	*pEvent;
    int             item;
    UINT            flags;
    CRect           ItemRect;
	CRect			LabelRect;
	
    ScreenToClient( &pt );
    item = HitTest( pt, &flags );
	if (m_pEdit)
		return;
    if ( ( item != -1 ) && ( flags & LVHT_ONITEM) )
	{
        GetItemRect( item, &ItemRect, LVIR_BOUNDS );
        GetItemRect( item, &LabelRect, LVIR_LABEL );
		//        rect.right = rect.left + 16;
        if ( ItemRect.PtInRect( pt ) )
		{
            VERIFY(pEvent = (CEventObject *)GetItemData(item));
			if (!(pEvent->m_sTime.IsEmpty()))
			{
				CSmartEdit* pEdit = new CSmartEdit;
				pEdit->m_pEventObject = pEvent;
				ItemRect.bottom ;
				ItemRect.left = LabelRect.right;
				pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN ,
					ItemRect, this, (UINT)-1);
				m_pEdit = pEdit;
				pEdit->SetFont( CFont::FromHandle( (HFONT) GetStockObject( DEFAULT_GUI_FONT ) ) );
			}
		}
	}
	
	
	
	
	CListCtrlEx::OnLButtonDblClk(nFlags, point);
}



afx_msg LRESULT CACStateListCtrl::OnUpdateValue(WPARAM wParam, LPARAM lParam)
{
	CEventObject *pEvent = NULL;
	CRect Rect;
	pEvent = (CEventObject *) lParam;
	GetItemRect( pEvent->m_iIndex, &Rect, LVIR_SELECTBOUNDS) ;
	Rect.bottom+=3;
    InvalidateRect( &Rect, TRUE );
	GetOwner()->PostMessage(WM_USER_INFO_DIRTY, 0,0);

	return S_OK;
	
}


afx_msg LRESULT CACStateListCtrl::OnEditClose(WPARAM wParam, LPARAM lParam)
{
	m_pEdit = NULL;
	return S_OK;
}


LRESULT CACStateListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
	case WM_CLOSE:
		ATLTRACE("received wm_close on CSmartEdit");
		return TRUE;
	default:
		break;
	}
	
	return CListCtrlEx::DefWindowProc(message, wParam, lParam);
}



void CACStateListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrlEx::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	
}


void CACStateListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(m_pEdit)
	{
		m_pEdit->PostMessage(WM_CLOSE,0,0);
	}		
	CListCtrlEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CACStateListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(m_pEdit)
	{
		m_pEdit->PostMessage(WM_CLOSE,0,0);
	}		
	
	CListCtrlEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
