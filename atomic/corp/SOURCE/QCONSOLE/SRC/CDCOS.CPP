/* Copyright 1996 Symantec, Peter Norton Computing Group                */
//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/QCONSOLE/VCS/Cdcos.cpv   1.0   27 Feb 1998 15:08:56   DBuches  $
//
//
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   03-12-96 01:52:48pm
//////////////////////////////////////////////////////////////////////////
/* $Log:   S:/QCONSOLE/VCS/Cdcos.cpv  $ */
// 
//    Rev 1.0   27 Feb 1998 15:08:56   DBuches
// Initial revision.
// 
//    Rev 1.0   22 Nov 1997 00:29:24   DBuches
// Initial revision.
// 
//    Rev 1.1   16 Jul 1996 19:58:48   DLEVITO
// Moved destructor into .cpp file.
// 
//    Rev 1.0   15 Apr 1996 15:09:06   DBUCHES
// Initial revision
// 
//    Rev 1.0   18 Mar 1996 14:55:36   DBUCHES
// Initial revision.
// 
//    Rev 1.0   12 Mar 1996 14:26:08   DBUCHES
// Initial revision.
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
