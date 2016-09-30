// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
//
// HPPScanOptionPage.cpp : implementation file
//

#include "stdafx.h"
#include "HPPScanOptionPage.h"
#include "HPPSheet.h"
#include "SymSaferStrings.h"
#include "dprintf.h"

// CHPPScanOptionPage dialog

IMPLEMENT_DYNAMIC(CHPPScanOptionPage, CPropertyPage)

CHPPScanOptionPage::CHPPScanOptionPage()
	: CBasePageUI(CHPPScanOptionPage::IDD)
	, m_bNewProcessScanningEnabled(REG_DEFAULT_SymHPPS_NewProcessScanningEnabled)
	, m_bUseDefaultScanFrequency(REG_DEFAULT_SymHPPS_UseDefaultScanFrequency)
	, m_dwMinutes(mmvMinutesMin)
	, m_dwSeconds(mmvSecondsMin)
{
}

CHPPScanOptionPage::~CHPPScanOptionPage()
{
}

void CHPPScanOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	CBasePageUI::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HPP_OPTION_PAGE_DEFAULT_SCAN_FREQUENCY, m_ctlDefaultScanFrequencyScan);
	DDX_Check(pDX, IDC_HPP_OPTION_PAGE_DEFAULT_SCAN_FREQUENCY, m_bUseDefaultScanFrequency);
	DDX_Control(pDX, IDC_HPP_OPTION_PAGE_MANUAL, m_ctlManualScan);
	DDX_Control(pDX, IDC_HPP_OPTION_PAGE_SCAN_NEW_IMMEDIATELY, m_ctlNewProcessScanningEnabled);
	DDX_Check(pDX, IDC_HPP_OPTION_PAGE_SCAN_NEW_IMMEDIATELY, m_bNewProcessScanningEnabled);

	DDX_Control(pDX, IDC_HPP_OPTION_PAGE_SCAN_MINUTES, m_ctlMinutes);
	DDX_Text(pDX, IDC_HPP_OPTION_PAGE_SCAN_MINUTES, m_dwMinutes);
	DDV_MinMaxDWord(pDX, m_dwMinutes, mmvMinutesMin, mmvMinutesMax);
	DDX_Control(pDX, IDC_HPP_OPTION_PAGE_SCAN_MINUTES_SPIN, m_ctlMinutesSpin);

	DDX_Control(pDX, IDC_HPP_OPTION_PAGE_SCAN_SECONDS, m_ctlSeconds);
	DDX_Text(pDX, IDC_HPP_OPTION_PAGE_SCAN_SECONDS, m_dwSeconds);
	DDV_MinMaxDWord(pDX, m_dwSeconds, mmvSecondsMin, mmvSecondsMax);
	DDX_Control(pDX, IDC_HPP_OPTION_PAGE_SCAN_SECONDS_SPIN, m_ctlSecondsSpin);

	DDX_Control(pDX, IDC_LABEL_HPP_OPTION_PAGE_SCAN_PROTECTED, m_ctlProtectedScanLabel);
	DDX_Control(pDX, IDC_LABEL_HPP_OPTION_PAGE_SCAN_PROTECTED_MIN, m_ctlProtectedScanMinLabel);
	DDX_Control(pDX, IDC_LABEL_HPP_OPTION_PAGE_SCAN_PROTECTED_SEC, m_ctlProtectedScanSecLabel);
}


BEGIN_MESSAGE_MAP(CHPPScanOptionPage, CBasePageUI)
	ON_BN_CLICKED(IDC_HPP_OPTION_PAGE_DEFAULT_SCAN_FREQUENCY, &CHPPScanOptionPage::OnBnClickedHppOptionPageDefaultScanFrequency)
	ON_BN_CLICKED(IDC_HPP_OPTION_PAGE_MANUAL, &CHPPScanOptionPage::OnBnClickedHppOptionPageManual)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HPP_OPTION_PAGE_SCAN_MINUTES_SPIN, &CHPPScanOptionPage::OnDeltaposScanMinutesSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HPP_OPTION_PAGE_SCAN_SECONDS_SPIN, &CHPPScanOptionPage::OnDeltaposSecondsSpin)
END_MESSAGE_MAP()


// CHPPScanOptionPage message handlers

BOOL CHPPScanOptionPage::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CBasePageUI::OnInitDialog();

	m_ctlMinutesSpin.SetBuddy(&m_ctlMinutes);
	m_ctlSecondsSpin.SetBuddy(&m_ctlSeconds);

	m_ctlMinutesSpin.SetRange32(mmvMinutesMin, mmvMinutesMax);
	m_ctlMinutesSpin.SetPos(mmvMinutesMin);

	m_ctlSecondsSpin.SetRange32(mmvSecondsMin, mmvSecondsMax);
	m_ctlSecondsSpin.SetPos(mmvSecondsMin);

	//Enabled
    //m_PortLock.AddBuddy( IDC_POP3_PORT );
    //Reset lock so new buddies get locked.
    //m_PortLock.Lock( m_PortLock.LockValue() );

	Load();	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CHPPScanOptionPage::OnSetActive() 
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	OnUpdateUI();
	return CBasePageUI::OnSetActive();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPScanOptionPage::Load
//
// Description: Load the configuration settings form the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPScanOptionPage::Load()
{
	IGenericConfig* pIGenConfig = GetIGenericConfig();
	if ( pIGenConfig )
	{
		DWORD dwErr = pIGenConfig->Open( NULL, HKEY_VP_STORAGE_REALTIME, reinterpret_cast<char*>(szReg_Key_SymHPPS), GC_MODEL_SINGLE /* flags ignored in Cli* objects */ );
		if ( dwErr )
		{
			// Read
			DWORD dwTotalSeconds = 0;
			Read(dwTotalSeconds, szReg_Val_SymHPPS_ScanFrequency, REG_DEFAULT_SymHPPS_ScanFrequencySecs);
			m_dwMinutes = dwTotalSeconds / 60;
			m_dwSeconds = dwTotalSeconds % 60;

			Read(m_bNewProcessScanningEnabled, szReg_Val_SymHPPS_NewProcessScanningEnabled, REG_DEFAULT_SymHPPS_NewProcessScanningEnabled);
			Read(m_bUseDefaultScanFrequency, szReg_Val_SymHPPS_UseDefaultScanFrequency, REG_DEFAULT_SymHPPS_UseDefaultScanFrequency);
		}
	}
	UpdateData(FALSE);
	OnUpdateUI();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPScanOptionPage::Store
//
// Description: Writes the configuration settings to the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPScanOptionPage::Store()
{
	UpdateData(TRUE);

	IGenericConfig* pIGenConfig = GetIGenericConfig();
	if ( pIGenConfig )
	{
		DWORD dwErr = pIGenConfig->Open( NULL, HKEY_VP_STORAGE_REALTIME, reinterpret_cast<char*>(szReg_Key_SymHPPS), GC_MODEL_SINGLE /* flags ignored in Cli* objects */ );			
		if ( dwErr )
		{
			// Write
			DWORD dwTotalSeconds = m_dwMinutes * 60;
			dwTotalSeconds += m_dwSeconds;
			Write(dwTotalSeconds, szReg_Val_SymHPPS_ScanFrequency);

			Write(m_bNewProcessScanningEnabled, szReg_Val_SymHPPS_NewProcessScanningEnabled);			
			Write(m_bUseDefaultScanFrequency, szReg_Val_SymHPPS_UseDefaultScanFrequency);			
		}
	}
}

void CHPPScanOptionPage::OnCancel()
{
	CBasePageUI::OnCancel();
}

void CHPPScanOptionPage::OnOK()
{
	UpdateData(TRUE);
	Store();

	CBasePageUI::OnOK();
}

void CHPPScanOptionPage::OnUpdateUI()
{
	m_ctlDefaultScanFrequencyScan.SetCheck(m_bUseDefaultScanFrequency ? 1L : 0L);
	m_ctlManualScan.SetCheck(!m_bUseDefaultScanFrequency ? 1L : 0L);
	OnBnClickedHppOptionPageDefaultScanFrequency();

	ValidateScanNewProc();
}

void CHPPScanOptionPage::OnBnClickedHppOptionPageDefaultScanFrequency()
{
	BOOL bEnable = m_ctlDefaultScanFrequencyScan.GetCheck() > 0;
	
	m_ctlNewProcessScanningEnabled.EnableWindow(!bEnable);
	m_ctlMinutes.EnableWindow(!bEnable);
	m_ctlSeconds.EnableWindow(!bEnable);

	m_ctlMinutesSpin.EnableWindow(!bEnable);
	m_ctlSecondsSpin.EnableWindow(!bEnable);

	m_ctlProtectedScanLabel.EnableWindow(!bEnable);
	m_ctlProtectedScanMinLabel.EnableWindow(!bEnable);
	m_ctlProtectedScanSecLabel.EnableWindow(!bEnable);
}

void CHPPScanOptionPage::OnBnClickedHppOptionPageManual()
{
	OnBnClickedHppOptionPageDefaultScanFrequency();
}

void CHPPScanOptionPage::OnDeltaposScanMinutesSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	long nValue = m_dwMinutes;
	nValue += pNMUpDown->iDelta;
	if ( nValue >= mmvMinutesMin && nValue <= mmvMinutesMax )
	{
		CString strValue; strValue.Format(_T("%ld"), nValue);
		m_ctlMinutes.SetWindowText(strValue);

		if ( BumpMinutesToSeconds(nValue, m_dwSeconds) )
		{
			strValue.Format(_T("%ld"), mmvSecondsMax);
			m_ctlSeconds.SetWindowText(strValue);
		}

		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CHPPScanOptionPage::OnDeltaposSecondsSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	ValidateScanNewProc();

	long nValue = m_dwSeconds;
	nValue += pNMUpDown->iDelta;
	if ( nValue >= mmvSecondsMin && nValue <= mmvSecondsMax )
	{
		CString strValue; strValue.Format(_T("%ld"), nValue);
		m_ctlSeconds.SetWindowText(strValue);
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

void CHPPScanOptionPage::ValidateScanNewProc()
{
	if ( ((m_dwMinutes * 60) + m_dwSeconds) <= mmvNewProcessScanWhenRangeUpper )
		m_ctlNewProcessScanningEnabled.SetCheck(0L);
}

BOOL CHPPScanOptionPage::BumpHoursToMinutes(long nHourValue, long nMinuteValue)
{
	if ( nHourValue == mmvHoursMin && nMinuteValue == mmvMinutesMin )
		return TRUE;

	return FALSE;
}

BOOL CHPPScanOptionPage::BumpMinutesToSeconds(long nMinuteValue, long nSecondValue)
{
	if ( nMinuteValue == mmvMinutesMin && nSecondValue == mmvSecondsMin )
		return TRUE;

	return FALSE;
}
