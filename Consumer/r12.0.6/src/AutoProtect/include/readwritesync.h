/////////////////////////////////////////////////////////////////////////////
// CReadWriteSync.h
// Copyright 1999-2000 by Symantec Corporation.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#if !defined(CREADWRITESYNC_H)
#define CREADWRITESYNC_H


/////////////////////////////////////////////////////////////////////////////
// @class CReadWriteSync
// 
// A readers-writer synchronization object.
//
// A CReadWriteSync is a synchronization object that allows either multiple
// readers or one writer to access a resource.  Threads needing read access
// must call ReaderLock() before using the resource, and ReaderUnlock() when
// they are done using it.  Similarly, threads needing write access must call
// WriterLock() and WriterUnlock().

class CReadWriteSync
{ 
public:

    CReadWriteSync();
    ~CReadWriteSync();
    void ReaderLock();
    void ReaderUnlock();
    void WriterLock();
    void WriterUnlock();

private:

#if defined(WIN32)

    CRITICAL_SECTION m_critsecExclusive;    // Critical section object to synchronize writers
    CRITICAL_SECTION m_critsecReader;       // Critical section object to synchronize readers
	HANDLE m_heventReadDone;                // Manual-reset event to notify writers of reader completion
	int m_nReadersActive;					// Number of active readers

#elif defined(UNIX)

    int m_nReadersActive;
    int m_nWritersActive;
    int m_nReadersWaiting;
    int m_nWritersWaiting;
    pthread_cond_t m_condReadOK;
    pthread_cond_t m_condWriteOK;
    pthread_mutex_t m_mutex;

#endif
};


/////////////////////////////////////////////////////////////////////////////
// @class CReadWriteLock
//
// A lock on a CReadWriteSync object.  
//
// A CReadWriteLock is associated with a CReadWriteSync object.  The Sync
// object may be locked and unlocked through the Lock object, and more 
// importantly, the Lock object's destructor will automatically unlock
// the Sync object if necessary.  Using this class makes it easy to ensure 
// that CReadWriteSyncs are unlocked when exceptions are thrown.

class CReadWriteLock
{
public:
    CReadWriteLock(CReadWriteSync& sync);
    ~CReadWriteLock();
    void Lock(bool bWriter);
    void Unlock();

private:

    CReadWriteLock(const CReadWriteLock&);  // disallowed
    CReadWriteLock& operator=(const CReadWriteLock&);  // disallowed

    CReadWriteSync& m_sync;
    bool m_bLocked;
    bool m_bWriter;
};


#endif // CREADWRITESYNC_H
