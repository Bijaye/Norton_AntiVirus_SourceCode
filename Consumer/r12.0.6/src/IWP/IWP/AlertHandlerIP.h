#pragma once

#include "AlertHandlerFW.h"

class CAlertHandlerIP : public CAlertHandlerFW
{
public:
	CAlertHandlerIP(void) throw();
	virtual ~CAlertHandlerIP(void) throw();

public:
    IAlertData::ALERT_RESULT DisplayAlert(  SymNeti::CSNAlertEventEx* pSNEvent,
                                            LPTSTR szAppName,
                                            IAlertData::ALERT_TYPE eAlertType) throw ();
private:
	CAlertHandlerIP(const CAlertHandlerIP&) throw();
	CAlertHandlerIP& operator=(const CAlertHandlerIP&) throw(); 
};
