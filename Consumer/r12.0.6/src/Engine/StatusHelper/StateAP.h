#pragma once
#include "state.h"

class CStateAP :
    public CState
{
public:
    CStateAP(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CStateAP(void);

    bool Save ();

protected:
    // CState overrides
    //
	bool GetAPEnabled();
    ccLib::CEvent m_eventAP;

    DWORD m_dwOldStatus;
    DWORD m_dwOldStatusSpyware;
};
