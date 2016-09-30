// MachineInfo.h : main header file for the MACHINEINFO application
//

#if !defined(AFX_MACHINEINFO_H__FA9C6EC2_3D70_4E9D_BB0B_B22734C29F33__INCLUDED_)
#define AFX_MACHINEINFO_H__FA9C6EC2_3D70_4E9D_BB0B_B22734C29F33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMachineInfoApp:
// See MachineInfo.cpp for the implementation of this class
//

class CMachineInfoApp : public CWinApp
{
public:
	CMachineInfoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMachineInfoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMachineInfoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACHINEINFO_H__FA9C6EC2_3D70_4E9D_BB0B_B22734C29F33__INCLUDED_)
