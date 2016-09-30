////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// InstallVerifierDlg.h : header file
//

#pragma once


// CInstallVerifierDlg dialog
class CInstallVerifierDlg : public CDialog
{
// Construction
public:
	CInstallVerifierDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_INSTALLVERIFIER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CString GetFileName(CString, CString, CString);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strMSIPath;
	CString m_strXMLPath;
	afx_msg void OnBnClickedButtonGenXml();
	afx_msg void OnBnClickedButtonMsiPath();
	afx_msg void OnBnClickedButtonXmlPath();

};
