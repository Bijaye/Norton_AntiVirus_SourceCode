// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ScanSelectedPage.h
//  Purpose: CScanSelectedPage definition file
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

#if !defined (SCAN_SELECTED_INCLUDED)
#define SCAN_SELECTED_INCLUDED

#include "WizardPage.h"
#include "Resource.h"

//----------------------------------------------------------------
//
// CScanSelectedPage class
//
//----------------------------------------------------------------
class CScanSelectedPage : public CWizardPage
{
	DECLARE_DYNCREATE(CScanSelectedPage)

private:
	BOOL m_bInitialized;

	void StoreOptions();

// Construction
public:
	CScanSelectedPage();
	~CScanSelectedPage();

	void SizePage( int cx, int cy );

// Dialog Data
	//{{AFX_DATA(CScanSelectedPage)
	enum { IDD = IDD_SCAN_SELECTED };
	CShelsel2	m_ShellSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanSelectedPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScanSelectedPage)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnOptions();
	afx_msg void OnOnSelectDrives(long itemId);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_EVENTSINK_MAP()
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
