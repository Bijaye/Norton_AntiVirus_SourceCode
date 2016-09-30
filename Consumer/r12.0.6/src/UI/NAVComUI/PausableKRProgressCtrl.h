#pragma once

// To use this in your project you need to do three things:
// 1. define _WIN32_WINNT to 0x0501 in your StdAfx.h
// 2. link to delayimp.lib
// 3. add uxtheme.dll in the "Linker.Input.Delay Loaded DLLs"
//    section of the project properties.


#if (_WIN32_WINNT >= 0x501)
#include <atltheme.h>
#endif

class CPausableKRProgressCtrl :
    public CWindowImpl<CPausableKRProgressCtrl>
#if (_WIN32_WINNT >= 0x0501)
    , public WTL::CThemeImpl<CPausableKRProgressCtrl>
#endif
{
public:
DECLARE_WND_CLASS("PAUSABLEKRPROGRESS")

BEGIN_MSG_MAP(CPausableKRProgressCtrl)
#if (_WIN32_WINNT >= 0x0501)
    CHAIN_MSG_MAP(WTL::CThemeImpl<CPausableKRProgressCtrl>)
#endif
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_PAINT, OnPaint)
    MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
#if (_WIN32_WINNT >= 0x0501)
    MESSAGE_HANDLER(WM_THEMECHANGED, OnThemeChanged)
#endif
END_MSG_MAP()

public:
    // Animation time is in MS
    // Bar Fraction controls the width of the colored area as a fraction (3 = 1/3)
    //
    // 105 value for animation is optimized for performance in when the
    // bar is used on the scan progress screen of NAV 12.0.  Minimizing
    // impact on scan time while still not appearing choppy.
    //
    CPausableKRProgressCtrl(int nAnimationMS = 105, int nBarFraction = 3);
    virtual ~CPausableKRProgressCtrl(void);

    // Message Handlers
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, 
                             BOOL& bHandled);
#if (_WIN32_WINNT >= 0x0501)
    LRESULT OnThemeChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, 
                               BOOL& bHandled);
#endif

    // Animation Thread
    void AnimateThread();

    // Pause and unpause
    void Pause();   // no effect if already paused
    void Resume();  // no effect if already unpaused

protected:
    // Helper methods
    void RealOnPaint(HDC hDC);
    void DrawBoxes(HDC hDC, int nOffset, RECT& rcClientRect);
    void DoGradientFill(HDC hDC, RECT& rcBoxRect, RECT& rcClientRect, 
                        COLORREF crLeft, COLORREF crRight, int nBoxWidth = 15);
    void UpdatePos();

protected:
    int m_nCurPos;
    bool m_bMovingRight;
    bool m_bPaused;
    int m_nAnimationMS;
    int m_nBarFraction;

    int m_nBarWidth;
    int m_nBarWidthMovement;
    int m_nBoxWidth;
    int m_nClientWidth;
    int m_nBoxSpace;
    
    RECT m_rcRect;
    RECT m_rcInflateRect;

    HANDLE m_hExitThreadEvt;
    HANDLE m_hThread;

    HBRUSH m_hBackground;
    HPEN m_hSpacePen;

    COLORREF m_crLeft;
    COLORREF m_crRight;
};
