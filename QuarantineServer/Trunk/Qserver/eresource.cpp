/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SERVER/VCS/syncro.cpv   1.0   18 Dec 1997 22:37:22   DBuches  $
////////////////////////////////////////////////////////////////////////////
//
// Class implementation for CEResource Synchronization class.
//
//
//
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SERVER/VCS/syncro.cpv  $
// 
//    Rev 1.0   18 Dec 1997 22:37:22   DBuches
// Initial revision.
////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "eresource.h"

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEResource::CEResource
//
// Description   : Create the event, initialize the critical section objects and reader count,
//                 and return.                                                                
//
///////////////////////////////////////////////////////////////////////////////
// 1/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CEResource::CEResource()
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

	return;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEResource::~CEResource
//
// Description   : Free the event, delete the critical section objects, and return
//
///////////////////////////////////////////////////////////////////////////////
// 1/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CEResource::~CEResource()
{
	if (IsValid())
	    {
		CloseHandle(hevReadDone);
		DeleteCriticalSection(&csExclusive);
		DeleteCriticalSection(&csReader);
	    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEResource::Lock
//
// Description   : Claim access to this object
//
// Return type   : BOOL 
//
// Argument      : SYNCH_LOCK_TYPE lt
//
///////////////////////////////////////////////////////////////////////////////
// 1/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CEResource::Lock(SYNCH_LOCK_TYPE lt)
{
	// Verify that the object is valid
	if (! IsValid())
		return FALSE;

	// Claim the read lock or write lock as specified
	if (lt == ER_SHARED )
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


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEResource::Unlock
//
// Description   : Release lock on this object
//
// Return type   : BOOL 
//
// Argument      : SYNCH_LOCK_TYPE lt
//
///////////////////////////////////////////////////////////////////////////////
// 1/26/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CEResource::Unlock( SYNCH_LOCK_TYPE lt )
{
	// Verify that the object is valid
	if( !IsValid() )
		return FALSE;

	// Release the read lock or write lock as specified
	if( lt == ER_SHARED )
	    {
		// Claim access to the reader count.  If this blocks, it's only for the
		// briefest moment, while other threads go through to increment or
		// decrement the reader count.
		EnterCriticalSection( &csReader );

		// Decrement the reader count.  If this is the last reader, set
		// <hevReadDone>, which allows the first waiting writer to proceed.
		if( --cReaders == 0 )
			SetEvent( hevReadDone );

		// Release access to the reader count
		LeaveCriticalSection( &csReader );
	    }
	else
    	{
		// Make <csExclusive> available to one other writer or to the first reader
		LeaveCriticalSection(&csExclusive);
	    }

	return TRUE;
}

