// VDBUnpackerTestDlg.h : header file
//

#if !defined(AFX_VDBUNPACKERTESTDLG_H__C16A2F7A_A80E_11D3_8ADC_CECFC7000000__INCLUDED_)
#define AFX_VDBUNPACKERTESTDLG_H__C16A2F7A_A80E_11D3_8ADC_CECFC7000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVDBUnpackerTestDlg dialog

class CVDBUnpackerTestDlg : public CDialog
{
// Construction
public:
	CVDBUnpackerTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVDBUnpackerTestDlg)
	enum { IDD = IDD_VDBUNPACKERTEST_DIALOG };
	CString	m_logFile;
	CString	m_package;
	CString	m_result;
	CString	m_targetDir;
	CString	m_traceFile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVDBUnpackerTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVDBUnpackerTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnUnpack();
	afx_msg void OnPrune();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VDBUNPACKERTESTDLG_H__C16A2F7A_A80E_11D3_8ADC_CECFC7000000__INCLUDED_)
