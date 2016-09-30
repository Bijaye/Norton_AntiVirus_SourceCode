// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// WarningDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "WarningDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarningDlg dialog


CWarningDlg::CWarningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWarningDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWarningDlg)
	m_strInfection = __T("");
	m_strSubject = __T("");
	m_strWarning = __T("");
	//}}AFX_DATA_INIT
}


void CWarningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarningDlg)
	DDX_OCText(pDX, IDC_INFECTION_INFO, DISPID(-517), m_strInfection);
	DDX_OCText(pDX, IDC_SUBJECT, DISPID(-517), m_strSubject);
	DDX_OCText(pDX, IDC_WARNING, DISPID(-517), m_strWarning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWarningDlg, CDialog)
	//{{AFX_MSG_MAP(CWarningDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWarningDlg message handlers

BOOL CWarningDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( !m_strTitle.IsEmpty() )
		SetWindowText( m_strTitle );

	CWnd* pWnd;

	VERIFY( pWnd = GetDlgItem( IDC_CHANGE_SUBJECT ) );
	if ( m_bMultiple )
		((CButton*)pWnd)->ModifyStyle( BS_GROUPBOX, BS_AUTO3STATE, 0 );
	((CButton*)pWnd)->SetCheck( m_bChangeSubject );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWarningDlg::OnOK() 
{
	CWnd* pWnd;

	VERIFY( pWnd = GetDlgItem( IDC_CHANGE_SUBJECT ) );
	m_bChangeSubject = ((CButton*)pWnd)->GetCheck();
	
	CDialog::OnOK();
}
