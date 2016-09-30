////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MigrationTestDlg.h : header file
//

#if !defined(AFX_MIGRATIONTESTDLG_H__51201E29_3B2D_422E_B35B_A21DD29F8424__INCLUDED_)
#define AFX_MIGRATIONTESTDLG_H__51201E29_3B2D_422E_B35B_A21DD29F8424__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMigrationTestDlg dialog

class CMigrationTestDlg : public CDialog
{
// Construction
public:
	CMigrationTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMigrationTestDlg)
	enum { IDD = IDD_MIGRATIONTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMigrationTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMigrationTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExit();
	afx_msg void OnWriteOptionsToFile();
	afx_msg void OnSetFromFile();
	afx_msg void OnWriteToFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIGRATIONTESTDLG_H__51201E29_3B2D_422E_B35B_A21DD29F8424__INCLUDED_)
