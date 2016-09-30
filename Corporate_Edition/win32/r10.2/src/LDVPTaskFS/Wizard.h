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

#define UWM_DESTROYING	6324

#include "ClientPropSheet.h"
#include "MscFuncs.h"

//Include files for pages
#include "ScanFloppyPage.h"
#include "ScanSelectedPage.h"
#include "QuickScanPage.h"
#include "FullScanPage.h"
#include "ScanCustomPage.h"
#include "ScanPCHealthPage.h"
#include "SaveTask.h"
#include "SelectScanTypePage.h"

//Include file for scheduled scans
#include "ScheduleOptionspage.h"

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
	CScanFloppyPage			scanFloppyPage;
	CScanSelectedPage		scanSelectedPage;
	CQuickScanPage			quickScanPage;
	CFullScanPage			fullScanPage;
	CScanCustomPage			scanCustomPage;
	CSaveTask				scanSavePage;
	CScanPCHealthPage		scanPCHealthPage;
	CSelectScanTypePage		selectScanTypePage;
	//Schedule pages
	CScheduleOptionsPage	schedOptionsPage;
	BOOL					m_bInitialized;

	//Pure virtual method implementations
	BOOL Initialized(){ return m_bInitialized; };
	BOOL IsWizard(){ return TRUE; }
	BOOL ValidateSettings();

	void OnDeleteScan();

// Construction
public:
	CWizard( long ID, CWnd* pParentWnd = NULL );
	virtual ~CWizard();


	void Scan( IScanCallback* pCallback = NULL );

// Attributes
public:
	DWORD	m_dwSelectedScanType;
// Public Data
public:
	CPropertyPage *m_pSelectedScanTypePage;
// Operations
public:
	void AddSelectedScanTypePage();
	void RemoveSelectedScanTypePage();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizard)
	public:
	virtual void WinHelpInternal(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

	// Generated message map functions
	void ShowWindow(int nCmdShow);
	CWnd* SetFocus();

protected:
	
	// Generated message map functions
	//{{AFX_MSG(CWizard)
	afx_msg void OnDestroy();
	//}}AFX_MSG

	//Overrides
	BOOL OnInitDialog();
	void OnClose();

// Implementation
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

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
