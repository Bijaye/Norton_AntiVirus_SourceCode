////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUARSVC/VCS/eresource.h_v   1.0   21 Apr 1998 19:24:58   SEDWARD  $
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
// $Log:   S:/QUARSVC/VCS/eresource.h_v  $
// 
//    Rev 1.0   21 Apr 1998 19:24:58   SEDWARD
// Initial revision.
////////////////////////////////////////////////////////////////////////////

#ifndef _ERESOURCE_H_
#define _ERESOURCE_H_

////////////////////////////////////////////////////////////////////////////
// Macros

#ifdef BUILDING_E_RESOURCE
#define DECLSPEC_ERESOURCE __declspec( dllexport )
#else
#define DECLSPEC_ERESOURCE __declspec( dllimport )
#endif


#include <windows.h>


////////////////////////////////////////////////////////////////////////////
// Lock types
typedef enum {SLT_SHARED, SLT_EXCLUSIVE} SYNCH_LOCK_TYPE;

////////////////////////////////////////////////////////////////////////////////
//
// Class name	    : CEResource
//
// Descritpion      : User mode implementation of the Windows NT Kernel mode
//                    ERESOURCE synchronization object.
//
//
////////////////////////////////////////////////////////////////////////////////
// 2/7/98 - DBUCHES created / header added
////////////////////////////////////////////////////////////////////////////////
class DECLSPEC_ERESOURCE CEResource
{
private:
    CRITICAL_SECTION csExclusive;   // Critical section object to CSyncnize writers
    CRITICAL_SECTION csReader;      // Critical section object to CSyncnize readers
	HANDLE hevReadDone;			    // Manual-reset event to notify writers of reader completion
	int cReaders;					// Count of current readers

	BOOL fInitSucceeded;			// TRUE if the constructor function succeeded

public:
    CEResource(void);               // Constructor
    ~CEResource(void);              // Destructor

	                                // This object is valid if it was initialized
	BOOL IsValid(void) { return fInitSucceeded; }

	BOOL Lock(SYNCH_LOCK_TYPE);	    // Lock the object
	BOOL Unlock(SYNCH_LOCK_TYPE);	// Unlock the object
};

////////////////////////////////////////////////////////////////////////////////
// Utility class used to ensure that shared access
// to guarded resources is released.
////////////////////////////////////////////////////////////////////////////////
class DECLSPEC_ERESOURCE CEnsureSharedRelease
{
private:
    CEResource *m_p;

public:
    CEnsureSharedRelease( CEResource *p) { m_p = p; };
    ~CEnsureSharedRelease() { if( m_p ) m_p->Unlock( SLT_SHARED ); };
};

////////////////////////////////////////////////////////////////////////////////
// Utility class used to ensure that exclusive access
// to guarded resources is released.
////////////////////////////////////////////////////////////////////////////////
class DECLSPEC_ERESOURCE CEnsureExclusiveRelease
{
private:
    CEResource *m_p;

public:
    CEnsureExclusiveRelease( CEResource *p){ m_p = p; };
    ~CEnsureExclusiveRelease() { if( m_p ) m_p->Unlock( SLT_EXCLUSIVE ); };
};

#endif _ERESOURCE_H_