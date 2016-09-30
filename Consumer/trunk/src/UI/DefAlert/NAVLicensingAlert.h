////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVLicensingAlert.h

#pragma once

#include "SymAlertInterface.h"
#include "CLTProductHelper.h"

class CLicensingAlert:
     public SIMON::CSimonClass<CLicensingAlert, &CLSID_CLicensingAlert>  
    ,public SIMON::IInterfaceManagementImpl<false>
    ,public ILicensingAlert
{
public:
	CLicensingAlert(void);
	~CLicensingAlert(void) {}

	SIMON_INTERFACE_MAP_BEGIN()
	SIMON_INTERFACE_ENTRY(IID_ILicensingAlert, ILicensingAlert)
	SIMON_INTERFACE_MAP_END()

	SIMON_STDMETHOD(Init)();
	SIMON_STDMETHOD(Refresh)(BOOL bRefreshCache = FALSE);
	SIMON_STDMETHOD(ShouldShowAlert)(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag);
	SIMON_STDMETHOD(GetLicenseType)(DWORD* pType);
	SIMON_STDMETHOD(GetLicenseAttribute)(DWORD* pAttribute);
	SIMON_STDMETHOD(IsAggregated)(BOOL* pbAggregated);
	SIMON_STDMETHOD(GetExpiredCyle)(DWORD* pdwCycle);
	SIMON_STDMETHOD(GetWarningCycle)(DWORD* pdwCycle);

private:
	bool IsTimeForAlert(SYMALERT_TYPE Type, bool bWarning, bool bCheckTimer);
	bool IsCfgWizRunOnce();
	bool IsCTO();

private:
	// License type.
	DWORD m_LicenseType;

    // License Attribute
    DWORD m_LicenseAttribute;

	// Is the product part of a suite?
	BOOL m_bLicenseAggregated;

	// How often to display a licensing warning alert?
	DWORD m_dwCycleLicWarning;

	// How often to display a licensing expired alert?
	DWORD m_dwCycleLicExpired;

	// Time when product installed.
	DWORD m_dwInstallTime;

	// For how long we should delay a licensing warning alert.
	DWORD m_dwFirstAlertDelay;

	bool m_bAlertEnabled;

    CCLTProductHelper m_cltHelper;
};