// NAVDenitionsAlert.h

#pragma once

#include "SymAlertInterface.h"

class CDefinitionsAlert: 
     public SIMON::CSimonClass<CDefinitionsAlert,&CLSID_CDefinitionsAlert>  
    ,public SIMON::IInterfaceManagementImpl<false>
    ,public ISymAlert
{
public:
	CDefinitionsAlert(void):
	m_dwMaxDefsAge(MAX_DEFS_AGE)
	,m_bAlertEnabled(true)
	,m_nDefsAge(0)
	,m_dwFirstAlertDelay(0)
	,m_dwInstallTime(0)
	{
	}
	~CDefinitionsAlert(void) {}

	SIMON_INTERFACE_MAP_BEGIN()
	SIMON_INTERFACE_ENTRY(IID_ISymAlert, ISymAlert)
	SIMON_INTERFACE_MAP_END()

	SIMON_STDMETHOD(Init)();
	SIMON_STDMETHOD(Refresh)(BOOL bRefreshDefsCount = FALSE); // Since the bRefreshCache is not in use anymore, we will re-use it for refreshing the virus defs information.
	SIMON_STDMETHOD(ShouldShowAlert)(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag);

private:
	HRESULT GetDefsAge(long& nDefsAge);
	BOOL IsTimeForAlert(SYMALERT_TYPE Type);

private:
	DWORD m_dwInstallTime;
	DWORD m_dwFirstAlertDelay;
	DWORD m_dwMaxDefsAge;
	bool m_bAlertEnabled;
	long m_nDefsAge;
};
