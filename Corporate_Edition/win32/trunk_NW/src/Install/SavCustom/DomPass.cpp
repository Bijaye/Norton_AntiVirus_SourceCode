// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// DomPass.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DomPass.h"
#include "vpsetup.h"
#include "ServerInfo.h"
#include "password.h"

#include <svrapi.h>
#include <lmcons.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPTM g_pTrans;

#define	DllExport		__declspec ( dllexport )


/////////////////////////////////////////////////////////////////////////////
// CDomPass dialog

CDomPass::CDomPass(CString sServer, CString *sPassword, CWnd* pParent /*=NULL*/)
	: CDialog(CDomPass::IDD, pParent),
	m_Server(sServer),
	m_Error(0)
{
	// constructor used to initialize domain and password information
	GetDomainInfo(sServer, m_Domain, m_Password, g_pTrans);

	// Set default password for client only
	if (m_Password.IsEmpty() && !ClientIsServer() ) 
		SetToDefault();

	if (sPassword)
		*sPassword = m_Password;

	Decript();
}

CDomPass::CDomPass(CString sServer, CString sPassword, BOOL bServer, CWnd* pParent /*=NULL*/)
	: CDialog(CDomPass::IDD, pParent),
	m_Server(sServer),
	m_Domain(sServer),
	m_Password(sPassword),
	m_Error(0)
{
	// Set default password for client only
	if (m_Password.IsEmpty() && !ClientIsServer() ) 
		SetToDefault();

	Decript();
}

void CDomPass::SetToDefault()
{
	// Set password to default
	MakeEP(m_Password.GetBuffer(PASS_MAX_CIPHER_TEXT_BYTES), PASS_MAX_CIPHER_TEXT_BYTES, PASS_KEY1, PASS_KEY2, DEFAULT_DOMAIN_PASSWORD, strlen(DEFAULT_DOMAIN_PASSWORD)+1);
	m_Password.ReleaseBuffer();
}

void CDomPass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDomPass)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDomPass, CDialog)
	//{{AFX_MSG_MAP(CDomPass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDomPass message handlers

void CDomPass::Decript()
{
	CString strPassOut;
	UnMakeEP(m_Password, m_Password.GetLength()+1,PASS_KEY1,PASS_KEY2,strPassOut.GetBuffer(PASS_MAX_PLAIN_TEXT_BYTES+1), PASS_MAX_PLAIN_TEXT_BYTES+1);
	strPassOut.ReleaseBuffer();
	m_Password = strPassOut;
}

void CDomPass::OnOK()
{
	CString sPass;
	CWnd *pPass = GetDlgItem(IDC_PASSWORD_EDIT);
	pPass->GetWindowText(sPass);

	if (VerifyPassword(sPass))
		CDialog::OnOK();
	else
	{
		CString sDisplay, sTitle;
		sDisplay.LoadString(IDS_INVALID_PASSWD);
		sTitle.LoadString(IDS_DEFAULT_TITLE);
		MessageBox(sDisplay,sTitle);
		((CEdit*)pPass)->SetFocus();
		((CEdit*)pPass)->SetSel(0,-1);
	}
}

BOOL CDomPass::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd *pWnd = GetDlgItem(IDC_STATIC_DOMAIN);
	pWnd->SetWindowText(m_Server);

	InitPasswords();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDomPass::Valid()
{
	return !m_Error;
}

void CDomPass::DisplayError()
{
	CString sText;
	sText.Format(IDS_SERVER_NORESPONSE,m_Server,m_Error);
	AfxMessageBox(sText);
}


BOOL CDomPass::VerifyPassword(CString userPass)
{
	// An Empty password is an invalid password...
	if(userPass.IsEmpty())
        return(FALSE);

	if (userPass == m_Password)
	{
		AddPasswordToList(userPass);
		return TRUE;
	}
	else
		return FALSE;
}




int CDomPass::DoModal()
{
	if (PasswordInList(m_Password))
		return IDOK;

	return CDialog::DoModal();
}


CStringArray *g_pPasswords = 0;

CStringArray* InitPasswords()
{
	try
	{
		if (!g_pPasswords)
			g_pPasswords = new CStringArray;
	}
	catch (std::bad_alloc &){}
	return g_pPasswords;
}


void CleanupPasswords()
{
	if (g_pPasswords)
	{
		g_pPasswords->RemoveAll();
		delete g_pPasswords;
		g_pPasswords = NULL;
	}
}


void AddPasswordToList(CString pPassword)
{
	if (!g_pPasswords) return;
	g_pPasswords->Add(pPassword);
}

BOOL PasswordInList(CString pPassword)
{
	if (!g_pPasswords) return FALSE;

	for (int idx = g_pPasswords->GetSize()-1; idx>=0; idx--)
		if (pPassword == g_pPasswords->GetAt(idx))
			return TRUE;
	return FALSE;
}

#if 0
extern "C" DllExport void _stdcall MsiModIniPass(LPSTR szInput, LPSTR szOutput)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    SAVASSERT( false ); // I don't think this function is used any longer.

	MakeEP(szOutput, PASS_MAX_BUF_SIZE, PASS_KEY1, PASS_KEY2, szInput); // buffer overflow risk: assuming PASS_MAX_BUF_SIZE
}
#endif

extern "C" DllExport BOOL _stdcall MsiDomPass(LPSTR szInput, LPSTR szInput2)
{
	CString sPassword, sDomain, InputPassword = szInput2;
	BOOL ret = FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	GetDomainNameAndPassword(szInput, sDomain, sPassword, g_pTrans);

	if(sPassword == InputPassword)
	{
		ret = TRUE;
	}

	return ret;
}