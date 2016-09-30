// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_SCANHISTORY_H__8AE4872B_AD33_11D1_821E_00A0C95C0756__INCLUDED_)
#define AFX_SCANHISTORY_H__8AE4872B_AD33_11D1_821E_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScanHistory.h : header file
//

#include "WizardPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CScanHistory dialog

class CScanHistory : public CWizardPage
{
	DECLARE_DYNCREATE(CScanHistory)

private:
	HWND	      m_hChildWnd;
	CResultsView *m_pResultsViewDlg;

protected:
	void SizePage( int cx, int cy );
	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM wParam, LPARAM );
	LRESULT OnReadLogs( WPARAM, LPARAM );

// Construction
public:
	CScanHistory();
	~CScanHistory();

// Dialog Data
	//{{AFX_DATA(CScanHistory)
	enum { IDD = IDD_SCAN_HISTORY };
	CStatic	m_ctlTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanHistory)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScanHistory)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANHISTORY_H__8AE4872B_AD33_11D1_821E_00A0C95C0756__INCLUDED_)
