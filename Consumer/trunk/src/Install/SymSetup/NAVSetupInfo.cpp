////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NAVSetupInfo.h"
#include "NAVInfo.h"
#include "shlobj.h"
#include "NAVSetupResource.h"
#include "resource.h"
#include "optnames.h"
#include "isResource.h"
#include "LiveUpdateWrap.h"
#include "NAVDetection.h"
#include "isNames.h"
#include "Instopts.h"
#include "FileAction.h"
#include "IWPSettingsInterface.h"
#include "IWPSettingsLoader.h"
#include "InstoptsNames.h"
#include "WaitForEventDialog.h"
#include "NAVInstoptsNames.h"
#include "ccPwdDynamic.h"
#include "ccOSInfo.h"
#include "Shobjidl.h"
#include "intshcut.h"
#include "IICFManager.h"
#include "WndDefenderHelperImpl.h"

#define __CFW_ICFMGR__
#include "FWLoaders.h"

//IDS
#include "InstallIDSDefs.h"
#include "ImportDefRules.h"

// headers to register with WMI
#include "WSCHelperImpl.h"

#include "V2LicensingAuthGuids.h"

// header to load up QBackup dll syminterface obj
#include <QBackupLoader.h>
#include <SRX.h>

// cclib headers
#include "ccLib.h"
#include "ccLibStaticLink.h"
#include "ccServicesConfigInterface.h"
#include "ccInstLoader.h"
#include "ccSettingsManagerHelper.h"
#include "ccSettingsInterface.h"
#include "ccHandle.h"

// cctrace debug headers
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVSetup"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

#include "SymSetupLog.h"
IMPLEMENT_SYMSETUPTRACE(g_DebugOutput);

// Module lifetime management for trust cache
#include "ccSymModuleLifetimeMgrHelper.h"
cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);

//installtoolbox headers
#include "instopts.h"
#include "FileAction.h"
#include "instoptsnames.h"
#include "OSVerInfo.h"
#include "NAVDetection.h"
#include "pathvalidation.h"
#include "OSInfo.h"


#include "ISComponentFrameworkGuids.h"
#include "CLTProductHelper.h"
#include "CfgWizNames.h"
// For turning on/off SymProtect
#include "SymProtectControlHelper.h"

#include "FWSettings.h"

// construction
CSetupInfo::CSetupInfo(void)
{
	m_hUIRES = NULL;
	m_szGUIDCACHEKEY = _T("Software\\Symantec\\SymSetup\\Norton AntiVirus");
    m_strSuiteOwnerPath = _T(".\\NAV\\Parent\\NORTON\\CLTVault.dll");
	m_szParentMSIPath = _T(".\\NAV\\NAV.msi");
	m_szPatchInstID = _T("NAV");
	m_szSKUPath = _T(".\\NAV\\SKU.dat");
	m_szSKUFilePath = _T("NAV\\SKU.dat");
	m_szSymHelpPath = _T(".\\Support\\Help\\External\\Symhelp.chm");
    m_szProdKeyFilePath = _T(".\\NAV\\SymLT\\NORTON\\OPC\\Prodkey.htm");

	m_NAVLaunchConditions.SetSetupInfo(this);
	m_NAVLaunchConditions.SetNAVInfo(this);
	
	CSetupInfoBase::CSetupInfoBase();
	AllowWin64(true);
}

// destruction
CSetupInfo::~CSetupInfo(void)
{
}

//////////////////////////////////////////////////////////////////////////
// This Function initializes all of the data for the information class..
// This should enumerate through the .msi files and get the feature listing
//
//////////////////////////////////////////////////////////////////////////
HRESULT CSetupInfo::InitInfo()
{
	CSetupInfoBase::InitInfo();
	
	AddFileDependency(_T("\\NAV\\Parent\\NORTON\\App\\isRes.dll"));

	if(GetAction() == ACTIONS_INSTALL)
	{
		//remove symprotect from the list if we're not on NT.
		InstallToolBox::COSVerInfo OSInfo;
		if(!OSInfo.IsNT())
		{
			m_InstallManager.m_FeatureManager.Remove("{77772678-817F-4401-9301-ED1D01A8DA56}");
			m_InstallManager.m_FeatureManager.Remove("{F64306A5-4C32-41bb-B153-53986527FAB4}");
		}
	}

	ATL::CString csProductNameWithVersion = _T("Norton AntiVirus");
	ATL::CString csProductNameNoVersion = _T("Norton AntiVirus");
	if(m_hUIRES)
	{
		if (FALSE == csProductNameWithVersion.LoadString(m_hUIRES,IDS_BRANDING_PRODUCT_NAME))
		{
			csProductNameWithVersion = _T("Norton AntiVirus");
		}

		if (FALSE == csProductNameNoVersion.LoadString(m_hUIRES,IDS_BRANDING_PRODUCT_NAME))
		{
			csProductNameNoVersion = _T("Norton AntiVirus");
		}
	}
	SetProperty(_T("CoBrandedProductName"), csProductNameWithVersion);
	SetProperty(_T("CoBrandedProductNameNoVer"), csProductNameNoVersion);

    
    if(GetAction() == ACTIONS_INSTALL)
	{
        // see if this is an IWP layout, we need to tell taxman this
        // also the unmark files for deletion logic
   	    ATL::CString sIWP = GetModulePath();
	    sIWP.Append(_T("\\NAV\\IWP.MSI"));
        InstallToolBox::CInstoptsDat Instopts;
        BOOL bIWP = InstallToolBox::FileExists(sIWP);
        SetProperty(_T("WMIREGISTER"), bIWP ? _T("3") : _T("2"));
        Instopts.TurnOffReadOnly();
        Instopts.WriteDWORD(_T("INSTALL:IWP"), bIWP ? 1 : 0);
        Instopts.Save();
        
        DWORD dwNoDrivers = 0;
        Instopts.ReadDWORD(InstallToolBox::INSTOPTS_NODRIVERS, dwNoDrivers, 0);
        
        DWORD dwSymProtect = 1;
        Instopts.ReadDWORD(InstallToolBox::INSTOPTS_SymProtect_Enabled, dwSymProtect, 1);
        if((1 == dwSymProtect || 3 == dwSymProtect) && dwNoDrivers == 0)
            SetProperty(_T("ENABLE_SYMPROTECT"), _T("1"));
        else
            SetProperty(_T("ENABLE_SYMPROTECT"), _T("0"));

        // only show this text if a reboot is not needed, 
        // must be winnt, noiwp layout thats not upgrading.
        COSInfo OSInfo;
	    if( (OSInfo.GetOSType() == COSInfo::OS_TYPE_WINNT || OSInfo.GetOSType() == COSInfo::OS_TYPE_WINXP)
            && !InstallToolBox::IsNAVInstalled() && !bIWP)
	    {
            CString sMSNWarning;
		    sMSNWarning.LoadString(IDS_MSN_WARNING);	
		    pageFinish.SetAdditionalText(sMSNWarning);
	    }
    }
    else
    {
        // HACK: if we installed IWP we need to pass taxman that information
        // because they dont keep track
        IWP::IIWPSettingsPtr IWPSettings;
        BOOL bIWP = SYM_SUCCEEDED(IWP_IWPSettings::CreateObject(GETMODULEMGR(),IWPSettings.m_p));
        SetProperty(_T("WMIREGISTER"), bIWP ? _T("3") : _T("2"));

		// We need to populate ENABLE_SYMPROTECT in case uninstall fails and we have to roll back.
		// If we aren't installing then instopts.dat isn't available so get it from navopts.dat
		HKEY hKey = NULL;
		DWORD dwSymProtect = 1;
		TCHAR szInstallDir[MAX_PATH] = {0};
		DWORD dwSize = MAX_PATH;
		LONG lRet;

		if(lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			if(RegQueryValueEx(hKey, _T("NAV"), NULL, NULL, (BYTE*)szInstallDir, &dwSize) == ERROR_SUCCESS)
			{                   
				TCHAR szNAVOptsDat[MAX_PATH] = {0};
				wsprintf(szNAVOptsDat, _T("%s\\NAVOpts.dat"), szInstallDir);
				InstallToolBox::CInstoptsDat NAVOptsDat(NULL, szNAVOptsDat, TRUE);

				if(NAVOptsDat.ReadDWORD(SYMPROTECT_FeatureEnabled, dwSymProtect, 1))
				{
					CCTRACEI(_T("Successfully read SP Enabled status from NAVOpts.dat, value = %i"), dwSymProtect);
				}
				else
				{
					CCTRACEW(_T("SP Enabled status not found in NAVOpts.dat, value = %i"), dwSymProtect);
				}

			}
			else
			{
				CCTRACEW(_T("Could not read NAV string from InstalledApps key."));
				CCTRACEW(_T("SP Enabled status defaulting to %i"), dwSymProtect);
			}
			RegCloseKey(hKey);
		}
		else
		{
			TCHAR messageText[MAX_PATH];
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lRet, 0, messageText, MAX_PATH, NULL);
			CCTRACEW(_T("Could not open InstalledApps key: %s.  SP Enabled status defaulting to %i"), messageText, dwSymProtect);
		}
        if(1 == dwSymProtect || 3 == dwSymProtect)
            SetProperty(_T("ENABLE_SYMPROTECT"), _T("1"));
        else
            SetProperty(_T("ENABLE_SYMPROTECT"), _T("0"));

    }

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// This function verifies that the user's machine meets the minimum system
// system requirements
//
//////////////////////////////////////////////////////////////////////////
HRESULT CSetupInfo::CheckLaunchConditions()
{
	if(ACTIONS_INSTALL == m_Action)
	{
		if(!m_NAVLaunchConditions.CheckInstallConditions())
			return E_FAIL;
	}
	else
	{
		if(!m_NAVLaunchConditions.CheckUninstallConditions())
			return E_FAIL;
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// This function launches the info gathering installer wizard 
//
//////////////////////////////////////////////////////////////////////////
HRESULT CSetupInfo::LaunchWizard()
{	
	return CSetupInfoBase::LaunchWizard();
}

//////////////////////////////////////////////////////////////////////////
// This function launches the finish wizard 
//
//////////////////////////////////////////////////////////////////////////
HRESULT CSetupInfo::LaunchFinishWizard()
{
	CSetupInfoBase::LaunchFinishWizard();

	InstallToolBox::CInstoptsDat Instopts;
	DWORD dwOEM = 0;
	Instopts.ReadDWORD(InstallToolBox::INSTOPTS_OEM, dwOEM, 0);

	//If its an OEM layout and a silent install we should not run CfgWiz
	if( (0 != dwOEM) && (UIMODE_NORMAL != m_UIMode))
	{
		CCTRACEW(_T("Not an Error : OEM Layout with UI mode silent. Not launching Confiuration Wiz"));
		return S_OK;
	}
	
	// if Finish wizard doesn't reboot then we should run CfgWiz
	CRebootHandler rbHandler;
	if(GetAction() == ACTIONS_INSTALL && !pageFinish.IsRebootNeeded() && !rbHandler.IsRebootNeeded() && !GetUserChoseReboot())
	{

		//check for the uber no-drivers switch
		InstallToolBox::CInstoptsDat InstoptsDat;
		DWORD dwValue = 0;
		InstoptsDat.ReadDWORD(InstallToolBox::INSTOPTS_NODRIVERS, dwValue, 0);
		if(dwValue == 1)
		{
			SetCCServicesStates(false, false);
		}

		CNAVInfo NAVInfo;
		CString sCfgWizDir = NAVInfo.GetOnlinePlatformClientDir();
		TCHAR szCfgWiz[MAX_PATH] = {0};
		_stprintf(szCfgWiz, _T("%s\\CfgWiz.exe"), sCfgWizDir);

        TCHAR szCfgWizArgs[MAX_PATH] = {0};
        _stprintf(szCfgWizArgs, _T("/GUID %s /MODE CfgWiz /CMDLINE \"REBOOT\""), SZ_IS_BUSINESS_TIER_ENDPOINT);
		HRESULT hr = InstallToolBox::ExecuteProgramEx(szCfgWiz, szCfgWizArgs, sCfgWizDir, FALSE);
		if(SUCCEEDED(hr))
		{
			CCTRACEW(_T("Not an Error : Starting the Wait dialog"));
			
			ccLib::CHandle hCfgWizUIReadyEvent;
			hCfgWizUIReadyEvent.Attach(::CreateEvent(NULL,TRUE,FALSE,CfgWiz::SZ_CFGWIZ_UI_START_EVENT));

			CString sTitle,sDescription;
			sTitle.LoadString(IDS_COMPANY_NAME);
			sDescription.LoadString(IDS_FINALIZING_INSTALLATION);

			CWaitForEventDialog CfgWizLaunchDialog(sTitle,sDescription,hCfgWizUIReadyEvent,60000);
			hr = CfgWizLaunchDialog.Display(UIMODE_NORMAL == m_UIMode);
			CCTRACEW(_T("CfgWizLaunchDialog.Display returned 0x%x"),hr);
		}
	}

	return(S_OK);
}

//////////////////////////////////////////////////////////////////////////
// Setup the pages in a uninstall
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::SetupUninstallWizard()
{
	//
	// yeah! the uninstall is exactly the same as modify in NAV!
	//
	return SetupModifyWizard();
}

//////////////////////////////////////////////////////////////////////////
// This function adds all the pages that should be added in a modify install
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::SetupModifyWizard()
{
	m_Wizard.SetSetupInfo(this);
	m_FinishWizard.SetSetupInfo(this);
	
	// find AVRES from the NAV dir
	CNAVInfo NAVInfo;
	ATL::CString sUIRES;
	sUIRES.Format(_T("%s\\ISRES.dll"), NAVInfo.GetNAVDir());
	m_hUIRES = LoadLibraryEx(sUIRES, NULL, LOAD_LIBRARY_AS_DATAFILE);

	if(m_hUIRES)
    {
	    // set up name for CED, if not found then setupinfobase will use ProductName instead
        ATL::CString csProductName;
        if (TRUE == csProductName.LoadString(m_hUIRES,IDS_BRANDING_PRODUCT_NAME))
        {
            SetProperty(_T("CoBrandedProductName"), csProductName);
			SetProperty(_T("SetupLogName"), csProductName + _T(" 2007 Uninstall"));
        }
    }

	bool bShowSubWarning = ShowUninstallSubWarningPage();

	// setup pages
	pageModify.SetSetupInfo(this);

	// Display "Reset password" button for XP,
	// and hide it for Vista because Vista is already protected by UAC.
	if(!ccLib::COSInfo::IsWinVista(true))
	{
		pageModify.SetupBottomButton(IDS_RESET_PASSWORD_BUTTON, IDS_RESET_PASSWORD_DESC, &CSetupInfo::ResetPassword);
	}

	if(bShowSubWarning)
		pageSubscriptionWarning.SetSetupInfo(this);

	pageICFRestore.SetSetupInfo(this);
	pageVerifyRemove.SetSetupInfo(this);
	pageVerifyRemove.SetBackgroundImageMask(SetupData::BIGIMAGE);
	pageFinish.SetSetupInfo(this);
	pageFinish.SetNoBackButton();
	pageFinish.SetBackgroundImageMask(SetupData::BIGIMAGE);

	// setup wizard
	m_Wizard.AddPage(pageModify);
	if(bShowSubWarning)
		m_Wizard.AddPage(pageSubscriptionWarning);
	m_Wizard.AddPage(pageICFRestore);
	m_Wizard.AddPage(pageVerifyRemove);
	m_FinishWizard.AddPage(pageFinish);
	
	AddBillboard(L"NAVSETUPBILLBOARD.HTML");

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// This function adds all the pages that should be in a typical install
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::SetupInstallWizard()
{
	// Get product name from AVRES.DLL
	ATL::CString sUIRES = GetModulePath();
	sUIRES.Append(_T("\\NAV\\Parent\\NORTON\\App\\ISRES.dll"));
	m_hUIRES = LoadLibraryEx(sUIRES, NULL, LOAD_LIBRARY_AS_DATAFILE);

	ATL::CString szProductName = _T("Norton AntiVirus");
	if(m_hUIRES)
    {
		// set up name for CED, if not found then setupinfobase will use ProductName instead
		ATL::CString csProductName;
		if (TRUE == csProductName.LoadString(m_hUIRES,IDS_BRANDING_PRODUCT_NAME))
		{
			SetProperty(_T("CoBrandedProductName"), csProductName);
			SetProperty(_T("SetupLogName"), csProductName + _T(" 2007"));
		}
	}

	//Set up the default install path
	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, (HANDLE)-1, 0, m_szFolder);
	_stprintf(m_szFolder, _T("%s\\%s"),m_szFolder, szProductName);
	InstallToolBox::RemoveBackslash(m_szFolder);

	m_Wizard.SetSetupInfo(this);
	m_FinishWizard.SetSetupInfo(this);
	
	// setup the pages with the information they
	// need.
	InstallToolBox::CInstoptsDat Instopts;

	pageActivate.SetSetupInfo(this);
	pageActivate.SetLicenseID(IDR_BRANDING_EULA);
	
    if(m_hUIRES)
    {
        pageActivate.SetLicenseResourceHandle(m_hUIRES);
    }

	pageActivate.SetNoBackButton();

    pageBrowseOptions.SetSetupInfo(this);
	pageBrowseOptions.SetFolderProperty(_T("INSTALLDIR"));

	DWORD dwSymPathExclusions = InstallToolBox::SYMPATH_NAV;
	if (!IWPIsYielding())
	{
		dwSymPathExclusions |= InstallToolBox::SYMPATH_NIS | InstallToolBox::SYMPATH_IS;
	}

	pageBrowseOptions.SetSymPathsFlags(dwSymPathExclusions);
	SetProperty(_T("INSTALLDIR"), m_szFolder);
	pageBrowseOptions.SetDefaultFolder(m_szFolder);
	pageBrowseOptions.SetTempRegKey(_T("SSINSTALLDIR"));

	BOOL bFound = FALSE;
	INSTALL_ITER it = m_InstallManager.FindInstallByProductCode(_T("YAHOOTOOLBAR"), bFound);
	DWORD dwShowYTBOption = 0;
	Instopts.ReadDWORD(InstallToolBox::YAHOO_TB_OPTION, dwShowYTBOption, 0);
	if (bFound && SUCCEEDED(m_InstallManager.FixInstallPath(it->sInstallPath, 0)) && dwShowYTBOption)
	{
		CString yahooHtml;
		yahooHtml.LoadString(IDS_YAHOO_HTML);

		DWORD dwYTBOptionChecked = 1;
		Instopts.ReadDWORD(InstallToolBox::YAHOO_TB_OPTION_CHECKED, dwYTBOptionChecked, 1);
		COption yahooToolBarOption(_T("YahooOption"),yahooHtml,dwYTBOptionChecked);

		AddOption(yahooToolBarOption);
	}

	if(m_UIMode != UIMODE_NORMAL)
		pageBrowseOptions.Reset();
	/*if(InstallToolBox::IsNAVInstalled())
		pageBrowseOptions.SetupCheck1(IDS_KEEPSETTINGS, TRUE);*/
	
	pageFinish.SetSetupInfo(this);
	pageFinish.SetNoCancelButton();
	pageStartInstall.SetSetupInfo(this);
	
	// add the activation dialog if it should be shown
	// OEM = 0 => Retail
	// OEM = 1 => OEM Limited
	// OEM = 2 => CTO/Prepaid
	DWORD dwOEM = 0;
	Instopts.ReadDWORD(InstallToolBox::INSTOPTS_OEM, dwOEM, 0);
	
	// if retail
	if( 0 == dwOEM)
	{
		m_Wizard.AddPage(pageActivate);
	}
	
	// add the custom/express and 
	m_Wizard.AddPage(pageBrowseOptions);
	m_Wizard.AddPage(pageStartInstall);
	
	// add the rest of the standard dialogs, these dialogs will
	// appear in every single install path no matter what
	m_FinishWizard.AddPage(pageFinish);

	AddBillboard(L"NAVSETUPBILLBOARD.HTML");

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// This function is called before the install begins
//	- Used to Check if we're migrating options and set the regkey to inform our MSI
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::PreInstall()
{
	bool isYTBOptionChecked = false;
	GetOptionChecked(_T("YahooOption"),isYTBOptionChecked);
	
	if (isYTBOptionChecked == false)
	{
		m_InstallManager.m_FeatureManager.Remove(_T("YAHOOTOOLBAR"));
		(m_InstallManager.m_FeatureManager).ResolveFeatureActions(ACTIONS_INSTALL);
	}

	// Shut down the SymProtect service in case we need to uninstall it in an overinstall scenario.
	CSymProtectControlHelper SymProtectCtrlHelper;

	if(SymProtectCtrlHelper.Create())
	{
		if (!SymProtectCtrlHelper.StopLegacySymProtect())
		{
			CCTRACEE (_T("CSetupInfo::PreInstall() - StopLegacySymProtect() failed."));
		}
		else
		{
			CCTRACEI (_T("CSetupInfo::PreInstall() - StopLegacySymProtect() Successful."));
		}

		if (!SymProtectCtrlHelper.SetBBSystemEnabled(false))
		{
			CCTRACEE (_T("CSetupInfo::PreInstall() - SetBBSystemEnabled(false) failed."));
		}
		else
		{
			CCTRACEI (_T("CSetupInfo::PreInstall() - SetBBSystemEnabled(false) Successful."));
		}
	}
	else
	{
		CCTRACEE(_T("CSetupInfo::PreInstall() - SymProtectCtrlHelper.Create() Failed"));
	}


	return TRUE;
}

void CSetupInfo::CreateShortcuts()
{
	HRESULT hr;
	CComPtr<IShellLink> psl;
	CComPtr<IPersistFile> ppfl;

	// Get a pointer to the IShellLink interface. 
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if(SUCCEEDED(hr))
	{
		//Link IShell to IPersistFile interface
		hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppfl);
	}
	
	if (SUCCEEDED(hr))
	{
		//Constants
		const CString sUIStubArgs = _T("{68175F05-68E7-47e7-A1F2-4CE1DEE316EE}");
		const CString sUninstallGUID = _T("{830D8CBD-C668-49E2-A969-C2C2106332E0}");		
		const CString sUninstallArgs = _T("/X");

		//Get Shortcut Options
		InstallToolBox::CInstoptsDat instopts(NULL);
		DWORD dwNoDesktopIcon(0), dwUninstallShortcut(0);

		instopts.ReadDWORD(InstallToolBox::INSTALL_NoIcon, dwNoDesktopIcon);
		instopts.ReadDWORD(InstallToolBox::INSTALL_UninstallShortcut, dwUninstallShortcut);

		//Get Target Directories
		CString sInstallDir, sSymSharedDir, sUIDir, sProgramFilesFolder;
		GetShorcutTargetPaths(sInstallDir, sSymSharedDir, sUIDir, sProgramFilesFolder);

		//Get Shortcut Directories
		CString sNAVProgramMenuFolder, sDesktopDir, sCoBrandedShortcutName;
		GetShortcutPaths(sNAVProgramMenuFolder, sDesktopDir, sCoBrandedShortcutName);

		//Get Strings
		CString sLaunch, sUninstall, sSubmitBugURL, sReadMe, sReadMeDesc, sLU, sLUDesc;
		sLaunch.LoadString(IDS_LAUNCH);
		sUninstall.LoadString(IDS_UNINSTALL);
		sReadMe.LoadString(IDS_READ_ME);
		sReadMeDesc.LoadString(IDS_READ_ME_DESC);
		sLU.LoadString(IDS_LIVEUPDATE);
		sLUDesc.LoadString(IDS_LIVEUPDATE_DESC);

		PropertyReplace(sLaunch);
		PropertyReplace(sUninstall);
		PropertyReplace(sReadMeDesc);

		// First, create the NAV Program Start Menu Folder
		CreateDirectory(sNAVProgramMenuFolder, NULL);

		//Create Program Menu Shortcuts
		if(dwUninstallShortcut)
		{
			CreateShortcut(sNAVProgramMenuFolder + sUninstall + sCoBrandedShortcutName + _T(".lnk"), 
				sSymSharedDir + _T("SymSetup\\") + sUninstallGUID + _T("\\") + sUninstallGUID + _T(".exe"), sUninstallArgs, sUninstall, 
				false, 1, sSymSharedDir, psl, ppfl); 
		}

		CreateShortcut(sNAVProgramMenuFolder + sLU + _T(".lnk"), sProgramFilesFolder + _T("Symantec\\LiveUpdate\\LUALL.EXE"), 
			_T(""), sLUDesc, false, 1, sInstallDir, psl, ppfl); 
						
		CreateShortcut(sNAVProgramMenuFolder + sReadMe + _T(".lnk"), sInstallDir + _T("ReadMe.txt"), 
			_T(""), sReadMeDesc, false, 1, sInstallDir, psl, ppfl); 

		CreateShortcut(sNAVProgramMenuFolder + sCoBrandedShortcutName + _T(".lnk"), sUIDir + _T("uiStub.EXE"), 
			sUIStubArgs, sLaunch, true, 0, sInstallDir, psl, ppfl); 

		//Create Desktop Shortcuts
		if(!dwNoDesktopIcon){
			CreateShortcut(sDesktopDir + sCoBrandedShortcutName + _T(".lnk"), sUIDir + _T("uiStub.EXE"), 
				sUIStubArgs, sLaunch, true, 0, sInstallDir, psl, ppfl);
		}

		ppfl.Release();
	}
}

void CSetupInfo::CreateShortcut(LPCTSTR szLocation, LPCTSTR szTarget, LPCTSTR szArguments, LPCTSTR szDescription, 
								bool bUseNAVIcon, int nShowCmd, LPCTSTR szWorkingDir,
								IShellLink *pShellLink, IPersistFile *pPersistFile)
{
	// Set all the shortcut parameters.
	if( !SUCCEEDED(pShellLink->SetPath(szTarget)) ||
		!SUCCEEDED(pShellLink->SetArguments(szArguments)) ||
		!SUCCEEDED(pShellLink->SetDescription(szDescription)) ||
		!SUCCEEDED(pShellLink->SetShowCmd(nShowCmd)) ||
		!SUCCEEDED(pShellLink->SetWorkingDirectory(szWorkingDir)))
	{
		return;
	}
	if (bUseNAVIcon) 
	{	
		CString szIconPath;
		ccSym::CInstalledApps::GetNAVDirectory(szIconPath);
		PathAddBackslash(szIconPath.GetBuffer(szIconPath.GetLength() + 2));
		szIconPath.ReleaseBuffer();
		szIconPath.Append(_T("NAVW32.exe"));
		if(SUCCEEDED(pShellLink->SetIconLocation(szIconPath, 0)))
		{
			pPersistFile->Save(szLocation, TRUE);
		}

	}
	else // Use the icon associated with the target executable
	{
		if(SUCCEEDED(pShellLink->SetIconLocation(szTarget, 0)))
		{
			pPersistFile->Save(szLocation, TRUE);
		}
	}
}

void CSetupInfo::DeleteShortcuts()
{
	CString sNAVProgramMenuFolder, sDesktopDir, sCoBrandedShortcutName;
	GetShortcutPaths(sNAVProgramMenuFolder, sDesktopDir, sCoBrandedShortcutName);

	CString sUninstall, sSubmitBugURL, sReadMe, sLU, sLUDesc;
	sUninstall.LoadString(IDS_UNINSTALL);
	sReadMe.LoadString(IDS_READ_ME);
	sLU.LoadString(IDS_LIVEUPDATE);
	
	//Delete Desktop Shortcuts
	this->DeleteFile(sDesktopDir + sCoBrandedShortcutName + _T(".lnk"));

	//Delete Program Menu Shortcuts
	this->DeleteFile(sNAVProgramMenuFolder + sUninstall + sCoBrandedShortcutName + _T(".lnk"));
	this->DeleteFile(sNAVProgramMenuFolder + sLU + _T(".lnk"));
	this->DeleteFile(sNAVProgramMenuFolder + sReadMe + _T(".lnk"));
	this->DeleteFile(sNAVProgramMenuFolder + sCoBrandedShortcutName + _T(".lnk")); 

	//Delet Program Menu Folder
	RemoveDirectory(sNAVProgramMenuFolder);
}

void CSetupInfo::DeleteFile(CString &sFileName)
{
	//Clear the read only flag before deleting
	DWORD dwAttribs = GetFileAttributes(sFileName);
	dwAttribs = dwAttribs & ~FILE_ATTRIBUTE_READONLY;
	SetFileAttributes(sFileName, dwAttribs);
	::DeleteFile(sFileName);
}

void CSetupInfo::GetShortcutPaths(CString &sNAVProgramMenuFolder, CString &sDesktopDir, CString &sCoBrandedShortcutName)
{
	sNAVProgramMenuFolder = GetProperty(_T("NAV_SHORTCUT_MENU"));
	PathAddBackslash(sNAVProgramMenuFolder.GetBuffer(sNAVProgramMenuFolder.GetLength() + 2));
	sNAVProgramMenuFolder.ReleaseBuffer();
	PropertyReplace(sNAVProgramMenuFolder);

	LPTSTR szDesktopDirBuffer = sDesktopDir.GetBuffer(MAX_PATH + 1);
	SHGetFolderPath(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, NULL, 0, szDesktopDirBuffer);
	PathAddBackslash(szDesktopDirBuffer);
	sDesktopDir.ReleaseBuffer();

	sCoBrandedShortcutName = GetProperty(_T("CoBrandedProductNameNoVer"));
}

void CSetupInfo::GetShorcutTargetPaths(CString &sInstallDir, CString &sSymSharedDir, CString &sUIDir, CString &sProgramFilesFolder){
	sInstallDir = GetProperty(_T("INSTALLDIR"));
	PathAddBackslash(sInstallDir.GetBuffer(sInstallDir.GetLength() + 2));
	sInstallDir.ReleaseBuffer();

	ccSym::CInstalledApps::GetSymSharedDirectory(sSymSharedDir);
	PathAddBackslash(sSymSharedDir.GetBuffer(sSymSharedDir.GetLength() + 2));
	sSymSharedDir.ReleaseBuffer();

	ccSym::CInstalledApps::GetInstAppsDirectory(_T("UI"), sUIDir);
	PathAddBackslash(sUIDir.GetBuffer(sUIDir.GetLength() + 2));
	sUIDir.ReleaseBuffer();

	sProgramFilesFolder = GetProperty(_T("ProgramFilesFolder"));
	PathAddBackslash(sProgramFilesFolder.GetBuffer(sProgramFilesFolder.GetLength() + 2));
	sProgramFilesFolder.ReleaseBuffer();
	PropertyReplace(sProgramFilesFolder);
}

//////////////////////////////////////////////////////////////////////////
// This function is called after install is complete.  It's for cleanup.
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::PostInstall(BOOL bRollback)
{
	// if rolling back don't need to continue initializing the product
	if(bRollback)
	{
		CCTRACEW(_T(__FUNCTION__) _T("Rollback mode. Return immediately"));
		return TRUE;
	}

	CRegKey NAVKey;
	NAVKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"));
	NAVKey.DeleteValue(_T("DontMigrate"));

	InstallToolBox::CInstoptsDat InstoptsDat;
		
	RegisterWithWMI();
	
	CreateShortcuts(); 
	
	InstallToolBox::CLiveUpdate::EnableExpresMode(false);

	try
	{
		StahlSoft::HRX hrx;
		CCLTProductHelper cltHelper;
		hrx << cltHelper.Initialize();
		//Register the Framework Plug ins

		//Setup the Component Framework Registration
		CString sFileList = _T("[ComponentFramework]\\Manifests\\CFReg.dll");
		sFileList.Append(_T(",[ComponentFramework]\\Manifests\\cltCFReg.dll"));
		sFileList.Append(_T(",[ComponentFramework]\\Manifests\\ISCFReg.dll"));

		sFileList.Append(_T(",[ComponentFramework]\\Manifests\\ISFWReg.dll"));
		hrx << cltHelper.RegisterFrameworkPlugins(sFileList);

		//Do the first Audit call. This will bootstrap the Framework.
		hrx << cltHelper.AuditSystem();

	}
	catch(_com_error& e)
	{
		CCTRACEE(CCTRCTX _T("Error when Registering Component Framework plugins HR = 0x%x"),e.Error());
	}

	//Write Build Type to ccSettings: InternetSecurity\\ResourceStore
	ccSettings::CSettingsManagerHelper settingsManager;
	ccSettings::ISettingsManagerPtr pManager0;
	ccSettings::ISettingsManager4QIPtr pManager;
	SYMRESULT symres = settingsManager.Create(*&pManager0);
	pManager = pManager0;
	InstallToolBox::CInstoptsDat instopts;

	if(SYM_SUCCEEDED(symres) && pManager) 
	{
		ccSettings::ISettingsPtr pSettings;
		symres = pManager->CreateSettings(IS_GLOBAL_RESOURCE_STORE_KEY, &pSettings);
		if(SYM_SUCCEEDED(symres))
		{
			TCHAR szBuildType[MAX_PATH] = {0};
			DWORD dwSize = sizeof(szBuildType) / sizeof(TCHAR);

			if(instopts.ReadString(InstallToolBox::INSTOPTS_BuildType, szBuildType, dwSize, _T("RETAIL")))
			{
				symres = pSettings->PutString(CC_ERR_BUILD_TYPE, szBuildType);

				CCTRACEW(_T("moved %s to %s = %s (0x%x)"), InstallToolBox::INSTOPTS_BuildType, CC_ERR_BUILD_TYPE, szBuildType, symres);
			}
			else
			{
				CCTRACEE(_T("failed to get setting from instopts (%s)"), InstallToolBox::INSTOPTS_BuildType);
			}

			symres = pManager->PutSettings(pSettings, true);
			if(SYM_FAILED(symres))
			{
				CCTRACEE(_T("failed to putsettings (0x%x)"), symres);
			}
            pSettings = NULL;
		}
		else
		{
			CCTRACEE(_T("failed to create %s (0x%x)"), IS_GLOBAL_RESOURCE_STORE_KEY, symres);
		}

        // ====================================================================
        // For OEM layout, delay fwAgent initialization until after cfgwiz runs
        // ====================================================================
	    DWORD dwOEM = 0;
    	instopts.ReadDWORD(InstallToolBox::INSTOPTS_OEM, dwOEM, 0);
        if(0 != dwOEM)
        {
            symres = pManager->CreateSettings(FW_SETTINGS_KEY, &pSettings);
            if(SYM_SUCCEEDED(symres))
            {
                symres = pSettings->PutDword(FW_OEM_RESEAL_SETTINGS, 1);
                if(SYM_SUCCEEDED(symres))
                {
                    symres = pManager->PutSettings(pSettings, true);
				    if(SYM_FAILED(symres))
				    {
					    CCTRACEE(_T("failed to putsettings (0x%x)"), symres);
				    }
                }
                else
                    CCTRACEE(_T("OEMResealDelayLoad <> 1 (0x%x)"), symres);
                pSettings = NULL;
            }
            else
            {
				CCTRACEE(_T("failed to create Firewall OEM setting (0x%x)"), symres);
            }
        }
    }

    //Start CC Services
	SetCCServicesStates(true, true);



	if (InstallIDSDefs())
	{
		CCTRACEW(_T("CSetupInfo::PostInstall() : InstallIDSDefs() Succeeded."));
	}
	else
	{
		CCTRACEE(_T("CSetupInfo::PostInstall() : InstallIDSDefs() Failed."));
	}

	//Import NAV's default rules regardless of whether or not IDS Defs have been installed
	//If we are yielding to NPF, don't import NAV's default rules.
	if(!IWPIsYielding())
	{
		CString sDefRulesPath;
		ccSym::CInstalledApps::GetNAVDirectory(sDefRulesPath);
		PathAddBackslash(sDefRulesPath.GetBuffer(sDefRulesPath.GetLength() + 2));
		sDefRulesPath.ReleaseBuffer();
		sDefRulesPath += _T("DefRules.xml");

		if(fw::ImportDefaultRules(sDefRulesPath))
		{
			CCTRACEW(_T("CSetupInfo::PostInstall() : ImportDefaultRules() Succeeded."));
		}
		else
		{
			CCTRACEE(_T("CSetupInfo::PostInstall() : ImportDefaultRules() Failed."));
		}
	}


    //HACK-Alert!!
	//Fake a NPF install only if IWP is not yielding
	//Put the NIS Installed Apps Registry key back, so that NAV 2007 can fake that NPF 7.0 is installed,
	//and there by block NPF 7.0. (NAV dir alreday has a nisver.dat, we just need to point NIS InstalledApps
	//path to NAV dir)
	FakeNISInstall(true);
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// This function is called before an uninstall.  Its only purpose in NAVs 
// case is to prompt to delete any Quarantined files.
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::PreUninstall()
{
    SYMINTERFACE_SRX_TRY(symRes)
    {
        // Init the QBackup loader
        QBackup::qb_IQBackup qbLoader;
        srx << qbLoader.Initialize();

        // Get an instance of QBackup
        QBackup::IQBackupPtr spQBackup;
        srx << qbLoader.CreateObject(&spQBackup);

        // Initialize QBackup
        srx << spQBackup->Initialize();

        // Get a count of backup sets
        size_t iSets = spQBackup->GetBackupSetCount();
        if(iSets > 0)
        {
			// Something's in QBackup.
	        CString sPrompt;	
			CString sTitle;
			sPrompt.LoadString(IDS_REMOVE_QUARANTINE_FILES);
			sTitle.LoadString(IDS_REMOVE_TITLE);
			if(MessageBoxEx(NULL, sPrompt, sTitle, MB_TASKMODAL | MB_YESNO | MB_SETFOREGROUND | MB_ICONQUESTION, 0) == IDYES)
			{
				SetProperty(_T("REMOVEQUARANTINE"), _T("1"));
			}
			else
			{
				SetProperty(_T("REMOVEQUARANTINE"), _T("0"));
			}
        }
    }
    SYMINTERFACE_SRX_CATCH_ALL_NO_RETURN(symRes);

    if(SYM_FAILED(symRes))
    {
		CCTRACEE(_T("PreUninstall(): Failed to get QBackup set count (sr=%08X)"), symRes);
		SetProperty(_T("REMOVEQUARANTINE"), _T("0"));
    }

	//Restore the ICF Settings
	if(getRestoreICFSettings())
	{
		ICFManagerLoader ICFMgrLoader;
		IICFManagerPtr pICFManager;
		if(SYM_SUCCEEDED(ICFMgrLoader.CreateObject(&pICFManager)))
		{
			if( SYM_SUCCEEDED(pICFManager->Init()))
			{
				if(SYM_SUCCEEDED(pICFManager->RestoreConnections()))
				{
					CCTRACEW(_T("SymSetup::ICFManager::RestoredConnections Succeeded"));
				}
				else
					CCTRACEE(_T("SymSetup::ICFManager::RestoredConnections Failed"));
			}
		}
	}

    // TODO: This should really be it's own custom action!

	// Shut down the SymProtect service in case we need to uninstall it.
	CSymProtectControlHelper SymProtectCtrlHelper;

	if(SymProtectCtrlHelper.Create())
	{
		// First get the current state
		m_bSPRunning = false;
		bool bError = true;
		if( !SymProtectCtrlHelper.GetStatus(m_bSPRunning, bError) )
		{
			CCTRACEW(_T("CSetupInfo::PreUninstall() - Failed to get the status for SymProtect...going to attempt to change the state anyway."));
		}

		// disable the service
		if( !SymProtectCtrlHelper.SetBBSystemEnabled(false))
		{
			CCTRACEE (_T("CSetupInfo::PreUninstall() - Failed to Disable SymProtect"));
		}
		else
		{
			CCTRACEI (_T("CSetupInfo::PreUninstall() - Successfully Disabled SymProtect"));
		}
	}
	else
	{
		CCTRACEE(_T("CSetupInfo::PreUninstall() - SymProtectCtrlHelper.Create() Failed"));
	}

	DeleteShortcuts();

	//HACK-Alert!!
	//Remove the Fake installation if IWP is non yielding.
	FakeNISInstall(false);

	return TRUE;
}

BOOL CSetupInfo::GetExistingProductVersion(InstallToolBox::LP_ITB_VERSION itbExistingVer)
{
    return InstallToolBox::GetNAVVersion(itbExistingVer);
}

BOOL CSetupInfo::GetNewProductVersion(InstallToolBox::LP_ITB_VERSION itbNewVer)
{
    return InstallToolBox::GetNewNAVVersion(itbNewVer);
}

BOOL CSetupInfo::GetExistingLicenseType(DWORD& nExistingLicType)
{
	CNAVInfo NAVInfo;
	TCHAR szNAVOpts[MAX_PATH] = {0};
	DWORD dwCurLicType = 0;
	wsprintf(szNAVOpts, _T("%s\\%s"), NAVInfo.GetNAVDir(), _T("navopts.dat"));
	InstallToolBox::CInstoptsDat Instopts(0, szNAVOpts, TRUE);
	
	// DRM changed their license type value so check both old and new
	if(!Instopts.ReadDWORD(LICENSE_Type, nExistingLicType, 0))
		Instopts.ReadDWORD("DRM:DRM::LicenseType", nExistingLicType, 0);
	
	//also check the installed version for one of the known scsss only release (they didn't all have the LicenseType value)
	InstallToolBox::ITB_VERSION NavVer;
	InstallToolBox::GetNAVVersion(&NavVer);
	if(NavVer.nMajorHi == 9)
		if(NavVer.nMajorLo == 14 || NavVer.nMajorLo == 10 || NavVer.nMajorLo == 02)
			nExistingLicType = 2;

    CCTRACEI(_T("GetExistingLicenseType = %d"), nExistingLicType);
        
    return TRUE;
}

BOOL CSetupInfo::GetNewLicenseType(DWORD& nNewLicType)
{
	InstallToolBox::CInstoptsDat InstoptsDat;
	BOOL bRet = InstoptsDat.ReadDWORD("DRM:DRM::LicenseType", nNewLicType);
	
    CCTRACEI(_T("GetNewLicenseType = %d"), nNewLicType);
	
    return bRet;
}

BOOL CSetupInfo::GetExistingProductPNU(CString& sExistingPNU)
{
    //check the PNU for the product already on the system
	ATL::CString sOldNAVOpts;
	CNAVInfo NAVInfo;
	wsprintf(sOldNAVOpts.GetBuffer(MAX_PATH), _T("%s\\NAVOPTS.DAT"), NAVInfo.GetNAVDir());
	sOldNAVOpts.ReleaseBuffer();
	InstallToolBox::CInstoptsDat NAVOpts(0, sOldNAVOpts, TRUE);
	BOOL bRet = NAVOpts.ReadString(_T("SKU:PNU"), sExistingPNU.GetBuffer(MAX_PATH), MAX_PATH);
	sExistingPNU.ReleaseBuffer();
	
    CCTRACEI(_T("GetExistingProductSKU = %s (bRet = %d)"), sExistingPNU, bRet);
	
    return bRet;
}

BOOL CSetupInfo::GetNewProductPNU(CString& sNewPNU)
{
	//check the PNU for the product we're about to install.
	InstallToolBox::CInstoptsDat SKU_DAT(0, m_szSKUPath, true);
	BOOL bRet = SKU_DAT.ReadString(_T("PNU"), sNewPNU.GetBuffer(MAX_PATH), MAX_PATH);
	sNewPNU.ReleaseBuffer();

	CCTRACEI(_T("GetExistingProductSKU = %s (bRet = %d)"), sNewPNU, bRet);

	return bRet;
}

BOOL CSetupInfo::PostUninstallPrevious(BOOL bRollback)
{   
    // dont need to restore the key when we're rolling back
    if(bRollback)
        return TRUE;

    // if we installed over NIS 2004, NIS has nuked our SSINSTALLDIR
    // key.  need to hack it and put it back now.  the CBrowsePage
    // dialog destructor will still clean it out
    CRegKey rkTempRegKeys;
    rkTempRegKeys.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\SymSetup\\TempRegKeys"));
    rkTempRegKeys.SetStringValue(_T("SSINSTALLDIR"), GetProperty(_T("INSTALLDIR")));

	RemoveSNDUpdaterPatches();

    return TRUE;
}

void CSetupInfo::PreOverInstallValidation(InstallToolBox::LP_ITB_VERSION itbExistingVer, InstallToolBox::LP_ITB_VERSION itbNewVer, DWORD dwExistingLicType, DWORD dwNewLicType, bool& bContinue, bool& bRet)
{
	// default to continue
	bRet = true;
	bContinue = true;

	// If the product is more than 2 years old, we don't support over install
	// Yeah, you'd think 2 years = 2 major version numbers. However,
	// NAV 2005 = 11.0
	// NAV 2006 = 12.0
	// NAV 2007 = 14.0???
	// Don't worry. It's not an off-by-one error. It's a we-can't-count error.
	if(itbExistingVer->nMajorHi < (itbNewVer->nMajorHi - 3))
	{
		// Have Setup display a link to Add/Remove Programs
		SetErrorResourceIDs(IDS_OLD_NAV , ADD_REMOVE_CPL, IDS_ARP_DESC_LC);

		bRet = false;
		bContinue = false;
		return;
	}

	// Check for the SuiteInstall key.  If this is set to 1 then we know the existing NAV
	// is installed with NIS or NSW, and we have different business rules in this case
    CRegKey rkNortonAntiVirus;
	DWORD dwSuiteInstall = 0;
    if(ERROR_SUCCESS == rkNortonAntiVirus.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus")) &&
		ERROR_SUCCESS == rkNortonAntiVirus.QueryDWORDValue(_T("SuiteInstall"), dwSuiteInstall))
	{
		if(1 == dwSuiteInstall)
		{
			DWORD dwPosition = 0;
			bool bExistingIsNewer = true;

			CompareProductVersions(dwPosition, bExistingIsNewer);

			if( (dwPosition == 0 || dwPosition == 4) && // if the first three version #s are the same
				(dwExistingLicType == dwNewLicType)) // and the license types are the same
			{
				// Normal overinstall logic would prompt to display Add/Remove programs here
				// But we want to allow the install of the standalone so short circuit the rest
				// and pass the launch condition
				bRet = true;
				bContinue = false;
				return;
			}
		}
	}

}

BOOL CSetupInfo::PostUninstall(BOOL bRollback)
{
	if(bRollback)
	{
		CreateShortcuts();
		return TRUE;
	}

	HRESULT hr = S_OK;
	bool bIsInstalled = false;
	CWndDefenderHelper myHelper;
	if(SUCCEEDED(myHelper.IsInstalled(bIsInstalled)) && bIsInstalled)
	{
		// Turn Windows Defender on if it is installed.
		hr = myHelper.SetState(true);
		if (FAILED(hr))
		{
			CCTRACEE(_T("Failed to turn on Windows Defender. HR = 0x%x"), hr);
		}
	}

	UnRegisterWithWMI();

	if(m_bSPRunning)
	{
		// SP was running when we started the uninstall, so try to re-enable
		// first we need to make sure ccEvtMgr is running.
		// scope this loader so it go out of scope last
		cc::ccInst_IServicesConfig SvcCfgLdr;
		{
			cc::IServicesConfigPtr spSvcCfg;

			if(SYM_FAILED(SvcCfgLdr.CreateObject(spSvcCfg.m_p)) || spSvcCfg == NULL)
			{
				CCTRACEW(_T("CSetupInfo::PostUninstall() - Unable to load IServicesConfig Object.\n"));
			}
			else if(!spSvcCfg->Create())
			{
				CCTRACEW(_T("CSetupInfo::PostUninstall() - spSvcCfg->Create() == false\n"));
			}
			else if(spSvcCfg->SetServicesStartup(spSvcCfg->SC_AUTO) == FALSE)
			{
				CCTRACEW(_T("CSetupInfo::PostUninstall() - spSvcCfg->SetAllServicesAuto() == FALSE.\n"));
			}
			else
			{
				CCTRACEI(_T("CSetupInfo::PostUninstall() - Starting all CC Services.\n"));
				if(spSvcCfg->Start() == FALSE)
				{
					CCTRACEW(_T("CSetupInfo::PostUninstall() - spSvcCfg->Start() == FALSE.\n"));
				}
			}
		}

		// Now try to start SP
		CSymProtectControlHelper SymProtectCtrlHelper;

		if(SymProtectCtrlHelper.Create())
		{
			// enable the service
			if( !SymProtectCtrlHelper.SetBBSystemEnabled(true) )
			{
				CCTRACEE (_T("CSetupInfo::PostUninstall() - Failed to Enable SymProtect"));
			}
			else
			{
				CCTRACEI (_T("CSetupInfo::PostUninstall() - Successfully Enabled SymProtect"));
			}
		}
		else
		{
			CCTRACEE(_T("CSetupInfo::PostUninstall() - SymProtectCtrlHelper.Create() Failed"));
		}
	}

	//Unregister Component Framework plugins
	CCLTProductHelper CLTProductHelper;
	hr = CLTProductHelper.Initialize();
	if(SUCCEEDED(hr))
	{
		hr = CLTProductHelper.UnRegisterFrameworkPlugins();
		if(SUCCEEDED(hr))
			CCTRACEI(CCTRCTX _T("UnRegisterFrameworkPlugins Succeeded"));
		else
			CCTRACEE(CCTRCTX _T("UnRegisterFrameworkPlugins Failed. HR = 0x%x"),hr);
	}
	else
		CCTRACEE(CCTRCTX _T("CLTProductHelper.Initialize() Failed. HR = 0x%x"),hr);

	return TRUE;
}

HRESULT CSetupInfo::SetupInstallMode()
{
	HRESULT hr = CSetupInfoBase::SetupInstallMode();
	CMSIDatabase dbLayout;

	if(SUCCEEDED(hr) && 
		ACTIONS_MODIFY == m_Action &&
		-1 == m_sCommandLine.Find(_T("/I")) &&
		-1 == m_sCommandLine.Find(_T("/i")) &&
		-1 == m_sCommandLine.Find(_T("/X")) &&
		-1 == m_sCommandLine.Find(_T("/x")) &&
		(!InstallToolBox::FileExists(GetFullPathToParent()) ||
		FAILED(dbLayout.OpenDatabase(GetFullPathToParent()))))
	{
		// At this point, we are in modify mode, which is the same as uninstall
		// and it was likely caused by missing or unreadable NAV.msi
		// In this case, set install mode to ACTIONS_INSTALL so the user will get
		// a message indicating that the setup related files could not be found
		// Defect 1-4WIW5U
		m_Action = ACTIONS_INSTALL;
	}

	return hr;
}

HRESULT CSetupInfo::ProcessCommandLines(LPTSTR lpstrCmdLine)
{
	HRESULT hret = CSetupInfoBase::ProcessCommandLines(lpstrCmdLine);
	if(FAILED(hret))
	{
		CCTRACEE(_T("CSetupInfo::ProcessCommandLines(): CSetupInfoBase::ProcessCommandLines() failed (%X)"), hret);
		return hret;
	}

	InstallToolBox::CInstoptsDat Instopts;

	HRESULT hr = Instopts.VerifyInstoptsPath();

	if(ERROR_SUCCESS != hr)
	{
		CCTRACEE(_T("CSetupInfo::ProcessCommandLines(): Could not verify instopts path (%X)"), hr);
		return hret;
	}

	bool bDriverManualStart = false;
	DWORD dwDriverManualStart = 0;
	if(TRUE == Instopts.ReadDWORD(AntiVirus::INSTOPTS_DRIVERS_MANUAL_START, dwDriverManualStart, 0))
	{
		if(1 == dwDriverManualStart) bDriverManualStart = true;
	}

	if(bDriverManualStart)
	{
		// use the manual symtdi switch
		m_sCommandLine += _T(" /O NO_LOAD_SYMTDI=1 ");
	}

	return hret;
}

bool CSetupInfo::RegisterWithWMI()
{
	bool bRet = true;
	CWSCHelper WSCHelper;
	CString sProductName = GetProperty(_T("CoBrandedProductNameNoVer"));

	HRESULT hr;
	hr = WSCHelper.WriteAVStatus((LPGUID)&IID_ProductID_NAV_AV,
								_T("Symantec Corporation"),
								_T("2007"),
								sProductName,
								_T(""),
								_T(""),
								_T(""),
								_T(""),
								true,
								true,
								_T(""));

	if (FAILED(hr))
	{
		CCTRACEE(_T("CSetupInfo::RegisterWithWMI() : Failed to WriteAVStatus() CWSCHelper. (0x%08X)"), hr);
		bRet = false;
	}

	hr = WSCHelper.WriteAntiSpywareStatus((LPGUID)&IID_ProductID_NORTON_ANTISPYWARE,
										_T("2007"),
										sProductName,
										_T(""),
										true,
										true);

	if (FAILED(hr))
	{
		CCTRACEE(_T("CSetupInfo::RegisterWithWMI() : Failed to WriteAntiSpywareStatus() CWSCHelper. (0x%08X)"), hr);
		bRet = false;
	}

    // Register IWP if it is installed.
	INSTALLSTATE isIWPState = MsiQueryProductState(_T("{2908F0CB-C1D4-447F-97A2-CFC135C9F8D4}"));
	if (isIWPState == INSTALLSTATE_DEFAULT || isIWPState == INSTALLSTATE_ABSENT)
	{
		hr = WSCHelper.WriteFirewallStatus((LPGUID)&IID_ProductID_NAV_FW,
										_T("Symantec Corporation"),
										_T("2007"),
										sProductName,
										_T(""),
										_T(""),
										true,
										_T(""));

		if (FAILED(hr))
		{
			CCTRACEE(_T("CSetupInfo::RegisterWithWMI() : Failed to WriteFirewallStatus() CWSCHelper. (0x%08X)"), hr);
			bRet = false;
		}
	}

	return bRet;
}

void CSetupInfo::UnRegisterWithWMI()
{
	CWSCHelper WSCHelper;
	WSCHelper.UninstallAV((LPGUID)&IID_ProductID_NAV_AV, (LPCWSTR)NULL);
	WSCHelper.UninstallFirewall((LPGUID)&IID_ProductID_NAV_FW, (LPCWSTR)NULL);
}

bool CSetupInfo::SetCCServicesStates(BOOL bAutoStart, BOOL bStartNow)
{
	bool return_value = true;

	// scope this loader so it go out of scope last
	cc::ccInst_IServicesConfig_Untrusted SvcCfgLdr;
	{
		cc::IServicesConfigPtr spSvcCfg;

		if(SYM_FAILED(SvcCfgLdr.CreateObject(spSvcCfg.m_p)) || spSvcCfg == NULL)
		{
			CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - Unable to load IServicesConfig Object.\n"));
			return false;
		}

		if(!spSvcCfg->Create())
		{
			CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->Create() == false\n"));
			return false;
		}

		CCTRACEI(_T("CInstallWatcher::SetCCServicesStates() - Setting CC services to automatic start.\n"));
		if(spSvcCfg->SetServicesStartup(bAutoStart ? spSvcCfg->SC_AUTO : spSvcCfg->SC_MANUAL) == FALSE)
		{
			CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->SetAllServicesAuto() == FALSE.\n"));
			return_value = false;
		}
		
		CRebootHandler rbHandler;
		if(bStartNow && !rbHandler.IsRebootNeeded())
		{
			CCTRACEI(_T("CInstallWatcher::SetCCServicesStates() - Starting all CC Services.\n"));
			if(spSvcCfg->Start() == FALSE)
			{
				CCTRACEE(_T("CInstallWatcher::SetCCServicesStates() - spSvcCfg->Start() == FALSE.\n"));
				return_value = false;
			}
		}
	}

	return return_value;
}

void CSetupInfo::ResetPassword()
{
	//	NOTE: Our caller needs to init/uninit COM (for ccPasswd/ccSettings).

	CString szPath;
	szPath.Empty();
	ccSym::CInstalledApps::GetNAVDirectory(szPath);
	szPath += _T("\\isRes.dll");

	HINSTANCE hISRes = LoadLibraryEx(szPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hISRes != NULL)
	{
		CString sProductName;
		sProductName.LoadString(hISRes, IDS_BRANDING_PRODUCT_NAME);
		FreeLibrary(hISRes);

		CCCPwdDynamic ccpw;
		ccpw.ResetPassword(NIS_CONSUMER_PASSWORD_ID, sProductName, NULL, NULL, FALSE);
	}
}


bool CSetupInfo::FakeNISInstall(bool bAdd)
{
	if(!IWPIsYielding())
	{
		//Add Hack
		if(bAdd)
		{
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
		}
		else //Remove Hack
		{
			CRegKey rk;
			if(ERROR_SUCCESS == rk.Open(HKEY_LOCAL_MACHINE, isRegLocations::szInstalledAppsKey))
			{
				if(ERROR_SUCCESS != rk.DeleteValue(isRegLocations::szInstalledAppsValue))
					CCTRACEE(CCTRCTX _T("Failed to DeleteValue %s"),isRegLocations::szInstalledAppsValue);

				if(ERROR_SUCCESS != rk.DeleteValue(isRegLocations::szInstalledAppsValue_Legacy))
					CCTRACEE(CCTRCTX _T("Failed to DeleteValue %s"),isRegLocations::szInstalledAppsValue_Legacy);
			}

		}
	}
	return true;
}

bool CSetupInfo::IWPIsYielding()
{
	bool bYielding = true;

	IWP_IWPSettingsNoCache IWPLoader;
	IWP::IIWPSettingsPtr pIWPSettings;
	SYMRESULT sr = IWPLoader.CreateObject(&pIWPSettings);
	if (SYM_SUCCEEDED(sr))
	{
		DWORD dwState;
		sr = pIWPSettings->GetValue(IWP::IIWPSettings::IWPState, dwState);
		if (SYM_SUCCEEDED(sr))
		{
			if (dwState == IWP::IIWPSettings::IWPStateYielding)
				bYielding = true;
			else
				bYielding = false;
			CCTRCTXI1(_T("IWP is %syielding"), (bYielding ? _T("") : _T("not ")));
		}
		else
			CCTRCTXE1(_T("pIWPSettings->GetValue failed (0x%08x)"), sr);
	}
	else
	{
		CCTRCTXE1(_T("IWPLoader.CreateObject failed (0x%08x)"), sr);
	}

	return bYielding;
}

bool CSetupInfo::ShowUninstallSubWarningPage()
{
	//check the PNU for the product already on the system
	ATL::CString sCfgwizDat;
	CNAVInfo NAVInfo;
	wsprintf(sCfgwizDat.GetBuffer(MAX_PATH), _T("%s\\CFGWIZ.DAT"), NAVInfo.GetNAVDir());
	sCfgwizDat.ReleaseBuffer();

	InstallToolBox::CInstoptsDat CfgWizDat(0, sCfgwizDat, TRUE);
	DWORD dwValue = 1;
	if(CfgWizDat.ReadDWORD(InstallToolBox::CFGWIZ_ShowUninstallSubWarningPage,dwValue))
	{
		if(0 == dwValue)
			return false;
	}
	return true;
}

void CSetupInfo::RemoveSNDUpdaterPatches()
{
	// OLD SND Patch Upgrade Code (SND 5.3.0, 5.3.1, 5.3.2, 5.4.3, 5.4.4, and 5.5.x patches)
	static const TCHAR SNDUPDATER_UPGRADE_CODE[] = _T("{E459629C-E294-4EDB-BEF4-C57A5CE85466}");
	static const TCHAR SYMTDI_SYS_COMPONENT_ID[] = _T("{61638B6B-D47A-4E2F-B1E2-469CA11F4994}");

	TCHAR szSNDUpdaterProduct[39] = {0};
	DWORD dwSNDUpdaterIndex = 0;
	std::vector<CString> SNDUpdaterProductCodeList;

	TCHAR szProductName[MAX_PATH] = {0};
	TCHAR szProductVersion[MAX_PATH] = {0};
	DWORD dwPropertyBufferSize = 0; 

	//Find out how many SNDUpdater Patches are installed.
	while (ERROR_SUCCESS == MsiEnumRelatedProducts(SNDUPDATER_UPGRADE_CODE, 0, dwSNDUpdaterIndex, szSNDUpdaterProduct))
	{
		szProductName[0] = szProductVersion[0] = _T('\0');
		dwPropertyBufferSize = CCDIMOF(szProductName);
		MsiGetProductInfo(szSNDUpdaterProduct, INSTALLPROPERTY_INSTALLEDPRODUCTNAME, szProductName, &dwPropertyBufferSize);

		dwPropertyBufferSize = CCDIMOF(szProductVersion);
		MsiGetProductInfo(szSNDUpdaterProduct, INSTALLPROPERTY_VERSIONSTRING, szProductVersion, &dwPropertyBufferSize);

		CCTRACEW(_T("Found an SNDUpdater Patch. ProductName: %s. ProductVersion: %s."), szProductName, szProductVersion);
		SNDUpdaterProductCodeList.push_back(CString(szSNDUpdaterProduct));
		dwSNDUpdaterIndex++;
	}

	//Return if no SNDUpdater Patches were found.
	if (!dwSNDUpdaterIndex)
	{
		CCTRACEW(_T("No SND Updater Patches found."));
		return;
	}

	TCHAR szSymTDIClients[39] = {0};
	DWORD dwSymTDIIndex = 0;

	//Find out how many MSIs are clients of SymTDI.sys
	while (ERROR_SUCCESS == MsiEnumClients(SYMTDI_SYS_COMPONENT_ID, dwSymTDIIndex, szSymTDIClients))
	{
		szProductName[0] = szProductVersion[0] = _T('\0');

		//Make sure this product is really installed before counting it as a valid client.
		INSTALLSTATE isSymTDIClientState = MsiQueryProductState(szSymTDIClients);
		if (isSymTDIClientState == INSTALLSTATE_ABSENT || isSymTDIClientState == INSTALLSTATE_DEFAULT)
		{
			dwPropertyBufferSize = CCDIMOF(szProductName);
			MsiGetProductInfo(szSymTDIClients, INSTALLPROPERTY_INSTALLEDPRODUCTNAME, szProductName, &dwPropertyBufferSize);

			dwPropertyBufferSize = CCDIMOF(szProductVersion);
			MsiGetProductInfo(szSymTDIClients, INSTALLPROPERTY_VERSIONSTRING, szProductVersion, &dwPropertyBufferSize);
	
			CCTRACEE(_T("Found a client of SymTDI. ProductName: %s. ProductVersion: %s"), szProductName, szProductVersion);
		}
		else
		{
			CCTRACEE(_T("Unknown product referencing SymTDI component. (%s)."), szSymTDIClients);
		}

		dwSymTDIIndex++;
	}

	//If the number of SND Updater Patches is the same as the number of SymTDI.sys clients, then there are only
	//patches remaining on the machine and they should all be removed.
	if (dwSNDUpdaterIndex == dwSymTDIIndex)
	{
		for (std::vector<CString>::iterator it = SNDUpdaterProductCodeList.begin(); it != SNDUpdaterProductCodeList.end(); it++)
		{
			static const TCHAR SNDUPDATER_UNINSTALL_PARAMS[] = _T("REBOOT=ReallySuppress SYMNETCLIENTTYPE=Consumer IDSCLIENTTYPE=Consumer");

			UINT uRet = MsiConfigureProductEx(*it, INSTALLLEVEL_DEFAULT, INSTALLSTATE_ABSENT, SNDUPDATER_UNINSTALL_PARAMS);
			CCTRACEW(_T("Uninstalled %s, returned %d."), *it, uRet);
		}
	}
}
