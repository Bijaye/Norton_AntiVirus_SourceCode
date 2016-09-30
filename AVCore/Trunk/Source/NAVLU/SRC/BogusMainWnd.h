// Copyright 1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/BogusMainWnd.h_v   1.0   01 Jul 1997 17:22:24   JBRENNA  $
//
// Description:
//   Provides the header file for CBogusMainWnd.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/BogusMainWnd.h_v  $
// 
//    Rev 1.0   01 Jul 1997 17:22:24   JBRENNA
// Initial revision.
// 
//****************************************************************************

#ifndef __BOGUSMAINWND_H
#define __BOGUSMAINWND_H

/////////////////////////////////////////////////////////////////////////////
// CMainWnd window

class CBogusMainWnd : public CWnd
{
// Construction
public:
	CBogusMainWnd();   // standard constructor

    BOOL MyCreate(LPCTSTR lpszWindowName, WORD nIconId = 0);
	LRESULT OnMoving (WPARAM wParam, LPARAM lParam);
    LRESULT OnWindowPosChanging (WPARAM wParam, LPARAM lParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBogusMainWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBogusMainWnd)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Private Functions
private:

// Private Data
private:
    RECT m_rcWinRect;
};

#endif // __BOGUSMAINWND_H
