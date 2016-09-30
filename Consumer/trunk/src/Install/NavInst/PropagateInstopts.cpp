////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// PropagateInstopts.cpp: implements propagation of options to CfgWiz.dat
//
// COLLIN DAVIS
/////////////////////////////////////////////////////////////////////////////////////////////

#include "helper.h"
#include "InstOptsNames.h"
#include "OptNames.h"
#include "NAVOptMigrate.h"
#include "NAVInstoptsNames.h"

//installtoolbox headers
#include "CommonStructs.h"
#include "propagate.h"
#include "instopts.h"
#include "fileaction.h"
#include "CLTNamedProperties.h"
#include "CustomAction.h"

using namespace InstallToolBox;

/////////////////////////////////////////////////////////////////////////////////////////////
//  Propagate Instopts to CfgWiz.Dat
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) BOOL __stdcall PropagateInstopts(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug PropagateInstopts()", "Debug Break", NULL);
#endif

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("PropagateInstopts() Starting."));

	HRESULT hrCoInit = E_FAIL;

	//
	// set up the map for values to propagated from instopts.dat to cfgwiz.dat
	//
	ITB_INSTOPTStoDATFILE lpmapInstoptsToCfgWiz[] =
	{
		/*{is it a str?, key in srource file, key in dest file, def str val, def int val, must exist, managed must exist}*/ 
		{FALSE, INSTOPTS_EnableAutoLiveUpdate, CFGWIZ_EnableAutoLiveUpdate, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableAutoProtect, CFGWIZ_EnableAutoProtect, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableEmailScanner, CFGWIZ_EnableEmailScanner, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableEmailIn, CFGWIZ_EnableEmailScannerIn, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableEmailOut, CFGWIZ_EnableEmailScannerOut, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_FactoryFileExist, CFGWIZ_FactoryFileExists, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_FactoryRegKeyExist, CFGWIZ_FactoryRegKeyExists, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_PostConfigScan, CFGWIZ_PostConfigScan, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_RebootCount, CFGWIZ_RebootCount, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_OEM, CFGWIZ_OEM, NULL, 0, FALSE, FALSE},
		{TRUE, INSTOPTS_FactoryFile, CFGWIZ_FactoryFile, NULL, 0, FALSE, FALSE},
		{TRUE, INSTOPTS_FactoryRegKey, CFGWIZ_FactoryRegKey, _T("HKLM\\Software\\Symantec"), 0, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableRegistration, CFGWIZ_EnableRegistration, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableAlertDefs, CFGWIZ_EnableAlertDefs, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_APRebootCount, CFGWIZ_APRebootCount, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableMSNScanning, CFGWIZ_EnableMSNScan, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_Reseal, CFGWIZ_Reseal, NULL, 0, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowEnableWeeklyScan, CFGWIZ_ShowEnableWeeklyScan, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowLaunchFullSysScan, CFGWIZ_ShowLaunchFullSysScan, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowLaunchLiveUpdate, CFGWIZ_ShowLaunchLiveUpdate, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_EnableWeeklyScan, CFGWIZ_EnableWeeklyScan, NULL, 1, FALSE, FALSE},
		{TRUE, INSTOPTS_CipherID, CFGWIZ_CipherID, _T(""), 0, FALSE, FALSE},
		{TRUE, INSTOPTS_CipherTag, CFGWIZ_CipherTag, _T(""), 0, FALSE, FALSE},
		{FALSE, INSTOPTS_NODRIVERS, CFGWIZ_NODRIVERS, NULL, 1, FALSE, FALSE},
		{FALSE, AntiVirus::INSTOPTS_DRIVERS_MANUAL_START,AntiVirus::CFGWIZ_DRIVERS_MANUAL_START, NULL,  1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableIWP, CFGWIZ_EnableIWP, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_Show_TurnOffWindowsFW_Btn, CFGWIZ_ShowICFCheckbox, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_TurnOff_WindowsFW, CFGWIZ_TurnOffICF, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowWelcomePage, CFGWIZ_ShowWelcomePage, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_EnableLUExpressMode, CFGWIZ_EnableLUExpressMode, NULL, 0, FALSE, FALSE},
		{FALSE, CFGWIZ_EnableLUExpressAutoStart, CFGWIZ_EnableLUExpressAutoStart, NULL, 0, FALSE, FALSE},
		{FALSE, CFGWIZ_EnableLUExpressAutoStop, CFGWIZ_EnableLUExpressAutoStop, NULL, 0, FALSE, FALSE},
		{FALSE, CFGWIZ_DisableLUExpressStopBtn, CFGWIZ_DisableLUExpressStopBtn, NULL, 0, FALSE, FALSE},
        {FALSE, INSTOPTS_SymProtect_Enabled, CFGWIZ_SymProtect_Enabled, NULL, 1, FALSE, FALSE},
        {FALSE, CFGWIZ_ShowUpsellBasics, CFGWIZ_ShowUpsellBasics, NULL, -1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowUpsellEmailMsg, CFGWIZ_ShowUpsellEmailMsg, NULL, -1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowUpsellWebBrowse, CFGWIZ_ShowUpsellWebBrowse, NULL, -1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowUpsellDataRecovery, CFGWIZ_ShowUpsellDataRecovery, NULL, -1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowUpsellPerformance, CFGWIZ_ShowUpsellPerformance, NULL, -1, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowWSC, INSTOPTS_ShowWSC, NULL, 1, FALSE, FALSE}, 
		{FALSE, INSTOPTS_NSCSystray, INSTOPTS_NSCSystray, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_HideNPCWUAlerts, INSTOPTS_HideNPCWUAlerts, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_HideNPCIEAlerts, INSTOPTS_HideNPCIEAlerts, NULL, 0, FALSE, FALSE}, 
		{FALSE, INSTOPTS_HideNPCUACAlerts, INSTOPTS_HideNPCUACAlerts, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowSecurityLink, INSTOPTS_ShowSecurityLink, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowConnectingLink, INSTOPTS_ShowConnectingLink, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowOnlineLink, INSTOPTS_ShowOnlineLink, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowMediaLink, INSTOPTS_ShowMediaLink, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowAccountLink, INSTOPTS_ShowAccountLink, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowTechSupportLink, INSTOPTS_ShowTechSupportLink, NULL, 0, FALSE, FALSE},
		{TRUE, INSTOPTS_GRBaseURL, CFGWIZ_GRBaseURL, NULL, 1, FALSE, FALSE},
		{TRUE, "INSTOPTS:WebRegURL", "WebRegURL", NULL, 1, FALSE, FALSE},
		{TRUE,  INSTOPTS_RegistrationServerWebURL, CFGWIZ_RegistrationServerWebURL, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowUninstallSubWarningPage, CFGWIZ_ShowUninstallSubWarningPage, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_ShowDisableWDCheckBox, CFGWIZ_ShowDisableWDCheckBox, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_DisableWDChecked, CFGWIZ_DisableWDChecked, NULL, 1, FALSE, FALSE},
        {TRUE, AntiVirus::INSTOPTS_DRM_UPGRADE_PRODUCT_IDS, AntiVirus::CFGWIZ_DRM_UPGRADE_PRODUCT_IDS, NULL, 0, FALSE, FALSE}
	};

	TCHAR szLicenseType[] = _T("DRM:DRM::LicenseType");

	//
	// set up the map for values to propagated from instopts.dat to navopts.dat/def
	//
	ITB_INSTOPTStoDATFILE lpmapInstoptsToNavopts[] =
	{
		/*{is it a str?, key in srource file, key in dest file, def str val, def int val, must exist, managed must exist}*/
		{TRUE, INSTOPTS_BuildType, CC_ERR_BUILD_TYPE, _T("RETAIL"), 0, FALSE, FALSE},
		{FALSE, INSTALL_NoThreatCat, THREAT_NoThreatCat, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_MaxDefsAge, DEFALERT_MaxDefsAge, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_StartSubWarning, DEFALERT_StartSubWarning, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_FirstAlertDelay, DEFALERT_FirstAlertDelay, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_VirusDefsDelay, DEFALERT_VirusDefsDelay, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_CycleSubWarning, DEFALERT_CycleSubWarning, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_CycleSubExpired, DEFALERT_CycleSubExpired, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_CycleSubWarningAutoRenew, DEFALERT_CycleSubWarningAutoRenew, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_CycleSubExpiredAutoRenew, DEFALERT_CycleSubExpiredAutoRenew, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_CycleLicWarning, DEFALERT_CycleLicWarning, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_CycleLicExpired, DEFALERT_CycleLicExpired, NULL, 0, FALSE, FALSE},
		{FALSE, DEFALERT_EnableSubsAlert, DEFALERT_EnableSubsAlert, NULL, 1, FALSE, FALSE},
		{FALSE, DEFALERT_EnableALUDisabledAlert, DEFALERT_EnableALUDisabledAlert, NULL, 1, FALSE, FALSE},		
		{FALSE, szLicenseType, szLicenseType, NULL, 0, FALSE, FALSE},
		{FALSE, LICENSE_Aggregated, LICENSE_Aggregated, NULL, 0, FALSE, FALSE},
		{FALSE, LICENSE_Licensed, LICENSE_Licensed, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowProductReg, HELPMENU_ShowProductReg, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowSymWebsite, HELPMENU_ShowSymWebsite, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowSupport, HELPMENU_ShowSupport, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowResponse, HELPMENU_ShowResponse, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowManageService, HELPMENU_ShowManageService, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowActivation, HELPMENU_ShowActivation, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowRenewalCenter, HELPMENU_ShowRenewalCenter, NULL, 0, FALSE, FALSE},
		{FALSE, HELPMENU_ShowClubSymantec, HELPMENU_ShowClubSymantec, NULL, 0, FALSE, FALSE},
		{FALSE, INSTALL_NoUpsellIcon, HELPMENU_NoUpsellIcon, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_HideNAVTrayIconOption, NAVAP_ShowIcon, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_RunLiveReg, "RegisterProduct", NULL, 1, FALSE, FALSE},
                {FALSE, INSTOPTS_SymProtect_Enabled, SYMPROTECT_FeatureEnabled, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_ScanTime, SCANNER_ScanTime, NULL, 20, FALSE, FALSE}, // Default = 20:00 (8 PM)
		{FALSE, INSTOPTS_ScanDay, SCANNER_ScanDay, NULL, 5, FALSE, FALSE}, // Default = Friday; 0 = Sunday
		{FALSE, INSTOPTS_EnableScanCookies, SCANNER_ScanTrackingCookies, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat4, THREAT_Threat4, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat5, THREAT_Threat5, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat6, THREAT_Threat6, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat7, THREAT_Threat7, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat8, THREAT_Threat8, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat9, THREAT_Threat9, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat10, THREAT_Threat10, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableThreat11, THREAT_Threat11, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableEmailIn, AVEMAIL_ScanIncoming, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableEmailOut, AVEMAIL_ScanOutgoing, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_EnableMSNScanning, IMSCAN_ScanMIM, NULL, 1, FALSE, FALSE}
	};

	//
	// set up the map for values to propagated from sku.dat to navopts.dat/def
	ITB_INSTOPTStoDATFILE lpmapSkuToNavopts[] =
	{
		/*{is it a str?, key in srource file, key in dest file, def str val, def int val, must exist, managed must exist}*/
		{TRUE, "OPSKU", LICENSE_MEDIA_SKU, _T("0"), 0, FALSE, FALSE},
		{TRUE, "SPEFSKU", LICENSE_FAMILY_SKU, _T("0"), 0, FALSE, FALSE},
		{TRUE, "PNU", "SKU:PNU", _T("0"), 0, FALSE, FALSE},
		{TRUE, "PLN", "SKU:PLN", _T("0"), 0, FALSE, FALSE},
		{TRUE, "PARTNERNAME", "SKU:PARTNERNAME", _T("0"), 0, FALSE, FALSE},
		{TRUE, "PARTNERTYPE", "SKU:PARTNERTYPE", _T("0"), 0, FALSE, FALSE},
		{TRUE, "EBESERVERURL", "SKU:EBESERVERURL", _T("0"), 0, FALSE, FALSE},
		{TRUE, "EBESERVERURLALT", "SKU:EBESERVERURLALT", _T("0"), 0, FALSE, FALSE}
	};

	//
	// set up the map for values to propagate from sku.dat to cfgwiz.dat
	//
	ITB_INSTOPTStoDATFILE lpmapSkuToCfgWiz[] =
	{
		/*{is it a str?, key in srource file, key in dest file, def str val, def int val, must exist, managed must exist}*/
		{TRUE, INSTOPTS_VendorName, CFGWIZ_VendorName, NULL, 0, FALSE, FALSE}
	};

	//
	// set up the map for values to propagated from instopts.dat to version.dat
	//
	tagINSTOPTStoINIFILE itbInstoptsToVersionDat[] = 
	{	
		{FALSE, "Versions", INSTOPTS_OEM, _T("NAVOEM"), _T(""), 0},
	};

	//
	// set up the map for values to propagated from sku.dat to version.dat
	//
	tagINSTOPTStoINIFILE itbSkuToVersionDat[] =
	{
		/* is it a str?, ini section, src key name, dest key name, default str val, default int val */
		{TRUE, "Versions", INSTOPTS_VendorName, _T("OEMVendor"), _T(""), 0},
		{TRUE, "Versions", "PLN", _T("Language"), _T(""), 0},
		{TRUE, "Versions", INSTOPTS_SKUDESC, _T("SKUDESC"), _T(""), 0}
	};

	try
	{
		// Get NAV install location.
		TCHAR szInstallDir[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		HKEY hKey = NULL;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSymAppsPath, REG_OPTION_NON_VOLATILE, KEY_READ, &hKey) == ERROR_SUCCESS)
		{       
			if(RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)szInstallDir, &dwSize) != ERROR_SUCCESS)
			{                   
				throw runtime_error("Unable to find NAV Install key value.");
			}

			RegCloseKey(hKey);
		}
		else
			throw runtime_error("Unable to find NAV Install key.");

		// find the dat files
		TCHAR szCfgWizDat[MAX_PATH] = {0};
		TCHAR szVersionDat[MAX_PATH] = {0};
		TCHAR szNAVOptsDat[MAX_PATH] = {0};
		TCHAR szNAVOptsDef[MAX_PATH] = {0};
		TCHAR szSKUPath[MAX_PATH] = {0};
		TCHAR szSourceDir[MAX_PATH] = {0};
		dwSize = MAX_PATH;
		MsiGetProperty(hInstall, _T("CustomActionData"), szSourceDir, &dwSize);
		_tcscpy(szSKUPath, szSourceDir);
		PathAppend(szSKUPath, _T("SKU.dat"));

		wsprintf(szCfgWizDat, _T("%s\\CfgWiz.dat"), szInstallDir);
		wsprintf(szVersionDat, _T("%s\\Version.dat"), szInstallDir);
		wsprintf(szNAVOptsDat, _T("%s\\NAVOpts.dat"), szInstallDir);   
		wsprintf(szNAVOptsDef, _T("%s\\NAVOpts.def"), szInstallDir);    

		// use the maps and propagate
		// to cfgWiz.dat
		PropagateInstoptsToDat(szCfgWizDat, lpmapInstoptsToCfgWiz, sizeof(lpmapInstoptsToCfgWiz)/sizeof(*lpmapInstoptsToCfgWiz), hInstall);
		PropagateDatToDat(szSKUPath, szCfgWizDat, lpmapSkuToCfgWiz,  sizeof(lpmapSkuToCfgWiz)/sizeof(*lpmapSkuToCfgWiz), hInstall);
		// to navopts.dat
		PropagateInstoptsToDat(szNAVOptsDat, lpmapInstoptsToNavopts,  sizeof(lpmapInstoptsToNavopts)/sizeof(*lpmapInstoptsToNavopts), hInstall);
		PropagateDatToDat(szSKUPath, szNAVOptsDat, lpmapSkuToNavopts,  sizeof(lpmapSkuToNavopts)/sizeof(*lpmapSkuToNavopts), hInstall);
		// to navopts.def
		PropagateInstoptsToDat(szNAVOptsDef, lpmapInstoptsToNavopts,  sizeof(lpmapInstoptsToNavopts)/sizeof(*lpmapInstoptsToNavopts), hInstall);
		PropagateDatToDat(szSKUPath, szNAVOptsDef, lpmapSkuToNavopts,  sizeof(lpmapSkuToNavopts)/sizeof(*lpmapSkuToNavopts), hInstall);
		// to version.dat
		PropagateInstoptsToIni(szVersionDat, itbInstoptsToVersionDat,  sizeof(itbInstoptsToVersionDat)/sizeof(*itbInstoptsToVersionDat), hInstall);
		PropagateDatToIni(szSKUPath, szVersionDat, itbSkuToVersionDat,  sizeof(itbSkuToVersionDat)/sizeof(*itbSkuToVersionDat), hInstall);

		// Propogate OEM to the registry.
		CInstoptsDat InstoptsDat(hInstall);
		DWORD dwOEM = -1;
		if(InstoptsDat.ReadDWORD(INSTOPTS_OEM, dwOEM, -1))
		{
			CRegKey key;
			TCHAR szKeyName[MAX_PATH] = {0};
			wsprintf(szKeyName, _T("Software\\Symantec\\Norton AntiVirus"));
			if(ERROR_SUCCESS == key.Create(HKEY_LOCAL_MACHINE, szKeyName))
			{
				key.SetDWORDValue(_T("OEM"), dwOEM);
			}
		}

		CInstoptsDat CfgWizDat(hInstall, szCfgWizDat, TRUE);
		CfgWizDat.TurnOffReadOnly();

		// look for the SSA.MSI file and set the flag in cfgwiz.dat if its there so it
		// knows to create the run key entry for ssa
		TCHAR szSSA[MAX_PATH] = {0};
		dwSize = MAX_PATH;
		MsiGetProperty(hInstall, _T("CustomActionData"), szSSA, &dwSize);
		wsprintf(szSSA, _T("%s..\\Support\\SSA\\SSA.MSI"), szSSA);
		if(InstallToolBox::FileExists(szSSA))
		{
			CfgWizDat.WriteDWORD(CFGWIZ_SetSSAKey, 1);
		}
		else
		{
			CfgWizDat.WriteDWORD(CFGWIZ_SetSSAKey, 0);
		}

		CfgWizDat.Save();

		hrCoInit = CoInitialize(NULL);
		if(SUCCEEDED(hrCoInit))
		{
			// now we have to tell options to propagate from the dat file to cc settings
			CNAVOptMigrate optmigrate;
			optmigrate.MigrateNAVOptFile(szNAVOptsDat, TRUE /*also set the defaults*/);
		}
		else
		{
			InstallToolBox::CCustomAction::LogMessage(hInstall,
				InstallToolBox::CCustomAction::LogSeverity::logError,
				_T("Failed to Initialize COM - 0x%08X"), hrCoInit);
		}
	}

	catch(_com_error &ce)
	{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logError,
			_T("_com_error Exception in PropagateInstopts.h. (0x%08X)"), ce.Error());
	}
	catch(exception &ex)
	{
		InstallToolBox::CCustomAction::LogMessage(hInstall,
			InstallToolBox::CCustomAction::LogSeverity::logError,
			CA2CT(ex.what()));
	}

	if(SUCCEEDED(hrCoInit))
	{
		CoUninitialize();
	}

	InstallToolBox::CCustomAction::LogMessage(hInstall,
		InstallToolBox::CCustomAction::LogSeverity::logStatus,
		_T("PropagateInstopts() Finished."));

	return ERROR_SUCCESS;
}
