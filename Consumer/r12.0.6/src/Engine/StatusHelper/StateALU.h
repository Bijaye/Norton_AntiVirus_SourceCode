#pragma once

#include "State.h"

class CStateALU : 
    public CState
{
public:
    CStateALU(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    ~CStateALU(void);

    // CState overrides
    //
    bool Save ();
    void HardRefresh ();

protected:
    ccLib::CEvent m_eventALU;
	long m_lOldStatus;
};
