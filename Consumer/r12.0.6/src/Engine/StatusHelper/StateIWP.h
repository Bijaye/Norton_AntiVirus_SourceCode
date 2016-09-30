#pragma once
#include "state.h"

class CStateIWP :
    public CState
{
public:
    CStateIWP(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CStateIWP(void);

protected:
    // CState overrides
    //
    bool Save ();
    ccLib::CEvent m_eventIWP;
    long m_lOldStatus;
};
