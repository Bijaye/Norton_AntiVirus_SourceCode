#include "StdAfx.h"

#include "Utils.h"
#include "PushGDI.h"

#include "CustomCellSelectList.h"
#include "CustomCellListView.h"

CCustomCellSelectList::CCustomCellSelectList(void) :
    m_iItem(-1),
    m_iSubItem(-1),
    m_iCtrlId(-1),
    m_bESC(FALSE),
    m_bLButtonReleased(FALSE)
{
}

CCustomCellSelectList::~CCustomCellSelectList(void)
{
}


//***************************************************************************
// 
//***************************************************************************
BOOL CCustomCellSelectList::PreTranslateMessage(MSG* pMsg)
{
    if( pMsg->message == WM_KEYDOWN )
    {
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
           pMsg->wParam == VK_SPACE  )
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);
            return TRUE;                // DO NOT process further
        }
    }

    return FALSE;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnCreate( UINT nMsg, WPARAM wParam, 
                                         LPARAM lParam, BOOL & bHandled )
{
    bHandled = FALSE;

    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    
    SetCapture();

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
void CCustomCellSelectList::OnFinalMessage(HWND hWnd)
{
    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    pLoop->RemoveMessageFilter(this);

    return;
}


//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnPaint ( UINT nMsg, WPARAM wParam, 
                                         LPARAM lParam, BOOL & bHandled )
{
    CRect rectClient;
    GetClientRect( rectClient );

    // The below code fills in the header with our own custom 
    // background color - i.e. fills in the areas in which the 
    // ListControl doesn't not redraw...

    CPaintDC dc( m_hWnd );

    CDC dcMem;
    dcMem.CreateCompatibleDC( dc );

    CBitmap bmpScreen;
    bmpScreen.CreateCompatibleBitmap( dc, rectClient.Width(), 
                                          rectClient.Height() );

    CPushGDI<HBITMAP> PushBitmap( dcMem, bmpScreen );
    dcMem.FillSolidRect( rectClient, GetSysColor(COLOR_WINDOW) );

    // Feed the off-screen bitmap to the header...
    DefWindowProc( WM_PRINTCLIENT, (WPARAM)dcMem.m_hDC, 0 );

    // Blast the bitmap onto the screen...
    dc.BitBlt( rectClient.left, rectClient.top, 
               rectClient.Width(), rectClient.Height(), 
               dcMem, rectClient.left, rectClient.top, SRCCOPY );

    bHandled = FALSE;
    return 1;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnMouseMove ( UINT nMsg, WPARAM wParam, 
                                             LPARAM lParam, BOOL & bHandled )
{
    bHandled = FALSE;

    BOOL bOutside = FALSE;
    
    INT iItem = ItemFromPoint(CPoint(lParam), bOutside);
    INT iCurSel = GetCurSel();
    
    if(iItem != iCurSel && !bOutside) {
        SetCurSel(iItem);
    }

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnLButtonDown ( UINT nMsg, WPARAM wParam, 
                                               LPARAM lParam, BOOL & bHandled )
{
    bHandled = FALSE;
    
    BOOL bOutside = FALSE;
    INT iItem = ItemFromPoint(CPoint(lParam), bOutside);
    
    m_bDragging = !bOutside;

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnLButtonUp ( UINT nMsg, WPARAM wParam, 
                                             LPARAM lParam, BOOL & bHandled )
{
    bHandled = FALSE;
    
    BOOL bOutside = TRUE;
    INT iItem = ItemFromPoint(CPoint(lParam), bOutside);
    
    if (m_bDragging && bOutside) {
        return 0;
    }

    if (m_bLButtonReleased || !bOutside) {
        ReleaseCapture();
        ::SetFocus(GetParent());

        if ( !bOutside ) {
            SendComboItemChangeMsg();
        }
    }

    m_bLButtonReleased = TRUE;

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnKillFocus ( UINT nMsg, WPARAM wParam, 
                                             LPARAM lParam, BOOL & bHandled )
{
    PostMessage(WM_CLOSE);

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnSelChanged(int idCtrl, LPNMHDR pnmh, 
                                            BOOL& bHandled)
{
    return 0;               
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnChar ( UINT nMsg, WPARAM wParam, 
                                        LPARAM lParam, BOOL & bHandled )
{
    bHandled = FALSE;
    
    TCHAR nChar = (TCHAR)wParam;
    INT iRepeatCount = (UINT)lParam & 0xFFFF;
    INT iFlags = (UINT)((lParam & 0xFFFF0000) >> 16);

    if ( nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_SPACE )
    {
        if ( nChar == VK_ESCAPE )
        {
            m_bESC = TRUE;
        }
        else 
        {
            SendComboItemChangeMsg();
        }
        
        ::SetFocus(GetParent());
        return 0;
    }

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellSelectList::OnNcDestroy ( UINT nMsg, WPARAM wParam, 
                                             LPARAM lParam, BOOL & bHandled )
{
    bHandled = FALSE;

    m_iItem = -1;
    m_iSubItem = -1;
    m_iCtrlId = -1;
    m_bESC = FALSE;
    m_bLButtonReleased = FALSE;

    return 0;
}

void CCustomCellSelectList::SendComboItemChangeMsg()
{
    CString str;
    GetText(GetCurSel(), str.GetBuffer(MAX_PATH));
    str.ReleaseBuffer();

    CWindow wndParent(GetParent());

    // Send Notification to parent of ListView ctrl
    CCustomCellListView::LVCOMBOITEMCHANGE lvcmb;
    lvcmb.hdr.code = CCustomCellListView::LVN_COMBOITEMCHANGE;
    lvcmb.hdr.hwndFrom = wndParent;
    lvcmb.hdr.idFrom = m_iCtrlId;
    
    LVITEM &itemListView = lvcmb.LVItem;
    itemListView.mask = LVIF_PARAM;
    itemListView.iItem = m_iItem;
    itemListView.iSubItem = m_iSubItem;
    itemListView.lParam = m_lParam;

    LVITEM &itemListBox = lvcmb.LBItem;
    itemListBox.mask = LVIF_PARAM | LVIF_TEXT;
    itemListBox.iItem = GetCurSel();
    itemListBox.iSubItem = 0;
    itemListBox.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
    itemListBox.cchTextMax = str.GetLength();
    itemListBox.lParam = GetItemData(itemListBox.iItem);

    ::SendMessage( m_hwndGrandParent, WM_NOTIFY, 
                   (WPARAM)m_iCtrlId, (LPARAM)&lvcmb );
}
