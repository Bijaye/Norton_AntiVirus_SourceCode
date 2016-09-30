// HomePageConfigDlg.h : header file
//

#pragma once


// CHomePageConfigDlg dialog
class CHomePageConfigDlg : public CDialog
{
// Construction
public:
	CHomePageConfigDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HOMEPAGECONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bLocal;
	BOOL m_bCurrent;
	CString m_strLocal;
	CString m_strCurrent;
	void LoadHomePageValues();
	void SetHomePageValues();
	
	afx_msg void OnBnClickedCheckLocal();
	afx_msg void OnBnClickedCheckCurrent();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonRefresh();
	int m_iNum;
	BOOL m_bStress;
	afx_msg void OnBnClickedCheckStress();
};
