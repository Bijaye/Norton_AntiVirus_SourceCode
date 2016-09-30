////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUARSVC/VCS/eresource.cpv   1.0   21 Apr 1998 19:24:56   SEDWARD  $
////////////////////////////////////////////////////////////////////////////
//
// Class implementation for CEResource Synchronization class.
//
//
//
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUARSVC/VCS/eresource.cpv  $
// 
//    Rev 1.0   21 Apr 1998 19:24:56   SEDWARD
// Initial revision.
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Includes

#include <assert.h>
#include "eresource.h"


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CEResource::CEResource
// Description	    : Create the event, initialize the critical section
//                    objects and reader count.
// Argument         : void
//
////////////////////////////////////////////////////////////////////////////////
// 2/7/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CEResource::CEResource(void)
{
	// Create the manual-reset event (the only init that can fail)
	hevReadDone = CreateEvent(NULL, TRUE, TRUE, NULL);

	fInitSucceeded = hevReadDone != NULL;

	// If we created the event, proceed with the risk-free initialization
	if (fInitSucceeded)
	    {
		cReaders = 0;
		InitializeCriticalSection(&csExclusive);
		InitializeCriticalSection(&csReader);
	    }
    else
        {
        assert(FALSE);
        }
}

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CEResource::~CEResource
// Description	    : Free the event, delete the critical section objects,
// Return type		:
// Argument         : void
//
////////////////////////////////////////////////////////////////////////////////
// 2/7/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
CEResource::~CEResource(void)
{
	if (IsValid())
	    {
		CloseHandle(hevReadDone);
		DeleteCriticalSection(&csExclusive);
		DeleteCriticalSection(&csReader);
	    }
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CEResource::Lock
// Description	    : Lock member function
// Return type		: BOOL - TRUE on success
// Argument         : SYNCH_LOCK_TYPE lt - type of lock requested
//                    either SLT_SHARED or SLT_EXCLUSIVE
////////////////////////////////////////////////////////////////////////////////
// 2/7/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CEResource::Lock(SYNCH_LOCK_TYPE lt)
{
	// Verify that the object is valid
	if (! IsValid())
		return FALSE;

	// Claim the read lock or write lock as specified
	if (lt == SLT_SHARED)
	    {
		// Claim the <csExclusive> critical section.  This call blocks if there's
		// an active writer or if there's a writer waiting for active readers to
		// complete.
		EnterCriticalSection(&csExclusive);

		// Claim access to the reader count.  If this blocks, it's only for the
		// briefest moment, while other threads go through to increment or
		// decrement the reader count.
		EnterCriticalSection(&csReader);

		// Increment the reader count.  If this is the first reader, we reset the
		// hevReadDone event so that the next writer blocks.
		if (cReaders++ == 0)
			ResetEvent(hevReadDone);

		// Release access to the reader count
		LeaveCriticalSection(&csReader);

		// Release access to the <csExclusive> critical section.  This enables
		// other readers to come through and the next writer to wait for active
		// readers to complete (which in turn prevents new readers from entering).
		LeaveCriticalSection(&csExclusive);
	    }
	else
	    {
		// Claim the <csExclusive> critical section.  This not only prevents other
		// threads from claiming the write lock, but also prevents any new threads
		// from claiming the read lock.
		EnterCriticalSection(&csExclusive);

		// Wait for the active readers to release their read locks.
		return WaitForSingleObject(hevReadDone, INFINITE) == WAIT_OBJECT_0;
	    }

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CEResource::Unlock
// Description	    : Unlock member function
// Return type		: BOOL - TRUE on success
// Argument         : SYNCH_LOCK_TYPE lt - type of lock requested
//
////////////////////////////////////////////////////////////////////////////////
// 2/7/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CEResource::Unlock(SYNCH_LOCK_TYPE lt)
{
	// Verify that the object is valid
	if (! IsValid())
		return FALSE;

	// Release the read lock or write lock as specified
	if (lt == SLT_SHARED)
	    {
		// Claim access to the reader count.  If this blocks, it's only for the
		// briefest moment, while other threads go through to increment or
		// decrement the reader count.
		EnterCriticalSection(&csReader);

		// Decrement the reader count.  If this is the last reader, set
		// <hevReadDone>, which allows the first waiting writer to proceed.
		if (--cReaders == 0)
			SetEvent(hevReadDone);

		// Release access to the reader count
		LeaveCriticalSection(&csReader);
	    }
	else
	    {
		// Make <csExclusive> available to one other writer or to the first reader
		LeaveCriticalSection(&csExclusive);
	    }

	return TRUE;
}
