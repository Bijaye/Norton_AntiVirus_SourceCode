// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPRTSOptions.cpp : implementation file
//

#include "stdafx.h"
#include "HPPProtectionProviderUI.h"
#include "HPPRTSOptions.h"


//*********************************************************************
//
//		**** CHPPRTSOptions dialog ****
//
//*********************************************************************

IMPLEMENT_DYNAMIC(CHPPRTSOptions, CDialog)
CHPPRTSOptions::CHPPRTSOptions(DWORD dwMode, DWORD dwConfiguration, CWnd* pParent /*=NULL*/)
	: CBaseUI(dwMode, dwConfiguration, CHPPRTSOptions::IDD, pParent)
	, m_bEnableHPPProccessProtect(false)
	, m_bEnableHPPcontiniousScan(false)
	, m_dwNewProccessScanInterval(0)
	, m_dwFullProcessScanInterval(0)
{
}

CHPPRTSOptions::~CHPPRTSOptions()
{
}

void CHPPRTSOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HPP_NEW_PROCESS_SCAN, m_editNewProccessScanInterval);
	DDX_Control(pDX, IDC_EDIT_HPP_FULL_PROCESS_SCAN, m_editFullProccessScanInterval);
	DDX_Check(pDX, IDC_ENABLE_HPP_RTS, m_bEnableHPPProccessProtect);
	DDX_Check(pDX, IDC_HPP_CONTINIOUS_SCAN, m_bEnableHPPcontiniousScan);
	DDX_Control(pDX, IDC_HPP_NEW_PROCCESS_SPIN, m_NewProccesScanIntervalSpin);
	DDX_Control(pDX, IDC_HPP_FULL_PROCCESS_SPIN, m_ProccesFullScanIntervalSpin);
	DDX_Text(pDX, IDC_EDIT_HPP_NEW_PROCESS_SCAN, m_dwNewProccessScanInterval);
	DDX_Text(pDX, IDC_EDIT_HPP_FULL_PROCESS_SCAN, m_dwFullProcessScanInterval);

	DDX_Control(pDX, IDC_HPP_CONTINIOUS_SCAN, m_crtlEnableContiniousScan);
}


BEGIN_MESSAGE_MAP(CHPPRTSOptions, CDialog)
	ON_BN_CLICKED(IDC_ENABLE_HPP_RTS, OnUpdateUI)
	ON_BN_CLICKED(IDC_HPP_CONTINIOUS_SCAN, OnUpdateUI)
	ON_EN_CHANGE(IDC_EDIT_HPP_NEW_PROCESS_SCAN, OnEnChangeEditHPPNewProcessScan)
	ON_EN_CHANGE(IDC_EDIT_HPP_FULL_PROCESS_SCAN, OnEnChangeEditHPPFullProcessScan)
	ON_STN_CLICKED(IDC_NEW_PROCCESS_TEXT, OnStnClickedNewProccessText)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HPP_NEW_PROCCESS_SPIN, OnDeltaposHPPNewProccessSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HPP_FULL_PROCCESS_SPIN, OnDeltaposHPPFullProccessSpin)
//	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_ADVANCED, OnBnClickedAdvanced)
	ON_BN_CLICKED(IDC_ACTIONS_BTN, OnBnClickedActionsBtn)
	ON_BN_CLICKED(IDC_NOTIFY_BTN, OnBnClickedNotifyBtn)
	ON_BN_CLICKED(IDC_BROWSE_EXCLUDE_FILES, OnBnClickedBrowseExcludeFiles)
	ON_WM_DESTROY()
	ON_EN_KILLFOCUS(IDC_EDIT_HPP_FULL_PROCESS_SCAN, &CHPPRTSOptions::OnEnKillfocusEditHPPFullProcessScan)
	ON_EN_KILLFOCUS(IDC_EDIT_HPP_NEW_PROCESS_SCAN, &CHPPRTSOptions::OnEnKillfocusEditHPPNewProcessScan)
END_MESSAGE_MAP()


// CHPPRTSOptions message handlers
BOOL CHPPRTSOptions::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CBaseUI::OnInitDialog();


	m_NewProccesScanIntervalSpin.SetRange(5, 60);
	m_ProccesFullScanIntervalSpin.SetRange(60,600);
	
	Load();

	m_NewProccesScanIntervalSpin.SetPos(m_dwNewProccessScanInterval);
	m_ProccesFullScanIntervalSpin.SetPos(m_dwFullProcessScanInterval);
	
	OnUpdateUI();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CHPPRTSOptions::Load()
{

	CConfigObj	config(m_ptrConfig);

	
	// Process protection
	Read(m_bEnableHPPProccessProtect, szReg_Val_SymHPPS_Enabled, FALSE);
	Read(m_bEnableHPPcontiniousScan, szReg_Val_SymHPPS_ContiniousScan,FALSE);

	//ScanInterval

	Read(m_dwNewProccessScanInterval, szReg_Val_SymHPPS_NewProcessScanInterval, 0);


	Read(m_dwFullProcessScanInterval, szReg_Val_SymHPPS_FullProcessScanInterval,0);

	UpdateData(FALSE);
}

void CHPPRTSOptions::Store()
{
	CConfigObj	config(m_ptrConfig);

	UpdateData(TRUE);

	// Process protection
	Write(m_bEnableHPPProccessProtect, szReg_Val_SymHPPS_Enabled);
	Write(m_bEnableHPPcontiniousScan, szReg_Val_SymHPPS_ContiniousScan);

	//ScanInterval

	Write(m_dwNewProccessScanInterval, szReg_Val_SymHPPS_NewProcessScanInterval);
	Write(m_dwFullProcessScanInterval, szReg_Val_SymHPPS_FullProcessScanInterval);

}

void CHPPRTSOptions::OnBnClickedEnableHPPRts()
{
	// TODO: Add your control notification handler code here
}

void CHPPRTSOptions::OnBnClickedHPPContiniousScan()
{
	// TODO: Add your control notification handler code here
}

void CHPPRTSOptions::OnEnChangeEditHPPNewProcessScan()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBaseUI::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CHPPRTSOptions::OnEnChangeEditHPPFullProcessScan()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBaseUI::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CHPPRTSOptions::OnStnClickedNewProccessText()
{
	// TODO: Add your control notification handler code here
}

void CHPPRTSOptions::OnDeltaposHPPNewProccessSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CHPPRTSOptions::OnDeltaposHPPFullProccessSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

//void CHPPRTSOptions::OnBnClickedOk()
//{
//	// TODO: Add your control notification handler code here
//}

void CHPPRTSOptions::OnBnClickedAdvanced()
{
	// TODO: Add your control notification handler code here
}

void CHPPRTSOptions::OnBnClickedActionsBtn()
{
	// TODO: Add your control notification handler code here
}

void CHPPRTSOptions::OnBnClickedNotifyBtn()
{
	// TODO: Add your control notification handler code here
}

void CHPPRTSOptions::OnBnClickedBrowseExcludeFiles()
{
	// TODO: Add your control notification handler code here
}

void CHPPRTSOptions::OnDestroy()
{
	CBaseUI::OnDestroy();

	// TODO: Add your message handler code here
}

void CHPPRTSOptions::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CBaseUI::OnCancel();
}


void CHPPRTSOptions::OnOK()
{
	Store();

	CBaseUI::OnOK();
}

void CHPPRTSOptions::OnEnKillfocusEditHPPFullProcessScan()
{
    m_NewProccesScanIntervalSpin.SetPos( LOWORD(m_NewProccesScanIntervalSpin.GetPos()) );
}

void CHPPRTSOptions::OnEnKillfocusEditHPPNewProcessScan()
{
    m_ProccesFullScanIntervalSpin.SetPos( LOWORD(m_ProccesFullScanIntervalSpin.GetPos()) );
}
void CHPPRTSOptions::OnUpdateUI()
{
	UpdateData(TRUE);

	m_crtlEnableContiniousScan.EnableWindow(m_bEnableHPPProccessProtect);

	m_editNewProccessScanInterval.EnableWindow(m_bEnableHPPProccessProtect && !m_bEnableHPPcontiniousScan);
	m_editFullProccessScanInterval.EnableWindow(m_bEnableHPPProccessProtect&& !m_bEnableHPPcontiniousScan);
}
