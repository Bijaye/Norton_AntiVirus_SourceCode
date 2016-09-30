// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ScanFloppyPage.h
//  Purpose: ScanFloppy definition file
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
//{{AFX_INCLUDES()
#include "shelsel2.h"
//}}AFX_INCLUDES

#if !defined(SCAN_FLOPPY_INCLUDED)
#define SCAN_FLOPPY_INCLUDED

#include "WizardPage.h"

//----------------------------------------------------------------
//
// CScanFloppyPage class
//
//----------------------------------------------------------------
class CScanFloppyPage : public CWizardPage
{
	DECLARE_DYNCREATE(CScanFloppyPage)
private:
	BOOL m_bEnableTheScanButton;

	void OnDestroying();

	void SizePage( int cx, int cy );

// Construction
public:
	CScanFloppyPage();
	virtual ~CScanFloppyPage();

// Dialog Data
	//{{AFX_DATA(CScanFloppyPage)
	enum { IDD = IDD_SCAN_FLOPPY };
	CShelsel2	m_ShellSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanFloppyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScanFloppyPage)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptions();
	afx_msg void OnOnSelectDrives(long itemId);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG

	BOOL OnSetActive();
	BOOL OnWizardFinish();

	DECLARE_MESSAGE_MAP()

};

#endif

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
