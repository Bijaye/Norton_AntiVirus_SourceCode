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
// SetDlg.h : Declaration of the CSetDlg

#ifndef __SETDLG_H_
#define __SETDLG_H_

#include "resource.h"       // main symbols
#include "ccPwdUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CSetDlg
class CSetDlg : 
	public CPasswordDlgBase<CSetDlg>
{
public:
	CSetDlg()
	{
	}

	~CSetDlg()
	{
	}

	enum { IDD = IDD_NEWPWD };

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

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetPwd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif //__SETDLG_H_
