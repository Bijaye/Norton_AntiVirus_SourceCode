// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef SYNCHLIST
#define SYNCHLIST

#include <list>
#include <windows.h>

// A synchronized list, designed for use by multiple threads within the same process.
// To use, the process instantiates the list and the threads then use that instance.
// Encapsulates an std::list object with access synchronized by a lock.
template<class Type> class CSynchronizedList
{
public:
	typedef typename std::list<Type> ListType;

	// Standard List API
	// Returns an UNPROTECTED iterator - Use With Caution!
	HRESULT begin( typename ListType::iterator* thisIterator )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
		*thisIterator = thisList.begin();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Returns an UNPROTECTED iterator - Use With Caution!
	HRESULT begin( typename ListType::const_iterator* thisIterator )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
		*thisIterator = thisList.begin();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Returns an UNPROTECTED iterator - Use With Caution!
	typename ListType::iterator begin( void )
	{
		ListType::iterator returnVal;

		// Validate state/parameters
		if (!initialized)
			return returnVal;

		EnterCriticalSection(&listLock);
  		returnVal = thisList.begin();
		LeaveCriticalSection(&listLock);
		return returnVal;
	}

	// Returns an UNPROTECTED iterator - Use With Caution!
	HRESULT end( typename ListType::iterator* thisIterator )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
		*thisIterator = thisList.end();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Returns an UNPROTECTED iterator - Use With Caution!
	HRESULT end( typename ListType::const_iterator* thisIterator )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
		*thisIterator = thisList.end();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Returns an UNPROTECTED iterator - Use With Caution!
	typename ListType::iterator end( void )
	{
		ListType::iterator returnVal;

		// Validate state/parameters
		if (!initialized)
			return returnVal;

		EnterCriticalSection(&listLock);
		returnVal = thisList.end();
		LeaveCriticalSection(&listLock);
		return returnVal;
	}

	HRESULT push_front( const Type& _Val )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
        try
        {
    		thisList.push_front(_Val);
            
		    SetEvent(newItemEvent);
        }
        catch (std::bad_alloc&) {}
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT push_back( const Type& _Val )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
        try
        {
            thisList.push_back(_Val);
            SetEvent(newItemEvent);
        }
        catch (std::bad_alloc&) {}
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT pop_front( void )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
		thisList.pop_front();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT pop_back( void )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
		thisList.pop_back();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT clear( void )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;

		EnterCriticalSection(&listLock);
		thisList.clear();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT empty( bool* isEmpty )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (isEmpty == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
		*isEmpty = thisList.empty();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Returns an UNPROTECTED iterator - Use With Caution!
	HRESULT erase( typename std::list<Type>::iterator _Where, typename std::list<Type>::iterator* resultIterator )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (resultIterator == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
		*resultIterator = thisList.erase(_Where);
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Returns an UNPROTECTED iterator - Use With Caution!
	HRESULT erase( typename std::list<Type>::iterator _First, typename std::list<Type>::iterator _Last, typename std::list<Type>::iterator* resultIterator )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (resultIterator == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
		*resultIterator = thisList.erase(_First, _Last);
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT size( typename std::list<Type>::size_type* listSize ) const
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (listSize == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
		listSize = thisList.size();
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

    typename ListType::size_type size( void ) const
    // As above, for the cases where 0 is equivalent to an error for the caller
    {
		typename ListType::size_type	returnVal		= 0;

		// Validate state/parameters
		if (initialized)
		{
			EnterCriticalSection((LPCRITICAL_SECTION) &listLock);
			returnVal = thisList.size();
			LeaveCriticalSection((LPCRITICAL_SECTION) &listLock);
		}
		return returnVal;
    }

	HRESULT front( typename std::list<Type>::pointer listFront )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (listFront == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
        try
        {
    		*listFront = thisList.front();
        }
        catch (std::bad_alloc&) {}
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT front( typename std::list<Type>::const_pointer listFront ) const
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (listFront == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
        try
        {
    		*listFront = thisList.front();
        }
        catch (std::bad_alloc&) {}
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT back( typename std::list<Type>::pointer listBack )
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (listBack == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
        try
        {
    		*listBack = thisList.back();
        }
        catch (std::bad_alloc&) {}
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	HRESULT back( typename std::list<Type>::const_pointer listBack ) const
	{
		// Validate state/parameters
		if (!initialized)
			return E_FAIL;
		if (listBack == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
        try
        {
    		*listBack = thisList.back();
        }
        catch (std::bad_alloc&) {}
		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Special synchronization functions
	// Waits for an item to be added to the list, StopThreads to be called,
    // or a user-specified event to be set, then returns.
	// newItem, gotItem, and secondaryEventSignaled are out parameters, set to 
    // the new item on the list, and whether or not an item was retrieved and
    // whether a return was due to the secondary event being signlaed, respectively.
	// All return states are summarized below.
	//  STATE            RETURN VALUE		GOTITEM     SECONDARYEVENTSIGNALED
	// =======================================================================
	//	0. New item				S_OK		TRUE        FALSE
    //  1. secondaryEvent set   S_OK        FALSE       TRUE
    //  2. StopThreads called   S_OK        FALSE       FALSE
    //  3. Timeout waiting      S_FALSE     FALSE       FALSE
	//	4. Error				<error>		FALSE       FALSE
	HRESULT WaitForItem( Type* newItem, bool* gotItem, HANDLE secondaryEvent = NULL, bool* secondaryEventSignaled = NULL, DWORD timeout = INFINITE )
	{
		HANDLE	eventHandles[3]		= {NULL, NULL};
		DWORD	noEventHandles		= 2;
		DWORD	returnValDW			= WAIT_TIMEOUT;
		HRESULT	returnValHR			= E_FAIL;

		// Validate state
		if (!initialized)
			return E_FAIL;
		if (newItem == NULL)
			return E_POINTER;
        if (gotItem == NULL)
            return E_POINTER;

        // Base case - check for data already in the list
        EnterCriticalSection(&listLock);
        if (thisList.size() > 0)
        {
            returnValHR = S_OK;
            try
            {
                *newItem = thisList.front();
                thisList.pop_front();
                *gotItem = true;
            }
            catch (std::bad_alloc&)
            {
                *gotItem = false;
                returnValHR = E_OUTOFMEMORY;
            }
            LeaveCriticalSection(&listLock);
            return returnValHR;
        }
        else
        {
            LeaveCriticalSection(&listLock);
        }

		// Initialize
		eventHandles[0] = newItemEvent;
		eventHandles[1] = shutdownEvent;
		if (secondaryEvent != NULL)
		{
			eventHandles[2] = secondaryEvent;
			noEventHandles = 3;
		}

		// Wait for an item to be added or a problem to occur.  Loop in case event is set but all items have already been consumed.
		while(true)
		{
			returnValDW = WaitForMultipleObjects(noEventHandles, eventHandles, FALSE, timeout);
			if (secondaryEventSignaled != NULL)
				*secondaryEventSignaled = false;
			if (returnValDW == WAIT_OBJECT_0)
			{
				// New item
				returnValHR = S_OK;
				EnterCriticalSection(&listLock);
				if (thisList.size() > 0)
				{
                    try
                    {
    					*newItem = thisList.front();
    					thisList.pop_front();
                        *gotItem = true;
                    }
                    catch (std::bad_alloc&)
                    {
                        *gotItem = false;
                        returnValHR = E_OUTOFMEMORY;
                    }
					LeaveCriticalSection(&listLock);
					break;
				}
				else
				{
					LeaveCriticalSection(&listLock);
				}
			}
			else if (returnValDW == WAIT_TIMEOUT)
			{
				// Timeout
				returnValHR = S_FALSE;
				*gotItem = false;
				break;
			}
			else if (returnValDW == (WAIT_OBJECT_0+1))
			{
				// Stop requested
				returnValHR = S_OK;
				*gotItem = false;
				break;
			}
			else if (returnValDW == (WAIT_OBJECT_0+2))
			{
				// Secondary event signaled
				returnValHR = S_OK;
				*gotItem = false;
				if (secondaryEventSignaled != NULL)
					*secondaryEventSignaled = true;
				break;
			}
			else
			{
				// Error
				if (returnValDW == WAIT_FAILED)
					returnValHR = HRESULT_FROM_WIN32(returnValDW);
				else
					returnValHR = E_FAIL;
				*gotItem = false;
				break;
			}
		}

		return returnValHR;
	}

	// Signal all threads waiting for items to exit immediately
	HRESULT StopThreads( void )
	{
		// Validate state
		if (!initialized)
			return E_FAIL;

		if (SetEvent(shutdownEvent))
			return S_OK;
		else
			return HRESULT_FROM_WIN32(GetLastError());
	}

	// Resets the stop immediately event.  Call this only if you have called StopThreads
	// and are ready to re-use the CSynchronizedList before spawning threads that call
	// WaitForNewItem
	HRESULT Reset( void )
	{
		// Validate state
		if (!initialized)
			return E_FAIL;

		if (ResetEvent(shutdownEvent))
			return S_OK;
		else
			return HRESULT_FROM_WIN32(GetLastError());
	}

	// Lock the list and allow the caller to manipulate it directly.
	HRESULT Lock( std::list<Type>** listCore )
	{
		// Validate state
		if (!initialized)
			return E_FAIL;
		if (listCore == NULL)
			return E_POINTER;

		EnterCriticalSection(&listLock);
		if (listCore != NULL)
			*listCore = &thisList;
		return S_OK;
	}

	// Release the list lock.
	HRESULT Unlock( void )
	{
		// Validate state
		if (!initialized)
			return E_FAIL;

		LeaveCriticalSection(&listLock);
		return S_OK;
	}

	// Constructor-destructor
	// Constructs the list.  No other threads may call into this object until this call has completed.
	CSynchronizedList() : initialized(false), newItemEvent(NULL), shutdownEvent(NULL)
	{
		InitializeCriticalSection(&listLock);
		newItemEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (newItemEvent != NULL)
			shutdownEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if ((newItemEvent != NULL) && (shutdownEvent != NULL))
		{
			thisList.clear();
			initialized = true;
		}
		else
		{
			Destroy();
		}
	}

	// Destroys the list.  ALL OTHER THREADS MUST BE SHUTDOWN BEFORE CALLING THIS MEMBER.
	virtual ~CSynchronizedList()
	{
		Destroy();
	}
private:
	// FUNCTION MEMBERS
	// Releases all system resources.
	HRESULT Destroy( void )
	{
		HRESULT returnVal = S_OK;

		initialized = false;
		// Just in case any thread is waiting on us, give them a chance to exit.
		// No other threads should be waiting or using this object when Destroy is called.
		if (shutdownEvent != NULL)
		{
			SetEvent(shutdownEvent);
			Sleep(0);
		}
		if (newItemEvent != NULL)
		{
			if (!CloseHandle(newItemEvent))
				returnVal = HRESULT_FROM_WIN32(GetLastError());
			newItemEvent = NULL;
		}
		if (shutdownEvent != NULL)
		{
			// Cleanup this event handle
			if (!CloseHandle(shutdownEvent))
				returnVal = HRESULT_FROM_WIN32(GetLastError());
			shutdownEvent = NULL;
		}
		DeleteCriticalSection(&listLock);

		return returnVal;
	}

	// DATA MEMBERS
	std::list<Type>		thisList;
	HANDLE				newItemEvent;
	HANDLE				shutdownEvent;
	CRITICAL_SECTION	listLock;
	bool				initialized;
};

#endif // SYNCHLIST
