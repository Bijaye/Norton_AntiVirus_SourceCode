// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VerifySGPass.cpp : implementation file
//

#include "stdafx.h"
#include "SavCustom.h"
#include "VerifySGPass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVerifySGPass dialog


CVerifySGPass::CVerifySGPass(CWnd* pParent /*=NULL*/)
	: CDialog(CVerifySGPass::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVerifySGPass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CVerifySGPass::CVerifySGPass( CString strPass, CWnd* pParent )
	: CDialog(CVerifySGPass::IDD, pParent)
{
	m_strPassword = strPass;
}

void CVerifySGPass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVerifySGPass)
	DDX_Control(pDX, IDC_PASSWORD, m_edtPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVerifySGPass, CDialog)
	//{{AFX_MSG_MAP(CVerifySGPass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVerifySGPass message handlers

BOOL CVerifySGPass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_edtPassword.SetPasswordChar( _T('*') );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVerifySGPass::OnOK() 
{
	CString strPass;

	m_edtPassword.GetWindowText( strPass );

	if( 0 == strPass.Compare( m_strPassword ) )
		CDialog::OnOK();
	else
	{
		CString strTitle;
		CString strMessage;
		
		strTitle.LoadString( IDS_CLIENT_LINK );
		strMessage.LoadString( IDS_ERROR_NOMATCH );

		::MessageBox( NULL, strMessage, strTitle, MB_OK | MB_ICONERROR );
	}
}

void CVerifySGPass::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
