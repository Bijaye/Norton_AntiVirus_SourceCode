// ListVwEx.cpp : implementation of the CListViewEx class
// SCC: @(#) 66 10/11/99 11:30:29 Avnadmin32 1.2.1.2 @(#)
//
#include "stdafx.h"
#include "ListVwEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListViewEx

IMPLEMENT_DYNCREATE(CListViewEx, CListView)

BEGIN_MESSAGE_MAP(CListViewEx, CListView)
    //{{AFX_MSG_MAP(CListViewEx)
    ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListViewEx construction/destruction

CListViewEx::CListViewEx()
{
	m_nRowsPerPage = 30;
}

CListViewEx::~CListViewEx()
{
}

BOOL CListViewEx::PreCreateWindow(CREATESTRUCT& cs)
{
    // default is report view and full row selection
    cs.style&=~LVS_TYPEMASK;
    cs.style|=LVS_REPORT | LVS_SHOWSELALWAYS;

    return(CListView::PreCreateWindow(cs));
}

/////////////////////////////////////////////////////////////////////////////
// CListViewEx drawing

/////////////////////////////////////////////////////////////////////////////
// CListViewEx diagnostics

#ifdef _DEBUG

void CListViewEx::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListViewEx message handlers

void CListViewEx::OnKillFocus(CWnd* pNewWnd) 
{
    m_titleTip.Hide();
    CListView::OnKillFocus(pNewWnd);
}

void CListViewEx::OnInitialUpdate() 
{
    CListCtrl& listCtrl = GetListCtrl();
	CListView::OnInitialUpdate();

    listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	m_titleTip.Create( this );
    m_tooltip.Create( this );
}

// CellRectFromPoint    - Determine the row, col and bounding rect of a cell
// Returns              - row index on success, -1 otherwise
// point                - point to be tested.
// cellrect             - to hold the bounding rect
// col                  - to hold the column index, can be NULL
int CListViewEx::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col)
{
    CListCtrl& listCtrl = GetListCtrl();
    int colnum;
    
    // Make sure that the ListView is in LVS_REPORT
    if( (listCtrl.GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
        return -1;
    
    // Get the top and bottom row visible
    int row = listCtrl.GetTopIndex();
    int bottom = row + listCtrl.GetCountPerPage();
    if( bottom > listCtrl.GetItemCount() )
        bottom = listCtrl.GetItemCount();
    
    // Get the number of columns
    CHeaderCtrl* pHeader = (CHeaderCtrl*)listCtrl.GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();
    
    // Loop through the visible rows
    for( ;row <=bottom;row++)
    {
        // Get bounding rect of item and check whether point falls in it.
        CRect rect;
        listCtrl.GetItemRect( row, &rect, LVIR_BOUNDS );
        if( rect.PtInRect(point) )
        {
            // Now find the column
            for( colnum = 0; colnum < nColumnCount; colnum++ )
            {
                int colwidth = listCtrl.GetColumnWidth(colnum);
                if( point.x >= rect.left && 
                    point.x <= (rect.left + colwidth ) )
                {
                    // Found the column
                    RECT rectClient;
                    listCtrl.GetClientRect( &rectClient );
                    if( point.x > rectClient.right )
                        return -1;
                    if( col ) 
                        *col = colnum;
                    rect.right = rect.left + colwidth;
                    if( rect.right > rectClient.right ) 
                        rect.right = rectClient.right;
                    *cellrect = rect;
                    return row;
                }
                rect.left += colwidth;
            }
        }
    }
    return -1;
}

void CListViewEx::OnMouseMove(UINT nFlags, CPoint point) 
{
    CWnd * parentWnd = GetParent();
    CWnd * focusWnd = GetFocus();
    
    if (focusWnd && (focusWnd == this || focusWnd == parentWnd))
    {
        CListCtrl& listCtrl = GetListCtrl();
        if( nFlags == 0 )
        {
            // To enable Title Tips
            int row, col;
            RECT cellrect;
            row = CellRectFromPoint(point, &cellrect, &col );
            if( row != -1 )
            {
                // offset is equal to TextExtent of 2 space characters.
                // Make sure you have the right font selected into the
                // device context before calling GetTextExtent.
                // You can save this value as a member variable.
                // offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;
                int offset = 6;
                if( col == 0 ) 
                {
                    CRect rcLabel;
                    listCtrl.GetItemRect( row, &rcLabel, LVIR_LABEL );
                    offset = rcLabel.left - cellrect.left + offset / 2;
                }
                cellrect.top--;
                m_titleTip.Show( cellrect, listCtrl.GetItemText( row, col ), offset-1 );
            }
        }
    }
    
    CListView::OnMouseMove(nFlags, point);
}

#define        HEADER_HEIGHT       4
#define        FOOTER_HEIGHT       3
#define        LEFT_MARGIN         8
#define        RIGHT_MARGIN        4

void CListViewEx::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CListCtrl& listCtrl = GetListCtrl();
	if(NULL == pDC || NULL == pInfo)
		return;
	
	//let us do all dimesions in ListControl units (Device) rather than
	//printer device units. Since we have more control on them
	CDC  *pCtlDC = GetDC();
	if(NULL == pCtlDC)
		return ;
	TEXTMETRIC tm;
	pCtlDC->GetTextMetrics(&tm);
	m_nCharWidth = tm.tmAveCharWidth;
	
	pDC->SetMapMode(MM_ANISOTROPIC);
	
	CRect     rc;
	listCtrl.GetItemRect(0, &rc, LVIR_BOUNDS);
	m_nRowHeight = rc.Height();
	
	int nMargins = (LEFT_MARGIN+RIGHT_MARGIN)*m_nCharWidth;
	int nWidth = 0;
	LV_COLUMN lvc;
	lvc.mask = LVCF_WIDTH;

	for (int nColumn = 0; listCtrl.GetColumn(nColumn, &lvc); nColumn++)
		nWidth += lvc.cx;

	pDC->SetWindowExt(nWidth + nMargins, (m_nRowsPerPage + 1 + HEADER_HEIGHT + FOOTER_HEIGHT) * m_nRowHeight);
	
	pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES),pDC->GetDeviceCaps(VERTRES));
	ReleaseDC(pCtlDC);
}

void CListViewEx::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if(NULL == pDC || NULL == pInfo)
		return;
	
	CListCtrl& listCtrl = GetListCtrl();
	int       nStartRow = 0;
	int       nEndRow = 0;
	int       nMaxRowCount = listCtrl.GetItemCount();
	
	nStartRow = (pInfo->m_nCurPage - 1)*m_nRowsPerPage;
	nEndRow = nStartRow+m_nRowsPerPage;
	if(nEndRow > nMaxRowCount)
		nEndRow = nMaxRowCount;
	
	//create bold font for header and footer
	CFont     *pOldFont = NULL;
	CFont     BoldFont;
	LOGFONT  lf;
	pOldFont = listCtrl.GetFont();;
	pOldFont->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	BoldFont.CreateFontIndirect(&lf);
	
	pOldFont = pDC->SelectObject(&BoldFont);
	int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);
	//print the header
	PrintHeader(pDC, pInfo);
	//Print the footer
	PrintFooter(pDC, pInfo);
	pDC->SetBkMode(nPrevBkMode);
	pDC->SelectObject(pOldFont);
	BoldFont.DeleteObject();

	//Print Header Control
	DrawRow(pDC, -1, pInfo);
	//start printing rows
	for(int i = nStartRow; i < nEndRow; i++)
		DrawRow(pDC, i, pInfo);
	
	return;
}

void CListViewEx::PrintFooter (CDC *pDC, CPrintInfo *pInfo)
{
	CRect     rc(pInfo->m_rectDraw);
	
	//draw line
	rc.left += LEFT_MARGIN*m_nCharWidth;
	rc.right -= RIGHT_MARGIN*m_nCharWidth;
	rc.top = rc.bottom - FOOTER_HEIGHT*m_nRowHeight;
	rc.bottom = rc.top + m_nRowHeight;
	pDC->MoveTo(rc.left, rc.top);
	pDC->LineTo(rc.right, rc.top);
	
	//draw page number
	CString   sTemp ;
	rc.OffsetRect(0, m_nRowHeight/2);
	sTemp.GetBuffer(2048);
    sTemp.Format ("%d/%d", pInfo->m_nCurPage, m_nMaxPage);

	pDC->DrawText(sTemp,-1,rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	
	CTime     t = CTime::GetCurrentTime();
	sTemp = t.Format("%c");
	pDC->DrawText(sTemp,-1,rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
}

int CListViewEx::DrawRow(CDC *pDC, int nItem, CPrintInfo *pInfo)
{
	CImageList          *pImageList = NULL;
	CFont               *pOldFont = NULL;
	CFont               BoldFont;
	CString             sLabel;
	UINT           dtFlags = DT_SINGLELINE|DT_NOPREFIX|DT_VCENTER;
	int                 nSaveDC = pDC->SaveDC();
	CListCtrl& listCtrl = GetListCtrl();
	LOGFONT  lf;
	pOldFont = listCtrl.GetFont();;
	pOldFont->GetLogFont(&lf);
	if (nItem == -1)
		lf.lfWeight = FW_BOLD;
	BoldFont.CreateFontIndirect(&lf);
	
	pOldFont = pDC->SelectObject(&BoldFont);
	int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);
	
	if(!pDC->IsPrinting())
		dtFlags |= (DT_NOCLIP | DT_END_ELLIPSIS);     //no clip because we add ellipsis at the end

	if (nItem >= 0)
	{
		// get item data
		LV_ITEM lvi;
		lvi.mask = LVIF_IMAGE | LVIF_STATE;
		lvi.iItem=nItem;
		lvi.iSubItem=0;
		lvi.stateMask=0xFFFF;         // get all state flags
		listCtrl.GetItem(&lvi);
		
		//Get rectangles for painting
		CRect     rcBounds, rcLabel, rcIcon;
		listCtrl.GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
		rcBounds.top = pInfo->m_rectDraw.top + ((HEADER_HEIGHT + 1 + nItem - ((pInfo->m_nCurPage - 1)*m_nRowsPerPage)) * m_nRowHeight);
		rcBounds.bottom = rcBounds.top + m_nRowHeight;
		rcBounds.left += LEFT_MARGIN * m_nCharWidth;
		rcBounds.right += LEFT_MARGIN * m_nCharWidth;
		CRect     rcCol(rcBounds);

		listCtrl.GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcLabel.top = pInfo->m_rectDraw.top + ((HEADER_HEIGHT + 1 + nItem - ((pInfo->m_nCurPage - 1)*m_nRowsPerPage)) * m_nRowHeight);
		rcLabel.bottom = rcLabel.top + m_nRowHeight;
		rcLabel.left += LEFT_MARGIN * m_nCharWidth;
		rcLabel.right += LEFT_MARGIN * m_nCharWidth;

		listCtrl.GetItemRect(nItem, rcIcon, LVIR_ICON);
		rcIcon.top = pInfo->m_rectDraw.top + ((HEADER_HEIGHT + 1 + nItem - ((pInfo->m_nCurPage - 1)*m_nRowsPerPage)) * m_nRowHeight);
		rcIcon.bottom = rcIcon.top + m_nRowHeight;
		rcIcon.left += LEFT_MARGIN * m_nCharWidth;
		rcIcon.right += LEFT_MARGIN * m_nCharWidth;
		
		sLabel = listCtrl.GetItemText(nItem, 0);

		//Label offset
		int offset = pDC->GetTextExtent(_T(" "), 1).cx;
		
		
		//set clip region
		rcCol.right = rcCol.left + listCtrl.GetColumnWidth(0);
		
		//Draw Normal Icon
		pImageList = listCtrl.GetImageList(LVSIL_SMALL);  //assuming printing in report mode only
		if(pImageList)
		{
			UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
			pImageList->Draw(pDC, lvi.iImage, CPoint(rcIcon.left, rcIcon.top), 
				(0)|ILD_TRANSPARENT|nOvlImageMask);
		}
		
		//Draw item label
		rcLabel.left += offset/2;
		rcLabel.right -= offset;
		dtFlags |= DT_LEFT;
		pDC->DrawText(sLabel, rcLabel, dtFlags);
		
		//dRAW LABELS FOR REMAINING COLUMNS
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT|LVCF_WIDTH;
		
		for(int nColumn = 1; listCtrl.GetColumn(nColumn, &lvc); nColumn++)
		{
			rcCol.left = rcCol.right;
			rcCol.right += lvc.cx;
			
			sLabel = listCtrl.GetItemText(nItem, nColumn);
			if(sLabel.IsEmpty())
				continue;
			
			//Get the text justification
			UINT nJustify = DT_LEFT;
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
				
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
				
			default:
				break;
			}
			rcLabel = rcCol;
			rcLabel.left += offset;
			rcLabel.right -= offset;
			
			dtFlags &= ~DT_RIGHT;
			dtFlags &= ~DT_CENTER;
			dtFlags |= nJustify;
			pDC->DrawText(sLabel, -1, rcLabel, dtFlags);
		}
	}
	else
	{
		//Draw Header Control

		// get item 0 data
		LV_ITEM lvi;
		lvi.mask = LVIF_IMAGE | LVIF_STATE;
		lvi.iItem=0;
		lvi.iSubItem=0;
		lvi.stateMask=0xFFFF;         // get all state flags
		listCtrl.GetItem(&lvi);
		
		//Get rectangles for painting
		CRect     rcBounds, rcLabel;
		listCtrl.GetItemRect(0, rcBounds, LVIR_BOUNDS);
		rcBounds.top = pInfo->m_rectDraw.top + (HEADER_HEIGHT * m_nRowHeight);
		rcBounds.bottom = rcBounds.top + m_nRowHeight;
		rcBounds.left += LEFT_MARGIN * m_nCharWidth;
		rcBounds.right += LEFT_MARGIN * m_nCharWidth;
		CRect     rcCol(rcBounds);

		rcCol.right = rcCol.left;
		
		//Label offset
		int offset = pDC->GetTextExtent(_T(" "), 1).cx;
		
		//DRAW HEADER FOR REMAINING COLUMNS
		LV_COLUMN lvc;
		char HeaderText[256];
		lvc.pszText = HeaderText;
		lvc.cchTextMax = sizeof(HeaderText);
		lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
		
		pDC->MoveTo (pInfo->m_rectDraw.left + LEFT_MARGIN*m_nCharWidth, rcBounds.bottom);
		pDC->LineTo (pInfo->m_rectDraw.right - RIGHT_MARGIN*m_nCharWidth, rcBounds.bottom);
		for(int nColumn = 0; listCtrl.GetColumn(nColumn, &lvc); nColumn++)
		{
			rcCol.left = rcCol.right;
			rcCol.right += lvc.cx;
			
			sLabel = lvc.pszText;

			if(sLabel.IsEmpty())
				continue;
			
			//Get the text justification
			UINT nJustify = DT_LEFT;
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
				
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
				
			default:
				break;
			}
			rcLabel = rcCol;
			rcLabel.left += offset;
			rcLabel.right -= offset;
			
			dtFlags &= ~DT_RIGHT;
			dtFlags &= ~DT_CENTER;
			dtFlags |= nJustify;
			pDC->DrawText(sLabel, -1, rcLabel, dtFlags);
		}
	}
	pDC->SetBkMode(nPrevBkMode);
	pDC->SelectObject(pOldFont);
	BoldFont.DeleteObject();
	pDC->RestoreDC(nSaveDC);
	return 0;
}

void CListViewEx::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
	CRect               rc(pInfo->m_rectDraw);
	CString             sTemp;
	
	//Print App title
	rc.left += LEFT_MARGIN*m_nCharWidth;
	rc.right -= RIGHT_MARGIN*m_nCharWidth;
	rc.bottom = rc.top+m_nRowHeight;
	
	//print App title on top right margin
	sTemp.LoadString(AFX_IDS_APP_TITLE);
	pDC->DrawText(sTemp, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
}

BOOL CListViewEx::OnPreparePrinting(CPrintInfo* pInfo) 
{
	CListCtrl& listCtrl = GetListCtrl();
	int  nMaxRowCount = listCtrl.GetItemCount();
	
	if (!nMaxRowCount)
		return FALSE;

	m_nMaxPage = nMaxRowCount/m_nRowsPerPage;
	if (nMaxRowCount % m_nRowsPerPage)
		m_nMaxPage++;

	pInfo->SetMaxPage(m_nMaxPage);
	pInfo->m_nCurPage = 1;  // start printing at page# 1
	return DoPreparePrinting(pInfo);
}

int CListViewEx::GetColumnCount() const
{
    if( (GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
            return 0;

     // get the header control
     CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
     // return the number of items in it - ie. the number of columns
     return pHeader->GetItemCount();
}

void CListViewEx::AutoSizeColumns(int col /*=-1*/)
{
     // Call this after your list control is filled
	CListCtrl& listCtrl = GetListCtrl();

    if( (GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
            return;

    SetRedraw(false);
    int mincol = col < 0 ? 0 : col;
    int maxcol = col < 0 ? GetColumnCount()-1 : col;
    for (col = mincol; col <= maxcol; col++)
    {
        listCtrl.SetColumnWidth(col,LVSCW_AUTOSIZE);
        int wc1 = listCtrl.GetColumnWidth(col) + 5;
        listCtrl.SetColumnWidth(col,LVSCW_AUTOSIZE_USEHEADER);
        int wc2 = listCtrl.GetColumnWidth(col) + 5;
        int wc = max(MINCOLWIDTH,max(wc1,wc2));
        listCtrl.SetColumnWidth(col,wc);
    }
    RecalcHeaderTips();
    SetRedraw(true);
}

BOOL CListViewEx::PreTranslateMessage(MSG* pMsg) 
{
    m_tooltip.RelayEvent( pMsg );   	
	return CListView::PreTranslateMessage(pMsg);
}

// AddHeaderToolTip     - Add a tooltip for the column header
//                        The control mode should be LVS_REPORT
// Returns              - TRUE on success
// nCol                 - the column index
// sTip                 - the tooltip text. A NULL will use the 
//                        column header text
BOOL CListViewEx::AddHeaderToolTip(int nCol, LPCTSTR sTip /*= NULL*/)
{
	CListCtrl& listCtrl = GetListCtrl();

    const int TOOLTIP_LENGTH = 80;
    char buf[TOOLTIP_LENGTH+1];

    if( (GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
            return FALSE;

    CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();
    if( nCol >= nColumnCount)
            return FALSE;

    // Get the header height
    RECT rect;
    pHeader->GetClientRect( &rect );

    RECT rctooltip;
    rctooltip.top = 0;
    rctooltip.bottom = rect.bottom;

    // Now get the left and right border of the column
    rctooltip.left = 0 - GetScrollPos( SB_HORZ );
    for( int i = 0; i < nCol; i++ )
            rctooltip.left += listCtrl.GetColumnWidth( i );
    rctooltip.right = rctooltip.left + listCtrl.GetColumnWidth( nCol );

    if( sTip == NULL )
    {
            // Get column heading
            LV_COLUMN lvcolumn;
            lvcolumn.mask = LVCF_TEXT;
            lvcolumn.pszText = buf;
            lvcolumn.cchTextMax = TOOLTIP_LENGTH;
            if( !listCtrl.GetColumn( nCol, &lvcolumn ) )
                    return FALSE;
    }


    m_tooltip.AddTool( GetDlgItem(0), sTip ? sTip : buf, &rctooltip, nCol+1 );
    return TRUE;
}

void CListViewEx::RecalcHeaderTips()
{
	CListCtrl& listCtrl = GetListCtrl();

    if( (GetStyle() & LVS_TYPEMASK) != LVS_REPORT )
            return;

     // Update the tooltip info
     CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
     if (!pHeader)
         return;

     RECT rect; pHeader->GetClientRect(&rect);
     RECT rctooltip; rctooltip.top = 0; rctooltip.bottom = rect.bottom;
     //rctooltip.left = 0 - GetScrollPos(SB_HORZ);
     rctooltip.left = 0;
     CToolInfo toolinfo; toolinfo.cbSize = sizeof(toolinfo);
     // Cycle through the tooltipinfo for each column
     int numcol = pHeader->GetItemCount();
     for (int col = 0; col <= numcol; col++ ) {
          m_tooltip.GetToolInfo(toolinfo,pHeader,col+1);
          rctooltip.right = rctooltip.left + listCtrl.GetColumnWidth(col);
          toolinfo.rect = rctooltip;
          m_tooltip.SetToolInfo (&toolinfo);
          rctooltip.left += listCtrl.GetColumnWidth(col);
     }
}

BOOL CListViewEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    HD_NOTIFY       *pHDN = (HD_NOTIFY*)lParam;
    
    switch (pHDN->hdr.code) {
        // ...
        // other message handling here
        // ...
    case HDN_ENDTRACKA:
    case HDN_ENDTRACKW:
        {
            BOOL ok = CListView::OnNotify(wParam, lParam, pResult);
            RecalcHeaderTips();
            return ok;
        }
        break;
    }
    
    return CListView::OnNotify(wParam, lParam, pResult);
}
