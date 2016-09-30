// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_VERIFYSGPASS_H__7D33213B_BA84_4C74_BD14_25A82D7D8F3C__INCLUDED_)
#define AFX_VERIFYSGPASS_H__7D33213B_BA84_4C74_BD14_25A82D7D8F3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VerifySGPass.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVerifySGPass dialog

class CVerifySGPass : public CDialog
{
// Construction
public:
	CVerifySGPass(CWnd* pParent = NULL);   // standard constructor

	CVerifySGPass( CString strPass, CWnd* pParent = NULL );   // standard constructor
// Dialog Data
	//{{AFX_DATA(CVerifySGPass)
	enum { IDD = IDD_VERIFYPASSWORD };
	CEdit	m_edtPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVerifySGPass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strPassword;

	// Generated message map functions
	//{{AFX_MSG(CVerifySGPass)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VERIFYSGPASS_H__7D33213B_BA84_4C74_BD14_25A82D7D8F3C__INCLUDED_)
