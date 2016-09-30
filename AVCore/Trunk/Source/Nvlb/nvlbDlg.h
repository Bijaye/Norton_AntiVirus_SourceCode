// nvlbDlg.h : header file
//

#if !defined(AFX_NVLBDLG_H__84268B48_0D11_11D3_8B45_00104B9DFF82__INCLUDED_)
#define AFX_NVLBDLG_H__84268B48_0D11_11D3_8B45_00104B9DFF82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNvlbDlg dialog

class CNvlbDlg : public CDialog
{

private:
	HACCEL	m_hAccel;

// Construction
public:
	void CreateVirLib( LPSTR lpszFile);
	CNvlbDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNvlbDlg)
	enum { IDD = IDD_NVLB_DIALOG };
	CStatic	m_Heur_Setting;
	CStatic	m_Virlib_Dir;
	CStatic	m_Inf_Path;
	CStatic	m_Incoming_Dir;
	CStatic	m_Defs_Dir;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNvlbDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNvlbDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonExit();
	afx_msg void OnBuildLibrary();
	afx_msg void OnDefsDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NVLBDLG_H__84268B48_0D11_11D3_8B45_00104B9DFF82__INCLUDED_)
