// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_PAUSEDIALOG_H__6B999893_FD28_443A_AB1A_BD74BC183D63__INCLUDED_)
#define AFX_PAUSEDIALOG_H__6B999893_FD28_443A_AB1A_BD74BC183D63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PauseDialog.h : header file
//

#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CPauseDialog dialog

class CPauseDialog : public CDialog
{
// Construction
public:
	CPauseDialog(CWnd* pParent = NULL);   // standard constructor

	CString strDelayCountText;
	BOOL m_bStopEnabled;
	BOOL m_b4HourEnabled;

// Dialog Data
	//{{AFX_DATA(CPauseDialog)
	enum { IDD = IDD_PAUSE_DIALOG };
	CButton	m_ctl4HourSnooze;
	CButton	m_ctlStopButton;
	CStatic	m_ctlDelayCountText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPauseDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPauseDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPauseContinue();
	afx_msg void OnPauseDelay1Hour();
	afx_msg void OnPauseDelay4Hour();
	afx_msg void OnPausePause();
	afx_msg void OnPauseStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAUSEDIALOG_H__6B999893_FD28_443A_AB1A_BD74BC183D63__INCLUDED_)
