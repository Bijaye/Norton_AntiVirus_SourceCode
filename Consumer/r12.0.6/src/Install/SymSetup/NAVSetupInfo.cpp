#include "stdafx.h"

#include "NAVSetupInfo.h"
#include "NAVInfo.h"
#include "shlobj.h"
#include "navresource.h"
#include "resource.h"
#include "optnames.h"
#include "AVRES.h"
#include "LiveUpdateWrap.h"
#include "NAVDetection.h"
#include "Instopts.h"
#include "FileAction.h"
#include "IWPSettingsInterface.h"
#include "IWPSettingsLoader.h"
#include "InstoptsNames.h"

#include "V2LicensingAuthGuids.h"

// header to load up quarantine dll syminterface obj
#include "QuarantineDllLoader.h"

// cclib headers
#include "ccLib.h"
#include "ccLibStaticLink.h"
#include "ccServicesConfigInterface.h"
#include "ccInstLoader.h"

// cctrace debug headers
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("NAVSetup"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

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

// For turning on/off SymProtect
#include "SymProtectControlHelper.h"

// YOP INSTALL : For removing Shortcut from Start Menu
#include <io.h>
#include <sys/stat.h>
BOOL DeleteDirectory(const TCHAR* sPath);
BOOL IsDots(const TCHAR* str);
HRESULT g_hrYOP = S_OK;

// construction
CSetupInfo::CSetupInfo(void)
{
	m_hUIRES = NULL;
	m_szGUIDCACHEKEY = _T("Software\\Symantec\\SymSetup\\Norton AntiVirus");
    m_strSuiteOwnerPath = _T(".\\NAV\\Parent\\NORTON\\AVSTE.dll");
	m_szParentMSIPath = _T(".\\NAV\\NAV.msi");
	m_szPatchInstID = _T("NAV");
	m_szSKUPath = _T(".\\NAV\\SKU.dat");
	m_szSymHelpPath = _T(".\\Support\\Help\\External\\Symhelp.chm");
    m_szWebRegPath = _T(".\\NAV\\SymLT\\NORTON\\WebReg.exe");

	m_NAVLaunchConditions.SetSetupInfo(this);
	m_NAVLaunchConditions.SetNAVInfo(this);
	
	CSetupInfoBase::CSetupInfoBase();
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
		if (FALSE == csProductNameWithVersion.LoadString(m_hUIRES,IDS_AV_PRODUCT_NAME_VERSION))
		{
			csProductNameWithVersion = _T("Norton AntiVirus");
		}

		if (FALSE == csProductNameNoVersion.LoadString(m_hUIRES,IDS_AV_PRODUCT_NAME))
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
				wsprintf(szNAVOptsDat, "%s\\NAVOpts.dat", szInstallDir);
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
			if (S_OK != g_hrYOP)
                return g_hrYOP;
			else
				return E_FAIL;
	}
	else
	{
		if(!m_NAVLaunchConditions.CheckUninstallConditions())
			if (S_OK != g_hrYOP)
                return g_hrYOP;
			else
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
	
	// if Finish wizard doesn't reboot then we should run CfgWiz
	CRebootHandler rbHandler;
	if(GetAction() == ACTIONS_INSTALL && !pageFinish.IsRebootNeeded() && !rbHandler.IsRebootNeeded())
	{
		CNAVInfo NAVInfo;
		TCHAR szCfgWiz[MAX_PATH] = {0};
		_stprintf(szCfgWiz, _T("%s\\CfgWiz.exe"), NAVInfo.GetNAVDir());

        TCHAR szCfgWizArgs[MAX_PATH] = {0};
        _stprintf(szCfgWizArgs, _T("/GUID %s /MODE CfgWiz /CMDLINE \"REBOOT\""), V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		InstallToolBox::ExecuteProgram(szCfgWiz, szCfgWizArgs, FALSE);
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
	sUIRES.Format(_T("%s\\AVRES.dll"), NAVInfo.GetNAVDir());
	m_hUIRES = LoadLibraryEx(sUIRES, NULL, LOAD_LIBRARY_AS_DATAFILE);

	if(m_hUIRES)
    {
	    // set up the static image resource ids for each image mask and the associated resoure handle
	    SetImage(SetupData::BIGIMAGE, IDB_INSTBACK, m_hUIRES);
	    SetImage(SetupData::TOPIMAGE, IDB_INSTTOP, m_hUIRES);
	    SetImage(SetupData::COBRANDIMAGE, IDB_INSTCOBRAND, m_hUIRES);

        // set up name for CED, if not found then setupinfobase will use ProductName instead
        ATL::CString csProductNameWithVersion;
        if (TRUE == csProductNameWithVersion.LoadString(m_hUIRES,IDS_AV_PRODUCT_NAME_VERSION))
        {
            SetProperty(_T("CoBrandedProductName"), csProductNameWithVersion);
        }
    }

	// setup pages
	pageModify.SetSetupInfo(this);
	pageModify.SetupPages(&pageVerifyRemove, NULL);
	pageVerifyRemove.SetSetupInfo(this);
	pageVerifyRemove.SetBackgroundImageMask(SetupData::BIGIMAGE);
	pageFinish.SetSetupInfo(this);
	pageFinish.SetNoBackButton();
	pageFinish.SetBackgroundImageMask(SetupData::BIGIMAGE);

	// setup wizard
	m_Wizard.AddPage(pageModify);
	m_Wizard.AddPage(pageVerifyRemove);
	m_FinishWizard.AddPage(pageFinish);
	

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
	sUIRES.Append(_T("\\NAV\\External\\NORTON\\AVRES.dll"));
	m_hUIRES = LoadLibraryEx(sUIRES, NULL, LOAD_LIBRARY_AS_DATAFILE);

	ATL::CString szProductName = _T("Norton AntiVirus");
	if(m_hUIRES)
    	{	
	    // Load AVRES.DLL for EULA and Bitmaps
	    // set up the static image resource ids for each image mask and the associated resoure handle
	    SetImage(SetupData::BIGIMAGE, IDB_INSTBACK, m_hUIRES);
	    SetImage(SetupData::TOPIMAGE, IDB_INSTTOP, m_hUIRES);
	    SetImage(SetupData::COBRANDIMAGE, IDB_INSTCOBRAND, m_hUIRES);

		// set up name for CED, if not found then setupinfobase will use ProductName instead
		ATL::CString csProductNameWithVersion;
		if (TRUE == csProductNameWithVersion.LoadString(m_hUIRES,IDS_AV_PRODUCT_NAME_VERSION))
		{
			SetProperty(_T("CoBrandedProductName"), csProductNameWithVersion);
		}
	}

	//Set up the default install path
	SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, (HANDLE)-1, 0, m_szFolder);
	_stprintf(m_szFolder, "%s\\%s",m_szFolder, szProductName);
	InstallToolBox::RemoveBackslash(m_szFolder);

	m_Wizard.SetSetupInfo(this);
	m_FinishWizard.SetSetupInfo(this);
	
	// setup the pages with the information they
	// need.
	DWORD dwShowWelcomePage = 1;
	DWORD dwShowScanPage = 1;
	InstallToolBox::CInstoptsDat Instopts;
	Instopts.ReadDWORD(InstallToolBox::INSTALL_ShowWelcomePage, dwShowWelcomePage, 1);
	Instopts.ReadDWORD(InstallToolBox::INSTALL_ShowScanPage, dwShowScanPage, 1);

	if(dwShowWelcomePage)
	{
		pageWelcome.SetSetupInfo(this);
		pageWelcome.SetNoBackButton();
		pageWelcome.SetBackgroundImageMask(SetupData::BIGIMAGE);
	}
	
	if(dwShowScanPage)
		pageScan.SetSetupInfo(this);

	pageActivate.SetSetupInfo(this);
	pageActivate.SetLicenseID(IDR_EULA);
	
    if(m_hUIRES)
    {
        pageActivate.SetLicenseResourceHandle(m_hUIRES);
    }

	// If welcome page is not displayed, activation page becomes the first page. 
	// Disable Back button for activation page.
	if(dwShowWelcomePage == 0)
		pageActivate.SetNoBackButton();

    pageBrowse.SetSetupInfo(this);
	pageBrowse.SetFolderProperty(_T("INSTALLDIR"));
	pageBrowse.SetSymPathsFlags(InstallToolBox::SYMPATH_NAV);
		SetProperty(_T("INSTALLDIR"), m_szFolder);
	pageBrowse.SetDefaultFolder(m_szFolder);
	pageBrowse.SetTempRegKey(_T("SSINSTALLDIR"));
	if(m_UIMode != UIMODE_NORMAL)
		pageBrowse.Reset();
	if(InstallToolBox::IsNAVInstalled())
		pageBrowse.SetupCheck1(IDS_KEEPSETTINGS, TRUE);
	
	pageFinish.SetSetupInfo(this);
	pageFinish.SetNoCancelButton();
	pageFinish.SetBackgroundImageMask(SetupData::BIGIMAGE);
		
	// add welcome dialogs
	if(dwShowWelcomePage)
		m_Wizard.AddPage(pageWelcome);
	
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

	if(dwShowScanPage)
	{
		m_Wizard.AddPage(pageScan);
	}
	
	// add the custom/express and 
	m_Wizard.AddPage(pageBrowse);
	
	// add the rest of the standard dialogs, these dialogs will
	// appear in every single install path no matter what
	m_FinishWizard.AddPage(pageFinish);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// This function is called before the install begins
//	- Used to Check if we're migrating options and set the regkey to inform our MSI
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::PreInstall()
{
	if(!pageBrowse.GetCheck1())
	{
		CRegKey NAVKey;
		NAVKey.Create(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"));
		NAVKey.SetDWORDValue(_T("DontMigrate"), 1);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// This function is called after install is complete.  It's for cleanup.
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::PostInstall(BOOL bRollback)
{
	CRegKey NAVKey;
	NAVKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"));
	NAVKey.DeleteValue(_T("DontMigrate"));
	
	// YOP INSTALL : We dont want Cfgwiz to show up after reboot. Deleting this from RUN key
	// if INSTOPTS:NoCfgwizAfterReboot = 1

	CCTRACEI ( "CSetupInfo::PostInstall -- Inside the YOP section of PostInstall" );
	
	InstallToolBox::CInstoptsDat Instopts;
	DWORD dwNoCfgwiz = 0;
    Instopts.ReadDWORD(InstallToolBox::INSTOPTS_NoCfgwizAfterReboot, dwNoCfgwiz, 0);
	
	if (dwNoCfgwiz)
	{
		CCTRACEI ( "CSetupInfo::PostInstall -- INSTOPTS_NoCfgwizAfterReboot is set to %d", dwNoCfgwiz);

		TCHAR cszCfgWizRunKey[] = _T("NAV CfgWiz");
		TCHAR cszMSRunKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
		HKEY hKey;
		
		LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, cszMSRunKey, NULL, KEY_WRITE, &hKey);
		if (ERROR_SUCCESS == lResult)
		{
			CCTRACEI ( "CSetupInfo::PostInstall -- MS Run key opened" );
			lResult = RegDeleteValue(hKey, cszCfgWizRunKey);
			if (ERROR_SUCCESS == lResult)
				CCTRACEI ( "CSetupInfo::PostInstall -- Removed cfgwiz from Run key" );
			else
				CCTRACEE ( "CSetupInfo::PostInstall -- ERROR! Cfgwiz key could not be deleted" );
		}
		else
			CCTRACEE ( "CSetupInfo::PostInstall -- Unable to open MS Run key" );
		
		RegCloseKey(hKey);
	}
	
	// YOP INSTALL : Deleting Shortcut menu if INSTOPTS:RemoveSMShortcut = 1
	
	DWORD dwNoShortcut = 0;
    Instopts.ReadDWORD(InstallToolBox::INSTOPTS_RemoveSMShortcut, dwNoShortcut, 0);
	CCTRACEI ( "CSetupInfo::PostInstall -- INSTOPTS:RemoveSMShortcut = %d", dwNoShortcut );

	if (dwNoShortcut)
	{
		// Need to remove the Start Menu Shortcuts
		CString sShortCutFolder, sProductName, sSMShortCutFolder;
		ULONG nSize;
		
		SHGetFolderPath(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, sShortCutFolder.GetBuffer(MAX_PATH));
		NAVKey.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\Norton AntiVirus"));
		//sSMShortCutFolder.LoadString(_Module.GetModuleInstance(),IDS_SMSHORTCUT_FOLDER);
		
		if (ERROR_SUCCESS == NAVKey.QueryStringValue(_T("ProductName"), sProductName.GetBuffer(MAX_PATH), &nSize))
		{
			sShortCutFolder.Format(_T("%s\\%s"), sShortCutFolder, sProductName);
			CCTRACEI ( "CSetupInfo::PostInstall -- Shortcut Path = %s", sShortCutFolder );
			if (DeleteDirectory(sShortCutFolder))
				CCTRACEI ( "CSetupInfo::PostInstall -- Shortcut Path deletion successful" );
			else
				CCTRACEE ( "CSetupInfo::PostInstall -- Shortcut Path deletion unsuccessful" );
		}
		else
			CCTRACEE ( "CSetupInfo::PostInstall -- ProductName key could not be queried!" );

		NAVKey.Close();
		sShortCutFolder.ReleaseBuffer();
		sProductName.ReleaseBuffer();
	}
	CCTRACEI ( "CSetupInfo::PostInstall -- End of YOP section of PostInstall" );

	try
	{
		if(SUCCEEDED(CoInitialize(NULL)))
		{
			InstallToolBox::CLiveUpdate::EnableExpresMode(false);
			CoUninitialize();
		}
	}
	catch(...)
	{
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// This function is called before an uninstall.  Its only purpose in NAVs 
// case is to prompt to delete any Quarantined files.
//
//////////////////////////////////////////////////////////////////////////
BOOL CSetupInfo::PreUninstall()
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		IQuarantineDLL* pIQuar = NULL;
		IQuarantineItem* pIQuarItem;
		IEnumQuarantineItems* pIEnum = NULL;
		QuarDLL_Loader	QuarDllLoader;
        // Create the quarantine server object.

        if(SYM_SUCCEEDED(QuarDllLoader.CreateObject(&pIQuar)) &&
           pIQuar != NULL )
		{
            if (SUCCEEDED(pIQuar->Initialize()))
			{
                if (SUCCEEDED(pIQuar->Enum(&pIEnum)))
				{
					ULONG ulFetched;
                    if (S_OK == pIEnum->Next(1, &pIQuarItem, &ulFetched))
					{
						// Something's in Quarantine.
                        CString sPrompt;	
						CString sTitle;
						sPrompt.LoadString(IDS_REMOVE_QUARANTINE_FILES);
						sTitle.LoadString(IDS_REMOVE_TITLE);
						if(MessageBoxEx(NULL, sPrompt, sTitle, MB_TASKMODAL | MB_DEFAULT_DESKTOP_ONLY | MB_YESNO | MB_SETFOREGROUND | MB_ICONQUESTION, 0) == IDYES)
						{
							SetProperty(_T("REMOVEQUARANTINE"), _T("1"));
						}
						else
						{
							SetProperty(_T("REMOVEQUARANTINE"), _T("0"));
						}
                        
						pIQuarItem->Release();
					}

                    pIEnum->Release();

				}
			}
            pIQuar->Release ();
		}   

		// Shut down the SymProtect service in case we need to uninstall it.
		CSymProtectControlHelper SymProtectCtrlHelper;

		if(SymProtectCtrlHelper.Create())
		{
			// First get the current state
			m_bSPRunning = false;
			bool bError = true;
			if( !SymProtectCtrlHelper.GetStatus(m_bSPRunning, bError) )
			{
				CCTRACEW(_T("CSetupInfo::PreInstall() - Failed to get the status for SymProtect...going to attempt to change the state anyway."));
			}

			// disable the service
			if( !SymProtectCtrlHelper.SetComponentEnabled(false) )
			{
				CCTRACEE (_T("CSetupInfo::PreInstall() - Failed to Disable SymProtect"));
			}
			else
			{
				CCTRACEI (_T("CSetupInfo::PreInstall() - Successfully Disabled SymProtect"));
			}
		}
		else
		{
			CCTRACEE("CSetupInfo::PreInstall() - SymProtectCtrlHelper.Create() Failed");
		}

		CoUninitialize();
	}

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
    CRegKey rkInstalledApps;
    rkInstalledApps.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec\\InstalledApps"));
    rkInstalledApps.SetStringValue(_T("SSINSTALLDIR"), GetProperty(_T("INSTALLDIR")));

    return TRUE;
}

void CSetupInfo::PreOverInstallValidation(InstallToolBox::LP_ITB_VERSION itbExistingVer, InstallToolBox::LP_ITB_VERSION itbNewVer, DWORD dwExistingLicType, DWORD dwNewLicType, bool& bContinue, bool& bRet)
{
	// default to continue
	bRet = true;
	bContinue = true;

	// If the product is more than 2 major versions old, we don't support over install
	if((itbNewVer->nMajorHi - itbExistingVer->nMajorHi) > 2)
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
	if(m_bSPRunning)
	{
		// SP was running when we started the uninstall, so try to re-enable
		// first we need to make sure ccEvtMgr is running.

		HRESULT hr;

		hr = CoInitialize(NULL);

		if(SUCCEEDED(hr))
		{
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
				if( !SymProtectCtrlHelper.SetComponentEnabled(true) )
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
				CCTRACEE("CSetupInfo::PostUninstall() - SymProtectCtrlHelper.Create() Failed");
			}

			CoUninitialize();
		}
		else
		{
			CCTRACEE(_T("CSetupInfo::PostUninstall() - Failed to initialize COM (0x%x)"), hr);
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// This function is used to delete a dorectory structure 
//
//////////////////////////////////////////////////////////////////////////
BOOL DeleteDirectory(const TCHAR* sPath) 
{
   HANDLE hFind;    // file handle
   WIN32_FIND_DATA FindFileData;

   TCHAR DirPath[MAX_PATH];
   TCHAR FileName[MAX_PATH];

   _tcscpy(DirPath,sPath);
   _tcscat(DirPath,"\\*");    // searching all files
   _tcscpy(FileName,sPath);
   _tcscat(FileName,"\\");

   // find the first file
   hFind = FindFirstFile(DirPath,&FindFileData);
   if(hFind == INVALID_HANDLE_VALUE) return FALSE;
   _tcscpy(DirPath,FileName);

   bool bSearch = true;
   while(bSearch) {    // until we find an entry
      if(FindNextFile(hFind,&FindFileData)) {
         if(IsDots(FindFileData.cFileName)) continue;
         _tcscat(FileName,FindFileData.cFileName);
         if((FindFileData.dwFileAttributes &
            FILE_ATTRIBUTE_DIRECTORY)) {

            // we have found a directory, recurse
            if(!DeleteDirectory(FileName)) {
                FindClose(hFind);
                return FALSE;    // directory couldn't be deleted
            }
            // remove the empty directory
            RemoveDirectory(FileName);
             _tcscpy(FileName,DirPath);
         }
         else {
            if(FindFileData.dwFileAttributes &
               FILE_ATTRIBUTE_READONLY)
               // change read-only file mode
                  _chmod(FileName, _S_IWRITE);
                  if(!DeleteFile(FileName)) {    // delete the file
                    FindClose(hFind);
                    return FALSE;
               }
               _tcscpy(FileName,DirPath);
         }
      }
      else {
         // no more files there
         if(GetLastError() == ERROR_NO_MORE_FILES)
         bSearch = false;
         else {
            // some error occurred; close the handle and return FALSE
               FindClose(hFind);
               return FALSE;
         }

      }

   }
   FindClose(hFind);                  // close the file handle

   return RemoveDirectory(sPath);     // remove the empty directory
}

BOOL IsDots(const TCHAR* str) 
{
   if(_tcscmp(str,".") && _tcscmp(str,"..")) return FALSE;
   return TRUE;
}



