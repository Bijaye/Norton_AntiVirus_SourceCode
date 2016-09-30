// NAVSubscriptionAlert.cpp

#include "StdAfx.h"
#include "NAVSubscriptionAlert.h"
#include "NAVSettingsHelperEx.h"
#include "NAVLicenseNames.h"

// PEP Headers
#define _V2AUTHORIZATION_SIMON
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"
#include "PepClientBase.h"

#include "NAVSecurity.h"

CSubscriptionAlert::CSubscriptionAlert(void):
m_bAlertable(FALSE)
,m_dwDaysRemaining(0)
,m_dwCycleSubExpired(_1_DAY)
,m_dwCycleSubWarning(7 * _1_DAY)
,m_dwStartSubWarning(30 * _1_DAY)
,m_SubStatus(STATUS_SUBSCRIPTION_OK)
,m_EndDate(0)
,m_bCanAutoRenew(FALSE)
,m_dwCycleSubWarningAutoRenew(_1_DAY)
,m_dwCycleSubExpiredAutoRenew(_1_DAY)
{
}

CSubscriptionAlert::~CSubscriptionAlert(void)
{
}

SIMON_STDMETHODIMP CSubscriptionAlert::Init()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwEnabled = 1;
		READ_SETTING_START()
			READ_SETTING_DWORD(DEFALERT_StartSubWarning, m_dwStartSubWarning, 30 * _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubWarning, m_dwCycleSubWarning, 7 * _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubExpired, m_dwCycleSubExpired, _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubWarningAutoRenew, m_dwCycleSubWarningAutoRenew, _1_DAY)
			READ_SETTING_DWORD(DEFALERT_CycleSubExpiredAutoRenew, m_dwCycleSubExpiredAutoRenew, _1_DAY)
			READ_SETTING_DWORD(DEFALERT_EnableSubsAlert, dwEnabled, 1)
		READ_SETTING_END

		m_bAlertable = (dwEnabled == 1) ? TRUE : FALSE;

		CCTRACEI(_T("CSubscriptionAlert::Init - m_bAlertable: %d"), m_bAlertable);
	}
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}

SIMON_STDMETHODIMP CSubscriptionAlert::ShouldShowAlert(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << (pType ? S_OK : E_POINTER);
		hrx << (pbShow ? S_OK : E_POINTER);

		// Initialize return values.
		*pType = SYMALERT_TYPE_INVALID;
		*pbShow = FALSE;

		// Get subscription information.
		hrx << Refresh();

		if(!m_bAlertable)
		{
			CCTRACEI(_T("CSubscriptionAlert::ShouldShowAlert - Suppress alert due to DEFALERT:EnableSubsAlert=0"));
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
						CCTRACEI(_T("CSubscriptionAlert::ShouldShowAlert - Suppress alert on timer because an alert on scan is currently active."));
						return S_OK;
					}

					//  Check to see if Actwiz or any Cfgwiz is currently running.            
					if(_IsCfgWizRunning() || _IsActWizRunning())
					{
						CCTRACEI(_T("CSubscriptionAlert::ShouldShowAlert - Suppress alert because ActWiz or CfgWiz is currently running."));
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
	STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
}


SIMON_STDMETHODIMP CSubscriptionAlert::Refresh(BOOL bRefreshCache)
{
    BOOL bExpired = TRUE;
    BOOL bWarning = FALSE;

    m_SubStatus = STATUS_SUBSCRIPTION_OK;

    STAHLSOFT_HRX_TRY(hr)
    {
        // Getting licensing and subscription properties needed to enable product features
        CPEPClientBase pepBase;
        hrx << pepBase.Init();
        pepBase.SetContextGuid(clt::pep::CLTGUID);
        pepBase.SetComponentGuid(V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT);
        pepBase.SetPolicyID(clt::pep::POLICY_ID_QUERY_SUBSCRIPTION_RECORD);
        pepBase.SetCollectionDWORD(clt::pep::SZ_PROPERTY_DISPOSITION,clt::pep::DISPOSITION_NONE);
        pepBase.SetCollectionDWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION,SUBSCRIPTION_STATIC::DISPOSITION_NONE);

        hrx << pepBase.QueryPolicy();

        // Getting Subscription disposition
        DWORD dwSubDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;
        hrx << pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, (DWORD &)dwSubDisposition, SUBSCRIPTION_STATIC::DISPOSITION_NONE);
        hrx << pepBase.GetValueFromCollection(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING, (DWORD &)m_dwDaysRemaining, SUBSCRIPTION_STATIC::DISPOSITION_NONE);

		bExpired = DWORD_HAS_FLAG(dwSubDisposition,SUBSCRIPTION_STATIC::DISPOSITION_SUBSCRIPTION_EXPIRED);
		bWarning = DWORD_HAS_FLAG(dwSubDisposition,SUBSCRIPTION_STATIC::DISPOSITION_SUBSCRIPTION_WARNING);
		m_bCanAutoRenew = DWORD_HAS_FLAG(dwSubDisposition,SUBSCRIPTION_STATIC::DISPOSITION_SUBSCRIPTION_AUTO_RENEW);
		
        CCTRACEI(CCTRCTX _T("Disposition: %x"), dwSubDisposition);
        CCTRACEI(CCTRCTX _T("Expired: %d"), bExpired);
		CCTRACEI(CCTRCTX _T("Warning: %d"), bWarning);
		CCTRACEI(CCTRCTX _T("CanAutoRenew: %d"), m_bCanAutoRenew);

        if(bExpired)
        {
            m_SubStatus = STATUS_SUBSCRIPTION_EXPIRED;
            return hr;
        }

        if(bWarning)
        {
            m_SubStatus = STATUS_SUBSCRIPTION_WARNING;
        }
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
    
	return hr;
}

BOOL CSubscriptionAlert::IsTimeForAlert(SYMALERT_TYPE Type)
{
	BOOL bShow = FALSE;

	try
	{
		switch(Type)
		{
		case SYMALERT_TYPE_SubWarning:
			{
				// Display the warning alert only if
				//  it's 30 days before subscription expires.
				if(m_dwDaysRemaining > (m_dwStartSubWarning / _1_DAY))
				{
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
	}
	catch(...)
	{
		// Ignore all errors.
	}

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
