// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#pragma once
#include "afxwin.h"


// CNewPassDlg dialog

class CNewPassDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewPassDlg)

public:
	CNewPassDlg(CWnd* pParent = NULL);   // standard constructor
	CNewPassDlg( CString strPass, CWnd* pParent = NULL );   // standard constructor
	virtual ~CNewPassDlg();

// Dialog Data
	enum { IDD = IDD_NEW_PASS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString m_strPassword;

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ceOldPassword;
	CEdit m_ceNewPassword;
	CEdit m_ceVerifyPassword;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CString szOldPassword;
	CString szNewPassword;
	CString szVerPassword;
};
