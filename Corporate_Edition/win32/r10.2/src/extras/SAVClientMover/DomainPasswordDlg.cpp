// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// DomainPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SAVClientMover.h"
#include "DomainPasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDomainPasswordDlg dialog


CDomainPasswordDlg::CDomainPasswordDlg(CWnd* pParent, const CString &csServerName /*=NULL*/)
	: CDialog(CDomainPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDomainPasswordDlg)
	m_csPassword = _T("");
	m_csServerName = csServerName;
	//}}AFX_DATA_INIT
}


void CDomainPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDomainPasswordDlg)
	DDX_Control(pDX, IDC_SERVER_NAME, m_ctlServerName);
	DDX_Control(pDX, IDC_PASSWORD_INPUT, m_ctlPassword);
	DDX_Text(pDX, IDC_PASSWORD_INPUT, m_csPassword);
	DDV_MaxChars(pDX, m_csPassword, 20);
	DDX_Text(pDX, IDC_SERVER_NAME, m_csServerName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDomainPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CDomainPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDomainPasswordDlg message handlers

void CDomainPasswordDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CDomainPasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctlPassword.SetFocus( );

	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

