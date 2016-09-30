#if !defined(AFX_MACHINESTATISTICS_H__20D416D3_637C_11D2_B975_0004ACEC31AA__INCLUDED_)
#define AFX_MACHINESTATISTICS_H__20D416D3_637C_11D2_B975_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MachineStatistics.h : header file
//

#define STATISTICS_CHECK_TIMERID    101
#define STATISTICS_THREAD_TIMERID   102

#include "ListCtEx.h"
#include "PerformanceMonitor.h"
#include <afxmt.h>
/////////////////////////////////////////////////////////////////////////////
// CMachineStatistics dialog

class CMachineStatistics : public CDialog
{
// Construction
public:
	void SaveToFile(CString fileName);
	CMachineStatistics(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMachineStatistics();

// Dialog Data
	//{{AFX_DATA(CMachineStatistics)
	enum { IDD = IDD_DIALOG_MACHINE_STATISTICS };
	CListCtrlEx	m_MachineListCtrl;
	CString	m_RefreshIntervalString;
	//}}AFX_DATA
    CStringArray m_MachineList;
    CStringArray m_NTMachineNameList;
    CStringArray m_UserList;
    CPerformanceMonitorList m_PerfMonList;
    UINT m_RefreshIntervalValue;
	CWinThread * m_StatThread;
	void DFLauncherRefreshStatistics();
	int m_ThreadCount;
	BOOL m_ThreadStop;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMachineStatistics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMachineStatistics)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeEditRefreshInterval();
	afx_msg void OnClose();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL CheckRSHStatus (CString &machine, CString &user);
	void RefreshDisplay();
	BOOL m_AutoSized;
	CMutex m_Mutex;
};

class CMStatDialogThread : public CWinThread
{
    DECLARE_DYNCREATE(CMStatDialogThread)
    CMStatDialogThread() {};
    virtual BOOL InitInstance();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACHINESTATISTICS_H__20D416D3_637C_11D2_B975_0004ACEC31AA__INCLUDED_)
