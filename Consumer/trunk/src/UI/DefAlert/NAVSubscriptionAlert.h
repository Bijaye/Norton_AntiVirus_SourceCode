////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVSubscriptionAlert.h

#pragma once

#include "SymAlertInterface.h"
#include "CLTProductHelper.h"

class CSubscriptionAlert:
     public SIMON::CSimonClass<CSubscriptionAlert, &CLSID_CSubscriptionAlert>  
    ,public SIMON::IInterfaceManagementImpl<false>
    ,public ISubscriptionAlert
{
public:
	CSubscriptionAlert(void);
	~CSubscriptionAlert(void);

	SIMON_INTERFACE_MAP_BEGIN()
	SIMON_INTERFACE_ENTRY(IID_ISubscriptionAlert, ISubscriptionAlert)
	SIMON_INTERFACE_MAP_END()

	SIMON_STDMETHOD(Init)();
	SIMON_STDMETHOD(Refresh)(BOOL bRefreshCache = FALSE);
	SIMON_STDMETHOD(ShouldShowAlert)(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag);
	SIMON_STDMETHOD(GetSubscriptionStatus)(STATUS_SUBSCRIPTION* pStatus);    
	SIMON_STDMETHOD(GetSubscriptionDaysRemaining)(long* pnDaysRemaining);    
	SIMON_STDMETHOD(GetExpiredCyle)(DWORD* pdwCycle);
	SIMON_STDMETHOD(GetWarningCycle)(DWORD* pdwCycle);
	SIMON_STDMETHOD(GetSubscriptionEndDate)(DATE* pnDate);

private:
	BOOL IsTimeForAlert(SYMALERT_TYPE Type);

private:
	// When does the warning period begins?
	DWORD m_dwStartSubWarning;

	// How often to display warning alerts (in seconds)?
	DWORD m_dwCycleSubWarning;

	// How often to display expired alerts (in seconds)?
	DWORD m_dwCycleSubExpired;

	// Subscription status
	STATUS_SUBSCRIPTION m_SubStatus;

	// Does LS allow us to display subscription alerts?
	bool m_bAlertable;

	// How many days left?
	DWORD m_dwDaysRemaining;

	// Subscription end date
	DATE m_EndDate;

	// Can subscription be auto renewed?
	bool m_bCanAutoRenew;

	// Is NAV a child product?
	bool m_bLicenseAggregated;

	// How often to display subscription warning alert in case of auto renew?
	DWORD m_dwCycleSubWarningAutoRenew;

	// How often to display subscription expired alert in case of auto renew?
	DWORD m_dwCycleSubExpiredAutoRenew;

    CCLTProductHelper m_cltHelper;
};
