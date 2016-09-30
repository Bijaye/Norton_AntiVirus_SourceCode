// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_DOMAINPASSWORDDLG_H__5BFFBF63_4DC9_11D7_BBDD_0010A4972180__INCLUDED_)
#define AFX_DOMAINPASSWORDDLG_H__5BFFBF63_4DC9_11D7_BBDD_0010A4972180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DomainPasswordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDomainPasswordDlg dialog

class CDomainPasswordDlg : public CDialog
{
// Construction
public:
	CDomainPasswordDlg(CWnd* pParent, const CString &csServerName );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDomainPasswordDlg)
	enum { IDD = IDD_SERVER_GROUP_PASSWORD };
	CStatic	m_ctlServerName;
	CEdit	m_ctlPassword;
	CString	m_csPassword;
	CString	m_csServerName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDomainPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDomainPasswordDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOMAINPASSWORDDLG_H__5BFFBF63_4DC9_11D7_BBDD_0010A4972180__INCLUDED_)
