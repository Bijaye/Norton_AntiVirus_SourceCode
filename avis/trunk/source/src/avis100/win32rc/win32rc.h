// win32rc.h : main header file for the WIN32RC application
//

#if !defined(AFX_WIN32RC_H__00C2D8E6_0A25_11D3_985D_000629167334__INCLUDED_)
#define AFX_WIN32RC_H__00C2D8E6_0A25_11D3_985D_000629167334__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWin32rcApp:
// See win32rc.cpp for the implementation of this class
//

class CWin32rcApp : public CWinApp
{
public:
	CWin32rcApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWin32rcApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL
	protected:
		RCError rcTrace;
		CString m_GoatsDir;
		CStringList m_GoatsDirs2;
		CString m_SampleName;

		bool m_trace;
		bool m_standard; //assume standard replication
		bool m_after_reboot; // called after reboot
		bool m_reboot_required;
		bool m_copy_results;
		bool m_help;
		bool m_noshutdown;

		int m_nSocketPort;
		CString m_strSocketAddr;
		CString m_strCommandFile;
		CString m_strIdentifier;
		CSocket m_socket;
		CString m_strResultsDir; // where to copy resulting replicants
		bool m_CopyResults;
		CString m_strVarList; //variable list if used
		int ProcessParams();
		void GetArgs (CStringList &Params);



// Implementation

	//{{AFX_MSG(CWin32rcApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIN32RC_H__00C2D8E6_0A25_11D3_985D_000629167334__INCLUDED_)
