#pragma once
#include "NAVBusinessRules.h"

bool CheckMismatch(CNAVOptSettingsCache* pOptFile);

class CBusinessRules:
 	 public SIMON::CSimonClass<CBusinessRules,&NAV::CLSID_CNAVBusinessRules>  
    ,public SIMON::IInterfaceManagementImpl<false>
	,public NAV::ICalc  
{
public:
	CBusinessRules(void);
	~CBusinessRules(void);
	HRESULT FinalConstruct();
    SIMON_INTERFACE_MAP_BEGIN()
        SIMON_INTERFACE_ENTRY( NAV::IID_ICalc ,NAV::ICalc  )
    SIMON_INTERFACE_MAP_END()
    SIMON_STDMETHOD(Calc)(CNAVOptSettingsCache* pOptFile);
private:
	HRESULT _InitializeVariables();
	bool IsCTO();
};
