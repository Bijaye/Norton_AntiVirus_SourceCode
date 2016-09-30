#pragma once

#include "ccEventUtil.h"
#include "ccEventFactoryHelper.h"

class CHPPEventFactory : public ccEvtMgr::CEventFactoryHelper
{
public:
    CHPPEventFactory(void) throw();
    virtual ~CHPPEventFactory(void) throw();
    
private:
    CHPPEventFactory(const CHPPEventFactory&) throw();
    CHPPEventFactory& operator =(const CHPPEventFactory&) throw();

public:
    DECLARE_EVENT_FACTORY();

protected:
    static const long m_EventTypeArray[];
};
