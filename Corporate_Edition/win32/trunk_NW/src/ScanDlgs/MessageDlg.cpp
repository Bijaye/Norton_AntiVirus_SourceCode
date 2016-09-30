// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog


CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
:	CDialog(CMessageDlg::IDD, pParent),
	m_bBeep( 0 ),
	m_bMultiple( FALSE )
{
	//{{AFX_DATA_INIT(CMessageDlg)
	m_strMessage = __T("");
	//}}AFX_DATA_INIT
}


void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	DDX_OCText(pDX, IDC_EDIT, DISPID(-517), m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

BOOL CMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( !m_strTitle.IsEmpty() )
		SetWindowText( m_strTitle );

	CWnd* pWnd;

	VERIFY( pWnd = GetDlgItem( IDC_BEEP ) );
	if ( m_bMultiple )
		((CButton*)pWnd)->ModifyStyle( BS_GROUPBOX, BS_AUTO3STATE, 0 );
	((CButton*)pWnd)->SetCheck( m_bBeep );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessageDlg::OnOK() 
{
	CWnd* pWnd;

	VERIFY( pWnd = GetDlgItem( IDC_BEEP ) );
	m_bBeep = ((CButton*)pWnd)->GetCheck();
	
	CDialog::OnOK();
}
