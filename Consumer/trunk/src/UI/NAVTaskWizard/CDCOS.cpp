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
// $Header:   S:/NAVWORK/VCS/cdcos.cpv   1.0   09 Mar 1998 23:45:36   DALLEE  $
//
//
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   03-12-96 01:52:48pm
//////////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVWORK/VCS/cdcos.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:36   DALLEE
// Initial revision.
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cdcos.h"

CDCOS::CDCOS(CDC* pDC, CRect& rect)
{
	m_rect = rect;
	m_pOrigDC = pDC;
	m_cBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CreateCompatibleDC(pDC);
	m_pOldBitmap = SelectObject(&m_cBitmap);
}

CDCOS::CDCOS(CDC* pDC, CRect& rectBitmap, CRect& rectPaint)
{
	m_rect = rectPaint;
	m_pOrigDC = pDC;
	m_cBitmap.CreateCompatibleBitmap(pDC, rectBitmap.Width(), rectBitmap.Height());
	CreateCompatibleDC(pDC);
	m_pOldBitmap = SelectObject(&m_cBitmap);
}

void CDCOS::MoveOnScreen(void)
{
	m_pOrigDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
					  this, m_rect.left,m_rect.top,SRCCOPY);
}


CDCOS::~CDCOS()
{
    MoveOnScreen(); 
    SelectObject(m_pOldBitmap); 
}
