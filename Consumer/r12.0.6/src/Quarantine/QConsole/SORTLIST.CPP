//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// SortList.cpp - created 03/05/1998 5:51:58 PM
//
// Description:
//      Implementation of CSortedListCtrl. Base class for automatic sorting
//      CListCtrl with nifty arrows on the columns (Thanks, DBUCHES).
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/SortList.cpv  $
// 
//    Rev 1.0   11 Mar 1998 15:18:58   DBuches
// Initial revision.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SortList.h"
#include "cdcos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Defines for the sort up and sort down arrows in Common Control 6 used to draw
// the collumn headers in our listview
// We have to do this becuase if we define _WIN32_WINNT=0x501, The CFileDialog
// destructor causes a crash

#ifndef HDF_SORTUP
#define HDF_SORTUP              0x0400
#endif

#ifndef HDF_SORTDOWN
#define HDF_SORTDOWN            0x0200
#endif


/////////////////////////////////////////////////////////////////////////////
// CSortedListCtrl

IMPLEMENT_DYNAMIC( CSortedListCtrl, CListCtrl )


////////////////////////////////////////////////////////////////////////////
// 03/05/1998 DALLEE, created - header added.
////////////////////////////////////////////////////////////////////////////
CSortedListCtrl::CSortedListCtrl()
{
    m_bSortAscending    = FALSE;
    m_nSortColumn       = -1;

    // Are we running on WinXP?
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    BOOL bNT = (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
    if( bNT )
    {
        if( os.dwMajorVersion > 5 )
            m_bWinXP = TRUE;
        else  
            m_bWinXP = ( os.dwMajorVersion == 5 && os.dwMinorVersion >= 1 );
    }
    else
    {
        m_bWinXP = FALSE;
    }

} // CSortedListCtrl::CSortedListCtrl()


////////////////////////////////////////////////////////////////////////////
// 03/05/1998 DALLEE, created - header added.
////////////////////////////////////////////////////////////////////////////
CSortedListCtrl::~CSortedListCtrl()
{
} // CSortedListCtrl::~CSortedListCtrl()


BEGIN_MESSAGE_MAP(CSortedListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSortedListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSortedListCtrl message handlers


////////////////////////////////////////////////////////////////////////////
// Handles WM_DRAWITEM for list view control's header.
// 03/05/1998 DALLEE, from DBUCHES' OnDrawItem() 12/1/97
////////////////////////////////////////////////////////////////////////////
void CSortedListCtrl::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct ) 
{
const int TRIANGLE_PADDING = 20;

    if( lpDrawItemStruct->CtlType != ODT_HEADER )
    {
    	CListCtrl::OnDrawItem( nIDCtl, lpDrawItemStruct );
        return;
    }

    // Get item text and width.
    
	HD_ITEM item;
    TCHAR szBuffer[100];
    item.mask = HDI_TEXT | HDI_WIDTH | HDI_LPARAM;
    item.pszText = szBuffer;
    item.cchTextMax = 100;
    CRect WindowRect;
    CHeaderCtrl *pHdr = (CHeaderCtrl*) CWnd::FromHandle( lpDrawItemStruct->hwndItem );
    pHdr->GetClientRect( &WindowRect );
    pHdr->GetItem(lpDrawItemStruct->itemID, &item );

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
    CString s(szBuffer);
    CSize size = dc.GetOutputTextExtent( s );

    // Compute text rectangle
    TEXTMETRIC tm;
    dc.GetTextMetrics( &tm );

    CRect textRect;
    CRect sizeRect;
    int iOffset = (lpDrawItemStruct->itemState & ODS_SELECTED) ? 1 : 0;

    textRect.right = rect.right - TRIANGLE_PADDING;
    textRect.left = rect.left + tm.tmAveCharWidth + iOffset;
    textRect.top = rect.top + (rect.Height() - size.cy) / 2 + iOffset;
    textRect.bottom = rect.bottom;
    dc.DrawText( szBuffer, &sizeRect, DT_LEFT | DT_CALCRECT );

    // Draw text to screen.
    dc.DrawText( szBuffer, &textRect, DT_LEFT | DT_END_ELLIPSIS  );

    // Get top of triangle.
    int iTop = ( rect.Height() - 7 ) / 2 + iOffset;
    int iLeft;

    // Compute correct X-coordinate for triangle
    if( sizeRect.Width()  + tm.tmAveCharWidth + iOffset< textRect.Width())
        iLeft = rect.left + sizeRect.Width() + 15 + iOffset;
    else
        iLeft = rect.right - 15 + iOffset;

    // Draw search direction triangle.  Only draw if we have room.
    if( m_nSortColumn == (int)lpDrawItemStruct->itemID &&
        iLeft >= textRect.left )
    {
        DrawArrow( &dc, iLeft, iTop, m_bSortAscending );
    }

    // Cleanup
    dc.SelectObject( pOldFont );
    dc.SetTextColor( oldTextColor );
    dc.SetBkColor( oldBkColor );
} // CSortedListCtrl::OnDrawItem()


////////////////////////////////////////////////////////////////////////////
//      CDC* pDC                        [in] DC to use for drawing.
//      int x                           [in] x coord of leftmost vertex
//      int y                           [in] y coord of topmost vertex.
//      BOOL bUp                        [in] direction of vertical point
//
// Draws the sort direction arrow in the list view's column header.
// For internal use by OnDrawItem().
// 03/05/1998 DALLEE, from DBUCHES' DrawArrow() 12/1/97
////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////
// Respond to column click. 
// Record last column clicked and sort direction, sort list items.
// Returns: Nothing. *pResult set to 0 for success.
// 03/05/1998 DALLEE, created - header added.
////////////////////////////////////////////////////////////////////////////
void CSortedListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    int nOldSortColumn = m_nSortColumn;
    int nNewSortColumn = pNMListView->iSubItem;

    if ( m_nSortColumn == pNMListView->iSubItem )
    {
        m_bSortAscending = !m_bSortAscending;
    }
    else
    {
        // Clear the sort Arrow
        if( m_bWinXP )
            ClearSortArrow(m_nSortColumn);
        
        m_bSortAscending = TRUE;
        m_nSortColumn = pNMListView->iSubItem;
    }

    // Set the correct sort arrow.
    if( m_bWinXP )
        SetSortArrow(m_nSortColumn, m_bSortAscending ? HDF_SORTUP : HDF_SORTDOWN);

    SortItems();

	*pResult = 0;
} // CSortedListCtrl::OnColumnClick()


void CSortedListCtrl::ClearSortArrow(int nColumn)
{
    CHeaderCtrl* pHeader = GetHeaderCtrl();

    HD_ITEM item;

    item.mask = HDI_FORMAT;
    pHeader->GetItem( nColumn, &item );

    // Clear the sort item
    item.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);

    pHeader->SetItem(nColumn, &item );
}


void CSortedListCtrl::SetSortArrow(int nColumn, int nDirection)
{
    CHeaderCtrl* pHeader = GetHeaderCtrl();

    HD_ITEM item;

    item.mask = HDI_FORMAT;
    pHeader->GetItem( nColumn, &item );
    
    // Clear the sort item
    item.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);

    item.fmt |= nDirection;
    
    pHeader->SetItem(nColumn, &item );
}


////////////////////////////////////////////////////////////////////////////
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
// 03/05/1998 DALLEE, created - header added.
////////////////////////////////////////////////////////////////////////////
int CALLBACK CSortedListCtrl::StaticCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lData )
{
    auto    CSortedListCtrl *pList;

    pList  = reinterpret_cast<CSortedListCtrl *>( lData );

    return ( pList->CompareFunc( lParam1, lParam2 ) );
} // CSortedListCtrl::StaticCompareFunc()


////////////////////////////////////////////////////////////////////////////
// These overridden InsertColumn() members behave as CListCtrl::InsertColumn()
// but add the owner draw format to all columns added to the list control.
//
// Returns: int 
//      Zero-based index of inserted column on success.
//      -1 on error
// 02/24/1998 DALLEE, created - header added.
////////////////////////////////////////////////////////////////////////////

int CSortedListCtrl::InsertColumn( int nCol, const LV_COLUMN* pColumn )
{
    auto    CHeaderCtrl *pHdr;
    auto    HD_ITEM     item;
    auto    int         nResult;

    nResult = CListCtrl::InsertColumn( nCol, pColumn );


    // Setup column header on non-WinXP clients
    if ( !m_bWinXP && (-1 != nResult) )
    {
        pHdr = reinterpret_cast<CHeaderCtrl *>( GetDlgItem( 0 ) );

        // Make new column owner draw
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

    // Setup column header on non-WinXP clients
    if ( !m_bWinXP && (-1 != nResult) )
    {
        pHdr = reinterpret_cast<CHeaderCtrl *>( GetDlgItem( 0 ) );

        // Make new column owner draw
        item.mask = HDI_FORMAT;
        item.fmt  = HDF_OWNERDRAW;

        pHdr->SetItem( nResult, &item );
    }

    return ( nResult );
} // CSortedListCtrl::InsertColumn()


////////////////////////////////////////////////////////////////////////////
// Default sort. Items will be compared using a derived class' CompareFunc().
// Returns: TRUE on success, FALSE otherwise.
// 02/24/1998 DALLEE, created - header added.
////////////////////////////////////////////////////////////////////////////
BOOL CSortedListCtrl::SortItems()
{
    return ( CListCtrl::SortItems( StaticCompareFunc, reinterpret_cast<DWORD>( this ) ) );
}
