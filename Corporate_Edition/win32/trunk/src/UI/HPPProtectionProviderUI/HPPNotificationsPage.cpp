// HPPNotificationsPage.cpp : implementation file
//

#include "stdafx.h"
#include "HPPNotificationsPage.h"
#include "HPPSheet.h"

// CHPPNotificationsPage dialog

IMPLEMENT_DYNAMIC(CHPPNotificationsPage, CPropertyPage)

CHPPNotificationsPage::CHPPNotificationsPage()
	: CBasePageUI(CHPPNotificationsPage::IDD)
	, m_bDisplayResults(REG_DEFAULT_SymHPPS_DisplayStatusDialog)
	, m_bPromptTerminateProcess(REG_DEFAULT_SymHPPS_ScanNotifyTerminateProcess)
	, m_bPromptStopService(REG_DEFAULT_SymHPPS_ScanNotifyStopService)
{
}

CHPPNotificationsPage::~CHPPNotificationsPage()
{
}

void CHPPNotificationsPage::DoDataExchange(CDataExchange* pDX)
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);
	CBasePageUI::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_HPP_NOTIFICATIONS_DISPLAY_RESULTS, m_bDisplayResults);
	DDX_Check(pDX, IDC_HPP_NOTIFICATIONS_PROMPT_TERMINATE_PROCESS, m_bPromptTerminateProcess);
	DDX_Check(pDX, IDC_HPP_NOTIFICATIONS_PROMPT_STOP_SERVICE, m_bPromptStopService);

	DDX_Control(pDX, IDC_HPP_NOTIFICATIONS_DISPLAY_RESULTS, m_ctlDisplayResults);
	DDX_Control(pDX, IDC_HPP_NOTIFICATIONS_PROMPT_TERMINATE_PROCESS, m_ctlPromptTerminateProcess);
	DDX_Control(pDX, IDC_HPP_NOTIFICATIONS_PROMPT_STOP_SERVICE, m_ctlPromptStopService);
}

BEGIN_MESSAGE_MAP(CHPPNotificationsPage, CBasePageUI)
	ON_BN_CLICKED(IDC_HPP_NOTIFICATIONS_DISPLAY_RESULTS, &CHPPNotificationsPage::OnBnClickedDisplayResults)
END_MESSAGE_MAP()


// CHPPNotificationsPage message handlers

BOOL CHPPNotificationsPage::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CBasePageUI::OnInitDialog();

	Load();
	OnBnClickedDisplayResults();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CHPPNotificationsPage::OnSetActive() 
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	OnUpdateUI();
	return CBasePageUI::OnSetActive();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPNotificationsPage::Load
//
// Description: Load the configuration settings form the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPNotificationsPage::Load()
{
	CBaseSheetUI* pSheet = (CBaseSheetUI*)GetParent();
	ASSERT(pSheet != NULL);

	CConfigObj	config( GetIGenericConfig() );

	m_bDisplayResults = config.GetOption(CString(szReg_Value_DisplayStatusDlg), REG_DEFAULT_SymHPPS_DisplayStatusDialog);
	m_bPromptTerminateProcess = config.GetOption(CString(szReg_Val_ScanNotifyTerminateProcess), REG_DEFAULT_SymHPPS_ScanNotifyTerminateProcess);
	m_bPromptStopService = config.GetOption(CString(szReg_Val_ScanNotifyStopServices), REG_DEFAULT_SymHPPS_ScanNotifyStopService);

	UpdateData(FALSE);
	OnUpdateUI();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPNotificationsPage::Store
//
// Description: Writes the configuration settings to the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPNotificationsPage::Store()
{
	CBaseSheetUI* pSheet = (CBaseSheetUI*)GetParent();
	ASSERT(pSheet != NULL);

	UpdateData(TRUE);

	// Write
	CConfigObj	config( GetIGenericConfig() );

	config.SetOption(CString(szReg_Value_DisplayStatusDlg), m_bDisplayResults);
	config.SetOption(CString(szReg_Val_ScanNotifyTerminateProcess), m_bPromptTerminateProcess);
	config.SetOption(CString(szReg_Val_ScanNotifyStopServices), m_bPromptStopService);
}

void CHPPNotificationsPage::OnCancel()
{
	CBasePageUI::OnCancel();
}

void CHPPNotificationsPage::OnOK()
{
	UpdateData(TRUE);
	Store();

	CBasePageUI::OnOK();
}

void CHPPNotificationsPage::OnUpdateUI()
{
}

void CHPPNotificationsPage::OnBnClickedDisplayResults()
{
	BOOL bEnable = m_ctlDisplayResults.GetCheck() > 0L;
	m_ctlPromptTerminateProcess.EnableWindow(bEnable);
	m_ctlPromptStopService.EnableWindow(bEnable);
}
