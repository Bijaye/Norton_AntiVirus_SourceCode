// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
// animation.cpp - created 03/05/1998 6:19:40 PM
//
// $Header:   S:/NAVWORK/VCS/animation.cpv   1.0   09 Mar 1998 23:45:36   DALLEE  $
//
// Description:
//      Implementation of CAnimation. Repair Wizard control for displaying
//      animated images (the siren).
//
// Contains:
//
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/animation.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:36   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
//#include "n_app.h"
#include "Animation.h"
#include "cdcos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimation


////////////////////////////////////////////////////////////////////////////
// Function name	: CAnimation::CAnimation
//
// Description	    : Constructor
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CAnimation::CAnimation()
{
    m_bFirstDraw = TRUE;
    m_iNumCells = 0;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CAnimation::~CAnimation
//
// Description	    : Destrutor
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CAnimation::~CAnimation()
{
}


BEGIN_MESSAGE_MAP(CAnimation, CButton)
	//{{AFX_MSG_MAP(CAnimation)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimation message handlers



////////////////////////////////////////////////////////////////////////////
// Function name	: CAnimation::Initialize
//
// Description	    : 
//
// Return type		: BOOL 
//
// Argument         : UINT uBitmapID        -- resource idenfication
// Argument         : int iNumCells         -- number of animation cells
// Argument         : int iCellWidth        -- Width of a cell
// Argument         : COLORREF crMaskColor  -- Mask color
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CAnimation::Initialize( UINT uBitmapID, int iNumCells, int iCellWidth, COLORREF crMaskColor )
{
    // Create our image list.
    if( m_ImageList.Create( uBitmapID, iCellWidth, 1, crMaskColor ) == FALSE )
        return FALSE;

    // Save off number of cells
    m_iNumCells = iNumCells;

    // Set current frame to 0.
    m_iCurrentFrame = 0;

    return TRUE;
}



////////////////////////////////////////////////////////////////////////////
// Function name	: CAnimation::DrawItem
//
// Description	    : Used to draw our control.
//
// Return type		: void 
//
// Argument         : LPDRAWITEMSTRUCT lpDrawItemStruct
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CAnimation::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    // If this is the first time we have to draw something,
    // set our timer.
    if( m_bFirstDraw )
        {
        m_uTimerID = SetTimer( ANIMATE_TIMER_ID, 100, NULL );
        m_bFirstDraw = FALSE;
        }

    // If we are not initialized, bail out.
    if( m_iNumCells == 0 )
        return;

    // Use offscreen DC.
    CRect rect( &lpDrawItemStruct->rcItem );
    CDCOS dc( CDC::FromHandle( lpDrawItemStruct->hDC ), rect );

    // Fill background.
    CBrush brush;
    brush.CreateSolidBrush( GetSysColor( COLOR_BTNFACE ));
    dc.FillRect( &rect, &brush );
    
    // Draw our bitmap
    POINT pt;
    pt.x = pt.y = 0;
    m_ImageList.Draw( &dc,
                      m_iCurrentFrame,
                      pt,
                      ILD_TRANSPARENT );
}



////////////////////////////////////////////////////////////////////////////
// Function name	: CAnimation::OnDestroy
//
// Description	    : Handles WM_DESTROY message.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CAnimation::OnDestroy() 
{
    // Kill our timer
    KillTimer( ANIMATE_TIMER_ID );

	CButton::OnDestroy();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CAnimation::OnTimer
//
// Description	    : Handles WM_TIMER event.
//
// Return type		: void 
//
// Argument         : UINT nIDEvent
//
////////////////////////////////////////////////////////////////////////////
// 2/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void CAnimation::OnTimer(UINT nIDEvent) 
{
	// We are only concerned with our timer event
    if( nIDEvent != ANIMATE_TIMER_ID )
        {
        CButton::OnTimer(nIDEvent);
        return;
        }
    
    // Adjust frame counter.  
    if( ++m_iCurrentFrame == m_iNumCells )
        m_iCurrentFrame = 0;

    // Force repaint of our window.
    Invalidate(FALSE);
    UpdateWindow();
}
