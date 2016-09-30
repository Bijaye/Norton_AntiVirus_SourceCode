// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//{{AFX_INCLUDES()
#include "ldvpschedule.h"
//}}AFX_INCLUDES
#if !defined(AFX_SCHEDULEDIALOG_H__16721263_021D_11D2_B7C5_00A0C99C7131__INCLUDED_)
#define AFX_SCHEDULEDIALOG_H__16721263_021D_11D2_B7C5_00A0C99C7131__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "vpcommon.h"
#include "resource.h"

// ScheduleDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScheduleDialog dialog

class CScheduleDialog : public CDialog
{
private:
	IConfig *m_ptrConfig;
	CString m_strTitle;
	DWORD	m_dwScheduleType;
	DWORD	m_dwPageType;

// Construction
public:
	CScheduleDialog(IConfig *pConfig, CWnd* pParent = NULL);
	~CScheduleDialog();

// Dialog Data
	//{{AFX_DATA(CScheduleDialog)
	enum { IDD = IDD_SCHEDULE_DIALOG };
	CLDVPSchedule	m_ctlSchedule;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScheduleDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScheduleDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEDIALOG_H__16721263_021D_11D2_B7C5_00A0C99C7131__INCLUDED_)
