////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SERVER/VCS/syncro.h_v   1.0   18 Dec 1997 22:37:24   DBuches  $
////////////////////////////////////////////////////////////////////////////
//
//
//    CEResource - C++ class to provide Synchronization object encapsulation
//
//    Inspiration from MSJ article circa Feb 1995
//
//
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SERVER/VCS/syncro.h_v  $
// 
//    Rev 1.0   18 Dec 1997 22:37:24   DBuches
// Initial revision.
////////////////////////////////////////////////////////////////////////////

#ifndef _ERESOURCE_H_
#define _ERESOURCE_H_

typedef enum { ER_SHARED, ER_EXCLUSIVE } SYNCH_LOCK_TYPE;

class CEResource
{
private:
    CRITICAL_SECTION csExclusive;   // Critical section object to CSyncnize writers
    CRITICAL_SECTION csReader;      // Critical section object to CSyncnize readers
	HANDLE hevReadDone;			    // Manual-reset event to notify writers of reader completion
	int cReaders;					// Count of current readers

	BOOL fInitSucceeded;			// TRUE if the constructor function succeeded

public:
    CEResource();                   // Constructor
    ~CEResource();                  // Destructor

	// This object is valid if it was initialized
	BOOL IsValid(void) { return fInitSucceeded; }

	BOOL Lock(SYNCH_LOCK_TYPE);	    // Lock the object
	BOOL Unlock(SYNCH_LOCK_TYPE);	// Unlock the object
};


// Utility class used to ensure that shared access
// to guarded resources is released.
class CEnsureSharedRelease
{
private:
    CEResource *m_p;

public:
    CEnsureSharedRelease( CEResource *p) { m_p = p; };
    ~CEnsureSharedRelease() { if( m_p ) m_p->Unlock( ER_SHARED ); };
};


// Utility class used to ensure that exclusive access
// to guarded resources is released.
class CEnsureExclusiveRelease
{
private:
    CEResource *m_p;

public:
    CEnsureExclusiveRelease( CEResource *p){ m_p = p; };
    ~CEnsureExclusiveRelease() { if( m_p ) m_p->Unlock( ER_EXCLUSIVE ); };
};




#endif 
