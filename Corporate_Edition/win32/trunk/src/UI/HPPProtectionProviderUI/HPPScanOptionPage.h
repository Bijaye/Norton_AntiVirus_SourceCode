// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPScanOptionPage.h
#pragma once
#include "BaseSheetUI.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "Resource.h"

// CHPPScanOptionPage dialog

class CHPPScanOptionPage : public CBasePageUI
{
	DECLARE_DYNAMIC(CHPPScanOptionPage)

public:
	CHPPScanOptionPage();
	virtual ~CHPPScanOptionPage();

// Dialog Data
	enum { IDD = IDD_HPP_OPTION_PAGE };

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

	enum MinMaxValues
	{
		mmvHoursMin = 0,
		mmvHoursMax = 24,
		mmvMinutesMin = 0,
		mmvMinutesMax = 60,
		mmvSecondsMin = 0,
		mmvSecondsMax = 59,
		mmvNewProcessScanWhenRangeLower = 20,
		mmvNewProcessScanWhenRangeUpper = 30,
		mmvScanMinutesRangeLower = 5,
		mmvScanHoursRangeUpper = 24,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();

	DECLARE_MESSAGE_MAP()

	void ValidateScanNewProc();
	BOOL BumpHoursToMinutes(long nHourValue, long nMinuteValue);
	BOOL BumpMinutesToSeconds(long nMinuteValue, long nSecondValue);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateUI();

private:
	CLockCtrl m_ctlEnableScanLock;
	CButton m_ctlDefaultScanFrequencyScan;
	CButton m_ctlManualScan;
	CButton m_ctlNewProcessScanningEnabled;
	CEdit m_ctlMinutes;
	CEdit m_ctlSeconds;

	CSpinButtonCtrl m_ctlMinutesSpin;
	CSpinButtonCtrl m_ctlSecondsSpin;

	CStatic m_ctlProtectedScanLabel;
	CStatic m_ctlProtectedScanMinLabel;
	CStatic m_ctlProtectedScanSecLabel;

private:
	BOOL m_bNewProcessScanningEnabled;
	BOOL m_bUseDefaultScanFrequency;
	DWORD m_dwMinutes;
	DWORD m_dwSeconds;
public:
	afx_msg void OnBnClickedHppOptionPageDefaultScanFrequency();
	afx_msg void OnBnClickedHppOptionPageManual();
	afx_msg void OnDeltaposScanMinutesSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSecondsSpin(NMHDR *pNMHDR, LRESULT *pResult);
};
