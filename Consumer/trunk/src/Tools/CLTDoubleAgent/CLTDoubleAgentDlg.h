////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CLTDoubleAgentDlg.h : header file
//

#pragma once
#include "cltproducthelper.h"

// CCLTDoubleAgentDlg dialog
class CCLTDoubleAgentDlg : public CDialog
{
// Construction
public:
	CCLTDoubleAgentDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLTDOUBLEAGENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CCLTProductHelper cltHelper;
	void PostError(LPCTSTR description, HRESULT error);
	void PostLicenseType(DWORD dwType);
	void PostLicenseAttributes(DWORD dwAttribs);
	void PostUnsigned(int uiControl, unsigned long ulValue);
	void PostBoolean(int uiControl, bool bValue);
	void PostString(int uiControl, LPCTSTR pszValue);
public:
	afx_msg void OnBnClickedQuery();
};
