#include "StdAfx.h"

// This CPP gets compiled first so this block goes here. 
// This block MUST get compiled before ANY SymInterface stuff gets used.
//
// For SymInterface
#define INITIIDS
#include "AvEvents.h"
#include "providermanager.h"

CProviderManager::CProviderManager(void)
{
    m_hNewWorkEvent = ::CreateEvent ( NULL, TRUE, FALSE, NULL );
    
    // This class wants to respond to items added to this queue.
    //
    m_qEvents.RegisterEventSink(this);
}

CProviderManager::~CProviderManager(void)
{

    ::CloseHandle (m_hNewWorkEvent);

    // Empty the queue
    //
    long lSize = ((qRealEvents*)(&m_qEvents))->size();

    CEventData* pEvent;

    if ( 0 != lSize )
    {
        m_qEvents.PopItem (pEvent);

        if ( pEvent )
        {
            // Delete it
            delete pEvent;
            pEvent = NULL;
        }
    }

}

int CProviderManager::DoWork ()
{
	// Attempt to initialize COM - MTA
    //
	HRESULT hrCOMInit = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    bool bCOMInitialized = false;

    if ( hrCOMInit != S_FALSE ) // already init
    {
        if ( hrCOMInit == S_OK )
            bCOMInitialized = true;
        else
        {
            if( FAILED (hrCOMInit))
            {
                CCTRACEE ( "CProviderManager::DoWork () failed to COINIT_MULTITHREADED" );
                return -1;
            }
        }
    }

    // Wait for new work to do.
    HANDLE hWaitHandles[2];
    hWaitHandles[0] = m_hShutdownEvent;
    hWaitHandles[1] = m_hNewWorkEvent;

    DWORD dwWaitResult = 0;

    // We must load this module in the same thread that we send events.    

    if (SYM_FAILED (m_pLoggerLoader.CreateObject ( m_pLogger.m_p )) ||
                    m_pLogger.m_p == NULL )
    {
        CCTRACEE ( _T("CProviderManager - Could not create IAvLogger object. - %d"), ::GetLastError() );
        m_pLogger = NULL;
        goto cleanup;
    }

    ::SetEvent (m_hThreadStartedEvent);

    for (;;)
    {
        dwWaitResult = StahlSoft::WaitForMultipleObjectsWithMessageLoop (2, &hWaitHandles[0], INFINITE );

        if ( dwWaitResult != WAIT_OBJECT_0 && dwWaitResult != WAIT_OBJECT_0 + 1 )
        {
            CCTRACEE ( "CProviderManager::DoWork() - Error in WaitForMultipleObjectsWithMessageLoop()" );
            goto cleanup;
        }

        // Shutdown?
        //
        if ( dwWaitResult == WAIT_OBJECT_0 )
        {
            CCTRACEI ( "CProviderManager::DoWork() - Got shutdown, shutting down." );
            goto cleanup;
        }

        // This weird function mess is to get around an ambigous call to size () since
        // there's two queues, each with a size () function. We want to call
        // the one that tell us how many work items are left, not how many sink clients
        // there are.
        //
        long lSize = ((qRealEvents*)(&m_qEvents))->size();

        if ( 0 != lSize )
        {
            CEventData* pEvent;

            m_qEvents.PopItem (pEvent);

            if ( pEvent )
            {
                SYMRESULT result = m_pLogger->BroadcastAvEvent (*pEvent);
                if ( SYM_FAILED (result))
                {
                    CCTRACEE ( _T("NAVTestEventProvider : DoWork () - BroadcastAvEvent failed - %d"), result);
                }

                // Delete it
                delete pEvent;
                pEvent = NULL;
            }
        }
        else
            ::ResetEvent (m_hNewWorkEvent);

    } // loop until thread closes

cleanup:
    ::SetEvent (m_hThreadStartedEvent);
    return 1;
}

void CProviderManager::OnItemAdded ()
{
    ::SetEvent (m_hNewWorkEvent);
}

void CProviderManager::SendEvent (CEventData& Event)
{
    CEventData* pEvent = new CEventData (Event);
    if ( pEvent )
    {
        m_qEvents.PushItem (pEvent);
    }
}