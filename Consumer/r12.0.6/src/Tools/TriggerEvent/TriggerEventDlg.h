// TriggerEventDlg.h : header file
//

#if !defined(AFX_TRIGGEREVENTDLG_H__C5880BA7_561F_11D5_AE6C_00E02995804B__INCLUDED_)
#define AFX_TRIGGEREVENTDLG_H__C5880BA7_561F_11D5_AE6C_00E02995804B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTriggerEventDlg dialog

class CTriggerEventDlg : public CDialog
{
// Construction
public:
	CTriggerEventDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTriggerEventDlg)
	enum { IDD = IDD_TRIGGEREVENT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriggerEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTriggerEventDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnFire();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGGEREVENTDLG_H__C5880BA7_561F_11D5_AE6C_00E02995804B__INCLUDED_)
