// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//  File: FullScanPage.cpp
//  Purpose: CFullScanPage definition file. Implements the 
//			Full Scan feature, which does a complete scan on the
//			system. 
//			It includes 
//				all the files and folders, 
//				generic loadpoints and extended eraser definitions
//
//	Revisions: 
//	Srikanth Vudathala	- Class created	- 10\10\2004
//----------------------------------------------------------------

//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined (FULL_SCAN_INCLUDED)
#define FULL_SCAN_INCLUDED

#include "WizardPage.h"

//----------------------------------------------------------------
//
// CFullScanPage class
//
//----------------------------------------------------------------
class CFullScanPage : public CWizardPage
{
	DECLARE_DYNCREATE(CFullScanPage)

private:
	BOOL m_bInitialized;

	void StoreOptions();

//Construction
public:
	CFullScanPage();
	~CFullScanPage();

	void SizePage(int cx, int cy);

//Dialog Data
	//{{AFX_DATA(CFullScanPage)
	enum { IDD = IDD_FULL_SCAN };
	//}}AFX_DATA


//Overrides
	//ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFullScanPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//Implementation
protected:
	//Generated message map functions
	//{{AFX_MSG(CFullScanPage)
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
