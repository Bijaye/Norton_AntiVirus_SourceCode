// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ScanResultsPage.h
//  Purpose: CScanResults Definition file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined(SCAN_RESULTS_INCLUDED)
#define SCAN_RESULTS_INCLUDED

#include "Ldvptaskfs.h"
#include "WizardPage.h"
#include "PreparingScan.h"
#include "ResultsHolder.h"

//----------------------------------------------------------------
//
// CScanResultsPage class
//
//----------------------------------------------------------------
class CScanResultsPage : public CWizardPage
{
	DECLARE_DYNCREATE(CScanResultsPage)
private:

	IScan *m_pScan;

	void OnDestroying();

// Construction
public:
	CScanResultsPage();
	virtual ~CScanResultsPage();

	long OnResultsDlgClosing( WPARAM, LPARAM );


protected:
	BOOL OnSetActive();
	BOOL OnWizardFinish();

	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CScanResultsPage)
	enum { IDD = IDD_SCAN_RESULTS };
	CStatic	m_ctlTitle;
	CResultsHolder	m_resultsPH;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanResultsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScanResultsPage)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
