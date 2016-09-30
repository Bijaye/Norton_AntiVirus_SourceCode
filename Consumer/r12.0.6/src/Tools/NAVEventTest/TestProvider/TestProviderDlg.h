#pragma once

#include "TestProviderPro.h"
#include "AllNAVEvents.h"

class CTestProviderDlg : public CDialog
{
// Construction
public:
	CTestProviderDlg(CWnd* pParent = NULL);	// standard constructor
	bool m_bTimer;
	void PeekIt();
protected:
    CTestProviderPro m_Provider;
	CEventEx* m_pEvent;

// Dialog Data
	//{{AFX_DATA(CTestProviderDlg)
	enum { IDD = IDD_TESTPROVIDER_DIALOG };
	long	m_nProviderId;
	long	m_nTestValue;
	long	m_nEventId;
	BOOL	m_bBroadcast;
	long	m_nCancelledId;
	CString m_strDBCSName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestProviderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestProviderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	afx_msg void OnEventErrorButton();
    afx_msg void OnVirusAlertEventButton();
	afx_msg void OnRegisterButton();
	afx_msg void OnUnregisterButton();
	virtual void OnCancel();
	afx_msg void OnConnectButton();
	afx_msg void OnVirusLikeButton();
	afx_msg void OnSbButton();
	afx_msg void OnScanButton();
	afx_msg void OnButtonAll();
	afx_msg void OnBroadcastCheck();
	afx_msg void OnButtonTimer();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonStop();
	//}}AFX_MSG
	afx_msg LRESULT OnEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShutdown(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

