/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/ImageButton.cpv   1.1   06 Mar 1998 17:16:42   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ImageButton.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/ImageButton.cpv  $
// 
//    Rev 1.1   06 Mar 1998 17:16:42   DBuches
// Switched to offscreen DC.
// 
//    Rev 1.0   27 Feb 1998 15:08:58   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "ImageButton.h"
#include "cdcos.h"
#include "ThemeBackground.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageButton

CImageButton::CImageButton()
{
    m_iIndex = 0;
}

CImageButton::~CImageButton()
{
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	//{{AFX_MSG_MAP(CImageButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageButton message handlers



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CImageButton::SetImageList
// Description	    : Sets the image list for this contrl
// Return type		: void 
// Argument         : CImageList * pImageList
//
////////////////////////////////////////////////////////////////////////////////
// 2/16/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CImageButton::SetImageList(CImageList * pImageList)
{
    m_pImageList = pImageList;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CImageButton::SetImageIndex
// Description	    : Sets the image list index for this control
// Return type		: void 
// Argument         : int iIndex
//
////////////////////////////////////////////////////////////////////////////////
// 2/16/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CImageButton::SetImageIndex(int iIndex)
{
    m_iIndex = iIndex;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CImageButton::DrawItem
// Description	    : Draw the control
// Return type		: void 
// Argument         : LPDRAWITEMSTRUCT lpDrawItemStruct
//
////////////////////////////////////////////////////////////////////////////////
// 2/16/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    // Get DC for drawing. 
    CRect rect( &lpDrawItemStruct->rcItem );
    CDCOS dc( CDC::FromHandle( lpDrawItemStruct->hDC ), rect );

	CThemeBackground theme;;
	if(theme.ThemeAvailable())
	{
		theme.PaintThemeBackground( GetSafeHwnd(), dc.GetSafeHdc(), &rect );
	}
	else
	{
		// Fill with button face color
		dc.FillSolidRect( &rect, GetSysColor( COLOR_BTNFACE ) );		
	}

    // Draw image
    CPoint pt(0,0);
    m_pImageList->Draw( &dc, m_iIndex, pt, ILD_TRANSPARENT );
}
