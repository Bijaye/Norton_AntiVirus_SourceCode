#pragma once

#include <map>
#include "EventData.h"
#include "SyncQueue.h"
#include "EMSubscriber.h"
#include "State.h"
#include "Plugin.h"         // Base class for ccApp plug-in
#include "OSInfo.h"         // For session id
#include "WMIIntegration.h"
#include "NSCIntegration.h"

typedef std::map <long /*feature ID*/, CState*> mapStates;
typedef mapStates::iterator iterStates;

class CStatusManager : public CPlugin, public CEMSubscriberSink
{
public:
    CStatusManager(void);
    virtual ~CStatusManager(void);

    // CPlugin
    void Run ( ICCApp* pCCApp );

    // Put an item into our queue for processing
    //
    void AddItem ( ::ccEvtMgr::CEventEx* pEvent);

    // EMSubscriber
    //
    // New event arrived. Make a copy of it if you want to process it.
    // The Event Manager is waiting for this call to return, so hurry.
    void EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                    ccEvtMgr::CSubscriberEx::EventAction& eAction);
    void EMSubscriberOnShutdown ();

    mapStates m_mapFeatures;

protected:
    void startup();
    void shutdown();
    void processQueue ();
    void startListening ();
    void waitForEM();
    bool loadPersistedStaus (CEventData& edOldStatus);
    void savePersistedStaus (CEventData& edOldStatus);


    CWMIIntegration m_WMI;
    CNSCIntegration m_NSC;
    bool m_bInternalSubscriber;

    // Work items
    std::queue<::ccEvtMgr::CEventEx*> m_queueWork;
    ccLib::CCriticalSection      m_critWorkQueue;
    ccLib::CEvent                m_eventNewWork;
    ccLib::CEvent                m_eventEMShutdown;
    ccLib::CCriticalSection      m_critWorking;

    // Our single EventManager subscriber
    std::auto_ptr<CEMSubscriber> m_pEMSubscriber;
};
