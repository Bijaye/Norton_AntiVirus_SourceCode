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
// CheckDlg.h : Declaration of the CCheckDlg

#ifndef __CHECKDLG_H_
#define __CHECKDLG_H_

#include "resource.h"       // main symbols
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CCheckDlg
class CCheckDlg : 
	public CPasswordDlgBase<CCheckDlg>
{
public:
	CCheckDlg()
	{
		_tcscpy(m_szCompPwd, _T(""));
		_tcscpy(m_szUserName, _T(""));
		m_bUseWindowsAccounts  = FALSE;
	}

	virtual ~CCheckDlg()
	{
		WipeBuffer((PBYTE)m_szCompPwd, (MAX_PATH+1)*sizeof(TCHAR));
	}

	void SetCompPwd(TCHAR *pszCmpPwd)
	{
		if(_tcslen(pszCmpPwd) < (sizeof(m_szCompPwd) / sizeof(TCHAR)) )
			_tcscpy(m_szCompPwd, pszCmpPwd);
		else
		{
			ZeroMemory(m_szCompPwd, sizeof(m_szCompPwd));
			_tcsncpy(m_szCompPwd, pszCmpPwd, (sizeof(m_szCompPwd) / sizeof(TCHAR)) - 1);
		}
	}

	void SetUserName(TCHAR *pszUserName)
	{
		if(_tcslen(pszUserName) < (sizeof(m_szUserName) / sizeof(TCHAR)))
			_tcscpy(m_szUserName, pszUserName);
		else
		{
			ZeroMemory(m_szUserName, sizeof(m_szUserName));
			_tcsncpy(m_szUserName, pszUserName, (sizeof(m_szUserName) / sizeof(TCHAR)) - 1);
		}
	}

	void SetUseWindowsAccounts(BOOL bVal)
	{
		m_bUseWindowsAccounts = bVal;
	}

	enum { IDD = IDD_PWDPROMPT };

private:
	TCHAR m_szCompPwd[MAX_PATH+1];
	TCHAR m_szUserName[MAX_PATH+1];
	BOOL m_bUseWindowsAccounts;

public:

BEGIN_MSG_MAP(CCheckDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(ID_SET_PWD, OnSetPwd)
	COMMAND_ID_HANDLER(ID_CANCEL_PWD, OnCancelPwd);
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	
	// CPasswordDlgBase overrides
	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnSetPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif //__CHECKDLG_H_
