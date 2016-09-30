// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_VIRUSHISTORYPAGE_H__494AC229_7180_11D1_81CC_00A0C95C0756__INCLUDED_)
#define AFX_VIRUSHISTORYPAGE_H__494AC229_7180_11D1_81CC_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VirusHistoryPage.h : header file
//

#include "WizardPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CVirusHistoryPage dialog

class CVirusHistoryPage : public CWizardPage
{
	DECLARE_DYNCREATE(CVirusHistoryPage)

private:
	HWND          m_hChildWnd;
	CResultsView *m_pResultsViewDlg;

protected:
	void SizePage( int cx, int cy );
	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM wParam, LPARAM );
	LRESULT OnReadLogs( WPARAM, LPARAM );

// Construction
public:
	CVirusHistoryPage();
	~CVirusHistoryPage();

// Dialog Data
	//{{AFX_DATA(CVirusHistoryPage)
	enum { IDD = IDD_VIRUS_HISTORY };
	CStatic	m_ctlTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CVirusHistoryPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CVirusHistoryPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUSHISTORYPAGE_H__494AC229_7180_11D1_81CC_00A0C95C0756__INCLUDED_)
