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
#include "LDVPView.h"
#include "ModalConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CModalConfig, CPropertySheet)

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CModalConfig::CModalConfig(UINT nIDCaption, DWORD dwPagemask, CWnd* pParentWnd, UINT iSelectPage)
	:CLDVPPropSheet(nIDCaption)
{

	m_psh.dwFlags |= PSH_HASHELP;

	//If they asked for the RTS page, give it to them
	if( dwPagemask & TASK_ID_RTS )
	{
		AddPage ( &m_pageRTSTask );
		m_pageRTSTask.m_strTaskName = ((CLDVPViewCtrl*)pParentWnd)->GetTaskName();
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

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
