#pragma once
#include "state.h"

class CStateIM :
    public CState
{
public:
    CStateIM(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CStateIM(void);

protected:
    // CState overrides
    //
    bool Save ();

    CNAVOptSettingsEx m_NavOpts;
    ccLib::CEvent m_eventIM;
    ccLib::CEvent m_eventLicensing;

    long m_lOldMSNProtected;
    long m_lOldAOLProtected;
    long m_lOldTOMProtected;
    long m_lOldYIMProtected;
};
