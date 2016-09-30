// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ImageCheckListBox.cpp : implementation file
//

#include "stdafx.h"
#include "ImageCheckListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageCheckListBox

CImageCheckListBox::CImageCheckListBox() : m_pImageList(NULL)
{
	EnableAutomation();
}

CImageCheckListBox::~CImageCheckListBox()
{
	if (m_pImageList)
		delete m_pImageList;
}

void CImageCheckListBox::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCheckListBox::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CImageCheckListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CImageCheckListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CImageCheckListBox, CCheckListBox)
	//{{AFX_DISPATCH_MAP(CImageCheckListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IImageCheckListBox to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {CFCEB968-2C03-11D0-8A2E-00A0C90D18F8}
static const IID IID_IImageCheckListBox =
{ 0xcfceb968, 0x2c03, 0x11d0, { 0x8a, 0x2e, 0x0, 0xa0, 0xc9, 0xd, 0x18, 0xf8 } };

BEGIN_INTERFACE_MAP(CImageCheckListBox, CCheckListBox)
	INTERFACE_PART(CImageCheckListBox, IID_IImageCheckListBox, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageCheckListBox message handlers

void CImageCheckListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CPoint
		p;
	CDC
		*pdc = CDC::FromHandle(lpDrawItemStruct->hDC);
	CBrush
		brush;
	int
		iOldBkMode = pdc->SetBkMode(TRANSPARENT);
	COLORREF
		oldTextColor;
	char
		szBuffer[256];

	// Draw the graphic and then the text in lParam of drawitemstruct.
	switch (lpDrawItemStruct->itemAction)
	{
#if 0
	case ODS_DRAWENTIRE:
		brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		pdc->FillRect(&lpDrawItemStruct->rcItem, &brush);
		p.x = lpDrawItemStruct->rcItem.left;
		p.y = lpDrawItemStruct->rcItem.top;
		if ( m_pImageList )
			m_pImageList->Draw(pdc, 0, p, ILD_NORMAL);
		// Now draw the itemData

		lpDrawItemStruct->rcItem.left += 16;
		// Make sure that the listbox ctrl has the LBS_HASSTRINGS style bit set. If not, you will
		// crash here.
		GetText(lpDrawItemStruct->itemID, szBuffer);
		pdc->DrawText((LPCTSTR)szBuffer, -1, &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER);
		break;
#endif

	case ODA_FOCUS:
//		if (lpDrawItemStruct->itemState == ODS_FOCUS)
			pdc->DrawFocusRect(&lpDrawItemStruct->rcItem);
		break;

	case ODA_DRAWENTIRE:
	case ODA_SELECT:
		// First paint the background the color of the default
		// selection rectangle.
		if (lpDrawItemStruct->itemState == ODS_SELECTED)
		{
			brush.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
			oldTextColor = pdc->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else
		{
			brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			oldTextColor = pdc->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}
		pdc->FillRect(&lpDrawItemStruct->rcItem, &brush);
		p.x = lpDrawItemStruct->rcItem.left;
		p.y = lpDrawItemStruct->rcItem.top;
		if ( m_pImageList )
			m_pImageList->Draw(pdc, 0, p, ILD_NORMAL);
		// Now draw the itemData

		lpDrawItemStruct->rcItem.left += 16;
		// Make sure that the listbox ctrl has the LBS_HASSTRINGS style bit set. If not, you will
		// crash here.
		GetText(lpDrawItemStruct->itemID, szBuffer);
		pdc->DrawText((LPCTSTR)szBuffer, -1, &lpDrawItemStruct->rcItem,	DT_SINGLELINE | DT_VCENTER);
		pdc->SetTextColor(oldTextColor);
		break;
	}
	// Restore the dc settings.
	pdc->SetBkMode(iOldBkMode);
}

int CImageCheckListBox::SetImageList(CImageList *pImageList)
{
	m_pImageList = pImageList;
	return 0;
}
