////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TestGLPDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>


// CTestGLPDlg dialog
class CTestGLPDlg : public CDialog
{
// Construction
public:
	CTestGLPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTGLP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CEdit* m_listEdit;
// Implementation
protected:
	HICON m_hIcon;

	afx_msg LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
public:

	BOOL m_bRegistry;
	BOOL m_bRegCheckAll;
	BOOL m_bReg1;
	BOOL m_bReg2;
	BOOL m_bReg3;
	BOOL m_bReg4;
	BOOL m_bReg5;
	BOOL m_bReg6;
	BOOL m_bReg7;
	BOOL m_bReg8;
	BOOL m_bReg9;
	BOOL m_bReg10;
	BOOL m_bReg11;
	BOOL m_bReg12;
	BOOL m_bReg13;
	BOOL m_bReg14;
	BOOL m_bReg15;
	BOOL m_bReg16;
	BOOL m_bReg17;
	BOOL m_bReg18;
	BOOL m_bReg19;
	BOOL m_bReg20;
	BOOL m_bReg21;
	BOOL m_bReg22;
	BOOL m_bReg23;
	BOOL m_bReg24;
	BOOL m_bReg25;
	BOOL m_bReg26;
	BOOL m_bReg27;
	BOOL m_bReg28;
	BOOL m_bReg29;
	BOOL m_bReg30;
	BOOL m_bReg31;
	BOOL m_bReg32;

	BOOL m_bStartup;
	//BOOL m_bStartCheckAll;
	//BOOL m_bStart1;
	//BOOL m_bStart2;
	//BOOL m_bStart3;

	CString m_strFullVirusName;
	CString m_strVirusName;
	CString m_editStartup;

	afx_msg void OnBnClickedModify();
	afx_msg void OnBnClickedCheckRegistry();
	afx_msg void OnBnClickedCheckAllreg();
	afx_msg void OnBnClickedCheckStartup();
	//afx_msg void OnBnClickedCheckStart1();
	//afx_msg void OnBnClickedCheckStart2();
	//afx_msg void OnBnClickedCheckStart3();
	
	afx_msg void OnBnClickedCheckAllstartup();

	CString GetVirusName();
	CString GenerateUID();
	BOOL ModifyRegistry(HKEY basekey,CString strSubKey);
	//BOOL ModifySubKeys(HKEY basekey, CString strSubKey);
	BOOL ModifyUserKeys(HKEY basekey, CString strSubKey);
	BOOL ModifyStartUpFodlers();
	BOOL ModifyBatchFile(CString strFileName);
	BOOL ModifyINIFile(CString strFileName);
	CListCtrl m_ListStartup;
	std:: vector <CString> m_strVector;
	std:: vector <CString> m_strRegVector;
	std:: vector <HKEY> m_strKeyBase;
	std:: vector <CString> m_strKeyPath;
	std:: vector <CString> m_strRegName;
	void GetStartupFolders();
	void UpdateStartupFolders();
	
	BOOL m_bAUTONT;
	BOOL m_bWINBAT;
	BOOL m_bAUTOBAT;
	BOOL m_bSYS;
	BOOL m_bWININI;
	afx_msg void OnBnClickedCheckFiles();
	BOOL m_bFiles;
	int m_iTotal;
	CString m_strResult;
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedSave();
	BOOL m_bReg33;
	BOOL m_bReg34;
	BOOL m_bReg35;
	BOOL m_bReg36;
	BOOL m_bReg37;
	BOOL m_bReg38;
};
