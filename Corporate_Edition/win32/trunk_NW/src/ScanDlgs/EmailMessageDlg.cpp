// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// EmailMessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "EmailMessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmailMessageDlg dialog


CEmailMessageDlg::CEmailMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEmailMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmailMessageDlg)
	m_strInfection = __T("");
	m_strSubject = __T("");
	m_strWarning = __T("");
	//}}AFX_DATA_INIT
}


void CEmailMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmailMessageDlg)
	DDX_OCText(pDX, IDC_INFECTION_INFO, DISPID(-517), m_strInfection);
	DDX_OCText(pDX, IDC_SUBJECT, DISPID(-517), m_strSubject);
	DDX_OCText(pDX, IDC_WARNING, DISPID(-517), m_strWarning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmailMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CEmailMessageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmailMessageDlg message handlers

BOOL CEmailMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( !m_strTitle.IsEmpty() )
		SetWindowText( m_strTitle );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
