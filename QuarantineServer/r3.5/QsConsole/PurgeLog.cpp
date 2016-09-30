/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// PurgeLog.cpp : implementation file
//

#include "stdafx.h"
#include "qscon.h"
#include "resource.h"
#include "PurgeLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPurgeLog dialog


CPurgeLog::CPurgeLog(CWnd* pParent /*=NULL*/)
	: CDialog(CPurgeLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPurgeLog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	hPurgeFile = NULL;
}


void CPurgeLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPurgeLog)
	DDX_Control(pDX, IDC_RICHEDIT_PURGE_LOG, m_PurgeEditCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPurgeLog, CDialog)
	//{{AFX_MSG_MAP(CPurgeLog)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPurgeLog message handlers

void CPurgeLog::OnSetFont(CFont* pFont) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::OnSetFont(pFont);
}

BOOL CPurgeLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPurgeLog::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	
}
BOOL CPurgeLog::OpenPurgeFile(void) 
{
	return TRUE;
//	CreateFile(
}

