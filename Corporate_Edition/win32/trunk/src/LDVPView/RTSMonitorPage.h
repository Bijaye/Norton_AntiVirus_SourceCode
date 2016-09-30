// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_TASK1PAGE_H__1651B0DD_4671_11D1_818B_00A0C95C0756__INCLUDED_)
#define AFX_TASK1PAGE_H__1651B0DD_4671_11D1_818B_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Task1Page.h : header file
//

#include "WizardPage.h"
#include "StaticPath.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CRTSMonitor dialog

class CRTSMonitor : public CWizardPage
{
	DECLARE_DYNCREATE(CRTSMonitor)

private:
	IRealTimeScan *m_ptrRts;

// Construction
public:
	CRTSMonitor();
	~CRTSMonitor();

// Dialog Data
	//{{AFX_DATA(CRTSMonitor)
	enum { IDD = IDD_RTS_MONITOR };
	//CStaticPath	m_ctlLocation;
	//CStaticPath	m_ctlLastVirusFile;
	CEdit	m_ctlLocation;
	CEdit	m_ctlLastVirusFile;
	CString	m_strLastFileScanned;
	CString	m_strLastUser;
	CString	m_strLastAction;
	CString	m_strLastVirusFound;
	CString	m_strTimeStarted;
	CString	m_strTotalScanned;
	CString	m_strVirusesFound;
	CString	m_strTimeOfLastInfection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRTSMonitor)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL ShowWindow(int nCmdShow);
	// Generated message map functions
	//{{AFX_MSG(CRTSMonitor)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SizePage( int cx, int cy );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASK1PAGE_H__1651B0DD_4671_11D1_818B_00A0C95C0756__INCLUDED_)
