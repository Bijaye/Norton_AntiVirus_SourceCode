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
// SecureReSetDlg.cpp : Implementation of SecureReSetDlg
#include "stdafx.h"
//#include "RegToSymReg.h"
#include "SecureReSetDlg.h"
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CSecureReSetDlg
LRESULT CSecureReSetDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;
	
	SetWindowText(m_szProductName);

	// Generate a random string
	if(!FAILED(hr))
	{		
		hr = GenerateRandomString(m_szRandString);
	}

	HWND hNewPwd =  GetDlgItem(IDC_NEWPWD);
	if(NULL != hNewPwd)
		SendMessage(hNewPwd, EM_SETPASSWORDCHAR, '*', 0);

	HWND hConfPwd =  GetDlgItem(IDC_CONFNEWPWD);
	if(NULL != hConfPwd)
		SendMessage(hConfPwd, EM_SETPASSWORDCHAR, '*', 0);

	return CPasswordDlgBase<CSecureReSetDlg>::OnInitDialog( uMsg, wParam, lParam, bHandled );
}

LRESULT CSecureReSetDlg::OnSetPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	TCHAR szPwd1[MAX_PATH+1] = {0};
	TCHAR szPwd2[MAX_PATH+1] = {0};
	TCHAR szRandString[MAX_PATH+1] = {0};
	DWORD dwSize = MAX_PATH;
	HRESULT hr = S_OK;

	GetDlgItemText(IDC_NEWPWD, szPwd1, MAX_PATH);
	GetDlgItemText(IDC_CONFNEWPWD, szPwd2, MAX_PATH);
	GetDlgItemText(IDC_OLDPWD, szRandString, MAX_PATH);

	// Make sure that the random string typed in matches the generated string
	if(!FAILED(hr) && (_tcscmp(szRandString, m_szRandString) != 0))
	{
		DisplayError(m_hWnd, CCPW_PASSWORD_INVALID_KEY);

		// Generate a random string
		GenerateRandomString(m_szRandString);
		SetDlgItemText(IDC_OLDPWD, "");
		SetDlgItemText(IDC_NEWPWD, "");
		SetDlgItemText(IDC_CONFNEWPWD, "");

		// Set focus to correct control. 
		::SetFocus( GetDlgItem( IDC_OLDPWD ) );
		hr = E_FAIL;
	}

	// Now make sure that both new passwords match
	if(!FAILED(hr) && (_tcscmp(szPwd1, szPwd2) != 0))
	{
		DisplayError(m_hWnd, CCPW_PASSWORD_MISMATCH);

		// Generate a random string
		GenerateRandomString(m_szRandString);
		SetDlgItemText(IDC_OLDPWD, "");
		SetDlgItemText(IDC_NEWPWD, "");
		SetDlgItemText(IDC_CONFNEWPWD, "");

		// Set focus to correct control. 
		::SetFocus( GetDlgItem( IDC_OLDPWD ) );

		hr = E_FAIL;
	}

	// Validate the new password
	if(!FAILED(hr))
	{
		hr = ValidatePassword(szPwd1);

		if(FAILED(hr))
		{
			DisplayError(m_hWnd, hr);
						
			SetDlgItemText(IDC_NEWPWD, "");
			SetDlgItemText(IDC_CONFNEWPWD, "");

			// Set focus to correct control. 
			::SetFocus( GetDlgItem( IDC_NEWPWD ) );
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
	WipeBuffer((PBYTE)szRandString, (11)*sizeof(TCHAR));

	// Force a repaint of the text button.
	::InvalidateRect( GetDlgItem( IDC_PWTEXT_BTN ), NULL, FALSE );
	::UpdateWindow( GetDlgItem( IDC_PWTEXT_BTN ) );

	if(!FAILED(hr))
		EndDialog(wID);

	return 0;
}

LRESULT CSecureReSetDlg::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Q: Is this our control?
	if( wParam != IDC_PWTEXT_BTN )
	{
		// Nope, bail out.
		bHandled = FALSE;
		return 0;
	}

	// Fetch input parameters.
	LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT) lParam;

	// Set color and background modes.
	int nBkMode = SetBkMode(pdis->hDC, TRANSPARENT);
	COLORREF crOld = SetTextColor(pdis->hDC, GetSysColor( COLOR_WINDOWTEXT ) );

	// Fetch the correct font to use.
	HFONT hOldFont = (HFONT)SelectObject(pdis->hDC, (HFONT)GetStockObject(DEFAULT_GUI_FONT) );

	// Fill the control with the correct color.
	FillRect( pdis->hDC, &pdis->rcItem, GetSysColorBrush( COLOR_3DFACE ) );

	// Finally, draw the text
	DrawText( pdis->hDC, m_szRandString, -1, &pdis->rcItem, DT_LEFT | DT_VCENTER );
	
	// Cleanup.
	SelectObject(pdis->hDC, hOldFont);
	SetTextColor(pdis->hDC, crOld);
	SetBkMode(pdis->hDC, nBkMode);

	// Done!
	bHandled = TRUE;
	return 1;
}

