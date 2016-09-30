////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// AvFactory.cpp: implementation of the CAvFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "AvFactory.h"
#include "NAVEventFactoryLoader.h"
#include "ccSingleLock.h"
#include "ccMessageLock.h"
#include "ccCriticalSection.h"
#include "NAVEventCommon.h"
#include "ccSymModuleLifetimeMgrHelper.h"

using namespace ccEvtMgr;
using namespace ccLib;

///////////////////////////////////////////////////////////////////////////////
// CAvFactory::CAvFactory()

CAvFactory::CAvFactory() :
	m_bShutdown( false ),
    m_bInit(false),
    m_bBroadcast(true)
{
    //CCTRACEI ( "CAvFactory::CAvFactory()" );
}

///////////////////////////////////////////////////////////////////////////////
// CAvFactory::~CAvFactory()

CAvFactory::~CAvFactory()
{
    destroy();
}

SYMRESULT CAvFactory::destroy()
{
    CSingleLock lock ( &m_critEvent, INFINITE, FALSE );

    // Destroy in reverse order
    m_EventManager.Destroy();

    m_bInit = false;

    return SYM_OK;
}


SYMRESULT CAvFactory::createNAVEvent(CEventEx*& pReturnEvent)
{
    CCTRACEI ( _T("CAvFactory::createNAVEvent") );

    // Create the real event. Any supported type will do.
    //
    SYMRESULT result = SYMERR_UNKNOWN;

	// Clear output.
	pReturnEvent = NULL;
    
    if( CError::eNoError != m_EventManager.NewEvent ( AV::Event_ID_Threat, pReturnEvent ))
    {
        CCTRACEE ( _T("CAvFactory::createNAVEvent:SYMERR_OUTOFMEMORY") );
        return SYMERR_OUTOFMEMORY;
    }

    if ( pReturnEvent )
    {
        CCTRACEI ( _T("CAvFactory::createNAVEvent - OK") );
        return SYM_OK;
    }

	// If we get here, bad things have happened.
		
	return result;
}

///////////////////////////////////////////////////////////////////////////////
// CAvFactory::initialize()

SYMRESULT CAvFactory::initialize(bool bBroadcast)
{
    // If we already initialized, but in the other style,
    // destroy and re-create.
    if ( m_bInit && m_bBroadcast == bBroadcast)
        return SYM_OK;

    if ( m_bInit && m_bBroadcast != bBroadcast )
    {
        destroy();
    }

    ccEvtMgr::CEventFactoryEx2Ptr m_pEventFactory;
    ccEvtMgr::CProxyFactoryExPtr m_pProxyFactory;

    // Load the factory and proxy DLLs
    if (SYM_FAILED(AV::NAVEventFactory_CEventFactoryEx2::CreateObject (GETMODULEMGR(), &m_pEventFactory)) ||
        m_pEventFactory == NULL ||
        SYM_FAILED(AV::NAVEventFactory_CProxyFactoryEx::CreateObject(GETMODULEMGR(), &m_pProxyFactory)) ||
        m_pProxyFactory == NULL ||
        m_EventManager.Create(m_pEventFactory, m_pProxyFactory) == FALSE)
    {
        destroy();
        return AVFACTORY_ERR_EVENT_MGR;
    }
    
    // All is well
    m_bBroadcast = bBroadcast;
    m_bInit = true;
    m_EventManager.AddRef();

    return SYM_OK;
}


SYMRESULT CAvFactory::BroadcastAvEvent( CEventData& event )
{
    CEventData* pNothing = NULL;
    return sendAvEvent ( event, pNothing );
}

SYMRESULT CAvFactory::SendAvEvent( CEventData& event, CEventData*& pEventReturn )
{
    return sendAvEvent ( event, pEventReturn, false );
}

SYMRESULT CAvFactory::SendAvEvent( CEventData& event, CEventData*& pEventReturn, DWORD dwWaitTime)
{
    return sendAvEvent ( event, pEventReturn, false, dwWaitTime );
}

SYMRESULT CAvFactory::SendAvEvent( CEventData& event, CEventData*& pEventReturn, DWORD dwWaitTime, HANDLE hTerminate )
{
    return sendAvEvent ( event, pEventReturn, false, dwWaitTime, hTerminate );
}

///////////////////////////////////////////////////////////////////////////////
// CAvFactory::SendAvEvent()

SYMRESULT CAvFactory::sendAvEvent( CEventData& event,
                                   CEventData*& pEventReturn /*out*/,
                                   bool bBroadcast /*true*/,
                                   DWORD dwWaitTime, /*30 secs*/
                                   HANDLE hTerminate /*NULL*/ )
{
    CSingleLock lock ( &m_critEvent, INFINITE, FALSE );

    // Not initialized!
    //
    SYMRESULT result = initialize (bBroadcast);
    if ( SYM_FAILED (result))
        return result;

    //CCTRACEI ( "CAvFactory::BroadcastAvEvent Submitting Event" );
	if( m_bShutdown ) 
	{
		// Event manager is shutting down.  We cannot route this
		// event.
		return AVFACTORY_ERR_EVENT_MGR;
	}
	
	// Fetch pointer to real event object.
    CEventEx* pEvent = NULL;
    createNAVEvent ( pEvent );

    if( pEvent == NULL )
    {
		CCTRACEE ( _T("CAvFactory::SendAvEvent pEventToSend == NULL") );
        return AVFACTORY_ERR_COM;
    }

    CNAVEventCommonQIPtr pEventToSend (pEvent);
	pEvent->Release();	//ACR##
    // Copy the data from the original event to this temp one.
    //
    pEventToSend->props = event;

    pEventToSend->InitCommon();

    // We *MUST* set the type here or the NAV Event Factory will reject it.
    //
    long lType = AV::Event_ID_Base;
    event.GetData ( AV::Event_Base_propType, lType );
    pEventToSend->SetType ( lType );

	// Send the event off to the event manager.
    long nEventId = 0;
    CError error;
    CEventEx* pReturnEventPlaceholder = NULL;
    pEventReturn = NULL;

    // Create a provider to send events
    CSymStaticRefCount<CProviderHelper> ProviderHelper;

    if( !ProviderHelper.Create(&m_EventManager, bBroadcast?FALSE:TRUE) )
    {
        CCTRACEE(_T("CAvFactory::sendAvEvent() - ProviderHelper.Create() failed.\n"));
        return AVFACTORY_ERR_COM;
    }

    try
    {
        if ( bBroadcast )
        {
            error = ProviderHelper.CreateEvent(*(pEventToSend.m_p), 
                                                true,
                                                nEventId); 
        }
        else
        {
            error = ProviderHelper.CreateEvent(*(pEventToSend.m_p), 
                                                false,
                                                nEventId,
                                                hTerminate,
                                                dwWaitTime,
                                                TRUE, // pump, otherwise we get RPC errors
                                                pReturnEventPlaceholder);

            if ( error.m_eError == ccEvtMgr::CError::eNoError &&
                 pReturnEventPlaceholder)                
            {
                CNAVEventCommon* pEventToReturn = static_cast<CNAVEventCommon*>(pReturnEventPlaceholder);
                pEventReturn = new CEventData (pEventToReturn->props);

                long lResult = pEventToReturn->GetState();
            }
        }
        if ( error.m_eError != ccEvtMgr::CError::eNoError )
            CCTRACEI ( _T("CAvFactory::SendAvEvent:CreateEvent - failure %d"), error.m_eError );
    }
    catch (std::bad_alloc&)
    {
        CCTRACEE ( _T("CAvFactory::SendAvEvent::CreateEvent"));
        error = ccEvtMgr::CError::eFatalError;
    }


    // Explictly call Destroy(). The static ref count prevents double destruction
    ProviderHelper.Destroy();

    return error.m_eError == ccEvtMgr::CError::eNoError ? SYM_OK : AVFACTORY_ERR_COM;
}
