// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// NewPassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SavCustom.h"
#include "NewPassDlg.h"
#include ".\newpassdlg.h"
#include "vpstrutils.h"


// CNewPassDlg dialog

IMPLEMENT_DYNAMIC(CNewPassDlg, CDialog)
CNewPassDlg::CNewPassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPassDlg::IDD, pParent)
{
}

CNewPassDlg::CNewPassDlg( CString strPass, CWnd* pParent )
	: CDialog(CNewPassDlg::IDD, pParent)
{
	m_strPassword = strPass;
}

CNewPassDlg::~CNewPassDlg()
{
}

void CNewPassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OLDPASSWORD, m_ceOldPassword);
	DDX_Control(pDX, IDC_EDIT_NEWPASSWORD, m_ceNewPassword);
	DDX_Control(pDX, IDC_EDIT_VERIFYPASSWORD, m_ceVerifyPassword);
}


BEGIN_MESSAGE_MAP(CNewPassDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CNewPassDlg message handlers

void CNewPassDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CNewPassDlg::OnBnClickedOk()
{
	TCHAR		szMessage[ MAX_PATH ] = {0};
	int			nSize=0;
	TCHAR		*pszTemp = NULL;

	nSize = strlen(szOldPassword);
	GetDlgItem(IDC_EDIT_OLDPASSWORD)->GetWindowText(szOldPassword);
//	OutputDebugString(szOldPassword);
	nSize = strlen(szNewPassword);
	GetDlgItem(IDC_EDIT_NEWPASSWORD)->GetWindowText(szNewPassword);
//	OutputDebugString(szNewPassword);
	nSize = strlen(szVerPassword);
	GetDlgItem(IDC_EDIT_VERIFYPASSWORD)->GetWindowText(szVerPassword);
//	OutputDebugString(szVerPassword);

//	vpsnprintf(szMessage, sizeof (szMessage), "Passed password = %s", m_strPassword);
//	OutputDebugString (szMessage);

	if ( 0 == szOldPassword.Compare( m_strPassword ) ) // auth old pass
	{
		OutputDebugString( "Old Pass & Consolepass match" );

		if ( 0 == szNewPassword.Compare( szVerPassword )) // auth new pass
		{
			OutputDebugString( "New Pass & Ver pass match" );
			if ( szNewPassword.GetLength() > 5 )
			{
				OutputDebugString( "New Pass lenght >6 chars " );
				CDialog::OnOK();
			}
			else
			{
				OutputDebugString( "New Passwords is too small > 6 chars" );
				CString strMessage, strTitle;
				strMessage.LoadString( IDS_ERROR_MIGSRV_NEW_PASS_LENGTH );
				strTitle.LoadString( IDS_ERROR_MIGSRV_PASS_TITLE );
				MessageBox( strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
			}
		}
		else
		{
			OutputDebugString( "New Passwords don't match" );
			CString strMessage, strTitle;
			strMessage.LoadString( IDS_ERROR_MIGSRV_NEW_PASS_MISMATCH );
			strTitle.LoadString( IDS_ERROR_MIGSRV_PASS_TITLE );
			MessageBox( strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
		}
	}
	else
	{
		OutputDebugString( "Old Passwords don't match" );
		CString strMessage, strTitle;
		strMessage.LoadString( IDS_ERROR_MIGSRV_OLD_PASS_MISMATCH );
		strTitle.LoadString( IDS_ERROR_MIGSRV_PASS_TITLE );
		MessageBox( strMessage, strTitle, MB_OK | MB_ICONEXCLAMATION );
	}
}



