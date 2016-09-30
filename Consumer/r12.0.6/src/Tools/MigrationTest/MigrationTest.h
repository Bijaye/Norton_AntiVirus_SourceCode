// MigrationTest.h : main header file for the MIGRATIONTEST application
//

#if !defined(AFX_MIGRATIONTEST_H__172FE057_2415_444D_BB41_870914A96B0C__INCLUDED_)
#define AFX_MIGRATIONTEST_H__172FE057_2415_444D_BB41_870914A96B0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMigrationTestApp:
// See MigrationTest.cpp for the implementation of this class
//

class CMigrationTestApp : public CWinApp
{
public:
	CMigrationTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMigrationTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMigrationTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIGRATIONTEST_H__172FE057_2415_444D_BB41_870914A96B0C__INCLUDED_)
