/* Copyright 1996 Symantec, Peter Norton Computing Group                */
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
