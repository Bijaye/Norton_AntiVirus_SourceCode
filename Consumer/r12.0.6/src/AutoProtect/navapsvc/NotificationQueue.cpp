// NotificationQueue.cpp: implementation of the CNotificationQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "navapsvc.h"
#include "NotificationQueue.h"
#include "AvEvents.h"
#include "AllNavEvents.h"
#include "NAVEventFactoryLoader.h"
#include "actions.h"
#include "ccSymModuleLifetimeMgrHelper.h"

#define MAX_CANCEL_SCAN_COOKIES_SAVED 1024 // # of stopped scan cookies to save, 1024 = 8KB.

//////////////////////////////////////////////////////////////////////
// CNotificationQueue::CNotificationQueue()

CNotificationQueue::CNotificationQueue( CEventHandlers& handlers ) :
	m_handlers( handlers )
{
    m_Options.m_eCOMModel = ccLib::CCoInitialize::eMTAModel;
    m_Options.m_bPumpMessages = TRUE;

    // Start the quarantine thread.
	m_QuarantineThread.Create ( NULL, 0, 0 );
}

//////////////////////////////////////////////////////////////////////
// CNotificationQueue::~CNotificationQueue()

CNotificationQueue::~CNotificationQueue()
{
	// Clean up queue
	CNotification* notification;
	while( getQueueFront( notification ) )
	{
		delete notification;
        notification = NULL;
	}

    // Erase the cache. I don't think this is really needed since they are on the stack.
    //
    m_dqStopScanCookies.erase (m_dqStopScanCookies.begin (), m_dqStopScanCookies.end ());
}


//////////////////////////////////////////////////////////////////////
// CNotificationQueue::processQueue()

void CNotificationQueue::processQueue()
{
    CCTRACEI ( _T("CNotificationQueue::ProcessQueue() - start"));	

    // Empty the queue.
    CNotification* pNotification;
    
    // In case we need to log
    AV::IAvEventFactoryPtr pLogger;

	while( !IsTerminating() && getQueueFront( pNotification ))
	{
        CEventData eventSend;

        // MakeEvent will return false if all the threats are excluded
        if ( pNotification->MakeEvent ( eventSend, _Module.GetThreatExclusions() ))
        {
            // Get general event type
            //
            switch ( pNotification->GetType () )
            {
                case CNotification::ThreatEvent:
                case CNotification::ContainerReport:
                {
                    // Different on 9x so we have to do this here, not in CNotification
                    //
                    eventSend.SetData ( AV::Event_Threat_propDefsRevision, _Module.Config().GetDefsRevision());
                    eventSend.SetData ( AV::Event_Threat_propProductVersion, _Module.Config().GetProductVersion());

                    if ( _Module.Config().GetShouldLog() )
                    {
                        if ( !pLogger )
                        {
                            if (SYM_FAILED (AV::NAVEventFactory_IAVEventFactory::CreateObject (GETMODULEMGR(), &pLogger)) ||
                                pLogger.m_p == NULL )
                            {
                                CCTRACEE ( _T("CActivityLogger - Could not create IAvFactory object. - %d"), ::GetLastError() );
                                pLogger = NULL;
                            }
                        }

                        if ( pLogger )
                            pLogger->BroadcastAvEvent (eventSend);
                    }

                    sendNotification ( eventSend );
                }
                break;

                case CNotification::ContainerStatus:
                {
                    long lStatus = 0;
                    SAVRT_ROUS64 cookie64;
                    long lCookieSize = sizeof (SAVRT_ROUS64);
                    eventSend.GetData ( AV::Event_Container_Status, lStatus );
                    eventSend.GetData ( AV::Event_Container_Cookie, (BYTE*) &cookie64, lCookieSize );

                    switch (lStatus)
                    {
                        case AV::Event_Container_Status_STARTED:
                            sendNotification ( eventSend, false );   // send to Session ID
                        break;

                        case AV::Event_Container_Status_COMPLETED:
                        case AV::Event_Container_Status_CANCELLED:
                        case AV::Event_Container_Status_ABORTED:
                        {
                            // Add cookie to the cookie cache.
                            //
                            ccLib::CSingleLock lock( &m_critStopScanCookies, INFINITE, FALSE );
                            m_dqStopScanCookies.push_back (cookie64);
                            if ( m_dqStopScanCookies.size () > MAX_CANCEL_SCAN_COOKIES_SAVED )
                            {
                                m_dqStopScanCookies.pop_front (); // Remove the oldest item
                            }
                            
                            sendNotification ( eventSend, true );    // broadcast to all
                        }
                        break;

                        case AV::Event_Container_Status_BLOCKED:
                            // If blocked app alerts are off, don't send this to the client
                            //
                            if ( _Module.Config().GetShowBlockedAppAlerts() &&
                                !wasScanStopped(cookie64) )
                                sendNotification ( eventSend, false );
                        break; 
                    }
                }
                break;
            }
        }

		// kill the notification.
		if ( pNotification )
        {
            delete pNotification;
            pNotification = NULL;
        }
	} // end while

   	// Finish any pending Quarantine operations.
	m_QuarantineThread.Signal();
    CCTRACEI ( _T("CNotificationQueue::ProcessQueue() - end"));
}

// Sends the notification to the registered clients
//
void CNotificationQueue::sendNotification ( CEventData& eventData, bool bAllClients /*false*/ )
{
	VARIANT v;
	VariantInit( &v );
	try
	{
		// Construct Variant containing infection data.
		eventData.SerializeToVariant( v );

        if ( bAllClients )
        {
            m_handlers.BroadcastData ( v );
        }
        else
        {
	        // Find the handler for this nofification.
	        long lSessionID = 0;
            if ( eventData.GetData ( AV::Event_Base_propSessionID, lSessionID ))
            {
                INAVAPEventHandler* pHandler = m_handlers.LookupHandler( lSessionID );
	            if( pHandler )
	            {
		            try
		            {
			            CCTRACEI ( _T("NAVAPSVC - Session data to session - %d"), lSessionID );
                        // Send to client.
			            pHandler->OnEvent( v );
		            }
		            catch(...)
		            {
			            // Ignore, since client may have disconnected without 
			            // unregistering thier handler.
			            _ASSERT( FALSE );
		            }

		            pHandler->Release();
	            }
                else
                    CCTRACEE ( _T("NAVAPSVC - No handlers for - %d"), lSessionID );
            }
            else
                CCTRACEE ( _T("NAVAPSVC - No session ID!"));
        }
    }
    catch (...)
    {
        _ASSERT(FALSE);
    }

    // Clean things up.
    VariantClear( &v );
}

bool CNotificationQueue::wasScanStopped (SAVRT_ROUS64 hCookie)
{
    ccLib::CSingleLock lock( &m_critStopScanCookies, INFINITE, FALSE );

    for ( iterStopScanCookies iter = m_dqStopScanCookies.begin ();
            iter != m_dqStopScanCookies.end();
            iter++)
    {
        if ( 0 == memcmp ( &(*iter), &hCookie, sizeof (SAVRT_ROUS64)))
        {
            return true;
        }
    }

    return false;
}

