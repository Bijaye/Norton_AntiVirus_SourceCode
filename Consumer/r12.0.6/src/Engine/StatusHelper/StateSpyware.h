#pragma once
#include "state.h"

class CStateSpyware :
    public CState
{
public:
    CStateSpyware(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CStateSpyware(void);

    bool Save ();

protected:
    ccLib::CEvent m_eventOptions;
    long m_lOldSpywareCat;

};
