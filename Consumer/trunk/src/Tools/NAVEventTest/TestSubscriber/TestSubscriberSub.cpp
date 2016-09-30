////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"


#define INITIIDS
#include "SymInterface.h"
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

#include "TestSubscriberSub.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL CTestSubscriberSub::Connect()
{
    // Create the event manager
    HRESULT hRes;
    hRes = m_piEventManager.CreateInstance(__uuidof(EVENTMANAGERLib::EventManager));
    if (FAILED(hRes))
    {
        return FALSE;
    }

    // Create a class factory
    if (CreateNAVEventFactory(m_pEventFactory) == FALSE ||
        m_pEventFactory == NULL)
    {
        Unregister();
        return FALSE;
    }

    return TRUE;
}

BOOL CTestSubscriberSub::Register(HWND hWnd, 
                                  long nPriority,
                                  long nEventType,
                                  long& nSubscriberID)
{
    m_hWnd = hWnd;
    nSubscriberID = 0;

    // Create a subscriber
    if (CreateNAVEventSubscriber(m_pEventFactory,
                                this,
                                m_piSubscriber) == FALSE ||
        m_piSubscriber == NULL)
    {
        Unregister();
        return FALSE;
    }

    long error = CError::eNoError;
    try
    {
        // Register the subscriber
        _variant_t vEventStates;
        EVENTMANAGERLib::ISubscriberExPtr piSubscriber;
        piSubscriber = m_piSubscriber;
        error = m_piEventManager->RegisterSubscriber(piSubscriber,
                                                     nEventType,
                                                     nPriority,
                                                     vEventStates,
                                                     0,
                                                     VARIANT_FALSE,
                                                     &nSubscriberID );
        
        m_mapEventSubData[nEventType] = nSubscriberID;
    }
    catch (...)
    {
        error = CError::eFatalError;
        nSubscriberID = 0;
    }

    if (error != CError::eNoError ||
        nSubscriberID == 0)
    {
        Unregister();
        return FALSE;
    }

    return TRUE;
}

BOOL CTestSubscriberSub::Disconnect()
{
    Unregister();

    if (m_piEventManager != NULL)
    {
        m_piEventManager.Release();
    }

    return TRUE;
}

BOOL CTestSubscriberSub::Unregister()
{
    if (m_piEventManager != NULL)
    {
        try
        {
            for ( iterSubData iter = m_mapEventSubData.begin();
                  iter != m_mapEventSubData.end ();
                  iter++)
            {
                long lSubscriberID = (*iter).second;
                
                if ( lSubscriberID )
                    m_piEventManager->UnregisterSubscriber(lSubscriberID);
            }

            m_mapEventSubData.erase ( m_mapEventSubData.begin (), m_mapEventSubData.end ());
        }
        catch(...)
        {
        }
    }

    if (m_piSubscriber != NULL)
    {
        DeleteNAVEventSubscriber(m_piSubscriber);
        m_piSubscriber = NULL;
    }

    if (m_pEventFactory != NULL)
    {
        DeleteNAVEventFactory(m_pEventFactory);
        m_pEventFactory = NULL;
    }

    m_hWnd = NULL;

    return TRUE;
}

void CTestSubscriberSub::DeleteEvent(CEventEx* pEvent)
{
    if (m_pEventFactory != NULL)
    {
        m_pEventFactory->DeleteEvent(pEvent);
        pEvent = NULL;
    }
}
/*
CError::ErrorType CTestSubscriberSub::OnEvent(long nSubscriberId, 
                                              IEventEx* piEvent,
                                              CSubscriberEx::EventAction& eAction)
                                              */
CError::ErrorType CTestSubscriberSub::OnEvent(long nSubscriberId, 
                                              const CEventEx& Event,
                                              CSubscriberEx::EventAction& eAction)
{
    eAction = m_eAction;

    if (m_hWnd != NULL &&
        m_pEventFactory != NULL)
    {
        m_pCurrentEvent = 0;

        // Copy event
        //
        if (m_pEventFactory->CopyEvent(Event, m_pCurrentEvent) == CError::eNoError)
            ::PostMessage(m_hWnd, UM_EVENT, eAction, reinterpret_cast<LPARAM>(m_pCurrentEvent));

    }
    return CError::eNoError;
}

BOOL CTestSubscriberSub::CommitEvent(CEventEx* pEvent)
{
    // Copy event
    IEventEx* pIEvent = NULL;
    if (m_pEventFactory->CopyEvent(*pEvent, pIEvent) != CError::eNoError)
    {
        return FALSE;
    }

    BOOL bRet = TRUE;
    try
    {
        EVENTMANAGERLib::IEventExPtr piEvent;
        piEvent = pIEvent;

        long lSubscriberID = m_mapEventSubData[piEvent->GetType ()];
        
        if ( lSubscriberID )
            m_piEventManager->CommitEvent(lSubscriberID, piEvent);
    }
    catch(...)
    {
        bRet = FALSE;
    }

    m_pEventFactory->DeleteEvent(pIEvent);
    pIEvent = NULL;

    return bRet;
}

BOOL CTestSubscriberSub::CancelEvent(CEventEx* pEvent)
{
    // Copy event
    IEventEx* pIEvent = NULL;
    if (m_pEventFactory->CopyEvent(*pEvent, pIEvent) != CError::eNoError)
    {
        return FALSE;
    }

    BOOL bRet = TRUE;
    try
    {
        EVENTMANAGERLib::IEventExPtr piEvent;
        piEvent = pIEvent;
        long lSubscriberID = m_mapEventSubData[piEvent->GetType ()];
        
        if ( lSubscriberID )
            m_piEventManager->CancelEvent(lSubscriberID, piEvent);
    }
    catch(...)
    {
        bRet = FALSE;
    }

    m_pEventFactory->DeleteEvent(pIEvent);
    pIEvent = NULL;

    return bRet;
}

CError::ErrorType CTestSubscriberSub::OnShutdown()
{
    // We may not directly unregister
    // Posting a message to unregister is safe
    if (m_hWnd != NULL &&
        m_pEventFactory != NULL)
    {
        ::PostMessage(m_hWnd, UM_SHUTDOWN, 0, 0);
    }
    return CError::eNoError;
}

void CTestSubscriberSub::SetOnEventAction(int nAction)
{
    switch (nAction)
    {
    case 0:
        m_eAction = CSubscriberEx::eNoAction;
        break;
    case 1:
        m_eAction = CSubscriberEx::eCommitAction;
        break;
    case 2:
        m_eAction = CSubscriberEx::eCancelAction;
        break;
    default:
        ASSERT(FALSE);
        m_eAction = CSubscriberEx::eNoAction;
        break;
    }
}