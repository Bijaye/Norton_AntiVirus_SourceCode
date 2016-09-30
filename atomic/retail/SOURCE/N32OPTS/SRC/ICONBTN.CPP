/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/IconBtn.cpv   1.0   25 Jun 1997 15:14:36   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
// IconBtn.cpp : implementation file
//
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/IconBtn.cpv  $
// 
//    Rev 1.0   25 Jun 1997 15:14:36   DBUCHES
// Initial revision.
//

#include "stdafx.h"
#include "n32opts.h"
#include "IconBtn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconButton

CIconButton::CIconButton()
{
    m_hIcon = NULL;
}

CIconButton::~CIconButton()
{
    DeleteObject(m_hIcon);
}


BEGIN_MESSAGE_MAP(CIconButton, CButton)
	//{{AFX_MSG_MAP(CIconButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconButton message handlers

BOOL CIconButton::SetIconID(int iID)
{
    // Delete old icon
    DeleteObject(m_hIcon);

    // Load icon image
    m_hIcon = (HICON)LoadImage(AfxGetInstanceHandle(),
                            MAKEINTRESOURCE(iID),
                            IMAGE_ICON,
                            16,
                            16,
                            LR_DEFAULTCOLOR);

    return (BOOL) m_hIcon != NULL;
}



void CIconButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	
	int iIconWidth = GetSystemMetrics(SM_CXSMICON);
    int iIconHeight = GetSystemMetrics(SM_CYSMICON);
	int iOffset = 0;
	int iXEdge = 0;
	int iYEdge = 0;

    // Get DC
    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    // Get Rect
    CRect rect = lpDrawItemStruct->rcItem;

    // Get brush    
    CBrush brush( GetSysColor(COLOR_BTNFACE) );

    // Paint background
    dc.FillRect(rect, &brush);

    // Draw button frame
	BOOL bPushed = (BOOL) (lpDrawItemStruct->itemState & ODS_SELECTED);
    dc.DrawEdge(rect, bPushed ? EDGE_SUNKEN : EDGE_RAISED, BF_RECT);

    // Shrink rect to handle border    
	iOffset = bPushed;
    iXEdge = GetSystemMetrics(SM_CXEDGE);
	iYEdge = GetSystemMetrics(SM_CYEDGE);
	rect.InflateRect(-iXEdge, -iYEdge);

    // Draw focus rect 
    if(lpDrawItemStruct->itemState & ODS_FOCUS)
	    {
	    dc.DrawFocusRect(rect);
	    }

    // Compute location for icon
    rect = lpDrawItemStruct->rcItem;
    int xPos = (rect.Width() - iIconWidth) / 2 + iOffset;
    int yPos = (rect.Height() - iIconHeight) / 2 + iOffset;

    // Draw icon
    DrawIconEx(  lpDrawItemStruct->hDC, 
                 xPos,
                 yPos,
                 m_hIcon,
                 iIconWidth,
                 iIconHeight,
                 0,
                 brush,
                 DI_NORMAL);
    
    // Cleanup
    dc.Detach();
}
