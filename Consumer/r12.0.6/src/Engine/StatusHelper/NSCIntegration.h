#pragma once
#include "EventData.h"

#include <set>

// Norton Security Console integration area.
//
class CNSCIntegration
{
public:
    CNSCIntegration(void);
    virtual ~CNSCIntegration(void);

    // Does WMI care about this feature?
    bool Subscribe ( long lEventID );

    // WMI cared about something so here's the status bag.
    // lEventID = 0 when we do our initial update
    //
    void OnStatusChange ( long lEventID /* 0 = INIT */,
                          CEventData& edNewStatus /*contains the new data*/ );

    // This is the Clean Bit used to improve performance of the avNSCPlugin.
    ccLib::CMutex m_mutexNSCCleanBit;

protected:
    std::set<long> m_setEventIDs;
    typedef std::set<long>::iterator iterEventIDs;
};
