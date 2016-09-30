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

#if !defined(SAVE_TASK_INCLUDED)
#define SAVE_TASK_INCLUDED

#include "WizardPage.h"
#include "Resource.h"

//----------------------------------------------------------------
//
// CScanFloppyPage class
//
//----------------------------------------------------------------
class CSaveTask : public CWizardPage
{
	DECLARE_DYNCREATE(CSaveTask)

// Construction
public:
	CSaveTask();
	~CSaveTask();

// Dialog Data
	//{{AFX_DATA(CSaveTask)
	enum { IDD = IDD_SAVEAS };
	CString	m_strname;
	CString	m_strDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSaveTask)
	public:
	virtual LRESULT OnWizardNext();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSaveTask)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	
	BOOL OnSetActive();
	void SizePage( int cx, int cy );

	DECLARE_MESSAGE_MAP()
};

#endif

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
