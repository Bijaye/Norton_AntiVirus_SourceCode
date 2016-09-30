// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/picture.cpv   1.0   09 Mar 1998 23:45:40   DALLEE  $
//
// Description:
//      Implementation of CPicture. Repair Wizard page picture display control.
//
// Contains:
//
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/picture.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:40   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
//#include "n_app.h"
#include "Picture.h"
#include "cdcos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicture

CPicture::CPicture()
{
}

CPicture::~CPicture()
{
}


BEGIN_MESSAGE_MAP(CPicture, CButton)
	//{{AFX_MSG_MAP(CPicture)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicture message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CPicture::DrawItem
//
// Description	    : Handles WM_DRAWITEM message
//
// Return type		: void 
//
// Argument         : LPDRAWITEMSTRUCT lpDrawItemStruct
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CPicture::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// Use offscreen DC.
    CRect rect( &lpDrawItemStruct->rcItem );
    CDCOS dc( CDC::FromHandle( lpDrawItemStruct->hDC ), rect );

    // Load bitmap into scratch DC
    CDC scratchDC;
    CBitmap* pOldBitmap;
    scratchDC.CreateCompatibleDC( &dc );

    pOldBitmap = scratchDC.SelectObject( &m_bitmap );

    // Stretch bitmap to our DC.
    dc.StretchBlt( 0, 
                   0, 
                   rect.Width(),
                   rect.Height(),
                   &scratchDC,
                   0,
                   0,
                   m_iBitmapWidth,
                   m_iBitmapHeight,
                   SRCCOPY );

    // Draw frame
    if( m_bDrawFrame )
        dc.DrawEdge( &rect, EDGE_SUNKEN, BF_RECT );

    // Cleanup
    scratchDC.SelectObject( pOldBitmap );
	
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CPicture::SetBitmapID
//
// Description	    : Sets the correct bitmap ID
//
// Return type		: BOOL 
//
// Argument         : int iID
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CPicture::SetBitmap(int iID, int iWidth, int iHeight, BOOL bDrawFrame /* = TRUE */)
{
    // Delete old bitmap object.
    m_bitmap.DeleteObject();

    // Load new one
    BOOL bRet = m_bitmap.LoadBitmap( iID );
    m_iBitmapWidth = iWidth;
    m_iBitmapHeight = iHeight;
    m_bDrawFrame = bDrawFrame;

    // Force repaint of our window.
    Invalidate(FALSE);
    UpdateWindow();

    return bRet;
}

