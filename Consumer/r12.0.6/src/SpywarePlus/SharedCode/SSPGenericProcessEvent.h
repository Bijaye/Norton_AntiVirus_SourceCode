#pragma once
#include "SSPProcessEventBase.h"

class CSSPGenericProcessEvent :
	public CSSPProcessEventBase
{
public:
	CSSPGenericProcessEvent(void);
	virtual ~CSSPGenericProcessEvent(void);

	STDMETHOD(Run)();

};
