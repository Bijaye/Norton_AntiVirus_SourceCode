// ScriptRecon.h : main header file for the SCRIPTRECON application
//

#if !defined(AFX_SCRIPTRECON_H__19BF2D44_0CE6_41A9_96F7_706270ACB291__INCLUDED_)
#define AFX_SCRIPTRECON_H__19BF2D44_0CE6_41A9_96F7_706270ACB291__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CScriptReconApp:
// See ScriptRecon.cpp for the implementation of this class
//

class CScriptReconApp : public CWinApp
{
public:
	CScriptReconApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptReconApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CScriptReconApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTRECON_H__19BF2D44_0CE6_41A9_96F7_706270ACB291__INCLUDED_)
