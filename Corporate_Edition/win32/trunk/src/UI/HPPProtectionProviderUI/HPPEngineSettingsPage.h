// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPScanOptionPage.h
#pragma once
#include "BaseSheetUI.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "Resource.h"


// CHPPEngineSettingsPage dialog

class CHPPEngineSettingsPage : public CBasePageUI
{
	DECLARE_DYNAMIC(CHPPEngineSettingsPage)

public:
	CHPPEngineSettingsPage();
	virtual ~CHPPEngineSettingsPage();

// Dialog Data
	enum { IDD = IDD_HPP_ENGINE_SETTINGS };

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

	enum Defaults
	{
		dftTicFreq	= 5,
		dftLineSize	= 10,
		dftPageSize	= 25,
		dftRangeLower = 0,
		dftRangeUpper = 100,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateUI();
	afx_msg void OnBnClickedTrojanOverrideSensitivity();
	afx_msg void OnBnClickedKeyloggersOverrideSensitivity();

private:
	CStatic m_ctlTrojanActionLabel;
	CComboBox m_ctlTrojanAction;
	CButton m_ctlOverrideTrojanSensitivity;
	CSliderCtrl m_ctlTrojanSensitivity;

	CStatic m_ctlKeyLoggersActionLabel;
	CComboBox m_ctlKeyLoggersAction;
	CButton m_ctlOverrideKeyLoggersSensitivity;
	CSliderCtrl m_ctlKeyLoggersSensitivity;

	CComboBox m_ctlCommercialKeyloggerAction;
	CComboBox m_ctlCommercialRemoteControlAction;

private:
	CButton m_ctlEnableTrojanEngine;
	BOOL m_bEnableTrojanEngine;
	BOOL m_bOverrideTrojanSensitivity;
	DWORD m_dwTrojanSensitivity;
	DWORD m_dwTrojanAction;

	CButton m_ctlEnableKeyLoggersEngine;
	BOOL  m_bEnableKeyLoggersEngine;
	BOOL m_bOverrideKeyLoggersSensitivity;
	DWORD m_dwKeyLoggersSensitivity;
	DWORD m_dwKeyLoggersAction;

	DWORD m_dwCommercialKeyloggersAction;
	DWORD m_dwCommercialRemoteControlAction;

	afx_msg void OnBnClickedTrojanEnabled();
	afx_msg void OnBnClickedKeyloggersEnabled();
};
