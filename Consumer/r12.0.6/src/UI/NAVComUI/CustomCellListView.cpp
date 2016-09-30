//***************************************************************************
// CustomCellListView.cpp: implementation of the CCustomCellListView class.
//***************************************************************************
#include "stdafx.h"
#include <commctrl.h>
#include "resource.h"

#include "Utils.h"
#include "PushGDI.h"

#include "CustomCellListView.h"
#include "CustomCellSelectList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#define  CC_COLOR_LTGRAY         RGB(220,220,220)
#define  CC_COLOR_MDGRAY         RGB(128,128,128)
#define  CC_COLOR_CTLBORDER      RGB(100,100,100)
#define  CC_COLOR_DARKBG         RGB(231,231,239)
#define  CC_COLOR_LIGHTBG        RGB(255,255,255)
#define  CC_COLOR_LISTDOTS       RGB(150,150,150)
#define  CC_COLOR_PROGRESS       CC_COLOR_DARKBG
#define  CC_COLOR_GRADIENT_HI    RGB(181,194,242)
#define  CC_COLOR_GRADIENT_LO    RGB(239,247,255)
#define  CC_COLOR_SUMMARYBOX_HI  RGB(181,194,242)
#define  CC_COLOR_SUMMARYBOX_LO  RGB(239,247,255)
#define  CC_COLOR_HYPERLINK      RGB(0, 0, 255)


#define CC_WIDTH_LISTDOTS        2
#define CC_WIDTH_CTLBORDER       1
#define CC_WIDTH_SECTIONBORDER   1
#define CC_WIDTH_TITLE_UNDERLINE 1

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//***************************************************************************
// 
//***************************************************************************
CCustomCellListView::CCustomCellListView() : 
    m_iPaintStyle(CUSTOM_CELL_LISTVIEW_STYLE_DOTTED), 
    m_iPaintExStyle(CUSTOM_CELL_LISTVIEW_EXSTYLE_NOBORDER),
    m_bHandcursor(FALSE),
    m_bAlternate(TRUE),
    m_hHyperlinkCursor(NULL),
    //m_iNavColumn(0)
    m_iNavColumn(1) 
            // Not 0 because of kludge fix in AxSysListView32::InitColumns()
{
}

//***************************************************************************
// 
//***************************************************************************
CCustomCellListView::~CCustomCellListView()
{
    if(m_hHyperlinkCursor != NULL) {
        ::DestroyCursor(m_hHyperlinkCursor);
    }
}

//***************************************************************************
// 
//***************************************************************************
BOOL CCustomCellListView::PreTranslateMessage(MSG* pMsg)
{
    if ( pMsg->message == WM_KEYDOWN &&  GetFocus() == m_hWnd && 
          pMsg->wParam == VK_RETURN  )
    {
        ::TranslateMessage(pMsg);
        ::DispatchMessage(pMsg);
        return TRUE;                // DO NOT process further
    }

    return FALSE;
}


//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnCreate( UINT nMsg, WPARAM wParam, 
                                       LPARAM lParam, BOOL & bHandled )
{
    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);

    SetWindowLong(GWL_STYLE, 
                  GetWindowLong(GWL_STYLE) | LVS_REPORT | LVS_EDITLABELS );
    ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_FULLROWSELECT  );

    m_wndHeader.SubclassWindow(GetHeader());
    m_wndHeader.Init();
    
    CClientDC dc( m_hWnd );

    //
    // Default fonts
    //
    m_fontComboBox = CUtils::CreateFontEz(dc, IDS_FONTEZ_LISTBOXDETAIL, NULL, 
                                          _Module.GetResourceInstance());
    m_fontHyperLink = CUtils::CreateFontEz(dc, IDS_FONTEZ_HYPERLINK, NULL, 
                                           _Module.GetResourceInstance());
    m_fontHeader = CUtils::CreateFontEz(dc, IDS_FONTEZ_LISTBOXHEADER, NULL, 
                                        _Module.GetResourceInstance());
    m_fontNormal = CUtils::CreateFontEz(dc, IDS_FONTEZ_LISTBOXDETAIL, NULL, 
                                        _Module.GetResourceInstance());
    
    m_hHyperlinkCursor = ::LoadCursor(NULL, IDC_HAND);
    
    CHeaderCtrl wndHeader(GetHeader());
    wndHeader.SetFont(m_fontHeader);

    InitControlColor();

    bHandled = FALSE;
    return TRUE;
}

//***************************************************************************
// 
//***************************************************************************
void CCustomCellListView::OnFinalMessage(HWND hWnd)
{
    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);

    pLoop->RemoveMessageFilter(this);

    return;
}


void CCustomCellListView::SetHeaderFont() 
{
    CHeaderCtrl wndHeader(GetHeader());
    wndHeader.SetFont(m_fontHeader);
}
//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnPaint ( UINT nMsg, WPARAM wParam, LPARAM lParam, 
                                       BOOL & bHandled )
{
    CRect rectClient;
    GetClientRect( rectClient );
    
    HWND hWndHeader = (HWND) ::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0);

    if( hWndHeader )
    {
        CRect rectHeader;
        ::GetClientRect( hWndHeader, &rectHeader );

        rectClient.top += rectHeader.Height();
        rectClient.bottom += rectHeader.Height();
    }

    // The below code fills in the ListCtrl with our own custom 
    // background color - i.e. fills in the areas in which the 
    // ListControl doesn't not redraw...

    CPaintDC dc( m_hWnd );

    CDC dcMem;
    dcMem.CreateCompatibleDC( dc );

    CBitmap bmpScreen;
    bmpScreen.CreateCompatibleBitmap( dc, rectClient.Width(), 
                                          rectClient.Height() );

    CPushGDI<HBITMAP> PushBitmap( dcMem, bmpScreen );
    dcMem.FillSolidRect( rectClient, m_crBackground );
    
    CDPaintRowColors(dcMem);

    // Feed the off-screen bitmap to the ListControl...
    DefWindowProc( WM_PRINTCLIENT, (WPARAM)dcMem.m_hDC, 0 );

    // Blast the bitmap onto the screen...
    dc.BitBlt( rectClient.left, rectClient.top, 
               rectClient.Width(), rectClient.Height(), dcMem, 
               rectClient.left, rectClient.top, SRCCOPY );

    bHandled = TRUE;
    return 1;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnLButtonDown ( UINT nMsg, WPARAM wParam, 
                                             LPARAM lParam, BOOL & bHandled )
{
    bHandled = TRUE;
    
    SetFocus();

    LVHITTESTINFO lvh = {0};
    CPoint pt(lParam);
    int iItem = -1, iSubItem = -1;
    
    iItem = HitTestEx(pt, iSubItem);

    if (iItem == -1) 
    {
        //
        // If there's only one item in the list, and that item is
        // currently selected, then allow the outside click to cause 
        // the item to be unselected.
        //
        if (1 == GetItemCount() && GetItemState(0, 
                                                LVIS_SELECTED | LVIS_FOCUSED))
        {
            SetItemState(0, 0, LVIS_SELECTED | LVIS_FOCUSED);
            SetSelectionMark( -1 );
        }

        return 0;
    }

    DWORD dwCellType = CELLTYPE_NORMAL;
    GetCellType(iItem, iSubItem, &dwCellType);

    if (m_wndSelectList.IsWindow())
    {
        m_wndSelectList.SendMessage(WM_CLOSE);
    }
    else 
    {
        if (m_iNavColumn != iSubItem)
        {
            m_iNavColumn = iSubItem;
            CRect rcItem;
            GetItemRect(iItem, &rcItem, LVIR_BOUNDS);
            Invalidate();
        }

        switch(dwCellType)
        {
            case CELLTYPE_HYPERLINK:
                OnLButtonDown_Hyperlink(iItem, iSubItem, pt);

                break;

            case CELLTYPE_COMBOBOX:
                OnLButtonDown_ComboBox(iItem, iSubItem, pt);

                break;

            default:
                // CELLTYPE_NORMAL
                break;
        }
    }

    SetItemState( iItem, LVIS_SELECTED | LVIS_FOCUSED,
                         LVIS_SELECTED | LVIS_FOCUSED );
    SetSelectionMark( iItem );

    return 0;
}

LRESULT CCustomCellListView::OnLButtonDown_Hyperlink(INT iItem, INT iSubItem, 
                                                     POINT &pt)
{
    RECT rectText;
    CalcIdealTextRect(iItem, iSubItem, rectText, m_fontHyperLink);

    if(::PtInRect(&rectText, pt))
    {
        NMITEMACTIVATE nma;
        ZeroMemory(&nma, sizeof(NMITEMACTIVATE));

        nma.hdr.code = LVN_HYPERLINKACTIVATE;
        nma.hdr.hwndFrom = m_hWnd;
        nma.hdr.idFrom = GetDlgCtrlID();
        nma.iItem = iItem;
        nma.iSubItem = iSubItem;
        nma.ptAction = pt;
        nma.lParam = (LPARAM)GetItemData(iItem);

        ::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nma.hdr.code, 
                                              (LPARAM)&nma);

    }

    return 0;
}

LRESULT CCustomCellListView::OnLButtonDown_ComboBox(INT iItem, INT iSubItem, 
                                                    POINT &pt)
{
    // Add check for LVS_EDITLABELS
    if( GetWindowLong(GWL_STYLE) & LVS_EDITLABELS )
    {
        if(m_wndSelectList.IsWindow())
        {
            m_wndSelectList.SendMessage(WM_CLOSE);
            return FALSE;
        }

        CRect rcItem;
        GetSubItemRect(iItem, iSubItem, LVIR_LABEL, &rcItem);

        // Reduce click area to the drop button (undesirable
        // currently since the item is not editable; possible
        // future enhancement)
        //
        //CRect rcImage = rcItem;
        //int iImageWidth = GetSystemMetrics(SM_CXVSCROLL);
        //rcImage.left = rcItem.right - iImageWidth;
        // 
        //if (!::PtInRect(rcImage, pt)) {
        //    return FALSE;
        //}
            
        if (!::PtInRect(rcItem, pt)) {
            return FALSE;
        }

        //
        // First must find out how many items will be in the
        // list, so that we'll know how large to make the 
        // window.  So send the COMBOITEMACTIVATE message to
        // the parrent control to retrieve all of the list
        // items from it.
        //
        LVCOMBOITEMACTIVATE lvcmb;
        NMITEMACTIVATE &nma = lvcmb.itemActivate;
        ZeroMemory(&lvcmb, sizeof(LVCOMBOITEMACTIVATE));

        lvcmb.hdr.code = LVN_COMBOITEMACTIVATE;
        lvcmb.hdr.hwndFrom = m_hWnd;
        lvcmb.hdr.idFrom = GetDlgCtrlID();
        nma.iItem = iItem;
        nma.iSubItem = iSubItem;
        nma.ptAction = pt;
        nma.lParam = (LPARAM)GetItemData(iItem);

        ::SendMessage(GetParent(), WM_NOTIFY, 
                      (WPARAM)lvcmb.hdr.code, (LPARAM)&lvcmb);
            
        // 
        // Defafult behavior is to draw our window directly beneath
        // the subitem, so use bottom of the sub-item rectangle as the 
        // starting pont for our draw
        // 
        CRect rcList = rcItem;
        rcList.MoveToY(rcItem.bottom);

        //
        // Figure out how tall the window needs to be
        //
        LOGFONT logFontComboBox;
        m_fontComboBox.GetLogFont(&logFontComboBox);

        // Minus because lfHeight comes back as a negative value
        // A little more than the log font size to account for 
        // whitespace pixels at above and below item text.
        rcList.bottom = rcList.top - (lvcmb.listItems.size() * 
                                      (long)(logFontComboBox.lfHeight * 1.3));

        //
        // If our calculatred window size would cause our drawing
        // to drop below the bottom of the parent control, then 
        // move our window location to be *above* the sub-item 
        // rather than below it.
        //
        CRect rcView;
        GetClientRect(&rcView);

        if (rcList.bottom > rcView.bottom) {
            rcList.MoveToY(rcItem.top - rcList.Height());

            if (rcList.top < rcView.top) {
                // 
                // Just not enough room, no matter where we put it.  
                // Cutting of bottom choices is better than cutting 
                // off top choices, so move it back below the subitem
                //
                CCTRACEE(_T("CCustomCellListView::OnLButtonDown_ComboBox() - Parent control's client rectange is not large enough to accomdate the select list for the element at %d, %d"), iItem, iSubItem);
                rcList.MoveToY(rcItem.bottom);
            }
        }

        //
        // Now we can go ahead and create the window.
        //
        DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL |
                        WS_HSCROLL | LBS_NOTIFY | LBS_HASSTRINGS | 
                        LBS_NOINTEGRALHEIGHT;
            
        m_wndSelectList.Create( m_hWnd, rcList, NULL, dwStyle );
            
        m_wndSelectList.m_iItem = iItem;
        m_wndSelectList.m_iSubItem = iSubItem;
        m_wndSelectList.m_hwndGrandParent = GetParent();
        m_wndSelectList.m_iCtrlId = GetDlgCtrlID();
        m_wndSelectList.m_lParam = GetItemData(iItem);

        m_wndSelectList.SetCurSel(0);
        m_wndSelectList.SetHorizontalExtent(0);
        m_wndSelectList.SetFont(m_fontComboBox);

        for (unsigned int counter = 0; counter < lvcmb.listItems.size(); 
             counter++)
        {
            CCustomCellSelectListItem * currentItem = 
                                                   lvcmb.listItems.at(counter);

            m_wndSelectList.SetItemData(
                           m_wndSelectList.AddString(currentItem->GetString()), 
                           currentItem->GetData());

            if (lvcmb.nCurrentSelection == currentItem->GetData())
            {
                m_wndSelectList.SetCurSel(counter);
            }

            delete currentItem;
        }

        m_wndSelectList.ShowWindow(TRUE);
        m_wndSelectList.SetFocus();
    }

    return FALSE;
}

//***************************************************************************
// 
//***************************************************************************
int CCustomCellListView::HitTestEx(const POINT &pt, int &iCol)
{
    int iItem = -1;
    iCol = -1;

    iItem = HitTest(pt, NULL);

    if (iItem == -1) {
        return -1;
    }

    for(int i=0; i < Header_GetItemCount(GetHeader()) ; i++)
    {
        CRect rcSubItem;
        GetSubItemRect(iItem, i, LVIR_LABEL, &rcSubItem);

        if(rcSubItem.PtInRect(pt))
        {
            iCol = i;
        }
    }

    return iItem;
}

void CCustomCellListView::CDPaintRowColors(HDC hdc)
{
    if (!GetItemCount()) {
        return;
    }

    CRect rcView, rcHeader, rcItem; 
    GetClientRect(&rcView);
    GetItemRect(0, &rcItem, LVIR_BOUNDS);
    CHeaderCtrl(GetHeader()).GetClientRect(&rcHeader);

    CRect rcRow;
    rcRow = rcView;
    rcRow.top = rcRow.top + rcHeader.Height();  
    rcRow.bottom = rcRow.top + rcItem.Height();

    CDCHandle dc;
    dc.Attach( hdc );
    
    CBrushHandle chBrush;
    CBrush brushLightRows; brushLightRows.CreateSolidBrush(m_crLightRows);
    CBrush brushDarkRows; brushDarkRows.CreateSolidBrush(m_crDarkRows);

    int iFirst = GetTopIndex();
    int iLast = iFirst + GetCountPerPage() + 1;  // want to include partial
                                                 // row at bottom of visible
                                                 // area

    for (int i = iFirst; i < iLast; i++) 
    {
        BOOL bEvenRow = 0 == i % 2;
        chBrush = (bEvenRow || !m_bAlternate) ? brushLightRows : brushDarkRows;

        dc.FillRect(rcRow, chBrush);

        rcRow.MoveToY(rcRow.bottom);
    }
    
    
    return;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnSetCursor( UINT nMsg, WPARAM wParam, 
                                          LPARAM lParam, BOOL & bHandled )
{
    POINT pt = { 0, 0 };
    GetCursorPos(&pt);
    ScreenToClient(&pt);

    INT iItem, iSubItem;
    iItem = HitTestEx(pt, iSubItem);

    DWORD dwCellType;

    if ( GetCellType(iItem, iSubItem, &dwCellType) &&
         CELLTYPE_HYPERLINK == dwCellType )
    {
        RECT rectText;
        CalcIdealTextRect(iItem, iSubItem, rectText, m_fontHyperLink);

        if (::PtInRect(&rectText, pt))
        {
            return TRUE;
        }
    }

    bHandled = FALSE;
    return FALSE;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnMouseMove ( UINT nMsg, WPARAM wParam, 
                                           LPARAM lParam, BOOL & bHandled )
{
    
    CPoint pt(lParam);

    INT iItem, iSubItem;
    iItem = HitTestEx(pt, iSubItem);
    
    DWORD dwCellType;

    if ( GetCellType(iItem, iSubItem, &dwCellType) &&
         CELLTYPE_HYPERLINK == dwCellType )
    {
        RECT rectText;
        CalcIdealTextRect(iItem, iSubItem, rectText, m_fontHyperLink);

        if (::PtInRect(&rectText, pt))
        {
            SetCursor(m_hHyperlinkCursor);
        }
    }

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnCtlColor ( UINT nMsg, WPARAM wParam, 
                                          LPARAM lParam, BOOL & bHandled )
{
    return (LRESULT)m_brushWindow.m_hBrush;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnSettingChange ( UINT nMsg, WPARAM wParam, 
                                               LPARAM lParam, BOOL & bHandled )
{
    InitControlColor();
    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListView::OnSysColorChange ( UINT nMsg, WPARAM wParam, 
                                                LPARAM lParam, BOOL & bHandled )
{
    InitControlColor();
    return 0;
}


//***************************************************************************
// 
//***************************************************************************
void CCustomCellListView::SetAlternating(BOOL bAlternate) 
{
   m_bAlternate = bAlternate;
}


//***************************************************************************
// 
//***************************************************************************
void CCustomCellListView::SetNormalFont(UINT fontName) 
{
   CClientDC dc(m_hWnd);

   m_fontNormal = CUtils::CreateFontEz(dc, fontName, NULL, 
                                       _Module.GetResourceInstance());
}


//***************************************************************************
// 
//***************************************************************************
CCustomCellListViewHeader& CCustomCellListView::GetCustomHeader() 
{
    return m_wndHeader;
}


//***************************************************************************
// 
//***************************************************************************
void CCustomCellListView::InitControlColor()
{
    CClientDC dc( m_hWnd );

    const BOOL bHighContrastMode = CUtils::IsSystemHCMode();
    const BOOL bLowColorMode = CUtils::IsLowColorMode(dc);

    if( bHighContrastMode )
    {
        m_crText = GetSysColor(COLOR_WINDOWTEXT);
        m_crDisableText = GetSysColor(COLOR_GRAYTEXT);
        //m_crBackground = GetSysColor(COLOR_WINDOW);
        m_crBackground = CC_COLOR_LIGHTBG;
        m_clrLink = CC_COLOR_HYPERLINK;    

        m_crLightRows = m_crBackground;
        m_crDarkRows = CC_COLOR_DARKBG;
    }
    else if( bLowColorMode )
    {
        m_crText = RGB(0,0,0);
        m_crDisableText = RGB(128,128,128);
        m_crBackground = RGB(255,255,255);
        m_clrLink = CC_COLOR_HYPERLINK;    
    
        m_crLightRows = m_crBackground;
        m_crDarkRows = CC_COLOR_DARKBG;
    }
    else
    {
        m_crText = RGB(0,0,0);
        m_crDisableText = CC_COLOR_MDGRAY;
        m_crBackground = CC_COLOR_LIGHTBG;
        m_clrLink = CC_COLOR_HYPERLINK;    

        m_crLightRows = m_crBackground;
        m_crDarkRows = CC_COLOR_DARKBG;
    }

    if( m_brushWindow.m_hBrush )
    {
        m_brushWindow.DeleteObject();
    }

    m_brushWindow.CreateSolidBrush( m_crBackground );
}


//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnPrePaint(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    ATLTRACE(_T("CCustomCellListView::OnPrePaint - ctlid:%d\n"), lpNMCustomDraw->hdr.idFrom);

    return CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnPostPaint(int idCtrl, 
                                       LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    ATLTRACE(_T("CCustomCellListView::OnPostPaint - ctlid:%d\n"), lpNMCustomDraw->hdr.idFrom);

    DWORD lrReturn = CDRF_DODEFAULT;

    return lrReturn;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnPreErase(int idCtrl, 
                                      LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    ATLTRACE(_T("CCustomCellListView::OnPreErase - ctlid:%d\n"), lpNMCustomDraw->hdr.idFrom);

    return CDRF_NOTIFYPOSTERASE;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnPostErase(int idCtrl, 
                                       LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    ATLTRACE(_T("CCustomCellListView::OnPostErase - ctlid:%d\n"), lpNMCustomDraw->hdr.idFrom);

    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnItemPrePaint(int idCtrl, 
                                          LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    ATLTRACE(_T("CCustomCellListView::OnItemPrePaint - ctlid:%d\n"), lpNMCustomDraw->hdr.idFrom);

    return CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnItemPostPaint(int idCtrl, 
                                           LPNMCUSTOMDRAW lpNMCustomDraw)
{
    LPNMLVCUSTOMDRAW lpLVCustomDraw = (LPNMLVCUSTOMDRAW)lpNMCustomDraw;

//    ATLTRACE(_T("CCustomCellListView::OnItemPostPaint - ctlid:%d, itemid:%d, subitem:%d\n"), lpNMCustomDraw->hdr.idFrom, lpNMCustomDraw->dwItemSpec, lpLVCustomDraw->iSubItem);
    
    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnItemPreErase(int idCtrl, 
                                          LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    ATLTRACE(_T("CCustomCellListView::OnItemPreErase - ctlid:%d\n"), lpNMCustomDraw->hdr.idFrom);

    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnItemPostErase(int idCtrl, LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    ATLTRACE(_T("CCustomCellListView::OnItemPostErase - ctlid:%d\n"), lpNMCustomDraw->hdr.idFrom);

    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListView::OnSubItemPrePaint(int idCtrl, 
                                             LPNMCUSTOMDRAW lpNMCustomDraw)
{
//    LPNMLVCUSTOMDRAW lpLVCustomDraw = (LPNMLVCUSTOMDRAW)lpNMCustomDraw;
//    ATLTRACE(_T("CCustomCellListView::OnSubItemPrePaint - ctlid:%d, itemid:%d, subitem:%d\n"), lpNMCustomDraw->hdr.idFrom, lpNMCustomDraw->dwItemSpec, lpLVCustomDraw->iSubItem);

    NMLVCUSTOMDRAW * pLvDraw = (NMLVCUSTOMDRAW*)lpNMCustomDraw;
    NMCUSTOMDRAW * pDraw = (NMCUSTOMDRAW*)&(pLvDraw->nmcd);

    DWORD dwCellType = CELLTYPE_NORMAL;
    GetCellType(pLvDraw->nmcd.dwItemSpec, pLvDraw->iSubItem, &dwCellType);

    COLORREF crText;
    GetCellTextColor(pLvDraw->nmcd.dwItemSpec, pLvDraw->iSubItem, &crText);

    BOOL bEvenColumn = 0 == lpNMCustomDraw->dwItemSpec % 2;
    
    pLvDraw->clrTextBk = (bEvenColumn || !m_bAlternate) ? m_crLightRows : 
                                                          m_crDarkRows;
    pLvDraw->clrText = crText;

    switch(dwCellType)
    {
        case CELLTYPE_NORMAL:
            CDPaintNormal(lpNMCustomDraw);
            break;

        case CELLTYPE_HYPERLINK:
            CDPaintHyperlink(lpNMCustomDraw);
            break;

        case CELLTYPE_COMBOBOX:
            CDPaintComboBoxImage(lpNMCustomDraw);
            break;
    }

    return CDRF_SKIPDEFAULT;
}

LRESULT CCustomCellListView::OnBeginLabelEdit(int idCtrl, LPNMHDR pnmh, 
                                              BOOL& bHandled)
{
    NMLVDISPINFO *pLvDispInfo = (NMLVDISPINFO*)pnmh;

    return 1;
}

LRESULT CCustomCellListView::OnHeaderBeginTrack(int idCtrl, LPNMHDR pnmh, 
                                                BOOL& bHandled)
{
    
    return FALSE;
}

LRESULT CCustomCellListView::OnLvnKeyDown(int idCtrl, LPNMHDR pnmh, 
                                          BOOL& bHandled)
{
    LPNMLVKEYDOWN pKeyDown = (LPNMLVKEYDOWN)pnmh;

    if (VK_RIGHT == pKeyDown->wVKey)
    {
        m_iNavColumn++;

        if (m_iNavColumn >= m_wndHeader.GetItemCount())
        {
            //m_iNavColumn = 0;
            m_iNavColumn = 1;    // Not 0 because of kludge fix in
                                 // AxSysListView32::InitColumns()
        } 
            

        CRect rcItem;
        GetItemRect(GetSelectionMark(), &rcItem, LVIR_BOUNDS);
        InvalidateRect(rcItem);

        return 1;
    }
    else if (VK_LEFT == pKeyDown->wVKey)
    {
        //if (m_iNavColumn == 0)
        if (m_iNavColumn == 1)      // Not 0 because of kludge fix in
        {                           // AxSysListView32::InitiColumns()
            m_iNavColumn = m_wndHeader.GetItemCount() - 1;
        }
        else
        {
            m_iNavColumn--;
        }
        
        CRect rcItem;
        GetItemRect(GetSelectionMark(), &rcItem, LVIR_BOUNDS);
        InvalidateRect(rcItem);

        return 1;
    }
    else if(VK_SPACE == pKeyDown->wVKey)
    {
        DWORD dwCellType = CELLTYPE_NORMAL;
        GetCellType(GetSelectionMark(), m_iNavColumn, &dwCellType);

        if(CELLTYPE_HYPERLINK == dwCellType)
        {
            CRect rcItem;
            INT iItem = GetSelectionMark(), iSubItem = m_iNavColumn;
            CalcIdealTextRect(iItem, iSubItem, rcItem, m_fontHyperLink);
            
            OnLButtonDown_Hyperlink(iItem, iSubItem, rcItem.CenterPoint());
        }
        else if(CELLTYPE_COMBOBOX == dwCellType)
        {
            CRect rcItem;
            INT iItem = GetSelectionMark(), iSubItem = m_iNavColumn;
            CalcIdealTextRect(iItem, iSubItem, rcItem, m_fontHyperLink);

            OnLButtonDown_ComboBox(iItem, iSubItem, rcItem.CenterPoint());
        }

        return 1;
    }

    return FALSE;
}

void CCustomCellListView::CDPaintNormal(LPNMCUSTOMDRAW lpNMCustomDraw)
{
    NMLVCUSTOMDRAW * pLvDraw = (NMLVCUSTOMDRAW*)lpNMCustomDraw;
    NMCUSTOMDRAW * pDraw = (NMCUSTOMDRAW*)&(pLvDraw->nmcd);

    int iItem = pDraw->dwItemSpec;
    int iCol  = pLvDraw->iSubItem;

    CRect rcItem;
    GetSubItemRect(iItem, iCol, LVIR_LABEL, &rcItem);

    CString cszItemText;
    GetItemText(iItem, iCol, cszItemText.GetBuffer(MAX_PATH), MAX_PATH);
    cszItemText.ReleaseBuffer();

    BOOL bHasFocus = FALSE;

    UINT flag = LVIS_FOCUSED;
    bHasFocus = (GetItemState( iItem, flag ) & flag) == flag;

    CDCHandle dc;
    dc.Attach( pDraw->hdc );
    
    dc.FillSolidRect( &rcItem, pLvDraw->clrTextBk );

    ::SetTextColor(dc, pLvDraw->clrText);
    ::SetBkColor(dc, pLvDraw->clrTextBk);

    // if this item has focus
    if (bHasFocus) {
        CBrush brushSelected;
        brushSelected.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)); 
        FillRect(dc, &rcItem, brushSelected);


        ::SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
        ::SetBkColor(dc, GetSysColor(COLOR_HIGHLIGHT));
    }

    CPushGDI<HFONT> PushFont(dc, m_fontNormal);

    rcItem.DeflateRect(1,1);
    DrawText( dc, cszItemText, cszItemText.GetLength(), &rcItem, 
              DT_END_ELLIPSIS | DT_VCENTER | DT_SINGLELINE );
    
    if (bHasFocus && m_iNavColumn == iCol) {
        rcItem.InflateRect(1,1);
        dc.DrawFocusRect(rcItem);
    }

    return;
}

void CCustomCellListView::CDPaintComboBoxImage(LPNMCUSTOMDRAW lpNMCustomDraw)
{
    NMLVCUSTOMDRAW * pLvDraw = (NMLVCUSTOMDRAW*)lpNMCustomDraw;
    NMCUSTOMDRAW * pDraw = (NMCUSTOMDRAW*)&(pLvDraw->nmcd);

    int iItem = pDraw->dwItemSpec;
    int iCol  = pLvDraw->iSubItem;

    //
    // Get the rectangle of the subitem to be drawn as a combo box
    //
    CRect rcItem;
    GetSubItemRect(iItem, iCol, LVIR_LABEL, &rcItem);

    //
    // Get the text that goes in the combo box window
    // 
    CString cszItemText;
    GetItemText(iItem, iCol, cszItemText.GetBuffer(MAX_PATH), MAX_PATH);
    cszItemText.ReleaseBuffer();

    BOOL bHasFocus = FALSE;

    UINT flag = LVIS_FOCUSED;
    bHasFocus = (GetItemState( iItem, flag ) & flag) == flag;

    CDCHandle dc;
    dc.Attach( pDraw->hdc );

    ::SetTextColor(dc, pLvDraw->clrText);
    ::SetBkColor(dc, pLvDraw->clrTextBk);

    // if this item has focus
    if (bHasFocus)
    {
        dc.FillSolidRect(&rcItem, GetSysColor(COLOR_HIGHLIGHT));

        ::SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
        ::SetBkColor(dc, GetSysColor(COLOR_HIGHLIGHT));
    }
    else
    {
        dc.FillSolidRect( &rcItem, pLvDraw->clrTextBk );
    }
    
    // 
    // First draw image of a scroll bar's down arrow.  Place image
    // to the far right of the sub-item's drawing rectangle
    //
    int iImageWidth = GetSystemMetrics(SM_CXVSCROLL);
    int iImageLength = GetSystemMetrics(SM_CYVSCROLL);

    //
    // Don't bother trying to draw anything at all if the width of
    // the client rectangle is less than one quarter the width of a
    // scrollbar.  There's nothing worth seeing, and doing the draw
    // anyway just leades to drawing leaking problems along the 
    // border with the cell to the left.
    //
    if (rcItem.right - rcItem.left < iImageWidth / 4) {
       return;
    }

    rcItem.DeflateRect(1,1);
    CRect rcImage = rcItem;

    //
    // Catch the case where the combo box column has been re-sized
    // so small that the drawing of the arrow will take up the entire
    // space.  Be sure not to allow leaking into the column bordering
    // to the left.
    //
    if (rcItem.right - iImageWidth >= rcItem.left) {
        rcImage.left = rcItem.right - iImageWidth;
    }
    else {
        rcImage.left = rcItem.left;
    }
    
    rcItem.right = rcImage.left - 1;
    
    DrawFrameControl(dc, &rcImage, DFC_SCROLL, DFCS_SCROLLDOWN);

    //
    // Don't bother drawing the rest of the item if the arrow image
    // has taken up the entire drawing space
    //
    if (rcItem.right > rcItem.left) {
        if(bHasFocus && m_iNavColumn == iCol) {
            dc.DrawFocusRect(rcItem);
        }

        rcItem.DeflateRect(1, 1);
    
        DrawEdge(dc, &rcItem, EDGE_BUMP, BF_RECT|BF_MONO);
    
        rcItem.DeflateRect(1,1);
    
        CPushGDI<HFONT> PushFont(dc, m_fontComboBox);
        rcItem.DeflateRect(1,1);

        DrawText( dc, cszItemText, cszItemText.GetLength(), &rcItem, 
                  DT_END_ELLIPSIS | DT_VCENTER );

        // DrawFrameControl(dc, &rcI)
    }

    return;
}

void CCustomCellListView::CDPaintHyperlink(LPNMCUSTOMDRAW lpNMCustomDraw)
{
    NMLVCUSTOMDRAW * pLvDraw = (NMLVCUSTOMDRAW*)lpNMCustomDraw;
    NMCUSTOMDRAW * pDraw = (NMCUSTOMDRAW*)&(pLvDraw->nmcd);

    int iItem = pDraw->dwItemSpec;
    int iCol  = pLvDraw->iSubItem;

    CRect rcItem;
    GetSubItemRect(iItem, iCol, LVIR_LABEL, &rcItem);

    CString cszItemText;
    GetItemText(iItem, iCol, cszItemText.GetBuffer(MAX_PATH), MAX_PATH);
    cszItemText.ReleaseBuffer();

    BOOL bHasFocus = FALSE;

    UINT flag = LVIS_FOCUSED;
    bHasFocus = (GetItemState( iItem, flag ) & flag) == flag;

    CDCHandle dc;
    dc.Attach( pDraw->hdc );

    dc.FillSolidRect( &rcItem, pLvDraw->clrTextBk );
    
    ::SetTextColor(dc, m_clrLink);
    ::SetBkColor(dc, pLvDraw->clrTextBk);

    // if this item has focus
    if (bHasFocus)
    {
        CBrush brushSelected;
        brushSelected.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)); 
        FillRect(dc, &rcItem, brushSelected);

        ::SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
        ::SetBkColor(dc, GetSysColor(COLOR_HIGHLIGHT));
    }

    CRect rcText = rcItem;
    rcText.DeflateRect(2,2);

    CPushGDI<HFONT> PushFont(dc, m_fontHyperLink);

    // 
    // Actually draw text
    // 
    DrawText( dc, cszItemText, cszItemText.GetLength(), &rcText, 
              DT_END_ELLIPSIS | DT_VCENTER | DT_SINGLELINE );

    //
    // Just adjust the rectangle properly with DT_CALCRECT to be able to
    // properly draw the focus rectangle.  No text drawn.
    //
    DrawText( dc, cszItemText, cszItemText.GetLength(), &rcText, 
              DT_END_ELLIPSIS | DT_VCENTER | DT_CALCRECT | DT_SINGLELINE );

    rcText.InflateRect(1,1);

    if (bHasFocus && m_iNavColumn == iCol)
    {
        dc.DrawFocusRect(rcText);
    }


    return;
}

void CCustomCellListView::CalcIdealTextRect(INT iItem, INT iSubItem, 
                                            RECT &rectText, CFont &font)
{
    CClientDC dc(m_hWnd);
    RECT rcClient = { 0 };
    GetClientRect(&rcClient);

    CPushGDI<HFONT> PushFont(dc, font);
    
    CString cszItemText;
    GetItemText(iItem, iSubItem, cszItemText.GetBuffer(MAX_PATH), MAX_PATH);
    cszItemText.ReleaseBuffer();

    CRect rcItem;
    GetSubItemRect(iItem, iSubItem, LVIR_LABEL, &rcItem);

//    DWORD dwStyle = GetStyle();
    
    int nDrawStyle = DT_LEFT;
//    if (dwStyle & SS_CENTER)
//    {
//        nDrawStyle = DT_CENTER;
//    }
//    else if (dwStyle & SS_RIGHT)
//    {
//        nDrawStyle = DT_RIGHT;
//    }
    
    rcItem.DeflateRect(2,2);
    dc.DrawText(cszItemText, -1, &rcItem, nDrawStyle | DT_WORDBREAK | DT_CALCRECT);
    
    CopyRect(&rectText, &rcItem);

    return;
}
