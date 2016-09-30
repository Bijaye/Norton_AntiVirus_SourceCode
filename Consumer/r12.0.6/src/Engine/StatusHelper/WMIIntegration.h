#pragma once

#include "EventData.h"
#include "ISWSCHelper_Loader.h" // New IS shared loader using Trust Cache
#include "WSCHelper.h"
#include "NAVInfo.h"

// This WMI integration class acts like an in-proc subscriber.
//
class CWMIIntegration
{
public:
    CWMIIntegration(void);
    virtual ~CWMIIntegration(void);

    // Does WMI care about this feature?
    bool Subscribe ( long lEventID );

    // WMI cared about something so here's the status bag.
    // lEventID = 0 when we do our initial update
    //
    void OnStatusChange ( long lEventID /* 0 = INIT */,
                          CEventData& edNewStatus /*contains the new data*/ );

protected:    
    BOOL m_bCfgWizFinished;

    // Pre-XP2 means we don't care about it!
    bool m_bIsPreXPSP2;

    CString m_sEnableUIPath;
    CString m_sEnableUIParams;

    void WriteStatusToWMI_AP(IWSCHelper* pWSCHelper, long lAPStatus);
    void WriteStatusToWMI_Defs(IWSCHelper* pWSCHelper, long lDefsStatus, time_t timeDefs);
    void WriteStatusToWMI_IWP(IWSCHelper* pWSCHelper, long lIWPStatus);
};
