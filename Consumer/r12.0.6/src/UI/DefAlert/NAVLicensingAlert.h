// NAVLicensingAlert.h

#pragma once

#include "SymAlertInterface.h"

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
	SIMON_STDMETHOD(GetLicenseType)(DJSMAR_LicenseType* pType);
	SIMON_STDMETHOD(GetLicenseState)(DJSMAR00_LicenseState* pState);
	SIMON_STDMETHOD(GetLicenseZone)(long* pnZone);
	SIMON_STDMETHOD(IsAggregated)(BOOL* pbAggregated);
	SIMON_STDMETHOD(GetExpiredCyle)(DWORD* pdwCycle);
	SIMON_STDMETHOD(GetWarningCycle)(DWORD* pdwCycle);

private:
	bool IsTimeForAlert(SYMALERT_TYPE Type, bool bWarning, bool bCheckTimer);
	bool IsCfgWizRunOnce();
	bool IsCTO();

private:
	// License type.
	DJSMAR_LicenseType m_LicenseType;

	// License state.
	DJSMAR00_LicenseState m_LicenseState;

	// Licensing zone.
	long m_LicenseZone;

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
};