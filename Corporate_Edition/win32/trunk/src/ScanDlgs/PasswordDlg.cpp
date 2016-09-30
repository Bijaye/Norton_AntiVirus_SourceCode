// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "PasswordDlg.h"
#include "ClientReg.h"
#include "ClientReg.h"
#include "ConfigObj.h"
#include "password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

void CPasswordDlg::OnOK() 
{
	CEdit	*ptrEdit = (CEdit*)GetDlgItem( IDC_PASSWORD );
	CString strText;

	if( ptrEdit )
	{
		ptrEdit->GetWindowText( strText );
	}

	//Check the password. If it is valid, then
	//	dismiss the dialog.
	//Otherwise, show an error and do not dismiss the dialog.
	if( CheckPassword( strText ) )	
	{
		CDialog::OnOK();
	}
	else
	{
		//Otherwise, don't.
		CString sTitle,
				sMsg;
		sTitle.LoadString(IDS_TITLE);
		sMsg.LoadString(IDS_INCORRECT_PASSWORD);
		MessageBox(sMsg,sTitle);
	}
}

BOOL CPasswordDlg::OnInitDialog() 
{
	CEdit	*ptrEdit = (CEdit*)GetDlgItem( IDC_PASSWORD );

	CDialog::OnInitDialog();

	CenterWindow();

	if( ptrEdit )
			ptrEdit->SetFocus();

	if( CheckPassword( CString("") ) )
	{
		EndDialog( IDOK );
	}

	return FALSE;  
}

//----------------------------------------------------------------
// CheckPassword
//----------------------------------------------------------------
BOOL CPasswordDlg::CheckPassword( CString strPassword )
{
	CString	strEncryptedPassword,
			strCorrectPassword;
	LPSTR	szEncryptedPassword;
	IScanConfig *pConfig;
	HRESULT hr;
	BOOL bResult=TRUE;
	DWORD dwPrompt;


	
	// get an IScanConfig
	hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IScanConfig,(void**)&pConfig);
	if (hr != S_OK) return FALSE;

	// open the Admin only key
	hr = pConfig->Open(NULL,HKEY_VP_MAIN_ROOT,szReg_Key_AdminOnly);
	// set the suboption to Security
	if (hr==S_OK)
		hr = pConfig->SetSubOption(szReg_Key_Security);

	// wrap the config
	CConfigObj config(pConfig);	
	

	// first check and see if we need to prompt
	dwPrompt = config.GetOption(szReg_Val_UseScanNetDrivePassword, 0 );

	
	if (dwPrompt)
	{
		// we do need to prompt so get the password
		strCorrectPassword = config.GetOption(szReg_Val_ScanNetDrivePassword,CString(""));

		//If the password was not read from the registry,
		//	then encrypt the default password
		//	to check against.
		if( strCorrectPassword.IsEmpty() )
		{
			szEncryptedPassword = strCorrectPassword.GetBuffer( PASS_MAX_CIPHER_TEXT_BYTES );

			MakeEP( szEncryptedPassword, PASS_MAX_CIPHER_TEXT_BYTES, PASS_KEY1, PASS_KEY2, DEFAULT_DOMAIN_PASSWORD, strlen(DEFAULT_DOMAIN_PASSWORD)+1 );

			strCorrectPassword.ReleaseBuffer();
		}


		//Now, encrypt the password entered in the field
		szEncryptedPassword = strEncryptedPassword.GetBuffer( PASS_MAX_CIPHER_TEXT_BYTES );

        MakeEP( szEncryptedPassword, PASS_MAX_CIPHER_TEXT_BYTES, PASS_KEY1, PASS_KEY2, strPassword, strPassword.GetLength()+1);

        strEncryptedPassword.ReleaseBuffer();


		//If it's valid, continue
		bResult = (strEncryptedPassword == strCorrectPassword );
	}

	// release the config object
	pConfig->SetSubOption(NULL);
	pConfig->Release();

	return bResult;
}
