// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_EMAILMESSAGEDLG_H__63A60645_975E_11D1_8203_00A0C95C0756__INCLUDED_)
#define AFX_EMAILMESSAGEDLG_H__63A60645_975E_11D1_8203_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EmailMessageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEmailMessageDlg dialog

class CEmailMessageDlg : public CDialog
{
// Construction
public:
	CString m_strTitle;
	CEmailMessageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEmailMessageDlg)
	enum { IDD = IDD_EMAIL_MESSAGE };
	CString	m_strInfection;
	CString	m_strSubject;
	CString	m_strWarning;
	//}}AFX_DATA

	BOOL	m_bMultiple;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmailMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEmailMessageDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMAILMESSAGEDLG_H__63A60645_975E_11D1_8203_00A0C95C0756__INCLUDED_)
