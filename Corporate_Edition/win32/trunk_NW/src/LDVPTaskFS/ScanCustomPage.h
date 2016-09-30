// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// CustomPage.h : header file
//
#if !defined (SCAN_CUSTOM_INCLUDED)
#define SCAN_CUSTOM_INCLUDED

#include "WizardPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CScanCustomPage dialog

class CScanCustomPage : public CWizardPage
{
	DECLARE_DYNCREATE(CScanCustomPage)
private:
	DWORD	m_dwDay,
			m_dwSchedule;
	CString	m_strTime;
	BOOL	m_bDeleted;

	void OnDestroying();
	void OnDeleteTask();

// Construction
public:
	CScanCustomPage();
	~CScanCustomPage();

	void OnMscButton();

// Dialog Data
	//{{AFX_DATA(CScanCustomPage)
	enum { IDD = IDD_CUSTOM_SCAN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanCustomPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScanCustomPage)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void GetScheduleInfo( CString &strSchedule );
	BOOL OnKillActive();

};

#endif
