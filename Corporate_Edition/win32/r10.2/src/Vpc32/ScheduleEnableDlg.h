// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_SCHEDULEENABLEDLG_H__6DDA6F20_1E5D_11D3_8F8D_006008AEC402__INCLUDED_)
#define AFX_SCHEDULEENABLEDLG_H__6DDA6F20_1E5D_11D3_8F8D_006008AEC402__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScheduleEnableDlg.h : header file
//

#include "ConfigObj.h"

//
// Types
//
enum SCHEDULE_FREQUENCY
{
	SCHEDULE_NONE,
	SCHEDULE_DAILY,
	SCHEDULE_WEEKLY,
	SCHEDULE_MONTHLY,
};

enum SCHEDULE_DAY_WHEN
{
	SCHEDULE_SUNDAY = 0,
	SCHEDULE_MONDAY,
	SCHEDULE_TUESDAY,
	SCHEDULE_WEDNESDAY,
	SCHEDULE_THURSDAY,
	SCHEDULE_FRIDAY,
	SCHEDULE_SATURDAY
};

/////////////////////////////////////////////////////////////////////////////
// CScheduleEnableDlg dialog

class CScheduleEnableDlg : public CDialog
{
private:
	IConfig     *m_ptrConfig;
    CConfigObj  m_ConfigObject;
	bool		m_bReadonly;
	
	void DayFromID(DWORD id, TCHAR* szBuf);
// Construction
public:
	CScheduleEnableDlg(IConfig *pConfig, CWnd* pParent = NULL);   // standard constructor
    ~CScheduleEnableDlg();

	void SetReadonly(bool bReadonly) { m_bReadonly = bReadonly; };

// Dialog Data
	//{{AFX_DATA(CScheduleEnableDlg)
	enum { IDD = IDD_SCHEDULE_ENABLE_DIALOG };
	CButton	m_ctlScheduleBtn;
	CEdit	m_ctlDescription;
	BOOL	m_bScheduleEnable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleEnableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bNormalUser;
    void GetScanDescription(CString &sScanDescription);

	// Generated message map functions
	//{{AFX_MSG(CScheduleEnableDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSchedule();
	afx_msg void OnEnableScheduling();
	afx_msg void ProtectScheduling();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEENABLEDLG_H__6DDA6F20_1E5D_11D3_8F8D_006008AEC402__INCLUDED_)
