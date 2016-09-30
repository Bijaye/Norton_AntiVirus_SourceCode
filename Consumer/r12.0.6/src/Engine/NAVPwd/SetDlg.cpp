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
// SetDlg.cpp : Implementation of CSetDlg
#include "stdafx.h"
//#include "RegToSymReg.h"
#include "SetDlg.h"
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CSetDlg
LRESULT CSetDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetWindowText(m_szProductName);

	HWND hNewPwd =  GetDlgItem(IDC_NEWPWD);
	if(NULL != hNewPwd)
		SendMessage(hNewPwd, EM_SETPASSWORDCHAR, '*', 0);

	HWND hConfPwd =  GetDlgItem(IDC_CONFNEWPWD);
	if(NULL != hConfPwd)
		SendMessage(hConfPwd, EM_SETPASSWORDCHAR, '*', 0);

	return CPasswordDlgBase<CSetDlg>::OnInitDialog( uMsg, wParam, lParam, bHandled );
}


LRESULT CSetDlg::OnSetPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	TCHAR szPwd1[MAX_PATH+1];
	TCHAR szPwd2[MAX_PATH+1];
	DWORD dwSize = MAX_PATH;
	HRESULT hr = S_OK;

	GetDlgItemText(IDC_NEWPWD, szPwd1, MAX_PATH);
	GetDlgItemText(IDC_CONFNEWPWD, szPwd2, MAX_PATH);

	// Now make sure that both new passwords match
	if(!FAILED(hr) && (_tcscmp(szPwd1, szPwd2) != 0))
	{
		DisplayError(m_hWnd, CCPW_PASSWORD_MISMATCH);

		hr = E_FAIL;
	}

	// Validate the new password
	if(!FAILED(hr))
	{
		hr = ValidatePassword(szPwd1);

		if(FAILED(hr))
		{
			DisplayError(m_hWnd, hr);
		}
	}

	if(!FAILED(hr))
	{
		dwSize = _tcslen(szPwd1);
		hr = HashPassword(szPwd1, (BYTE*)m_szNewPwd, dwSize);	
	}

	WipeBuffer((PBYTE)szPwd1, (MAX_PATH+1)*sizeof(TCHAR));
	WipeBuffer((PBYTE)szPwd2, (MAX_PATH+1)*sizeof(TCHAR));

	SetDlgItemText(IDC_NEWPWD, _T(""));
	SetDlgItemText(IDC_CONFNEWPWD, _T(""));

	if(!FAILED(hr))
		EndDialog(wID);
	else
	{
		// Set the focus to the proper edit control.
		::SetFocus( GetDlgItem( IDC_NEWPWD ) );
	}

	return 0;
}
