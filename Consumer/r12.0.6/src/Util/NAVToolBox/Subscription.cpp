#include "stdafx.h"
#include "subscription.h"
//#include "GlobalMutex.h"
#include "DRMNamedProperties.h"
#include "Subscription_static.h"

// For renew
#include "NAVTrust.h"
#include "NAVLicense_h.h"
#include "NAVLicense_i.c"
#include <atltime.h>

CSubscription::CSubscription(void)
{
    resetSubData();
}

CSubscription::~CSubscription(void)
{
}

// Lock the critical section before entering!
void CSubscription::resetSubData (void)
{
	m_Data.bCanRenewSub = true;
    m_Data.LicenseState = DJSMAR00_LicenseState_Violated;
    m_Data.LicenseType = DJSMAR_LicenseType_Violated;
    m_Data.LicenseZone = DJSMAR_LicenseZone_Violated;
    m_Data.VendorID = DJSMAR00_VendorID_XtreamLok;
    m_Data.bLicenseValid = false;
    m_Data.lDaysLeft = 0;
    m_Data.dwEndDate = 0;
    m_Data.bWarning = true;
    m_Data.bExpired = true;
    strcpy ( m_szSubDate, "/0");
}

bool CSubscription::init (void)
{
    try
	{
        HRESULT hr = S_OK;

		// Check for a valid digital signature on the COM Server before loading it
		if ( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(CLSID_NAVLicenseInfo))
        {
		    if ( SUCCEEDED ( m_spNavLicense.CoCreateInstance(CLSID_NAVLicenseInfo, NULL, CLSCTX_ALL)))
            {
                hr = m_spNavLicense.QueryInterface(&m_spSymScriptSafe);
                if ( FAILED (hr))
                {
                    CCTRACEE ( "CSubscription::init - failed QI scriptsafe 0x%x", hr );
                    return false;
                }

		        hr = m_spSymScriptSafe->SetAccess(ISYMSCRIPTSAFE_UNLOCK_KEYA, ISYMSCRIPTSAFE_UNLOCK_KEYB);
                if ( FAILED (hr))
                {
                    CCTRACEE ( "CSubscription::init - failed access scriptsafe 0x%x", hr );
                    return false;
                }
            }
        }
	}
	catch(_com_error& e)
	{
        CCTRACEE(_T("CSubscription::init - Exception while launching subscription wizard 0x%08X"), e.Error());
        return false;
	}
    
    return true;
}
void CSubscription::readSubscription(void)
{
    resetSubData ();

    if (!m_spNavLicense && !m_spSymScriptSafe && !init ())
    {
        CCTRACEE ("CSubscription::readSubscription - Failed to init");
        return;
    }

    DWORD dwDaysWarning = 30;
    HRESULT hr = S_OK;

    // Licensing
    hr = m_spNavLicense->GetLicenseType (&m_Data.LicenseType);
    if ( FAILED (hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed GetLicenseType 0x%x", hr );
        return;
    }

    hr = m_spNavLicense->GetLicenseState (&m_Data.LicenseState);
    if ( FAILED (hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed GetLicenseState 0x%x", hr );
        return;
    }

    hr = m_spNavLicense->GetLicenseZone ((long*)&m_Data.LicenseZone);
    if ( FAILED (hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed GetLicenseZone 0x%x", hr );
        return;
    }

    hr = m_spNavLicense->get_LicenseVendorId ((LONG*)&m_Data.VendorID);
    if ( FAILED (hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed get_LicenseVendorId 0x%x", hr );
        return;
    }

    // Subscription
    BOOL bSubscriptionActive;
    hr = m_spNavLicense->get_ShouldCheckSubscription(&bSubscriptionActive);
    if(FAILED(hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed get_ShouldCheckSubscription 0x%x", hr );
        return;
    }

    CCTRACEI(_T("CSubscription::readSubscription - Should check subscription: %d"), (int)bSubscriptionActive);

    if(bSubscriptionActive)
    {
        hr = m_spNavLicense->get_SubscriptionRemaining (&m_Data.lDaysLeft);
        CCTRACEI(_T("CSubscription::readSubscription - Subscription Days Remaining: %d"), m_Data.lDaysLeft);
        if ( FAILED (hr))
        {
            CCTRACEE ( "CSubscription::readSubscription - failed get_SubscriptionRemaining 0x%x", hr );
            return;
        }
    }
    else
    {
        hr = m_spNavLicense->get_LicenseDaysRemaining(&m_Data.lDaysLeft);
        CCTRACEI(_T("CSubscription::readSubscription - License Days Remaining: %d"), m_Data.lDaysLeft);
        if ( FAILED (hr))
        {
            CCTRACEE ( "CSubscription::readSubscription - failed get_LicenseDaysRemaining 0x%x", hr );
            return;
        }
    }


    // DRM NO LONGER SUPPORTS THIS.
/*  hr = m_spNavLicense->get_SubscriptionDateDW (&m_Data.dwEndDate);
    if ( FAILED (hr))
    {
    CCTRACEE ( "CSubscription::readSubscription - failed get_SubscriptionDateDW 0x%x", hr );
    return;
    }
*/

    SYSTEMTIME sysTime;
    ATL::CTime curTime = ATL::CTime::GetCurrentTime();
    ATL::CTimeSpan tSpan(m_Data.lDaysLeft,0,0,0);
    curTime += tSpan;
    if(!curTime.GetAsSystemTime(sysTime))
    {
        CCTRACEE(_T("CSubscription::readSubscription - failed to convert days remaining to datestamp."));
        return;
    }

    DATE varTime;
    if(SystemTimeToVariantTime(&sysTime, &varTime) == 0)
    {
        CCTRACEE(_T("CSubscription::readSubscription - failed to convert datestamp to VariantTime."));
        return;
    }

    m_Data.dwEndDate = (DWORD)varTime;  // Fixes an odd conversion bug.

    hr = m_spNavLicense->get_CanRenewSubscription (&m_Data.bCanRenewSub);
    if ( FAILED (hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed GetLicenseType 0x%x", hr );
        return;
    }

    hr = m_spNavLicense->get_SubscriptionWarning (&m_Data.bWarning);
    if ( FAILED (hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed get_SubscriptionWarning 0x%x", hr );
        return;
    }

    hr = m_spNavLicense->get_SubscriptionExpired (&m_Data.bExpired);
    if ( FAILED (hr))
    {
        CCTRACEE ( "CSubscription::readSubscription - failed get_SubscriptionExpired 0x%x", hr );
        return;
    }

    CCTRACEI( "License: Type=%d, State=%d, Zone=%d, Warning=%d, Expired=%d", 
              m_Data.LicenseType,
              m_Data.LicenseState,
              m_Data.LicenseZone,
              m_Data.bWarning,
              m_Data.bExpired);
    
	m_Data.bLicenseValid = (DJSMAR00_LicenseState_EXPIRED != m_Data.LicenseState &&
		                    DJSMAR00_LicenseState_Violated != m_Data.LicenseState);

    if(GetDateFormatA ( LOCALE_USER_DEFAULT,
                     DATE_SHORTDATE, 
                     &sysTime,
                     NULL,
                     m_szSubDate,
                     64 ) == 0)
    {
        DWORD dwErr = GetLastError();
        if(dwErr == ERROR_INSUFFICIENT_BUFFER)
        {
            CCTRACEE(_T("CSubscription::MakeStatus - Insufficient buffer."));
        }
        else if(dwErr == ERROR_INVALID_FLAGS)
        {
            CCTRACEE(_T("CSubscription::MakeStatus - Invalid flags."));
        }
        else if(dwErr == ERROR_INVALID_PARAMETER)
        {
            CCTRACEE(_T("CSubscription::MakeStatus - Invalid parameter(s)."));
        }
        else
        {
            CCTRACEE(_T("CSubscription::MakeStatus - Other error (%d)."), dwErr);
        }
    }

	m_Data.szEndDate = m_szSubDate;

    CCTRACEI(_T("CSubscription::MakeStatus - DaysLeft=%d, EndDate=%s (%d)"), m_Data.lDaysLeft, m_Data.szEndDate,
                m_Data.dwEndDate);

}

CSubscription::LICENSE_SUB_DATA CSubscription::GetData ()
{
    ccLib::CSingleLock lock (&m_critAccess);
    readSubscription();
    return m_Data;
}

// We use the COM licensing object so we don't have to link in SIMON stuff!!!
//
HRESULT CSubscription::LaunchSubscriptionWizard(HWND hWndParent /*NULL*/)
{
    if (!m_spNavLicense && !m_spSymScriptSafe && !init ())
    {
        CCTRACEE ("CSubscription::LaunchSubscriptionWizard - Failed to init");
        return E_FAIL;
    }

    if ( NULL == hWndParent )
        hWndParent = ::GetDesktopWindow ();

	try
	{
        HRESULT hr = S_OK;
        hr = m_spNavLicense->LaunchSubscriptionWizard((long) hWndParent);
		if ( FAILED (hr))
        {
            CCTRACEE ("CSubscription::LaunchSubscriptionWizard - failed LaunchSubscriptionWizard 0x%x", hr);
            return hr;
        }
	}
	catch(_com_error& e)
	{
        CCTRACEE(_T("CSubscription::LaunchSubscriptionWizard - Exception while launching subscription wizard 0x%08X"), e.Error());
        return e.Error();
	}

	return S_OK;
}

HRESULT CSubscription::HasUserAgreedToEULA(BOOL* pbAgreed)
{
    if (!m_spNavLicense && !m_spSymScriptSafe && !init ())
    {
        CCTRACEE ("CSubscription::HasUserAgreedToEULA - Failed to init");
        return E_FAIL;
    }

    try
	{
        HRESULT hr = S_OK;
        hr = m_spNavLicense->get_HasUserAgreedToEULA(pbAgreed);
		if ( FAILED (hr))
        {
            CCTRACEE ("CSubscription::HasUserAgreedToEULA - failed HasUserAgreedToEULA 0x%x", hr);
            return hr;
        }
	}
	catch(_com_error& e)
	{
        CCTRACEE(_T("CSubscription::HasUserAgreedToEULA - Exception while HasUserAgreedToEULA 0x%08X"), e.Error());
        return e.Error();
	}

	return S_OK;
}

HRESULT CSubscription::IsCfgWizFinished(BOOL* pbFinished)
{
    if (!m_spNavLicense && !m_spSymScriptSafe && !init ())
    {
        CCTRACEE ("CSubscription::IsCfgWizFinished - Failed to init");
        return E_FAIL;
    }

    try
	{
        HRESULT hr = S_OK;
        hr = m_spNavLicense->get_CfgWizFinished(pbFinished);
		if ( FAILED (hr))
        {
            CCTRACEE ("CSubscription::IsCfgWizFinished - failed IsCfgWizFinished 0x%x", hr);
            return hr;
        }
	}
	catch(_com_error& e)
	{
        CCTRACEE(_T("CSubscription::IsCfgWizFinished - Exception while IsCfgWizFinished 0x%08X"), e.Error());
        return e.Error();
	}

	return S_OK;
}