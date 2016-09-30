// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScheduleOptionsPage.h : header file
//
//{{AFX_INCLUDES()
#include "ldtime.h"
#include "ldvpschedule.h"
#include "ldvpschedule2.h"
//}}AFX_INCLUDES
#if !defined ( SCHEDULE_OPTIONS_1_INCLUDED )
#define SCHEDULE_OPTIONS_1_INCLUDED

#include "ClientPropSheet.h"

#include "WizardPage.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CScheduleOptionsPage dialog

//class CScheduleOptionsPage : public CPropertyPage
class CScheduleOptionsPage : public CWizardPage
{
	DECLARE_DYNCREATE(CScheduleOptionsPage)
private:
	BOOL		m_bInitialized;
    BOOL        m_useResumableScan;
	BOOL		m_scheduleControlInitialized;

	void SaveOptions();

// Construction
public:

	CScheduleOptionsPage();
	~CScheduleOptionsPage();


// Dialog Data
	//{{AFX_DATA(CScheduleOptionsPage)
	enum { IDD = IDD_SCHEDULE_PAGE };
	// CLDVPSchedule handles service scheduling and scan scheduling when resumable scan feature is disabled.
	// CLDVPSchedule2 is ONLY used for scan scheduling when resumable UI is enabled.
    CLDVPSchedule*   m_poCtlSchedule;
	CLDVPSchedule2*	m_poCtlScheduleResumable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScheduleOptionsPage)
	public:
//	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScheduleOptionsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
