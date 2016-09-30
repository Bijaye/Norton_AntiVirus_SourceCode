// Copyright 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/THDATA.CPv   1.1   02 May 1997 22:26:10   RStanev  $
//
// Description:
//      This file contains SYM_VXD and SYM_NTK per-thread data functions.
//
// Contains:
//      LocalLock()                 (inline function)
//      LocalUnLock()               (inline function)
//      CoreInitThreadData()
//      CoreDeInitThreadData()
//      CoreAllocThreadData()
//      CoreFreeThreadData()
//      CoreLockThreadDataPtr()
//      CoreUnLockThreadDataPtr()
//      _CoreLockThreadDataPtr()    (debug and prerelease only)
//      _CoreUnLockThreadDataPtr()  (debug and prerelease only)
//
// Note:
//      Core supports two types of per-thread data management.  The first
//      type is based on the per-thread data management functions of
//      SYMEvent.  This is the preferred type, but it is currently not
//      implemented.  The second type of per-thread data management is
//      local to Core - it is a bit slower than the first type, but it does
//      not depend on SYMEvent.
//
//      The per-thread data management supports stack switching under the
//      SYM_NTK and SYM_VXD platform.  Under the SYM_NTK platform, it is
//      necessary to switch to a kernel stack prior to making calls into
//      OS provided services.  This includes the memory allocation routines
//      MemAllocPtr/MemFreePtr which use this stack switching support.
//      Hence, these support routines must have a mechanism to bypass the
//      stack switching support during memory allocation -- this support
//      is provided by using the MemAllocPtrX and MemFreePtrX routines.
//
// See also:
//      drvmisc.cpp
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/THDATA.CPv  $
//
//    Rev 1.1   02 May 1997 22:26:10   RStanev
// Added CoreFreeThreadData().
//
//    Rev 1.0   25 Feb 1997 01:04:36   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "symsync.h"

//---------------------------------------------------------------------------
//
// Define structures and variables for the per-thread data "engine".
//
// The base of the per-thread data engine is an array of THREADDATAHANDLE
// structures.  The beginning of the array is pointed to by prTDHandles.
// When no per-thread data is allocated, prTDHandles will point to NULL.
// When per-thread data is allocated, the "engine" extends the array pointed
// to by prTDHandles.  The per-thread data handles returned by the per-thread
// APIs are indexes in the prTDHandles array.  The per-thread data block
// at prTDHandles[0] uses per-thread data handle with value 1, prTDHandles[1]
// uses per-thread data handle with value 2, prTDHandles[2] uses per-thread
// data handle with value 3, etc.  Each per-thread data handle (structure in
// the THREADDATAHANDLE array) contains a pointer to a linked list of locked
// per-thread data areas.  Each thread on the system will have no more than
// one entry into each list pointed to by a handle.
//
//---------------------------------------------------------------------------
typedef struct tagTHREADDATAENTRY
    {
    tagTHREADDATAENTRY *prNext;
    tagTHREADDATAENTRY *prPrev;
    DWORD              dwThreadID;
    int                iLockCount;
    BYTE               abyData[1];
    } THREADDATAENTRY, *PTHREADDATAENTRY;

typedef struct tagTHREADDATAHANDLE
    {
    PTHREADDATAENTRY prEntries;
    UINT             uEntrySize;
    } THREADDATAHANDLE, *PTHREADDATAHANDLE;

static PTHREADDATAHANDLE prTDHandles = NULL;
static int               iTDHandles = 0;

//---------------------------------------------------------------------------
//
// Define LocalLock(), LocalUnLock(), and supporting variables for
// synchronizing access to the per-thread data engine structures.  Note,
// that we will use OS-specific synchronization objects to avoid the extra
// overhead of using SYMMUTEX.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static FAST_MUTEX LocalLockMutex = {0};

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

inline VOID LocalLock ( VOID )
    {
    ExAcquireFastMutex ( &LocalLockMutex );
    }

inline VOID LocalUnLock ( VOID )
    {
    ExReleaseFastMutex ( &LocalLockMutex );
    }

#elif defined(SYM_VXD)

static PVMMMUTEX LocalLockMutex = {0};

inline VOID LocalLock ( VOID )
    {
    SYM_ASSERT ( LocalLockMutex );

    _EnterMutex ( LocalLockMutex, BLOCK_THREAD_IDLE );
    }

inline VOID LocalUnLock ( VOID )
    {
    SYM_ASSERT ( LocalLockMutex );

    _LeaveMutex ( LocalLockMutex );
    }

#endif

//---------------------------------------------------------------------------
//
// BOOL CoreInitThreadData (
//      VOID
//      );
//
// This function initializes Core's per-thread data APIs.  Clients of these
// APIs should not call this function directly.  Instead, they should invoke
// CoreGlobalInit().
//
// Entry:
//      -
//
// Exit:
//      TRUE if successful, FALSE if error.
//
//---------------------------------------------------------------------------
BOOL CoreInitThreadData (
    VOID
    )
    {
                                        // Initialize local lock/unlock
                                        // resources.

#if defined(SYM_NTK)

    ExInitializeFastMutex ( &LocalLockMutex );

#elif defined(SYM_VXD)

    if ( ! ( LocalLockMutex = _CreateMutex ( 0, 0 ) ) )
        {
        SYM_ASSERT ( LocalLockMutex );
        return ( FALSE );
        }

#endif

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL CoreDeInitThreadData (
//      VOID
//      );
//
// This function uninitializes Core's per-thread data APIs.  Clients of these
// APIs should not call this function directly.  Instead, they should invoke
// CoreGlobalDeInit().
//
// Entry:
//      -
//
// Exit:
//      TRUE if successful, FALSE if error.
//
//---------------------------------------------------------------------------
BOOL CoreDeInitThreadData (
    VOID
    )
    {
                                        // Free the per-thread data list.
                                        // There shouldn't be any per-thread
                                        // data locks active at the moment!

#if defined(SYM_DEBUG) || defined(PRERELEASE)

    auto int iCounter;

    SYM_ASSERT ( !iTDHandles || prTDHandles );

    for ( iCounter = iTDHandles; iCounter; iCounter-- )
        {
        SYM_ASSERT ( !prTDHandles[iCounter - 1].prEntries );
        }

#endif

    if ( prTDHandles )
        {
        MemFreePtrX ( prTDHandles );
        }

                                        // Destroy local lock/unlock
                                        // resources.

#if defined(SYM_NTK)

    memset ( &LocalLockMutex, 0, sizeof(LocalLockMutex) );

#elif defined(SYM_VXD)

    SYM_ASSERT ( !_GetMutexOwner ( LocalLockMutex ) );

    if ( !_DestroyMutex ( LocalLockMutex ) )
        {
        SYM_ASSERT ( FALSE );
        }

#endif

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// HOFDATA CoreAllocThreadData (
//      UINT uSize
//      );
//
// This function allocates per-thread data area.
//
// Entry:
//      uSize - size of data area to allocate
//
// Exit:
//      Returns handle to allocated data area.  The handle will be non-zero
//      value if successful, or 0 if error.  The data area for each thread
//      cannot be used until it is locked.
//
//---------------------------------------------------------------------------
HOFDATA CoreAllocThreadData (
    UINT uSize
    )
    {
    auto int               iNewHandle;
    auto PTHREADDATAHANDLE prNewTDHandles;

                                        // Validate input parameters.

    SYM_ASSERT ( uSize );

                                        // Acquire lock on the list.

    LocalLock();

                                        // Assign a new handle.

    iNewHandle = iTDHandles + 1;

                                        // Allocate larger array for list of
                                        // handles.

    prNewTDHandles = (PTHREADDATAHANDLE) MemAllocPtrX (
                         GHND,
                         iNewHandle * sizeof(THREADDATAHANDLE)
                         );

    if ( prNewTDHandles )
        {
        if ( prTDHandles )
            {
                                        // Copy the old handle data to the
                                        // new block with handle data.

            memcpy ( prNewTDHandles,
                     prTDHandles,
                     iTDHandles * sizeof(THREADDATAHANDLE) );

                                        // Release the old list.

            MemFreePtrX ( prTDHandles );
            }

                                        // Make the new list current.

        prTDHandles = prNewTDHandles;

                                        // Initialize the length of the entry
                                        // of the new handle.  Include the
                                        // length of the whole structure, so
                                        // that we don't have to calculate it
                                        // during lock.

        prTDHandles[iTDHandles].uEntrySize = uSize + sizeof(THREADDATAENTRY);

                                        // Update the counter of handles.

        iTDHandles = iNewHandle;

                                        // Unlock the list.

        LocalUnLock();

                                        // Return the new handle.

        return ( (HOFDATA) iNewHandle );
        }

    LocalUnLock();

    return ( (HOFDATA) 0 );
    }

//---------------------------------------------------------------------------
//
// BOOL CoreFreeThreadData (
//      HOFDATA hHandle
//      );
//
// This function frees per-thread data area.  No locks on the data specified
// by hHandle should be active when this function is called!  If there are
// active locks, memory leakage is guaranteed, and the system may become
// unstable.
//
// Entry:
//      hHandle - a valid per-thread data handle returned by
//                CoreAllocThreadData().
//
// Exit:
//      Returns TRUE if handle released, or FALSE if hHandle is permanently
//      allocated.  In case the handle is permanently allocated, the client
//      should store its value in a persistent data area, and reuse it later.
//      Permanently allocated per-thread data handles will be allocated
//      when SymEvent is used for managing the per-thread data.  SymEvent
//      provides APIs for managing persistent data.
//
// Note:
//      Currently, this function may release hHandle and return TRUE, but
//      hHandle will never be reused!  The memory allocated for the handle
//      will be freed up only by CoreDeInitThreadData().  This means that the
//      clients should be very careful with allocating and freeing handles -
//      they should allocate per-thread data after initializing Core, and
//      free it only before deinitializing Core.
//
//---------------------------------------------------------------------------
BOOL CoreFreeThreadData (
    HOFDATA hHandle
    )
    {
    auto int iIndex;

                                        // Validate input parameters.

    SYM_ASSERT ( int(hHandle) > 0 );

                                        // Convert the handle to direct
                                        // index in the list.

    iIndex = int(hHandle) - 1;

                                        // Acquire lock on the list.

    LocalLock();

                                        // Validate hHandle one more time,
                                        // after we have locked iTDHandles.

    SYM_ASSERT ( int(hHandle) <= iTDHandles );

                                        // Make sure there are no active
                                        // locks for hHandle.

    SYM_ASSERT ( !prTDHandles[iIndex].prEntries );

                                        // Invalidate the handle.

    prTDHandles[iIndex].uEntrySize = 0;

                                        // Unlock the list.

    LocalUnLock();

                                        // hHandle was successfully released.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// PVOID CoreLockThreadDataPtr (
//      HOFDATA hHandle
//      );
//
// This function locks the per-thread data area for the current thread.  If
// the data area hasn't been locked yet, upon return all data in the area
// will be initialized to 0.  If the area was previously locked, then only
// its lock counter will be incremented, and all data in it will be
// preserved.
//
// Entry:
//      hHandle - valid handle to a per-thread data area, previously
//                returned by CoreAllocThreadData().
//
// Exit:
//      Returns a pointer to the thread data area for the current thread.
//      The return value will be non-NULL if successful, or NULL if error.
//      For every successful call to CoreLockThreadDataPtr(), there should
//      be a matching call to CoreUnLockPerThreadDataPtr() from the same
//      thread.
//
//---------------------------------------------------------------------------
PVOID CoreLockThreadDataPtr (
    HOFDATA hHandle
    )
    {
    auto int              iIndex;
    auto DWORD            dwThreadID;
    auto PTHREADDATAENTRY prTDEntry;

                                        // Validate input parameters.

    SYM_ASSERT ( int(hHandle) > 0 );

                                        // Convert the handle to direct
                                        // index in the list.

    iIndex = int(hHandle) - 1;

                                        // Get thread ID.

#if defined(SYM_NTK)

    dwThreadID = (DWORD)PsGetCurrentThread();

#elif defined(SYM_VXD)

    dwThreadID = (DWORD)Get_Cur_Thread_Handle();

#else

    dwThreadID = 0;

#endif

    SYM_ASSERT ( dwThreadID );

    if ( !dwThreadID )
        {
        return ( NULL );
        }

                                        // Acquire lock on the list.

    LocalLock();

                                        // Validate hHandle one more time,
                                        // after we have locked iTDHandles.

    SYM_ASSERT ( int(hHandle) <= iTDHandles );

                                        // Find an entry for the current
                                        // thread in the list.

    for ( prTDEntry = prTDHandles[iIndex].prEntries;
          prTDEntry && prTDEntry->dwThreadID != dwThreadID;
          prTDEntry = prTDEntry->prNext );

                                        // If an entry is not found, then
                                        // allocate one.

    if ( !prTDEntry )
        {
        prTDEntry = (PTHREADDATAENTRY) MemAllocPtrX (
                        GHND,
                        prTDHandles[iIndex].uEntrySize );

        if ( prTDEntry )
            {
                                        // Set the thread ID.

            prTDEntry->dwThreadID = dwThreadID;

                                        // Link the entry into the list.

            if ( prTDEntry->prNext = prTDHandles[iIndex].prEntries )
                {
                prTDEntry->prNext->prPrev = prTDEntry;
                }

            prTDHandles[iIndex].prEntries = prTDEntry;
            }
        }

                                        // If we are going to return an
                                        // entry, then increment its lock
                                        // count.

    if ( prTDEntry )
        {
        prTDEntry->iLockCount++;
        }

                                        // Unlock the list.

    LocalUnLock();

                                        // Return pointer to entry's data.

    return ( prTDEntry ? prTDEntry->abyData : NULL );
    }

//---------------------------------------------------------------------------
//
// VOID CoreUnLockThreadDataPtr (
//      HOFDATA hHandle,
//      PVOID   pvData
//      );
//
// This function unlocks the per-thread data area for the current thread,
// previously locked by a successful call to CoreLockThreadDataPtr().  If
// after unlocking of the thread data area, there are no more outstanding
// locks, then the thread data area will be removed from memory.  Otherwise,
// only the lock counter will be updated.
//
// Entry:
//      hHandle - valid handle to a per-thread data area, previously
//                returned by CoreAllocThreadData().
//      pvData  - pointer returned by a call to CoreLockThreadDataPtr().
//                This value is only for verification that the correct
//                thread data area is released.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID CoreUnLockThreadDataPtr (
    HOFDATA hHandle,
    PVOID   pvData
    )
    {
    auto int              iIndex;
    auto DWORD            dwThreadID;
    auto PTHREADDATAENTRY prTDEntry;

                                        // Validate input parameters.

    SYM_ASSERT ( int(hHandle) > 0 );

                                        // Convert the handle to direct
                                        // index in the list.

    iIndex = int(hHandle) - 1;

                                        // Get thread ID.

#if defined(SYM_NTK)

    dwThreadID = (DWORD)PsGetCurrentThread();

#elif defined(SYM_VXD)

    dwThreadID = (DWORD)Get_Cur_Thread_Handle();

#else

    dwThreadID = 0;

#endif

    SYM_ASSERT ( dwThreadID );

    if ( !dwThreadID )
        {
        return;
        }

                                        // Acquire lock on the list.

    LocalLock();

                                        // Validate hHandle one more time,
                                        // after we have locked iTDHandles.

    SYM_ASSERT ( int(hHandle) <= iTDHandles );

                                        // Find an entry for the current
                                        // thread in the list.

    for ( prTDEntry = prTDHandles[iIndex].prEntries;
          prTDEntry && prTDEntry->dwThreadID != dwThreadID;
          prTDEntry = prTDEntry->prNext );

                                        // It is illegal to unlock something
                                        // that is not locked.

    SYM_ASSERT ( prTDEntry );

    if ( prTDEntry )
        {
                                        // The user should've passed a
                                        // pointer to the abyData member of
                                        // prTDEntry.

        SYM_ASSERT ( prTDEntry->abyData == pvData );

                                        // Decrement entry's lock count.

        if ( ! --prTDEntry->iLockCount )
            {
                                        // If the lock count is 0, then
                                        // release the entry.

            if ( prTDEntry->prNext )
                {
                prTDEntry->prNext->prPrev = prTDEntry->prPrev;
                }

            if ( prTDEntry->prPrev )
                {
                prTDEntry->prPrev->prNext = prTDEntry->prNext;
                }
            else
                {
                prTDHandles[iIndex].prEntries = prTDEntry->prNext;
                }

            MemFreePtrX ( prTDEntry );
            }
        }

                                        // Unlock the list.

    LocalUnLock();
    }

//---------------------------------------------------------------------------
//
// Define _CoreLockThreadDataPtr() and _CoreUnLockThreadDataPtr().
//
// These two functions are available only in SYM_DEBUG and PRERELEASE
// compilations.  They work exactly the same way as CoreLockThreadDataPtr()
// and CoreUnLockThreadDataPtr(), except that they update the counter
// LONG counterPerThreadDataLocks.  CoreLockThreadDataPtr() increments
// the counter, and CoreUnLockThreadDataPtr() decrements it.
//
//---------------------------------------------------------------------------
#if defined(SYM_DEBUG) || defined(PRERELEASE)

#define DRIVER_LOCKED_DATA_SEGMENT
#include "drvseg.h"

LONG counterPerThreadDataLocks = 0L;

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

PVOID __stdcall _CoreLockThreadDataPtr ( HOFDATA h )
    {
    auto PVOID p;

    if ( p = CoreLockThreadDataPtr(h) )
        {
        SymInterlockedIncrement ( &counterPerThreadDataLocks );
        }

    return ( p );
    }

VOID __stdcall _CoreUnLockThreadDataPtr ( HOFDATA h, PVOID p )
    {
    if ( p )
        {
        SymInterlockedDecrement ( &counterPerThreadDataLocks );
        }

    CoreUnLockThreadDataPtr ( h, p );
    }

#endif
