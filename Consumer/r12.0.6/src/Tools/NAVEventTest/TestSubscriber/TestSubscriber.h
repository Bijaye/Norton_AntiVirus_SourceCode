// TestSubscriber.h : main header file for the TESTSUBSCRIBER application
//

#if !defined(AFX_TESTSUBSCRIBER_H__CD70023D_BEF5_4838_A6A3_FFD3766C9FF7__INCLUDED_)
#define AFX_TESTSUBSCRIBER_H__CD70023D_BEF5_4838_A6A3_FFD3766C9FF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestSubscriberApp:
// See TestSubscriber.cpp for the implementation of this class
//

class CTestSubscriberApp : public CWinApp
{
public:
	CTestSubscriberApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestSubscriberApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestSubscriberApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTSUBSCRIBER_H__CD70023D_BEF5_4838_A6A3_FFD3766C9FF7__INCLUDED_)
