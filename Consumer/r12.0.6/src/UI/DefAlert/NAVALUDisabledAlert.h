// NAVALUDisabledAlert.h

#pragma once

#include "SymAlertInterface.h"

class CALUDisabledAlert: 
     public SIMON::CSimonClass<CALUDisabledAlert,&CLSID_CALUDisabledAlert>  
    ,public SIMON::IInterfaceManagementImpl<false>
    ,public ISymAlert
{
public:
	CALUDisabledAlert(void): m_bAlertable(TRUE) {}
	~CALUDisabledAlert(void) {}

    SIMON_INTERFACE_MAP_BEGIN()
   	SIMON_INTERFACE_ENTRY(IID_ISymAlert, ISymAlert)
    SIMON_INTERFACE_MAP_END()

    SIMON_STDMETHOD(Init)();
    SIMON_STDMETHOD(Refresh)(BOOL bRefreshCache = FALSE);
    SIMON_STDMETHOD(ShouldShowAlert)(BOOL* pbShow, SYMALERT_TYPE* pType, BOOL bCheckTimer, DWORD dwFlag);

private:
	BOOL m_bAlertable;
};
