////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVWORK/VCS/cdcos.h_v   1.0   09 Mar 1998 23:47:46   DALLEE  $
//
//
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   04-15-96 01:52:48pm
//////////////////////////////////////////////////////////////////////////
/* $Log:   S:/NAVWORK/VCS/cdcos.h_v  $ */
// 
//    Rev 1.0   09 Mar 1998 23:47:46   DALLEE
// Initial revision.
//////////////////////////////////////////////////////////////////////////

#ifndef AFX_CDCOS_H__F48DE345_D474_11D2_8D3B_EFF7DE59A93E__INCLUDED_
#define AFX_CDCOS_H__F48DE345_D474_11D2_8D3B_EFF7DE59A93E__INCLUDED_

class CDCOS : public CDC
	{
	private:
		CBitmap		m_cBitmap;
		CBitmap*	m_pOldBitmap;
		CDC*		m_pOrigDC;
		CRect		m_rect;

	public:
		CDCOS(CDC* pDC, CRect& rect);
		CDCOS(CDC* pDC, CRect& rectBitmap, CRect& rectPaint);
		~CDCOS();

		void MoveOnScreen(void);
	};

#endif  // ndef AFX_CDCOS_H__F48DE345_D474_11D2_8D3B_EFF7DE59A93E__INCLUDED_