#pragma once

#include "ccThread.h"
#include "ccSyncLock.h"
#include <queue>
#include <vector>

//////////////////////////////////////////////////////////////////////
//
// Thread safe STL Queue.
//
// template< class T > class CSyncQueue

template< class T >
class CSyncQueue : public ccLib::CThread  
{
public:
	CSyncQueue();
	virtual ~CSyncQueue();
	virtual void AddItem( const T& item );             // Add a single item
    virtual void AddItems( std::vector< T > &vItems ); // Add multiple items. Performance based.
    virtual size_t GetSize();                          // Returns # of items in the Q.

protected:
	virtual bool getQueueFront( T& item );              // Pop - only used inside the processQueue
	virtual void processQueue() = 0;                    // Does the real work.
    virtual void shutdown (void);                       // Called when the thread is about to shutdown

	// Override of CThread::Run()	
	int Run();

	// Queue of items
    std::queue< T >				m_queue;

	// Critical section for queue.
    ccLib::CCriticalSection     m_csQueue;

	// Event object for item insertion.
    ccLib::CEvent 				m_eventQueueInsertion;
};

//////////////////////////////////////////////////////////////////////
// CSyncQueue::~CSyncQueue()

template< class T > 
CSyncQueue< T >::CSyncQueue( )
{
	// Create event objects
	if ( !m_eventQueueInsertion.Create( NULL, FALSE, FALSE, NULL, FALSE ))
        throw std::runtime_error( "Could not create event." );
}

//////////////////////////////////////////////////////////////////////
// CSyncQueue::~CSyncQueue()

template< class T > 
CSyncQueue< T >::~CSyncQueue()
{
}

//////////////////////////////////////////////////////////////////////
// CSyncQueue::AddItem()

template< class T > 
void CSyncQueue< T >::AddItem( const T& item )
{
	if ( IsTerminating() )
        return;

    ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
	m_queue.push( item );
    m_eventQueueInsertion.SetEvent();
}

//////////////////////////////////////////////////////////////////////
// CSyncQueue::AddItems()

template< class T > 
void CSyncQueue< T >::AddItems( std::vector< T >& vItems )
{
	if ( IsTerminating() )
        return;

    ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
    std::vector<T>::iterator it;
	for( it = vItems.begin(); it != vItems.end(); ++it )
	{
		m_queue.push( *it );
	}
	m_eventQueueInsertion.SetEvent();
}

//////////////////////////////////////////////////////////////////////
// CSyncQueue::Run()

template< class T > 
int CSyncQueue< T >::Run()
{
    // Set up waiting objects.

    HANDLE apWaitObjects[] = {m_Terminating.GetHandle(), m_eventQueueInsertion.GetHandle() };
    ccLib::CMessageLock msglock (TRUE, FALSE);

	for(;;)
	{
        // Wait here for something to happen
        DWORD dwWait = msglock.Lock ( 2, &apWaitObjects[0], FALSE, INFINITE, FALSE ); // pump messages
		
		// Check for error.
		if( dwWait == WAIT_FAILED )
		{
            return 0xFFFFFFFF;
		} 
		else if( dwWait - WAIT_OBJECT_0 == 0 )
		{
			// exit signaled.
			shutdown();
			return 0;
		}
		
		// Process the queue.
		processQueue();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// CSyncQueue::getQueueFront()

template< class T > bool CSyncQueue< T >::getQueueFront( T& item )
{
	if ( IsTerminating() )
        return false;

    ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
		
	if( m_queue.empty() )
		return false;

	item = m_queue.front();
	m_queue.pop();
	return true;
}

//////////////////////////////////////////////////////////////////////
// CSyncQueue::GetSize()

template< class T > size_t CSyncQueue< T >::GetSize()
{
    ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
	return( m_queue.size() );
}

template< class T> void CSyncQueue< T >::shutdown ()
{
    return;
}