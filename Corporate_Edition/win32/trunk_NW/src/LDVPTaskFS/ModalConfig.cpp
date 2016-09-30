// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: ModalConfig.Cpp
//  Purpose: Definition of the Modal Config sheet
//
//	Date: 3-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "ModalConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CModalConfig, CClientPropSheet)

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CModalConfig::CModalConfig(UINT nIDCaption, DWORD dwPagemask, IConfig *ptrScan, CWnd* pParentWnd, UINT iSelectPage)
	:CClientPropSheet(nIDCaption, pParentWnd, iSelectPage, ptrScan)
{
	CString strTaskName = ((CLDVPTaskFSCtrl*)pParentWnd)->GetTaskName();

	m_psh.dwFlags |= PSH_HASHELP |	PSH_NOAPPLYNOW;
	
	m_bInitialized = FALSE;

	if( dwPagemask & PAGE_SCAN_SELECTED )
	{
		AddPage ( &pageScanSelected );
		pageScanSelected.m_strTaskName = strTaskName;
	}
	
	if( dwPagemask & PAGE_QUICK_SCAN )
	{
		AddPage ( &pageQuickScan );
		pageScanSelected.m_strTaskName = strTaskName;
	}
	
	if( dwPagemask & PAGE_FULL_SCAN )
	{
		AddPage ( &pageFullScan );
		pageScanSelected.m_strTaskName = strTaskName;
	}
	
	if( dwPagemask & PAGE_SCHEDULE )
	{
		AddPage ( &pageSchedule );
		pageSchedule.m_strTaskName = strTaskName;
	}

	if( dwPagemask & PAGE_SAVE_TASK )
	{
		AddPage ( &pageSaveTask );
		pageSaveTask.m_strTaskName = strTaskName;
	}
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CModalConfig::~CModalConfig()
{
}

//----------------------------------------------------------------
// Message Map
//----------------------------------------------------------------
BEGIN_MESSAGE_MAP(CModalConfig, CPropertySheet)
	//{{AFX_MSG_MAP(CModalConfig)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND( ID_HELP, OnHelp )
END_MESSAGE_MAP()

BOOL CModalConfig::OnInitDialog() 
{
	BOOL bResult = CClientPropSheet::OnInitDialog();
	
	m_bInitialized = TRUE;

	return bResult;
}


/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
