/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx() : CListCtrl(), m_iSortOrder(SORT_NONE), m_iSortColumn(-1),
		m_iLastSortColumn(-1), m_hbmpAscending(0), m_hbmpDescending(0), m_hbmpNone(0),
		m_colorButtonFace(0), m_fBmpsCreated(0), m_bClientWidthSel(TRUE), m_cxClient(0),
		m_dwUserStyle(0), m_phStaticText(NULL), m_cyHeader(0), 
		m_pPreCaptureWnd(NULL), m_bCaptured(FALSE)

{
}

CListCtrlEx::~CListCtrlEx()
{
	DeleteObject(m_hbmpAscending);
	DeleteObject(m_hbmpDescending);
	DeleteObject(m_hbmpNone);
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_NOTIFY_REFLECT_EX(LVN_DELETEITEM, OnDeleteitem)
	ON_WM_CREATE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
//	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(WM_POSTCREATE, OnPostCreate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers
/*----------------------------------------------------------------------------
    CMyListCtrl::MakeShortString
    Trim a copy of the strings from DrawItem to fit the currently 
        displayable column width.
 ----------------------------------------------------------------------------*/
LPCTSTR CListCtrlEx::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
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
#if 0

LPCTSTR CListCtrlEx::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
    CString strThreeDots = _T("...");
	int nStringLen = lstrlen(lpszLong);
	if(nStringLen == 0 || pDC->GetTextExtent(lpszLong , nStringLen).cx + nOffset <= nColumnLen)
		return(lpszLong);

	CString strShort(lpszLong);

	int nAddLen = pDC->GetTextExtent(strThreeDots, strThreeDots.GetLength()).cx;

	if ( pDC->GetTextExtent( strShort, 0 ).cx + nOffset + nAddLen > nColumnLen )
		strShort = _T("");
	else
	{
		for(int i = nStringLen - 1; i > 0; i--)
		{
			strShort = strShort.Left(i);
			if(pDC->GetTextExtent(strShort, i).cx + nOffset + nAddLen <= nColumnLen)
				break;
		}
	
		strShort += strThreeDots;
	}

	return((LPCTSTR)strShort);
}
#endif
void CListCtrlEx::DoGenericColumnDraw(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC
		*pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect
		rcItem(lpDrawItemStruct->rcItem);
	UINT
		uiFlags = ILD_TRANSPARENT;
	CImageList 
		*pImageList;
	int
		nItem = lpDrawItemStruct->itemID;
	BOOL
		bFocus = (GetFocus() == this);
	COLORREF
		clrTextSave,
		clrBkSave,
		clrImage = GetSysColor(COLOR_WINDOW);
	static _TCHAR
		szBuff[MAX_PATH];
	LPCTSTR
		pszText;
	CRect
		rcLabel,
		rcAllLabels;

// get item data

//TRACE("\nDrawItem %d", nItem);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff) / sizeof(_TCHAR);
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

// set colors if item is selected

	GetItemRect(nItem,rcAllLabels,LVIR_BOUNDS);
	GetItemRect(nItem,rcLabel,LVIR_LABEL);
	rcAllLabels.left = rcLabel.left;
	if(m_bClientWidthSel && rcAllLabels.right < m_cxClient)
		rcAllLabels.right = m_cxClient;

	if(bSelected && bFocus)
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		pDC->FillRect(rcAllLabels,&CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}
	else if(bSelected && (GetStyle() & LVS_SHOWSELALWAYS))
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));

		pDC->FillRect(rcAllLabels,&CBrush(::GetSysColor(COLOR_BTNFACE)));
	}
	else
		pDC->FillRect(rcAllLabels,&CBrush(GetBkColor()));

// set color and mask for the icon

	if(lvi.state & LVIS_CUT)
	{
		clrImage = GetSysColor(COLOR_WINDOW);
		uiFlags|=ILD_BLEND50;
	}
	else if(bSelected)
	{
		clrImage=::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags|=ILD_BLEND50;
	}

// draw state icon
	LV_COLUMN lvc;
	lvc.mask=LVCF_FMT | LVCF_WIDTH;

	// for now, we hard code to 16 pixel size
	GetColumn( 0, &lvc );
	if ( lvc.cx > 16 )
	{
		UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
		if(nStateImageMask)
		{
			int nImage = (nStateImageMask>>12) - 1;
			pImageList = GetImageList(LVSIL_STATE);
			if(pImageList)
			pImageList->Draw(pDC,nImage,CPoint(rcItem.left,rcItem.top),ILD_TRANSPARENT);
		}

		// draw normal and overlay icon

		CRect rcIcon;
		GetItemRect(nItem,rcIcon,LVIR_ICON);

		pImageList=GetImageList(LVSIL_SMALL);
		if(pImageList)
		{
			UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
			if(rcItem.left<rcItem.right-1)
				ImageList_DrawEx(pImageList->m_hImageList, lvi.iImage, pDC->m_hDC,rcIcon.left, 
					rcIcon.top,16,16, GetBkColor(),	clrImage,uiFlags | nOvlImageMask);
		}
	}

	// draw item label
	GetItemRect(nItem,rcItem,LVIR_LABEL);
	//	rcItem.right-=m_cxStateImageOffset;

	pszText=MakeShortString(pDC,szBuff,rcItem.right-rcItem.left,2*OFFSET_FIRST);

	rcLabel=rcItem;
	rcLabel.left+=OFFSET_FIRST;
	rcLabel.right-=OFFSET_FIRST;

	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_WORD_ELLIPSIS);

	// set original colors if item was selected
	if(bSelected)
	{
        pDC->SetTextColor(clrTextSave);
	}
	pDC->SetBkColor(clrBkSave);
}

void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC
		*pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect
		rcItem(lpDrawItemStruct->rcItem);
	UINT
		uiFlags = ILD_TRANSPARENT;
	int
		nItem = lpDrawItemStruct->itemID;
	BOOL
		bSelected,
		bFocus = (GetFocus() == this);
	static _TCHAR
		szBuff[MAX_PATH];
	LPCTSTR
		pszText;
	CRect
		rcAllLabels,
		rcLabel;
	COLORREF
		clrTextSave,
		clrBkSave;

	// Draw the first column generically. This column is drawn the same regardless of
	// whether or not we need to draw state colunm information (see class CColumnStateListCtrl).
	DoGenericColumnDraw(lpDrawItemStruct);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);
	if(bSelected && bFocus)
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	}
	else if(bSelected && (GetStyle() & LVS_SHOWSELALWAYS))
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
	}

	LV_COLUMN lvc;
	lvc.mask=LVCF_FMT | LVCF_WIDTH;

	GetItemRect(nItem,rcItem,LVIR_LABEL);

	// draw labels for extra columns
	for(int nColumn=1; GetColumn(nColumn,&lvc); nColumn++)
	{
		rcItem.left=rcItem.right;
		rcItem.right+=lvc.cx;

		int nRetLen = GetItemText(nItem,nColumn,szBuff,sizeof(szBuff));
		if (nRetLen == 0)
			continue;

		pszText = MakeShortString(pDC,szBuff,rcItem.right-rcItem.left,2*OFFSET_OTHER);

		UINT nJustify=DT_LEFT;

		if (pszText == szBuff)
		{
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
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

// draw focus rectangle if item has focus

	// set original colors if item was selected
	if(bSelected)
	{
        pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}

	if(lvi.state & LVIS_FOCUSED && bFocus)
	{
		GetItemRect(nItem,rcAllLabels,LVIR_BOUNDS);
		GetItemRect(nItem,rcLabel,LVIR_LABEL);
		rcAllLabels.left = rcLabel.left;
		if(m_bClientWidthSel && rcAllLabels.right < m_cxClient)
			rcAllLabels.right = m_cxClient;

		pDC->DrawFocusRect(rcAllLabels);
	}
}

void CListCtrlEx::RepaintSelectedItems()
{
	CRect
		rcItem,
		rcLabel;

// invalidate focused item so it can repaint properly

	int nItem = GetNextItem(-1,LVNI_FOCUSED);

	if(nItem!=-1)
	{
		GetItemRect(nItem,rcItem,LVIR_BOUNDS);
		GetItemRect(nItem,rcLabel,LVIR_LABEL);
		rcItem.left = rcLabel.left;

		InvalidateRect(rcItem,FALSE);
	}

// if selected items should not be preserved, invalidate them

	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = GetNextItem(-1,LVNI_SELECTED); nItem!=-1; nItem = GetNextItem(nItem,LVNI_SELECTED))
		{
			GetItemRect(nItem,rcItem,LVIR_BOUNDS);
			GetItemRect(nItem,rcLabel,LVIR_LABEL);
			rcItem.left = rcLabel.left;

			InvalidateRect(rcItem,FALSE);
		}
	}

// update changes 

	UpdateWindow();
}

//LRESULT CListCtrlEx::OnSetImageList(WPARAM wParam, LPARAM lParam)
//{
//	if((int)wParam==LVSIL_STATE)
//	{
//		int cx, cy;
//
//		if(::ImageList_GetIconSize((HIMAGELIST)lParam,&cx,&cy))
//			m_cxStateImageOffset=cx;
//		else
//			m_cxStateImageOffset=0;
//	}
//
//	return(Default());
//}

void CListCtrlEx::OnSize(UINT nType, int cx, int cy) 
{
	CListCtrl::OnSize(nType, cx, cy);
	m_cxClient = cx;
	if (m_phStaticText)
	{
		m_phStaticText->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
	}
}


void CListCtrlEx::PreSort(int iSortColumn)
{
	// Only toggle the sort order if the last column is the same as the
	// new column
	m_iLastSortColumn = m_iSortColumn;
	m_iSortColumn = iSortColumn;
	if (iSortColumn == m_iLastSortColumn)
	{
		switch (m_iSortOrder)
		{
		case SORT_NONE:
			m_iSortOrder = SORT_ASCENDING;
			break;

		case SORT_ASCENDING:
			m_iSortOrder = SORT_DESCENDING;
			break;

		case SORT_DESCENDING:
			m_iSortOrder = SORT_ASCENDING;
			break;
		}
	}
	else
		m_iSortOrder = SORT_ASCENDING;

}

CHeaderCtrl *CListCtrlEx::GetHeaderCtrl()
{
	CHeaderCtrl
		*pHeaderCtrl;
	CPoint
		point(2,2);


	// The header control is a child of the ListCtrl, but is not exposed,
	// so we need to find it by getting the child window of the point
	// where we think the control is (in this case 1,1).
	// listview control, we obtain its handle here.
	pHeaderCtrl = (CHeaderCtrl *)ChildWindowFromPoint(point);

	if ((pHeaderCtrl) && (pHeaderCtrl->m_hWnd != this->m_hWnd))
	{
		TCHAR* pszClass = new TCHAR[50];

		// Check the class of the window that we've found to make sure that
		// it really is the header control.
		GetClassName(pHeaderCtrl->m_hWnd, pszClass, 50);
		if (lstrcmp (pszClass, _T("SysHeader32")))
			pHeaderCtrl = NULL;

		delete[] pszClass;
	}
	return pHeaderCtrl;
}

/*---------------------------------------------------------------------------
|	FUNCTION	-	BMPChamelion
|
|	Changes the background of the bitmap passed in to the default windows
|	background color.
---------------------------------------------------------------------------*/
HBITMAP FAR PASCAL BMPChamelion(HDC hdc, HBITMAP hBitmap, COLORREF crBGColor, BOOL ReturnBitmap)
{
	HDC
		hdcTemp,
		hdcSave,
		hdcMem,					  
		hdcObject,
		hdcBack;
	HBRUSH
		hBrush;
	RECT
		rect;
	POINT	
		ptSize;
	COLORREF
		cColor;
	BITMAP
		bm;
	HBITMAP
		bmAndBack,
		bmAndObject,
		bmAndMem,
		bmBackOld,
		bmTempOld,
		bmAndTemp,
		bmObjectOld,
		bmSaveOld,
		bmMemOld;


	// create some DCs to hold temporary data
	hdcBack = CreateCompatibleDC (hdc);
	hdcObject = CreateCompatibleDC (hdc);
	hdcMem = CreateCompatibleDC (hdc);
	hdcTemp = CreateCompatibleDC (hdc);
	hdcSave = CreateCompatibleDC (hdc);

	bmSaveOld = (HBITMAP)SelectObject(hdcSave, hBitmap);    // Select the bitmap

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;				  // Get width of bitmap
	ptSize.y = bm.bmHeight;				  // Get height of bitmap
	DPtoLP(hdcTemp, &ptSize, 1);		  // Convert from device to logical points
	
	// two monochrome DCs
	bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	bmAndMem = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmAndTemp = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	// Each DC must select a bitmap object to store pixel data
	bmBackOld = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmTempOld = (HBITMAP)SelectObject(hdcTemp, bmAndTemp);

	// Set proper mapping mode.
	SetMapMode(hdcTemp, GetMapMode(hdc));

	// Make copy of original bitmap
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	// Set the background color of the source DC to the color
	// contained in the parts of the bitmap that should be changed
	cColor = SetBkColor(hdcTemp, 0x0000FF00L);

	// Create the object mask for the bitmap by performing a BitBlt
	// from the source bitmap to a monochrome bitmap.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	// Set the background color of the source DC back to the original color.
	SetBkColor(hdcTemp, cColor);

	// Create the inverse of the object mask.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);


	// Use color passed to assume the background on the finished bmp in hdcMem
	GetObject(bmAndMem, sizeof(BITMAP), (LPSTR)&bm);

	hBrush = CreateSolidBrush(crBGColor);
	
	rect.top = 0;
	rect.left = 0;
	rect.right = bm.bmWidth;				  // Get width of bitmap
	rect.bottom = bm.bmHeight;				  // Get height of bitmap

	DPtoLP(hdcMem,(POINT *) &rect.right, 1);	// Convert from device to logical points
	FillRect(hdcMem, &rect, hBrush);


	// mask out the places where the bitmap will be placed.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

	// mask out background on bitmap copy
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

	// OR the bitmap with the background on the destination DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);


	if (!ReturnBitmap)		 		/* modify original bitmap */
		BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcBack, bmBackOld);
	SelectObject(hdcObject, bmObjectOld);
	SelectObject(hdcTemp, bmTempOld);
	SelectObject(hdcSave, bmSaveOld);
	SelectObject(hdcMem, bmMemOld);
	/* bmAndMem is what was set into here */

	// delete the memory bitmaps.

	DeleteObject(bmAndBack);
	DeleteObject(bmAndObject);
	DeleteObject(bmAndTemp);

	DeleteObject(hBrush);


	// delete the memory DCs.
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcTemp);
	DeleteDC(hdcSave);
	if (ReturnBitmap)
		return (bmAndMem);
	else
		DeleteObject(bmAndMem);
	return 0;
}	/*--- END FUNCTION:	BMPChamelion ---*/

void CListCtrlEx::PostSort()
{
	// Put a little notification bitmap on the column header to let user
	// know what direction the sort is taking place.

	if (m_iLastSortColumn != -1 && m_iLastSortColumn != m_iSortColumn)
		// Clear the bitmap from the old column
		SetHeaderBmp(m_iLastSortColumn, m_hbmpNone);

	SetHeaderBmp(m_iSortColumn, m_iSortOrder == SORT_ASCENDING ? m_hbmpAscending : m_hbmpDescending);
}

BOOL CListCtrlEx::SortItems( PFNLVCOMPARE pfnCompare, DWORD dwData, int iSortColumn )
{
	PreSort(iSortColumn);
	CListCtrl::SortItems(pfnCompare, dwData);
	PostSort();
	return 0;
}

int CListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (lpCreateStruct)
	{
		if (CListCtrl::OnCreate(lpCreateStruct) == -1)
			return -1;
	}

	// Create the font to be used for the unline selection stuff.
	CFont* pFont = GetFont();
	LOGFONT lf;
	BYTE oldValue;

	pFont->GetLogFont( &lf );

	VERIFY( m_font.CreateFontIndirect( &lf ) );

	oldValue = lf.lfUnderline;
	lf.lfUnderline = TRUE;
	VERIFY( m_underlineFont.CreateFontIndirect( &lf ) );
	lf.lfUnderline = oldValue;

	PostMessage(WM_POSTCREATE, 0, 0);
	return 0;
}

BOOL CListCtrlEx::SetHeaderBmp(int nCol, HBITMAP hBmp)
{
	HD_ITEM
		item;
	TCHAR
		*pszTitle = new TCHAR[256];
	CHeaderCtrl
		*pHeaderCtrl = GetHeaderCtrl();
	BOOL
		bRet = 0;

	if (pHeaderCtrl)
	{
		// Add the bitmap for NONE to every control for starters.
		item.mask = HDI_FORMAT | HDI_BITMAP | HDI_FORMAT | HDI_LPARAM |
				HDI_TEXT | HDI_WIDTH | HDI_IMAGE;

		item.cchTextMax = 256;
		item.pszText = pszTitle;

		// Set the bitmap for the new column
		pHeaderCtrl->GetItem(nCol, &item);

		// Add the bitmap flag, and a handle to the bitmap
		item.hbm = hBmp;
		item.fmt |= HDF_BITMAP;
		bRet = pHeaderCtrl->SetItem(nCol, &item);
	}
	delete[] pszTitle;
	return bRet;
}

int CListCtrlEx::InsertColumn(int nCol, const LV_COLUMN* pColumn)
{
	int
		rv;

	if (!m_fBmpsCreated)
	{
		OnPostCreate(0, 0);
	}

	// First add the column to the ListCtrl. Then modify it to have
	// the bitmap
	rv = CListCtrl::InsertColumn(nCol, pColumn);

	if (rv != -1)
		SetHeaderBmp(nCol, m_hbmpNone);
	return rv;
}

int CListCtrlEx::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	int
		rv;

	if (!m_fBmpsCreated)
	{
		OnPostCreate(0, 0);
	}

	// First add the column to the ListCtrl. Then modify it to have
	// the bitmap
	rv = CListCtrl::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);

	if (rv != -1)
		SetHeaderBmp(nCol, m_hbmpNone);
	return rv;
}



void CListCtrlEx::ForwardSysColorChange()
{
	CHeaderCtrl
		*pHeaderCtrl = GetHeaderCtrl();
	
	if (pHeaderCtrl)
	{
		HDC
			hdc;
		COLORREF
			bkColor;
		int
			i,
			iColumnCount;
		HBITMAP
			// Use these to keep track of the existing bitmaps so we know what to switch
			// the column bitmaps to.
			hbmpLastAscending,
			hbmpLastDescending,
			hbmpLastNone;


		iColumnCount = pHeaderCtrl->GetItemCount();
		m_colorButtonFace = bkColor = GetSysColor(COLOR_BTNFACE);

		// Reconvert the bitmaps to the new background color
		hdc = ::GetDC(pHeaderCtrl->m_hWnd);
		hbmpLastAscending = m_hbmpAscending;
		hbmpLastDescending = m_hbmpDescending;
		hbmpLastNone = m_hbmpNone;

		// We need to delete and reload the old bitmaps so that we can re-determine the new
		// background color;
		DeleteObject(m_hbmpAscending);
		DeleteObject(m_hbmpDescending);
		DeleteObject(m_hbmpNone);
		m_hbmpAscending = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ASCENDING));
		m_hbmpDescending = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DESCENDING));
		m_hbmpNone = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_NONE));
		BMPChamelion(hdc, m_hbmpAscending, bkColor, FALSE);
		BMPChamelion(hdc, m_hbmpDescending, bkColor, FALSE);
		BMPChamelion(hdc, m_hbmpNone, bkColor, FALSE);
		::ReleaseDC(pHeaderCtrl->m_hWnd, hdc);

		for (i = 0; i < iColumnCount; i++)
		{
			HD_ITEM
				item;

			// Replace the bitmap in the headerctrl with the newly changed
			// bitmap
			item.mask = HDI_BITMAP;

			// Set the bitmap for the new column
			pHeaderCtrl->GetItem(i, &item);

			// Add the bitmap flag, and a handle to the bitmap
			if (item.hbm == hbmpLastAscending)
				SetHeaderBmp(i, m_hbmpAscending);
			else if (item.hbm == hbmpLastDescending)
				SetHeaderBmp(i, m_hbmpDescending);
			else
				SetHeaderBmp(i, m_hbmpNone);
		}
		pHeaderCtrl->InvalidateRect(NULL, FALSE);
	}
	PostMessage(WM_SYSCOLORCHANGE, 0, 0);
}


BOOL CListCtrlEx::HiliteItem(CPoint &point, BOOL force /* = FALSE */, BOOL bUrgent /* = FALSE */)
{
	static int hLastItem = -1;
	int hItem;
	UINT itemFlags;
	BOOL rc = TRUE;

	hItem = HitTest( point, &itemFlags );
	if (/*!m_bCaptured || */( hItem == -1) )
	{
		if ( hLastItem != -1 )
		{
			CRect
				rc,
				rcListCtrl;

			GetClientRect(&rcListCtrl);
			GetItemRect( hLastItem, &rc, LVIR_LABEL );
			// Force the repaint to go all the way to the right edge of the client.
			rc.right = rcListCtrl.right;
			InvalidateRect( &rc, FALSE);	
			hLastItem = -1;
		}
		goto cleanup;
	}

	if( ( hLastItem == hItem ) && !force )
	{
		goto cleanup;
	}

	if( (hLastItem != hItem) && ( hLastItem != -1 ) )
	{
		CRect
			rc,
			rcListCtrl;
	
		GetClientRect(&rcListCtrl);
		GetItemRect( hLastItem, &rc, LVIR_LABEL );
		// Force the repaint to go all the way to the right edge of the client.
		rc.right = rcListCtrl.right;
		InvalidateRect( &rc, FALSE);

		// Fixed severnal paint problems by not calling updatewindow here.
//		UpdateWindow();
	}
	
	if ( itemFlags & LVHT_ONITEM )
	{
		CRect rc;
		CRect rcItem;
		CDC *pdc = GetDC();
		COLORREF colorText;
		int bkMode;
		CFont* pOldFont;
		LV_ITEM
			item;
		TCHAR
			*pText = new TCHAR[256];
		LPCTSTR
			pszText;

		item.mask = LVIF_TEXT;
		item.iItem = hItem;
		item.iSubItem = 0;
		item.cchTextMax = 256;
		item.pszText = pText;
		GetItem(&item);

		VERIFY( GetItemRect( hItem, &rc, LVIR_LABEL ) );
		if (GetItemState(hItem, LVIS_SELECTED) == LVIS_SELECTED)
		{
			colorText = pdc->SetTextColor( 
						(COLORREF)GetSysColor( COLOR_HIGHLIGHTTEXT ) );
		} else {
			colorText = pdc->SetTextColor( 
						(COLORREF)GetSysColor( COLOR_HIGHLIGHT ) );
		}

		pOldFont = pdc->SelectObject( &m_underlineFont );
		bkMode = pdc->SetBkMode( TRANSPARENT );

		pszText = MakeShortString(pdc, pText, rc.right - rc.left, 2 * OFFSET_FIRST);

		rcItem = rc;
		rcItem.left+=OFFSET_FIRST; // Draw this text in the same place that we're drawing the ownerdraw text.
		rcItem.right-=OFFSET_FIRST;

		VERIFY( pdc->DrawText( pszText, -1, &rcItem, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER) );

		// Now draw the column text
		LV_COLUMN lvc;
		lvc.mask=LVCF_FMT | LVCF_WIDTH;
		TCHAR
			szBuff[256];

		for(int nColumn=1; GetColumn(nColumn,&lvc); nColumn++)
		{
			rc.left = rc.right;
			rc.right += lvc.cx;

			int nRetLen = GetItemText(hItem,nColumn,szBuff,sizeof(szBuff));
			if(nRetLen==0) continue;

			pszText = MakeShortString(pdc,szBuff,rc.right - rc.left, 2 * OFFSET_OTHER);

			UINT nJustify=DT_LEFT;

			if(pszText == szBuff)
			{
				switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
				{
				case LVCFMT_RIGHT:
					nJustify=DT_RIGHT;
					break;
				case LVCFMT_CENTER:
					nJustify=DT_CENTER;
					break;
				default:
					break;
				}
			}

			rcItem = rc;
			rcItem.left+=OFFSET_OTHER;
			rcItem.right-=OFFSET_OTHER;

			pdc->DrawText(pszText,-1,rcItem, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
		}

		pdc->SelectObject( pOldFont );
		pdc->SetBkMode( bkMode );
		pdc->SetTextColor( colorText );
		delete[] pText;
		ReleaseDC( pdc );
		hLastItem = hItem;
	}
	else
	{
		hLastItem = -1;
	}

	rc = TRUE;

cleanup:
	return rc;
}

void CListCtrlEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetUserStyle() & LVS_PRESELECTION)
	{
		CHeaderCtrl*	pHeader = GetHeaderCtrl();
		CRect			rect;
		POINT			pt;

		GetClientRect(&rect);
		ClientToScreen(&rect);
		GetCursorPos(&pt); 
		if(pHeader)
		{
			CRect headerRect;

			pHeader->GetWindowRect(&headerRect);
			if(headerRect.PtInRect(pt))
			{
				point.y = -1;
				point.x = -1;
			}
			rect.top = headerRect.bottom;
		}

		if(m_bCaptured && !rect.PtInRect(pt))
		{
			ReleaseCapture();
			m_bCaptured = FALSE;
			point.y = -1;
			point.x = -1;
		}
		else if(!m_bCaptured)
		{
			SetCapture();
			m_bCaptured = TRUE;
		}

		HiliteItem(point);
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

void CListCtrlEx::ModifyUserStyle(DWORD dwClear, DWORD dwSet)
{
	m_dwUserStyle |= dwSet;
	m_dwUserStyle &= ~dwClear;
}

DWORD CListCtrlEx::GetUserStyle()
{
	return m_dwUserStyle;
}

long CListCtrlEx::OnPostCreate(WPARAM wParam, LPARAM lParam)
{
	CHeaderCtrl
		*pHeaderCtrl = GetHeaderCtrl();

	if (!m_fBmpsCreated)
	{	
		if (pHeaderCtrl)
		{
			HDC
				hdc;
			COLORREF
				bkColor;

			m_colorButtonFace = bkColor = GetSysColor(COLOR_BTNFACE);

			// Load the bitmaps for the ascending/descending notification.
			hdc = ::GetDC(pHeaderCtrl->m_hWnd);
			m_hbmpAscending = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ASCENDING));
			m_hbmpDescending = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DESCENDING));
			m_hbmpNone = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_NONE));
			BMPChamelion(hdc, m_hbmpAscending, bkColor, FALSE);
			BMPChamelion(hdc, m_hbmpDescending, bkColor, FALSE);
			BMPChamelion(hdc, m_hbmpNone, bkColor, FALSE);
			::ReleaseDC(pHeaderCtrl->m_hWnd, hdc);
			m_fBmpsCreated = TRUE;
		}
	}
	return 0l;
}

void CListCtrlEx::OnPaint() 
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if(m_bClientWidthSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
	{
		CRect rcAllLabels;
		GetItemRect(0,rcAllLabels,LVIR_BOUNDS);

		if(rcAllLabels.right<m_cxClient)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left=min(rcAllLabels.right-1,rcClip.left);
			rcClip.right=m_cxClient;

			InvalidateRect(rcClip,FALSE);
			// EndPaint will be called in CPaintDC d-tor
		}
	}

	CListCtrl::OnPaint();
}

void CListCtrlEx::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);

	if ( m_bCaptured )
	{
		CRect rcListCtrl;
		CRect rc;
		CPoint point;
		m_bCaptured = FALSE;
		ReleaseCapture();
		if ( m_pPreCaptureWnd )
			m_pPreCaptureWnd->SetCapture();
		m_pPreCaptureWnd = NULL;
		HiliteItem( point );
	}

	// check if we are losing focus to label edit box
	if(pNewWnd!=NULL && pNewWnd->GetParent()==this)
		return;

	// repaint items that should change appearance
	if((GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}


// This function is used to set static text in the area that the listctrl
// is occupying. It does this by creating a child CWnd and handling the paint message for that
// window to paint the static text.
int CListCtrlEx::ClearStaticText()
{
	if (m_phStaticText)
	{
		m_phStaticText->DestroyWindow();
		delete m_phStaticText;
		m_phStaticText = NULL;
	}
	return 0;
}

int CListCtrlEx::SetStaticText(DWORD idStr)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString str((LPCTSTR)idStr);
	return (SetStaticText(str));
}

int CListCtrlEx::SetStaticText(CString str)
{
	CRect
		rect;

	GetClientRect(rect);
	if (m_phStaticText == NULL)
	{
TRACE(_T("\nCreate the static text window with '%s'"), str);
		CHeaderCtrl
			*pHeader = GetHeaderCtrl();
		
		if (pHeader)
		{
			RECT
				r;

			pHeader->GetClientRect(&r);
			m_cyHeader = r.bottom;
		}

		rect.top += m_cyHeader;

		// The window isn't created yet. Create it first.
		m_phStaticText = new CStaticTextWnd;
		m_phStaticText->Create(NULL, str, WS_CHILD | WS_VISIBLE | WS_DISABLED, rect, this, 2);
	}
	else
	{
TRACE(_T("\nUpdate the static text window with '%s'"), str);
		m_phStaticText->SetWindowText(str);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CStaticTextWnd

CStaticTextWnd::CStaticTextWnd()
{
}

CStaticTextWnd::~CStaticTextWnd()
{
}


BEGIN_MESSAGE_MAP(CStaticTextWnd, CWnd)
	//{{AFX_MSG_MAP(CStaticTextWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStaticTextWnd message handlers

void CStaticTextWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CString
		str;
	RECT
		rect;
	HFONT
		hFont = (HFONT)GetParent()->GetFont();
	CFont
		*pOldFont,
		*pFont;
	CBrush
		brGray,
		brWindow;
	COLORREF
		fg = GetSysColor(COLOR_GRAYTEXT),
		bk = GetSysColor(COLOR_WINDOW),
		bkSave,
		fgSave;

	GetClientRect(&rect);
	pFont = CFont::FromHandle(hFont);
	brWindow.CreateSolidBrush(bk);
	brGray.CreateSolidBrush(fg);
	dc.FillRect(&rect, &brWindow);

	fgSave = dc.SetTextColor(fg);
	bkSave = dc.SetBkColor(bk);
	pOldFont = (CFont *)dc.SelectObject(pFont);
	GetWindowText(str);
	dc.DrawText(str, &rect, DT_CENTER | DT_WORDBREAK);

	dc.SelectObject(pOldFont);
	dc.SetTextColor(fgSave);
	dc.SetBkColor(bkSave);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CListCtrlEx::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// This code paints the row COLOR_WINDOW when in item is deleted because
	// windows only repaints the columns and not any non column on the end
	// that we painted ourselves.

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CRect	rcLabel, rcAllLabels;
	CDC		*pDC = GetDC();

	GetItemRect(pNMListView->iItem, rcAllLabels, LVIR_BOUNDS);
	GetItemRect(pNMListView->iItem, rcLabel, LVIR_LABEL);
	rcAllLabels.left = rcLabel.left;
	if(m_bClientWidthSel && rcAllLabels.right < m_cxClient)
		rcAllLabels.right = m_cxClient;

	pDC->FillRect(rcAllLabels,&CBrush(GetSysColor(COLOR_WINDOW)));

	ReleaseDC(pDC);
	
	*pResult = TRUE;
	return TRUE;
}

void CListCtrlEx::OnCaptureChanged(CWnd *pWnd) 
{
	if(pWnd == this)
	{
		CPoint	pt;

		m_bCaptured = FALSE;
		pt.y = -1;
		pt.x = -1;

		HiliteItem(pt);
	}

	CListCtrl::OnCaptureChanged(pWnd);
}
