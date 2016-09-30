////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CfgWizMgr.cpp : Implementation of CNAVCfgWizMgr
#include "stdafx.h"
#include <ATLComTime.h>
#include <simon.h>

#include "cltProductHelper.h"
#include "ProdInfoNamedProperties.h"

#include "ccverifytrustinterface.h"
#include "NAVError.h"       // For errors
#include "NAVTrust.h"       // For trusting the NAVError object
#include "ccSettingsInterface.h"
#include "ccServicesConfigInterface.h"
#include "ccInstLoader.h"
#include "IWPSettingsInterface.h"
#include "IWPSettingsLoader.h"
#include "SymIDSI.h"
#include "IDSLoader.h"
#include "CommonStructs.h"
#include "Utilities.h"
#include "NAVDetection.h"
#include "AVccModuleId.h"
#include "OSInfo.h" // NAV Version
#include "ccOSInfo.h"
#include "SNDHelper.h"

using namespace InstallToolBox;

//#define _INIT_COSVERSIONINFO

#include "NAVCfgWizDll_h.h"
#include "NAVCfgWizMgr.h"
#include "DefAlertOptNames.h"
#include "InstOptsNames.h"
#include "ISVersion.h"
#include "isResource.h"
#include "NAVLUStrings.h"
#include "isIDSStrings.h"
#include "InstallIDSDefs.h"
#include "ImportDefRules.h"

#include "SimonCollectionImpl.h"
#include "NAVSettingsHelperEx.h"

#include "cltLicenseConstants.h"
#include "cltProdInfoPEPConstants.h"
#include "V2LicensingAuthGuids.h"
#include "cltProductHelper.h"
#include "cfgWizNames.h"

#include "NAVOptHelperEx.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "SettingsNames.h"	// The LiveUpdate Settings Properties' Names
#import "LUCOM.tlb"         // The LuComServer COM Interfaces
#include "LUCOM.h"			// The LuComServer Smart Pointers

#include "uiPathProvider.h" // For location of Main UI
#define UI_STUB_EXE  L"uiStub.exe"
const TCHAR *szUIStubArgs = _T("{68175F05-68E7-47e7-A1F2-4CE1DEE316EE}");

// IM protection
#include "IMConfigLoader.h"

#pragma warning(disable : 4996)
/////////////////////////////////////////////////////////////////////////////
// CNAVCfgWizMgr 

CNAVCfgWizMgr::CNAVCfgWizMgr()
{
    m_pBackGroundTasksThread = new CBackGroundTasks();
	m_pBackGroundTasksThread->Init(this);
}

CNAVCfgWizMgr::~CNAVCfgWizMgr()
{
	if(m_pBackGroundTasksThread)
	{
		delete m_pBackGroundTasksThread;
		m_pBackGroundTasksThread = NULL;
	}
}

HRESULT CNAVCfgWizMgr::FinalConstruct()
{
	HRESULT hr = S_OK;
	StahlSoft::HRX hrx;

	try
	{
		m_bInitSubscription = true;

		CComBSTR bstrErrorClass ("NAVError.NAVCOMError");

		// Check NAVError module for Symantec Signature...
		if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer("NAVError.NAVCOMError") )
		{
			if ( FAILED ( m_spError.CoCreateInstance( bstrErrorClass,
				NULL,
				CLSCTX_INPROC_SERVER)))
			{
			}
		}

		m_Properties = new CNAVProperties( GetProductDir());

		m_dwSubDaysRemaining = 0;
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRCTXE1 (_T("Error in FinalConstruct 0x%08X"), e.Error());
	}
	catch(exception &ex)
	{
		hr = E_UNEXPECTED;
        CCTRCTXE1 (_T("Unexpected Error in FinalConstruct: %s"), ex.what());
	}

	return hr;
}

HRESULT CNAVCfgWizMgr::ValidateFunctionSafety(int iResID)
{
	BOOL bCondition = FALSE;
	HRESULT hr = S_OK;

	switch( iResID )
	{
	case  ERR_INFO_WIZARD_SECURITY_FAILED:

	case ERR_FAILED_OPEN_TASK_LIST:

	case ERR_FAILED_ADD_LU:

	case ERR_FAILED_ADD_TASK:

	case ERR_FAILED_FINISH:
		hr = E_ACCESSDENIED;
		break;

	case ERR_FAILED_LAUNCH_UI:

	case ERR_FAILED_ADD_FSS:
		hr = E_FAIL;
		break;

	default:
		bCondition = FALSE;
		hr = E_FAIL;

	}

	if (forceError (iResID) || bCondition)
	{
		// Determine which generic error message we want to display
		LONG lNAVErrorID = IDS_ISSHARED_ERROR_INTERNAL;

		if(iResID == ERR_INFO_WIZARD_SECURITY_FAILED)
		{
			lNAVErrorID = IDS_ISSHARED_ERROR_INTERNAL_REINSTALL;
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
    ccLib::CString strUIStub;

    TCHAR szUIPath [_MAX_PATH] = {0};
    size_t nSize = _MAX_PATH;
    HRESULT hr = E_FAIL;

    if (! ui::CUIPathProvider::GetPath(szUIPath, nSize))
	{
		CCTRCTXE0(_T("Failed to GetPath from CUIPathProvider"));
		return E_FAIL;
	}

    strUIStub.Format ( L"\"%s\\%s\" %s", szUIPath, UI_STUB_EXE, szUIStubArgs);

	hr = AddCfgWizTask(_T("Launch UI"), strUIStub);
	if (FAILED(hr))
	{
		CCTRCTXE1 (_T("Failed to Add Launch UI to CfgWiz tasks list: %08X"), hr);
		makeGenericError ( ERR_FAILED_LAUNCH_UI, hr, IDS_ISSHARED_ERROR_INTERNAL);

		return hr;
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

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	*pVal = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_LiveUpdate(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_LaunchLiveUpdate), &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
	catch(exception &e)
	{
        CCTRCTXW1(_T("Exception: %s. Returning true."), e.what());
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}


STDMETHODIMP CNAVCfgWizMgr::get_SilentMode(VARIANT_BOOL	*pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_SilentMode), &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
    catch(exception &e)
    {
        CCTRCTXW1(_T("Exception: %s. Returning false."), e.what());
		*pVal = VARIANT_FALSE;
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

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_EnableAlertDefs), &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
    catch(exception &e)
    {
        CCTRCTXW1(_T("Exception: %s. Returning true."), e.what());
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}


STDMETHODIMP CNAVCfgWizMgr::get_FullSystemScan(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 1;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_PostConfigScan), &m_dwSetting);
		*pVal = (m_dwSetting != 0 ? VARIANT_TRUE : VARIANT_FALSE);
	}
    catch(exception &e)
    {
        CCTRCTXW1(_T("Exception: %s. Returning true."), e.what());
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_MainUI(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_LaunchUI), &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
    catch(exception &e)
    {
        CCTRCTXW1(_T("Exception: %s. Returning true."), e.what());
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_Registered(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_Registered), &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
    catch(exception &e)
    {
        CCTRCTXW1(_T("Exception: %s. Returning false."), e.what());
		*pVal = VARIANT_FALSE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::put_CfgWizCompleted(VARIANT_BOOL newVal)
{
	// Set the Finish setting in CfgWiz.dat
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = (newVal == VARIANT_TRUE ? 1 : 0);

	try
	{
		m_Properties->PutProperty(CA2CT(InstallToolBox::CFGWIZ_Finished), m_dwSetting);
	}
	catch (std::exception&)
	{
		// do nothing - PutProperty already logs.
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_AutoLiveUpdate(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_EnableAutoLiveUpdate), &m_dwSetting);

		*pVal = (m_dwSetting == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	}
    catch(exception &e)
    {
        CCTRCTXW1(_T("Exception: %s. Returning true."), e.what());
		*pVal = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_AutoLiveUpdateMode(long *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 1;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_AutoLiveUpdateMode), &m_dwSetting);
	}
	catch (std::exception&)
	{
		// do nothing - GetProperty already logs failure.
	}

	*pVal = m_dwSetting;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_OSType(long *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	*pVal = COSInfo::GetOSType();

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::RemoveDefAlertDelay()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	HNAVOPTS32 hAlert = NULL;

	try
	{
		// Change the ALERT:StartTimeOut value in alert.dat to 5 minutes (300 seconds)
		TCHAR szAlertDatFile[ MAX_PATH ] = { 0 };
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
				CCTRCTXE0 (_T("Failed to save alert.dat after changing the start time-out value"));
		}
		else
			CCTRCTXE0(_T("Failed to allocate a navopts32 object in order to change the alert.dat start time-out value"));

	}
	catch(exception& Ex)
	{
		ATL::CString strException (Ex.what());
        CCTRCTXE1(_T("Exception: %s"),strException);
	}

	// Free the navopts object
	if( hAlert )
		NavOpts32_Free( hAlert );

	return S_OK;
}

// Enables all IM clients, MSN has an optional switch
//
STDMETHODIMP CNAVCfgWizMgr::EnableIMScan()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		// Retrieve MSN specific property from cfgwiz.dat
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_EnableMSNScan), &m_dwSetting);
	}
	catch (std::exception&)
	{
		// do nothing - GetProperty already logs failure.
	}

    ISShared::ISShared_IMLoader loader; 
    ISShared::IIMConfigPtr pIM;
    SYMRESULT sr = loader.CreateObject (GETMODULEMGR(), &pIM);

    if(SYM_FAILED(sr) || !pIM)
    {
        CCTRCTXE1 (_T("Failed to create IM config = 0%x80"), sr);
        return E_FAIL;
    }
    else
    {
        // Set all
        pIM->ConfigureIM (ISShared::IIMConfig::IMTYPE_ALL);

		// Unset MSN
		if ( !m_dwSetting )
		{
			// Set Norton Antivirus\\navopts.dat\\IMSCAN to 0
			ccSettings::CSettingsManagerHelper settingsManagerDat;
			ccSettings::ISettingsManagerPtr pManagerDat = NULL;

			SYMRESULT symres = settingsManagerDat.Create(*&pManagerDat);

			if (SYM_SUCCEEDED(symres) && pManagerDat)
			{
				ccSettings::ISettingsPtr spNavDatSettings;
				ccSettings::ISettings * pNavDatSettings;

				symres = pManagerDat->GetSettings(_T("Norton AntiVirus\\navopts.dat\\IMSCAN"), &pNavDatSettings);
				spNavDatSettings.Attach(pNavDatSettings);
				symres = spNavDatSettings->DeleteValue(_T("ScanTOL"));
				symres = spNavDatSettings->PutDword(_T("ScanMIM"), 0);
				if(SYM_SUCCEEDED(symres))
				{
					symres = pManagerDat->PutSettings(spNavDatSettings);
					if(!SYM_SUCCEEDED(symres))
					{
						CCTRACEE(_T("EnableIMScan(): Failed to commit settings changes"));
						return E_FAIL;
					}
				}
				else
				{
					CCTRACEE(_T("EnableIMScan(): Failed to delete ScanTOL from settings"));
					return E_FAIL;
				}
			}

			// Set Norton AntiVirus\\navopts.def\IMSCAN to 0
			ccSettings::CSettingsManagerHelper settingsManagerDef;
			ccSettings::ISettingsManagerPtr pManagerDef = NULL;

			symres = settingsManagerDef.Create(*&pManagerDef);

			if (SYM_SUCCEEDED(symres) && pManagerDef)
			{
				ccSettings::ISettingsPtr spNavDefSettings;
				ccSettings::ISettings * pNavDefSettings;

				symres = pManagerDef->GetSettings(_T("Norton AntiVirus\\navopts.def\\IMSCAN"), &pNavDefSettings);
				spNavDefSettings.Attach(pNavDefSettings);
				symres = spNavDefSettings->DeleteValue(_T("ScanTOL"));
				symres = spNavDefSettings->PutDword(_T("ScanMIM"), 0);
				if(SYM_SUCCEEDED(symres))
				{
					symres = pManagerDef->PutSettings(spNavDefSettings);
					if(!SYM_SUCCEEDED(symres))
					{
						CCTRACEE(_T("EnableIMScan(): Failed to commit settings changes"));
						return E_FAIL;
					}
				}
				else
				{
					CCTRACEE(_T("EnableIMScan(): Failed to delete ScanTOL from settings"));
					return E_FAIL;
				}
			}
		}
	}

    return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::LaunchLiveUpdate()
{
    StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_FAILED_ADD_LU);

	try
	{
		// Set LU mode configuration before launching it

		DWORD dwEnableLUExpressMode = 0;
		DWORD dwEnableLUExpressAutoStart = 0;
		DWORD dwEnableLUExpressAutoStop = 0;
		DWORD dwDisableLUStopBtn = 0;

        try
        {
            hrx << m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_EnableLUExpressMode), &dwEnableLUExpressMode);
		    hrx << m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_EnableLUExpressAutoStart), &dwEnableLUExpressAutoStart);
		    hrx << m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_EnableLUExpressAutoStop), &dwEnableLUExpressAutoStop);
		    hrx << m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_DisableLUExpressStopBtn), &dwDisableLUStopBtn);
        }
        catch(exception &e)
        {
            CCTRCTXW1(_T("Exception: %s."), e.what());
        }

		CCTRCTXI4 (_T("Set LU configuration: ExpressMode: %d, AutoStart: %d, AutoStop: %d, DisableStopBtn: %d"),
			dwEnableLUExpressMode, dwEnableLUExpressAutoStart, dwEnableLUExpressAutoStop, dwDisableLUStopBtn);

		if(dwEnableLUExpressMode)
		{
			STLIVEUPDATECOMLib::IstCheckForUpdatesPtr ptrLiveUpdate;
            HRESULT hr = S_FALSE;

            // Retry 5 times, per LU's recommendations for defect 1-5E70IO
            for(int iCreateLUPtr = 0; iCreateLUPtr < 5; iCreateLUPtr++)
            {
                CCTRCTXI1 (_T("Attempt %d to create the LiveUpdate CheckForUpdates object"), iCreateLUPtr);
                try
                {
                    // Create the LiveUpdate CheckForUpdates object.
                    hr = ptrLiveUpdate.CreateInstance( __uuidof( stCheckForUpdates ), NULL,
                        CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER );
                    if(FAILED(hr))
                    {
                        CCTRCTXE1 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Create the LiveUpdate CheckForUpdates object failed!!! Error: %d"), hr);
                        hrx << hr;
                        Sleep(500);
                    }
                }
                catch(_com_error &e)
                {
                    CCTRACEW(_T("CNAVCfgWizMgr::LaunchLiveUpdate - Attempt %d received a COM error."), iCreateLUPtr);
                    if(iCreateLUPtr >= 4)
                    {   // Multiple failures - we can't connect to LU - rethrow
                        CCTRCTXW0 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Fifth retry, throwing.")); 
                        throw e;
                    }
                    else
                    {
                        // I hate doing this
                        CCTRCTXI0 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - 500 millisecond sleeps solve everything."));
                        Sleep(500);
                    }
                }
            } // end LU retry loop

			CCTRCTXI0 (_T("Load the Settings file."));

			// Load the Settings file into memory.
			hr = ptrLiveUpdate->LoadSettings();
			if(FAILED(hr))
			{
				CCTRCTXE1 (_T("Load the Settings file failed!!! Error: %d"), hr);
				hrx << hr;
			}

			CCTRCTXI0 (_T("Get the LiveUpdate Settings file object."));

			// Get the LiveUpdate Settings file object.
			STLIVEUPDATECOMLib::IstSettingsPtr ptrLUSettings = ptrLiveUpdate->GetSettings();
			if(ptrLUSettings == NULL)
			{
				CCTRCTXI0 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Failed to get LiveUpdate Settings file object."));
				hrx << E_POINTER;
			}

			// Add Corporate Mode settings to the Settings file.
			_bstr_t bstrYes( L"YES" );

			// Force Express mode with Auto Start, Auto Exit, and the Stop Button disabled.
			// (Express Mode isn't required for corporate installs, but it is recommended.)
			CCTRCTXI0 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Enable LU express mode."));
			ptrLUSettings->PutProperty( bstrPREFS_EXPRESS_MODE_ON, bstrYes );

			if(dwEnableLUExpressAutoStart)
			{
				CCTRCTXI0 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Enable LU auto start."));
				ptrLUSettings->PutProperty( bstrPREFS_EXPRESS_MODE_AUTO_START, bstrYes );
			}

			if(dwEnableLUExpressAutoStop)
			{
				CCTRCTXI0 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Enable LU auto exit."));
				ptrLUSettings->PutProperty( bstrPREFS_EXPRESS_MODE_AUTO_EXIT, bstrYes );
			}

			if(dwDisableLUStopBtn)
			{
				CCTRCTXI0 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Disable LU stop button."));
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
		CCTRCTXE1 (_T("CNAVCfgWizMgr::LaunchLiveUpdate - Exception during update LU express mode settings!!! Error: %d"), e.Error());
	}
	catch(std::exception&)
	{
		// do nothing - PutProperty already logs
	}

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

		HRESULT hr = AddCfgWizTask(_T("LiveUpdate"), szLUDir);
		if (FAILED(hr))
		{
			CCTRCTXE0 (_T("Failed to Add LiveUpdate to CfgWiz tasks list"));

			makeGenericError ( ERR_FAILED_ADD_LU, hr, IDS_ISSHARED_ERROR_INTERNAL );
			return hr;
		}
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::LaunchSystemScan()
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_FAILED_ADD_FSS);

	// Check to see if we need to launch Quick or Full System Scan
	// (Typically, SOS wants Quick Scan)

	DWORD dwSystemScanType = 1;	// 0 - off, 1 - full scan, 2 - quick scan
	
	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_PostConfigScan), &dwSystemScanType);
	}
    catch(exception &e)
    {
        CCTRCTXW1(_T("Exception: %s. Defaulting to 0."), e.what());		
        // If we failed to retrieve this flag, then default to off.
		dwSystemScanType = 0;
	}

	// Determine the path of NAVW32.exe  
	TCHAR szNAVWPath[MAX_PATH] = {0};

	HRESULT hr = S_OK;
	switch(dwSystemScanType)
	{
	case 0:
		// Do not perform a scan
		return S_OK;
		break;
	case 1:
		// Add full system scan to the tasks list
		// NOTE: Enclose the executable's path in quotes to avoid defect 1-3MLT47
		wsprintf(szNAVWPath, _T("\"%s\\NAVW32.EXE\" /task:\"%s\\Tasks\\mycomp.sca\""), m_NAVInfo.GetNAVDir(), m_NAVInfo.GetNAVCommonDir());
		hr = AddCfgWizTask(_T("Full System Scan"), szNAVWPath);
		break;
	case 2:
		// Add quick system scan to the tasks list
		// Always launch quick scan for SOS
		wsprintf(szNAVWPath, _T("\"%s\\NAVW32.EXE\" /task:\"%s\\Tasks\\quick.sca\""), m_NAVInfo.GetNAVDir(), m_NAVInfo.GetNAVCommonDir());
		hr = AddCfgWizTask(_T("Quick System Scan"), szNAVWPath);
		break;
	default:
		// Invalid scan type
		CCTRCTXE0 (_T("Invalid PostScanConfig type"));
		return E_FAIL;
	}

	CCTRACEI(CCTRCTX _T("Command: %s"), szNAVWPath);

	if (FAILED(hr))
	{
		CCTRCTXE0 (_T("Failed to Add Full System Scan to CfgWiz tasks list"));
		makeGenericError ( ERR_FAILED_ADD_FSS, hr, IDS_ISSHARED_ERROR_INTERNAL );
		return hr;
	}

	return S_OK;
}

// Running these tasks is owned by Licensing team (CfgWiz). They must be command-line strings.
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

		lResult = CfgWizKey.QueryValue(szTasksList, NULL, &dwSize);
		if ( lResult != ERROR_SUCCESS)
			CCTRACEW(CCTRCTX _T("Failed to QueryValue regkey [%ls] : %08X. Tasks might not be scheduled yet"), szTasksList, lResult);

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
			CCTRCTXE1 (_T("Failed to add task %s to task list"), szNewTasksList);
			makeGenericError ( ERR_FAILED_ADD_TASK, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );            
			return E_FAIL;
		}

		// Add task

		lResult = CfgWizKey.SetValue(pszTaskValue, pszTaskName);

		// Failed to add the task

		if (forceError (ERR_FAILED_ADD_TASK) || lResult != ERROR_SUCCESS)
		{
			CCTRCTXE1 (_T("Failed to add task %s"), pszTaskName);

			// Restore to the old tasks list

			CfgWizKey.SetValue(szTasksList);
			makeGenericError ( ERR_FAILED_ADD_TASK, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );                        
			return E_FAIL;
		}

		CfgWizKey.Close();
	}
	else
	{
		CCTRCTXE1 (_T("Failed to open %s"), g_cszCfgWizRegKey);
		makeGenericError ( ERR_FAILED_OPEN_TASK_LIST, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL );
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::ReactivateSubscription()
{
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::LaunchLiveReg(VARIANT_BOOL bUIMode)
{
	return E_NOTIMPL;
}

STDMETHODIMP CNAVCfgWizMgr::RepairShortcuts()
{	
	return E_NOTIMPL;
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

	CComBSTR bstrErrorMessage;
	bstrErrorMessage.LoadString ( lMessageID );
	m_spError->put_ErrorResourceID ( lMessageID );
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
	ccLib::CStringW sBuffer;
	CISVersion::LoadString(sBuffer, IDS_BRANDING_CFGWIZ_BRANDINGTEXT_COUNT);
	if(sBuffer.GetLength())
	{
		*pnTotalText = _wtoi(sBuffer);
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::GetBrandingText(LONG nTextId, BSTR* pbstrText)
{
	LONG nResId = 0;
	switch(nTextId)
	{
	case 0:
		nResId = IDS_BRANDING_CFGWIZ_WELCOMETEXT_1;
		break;

	case 1:
		nResId = IDS_BRANDING_CFGWIZ_WELCOMETEXT_2;
		break;

	case 2:/* Not used in NAV 2006 */
		nResId = IDS_BRANDING_CFGWIZ_WELCOMETEXT_3;
		break;

	case 3:/* Not used in NAV 2006 */
		nResId = IDS_BRANDING_CFGWIZ_WELCOME_TAGLINE;
		break;

	case 4:
		nResId = IDS_BRANDING_PRODUCT_NAME;
		break;

	case 5:
		nResId = IDS_BRANDING_PRODUCT_NAME;
		break;
	}

	ccLib::CStringW sBuffer;
	CISVersion::LoadString(sBuffer, nResId);
	if(!sBuffer.IsEmpty())
	{
		CComBSTR bstrBuff(sBuffer);
		*pbstrText = bstrBuff.Detach();
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_ShowBrandingLogo(VARIANT_BOOL* pVal)
{
	// N/A - Starting 2005, vendor logo is part of cfgwiz background
	// image. It's no longer in its own graphic. Always return FALSE.
	*pVal = VARIANT_FALSE;
	return S_OK;
}


STDMETHODIMP CNAVCfgWizMgr::get_IWPInstalled(VARIANT_BOOL *pVal)
{
	if(!pVal)
	{
		return E_POINTER;
	}

	// scope the loader up here so it gets
	*pVal = VARIANT_FALSE;

	//IWP objects: scope them up here
	try
	{
		IWP::IIWPSettingsPtr spIWPSettings;

		// load IWPSettings object
		if(SYM_SUCCEEDED(IWP_IWPSettings::CreateObject(GETMODULEMGR(), spIWPSettings.m_p)) && (spIWPSettings != NULL))
		{
			*pVal = VARIANT_TRUE;
			return S_OK;
		}
	}
	catch(_com_error& e)
	{
        CCTRCTXW2(_T("COM Exception %08x: %s"), e.Error(), e.ErrorMessage());
	}
    catch(exception &ex)
    {
        CCTRCTXW1(_T("Exception: %s"), ex.what());
    }

	return S_FALSE;
}

STDMETHODIMP CNAVCfgWizMgr::put_bDefaultValueForDisableWD(VARIANT_BOOL newVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	try
	{
		m_Properties->PutProperty(CA2CT(InstallToolBox::CFGWIZ_DisableWDChecked), newVal);
	}
	catch(exception& Ex)
	{
		ATL::CString strException (Ex.what());
		CCTRCTXE1(_T("%s"),strException);
	}
    catch(_com_error &e)
    {
        CCTRCTXE2(_T("COM Exception %08x: %s"), e.Error(), e.ErrorMessage());
    }

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_bDefaultValueForDisableWD(VARIANT_BOOL *pVal)
{
	StahlSoft::HRX hrx;
	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	try
	{
		if(pVal)
		{
			*pVal = VARIANT_FALSE;

			DWORD dwVal = 0;

			m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_DisableWDChecked), &dwVal);

			*pVal = (dwVal == 0) ? VARIANT_FALSE : VARIANT_TRUE;
		}
	}
	catch(exception& Ex)
	{
		ATL::CString strException (Ex.what());
		CCTRCTXE1(_T("%s"),strException);
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_bShowDisableWDCheckBox(VARIANT_BOOL *pVal)
{
	if(!pVal)
		return E_POINTER;
	

	StahlSoft::HRX hrx;
	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	try
	{
		*pVal = VARIANT_FALSE;
		if(!ccLib::COSInfo::IsWinVista(true))
		{

			return S_OK;
		}

		DWORD dwVal = 0;

		HRESULT hr=m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_ShowDisableWDCheckBox), &dwVal);

		if(SUCCEEDED(hr))
			*pVal = (dwVal == 0) ? VARIANT_FALSE : VARIANT_TRUE;

	}


	catch(exception& Ex)
	{
		ATL::CString strException (Ex.what());
		CCTRCTXE1(_T("%s"),strException);

	}	
	return S_OK;
}
STDMETHODIMP CNAVCfgWizMgr::put_bShowDisableWDCheckBox(VARIANT_BOOL newVal)
{
		return S_OK;
}
STDMETHODIMP CNAVCfgWizMgr::StartSymTDI()
{
	try
	{
		CSymPtr<CSNDHelper> sndhelper;
		CSNDHelper::CreateSNDHelper(SND_CLIENT_NAV, sndhelper);
		if (sndhelper.m_p)
		{
			if(sndhelper->SetSNDSetting(SymNeti::FS_OEMState, SymNeti::ISymNetSettings::eEnableSymTdiLoad))
			{
				return S_OK;
			}
		}
	}
    catch(_com_error& e)
    {
        CCTRCTXW2(_T("COM Exception %08x: %s"), e.Error(), e.ErrorMessage());
    }
    catch(exception &ex)
    {
        CCTRCTXW1(_T("Exception: %s"), ex.what());
    }

	return S_FALSE;
}

STDMETHODIMP CNAVCfgWizMgr::get_StartIWP(VARIANT_BOOL *pVal)
{
	return E_NOTIMPL;
}


STDMETHODIMP CNAVCfgWizMgr::get_AgreedToEULA(VARIANT_BOOL* pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	try
	{
		if(pVal)
		{
			DWORD dwVal = 0;

			m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_AgreedToEULA), &dwVal);

			*pVal = (dwVal == 0) ? VARIANT_FALSE : VARIANT_TRUE;
		}
	}
	catch(exception& Ex)
	{
		ATL::CString strException (Ex.what());
		CCTRCTXE1(_T("%s"),strException);
	}

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::put_AgreedToEULA(VARIANT_BOOL bVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	try
	{
		m_Properties->PutProperty(CA2CT(InstallToolBox::CFGWIZ_AgreedToEULA), bVal);

		// Tell PEP that user has accepted EULA
		CCLTProductHelper CLTProductHelper;
		CLTProductHelper.Initialize();

		PEP_COLLECTION::CPEPCollectionHelper ColHelper;
		hrx << ColHelper.InitCollection();
		hrx << ColHelper.SetDWORDValue(PRODINFO::szPIEulaAccepted, 1);
		hrx << CLTProductHelper.SetProdInfo(ColHelper);
	}
	catch(exception& Ex)
	{
		ATL::CString strException (Ex.what());
		CCTRCTXE1(_T("%s"),strException);
	}
    catch(_com_error &e)
    {
        CCTRCTXE2(_T("COM Exception %08x: %s"), e.Error(), e.ErrorMessage());
    }

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_NortonAntiVirusPath(BSTR *pbstrNortonAntiVirusPath)
{	
    StahlSoft::HRX hrx;
	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

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
	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	try
	{
		CCLTProductHelper cltHelper;
		hrx << cltHelper.Initialize();
		hrx << cltHelper.StartLicense(); // Is this actually what we want to do?
    }
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRCTXE1 (_T("CNAVCfgWizMgr::AutoStartLCService - failed 0x%08X"), hr);
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
			CCLTProductHelper cltHelper;
			hrx << cltHelper.Initialize(V2LicensingAuthGuids::GUID_AntiVirus_Component);

			// Get subscription property
            hrx << cltHelper.GetDaysRemaining((unsigned long&)m_dwSubDaysRemaining);

			CCTRCTXI1 (_T("Subscription - daysRemaining=%d"), m_dwSubDaysRemaining);

			m_bInitSubscription = false;
		}
	}
	catch(_com_error& e)
	{
		hr = e.Error();
		CCTRCTXE1 (_T("CNAVCfgWizMgr::GetSubscriptionInfo - failed 0x%08X"), hr);
	}

	return hr;
}

STDMETHODIMP CNAVCfgWizMgr::WaitForLicenseData(VARIANT_BOOL *pVal)
{
    StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

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
	catch(exception& e)
	{
        CCTRCTXW1(_T("Exception: %s"), e.what());
	}
	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::RunBackGroundTasks(VARIANT_BOOL *pVal)
{
    StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

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
	catch(exception &ex)
	{
        CCTRCTXW1(_T("Exception: %s"), ex.what());
	}
    catch(_com_error& e)
    {
        CCTRCTXW2(_T("COM Exception %08x: %s"), e.Error(), e.ErrorMessage());
    }

	return S_OK;
}

int CBackGroundTasks::Run()
{
    m_pCfgWizMgr->RemoveDefAlertDelay();
    m_pCfgWizMgr->AutoStartLCService();
    m_hBackGroundTasksEvent.SetEvent();

    return NO_ERROR;
}
STDMETHODIMP CNAVCfgWizMgr::get_ProductType(long *pVal)
{
	StahlSoft::HRX hrx;

	hrx << ValidateFunctionSafety(ERR_INFO_WIZARD_SECURITY_FAILED);

	m_dwSetting = 0;

	try
	{
		m_Properties->GetProperty(CA2CT(InstallToolBox::CFGWIZ_OEM), &m_dwSetting);
	}
	catch (std::exception&)
	{
		// do nothing - GetProperty already logs failure.
	}

	*pVal = m_dwSetting;

	return S_OK;
}

STDMETHODIMP CNAVCfgWizMgr::get_IsAggregateProduct(VARIANT_BOOL *pVal)
{
	DWORD dwAggregated = 0;

	// Find out if NAV is a child product
	READ_SETTING_START()
		READ_SETTING_DWORD(LICENSE_Aggregated, dwAggregated, 0)
	READ_SETTING_END

	if(dwAggregated == 0)
	{
		*pVal = VARIANT_FALSE;
	}
	else
	{
		*pVal = VARIANT_TRUE;
	}

	return S_OK;
}
STDMETHODIMP CNAVCfgWizMgr::SignalPreCfgwizUI()
{
	ccLib::CEvent CfgwizEvent;
	if(CfgwizEvent.Open(EVENT_MODIFY_STATE |SYNCHRONIZE, TRUE, CfgWiz::SZ_CFGWIZ_UI_START_EVENT, FALSE))
	{
		if(!CfgwizEvent.SetEvent())
			CCTRACEE(CCTRCTX _T("Cannot SetEvent %s"),CfgWiz::SZ_CFGWIZ_UI_START_EVENT);
	}
	else
	{
		CCTRACEE(CCTRCTX _T("Cannot Open event %s"),CfgWiz::SZ_CFGWIZ_UI_START_EVENT);
	}
	return S_OK;
}

