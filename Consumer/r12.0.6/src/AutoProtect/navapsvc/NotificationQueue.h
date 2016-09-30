// NotificationQueue.h: interface for the CNotificationQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFICATIONQUEUE_H__F4AE4A87_3699_4B83_89AF_7C22F9E50E43__INCLUDED_)
#define AFX_NOTIFICATIONQUEUE_H__F4AE4A87_3699_4B83_89AF_7C22F9E50E43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "syncqueue.h"
#include "notification.h"
#include "quarantinethread.h"
#include "AvEvents.h"

class CNotificationQueue : public CSyncQueue< CNotification* >  
{
public:
	CNotificationQueue( CEventHandlers& handlers );
	virtual ~CNotificationQueue();

private:
	// Disallowed
	CNotificationQueue();
	CNotificationQueue( CNotificationQueue& other );

	// Override of CSyncQueue<>::processQueue()
	virtual void processQueue();
    
    // Sends the notification to the registered clients
    //
    void sendNotification ( CEventData& eventData,  bool bAllClients = false );

	// Event handlers object
	CEventHandlers&				m_handlers;

	// Quarantine thread object
	CQuarantineThread			m_QuarantineThread;

    // FILO container of cookies for scans that have been stopped (completed, cancelled, etc.)
    // This is in case we get a Block app for a scan that has already
    // been cancelled. Otherwise we wait for a loooong time for a
    // cancel event that will never come.
    //
    ccLib::CCriticalSection m_critStopScanCookies;
    typedef std::deque <SAVRT_ROUS64> dqStopScanCookies;
    typedef dqStopScanCookies::iterator iterStopScanCookies;

    dqStopScanCookies m_dqStopScanCookies;

    // returns true if the cookie is in the cancelled scan list
    //
    bool wasScanStopped (SAVRT_ROUS64 hCookie);
};

#endif // !defined(AFX_NOTIFICATIONQUEUE_H__F4AE4A87_3699_4B83_89AF_7C22F9E50E43__INCLUDED_)
