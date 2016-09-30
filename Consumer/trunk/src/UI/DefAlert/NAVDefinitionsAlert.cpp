////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVDefinitionsAlert.cpp

#include "stdafx.h"
#include "NAVDefinitionsAlert.h"
#include "NavSettingsHelperEx.h"
#include "ccScanInterface.h"
#include "navtrust.h"
#include "ccSymCommonClientInfo.h"
#include "atltime.h"
#include "NAVDefutilsLoader.h"
#include "ccScanLoader.h"
#include "ISymMceCmdLoader.h"	// detect optional Media center components
#include "DefHelper.h"


SIMON_STDMETHODIMP CDefinitionsAlert::Init()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		m_dwInstallTime = _GetAlertDword(ALERT_InstallTime, 0, FALSE);

		DWORD dwEnabled = 0;
		READ_SETTING_START()
			READ_SETTING_DWORD(DEFALERT_VirusDefsDelay, m_dwFirstAlertDelay, 0)
			READ_SETTING_DWORD(DEFALERT_MaxDefsAge, m_dwMaxDefsAge, MAX_DEFS_AGE)
			READ_SETTING_DWORD(DEFALERT_EnableOldDefs, dwEnabled, 1)
		READ_SETTING_END

		m_bAlertEnabled = (dwEnabled == 0) ? false : true;

		// Check QA flag to see if we needs to disable licensing alerts.
		CRegKey key;
		if(ERROR_SUCCESS == key.Open(HKEY_LOCAL_MACHINE, _T("Software\\Symantec"), KEY_READ))
		{
			DWORD dwVal = 1;
			if(ERROR_SUCCESS == key.QueryDWORDValue(_T("DADEnabled"), dwVal))
			{
				if(dwVal == 0)
				{
					m_bAlertEnabled = false;
					CCTRACEI(_T("CDefinitionsAlert::Init - Virus Defs alert disabled for testing purpose."));
				}
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CDefinitionsAlert::Refresh(BOOL bRefreshDefsCount)
{
	return S_OK;
}

SIMON_STDMETHODIMP CDefinitionsAlert::ShouldShowAlert(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (pbShow ? S_OK : E_POINTER);
		hrx << (pType ? S_OK : E_POINTER);

		*pbShow = FALSE;
		*pType = SYMALERT_TYPE_INVALID;

		if(m_bAlertEnabled == false)
			return S_OK;

		DWORD nDefsAge = 0;
		hrx << CDefHelper::GetDefAge(nDefsAge);

		CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - DefsAge=%d"), nDefsAge);

		// Are the defs more than 2 weeks old?
		if(nDefsAge > m_dwMaxDefsAge) 
		{
			BOOL bOnScan = ((dwFlag & SYMALERT_TYPE_OldDefsScan) == SYMALERT_TYPE_OldDefsScan);
			if(bOnScan)
			{
				*pType = SYMALERT_TYPE_OldDefsScan;
			}
			else
			{
				// Check to see if an alert on scan is currently active.
				StahlSoft::CSmartHandle smAlertMutex;
				smAlertMutex = OpenMutex(SYNCHRONIZE, FALSE, SZ_NAV_OLDDEFS_ONSCAN_ALERT_MUTEX);
				if(smAlertMutex != NULL)
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress alert on timer because an alert on scan is currently active"));
					return S_OK;
				}

				// Check to see if ALU is running.
				StahlSoft::CSmartHandle shALUMutex;
				shALUMutex = ::OpenMutex(SYNCHRONIZE, FALSE, _T("Symantec.LuComServer.Running"));				
				if(shALUMutex)
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress on timer alert because ALU is currently running."));
					return FALSE;
				}

				// Check to see if actwiz or cfgwiz is currently running.
				if(_IsCfgWizRunning() || _IsActWizRunning())
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress alert because CfgWiz or ActWiz is currently active"));
					return S_OK;
				}

				// Under Windows MCE surpress alert if user is watching video.
				SymMCE::ISymMceCmdLoader mce;
				if (mce.IsMceVideo())
				{
					CCTRACEI(_T("CDefinitionsAlert::ShouldShowAlert - Suppress alert because User is currently watching video on MCE"));
					return S_OK;
				}

				*pType = SYMALERT_TYPE_OldDefs;
			}

			if(*pType != SYMALERT_TYPE_INVALID)
			{
				if(bCheckTimer)
					*pbShow = IsTimeForAlert(*pType);
				else
					*pbShow = TRUE;
			}
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

BOOL CDefinitionsAlert::IsTimeForAlert(SYMALERT_TYPE Type)
{
	BOOL bShow = FALSE;

	// Check to see if we need to delay this alert.
	time_t curtime = 0;	
	if(Type == SYMALERT_TYPE_OldDefs && m_dwFirstAlertDelay)
	{
		time(&curtime);
		if((abs(curtime - m_dwInstallTime)) < m_dwFirstAlertDelay)
		{
			CCTRACEI(_T("CDefinitionsAlert::IsTimeForAlert - Definitions alert on-timer is delayed for %ds."), m_dwFirstAlertDelay);
			return bShow;
		}
	}
	// Check if the cycle says to show for this alert.	
	ALERT_MAP item;
	_LookupAlertMap(Type, item);

	time(&curtime);
	long nLastDisplay = 0;
	long nCycle = 0;

	nLastDisplay = _GetAlertDword(item.lpszLastDisplay, 0);  
	nCycle = _GetAlertDword(item.lpszCycle, (Type == SYMALERT_TYPE_OldDefs)? _1_DAY : 0); 
	bShow = (abs(curtime - nLastDisplay) >= nCycle);

	return bShow;
}