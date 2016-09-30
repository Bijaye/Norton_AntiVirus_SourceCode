// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ModalConfig.H
//  Purpose: Definition of the Modal Config sheet
//
//	Date: 3-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#include "ClientPropSheet.h"
#include "ScheduleOptionsPage.h"
#include "SaveTask.h"
#include "ScanSelectedPage.h"
#include "QuickScanPage.h"
#include "FullScanPage.h"


//----------------------------------------------------------------
//
// class CModalConfig
//
//----------------------------------------------------------------
class CModalConfig : public CClientPropSheet
{
	DECLARE_DYNAMIC(CModalConfig)

private:

	CScheduleOptionsPage	pageSchedule;
	CSaveTask				pageSaveTask;
	CScanSelectedPage		pageScanSelected;
	CQuickScanPage			pageQuickScan;
	CFullScanPage			pageFullScan;

	BOOL					m_bInitialized;

	//Pure virtual method implementations
	BOOL Initialized(){ return m_bInitialized; };
	BOOL IsWizard(){ return FALSE; }

// Construction
public:

	CModalConfig(UINT nIDCaption, DWORD PageMask, IConfig *ptrScan, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModalConfig)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModalConfig();

	// Generated message map functions
protected:
	//{{AFX_MSG(CModalConfig)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
