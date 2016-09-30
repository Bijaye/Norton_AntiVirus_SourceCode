#include "StdAfx.h"
#include "resource.h"

#include "Utils.h"
#include "PushGDI.h"

#include "CustomCellListViewHeader.h"

#define  CC_COLOR_LTGRAY         RGB(220,220,220)
#define  CC_COLOR_MDGRAY         RGB(128,128,128)
#define  CC_COLOR_CTLBORDER      RGB(100,100,100)
#define  CC_COLOR_DARKBG         RGB(255, 222, 1)
#define  CC_COLOR_LIGHTBG        RGB(255,255,255)
#define  CC_COLOR_LISTDOTS       RGB(150,150,150)
#define  CC_COLOR_PROGRESS       CC_COLOR_DARKBG
#define  CC_COLOR_GRADIENT_HI    RGB(181,194,242)
#define  CC_COLOR_GRADIENT_LO    RGB(239,247,255)
#define  CC_COLOR_SUMMARYBOX_HI  RGB(181,194,242)
#define  CC_COLOR_SUMMARYBOX_LO  RGB(239,247,255)

#define CC_WIDTH_CTLBORDER       1
#define CC_WIDTH_SECTIONBORDER   1
#define CC_WIDTH_TITLE_UNDERLINE 1

CCustomCellListViewHeader::CCustomCellListViewHeader(void) : 
m_iPaintExStyle(NULL),
m_bCustomDraw(false),
m_bAllowResize(true)
{
}

CCustomCellListViewHeader::~CCustomCellListViewHeader(void)
{
}

//***************************************************************************
// 
//***************************************************************************
void CCustomCellListViewHeader::Init()
{
    CClientDC dc( m_hWnd );
    m_fontHeader = CUtils::CreateFontEz( dc, IDS_FONTEZ_LISTBOXHEADER, NULL, 
                                         _Module.GetResourceInstance() );
    
    SetFont(m_fontHeader.m_hFont, 0 );
    
    InitControlColor();
}
//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListViewHeader::OnBeginTrack(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
    return TRUE;
}

//***************************************************************************
// 
//***************************************************************************
void CCustomCellListViewHeader::InitControlColor()
{
    CClientDC dc( m_hWnd );

    const BOOL bHighContrastMode = CUtils::IsSystemHCMode();
    const BOOL bLowColorMode = CUtils::IsLowColorMode(dc);

    if( bHighContrastMode )
    {
        m_crText = GetSysColor(COLOR_WINDOWTEXT);
        m_crDisableText = GetSysColor(COLOR_GRAYTEXT);
        //m_crBackground = RGB(0,0,0);
    }
    else if( bLowColorMode )
    {
        m_crText = RGB(0,0,0);
        m_crDisableText = RGB(128,128,128);
        m_crBackground = RGB(255,255,255);
    }
    else
    {
        m_crText = RGB(0,0,0);
        m_crDisableText = CC_COLOR_MDGRAY;
        m_crBackground = RGB(255,0,0);
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
LRESULT CCustomCellListViewHeader::OnMouseMove ( UINT nMsg, WPARAM wParam, 
                                                 LPARAM lParam, 
                                                 BOOL & bHandled )
{
    
    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListViewHeader::OnNcHitTest ( UINT nMsg, WPARAM wParam, 
                                                 LPARAM lParam, 
                                                 BOOL & bHandled )
{
    // 
    // This whole thing is just here to support the dummy first column
    // kludge from AxSysListView32. 
    //
    // Determine whether to allow the cursor to change to the resize
    // cursor when the cursor is over a column divider.  Don't want to
    // let it change if it's over the divider between the invisible dummy
    // first column and the second column.
    //
    POINT hitPoint;
    CRect rcSecondItem;

    hitPoint.x = LOWORD(lParam);

    ScreenToClient(&hitPoint);

    GetItemRect(1, &rcSecondItem);

    if ( hitPoint.x > 8 ||          // 8 seems to be the magic number at
         rcSecondItem.right <= 8)   // which the cursor change happens
    {
        m_bAllowResize = true;
    }
    else
    {
        m_bAllowResize = false;
    }

    bHandled = FALSE;
    return 0;
}

//***************************************************************************
// 
//***************************************************************************

LRESULT CCustomCellListViewHeader::OnSetCursor ( UINT nMsg, WPARAM wParam, 
                                                 LPARAM lParam, 
                                                 BOOL & bHandled )
{
    // 
    // This whole thing is just here to support the dummy first column
    // kludge from AxSysListView32
    //
    if ( m_bAllowResize ) 
    {
       bHandled = FALSE; 
       return 0;
    }

    //
    // Don't let cursor change happen if in the resize disallowed area.
    // 
    bHandled = TRUE;
    return 1;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListViewHeader::OnSettingChange ( UINT nMsg, WPARAM wParam, 
                                                     LPARAM lParam, 
                                                     BOOL & bHandled )
{
    InitControlColor();
    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListViewHeader::OnEraseBkgnd ( UINT nMsg, WPARAM wParam, 
                                                  LPARAM lParam, 
                                                  BOOL & bHandled )
{
    if ( m_bCustomDraw ) 
    {
        // 
        // Fill in empty space between the right edge of the last
        // header item and the right edge of the list view, with
        // the custom background color.
        //
        CRect rect;
        GetClientRect(&rect);

        CRect rcLastItem;
        GetItemRect(GetItemCount() - 1, &rcLastItem);

        rect.left = rcLastItem.right + 2;
        rect.bottom -= 1;
        rect.top += 1;

        HDC hDC = GetDC();

        CDCHandle dc;
        dc.Attach( hDC );

        dc.FillSolidRect( &rect, m_crBackground );
        dc.SetBkColor( m_crBackground );

        ReleaseDC(hDC);

        bHandled = TRUE;
    }
    else
    {
        bHandled = FALSE;
    }

    return 0;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListViewHeader::OnSysColorChange ( UINT nMsg, 
                                                      WPARAM wParam, 
                                                      LPARAM lParam, 
                                                      BOOL & bHandled )
{
    InitControlColor();
    return 0;
}


//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListViewHeader::OnPaint ( UINT nMsg, WPARAM wParam, 
                                             LPARAM lParam, BOOL & bHandled )
{
    if (!m_bCustomDraw) {
       bHandled = FALSE;
       return 0;
    }


    // 
    // Only process WM_PAINT for the client area of the header items
    // themselves.  If there is any visible header area that is blank
    // (i.e. user has resized headers to where they don't cover the
    // whole client area), then don't paint that blank area.
    //
    CRect rectClient;
    GetClientRect( rectClient );

    CRect rcLastItem;
    GetItemRect(GetItemCount() - 1, &rcLastItem);

    rectClient.right -= (rectClient.right - rcLastItem.right);
    //rectClient.bottom -= 1;

    // The below code fills in the header with our own custom 
    // background color - i.e. fills in the areas in which the 
    // ListControl doesn't not redraw...

    CPaintDC dc( m_hWnd );

    dc.SetBkColor( m_crBackground );
    CDC dcMem;
    dcMem.CreateCompatibleDC( dc );
    dcMem.SetBkColor( m_crBackground );

    CBitmap bmpScreen;
    bmpScreen.CreateCompatibleBitmap( dc, rectClient.Width(), 
                                          rectClient.Height() );
    dc.SetBkColor( m_crBackground );
    dcMem.SetBkColor( m_crBackground );

    CPushGDI<HBITMAP> PushBitmap( dcMem, bmpScreen );
    //dcMem.FillSolidRect( rectClient, m_crBackground );
    //dcMem.FillSolidRect( rectClient, RGB(255,0,0) );
    //
    dc.SetBkColor( m_crBackground );
    dcMem.SetBkColor( m_crBackground );

    // Feed the off-screen bitmap to the header...
    DefWindowProc( WM_PRINTCLIENT, (WPARAM)dcMem.m_hDC, 0 );
    dc.SetBkColor( m_crBackground );
    dcMem.SetBkColor( m_crBackground );

    // Blast the bitmap onto the screen...
    dc.BitBlt( rectClient.left, rectClient.top, 
               rectClient.Width(), rectClient.Height(), 
               dcMem, rectClient.left, rectClient.top, SRCCOPY );
    dc.SetBkColor( m_crBackground );
    dcMem.SetBkColor( m_crBackground );

    bHandled = TRUE;
    return 1;
}

//***************************************************************************
// 
//***************************************************************************
LRESULT CCustomCellListViewHeader::OnCtlColor ( UINT nMsg, 
                                                WPARAM wParam, 
                                                LPARAM lParam, 
                                                BOOL & bHandled )
{
    return (LRESULT)m_brushWindow.m_hBrush;
}


//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnPrePaint(int idCtrl, 
                                            LPNMCUSTOMDRAW lpNMCustomDraw)
{
    CDCHandle dc;
    dc.Attach( lpNMCustomDraw->hdc );
    
    dc.SelectFont(m_fontHeader);
    ::SelectObject(dc, m_fontHeader);

    return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnPostPaint(int idCtrl, 
                                             LPNMCUSTOMDRAW lpNMCustomDraw)
{
    // 
    // Draw border of empty space, if there is any.
    //
    CRect rectClient;
    GetClientRect( rectClient );

    CRect rcLastItem;
    GetItemRect(GetItemCount() - 1, &rcLastItem);

    if (rcLastItem.right < rectClient.right - 1) {
        //
        // There is empty space between right edge of the last header
        // and right edge of list view.
        //
        rectClient.left += rcLastItem.right;
        rectClient.right += 5;

        HDC hDC = GetDC();

        CDCHandle dc;
        dc.Attach( hDC );

        CBrush borderBrush;
        borderBrush.CreateSolidBrush(RGB(224,224,224));

        dc.FrameRect( &rectClient, borderBrush );

        ReleaseDC(hDC);
    }

    return CDRF_SKIPDEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnPreErase(int idCtrl, 
                                            LPNMCUSTOMDRAW lpNMCustomDraw)
{
    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnPostErase(int idCtrl, 
                                             LPNMCUSTOMDRAW lpNMCustomDraw)
{
    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnItemPrePaint(int idCtrl, 
                                                LPNMCUSTOMDRAW lpNMCustomDraw)
{    
//    PaintHeaderItem(lpNMCustomDraw);
//
//    return CDRF_SKIPDEFAULT;
    CDCHandle dc;
    dc.Attach( lpNMCustomDraw->hdc );

    m_fontHeader = CUtils::CreateFontEz( dc, IDS_FONTEZ_LISTBOXHEADER, NULL, 
                                         _Module.GetResourceInstance() );
    
    SetFont(m_fontHeader.m_hFont, 0 );

    if (!m_bCustomDraw) {
       return CDRF_DODEFAULT;
    } 

    CRect rcItem = lpNMCustomDraw->rc;

 //   dc.FillSolidRect( &rcItem, RGB(255,0,0) );
    if (CUtils::IsSystemHCMode()) {
        //dc.SetBkColor( RGB(0,0,0) );
        dc.FillSolidRect( &rcItem, RGB(0,0,0) );
    }
    else {
        rcItem.right++;
        //dc.SetBkColor( m_crBackground );
        CBrush frameBrush;
        frameBrush = frameBrush.CreateSolidBrush(RGB(224,224,224));
        dc.FrameRect( &rcItem, frameBrush);
        rcItem.DeflateRect(1,1);
        dc.FillSolidRect( &rcItem, m_crBackground );
    }

    CString cszItemText;

    HDITEM hdi = {0};
    hdi.mask = HDI_TEXT | HDI_FORMAT;
    hdi.pszText = cszItemText.GetBuffer(MAX_PATH);
    hdi.cchTextMax = MAX_PATH;

    GetItem(lpNMCustomDraw->dwItemSpec, &hdi);
    cszItemText.ReleaseBuffer();

    cszItemText = hdi.pszText;

    CPushGDI<HFONT> PushFont( dc, m_fontHeader );
    ::SelectObject(dc, m_fontHeader);
    dc.SetBkMode( TRANSPARENT );
    dc.SetTextColor( m_crText );

    int iTextFormat = 0;

    if(hdi.fmt & HDF_CENTER)
    {
        iTextFormat |= DT_CENTER;
    }

    if(hdi.fmt & HDF_LEFT)
    {
        iTextFormat |= DT_LEFT;
    }

    if(hdi.fmt & HDF_RIGHT)
    {
        iTextFormat |= DT_RIGHT;
    }
    
    CRect rcText = lpNMCustomDraw->rc;
    rcText.DeflateRect(3,2);

    iTextFormat |= DT_VCENTER | DT_SINGLELINE;
    dc.DrawText( cszItemText, -1, rcText, iTextFormat );

    return CDRF_SKIPDEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnItemPostPaint(int idCtrl, 
                                                 LPNMCUSTOMDRAW lpNMCustomDraw)
{
    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnItemPreErase(int idCtrl, 
                                                LPNMCUSTOMDRAW lpNMCustomDraw)
{
    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnItemPostErase(int idCtrl, 
                                                 LPNMCUSTOMDRAW lpNMCustomDraw)
{
    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
DWORD CCustomCellListViewHeader::OnSubItemPrePaint(
                                                 int idCtrl, 
                                                 LPNMCUSTOMDRAW lpNMCustomDraw)
{
    return CDRF_DODEFAULT;
}

//***************************************************************************
// 
//***************************************************************************
void CCustomCellListViewHeader::PaintHeaderRect(NMCUSTOMDRAW *pnmc)
{
    HDC hdc = pnmc->hdc;
    CRect rectHeader(0,0,0,0);

    GetClientRect(&rectHeader);
    rectHeader.bottom -= CC_WIDTH_CTLBORDER;
    rectHeader.right  -= CC_WIDTH_CTLBORDER;

//    Graphics dc(hdc);
//    SolidBrush brushBorder( m_colorBorder );
//
//    Pen penBorder(&brushBorder, CC_WIDTH_CTLBORDER);
//    penBorder.SetDashStyle(DashStyleSolid);
//
//    Point pointTL(rectHeader.left, rectHeader.top);
//    Point pointBL(rectHeader.left, rectHeader.bottom);
//    Point pointBR(rectHeader.right, rectHeader.bottom);
//    Point pointTR(rectHeader.right, rectHeader.top);
//
//    dc.DrawLine(&penBorder, pointTL, pointTR);
//    dc.DrawLine(&penBorder, pointBL, pointBR);
//
//    if( !(m_iPaintExStyle & CUSTCELLISTVIEWHDR_EXSTYLE_NOEDGE) )
//    {
//        dc.DrawLine(&penBorder, pointTL, pointBL);
//        dc.DrawLine(&penBorder, pointTR, pointBR);
//    }

    return;
}

//***************************************************************************
// 
//***************************************************************************
void CCustomCellListViewHeader::PaintHeaderItem(NMCUSTOMDRAW *pnmc)
{
    CString cszItemText;

    CDCHandle dc;
    dc.Attach( pnmc->hdc );
    
    CRect rcItem = pnmc->rc;

    dc.FillSolidRect( &rcItem, RGB(0,0,0) );

    HDITEM hdi = {0};
    hdi.mask = HDI_TEXT | HDI_FORMAT;
    hdi.pszText = cszItemText.GetBuffer(MAX_PATH);
    hdi.cchTextMax = MAX_PATH;

    GetItem(pnmc->dwItemSpec, &hdi);
    cszItemText.ReleaseBuffer();

    cszItemText = hdi.pszText;

    CPushGDI<HFONT> PushFont( dc, m_fontHeader );
    ::SelectObject(dc, m_fontHeader);
    dc.SetBkMode( TRANSPARENT );
    dc.SetTextColor( m_crText );

    int iTextFormat = 0;

    if(hdi.fmt & HDF_CENTER)
    {
        iTextFormat |= DT_CENTER;
    }

    if(hdi.fmt & HDF_LEFT)
    {
        iTextFormat |= DT_LEFT;
    }

    if(hdi.fmt & HDF_RIGHT)
    {
        iTextFormat |= DT_RIGHT;
    }
    
    CRect rcText = pnmc->rc;
    rcText.DeflateRect(3,2);

    iTextFormat |= DT_VCENTER | DT_SINGLELINE;
    dc.DrawText( cszItemText, -1, rcText, iTextFormat );

    return;
}

void CCustomCellListViewHeader::SetBackgroundColor(COLORREF crBackground)
{
    m_crBackground = crBackground;
    m_bCustomDraw = true;
}
