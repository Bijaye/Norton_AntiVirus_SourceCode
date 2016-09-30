////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AVComponent AutoProtect-related functions

#include "stdafx.h"
#include "NavOptionsObj.h"
#include "../navoptionsres/resource.h"
#include "isErrorResource.h"
#include "isErrorIDs.h"
#include "AVDefines.h"

HRESULT CNAVOptions::LoadAP()
{
	AVModule::AutoProtect::FeatureStateAP apState;
	SYMRESULT sr;
	HRESULT hr;

	if(m_bAPLoaded)
		return S_FALSE;

	if(!m_spAP)
	{
		if(SYM_FAILED(sr = m_APLoader.CreateObject(&m_spAP)))
		{
			CCTRCTXE1(_T("Failed to create AVComponent AP object. SR = 0x%08x"), sr);
			return E_FAIL;
		}
	}

	if(FAILED(hr = m_spAP->GetAPState(apState)))
	{
		CCTRCTXW1(_T("Failed to get AP state. Assuming disabled. HR = 0x%08x"), hr);
		apState = AVModule::AutoProtect::FeatureState_AP_Disabled;
	}
	m_bAPEnabled = (apState == AVModule::AutoProtect::FeatureState_AP_Enabled);
	m_bAPEnabledAtStart = m_bAPEnabled; // Cache for snoozing purposes

	if(FAILED(hr = m_spAP->GetOptions(m_APOptsMap)))
	{
		CCTRCTXE1(_T("Failed to get AP Options Map. HR = 0x%08x"), hr);
		return hr;
	}

	m_bAPLoaded = true;
	return S_OK;
}

HRESULT CNAVOptions::get_APEnabled(BOOL* pEnabled)
{
	HRESULT hr;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	// Make sure pointer is valid
	if( forceError ( ERR_INVALID_POINTER ) || NULL == pEnabled )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	if(!m_bAPLoaded && FAILED(hr = this->LoadAP()))
	{
		CCTRCTXE1(_T("Error trying to enable AP! HR = 0x%08x"), hr);
		return hr;
	}

	*pEnabled = m_bAPEnabled ? TRUE:FALSE;
	return S_OK;
}

HRESULT CNAVOptions::put_APEnabled(BOOL bEnabled)
{
	HRESULT hr;

	// Make sure the caller is Symantec signed
	if (forceError ( IDS_ERR_IT_IS_NOT_SAFE ))
	{
		MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
		return E_FAIL;
	}

	if(!m_bAPLoaded && FAILED(hr = this->LoadAP()))
	{
		CCTRCTXE1(_T("Error trying to enable AP! HR = 0x%08x"), hr);
		return hr;
	}

	m_bAPEnabled = bEnabled == FALSE ? false:true;
	
/*************************************************************************************/
	return S_OK;
}

HRESULT CNAVOptions::SetAPDefaults()
{
	DWORD dwVal;
	HRESULT hr;
    bool bLock;

	if(m_bAPLoaded)
	{
		// Initialize the defaults if they have not been initialized yet
		if (!m_bNavDefsInitialized)
		{
			// Use the default options file name in order to initialize
			// the default ccSettings NAV hive
			if( !m_pNavDefs->Init(_T("NAVOPTS.DEF"), FALSE) )
			{
				CCTRCTXE0(_T("Failed to initialize the navopts.def ccSetting hive."));
				return E_FAIL;
			}
			else
				m_bNavDefsInitialized = true;
		}

        if(FAILED(hr = m_pNavOpts->GetValue(NAVAP_DelayLoadLock, dwVal, 0)))
        {
            CCTRCTXW1(_T("Could not get NAVAP:DelayLoadLock setting. Defaulting to Not Locked. HR = 0x%X"), hr);
            dwVal = 0;
        }
        bLock = (dwVal == 0 ? false:true);

		//Always default AP to ON, even if delay load locking is on. In the locked case, we WANT AP to be on!
    	m_bAPEnabled = true;

		if(FAILED(hr = m_pNavDefs->GetValue(AUTOPROTECT_CheckFloppyOnMount, dwVal, 1)))
		{
			CCTRCTXE1(_T("Error getting AP:CheckRemovableOnMount default. HR = 0x%08x"), hr);
		}
		else
		{
			if(FAILED(hr = m_APOptsMap->SetValue(AVModule::AutoProtect::APOPTION_SCAN_REMOVABLE_BRSMBRS_ON_MOUNT, dwVal)))
			{
				CCTRCTXE1(_T("Error setting CROM default into map. HR = 0x%08x"), hr);
			}
		}

        if(!bLock)
        {
		    if(FAILED(hr = m_pNavDefs->GetValue(AUTOPROTECT_DriversSystemStart, dwVal, 1)))
		    {
			    CCTRCTXE1(_T("Error getting AP:DelayLoad default. HR = 0x%08x"), hr);
		    }
		    else
		    {
			    if(FAILED(hr = m_APOptsMap->SetValue(AVModule::AutoProtect::APOPTION_SYSTEMSTART_DRIVERS, dwVal)))
			    {
				    CCTRCTXE1(_T("Error setting SystemStart default into map. HR = 0x%08x"), hr);
			    }
		    }
        }
        else
        {
            CCTRCTXI0(_T("Delay Loading is locked. Not changing DriversSystemStart option."));
        }

		if(FAILED(hr = m_pNavDefs->GetValue(AUTOPROTECT_EnableHeuristicScan, dwVal, 1)))
		{
			CCTRCTXE1(_T("Error getting AP:EnableHeuristicScan default. HR = 0x%08x"), hr);
		}
		else
		{
			if(FAILED(hr = m_APOptsMap->SetValue(AVModule::AutoProtect::APOPTION_ENABLE_HEURISTICS, dwVal)))
			{
				CCTRCTXE1(_T("Error setting Heuristic Scanning default into map. HR = 0x%08x"), hr);
			}
		}
	}

	return S_OK;
}