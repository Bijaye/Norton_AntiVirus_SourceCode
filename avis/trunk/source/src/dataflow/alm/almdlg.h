// ALMDlg.h : header file
//

#if !defined(AFX_ALMDLG_H__99598B38_35E6_11D2_A579_0004ACECC1E1__INCLUDED_)
#define AFX_ALMDLG_H__99598B38_35E6_11D2_A579_0004ACECC1E1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define TIMER_INTERVAL 125
/////////////////////////////////////////////////////////////////////////////
// CALMDlg dialog
class CALMDlg : public CDialog
{
// Construction
public:
	CALMDlg(CWnd* pParent = NULL);	// standard constructor
    afx_msg void OnTimer(UINT);
	int GetCurSel(void);
    DFSample * FindSampleObject(int& nSel, POSITION& pos);
	int SubmitJob(CString cookieId, int state);
	int RequestJobStatus(CString cookieId , int state);
	int FindSampleInList(DFSample *sample);
	int IgnoreJob(CString cookieId, int state);
	void OnManualTimer();
	int  UpdateSampleList();
    int  OnJobFromLauncher(WPARAM p1, LPARAM p2);

	// Testing methods
    int OnJobAccept(WPARAM p1, LPARAM p2 );
	int JobAccept();
	int OnJobEnd(WPARAM p1, LPARAM p2 );
	int JobEnd();
	int OnJobPostpone(WPARAM p1, LPARAM p2 );
	int JobPostpone();
	int OnJobTimeout(WPARAM p1, LPARAM p2 );
	int JobTimeout();
	

public:
	void StartThread();
    DFManager Manager;
	// Dialog Data
	//{{AFX_DATA(CALMDlg)
	enum { IDD = IDD_ALM_DIALOG };
	CDFListCtrl	m_ctlListCtrl;
	DFSample *selectedObj;
		//CDFListCtrl m_ctrlListCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CALMDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

    HWND DFLauncherWnd;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CALMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALMDLG_H__99598B38_35E6_11D2_A579_0004ACECC1E1__INCLUDED_)
