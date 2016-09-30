/* Copyright 1996 Symantec, Peter Norton Computing Group                */
//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/QCONSOLE/VCS/Cdcos.h_v   1.0   27 Feb 1998 15:10:22   DBuches  $
//
//
//
//////////////////////////////////////////////////////////////////////////
// Created by: DBUCHES   04-15-96 01:52:48pm
//////////////////////////////////////////////////////////////////////////
/* $Log:   S:/QCONSOLE/VCS/Cdcos.h_v  $ */
// 
//    Rev 1.0   27 Feb 1998 15:10:22   DBuches
// Initial revision.
// 
//    Rev 1.0   22 Nov 1997 00:30:12   DBuches
// Initial revision.
// 
//    Rev 1.1   16 Jul 1996 19:58:50   DLEVITO
// Moved destructor into .cpp file.
// 
//    Rev 1.0   15 Apr 1996 15:10:34   DBUCHES
// Initial revision


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
