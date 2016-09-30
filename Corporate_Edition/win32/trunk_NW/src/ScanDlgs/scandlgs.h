// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// shellscan.h : main header file for the SHELLSCAN DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "scndlgex.h"

#include "vpcommon.h"

/////////////////////////////////////////////////////////////////////////////
// CShellscanApp
// See shellscan.cpp for the implementation of this class
//

class CShellscanApp : public CWinApp
{
private:
	CString m_strAppname;

public:
	CShellscanApp();

	char m_szHelpPath[MAX_PATH];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellscanApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CShellscanApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void WinHelpInternal( DWORD dwData, UINT nCmd = HELP_CONTEXT );
	BOOL IsHTMLHelpEnabled();

protected:	

    BOOL GetHelpDir(LPTSTR szHelpPath, UINT nMax);

    TCHAR m_szHelpCommand[MAX_PATH];

};


void wThreadSwitch(void);
/////////////////////////////////////////////////////////////////////////////
