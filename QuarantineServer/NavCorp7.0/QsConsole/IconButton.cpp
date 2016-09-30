// IconButton.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "IconButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconButton

CIconButton::CIconButton() :
m_hIcon( NULL )
{
}

CIconButton::~CIconButton()
{
}


BEGIN_MESSAGE_MAP(CIconButton, CButton)
	//{{AFX_MSG_MAP(CIconButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconButton message handlers


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CIconButton::DrawItem
//
// Description   : Draws the actual control.
//
// Return type   : void 
//
// Argument      : LPDRAWITEMSTRUCT lpDrawItemStruct
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CIconButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    // 
    // Get DC for drawing.
    // 
    CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	
    // 
    // Get rectangle
    // 
    CRect rect = lpDrawItemStruct->rcItem;

    // 
    // Fill with background color
    // 
    pDC->FillSolidRect( &rect, GetSysColor( COLOR_BTNFACE ) );

    // 
    // Draw icon 
    // 
    pDC->DrawIcon( 0,0, m_hIcon );

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CIconButton::SetIcon
//
// Description   : Sets the desired icon for drawing.
//
// Return type   : void 
//
// Argument      : HICON hIcon
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CIconButton::SetIcon(HICON hIcon)
{
    m_hIcon = hIcon;
}
