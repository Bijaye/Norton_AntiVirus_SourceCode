// ScanTaskTestDlg.h : header file
//

#if !defined(AFX_SCANTASKTESTDLG_H__8DE683B9_7EDB_42ED_A28C_B81B5C451246__INCLUDED_)
#define AFX_SCANTASKTESTDLG_H__8DE683B9_7EDB_42ED_A28C_B81B5C451246__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ScriptSafe.h"
#include "NAVTasks.h"


/////////////////////////////////////////////////////////////////////////////
// CScanTaskTestDlg dialog

class CScanTaskTestDlg : public CDialog
{
// Construction
public:
	CScanTaskTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScanTaskTestDlg)
	enum { IDD = IDD_SCANTASKTEST_DIALOG };
	CString	m_strAllUsers;
	CString	m_strCurrentUser;
	CString	m_strScanDateRegKey;
	CString	m_strScanDateTask;
    CString m_strNSWElapsed;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanTaskTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ResetScanTasks();
	void RefreshScanDates();
	void SetMyCompScanTime (time_t &timeLastRun);
    void LoadAType( long* plScanType );
    DWORD GetDaysElapsed(SYSTEMTIME* pstStart);

    CComPtr <INAVScanTasks> m_spTasks;
    CComPtr <ISymScriptSafe> m_spSymScriptSafe;

	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScanTaskTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMycomputer();
	afx_msg void OnDeleteall();
	afx_msg void OnNew();
	afx_msg void OnScan();
	afx_msg void OnScheduleMycomp();
	afx_msg void OnDelete();
	afx_msg void OnFakeMyComp();
	afx_msg void OnOldScan();
	afx_msg void OnResetFss();
	afx_msg void OnEdit();
	afx_msg void OnShowMyCompSched();
	afx_msg void OnLoadFolder();
	afx_msg void OnLoadFiles();
	afx_msg void OnLoadMyComp();
	afx_msg void OnLoadDrives();
    afx_msg void JustScanTask();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANTASKTESTDLG_H__8DE683B9_7EDB_42ED_A28C_B81B5C451246__INCLUDED_)
