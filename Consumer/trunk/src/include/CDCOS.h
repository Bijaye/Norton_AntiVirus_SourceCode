////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
// Chris Brown 11/24/2004
//******************************************************************************

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CDCOS : public CDC
{
public:

    CDCOS::CDCOS(CDC* pDC, CRect& rect)
    {
        m_rect = rect;
        m_pOrigDC = pDC;
        m_cBitmap.CreateCompatibleBitmap(pDC->m_hDC, rect.Width(), rect.Height());
        CreateCompatibleDC(pDC->m_hDC);
        m_OldBitmap = SelectBitmap( m_cBitmap.m_hBitmap );
    }

    CDCOS::CDCOS(CDC* pDC, CRect& rectBitmap, CRect& rectPaint)
    {
        m_rect = rectPaint;
        m_pOrigDC = pDC;
        m_cBitmap.CreateCompatibleBitmap(pDC->m_hDC, rectBitmap.Width(), rectBitmap.Height());
        CreateCompatibleDC(pDC->m_hDC);
        m_OldBitmap = SelectBitmap( m_cBitmap.m_hBitmap );
    }

    CDCOS::~CDCOS()
    {
        MoveOnScreen(); 
        SelectBitmap( m_OldBitmap.m_hBitmap ); 
    }

protected:

    CBitmap m_cBitmap;
    CBitmapHandle m_OldBitmap;
    CDC* m_pOrigDC;
    CRect m_rect;

    void CDCOS::MoveOnScreen(void)
    {
        m_pOrigDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_hDC, m_rect.left,m_rect.top,SRCCOPY);
    }

};





