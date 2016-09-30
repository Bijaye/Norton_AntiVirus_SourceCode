// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: Wizard.h
//  Purpose: CWizard Definition file. Base class for Task wizards
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#if !defined (WIZARD_BASECLASS_INCLUDED)
#define WIZARD_BASECLASS_INCLUDED

#include "ClientPropSheet.h"

//Include files for pages
#include "RTSMonitorPage.h"
#include "SchedulePage.h"
#include "VirusBinPage.h"
#include "VirusHistoryPage.h"
#include "EventLogPage.h"
#include "ScanHistory.h"
#include "BackupPage.h"
#include "RepairedItemsPage.h"
#include "LicensingView.h"
#include "LSLicensingView.h"
#include "TamperBehaviorHistory.h"

//----------------------------------------------------------------
//
// CWizard class
//
//----------------------------------------------------------------
class CWizard : public CClientPropSheet
{
	DECLARE_DYNAMIC(CWizard)


private:
	//Pages
	CRTSMonitor			m_pageRTSMonitor;
	CSchedulePage		m_pageSchedule;
	CVirusBinPage		m_pageVirusBin;
	CVirusHistoryPage	m_pageVirusHistory;
	CEventLogPage		m_pageEventLog;
	CScanHistory		m_pageScanHistory;
    CBackupPage         m_pageBackup;
    CRepairedItemsPage  m_pageRepairedItems;
	CLicensingView		m_pageLicensing;
	CLSLicensingView	m_pageLSLicensing; // for SLICLIB 1.6
	CTamperBehaviorHistory	m_pageTamperBehaviorHistory;
	
	BOOL				m_bInitialized;

	void SetButtonsPos();


// Construction
public:
	CWizard( long ID, CWnd* pParentWnd = NULL );
	virtual ~CWizard();

	BOOL Initialized(){ return m_bInitialized; };
	BOOL IsWizard(){ return TRUE; };


// Attributes
public:

// Operations
public:

// Overrides
	void ShowWindow(int nCmdShow);

	// Generated message map functions
protected:
	
	//Overrides
	BOOL OnInitDialog();
	void OnDestroy();
	void OnClose();
	void OnSetFocus(CWnd* pOldWnd);
	void OnSize(UINT nType, int cx, int cy);
	void OnPaint();

// Implementation
protected:
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
