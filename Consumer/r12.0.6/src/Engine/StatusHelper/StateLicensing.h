#pragma once

#include "State.h"
#include "Subscription.h"

class CStateLicensing : public CState
{
public:
    CStateLicensing(CWMIIntegration* pWMI, CNSCIntegration* pNSC);
    virtual ~CStateLicensing(void);

    bool Save ();

protected:

    ccLib::CEvent m_eventLicensing;
    ccLib::CEvent m_eventSubscription;

    CSubscription::LICENSE_SUB_DATA m_OldSubData;
};
