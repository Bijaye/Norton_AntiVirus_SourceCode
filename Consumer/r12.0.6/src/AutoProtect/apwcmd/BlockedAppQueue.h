#pragma once

// This project
#include "APDequeue.h"
#include "EventData.h"
#include "savrt32.h"
//#include "naveventdata.h"
#include "BlockedAppGroup.h"

typedef std::deque <CBlockedAppGroup*> dqBAG;
typedef dqBAG::iterator iterBAGQueue;

// The Blocked App Queue
//
// ED's come into the threadQ and are translated into BAGs.
// Then they are put in the BAG-Q to be shown when available.
//
class CBlockedAppQueue : 
    public CAPDequeue<CEventData*>,
    public CBlockedAppGroupSink
{
public:
    CBlockedAppQueue(void);
    ~CBlockedAppQueue(void);

    // BAGSink
    //
    void OnBAGUIClosed (CBlockedAppGroup* pBAG);

    static void SetMaxActiveBlocked ( DWORD dwMaxActiveBlocked );

protected:
    // Marks an entry for cancel. UI won't show up.
    //
    void Remove (CEventData& EventData);

    // The Queue of BAG's
    //    
    dqBAG  m_qBAG;
    ccLib::CCriticalSection m_csBAGQueue;

    void showNextBAG ();

    // Is this EventData already in the Q, based on our grouping?
    // Returning true will fill our spBAGOut with the pointer to the existing
    // object in the queue.
    //
    bool findBAG (CEventData& EventData, CBlockedAppGroup*& pBAGOut);

    // Process the incoming CEventData objects.
    //
    void processQueue ();

    void deleteBAG ( CBlockedAppGroup* pBAG );

    static DWORD m_dwMaxActiveBlocked; // Maximum number of displayed Blocked App UI's at one time
};
