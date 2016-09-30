#pragma once

// Wrapper classes for NAVFactory
#include "AvEvents.h"
#include "Thread.h"
#include "PushPopQueWithEvents.h"
#include "NAVEventFactoryLoader.h"

//typedef IAvEvent* pAvEvent;
typedef StahlSoft::CPushPopQueueSignal <CEventData*> qEvents;
typedef StahlSoft::CPushPopQueue<CEventData*> qRealEvents;

// This class manages the calls to NAVEventFactory and
// is a thread.
//
class CProviderManager : public CThread, public StahlSoft::CPushPopQueueSignalSink
{
public:
    CProviderManager(void);
    virtual ~CProviderManager(void);
    
    void SendEvent ( CEventData& Event /*in*/);

    // Gets called when a new item enters the queue
    void OnItemAdded ();

protected:
    // Does the work
    int DoWork ();

	// Pointer to logging object.
    // Always list the loader first!
    AV::NAVEventFactory_IAVEventFactory m_pLoggerLoader;
    AV::IAvEventFactoryPtr m_pLogger;

    qEvents m_qEvents;

    HANDLE m_hNewWorkEvent;
};
