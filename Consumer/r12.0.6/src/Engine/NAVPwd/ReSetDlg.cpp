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
// ReSetDlg.cpp : Implementation of CReSetDlg
#include "stdafx.h"
//#include "RegToSymReg.h"
#include "ReSetDlg.h"
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CReSetDlg
LRESULT CReSetDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetWindowText(m_szProductName);

	HWND hOldPwd =  GetDlgItem(IDC_OLDPWD);
	if(NULL != hOldPwd)
		SendMessage(hOldPwd, EM_SETPASSWORDCHAR, '*', 0);

	HWND hNewPwd =  GetDlgItem(IDC_NEWPWD);
	if(NULL != hNewPwd)
		SendMessage(hNewPwd, EM_SETPASSWORDCHAR, '*', 0);

	HWND hConfPwd =  GetDlgItem(IDC_CONFNEWPWD);
	if(NULL != hConfPwd)
		SendMessage(hConfPwd, EM_SETPASSWORDCHAR, '*', 0);

	return CPasswordDlgBase<CReSetDlg>::OnInitDialog( uMsg, wParam, lParam, bHandled );
}

LRESULT CReSetDlg::OnSetPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	TCHAR szPwd1[MAX_PATH+1] = {0};
	TCHAR szPwd2[MAX_PATH+1] = {0};
	TCHAR szOld[MAX_PATH+1] = {0};
	TCHAR szHashedOld[MAX_PATH+1] = {0};
	DWORD dwSize = MAX_PATH;
	DWORD dwOldSize = MAX_PATH;
	HRESULT hr = S_OK;

	GetDlgItemText(IDC_NEWPWD, szPwd1, MAX_PATH);
	GetDlgItemText(IDC_CONFNEWPWD, szPwd2, MAX_PATH);
	GetDlgItemText(IDC_OLDPWD, szOld, MAX_PATH);

	dwOldSize = _tcslen(szOld);
	hr = HashPassword(szOld, (BYTE*)szHashedOld, dwOldSize);

	if(_tcscmp(szHashedOld, m_szOldPwd) != 0)
	{
		DisplayError(m_hWnd, CCPW_PASSWORD_INVALID_OLD);
		hr = E_FAIL;
	}

	// Now make sure that both new passwords match
	if(!FAILED(hr) && (_tcscmp(szPwd1, szPwd2) != 0))
	{
		DisplayError(m_hWnd, CCPW_PASSWORD_MISMATCH);
		hr = E_FAIL;
	}

	// Now make sure that the new password is different than the old password
	if(!FAILED(hr))
	{
		if(_tcscmp(szOld, szPwd1) == 0)
		{
			DisplayError(m_hWnd, CCPW_PASSWORD_INVALID_SAME);
			hr = E_FAIL;
		}
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

	// All is well, set out param
	if(!FAILED(hr))
	{
		dwSize = _tcslen(szPwd1);
		hr = HashPassword(szPwd1, (BYTE*)m_szNewPwd, dwSize);
	}

	WipeBuffer((PBYTE)szPwd1, (MAX_PATH+1)*sizeof(TCHAR));
	WipeBuffer((PBYTE)szPwd2, (MAX_PATH+1)*sizeof(TCHAR));
	WipeBuffer((PBYTE)szOld, (MAX_PATH+1)*sizeof(TCHAR));
	WipeBuffer((PBYTE)szHashedOld, (MAX_PATH+1)*sizeof(TCHAR));

	SetDlgItemText(IDC_NEWPWD, _T(""));
	SetDlgItemText(IDC_CONFNEWPWD, _T(""));
	SetDlgItemText(IDC_OLDPWD, _T(""));

	if(!FAILED(hr))
		EndDialog(wID);
	else
	{
		::SetFocus( GetDlgItem( IDC_OLDPWD ) );
	}

	return 0;
}


