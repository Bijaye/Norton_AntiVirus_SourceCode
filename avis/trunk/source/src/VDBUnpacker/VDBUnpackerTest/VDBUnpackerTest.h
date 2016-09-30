// VDBUnpackerTest.h : main header file for the VDBUNPACKERTEST application
//

#if !defined(AFX_VDBUNPACKERTEST_H__C16A2F78_A80E_11D3_8ADC_CECFC7000000__INCLUDED_)
#define AFX_VDBUNPACKERTEST_H__C16A2F78_A80E_11D3_8ADC_CECFC7000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVDBUnpackerTestApp:
// See VDBUnpackerTest.cpp for the implementation of this class
//

class CVDBUnpackerTestApp : public CWinApp
{
public:
	CVDBUnpackerTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVDBUnpackerTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVDBUnpackerTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VDBUNPACKERTEST_H__C16A2F78_A80E_11D3_8ADC_CECFC7000000__INCLUDED_)
