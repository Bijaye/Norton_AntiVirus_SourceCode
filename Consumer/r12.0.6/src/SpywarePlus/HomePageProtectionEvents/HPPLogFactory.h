#pragma once

#include "ccLogFactoryHelper.h"

class CHPPLogFactory : public ccEvtMgr::CLogFactoryHelper
{
public:
    CHPPLogFactory(void) throw();
    virtual ~CHPPLogFactory(void) throw();

private:
    CHPPLogFactory(const CHPPLogFactory&) throw();
    CHPPLogFactory& operator =(const CHPPLogFactory&) throw();

public:
    virtual BOOL Register(long nModuleId,
                  ccEvtMgr::CLogManagerExPtr pLogManager,
				  ccEvtMgr::CEventFactoryExPtr pEventFactory) throw();

protected:
    static const EventLogInfo m_EventTypeInfoArray[];

};
