// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//{{AFX_INCLUDES()
#include "shelsel2.h"
//}}AFX_INCLUDES
#if !defined(AFX_SELECTFOLDERDLG_H__321C5A13_8C56_11D1_81EF_00A0C95C0756__INCLUDED_)
#define AFX_SELECTFOLDERDLG_H__321C5A13_8C56_11D1_81EF_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectFolderDlg.h : header file
//
#include "vpcommon.h"
#include "resource.h"

//Load/save flags
#define SHELLSEL_LOAD	2
#define	SHELLSEL_SAVE	4

/////////////////////////////////////////////////////////////////////////////
// CShellSelDlg dialog

class CShellSelDlg : public CDialog
{
// Construction
public:
	CShellSelDlg(CWnd* pParent = NULL);   // standard constructor

	CString		m_strItems,
				m_strFolders;
	DWORD		m_dwLoadSaveFlag;
	IConfig		*m_ptrConfig;
	IVPStorage	*m_ptrStorage;

	CShelsel2	&GetShellSelCtl(){ return m_ShellSel; };

// Dialog Data
	//{{AFX_DATA(CShellSelDlg)
	enum { IDD = IDD_SELECT_FOLDER };
	CShelsel2	m_ShellSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShellSelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTFOLDERDLG_H__321C5A13_8C56_11D1_81EF_00A0C95C0756__INCLUDED_)
