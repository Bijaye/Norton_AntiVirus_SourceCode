// Copyright 1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/BogusMainWnd.cpv   1.0   01 Jul 1997 17:22:20   JBRENNA  $
//
// Description:
//   This is used to emulate that we have a main window. This window will
//   cause an item in the Win95/WinNT4 task bar to appear. Note that the
//   window is WS_VISIBLE. But, it is positioned off the screen. It also
//   will not move or resize onto the visible screen.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/BogusMainWnd.cpv  $
// 
//    Rev 1.0   01 Jul 1997 17:22:20   JBRENNA
// Initial revision.
// 
//****************************************************************************

#include "stdafx.h"
#include "BogusMainWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//****************************************************************************
//* Message Map **************************************************************
//****************************************************************************

BEGIN_MESSAGE_MAP(CBogusMainWnd, CWnd)
	//{{AFX_MSG_MAP(CBogusMainWnd)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_MOVING,            CBogusMainWnd::OnMoving)
    ON_MESSAGE (WM_WINDOWPOSCHANGING, CBogusMainWnd::OnWindowPosChanging)
END_MESSAGE_MAP()

//****************************************************************************
//* CBogusMainWnd implementation *********************************************
//****************************************************************************

CBogusMainWnd::CBogusMainWnd ()
{
    // Set up the coordinates such that the window is really
    // off the screen.
    m_rcWinRect = CRect (-32668, -32668, -32618, -32618);
}

BOOL CBogusMainWnd::MyCreate (LPCTSTR lpszWindowName, WORD nIconId /*= 0*/)
{
    HICON   hIcon           = nIconId? LoadIcon (AfxGetInstanceHandle(), MAKEINTRESOURCE (nIconId)) : 0;
    CString strWndClassName = AfxRegisterWndClass (0, 0, 0, hIcon);
    BOOL    bSuccess;

    bSuccess = CWnd::CreateEx(0, strWndClassName, lpszWindowName, 
                              WS_VISIBLE|WS_SYSMENU,
                              m_rcWinRect.left, 
                              m_rcWinRect.top, 
                              m_rcWinRect.right - m_rcWinRect.left,
                              m_rcWinRect.bottom - m_rcWinRect.top,
                              NULL, NULL);

    if (bSuccess && hIcon)
    {
        SetIcon (hIcon, TRUE);  // Set big icon
        SetIcon (hIcon, FALSE); // Set small icon.
    }

    return bSuccess;
}

//========================================================================
// Only process non-SC_MOVE and non-SC_SIZE system menu requests. The
// window should not move or change size.
//========================================================================
void CBogusMainWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if (nID != SC_MOVE && nID != SC_SIZE)
	    CWnd::OnSysCommand(nID, lParam);
}

//========================================================================
// Override the WM_MOVING message handler. Force the new window position
// to our hard coded location. The window should never move or size.
//========================================================================
LRESULT CBogusMainWnd::OnMoving(WPARAM wParam, LPARAM lParam) 
{
    WORD   fwSide = wParam;             // edge of window to be moved 
    LPRECT lprc   = (LPRECT) lParam;    // screen coordinates of drag rectangle

    *lprc = m_rcWinRect;

    return 0;
}

//========================================================================
// Override the WM_WINDOWSPOSCHANGING message handler. Force the new window
// position to our hard coded location. The window should never move or size.
//========================================================================
LRESULT CBogusMainWnd::OnWindowPosChanging (WPARAM wParam, LPARAM lParam)
{
    LPWINDOWPOS lpwp = (LPWINDOWPOS) lParam; // points to size and position data 

    lpwp->x  = m_rcWinRect.left;
    lpwp->y  = m_rcWinRect.top;
    lpwp->cx = m_rcWinRect.right  - m_rcWinRect.left;
    lpwp->cy = m_rcWinRect.bottom - m_rcWinRect.top;

    return 1;
}
