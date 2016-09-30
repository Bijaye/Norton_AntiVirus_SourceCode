// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPRTSOptions.h
#pragma once
#include "BaseUI.h"
#include "afxwin.h"
#include "afxcmn.h"

/**
* @pkg CHPPBasic
* @author Terry Marles
* @version 1.0              
*
* The CHPPRTSOptions class implements the RTS configuration panel for HPP.
*
**/

// CHPPRTSOptions dialog

class CHPPRTSOptions : public CBaseUI
{
	DECLARE_DYNAMIC(CHPPRTSOptions)

public:
	CHPPRTSOptions(DWORD dwMode, DWORD dwConfiguration,CWnd* pParent = NULL);   // standard constructor
	virtual ~CHPPRTSOptions();

// Dialog Data
	enum { IDD = IDD_HPP_RT_OPTIONS };


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

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnKillfocusEditHPPFullProcessScan();
	afx_msg void OnEnKillfocusEditHPPNewProcessScan();
	afx_msg void OnBnClickedEnableHPPRts();
	afx_msg void OnBnClickedHPPContiniousScan();
	afx_msg void OnEnChangeEditHPPNewProcessScan();
	afx_msg void OnEnChangeEditHPPFullProcessScan();
	afx_msg void OnStnClickedNewProccessText();
	afx_msg void OnDeltaposHPPNewProccessSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposHPPFullProccessSpin(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAdvanced();
	afx_msg void OnBnClickedActionsBtn();
	afx_msg void OnBnClickedNotifyBtn();
	afx_msg void OnBnClickedBrowseExcludeFiles();
	afx_msg void OnDestroy();
	afx_msg void OnUpdateUI();

	CButton m_crtlEnableContiniousScan;
	CEdit m_editNewProccessScanInterval;
	CEdit m_editFullProccessScanInterval;
	CSpinButtonCtrl m_NewProccesScanIntervalSpin;
	CSpinButtonCtrl m_ProccesFullScanIntervalSpin;
	DWORD m_dwNewProccessScanInterval;
	DWORD m_dwFullProcessScanInterval;
	BOOL m_bEnableHPPProccessProtect;
	BOOL m_bEnableHPPcontiniousScan;
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
};
