////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <simon.h>

// Windows Defender Helper Header
#include "WndDefenderHelperImpl.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ISVersion.h"
#include "ISCanIRun.h"
#include "cltProdInfoPEPConstants.h"
#include "ProdInfoNamedProperties.h"
#include "ProdInfoConstants.h"

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
#include "cfgWizCollectionHelper.h"

// InstallToolbox Headers
#include "PathValidation.h"
#include "InstOptsNames.h"
#include "OptHelper.h"

// Misc NAV Headers
#include "NAVSettingsHelperEx.h"
#include "build.h"
#include "NAVInfo.h"
#include "OSInfo.h"
#include "OptNames.h"
#include "prodconsts.h"
#include "NAVCfgWizDll_h.h"
#include "NAVError.h"
#include "AVccModuleID.h"
#include "DefAlertOptNames.h"
#include "ScanTaskLoader.h"
#include "isResource.h"
#include "symhelplauncher.h"    // for symhelp path
#include "NAVInstoptsNames.h"

#include "APSRTSPLoader.h"
#include "SrtExclusionsInterface.h" // AP Exclusions
#include <list>
#include "APExtWrapper.h"

// custom interfaces
#include "isDataNoUIInterface.h"

// Common Client Headers
#include "StartNAVcc.h"
#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"

// IWP headers
#include "NotifyIDS.h"
#include "ccSettingsManagerHelper.h"
#include "isNames.h"

// SymProtect helpers
#include "SymProtectControlHelperNAV.h"

// NIS Shared headers
#include "CLTProductHelper.h"

// IDS loader
#include "IDSLoader.h"

//UI
#include "uiPEPEventInterface.h"
#include "uiPEPEventLoader.h"

// CED errors
#include "isErrorIDs.h"
#include "isErrorResource.h"

#define __CFW_ICFMGR__
#include "IICFManager.h"
#include "FWLoaders.h"
#include "FWSettings.h"

// Elements
#include "ISDataClientLoader.h"
#include "uiNISDataElementGuids.h"
#include "uiProviderInterface.h"
#include "uiElementInterface.h"
#include "uiNumberDataInterface.h"
#include "isDataNoUIInterface.h"

// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

// TRACE implementation
ccSym::CDebugOutput g_DebugOutput(_T("NAVCfgWizRes"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

using namespace avScanTask;

const TCHAR NAV_MUTEX_NAME[]            = _T("NAVCFGWIZMutex");

// Please Wait dialog stuff
ccLib::CMutex g_DlgProcMutex;
HWND g_hWndPleaseWaitDlg = NULL;


////////////////////////////////////////////////////////////////
/////  CCfgWizHelper

HRESULT CCfgWizHelper::FinalConstruct()
{
	StahlSoft::HRX hrx;
	HRESULT hr = S_OK;

	try
	{
        m_LicenseType = clt::cltLicenseType_Violated;
        m_LicenseAttrib = clt::cltLicenseAttrib_Violated;   
	}
	catch( _com_error &e)
	{
		hr = e.Error();
	}
	catch(exception &ex)
	{
        CCTRCTXE1(_T("Caught unexpected exception: %s"), ex.what());
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

SYMRESULT CCfgWizHelper::InitIWPSettings()
{
	if (!m_pIWPSettings)
	{
		IWP::IIWPSettingsPtr pIWPSettings;
		SYMRESULT sr = m_IWPSettingsLoader.CreateObject(GETMODULEMGR(), pIWPSettings);
		if (SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to create IWPSettings"), sr);
		}

		IWP::IIWPSettings2QIPtr pIWPSettings2 = pIWPSettings;
		if (!pIWPSettings2)
		{
			CCTRCTXE0(_T("Failed to QI for IWPSettings2"));
			return SYMERR_UNKNOWN;
		}

		m_pIWPSettings = pIWPSettings2;
	}

	return SYM_OK;
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

		TCHAR szModulePath[MAX_PATH] = {0};

		StahlSoft::CCommandLine cmdLine;
		cmdLine.Parse(m_szCommandLine);

		CCTRACEI( _T("Getting rental mode") );
		if( cmdLine.HasSwitch( _T("/RM")) )
		{
			StahlSoft::StringType  szCmdLineArg;
			hr = cmdLine.GetArgument( _T("/RM"), 0, szCmdLineArg );

			if(SUCCEEDED(hr))
			{
				m_spColl->Item[clt::szRentalMode] = _bstr_t(szCmdLineArg.c_str());
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

		m_csProductName = CISVersion::GetProductName();

		CCTRACEI(_T("ProductName = %s"), m_csProductName);

		BOOL bRunCondition = FALSE;

		
        {   // Incident 626395,626384: 
			// Scope CfgWizDat so that it is released before
			// the preblock thread got created because
			// the thread also creates another CfgWizDat object.

			CCfgWizDat CfgWizDat(m_szCommandLine);
			if (!CfgWizDat.Load())
				CCTRCTXE0(_T("Failed to load CfgWizDat"));

			bRunCondition = ((RunConditions(CfgWizDat) == S_OK) ? TRUE : FALSE);
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

            if(CfgWizDat.IsSilentMode())
            {
                m_spColl->Item[CfgWiz::SILENT_MODE] = TRUE;
                g_hWndPleaseWaitDlg = NULL;
            }
            else
            {
                // Start up progress window worker thread
                m_spColl->Item[CfgWiz::SILENT_MODE] = FALSE;
            }

			szScratch = _T("res://");           
			if (GetModuleFileName(g_hModule, szModulePath, MAX_PATH) == 0)
			{
				CCTRCTXE1(_T("GetModuleFileName error: %08X"), GetLastError());
				return E_FAIL;
			}
			szScratch += szModulePath;

			// If silent mode or retail layout, skip the welcome page.
			if( (CfgWizDat.IsSilentMode() == TRUE) || (CfgWizDat.InstallType() == 0) )
			{
				// Show cfgwiz without security panel in the road map
				szScratch += "/home_NoTasks.htm";
			}
			else
			{
				szScratch += "/welcome.htm";
			}

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
		m_spColl->Item[clt::szSuiteOwner] = V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT; 

        ccLib::CStringW cswWindowTitle = L"Symantec";    // Just in case
        cswWindowTitle.LoadString(IDS_WINDOW_TITLE);
        szScratch = cswWindowTitle;
		m_spColl->Item[CfgWiz::WW_TITLE] = szScratch;
        
		// start the preblock() running in parallel
		StartPreBlockThread();

		CComPtr<INAVCCfgWizSubMgr> spNAVCfgWiz;

		if(SUCCEEDED(spNAVCfgWiz.CoCreateInstance(CLSID_NAVCfgWizMgr)))
        {
            IDispatchPtr disp;
			hrx << spNAVCfgWiz->QueryInterface(IID_IDispatch, (void**) &disp);

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

HRESULT CCfgWizHelper::RunConditions(CCfgWizDat& CfgWizDat)
{
	if(NeedReboot())
	{
		// Pop up message to tell user to reboot
		CString csMsg;
		csMsg.Format(IDS_REBOOT_MSG, m_csProductName);
		MessageBox ( NULL, csMsg, m_csProductName, MB_ICONEXCLAMATION );
		CCTRACEE(_T("CCfgWizHelper::RunConditions - Cfgwiz exits. System needs to reboot."));
		return E_FAIL;
	}

    // Read CfgWiz.dat

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
            CCTRCTXI0 (_T("Starting CC"));
            NAVToolbox::CStartNAVcc StartCC;
            StartCC.StartCC(true);
		}
	}

	// Intantiate the FRAT Class

	CFRAT FRAT;
	FRAT.Initialize();

	if((!FRAT.IsAuditMode()) && (CfgWizDat.CheckDriverManualStart() || CfgWizDat.CheckNoDrivers()))
	{
		// Call IIWPSettings2::ConfigureIWP()
		SYMRESULT sr = InitIWPSettings();
		if (SYM_SUCCEEDED(sr))
		{
			DWORD dwFlags = IWP::IIWPSettings2::ConfigureIWP_SetOEMState;
			sr = m_pIWPSettings->ConfigureIWP(dwFlags, NULL);
			if (SYM_FAILED(sr))
				CCTRCTXE2(_T("Failed to configure IWP with tasks [%lu]: %08X"), dwFlags, sr);
		}
		else
			CCTRCTXE1(_T("Failed to init IWP"), sr);
	}

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
#ifdef _DEBUG
		if(evt.Lock(INFINITE, FALSE))
#else
		if(evt.Lock(180000, FALSE))
#endif
        {
            evt.Unlock(); // unlock it and set it for cfgwizdll who's also lookin for it.
            evt.SetEvent();
            return S_OK;
        }
        else
        {
            CCTRCTXE0 (_T("Preblock thread timed out."));
            return E_FAIL;
        }
    }

    return E_FAIL;
}

HRESULT CCfgWizHelper::_CfgWizPreBlock()
{
	try
	{
		
		CRegKey key;
		CString csKey;
		StahlSoft::HRX hrx;

        CCTRCTXI0 (_T("Querying license info"));
        
        CComBSTR bstrErrorClass ("NAVError.NAVCOMError");
        CComPtr <INAVCOMError> spError;
        spError.CoCreateInstance( bstrErrorClass, NULL, CLSCTX_INPROC_SERVER);

        CCLTProductHelper cltHelper;
        hrx << cltHelper.Initialize();
		
        // Getting licensing properties
        hrx << cltHelper.GetLicenseAttributes(m_LicenseAttrib);
        hrx << cltHelper.GetLicenseType(m_LicenseType);

		CCTRACEI(_T("Start Synchronizing collections"));
		PEP_COLLECTION::ICollection* pCollection;
		cltHelper.GetCollection(pCollection);
		hrx << SynchronizeCollections(pCollection, m_spColl, _T(""));
		CCTRACEI(_T("Synchronizing collections Succeeded"));

		// Vendor ID and TAG for cipher registration with LiveReg
		CString csVendorID;
		CString csVendorTag; 
		CString szWebRegServerUrl;
		CString szWebRegServerWebUrl;
		CString szGlobalRedirectorUrl;
        CString szDRMUpgradeIDs;

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

			wsprintf(szCfgWizDat, _T("%s\\%s"), NAVInfo.GetNAVDir(), g_cszCfgWizDat);

			if(CfgWizDatFile.Init(szCfgWizDat, FALSE))
			{
                CStringA csVendorIdTemp;
                CStringA csVendorTagTemp;
				CStringA sWRServerURLTemp;
				CStringA sWRServerWebURLTemp;
				CStringA szGlobalRedirectorUrlTemp;
                CStringA csaUpgradeIDs;

				// Attempt to get the VendorID and VendorTag from cfgwiz.dat to register
				// with LS later provided they exist

				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_CipherID, csVendorIdTemp.GetBuffer(MAX_PATH), MAX_PATH, "");
                csVendorIdTemp.ReleaseBuffer();
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_CipherTag, csVendorTagTemp.GetBuffer(MAX_PATH), MAX_PATH, "");
                csVendorTagTemp.ReleaseBuffer();
                csVendorID = csVendorIdTemp;
                csVendorTag = csVendorTagTemp;

				CfgWizDatFile.GetValue("WebRegURL", sWRServerURLTemp.GetBuffer(MAX_PATH), MAX_PATH, "");
				sWRServerURLTemp.ReleaseBuffer();
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_RegistrationServerWebURL, sWRServerWebURLTemp.GetBuffer(MAX_PATH), MAX_PATH, "");
				sWRServerWebURLTemp.ReleaseBuffer();
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_GRBaseURL, szGlobalRedirectorUrlTemp.GetBuffer(MAX_PATH), MAX_PATH, "");
				szGlobalRedirectorUrlTemp.ReleaseBuffer();

                CfgWizDatFile.GetValue(AntiVirus::CFGWIZ_DRM_UPGRADE_PRODUCT_IDS, csaUpgradeIDs.GetBuffer(MAX_PATH), MAX_PATH, "");
                csaUpgradeIDs.ReleaseBuffer();

				szWebRegServerUrl = sWRServerURLTemp;
				szWebRegServerWebUrl = sWRServerWebURLTemp;
				szGlobalRedirectorUrl = szGlobalRedirectorUrlTemp;
                szDRMUpgradeIDs = csaUpgradeIDs;
				
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwOEM, 0);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_EnableRegistration, dwRegisterProduct, -1); 

				// Get upsell URL flags from dat file.
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellBasics, dwShowUpsellBasics, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellEmailMsg, dwShowUpsellEmail, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellWebBrowse, dwShowUpsellWebBrowse, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellDataRecovery, dwShowUpsellRecovery, -1);
				CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_ShowUpsellPerformance, dwShowUpsellPerform, -1);

				CCTRCTXI2 (_T("CfgWizPreBlock: szVendorID: %s, szVendorTag: %s"), csVendorID, csVendorTag);
			}
			else
			{
				CCTRCTXE1 (_T("CfgWizPreBlock: Failed to open %s."), szCfgWizDat);
			}
		} /*END OF SCOPE CNAVOptFileEx*/

		// Nag and bail out only if License is violated.
        if((m_LicenseAttrib & clt::cltLicenseAttrib_Violated) == clt::cltLicenseAttrib_Violated)
		{
			CCTRCTXE0 (_T("CCfgWizHelper::CfgWizPreBlock(): cfgwiz aborted. License is violated."));
            
            if(spError)
            {
                spError->put_ModuleID ( AV_MODULE_ID_INFO_WIZARD );
				spError->put_ErrorID ( ERR_AV_CFGWIZ_LICENSE_VIOLATED );
				spError->put_ErrorResourceID( IDS_ISSHARED_ERROR_INTERNAL_REINSTALL );
                spError->Show(TRUE, FALSE, NULL);
            }

            return E_FAIL;
		}

		// Set install time
		CCTRCTXI0(L"Setting install time.");
		if(SetInstallTime() != S_OK)
		{
			CCTRCTXE0(L"Error while setting install time.");
		}

		CNAVInfo NavInfo;

		if(m_dwAggregated == 0)
		{
            // Setup PROD info cache                                                
   			COptFile NAVOptsDat;
			TCHAR szLang[MAX_PATH] = _T("");
			TCHAR szSPEFSKU[MAX_PATH] = _T("");
			TCHAR szOPSKU[MAX_PATH] = _T("");
            TCHAR szPartnerName[MAX_PATH] = _T("");
            TCHAR szPartnerType[MAX_PATH] = _T("");
            TCHAR szEBEServerUrl[MAX_PATH] = _T("");
            TCHAR szEBEServerUrlAlt[MAX_PATH] = _T("");
			DWORD dwShowNortonAccount = 0;

			CNAVInfo NAVInfo;
			ccLib::CStringW sNavOptsDat;
			sNavOptsDat.Format(L"%s\\NavOpts.dat", NAVInfo.GetNAVDir());

			if(NAVOptsDat.Init(sNavOptsDat))
			{
				NAVOptsDat.GetValue(_T("SKU:PLN"), szLang, MAX_PATH, _T("EN"));
				NAVOptsDat.GetValue(_T("SKU:SPEFSKU"), szSPEFSKU, MAX_PATH, _T(""));
				NAVOptsDat.GetValue(_T("SKU:OPSKU"), szOPSKU, MAX_PATH, _T(""));
                NAVOptsDat.GetValue(_T("SKU:PARTNERNAME"), szPartnerName, MAX_PATH, _T(""));
                NAVOptsDat.GetValue(_T("SKU:PARTNERTYPE"), szPartnerType, MAX_PATH, _T(""));
                NAVOptsDat.GetValue(_T("SKU:EBESERVERURL"), szEBEServerUrl, MAX_PATH, _T(""));
                NAVOptsDat.GetValue(_T("SKU:EBESERVERURLALT"), szEBEServerUrlAlt, MAX_PATH, _T(""));
                

				switch(m_LicenseType)
				{
				case clt::cltLicenseType_Beta:
				case clt::cltLicenseType_Violated:
				case clt::cltLicenseType_SOS:
				case clt::cltLicenseType_TryDie:
					// Don't show Norton account
					CCTRCTXI1(L"Do not display Norton Account link for license type: %d", m_LicenseType);
					break;

				case clt::cltLicenseType_Activation:
				case clt::cltLicenseType_TryBuy:
				case clt::cltLicenseType_ESD:
				case clt::cltLicenseType_Unlicensed:
				case clt::cltLicenseType_OEM:
				default:
					NAVOptsDat.GetValue(HELPMENU_ShowProductReg, dwShowNortonAccount, 1); 
					break;
				}


				// set product name, version, language, copyright, and SKU information
                PEP_COLLECTION::CPEPCollectionHelper pepCol;
				pepCol.InitCollection();
                hrx << pepCol.SetStringValue(PRODINFO::szPIProductName, m_csProductName);
                hrx << pepCol.SetStringValue(PRODINFO::szPIDisplayProduct, 
                                m_csProductName);
				hrx << pepCol.SetStringValue(PRODINFO::szPIProductVersion, 
                                _T(VER_STR_PRODVERMAJOR) _T(".") 
                                _T(VER_STR_PRODVERMINOR) _T(".") 
                                _T(VER_STR_PRODVERSUBMINOR));
				hrx << pepCol.SetStringValue(PRODINFO::szPIDisplayVersion, 
								_T(VER_STR_PRODVERMAJOR) _T(".") 
								_T(VER_STR_PRODVERMINOR) _T(".") 
								_T(VER_STR_PRODVERSUBMINOR) _T(".")
								_T(VER_STR_BUILDNUMBER));
				hrx << pepCol.SetStringValue(PRODINFO::szPICUWProductGUID,szISCUWGuid);
				hrx << pepCol.SetStringValue(PRODINFO::szPIProductLanguage, 
                                szLang);
				hrx << pepCol.SetStringValue(PRODINFO::szPISymSKUFamily, 
                                szSPEFSKU);
				hrx << pepCol.SetStringValue(PRODINFO::szPISymSKUMedia, 
                                szOPSKU);
                hrx << pepCol.SetStringValue(PRODINFO::szPIRegistrationServerUrl,
                                szWebRegServerUrl);
				hrx << pepCol.SetStringValue(PRODINFO::szPIRegistrationServerWebUrl,
					szWebRegServerWebUrl);
				hrx << pepCol.SetStringValue(PRODINFO::szGRBaseURL,szGlobalRedirectorUrl);
				hrx << pepCol.SetDWORDValue(PRODINFO::szShowLinkAccount, dwShowNortonAccount);
                hrx << pepCol.SetStringValue(PRODINFO::szCSVProductIDs, szDRMUpgradeIDs);

				// Set copyright text
				ccLib::CString sCopyRight;
				if(CISVersion::LoadString(sCopyRight, IDS_SYMANTEC_COPYRIGHT))
				{
					if(sCopyRight.GetLength())
					{
                        hrx <<pepCol.SetStringValue(PRODINFO::szPICopyright,sCopyRight);            
					}
				}
                
				// Set the EULA text
				ccLib::CString sEulaFile;
				if(sEulaFile.LoadString(IDS_EULA_NAME))
				{
					if(sEulaFile.GetLength())
					{
						ccLib::CStringW sNISCfgWizResPath;
						sNISCfgWizResPath.Format(L"%s\\CfgWzRes.dll", NAVInfo.GetNAVDir());

						hrx << pepCol.SetStringValue(PRODINFO::szPIEulaResourcePathAndFilename, sNISCfgWizResPath);
						hrx << pepCol.SetStringValue(PRODINFO::szPIEulaHtml, sEulaFile);
					}
				}

                // set Partnername, PartnerType, EBEServerURL 
                hrx << pepCol.SetStringValue(PRODINFO::szPIPartnerType, 
                                szPartnerType);
				hrx << pepCol.SetStringValue(PRODINFO::szPIPartnerName, 
                                szPartnerName);
				hrx << pepCol.SetStringValue(PRODINFO::szPIEBEServerUrl, 
                                szEBEServerUrl);
				hrx << pepCol.SetStringValue(PRODINFO::szPIEBEServerUrlAlt, 
                                szEBEServerUrlAlt);


				// 2006 requirements: Must register vendor ID and tag for all layouts.
				// These info are used in Upsell URL that appears in the console 
				hrx << pepCol.SetDWORDValue(PRODINFO::SZ_PRODINFO_DISPOSITION, 
                                PRODINFO::DISPOSITION_OEM_PRODUCT);
				hrx << pepCol.SetStringValue(PRODINFO::szPIVendorID, csVendorID);
				hrx << pepCol.SetStringValue(PRODINFO::szPIVendorTag, csVendorTag);

				// set whether or not we can register
				if(dwRegisterProduct != -1)
				{
                    if (clt::cltLicenseType_SOS == m_LicenseType)
					{
						dwRegisterProduct = 0;
					}
					hrx << pepCol.SetDWORDValue(PRODINFO::szPISupportsRegistration,
                                dwRegisterProduct);     
				}

                // Set the license type so that DRM knows when to show the EULA
                hrx << pepCol.SetDWORDValue(PRODINFO::szPIINSTOPTSLicType, dwOEM);

                // Set the SymHelp path.
                ccLib::CString csHelpPath;
                isshared::CHelpLauncher celp;
                if(celp.GetPathToHelpFile(csHelpPath))
                {
                    hrx << pepCol.SetStringValue(PRODINFO::szPIHelpFilename, csHelpPath.GetString());
                }

                hrx << cltHelper.SetProdInfo(pepCol);
			}


			//Initilize the License
			hrx << cltHelper.InitLicense(szOPSKU,szLang);

            if((m_LicenseType != clt::cltLicenseType_SOS) && 
                (m_LicenseType != clt::cltLicenseType_TryDie))
			{
				CCTRACEI(CCTRCTX _T("Register subscription begins"));
                hrx << cltHelper.RegisterSubscription();
				CCTRACEI(CCTRCTX _T("Register subscription ends"));
			}
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
            ATL::CStringW strAppPath;

            UINT iSize = MAX_PATH;

            // Get the Windows\System32 path
            //
            SHGetFolderPathW ( ::GetDesktopWindow(),
                               CSIDL_SYSTEM,
                               NULL,
                               0,
                               strWindowsSystem.GetBuffer(iSize));
            strWindowsSystem.ReleaseBuffer();

            strAppPath = strWindowsSystem;
            PathAppendW(strAppPath.GetBuffer(iSize), L"CSRSS.EXE");
            strAppPath.ReleaseBuffer();

            if(SymProtect.AddNotificationFilter(strAppPath))
                CCTRCTXI0 (_T("AddNotificationFilter (CSRSS.EXE) - succeeded"));
            else
                CCTRCTXE0 (_T("AddNotificationFilter (CSRSS.EXE) - failed"));

            strAppPath = strWindowsSystem;
            PathAppendW(strAppPath.GetBuffer(iSize), L"lsass.exe");
            strAppPath.ReleaseBuffer();

            if(SymProtect.AddNotificationFilter(strAppPath))
                CCTRCTXI0 (_T("AddNotificationFilter (lsass.exe) - succeeded"));
            else
                CCTRCTXE0 (_T("AddNotificationFilter (lsass.exe) - failed"));

            strAppPath = strWindowsSystem;
            PathAppendW(strAppPath.GetBuffer(iSize), L"winlogon.exe");
            strAppPath.ReleaseBuffer();

            if(SymProtect.AddNotificationFilter(strAppPath))
                CCTRCTXI0 (_T("AddNotificationFilter (winlogon.exe) - succeeded"));
            else
                CCTRCTXE0 (_T("AddNotificationFilter (winlogon.exe) - failed"));
        }

		// Install the NAV AP Defaults
		if(FAILED(SetAutoProtectDefaults()))
		{
			CCTRCTXW0(_T("Failed to configure AutoProtect defaults."));
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
		CCLTProductHelper cltHelper;
		CCfgWizDat CfgWizDat(m_szCommandLine);
		CfgWizDat.Load();

#ifdef _DEBUG
		MessageBox(NULL, L"postblock begin", L"attach to me", MB_OK);
#endif

		if (CfgWizDat.IsFinished() == TRUE)
		{
			// Schedule FSS
			BOOL bScheduleWeekly = CfgWizDat.CheckScheduleWeeklyScan();
			ScheduleFullSystemScan(bScheduleWeekly);

            // sync up with our backgroundtasks thread
            ccLib::CEvent evt;
		    if(evt.Open(EVENT_ALL_ACCESS, FALSE, _T("NAV_CFGWIZ_BACKGROUNDTAKS_DONE"), FALSE))
		    {
			    if(evt.Lock(60000, FALSE))
                {
                    CCTRACEI(_T("Sync'd up with back ground thread all OK."));
                }
			    else
				    CCTRACEE(_T("Sync'd up with back ground thread timed out after 1 minute."));
		    }
		    else
			    CCTRACEE(_T("Failed to open background tasks event!"));

			//Do not need to do some configurations if we are a child
			if(m_dwAggregated == 0)
			{
				STAHLSOFT_HRX_TRY(hr)
				{
					hrx << cltHelper.Initialize();
					// tell Norton Security Console we've been configured
					{
						PEP_COLLECTION::CPEPCollectionHelper pepCol;
						hrx << pepCol.InitCollection();
						hrx << pepCol.SetDWORDValue(PRODINFO::SZ_PRODINFO_DISPOSITION
							, PRODINFO::DISPOSITION_NO_PRODUCT_VALIDATION);
						hrx << pepCol.SetDWORDValue(PRODINFO::szPIProductConfigured, 1);
						hrx << cltHelper.SetProdInfo(pepCol);
					}
				}
				STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

				//Send settings to  COnsole via Post Install event

				// Read UI Options, and sent a post install pep call.
				try
				{

					BOOL bShowWSC,bNSCSystray,bHideWUAlerts,bHideIEAlerts,bHideUACAlerts;
					BOOL bShowSecurityLink,bShowConnectingLink, bShowOnlineLink, bShowMediaLink, bShowAccountLink, bShowTechSupportLink;

					bShowWSC = CfgWizDat.GetShowWSC();
					bNSCSystray = CfgWizDat.GetNSCSysTray();
					bHideWUAlerts = CfgWizDat.GetHideWUAlerts();
					bHideIEAlerts = CfgWizDat.GetHideIEAlerts();
					bHideUACAlerts = CfgWizDat.GetHideUACAlerts();
					CCTRACEI(CCTRCTX _T("%d - %d - %d - %d - %d"),bShowWSC,bNSCSystray,bHideWUAlerts,bHideIEAlerts,bHideUACAlerts);

					bShowSecurityLink = CfgWizDat.GetShowSecurityLink();
					bShowConnectingLink = CfgWizDat.GetShowConnectingLink();
					bShowOnlineLink = CfgWizDat.GetShowOnlineLink();
					bShowMediaLink = CfgWizDat.GetShowMediaLink();
					bShowAccountLink = CfgWizDat.GetShowAccountLink();
					bShowTechSupportLink = CfgWizDat.GetShowTechSupportLink();

					CCTRACEI(CCTRCTX _T("%d - %d - %d - %d - %d - %d"),
						bShowSecurityLink,bShowConnectingLink,bShowOnlineLink,bShowMediaLink,
						bShowAccountLink,bShowTechSupportLink);

                    //Fill the collection with UI Options   
					PEP_COLLECTION::CPEPCollectionHelper ColHelper;
					hrx <<ColHelper.InitCollection();

					//ui option
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszShowWSC,bShowWSC);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszNSCSystray,bNSCSystray);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszHideNPCAUAlerts,bHideWUAlerts);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszHideNPCIEAlerts,bHideIEAlerts);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszHideNPCUACAlerts,bHideUACAlerts);

					//Links
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszShowSecurityLink,bShowSecurityLink);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszShowConnectingLink,bShowConnectingLink);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszShowOnlineLink,bShowOnlineLink);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszShowMediaLink,bShowMediaLink);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszShowAccountLink,bShowAccountLink);
					hrx << ColHelper.SetDWORDValue(uiPep2Defines::wszShowTechSupportLink,bShowTechSupportLink);


					CStringA sWRServerURLTemp;
					CStringA sWRServerWebURLTemp;
					CStringA szGlobalRedirectorUrlTemp;
					CString szWebRegServerUrl;
					CString szWebRegServerWebUrl;
					CString szGlobalRedirectorUrl;


					CNAVInfo NAVInfo;
					TCHAR szCfgWizDat[MAX_PATH] = {0};
					CNAVOptFileEx CfgWizDatFile;

					wsprintf(szCfgWizDat, _T("%s\\%s"), NAVInfo.GetNAVDir(), g_cszCfgWizDat);

					if(CfgWizDatFile.Init(szCfgWizDat, FALSE))
					{
						CfgWizDatFile.GetValue("WebRegURL", sWRServerURLTemp.GetBuffer(MAX_PATH), MAX_PATH, "");
						sWRServerURLTemp.ReleaseBuffer();
						CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_RegistrationServerWebURL, sWRServerWebURLTemp.GetBuffer(MAX_PATH),
							MAX_PATH, "");
						sWRServerWebURLTemp.ReleaseBuffer();
						CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_GRBaseURL, szGlobalRedirectorUrlTemp.GetBuffer(MAX_PATH), 
							MAX_PATH, "");
						szGlobalRedirectorUrlTemp.ReleaseBuffer();

						szWebRegServerUrl = sWRServerURLTemp;
						szWebRegServerWebUrl = sWRServerWebURLTemp;
						szGlobalRedirectorUrl = szGlobalRedirectorUrlTemp;
					}

					//Add Web Reg Server URL
					hrx << ColHelper.SetStringValue(PRODINFO::szPIRegistrationServerUrl, (LPCTSTR)szWebRegServerUrl );
					hrx << ColHelper.SetStringValue(PRODINFO::szPIRegistrationServerWebUrl, (LPCTSTR)szWebRegServerWebUrl );
					hrx << ColHelper.SetStringValue(PRODINFO::szGRBaseURL, (LPCTSTR)szGlobalRedirectorUrl );

					CCLTProductHelper CLTProductHelper;
					CLTProductHelper.Initialize();
					hrx << CLTProductHelper.FirePostInstallEvent(ColHelper);
				}
				STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
				if(FAILED(hr))
				{
					CCTRACEE(CCTRCTX _T("Error in FirePostInstallEvent. HR = 0x%x"),hr);
				}

				//Sent a event to UI that confgiwiz is finished
				pep::IPEPEventSourcePtr spSource;
				SYMRESULT sr = pep::uiPEPMgd_IPEPEventSourceLoader::CreateObject(GETMODULEMGR(), &spSource);
				if(SYM_SUCCEEDED(sr) && spSource)
				{
					hr = spSource->SetEndPointGuid(guidISBusinessTierEndPoint);
					if(SUCCEEDED(hr))
					{
						hr = spSource->SetEventType(pep::eConfigurationChange);
						if(SUCCEEDED(hr))
						{
							hr = spSource->SendEvent();
							if(SUCCEEDED(hr))
								CCTRACEI(CCTRCTX _T("Event sent to UI signaling Configuration is done"));
							else
								CCTRACEE(CCTRCTX _T("Error in SendEvent - HR = 0x%x"),hr);
						}
						else
							CCTRACEE(CCTRCTX _T("Error in SetEventType - HR = 0x%x"),hr);
					}
					else
						CCTRACEE(CCTRCTX _T("Error in SetEndPointGuid - HR = 0x%x"),hr);
				}
				else
					CCTRACEE(_T("Failed to create PEPEventSourcePtr SR= 0x%x"),sr);


				// Activate Subscription
                hr = S_OK;  

				// If users have not purchase, we will not activate

				m_LicenseType = static_cast<DWORD>(static_cast<long>(m_spColl->Item[clt::szLicenseType]));
				m_LicenseAttrib = static_cast<DWORD>(static_cast<long>(m_spColl->Item[clt::szLicenseAttributes]));

				// Turn IWP on/off according to CfgWiz.dat

				// Create CfgWiz.dat path
				CNAVInfo NAVInfo;
				TCHAR szCfgWizFile[MAX_PATH] = {0};
				CCTRACEI(_T("CCfgWizHelper::CfgWizPostBlock - Formatting CfgWiz.dat directory string."));
				wsprintf(szCfgWizFile, _T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());

				// Load CfgWiz.dat
				HNAVOPTS32 hCfgWiz = NULL;
				NAVOPTS32_STATUS status = NAVOPTS32_OK;
				status = NavOpts32_Allocate(&hCfgWiz);

				if(status != NAVOPTS32_OK)
				{
					throw exception("Unable to allocate NavOpts32.");
				}

				CCTRACEI(_T("CCfgWizHelper::CfgWizPostBlock - Loading CfgWiz.dat."));

#ifdef _UNICODE
				status = NavOpts32_LoadU(szCfgWizFile, hCfgWiz, TRUE);
#else
				status = NavOpts32_Load(szCfgWizFile, hCfgWiz, TRUE);
#endif

				if(status != NAVOPTS32_OK)
				{
					throw exception("Unable to load CfgWiz.dat.");
				}


				// Read CFGWIZ:DefaultValueOfDisableWDCheckBox from CfgWiz.dat
				DWORD dwDisableWDChecked = 1;
				NavOpts32_GetDwordValue(hCfgWiz, InstallToolBox::CFGWIZ_DisableWDChecked, &dwDisableWDChecked, 1);
				
				HRESULT hRet;
				bool bWDStatus = !((bool)dwDisableWDChecked);
				bool bIsInstalled=false;
				CWndDefenderHelper myHelper;
				if(SUCCEEDED(myHelper.IsInstalled(bIsInstalled)) && bIsInstalled)
					hRet = myHelper.SetState(bWDStatus);

				// Read INSTOPTS:EnableIWP from CfgWiz.dat
				DWORD dwEnableIWP = 0;
				NavOpts32_GetDwordValue(hCfgWiz, InstallToolBox::CFGWIZ_EnableIWP, &dwEnableIWP, 1);

				// Set Port Blocking flags
				sr = SetPortBlockingFlags();
				if (SYM_FAILED(sr))
					CCTRCTXE1(_T("SetPortBlockingFlags returned: %08X"), hr);

				if (dwEnableIWP == 0)
				{
					// disable IWP
					hr = SetIWPEnableStatus(FALSE, CfgWizDat.InstallType());
				}
				else
				{
					// enable IWP
					hr = SetIWPEnableStatus(TRUE, CfgWizDat.InstallType());

					//We enabled our Firewall, now we can disable windows firewall
					DWORD dwDisableICF = 1;
					NavOpts32_GetDwordValue(hCfgWiz, InstallToolBox::CFGWIZ_TurnOffICF, &dwDisableICF, 1);
					if(dwDisableICF)
						DisableWindowsFirewall();
					else
						CCTRACEI(CCTRCTX _T("Not disabling Windows Firewall. dwDisableICF = %d"),dwDisableICF);
				}
				if (FAILED(hr))
					CCTRCTXE1(_T("EnableIWP failed to complete all of its tasks: %08X"), hr);

				// Notify IDS of the subscription date
				try
				{
					DATE dtSubEnd;
					ULONG ulSubDaysRemaining;
					
					// Get licensing crap
					hrx << cltHelper.Initialize();
					hrx << cltHelper.GetDaysRemaining(ulSubDaysRemaining);
					
					// Get the current time.
					COleDateTime oleCurrentTime(COleDateTime::GetCurrentTime());

                    //  Figure out the expired date/time and convert it to DATE.
					dtSubEnd = (DATE) oleCurrentTime + ulSubDaysRemaining;

					// Send to IDS
					hrx << NotifyIDS(true,true,dtSubEnd,true);
				}
				STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

				// Enable IDS.
				sr = SYMERR_UNKNOWN;
				CIDSLoader IDSLoader;
				CSymPtr< SymIDS::IIDSGlobalSettings > pIDSGlobalSettings;
				sr = IDSLoader.GetObject(SymIDS::IID_IDSGlobalSettings, (ISymBase **)&pIDSGlobalSettings);
				CCTRACEI(_T("GetObject returned SYMRESULT: 0x%08X"), sr);
				if (SYM_SUCCEEDED(sr))
				{
					sr = pIDSGlobalSettings->SetSetting(SymIDS::IIDSGlobalSettings::SETTING_ID_ENABLED,(DWORD)TRUE);
					CCTRACEI(_T("SetSetting returned SYMRESULT: 0x%08X"), sr);
				}

				//Enable/Disable SymProtect

				DWORD dwEnableSymProtect = 1;
				NavOpts32_GetDwordValue(hCfgWiz, InstallToolBox::CFGWIZ_SymProtect_Enabled, &dwEnableSymProtect, 1);

				bool bStartSymProtect = (dwEnableSymProtect == 1 || dwEnableSymProtect == 0) ? true : false;

				CSymProtectControlHelper SymProtectCtrlHelper;
				if(SymProtectCtrlHelper.Create())
				{
					if( !SymProtectCtrlHelper.SetBBSystemEnabled(bStartSymProtect) )
					{
						CCTRACEE (_T("CfgWizPostBlock::SetBBSystemEnabled() - Failed to %s SymProtect"), bStartSymProtect ? _T("Enable"):_T("Disable"));
					}
				}
				else
				{
					CCTRACEE(_T("CfgWizPostBlock::SymProtectCtrlHelper.Create() Failed"));
				}
			}

            // Set the IS_CAN_SHOW_UI so the ISDataCl clients (IElements)
            // get a real-time update that CfgWiz is done.
            // The IS_PRODUCT_STATE is set by the firewall/IWP stuff.
            //
	        // do not change to ISettingsManager4 or above, we need the eventing!
	        ccSettings::CSettingsManagerHelper ldrSetMgr;
	        ccSettings::ISettingsManagerPtr pSetMgr;
	        SYMRESULT sr = ldrSetMgr.Create(pSetMgr);
            DWORD dwEnable = 1; // done
	        if (SYM_FAILED(sr) || pSetMgr == NULL)
	        {
		        CCTRCTXE1(_T("Failed to create setmgr, ec=%08x"), sr);
	        }
	        else
	        {
		        ccSettings::ISettingsPtr pSettings;
		        sr = pSetMgr->GetSettings(IS_GLOBAL_SETTINGS_KEY, &pSettings);
		        if (SYM_FAILED(sr))
		        {
			        CCTRCTXE2(_T("failed to get settings from %s:%08x"), IS_GLOBAL_SETTINGS_KEY, sr);
		        }
		        else
		        {
			        sr = pSettings->PutDword(IS_CAN_SHOW_UI, dwEnable);
			        if (SYM_FAILED(sr))
			        {
				        CCTRCTXE2(_T("failed to put dword %s:%08x"), IS_CAN_SHOW_UI, sr);
			        }
			        else
			        {
				        sr = pSetMgr->PutSettings(pSettings);
				        if (SYM_FAILED(sr))
				        {
					        CCTRCTXE1(_T("failed to commit UI settings, ec=%08x"), sr);
				        }
			        }
		        }
	        }

            // Turn ON ALU ?
            //
            // Read INSTOPTS:CFGWIZ_EnableAutoLiveUpdate from CfgWiz.dat
            CCTRCTXI0(_T("ALU enable - start"));
            ui::IProviderPtr pProvider;

            //ALU
            ui::IElementPtr pElementALU;
            ISShared::INumberDataNoUIQIPtr pDataALU;

            ISShared::ISShared_IProvider providerFactory;

            sr = providerFactory.CreateObject(GETMODULEMGR(), &pProvider);
            if(SYM_FAILED(sr))
            {
                CCTRCTXE1 (_T("Failed to create provider = 0%x80"), sr);
            }
            else
            {
                // ALU
                HRESULT hr = pProvider->GetElement ( ISShared::CLSID_NIS_ALU, pElementALU);
                if ( SUCCEEDED (hr))
                {
                    ui::IDataPtr pData;

                    hr = pElementALU->GetData (pData);
                    if ( SUCCEEDED (hr))
                    {
                        pDataALU = pData;
                        LONGLONG i64State = CfgWizDat.GetEnableALU() ? ISShared::i64StateOn : ISShared::i64StateOff;
                        
                        if ( pDataALU )
                        {
                            hr = pDataALU->SetNumberNoUI ( i64State );

                            if (SUCCEEDED (hr))
                            {
                                CCTRCTXI0(_T("ALU on"));
                            }
                            else
                                CCTRCTXE1 (_T("Failed to set number = 0%x80"), hr);
                        }
                        else
                            CCTRCTXE0 (_T("Failed to QI no ui"));
                    }
                    else
                        CCTRCTXE1 (_T("Failed to getdata = 0%x80"), hr);
                }
                else
                    CCTRCTXE1 (_T("Failed to get element = 0%x80"), hr);
            }
            CCTRCTXI0(_T("ALU enable - end"));
	
            // Remove CfgWiz from the RUN key - if the post tasks fail, we don't CARE.
            HKEY hKey;
            LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_cszMSRunKey, NULL, KEY_WRITE, &hKey);

            if (lResult == ERROR_SUCCESS)
            {
                lResult = RegDeleteValue(hKey, g_cszCfgWizRunKey);
                CCTRCTXI0 (_T( "Removed cfgwiz from Run key" ));
            }
            else
                CCTRCTXI0(_T("Failed to remove Cfgwiz from the Run key."));
  
            RegCloseKey(hKey);

			CPostCfgWizTasks CfgWizTasks;
            HRESULT hr;
            if(FAILED(hr = CfgWizTasks.ConfigureAVComponentDefaults()))
            {
                CCTRCTXE1(_T("ConfigureAVComponentDefaults failed with HR = 0x%X"), hr);
            }

			if(FAILED(hr = CfgWizTasks.RunTasks()))
            {
                CCTRCTXE1 (_T( "RunTasks() failed with HR = 0x%X"), hr);
            }
		}
	}
	catch(exception &ex)
	{
		ATL::CString strException (ex.what());
		CCTRCTXE1(_T("%s"),strException);
	}
	catch(_com_error &e)
	{
        CCTRCTXE1(_T("Caught COM Exception in PostBlock: %s"), e.ErrorMessage());
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

	return bNeedReboot;
}

void CCfgWizHelper::StartAP()
{
    AVModule::IAVAutoProtectPtr spAP;
	AVModule::AVLoader_IAVAutoProtect APLoader;

	SYMRESULT sr = APLoader.CreateObject(&spAP);
	if(SYM_FAILED(sr) || !spAP.m_p )
	{
		CCTRCTXE1(_T("Failed to create AVComponent AP object. SR = 0x%08x"), sr);
		return;
	}
	else
	{
		HRESULT hr = spAP->EnableAP(true);
		if ( SUCCEEDED (hr))
		{
			CCTRACEI(_T("AP enabled"));
		}
		else
		{
			CCTRCTXE1(_T("Failed to enable AP 0x%x"), hr);
		}
	}
	spAP.Release();
	return;
}

HRESULT CCfgWizHelper::SetIWPEnableStatus(BOOL enable, const DWORD dwOEM)
{
	CCTRCTXI0(_T("Enter"));
	HRESULT hr = S_OK;
	SYMRESULT sr = SYM_OK;

	// do not change to ISettingsManager4 or above, we need the eventing!
	ccSettings::CSettingsManagerHelper ldrSetMgr;
	ccSettings::ISettingsManagerPtr pSetMgr;
	sr = ldrSetMgr.Create(pSetMgr);
	if (SYM_FAILED(sr) || pSetMgr == NULL)
	{
		CCTRCTXE1(_T("Failed to create setmgr, ec=%08x"), sr);
		hr = E_FAIL;
	}
	else
	{
		ccSettings::ISettingsPtr pSettings;
		sr = pSetMgr->GetSettings(IS_GLOBAL_SETTINGS_KEY, &pSettings);
		if (SYM_FAILED(sr))
		{
			CCTRCTXE2(_T("failed to get settings from %s:%08x"), IS_GLOBAL_SETTINGS_KEY, sr);
			hr = E_FAIL;
		}
		else
		{
			sr = pSettings->PutDword(IS_FIREWALL_STATE, (DWORD)enable);

			CProductCanIHelper CanIRunHelper;
			DWORD dwCanIRun = CanIRunHelper.CanIRun();
			if(!enable) dwCanIRun = 0;
			SYMRESULT sr1 = pSettings->PutDword(IS_PRODUCT_STATE, dwCanIRun);

			if (SYM_FAILED(sr) || SYM_FAILED(sr1))
			{
				CCTRCTXE4(_T("failed to put dword %s:%08x OR dword %s:%08x"), IS_FIREWALL_STATE, sr, IS_PRODUCT_STATE, sr1);
				hr = E_FAIL;
			}
			else
			{
				sr = pSetMgr->PutSettings(pSettings);
				if (SYM_FAILED(sr))
				{
					CCTRCTXE1(_T("failed to commit IWP settings, ec=%08x"), sr);
					hr = E_FAIL;
				}
			}
            pSettings = NULL;
		}
	
        // ====================================================================
        // For OEM layout, fwAgent initialization was dealyed by SymSetup
        // ====================================================================
        if(0 != dwOEM)
        {
		    sr = pSetMgr->CreateSettings(FW_SETTINGS_KEY, &pSettings);
            if(SYM_SUCCEEDED(sr))
            {
                sr = pSettings->PutDword(FW_OEM_RESEAL_SETTINGS, 0);
                if(SYM_SUCCEEDED(sr))
                {
				    sr = pSetMgr->PutSettings(pSettings);
                    if(SYM_FAILED(sr))
			            CCTRACEE(_T("failed to PutSettings %s\\%s (0x%x)"), FW_SETTINGS_KEY, FW_OEM_RESEAL_SETTINGS, sr);
                }
                else
                {
			        CCTRACEE(_T("failed to PutDword %s (0x%x)"), FW_OEM_RESEAL_SETTINGS, sr);
                }
                pSettings = NULL;
            }
            else
            {
			    CCTRACEE(_T("failed to create %s (0x%x)"), FW_SETTINGS_KEY, sr);
            }
		}
	}

	CCTRCTXI0(_T("Exit"));
	return hr;
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
	catch(exception &ex)
	{
		hr = E_FAIL;
        CCTRCTXE1(_T("Caught unknown error in DoPreBlock: %s"), ex.what());
	}
	
    m_hPreblockEvent.SetEvent();
	
    return SUCCEEDED(hr) ? NO_ERROR : -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//  SetInstallTime(): Record InstallTime to alert.dat
// 
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CCfgWizHelper::SetInstallTime()
{
#ifdef _DEBUG
    MessageBox(NULL, _T("Attach to this MessageBox to Debug SetInstallTime()"), _T("Debug Break"), NULL);
#endif

    CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Starting."));

    HNAVOPTS32 hDefAlert = NULL;

    try
    {
        // Get full path of alert.dat

        TCHAR szAlertFile[MAX_PATH] = {0};
        CNAVInfo NAVInfo;

        CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Formatting alert.dat directory string."));
        wsprintf(szAlertFile, _T("%s\\alert.dat"), NAVInfo.GetNAVCommonDir());

        CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Allocating status variable."));
        NAVOPTS32_STATUS status = NAVOPTS32_OK;
        status = NavOpts32_Allocate(&hDefAlert);

        if(status != NAVOPTS32_OK)
            throw exception("Unable to allocate NavOpts32.");

        CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Loading alert.dat."));

#ifdef _UNICODE
        status = NavOpts32_LoadU(szAlertFile, hDefAlert, TRUE);
#else
        status = NavOpts32_Load(szAlertFile, hDefAlert, TRUE);
#endif

        if(status != NAVOPTS32_OK)
            throw exception("Unable to load alert.dat.");

        long iTime = 0;
        DWORD dwValue = 0;

        CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Checking for existing timestamp."));

        NavOpts32_GetDwordValue(hDefAlert, ALERT_InstallTime, &dwValue, 0);

        if(dwValue == 0)
        {
            CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Setting timestamp."));

            time(&iTime);
            dwValue = static_cast<DWORD>(iTime);
            NavOpts32_SetDwordValue(hDefAlert, ALERT_InstallTime, dwValue);

            CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Saving changes."));

#ifdef _UNICODE
            NavOpts32_SaveU(szAlertFile, hDefAlert);            
#else
            NavOpts32_Save(szAlertFile, hDefAlert);         
#endif
        }
    }
    catch(exception& ex)
    {
        CCTRACEE(_T("CCfgWizHelper::SetInstallTime - %s"), ex.what());
    }

    try
    {
        CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Freeing NavOpts."));

        if(hDefAlert)
            NavOpts32_Free(hDefAlert);
    }
    catch(exception &e)
    {
        CCTRACEE(_T("CCfgWizHelper::SetInstallTime - Free NavOpts failed: %s"), e.what());
    }

    CCTRACEI(_T("CCfgWizHelper::SetInstallTime - Finished."));

    return S_OK;
}

HRESULT CCfgWizHelper::ScheduleFullSystemScan(BOOL bScheduleWeekly)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		try
		{
			// Fix a scheduler bug related to Wise/Microsoft (Defect# 362816)

			Scheduler_Loader SchedulerLoader;
			ISchedulerPtr spScheduler;

			hrx << (SYM_SUCCEEDED(SchedulerLoader.CreateObject(&spScheduler))? S_OK : E_FAIL);

			hrx << spScheduler->SetRunning(false);
			hrx << spScheduler->SetRunning(true);
		}
		catch(_com_error &)
		{
			// Don't show an error display here. There's
			// nothing the user can do anyway and it's not
			// a critical error.
		}
        catch(exception &)
        {
            // see above
        }

		ScanTaskCollection_Loader loader;
		IScanTaskCollectionPtr spScanTaskColl;
		IScanTaskPtr spScanTask;

		CCTRCTXI0(L"Create ScanTaskCollection object");

		// Create scan task collection object
		hrx << (SYM_SUCCEEDED(loader.CreateObject(&spScanTaskColl))? S_OK : E_FAIL);

		// Get number of scan tasks available
		DWORD dwCount = 0;
		hrx << spScanTaskColl->GetCount(dwCount);
		if(!dwCount)
			return S_OK;

		CCTRCTXI1(L"Scan task count: %d", dwCount);

		// Walk through the list of scan tasks to find the Full System Scan
		for(DWORD i=0; i<dwCount; ++i)
		{
			// Get a scan task
			spScanTask = NULL;
			hrx << spScanTaskColl->GetItem(i, &spScanTask);
			if(!spScanTask.m_p)
				continue;

			// Get task type
			SCANTASKTYPE type = scanUnset;
			hrx << spScanTask->GetType(type);

			// Is this the FSS?
			if(scanMyComputer == type)
			{
				if(bScheduleWeekly)
				{
					CCTRCTXI0(L"Schedule FSS");

					// Schedule FSS
					hrx << spScanTask->ScheduleTask(true);
				}
				else
				{
					// Has FSS been scheduled?
					EScheduled eScheduled = EScheduled_na;
					hrx << spScanTask->GetScheduled(eScheduled);

					if(EScheduled_yes == eScheduled)
					{
						CCTRCTXI0(L"Delete schedule for FSS");

						// Delete the schedule for FSS
						hrx << spScanTask->DeleteSchedule();
					}
				}
				break;
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. HR:0x%08X", hr);
	return hr;
}


// Currently this is only the AP Exclusion Defaults
HRESULT CCfgWizHelper::SetAutoProtectDefaults()
{
	HRESULT hr;
	SYMRESULT sr;
	SRTSP::SRTSPLoader_ISrtExclusions APExclusionLoader;
	ISrtExclusionsPtr apExcl;
	CStringW cswDefaults;
	CStringW cswToken;
	DWORD dwLength = 1024;
	COptFile NAVOptsDat;
	TCHAR szDatFile[MAX_PATH] = _T("");
	CNAVInfo NAVInfo;
	int iToken = 0;
	LPCWSTR pszWildCard;
	CAPExtWrapper APExtensions;

	if(SYM_FAILED(sr = APExclusionLoader.CreateObject(&apExcl)))
	{
		CCTRCTXE1(_T("Error loading AP Exclusions interface. SR = 0x%08x"), sr);
		return E_FAIL;
	}

	if(FAILED(hr = apExcl->Load()))
	{
		CCTRCTXE1(_T("AP Exclusions failed to load. HR = 0x%08x"), hr);
		apExcl.Release();
		return hr;
	}

	wsprintf(szDatFile, _T("%s\\%s"), NAVInfo.GetNAVDir(), _T("NavOpts.dat"));
	if(!NAVOptsDat.Init(szDatFile))
	{
		CCTRCTXE1(_T("Failed to open datfile [%s]"), szDatFile);
		apExcl.Release();
		return E_FAIL;
	}

	// Load defaults from ccSettings
	if(FAILED(hr = NAVOptsDat.GetValue(_T("AUTOPROTECT:Exclusions"), cswDefaults.GetBuffer(dwLength), dwLength, _T(""))))
	{
		CCTRCTXE1(_T("Failed to get AP Exclusion defaults. Hr = 0x%08x"), hr);
		cswDefaults.ReleaseBuffer();
		apExcl.Release();
		return S_FALSE;
	}
	cswDefaults.ReleaseBuffer();

	iToken = 0;
	cswToken = cswDefaults.Tokenize(_T("|"), iToken);
	while(iToken > 0)
	{
		dwLength = cswToken.GetLength();

		pszWildCard = wcschr(cswToken, WCHAR('*'));
		if( (NULL != pszWildCard && (*CharNextW(pszWildCard)) == WCHAR('.')) )
		{
			// Extension Exclusion
			cswToken = cswToken.Right(cswToken.GetLength()-2); // Slash off '*.'
			if(FAILED(hr = APExtensions.Add(cswToken)))
			{
				CCTRCTXW2(_T("Error adding AP Extension Exclusion [%s]. Hr = 0x%08x"), cswToken, hr);
			}
		}
		else
		{
			if(FAILED(hr = apExcl->AddExclusion(cswToken, dwLength, SRT_EXCL_SUBFOLDERS)))
			{
				CCTRCTXW2(_T("Error saving AP Exclusion [%s]. Hr = 0x%08x"), cswToken, hr);
			}
		}

		cswToken = cswDefaults.Tokenize(_T("|"),iToken);
	}

	// Render AP Extension Exclusions from list
	size_t rlen = APExtensions.GetRenderLength();
	LPWSTR pszTemp = new WCHAR[rlen];
	if(!pszTemp)
	{
		CCTRCTXE0(_T("Could not allocate rendering string for AP Extension exclusions."));
		apExcl.Release();
		return E_OUTOFMEMORY;
	}

	if(FAILED(hr = APExtensions.Render(pszTemp, rlen)))
	{
		CCTRCTXE1(_T("Error rendering AP Extensions to string. Hr = 0x%08x"), hr);
		apExcl.Release();
		delete[] pszTemp;
		return hr;
	}

	if(FAILED(hr = apExcl->SetExtensions(pszTemp, rlen)))
	{
		CCTRCTXE1(_T("Error setting AP Extension exclusions. Hr = 0x%08x"), hr);
	}

	delete[] pszTemp;

	if(FAILED(hr = apExcl->Commit()))
	{
		CCTRCTXE1(_T("Failed to commit AP Exclusion Defaults. Hr = 0x%08x"), hr);
		apExcl.Release();
		return hr;
	}

	apExcl.Release();
	return S_OK;
}

SYMRESULT CCfgWizHelper::SetPortBlockingFlags()
{
	SYMRESULT sr = InitIWPSettings();
	if (SYM_FAILED(sr))
	{
		CCTRCTXE1(_T("Failed to init IWPSettings: %08X"), sr);
		return sr;
	}

	DWORD dwState;
	sr = m_pIWPSettings->GetValue(IWP::IIWPSettings::IWPState, dwState);
	if (SYM_FAILED(sr))
	{
		CCTRCTXE1(_T("Failed to get IWP Yield state: %08X"), sr);
		return sr;
	}

	// Don't set these flags if we are yielding
	if (dwState == IWP::IIWPSettings::IWPStateYielding)
	{
		CCTRCTXI0(_T("IWP is yielding.  Not setting port blocking flags."));
		return S_OK;
	}

	DWORD dwFlags = IWP::IIWPSettings2::ConfigureIWP_SetPortBlockingFlags;
	sr = m_pIWPSettings->ConfigureIWP(dwFlags, NULL);
	if (SYM_FAILED(sr))
	{
		CCTRCTXE1(_T("Failed to set port blocking flags: %08X"), sr);
		return sr;
	}

	return S_OK;
}

bool CCfgWizHelper::DisableWindowsFirewall()
{
	bool bRet = false;
	ICFManagerLoader icfMgrLoader;
	IICFManagerPtr pICFManager;
	if(SYM_SUCCEEDED(icfMgrLoader.CreateObject(&pICFManager)))
	{
		BOOL bEnabled = FALSE;
		if(SYM_SUCCEEDED(pICFManager->Init()))
		{
			if(SYM_SUCCEEDED(pICFManager->IsEnabled(bEnabled)))
			{
				if(bEnabled)
				{
					CCTRACEI(CCTRCTX _T("Disable XP ICF"));
					if(SYM_SUCCEEDED(pICFManager->StoreConnections()))
					{
						if(SYM_SUCCEEDED(pICFManager->DisableConnections()))
						{
							CCTRACEI(CCTRCTX _T("Successfully Disabled ICF"));
							bRet = true;
						}
						else
							CCTRACEE(CCTRCTX _T("Could not Disable ICF"));
					}
					else
					{
						CCTRACEE(CCTRCTX _T("ICFManager->StoreConnections()failed."));
					}
				}
			}
			else
			{
				CCTRACEI(CCTRCTX _T("ICF not enabled on any Connections"));
			}
		}
	}
	else
	{
		CCTRACEE(CCTRCTX _T("Cannot get the ICFManager Object"));
	}
	return bRet;
}