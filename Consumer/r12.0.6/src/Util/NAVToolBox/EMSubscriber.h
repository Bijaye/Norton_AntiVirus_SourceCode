#pragma once

#include "ccEventManagerHelper.h"
#include "ccEventFactoryEx.h"
#include "ccSubscriberHelper.h"
#include "ccProxyFactoryEx.h"
#include "ccProsubloader.h"

#include "NAVEventFactoryLoader.h"

#include <vector>

class CEMSubscriberSink  
{
public:
    CEMSubscriberSink(){};
    virtual ~CEMSubscriberSink(){};

    // New event arrived. Make a copy of it if you want to process it.
    // The Event Manager is waiting for this call to return, so hurry.
    virtual void EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                      ccEvtMgr::CSubscriberEx::EventAction& eAction) = 0;
    virtual void EMSubscriberOnShutdown () = 0;
};

class CEMSubscriber : 
    public ccEvtMgr::CSubscriberHelper
{
public:
    CEMSubscriber(CEMSubscriberSink* pSink);    
	virtual ~CEMSubscriber();
    
public:
    bool Connect();
    bool Disconnect();
    bool IsEMActive();

    // Makes a copy of the event so clients can process it later
    //
    bool CopyEvent (const ccEvtMgr::CEventEx& Event /*in*/, ccEvtMgr::CEventEx*& pEvent /*out*/);

    // If you copied the event, make sure you delete it when done.
    void DeleteEvent(ccEvtMgr::CEventEx* pEvent);

    // Fill this data object with the event id's you want to subscribe to.
    //
    std::vector<ccEvtMgr::CSubscriberHelper::SubscriberInfo> m_vecSubscriberInfo;

    // CSubscriberEx
    //
    virtual ccEvtMgr::CError::ErrorType OnEvent(long nSubscriberId, 
                                      const ccEvtMgr::CEventEx& Event,
                                      ccEvtMgr::CSubscriberEx::EventAction& eAction);

    virtual  ccEvtMgr::CError::ErrorType OnShutdown();

protected:    
    // Declare loaders before interfaces.
    //
	// Pointer to event manager object
    ccEvtMgr::CEventManagerHelper m_EventManager;
    CEMSubscriberSink* m_pSink;

private:
    CEMSubscriber(){}; // No no, I want a sink.
};
