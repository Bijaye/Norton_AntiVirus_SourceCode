#include "stdafx.h"
#include "NAVStatus.h"
#include "NAVErrorResource.h"

#include "TraceHR.h"

//
// Code that implements the INAVStatus old interface.
// I moved it to it's own CPP because there's soooo much of it.
//
STDMETHODIMP CNAVStatus::get_APStatus(long *pVal)
{
    return getLong ( AVStatus::propAPStatus, pVal );
}

STDMETHODIMP CNAVStatus::get_VirusDefDate(BSTR *pVal)
{
    return getString ( AVStatus::propDefsDate, pVal );
}

STDMETHODIMP CNAVStatus::get_VirusDefAge(long *pVal)
{
    return getLong ( AVStatus::propDefsAge, pVal );
}

STDMETHODIMP CNAVStatus::get_APSpywareStatus(long *pVal)
{
    return getLong ( AVStatus::propAPSpywareStatus, pVal );
}

STDMETHODIMP CNAVStatus::get_SpywareCategory(long *pVal)
{
    return getLong ( AVStatus::propSpywareCat, pVal );
}

STDMETHODIMP CNAVStatus::put_UseSpyware(BOOL pVal)
{
	m_bUseSpyware = pVal;
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_SpywareCanEnable(BOOL *pVal)
{
    return getLong ( AVStatus::propSpywareCanEnable, (long*)pVal );
}

//
// Return if user has requested to be notified of AP status.
//
STDMETHODIMP CNAVStatus::get_UseAP(BOOL *pVal)
{
	*pVal = m_bUseAP;
	return S_OK;
}

//
// User can choose to be notified of AP status changes
//
STDMETHODIMP CNAVStatus::put_UseAP(BOOL newVal)
{
	m_bUseAP = newVal;
	return S_OK;
}

//
// Return if user has requested to be notified of Virus Def date status.
//
STDMETHODIMP CNAVStatus::get_UseVirusDef(BOOL *pVal)
{
	*pVal = m_bUseVirusDef;
	return S_OK;
}

//
// User can choose to be notified of virus def status changes
//
STDMETHODIMP CNAVStatus::put_UseVirusDef(BOOL newVal)
{
	m_bUseVirusDef = newVal;
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_ALUStatus(long *pVal)
{
	return getLong ( AVStatus::propALUStatus, pVal );
}

STDMETHODIMP CNAVStatus::get_UseALU(BOOL *pVal)
{
	*pVal = m_bUseALU;
	return S_OK;
}

STDMETHODIMP CNAVStatus::put_UseALU(BOOL newVal)
{
	m_bUseALU = newVal;
	return S_OK;
}

// Quarantine not implemented (never was)
//
STDMETHODIMP CNAVStatus::get_UseQuar(BOOL *pVal)
{
	return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

// Quarantine not implemented (never was)
//
STDMETHODIMP CNAVStatus::put_UseQuar(BOOL newVal)
{
	return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

// Quarantine not implemented (never was)
//
STDMETHODIMP CNAVStatus::get_QuarFileCount(long *pVal)
{
    return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

STDMETHODIMP CNAVStatus::get_VirusDefCount(long *pVal)
{
    return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

STDMETHODIMP CNAVStatus::get_UseVirusDefSubscription(BOOL *pVal)
{
	*pVal = m_bUseVirusDefSub;
	return S_OK;
}

STDMETHODIMP CNAVStatus::put_UseVirusDefSubscription(BOOL newVal)
{
	m_bUseVirusDefSub = newVal;
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_VirusDefSubDaysLeft(long *pVal)
{
    return getLong ( AVStatus::propSubDaysLeft, pVal );
}

// Returns Enabled, NotInstalled (same as Licensing status)
//
STDMETHODIMP CNAVStatus::get_VirusDefSubStatus(long *pVal)
{
	return getLong ( AVStatus::propLicStatus, pVal );
}


// Returns None, OK, Warning, Expired
//
STDMETHODIMP CNAVStatus::get_VirusDefSubState(long *pVal)
{
    enum SubscriptionStatusTypes
    {
        SUB_UNKNOWN = 0,
        SUB_OK,
        SUB_WARNING,
        SUB_EXPIRED
    };

    long lExpired = 0;
    long lWarning = 0;
    HRESULT hr = S_OK;
    *pVal = SUB_UNKNOWN;

    hr = getLong ( AVStatus::propSubExpired, &lExpired );
    
    if ( FAILED (hr))
        return hr;

    hr = getLong ( AVStatus::propSubWarning, &lWarning );

    if ( FAILED (hr))
        return hr;

    if ( lExpired )
    {
        *pVal = SUB_EXPIRED;
        return hr;
    }

    if ( lWarning )
    {
        *pVal = SUB_WARNING;
        return hr;
    }

    *pVal = SUB_OK;
    return hr;
}

// Rescue Disk not implemented (never was)
//
STDMETHODIMP CNAVStatus::get_UseRescueDisk(BOOL *pVal)
{
	return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

// Rescue Disk not implemented (never was)
//
STDMETHODIMP CNAVStatus::put_UseRescueDisk(BOOL newVal)
{
	return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

// Rescue Disk not implemented (never was)
//
STDMETHODIMP CNAVStatus::get_RescueDiskStatus(long *pVal)
{
	return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

// Rescue Disk not implemented (never was)
//
STDMETHODIMP CNAVStatus::get_RescueDiskLastRunYear(long *pVal)
{
    return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

// Rescue Disk not implemented (never was)
//
STDMETHODIMP CNAVStatus::get_RescueDiskLastRunMonth(long *pVal)
{
    return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

// Rescue Disk not implemented (never was)
//
STDMETHODIMP CNAVStatus::get_RescueDiskLastRunDay(long *pVal)
{
    return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

STDMETHODIMP CNAVStatus::get_UseScriptBlocking(BOOL *pVal)
{
	*pVal = FALSE;
	return S_OK;
}

STDMETHODIMP CNAVStatus::put_UseScriptBlocking(BOOL newVal)
{
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_UseEmail(BOOL *pVal)
{
	*pVal = m_bUseEmail;
	return S_OK;
}

STDMETHODIMP CNAVStatus::put_UseEmail(BOOL newVal)
{
	m_bUseEmail = newVal;
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_ScriptBlockingStatus(long *pVal)
{
    *pVal = AVStatus::statusNotInstalled;
    return S_OK;
}

STDMETHODIMP CNAVStatus::get_EmailStatus(long *pVal)
{
	return getLong ( AVStatus::propEmailStatus, pVal );
}

STDMETHODIMP CNAVStatus::get_VirusDefStatus(long *pVal)
{
	return getLong ( AVStatus::propDefsStatus, pVal );
}

STDMETHODIMP CNAVStatus::get_UseFullSystemScan(BOOL *pVal)
{
	*pVal = m_bUseFullSystemScan;
	return S_OK;
}

STDMETHODIMP CNAVStatus::put_UseFullSystemScan(BOOL newVal)
{
	m_bUseFullSystemScan = newVal;
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_FullSystemScanStatus(long *pVal)
{
	return getLong ( AVStatus::propFSSStatus, pVal );
}

STDMETHODIMP CNAVStatus::get_FullSystemScanDate(BSTR *pVal)
{
    return getString ( AVStatus::propFSSDateS, pVal );
}

STDMETHODIMP CNAVStatus::get_FullSystemScanAge(long *pVal)
{
    return getLong ( AVStatus::propFSSAge, pVal );
}

STDMETHODIMP CNAVStatus::get_QuarStatus(long *pVal)
{
	return makeError ( ERR_GET_ON_INVALID_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL );
}

STDMETHODIMP CNAVStatus::get_EmailPOP(BOOL *pVal)
{
    return getLong (AVStatus::propEmailPOP, (long*)pVal);
}

STDMETHODIMP CNAVStatus::get_EmailSMTP(BOOL *pVal)
{
    return getLong ( AVStatus::propEmailSMTP, (long*)pVal );
}

STDMETHODIMP CNAVStatus::get_VirusDefSubDate(BSTR *pVal)
{
    return getString ( AVStatus::propSubDateS, pVal );
}

STDMETHODIMP CNAVStatus::get_ALUCanEnable(BOOL *pVal)
{
    return getLong ( AVStatus::propALUCanEnable, (long*)pVal );
}

STDMETHODIMP CNAVStatus::get_ScriptBlockingCanEnable(BOOL *pVal)
{
    *pVal = FALSE;
    return S_OK;
}

STDMETHODIMP CNAVStatus::get_APCanEnable(BOOL *pVal)
{
    return getLong ( AVStatus::propAPCanEnable, (long*)pVal );
}

STDMETHODIMP CNAVStatus::get_EmailCanEnable(BOOL *pVal)
{
    return getLong ( AVStatus::propEmailCanEnable, (long*)pVal );
}
STDMETHODIMP CNAVStatus::get_ProductLicenseType(DJSMAR_LicenseType *pVal)
{
    return getLong ( AVStatus::propLicType, (long*)pVal );
}

STDMETHODIMP CNAVStatus::get_ProductLicenseState(DJSMAR00_LicenseState *pVal)
{
    return getLong ( AVStatus::propLicState, (long*)pVal );
}

STDMETHODIMP CNAVStatus::get_ProductLicenseZone(long *pVal)
{
    return getLong ( AVStatus::propLicZone, pVal );
}

STDMETHODIMP CNAVStatus::get_UseLicensing(BOOL *pVal)
{
	*pVal = m_bUseLicensing;
	return S_OK;
}

STDMETHODIMP CNAVStatus::put_UseLicensing(BOOL newVal)
{
	m_bUseLicensing = newVal;
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_ProductLicenseValid(BOOL *pVal)
{
    return getLong ( AVStatus::propLicValid, (long*)pVal );
}

STDMETHODIMP CNAVStatus::get_LicensingStatus(long *pVal)
{
	return getLong ( AVStatus::propLicStatus, (long*)pVal );
}

STDMETHODIMP CNAVStatus::put_UseIWP(BOOL newVal)
{
	m_bUseIWP = newVal;
	return S_OK;
}

STDMETHODIMP CNAVStatus::get_IWPStatus(long *pVal)
{
	return getLong ( AVStatus::propIWPStatus, (long*)pVal );
}

