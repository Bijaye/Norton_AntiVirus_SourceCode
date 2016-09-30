// ScanQueue.h: interface for the CScanQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANQUEUE_H__7BBFF1AD_0038_41A6_9712_FD4647DB8ED4__INCLUDED_)
#define AFX_SCANQUEUE_H__7BBFF1AD_0038_41A6_9712_FD4647DB8ED4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "syncqueue.h"
#include "scanner.h"
#include <set>

//////////////////////////////////////////////////////////////////////
// class CScanQueueItem
//
class CScanQueueItem 
{
public:
	CScanQueueItem();
	CScanQueueItem( BYTE byDevice );
	CScanQueueItem( const CScanQueueItem& other );
	CScanQueueItem( const SAVRT_SCANMOUNTEDDEVICEPACKET* pPacket);
	CScanQueueItem& operator=( const CScanQueueItem& other );
	~CScanQueueItem();
	bool FromService();
	BYTE GetDeviceID() const;
	void SetDeviceID( BYTE byDevice );
	SAVRT_SCANMOUNTEDDEVICEPACKET * GetPacket();
	DWORD GetTickCount(){ return m_tc; }

	bool operator==(const CScanQueueItem& other ) const;

private:
	SAVRT_SCANMOUNTEDDEVICEPACKET *m_pPacket;
	BYTE m_byDevice;

	DWORD m_tc;
};

//////////////////////////////////////////////////////////////////////
// class CScanQueue
//

class CScanQueue : public CSyncQueue< CScanQueueItem >  
{
public:
	CScanQueue( CNotificationQueue& notifyQueue );
	virtual ~CScanQueue();

	void Pause();
	void Resume();

	virtual void AddItem( const CScanQueueItem& item );
    virtual void AddItems( ::std::vector< CScanQueueItem > &vItems );

protected:
	// Override of CSyncQueue::processQueue()
	virtual void processQueue();
	virtual bool getQueueFront( CScanQueueItem& item );


	// Scan routines.
	void scanMasterBootRecord( CScanner* pScanner, CScanQueueItem& item );

	void reportVirus( CScanQueueItem& item, IScanResults* pResults );

private:
	// Disallowed
	CScanQueue( );
	CScanQueue( CScanQueue& other );

	// Notification queue object
	CNotificationQueue&			m_NotificationQueue;

	// Critical section object used for pause.
    ccLib::CCriticalSection		m_csPause;

	// Set used to check for duplicate entries
	::std::set<DWORD>					m_dupSet;
};

#endif // !defined(AFX_SCANQUEUE_H__7BBFF1AD_0038_41A6_9712_FD4647DB8ED4__INCLUDED_)
