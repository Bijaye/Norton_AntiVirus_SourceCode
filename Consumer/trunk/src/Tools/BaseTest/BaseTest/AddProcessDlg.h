////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>

// AddProcessDlg dialog

class AddProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(AddProcessDlg)

public:
	AddProcessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddProcessDlg();

	void SetProcessName(std::vector <CString> * vProcessName);
// Dialog Data
	enum { IDD = IDD_ADD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()
public:
	CString m_strProcessName;
	std::vector<CString> * m_vProcessName;
	afx_msg void OnBnClickedOk();

};
