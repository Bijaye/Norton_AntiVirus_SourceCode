// CfgWizMgr.cpp : Implementation of CNAVCfgWizMgr
#include "stdafx.h"
#include <ATLComTime.h>
#include <simon.h>

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "AVPEPComponent.h"
#include "cltPEPConstants.h"
#include "Subscription_Static.h"
#include "DRMNamedProperties.h"

#include "ccverifytrustinterface.h"
#include "NAVError.h"       // For errors
#include "NAVTrust.h"       // For trusting the NAVError object
#include "ccSettingsInterface.h"
#include "IICFManager.h"
#include "NAVICFManagerLoader.h"
#include "ccServicesConfigInterface.h"
#include "ccInstLoader.h"
#include "IWPPrivateSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "IWPSettingsLoader.h"
#include "IWPPrivateSettingsLoader.h"
#include "ISWSCHelper_Loader.h"
#include "WSCHelperInterface.h"
#include "SymIDSI.h"
#include "IDSLoader.h"
#include "CommonStructs.h"
#include "Utilities.h"
#include "NAVDetection.h"
#include "AVccModuleId.h"

using namespace InstallToolBox;

//#define _INIT_COSVERSIONINFO

#include "NAVCfgWizDll_h.h"
#include "NAVCfgWizMgr.h"
#include "DefAlertOptNames.h"
#include "InstOptsNames.h"
#include "AVRES.h"
#include "SymNetDriverAPI.h"

#include "SimonCollectionImpl.h"
#include "NAVSettingsHelperEx.h"

#include "NavUIHelp.h"

#include "NAVOptHelperEx.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "NAVErrorResource.h"
#include "SettingsNames.h"	// The LiveUpdate Settings Properties' Names
#import "LUCOM.tlb"         // The LuComServer COM Interfaces
#include "LUCOM.h"			// The LuComServer Smart Pointers

#include "GlobalEvents.h"   // For IMSCAN change event

#pragma warning(disable : 4996)
/////////////////////////////////////////////////////////////////////////////
// CNAVCfgWizMgr 

CNAVCfgWizMgr::CNAVCfgWizMgr()
{
    m_pBackGroundTasksThread = new CBackGroundTasks();
	m_pBackGroundTasksThread->Init(this);
}

HRESULT CNAVCfgWizMgr::FinalConstruct()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		m_spLog = new CDebugLog("NAVCfgWizDll.log");

		m_bInitSubscription = true;

		CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

		// Check NAVError module for Symantec Signature...
		if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(_T("NAVError.NAVCOMError")) )
		{
			if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
				NULL,
				CLSCTX_INPROC_SERVER)))
			{
			}
		}

		m_Properties = new CNAVProperties( GetProductDir());
		InitSecurityObject(this);

		m_dwSubDaysRemaining = 0;
		m_dwSubBeginDate = 0;
		m_dwSubEndDate = 0;

		// Getting licensing and subscription properties
		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
		pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

		hrx << pepBase.QueryPolicy();

		// Getting licensing properties
		hrx << pepBase.GetValueFromCollection(DRM::szDRMLicenseType, (DWORD &)m_licenseType, DJSMAR_LicenseType_Violated);
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		// Must check for null pointer of m_spLog since the call to "new" to create log object may fail.
		if(m_spLog)
			m_spLog->Log("Error in FinalConstruct 0x%08X", e.Error());
	}
	catch(...)
	{
		hr = E_UNEXPECTED;
		// Must check for null pointer of m_spLog since the call to "new" to create log object may fail.
		if(m_spLog)
			m_spLog->Log("Unexpected Error in FinalConstruct");
	}

	return hr;
}

HRESULT CNAVCfgWizMgr::ValidateFunctionSafety(int iResID)
{
	BOOL bCondition = FALSE;
	HRESULT hr = S_OK;

	switch( iResID )
	{
	case  ERR_SECURITY_FAILED:

	case ERR_FAILED_OPEN_TASK_LIST:

	case ERR_SUBSCRIPTION_LENGTH:

	case ERR_SUBSCRIPTION_END:

	case ERR_SUBSCRIPTION_START:

	case ERR_FAILED_ADD_LU:

	case ERR_FAILED_ADD_TASK:

	case ERR_FAILED_FINISH:
		bCondition = !IsItSafe();
		hr = E_ACCESSDENIED;
		break;

	case ERR_FAILED_LAUNCH_UI:

	case ERR_FAILED_ADD_FSS:
		bCondition = !IsItSafe();
		hr = E_FAIL;
		break;

	default:
		bCondition = FALSE;
		hr = E_FAIL;

	}

	if (forceError (iResID) || bCondition)
	{
		// Determine which generic error message we want to display
		LONG lNAVErrorID = IDS_NAVERROR_INTERNAL;

		if(iResID == ERR_SECURITY_FAILED)
		{
			lNAVErrorID = IDS_NAVERROR_INTERNAL_REINSTALL;
		}

		makeGenericError ( iResID, hr, lNAVErrorID );
		return hr;
	}

	return S_OK;
}

const TCHAR* CNAVCfgWizMgr::GetProductDir()
{
	return( m_NAVInfo.GetNAVDir());
}

STDMETHODIMP CNAVCfgWizMgr::LaunchUI()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_FAILED_LAUNCH_UI);

	// Determine the path of Main UI
	TCHAR szPluginPath[MAX_PATH] = {0};

	TCHAR szNisPlugParam[MAX_PATH*2] = {0};
	DWORD dwSize = sizeof(szNisPlugParam) / sizeof(TCHAR);

	if(SUCCEEDED(NAVUIHelp::BuildNisPluginParams(szNisPlugParam, dwSize)))
	{
		// Launch NIS UI with NAV Status panel active.
        // NOTE: Due to defect 1-3MLT47, we MUST enclose the paths to executables
        // in quotes, or we open ourselves up to naming vulnerabilities.
        wsprintf(szPluginPath, "\"%s\\NMain.exe\" %s", m_NAVInfo.GetSymantecCommonDir(), szNisPlugParam);
	}
	else
	{
		CNAVInfo NAVInfo;
		CString csTitle;
		CString csStatusPanel;
		CString csAVRES;

		// Now tell NMain to give focus to the NAV panel, since
		// Norton Protection Center is the default home

		// Setup path to AVRES.DLL
		csAVRES.Format(_T("%s\\AVRES.DLL"), NAVInfo.GetNAVDir());

		StahlSoft::CSmartModuleHandle shMod(LoadLibrary(csAVRES));
		hrx << (((HANDLE) shMod != (HANDLE)NULL) ? S_OK : CO_E_DLLNOTFOUND);

		// Get names of NAV main UI title and status panel.
		csTitle.LoadString(shMod, IDS_PRODUCTNAME);
		csStatusPanel.LoadString(shMod, IDS_STATUS_CATEGORYTITLE);

		csTitle.Remove(_T('&'));
		csStatusPanel.Remove(_T('&'));

		if(m_licenseType == DJSMAR_LicenseType_Rental)
			wsprintf(szPluginPath, "\"%s\\NMain.exe\" /dat:%s\\navui.nsi /nosysworks /goto:%s\\%s", m_NAVInfo.GetSymantecCommonDir(), m_NAVInfo.GetNAVDir(), csTitle.GetBuffer(), csStatusPanel.GetBuffer());
		else
            wsprintf(szPluginPath, "\"%s\\NMain.exe\" /dat:%s\\navui.nsi /goto:%s\\%s", m_NAVInfo.GetSymantecCommonDir(), m_NAVInfo.GetNAVDir(), csTitle.GetBuffer(), csStatusPanel.GetBuffer());

		csTitle.ReleaseBuffer();
		csStatusPanel.ReleaseBuffer();

	}

	if (FAILED(AddCfgWizTask(_T("Launch UI"), szPluginPath)))
	{
		m_spLog->Log("Failed to Add Launch UI to CfgWiz tasks list");
		makeGenericError ( ERR_FAILED_LAUNCH_UI, E_FAIL, IDS_NAVERROR_INTERNAL);

		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::LaunchRescue()
{
	// NAV no longer ships Rescue, could technically remove this method
	// since nobody uses this interface but cfgwiz
	return E_NOTIMPL;
}

STDMETHODIMP CNAVCfgWizMgr::get_EnableAsyncScan(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		//m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableAPCompressed, &m_dwSetting);

		//*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);

		// Compressed file scanning is default to off since NAV 05, this option is not configurable since then
		*pVal = VARIANT_FALSE;
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_LiveUpdate(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

    //YAHOO: We want to set LU Express settings, but defer the LU call to YOP Installer
    //So CFGWIZ_LaunchLiveUpdate will be FALSE
    hrx << SetLUExpressMode();

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_LaunchLiveUpdate, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}


STDMETHODIMP CNAVCfgWizMgr::get_SilentMode(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_SilentMode, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_AutoProtect(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableAutoProtect, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_WeeklyScan(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableWeeklyScan, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_ScanAtStartup(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableScanAtStartup, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}


STDMETHODIMP CNAVCfgWizMgr::get_EmailScanner(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableEmailScanner, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}


STDMETHODIMP CNAVCfgWizMgr::get_EmailScannerIn(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableEmailScannerIn, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_EmailScannerOut(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableEmailScannerOut, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_Rescue(VARIANT_BOOL	*pVal)
{
	// NAV no longer ships Rescue, could technically remove this method
	// since nobody uses this interface but cfgwiz
	return E_NOTIMPL;
}


STDMETHODIMP CNAVCfgWizMgr::get_DefinitionAlert(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableAlertDefs, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}


STDMETHODIMP CNAVCfgWizMgr::get_FullSystemScan(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_LaunchFullSystemScan, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_EnableDelayLoad(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableDelayLoad, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_MainUI(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_LaunchUI, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_Registered(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_Registered, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::put_CfgWizCompleted(VARIANT_BOOL newVal)
{
	// Set the Finish setting in CfgWiz.dat
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = (newVal == VARIANT_TRUE ? 1 : 0);

	m_Properties->PutProperty(InstallToolBox::CFGWIZ_Finished, m_dwSetting);

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_AutoLiveUpdate(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableAutoLiveUpdate, &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(...)
	{
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_AutoLiveUpdateMode(long *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 1;

	m_Properties->GetProperty(InstallToolBox::CFGWIZ_AutoLiveUpdateMode, &m_dwSetting);

	*pVal = m_dwSetting;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_OSType(long *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	*pVal = m_OSInfo.GetOSType();

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::RemoveDefAlertDelay()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	try
	{
		// Change the ALERT:StartTimeOut value in alert.dat to 5 minutes (300 seconds)
		TCHAR szAlertDatFile[ MAX_PATH ] = { 0 };
		HNAVOPTS32 hAlert = NULL;
		NAVOPTS32_STATUS status = NAVOPTS32_OK;

		wsprintf( szAlertDatFile, _T( "%s\\alert.dat" ), m_NAVInfo.GetNAVCommonDir() );

		// Allocate a HNAVOPTS32 object
		status = NavOpts32_Allocate( &hAlert );

		if( status == NAVOPTS32_OK )
		{
			// Load alert data from alert.dat.
#ifndef _UNICODE
			status = NavOpts32_Load( szAlertDatFile, hAlert, TRUE );
#else
			status = NavOpts32_LoadU( szAlertDatFile, hAlert, TRUE );
#endif
			if( status == NAVOPTS32_OK )
			{
				// Set the start time-out to 5 minutes
				NavOpts32_SetDwordValue( hAlert, ALERT_StartTimeOut, 300 );

				// Save the file
#ifndef _UNICODE
				NavOpts32_Save( szAlertDatFile, hAlert );           
#else
				NavOpts32_SaveU( szAlertDatFile, hAlert );          
#endif
			}
			else
				m_spLog->Log("Failed to save alert.dat after changing the start time-out value");
		}
		else
			m_spLog->Log("Failed to allocate a navopts32 object in order to change the alert.dat start time-out value");

		// Free the navopts object
		if( hAlert )
			NavOpts32_Free( hAlert );
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to remove Defalert delay.");
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::EnableMSNScan()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 0;

	// Retrieve property from cfgwiz.dat
	m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableMSNScan, &m_dwSetting);

	// Load navopts.dat	
	HRESULT hRes = S_OK;

	try
	{
        ccSettings::CSettingsManagerHelper ccSettingsHelper;
        ccSettings::ISettingsManagerPtr pSettingsManager;
        if ( SYM_SUCCEEDED (ccSettingsHelper.Create(pSettingsManager)))
        {
            CCTRACEI(_T("CNAVCfgWizMgr::EnableMSNScan - Initializing Settings."));
            ccSettings::ISettingsPtr pSettings;
            pSettingsManager->GetSettings (_T("Norton AntiVirus\\NAVOpts.dat\\IMSCAN"), &pSettings);
            if( pSettings )
            {
                CCTRACEI(_T("CNAVCfgWizMgr::EnableMSNScan - Setting ScanMIM = %d."), m_dwSetting);
                if(SYM_FAILED(pSettings->PutDword( "ScanMIM", m_dwSetting)))
                {
                    CCTRACEE(_T("CNAVCfgWizMgr::EnableMSNScan - Could not set ScanMIM."));;
                    throw runtime_error("Unable to set the IMSCAN:ScanMIM Value.");
                }
                CCTRACEI(_T("CNAVCfgWizMgr::EnableMSNScan - Setting FeatureEnabledScanMIM = %d."), m_dwSetting);
                if(SYM_FAILED(pSettings->PutDword( "FeatureEnabledScanMIM", m_dwSetting)))
                {
                    CCTRACEE(_T("CNAVCfgWizMgr::EnableMSNScan - Could not set FeatureEnabledScanMIM."));;
                    throw runtime_error("Unable to set the IMSCAN:FeatureEnabledScanMIM Value.");
                }
                CCTRACEI(_T("CNAVCfgWizMgr::EnableMSNScan - Saving settings."));
                if(SYM_FAILED(pSettingsManager->PutSettings(pSettings)))
                {
                    CCTRACEE(_T("CNAVCfgWizMgr::EnableMSNScan - Could not save settings."));;
                    throw runtime_error("Unable to save settings changes.");
                }

                ccLib::CEvent eventIMScan;        
                if ( eventIMScan.Create ( NULL, TRUE, FALSE, SYM_REFRESH_IMSCANNER_STATUS_EVENT, TRUE ))
                {
                     CCTRACEI(_T("CNAVCfgWizMgr::EnableMSNScan - Pulsing IM Scan event."));
                     eventIMScan.PulseEvent();
                }
                else
                {
                    CCTRACEE(_T("CNAVCfgWizMgr::EnableMSNScan - Could not create IM Scan event."));
                }
            }
            else
            {
                CCTRACEE ("CNAVCfgWizMgr::EnableMSNScan - Could not get settings object.");
                throw runtime_error("Could not get settings object.");
            }
        }
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
		hRes = E_FAIL;	
	}
	catch(...)
	{
		m_spLog->LogEx(_T("Failure Enabling MSN Scan"));
		hRes = E_FAIL;	
	}

	return hRes;
}

STDMETHODIMP CNAVCfgWizMgr::LaunchLiveUpdate()
{
    StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_FAILED_ADD_LU);

    hrx << SetLUExpressMode();

	// Get the path to LiveUpdate

	TCHAR szLUDir[MAX_PATH+2] = {0};
    TCHAR szTemp[MAX_PATH] = {0};

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, szTemp)))
	{
        // Hack to enclose in quotes, c.f. defect 1-3MLT47
        _tcscpy(szLUDir, _T("\""));
        _tcscat(szLUDir, szTemp);
		_tcscat(szLUDir, _T("\\Symantec\\LiveUpdate\\LUAll.exe\""));

		// Add LiveUpdate to the tasks list

		if (FAILED(AddCfgWizTask(_T("LiveUpdate"), szLUDir)))
		{
			m_spLog->Log("Failed to Add LiveUpdate to CfgWiz tasks list");

			makeGenericError ( ERR_FAILED_ADD_LU, E_FAIL, IDS_NAVERROR_INTERNAL );
			return E_FAIL;
		}
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::LaunchSystemScan()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_FAILED_ADD_FSS);

	// Determine the path of NAVW32.exe  
	TCHAR szNAVWPath[MAX_PATH] = {0};

	if(m_licenseType == DJSMAR_LicenseType_Rental)
	{
		// Always launch quick scan for SOS
		wsprintf(szNAVWPath, "\"%s\\NAVW32.EXE\" /task:\"%s\\Tasks\\quick.sca\"", m_NAVInfo.GetNAVDir(), m_NAVInfo.GetNAVCommonDir());
	}
	else
	{
		// NOTE: Enclose the executable's path in quotes to avoid defect 1-3MLT47
		wsprintf(szNAVWPath, "\"%s\\NAVW32.EXE\" /task:\"%s\\Tasks\\mycomp.sca\"", m_NAVInfo.GetNAVDir(), m_NAVInfo.GetNAVCommonDir());
	}

	CCTRACEI(CCTRCTX _T("Command: %s"), szNAVWPath);

	// Add Full system scan to the tasks list

	if (FAILED(AddCfgWizTask(_T("Full System Scan"), szNAVWPath)))
	{
		m_spLog->Log("Failed to Add Full System Scan to CfgWiz tasks list");
		makeGenericError ( ERR_FAILED_ADD_FSS, E_FAIL, IDS_NAVERROR_INTERNAL );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNAVCfgWizMgr::AddCfgWizTask(const TCHAR* pszTaskName, const TCHAR* pszTaskValue)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_FAILED_OPEN_TASK_LIST);

	// Get the CfgWiz tasks list key

	CRegKey CfgWizKey;

	LONG lResult = CfgWizKey.Create(HKEY_LOCAL_MACHINE, g_cszCfgWizRegKey, REG_NONE, 
		REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, NULL);

	if (lResult == ERROR_SUCCESS)
	{
		// Retrieve the tasks list

		TCHAR szTasksList[MAX_PATH] = {0};
		TCHAR szNewTasksList[MAX_PATH] = {0};
		DWORD dwSize = sizeof(szTasksList);

		// TODO: define NAV CfgWiz somewhere in CfgWiz

		CfgWizKey.QueryValue(szTasksList, NULL, &dwSize);

		_tcscpy(szNewTasksList, szTasksList);

		// Add the task to the tasks list

		if (_tcslen(szNewTasksList) > 0)
		{
			_tcscat(szNewTasksList, _T(";"));
			_tcscat(szNewTasksList, pszTaskName);
		}
		else
		{
			_tcscat(szNewTasksList, pszTaskName);
		}

		lResult = CfgWizKey.SetValue(szNewTasksList);

		// Failed to add the task to the tasks list

		if (forceError (ERR_FAILED_ADD_TASK) || lResult != ERROR_SUCCESS)
		{
			m_spLog->Log("Failed to add task %s to task list", szNewTasksList);
			makeGenericError ( ERR_FAILED_ADD_TASK, E_FAIL, IDS_NAVERROR_INTERNAL );            
			return E_FAIL;
		}

		// Add task

		lResult = CfgWizKey.SetValue(pszTaskValue, pszTaskName);

		// Failed to add the task

		if (forceError (ERR_FAILED_ADD_TASK) || lResult != ERROR_SUCCESS)
		{
			m_spLog->Log("Failed to add task %s", pszTaskName);

			// Restore to the old tasks list

			CfgWizKey.SetValue(szTasksList);
			makeGenericError ( ERR_FAILED_ADD_TASK, E_FAIL, IDS_NAVERROR_INTERNAL );                        
			return E_FAIL;
		}

		CfgWizKey.Close();
	}
	else
	{
		m_spLog->Log("Failed to open %s", g_cszCfgWizRegKey);
		makeGenericError ( ERR_FAILED_OPEN_TASK_LIST, E_FAIL, IDS_NAVERROR_INTERNAL );
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::ReactivateSubscription()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	try
	{
		// Getting licensing and subscription properties
		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_ACTIVATE_SUBSCRIPTION);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
		pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_ACTIVATE_OEM_SUBS);

		hrx << pepBase.QueryPolicy();
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to reactivate subscription");
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::LaunchLiveReg(VARIANT_BOOL bUIMode)
{
	return E_NOTIMPL;
}

STDMETHODIMP CNAVCfgWizMgr::RepairShortcuts()
{	
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	HRESULT hRes;
	CComPtr<IShellLink> pShellLink;         
	CComPtr<IPersistFile> pPersistFile; 

	// Get a pointer to the IShellLink interface. 
	hRes = pShellLink.CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER);
	if (FAILED(hRes))
		return S_FALSE;

	// Query IShellLink for the IPersistFile interface for saving the shortcut in persistent storage
	hRes = pShellLink->QueryInterface(IID_IPersistFile, (void**) &pPersistFile);
	if (FAILED(hRes))
		return S_FALSE;

	// open the Links key where link information is stored.
	ATL::CRegKey rkLinks;
	if(ERROR_SUCCESS != rkLinks.Open(HKEY_LOCAL_MACHINE, _T("software\\Symantec\\Norton AntiVirus\\Links")))
		return S_FALSE;

	// set up where the links will point
	TCHAR szNMainPath[MAX_PATH] = {0};
	TCHAR szCmdLine[MAX_PATH] = {0};
	TCHAR szIconPath[MAX_PATH] = {0};
	TCHAR szNisPlugParam[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szNisPlugParam) / sizeof(TCHAR);

	wsprintf(szNMainPath, "%s\\NMain.exe", m_NAVInfo.GetSymantecCommonDir());

	// Check to see if NAV is part of NIS.
	if(SUCCEEDED(NAVUIHelp::BuildNisPluginParams(szNisPlugParam, dwSize)))
	{
		// Launch NIS UI with NAV Status panel active.
		_tcscpy(szCmdLine, szNisPlugParam);
	}
	else
	{
		// Need to tell NMain to give focus to the NAV panel, since
		// Norton Protection Center is the default home

		CString csTitle;
		CString csStatusPanel;
		CString csAVRES;

		// Setup path to AVRES.DLL
		csAVRES.Format(_T("%s\\AVRES.DLL"), m_NAVInfo.GetNAVDir());

		StahlSoft::CSmartModuleHandle shMod(LoadLibrary(csAVRES));
		hrx << (((HANDLE) shMod != (HANDLE)NULL) ? S_OK : CO_E_DLLNOTFOUND);

		// Get names of NAV main UI title and status panel.
		csTitle.LoadString(shMod, IDS_PRODUCTNAME);
		csStatusPanel.LoadString(shMod, IDS_STATUS_CATEGORYTITLE);

		csTitle.Remove(_T('&'));
		csStatusPanel.Remove(_T('&'));


		if(m_licenseType == DJSMAR_LicenseType_Rental)
			wsprintf(szCmdLine, "/dat:%s\\navui.nsi /nosysworks /goto:%s\\%s", m_NAVInfo.GetNAVDir(), csTitle.GetBuffer(), csStatusPanel.GetBuffer());
        else
			wsprintf(szCmdLine, "/dat:%s\\navui.nsi /goto:%s\\%s", m_NAVInfo.GetNAVDir(), csTitle.GetBuffer(), csStatusPanel.GetBuffer());

		csTitle.ReleaseBuffer();
		csStatusPanel.ReleaseBuffer();
	}

	wsprintf(szIconPath, "%s\\NavW32.exe", m_NAVInfo.GetNAVDir());


	// Read in the link name.
	TCHAR szStartMenuShortcut[MAX_PATH] = {0};
	DWORD dwBuffSize = sizeof(szStartMenuShortcut);
	if(ERROR_SUCCESS == rkLinks.QueryStringValue(_T("StartLinkPath"), szStartMenuShortcut, &dwBuffSize))
	{
		// Ensure that the string is Unicode
		WCHAR wszShortcut[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szStartMenuShortcut, -1, wszShortcut, MAX_PATH);

		// load the link
		hRes = pPersistFile->Load(wszShortcut, STGM_READWRITE);
		if(SUCCEEDED(hRes))
		{
			// Update the shortcut to point to NAVUI
			pShellLink->SetPath(szNMainPath);
			pShellLink->SetArguments(szCmdLine);
			pShellLink->SetWorkingDirectory(m_NAVInfo.GetNAVDir());
			pShellLink->SetIconLocation(szIconPath, 0);

			// Save the link by calling IPersistFile::Save
			hRes = pPersistFile->Save(wszShortcut, TRUE);
		}
	}		

	TCHAR szDesktopShortcut[MAX_PATH] = {0};
	dwBuffSize = sizeof(szDesktopShortcut);
	if(ERROR_SUCCESS == rkLinks.QueryStringValue(_T("DeskLinkPath"), szDesktopShortcut, &dwBuffSize))
	{                
		// Ensure that the string is Unicode
		WCHAR wszShortcut[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, szDesktopShortcut, -1, wszShortcut, MAX_PATH);

		hRes = pPersistFile->Load(wszShortcut, STGM_READWRITE);
		if (SUCCEEDED(hRes))
		{   
			// Update the shortcut to point to NAVUI
			pShellLink->SetPath(szNMainPath);
			pShellLink->SetArguments(szCmdLine);
			pShellLink->SetWorkingDirectory(m_NAVInfo.GetNAVDir());
			pShellLink->SetIconLocation(szIconPath, 0);

			// Save the link by calling IPersistFile::Save
			hRes = pPersistFile->Save(wszShortcut, TRUE);
		}
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::SetSSARunKey()
{
	return E_NOTIMPL;
}

STDMETHODIMP CNAVCfgWizMgr::get_NAVError(INAVCOMError* *pVal)
{
	if ( !m_spError )
		return E_FAIL;

	*pVal = m_spError;
	(*pVal)->AddRef (); // We get a ref and the caller gets one

	return S_OK;
}


void CNAVCfgWizMgr::makeError(long lMessageID, long lHResult)
{
	if ( !m_spError )
		return;

	switch(lMessageID)
	{
	case ERR_SUBSCRIPTION_START:
	case ERR_SUBSCRIPTION_END:
	case ERR_SUBSCRIPTION_LENGTH:
		makeGenericError(lMessageID, lHResult, IDS_NAVERROR_INTERNAL);
		return;
		break;

	default:
		break;
	}

	CComBSTR bstrErrorMessage;
	bstrErrorMessage.LoadString ( lMessageID );
	m_spError->put_Message ( bstrErrorMessage );
	m_spError->put_ModuleID ( AV_MODULE_ID_INFO_WIZARD );
	m_spError->put_ErrorID ( lMessageID );
	m_spError->put_HResult ( lHResult );
}

void CNAVCfgWizMgr::makeGenericError(long lMessageID, long lHResult, long lNAVErrorResID)
{
	if ( !m_spError )
		return;

	m_spError->put_ModuleID ( AV_MODULE_ID_INFO_WIZARD );
	m_spError->put_ErrorID ( lMessageID );
	m_spError->put_HResult ( lHResult );
	m_spError->put_ErrorResourceID ( lNAVErrorResID );		
}

bool CNAVCfgWizMgr::forceError(long lErrorID)
{
	if ( !m_spError )
		return false;

	long lTempErrorID = 0;
	long lTempModuleID = 0;

	m_spError->get_ForcedModuleID ( &lTempModuleID );
	m_spError->get_ForcedErrorID ( &lTempErrorID );

	if ( lTempModuleID == AV_MODULE_ID_INFO_WIZARD &&
		lTempErrorID == lErrorID )
		return true;
	else
		return false;
}

STDMETHODIMP CNAVCfgWizMgr::get_TotalBrandingText(LONG* pnTotalText)
{
	TCHAR szBuff[MAX_PATH] = {0};	

	InitGetBrandingText(IDS_TOTAL_CFGWIZ_BRANDINGTEXT, szBuff, sizeof(szBuff)/sizeof(TCHAR));

	*pnTotalText = atoi(szBuff);

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::GetBrandingText(LONG nTextId, BSTR* pbstrText)
{
	LONG nResId = 0;
	switch(nTextId)
	{
	case 0:
		nResId = IDS_CFGWIZ_WELCOMETEXT_1;
		break;

	case 1:
		nResId = IDS_CFGWIZ_WELCOMETEXT_2;
		break;

	case 2:/* Not used in NAV 2006 */
		nResId = IDS_CFGWIZ_WELCOMETEXT_3;
		break;

	case 3:/* Not used in NAV 2006 */
		nResId = IDS_CFGWIZ_WELCOMETEXT_HEADER;
		break;

	case 4:
		nResId = IDS_AV_PRODUCT_NAME;
		break;

	case 5:
		nResId = IDS_AV_PRODUCT_NAME_VERSION;
		break;
	}

	TCHAR szBuff[MAX_PATH] = {0};	
	InitGetBrandingText(nResId, szBuff, sizeof(szBuff)/sizeof(TCHAR));
	if(_tcslen(szBuff))
	{
		CComBSTR bstrBuff(szBuff);
		*pbstrText = bstrBuff.Detach();
	}

	return S_OK;
}

HRESULT CNAVCfgWizMgr::InitGetBrandingText(LONG nResID, LPTSTR lpszBuff, int nLength)
{
	if(m_shAVRES == NULL)
	{
		TCHAR szAVRES[MAX_PATH] = {0};
		::wsprintf(szAVRES, _T("%s\\AVRES.DLL"), m_NAVInfo.GetNAVDir());
		m_shAVRES = LoadLibrary(szAVRES);
	}

	LoadString(m_shAVRES, nResID, lpszBuff, nLength);

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_ShowBrandingLogo(VARIANT_BOOL* pVal)
{
	TCHAR szBuff[MAX_PATH] = {0};

	InitGetBrandingText(IDS_CFGWIZ_BRANDING_LOGO_SHOW, szBuff, sizeof(szBuff)/sizeof(TCHAR));

	*pVal = atoi(szBuff) == 0 ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_ICFEnabled(VARIANT_BOOL *pVal)
{
	if(!pVal)
	{
		return E_POINTER;
	}

	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	// scope the loader up here so it gets
	*pVal = VARIANT_FALSE;
	NAVICFManagerLoader ICFLdr;
	try
	{
		// get our ICFMgr object
		IICFManagerPtr ICFMgr;
		if(SYM_FAILED(ICFLdr.CreateObject(ICFMgr.m_p)) || !ICFMgr)
			return S_OK;

		// initialize the object
		if(SYM_FAILED(ICFMgr->Init()))
			return S_OK;

		// first check if its enabled
		BOOL bEnabled;
		if(SYM_FAILED(ICFMgr->IsEnabled(bEnabled)))
			return S_OK;

		if(bEnabled)
		{
			*pVal = VARIANT_TRUE;
		}
	}
	catch(...)
	{
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::put_DisableICF(VARIANT_BOOL bVal)
{
	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	if(bVal == VARIANT_TRUE)
	{
		try
		{
			// scope the loader up here so it gets
			NAVICFManagerLoader ICFLdr;
			{
				// get our ICFMgr object
				IICFManagerPtr ICFMgr;
				if(SYM_FAILED(ICFLdr.CreateObject(ICFMgr.m_p)))
					return S_FALSE;

				// initialize the object
				if(SYM_FAILED(ICFMgr->Init()))
					return S_FALSE;

				// first check if its enabled
				BOOL bEnabled;
				if(SYM_FAILED(ICFMgr->IsEnabled(bEnabled)))
					return S_FALSE;

				// if it is enabled then disable it
				if(bEnabled && SYM_SUCCEEDED(ICFMgr->StoreConnections()) && SYM_SUCCEEDED(ICFMgr->DisableConnections()))
				{
					return S_OK;
				}
			}
		}
		catch(...)
		{
		}
	}

	return S_FALSE;
}

STDMETHODIMP CNAVCfgWizMgr::get_StartNPFMonitor(VARIANT_BOOL *pVal)
{
	if(!pVal)
	{
		return E_POINTER;
	}

	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	*pVal = VARIANT_FALSE;

	COSInfo osinfo;
	if(osinfo.IsWinNT())
	{
		// Start the NPFMonitor Service and configure it to auto
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
		if(hSCM)
		{
			// open the npfmonitor service
			SC_HANDLE hNPFMonitor = OpenService(hSCM, _T("NPFMntor"), SERVICE_START | SERVICE_CHANGE_CONFIG);
			if(hNPFMonitor)
			{
				// configure the service
				ChangeServiceConfig(hNPFMonitor, SERVICE_NO_CHANGE, SERVICE_AUTO_START,
					SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, _T(""), 
					_T("Norton AntiVirus Firewall Monitor Service"));

				// start the service
				if(StartService(hNPFMonitor, 0, NULL))
				{
					*pVal = VARIANT_TRUE;
				}

				CloseServiceHandle(hNPFMonitor);
			}

			CloseServiceHandle(hSCM);
		}
	}
	else
	{
		CNAVInfo NAVInfo;
		TCHAR szNPFMonitorPath[MAX_PATH] = {0};
		wsprintf(szNPFMonitorPath, _T("%s\\IWP\\NPFMntor.exe"), NAVInfo.GetNAVDir());

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.wShowWindow = SW_SHOWNORMAL;

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		//launch "NPFMonitor -install" to register it
		if(CreateProcess(szNPFMonitorPath, _T("-install"), NULL, NULL, FALSE, 0, 
			NULL, NAVInfo.GetNAVDir(), &si, &pi) == 0)
		{
			WaitForSingleObject(pi.hProcess, 2*60*1000 /*2 mins*/);

			if(pi.hProcess)
				CloseHandle(pi.hProcess);
			if(pi.hThread)
				CloseHandle(pi.hThread);

			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.wShowWindow = SW_SHOWNORMAL;

			ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

			// now launch it
			if(CreateProcess(szNPFMonitorPath, NULL, NULL, NULL, FALSE, 0, 
				NULL, NAVInfo.GetNAVDir(), &si, &pi))
			{
				*pVal = VARIANT_TRUE;
			}

			if(pi.hProcess)
				CloseHandle(pi.hProcess);
			if(pi.hThread)
				CloseHandle(pi.hThread);
		}
	}

	return (VARIANT_TRUE == *pVal ? S_OK : S_FALSE);
}

STDMETHODIMP CNAVCfgWizMgr::get_IWPInstalled(VARIANT_BOOL *pVal)
{
	if(!pVal)
	{
		return E_POINTER;
	}

	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	// scope the loader up here so it gets
	*pVal = VARIANT_FALSE;

	//IWP objects: scope them up here
	try
	{
		IWP::IIWPPrivateSettingsPtr spIWPPrivateSettings;
		IWP::IIWPSettingsPtr spIWPSettings;

		// load IWPSettings object
		if(SYM_SUCCEEDED(IWP_IWPSettings::CreateObject(GETMODULEMGR(), spIWPSettings.m_p)) && (spIWPSettings != NULL))
		{
			*pVal = VARIANT_TRUE;
			return S_OK;
		}
	}
	catch(...)
	{

	}

	return S_FALSE;
}

STDMETHODIMP CNAVCfgWizMgr::StartSymTDI()
{
	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	try
	{
		if(SUCCEEDED(SNSetOEMState(SNOEM_ENABLE_SYMTDI_LOAD)))
		{
			return S_OK;
		}	
	}
	catch(...)
	{

	}

	return S_FALSE;
}

HRESULT CNAVCfgWizMgr::InstallIDSDefs()
{
	using namespace SymIDS;

	CCTRACEI("InstallIDSDefs() - Start");
	CIDSLoader oIDSLoader;
	SYMRESULT dwResult;

	CString szIDSAppName = _T("NAV_IWP");
	CString szIDSDir = _T("IDS-Diskless");
	CString szIDSMoniker = _T("{E608DE78-7C7C-41af-9466-EBBD707FE3D2}");
	CString szIDSMicroMoniker = _T("{7FD40184-A905-4f7d-8078-BE634AB384DA}");
	CString szIDSGroupMoniker = _T("{303D4D8C-76A8-4f99-8191-7B382B182F7E}");
	SIGNATURE_PROFILE cSigProfile = SIGNATURE_PROFILE_DISKLESS;

	ITB_VERSION verProduct;
	InstallToolBox::GetNAVVersion(&verProduct);
	UINT64 ui64Version = MAKE_PRODUCT_VERSION(verProduct.nMajorHi,verProduct.nMajorLo,verProduct.nMinorHi,verProduct.nMinorLo);

	CNAVInfo NAVInfo;
	CString szDefSourcePath = NAVInfo.GetNAVDir();
	PathAppend(szDefSourcePath.GetBuffer(MAX_PATH), _T("IWP\\IDSDefs"));
	szDefSourcePath.ReleaseBuffer();

	//
	//Scope CSymPtr
	{
		CSymPtr<IIDSSubscription> pIDSSubscription;
		dwResult = oIDSLoader.GetObject(IID_IDSSubscription, (ISymBase **)&pIDSSubscription);													
		if(SYM_SUCCEEDED(dwResult))
		{
			CCTRACEI("InstallDefs::Loaded IIDSSubscription object");
			dwResult = pIDSSubscription->Initialize ();
			if(SYM_SUCCEEDED(dwResult))
			{
				CCTRACEI("InstallDefs::Initialize() Successful");
				//CCTRACEI("Subscribe(%s , %s, %s, %s, %s,%s)",szIDSAppName,cSigProfile,szIDSDir,szIDSMoniker,szIDSMicroMoniker,szIDSGroupMoniker);

				dwResult = pIDSSubscription->Subscribe (szIDSAppName,
					cSigProfile,
					ui64Version,
					szIDSDir,
					szIDSMoniker,
					szIDSMicroMoniker,
					szIDSGroupMoniker);

				if(SYM_SUCCEEDED( dwResult ))
				{
					CCTRACEI("Subscribe() to IDS Successful");
				}
				else
				{
					CCTRACEE("Subscribe() to IDS Un Successful. ERROR: 0x%x",dwResult);
					return -1;
				}
			}
			else
			{
				CCTRACEE("InstallDefs::pIDSSubscription->Initialize () Failed ERROR: 0x%x",dwResult );
			}

		}
		else
		{
			CCTRACEE("InstallDefs::GetObject(IID_IDSSubscription..) Failed ERROR: 0x%x",dwResult);
		}
	}


	//scope IIDSDefsManager
	{
		CSymPtr <IIDSDefsManager> poIDSDefsManager;
		dwResult = oIDSLoader.GetObject (IID_IDSDefsManager, (ISymBase **) &poIDSDefsManager);

		if (SYM_FAILED (dwResult))
		{
			CCTRACEE(_T("InstallDefs::Could not get object IID_IDSDefsManager. ERROR: 0x%x"),dwResult);
			return E_FAIL;
		}

		dwResult = poIDSDefsManager->Initialize ();

		if (SYM_FAILED (dwResult))
		{
			CCTRACEE(_T("InstallDefs::poIDSDefsManager->Initialize () Failed. ERROR: 0x%x"),dwResult);
			return E_FAIL;
		}


		dwResult = poIDSDefsManager->InstallDefs (szIDSAppName, szDefSourcePath);
	}

	if (SYM_FAILED (dwResult))
	{	
		CCTRACEE("InstallDefs::InstallDefs (%s, %s) Failed. ERROR: 0x%x ",szIDSAppName,szDefSourcePath,dwResult);
		return E_FAIL;
	}
	else
		CCTRACEI("InstallDefs::InstallDefs (%s, %s) Succeeded",szIDSAppName,szDefSourcePath);

	CCTRACEI("InstallIDSDefs() - Finish");
	return S_OK;
}

HRESULT CNAVCfgWizMgr::SetLUExpressMode()
{
    StahlSoft::HRX hrx;

    try
	{
		// Set LU mode configuration before launching it

		DWORD dwEnableLUExpressMode = 0;
		DWORD dwEnableLUExpressAutoStart = 0;
		DWORD dwEnableLUExpressAutoStop = 0;
		DWORD dwDisableLUStopBtn = 0;

        try
        {
            hrx << m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableLUExpressMode, &dwEnableLUExpressMode);
		    hrx << m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableLUExpressAutoStart, &dwEnableLUExpressAutoStart);
		    hrx << m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableLUExpressAutoStop, &dwEnableLUExpressAutoStop);
		    hrx << m_Properties->GetProperty(InstallToolBox::CFGWIZ_DisableLUExpressStopBtn, &dwDisableLUStopBtn);
        }
        catch(...)
        {
        }

		CCTRACEI("CNAVCfgWizMgr::SetLUMode - Set LU configuration: ExpressMode: %d, AutoStart: %d, AutoStop: %d, DisableStopBtn: %d",
			dwEnableLUExpressMode, dwEnableLUExpressAutoStart, dwEnableLUExpressAutoStop, dwDisableLUStopBtn);

		if(dwEnableLUExpressMode)
		{
			STLIVEUPDATECOMLib::IstCheckForUpdatesPtr ptrLiveUpdate;

			CCTRACEI("CNAVCfgWizMgr::SetLUMode - Create the LiveUpdate CheckForUpdates object");

			// Create the LiveUpdate CheckForUpdates object.
			HRESULT hr = ptrLiveUpdate.CreateInstance( __uuidof( stCheckForUpdates ), NULL,
				CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER );
			if(FAILED(hr))
			{
				CCTRACEE("CNAVCfgWizMgr::SetLUMode - Create the LiveUpdate CheckForUpdates object failed!!! Error: %d", hr);
				hrx << hr;
			}

			CCTRACEI("CNAVCfgWizMgr::SetLUMode - Load the Settings file.");

			// Load the Settings file into memory.
			hr = ptrLiveUpdate->LoadSettings();
			if(FAILED(hr))
			{
				CCTRACEE("CNAVCfgWizMgr::SetLUMode - Load the Settings file failed!!! Error: %d", hr);
				hrx << hr;
			}

			CCTRACEI("CNAVCfgWizMgr::SetLUMode - Get the LiveUpdate Settings file object.");

			// Get the LiveUpdate Settings file object.
			STLIVEUPDATECOMLib::IstSettingsPtr ptrLUSettings = ptrLiveUpdate->GetSettings();
			if(ptrLUSettings == NULL)
			{
				CCTRACEI("CNAVCfgWizMgr::SetLUMode - Failed to get LiveUpdate Settings file object.");
				hrx << E_POINTER;
			}

			// Add Corporate Mode settings to the Settings file.
			_bstr_t bstrYes( L"YES" );

			// Force Express mode with Auto Start, Auto Exit, and the Stop Button disabled.
			// (Express Mode isn't required for corporate installs, but it is recommended.)
			CCTRACEI("CNAVCfgWizMgr::SetLUMode - Enable LU express mode.");
			ptrLUSettings->PutProperty( bstrPREFS_EXPRESS_MODE_ON, bstrYes );

			if(dwEnableLUExpressAutoStart)
			{
				CCTRACEI("CNAVCfgWizMgr::SetLUMode - Enable LU auto start.");
				ptrLUSettings->PutProperty( bstrPREFS_EXPRESS_MODE_AUTO_START, bstrYes );
			}

			if(dwEnableLUExpressAutoStop)
			{
				CCTRACEI("CNAVCfgWizMgr::SetLUMode - Enable LU auto exit.");
				ptrLUSettings->PutProperty( bstrPREFS_EXPRESS_MODE_AUTO_EXIT, bstrYes );
			}

			if(dwDisableLUStopBtn)
			{
				CCTRACEI("CNAVCfgWizMgr::SetLUMode - Disable LU stop button.");
				ptrLUSettings->PutProperty( bstrPREFS_EXPRESS_MODE_DISABLE_STOP, bstrYes );
			}

			// This settings gets around a legacy issue that occurs on certain machines with
			// SCS installed.  This property isn't required, but it doesn't hurt to set it.
			ptrLUSettings->PutProperty( bstrPREFS_ALL_TRANSPORTS_AVAILABLE, bstrYes );
		}
	}
	catch(_com_error& e)
	{
		// Ignore error if we failed to set LU mode.
		//  Continue to launch LU.		
		CCTRACEE("CNAVCfgWizMgr::SetLUMode - Exception during update LU express mode settings!!! Error: %d", e.Error());
	}
    return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_StartIWP(VARIANT_BOOL *pVal)
{
	if(!pVal)
	{
		return E_POINTER;
	}

	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	// scope the loader up here so it gets
	*pVal = VARIANT_TRUE;

	try
	{
		IWP::IIWPPrivateSettingsPtr spIWPPrivateSettings;

		// load IWPSettings object
		if(SYM_FAILED(IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(), spIWPPrivateSettings.m_p)) ||
			(spIWPPrivateSettings == NULL))
		{
			CCTRACEW("get_StartIWP() : Failed to Get IIWPPrivateSettings object (could mean its not installed)");
			*pVal = VARIANT_FALSE;
			return S_FALSE;
		}

		IWP::IIWPSettingsQIPtr spIWPSettings(spIWPPrivateSettings);
		if(spIWPSettings == NULL)
		{
			CCTRACEW("get_StartIWP() : Failed to Get IIWPSettings object (could mean its not installed)");
			*pVal = VARIANT_FALSE;
			return S_FALSE;
		}

		// now we know IWP is installed so install IDS first of all
		InstallIDSDefs();

		// find out if its ok to enable IWP (ie: we're not in a yielding state and DRM is ok)
		DWORD dwCanEnable = 0;
		if(SYM_FAILED(spIWPSettings->GetValue(spIWPSettings->IWPCanEnable, dwCanEnable)))
		{
			CCTRACEW("get_StartIWP() : Failed to get IWPCanEnable value.");
			*pVal = VARIANT_FALSE;
			return S_FALSE;
		}

		// only install the rules/ales if we can enable
		if(dwCanEnable)
		{
			// install default FW rules.
			if(SYM_FAILED(spIWPPrivateSettings->InstallDefaultFWRules()))
			{
				CCTRACEE("get_StartIWP() : InstallDefaultFWRules() failed. ");
				*pVal = VARIANT_FALSE;
			}

			// install default ALES
			if(SYM_FAILED(spIWPPrivateSettings->InstallDefaultAles()))
			{
				CCTRACEE("get_StartIWP() : InstallDefaultAles() failed. ");
				*pVal = VARIANT_FALSE;
			}
		}

		DWORD dwEnableIWP = 1;
		m_Properties->GetProperty(InstallToolBox::CFGWIZ_EnableIWP, &dwEnableIWP);

		// always set the userwantson bit so if we unyield we come on auto
		if(SYM_FAILED(spIWPSettings->SetValue(spIWPSettings->IWPUserWantsOn, dwEnableIWP)))
		{
			CCTRACEE("get_StartIWP() : Failed to set IWPUserWantsOn value.");
			*pVal = VARIANT_FALSE;
		}

		// save everything and we're done
		if(SYM_FAILED(spIWPSettings->Save()))
		{
			CCTRACEE("get_StartIWP() : Failed to Save IWP Settings.");
			*pVal = VARIANT_FALSE;
		}
	}
	catch(...)
	{
		CCTRACEE("StartIWP() : Uknown Error.");
		*pVal = VARIANT_FALSE;
	}

	return (VARIANT_TRUE == *pVal ? S_OK : S_FALSE);
}

STDMETHODIMP CNAVCfgWizMgr::get_AgreedToEULA(VARIANT_BOOL* pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	try
	{
		if(pVal)
		{
			DWORD dwVal = 0;

			m_Properties->GetProperty(InstallToolBox::CFGWIZ_AgreedToEULA, &dwVal);

			*pVal = (dwVal == 0) ? VARIANT_FALSE : VARIANT_TRUE;
		}
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to retrieve AgreedToEULA property");
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::put_AgreedToEULA(VARIANT_BOOL bVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	try
	{
		m_Properties->PutProperty(InstallToolBox::CFGWIZ_AgreedToEULA, bVal);
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to set AgreedToEULA");
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_ShowSSCOption(VARIANT_BOOL* pVal)
{	
	if(!pVal)
	{
		return E_POINTER;
	}

	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	ISShared::WSCHelper_Loader WSCLdr;
	*pVal = VARIANT_FALSE;
	try
	{
		CSymPtr<IWSCHelper> WSCHelper;
		if(SYM_FAILED(WSCLdr.CreateObject(WSCHelper.m_p)) || !WSCHelper)
			return S_FALSE;

		// if they want us to show it AND it hasnt been shown yet
		if(WSCHelper->GetShowBroadcastCheckbox() && !WSCHelper->GetInitialBroadcastPrompt() && !WSCHelper->IsPreXPSP2())
		{
			*pVal = VARIANT_TRUE;
			return S_OK;
		}
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to retrieve InitialBroadcastPrompt property");
	}

	return S_FALSE;
}

STDMETHODIMP CNAVCfgWizMgr::get_AlreadyShownSSCOption(VARIANT_BOOL* pVal)
{	
	if(!pVal)
	{
		return E_POINTER;
	}

	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	ISShared::WSCHelper_Loader WSCLdr;
	*pVal = VARIANT_FALSE;
	try
	{
		CSymPtr<IWSCHelper> WSCHelper;
		if(SYM_FAILED(WSCLdr.CreateObject(WSCHelper.m_p)) || !WSCHelper)
			return S_FALSE;

		if(WSCHelper->GetInitialBroadcastPrompt())
		{
			*pVal = VARIANT_TRUE;
			return S_OK;
		}
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to retrieve InitialBroadcastPrompt property");
	}

	return S_FALSE;
}

STDMETHODIMP CNAVCfgWizMgr::get_SSCOption(VARIANT_BOOL* pVal)
{	
	if(!pVal)
	{
		return E_POINTER;
	}

	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	*pVal = VARIANT_FALSE;
	ISShared::WSCHelper_Loader WSCLdr;
	try
	{
		CSymPtr<IWSCHelper> WSCHelper;
		if(SYM_FAILED(WSCLdr.CreateObject(WSCHelper.m_p)) || !WSCHelper)
			return S_FALSE;

		if(WSCHelper->GetDisableBroadcastSetting())
		{
			*pVal = VARIANT_TRUE;
			return S_OK;
		}
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to retrieve DisableBroadcastSetting property");
	}

	return S_FALSE;
}

STDMETHODIMP CNAVCfgWizMgr::put_EnableSSC(VARIANT_BOOL vbVal)
{
	if(!IsItSafe())
	{
		return E_ACCESSDENIED;
	}

	ISShared::WSCHelper_Loader WSCLdr;
	try
	{
		CSymPtr<IWSCHelper> WSCHelper;
		if(SYM_FAILED(WSCLdr.CreateObject(WSCHelper.m_p)) || !WSCHelper)
			return S_FALSE;

		WSCHelper->SetDisableBroadcast(VARIANT_TRUE == vbVal);
		WSCHelper->SetAVOverride(VARIANT_TRUE == vbVal);

		VARIANT_BOOL vbIWPInstalled = VARIANT_FALSE;
		get_IWPInstalled(&vbIWPInstalled);
		if (vbIWPInstalled == VARIANT_TRUE)
			WSCHelper->SetFWOverride(VARIANT_TRUE == vbVal);

		WSCHelper->SetInitialBroadcastPrompt(true);
		return S_OK;
	}
	catch(exception& Ex)
	{
		m_spLog->LogEx(Ex.what());
	}
	catch(...)
	{
		m_spLog->LogEx("Failed to set SetDisableBroadcast property");
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_NortonAntiVirusPath(BSTR *pbstrNortonAntiVirusPath)
{	
    StahlSoft::HRX hrx;
	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	if(pbstrNortonAntiVirusPath)
	{
		CNAVInfo NAVInfo;
		CComBSTR bstrPath(NAVInfo.GetNAVDir());
		*pbstrNortonAntiVirusPath = bstrPath.Detach();
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::AutoStartLCService()
{
	HRESULT hr = S_OK;	
	StahlSoft::HRX hrx;
	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	try
	{
		CPEPClientBase pepBase;
		hrx << pepBase.Init();
		pepBase.SetContextGuid(clt::pep::CLTGUID);
		pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
		pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_LICENSE_RECORD);
		pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_AUTO_START_LICENSE);
		pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

		hrx << pepBase.QueryPolicy();
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		m_spLog->LogEx(_T("CNAVCfgWizMgr::AutoStartLCService - failed 0x%08X"), hr);
	}

	return hr;
}

HRESULT CNAVCfgWizMgr::GetSubscriptionInfo()
{
	HRESULT hr = S_OK;	
	StahlSoft::HRX hrx;

	try
	{
		if(m_bInitSubscription)
		{
			// Getting subscription properties 
			// Do not combine subscription call with license call in FinalConstruct since 
			//  Subscription properties are changed after user accepts EULA.
			CPEPClientBase pepBase;
			hrx << pepBase.Init();
			pepBase.SetContextGuid(clt::pep::CLTGUID);
			pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
			pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_RECORD);
			pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
			pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

			hrx << pepBase.QueryPolicy();

			// Get subscription property
			hrx << pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING, (DWORD &)m_dwSubDaysRemaining, SUBSCRIPTION_STATIC::DISPOSITION_NONE);
			hrx << pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_BEGIN_DATE, (DWORD &)m_dwSubBeginDate, SUBSCRIPTION_STATIC::DISPOSITION_NONE);
			hrx << pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_END_DATE, (DWORD &)m_dwSubEndDate, SUBSCRIPTION_STATIC::DISPOSITION_NONE);

			m_spLog->Log("Subscription - daysRemaining=%d, BeginDate=%d, EndDate=%d", m_dwSubDaysRemaining, m_dwSubBeginDate, m_dwSubEndDate);

			m_bInitSubscription = false;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		m_spLog->LogEx(_T("CNAVCfgWizMgr::GetSubscriptionInfo - failed 0x%08X"), hr);
	}

	return hr;
}

STDMETHODIMP CNAVCfgWizMgr::get_DisableICF(VARIANT_BOOL* pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 1;

	m_Properties->GetProperty(InstallToolBox::CFGWIZ_TurnOffICF, &m_dwSetting);

	*pVal = m_dwSetting;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_ShowICFCheckbox(VARIANT_BOOL* pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 1;

	m_Properties->GetProperty(InstallToolBox::CFGWIZ_ShowICFCheckbox, &m_dwSetting);

	*pVal = m_dwSetting;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::WaitForLicenseData(VARIANT_BOOL *pVal)
{
    StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

    try
	{
		if(!pVal)
			return E_POINTER;

		*pVal = VARIANT_FALSE;

		ccLib::CEvent evt;
		if(evt.Open(EVENT_ALL_ACCESS, FALSE, _T("NAV_CFGWIZ_PREBLOCK_DONE"), FALSE))
		{
			if(evt.Lock(30000, FALSE))
			{
				*pVal = VARIANT_TRUE;
			}
			else
				CCTRACEE(_T("WaitForLicenseData Failed"));
		}
		else
			CCTRACEE(_T("Failed to open CFGWIZ PREBLOCK event!"));
	}
	catch(...)
	{
		CCTRACEE(_T("Caught exception in WaitForLicenseData"));
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_EnableSymProtect(long *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	m_dwSetting = 1;

	m_Properties->GetProperty(InstallToolBox::CFGWIZ_SymProtect_Enabled, &m_dwSetting);

	*pVal = m_dwSetting;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::RunBackGroundTasks(VARIANT_BOOL *pVal)
{
    StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_SECURITY_FAILED);

	try
	{
		if(!pVal)
			return E_POINTER;
        
        CCTRACEI(_T("Launching BackgroundTasks Thread"));
        if(m_pBackGroundTasksThread->Create(NULL, 0, 0))
	    {
		    m_pBackGroundTasksThread->SetPriority(THREAD_PRIORITY_HIGHEST);
    	}

		*pVal = VARIANT_TRUE;
	}
	catch(...)
	{
		CCTRACEE(_T("Caught exception in RunBackGroundTasks"));
	}
	return S_OK;
}

int CBackGroundTasks::Run()
{
    VARIANT_BOOL Val;
    
    m_pCfgWizMgr->RemoveDefAlertDelay();
    m_pCfgWizMgr->AutoStartLCService();
    m_pCfgWizMgr->get_StartNPFMonitor(&Val);
    m_pCfgWizMgr->get_StartIWP(&Val);

    m_hBackGroundTasksEvent.SetEvent();

    return NO_ERROR;
}