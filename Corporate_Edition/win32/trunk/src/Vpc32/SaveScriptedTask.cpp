// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: SaveScriptedTask.cpp
//  Purpose: CSaveScriptedTask Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "wprotect32.h"
#include "SaveScriptedTask.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CSaveScriptedTask::CSaveScriptedTask(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveScriptedTask::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveScriptedTask)
	m_scriptedTaskName = _T("");
	m_taskDescription = _T("");
	//}}AFX_DATA_INIT
}


//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CSaveScriptedTask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveScriptedTask)
	DDX_Text(pDX, IDC_NAME, m_scriptedTaskName);
	DDX_Text(pDX, IDC_TASK_DESCRIPTION, m_taskDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveScriptedTask, CDialog)
	//{{AFX_MSG_MAP(CSaveScriptedTask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSaveScriptedTask::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Limit the text to 20 characters
	CEdit	*ptrEdit = (CEdit*)GetDlgItem( IDC_NAME );
	
	if( ptrEdit )
	{
		ptrEdit->LimitText( TASK_NAME_LIMIT );
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
