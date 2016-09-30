// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef SYMPROTECTEVENTSUBSCRIBER_HEADER
#define SYMPROTECTEVENTSUBSCRIBER_HEADER

#include "stdafx.h"
#include "ccEventManagerHelper.h"
#include "ccSubscriberHelper.h"
#include "ccEventFactoryHelper.h"
#include "SymProtectEventsLoader.h"
#include "SymProtectEventsInterface.h"
#include "CSynchronizedList.h"

// The Common Client event manager interface.  An instance of this class receives events
// through the common client event framework and deposites them into a queue for the
// RTVScan storage to process.
class CSymProtectEventSubscriber : public ccEvtMgr::CSubscriberHelper
{
public:
	// Constructor-destructor
	CSymProtectEventSubscriber();
	virtual ~CSymProtectEventSubscriber();

	// CSubscriber
	// Initialization for CSymProtectEventSubscriber.  Renamed to prevent naming collisions with inherited classes.
    HRESULT InitializeMain( void );
	// Shutdown for CSymProtectEventSubscriber.  Renamed to prevent naming collisions with inherited classes.
    HRESULT ShutdownMain( void );
	// Delete an event.
    HRESULT DeleteEvent( ccEvtMgr::CEventEx* thisEvent );

	// Sets the event list to queue events onto when they are received.  If a list has already been set, it is deleted.
	HRESULT SetEventList( CSynchronizedList<ccEvtMgr::CEventEx*>* newEventList );
	// Sets an event handle to signal when shutdown notification is received.
	HRESULT SetShutdownEvent( HANDLE newShutdownEventHandle );

	// Queue events received from the Common Client to the specified event list
    virtual ccEvtMgr::CError::ErrorType OnEvent( long subscriberID, const ccEvtMgr::CEventEx& thisEvent, ccEvtMgr::CSubscriberEx::EventAction& actionCode );
	// Called when Common Client is shutting down.
    virtual ccEvtMgr::CError::ErrorType OnShutdown();

private:
	// ** FUNCTION MEMBERS **
    CSymProtectEventSubscriber(const CSymProtectEventSubscriber&);
    CSymProtectEventSubscriber& operator =(const CSymProtectEventSubscriber&);

	// ** DATA MEMBERS **
    ccEvtMgr::CEventFactoryEx2Ptr						eventFactory;
	SymProtectEvt::SymProtectEvents_CEventFactoryEx2	eventFactoryLoader;
    ccEvtMgr::CProxyFactoryExPtr						proxyFactory;
	SymProtectEvt::SymProtectEvents_CProxyFactoryEx		proxyFactoryLoader;
    CSavStaticRefCount<ccEvtMgr::CEventManagerHelper>	eventManager;

	bool												initialized;			// Set to TRUE when initialized, else FALSE
	CRITICAL_SECTION									configLock;				// Synchronize access to all following members, which may be used by both the callback thread and the control thread(s)
	HANDLE												shutdownEventHandle;
	CSynchronizedList<ccEvtMgr::CEventEx*>*				eventList;
};

#endif // SYMPROTECTEVENTSUBSCRIBER_HEADER