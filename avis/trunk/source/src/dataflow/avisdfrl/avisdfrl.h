// AVISDFRL.h : main header file for the AVISDFRL DLL
//

#if !defined(AFX_DFLAUNCHER_H__2D0526B6_48DA_11D2_B96F_0004ACEC31AA__INCLUDED_)
#define AFX_DFLAUNCHER_H__2D0526B6_48DA_11D2_B96F_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

void MyAfxFormatStrings (CString& outString, UINT formatID, 
						 int count,
						 LPCTSTR str1,
						 LPCTSTR str2 = NULL,
						 LPCTSTR str3 = NULL,
						 LPCTSTR str4 = NULL,
						 LPCTSTR str5 = NULL,
						 LPCTSTR str6 = NULL,
						 LPCTSTR str7 = NULL,
						 LPCTSTR str8 = NULL,
						 LPCTSTR str9 = NULL);

void MyAfxFormatStrings (CString& outString, LPCTSTR formatStr, 
						 int count,
						 LPCTSTR str1,
						 LPCTSTR str2 = NULL,
						 LPCTSTR str3 = NULL,
						 LPCTSTR str4 = NULL,
						 LPCTSTR str5 = NULL,
						 LPCTSTR str6 = NULL,
						 LPCTSTR str7 = NULL,
						 LPCTSTR str8 = NULL,
						 LPCTSTR str9 = NULL);

/////////////////////////////////////////////////////////////////////////////
// CDFLauncherApp
// See DFLauncher.cpp for the implementation of this class
//

class CDFLauncherApp : public CWinApp
{
public:
	void TermDFLauncherWindow();
	BOOL FilterDFLauncherMessage (MSG *pMsg);
	HWND m_hALMWnd;
	void InitDFLauncherWindow();
	CDFLauncherApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFLauncherApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDFLauncherApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFLAUNCHER_H__2D0526B6_48DA_11D2_B96F_0004ACEC31AA__INCLUDED_)
