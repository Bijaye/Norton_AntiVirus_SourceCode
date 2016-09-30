// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_PASSDLG_H__685F2583_022D_4F9E_899A_1A4E02C8E634__INCLUDED_)
#define AFX_PASSDLG_H__685F2583_022D_4F9E_899A_1A4E02C8E634__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PassDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPassDlg dialog

class CPassDlg : public CDialog
{
// Construction
public:
	CPassDlg(CWnd* pParent = NULL);   // standard constructor

	DWORD m_Error;

	void Decrypt( CString& );
	void Encrypt( CString& );

// Dialog Data
	//{{AFX_DATA(CPassDlg)
	enum { IDD = IDD_PASSWORD };
	CEdit	m_edtPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPassDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_Password;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSDLG_H__685F2583_022D_4F9E_899A_1A4E02C8E634__INCLUDED_)
