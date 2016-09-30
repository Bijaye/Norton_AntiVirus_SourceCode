// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_REPAIREDITEMSPAGE_H__A53D35D0_290A_11D3_919F_00C04F980248__INCLUDED_)
#define AFX_REPAIREDITEMSPAGE_H__A53D35D0_290A_11D3_919F_00C04F980248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepairedItemsPage.h : header file
//

#include "WizardPage.h"

/////////////////////////////////////////////////////////////////////////////
// CRepairedItemsPage dialog

class CRepairedItemsPage : public CWizardPage
{

    DECLARE_DYNCREATE(CRepairedItemsPage)

// Construction
public:
	CRepairedItemsPage();   // standard constructor
  	virtual ~CRepairedItemsPage();

// Dialog Data
	//{{AFX_DATA(CRepairedItemsPage)
	enum { IDD = IDD_REPAIRED_ITEMS };
	CStatic	m_ctlTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepairedItemsPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CheckDefwatchKeys();
	BOOL m_bCheckDefwatchKeys;

	void SizePage( int cx, int cy );
	long OnResultsDlgClosing( WPARAM, LPARAM );
	long OnCreateScanDlg( WPARAM wParam, LPARAM );
	LRESULT OnFillBin( WPARAM, LPARAM );
    LRESULT AddItemToView( IVBin2 *ptrVBin, VBININFO *vBinInfo );

	// Generated message map functions
	//{{AFX_MSG(CRepairedItemsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CResultsView *m_pResultsViewDlg;
	HWND          m_hChildWnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPAIREDITEMSPAGE_H__A53D35D0_290A_11D3_919F_00C04F980248__INCLUDED_)
