////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScriptReconDlg.h : header file
//

#if !defined(AFX_SCRIPTRECONDLG_H__D3A48AD7_600F_449A_BCC7_CCBE041E3279__INCLUDED_)
#define AFX_SCRIPTRECONDLG_H__D3A48AD7_600F_449A_BCC7_CCBE041E3279__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CScriptReconDlg dialog

class CScriptReconDlg : public CDialog
{
// Construction
public:
	CScriptReconDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScriptReconDlg)
	enum { IDD = IDD_SCRIPTRECON_DIALOG };
	CEdit	m_edtCode;
	CComboBox	m_cboURL;
	CString	m_strFileName;
	DWORD	m_dwLineNumber;
	CString	m_strCode;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptReconDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScriptReconDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFind();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTRECONDLG_H__D3A48AD7_600F_449A_BCC7_CCBE041E3279__INCLUDED_)
