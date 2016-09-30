#pragma once

#include "SSPProcessEventBase.h"
#include "ccSerializableEventHelper.h"

class CSSPProcessDelayEvent :
    public CSSPProcessEventBase

{
public:
    CSSPProcessDelayEvent(void);
    virtual ~CSSPProcessDelayEvent(void);

    STDMETHOD(Run)();
    void SetDelayValue(INT nMilliseconds);

protected:
    INT m_nMilliseconds;
};
