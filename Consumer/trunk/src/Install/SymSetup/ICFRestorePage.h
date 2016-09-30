// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

#ifndef ICFRESTORE_H
#define ICFRESTORE_H

#pragma once

#include "NAVSetupResource.h"

//class CBasePropPage;

class CICFRestorePage : public CBasePropPage<IDD_ICFRESTORE>
{
public:
	CICFRestorePage() : CBasePropPage<IDD_ICFRESTORE>()
	{}

	enum {IDD = IDD_ICFRESTORE};

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnYes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	int OnWizardNext();
	BOOL OnSetActive();

	//	COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)

	BEGIN_MSG_MAP(CICFRestorePage)
		COMMAND_ID_HANDLER(IDC_ICFRESTORE_YES, OnYes)
		COMMAND_ID_HANDLER(IDC_ICFRESTORE_NO, OnNo)
		CHAIN_MSG_MAP(CBasePropPage<IDD_ICFRESTORE>)
	END_MSG_MAP()
	
};

#endif // ICFRESTORE_H