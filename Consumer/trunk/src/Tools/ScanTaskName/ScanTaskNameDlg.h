////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskNameDlg.h : header file
//

#pragma once


// CScanTaskNameDlg dialog
class CScanTaskNameDlg : public CDialog
{
// Construction
public:
	CScanTaskNameDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SCANTASKNAME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnBnClickedSaveFile();

protected:
	CStringW m_sFileName;
	CStringW m_sNewTaskName;
	CStringW m_sCurTaskName;
};
