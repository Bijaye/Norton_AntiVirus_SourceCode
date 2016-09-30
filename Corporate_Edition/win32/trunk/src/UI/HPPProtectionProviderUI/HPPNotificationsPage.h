// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPScanOptionPage.h
#pragma once
#include "BaseSheetUI.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "Resource.h"


// CHPPNotificationsPage dialog

class CHPPNotificationsPage : public CBasePageUI
{
	DECLARE_DYNAMIC(CHPPNotificationsPage)

public:
	CHPPNotificationsPage();
	virtual ~CHPPNotificationsPage();

// Dialog Data
	enum { IDD = IDD_HPP_NOTIFICATIONS };

	/**
	* Load is called by various dialog routines to read the settings out of the 
	* configuration store.
	*
    * @return void
	* 
	*/
	void Load();

	/**
	* Store is called by various dialog routines to write modified settings out to the 
	* configuration store.
	*
    * @return void
	* 
	*/
	void Store();
	DWORD HelpId(){ return IDD; };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateUI();
	afx_msg void OnBnClickedDisplayResults();

private:
	BOOL m_bDisplayResults;
	BOOL m_bPromptTerminateProcess;
	BOOL m_bPromptStopService;
	CButton m_ctlDisplayResults;
	CButton m_ctlPromptTerminateProcess;
	CButton m_ctlPromptStopService;

	CLockCtrl m_lockDisplayResults;
};
