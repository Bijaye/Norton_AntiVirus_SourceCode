// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "savMainCA.h"
#include "PassDlg.h"
#include "Password.h"
#include "ClientReg.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPassDlg dialog


CPassDlg::CPassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassDlg)
	DDX_Control(pDX, IDC_PASSWORD, m_edtPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassDlg, CDialog)
	//{{AFX_MSG_MAP(CPassDlg)
	ON_BN_CLICKED(IDOK, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassDlg message handlers

void CPassDlg::Decrypt(CString& sPassword)
{
	CString strPassOut;
    UnMakeEP( sPassword, sPassword.GetLength()+1, PASS_KEY1, PASS_KEY2, strPassOut.GetBuffer( PASS_MAX_PLAIN_TEXT_BYTES+1 ), PASS_MAX_PLAIN_TEXT_BYTES+1 );
	strPassOut.ReleaseBuffer();
	sPassword = strPassOut;
}

void CPassDlg::Encrypt(CString& sPassword)
{
	CString strPassOut;
	MakeEP( strPassOut.GetBuffer(PASS_MAX_CIPHER_TEXT_BYTES), PASS_MAX_CIPHER_TEXT_BYTES, PASS_KEY1, PASS_KEY2, sPassword, sPassword.GetLength()+1 );
	strPassOut.ReleaseBuffer();
	sPassword = strPassOut;
}

BOOL CPassDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Read the current password
	HKEY hKey;
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                       szReg_Key_Main "\\" szReg_Key_AdminOnly "\\" szReg_Key_Security,
                                       0, KEY_READ, &hKey ) )
    {
		m_Error = TRUE;
    }
	else
	{
		DWORD dwSize = MAX_PATH;
		char *pPwd = m_Password.GetBuffer( dwSize );
		LONG lRet = SymSaferRegQueryValueEx( hKey, szReg_Val_VPUninstallPassword, NULL, NULL, reinterpret_cast<LPBYTE>(pPwd), &dwSize );
		m_Password.ReleaseBuffer();
	}

	m_edtPassword.SetPasswordChar (_T('*'));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPassDlg::OnOK() 
{
	CString strPassword;
	m_edtPassword.GetWindowText( strPassword );
	Encrypt( strPassword );
	
	if( strPassword == m_Password )
		CDialog::OnOK();
	else
	{
		CString strDisplay, strTitle;
		strDisplay.LoadString( IDS_INVALID_PASSWORD );
		strTitle.LoadString( IDS_Q_UNINSTALL_TITLE );
		::MessageBox( this->m_hWnd, strDisplay, strTitle, MB_OK );
		m_edtPassword.SetFocus();
		m_edtPassword.SetSel( 0, -1 );
	}
}

void CPassDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
