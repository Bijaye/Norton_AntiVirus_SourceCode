////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "IWPSettings.h"
#include "IWPSettingsInterface.h"
#include "FWSetupInterface.h"
#include "FWLoaders.h"
#include "FWSettings.h"
#include "build.h"				// for our version information

#include "ccVerifyTrustLoader.h"
#include "ccRegistry.h"
#include "ccSettingsManagerHelper.h"
#include "ccSymInstalledApps.h"
#include "IWPSettingsShared.h"
#include "isLocations.h"

#include "ArbitratorInterface.h"
#include "ArbitratorLoader.h"

namespace IWP
{


// Trust
bool CIWPSettings::g_bValidated = false;

CIWPSettings::CIWPSettings(void)
{

}

CIWPSettings::~CIWPSettings(void)
{
}

// IIWPSettings interface
const SYMRESULT CIWPSettings::GetValue ( const int iProperty /*in*/,
                                         DWORD& dwResult /*out*/)
{
	switch (iProperty)
	{
	// We now only implement IWPState
	case IWPState:
	{
		dwResult = IWPStateError;

		bool bYieldState = false;
		SYMRESULT sr = GetYield(bYieldState);
		if (SYM_FAILED(sr))
			return sr;

		if (bYieldState)
			dwResult = IWPStateYielding;	// we will use this flag to tell the caller that we are "yielded"
		else
			dwResult = IWPStateEnabled;		// we will use this flag to tell the caller that we are "unyielded"

		return SYM_OK;
	}
		break;
	// We ignore other flags that are passed in because they are not valid anymore.  Clients will have to change the way
	// they call into us now.
	default:
		return SYMERR_INVALIDARG;
	}

}

// IIWPSettings interface
SYMRESULT CIWPSettings::SetValue ( const int iProperty /*in*/,
                                   const DWORD dwValue /*on*/)
{
    return SYMERR_NOTIMPLEMENTED;
}


// IIWPSettings interface
SYMRESULT CIWPSettings::SetYield(const DWORD dwYield, LPCSTR lpcszAppID)
{
	SYMRESULT sr = SYMERR_UNKNOWN;
	if (dwYield)
	{
		sr = PerformYield();
	}
	else
	{
		sr = PerformUnYield();
	}

	return sr;
}

// IIWPSettings interface
SYMRESULT CIWPSettings::Save ()
{
    return SYMERR_NOTIMPLEMENTED;
}


// IIWPSettings interface
SYMRESULT CIWPSettings::Reload ()
{
    return SYMERR_NOTIMPLEMENTED;
}

bool CIWPSettings::isItSafe ()
{
	if (!g_bValidated)
	{
        if (GetDisabled())
        {
            g_bValidated = true;
            return true;
        }

		ccVerifyTrust::ccVerifyTrust_IVerifyTrust TrustLoader;
		ccVerifyTrust::IVerifyTrustPtr pVerifyTrust;

		if(SYM_SUCCEEDED(TrustLoader.CreateObject(pVerifyTrust.m_p)) && (pVerifyTrust != NULL))
		{
			if(pVerifyTrust->Create(true) == ccVerifyTrust::eNoError)
			{
				if (pVerifyTrust->VerifyCurrentProcess(ccVerifyTrust::eSymantecSignature) 
					== ccVerifyTrust::eVerifyError)
                {
                    CCTRACEE(L"CIWPSettings::isItSafe - not verified");
					return false;
                }
			}
		}

		g_bValidated = true;
	}

	return g_bValidated;
}

bool
CIWPSettings::InitFWSetup()
{
	if (! m_pFWSetup)
	{
		SYMRESULT sr = m_FWSetupLoader.CreateObject(m_pFWSetup);
		if (SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to create FWSetup: %08x"), sr);
			return false;
		}
	}

	return true;
}

bool
CIWPSettings::InitSNDHelper()
{
	if (!m_pSNDHelper.m_p)
	{
		return CSNDHelper::CreateSNDHelper(SND_CLIENT_NAV, m_pSNDHelper);
	}

	return true;
}

SYMRESULT
CIWPSettings::PerformYield()
{
	CCTRCTXI0(_T("Begin"));

	// Init FWSetup
	if (! InitFWSetup())
		return SYMERR_UNKNOWN;

	//
	// Yield:
	// 1. Remove FWAgent from ccEvtMgr plugin
	//
	HRESULT hr = m_pFWSetup->RevokeFromEventManager();
	if (FAILED(hr))
	{
		CCTRCTXE0(_T("Failed to revoke CFW from ccEvtMgr plugin"));
		return SYMERR_UNKNOWN;
	}

	//
	// 2. Remove FWAlert from ccApp plugin.
	//

	ccSettings::CSettingsManagerHelper ccSettingsManagerHelper;
	ccSettings::ISettingsManagerPtr pSettingsManager;

	SYMRESULT sr = ccSettingsManagerHelper.Create(pSettingsManager);
	if (SYM_FAILED(sr) || !pSettingsManager)
	{
		CCTRCTXE1(_T("Failed to create ccSettingsManagerHelper: %08x"), sr);
		return sr;
	}

	ccSettings::ISettingsPtr pSettings;
	sr = pSettingsManager->GetSettings(g_szCCAppPluginPath, &(pSettings.m_p));
	if (SYM_FAILED(sr) || !pSettings)
	{
		CCTRCTXE2(_T("Failed to get ccSettings path [%s]: %08x"), g_szCCAppPluginPath, sr);
		return sr;
	}

	TCHAR szBuff[MAX_PATH] = {0};
	DWORD dwBuffSize = MAX_PATH;

	// TODO: Verify g_szFWAlertName name entry in ccApp plugin ccSettings key is correct.
	sr = pSettings->GetString(IWP::g_szFWAlertKeyName, szBuff, dwBuffSize);
	if (sr == SYM_SETTINGS_NOT_FOUND)
	{
		CCTRCTXI0(_T("FWAlert ccApp plugin not found.  Product already yielded somehow"));
		return SYM_OK;
	}
	else if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to get [%s]: %08x"), g_szFWAlertKeyName, sr);
		return sr;
	}

	sr = pSettings->DeleteValue(g_szFWAlertKeyName);
	if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to delete [%s]: %08x"), g_szFWAlertKeyName, sr);
		return sr;
	}

	sr = pSettingsManager->PutSettings(pSettings);
	if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to PutSettings to [%s]: %08x"), g_szCCAppPluginPath, sr);
		return sr;
	}

	CCTRCTXI0(_T("Exit"));

	return SYM_OK;
};

SYMRESULT
CIWPSettings::PerformUnYield()
{
	CCTRCTXI0(_T("Begin"));
	// Init FWSetup
	if (! InitFWSetup())
		return SYMERR_UNKNOWN;

	//
	// UnYield:
	// 1. Register FWAgent as ccEvtMgr plugin
	//

	CString cstrFWAgentPath;
	if (! IWP::CIWPSettingsShared::GetFWAgentPath(cstrFWAgentPath))
		return SYMERR_UNKNOWN;

	HRESULT hr = m_pFWSetup->RegisterToEventManager((LPCWSTR) cstrFWAgentPath);
	if (FAILED(hr))
	{
		CCTRCTXE2(_T("Failed to register [%s] to EvtMgr: %08x"), cstrFWAgentPath, hr);
		return SYMERR_UNKNOWN;
	}

	//
	// 2. Register FWAlert as ccApp plugin.
	//
	ccSettings::CSettingsManagerHelper ccSettingsManagerHelper;
	ccSettings::ISettingsManagerPtr pSettingsManager;

	SYMRESULT sr = ccSettingsManagerHelper.Create(pSettingsManager);
	if (SYM_FAILED(sr) || !pSettingsManager)
	{
		CCTRCTXE1(_T("Failed to create ccSettingsManagerHelper: %08x"), sr);
		return sr;
	}

	ccSettings::ISettingsPtr pSettings;
	sr = pSettingsManager->GetSettings(g_szCCAppPluginPath, &(pSettings.m_p));
	if (SYM_FAILED(sr) || !pSettings)
	{
		CCTRCTXE2(_T("Failed to get ccSettings path [%s]: %08x"), g_szCCAppPluginPath, sr);
		return sr;
	}

	CString cstrFWAlertPath;
	if (! IWP::CIWPSettingsShared::GetFWAlertPath(cstrFWAlertPath))
		return SYMERR_UNKNOWN;

	sr = pSettings->PutString(g_szFWAlertKeyName, cstrFWAlertPath);
	if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to create string at [%s]: %08x"), g_szFWAlertKeyName, sr);
		return sr;
	}

	sr = pSettingsManager->PutSettings(pSettings);
	if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to PutSettings on [%s]: %08x"), g_szCCAppPluginPath, sr);
		return sr;
	}

	// Call IArbit::Aribtrate() to perform factory reset, ALE + Default Rules import
	sr = InvokeIArbit();
	if (SYM_FAILED(sr))
		CCTRCTXE1(_T("Failed to invoke IArbit: %08X"), sr);

	// Set our port blocking flags
	sr = SetPortBlockingFlags();
	if (SYM_FAILED(sr))
		CCTRCTXE1(_T("Failed to set port blocking flags: %08X"), sr);

	// Enable SND
	hr = m_pFWSetup->ToggleSNDFirewall(true, true);
	if (FAILED(hr))
		CCTRCTXE1(_T("Failed to ToggleSNDFirewall: %08X"), hr);

	//Add Fake NIS Installation.
	//Put the NIS Installed Apps Registry key back, so that NAV 2007 can fake that NPF 7.0 is installed,
	//and there by block NPF 7.0. (NAV dir alreday has a nisver.dat, we just need to point NIS InstalledApps
	//path to NAV dir)
	CRegKey rk;
	if(ERROR_SUCCESS == rk.Create(HKEY_LOCAL_MACHINE, isRegLocations::szInstalledAppsKey))
	{
		CString sNAVDir;
		ccSym::CInstalledApps::GetNAVDirectory(sNAVDir);
		if(ccSym::CInstalledApps::GetNAVDirectory(sNAVDir))
		{
			if(ERROR_SUCCESS != rk.SetStringValue(isRegLocations::szInstalledAppsValue, sNAVDir))
				CCTRACEE(CCTRCTX _T("Failed to SetStringValue %s"),isRegLocations::szInstalledAppsValue);
			if(ERROR_SUCCESS != rk.SetStringValue(isRegLocations::szInstalledAppsValue_Legacy, sNAVDir))
				CCTRACEE(CCTRCTX _T("Failed to SetStringValue %s"),isRegLocations::szInstalledAppsValue_Legacy);
		}
		else
		{
			CCTRACEE(CCTRCTX _T("Failed to GetNAVDirectory"));
		}
	}

	CCTRCTXI0(_T("Exit"));

	return SYM_OK;
}

SYMRESULT
CIWPSettings::InvokeIArbit()
{
	ArbitratorLoader arbitLoader;
	IArbitratorPtr pArbitrator;

	SYMRESULT sr = arbitLoader.CreateObject(pArbitrator);
	if (SYM_FAILED(sr))
	{
		CCTRCTXE1(_T("Failed to create IArbit: %08X"), sr);
		return sr;
	}

	// Call Arbitrate to perform all its tasks: Factory Reset, Import rules.
	HRESULT hr = pArbitrator->Arbitrate();
	if (FAILED(hr))
	{
		CCTRCTXE1(_T("Failed to Arbitrate(): %08X"), hr);
		return SYMERR_UNKNOWN;
	}

	return SYM_OK;
}

SYMRESULT
CIWPSettings::GetYield(bool& bYieldState)
{
	//
	// Check to see if 
	// 1. FWAgent ccEvtMgr plugin is registered
	//

	ccSettings::CSettingsManagerHelper ccSettingsManagerHelper;
	ccSettings::ISettingsManagerPtr pSettingsManager;

	SYMRESULT sr = ccSettingsManagerHelper.Create(pSettingsManager);
	if (SYM_FAILED(sr) || !pSettingsManager)
	{
		CCTRCTXE1(_T("Failed to create ccSettingsManagerHelper: %08x"), sr);
		return sr;
	}

	ccSettings::ISettingsPtr pSettings;
	sr = pSettingsManager->GetSettings(CCEVTMGR_PLUGIN, &(pSettings.m_p));
	if (SYM_FAILED(sr) || !pSettings)
	{
		CCTRCTXE2(_T("Failed to get ccSettings path [%s]: %08x"), CCEVTMGR_PLUGIN, sr);
		return sr;
	}

	TCHAR szBuff[MAX_PATH] = {0};
	DWORD dwBuffSize = MAX_PATH;

	sr = pSettings->GetString(FWAGENT_KEY, szBuff, dwBuffSize);
	if (sr == SYM_SETTINGS_NOT_FOUND)
	{
		CCTRCTXI0(_T("FWAgent not registered with ccEvtMgr.  We are in the yielded state."));

		bYieldState = true;
		return SYM_OK;
	}
	else if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to get [%s] key: %08x"), FWAGENT_KEY, sr);
		return sr;
	}

	//
	// 2. FWAlert ccApp plugin is registered
	//

	pSettings = NULL;
	sr = pSettingsManager->GetSettings(g_szCCAppPluginPath, &(pSettings.m_p));
	if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to get [%s] key: %08x"), g_szCCAppPluginPath, sr);
		return sr;
	}

	szBuff[0] = _T('\0');
	dwBuffSize = MAX_PATH;

	sr = pSettings->GetString(g_szFWAlertKeyName, szBuff, dwBuffSize);
	if (sr == SYM_SETTINGS_NOT_FOUND)
	{
		CCTRCTXI0(_T("FWAlert not registered with ccApp.  We are in the yielded state."));

		bYieldState = true;
		return SYM_OK;
	}
	else if (SYM_FAILED(sr))
	{
		CCTRCTXE2(_T("Failed to get [%s] key: %08x"), g_szFWAlertKeyName, sr);
		return sr;
	}

	CCTRCTXI0(_T("Both FWAgent & FWAlert are registered.  We are in the unyielded state."));

	bYieldState = false;

	return SYM_OK;
}

SYMRESULT CIWPSettings::SetPortBlockingFlags()
{
	if (!InitSNDHelper() || !m_pSNDHelper)
	{
		CCTRCTXE0(_T("Failed to init SNDHelper"));
		return SYMERR_UNKNOWN;
	}

	// First read the current FS_PBDecisionFlags value
	DWORD dwPBDecisionFlags = 0;

	if (! m_pSNDHelper->GetSNDSetting(SymNeti::FS_PBDecisionFlags, dwPBDecisionFlags))
	{
		CCTRCTXE0(_T("Failed to get PBDecisionFlags"));
		return SYMERR_UNKNOWN;
	}

	// Set the "Allow When Found" flag
	dwPBDecisionFlags |= SymNeti::ISymNetSettings::eAllowWhenFound;

	// Set the "Block when NOT Found" flag to OFF
	dwPBDecisionFlags &= ~SymNeti::ISymNetSettings::eBlockWhenNOTFound;

	// Write new value back
	if (! m_pSNDHelper->SetSNDSetting(SymNeti::FS_PBDecisionFlags, dwPBDecisionFlags))
	{
		CCTRCTXE0(_T("Failed to set PBDecisionFlags"));
		return SYMERR_UNKNOWN;
	}

	return SYM_OK;
}

SYMRESULT CIWPSettings::SetOEMState(SymNeti::ISymNetSettings::OEMState state)
{
	if (!InitSNDHelper() || !m_pSNDHelper)
	{
		CCTRCTXE0(_T("Failed to init SNDHelper"));
		return SYMERR_UNKNOWN;
	}

	if (! m_pSNDHelper->SetSNDSetting(SymNeti::FS_OEMState, state))
	{
		CCTRCTXE0(_T("SNSetOEMState: FAILED"));
		return SYMERR_UNKNOWN;
	}
	
	return SYM_OK;
}

// IIWPSettings2
SYMRESULT CIWPSettings::ConfigureIWP(DWORD dwConfigureTasks, const cc::IKeyValueCollection* pTaskKVC) throw()
{
	SYMRESULT	result	= S_OK,
				sr		= S_OK;	

	//
	// Configure PortBlocking flags
	//
	if (dwConfigureTasks & ConfigureIWP_SetPortBlockingFlags)
	{
		CCTRCTXI0(_T("ConfigureIWP_SetPortBlockingFlags specified"));

		sr = SetPortBlockingFlags();
		if (SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("SetPortBlocking Flags returned: %08X"), sr);
			result = SYMERR_UNKNOWN;
		}
	}

	//
	// Configure OEMState flags
	//
	if (dwConfigureTasks & ConfigureIWP_SetOEMState)
	{
		CCTRCTXI0(_T("ConfigureIWP_SetOEMState specified"));

		sr = SetOEMState(SymNeti::ISymNetSettings::eEnableSymTdiLoad);
		if (SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("SetOEMStateFlag Flags returned: %08X"), sr);
			result = SYMERR_UNKNOWN;
		}
	}

	return result;
}


}