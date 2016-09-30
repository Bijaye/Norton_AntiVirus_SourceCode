// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// SortList.cpp - created 03/05/1998 5:51:58 PM
//
// $Header:   S:/QCONSOLE/VCS/SortList.cpv   1.0   11 Mar 1998 15:18:58   DBuches  $
//
// Description:
//      Implementation of CSortedListCtrl. Base class for automatic sorting
//      CListCtrl with nifty arrows on the columns (Thanks, DBUCHES).
//
// Contains:
//
//*************************************************************************
// $Log:   S:/QCONSOLE/VCS/SortList.cpv  $
// 
//    Rev 1.0   11 Mar 1998 15:18:58   DBuches
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "SortList.h"
#include "cdcos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortedListCtrl

IMPLEMENT_DYNAMIC( CSortedListCtrl, CListCtrl )


//*************************************************************************
// CSortedListCtrl::CSortedListCtrl()
// CSortedListCtrl::~CSortedListCtrl()
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

CSortedListCtrl::CSortedListCtrl()
{
    m_bSortAscending    = FALSE;
    m_nSortColumn       = -1;
} // CSortedListCtrl::CSortedListCtrl()

CSortedListCtrl::~CSortedListCtrl()
{
} // CSortedListCtrl::~CSortedListCtrl()


BEGIN_MESSAGE_MAP(CSortedListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSortedListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSortedListCtrl message handlers


//*************************************************************************
// CSortedListCtrl::OnDrawItem()
//
// void CSortedListCtrl::OnDrawItem(
//      int nIDCtl
//      LPDRAWITEMSTRUCT lpDrawItemStruct )
//
// Handles WM_DRAWITEM for list view control's header.
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, from DBUCHES' OnDrawItem() 12/1/97
//*************************************************************************

void CSortedListCtrl::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
#define TRIANGLE_PADDING 20

    if( lpDrawItemStruct->CtlType != ODT_HEADER )
    {
    	CListCtrl::OnDrawItem( nIDCtl, lpDrawItemStruct );
        return;
    }

    // Get item text and width.

    HD_ITEM item;
    TCHAR szBuffer[ 100 ];
    item.mask = HDI_TEXT | HDI_WIDTH | HDI_LPARAM;
    item.pszText = szBuffer;
    item.cchTextMax = 100;
    CRect WindowRect;
    CHeaderCtrl *pHdr = ( CHeaderCtrl* ) CWnd::FromHandle( lpDrawItemStruct->hwndItem );
    pHdr->GetClientRect( &WindowRect );
    pHdr->GetItem( lpDrawItemStruct->itemID, &item );

    // Get rectangle for this item
    CRect rect( lpDrawItemStruct->rcItem );

    // Set up DC
    CDCOS dc( CDC::FromHandle( lpDrawItemStruct->hDC ), WindowRect, rect );

    // Fill rect.
    CBrush brush( GetSysColor( COLOR_BTNFACE ) );
    dc.FillRect( &rect, &brush );

    // Set up DC
    CFont *pOldFont = dc.SelectObject( pHdr->GetFont() );

    COLORREF oldBkColor = dc.SetBkColor( GetSysColor( COLOR_BTNFACE ) );
    COLORREF oldTextColor = dc.SetTextColor( GetSysColor( COLOR_BTNTEXT ) );

    // Compute size of text
    CString s( szBuffer );
    CSize size = dc.GetOutputTextExtent( s );

    // Compute text rectangle
    TEXTMETRIC tm;
    dc.GetTextMetrics( &tm );

    CRect textRect;
    CRect sizeRect;
    int iOffset = ( lpDrawItemStruct->itemState & ODS_SELECTED ) ? 1 : 0;

    textRect.right = rect.right - TRIANGLE_PADDING;
    textRect.left = rect.left + tm.tmAveCharWidth + iOffset;
    textRect.top = rect.top + ( rect.Height() - size.cy ) / 2 + iOffset;
    textRect.bottom = rect.bottom;
    dc.DrawText( szBuffer, &sizeRect, DT_LEFT | DT_CALCRECT );

    // Draw text to screen.
    dc.DrawText( szBuffer, &textRect, DT_LEFT | DT_END_ELLIPSIS  );

    // Get top of triangle.
    int iTop = ( rect.Height() - 7 ) / 2 + iOffset;
    int iLeft;

    // Compute correct X-coordinate for triangle
    if( sizeRect.Width()  + tm.tmAveCharWidth + iOffset< textRect.Width() )
        iLeft = rect.left + sizeRect.Width() + 15 + iOffset;
    else
        iLeft = rect.right - 15 + iOffset;

    // Draw search direction triangle.  Only draw if we have room.
    if( m_nSortColumn == ( int )lpDrawItemStruct->itemID &&
        iLeft >= textRect.left )
    {
        DrawArrow( &dc, iLeft, iTop, m_bSortAscending );
    }

    // Cleanup
    dc.SelectObject( pOldFont );
    dc.SetTextColor( oldTextColor );
    dc.SetBkColor( oldBkColor );
} // CSortedListCtrl::OnDrawItem()


//*************************************************************************
// CSortedListCtrl::DrawArrow()
//
// void CSortedListCtrl::DrawArrow(
//      CDC* pDC                        [in] DC to use for drawing.
//      int x                           [in] x coord of leftmost vertex
//      int y                           [in] y coord of topmost vertex.
//      BOOL bUp )                      [in] direction of vertical point
//
// Draws the sort direction arrow in the list view's column header.
// For internal use by OnDrawItem().
//
// Returns: 
//      Nothing.
//*************************************************************************
// 03/05/1998 DALLEE, from DBUCHES' DrawArrow() 12/1/97
//*************************************************************************

void CSortedListCtrl::DrawArrow( CDC* pDC, int x, int y, BOOL bUp )
{
    // Create pens needed
    CPen whitePen;
    whitePen.CreateStockObject( WHITE_PEN );
    CPen grayPen( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );

    int iTop = y;
    int iBottom = iTop + 6;


    CPen * oldPen = pDC->SelectObject( &grayPen );

    if( bUp )
    {
        // Draw bottom line
        pDC->SelectObject( &whitePen );
        pDC->MoveTo( x, iBottom );
        pDC->LineTo( x + 7, iBottom );

        // Draw left diagonal
        pDC->SelectObject( &grayPen );
        pDC->MoveTo( x + 3, iTop );
        pDC->LineTo( x , iBottom );

        // Draw right diagonal
        pDC->MoveTo( x + 4, iTop );
        pDC->SelectObject( &whitePen );
        pDC->LineTo( x + 8, iBottom +1);
    }
    else
    {
        // Draws downward pointing triangle

        // Draw top line
        pDC->MoveTo( x, iTop );
        pDC->LineTo( x + 7, iTop );

        // Draw left diagonal
        pDC->MoveTo( x + 3, iBottom );
        pDC->LineTo( x , iTop );

        // Draw right diagonal
        pDC->MoveTo( x + 4, iBottom );
        pDC->SelectObject( &whitePen );
        pDC->LineTo( x + 8, iTop -1);
    }

    // Cleanup
    pDC->SelectObject( oldPen );
} // CSortedListCtrl::DrawArrow()


//*************************************************************************
// CSortedListCtrl::OnColumnclick()
//
// void CSortedListCtrl::OnColumnclick(
//      NMHDR* pNMHDR
//      LRESULT* pResult )
//
// Respond to column click. 
// Record last column clicked and sort direction, sort list items.
//
// Returns: 
//      Nothing. *pResult set to 0 for success.
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

void CSortedListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if ( m_nSortColumn == pNMListView->iSubItem )
    {
        m_bSortAscending = !m_bSortAscending;
    }
    else
    {
        m_bSortAscending = TRUE;
        m_nSortColumn = pNMListView->iSubItem;
    }

    SortItems();

	*pResult = 0;
} // CSortedListCtrl::OnColumnclick()


//*************************************************************************
// CSortedListCtrl::StaticCompareFunc()
//
// int CALLBACK CSortedListCtrl::StaticCompareFunc(
//      LPARAM lParam1                  [in] item data for first item.
//      LPARAM lParam2                  [in] item data for second item.
//      LPARAM lData )                  [in] CSortedListCtrl*
//
// This is the callback routine for SortItems().
// This routine simply passes the comparison query to a derived class'
// CompareFunc(). The derived class should return the following:
//      <0 if item one should precede item two.
//       0 if items are equivalent
//      >0 if item one should follow item two.
// Member variables m_bSortAscending and m_nSortColumn can be queried to
// determine sort criteria.
//
// Returns: 
//      Result of derived class' CompareFunc().
//*************************************************************************
// 03/05/1998 DALLEE, created - header added.
//*************************************************************************

int CALLBACK CSortedListCtrl::StaticCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lData )
{
    auto    CSortedListCtrl *pList;

    pList  = reinterpret_cast<CSortedListCtrl *>( lData );

    return ( pList->CompareFunc( lParam1, lParam2 ) );
} // CSortedListCtrl::StaticCompareFunc()


//*************************************************************************
// CSortedListCtrl::InsertColumn()
//
// CSortedListCtrl::InsertColumn(
//      int nCol
//      const LV_COLUMN* pColumn )
//
// CSortedListCtrl::InsertColumn(
//      int       nCol
//      LPCTSTR   lpszColumnHeading
//      int       nFormat
//      int       nWidth
//      int       nSubItem )
//
// These overridden InsertColumn() members behave as CListCtrl::InsertColumn()
// but add the owner draw format to all columns added to the list control.
//
// Returns: int 
//      Zero-based index of inserted column on success.
//      -1 on error
//*************************************************************************
// 02/24/1998 DALLEE, created - header added.
//*************************************************************************

int CSortedListCtrl::InsertColumn( int nCol, const LV_COLUMN* pColumn )
{
    auto    CHeaderCtrl *pHdr;
    auto    HD_ITEM     item;
    auto    int         nResult;

    nResult = CListCtrl::InsertColumn( nCol, pColumn );

    // Make new column owner draw

    if ( -1 != nResult )
    {
        pHdr = reinterpret_cast<CHeaderCtrl *>( GetDlgItem( 0 ) );

        item.mask = HDI_FORMAT;
        item.fmt  = HDF_OWNERDRAW;

        pHdr->SetItem( nResult, &item );
    }

    return ( nResult );
} // CSortedListCtrl::InsertColumn()

int CSortedListCtrl::InsertColumn( int       nCol, 
                                   LPCTSTR   lpszColumnHeading, 
                                   int       nFormat,
                                   int       nWidth, 
                                   int       nSubItem )
{
    auto    CHeaderCtrl *pHdr;
    auto    HD_ITEM     item;
    auto    int     nResult;

    nResult = CListCtrl::InsertColumn( nCol, lpszColumnHeading, nFormat, nWidth, nSubItem );

    // Make new column owner draw

    if ( -1 != nResult )
    {
        pHdr = reinterpret_cast<CHeaderCtrl *>( GetDlgItem( 0 ) );

        item.mask = HDI_FORMAT;
        item.fmt  = HDF_OWNERDRAW;

        pHdr->SetItem( nResult, &item );
    }

    return ( nResult );
} // CSortedListCtrl::InsertColumn()


//*************************************************************************
// CSortedListCtrl::SortItems()
//
// Default sort. Items will be compared using a derived class' CompareFunc().
//
// Returns:
//      TRUE on success, FALSE otherwise.
//*************************************************************************
// 02/24/1998 DALLEE, created - header added.
//*************************************************************************

BOOL CSortedListCtrl::SortItems()
{
    return ( CListCtrl::SortItems( StaticCompareFunc, reinterpret_cast<DWORD>( this ) ) );
} // CSortedListCtrl::SortItems()

