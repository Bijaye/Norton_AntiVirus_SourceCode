/////////////////////////////////////////////////////////////////////////////
// events.cpp
//
// This file contains code to handle the various events fired from the NAVAP
// driver.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "navapsvc.h"
#include "notificationqueue.h"
#include "Notification.h"
#include "scanqueue.h"
#include "actions.h" // for AVCONTEXT

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::handleProgressEvent()

void CServiceModule::handleProgressEvent( PCSAVRT_EVENTREPORTPACKET pEvent, 
										  DWORD dwInBufferSize )
{
	// Populate a notification object with data from the packet and
	// add it to the queue.
	try
	{
		if( m_pNotificationQueue )
        {
            CCTRACEI (_T("NAVAPSVC - Received : Threat"));

            // Filter out identical alerts within a 5 second interval
            // This is more for Spyware events than Viral events since we get final report
            // events with all actions now for viral. Spyware gives us multiple duplicate
            // events, so we're keeping this functionality.
            //
            if( pEvent->dwAVContext == AVCONTEXT_SCANFILE )
            {
                PSAVRT_THREATENTRY pCurrentEntry = (PSAVRT_THREATENTRY)&(pEvent->byData[pEvent->dwEntryIndex]);

                // Compare the current packet to the last one we recieved
                if( m_LastEvent.dwVID == pCurrentEntry->dwVirusID &&
                    0 == m_LastEvent.wsFile.compare((LPWSTR)&pEvent->byData[pEvent->dwFileInTroubleIndex]) )
                {
                    // This is the same vid/file we last recieved
                    
                    // Check to see if we've already displayed the current action for this item
                    if( pEvent->Action1.dwAction == m_LastEvent.dwActions[0] ||
                        pEvent->Action1.dwAction == m_LastEvent.dwActions[1] ||
                        pEvent->Action1.dwAction == m_LastEvent.dwActions[2] )
                    {
                        // We have a duplicate action now check to see if the time since
                        // we last displayed it is over 5 seconds
                        CTime curTime = CTime::GetCurrentTime();
                        CTimeSpan ts = curTime - m_LastEvent.time;
                        if( ts.GetSeconds() < 5 )
                        {
                            CCTRACEI(_T("CServiceModule::handleProgressEvent() - recieved the same event in a 5 second window. Going to eat it. VID = %lu"), m_LastEvent.dwVID);
                            return;
                        }
                        else
                        {
                            // Time is greater than 5 seconds since we showed this alert, so show it again
                            // but reset the time interval and actions for it
                            CCTRACEI(_T("CServiceModule::handleProgressEvent() - The same event has been recieved, but it's been longer than 5 seconds so we'll show it again."));
                            m_LastEvent.time = CTime::GetCurrentTime();
                            m_LastEvent.dwActions[0] = pEvent->Action1.dwAction;
                            m_LastEvent.dwActions[1] = m_LastEvent.dwActions[2] = -1;
                        }
                    }
                    else
                    {
                        // This action has not been displayed to the user yet so we'll show it
                        // and save off the current action
                        for(int i=0; i<3; i++)
                        {
                            // Save this action in the last unused action field, if we're at the end (i==2),
                            // which should never happen because the max number of actions for a single vid/file
                            // match is 3 (repair failed/quarantine failed/denied access), then save it to the
                            // end so if it comes in again we won't keep displaying it
                            if( m_LastEvent.dwActions[i] == -1 || i==2 )
                            {
                                CCTRACEI(_T("CServiceModule::handleProgressEvent() - Same event new action. Saving to dwActions[%d] and displaying."), i);
                                m_LastEvent.dwActions[i] = pEvent->Action1.dwAction;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // This is a new file/vid so save it off and display it

                    // Save the last packed event
                    m_LastEvent.time = CTime::GetCurrentTime();
                    m_LastEvent.dwVID = pCurrentEntry->dwVirusID;
                    m_LastEvent.wsFile = (LPWSTR) &pEvent->byData[pEvent->dwFileInTroubleIndex];

                    // Set the first action as the current one and reset the others
                    m_LastEvent.dwActions[0] = pEvent->Action1.dwAction;
                    m_LastEvent.dwActions[1] = m_LastEvent.dwActions[2] = -1;
                }
            }

            m_pNotificationQueue->AddItem( new CNotification( pEvent, dwInBufferSize ) );
			CCTRACEI (_T("NAVAPSVC - Received : Processing"));
        }
	}
	catch( bad_alloc e )
	{
		LogEvent( _T("Out of memory" ) );
        CCTRACEE( _T("Out of memory" ) );
	}
	catch(...)
	{
		LogEvent( _T("Error processing progress event" ) );
        CCTRACEE( _T("Error processing progress event" ) );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::handleContainerReportEvent()

void CServiceModule::handleContainerReportEvent( PSAVRT_CONTAINERREPORTPACKET pEvent, 
										         DWORD dwInBufferSize )
{
	// Populate a notification object with data from the packet and
	// add it to the queue.
	try
	{
		if( m_pNotificationQueue )
        {
            CCTRACEI (_T("NAVAPSVC - Received : Container Report"));
            m_pNotificationQueue->AddItem( new CNotification( pEvent, dwInBufferSize ) );
			CCTRACEI (_T("NAVAPSVC - Received : Processing"));
        }
        else
            CCTRACEE (_T("NAVAPSVC - Received : Container Report - NotificationQ gone!!!"));
	}
	catch( bad_alloc e )
	{
		LogEvent( _T("Out of memory" ) );
        CCTRACEE( _T("Out of memory" ) );
	}
	catch(...)
	{
		LogEvent( _T("Error processing progress event" ) );
        CCTRACEE( _T("Error processing progress event" ) );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CServiceModule::handleSemiSyncNotificationEvent()

void CServiceModule::handleSemiSyncNotificationEvent( PSAVRT_SEMISYNCSCANNOTIFICATIONPACKET pEvent, 
										              DWORD dwInBufferSize )
{
	// Populate a notification object with data from the packet and
	// add it to the queue.
	try
	{
		if( m_pNotificationQueue )
			m_pNotificationQueue->AddItem( new CNotification( pEvent, dwInBufferSize ) );
	}
	catch( bad_alloc e )
	{
		LogEvent( _T("Out of memory" ) );
	}
	catch(...)
	{
		LogEvent( _T("Error processing progress event" ) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CServiceModule::handleScanMountEvent()

void CServiceModule::handleScanMountEvent( PSAVRT_SCANMOUNTEDDEVICEPACKET pEvent, 
										   DWORD dwInBufferSize )
{
	UNREFERENCED_PARAMETER( dwInBufferSize );
	
	try
	{
		// Add this scan request to the queue.
        if ( !m_pScanQueue )
        {
            if (!CreateScanQueue())
                return;
        }

		m_pScanQueue->AddItem( CScanQueueItem( pEvent ) );
	}
	catch(...)
	{
		LogEvent( _T("Error processing scan boot record event" ) );
	}
}