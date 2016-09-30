// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//  File: QuickScanPage.h
//  Purpose: CQuickScanPage definition file. Implements the 
//			QuickScan feature, which does a generic loadpoint scanning.
//
//	Revisions: 
//	Srikanth Vudathala	- Class created	- 10\10\2004
//----------------------------------------------------------------
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined (QUICK_SCAN_INCLUDED)
#define QUICK_SCAN_INCLUDED

#include "WizardPage.h"
#include "Resource.h"

//----------------------------------------------------------------
//
// CQuickScanPage class
//
//----------------------------------------------------------------
class CQuickScanPage : public CWizardPage
{
	DECLARE_DYNCREATE(CQuickScanPage)

private:
	BOOL m_bInitialized;

	void StoreOptions();

//Construction
public:
	CQuickScanPage();
	~CQuickScanPage();

	void SizePage(int cx, int cy);

//Dialog Data
	//{{AFX_DATA(CQuickScanPage)
	enum { IDD = IDD_QUICK_SCAN };
	//}}AFX_DATA


//Overrides
	//ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQuickScanPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CQuickScanPage)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptions();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
