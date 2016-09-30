////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// AUTHOR(S)     : Shaun Cooley, 
//               : Michael M. Welch (pause capability)
// COMPILED WITH : Microsoft Visual C++ .Net (v7.0)
// MODEL         : Flat (32bit)
// ENVIRONMENT   : WIN32, C runtime, C++ runtime, ATL
///////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PausableKRProgressctrl.h"
#include "MemHDC.h"

///////////////////////////////////////////////////////////////////////
// Thread proc
///////////////////////////////////////////////////////////////////////
void __cdecl KRPROGPAUS_AnimateThreadProc(void* pParam)
{
    CPausableKRProgressCtrl* pClass = (CPausableKRProgressCtrl*)pParam;
    pClass->AnimateThread();
    _endthread();
}

///////////////////////////////////////////////////////////////////////
// Ctor/Dtor
///////////////////////////////////////////////////////////////////////
CPausableKRProgressCtrl::CPausableKRProgressCtrl(int nAnimationMS, 
                                                 int nBarFraction) :
    m_nAnimationMS(nAnimationMS),
    m_nBarFraction(nBarFraction),
    m_bPaused(false),
    m_crLeft(GetSysColor(COLOR_HIGHLIGHT) | 0x02000000),
    m_crRight(GetSysColor(COLOR_3DFACE) | 0x02000000),
    m_hBackground(NULL),
    m_hSpacePen(NULL)
{
#if (_WIN32_WINNT >= 0x0501)
    SetThemeClassList(L"Progress;Window");
    SetThemeExtendedStyle(0, 0);
#endif
}

CPausableKRProgressCtrl::~CPausableKRProgressCtrl(void)
{
    if (m_hBackground) 
    {
        DeleteObject(m_hBackground);
    }

      
    if (m_hSpacePen) 
    {
        DeleteObject(m_hSpacePen);
    }
}

///////////////////////////////////////////////////////////////////////
// Message Handlers
///////////////////////////////////////////////////////////////////////
LRESULT CPausableKRProgressCtrl::OnCreate(UINT uMsg, WPARAM wParam, 
                                          LPARAM lParam, BOOL& bHandled)
{
    // Reset our data...
    m_nCurPos = -m_nBarWidthMovement;
    m_bMovingRight = true;

#if (_WIN32_WINNT >= 0x0501)
    // Fake a theme change
    OnThemeChanged(0, 0, 0, bHandled);
#endif

    // Start a thread for animation
    m_hExitThreadEvt = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hThread = (HANDLE)_beginthread(KRPROGPAUS_AnimateThreadProc, 0, this);

    return 0;
}

LRESULT CPausableKRProgressCtrl::OnDestroy(UINT uMsg, WPARAM wParam, 
                                           LPARAM lParam, BOOL& bHandled)
{
    // Terminate our thread and wait 1.5 seconds
    SetEvent(m_hExitThreadEvt);
    WaitForSingleObject(m_hThread, 1500);

    return 0;
}

LRESULT CPausableKRProgressCtrl::OnPaint(UINT uMsg, WPARAM wParam, 
                                         LPARAM lParam, BOOL& bHandled)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(&ps);
    RealOnPaint(hDC);
    EndPaint(&ps);

    bHandled = TRUE;

    return 0;
}

LRESULT CPausableKRProgressCtrl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, 
                                              LPARAM lParam, BOOL& bHandled)
{
    if (wParam) {
        RealOnPaint((HDC)wParam);
    }
    else {
        HDC hDC = GetDC();
        RealOnPaint(hDC);
        ReleaseDC(hDC);
    }

    bHandled = TRUE;

    return 0;
}

#if (_WIN32_WINNT >= 0x0501)
LRESULT CPausableKRProgressCtrl::OnThemeChanged(UINT uMsg, WPARAM wParam, 
                                                LPARAM lParam, BOOL& bHandled)
{
    // in non-themeland we use WS_EX_STATICEDGE...
    if (m_hTheme) {
        DWORD dwCurStyle = GetWindowLong(GWL_EXSTYLE);
        dwCurStyle &= ~WS_EX_STATICEDGE;
        SetWindowLong(GWL_EXSTYLE, dwCurStyle);
    }
    else {
        DWORD dwCurStyle = GetWindowLong(GWL_EXSTYLE);
        dwCurStyle |= WS_EX_STATICEDGE;
        SetWindowLong(GWL_EXSTYLE, dwCurStyle);
    }

    bHandled = FALSE;

    return 0;
}
#endif

///////////////////////////////////////////////////////////////////////
// Animation Thread
///////////////////////////////////////////////////////////////////////
void CPausableKRProgressCtrl::AnimateThread()
{
    m_nBoxSpace = 2;
    m_nBoxWidth = 12;

    GetClientRect(&m_rcRect);

    m_nClientWidth = m_rcRect.right - m_rcRect.left;
    m_nBarWidthMovement = m_nClientWidth / m_nBarFraction;

#if (_WIN32_WINNT >= 0x0501)
    // Test for theme support...
    if (m_hTheme != NULL) 
    {
        // Use theme progress bar colors...
        GetThemeColor(PP_CHUNK, 0, TMT_FILLCOLORHINT, &m_crLeft);
        GetThemeColor(PP_BAR, 0, TMT_FILLCOLORHINT, &m_crRight);

        // Get the chunk size and spacing from uxtheme...
        GetThemeInt(PP_CHUNK, 0, TMT_PROGRESSCHUNKSIZE, &m_nBoxWidth);
        GetThemeInt(PP_CHUNK, 0, TMT_PROGRESSSPACESIZE, &m_nBoxSpace);
    }
    else 
#endif
    {
        // In non-themeland we need to leave space for the static edge
        m_rcInflateRect = m_rcRect;
        int nXEdge = GetSystemMetrics(SM_CXEDGE);
        int nYEdge = GetSystemMetrics(SM_CYEDGE);
        InflateRect(&m_rcInflateRect, -(nXEdge / 2), -(nYEdge / 2));
        m_nClientWidth = m_rcInflateRect.right - m_rcInflateRect.left;
    }

    m_nBarWidth = m_nClientWidth / m_nBarFraction;

    m_hBackground = CreateSolidBrush(m_crRight);
    m_hSpacePen = CreatePen(PS_SOLID, m_nBoxSpace, m_crRight);

    // Check for the exit event...
    // note: the wait time on this controls our animation speed...
    while (WAIT_TIMEOUT == WaitForSingleObject(m_hExitThreadEvt, 
                                               m_nAnimationMS)) {
        UpdatePos();
    }
}

///////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////
void CPausableKRProgressCtrl::RealOnPaint(HDC hDC)
{
    // Create a MemDC
    CMemHDC memDC(hDC, m_rcRect);

    RECT rcThemeRect;
    RECT * rcUseRect;

#if (_WIN32_WINNT >= 0x0501)
    // Test for theme support...
    if (m_hTheme != NULL) {
        // Fill in the background
        DrawThemeBackground(memDC, PP_BAR, 0, &m_rcRect, NULL);
        GetThemeBackgroundContentRect(memDC, PP_BAR, 0, &m_rcRect, 
                                      &rcThemeRect);
        FillRect(memDC, &rcThemeRect, m_hBackground);
        rcUseRect = &rcThemeRect;
    }
    else 
#endif
    {
        FillRect(memDC, &m_rcRect, m_hBackground);
        rcUseRect = &m_rcInflateRect;
    }

    DrawBoxes(memDC, (*rcUseRect).left + m_nCurPos, *rcUseRect);
}

void CPausableKRProgressCtrl::DrawBoxes(HDC hDC, int nOffset, 
                                        RECT& rcClientRect)
{
    COLORREF crLeft;
    COLORREF crRight;

    if ( m_bMovingRight ) 
    {
        crLeft  = m_crRight;       
        crRight = m_crLeft;       
    }
    else 
    { 
        crRight = m_crRight;       
        crLeft  = m_crLeft;       
    } 

    // Fill in the gradient...
    RECT rcRect;
    rcRect.top = rcClientRect.top;
    rcRect.bottom = rcClientRect.bottom;
    rcRect.left = rcClientRect.left + nOffset;
    rcRect.right = rcRect.left + m_nBarWidth;
    DoGradientFill(hDC, rcRect, rcClientRect, crLeft, crRight, 
                   m_nBoxWidth + m_nBoxSpace);

    // Cut out all the spaces (makes boxes)
    HPEN hpOld = (HPEN)SelectObject(hDC, m_hSpacePen);

    // Build a path to fill in...
    BeginPath(hDC);

    for (int i = rcClientRect.left + m_nBoxWidth + 1; 
         i < m_nClientWidth; i += (m_nBoxWidth + m_nBoxSpace)) {
        MoveToEx(hDC, i, rcRect.top, NULL);
        LineTo(hDC, i, rcRect.bottom);        
    }

    // End the path
    EndPath(hDC);

    // Draw the spacers
    StrokePath(hDC);

    // Cleanup...
    SelectObject(hDC, hpOld);
}

void CPausableKRProgressCtrl::DoGradientFill(HDC hDC, RECT& rcBoxRect, 
                                             RECT& rcClientRect, 
                                             COLORREF crLeft, COLORREF crRight,
                                             int nBoxWidth /* = 15 */)
{
    int nWidth = rcBoxRect.right - rcBoxRect.left; 

    RECT rcTmpRect;
    int r = 0, g = 0, b = 0;

    // Fill rcBoxRect
    rcTmpRect.top = rcBoxRect.top;
    rcTmpRect.bottom = rcBoxRect.bottom;

    for (int i = rcBoxRect.left; i < rcBoxRect.left + nWidth; i += nBoxWidth) {
        rcTmpRect.left = i;
        rcTmpRect.right = i + nBoxWidth;

        // Fix left and right
        if (rcTmpRect.left < rcClientRect.left) {
            rcTmpRect.left = rcClientRect.left;
        }

        if (rcTmpRect.right > rcClientRect.right) {
            rcTmpRect.right = rcClientRect.right;
        }

        // Make sure we still have something to draw...
        if (rcTmpRect.right - rcTmpRect.left > 0) {
            // Setup our colors for this time 'round
            r = GetRValue(crLeft) - 
                                 MulDiv(i - rcBoxRect.left, 
                                        GetRValue(crLeft) - GetRValue(crRight),
                                        nWidth);
            g = GetGValue(crLeft) - 
                                 MulDiv(i - rcBoxRect.left, 
                                        GetGValue(crLeft) - GetGValue(crRight),
                                        nWidth);
            b = GetBValue(crLeft) - 
                                 MulDiv(i - rcBoxRect.left, 
                                        GetBValue(crLeft) - GetBValue(crRight),
                                        nWidth);

            // Create our brush and fill in the rect
            HBRUSH hBrush = CreateSolidBrush((COLORREF)(RGB(r, g, b) | 0x02000000));
            FillRect(hDC, &rcTmpRect, hBrush);
            DeleteObject(hBrush);
        }
    }
}

void CPausableKRProgressCtrl::UpdatePos()
{
    // Always move to the right
    if(m_nCurPos >= m_rcRect.right)
    {
        m_bMovingRight = true;
        m_nCurPos = -m_nBarWidthMovement;
    }

    // Update our position
    //
    // 13 pixel value is optimized for use of the bar on the scan
    // progress screen of NAV 12.0.
    //
    if (!m_bPaused)  
    { 
        if (m_bMovingRight) 
        {
            m_nCurPos += 13;
        }
        else 
        {
            m_nCurPos -= 13;
        }
    }

    // Force a redraw
    RedrawWindow(NULL, NULL, 
                 RDW_UPDATENOW | RDW_INVALIDATE | RDW_INTERNALPAINT);    
}


void CPausableKRProgressCtrl::Pause()
{
    m_bPaused = true;
}


void CPausableKRProgressCtrl::Resume()
{
    m_bPaused = false;
}

