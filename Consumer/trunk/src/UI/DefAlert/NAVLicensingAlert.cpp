////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVLicensingAlert.cpp

#include "stdafx.h"
#include "NAVLicensingAlert.h"
#include "NavOptHelperEx.h"
#include "CfgWizNames.h"
#include "InstOptsNames.h"
#define _INIT_COSVERSIONINFO
#include "SSOSInfo.h"

#include "NAVSettingsHelperEx.h"

CLicensingAlert::CLicensingAlert(void):
m_dwInstallTime(0)
,m_dwFirstAlertDelay(0)
,m_dwCycleLicWarning(_1_DAY)
,m_dwCycleLicExpired(_1_DAY)
,m_bLicenseAggregated(FALSE)
,m_LicenseType(clt::cltLicenseType_Violated)
,m_LicenseAttribute(clt::cltLicenseAttrib_Violated)
,m_bAlertEnabled(true)
{
}

SIMON_STDMETHODIMP CLicensingAlert::Init()
{
	STAHLSOFT_HRX_TRY(hr)
	{
        if(!SUCCEEDED(hr = m_cltHelper.Initialize()))
        {
            CCTRCTXE1(_T("Could not initialize CLT helper. HR = 0x%08X"), hr);
            return hr;
        }

		m_dwInstallTime = _GetAlertDword(ALERT_InstallTime, 0, FALSE);
		CCTRCTXI1(L"m_dwInstallTime: %d", m_dwInstallTime);

		DWORD dwAggregated = 0;
		READ_SETTING_START()
			READ_SETTING_DWORD(DEFALERT_FirstAlertDelay, m_dwFirstAlertDelay, 0)
			READ_SETTING_DWORD(DEFALERT_CycleLicWarning, m_dwCycleLicWarning, _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleLicExpired, m_dwCycleLicExpired, _1_DAY)
			READ_SETTING_DWORD(LICENSE_Aggregated, dwAggregated, 0)
		READ_SETTING_END
		m_bLicenseAggregated = (dwAggregated == 1);

		CCTRCTXI1(L"DEFALERT_FirstAlertDelay: %d", m_dwFirstAlertDelay);

		// Check QA flag to see if we needs to disable licensing alerts.
		CRegKey key;
		if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), KEY_READ))
		{
			DWORD dwVal = 1;
			if(ERROR_SUCCESS == key.QueryDWORDValue(_T("DALEnabled"), dwVal))
			{
				if(dwVal == 0)
				{
					CCTRCTXI0(_T("Disable licensing alerts for testing purpose."));
					m_bAlertEnabled = false;
				}
			}
		}

		// Get path to CfgWiz.dat.
		TCHAR szCfgWizDat[MAX_PATH] = {0};
		CNAVInfo NAVInfo;
		::wsprintf(szCfgWizDat, _T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());

		// Do we need to check for factory reseal?
		CNAVOptFileEx cfgWizFile;
		DWORD dwCheckReseal = 0;
		if(cfgWizFile.Init(szCfgWizDat))
		{
			cfgWizFile.GetValue(InstallToolBox::CFGWIZ_Reseal, dwCheckReseal, 0);
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CLicensingAlert::ShouldShowAlert(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag)
{
	CCTRCTXI0(L"Enter");

	bool bWarning = false;
	bool bExpired = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pbShow == NULL)? E_POINTER : S_OK);
		hrx << ((pType == NULL)? E_POINTER : S_OK);		

		*pbShow = FALSE;
		*pType = SYMALERT_TYPE_INVALID;

		// Do not alert if
		//	1. NAV is part of a suite, let parent nags.
		//	2. QA alert disabled flag = 1
		if(m_bLicenseAggregated || (m_bAlertEnabled == false))
		{
			return S_OK;
		}

		// Don't show alert 
		//  1. If CfgWiz has never been launched.
		//  2. If ANY CfgWiz or ActWiz is currently running.
		if(_IsCfgWizRunning() || _IsActWizRunning() || !IsCfgWizRunOnce())
		{
			CCTRACEI(_T("CLicensingAlert::ShouldShowAlert - Cfgwiz or ActWiz is currently running or CfgWiz has never been launched"));
			return S_OK;
		}

		// Do not nag if the system is in the safe mode.
		int nRet = GetSystemMetrics(SM_CLEANBOOT);
		bool bSafeMode = (nRet != 0);
		if(bSafeMode)
		{
			CCTRACEI(_T("CLicensingAlert::ShouldShowAlert - System is in safe mode. No nag."));
			return S_OK;
		}


		hrx << Refresh();

		bool bKilled		= ((m_LicenseAttribute & clt::cltLicenseAttrib_Killed)			
                                    == clt::cltLicenseAttrib_Killed);
		bool bPreActivation = ((m_LicenseAttribute & clt::cltLicenseAttrib_PreActivation)	
                                == clt::cltLicenseAttrib_PreActivation);	
		bool bNotStarted	= ((m_LicenseAttribute & clt::cltLicenseAttrib_NotStarted)		
                                == clt::cltLicenseAttrib_NotStarted);	
		bool bGrace			= ((m_LicenseAttribute & clt::cltLicenseAttrib_Grace)			
                                == clt::cltLicenseAttrib_Grace);	
		bExpired			= ((m_LicenseAttribute & clt::cltLicenseAttrib_Expired)			
                                == clt::cltLicenseAttrib_Expired);
		bWarning			= ((m_LicenseAttribute & clt::cltLicenseAttrib_Subscription_Warning)
                                == clt::cltLicenseAttrib_Subscription_Warning) 
								|| ((m_LicenseAttribute & clt::cltLicenseAttrib_Trial)		
                                == clt::cltLicenseAttrib_Trial);
	   
		if(bPreActivation)
		{
			switch(m_LicenseType)
			{			
            case clt::cltLicenseType_SOS:
				if(bExpired)
				{
					if(bWarning)
					{
						if(bGrace)
						{
							*pType = SYMALERT_TYPE_LicRentalPreActivGraceExpired;
						}
						else
						{
							*pType = SYMALERT_TYPE_LicRentalPreActivTrialExpired;
						}
					}
					else
					{
						*pType = SYMALERT_TYPE_LicRentalPreActivExpired;
					}
				}
				else if(bWarning)
				{
					if(bGrace)
					{
						*pType = SYMALERT_TYPE_LicRentalPreActivGrace;
					}
					else
					{
						*pType = SYMALERT_TYPE_LicRentalPreActivTrial;
					}
				}
				break;

			case clt::cltLicenseType_Unlicensed:
			case clt::cltLicenseType_Activation:
			case clt::cltLicenseType_ESD:
			case clt::cltLicenseType_Beta:
				if(bExpired)
				{
					if(bNotStarted)
					{
						if(IsCTO())
						{
							// Do not alert if trial period for CTO product 
							//  has not beeen started.
							return hr;
						}
					}
					else
					{
						*pType = SYMALERT_TYPE_LicRetailPreActivExpired;
					}
				}
				else if(bWarning)
				{
					*pType = SYMALERT_TYPE_LicRetailPreActiv;
				}
				break;

			case clt::cltLicenseType_TryBuy:
				if(bExpired)
				{
					*pType = SYMALERT_TYPE_LicTryBuyPreActivExpired;
				}
				else if(bWarning)
				{
					*pType = SYMALERT_TYPE_LicTryBuyPreActiv;
				}
				break;

			case clt::cltLicenseType_TryDie:
				if(bExpired)
				{
					*pType = SYMALERT_TYPE_LicTryDiePreActivExpired;
				}
				else if(bWarning)
				{
					*pType = SYMALERT_TYPE_LicTryDiePreActiv;
				}
				break;

			default:
				return hr;
			}

			goto CHECKTIMER;
		}
	
		switch(m_LicenseType)
		{
		case clt::cltLicenseType_SOS:
			if(bKilled)
			{
				*pType = SYMALERT_TYPE_LicRentalKilled;
			}
			else if(bExpired)
			{
				*pType = SYMALERT_TYPE_LicRentalExpired;
			}
			else if(bWarning)
			{
				*pType = SYMALERT_TYPE_LicRentalWarning;
			}
			break;

		case clt::cltLicenseType_Beta:
			if(bExpired)
			{
				*pType = SYMALERT_TYPE_LicTryDiePreActivExpired;
			}
			break;

		default:
			return hr;
		}	
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

CHECKTIMER:
	if(*pType != SYMALERT_TYPE_INVALID)
	{
		// Delay alert only if product is in trial state.
		BOOL bCheckDelayAlert = FALSE;
		if((m_LicenseAttribute & clt::cltLicenseAttrib_Trial) == clt::cltLicenseAttrib_Trial)
			bCheckDelayAlert = TRUE;

		CCTRCTXI1(L"bCheckDelayAlert: %d", bCheckDelayAlert);

		*pbShow = IsTimeForAlert(*pType, bCheckDelayAlert, bCheckTimer);
	}

	CCTRCTXI1(L"Exit. ShouldShow: %d", *pbShow);
	return hr;
}

SIMON_STDMETHODIMP CLicensingAlert::GetLicenseType(DWORD* pType)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pType == NULL) ? E_POINTER : S_OK);
		*pType = m_LicenseType;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CLicensingAlert::GetLicenseAttribute(DWORD* pAttribute)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pAttribute == NULL) ? E_POINTER : S_OK);
		*pAttribute = m_LicenseAttribute;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CLicensingAlert::GetWarningCycle(DWORD* pdwCycle)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pdwCycle == NULL) ? E_POINTER : S_OK);
		*pdwCycle = m_dwCycleLicWarning;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CLicensingAlert::GetExpiredCyle(DWORD* pdwCycle)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pdwCycle == NULL) ? E_POINTER : S_OK);
		*pdwCycle = m_dwCycleLicExpired;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CLicensingAlert::IsAggregated(BOOL* pbAggregated)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pbAggregated == NULL) ? E_POINTER : S_OK);
		*pbAggregated = m_bLicenseAggregated;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CLicensingAlert::Refresh(BOOL bRefreshCache)
{
	STAHLSOFT_HRX_TRY(hr)
	{
        hrx << m_cltHelper.GetLicenseType(m_LicenseType);
        hrx << m_cltHelper.GetLicenseAttributes(m_LicenseAttribute);

		CCTRACEI(_T("CLicensingAlert::Refresh - Type=0x%08X, Attrib=0x%08X"), m_LicenseType, m_LicenseAttribute);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

bool CLicensingAlert::IsTimeForAlert(SYMALERT_TYPE Type, bool bWarning, bool bCheckTimer)
{
	CCTRCTXI0(L"Enter");
	bool bShow = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		// Check to see if we need to delay this alert.
		//  Apply to warning alerts only.
		time_t curtime = 0;	
		if(bWarning && m_dwFirstAlertDelay)
		{
			time(&curtime);
			if((abs(curtime - m_dwInstallTime)) < m_dwFirstAlertDelay)
			{
				CCTRACEI(_T("CLicensingAlert::IsTimeForAlert - Trial alert is delayed for %ds."), m_dwFirstAlertDelay);
				return false;
			}
		}

		// If this is an on demand alert, display alert right away.
		if(!bCheckTimer)
			return true;

		// If we haven't show the same alert a day ago, show it.
		long nLastType = _GetAlertDword(ALERT_LastLicAlert, -1);  
		if(nLastType != (long)Type)
		{
			bShow = true;
			return bShow;
		}

		// Check to see if it is time to display this alert.
		ALERT_MAP item;
		_LookupAlertMap(Type, item);
		long nLastDisplay = _GetAlertDword(item.lpszLastDisplay, 0);  
		long nDefaultCycle = 0;
		long nCycle = 0;

		if(bWarning)
		{
			nDefaultCycle = m_dwCycleLicWarning;
		}
		else
		{
			nDefaultCycle = m_dwCycleLicExpired;
		}

		// Get user's setting for "Notify me again in x days".
		time(&curtime);
		nCycle = _GetAlertDword(item.lpszCycle, nDefaultCycle);
		bShow = (abs(curtime - nLastDisplay) >= nCycle);	
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI1(L"Exit. ShowAlert: %d", bShow);

	return bShow;
}

bool CLicensingAlert::IsCfgWizRunOnce()
{
	bool bRunOnce = true;	

	STAHLSOFT_HRX_TRY(hr)
	{
		// NOTE: If function fails for any reason, 
		//  always assume Cfgwiz has been launched at least once
		//  since we want to display licensing nag.

		CNAVInfo NAVInfo;
		TCHAR szCfgWizDat[MAX_PATH] = {0};
		DWORD dwCfgWizRunOnce = 0;
		CNAVOptFileEx cfgWizFile;

		::wsprintf(szCfgWizDat, _T("%s\\CfgWiz.dat"), NAVInfo.GetNAVDir());

		hrx << (cfgWizFile.Init(szCfgWizDat, FALSE)? S_OK : E_FAIL);

		cfgWizFile.GetValue(InstallToolBox::CFGWIZ_RunOnce, dwCfgWizRunOnce, 1);

		if(dwCfgWizRunOnce == 0)
		{
			bRunOnce = false;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return bRunOnce;
}


bool CLicensingAlert::IsCTO()
{
	bool bCTO = false;

	STAHLSOFT_HRX_TRY(hr)
	{
		CNAVInfo NAVInfo;
		TCHAR szCfgWizDat[MAX_PATH] = {0};
		CNAVOptFileEx CfgWizDatFile;
		DWORD dwVal = 0;
		
		wsprintf(szCfgWizDat, _T("%s\\Cfgwiz.dat"), NAVInfo.GetNAVDir());

		hrx << (CfgWizDatFile.Init(szCfgWizDat, FALSE) ? S_OK : E_FAIL);
		hrx << CfgWizDatFile.GetValue(InstallToolBox::CFGWIZ_OEM, dwVal, 0);
		bCTO = (dwVal == 2) ? true : false;
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)

	return bCTO;
}
