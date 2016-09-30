// AVISMonitor.h : main header file for the AVISMONITOR application
//

#if !defined(AFX_AVISMONITOR_H__66A8FF47_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
#define AFX_AVISMONITOR_H__66A8FF47_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "singleinstance.h"

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
// CAVISMonitorApp:
// See AVISMonitor.cpp for the implementation of this class
//

class CAVISMonitorApp : public CWinApp
{
public:
	CAVISMonitorApp();

    CSingleInstance m_singleInstance;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVISMonitorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CAVISMonitorApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVISMONITOR_H__66A8FF47_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
