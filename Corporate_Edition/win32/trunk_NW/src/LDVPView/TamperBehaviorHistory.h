// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#pragma once

#include "WizardPage.h"
#include "Resource.h"

// CTamperBehaviorHistory dialog

class CTamperBehaviorHistory : public CWizardPage
{
	DECLARE_DYNAMIC(CTamperBehaviorHistory)

private:
	HWND          m_hChildWnd;
	CResultsView *m_pResultsViewDlg;

protected:
	void SizePage( int cx, int cy );
	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM wParam, LPARAM );
	LRESULT OnReadLogs( WPARAM, LPARAM );

public:
	CTamperBehaviorHistory();
	virtual ~CTamperBehaviorHistory();

// Dialog Data
	enum { IDD = IDD_TAMPERBEHAVIOR_HISTORY };
	CStatic	m_ctlTitle;

public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
