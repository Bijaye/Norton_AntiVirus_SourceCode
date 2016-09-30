////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\errorhandling.h"

#include "AVDefines.h"  // AP
#include "isErrorResource.h" // error resource id's
#include "isErrorIds.h" // error id's
#include "avccmoduleid.h"   // NAV module IDs
#include "ccSymModuleLifetimeMgrHelper.h" // GETMODULEMGR()
#include "isErrorLoader.h"
#include "IDSLoader.h"
#include "globals.h"
#include "ccMutex.h"

using namespace AVModule;

CErrorHandling::CErrorHandling(void) :m_eStateAP()
{
}

CErrorHandling::~CErrorHandling(void)
{
	CCTRCTXI0(L"Enter");
	CCTRCTXI0(L"Exit");
}

BOOL CErrorHandling::InitInstance()
{
	CCTRCTXI0(L"Enter");

	// Hook up to AntiVirus component
	//
	bool bAPOK = true;
	HRESULT hr = S_OK;
	DWORD dwError = 0;

	// Donot show alerts if Symsetup is running OR Cfgwiz is not finished.
	BOOL bCanShowAlert = TRUE;
	ccLib::CMutex mutexSymSetup;
	if ( mutexSymSetup.Open(SYNCHRONIZE, FALSE, SYMSETUP_MUTEX, TRUE )  || (! _IsCfgWizFinished()) ) 
	{
		bCanShowAlert = FALSE;
		CCTRACEI(CCTRCTX _T("CanShowAlert is set to FALSE"));
	}

	SYMRESULT sr = m_APLoader.CreateObject(&m_spAP);
	if(SYM_FAILED(sr) || !m_spAP)
	{
		CCTRCTXE1 (_T("Failed to create AP module = 0%08X"), sr);
		bAPOK = false;
		dwError = ERR_FAILED_CREATE_AP;
	}
	else
	{
		// Register as a sink
		//
		hr = m_spAP->RegisterAPOptionSink (this);

		if(FAILED(hr))
		{
			CCTRCTXE1 (_T("Failed to register AP sink = 0x%08X"), hr);
			bAPOK = false;
			dwError = ERR_FAILED_REGISTER_APSINK;
		}

		// Get the initial state
		//
		hr = m_spAP->GetAPState (m_eStateAP);

		if(FAILED(hr))
		{
			CCTRCTXE1 (_T("Failed to get AP state = 0x%08X"), hr);
			bAPOK = false;
			if(0 == dwError)
				dwError = ERR_FAILED_GETAPSTATE;
		}

		CCTRCTXI1(_T("APState = %d"), m_eStateAP);

	}

	if ( !bAPOK )
	{
		if(bCanShowAlert)
		{
			ERROR_DATA error_data;
			error_data.dwModuleID = AV_MODULE_ID_AVSHARED_AP;
			error_data.hr = hr;
			error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;
			error_data.dwErrorID = dwError;
			AddItem(error_data);
		}
		else
		{
			CCTRCTXE3( _T( "SymSetup is Running OR Cfgwiz is not finished. Not displaying alert: Module=%d, ErrorID=%d,HR=0x%08X" ),
				AV_MODULE_ID_AVSHARED_AP,dwError,hr);
		}
	}
	else
	{
		if ( bCanShowAlert )
		{
			CheckAP();
		}
		else
		{
			CCTRCTXI0( _T( "SymSetup is Running OR Cfgwiz is not finished. Skip checking AP" ));
		}
	}


	if ( bCanShowAlert )
	{
		CheckIDS();
	}
	else
	{
		CCTRCTXI0( _T( "SymSetup is Running OR Cfgwiz is not finished. Not checking IDS" ));
	}

	CCTRCTXI0(L"Exit");
	return TRUE;
}

void CErrorHandling::ExitInstance()
{
	if ( m_spAP )
	{
		CCTRCTXI0(L"UnregisterAPOptionSink");
		m_spAP->UnregisterAPOptionSink();
		m_spAP.Release();
	}
}

void CErrorHandling::OnAPStateChange(AutoProtect::FeatureStateAP eCurState)
{
    CCTRCTXI1 (_T("AP state change = %d"), eCurState);
    m_eStateAP = eCurState;
    CheckAP();
}

void CErrorHandling::CheckAP()
{
	CCTRCTXI0(L"Enter");
	ccLib::CMutex mutexSymSetup;
	if( mutexSymSetup.Open(SYNCHRONIZE, FALSE, _T("SYMSETUP_MUTEX"), TRUE ) )
	{
		CCTRACEI(CCTRCTX _T("Symsetup is running. Return."));
		return;
	}

	// Query for this DRM value first since the AV component
    // considers it an error.
    //
    if ( m_eStateAP == AVModule::FeatureStateAV_LicensingDisabled)
    {
        return;
    }

    // Error state
    if ( AutoProtect::IsAPFeatureInErrorState(m_eStateAP))
    {
        ERROR_DATA error_data;
        error_data.dwModuleID = AV_MODULE_ID_AVSHARED_AP;
        error_data.hr = (HRESULT) m_eStateAP;

        switch (m_eStateAP)
        {
		case AutoProtect::FeatureState_AP_AVModuleUnloaded:
			{
				CCTRCTXI0(L"Suppress CED for FeatureState_AP_AVModuleUnloaded error. This is a generic shutdown code, and will likely occur when the component is updated.");
				return;
			}
			break;

        case FeatureStateAV_VirusDefsInvalid:
            {
            error_data.dwErrorID = ERR_NO_AUTH_DEFS;
			error_data.dwResourceID = IDS_ISSHARED_ERROR_VIRUSDEFS_INVALID;			
            }
			break;
		case AutoProtect::FeatureState_AP_SRTSPDriverLoadFailure:
			{
				error_data.dwErrorID = ERR_AP_SRTSP_DRIVER_LOAD_FAILURE;
				error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;			
			}
			break;
		case AutoProtect::FeatureState_AP_FailedToLoadISrtConfig:
			{
				error_data.dwErrorID = ERR_AP_FAILED_TO_LOAD_ISRT_CONFIG;
				error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;			
			}
			break;
		case AutoProtect::FeatureState_AP_ISrtConfigInitFailed:
			{
				error_data.dwErrorID = ERR_AP_ISRT_CONFIG_INIT_FAILED;
				error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;			
			}
			break;
		case AutoProtect::FeatureState_AP_ISrtConfigIsEnabledFailed:
			{
				error_data.dwErrorID = ERR_AP_ISRT_CONFIG_IS_ENABLED_FAILED;
				error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;			
			}
			break;
		case AutoProtect::FeatureState_AP_ISrtConfigSetEnabledFailed:
			{
				error_data.dwErrorID = ERR_AP_ISRT_CONFIG_SET_ENABLED_FAILED;
				error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;			
			}
			break;
		case AutoProtect::FeatureState_AP_ISrtConfigSetDisabledFailed:
			{
				error_data.dwErrorID = ERR_AP_ISRT_CONFIG_SET_DISABLED_FAILED;
				error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;			
			}
			break;
		case AutoProtect::FeatureState_AP_ISrtRegisterForNoitficationsFailed:
			{
				error_data.dwErrorID = ERR_AP_ISRT_REGISTER_FOR_NOITFICATIONS_FAILED;
				error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;			
			}
			break;
        default:
            {
            error_data.dwErrorID = ERR_FAILED_START_AP;
			error_data.dwResourceID = IDS_AUTOPROTECT_FAILED;
            }
			break;
        }

		CCTRCTXE2(L"Display CED: %d, %d", error_data.dwModuleID, error_data.dwErrorID);
        AddItem (error_data);
    }

	CCTRCTXI0(L"Exit");
    // default is OK
}

// Poke the tiger (IDS) to see if it is alive.
//
void CErrorHandling::CheckIDS()
{
	CCTRCTXI0(_T(""));

	HRESULT hr = S_OK;

	CIDSLoader IDSLoader;
	CSymPtr< SymIDS::IIDSGlobalSettings > pIDSGlobalSettings;
	SYMRESULT sr = IDSLoader.GetObject(SymIDS::IID_IDSGlobalSettings,
		(ISymBase **)&pIDSGlobalSettings);
	if (SYM_SUCCEEDED(sr))
	{
        DWORD dwEnabled = 0;
		sr = pIDSGlobalSettings->GetSetting(
			SymIDS::IIDSGlobalSettings::SETTING_ID_ENABLED, dwEnabled);
		if (SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("GetSetting failed (0x%08X)"), sr);
		}
        else
            return; // all good
	}
	else
	{
		CCTRCTXE1(_T("GetObject(IID_IDSGlobalSettings) failed (0x%08X)"), sr);
	}

    ERROR_DATA error_data;
    error_data.dwModuleID = AV_MODULE_ID_DEF_ALERT;
    error_data.hr = (HRESULT) sr;
    error_data.dwResourceID = IDS_ISSHARED_ERROR_INTERNAL;
    error_data.dwErrorID = ERR_IDS_FAILED;

    AddItem (error_data);
}


void CErrorHandling::processQueue()
{
    ERROR_DATA error_data;
    while (getQueueFront(error_data))
    {
        ISShared::ISShared_IError errorLoader;
        ISShared::IErrorPtr error;
        if ( SYM_SUCCEEDED (errorLoader.CreateObject (GETMODULEMGR(), &error)))
        {
            error->Show ( error_data.dwModuleID, 
                        error_data.dwErrorID,
                        error_data.dwResourceID,    // custom resource
                        L"",
                        L"",
                        error_data.hr,
                        true, // only show the error dialog if we can show UI
                        true,
                        NULL );
        }
    }
}