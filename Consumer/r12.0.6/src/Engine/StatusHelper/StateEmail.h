#pragma once
#include "state.h"

class CStateEmail :
    public CState
{
public:
    CStateEmail(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CStateEmail(void);

    // CState overrides
    //
    bool Save ();

protected:
    ccLib::CEvent m_eventEmail;
    ccLib::CEvent m_eventLicensing;

    long m_lOldStatus;
    long m_lOldSMTP;
    long m_lOldPOP;
    long m_lOldOEH;
};
