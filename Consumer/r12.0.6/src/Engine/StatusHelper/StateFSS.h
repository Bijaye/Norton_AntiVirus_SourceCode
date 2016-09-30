#pragma once
#include "state.h"

class CStateFSS :
    public CState
{
public:
    CStateFSS(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CStateFSS(void);

    // CState overrides
    //
    bool Save ();

protected:
    ccLib::CEvent m_eventFSS;
};
