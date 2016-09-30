// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_EVENTLOGPAGE_H__494AC228_7180_11D1_81CC_00A0C95C0756__INCLUDED_)
#define AFX_EVENTLOGPAGE_H__494AC228_7180_11D1_81CC_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EventLogPage.h : header file
//
#include "WizardPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CEventLogPage dialog

class CEventLogPage : public CWizardPage
{
	DECLARE_DYNCREATE(CEventLogPage)
private:
	HWND		  m_hChildWnd;
	CResultsView *m_pResultsViewDlg;

protected:
	void SizePage( int cx, int cy );
	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM wParam, LPARAM );
	LRESULT OnReadLogs( WPARAM, LPARAM );

// Construction
public:
	CEventLogPage();
	~CEventLogPage();

// Dialog Data
	//{{AFX_DATA(CEventLogPage)
	enum { IDD = IDD_EVENT_LOG };
	CStatic	m_ctlTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEventLogPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEventLogPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTLOGPAGE_H__494AC228_7180_11D1_81CC_00A0C95C0756__INCLUDED_)
