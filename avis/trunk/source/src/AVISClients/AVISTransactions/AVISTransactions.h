// AVISTransactions.h : main header file for the AVISTRANSACTIONS DLL
//

#if !defined(AFX_AVISTRANSACTIONS_H__FD364CB6_7F1A_11D2_8A64_00203529AC86__INCLUDED_)
#define AFX_AVISTRANSACTIONS_H__FD364CB6_7F1A_11D2_8A64_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "impexp.h"

#include "resource.h"		// main symbols

#include "MyTraceClient.h"

/////////////////////////////////////////////////////////////////////////////
// CAVISTransactionsApp
// See AVISTransactions.cpp for the implementation of this class
//

typedef void (*AutoTerminateFunction)(LPCSTR);

class CAVISTransactionsApp : public CWinApp
{
public:
	static MyTraceClient* s_pTc;
	static AutoTerminateFunction s_pAutoTerminateFunction;

public:
	CAVISTransactionsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVISTransactionsApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAVISTransactionsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// object for tracing/logging declared in calling application
	static void IMPEXP SetTraceClient(MyTraceClient* pTc);
	static void IMPEXP SetAutoTerminateFunction(AutoTerminateFunction pAutoTerminateFunction);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVISTRANSACTIONS_H__FD364CB6_7F1A_11D2_8A64_00203529AC86__INCLUDED_)
