// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPBasic.cpp : implementation file
//

#include "stdafx.h"
#include "HPPProtectionProviderUI.h"
#include "HPPBasic.h"


//*********************************************************************
//
//		**** CHPPBasic dialog ****
//
//*********************************************************************
// 

IMPLEMENT_DYNAMIC(CHPPBasic, CDialog)

////////////////////////////////////////////////////////////////////////
// Function:    CHPPBasic::CHPPBasic
//
// Description: constructor for basci panel
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////

CHPPBasic::CHPPBasic(DWORD dwMode, DWORD dwConfiguration, CWnd* pParent /*=NULL*/)
	: CBaseUI(dwMode, dwConfiguration, CHPPBasic::IDD, pParent)
	, m_bEnableHeurPP(false)
{

}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPBasic::~CHPPBasic
//
// Description: Destructor for basic UI panel
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
CHPPBasic::~CHPPBasic()
{
}

void CHPPBasic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENABLE_HPP_RTS, m_ctrlEnableHeurPP);
	DDX_Check(pDX, IDC_ENABLE_HPP_RTS, m_bEnableHeurPP);
	DDX_Control(pDX, IDC_HPP_RT_STATUS, m_ctrlHeurPPStatusText);
	DDX_Control(pDX, IDC_HPP_DETECTIONS, m_ctrlHeurPPDetectionsText);
	DDX_Control(pDX, IDC_NEW_PROC_SCAN_TIME, m_ctrlHeurPPNewPSTText);
	DDX_Control(pDX, IDC_FULL_PROC_SCAN_TIME, m_ctrlHeurPPFPStimeText);
	DDX_Control(pDX, IDC_HPP_PROCESS_DETECTED, m_ctrlHeurPPProcessNameText);
	DDX_Control(pDX, IDC_HPP_FILENAME, m_ctrlHeurPPFilenameText);
	DDX_Control(pDX, IDC_HPP_ACTION_TAKEN, m_ctrlHeurPPActionText);
	DDX_Control(pDX, IDC_HPP_DETECTION_TIME, m_ctrlHeurPPActionTimeText);
}


BEGIN_MESSAGE_MAP(CHPPBasic, CDialog)
	ON_BN_CLICKED(IDC_ENABLE_HPP_RTS, &CHPPBasic::OnBnClickedEnableHPPRts)
	ON_STN_CLICKED(IDC_HPP_RT_STATUS, &CHPPBasic::OnStnClickedHPPRtStatus)
	ON_STN_CLICKED(IDC_HPP_DETECTIONS, &CHPPBasic::OnStnClickedHPPDetections)
END_MESSAGE_MAP()


////////////////////////////////////////////////////////////////////////
// Function:    CHPPBasic::Load
//
// Description: Load the configuration settings form the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPBasic::Load()
{
	CConfigObj	config(m_ptrConfig);

	
	// Process protection
	Read(m_bEnableHeurPP, szReg_Val_SymHPPS_Enabled, FALSE);

	//ScanInterval
	Read(m_dwNewProccessScanInterval, szReg_Val_SymHPPS_NewProcessScanInterval, 0);
	
	Read(m_dwFullProcessScanInterval, szReg_Val_SymHPPS_FullProcessScanInterval,0);

	UpdateData(FALSE);
	OnUpdateUI();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPBasic::Store
//
// Description: Writes the configuration settings to the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPBasic::Store()
{
	CConfigObj	config(m_ptrConfig);

	UpdateData(TRUE);

	// Process protection
	Write(m_bEnableHeurPP, szReg_Val_SymHPPS_Enabled);

}

void CHPPBasic::OnBnClickedEnableHPPRts()
{
	// TODO: Add your control notification handler code here
}

void CHPPBasic::OnStnClickedHPPRtStatus()
{
	// TODO: Add your control notification handler code here
}

void CHPPBasic::OnStnClickedHPPDetections()
{
	// TODO: Add your control notification handler code here
}

BOOL CHPPBasic::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CBaseUI::OnInitDialog();
	Load();

	OnUpdateUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CHPPBasic::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CBaseUI::OnCancel();
}

void CHPPBasic::OnOK()
{
	UpdateData(TRUE);
	Store();

	CBaseUI::OnOK();
}
void CHPPBasic::OnUpdateUI()
{
	CString strWindowText;
	// ATL Loadstring thow catch.  
	
	strWindowText.Format(IDS_NEW_PROC_TIME_FORMAT, m_dwNewProccessScanInterval);
	m_ctrlHeurPPNewPSTText.SetWindowText(strWindowText);

	strWindowText.Format(IDS_NEW_PROC_TIME_FORMAT, m_dwFullProcessScanInterval);
	m_ctrlHeurPPFPStimeText.SetWindowText(strWindowText);

	if(m_bEnableHeurPP == TRUE)
		strWindowText.LoadString(IDS_ENABLED);
	else
		strWindowText.LoadString(IDS_DISABLED);

	m_ctrlHeurPPStatusText.SetWindowText(strWindowText);
}
