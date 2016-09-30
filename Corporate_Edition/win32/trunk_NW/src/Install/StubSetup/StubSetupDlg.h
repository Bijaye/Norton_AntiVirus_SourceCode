// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// StubSetupDlg.h : header file
//

#if !defined(AFX_STUBSETUPDLG_H__AB3B81E7_FD93_11D3_BEEA_00A0CC272509__INCLUDED_)
#define AFX_STUBSETUPDLG_H__AB3B81E7_FD93_11D3_BEEA_00A0CC272509__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CStubSetupDlg dialog

class CStubSetupDlg : public CDialog
{
// Construction
public:
	CStubSetupDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CStubSetupDlg)
	enum { IDD = IDD_STUBSETUP_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStubSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CStubSetupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STUBSETUPDLG_H__AB3B81E7_FD93_11D3_BEEA_00A0CC272509__INCLUDED_)
