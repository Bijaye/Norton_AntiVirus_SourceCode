////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/REMIND/VCS/N32RMD.H_v   1.2   15 Dec 1997 17:33:58   jtaylor  $
////////////////////////////////////////////////////////////////////////////////////
//
// n32rmd.h : main header file for the N32RMD application
//
////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/REMIND/VCS/N32RMD.H_v  $
// 
//    Rev 1.2   15 Dec 1997 17:33:58   jtaylor
// fixed the logout problem, updated the timer logic.
//
////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_N32RMD_H__85687EC6_6DD3_11D1_8F2D_444553540000__INCLUDED_)
#define AFX_N32RMD_H__85687EC6_6DD3_11D1_8F2D_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CN32rmdApp:
// See n32rmd.cpp for the implementation of this class
//

class CN32rmdApp : public CWinApp
{
public:
	CN32rmdApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CN32rmdApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CN32rmdApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	VOID SetMilisecondsLeft( DWORD dwTime);
	DWORD GetMilisecondsLeft();
	void DialogClosed();
	void IncrementLaunchTime(void);
	DWORD GetSecondsTillLaunch();
	time_t GetLaunchTime( void );
	void ReadReminderFile( LPTSTR szFullText, DWORD dwSize );
	BOOL m_bWaitTimer;
private:
	DWORD m_dwTimeLeft;
};

void CALLBACK OnTimer(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_N32RMD_H__85687EC6_6DD3_11D1_8F2D_444553540000__INCLUDED_)
