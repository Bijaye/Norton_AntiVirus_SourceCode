// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventsSubscriber.h"
#include "ccMessageLock.h"
#include "ccSingleLock.h"
#include "ccEveryoneDACL.h"
#include "ClientCCSettingNames.h"

using namespace cc;
using namespace ccSym;
using namespace ccLib;
using namespace ccSettings;
using namespace ccEvtMgr;

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

// Event subscription data
CSubscriberHelper::SubscriberInfo SubscribedEvents[] =
{
	{CSettingsChangeEventEx::TypeId, 0, NULL, 0, 0, false},
};

CEventsSubscriber::CEventsSubscriber() :
	m_hThread(NULL), m_hTrayWnd(NULL)
{
	// COM will be initialized on the main thread of VPTray.  Note: the portion that
	// initializes the factories can be done in STA, while the thread itself will
	// be MTA.  The registration with event manager using STA was confirmed to 
	// be OK by the CC team.
	m_bInitFactories = InitializeFactories();

	if (m_bInitFactories == FALSE)
		CCTRACEE(_T("%s : Failed to initialize the event factories.\n"), __FUNCTION__);
}

CEventsSubscriber::~CEventsSubscriber()
{
	// From SymProtectEventSubscriber.cpp:
	// We want our duration (lifetime) equal to the lifetime of this C++ object, 
	// but CSubscriberHelper has a different idea.
	// Prevent us from being destroyed by CSubscriberHelper::Destroy's Release call 
	// in it's Destroy member.  This is necessary because CSubscriberHelper's destructor 
	// both shuts down and calls Release, instead of just shutting down
	// So we bind our lifetime to the object's lifespan and let the destructor cleanup for us
	// Without this, a GPF occcurs when our destructor calls Shutdown main, which calls 
	// CsubscriberHelper::Destroy, which calls Release and tries to delete this object
	// again, but the destructor is already running.
	AddRef();

	// Make sure that we destroy the subscriber helper before the passed in
	// Event manager helper is destroyed.
	if (m_bInitFactories)
		UninitializeFactories();

	if (m_hQueueEvent != NULL)
	{
		CloseHandle(m_hQueueEvent);
		m_hQueueEvent = NULL;
	}
}

void CEventsSubscriber::SetTrayWnd(const HWND hTrayWnd)
{
	m_hTrayWnd = hTrayWnd;
}

BOOL CEventsSubscriber::Start()
{
	SAV::CEveryoneDACL everyoneDacl;
	everyoneDacl.Create();

	m_hQueueEvent = CreateEvent(everyoneDacl, FALSE, FALSE, NULL);

	m_eraserNotify.SetTrayWnd(m_hTrayWnd);
	m_eraserNotify.Start();

	return CThread::Create(NULL, 0, 0);
}

void CEventsSubscriber::Stop()
{
	// Stop the eraser notify thread.
	if (m_eraserNotify.IsThread())
		m_eraserNotify.Terminate(INFINITE);

	// Signal the exit event and wait for our thread to finish.
	if (IsThread())
		Terminate(INFINITE);
}

CError::ErrorType CEventsSubscriber::OnEvent(long nSubscriberId, 
											 const CEventEx& Event, 
											 CSubscriberEx::EventAction& eAction)
{
	// Create a copy of the event.
	CEventEx* pEventCopy;
	
	if (CopyEvent(Event, pEventCopy) != CError::eNoError)
	{
		CCTRACEE(_T("%s : Failed to copy event.\n"), __FUNCTION__);
		return CError::eFatalError;
	}

	// Insert the copy into our queue.
	if (pEventCopy != NULL)
	{
		ccLib::CSingleLock Lock(&m_critQueue, INFINITE, FALSE);
		m_eventQueue.push(pEventCopy);
		SetEvent(m_hQueueEvent);
	}

	// If the event is not a broadcast event, commit it back to the event manager
	if (Event.GetBroadcast() == false)
	{
		CError::ErrorType eError = m_EventManager.CommitEvent(nSubscriberId, Event);

		if (eError != CError::eNoError)
		{
			CCTRACEE(_T("%s : Failed to commit event.\n"), __FUNCTION__);
			return eError;
		}
	}

	return CError::eNoError;
}

CError::ErrorType CEventsSubscriber::OnShutdown()
{
	return CError::eNoError;
}

int CEventsSubscriber::Run()
{
	CCTRACEI(_T("%s : Starting EventsSubscriber thread.\n"), __FUNCTION__);

	HRESULT hr = ccLib::CCoInitialize::CoInitialize(ccLib::CCoInitialize::eMTAModel);

	if (FAILED(hr))
	{
		CCTRACEE(_T("%s : Failed to initialize COM.  hr: %d\n"), __FUNCTION__, hr);
		return 0;
	}

	BOOL bContinue = TRUE;
	HANDLE eventHandles[] = { m_Terminating.GetHandle(), m_hQueueEvent };

	CMessageLock msgLock(TRUE, TRUE);

	for (;;)
	{
		DWORD dwResult = msgLock.Lock(CCDIMOF(eventHandles), eventHandles, FALSE, INFINITE, FALSE);

		if (dwResult == WAIT_OBJECT_0)
			break;
		else if (dwResult == WAIT_OBJECT_0 + 1)
		{
			ProcessQueue();
		}
	}

	ccLib::CCoInitialize::CoUninitialize();

	return 0;
}

BOOL CEventsSubscriber::InitializeFactories()
{
    // Create all of our event factories
	BOOL bAllSucceeded = TRUE;

	try
	{
		// Build the path to ccSetEvt.dll
		ccLib::CString sSetEvt;

		ccSym::CCommonClientInfo::GetCCDirectory(sSetEvt);
		sSetEvt += _T("\\ccSetEvt.dll");

		TCHAR* EVENT_FACTORY_ARRAY[] = {sSetEvt.GetBuffer()};

		// Load the Event factories
		if (m_EventFactories.Create(EVENT_FACTORY_ARRAY, CCDIMOF(EVENT_FACTORY_ARRAY)) == FALSE)
		{
			CCTRACEE(_T("%s : Failed to load the event factories.\n"), __FUNCTION__);
			bAllSucceeded = FALSE;
		}

		if (Register() != CError::eNoError)
			bAllSucceeded = FALSE;

		if (!bAllSucceeded)
		{
			UninitializeFactories();
		}
	}
	VP_CATCH_MEMORYEXCEPTIONS
	(
		CCTRACEE(_T("%s : Caught memory exception.\n"), __FUNCTION__);
		bAllSucceeded = FALSE;
	)

    return bAllSucceeded;
}

void CEventsSubscriber::EmptyQueue()
{
	ccLib::CSingleLock Lock(&m_critQueue, INFINITE, FALSE);

	CEventEx* pEvent = NULL;

	while (!m_eventQueue.empty())
	{
		pEvent = m_eventQueue.front();
		m_eventQueue.pop();
		pEvent->Release();
	}
}

void CEventsSubscriber::UninitializeFactories()
{
	// Unregister all the events for this factory
	Destroy();

	// Empty event queue
	EmptyQueue();

	// Delete the event factory
    m_EventFactories.Destroy();
}

void CEventsSubscriber::ProcessQueue()
{
	for (;;)
	{
		// Is there anything in the queue?
		ccLib::CSingleLock Lock(&m_critQueue, INFINITE, FALSE);

		if (m_eventQueue.empty() == true)
			break;

		// Pull the next event from the queue.
		CEventEx* pEvent = m_eventQueue.front();
		m_eventQueue.pop();

		// Unlock the queue since we've pulled out the event.
		// This allows us to queue more events while we do our processing.
		Lock.Unlock();

		// Process the event.
		switch (pEvent->GetType())
		{
		case CSettingsChangeEventEx::TypeId:
			{
				// Does this change event concern us?
				CSettingsChangeEventExQIPtr pSettingsEvent = pEvent;

				if (pSettingsEvent != NULL)
				{
					TCHAR szKey[_MAX_PATH]={0};
					DWORD dwSize = sizeof(szKey);

					// Get ccSettings hint.
					if (pSettingsEvent->GetKey(szKey, dwSize))
					{
						try
						{
							ccLib::CString sKey(szCCSet_Key_Reboot_Processing_Options);
							if (sKey.CompareNoCase(szKey) == 0)
							{
								// If the thread is already running, do not process any change notifications.
								if (!m_eraserNotify.IsThread() &&
									m_eraserNotify.ProcessNotifyType() != eNone)
									// Restart the thread if a new notification is set.
									m_eraserNotify.Start();
							}
						}
						VP_CATCH_MEMORYEXCEPTIONS
						(
							CCTRACEE(_T("%s : Caught memory exception.\n"), __FUNCTION__);
						)
					}
				}
			}
			break;
		default:
			CCTRACEE(_T("%s : Unknown event type.\n"), __FUNCTION__);
			break;
		}

		// Delete the event.
		DeleteEvent(pEvent);
	}
}

CError::ErrorType CEventsSubscriber::Register()
{
	SYMRESULT sr = SYM_OK;
	CError::ErrorType errRet = CError::eFatalError;

	sr = m_EventFactoryLoader.CreateObject(&m_pEventFactory);

	if (SYM_SUCCEEDED(sr))
	{
		sr = m_ProxyFactoryLoader.CreateObject(&m_pProxyFactory);

		if (SYM_SUCCEEDED(sr))
			if (m_EventManager.Create(m_pEventFactory, m_pProxyFactory))
				if (ccEvtMgr::CSubscriberHelper::Create(&m_EventManager, SubscribedEvents, 1))
					errRet = CError::eNoError;
	}

	return errRet;
}

CError::ErrorType CEventsSubscriber::CopyEvent(const CEventEx& Event, CEventEx* &pEventCopy)
{
    return m_EventFactories.CopyEvent(Event, pEventCopy);
}

CError::ErrorType CEventsSubscriber::DeleteEvent(CEventEx* pEvent)
{
    return m_EventFactories.DeleteEvent(pEvent);
}

void CEventsSubscriber::Destroy()
{
	CSubscriberHelper::Destroy();

	m_EventManager.Destroy();
	m_pProxyFactory.Release();
	m_pEventFactory.Release();
}