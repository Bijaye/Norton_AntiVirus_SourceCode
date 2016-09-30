// HPPEngineSettingsPage.cpp : implementation file
//

#include "stdafx.h"
#include "HPPEngineSettingsPage.h"
#include "HPPSheet.h"
#include "ccEraserInterface.h"

const static long g_nHPP_QUARANTINE = AC_MOVE;
const static long g_nHPP_TERMINATE	= AC_TERMINATE_PROCESS_ONLY;
const static long g_nHPP_LOG		= AC_NOTHING;

// CHPPEngineSettingsPage dialog

IMPLEMENT_DYNAMIC(CHPPEngineSettingsPage, CPropertyPage)

CHPPEngineSettingsPage::CHPPEngineSettingsPage()
	: CBasePageUI(CHPPEngineSettingsPage::IDD)
	, m_bOverrideTrojanSensitivity(!REG_DEFAULT_SymHPPS_TrojanUseDefaultSensitivity)
	, m_dwTrojanSensitivity(REG_DEFAULT_SymHPPS_TrojanSensitivity)
	, m_bOverrideKeyLoggersSensitivity(!REG_DEFAULT_SymHPPS_KeyLoggerUseDefaultSensitivity)
	, m_dwKeyLoggersSensitivity(REG_DEFAULT_SymHPPS_KeyLoggerSensitivity)
	, m_dwTrojanAction(REG_DEFAULT_SymHPPS_TrojanAction)
	, m_dwKeyLoggersAction(REG_DEFAULT_SymHPPS_KeyLoggerAction)
	, m_bEnableTrojanEngine(REG_DEFAULT_SymHPPS_TrojanEnabled)
  	, m_bEnableKeyLoggersEngine(REG_DEFAULT_SymHPPS_KeyLoggerEnabled)
	, m_dwCommercialKeyloggersAction(REG_DEFAULT_SymHPPS_CommercialKeyloggerAction)
	, m_dwCommercialRemoteControlAction(REG_DEFAULT_SymHPPS_CommercialRemoteControlAction)
{
}

CHPPEngineSettingsPage::~CHPPEngineSettingsPage()
{
}

void CHPPEngineSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);
	CBasePageUI::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_HPP_ENGINE_SETTINGS_TROJAN_ACTION, m_ctlTrojanAction);
	DDX_Control(pDX, IDC_HPP_ENGINE_SETTINGS_TROJAN_OVERRIDE_SENSITIVITY, m_ctlOverrideTrojanSensitivity);
	DDX_Control(pDX, IDC_HPP_ENGINE_SETTINGS_TROJAN_SENSITIVITY, m_ctlTrojanSensitivity);

	DDX_Control(pDX, IDC_CB_HPP_ENGINE_SETTINGS_KEYLOGGERS_ACTION, m_ctlKeyLoggersAction);
	DDX_Control(pDX, IDC_HPP_ENGINE_SETTINGS_KEYLOGGERS_OVERRIDE_SENSITIVITY, m_ctlOverrideKeyLoggersSensitivity);
	DDX_Control(pDX, IDC_HPP_ENGINE_SETTINGS_KEYLOGGERS_SENSITIVITY, m_ctlKeyLoggersSensitivity);
	DDX_Control(pDX, IDC_LABEL_HPP_ENGINE_SETTINGS_TROJAN_ACTION, m_ctlTrojanActionLabel);
	DDX_Control(pDX, IDC_LABEL_HPP_ENGINE_SETTINGS_KEYLOGGERS, m_ctlKeyLoggersActionLabel);

	DDX_Check(pDX, IDC_HPP_ENGINE_SETTINGS_TROJAN_OVERRIDE_SENSITIVITY, m_bOverrideTrojanSensitivity);

	DDX_Check(pDX, IDC_HPP_ENGINE_SETTINGS_KEYLOGGERS_OVERRIDE_SENSITIVITY, m_bOverrideKeyLoggersSensitivity);

	DDX_Control(pDX, IDC_HPP_ENGINE_SETTINGS_TROJAN_ENABLED, m_ctlEnableTrojanEngine);
	DDX_Control(pDX, IDC_HPP_ENGINE_SETTINGS_KEYLOGGERS_ENABLED, m_ctlEnableKeyLoggersEngine);
	DDX_Check(pDX, IDC_HPP_ENGINE_SETTINGS_TROJAN_ENABLED, m_bEnableTrojanEngine);
	DDX_Check(pDX, IDC_HPP_ENGINE_SETTINGS_KEYLOGGERS_ENABLED, m_bEnableKeyLoggersEngine);

	DDX_Control(pDX, IDC_CB_HPP_ENGINE_SETTINGS_CAL_KEYLOGGERS_ACTION, m_ctlCommercialKeyloggerAction);
	DDX_Control(pDX, IDC_CB_HPP_ENGINE_SETTINGS_CAL_REMOTECONTROL_ACTION, m_ctlCommercialRemoteControlAction);
}

BEGIN_MESSAGE_MAP(CHPPEngineSettingsPage, CBasePageUI)
	ON_BN_CLICKED(IDC_HPP_ENGINE_SETTINGS_TROJAN_OVERRIDE_SENSITIVITY, &CHPPEngineSettingsPage::OnBnClickedTrojanOverrideSensitivity)
	ON_BN_CLICKED(IDC_HPP_ENGINE_SETTINGS_KEYLOGGERS_OVERRIDE_SENSITIVITY, &CHPPEngineSettingsPage::OnBnClickedKeyloggersOverrideSensitivity)
	ON_BN_CLICKED(IDC_HPP_ENGINE_SETTINGS_TROJAN_ENABLED, &CHPPEngineSettingsPage::OnBnClickedTrojanEnabled)
	ON_BN_CLICKED(IDC_HPP_ENGINE_SETTINGS_KEYLOGGERS_ENABLED, &CHPPEngineSettingsPage::OnBnClickedKeyloggersEnabled)
END_MESSAGE_MAP()


// CHPPEngineSettingsPage message handlers

BOOL CHPPEngineSettingsPage::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CBasePageUI::OnInitDialog();

	CString strLog; strLog.LoadString(IDS_LOG);
	CString strQuarantine; strQuarantine.LoadString(IDS_QUARANTINE);
	CString strTerminate; strTerminate.LoadString(IDS_TERMINATE);

	int npos = m_ctlTrojanAction.AddString(strQuarantine);
	m_ctlTrojanAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_QUARANTINE);

	npos = m_ctlTrojanAction.AddString(strTerminate);
	m_ctlTrojanAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_TERMINATE);

	npos = m_ctlTrojanAction.AddString(strLog);
	m_ctlTrojanAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_LOG);

	npos = m_ctlKeyLoggersAction.AddString(strQuarantine);
	m_ctlKeyLoggersAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_QUARANTINE);

	npos = m_ctlKeyLoggersAction.AddString(strTerminate);
	m_ctlKeyLoggersAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_TERMINATE);

	npos = m_ctlKeyLoggersAction.AddString(strLog);
	m_ctlKeyLoggersAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_LOG);

	npos = m_ctlCommercialKeyloggerAction.AddString(strQuarantine);
	m_ctlCommercialKeyloggerAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_QUARANTINE);

	npos = m_ctlCommercialKeyloggerAction.AddString(strTerminate);
	m_ctlCommercialKeyloggerAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_TERMINATE);

	npos = m_ctlCommercialKeyloggerAction.AddString(strLog);
	m_ctlCommercialKeyloggerAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_LOG);

	npos = m_ctlCommercialRemoteControlAction.AddString(strQuarantine);
	m_ctlCommercialRemoteControlAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_QUARANTINE);

	npos = m_ctlCommercialRemoteControlAction.AddString(strTerminate);
	m_ctlCommercialRemoteControlAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_TERMINATE);

	npos = m_ctlCommercialRemoteControlAction.AddString(strLog);
	m_ctlCommercialRemoteControlAction.SetItemDataPtr(npos, (VOID*)&g_nHPP_LOG);

	m_ctlTrojanSensitivity.SetTicFreq( dftTicFreq );
	m_ctlTrojanSensitivity.SetLineSize( dftLineSize );
	m_ctlTrojanSensitivity.SetPageSize( dftPageSize );
	m_ctlTrojanSensitivity.SetRange( dftRangeLower, dftRangeUpper, TRUE );

	m_ctlKeyLoggersSensitivity.SetTicFreq( dftTicFreq );
	m_ctlKeyLoggersSensitivity.SetLineSize( dftLineSize );
	m_ctlKeyLoggersSensitivity.SetPageSize( dftPageSize );
	m_ctlKeyLoggersSensitivity.SetRange( dftRangeLower, dftRangeUpper, TRUE );

	Load();	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CHPPEngineSettingsPage::OnSetActive() 
{
	CHPPSheet* pSheet = (CHPPSheet*)GetParent();
	ASSERT(pSheet != NULL);

	OnUpdateUI();
	return CBasePageUI::OnSetActive();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPEngineSettingsPage::Load
//
// Description: Load the configuration settings form the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPEngineSettingsPage::Load()
{
	CBaseSheetUI* pSheet = (CBaseSheetUI*)GetParent();
	ASSERT(pSheet != NULL);

	CConfigObj	config( GetIGenericConfig() );

	// Trojan Store
	config.SetSubOption(szReg_Val_SymHPPS_TrojanEngineConfig);
	m_bEnableTrojanEngine			= config.GetOption(CString(szReg_Val_SymHPPS_EngineEnabled), REG_DEFAULT_SymHPPS_TrojanEnabled);

	m_dwTrojanAction				= config.GetOption(CString(szReg_Val_SymHPPS_EngineAction), REG_DEFAULT_SymHPPS_TrojanAction);

	m_bOverrideTrojanSensitivity	= config.GetOption(CString(szReg_Val_SymHPPS_EngineUseDefaultSensitivity), !REG_DEFAULT_SymHPPS_TrojanUseDefaultSensitivity);

	m_dwTrojanSensitivity			= config.GetOption(CString(szReg_Val_SymHPPS_EngineSensitivity), REG_DEFAULT_SymHPPS_TrojanSensitivity);
	config.SetSubOption(_T(""));

	// KeyLogger Store
	config.SetSubOption(szReg_Val_SymHPPS_KeyloggerEngineConfig);
	m_bEnableKeyLoggersEngine			= config.GetOption(CString(szReg_Val_SymHPPS_EngineEnabled), REG_DEFAULT_SymHPPS_KeyLoggerEnabled);

	m_dwKeyLoggersAction				= config.GetOption(CString(szReg_Val_SymHPPS_EngineAction), REG_DEFAULT_SymHPPS_KeyLoggerAction);

	m_bOverrideKeyLoggersSensitivity	= config.GetOption(szReg_Val_SymHPPS_EngineUseDefaultSensitivity, !REG_DEFAULT_SymHPPS_KeyLoggerUseDefaultSensitivity);

	m_dwKeyLoggersSensitivity			= config.GetOption(szReg_Val_SymHPPS_EngineSensitivity, REG_DEFAULT_SymHPPS_KeyLoggerSensitivity);
	config.SetSubOption(_T(""));

	m_dwCommercialKeyloggersAction		= config.GetOption(CString(szReg_Val_SymHPPS_CommercialKeyloggerAction), REG_DEFAULT_SymHPPS_CommercialKeyloggerAction);
	m_dwCommercialRemoteControlAction	= config.GetOption(CString(szReg_Val_SymHPPS_CommercialRemoteControlAppAction), REG_DEFAULT_SymHPPS_CommercialRemoteControlAction);

	UpdateData(FALSE);
	OnUpdateUI();
}

////////////////////////////////////////////////////////////////////////
// Function:    CHPPEngineSettingsPage::Store
//
// Description: Writes the configuration settings to the config store
//
// Parameters:  none
//
// Returns:     None
//
//
///////////////////////////////////////////////////////////////////////////////
void CHPPEngineSettingsPage::Store()
{
	CBaseSheetUI* pSheet = (CBaseSheetUI*)GetParent();
	ASSERT(pSheet != NULL);

	UpdateData(TRUE);

	m_dwTrojanSensitivity = m_ctlTrojanSensitivity.GetPos();
	m_dwKeyLoggersSensitivity = m_ctlKeyLoggersSensitivity.GetPos();

	int npos = m_ctlTrojanAction.GetCurSel();
	if ( npos != CB_ERR )
	{
		VOID* pValue = m_ctlTrojanAction.GetItemDataPtr(npos);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			m_dwTrojanAction = *pnValue;
		}
	}

	npos = m_ctlKeyLoggersAction.GetCurSel();
	if ( npos != CB_ERR )
	{
		VOID* pValue = m_ctlKeyLoggersAction.GetItemDataPtr(npos);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			m_dwKeyLoggersAction = *pnValue;
		}		
	}

	npos = m_ctlCommercialKeyloggerAction.GetCurSel();
	if ( npos != CB_ERR )
	{
		VOID* pValue = m_ctlCommercialKeyloggerAction.GetItemDataPtr(npos);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			m_dwCommercialKeyloggersAction = *pnValue;
		}		
	}

	npos = m_ctlCommercialRemoteControlAction.GetCurSel();
	if ( npos != CB_ERR )
	{
		VOID* pValue = m_ctlCommercialRemoteControlAction.GetItemDataPtr(npos);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			m_dwCommercialRemoteControlAction = *pnValue;
		}		
	}

	// Write
	CConfigObj	config( GetIGenericConfig() );

	config.SetSubOption(szReg_Val_SymHPPS_TrojanEngineConfig);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineEnabled), m_bEnableTrojanEngine);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineAction), m_dwTrojanAction);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineUseDefaultSensitivity), !m_bOverrideTrojanSensitivity);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineSensitivity), m_dwTrojanSensitivity);
	config.SetSubOption(_T(""));

	config.SetSubOption(szReg_Val_SymHPPS_KeyloggerEngineConfig);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineEnabled), m_bEnableKeyLoggersEngine);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineAction), m_dwKeyLoggersAction);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineUseDefaultSensitivity), !m_bOverrideKeyLoggersSensitivity);
	config.SetOption(CString(szReg_Val_SymHPPS_EngineSensitivity), m_dwKeyLoggersSensitivity);
	config.SetSubOption(_T(""));

	config.SetOption(CString(szReg_Val_SymHPPS_CommercialKeyloggerAction), m_dwCommercialKeyloggersAction);
	config.SetOption(CString(szReg_Val_SymHPPS_CommercialRemoteControlAppAction), m_dwCommercialRemoteControlAction);
}

void CHPPEngineSettingsPage::OnCancel()
{
	CBasePageUI::OnCancel();
}

void CHPPEngineSettingsPage::OnOK()
{
	UpdateData(TRUE);
	Store();

	CBasePageUI::OnOK();
}

void CHPPEngineSettingsPage::OnUpdateUI()
{
	for ( int nIndex = 0L; nIndex < m_ctlTrojanAction.GetCount(); nIndex++ )
	{
		VOID* pValue = m_ctlTrojanAction.GetItemDataPtr(nIndex);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			if ( *pnValue == m_dwTrojanAction )
			{
				m_ctlTrojanAction.SetCurSel(nIndex);
				break;
			}
		}
	}

	for ( int nIndex = 0L; nIndex < m_ctlKeyLoggersAction.GetCount(); nIndex++ )
	{
		VOID* pValue = m_ctlKeyLoggersAction.GetItemDataPtr(nIndex);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			if ( *pnValue == m_dwKeyLoggersAction )
			{
				m_ctlKeyLoggersAction.SetCurSel(nIndex);
				break;
			}
		}
	}

	for ( int nIndex = 0L; nIndex < m_ctlCommercialKeyloggerAction.GetCount(); nIndex++ )
	{
		VOID* pValue = m_ctlCommercialKeyloggerAction.GetItemDataPtr(nIndex);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			if ( *pnValue == m_dwCommercialKeyloggersAction )
			{
				m_ctlCommercialKeyloggerAction.SetCurSel(nIndex);
				break;
			}
		}
	}

	for ( int nIndex = 0L; nIndex < m_ctlCommercialRemoteControlAction.GetCount(); nIndex++ )
	{
		VOID* pValue = m_ctlCommercialRemoteControlAction.GetItemDataPtr(nIndex);
		if ( pValue )
		{
			long* pnValue = (long*)pValue;
			if ( *pnValue == m_dwCommercialRemoteControlAction )
			{
				m_ctlCommercialRemoteControlAction.SetCurSel(nIndex);
				break;
			}
		}
	}

	m_ctlTrojanSensitivity.SetPos(m_dwTrojanSensitivity);
	m_ctlKeyLoggersSensitivity.SetPos(m_dwKeyLoggersSensitivity);

	OnBnClickedTrojanEnabled();
	OnBnClickedKeyloggersEnabled();
}

void CHPPEngineSettingsPage::OnBnClickedTrojanOverrideSensitivity()
{
	BOOL bEnable = m_ctlOverrideTrojanSensitivity.GetCheck() > 0L;
	m_ctlTrojanSensitivity.EnableWindow(bEnable);
}

void CHPPEngineSettingsPage::OnBnClickedKeyloggersOverrideSensitivity()
{
	BOOL bEnable = m_ctlOverrideKeyLoggersSensitivity.GetCheck() > 0L;
	m_ctlKeyLoggersSensitivity.EnableWindow(bEnable);
}

void CHPPEngineSettingsPage::OnBnClickedTrojanEnabled()
{
 	BOOL bEnable = m_ctlEnableTrojanEngine.GetCheck() > 0L;
 	m_ctlTrojanAction.EnableWindow(bEnable);
 	m_ctlOverrideTrojanSensitivity.EnableWindow(bEnable);
 	m_ctlTrojanSensitivity.EnableWindow(bEnable);
 	m_ctlTrojanActionLabel.EnableWindow(bEnable);
 	if ( bEnable ) OnBnClickedTrojanOverrideSensitivity();
}

void CHPPEngineSettingsPage::OnBnClickedKeyloggersEnabled()
{
 	BOOL bEnable = m_ctlEnableKeyLoggersEngine.GetCheck() > 0L;
 	m_ctlKeyLoggersAction.EnableWindow(bEnable);
 	m_ctlOverrideKeyLoggersSensitivity.EnableWindow(bEnable);
 	m_ctlKeyLoggersSensitivity.EnableWindow(bEnable);
	m_ctlKeyLoggersActionLabel.EnableWindow(bEnable);
	if ( bEnable ) OnBnClickedKeyloggersOverrideSensitivity();
}
