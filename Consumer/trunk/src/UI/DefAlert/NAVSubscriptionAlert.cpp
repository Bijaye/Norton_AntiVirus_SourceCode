////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVSubscriptionAlert.cpp

#include "StdAfx.h"
#include "NAVSubscriptionAlert.h"
#include "NAVSettingsHelperEx.h"

#include "NAVSecurity.h"

CSubscriptionAlert::CSubscriptionAlert(void):
m_bAlertable(true)
,m_dwDaysRemaining(0)
,m_dwCycleSubExpired(_1_DAY)
,m_dwCycleSubWarning(7 * _1_DAY)
,m_dwStartSubWarning(30 * _1_DAY)
,m_SubStatus(STATUS_SUBSCRIPTION_OK)
,m_EndDate(0)
,m_bCanAutoRenew(FALSE)
,m_dwCycleSubExpiredAutoRenew(_1_DAY)
,m_dwCycleSubWarningAutoRenew(7 * _1_DAY)
,m_bLicenseAggregated(false)
{
}

CSubscriptionAlert::~CSubscriptionAlert(void)
{
}

SIMON_STDMETHODIMP CSubscriptionAlert::Init()
{
	STAHLSOFT_HRX_TRY(hr)
	{
        if(!SUCCEEDED(hr = m_cltHelper.Initialize()))
        {
            CCTRCTXE1(_T("Could not initialized CLT Helper. HR = 0x%08X"), hr);
            m_bAlertable = FALSE;
            return hr;
        }

		DWORD dwEnabled = 1;
		DWORD dwAggregated = 0;
		READ_SETTING_START()
			READ_SETTING_DWORD(DEFALERT_StartSubWarning, m_dwStartSubWarning, 30 * _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubWarning, m_dwCycleSubWarning, 7 * _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubExpired, m_dwCycleSubExpired, _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubWarningAutoRenew, m_dwCycleSubWarningAutoRenew, 7 * _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubExpiredAutoRenew, m_dwCycleSubExpiredAutoRenew, _1_DAY)
			READ_SETTING_DWORD(LICENSE_Aggregated, dwAggregated, 0)
		READ_SETTING_END

		m_bLicenseAggregated = (dwAggregated == 1) ? true : false;

		CCTRCTXI1(L"CycleSubWarning: %d", m_dwCycleSubWarning);
		CCTRCTXI1(L"CycleSubExpired: %d", m_dwCycleSubExpired);
		CCTRCTXI1(L"CycleSubWarningAutoRenew: %d", m_dwCycleSubWarningAutoRenew);
		CCTRCTXI1(L"CycleSubExpiredAutoRenew: %d", m_dwCycleSubExpiredAutoRenew);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CSubscriptionAlert::ShouldShowAlert(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag)
{
	CCTRCTXI0(L"Enter");
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (pType ? S_OK : E_POINTER);
		hrx << (pbShow ? S_OK : E_POINTER);

		// Initialize return values.
		*pType = SYMALERT_TYPE_INVALID;
		*pbShow = FALSE;

		// Get subscription information.
		hrx << Refresh();

		if(m_bLicenseAggregated)
		{
			CCTRCTXI0(L"Suppress alert because NAV is a child product");
			return S_OK;
		}

		if(!m_bAlertable)
		{
			CCTRCTXI0(L"Suppress alert because CLT says so");
			return S_OK;
		}

		BOOL bOnScan = ((dwFlag & SYMALERT_TYPE_SubExpiredScan) == SYMALERT_TYPE_SubExpiredScan) ||
						((dwFlag & SYMALERT_TYPE_SubExpiredAutoRenewScan) == SYMALERT_TYPE_SubExpiredAutoRenewScan);

		switch(m_SubStatus)
		{
		case STATUS_SUBSCRIPTION_EXPIRED:
			{
				// There are two possible alerts.
				//  Check to see which one the caller is interested in.
				if(bOnScan)
				{
					if(m_bCanAutoRenew)
					{
						*pType = SYMALERT_TYPE_SubExpiredAutoRenewScan;
					}
					else
					{
						*pType = SYMALERT_TYPE_SubExpiredScan;
					}
				}
				else
				{
					// Check to see if an alert on scan is currently active.
					StahlSoft::CSmartHandle smMutex;
					smMutex = OpenMutex(SYNCHRONIZE, FALSE, SZ_NAV_SUBEXPIRED_ONSCAN_ALERT_MUTEX);
					if(smMutex != NULL)
					{
						CCTRCTXI0(L"Suppress alert on timer because an alert on scan is currently active.");
						return S_OK;
					}

					//  Check to see if Actwiz or any Cfgwiz is currently running.            
					if(_IsCfgWizRunning() || _IsActWizRunning())
					{
						CCTRCTXI0(L"Suppress alert because ActWiz or CfgWiz is currently running.");
						return S_OK;
					}

					if(m_bCanAutoRenew)
					{
						*pType = SYMALERT_TYPE_SubExpiredAutoRenew;
					}
					else
					{
						*pType = SYMALERT_TYPE_SubExpired;
					}
				}
			}
			break;

		case STATUS_SUBSCRIPTION_WARNING:
			// Warning alert is displayed on timer only, not on scan.
			if(bOnScan == FALSE)
			{
				if(m_bCanAutoRenew)
				{
					*pType = SYMALERT_TYPE_SubWarningAutoRenew;
				}
				else
				{
					*pType = SYMALERT_TYPE_SubWarning;
				}
			}
			break;

		default:
			return hr;
			break;
		}


		if(*pType != SYMALERT_TYPE_INVALID)
		{
			if(bCheckTimer)
				*pbShow = IsTimeForAlert(*pType);
			else
				*pbShow = TRUE;
		}
	}
	STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
	CCTRCTXI2(L"Exit. ShouldShowAlert: %d. Type: %d", *pbShow, *pType);
	return hr;
}


SIMON_STDMETHODIMP CSubscriptionAlert::Refresh(BOOL bRefreshCache)
{
    bool bExpired = true;
    bool bWarning = false;

    m_SubStatus = STATUS_SUBSCRIPTION_OK;

    STAHLSOFT_HRX_TRY(hr)
    {
        hrx << m_cltHelper.IsSubscriptionExpired(bExpired);
        hrx << m_cltHelper.IsSubscriptionWarningPeriod(bWarning);
        hrx << m_cltHelper.CanAutoRenewSubscription(m_bCanAutoRenew);
		hrx << m_cltHelper.ShouldShowAlerts(m_bAlertable);
		
        CCTRACEI(CCTRCTX _T("Expired: %d"), bExpired);
		CCTRACEI(CCTRCTX _T("Warning: %d"), bWarning);
		CCTRACEI(CCTRCTX _T("CanAutoRenew: %d"), m_bCanAutoRenew);
		CCTRACEI(CCTRCTX _T("Alertable: %d"), m_bAlertable);

        if(bExpired)
        {
            m_SubStatus = STATUS_SUBSCRIPTION_EXPIRED;
        } 
        else if(bWarning)
        {
            m_SubStatus = STATUS_SUBSCRIPTION_WARNING;			
        }
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

BOOL CSubscriptionAlert::IsTimeForAlert(SYMALERT_TYPE Type)
{
	CCTRCTXI0(L"Enter");
	BOOL bShow = FALSE;

	switch(Type)
	{
	case SYMALERT_TYPE_SubWarning:
		{
			// Display the warning alert only if
			//  it's 30 days before subscription expires.
			if(m_dwDaysRemaining > (m_dwStartSubWarning / _1_DAY))
			{
				CCTRCTXI0(L"Skip subscription warning alert because it can be showned only if it is 30 days or less before subscription expired.");
				return S_OK;	
			}

			// Display alerts every 7 days.
			time_t curtime = 0;
			time(&curtime);
			LONG nLastDisplay = _GetAlertDword(ALERT_LastDisplaySubWarning, 0);
			bShow = (abs(curtime - nLastDisplay) >= m_dwCycleSubWarning);
		}
		break;

	case SYMALERT_TYPE_SubWarningAutoRenew:
		{
			time_t curtime = 0;
			time(&curtime);
			LONG nLastDisplay = _GetAlertDword(ALERT_LastDisplaySubWarningAutoRenew, 0);
			bShow = (abs(curtime - nLastDisplay) >= m_dwCycleSubWarningAutoRenew);
		}
		break;

	case SYMALERT_TYPE_SubExpired:
	case SYMALERT_TYPE_SubExpiredScan:
		{
			// Check to see if users previously selected
			//  "Don't show this alert again".
			ALERT_MAP item;
			_LookupAlertMap(Type, item);
			
			LONG nCycle = 0;
			LONG nDefaultCycle = _1_DAY;

			if(Type == SYMALERT_TYPE_SubExpiredScan)
				nDefaultCycle = 0;

			nCycle = _GetAlertDword(item.lpszCycle, nDefaultCycle); 
			if(nCycle == -1)
			{
				return S_OK;
			}

			// Check to see if it has been more than 1 cycle 
			//  since the last time we display this alert.
			time_t curtime = 0;
			time(&curtime);
			LONG nLastDisplay = 0;
			nLastDisplay = _GetAlertDword(item.lpszLastDisplay, 0);  
			bShow = (abs(curtime - nLastDisplay) >= nCycle);
		}
		break;

	case SYMALERT_TYPE_SubExpiredAutoRenewScan:
		bShow = TRUE;
		break;

	case SYMALERT_TYPE_SubExpiredAutoRenew:
		{
			time_t curtime = 0;
			time(&curtime);
			LONG nLastDisplay = _GetAlertDword(ALERT_LastDisplaySubExpiredAutoRenew, 0);
			bShow = (abs(curtime - nLastDisplay) >= m_dwCycleSubExpiredAutoRenew);
		}
		break;

	default:
		// Wrong type!!!
		break;
	}

	CCTRCTXI2(L"Exit. Type: %d. Show: %d", Type, bShow);

	return bShow;
}

SIMON_STDMETHODIMP CSubscriptionAlert::GetSubscriptionStatus(STATUS_SUBSCRIPTION* pStatus)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pStatus == NULL) ? E_POINTER : S_OK);
		*pStatus = m_SubStatus;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CSubscriptionAlert::GetSubscriptionDaysRemaining(long* pnDaysRemaining)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pnDaysRemaining == NULL) ? E_POINTER : S_OK);
		*pnDaysRemaining = (long) m_dwDaysRemaining;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CSubscriptionAlert::GetWarningCycle(DWORD* pdwCycle)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pdwCycle == NULL) ? E_POINTER : S_OK);
		*pdwCycle = m_dwCycleSubWarning;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CSubscriptionAlert::GetExpiredCyle(DWORD* pdwCycle)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pdwCycle == NULL) ? E_POINTER : S_OK);
		*pdwCycle = m_dwCycleSubExpired;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CSubscriptionAlert::GetSubscriptionEndDate(DATE* pnDate)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << ((pnDate == NULL) ? E_POINTER : S_OK);
		*pnDate = m_EndDate;
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}
