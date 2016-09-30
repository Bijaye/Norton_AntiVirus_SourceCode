#if !defined(AFX_MONITORSETUPDIALOG_H__8FD8D426_D89E_11D2_B98E_0004ACEC31AA__INCLUDED_)
#define AFX_MONITORSETUPDIALOG_H__8FD8D426_D89E_11D2_B98E_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonitorSetupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonitorSetupDialog dialog

class CMonitorSetupDialog : public CDialog
{
// Construction
public:
	CMonitorSetupDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMonitorSetupDialog)
	enum { IDD = IDD_DIALOG_MONITOR_SETTINGS };
	BOOL	m_autoRestart;
	UINT	m_StatusCheckInterval;
	UINT	m_AutoTerminateErrorCount;
	BOOL	m_startOnMonitorStartup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonitorSetupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMonitorSetupDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONITORSETUPDIALOG_H__8FD8D426_D89E_11D2_B98E_0004ACEC31AA__INCLUDED_)
