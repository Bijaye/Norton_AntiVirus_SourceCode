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
// ReSetDlg.h : Declaration of the CSetDlg

#ifndef __RESETDLG_H_
#define __RESETDLG_H_

#include "resource.h"       // main symbols
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CSetDlg
class CReSetDlg : 
	public CPasswordDlgBase<CReSetDlg>
{
public:
	CReSetDlg()
	{
		_tcscpy(m_szOldPwd, _T(""));
	}

	~CReSetDlg()
	{
		WipeBuffer((PBYTE)m_szOldPwd, (MAX_PATH+1)*sizeof(TCHAR));
	}

	void SetOldPwd(TCHAR *pszOldPwd)
	{
		if(_tcslen(pszOldPwd) < (sizeof(m_szOldPwd) / sizeof(TCHAR)))
			_tcscpy(m_szOldPwd, pszOldPwd);
		else
		{
			ZeroMemory(m_szOldPwd, sizeof(m_szOldPwd));
			_tcsncpy(m_szOldPwd, pszOldPwd, (sizeof(m_szOldPwd) / sizeof(TCHAR)) - 1);
		}
	}

	enum { IDD = IDD_RESETPWD };

private:
	TCHAR m_szOldPwd[MAX_PATH+1];

public:

BEGIN_MSG_MAP(CSetDlg)
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

#endif //__RESETDLG_H_