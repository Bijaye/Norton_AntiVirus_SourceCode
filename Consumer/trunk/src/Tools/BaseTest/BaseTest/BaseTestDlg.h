////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// BaseTestDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include <vector>



// CBaseTestDlg dialog
class CBaseTestDlg : public CDialog
{
// Construction
public:
	CBaseTestDlg(CWnd* pParent = NULL);	// standard constructor
	
// Dialog Data
	enum { IDD = IDD_BASETEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	//CList<CBaseTestDlg *> listWnd;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonFile();
	CListCtrl m_listView;
	CString m_strFileName;
	CString m_strBaseAdd;
	CString m_strPath;
	void GetBaseAddress();
	void LoadFiles(PSTR strPath);
	void EnumProcessModules(LPCTSTR szProcessName);
	//void Start(void*);
	std::vector <CString> m_vDllName;
	std::vector <CString> m_vDllPath;
	std::vector <CString> m_vDllBase;
	std::vector <CString> m_vProcessName;

	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonRun();
	BOOL m_bShowAll;
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	BOOL m_bStop;
};
