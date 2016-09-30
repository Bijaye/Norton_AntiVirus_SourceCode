// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//{{AFX_INCLUDES()
#include "ldvpvirusdetails.h"
//}}AFX_INCLUDES
#if !defined(AFX_VIRUSFOUND_H__0FDBCB3B_B900_11D1_AB29_00A0C90F8F6F__INCLUDED_)
#define AFX_VIRUSFOUND_H__0FDBCB3B_B900_11D1_AB29_00A0C90F8F6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// VirusFound.h : header file
//

// TCashin 2/12/03 Port CRT fix for defect 1-CBVAJ
// 1-CBVAJ - Symantec internal (Intel) Display message RTVSCAN scan Memory Leak - KAP
//	replace CStrings with STL string class.  The release implementation of CString caches
//	memory allocations causing incredible memory growth under a 100k eicar test.  We allocate
//	over 200k CString memory allocations for this test, none of which is released when the dialog
//	is closed.
#pragma warning (disable:4786)
#include <map>
#include <string>
#include "Resource.h"
using namespace std;

typedef struct _VIRUSFOUND {
	struct _VIRUSFOUND *Next;
	struct _VIRUSFOUND *Prev;
	string LogLine;
	string Description;
	} VIRUSFOUND, *PVIRUSFOUND;

// TCashin 2/12/03 End Port CRT fix for defect 1-CBVAJ

#include "vptoolbar.h"
/////////////////////////////////////////////////////////////////////////////
// CVirusFound dialog

class CVirusFound : public CDialog
{
// Construction
public:
	DWORD GetKenFlags();
	DWORD SetCurrent(PVIRUSFOUND Virus);
	PVIRUSFOUND Head;
	PVIRUSFOUND CurrentVirus;
	CString m_Title;
	HKEY hKey;
	DWORD Total;
	DWORD CurrentValue;
	CToolBar	m_toolBar;
	CStatusBar m_Status;
	VIEWCLOSED m_pfnViewClosed;
	LPVOID	   m_pContext;
	void *Context;
	void OnPrev();
	void OnNext();
	void OnDetails();
	BOOL m_Modeless;
	HWND m_Parent;
	DWORD m_Flags;
	BOOL m_bInitialized;
	DWORD AddVirus(char *line,char *Description);
	DWORD Open(PVIRUSFOUNDDLG VirusFound);
	CVirusFound(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVirusFound)
	enum { IDD = IDD_VIRUS_FOUND };
	CStatic	m_Text;
	CLDVPVirusDetails	m_KenD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVirusFound)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVirusFound)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg long AddItem(UINT wParam,LPARAM lParam);
	afx_msg long Set(UINT wParam,LPARAM lParam);
	afx_msg void OnPreviousKensDetails();
	afx_msg void OnNextKensDetails();
	afx_msg void OnDialogClosedKensDetails();
	afx_msg void OnUpdatePrev(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNext(CCmdUI* pCmdUI);
	afx_msg void OnHelp();
    afx_msg BOOL OnQueryEndSession();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg BOOL OnTTT(UINT,NMHDR *NotifyMessageHeader,LRESULT *);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUSFOUND_H__0FDBCB3B_B900_11D1_AB29_00A0C90F8F6F__INCLUDED_)
