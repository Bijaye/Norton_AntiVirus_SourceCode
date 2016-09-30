//////////////////////////////////////////////////////////////////////
// APDequeue.h: interface for the APDequeue template class.
//
// Double-ended thread safe queue hosted in a thread.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if (_WIN32_WINNT < 0x0400 )
#pragma message ("_WIN32_WINNT is less than 4?!")
#endif

#include "ccLib.h"
#include "ccThread.h"
#include "ccCriticalSection.h"
#include "ccEvent.h"

#include <vector>
#include <deque>
#include <stdexcept>

//////////////////////////////////////////////////////////////////////
// template< class T > class CAPDequeue

template< class T >
class CAPDequeue : public ccLib::CThread  
{
public:
	CAPDequeue();
	virtual ~CAPDequeue();
	virtual void AddItem( const T& item );          // Puts an item at the back of the queue.
    virtual void AddItems( std::vector< T > &vItems );   // Puts multiple items at the back of the queue.
    virtual void AddItemFront ( const T& item);        // Puts an item in the front of the queue.
    virtual long GetSize();                         // Returns # of items in the Q.

protected:
	virtual bool getQueueFront( T& item );          // Gets the first item in the queue.
	virtual void processQueue() = 0;                // Gets called every time an item is added (front or back)

	// Override of CThread
	int Run();

	// Queue of items
    std::deque< T >	        	m_queue;

	// Critical section for queue.
    ccLib::CCriticalSection 	m_csQueue;

	// Event object for item insertion.
    ccLib::CEvent				m_eventQueueInsertion;

	// Debug variables.
#ifdef _DEBUG
	int							m_iMaxQueueSize;
#endif
};

//////////////////////////////////////////////////////////////////////
// CAPDequeue::~CAPDequeue()

template< class T > 
CAPDequeue< T >::CAPDequeue( )
#ifdef _DEBUG
:m_iMaxQueueSize(0)
#endif
{
	// Create event objects
	m_eventQueueInsertion.Create( NULL, FALSE, FALSE, NULL, FALSE );
	if( m_eventQueueInsertion == NULL )
        throw std::runtime_error( "Counld not create event." );
}

//////////////////////////////////////////////////////////////////////
// CAPDequeue::~CAPDequeue()

template< class T > 
CAPDequeue< T >::~CAPDequeue()
{
}

//////////////////////////////////////////////////////////////////////
// CAPDequeue::AddItem()

template< class T > 
void CAPDequeue< T >::AddItem( const T& item )
{
    ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
    m_queue.push_back( item );

#ifdef _DEBUG
	int iSize = m_queue.size();
	m_iMaxQueueSize = iSize > m_iMaxQueueSize ? iSize : m_iMaxQueueSize;
#endif

	m_eventQueueInsertion.SetEvent();
}

//////////////////////////////////////////////////////////////////////
// CAPDequeue::AddItems()

template< class T > 
void CAPDequeue< T >::AddItems( std::vector< T >& vItems )
{
	ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
    std::vector<T>::iterator it;
	for( it = vItems.begin(); it != vItems.end(); ++it )
	{
		m_queue.push_back( *it );
	}

#ifdef _DEBUG
	int iSize = m_queue.size();				
	m_iMaxQueueSize = iSize > m_iMaxQueueSize ? iSize : m_iMaxQueueSize;
#endif

	m_eventQueueInsertion.SetEvent();
}

//////////////////////////////////////////////////////////////////////
// CAPDequeue::PushFront()

template< class T > 
void CAPDequeue< T >::AddItemFront( const T& item )
{
	ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
	m_queue.push_front( item );

#ifdef _DEBUG
	int iSize = m_queue.size();
	m_iMaxQueueSize = iSize > m_iMaxQueueSize ? iSize : m_iMaxQueueSize;
#endif

	m_eventQueueInsertion.SetEvent();
}


//////////////////////////////////////////////////////////////////////
// CAPDequeue::Run()

template< class T > 
int CAPDequeue< T >::Run()
{
	// Set up waiting objects.
	HANDLE aHandles[2] = { m_Terminating.GetHandle(), m_eventQueueInsertion };
    ccLib::CMessageLock msgLock (TRUE, TRUE);

	for(;;)
	{
		// Wait here for something to happen
        DWORD dwWait = msgLock.Lock ( 2, aHandles, FALSE, INFINITE, FALSE );
		switch ( dwWait )
        {
            // Check for error.
            case WAIT_FAILED:
			    return 0xFFFFFFFF;

            case WAIT_OBJECT_0:
			    // exit signaled.
			    return 0;

            case WAIT_OBJECT_0 + 1:
                processQueue();
                break;
        }
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// CAPDequeue::getQueueFront()

template< class T > bool CAPDequeue< T >::getQueueFront( T& item )
{
	ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );
		
	if( m_queue.empty() )
		return false;

	item = m_queue.front();
    m_queue.pop_front();
	return true;
}

//////////////////////////////////////////////////////////////////////
// CAPDequeue::GetSize()

template< class T > long CAPDequeue< T >::GetSize()
{
	ccLib::CSingleLock lock( &m_csQueue, INFINITE, FALSE );		
	return( m_queue.size() );
}
