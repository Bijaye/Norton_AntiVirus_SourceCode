#pragma once
#include "SSPProcessEventBase.h"

class CProcessSysMonNotifyEvent :
    public CSSPProcessEventBase
{
public:
    CProcessSysMonNotifyEvent(void);
    virtual ~CProcessSysMonNotifyEvent(void);

    STDMETHOD(Run)();

protected:

};
