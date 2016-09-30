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

#ifndef __SECURERESETDLG_H_
#define __SECURERESETDLG_H_

#include "resource.h"       // main symbols
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CSetDlg
class CSecureReSetDlg : 
	public CPasswordDlgBase<CSecureReSetDlg>
{
public:
	CSecureReSetDlg()
	{
		_tcscpy(m_szRandString, _T(""));
	}

	~CSecureReSetDlg()
	{
		WipeBuffer((PBYTE)m_szRandString, (11)*sizeof(TCHAR));
	}	

	enum { IDD = IDD_SECURERESETPWD };

private:
	TCHAR m_szRandString[11];

	void PaintRandString(HDC hDC, TCHAR *szString, BOOL bInvalidate);
public:

BEGIN_MSG_MAP(CSetDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_ID_HANDLER(ID_SET_PWD, OnSetPwd)
	COMMAND_ID_HANDLER(ID_CANCEL_PWD, OnCancelPwd);
	MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	// CPasswordDlgBase overrides
	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnSetPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

#endif //__RESETDLG_H_