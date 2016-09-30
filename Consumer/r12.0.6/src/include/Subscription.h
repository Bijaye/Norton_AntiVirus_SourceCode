#pragma once

#include "NAVSettingsCache.h"
#include "NAVLicense_h.h"
#include "SymScriptSafe_h.h"

// This object collects subscription and licensing data from PEP.
// It uses the NAV License COM wrapper so you don't have to link with PEP directly.
// NAV License has it's own internal copy of the data so if the data
// refreshes and you want the latest data you must destroy and recreate this
// object.
//
// INITIALIZE COM *STA* ON THE CALLING THREAD!! (unless NAV License gets updated to use Both)
//
class CSubscription
{
public:
    CSubscription(void);
    virtual ~CSubscription(void);

    struct LICENSE_SUB_DATA
    {
    // Licenses
        DJSMAR_LicenseType LicenseType;
        DJSMAR00_LicenseState LicenseState;
        DJSMAR_LicenseZone LicenseZone;
        DJSMAR00_VendorId VendorID;
        bool bLicenseValid; // are we violated?

    // Subscriptions
        BOOL bCanRenewSub;
        long lDaysLeft;
        DWORD dwEndDate;
        BOOL bWarning;
        BOOL bExpired;
        LPCSTR szEndDate; // in short date format
    };

    LICENSE_SUB_DATA GetData ();

    HRESULT LaunchSubscriptionWizard(HWND hWndParent = NULL );
    HRESULT HasUserAgreedToEULA(BOOL* pbAgreed);
    HRESULT IsCfgWizFinished(BOOL* pbFinished);

protected:
    bool init (void);

    ccLib::CCriticalSection m_critAccess;

    // data
	LICENSE_SUB_DATA m_Data;
    char m_szSubDate [64];

    void readSubscription ();
    void resetSubData ();

	CComPtr<INAVLicenseInfo> m_spNavLicense;
	CComPtr <ISymScriptSafe> m_spSymScriptSafe;

};
