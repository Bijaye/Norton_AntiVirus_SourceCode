////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// InstoptsEditDlg.h : header file
//

#pragma once
#include "navopt32.h"



// CInstoptsEditDlg dialog
class CInstoptsEditDlg : public CDialog
{
// Construction
public:
	CInstoptsEditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_INSTOPTSEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
// Implementation
protected:
	HICON m_hIcon;
	CString m_sFolderName;
	CString m_sName;
	CString m_sValue;
	bool m_bString;
	bool m_bDword;
	bool m_bAdd;
	bool m_bDelete;
	HNAVOPTS32 m_hInstOptsDat;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedRadioString();
	afx_msg void OnBnClickedRadioAdd();
	afx_msg void OnBnClickedRadioDelete();
	afx_msg void OnBnClickedRadioChange();
	//afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeDir();
	afx_msg void OnBnClickedRadioDword();
	afx_msg void OnEnChangeName();
	afx_msg void OnEnChangeValue();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnOK();
};
