// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_WARNINGDLG_H__63A60648_975E_11D1_8203_00A0C95C0756__INCLUDED_)
#define AFX_WARNINGDLG_H__63A60648_975E_11D1_8203_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WarningDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWarningDlg dialog

class CWarningDlg : public CDialog
{
// Construction
public:
	CString m_strTitle;
	CWarningDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWarningDlg)
	enum { IDD = IDD_WARNING_DIALOG };
	CString	m_strInfection;
	CString	m_strSubject;
	CString	m_strWarning;
	//}}AFX_DATA

	BOOL	m_bMultiple;
	DWORD	m_bChangeSubject;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWarningDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWarningDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WARNINGDLG_H__63A60648_975E_11D1_8203_00A0C95C0756__INCLUDED_)
