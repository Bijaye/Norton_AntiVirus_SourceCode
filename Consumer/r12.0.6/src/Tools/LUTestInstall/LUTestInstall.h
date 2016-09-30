// LUTestInstall.h : main header file for the LUTESTINSTALL application
//

#if !defined(AFX_LUTESTINSTALL_H__7E9F3937_91BF_4062_AE81_6496F7492F01__INCLUDED_)
#define AFX_LUTESTINSTALL_H__7E9F3937_91BF_4062_AE81_6496F7492F01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLUTestInstallApp:
// See LUTestInstall.cpp for the implementation of this class
//

class CLUTestInstallApp : public CWinApp
{
public:
	CLUTestInstallApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLUTestInstallApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLUTestInstallApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUTESTINSTALL_H__7E9F3937_91BF_4062_AE81_6496F7492F01__INCLUDED_)
