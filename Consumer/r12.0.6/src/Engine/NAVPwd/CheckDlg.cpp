// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// CheckDlg.cpp : Implementation of CCheckDlg
#include "stdafx.h"
//#include "RegToSymReg.h"

#include "CheckDlg.h"
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CCheckDlg
LRESULT CCheckDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;
	TCHAR szFmt[MAX_PATH+1] = {0};
	TCHAR szMessage[MAX_PATH+MAX_PATH+1] = {0};
	
	// Load check password format string
	if(0 == LoadString(GetMyModule(), IDS_FMT_CHECKPWD, szFmt, MAX_PATH))
		hr = E_FAIL;

	// Format the string using feature name
	if(SUCCEEDED(hr))
	{
		_stprintf(szMessage, szFmt, m_szFeatureName);

		SetDlgItemText(IDC_ENTERPWD_MSG, szMessage);

		SetWindowText(m_szProductName);
	}

	HWND hPwdPrompt =  GetDlgItem(IDC_NEWPWD);
	if(NULL != hPwdPrompt)
		SendMessage(hPwdPrompt, EM_SETPASSWORDCHAR, '*', 0);

	return CPasswordDlgBase<CCheckDlg>::OnInitDialog( uMsg, wParam, lParam, bHandled );
}

LRESULT CCheckDlg::OnSetPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	TCHAR szPwd[MAX_PATH+1] = {0};
	TCHAR szHashPwd[MAX_PATH+1] = {0};
	DWORD dwSize = MAX_PATH;
	HRESULT hr = S_OK;

	GetDlgItemText(IDC_NEWPWD, szPwd, MAX_PATH);	

/*	if(m_bUseWindowsAccounts)
	{
		BOOL bValid = FALSE;

		bValid = VerifyWindowsAccountCreds(m_szUserName, szPwd);

		if(!bValid)
		{
			DisplayError(m_hWnd, CCPW_PASSWORD_INVALID);

			hr = E_FAIL;
		}
	}
	else*/
	{
		dwSize = _tcslen(szPwd);
		hr = HashPassword(szPwd, (BYTE*)szHashPwd, dwSize);

		if(!FAILED(hr))
		{
			if(_tcscmp(szHashPwd, m_szCompPwd) != 0)
			{
				DisplayError(m_hWnd, CCPW_PASSWORD_INVALID);

				hr = E_FAIL;
			}
		}	
	}

	WipeBuffer((PBYTE)szPwd, (MAX_PATH+1)*sizeof(TCHAR));
	WipeBuffer((PBYTE)szHashPwd, (MAX_PATH+1)*sizeof(TCHAR));

	SetDlgItemText(IDC_NEWPWD, _T(""));

	if(SUCCEEDED(hr))
		EndDialog(wID);
	{
		::SetFocus( GetDlgItem( IDC_NEWPWD ) );
	}

	return 0;
}

