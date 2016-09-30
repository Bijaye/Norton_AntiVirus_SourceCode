/////////////////////////////////////////////////////////////////////////////////////////////
// PropagateInstopts.cpp: implements propagation of options to CfgWiz.dat
//
// COLLIN DAVIS
/////////////////////////////////////////////////////////////////////////////////////////////

#include "helper.h"
#include "InstOptsNames.h"
#include "OptNames.h"
#include "NAVLicenseNames.h"
#include "NAVOptMigrate.h"
#include "APOptNames.h"

//installtoolbox headers
#include "CommonStructs.h"
#include "propagate.h"
#include "instopts.h"
#include "fileaction.h"
#include "DRMNamedProperties.h"

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

	g_Log.Log("PropagateInstopts() Starting.");

	HRESULT hrCoInit = E_FAIL;

	//
	// set up the map for values to propagated from instopts.dat to cfgwiz.dat
	//
	ITB_INSTOPTStoDATFILE lpmapInstoptsToCfgWiz[] =
	{
		/*{is it a str?, key in srource file, key in dest file, def str val, def int val, must exist, managed must exist}*/ 
		{FALSE, INSTOPTS_EnableAutoLiveUpdate, CFGWIZ_EnableAutoLiveUpdate, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableAutoProtect, CFGWIZ_EnableAutoProtect, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableEmailScanner, CFGWIZ_EnableEmailScannerIn, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableEmailScanner, CFGWIZ_EnableEmailScannerOut, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_FactoryFileExist, CFGWIZ_FactoryFileExists, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_FactoryRegKeyExist, CFGWIZ_FactoryRegKeyExists, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_RebootCount, CFGWIZ_RebootCount, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_OEM, CFGWIZ_OEM, NULL, 0, FALSE, FALSE},
		{TRUE, INSTOPTS_FactoryFile, CFGWIZ_FactoryFile, NULL, 0, FALSE, FALSE},
		{TRUE, INSTOPTS_FactoryRegKey, CFGWIZ_FactoryRegKey, "HKLM\\Software\\Symantec", 0, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableRegistration, CFGWIZ_EnableRegistration, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableAlertDefs, CFGWIZ_EnableAlertDefs, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_APRebootCount, CFGWIZ_APRebootCount, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableMSNScanning, CFGWIZ_EnableMSNScan, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_Reseal, CFGWIZ_Reseal, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableDelayLoad, CFGWIZ_EnableDelayLoad, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowEnableWeeklyScan, CFGWIZ_ShowEnableWeeklyScan, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowLaunchFullSysScan, CFGWIZ_ShowLaunchFullSysScan, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_ShowLaunchLiveUpdate, CFGWIZ_ShowLaunchLiveUpdate, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_EnableWeeklyScan, CFGWIZ_EnableWeeklyScan, NULL, 1, FALSE, FALSE},
		{FALSE, CFGWIZ_LaunchFullSystemScan, CFGWIZ_LaunchFullSystemScan, NULL, 1, FALSE, FALSE},
		{TRUE, INSTOPTS_CipherID, CFGWIZ_CipherID, _T(""), 0, FALSE, FALSE},
		{TRUE, INSTOPTS_CipherTag, CFGWIZ_CipherTag, _T(""), 0, FALSE, FALSE},
		{FALSE, INSTOPTS_NODRIVERS, CFGWIZ_NODRIVERS, NULL, 1, FALSE, FALSE},
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
		{FALSE, INSTOPTS_LaunchUI, CFGWIZ_LaunchUI, NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_HideFromAddRemove, CFGWIZ_HideFromAddRemove, NULL, 0, FALSE, FALSE},
		{FALSE, CFGWIZ_LaunchLiveUpdate, CFGWIZ_LaunchLiveUpdate, NULL, 1, FALSE, FALSE}
	};

	TCHAR szLicenseType[32] = {0};

	wsprintf(szLicenseType, "DRM:%s", DRM::szDRMLicenseType);

	//
	// set up the map for values to propagated from instopts.dat to navopts.dat/def
	//
	ITB_INSTOPTStoDATFILE lpmapInstoptsToNavopts[] =
	{
		/*{is it a str?, key in srource file, key in dest file, def str val, def int val, must exist, managed must exist}*/
		{TRUE, INSTOPTS_BuildType, CC_ERR_BUILD_TYPE, "RETAIL", 0, FALSE, FALSE},
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
		{FALSE, INSTOPTS_HideNAVTrayIconOption, NAVOPTION_HideAPIconOption, NULL, 0, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableHPP, HOMEPAGEPROTECTION_FeatureEnabled, NULL, 1, FALSE, FALSE},
		{FALSE, INSTALL_RunLiveReg, _T("RegisterProduct"), NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_EnableNSC, NSC_FeatureEnabled, NULL, 1, FALSE, FALSE},
        {TRUE, _T("INSTOPTS:WebRegURL"), _T("WebRegURL"), NULL, 1, FALSE, FALSE},
		{FALSE, INSTOPTS_SymProtect_Enabled, SYMPROTECT_FeatureEnabled, NULL, 1, FALSE, FALSE}
	};

	//
	// set up the map for values to propagated from sku.dat to navopts.dat/def
	ITB_INSTOPTStoDATFILE lpmapSkuToNavopts[] =
	{
		/*{is it a str?, key in srource file, key in dest file, def str val, def int val, must exist, managed must exist}*/
		{TRUE, "OPSKU", LICENSE_MEDIA_SKU, "0", 0, FALSE, FALSE},
		{TRUE, "SPEFSKU", LICENSE_FAMILY_SKU, "0", 0, FALSE, FALSE},
		{TRUE, "PNU", "SKU:PNU", "0", 0, FALSE, FALSE},
		{TRUE, "PLN", "SKU:PLN", "0", 0, FALSE, FALSE},
		{TRUE, "PARTNERNAME", "SKU:PARTNERNAME", "0", 0, FALSE, FALSE},
		{TRUE, "PARTNERTYPE", "SKU:PARTNERTYPE", "0", 0, FALSE, FALSE},
		{TRUE, "EBESERVERURL", "SKU:EBESERVERURL", "0", 0, FALSE, FALSE},
		{TRUE, "EBESERVERURLALT", "SKU:EBESERVERURLALT", "0", 0, FALSE, FALSE}
	};

	//
	// set up the map for values to propagated from instopts.dat to version.dat
	//
	tagINSTOPTStoINIFILE itbInstoptsToVersionDat[] = 
	{	
		{FALSE, "Versions", INSTOPTS_OEM, "NAVOEM", "", 0},
	};

	//
	// set up the map for values to propagated from sku.dat to version.dat
	//
	tagINSTOPTStoINIFILE itbSkuToVersionDat[] =
	{
		/* is it a str?, ini section, src key name, dest key name, default str val, default int val */
		{TRUE, "Versions", INSTOPTS_VendorName, "OEMVendor", "", 0},
		{TRUE, "Versions", "PLN", "Language", "", 0},
		{TRUE, "Versions", INSTOPTS_SKUDESC, "SKUDESC", "", 0}
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

		wsprintf(szCfgWizDat, "%s\\CfgWiz.dat", szInstallDir);
		wsprintf(szVersionDat, "%s\\Version.dat", szInstallDir);
		wsprintf(szNAVOptsDat, "%s\\NAVOpts.dat", szInstallDir);   
		wsprintf(szNAVOptsDef, "%s\\NAVOpts.def", szInstallDir);    

		// use the maps and propagate
		// to cfgWiz.dat
		PropagateInstoptsToDat(szCfgWizDat, lpmapInstoptsToCfgWiz, sizeof(lpmapInstoptsToCfgWiz)/sizeof(*lpmapInstoptsToCfgWiz), hInstall);
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
				key.SetDWORDValue("OEM", dwOEM);
			}
		}

        DWORD dwNTC = 0; // INSTALL:NoThreatCat
        if(InstoptsDat.ReadDWORD(INSTALL_NoThreatCat, dwNTC, 0))
        {
            CInstoptsDat navoptsDat(hInstall, szNAVOptsDat, TRUE);
            navoptsDat.TurnOffReadOnly();

            dwNTC = (dwNTC == 0 ? 1 : 0); // NoThreatCat = 0 means turn on (1)

            if(!navoptsDat.WriteDWORD(IMSCAN_ThreatCatEnabled, dwNTC))
            {
                CCTRACEE(_T("Could not write IMSCAN:ThreatCatEnabled to navopts.dat"));
            }
            if(!navoptsDat.WriteDWORD(SCANNER_ThreatCatEnabled, dwNTC))
            {
                CCTRACEE(_T("Could not write SCANNER:ThreatCatEnabled to navopts.dat"));
            }
            if(!navoptsDat.WriteDWORD(NAVEMAIL_ThreatCatEnabled, dwNTC))
            {
                CCTRACEE(_T("Could not write NAVEMAIL:ThreatCatEnabled to navopts.dat"));
            }
            if(!navoptsDat.Save())
            {
                CCTRACEE(_T("Could not save navopts.dat (ThreatCatEnabled options)"));
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
			g_Log.LogEx(_T("Failed to Initialize COM - 0x%08X"), hrCoInit);
		}
	}

	catch(exception &ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown Exception in PropagateInstopts.h");
	}

	if(SUCCEEDED(hrCoInit))
	{
		CoUninitialize();
	}

	g_Log.Log("PropagateInstopts() Finished.");

	return ERROR_SUCCESS;
}
