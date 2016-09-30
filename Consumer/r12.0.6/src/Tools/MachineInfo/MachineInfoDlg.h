// MachineInfoDlg.h : header file
//

#if !defined(AFX_MACHINEINFODLG_H__3C053E19_2F58_4DE4_8B76_6CEFDFB7994D__INCLUDED_)
#define AFX_MACHINEINFODLG_H__3C053E19_2F58_4DE4_8B76_6CEFDFB7994D__INCLUDED_

#include "MSSystemInfo.h"	// Added by ClassView
#include "ActivityLogReport.h"
#include "SymRegKeys.h"
#include "DebugLogs.h"
#include "LiveUpdateLogs.h"
#include "NAVOptionsReport.h"
#include "NAVFileVersions.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMachineInfoDlg dialog

class CMachineInfoDlg : public CDialog
{
// Construction
public:
	CMachineInfoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMachineInfoDlg)
	enum { IDD = IDD_MACHINEINFO_DIALOG };
	CButton	m_btnNAVFileVersions;
	CButton	m_btnNAVOptions;
	CButton	m_btnLiveUpdateLogs;
	CButton	m_btnDebugLogs;
	CButton	m_btnSymRegKeys;
	CButton	m_btnActivityLog;
	CButton	m_btnStart;
	CButton	m_btnSystemInfo;
	CString	m_strSavePath;
	BOOL	m_bActivityLog;
	BOOL	m_bDebugLogs;
	BOOL	m_bNAVFileVersions;
	BOOL	m_bSystemInfo;
	BOOL	m_bSymRegKeys;
	BOOL	m_bLiveUpdateLogs;
	BOOL	m_bNAVOptions;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMachineInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMSSystemInfo       m_MSSystemInfo;
    CActivityLogReport  m_ActivityLog;
    CSymRegKeys         m_SymRegKeys;
    CDebugLogs          m_DebugLogs;
    CLiveUpdateLogs     m_LiveUpdateLogs;
    CNAVOptionsReport   m_NAVOptions;
    CNAVFileVersions    m_NAVFileVersions;

	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMachineInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MACHINEINFODLG_H__3C053E19_2F58_4DE4_8B76_6CEFDFB7994D__INCLUDED_)
