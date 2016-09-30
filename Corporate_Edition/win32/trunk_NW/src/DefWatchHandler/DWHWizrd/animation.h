// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/animation.h_v   1.0   09 Mar 1998 23:47:44   DALLEE  $
//
// Description:
//      Declaration of CAnimation. Repair Waizard control for displaying 
//      animated images.
//
// Contains:
//      CAnimation()
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/animation.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:47:44   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __RWANIM_H
#define __RWANIM_H

/////////////////////////////////////////////////////////////////////////////
// CAnimation window

class CAnimation : public CButton
{
// Construction
public:
	CAnimation();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimation)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimation();

    BOOL Initialize( UINT uBitmapID, int iNumCells, int iCellWidth, COLORREF crMaskColor );

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimation)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
    // Image list for frames of animation
    CImageList m_ImageList;

    // Number of cells
    int m_iNumCells;
    
    // Current frame to draw
    int m_iCurrentFrame;

    // Timer ID
    UINT m_uTimerID;

    // First draw item flag.
    BOOL m_bFirstDraw;
};

/////////////////////////////////////////////////////////////////////////////
#define ANIMATE_TIMER_ID    1


#endif // !__RWANIM_H
