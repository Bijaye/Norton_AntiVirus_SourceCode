// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "CSymProtectEventSubscriber.h"
#include "SymProtectEventTypes.h"
#include "SymProtectEventsInterface.h"

// Constructor-destructor
CSymProtectEventSubscriber::CSymProtectEventSubscriber() : initialized(false), eventList(NULL), shutdownEventHandle(NULL)
{
	// Nothing for now
	InitializeCriticalSection(&configLock);
}

CSymProtectEventSubscriber::~CSymProtectEventSubscriber()
{
	ShutdownMain();
	DeleteCriticalSection(&configLock);
}

// CSubscriber
HRESULT CSymProtectEventSubscriber::InitializeMain( void )
// Initialization for CSymProtectEventSubscriber.  Renamed to prevent naming collisions with inherited classes.
{
    CSubscriberHelper::SubscriberInfo subscriberInfo[] =
    {
        { CC_SYMPROTECT_POLICY_EVENT, 0, NULL, 0, 0, true }
    };
	HRESULT			returnValHR			= E_FAIL;
	SYMRESULT		returnValSR			= SYMERR_UNKNOWN;

	// Validate state
	if (initialized)
		return E_ABORT;

	// Load the factory and proxy DLLs
	returnValSR = eventFactoryLoader.CreateObject(eventFactory.m_p);
    if (SYM_SUCCEEDED(returnValSR))
		returnValSR = proxyFactoryLoader.CreateObject(proxyFactory.m_p);
	if (SYM_SUCCEEDED(returnValSR))
        eventManager.Create(eventFactory, proxyFactory);
	if (SYM_FAILED(returnValSR))
	{
        ShutdownMain();
        return E_FAIL;
	}

    // Subscribe to the SymProtect events
    if (CSubscriberHelper::Create(&eventManager, subscriberInfo, sizeof(subscriberInfo)/sizeof(subscriberInfo[0])) == FALSE)
    {
        ShutdownMain();
        return E_FAIL;
    }

	// Note that CSubscribeHelper stores subscription IDs in m_SubscriberIdMap, a map of event type:subscriber ID
	initialized = true;
    return S_OK;
}

HRESULT CSymProtectEventSubscriber::ShutdownMain( void )
// Shutdown for CSymProtectEventSubscriber.  Renamed to prevent naming collisions with inherited classes.
{
    // Shutdown services in reverse order of initialization
	initialized = false;

	// We want our duration (lifetime) equal to the lifetime of this C++ object, but CSubscriberHelper has a different idea
	// Prevent us from being destroyed by CSubscriberHelper::Destroy's Release call in it's Destroy member
	// This is necessary because CSubscriberHelper's destructor both shuts down and calls Release, instead of just shutting down
	// So we bind our lifetime to the object's lifespan and let the destructor cleanup for us
	// Without this, a GPF occcurs when our destructor calls Shutdown main, which calls CsubscriberHelper::Destroy, which calls Release and tries to delete this object
	// again, but the destructor is already running.
	AddRef();

	// Disconnect from CC notifications
    CSubscriberHelper::Destroy();

	// Shutdown remaining CC support
	eventManager.Destroy();
    proxyFactory.Release();
	eventFactory.Release();

	return S_OK;
}

HRESULT CSymProtectEventSubscriber::DeleteEvent( ccEvtMgr::CEventEx* thisEvent )
// Delete an event.
{
	if (thisEvent == NULL)
		return E_POINTER;

	if (initialized)
	{
        eventFactory->DeleteEvent(thisEvent);
        thisEvent = NULL;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CSymProtectEventSubscriber::SetEventList( CSynchronizedList<ccEvtMgr::CEventEx*>* newEventList )
// Sets the event list to queue events onto when they are received
// Returns S_OK on success, E_FAIL if subscriber is active, else the error code of the failure
{
	CSynchronizedList<ccEvtMgr::CEventEx*>::ListType::iterator	currEvent;
	HRESULT returnValHR = E_FAIL;

	EnterCriticalSection(&configLock);

	// Set the event list
	eventList = newEventList;

	LeaveCriticalSection(&configLock);
	return S_OK;
}

HRESULT CSymProtectEventSubscriber::SetShutdownEvent( HANDLE newShutdownEventHandle )
// Sets an event handle to signal when shutdown notification is received.
// Returns S_OK on success, E_FAIL if subscriber is active, else the error code of the failure
{
	EnterCriticalSection(&configLock);

	// Set the shutdown notification handle
	shutdownEventHandle = newShutdownEventHandle;

	LeaveCriticalSection(&configLock);
	return S_OK;
}


ccEvtMgr::CError::ErrorType CSymProtectEventSubscriber::OnEvent( long subscriberID, const ccEvtMgr::CEventEx& thisEvent, ccEvtMgr::CSubscriberEx::EventAction& actionCode )
// Primary event interface - called by Common Client Event Manager when an event is received
{
	SymProtectEvt::CPolicyEventExQIPtr		policyEventPtr;
	ccEvtMgr::CEventEx*						thisEventCopy			= NULL;
	HRESULT									returnValHR				= E_FAIL;

	switch (thisEvent.GetType())
	{
	case CC_SYMPROTECT_POLICY_EVENT:
	{
		if (initialized)
		{
			EnterCriticalSection(&configLock);

			policyEventPtr = &thisEvent;
			if ((policyEventPtr.m_p != NULL) && (eventList != NULL))
			{
				if (eventFactory->CopyEvent(*policyEventPtr, thisEventCopy) == ccEvtMgr::CError::eNoError)
					eventList->push_back(thisEventCopy);
			}

			LeaveCriticalSection(&configLock);
		}
		break;
	}
	default:
	{
		// Nothing for now
		break;
	}
	}
	// Should we use eCommitAction instead for better performance?
	actionCode = CSubscriberEx::eNoAction;
	return ccEvtMgr::CError::eNoError;
}

ccEvtMgr::CError::ErrorType CSymProtectEventSubscriber::OnShutdown()
// Called when Common Client is shutting down.
{
	EnterCriticalSection(&configLock);

	// Set the shutdown notification event, if requested
	if (shutdownEventHandle != NULL)
		SetEvent(shutdownEventHandle);

	LeaveCriticalSection(&configLock);
	return ccEvtMgr::CError::eNoError;
}