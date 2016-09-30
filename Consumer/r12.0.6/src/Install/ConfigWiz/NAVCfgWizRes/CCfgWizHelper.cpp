#include "stdafx.h"
#include <simon.h>

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "AVPEPComponent.h"
#include "cltPEPConstants.h"
#include "PEPClientBase.h"
#include "Subscription_Static.h"
#include "DRMNamedProperties.h"
#include "AVRESBranding.h"
#include "prodinfopephelper.h"
#include "ConsPEPDefines.h"

CComModule _Module;
#ifdef _MY_DEBUG
#define DEBUG_MESSAGE_BOX(msg) MessageBox(NULL, _T(msg), _T("Debug"), MB_OK);
#else
#define DEBUG_MESSAGE_BOX(msg)
#endif

// CfgWiz/Licensing Headers
#include "ICfgWizHelper.h"
#include "CCfgWizHelperImpl.h"
#include "ccfgwizhelper.h"
#include "cltExtendedLicenseHelper.h"
#include "cfgWizCollectionHelper.h"

// InstallToolbox Headers
#include "PathValidation.h"
#include "InstOptsNames.h"

// Misc NAV Headers
#include "NAVSettingsHelperEx.h"
#include "build.h"
#include "AVRES.h"
#include "NAVInfo.h"
#include "OSInfo.h"
#include "OptNames.h"
#include "prodconsts.h"
#include "NAVCfgWizDll_h.h"
#include "NAVError.h"
#include "NAVErrorResource.h"
#include "AVccModuleID.h"


// Headers for setting AP options
#include "NAVAPSCR.h"
#include "NAVAPSCR_i.c"
#include "Savrt32.h"
#include "apoptnames.h"

// Common Client Headers
#include "StartNAVcc.h"
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
#include "SymScriptSafe.h"
#include "SymScriptSafe_i.c"

// IWP and WSC headers
#include "IWPPrivateSettingsInterface.h"
#include "IWPPrivateSettingsLoader.h"
#include "ISWSCHelper_Loader.h"
#include "WSCHelperInterface.h"
#include "NAVICFManagerLoader.h"

// SymProtect helpers
#include "SymProtectControlHelperNAV.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

// TRACE implementation
ccSym::CDebugOutput g_DebugOutput(_T("NAVCfgWizRes"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

////////////////////////////////////////////////////////////////
/////  CCfgWizHelper

HRESULT CCfgWizHelper::FinalConstruct()
{
	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;

	try
	{
		m_LicenseType = DJSMAR_LicenseType_Violated;
		m_LicenseState = DJSMAR00_LicenseState_Violated;	
	}
	catch( _com_error &e)
	{
		hr = e.Error();
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
	}

	return hr;
};

void CCfgWizHelper::FinalRelease()
{
	g_Shutdown.Terminate();
}

CCfgWizHelper::CCfgWizHelper()
{
	m_pPreBlockThread = new CPreBlockThread();
	m_pPreBlockThread->Init(this);
}

CCfgWizHelper::~CCfgWizHelper()
{
	if(m_pPreBlockThread)
	{
		delete m_pPreBlockThread;
	}
}

HRESULT CCfgWizHelper::Initialize(IDispatch* pDispatch)
{
	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;
	DWORD dwSize = MAX_PATH;

	try
	{
		// Put Command Line in Global Collection	
		m_spColl = pDispatch; 	
		m_szCommandLine= m_spColl->Item[CfgWiz::COMMAND_LINE];

        //initialize this value, preblock thread will set to success if necessary.
        _bstr_t szScratch;
		szScratch = L"NIS_CFGWIZ_PREBLOCK_RESULT";
		m_spColl->Item[szScratch] = _variant_t(false);

 		CCfgWizDat CfgWizDat(m_szCommandLine);
		CfgWizDat.Load();

		TCHAR szModulePath[MAX_PATH];

		StahlSoft::CCommandLine cmdLine;
		cmdLine.Parse(m_szCommandLine);

		CCTRACEI( _T("Getting rental mode") );
		if( cmdLine.HasSwitch( _T("/RM")) )
		{
			StahlSoft::StringType  szCmdLineArg;
			hr = cmdLine.GetArgument( _T("/RM"), 0, szCmdLineArg );

			if(SUCCEEDED(hr))
			{
				m_spColl->Item[DRM::szDRMRentalMode] = _bstr_t(szCmdLineArg.c_str());
			}
		}

		// If system is in safe mode, exit cfgwiz because
		//  product cannot be activated and ccapp does not run anyway.
		int nRet = GetSystemMetrics(SM_CLEANBOOT);
		bool bSafeMode = (nRet != 0);
		if(bSafeMode)
		{
			CCTRACEE(_T("System is in safe mode. Exit"));
			return E_FAIL;
		}

		// Get product name from AVRES.
		CBrandingRes BrandRes;
		m_csProductName = BrandRes.ProductName();
		m_csProductNameWithVersion = BrandRes.ProductNameWithVersion();

		CCTRACEI(_T("ProductName = %s"), m_csProductName);
		CCTRACEI(_T("ProductNameWithVersion = %s"), m_csProductNameWithVersion);

		BOOL bRunCondition = ((RunConditions() == S_OK) ? TRUE : FALSE);
		if(bRunCondition)
		{
			// Set flag to indicate that cfgwiz will be launched.
			// This flag is used to signal ISLAlert that it can
			//  start nagging if needed.
			CfgWizDat.SetCfgwizRunOnce();
		}



		// If the finish flag is already set we will not display the webwindow.
		if (CfgWizDat.IsFinished() == TRUE)
		{
			m_spColl->Item[CfgWiz::SZ_WEBWND_DISABLED] = TRUE;
		}

		m_spColl->Item[CfgWiz::SILENT_MODE] = CfgWizDat.IsSilentMode();

		szScratch = _T("res://");           
		GetModuleFileName(g_hModule, szModulePath, MAX_PATH);
		szScratch += szModulePath;

		if ( (CfgWizDat.IsSilentMode() == TRUE) || (CfgWizDat.ShouldShowWelcomePage() == FALSE) )
		{
			// Check if we need to display ICF or SSC check boxes
			if(ShouldShowSSCCheckBox() || (CfgWizDat.ShouldShowICFCheckbox() && ShouldDisableICF()))
			{
				// Show cfgwiz with security panel in the road map
			szScratch += "/home.htm";
		}
		else
		{
				// Show cfgwiz without security panel in the road map
				szScratch += "/home_NoTasks.htm";
			}
		}
		else
		{
			szScratch += "/welcome.htm";
		}

		m_spColl->Item[CfgWiz::START_PAGE] = szScratch;


		szScratch = NAV_MUTEX_NAME;
		m_spColl->Item[CfgWiz::MUTEX_NAME] = szScratch;

		m_spColl->Item[CfgWiz::SZ_WW_WIDTH] = NAV_WW_WIDTH;
		m_spColl->Item[CfgWiz::SZ_WW_HEIGHT] = NAV_WW_HEIGHT;
		m_spColl->Item[CfgWiz::DLL_MODULE] = (DWORD) g_hModule;
		m_spColl->Item[CfgWiz::APP_ICON] = IDI_ICON1;
		m_spColl->Item[CfgWiz::RUN_CONDITIONS] = bRunCondition;

		szScratch = m_csProductName;
		m_spColl->Item[CfgWiz::PRODUCT_NAME] = szScratch;
		m_spColl->Item[CfgWiz::WW_TITLE] = szScratch;
		m_spColl->Item[DRM::szDRMSuiteOwner] = V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT; 
        
        // start the preblock() running in parallel
		StartPreBlockThread();

        CComPtr<INAVCCfgWizSubMgr> spNAVCfgWiz;

        if(SUCCEEDED(spNAVCfgWiz.CoCreateInstance(CLSID_NAVCfgWizMgr)))
        {
            //set security
		    CComPtr<ISymScriptSafe> spSS ;
		    if(!SUCCEEDED (spNAVCfgWiz->QueryInterface( IID_ISymScriptSafe, (void**) &spSS )) )
		    {
                hrx << E_FAIL;
		    }

		    if(!SUCCEEDED( spSS->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB)) )
		    {
                hrx << E_FAIL;
		    }
            
            IDispatchPtr disp;
            if(!SUCCEEDED(spNAVCfgWiz->QueryInterface(IID_IDispatch, (void**) &disp)))
            {
                hrx << E_FAIL;
            }

            if(disp)
			{
                _variant_t vparam((IDispatch*)disp);
				szScratch = L"NAV_CFGWIZ_MGR_OBJ";
				m_spColl->Item[szScratch] = vparam;
				CCTRACEI(_T("Cached NavCfgWizMgr in Initialize"));
			}
			else
				CCTRACEE(_T("Failed NAVCfgWizDLL.NAVCfgWizMgr is NULL in Initialize"));
		}
		else
			CCTRACEE(_T("Failed to create NavCfgWizMgr in Initialize"));
	}
	catch( _com_error &e)
	{
		hr = e.Error();
		CCTRACEE( _T("CCfgWizHelper::Initialize() - Exception %s"), e.ErrorMessage() );
	}

	return hr;
}

HRESULT CCfgWizHelper::RunConditions()
{
	if(NeedReboot())
	{
		// Pop up message to tell user to reboot
		CString csMsg;
		csMsg.Format(IDS_REBOOT_MSG, m_csProductNameWithVersion);
		MessageBox ( NULL, csMsg, m_csProductNameWithVersion, MB_ICONEXCLAMATION );
		CCTRACEE(_T("CCfgWizHelper::RunConditions - Cfgwiz exits. System needs to reboot."));
		return E_FAIL;
	}

    // Read CfgWiz.dat

	CCfgWizDat CfgWizDat(m_szCommandLine);
	CfgWizDat.Load();

    // If the no driver flag is set and user has not agreed to EULA,
	//  prompt the user if they would like to run CfgWiz
	if (!CfgWizDat.IsSilentMode() 
		&& (CfgWizDat.CheckNoDrivers() == TRUE) 
		&& CfgWizDat.ShouldShowCfgwizPrompt())
	{
		CString csMsg;
		CString csMsgFormat;
		csMsgFormat.LoadString(IDS_CFGWIZ_OEM_FIRSTRUN);
		csMsg.Format(csMsgFormat, m_csProductName);
		
		int nRetVal = MessageBox(NULL, csMsg, m_csProductName, MB_OKCANCEL);
		if(nRetVal == IDCANCEL)
		{
			// Exit cfgwiz.
			m_spColl->Item[CfgWiz::RUN_CONDITIONS] = FALSE;
            return E_FAIL;
		}
		else
		{
			// Set flags so that this messagebox won't pop up again.
			CfgWizDat.DisableCfgwizPrompt();

            // Start CommonClient services which are needed for cfgwiz to run and
            // set them to auto.
            CCTRACEI("Starting CC");
            NAVToolbox::CStartNAVcc StartCC;
            StartCC.StartCC(true);
		}
	}

	// Intantiate the FRAT Class

	CFRAT FRAT;
	FRAT.Initialize();

	// Find out if NAV is a child product
	READ_SETTING_START()
		READ_SETTING_DWORD(LICENSE_Aggregated, m_dwAggregated, 0)
	READ_SETTING_END

	// NOTE: some of these conditional expressions do have side-effects and we rely on short-circuiting here
	HKEY hKey;
	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVCfgWizRegKey, NULL, KEY_READ, &hKey);
	if (lResult == ERROR_SUCCESS)
		RegCloseKey(hKey);
	
	COSInfo OSInfo;
	if(!OSInfo.IsAdminProcess())
	{
		CString sTitle, sMessage, sMessageFormat;
		sMessageFormat.LoadString(IDS_ADMIN_RIGHTS);
		sMessage.Format(sMessageFormat, m_csProductName);

		sTitle.LoadString(IDS_ERROR_TITLE);
		MessageBox(NULL, sMessage, sTitle, MB_ICONWARNING | MB_OK);
		return E_FAIL; // bail out now.. dont decrement reboot count or anything
	}

	// if we're in audit/factory mode or still need a reseal then do nothing
	if(!(FRAT.IsAuditMode() || FRAT.IsFactoryMode()) &&
		(!CfgWizDat.CheckFactoryReseal() || FRAT.IsReseal()))
	{
		// check if AP should be started
		if(CfgWizDat.ShouldStartAP() == TRUE)
		{
			StartAP();
		}

		// check other run condicions if NAV is NOT a child product
		if ( ((1 == m_dwAggregated) || (CfgWizDat.CheckFactoryFile() && 
										CfgWizDat.CheckFactoryRegKey() && 
										CfgWizDat.CheckRebootCount())) ||
			 lResult == ERROR_SUCCESS )
		{
			// Check if user has already walked through CfgWiz           
			if ( (CfgWizDat.IsFinished() == FALSE) || (lResult == ERROR_SUCCESS) )
			{
				return S_OK;
			}		
		}
	}

	return E_FAIL;
}

void CCfgWizHelper::StartPreBlockThread()
{
    CCTRACEI(_T("Launching PREBLOCK Thread"));
    if(m_pPreBlockThread->Create(NULL, 0, 0))
	{
		m_pPreBlockThread->SetPriority(THREAD_PRIORITY_HIGHEST);

	}
}

SIMON_STDMETHODIMP CCfgWizHelper::CfgWizPreBlock()
{
    // synchronize CfgWiz pre blck thread here.  currently the multithreading
    // isnt working because cfgwiz looks in the collection for values that
    // are not set.  if ths is ever resolved then all you need to do to make
    // this multithreaded again is take this code out and just return S_OK.
    ccLib::CEvent evt;
	if(evt.Open(EVENT_ALL_ACCESS, FALSE, _T("NAV_CFGWIZ_PREBLOCK_DONE"), FALSE))
	{
		if(evt.Lock(100000, FALSE))
        {
            evt.Unlock(); // unlock it and set it for cfgwizdll who's also lookin for it.
            evt.SetEvent();
            return S_OK;
        }
        else
        {
            CCTRACEE("Preblock thread timed out.");
            return E_FAIL;
        }
    }
}

HRESULT CCfgWizHelper::_CfgWizPreBlock()
{
	try
	{
		CRegKey key;
		CString csKey;
		StahlSoft::HRX hrx;

        CCTRACEI("Querying license info");
        
        CComBSTR bstrErrorClass ("NAVError.NAVCOMError");
        CComPtr <INAVCOMError> spError;
        spError.CoCreateInstance( bstrErrorClass, NULL, CLSCTX_INPROC_SERVER);

        CPEPClientBase pepBase;
        hrx << pepBase.Init();
        pepBase.SetContextGuid(clt::pep::CLTGUID);
        pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
        pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
        pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_CALL_DIRECT);
        pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

        hrx << pepBase.QueryPolicy();
        
        //1-43HLSX : fix for ESD install requests a product key during config wiz.
		hrx << SynchronizeCollections( pepBase.GetCollectionPtr(), m_spColl, _T(""));

        //1-2XJYC3: We should never get an expired, because everything has a trial period.  If we are expired it may be because
        //          NAV made a call too early in the install when they should have never have called when they needed to reboot.
        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)m_LicenseState, DJSMAR00_LicenseState_Violated);
        if(m_LicenseState == DJSMAR00_LicenseState_EXPIRED)
        {
            // Reset the current collection allocated in the prvious macro
            hrx << pepBase.Reset();
            
            CCTRACEW("CfgWizPreBlock: Product state expired calling direct.");

            // Re-Initialize it to do a license record read
            pepBase.Init();
            pepBase.SetContextGuid(clt::pep::CLTGUID);
            pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
            pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
            pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION, clt::pep::DISPOSITION_CALL_DIRECT);
            pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);
          
            // Query License Record
            hrx << pepBase.QueryPolicy();
        }

        // Getting licensing properties
        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseState, (DWORD &)m_LicenseState, DJSMAR00_LicenseState_Violated);
        hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)m_LicenseType, DJSMAR_LicenseType_Violated);

		// Vendor ID and TAG for cipher registration with LiveReg
		TCHAR szVendorID[MAX_PATH] = {0};
		TCHAR szVendorTag[MAX_PATH] = {0};
		DWORD dwOEM = 0;
		DWORD dwShowUpsellBasics = -1;
		DWORD dwShowUpsellEmail = -1;
		DWORD dwShowUpsellWebBrowse = -1;
		DWORD dwShowUpsellRecovery = -1;
		DWORD dwShowUpsellPerform = -1;
		DWORD dwRegisterProduct = -1;
		{ 
			// Do not launch if CfgWizSuiteOEM value is set, if so run what is pass in.
			CRegKey key;
			TCHAR szCfgWizSuiteOEM[MAX_PATH] = {0};
			LONG rc = key.Open(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Symantec\\Norton AntiVirus"), KEY_READ );

			if(REG_SUCCEEDED(rc))
			{
				DWORD dwSize = MAX_PATH;		
				rc = key.QueryStringValue(_T("CfgWizSuiteOEM"), szCfgWizSuiteOEM, &dwSize);

				if(REG_SUCCEEDED(rc))
				{				
					STARTUPINFO si = {0};
					si.cb = sizeof( STARTUPINFO );
					PROCESS_INFORMATION pi = {0};

					CreateProcess(NULL, szCfgWizSuiteOEM, NULL,
						NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);

					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);

					CCTRACEE(_T("CmdLine: %s"), szCfgWizSuiteOEM);

					return E_FAIL;
				}
			}


			/*SCOPE CNAVOptFileEx*/
			// Write flag to cfgwiz.dat to show that CfgWiz is executed.
			//  DefAlert will display licensing alert only if
			//  Cfgwiz is executed at least once (Welcome page not neccessary comes up).

			CNAVInfo NAVInfo;
			TCHAR szCfgWizDat[MAX_PATH] = {0};
			CNAVOptFileEx CfgWizDatFile;
			DWORD dwAddRemove = 0;

			wsprintf(szCfgWizDat, _T("%s\\%s"), NAVInfo.GetNAVDir(), g_cszCfgWizDat);

			if(CfgWizDatFile.Init(szCfgWizDat, FALSE))
			{
				// Attempt to get the VendorID and VendorTag from cfgwiz.dat to register
				// with LS later provided they exist
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_CipherID, szVendorID, MAX_PATH, "");
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_CipherTag, szVendorTag, MAX_PATH, "");
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0);

				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_EnableRegistration, dwRegisterProduct, -1); 

				// Get upsell URL flags from dat file.
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellBasics, dwShowUpsellBasics, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellEmailMsg, dwShowUpsellEmail, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellWebBrowse, dwShowUpsellWebBrowse, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellDataRecovery, dwShowUpsellRecovery, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellPerformance, dwShowUpsellPerform, -1);

				CCTRACEI("CfgWizPreBlock: szVendorID: %s, szVendorTag: %s", szVendorID, szVendorTag);
				
				// YOP INSTALL : To check if we need to delete DisplayName from SymSetup and LU regkey
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_HideFromAddRemove, dwAddRemove, 0);
			}
			else
			{
				CCTRACEE("CfgWizPreBlock: Failed to open %s.", szCfgWizDat);
			}
			
			// *********** YOP INSTALL ****************
			// YOP INSTALL : We dont want NAV and LiveUpdate to Show up in Add/Remove programs.
			// Removing NAV and Liveupdate DisplayName from MS Uninstall reg key if CFGWIZ:HideFromAddRemove = 1
			if (dwAddRemove)
			{
				CCTRACEI("CfgWizPreBlock: YOP INSTALL TASKS BEGINS");
				
				// SYMSETUP KEY
				CString csSymSetupKey;
				csSymSetupKey.Format(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\SymSetup.{C6F5B6CF-609C-428E-876F-CA83176C021B}"));
				CRegKey SymSetupKey;
				if (ERROR_SUCCESS == SymSetupKey.Open(HKEY_LOCAL_MACHINE, csSymSetupKey))
				{
					SymSetupKey.DeleteValue(_T("DisplayName"));	
					SymSetupKey.Close();
					CCTRACEI("CfgWizPreBlock: DisplayName from SymSetup key deleted successfully.");
				}
				else
				{
					TCHAR messageText[MAX_PATH];
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, messageText, MAX_PATH, NULL);
					CCTRACEE ( "CfgWizPreBlock -- Unable to open SymSetup key. Error = %s", messageText );
				}

				// LIVEUPDATE KEY
				CString csLUKey;
				csLUKey.Format(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LiveUpdate"));
				CRegKey LUKey;
				if (ERROR_SUCCESS == LUKey.Open(HKEY_LOCAL_MACHINE, csLUKey))
				{
					LUKey.DeleteValue(_T("DisplayName"));	
					LUKey.Close();
					CCTRACEI("CfgWizPreBlock: DisplayName from LiveUpdate key deleted successfully.");
				}
				else
				{
					TCHAR messageText[MAX_PATH];
					FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, messageText, MAX_PATH, NULL);
					CCTRACEE ( "CfgWizPreBlock -- Unable to open LiveUpdate key. Error = %s", messageText );
				}
			}
			// ****************************************

		} /*END OF SCOPE CNAVOptFileEx*/

		// Nag and bail out only if License is violated.
		if(m_LicenseState == DJSMAR00_LicenseState_Violated)
		{
			CCTRACEE("CCfgWizHelper::CfgWizPreBlock(): cfgwiz aborted. License is violated.");
            
            if(spError)
            {
                spError->put_ModuleID ( AV_MODULE_ID_INFO_WIZARD );
	            spError->put_ErrorID ( ERR_VIOLATED );
                spError->put_ErrorResourceID( IDS_NAVERROR_INTERNAL_REINSTALL );
                spError->Show(TRUE, FALSE, NULL);
            }

            return E_FAIL;
		}

		CNAVInfo NavInfo;

		CCfgWizDat CfgWizDat(m_szCommandLine);
		CfgWizDat.Load();

		if(m_dwAggregated == 0)
		{
			// Setup PROD info cache
			CPEPClientBase pepBase;
			pepBase.Init();
			pepBase.SetContextGuid(PRODINFO::PRODINFOGUID);
			pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
			pepBase.SetPolicyID(PRODINFO::POLICY_ID_SET_PRODUCT_INFORMATION);
	                	                        
   			CNAVOptFileEx NAVOptsDat;
			TCHAR szLang[MAX_PATH] = {0};
			TCHAR szSPEFSKU[MAX_PATH] = {0};
			TCHAR szOPSKU[MAX_PATH] = {0};
            TCHAR szPartnerName[MAX_PATH] = {0};
            TCHAR szPartnerType[MAX_PATH] = {0};
            TCHAR szEBEServerUrl[MAX_PATH] = {0};
            TCHAR szEBEServerUrlAlt[MAX_PATH] = {0};
            TCHAR szWebRegServerUrl[MAX_PATH] = {0};


			if(NAVOptsDat.Init())
			{
				NAVOptsDat.GetValue(_T("SKU:PLN"), szLang, MAX_PATH, _T("\0"));
				NAVOptsDat.GetValue(LICENSE_FAMILY_SKU, szSPEFSKU, MAX_PATH, _T("\0"));
				NAVOptsDat.GetValue(LICENSE_MEDIA_SKU, szOPSKU, MAX_PATH, _T("\0"));
                NAVOptsDat.GetValue(_T("SKU:PARTNERNAME"), szPartnerName, MAX_PATH, _T("\0"));
                NAVOptsDat.GetValue(_T("SKU:PARTNERTYPE"), szPartnerType, MAX_PATH, _T("\0"));
                NAVOptsDat.GetValue(_T("SKU:EBESERVERURL"), szEBEServerUrl, MAX_PATH, _T("\0"));
                NAVOptsDat.GetValue(_T("SKU:EBESERVERURLALT"), szEBEServerUrlAlt, MAX_PATH, _T("\0"));
                NAVOptsDat.GetValue(_T("WebRegURL"), szWebRegServerUrl, MAX_PATH, _T("\0"));

				// set product name, version, language, copyright, and SKU information
				pepBase.SetCollectionString(PRODINFO::szPIProductName, m_csProductName);
                pepBase.SetCollectionString(PRODINFO::szPIDisplayProduct, m_csProductName);
				pepBase.SetCollectionString(PRODINFO::szPIProductVersion, VER_STR_PRODVERTUPLE);
				pepBase.SetCollectionString(PRODINFO::szPIProductLanguage, szLang);
				pepBase.SetCollectionString(PRODINFO::szPISymSKUFamily, szSPEFSKU);
				pepBase.SetCollectionString(PRODINFO::szPISymSKUMedia, szOPSKU);
                pepBase.SetCollectionString(PRODINFO::szPIRegistrationServerUrl, szWebRegServerUrl);
                
				// Display year only if non rental
				if(m_LicenseType != DJSMAR_LicenseType_Rental)
					pepBase.SetCollectionString(PRODINFO::szPIDisplayVersion, VER_STR_PRODVERYEAR);

                // set Partnername, PartnerType, EBEServerURL 
                pepBase.SetCollectionString(PRODINFO::szPIPartnerType, szPartnerType);
				pepBase.SetCollectionString(PRODINFO::szPIPartnerName, szPartnerName);
				pepBase.SetCollectionString(PRODINFO::szPIEBEServerUrl, szEBEServerUrl);
				pepBase.SetCollectionString(PRODINFO::szPIEBEServerUrlAlt, szEBEServerUrlAlt);


				// 2006 requirements: Must register vendor ID and tag for all layouts.
				// These info are used in Upsell URL that appears in the console 
				pepBase.SetCollectionDWORD(PRODINFO::SZ_PRODINFO_DISPOSITION, PRODINFO::DISPOSITION_OEM_PRODUCT);
				pepBase.SetCollectionString(PRODINFO::szPIVendorID, szVendorID);
				pepBase.SetCollectionString(PRODINFO::szPIVendorTag, szVendorTag);

				// set whether or not we can register
				if(dwRegisterProduct != -1)
				{
					if (DJSMAR_LicenseType_Rental == m_LicenseType)
					{
						dwRegisterProduct = 0;
					}
					pepBase.SetCollectionDWORD(PRODINFO::szPISupportsRegistration, dwRegisterProduct);		
				}

				pepBase.QueryPolicy();
			}

			if((m_LicenseType != DJSMAR_LicenseType_Rental) && (m_LicenseType != DJSMAR_LicenseType_TryDie))
			{
				CCTRACEI(CCTRCTX _T("Register subscription begins"));
				pepBase.SetContextGuid(clt::pep::CLTGUID);
				pepBase.SetPolicyID(clt::pep::POLICY_ID_REGISTER_SUBSCRIPTION);
				pepBase.QueryPolicy();
				CCTRACEI(CCTRCTX _T("Register subscription ends"));
			}

			pepBase.Reset();
			pepBase.SetContextGuid(PRODINFO::PRODINFOGUID);
			pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
			
            // Register upsell URL flag to PEP only if the flag exists in dat file.
			if(dwShowUpsellBasics != -1)
			{
				pepBase.SetCollectionDWORD(PRODINFO::szPIShowUrlUpsellBasics, dwShowUpsellBasics);
			}
			if(dwShowUpsellEmail != -1)
			{
				pepBase.SetCollectionDWORD(PRODINFO::szPIShowUrlUpsellEmailMessaging, dwShowUpsellEmail);
			}
			if(dwShowUpsellWebBrowse != -1)
			{
				pepBase.SetCollectionDWORD(PRODINFO::szPIShowUrlUpsellWebBrowsing, dwShowUpsellWebBrowse);
			}
			if(dwShowUpsellRecovery != -1)
			{
				pepBase.SetCollectionDWORD(PRODINFO::szPIShowUrlUpsellDataRecovery, dwShowUpsellRecovery);
			}
			if(dwShowUpsellPerform != -1)
			{
				pepBase.SetCollectionDWORD(PRODINFO::szPIShowUrlUpsellPerformance, dwShowUpsellPerform);
			}
            
            pepBase.SetPolicyID(PRODINFO::POLICY_ID_SET_URL_UPSELL_INFORMATION);
			pepBase.QueryPolicy();
        }

        // Filter out common processes from SymProtect.
        // Clients are responsible for this, and SymProtect is not running at install time
        // so I have to put the work here in config wiz.
        //
        CSymProtectControlHelperNAV SymProtect;

        if( !SymProtect.Create() )
        {
            CCTRACEE(_T("SetSymProtectFilters() - Failed to create() the symprotect control helper"));
        }
        else
        {
            ATL::CStringW strWindowsSystem;

            UINT iSize = MAX_PATH;

            // Get the Windows\System32 path
            //
            SHGetFolderPathW ( ::GetDesktopWindow(),
                               CSIDL_SYSTEM,
                               NULL,
                               0,
                               strWindowsSystem.GetBuffer(iSize));

            strWindowsSystem.ReleaseBuffer();

            DWORD dwSize = iSize + wcslen(L"CSRSS.EXE") + 1;
            PathAppendW(strWindowsSystem.GetBuffer(dwSize), L"CSRSS.EXE");
            
            SYMRESULT sr = SymProtect.AddNotificationFilter(strWindowsSystem);

            if ( SYM_SUCCEEDED (sr))
                CCTRACEI ("SetSymProtectFilters - succeeded");
            else
                CCTRACEE ("SetSymProtectFilters - failed - %d", sr);
        }
    }
	catch(_com_error& e)
	{
		CCTRACEE(_T("CCfgWizHelper::CfgWizPreBlock() - Exception %s"), e.ErrorMessage());
	}

	return S_OK;
}


SIMON_STDMETHODIMP CCfgWizHelper::CfgWizPostBlock()
{
	try
	{
		StahlSoft::HRX hrx;
		CCfgWizDat CfgWizDat(m_szCommandLine);
		CfgWizDat.Load();

		if (CfgWizDat.IsFinished() == TRUE)
		{
            // sync up with our backgroundtasks thread
            ccLib::CEvent evt;
		    if(evt.Open(EVENT_ALL_ACCESS, FALSE, _T("NAV_CFGWIZ_BACKGROUNDTAKS_DONE"), FALSE))
		    {
			    if(evt.Lock(60000, FALSE))
                {
                    CCTRACEE(_T("Sync'd up with back ground thread all OK."));
                }
			    else
				    CCTRACEE(_T("Sync'd up with back ground thread timed out after 1 minute."));
		    }
		    else
			    CCTRACEE(_T("Failed to open background tasks event!"));
            
            STAHLSOFT_HRX_TRY(hr)
            {
                // tell Norton Security Console we've been configured
                {
                    CPEPClientBase pepBase;
                    
                    hrx << pepBase.Init();
                    pepBase.SetContextGuid(PRODINFO::PRODINFOGUID);
                    pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
                    pepBase.SetPolicyID(PRODINFO::POLICY_ID_SET_PRODUCT_INFORMATION);
                    
                    pepBase.SetCollectionDWORD(PRODINFO::SZ_PRODINFO_DISPOSITION
                        , PRODINFO::DISPOSITION_NO_PRODUCT_VALIDATION);

                    hrx << pepBase.SetCollectionDWORD(PRODINFO::szPIProductConfigured, 1);

                    pepBase.QueryPolicy();
                }
                
                // notify console PEP handler that the configured flag has been set.
                // only do this if we're a suiteowner.
                if(!m_dwAggregated)
                {
                    // create and initialize PEP object
                    CPEPClientBase pepBase;
	                hrx << pepBase.Init();

	                pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_CONSOLE_COMPONENT);
                    pepBase.SetContextGuid(CONSOLE::CONSOLEGUID);
	                pepBase.SetPolicyID(CONSOLE::POLICY_ID_CONSOLE_NOTIFY_DONE);
                        
                    // since we're PEPing to NSC's handler, need an empty V2Map.
                    pepBase.SetEmptyV2Mode(true);
	                
                    // execute PEP call
	                hrx << pepBase.QueryPolicy();
                }
            }
            STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);


			// Activate Subscription
			hr = S_OK;	

			// If users have not purchase, we will not activate

			m_LicenseType = static_cast<DJSMAR_LicenseType>(static_cast<long>(m_spColl->Item[DRM::szDRMLicenseType]));
			m_LicenseState = static_cast<DJSMAR00_LicenseState>(static_cast<long>(m_spColl->Item[DRM::szDRMLicenseState]));

			try
			{
				StahlSoft::HRX hrxTmp;

				// If product has not been activated,
				//  set flag to suppress licensing baby nag 
				//  on first launch of main UI because we already
				//  nag on activation panel of cfgwiz.
				if(m_LicenseState != DJSMAR00_LicenseState_PURCHASED)
				{
					CNAVInfo NAVInfo;
					TCHAR szCfgWizDat[MAX_PATH] = {0};
					CNAVOptFileEx CfgWizDatFile;

					wsprintf(szCfgWizDat, _T("%s\\%s"), NAVInfo.GetNAVDir(), g_cszCfgWizDat);

					hrxTmp << (CfgWizDatFile.Init(szCfgWizDat, FALSE) ? S_OK : E_FAIL);
					hrxTmp << CfgWizDatFile.SetValue(InstallToolBox::CFGWIZ_NoNagOnFirstMainUI, 1);
					hrxTmp << CfgWizDatFile.Save();
				}
			}
			catch(...)
			{
				// ignore error.
			}

			CPostCfgWizTasks CfgWizTasks;

			HRESULT hResult = CfgWizTasks.RunTasks();

			if (SUCCEEDED(hResult))
			{
				// Remove CfgWiz from the RUN key only if the PostTasks key is gone.
				HKEY hKey;

				LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszNAVCfgWizRegKey, NULL, KEY_WRITE, &hKey);

				if (lResult != ERROR_SUCCESS)
				{
					lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszMSRunKey, NULL, KEY_WRITE, &hKey);

					if (lResult == ERROR_SUCCESS)
					{
						lResult = RegDeleteValue(hKey, g_cszCfgWizRunKey);
						CCTRACEI ( "Removed cfgwiz from Run key" );
					}
				}

				RegCloseKey(hKey);
			}
			else
			{
				CCTRACEE ( "RunTasks() failed" );
			}
		}
	}
	catch(exception &ex)
	{
		CCTRACEE(ex.what());		
	}
	catch(...)
	{
		CCTRACEE("Unknown Exception in PostBlock()");		
	}

	return S_OK;
}

// see if product or system needs a reboot
BOOL CCfgWizHelper::NeedReboot()
{
	CString sRebootFile;
	InstallToolBox::SymGetTempPath(MAX_PATH, sRebootFile.GetBuffer(MAX_PATH));
	sRebootFile.ReleaseBuffer();

	InstallToolBox::RemoveBackslash(sRebootFile.GetBuffer());
	sRebootFile.ReleaseBuffer();

	sRebootFile += _T("\\SymSetup.ini");	

	BOOL bNeedReboot = false;
	int nReboot = 0;		
	nReboot = GetPrivateProfileInt(_T("SymSetup"), _T("Product"), 0, sRebootFile);

	bNeedReboot = (0 != nReboot);

	nReboot = GetPrivateProfileInt(_T("SymSetup"), _T("System"), 0, sRebootFile);

	bNeedReboot = bNeedReboot || (0 != nReboot);

	return bNeedReboot;
}

void CCfgWizHelper::StartAP()
{
	//initialize COM
	if ( FAILED (CoInitialize(NULL)) )
	{
		return;
	}

	try
	{
		CComPtr<IScriptableAutoProtect> spAPScriptObject;
		//instantiate the AP com object
		if( FAILED (CoCreateInstance(CLSID_ScriptableAutoProtect, NULL, CLSCTX_INPROC_SERVER, 
			IID_IScriptableAutoProtect, (void**) &spAPScriptObject)) )
		{
            throw std::runtime_error("StartAP() - Could not get AP Object");
		}

		//set security
		CComPtr<ISymScriptSafe> spSS ;
		if(!SUCCEEDED (spAPScriptObject->QueryInterface( IID_ISymScriptSafe, (void**) &spSS )) )
		{
            throw std::runtime_error("StartAP() -- Could not get AP Scriptsafe interface");
		}

		if(!SUCCEEDED( spSS->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB)) )
		{
            throw std::runtime_error("StartAP() -- Could not SetAccess on AP Object");
		}

		//set AP to on and to start on reboot
		spAPScriptObject->Configure(true);
		spAPScriptObject->put_Enabled(true);

        // Set the STARTUP:LoadVxD option to on since we configured AP to load at startup
        SAVRT_OPTS_STATUS savrt_status = SAVRT_Options_Load();
        if( SAVRT_OPTS_OK == savrt_status )
        {
            savrt_status = SAVRT_Options_SetDwordValue(STARTUP_LoadVxD, 1);

            if( SAVRT_OPTS_OK == savrt_status )
            {
                savrt_status = SAVRT_Options_Save();

                if( savrt_status != SAVRT_OPTS_OK )
                    CCTRACEE("StartAP() -- Failed to save the savrt options with STARTUP:LoadVxD value = 1. Error = %d", savrt_status);
                else
                    CCTRACEI("StartAP() -- Successfully saved the savrt options with STARTUP:LoadVxD value = 1");
            }
            else
                CCTRACEE("StartAP() -- Failed to set the STARTUP:LoadVxD value to 1. Error = %d", savrt_status);
        }
        else
            CCTRACEE("StartAP() -- Failed to load savrt options to set the STARTUP:LoadVxD value. Error = %d", savrt_status);
	}
	catch(exception &ex)
	{
		CCTRACEE(ex.what());		
	}
	catch(...)
	{
		CCTRACEE("Unknown Exception in StartAP()");		
	}

	CoUninitialize();
}

BOOL CCfgWizHelper::ShouldShowSSCCheckBox()
{
	BOOL bShowSSC = FALSE;

	try
	{
		ISShared::WSCHelper_Loader WSCLdr;
		CSymPtr<IWSCHelper> WSCHelper;
		if(SYM_SUCCEEDED(WSCLdr.CreateObject(WSCHelper.m_p)) && WSCHelper)
		{
			// if they want us to show it AND it hasn't been shown yet
			if(WSCHelper->GetShowBroadcastCheckbox() && !WSCHelper->GetInitialBroadcastPrompt() && !WSCHelper->IsPreXPSP2())
			{
				bShowSSC = TRUE;
			}
		}
	}
	catch(...)
	{
		// Ignore error
	}

	CCTRACEI(CCTRCTX _T("Show SSC check box: %d"), bShowSSC);

	return bShowSSC;
}

BOOL CCfgWizHelper::ShouldDisableICF()
{
	BOOL bDisableICF = FALSE;

	try
	{
		// Check to see if ICF is enabled
		BOOL bICFEnabled = FALSE;
		NAVICFManagerLoader ICFLdr;
		IICFManagerPtr ICFMgr;

		if(SYM_SUCCEEDED(ICFLdr.CreateObject(ICFMgr.m_p)) && ICFMgr.m_p)
		{
			if(SYM_SUCCEEDED(ICFMgr->Init()))
			{
				if(SYM_SUCCEEDED(ICFMgr->IsEnabled(bICFEnabled)))
				{
					CCTRACEI(CCTRCTX _T("ICFEnabled: %d"), bICFEnabled);

					// Check to see if IWP installed
					if(bICFEnabled)
					{
						IWP::IIWPPrivateSettingsPtr spIWPPrivateSettings;
						IWP::IIWPSettingsPtr spIWPSettings;

						// load IWPSettings object
						if(SYM_SUCCEEDED(IWP_IWPSettings::CreateObject(GETMODULEMGR(), spIWPSettings.m_p)) && (spIWPSettings != NULL))
						{
							CCTRACEI(CCTRCTX _T("IWPInstalled: TRUE"));
							bDisableICF = TRUE;
						}
						else
						{
							CCTRACEI(CCTRCTX _T("IWPInstalled: FALSE"));
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		// Ignore error
	}

	CCTRACEI(CCTRCTX _T("DisableICF: %d"), bDisableICF);

	return bDisableICF;
}

////////////////////////////////////////////////////////////////
/////  CPreBlockThread

CPreBlockThread::Run()
{
    int result = NO_ERROR;

	HRESULT hr = S_OK;
	try
	{
		HRESULT hr = m_pCfgWiz->_CfgWizPreBlock();
		_bstr_t bValueName (L"NAV_CFGWIZ_PREBLOCK_RESULT");
		_variant_t vResult (hr == S_OK ? true : false);

        m_pCfgWiz->m_spColl->Item[bValueName] = vResult;
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRACEE(_T("Caught COM Error 0x%08x (%s) in DoPreBlock"), e.Error(), e.ErrorMessage());
	}
	catch(...)
	{
		hr = E_FAIL;
		CCTRACEE(_T("Caught unknown error in DoPreBlock"));
	}
	
    m_hPreblockEvent.SetEvent();
	
    return SUCCEEDED(hr) ? NO_ERROR : -1;
}
