/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync.cpp
// Copyright 1999-2000 by Symantec Corporation.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Headers

#include "stdafx.h"
#include "readwritesync.h"


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync - Unix Implementation
//
// Based on a web article on threads: http://www.ucs.tche.br/so/ch6.htm
// and on a messier example in the comp.programming.threads FAQ.
/////////////////////////////////////////////////////////////////////////////

#if defined(UNIX)


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteSync::CReadWriteSync()
//
// Constructs a readers-writer synchronization object.

CReadWriteSync::CReadWriteSync()
{ 
    m_nReadersActive = 0; 
    m_nWritersActive = 0;
    m_nReadersWaiting = 0; 
    m_nWritersWaiting = 0; 
    pthread_cond_init(&m_condReadOK, NULL); 
    pthread_cond_init(&m_condWriteOK, NULL); 
    pthread_mutex_init(&m_mutex, NULL); 
} 


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteSync::CReadWriteSync()
//
// Destructs the readers-writer synchronization object.

CReadWriteSync::~CReadWriteSync()
{
    pthread_cond_destroy(&m_condReadOK);
    pthread_cond_destroy(&m_condWriteOK);
    pthread_mutex_destroy(&m_mutex);
}


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteSync::ReaderLock()
//
// Request read access to the resource.
//
// This call will block as long as any writer has access or is waiting for
// access to the resource.

void CReadWriteSync::ReaderLock() 
{ 
    pthread_mutex_lock(&m_mutex); 

    // Wait if the resource is held by a writer or if any writers are waiting.
    while (m_nWritersActive > 0 || m_nWritersWaiting > 0) 
    { 
        ++m_nReadersWaiting; 
        pthread_cond_wait(&m_condReadOK, &m_mutex); 
        --m_nReadersWaiting; 
    } 

    ++m_nReadersActive; 

    pthread_mutex_unlock(&m_mutex); 
} 


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteSync::ReaderUnlock()
//
// Relinquish read access to the resource.

void CReadWriteSync::ReaderUnlock()
{ 
    pthread_mutex_lock(&m_mutex); 

    --m_nReadersActive; 

    bool bReleaseWriter = (m_nReadersActive == 0 && m_nWritersWaiting > 0);

    pthread_mutex_unlock(&m_mutex); 

    // The last reader releases a single waiting writer.
    if (bReleaseWriter) 
        pthread_cond_signal(&m_condWriteOK); 
} 


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteSync::WriterLock()
//
// Request write access to the resource.
//
// This call will block as long as there are any readers or writers accessing
// the resource.

void CReadWriteSync::WriterLock()
{ 
    pthread_mutex_lock(&m_mutex); 

    // Wait until the resource is unheld (i.e. no readers or writers).
    while (m_nReadersActive > 0 || m_nWritersActive > 0)
    { 
        ++m_nWritersWaiting; 
        pthread_cond_wait(&m_condWriteOK, &m_mutex); 
        --m_nWritersWaiting; 
    } 

    ++m_nWritersActive; 

    pthread_mutex_unlock(&m_mutex); 
} 


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteSync::WriterUnlock()
//
// Relinquish write access to the resource.

void CReadWriteSync::WriterUnlock()
{ 
    pthread_mutex_lock(&m_mutex); 

    --m_nWritersActive; 

    bool bReleaseWriter = (m_nWritersWaiting > 0);
    bool bReleaseReaders = (m_nWritersWaiting == 0 && m_nReadersWaiting > 0);

    pthread_mutex_unlock(&m_mutex); 

    // The last writer releases one waiting writer, or all waiting readers.
    if (bReleaseWriter) 
        pthread_cond_signal(&m_condWriteOK);
    else if (bReleaseReaders) 
        pthread_cond_broadcast(&m_condReadOK);
} 


#endif  // UNIX


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync - Win32 Implementation
//
// Based on "Efficient Synchronization Techniques" in Microsoft System Journal,
// Feb 1995, and on the CEResource implemenation in Quarantine Server.
/////////////////////////////////////////////////////////////////////////////

#if defined(WIN32)


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync::CReadWriteSync()

CReadWriteSync::CReadWriteSync()
{
	// Create the manual-reset event.
    m_heventReadDone = CreateEvent(NULL, TRUE, TRUE, NULL);
    if (m_heventReadDone == NULL)
        throw std::runtime_error("error creating read-write sync");

    m_nReadersActive = 0;
    InitializeCriticalSection(&m_critsecExclusive);
	InitializeCriticalSection(&m_critsecReader);
}


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync::~CReadWriteSync()

CReadWriteSync::~CReadWriteSync()
{
    CloseHandle(m_heventReadDone);
    DeleteCriticalSection(&m_critsecExclusive);
    DeleteCriticalSection(&m_critsecReader);
}


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync::ReaderLock()

void CReadWriteSync::ReaderLock()
{
	// Claim the Exclusive critical section.  This call blocks if there's
	// an active writer or if there's a writer waiting for active readers to
	// complete.
	EnterCriticalSection(&m_critsecExclusive);

	// Claim access to the reader count.  If this blocks, it's only for the
	// briefest moment, while other threads go through to increment or
	// decrement the reader count.
	EnterCriticalSection(&m_critsecReader);

	// Increment the reader count.  If this is the first reader, we reset the
	// ReadDone event so that the next writer blocks.
	if (m_nReadersActive++ == 0)
		ResetEvent(m_heventReadDone);

	// Release access to the reader count.
	LeaveCriticalSection(&m_critsecReader);

	// Release access to the Exclusive critical section.  This enables
	// other readers to come through and the next writer to wait for active
	// readers to complete (which in turn prevents new readers from entering).
	LeaveCriticalSection(&m_critsecExclusive);
}


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync::WriterLock()

void CReadWriteSync::WriterLock()
{
    // Claim the Exclusive critical section.  This not only prevents other
    // threads from claiming the write lock, but also prevents any new threads
    // from claiming the read lock.
    EnterCriticalSection(&m_critsecExclusive);

    // Wait for the active readers to release their read locks.
    ccLib::CMessageLock msgLock (TRUE, FALSE);
    msgLock.Lock ( 1, &m_heventReadDone, FALSE, INFINITE, FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync::ReaderUnlock()

void CReadWriteSync::ReaderUnlock()
{
	// Claim access to the reader count.  If this blocks, it's only for the
	// briefest moment, while other threads go through to increment or
	// decrement the reader count.
	EnterCriticalSection(&m_critsecReader);

	// Decrement the reader count.  If this is the last reader, set
	// ReadDone, which allows the first waiting writer to proceed.
	if (--m_nReadersActive == 0)
		SetEvent(m_heventReadDone);

	// Release access to the reader count.
	LeaveCriticalSection(&m_critsecReader);
}


/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync::WriterUnlock()

void CReadWriteSync::WriterUnlock()
{
	// Make the Exclusive critical section available to one other writer 
    // or to the first reader.
	LeaveCriticalSection(&m_critsecExclusive);
}


#endif  // WIN32


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteLock::CReadWriteLock()
//
// Constructs a lock-tracking object for a CReadWriteSync.
//
// @param sync  The CReadWriteSync to be tracked.

CReadWriteLock::CReadWriteLock(CReadWriteSync& sync) :
    m_sync(sync),
    m_bLocked(false),
    m_bWriter(false)
{
    // intentionally empty
}


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteLock::~CReadWriteLock()
//
// Destructs the lock-tracking object.
//
// Any lock claimed through the lock-tracking object is released.

CReadWriteLock::~CReadWriteLock()
{
    Unlock();
}


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteLock::Lock()
//
// Locks the associated CReadWriteSync object.
//
// @param bWriter  Locking mode; true for write access, false for read access.

void CReadWriteLock::Lock(bool bWriter)
{
    m_bWriter = bWriter;
    if (m_bWriter)
        m_sync.WriterLock();
    else
        m_sync.ReaderLock();
    m_bLocked = true;
}


/////////////////////////////////////////////////////////////////////////////
// @function CReadWriteLock::Unlock()
//
// Unlocks the associated CReadWriteSync object.

void CReadWriteLock::Unlock()
{
    if (m_bLocked) 
    {
        if (m_bWriter)
            m_sync.WriterUnlock();
        else
            m_sync.ReaderUnlock();
        m_bLocked = false;
    }
}

