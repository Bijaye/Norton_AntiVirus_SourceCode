// LUTestInstallDlg.h : header file
//

#if !defined(AFX_LUTESTINSTALLDLG_H__0DF88C6C_7A9D_479F_9B1E_4B97FA66C24C__INCLUDED_)
#define AFX_LUTESTINSTALLDLG_H__0DF88C6C_7A9D_479F_9B1E_4B97FA66C24C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLUTestInstallDlg dialog

class CLUTestInstallDlg : public CDialog
{
// Construction
public:
	CLUTestInstallDlg(CWnd* pParent = NULL);	// standard constructor
    virtual ~CLUTestInstallDlg ();

// Dialog Data
	//{{AFX_DATA(CLUTestInstallDlg)
	enum { IDD = IDD_LUTESTINSTALL_DIALOG };
	CString	m_strLog;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLUTestInstallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool closeMutex();
	TCHAR m_szMutexName [100];
	HANDLE m_hALUMutex;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLUTestInstallDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCheckState();
	afx_msg void OnCloseMutex();
	afx_msg void OnCreateMutex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUTESTINSTALLDLG_H__0DF88C6C_7A9D_479F_9B1E_4B97FA66C24C__INCLUDED_)
