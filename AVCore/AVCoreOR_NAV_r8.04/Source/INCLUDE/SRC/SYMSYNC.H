// Copyright 1996 - 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/symsync.h_v   1.1   24 Mar 1998 14:31:00   MKEATIN  $
//
// Description:
//      This file defines synchronization functions for multithreaded
//      applications.
//
// Contains:
//      SymInterlockedIncrement()       (VXD, NTK, W32, WIN, DX, DOS)
//      SymInterlockedDecrement()       (VXD, NTK, W32, WIN, DX, DOS)
//      SymInterlockedExchange()        (VXD, NTK, W32, WIN, DX, DOS, NLM)
//      SymCreateMutex()                (VXD, NTK)
//      SymWaitForMutex()               (VXD, NTK)
//      SymReleaseMutex()               (VXD, NTK)
//      SymDestroyMutex()               (VXD, NTK)
//      SymCreateSemaphore()            (VXD, NTK)
//      SymWaitForSemaphore()           (VXD, NTK)
//      SymReleaseSemaphore()           (VXD, NTK)
//      SymDestroySemaphore()           (VXD, NTK)
//      SymAcquireFastGlobalLock()      (VXD, NTK)
//      SymReleaseFastGlobalLock()      (VXD, NTK)
//      SymCreateResource()             (VXD, NTK)
//      SymAcquireResourceShared()      (VXD, NTK)
//      SymAcquireResourceExclusive()   (VXD, NTK)
//      SymReleaseResource()            (VXD, NTK)
//      SymDestroyResource()            (VXD, NTK)
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/symsync.h_v  $
// 
//    Rev 1.1   24 Mar 1998 14:31:00   MKEATIN
// Latest symsync.h from core.
// 
//    Rev 1.20   24 Jul 1997 13:31:18   DDREW
// Removed SymNLMInterlockedExchange() because it was unresolved on NLM
// 
//    Rev 1.19   01 May 1997 18:31:30   BILL
// 
//    Rev 1.18   01 May 1997 18:21:40   BILL
// Undefined some Interlocked* defines which are already set on Alpha
// 
//    Rev 1.17   17 Mar 1997 00:48:50   RStanev
// Added resource support in SYM_VXD.
//
//    Rev 1.16   17 Jan 1997 18:34:16   RSTANEV
// SYMMUTEX for SYM_NTK now uses KMUTANT instead of KMUTEX objects.  This
// allows threads owning a mutant/mutex to issue APCs.
//
//    Rev 1.15   06 Jan 1997 17:39:16   RSTANEV
// Changed SymWaitForSemaphore() to use local interlocked functions.
//
//    Rev 1.14   31 Oct 1996 21:10:22   RSTANEV
// Added SYM_VXD support for the fast global lock APIs.
//
//    Rev 1.13   11 Jul 1996 12:46:44   RSTANEV
// Updated the comments for SymAcquireFastGlobalLock().  This and
// SymReleaseFastGlobalLock() functions are currently available for
// SYM_NTK platform only.
//
//    Rev 1.12   01 Jul 1996 17:59:24   RSTANEV
// Added limited SYM_NLM support.
//
//    Rev 1.11   17 May 1996 11:38:36   jmillar
// don't provide NT includes - we get them from the source
//
//    Rev 1.10   05 May 1996 12:36:26   RSTANEV
// Added validation of the parameters in SYM_NTK.  Now we will complain
// if the user did not follow the rules for allocating storage in locked
// memory.
//
//    Rev 1.9   04 May 1996 13:22:26   RSTANEV
// What the...
//
//    Rev 1.8   04 May 1996 12:16:50   RSTANEV
// Fixed the SYM_VXD interlocked function.
//
//    Rev 1.7   03 May 1996 19:16:26   RSTANEV
// Added interlocked functions.
//
//    Rev 1.6   05 Apr 1996 08:44:34   RSTANEV
// Added SymReleaseResource().
//
//    Rev 1.5   01 Apr 1996 11:53:22   RSTANEV
// Oops...
//
//    Rev 1.4   01 Apr 1996 11:42:30   RSTANEV
// Added functions for synchronizing shared access to resources.
//
//    Rev 1.3   28 Mar 1996 09:02:16   RSTANEV
// Fixed VxD compilation issues.
//
//    Rev 1.2   27 Mar 1996 15:23:26   RSTANEV
// Added SymAcquireFastGlobalLock() and SymReleaseFastGlobalLock().
//
//    Rev 1.1   12 Mar 1996 19:52:16   RSTANEV
// Changed the limit of NTK semaphores from 1 to 0x7fffffff.
//
//    Rev 1.0   06 Mar 1996 15:25:38   RSTANEV
// Initial revision.
//
//***************************************************************************

#if !defined(__cplusplus) && !defined(SYM_NLM)
#error Need a C++ compiler!
#endif  // __cplusplus

#ifndef _SYMSYNC_H_
#define _SYMSYNC_H_

//---------------------------------------------------------------------------
//
// Interlocked functions.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// The SYM_VXD interlocked functions are all in asm and they directly load
// the result in eax.  Make sure the compiler does not complain about that.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#pragma warning(disable:4035)

#endif

//---------------------------------------------------------------------------
//
// inline LONG SymInterlockedIncrement (
//      LPLONG lpAddend
//      );
//
// This function works just like the Win32 API InterlockedIncrement().
//
// SYM_NTK: The variables upon which the interlocked operations are
//          performed must be in locked memory!  This can be done by
//          defining DRIVER_NTK_LOCKED_DATA_SEGMENT, including drvseg.h,
//          and then defining the variable.  If the variable has to be
//          allocated from program's heap, then the GMEM_FIXED flag should
//          be used when calling MemAllocPtr()!
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_W32) || defined(SYM_WIN) || defined(SYM_DX) || defined(SYM_DOS)

inline LONG SymInterlockedIncrement (
    LPLONG lpAddend
    )
    {
#if defined(SYM_VXD)

    _asm {
        sub     eax,eax
        mov     ecx,lpAddend
        inc     dword ptr [ecx]
        jz      done
        lahf
        shl     eax,10h
    done:
        }

#elif defined(SYM_NTK)

#if defined(SYM_PARANOID) && ( defined(SYM_DEBUG) || defined(PRERELEASE) )

    NTKAssertMemoryIsLocked ( lpAddend, sizeof(*lpAddend) );

#endif

    return ( InterlockedIncrement ( lpAddend ) );

#elif defined(SYM_W32)

    return ( InterlockedIncrement ( lpAddend ) );

#elif defined(SYM_WIN) || defined(SYM_DX) || defined(SYM_DOS)

    return ( *lpAddend++ );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// inline LONG SymInterlockedDecrement (
//      LPLONG lpAddend
//      );
//
// This function works just like the Win32 API InterlockedDecrement().
//
// SYM_NTK: The variables upon which the interlocked operations are
//          performed must be in locked memory!  This can be done by
//          defining DRIVER_NTK_LOCKED_DATA_SEGMENT, including drvseg.h,
//          and then defining the variable.  If the variable has to be
//          allocated from program's heap, then the GMEM_FIXED flag should
//          be used when calling MemAllocPtr()!
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_W32) || defined(SYM_WIN) || defined(SYM_DX) || defined(SYM_DOS)

inline LONG SymInterlockedDecrement (
    LPLONG lpAddend
    )
    {
#if defined(SYM_VXD)

    _asm {
        sub     eax,eax
        mov     ecx,lpAddend
        dec     dword ptr [ecx]
        jz      done
        lahf
        shl     eax,10h
    done:
        }

#elif defined(SYM_NTK)

#if defined(SYM_PARANOID) && ( defined(SYM_DEBUG) || defined(PRERELEASE) )

    NTKAssertMemoryIsLocked ( lpAddend, sizeof(*lpAddend) );

#endif

    return ( InterlockedDecrement ( lpAddend ) );

#elif defined(SYM_W32)

    return ( InterlockedDecrement ( lpAddend ) );

#elif defined(SYM_WIN) || defined(SYM_DX) || defined(SYM_DOS)

    return ( *lpAddend-- );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// inline LONG SymInterlockedExchange (
//      LPLONG Target,
//      LONG   Value
//      );
//
// This function works just like the Win32 API InterlockedExchange().
//
// SYM_NTK: The variables upon which the interlocked operations are
//          performed must be in locked memory!  This can be done by
//          defining DRIVER_NTK_LOCKED_DATA_SEGMENT, including drvseg.h,
//          and then defining the variable.  If the variable has to be
//          allocated from program's heap, then the GMEM_FIXED flag should
//          be used when calling MemAllocPtr()!
//
// SYM_NLM: This is a macro that calls a static function.  This static
//          function is defined in every file that SYMSYNC.H is included in.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_W32) || defined(SYM_WIN) || defined(SYM_DX) || defined(SYM_DOS)

inline LONG SymInterlockedExchange (
    LPLONG Target,
    LONG   Value
    )
    {
#if defined(SYM_VXD)

    _asm {
        mov     eax,Value
        mov     ecx,Target
        xchg    [ecx],eax
        }

#elif defined(SYM_NTK)

#if defined(SYM_PARANOID) && ( defined(SYM_DEBUG) || defined(PRERELEASE) )

    NTKAssertMemoryIsLocked ( Target, sizeof(*Target) );

#endif

    return ( InterlockedExchange ( Target, Value ) );

#elif defined(SYM_W32)

    return ( InterlockedExchange ( Target, Value ) );

#elif defined(SYM_WIN) || defined(SYM_DX) || defined(SYM_DOS)

    auto LONG Save;

    Save = *Target;

    *Target = Value;

    return ( Save );

#endif
    }

#endif

/*
#elif defined(SYM_NLM)

static LONG SymNLMInterlockedExchange (
    LPLONG Target,
    LONG   Value
    )
    {
    auto LONG Save;

    Save = *Target;

    *Target = Value;

    return ( Save );
    }

#define SymInterlockedExchange(l,v) SymNLMInterlockedExchange(l,v)

#endif
*/

//---------------------------------------------------------------------------
//
// Recover from a SYM_VXD hack.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#pragma warning(default:4035)

#endif

//---------------------------------------------------------------------------
//
// Once we have defined all SymInterlockedxxx functions, make sure that
// the Interlockedxxx() APIs become unavailable to SYM_NTK callers.  This
// way we can "police" the usage of these APIs, and make sure that everyone
// uses SymInterlockedxxx() with LOCKED NTK data.  If PAGED data is passed
// to these APIs while the data was paged out, the computer will crash.  But
// since something like this happens once in blue moon, the chances are that
// it will happen on user's computer before it happens on QA's computers.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

// These are already defines on Alpha, remove them.
#ifdef _ALPHA_
#undef InterlockedIncrement 
#undef InterlockedDecrement 
#undef InterlockedExchange  
#endif

#define InterlockedIncrement Hey_You_Should_Use_SymInterlockedIncrement_Instead
#define InterlockedDecrement Hey_You_Should_Use_SymInterlockedDecrement_Instead
#define InterlockedExchange  Hey_You_Should_Use_SymInterlockedExchange_Instead

#endif

//---------------------------------------------------------------------------
//
// Mutex structures and functions.
//
// SYMMUTEX is a structure which should be defined in a locked memory
// accessible by all threads which use the mutex-protected object.  This
// memory can be located in program's heap, or can be in a program data
// segment.  Initially, all members of the mutex object should be set to 0:
//
// SYMMUTEX ListLock = {0};
//
// or
//
// PSYMMUTEX pListLock = MemAllocPtr ( GMEM_ZEROINIT | GMEM_FIXEED,
//                                     sizeof(SYMMUTEX) );
//
// The functions for working with mutex objects are documented below.
//
//---------------------------------------------------------------------------
typedef struct
    {
    BOOL      bMutexInitialized;
#if defined(SYM_VXD)
    PVMMMUTEX hMutex;
#elif defined(SYM_NTK)
    KMUTANT   KeMutant;
#endif
    } SYMMUTEX, *PSYMMUTEX;

//---------------------------------------------------------------------------
//
// inline BOOL SymCreateMutex (
//      PSYMMUTEX pMutex
//      );
//
// This function initializes a mutex object.  Before calling this function,
// all members of pMutex should be zeroed.  After initialization, the mutex
// is set to signaled state.  This means, that the next call to
// SymWaitForMutex(pMutex) will immediately acquire the lock and return.
//
// Returns TRUE if successful, FALSE if error.
//
// Note: Read the section above on allocating SYMMUTEX.
//       SYM_NTK returns always TRUE.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

#if defined(SYM_NTK)

extern "C"
NTKERNELAPI
VOID
KeInitializeMutant (
    IN PRKMUTANT Mutant,
    IN BOOLEAN InitialOwner
    );

#endif

inline BOOL SymCreateMutex (
    PSYMMUTEX pMutex
    )
    {
    SYM_ASSERT ( pMutex );
    SYM_VERIFY_BUFFER ( pMutex, sizeof(*pMutex) );
    SYM_ASSERT ( !pMutex->bMutexInitialized );

#if defined(SYM_VXD)

    pMutex->hMutex = _CreateMutex ( 0, 0 );

    if ( !pMutex->hMutex )
        {
        return ( FALSE );
        }

#elif defined(SYM_NTK)

#if defined(SYM_PARANOID) && ( defined(SYM_DEBUG) || defined(PRERELEASE) )

    NTKAssertMemoryIsLocked ( pMutex, sizeof(*pMutex) );

#endif

    KeInitializeMutant ( &pMutex->KeMutant, FALSE );

#endif

    pMutex->bMutexInitialized = TRUE;

    return ( TRUE );
    }

#endif

//---------------------------------------------------------------------------
//
// inline VOID SymWaitForMutex (
//      PSYMMUTEX pMutex
//      );
//
// This function acquires a mutex.  If the mutex is currently owned by
// another thread, the current thread will block until the mutex is
// signaled.
//
// Note: SYM_NTK waits on behalf of the Executive, in Kernel mode,
//       non-alertable state.
//       SYM_VXD waits with flags BLOCK_THREAD_IDLE.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline VOID SymWaitForMutex (
    PSYMMUTEX pMutex
    )
    {
    SYM_ASSERT ( pMutex );
    SYM_ASSERT ( pMutex->bMutexInitialized );

#if defined(SYM_VXD)

    _EnterMutex ( pMutex->hMutex, BLOCK_THREAD_IDLE );

#elif defined(SYM_NTK)

    while ( KeWaitForSingleObject ( &pMutex->KeMutant,
                                    Executive,
                                    KernelMode,
                                    FALSE,
                                    0 ) != STATUS_SUCCESS )
        {
        SYM_ASSERT ( FALSE );
        }

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// inline VOID SymReleaseMutex (
//      PSYMMUTEX pMutex
//      );
//
// This function signals a mutex.  The current thread must own the mutex
// in order to signal it.  If not, then the results are probably a bug-check
// on NT, and crash in VxD-land.  After this function completes, the next
// thread waiting to acquire the mutex is given it's ownership.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

#if defined(SYM_NTK)

extern "C"
NTKERNELAPI
LONG
KeReleaseMutant (
    IN PRKMUTANT Mutant,
    IN KPRIORITY Increment,
    IN BOOLEAN Abandoned,
    IN BOOLEAN Wait
    );

#endif

inline VOID SymReleaseMutex (
    PSYMMUTEX pMutex
    )
    {
    SYM_ASSERT ( pMutex );
    SYM_ASSERT ( pMutex->bMutexInitialized );

#if defined(SYM_VXD)

    _LeaveMutex ( pMutex->hMutex );

#elif defined(SYM_NTK)

    KeReleaseMutant ( &pMutex->KeMutant, 1, FALSE, FALSE );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// inline VOID SymDestroyMutex (
//      PSYMMUTEX pMutex
//      );
//
// This function destroys a mutex objet.  No thread can own, or use the
// mutex when SymDestroyMutex() is called.  After the function completes,
// the mutex cannot be used, unless it is initialized again.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline VOID SymDestroyMutex (
    PSYMMUTEX pMutex
    )
    {
    SYM_ASSERT ( pMutex );
    SYM_ASSERT ( pMutex->bMutexInitialized );

#if defined(SYM_VXD)

    SYM_ASSERT ( !_GetMutexOwner ( pMutex->hMutex ) );

    if ( !_DestroyMutex ( pMutex->hMutex ) )
        {
        SYM_ASSERT ( FALSE );
        }

#elif defined(SYM_NTK)

    SYM_ASSERT ( KeReadStateMutant ( &pMutex->KeMutant ) == 1 );

#endif

    MEMSET ( pMutex, 0, sizeof ( *pMutex ) );
    }

#endif

//---------------------------------------------------------------------------
//
// Semaphore structures and functions.
//
// SYMSEMAPHORE is a structure which defines a semaphore object.  The
// allocation of a semaphore objects is the same as mutex objects.
//
// The functions for working with semaphore objects are documented below.
//
//---------------------------------------------------------------------------
typedef struct
    {
    BOOL       bSemaphoreInitialized;
    LONG       lBlockedCount;
#if defined(SYM_VXD)
    DWORD      dwSemaphore;
#elif defined(SYM_NTK)
    KSEMAPHORE KeSemaphore;
#endif
    } SYMSEMAPHORE, *PSYMSEMAPHORE;

//---------------------------------------------------------------------------
//
// inline BOOL SymCreateSemaphore (
//      PSYMSEMAPHORE pSemaphore,
//      LONG          lInitialCount
//      );
//
// This function initializes a semaphore object.  Before calling this
// function, all members of pSemaphore should be zeroed.  After
// initialization, the semaphore is set to a not-signaled state, if
// lInitialCount is 0.  This means, that the token count is set to 0, and
// any number of calls to SymWaitForSemaphore(pSemaphore) will block until
// the same number of calls to SymSignalSemaphore(pSemaphore) are made.
// If lInitialCount is greater than 0, then this would correspond to what
// lInitialCount calls to SymSignalSemaphore() would have done.... or at
// least I think that's what it means.....
//
// Returns TRUE if successful, FALSE if error.
//
// Note: Read the section above on allocating SYMSEMAPHORE.
//       SYM_NTK returns always TRUE.
//       A signaled SYMSEMAPHORE releases only one thread at a time.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline BOOL SymCreateSemaphore (
    PSYMSEMAPHORE pSemaphore,
    LONG          lInitialCount
    )
    {
    SYM_ASSERT ( pSemaphore );
    SYM_VERIFY_BUFFER ( pSemaphore, sizeof(*pSemaphore) );
    SYM_ASSERT ( !pSemaphore->bSemaphoreInitialized );
    SYM_ASSERT ( lInitialCount >= 0 );

#if defined(SYM_VXD)

    pSemaphore->dwSemaphore = Create_Semaphore ( lInitialCount );

    if ( !pSemaphore->dwSemaphore )
        {
        return ( FALSE );
        }

#elif defined(SYM_NTK)

#if defined(SYM_PARANOID) && ( defined(SYM_DEBUG) || defined(PRERELEASE) )

    NTKAssertMemoryIsLocked ( pSemaphore, sizeof(*pSemaphore) );

#endif

    KeInitializeSemaphore ( &pSemaphore->KeSemaphore, lInitialCount, 0x7fffffff );

#endif

    pSemaphore->lBlockedCount = 0;

    pSemaphore->bSemaphoreInitialized = TRUE;

    return ( TRUE );
    }

#endif

//---------------------------------------------------------------------------
//
// inline VOID SymWaitForSemaphore (
//      PSYMSEMAPHORE pSemaphore
//      );
//
// This function waits until a semaphore changes its state to signaled.
//
// Note: SYM_NTK waits on behalf of the Executive, in Kernel mode,
//       non-alertable state.
//       SYM_VXD waits with flags BLOCK_THREAD_IDLE.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline VOID SymWaitForSemaphore (
    PSYMSEMAPHORE pSemaphore
    )
    {
    SYM_ASSERT ( pSemaphore );
    SYM_ASSERT ( pSemaphore->bSemaphoreInitialized );

    SymInterlockedIncrement ( &pSemaphore->lBlockedCount );

#if defined(SYM_VXD)

    Wait_Semaphore ( pSemaphore->dwSemaphore, BLOCK_THREAD_IDLE );

#elif defined(SYM_NTK)

    while ( KeWaitForSingleObject ( &pSemaphore->KeSemaphore,
                                     Executive,
                                     KernelMode,
                                     FALSE,
                                     0 ) != STATUS_SUCCESS )
        {
        SYM_ASSERT ( FALSE );
        }

#endif

    SymInterlockedDecrement ( &pSemaphore->lBlockedCount );
    }

#endif

//---------------------------------------------------------------------------
//
// inline VOID SymReleaseSemaphore (
//      PSYMSEMAPHORE pSemaphore
//      );
//
// This function signals a semaphore, thus releasing one blocked thread.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline VOID SymReleaseSemaphore (
    PSYMSEMAPHORE pSemaphore
    )
    {
    SYM_ASSERT ( pSemaphore );
    SYM_ASSERT ( pSemaphore->bSemaphoreInitialized );

#if defined(SYM_VXD)

    Signal_Semaphore ( pSemaphore->dwSemaphore );

#elif defined(SYM_NTK)

    KeReleaseSemaphore ( &pSemaphore->KeSemaphore,
                         1,
                         1,
                         FALSE );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// inline VOID SymDestroySemaphore (
//      PSYMSEMAPHORE pSemaphore
//      );
//
// This function destroys a semaphore.  When called, no threads can be
// bloked on pSemaphore.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline VOID SymDestroySemaphore (
    PSYMSEMAPHORE pSemaphore
    )
    {
    SYM_ASSERT ( pSemaphore );
    SYM_ASSERT ( pSemaphore->bSemaphoreInitialized );

    SYM_ASSERT ( !pSemaphore->lBlockedCount );

#if defined(SYM_VXD)

    Destroy_Semaphore ( pSemaphore->dwSemaphore );

#endif

    MEMSET ( pSemaphore, 0, sizeof(*pSemaphore) );
    }

#endif

//---------------------------------------------------------------------------
//
// FastGlobalLockMutex is defined in CORE\SYMKRNL\DRVMISC.CPP, and is used
// only by SymAcquireFastGlobalLock() and SymReleaseFastGlobalLock().
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

extern FAST_MUTEX FastGlobalLockMutex;

#elif defined(SYM_VXD)

extern PVMMMUTEX FastGlobalLockMutex;

#endif

//---------------------------------------------------------------------------
//
// SymAcquireFastGlobalLock() begins a "global" critical section, which
// ensures that no other thread will be given access to the same or similar
// critical section, until SymReleaseFastGlobalLock() is called.  In other
// words, this function acquires a mutex, thus blocking any other threads
// which try to acquire the same mutex.  SymReleaseFastGlobalLock() releases
// that mutex, which automatically unblocks the next thread waiting for the
// mutex.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK) || defined(SYM_VXD)

inline VOID SymAcquireFastGlobalLock ( VOID )
    {
#if defined(SYM_VXD)

    SYM_ASSERT ( FastGlobalLockMutex );

    _EnterMutex ( FastGlobalLockMutex, BLOCK_THREAD_IDLE );

#elif defined(SYM_NTK)

    ExAcquireFastMutex ( &FastGlobalLockMutex );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// SymReleaseFastGlobalLock() translates into code which releases the lock
// acquired by SymAcquireFastGlobalLock().
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK) || defined(SYM_VXD)

inline VOID SymReleaseFastGlobalLock ( VOID )
    {
#if defined(SYM_VXD)

    SYM_ASSERT ( FastGlobalLockMutex );

    _LeaveMutex ( FastGlobalLockMutex );

#elif defined(SYM_NTK)

    ExReleaseFastMutex ( &FastGlobalLockMutex );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// Generic resource structures and functions.
//
// SYMRESOURCE is a structure which defines a generic resource object, which
// can be acquired for shared or exclusive access.  The allocation rules for
// generic resource objects are the same as mutex and semaphore objects.
//
// The functions for working with SYMRESOURCE objects are documented below.
//
//---------------------------------------------------------------------------
typedef struct
    {
    BOOL      bResourceInitialized;
#if defined(SYM_NTK)
    ERESOURCE rResource;
#elif defined(SYM_VXD)
    PVOID     pvExclusiveOwner;
    PVOID     pvBlockedExclusiveRequests;
    PVOID     pvSharedOwners;
    PVOID     pvBlockedSharedRequests;
    DWORD     dwExclusiveSemaphore;
    DWORD     dwSharedSemaphore;
#endif
    } SYMRESOURCE, *PSYMRESOURCE;

//---------------------------------------------------------------------------
//
// SymCreateResource() initializes a generic resource object.  This function
// will return TRUE if it succeeded, in which case the object is ready for
// acquiring.  FALSE will be returned upon error.
//
// Note: Read the section above on allocating SYMRESOURCE.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline BOOL SymCreateResource (
    PSYMRESOURCE prResource
    )
    {
    SYM_ASSERT ( prResource );
    SYM_ASSERT ( !prResource->bResourceInitialized );

#if defined(SYM_VXD)

    extern BOOL SymVXDCreateResource ( PSYMRESOURCE prResource );

    return ( SymVXDCreateResource ( prResource ) );

#elif defined(SYM_NTK)

#if defined(SYM_PARANOID) && ( defined(SYM_DEBUG) || defined(PRERELEASE) )

    NTKAssertMemoryIsLocked ( prResource, sizeof(*prResource) );

#endif

    prResource->bResourceInitialized =
        ( ExInitializeResourceLite ( &prResource->rResource ) ==
          STATUS_SUCCESS ) ? TRUE : FALSE;

    return ( prResource->bResourceInitialized );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// inline BOOL SymAcquireResourceShared (
//      PSYMRESOURCE prResource,
//      BOOL         bWait,
//      int          iTimeOut
//      );
//
// This function acquires shared access to a resource.  It will immediately
// succeed (return TRUE) if noone has an exclusive ownership of the resource,
// and there is noone waiting for exclusive access.  If exclusive access has
// already been granted, or is being waited for, then this function will
// immediately return FALSE (if bBlock == FALSE), or will block until access
// is granted, or until iTimeOut milliseconds expire.  If bBlock == TRUE &&
// iTimeOut == 0, then this function will block until access is granted.
// Recursively acquiring for either shared or exclusive access of the same
// object from the same thread should not be done in SYM_VXD.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline BOOL SymAcquireResourceShared (
    PSYMRESOURCE prResource,
    BOOL         bBlock,
    int          iTimeOut
    )
    {
    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );

#if defined(SYM_VXD)

    extern BOOL SymVXDAcquireResourceShared ( PSYMRESOURCE prResource,
                                              BOOL         bBlock,
                                              int          iTimeOut );

    return ( SymVXDAcquireResourceShared ( prResource, bBlock, iTimeOut ) );

#elif defined(SYM_NTK)

    if ( !bBlock )
        {
        return ( ExAcquireResourceSharedLite ( &prResource->rResource,
                                               FALSE ) );
        }
    else
        {
        while ( iTimeOut >= 0 )
            {
            if ( ExAcquireResourceSharedLite ( &prResource->rResource,
                                               iTimeOut ? FALSE : TRUE ) )
                {
                return ( TRUE );
                }

            if ( ( iTimeOut -= 100 ) > 0 ) Sleep ( 100 );
            }
        }

    return ( FALSE );
#endif
    }

#endif

//---------------------------------------------------------------------------
//
// inline BOOL SymAcquireResourceExclusive (
//      PSYMRESOURCE prResource,
//      BOOL         bBlock,
//      int          iTimeOut
//      );
//
// This function acquires exclusive access to a resource.  It will
// immediately succeed (return TRUE) if noone owns the resource.  If the
// resource has already been acquired, this function will return FALSE,
// unless bBlock == TRUE. In the latter case, the current thread will block
// until the resource is acquired or until iTimeOut milliseconds haven't
// expired (iTimeOut > 0).
// Recursively acquiring for either shared or exclusive access of the same
// object from the same thread should not be done in SYM_VXD.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline BOOL SymAcquireResourceExclusive (
    PSYMRESOURCE prResource,
    BOOL         bBlock,
    int          iTimeOut
    )
    {
    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );

#if defined(SYM_VXD)

    extern BOOL SymVXDAcquireResourceExclusive ( PSYMRESOURCE prResource,
                                                 BOOL         bBlock,
                                                 int          iTimeOut );

    return ( SymVXDAcquireResourceExclusive ( prResource, bBlock, iTimeOut ) );

#elif defined(SYM_NTK)

    if ( !bBlock )
        {
        return ( ExAcquireResourceExclusiveLite ( &prResource->rResource,
                                                  FALSE ) );
        }
    else
        {
        while ( iTimeOut >= 0 )
            {
            if ( ExAcquireResourceExclusiveLite ( &prResource->rResource,
                                                  iTimeOut ? FALSE : TRUE ) )
                {
                return ( TRUE );
                }

            if ( ( iTimeOut -= 100 ) > 0 ) Sleep ( 100 );
            }
        }

    return ( FALSE );
#endif
    }

#endif

//---------------------------------------------------------------------------
//
// SymReleaseResource() releases the resource allocated by the current
// thread.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline VOID SymReleaseResource (
    PSYMRESOURCE prResource
    )
    {
    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );

#if defined(SYM_VXD)

    extern VOID SymVXDReleaseResource ( PSYMRESOURCE prResource );

    SymVXDReleaseResource ( prResource );

#elif defined(SYM_NTK)

    ExReleaseResourceForThreadLite ( &prResource->rResource,
                                     ExGetCurrentResourceThread() );

#endif
    }

#endif

//---------------------------------------------------------------------------
//
// SymDestroyResource() deinitializes an object initialized by
// SymCreateResource().  The resource should not be owned when it is
// destroyed.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) || defined(SYM_NTK)

inline VOID SymDestroyResource (
    PSYMRESOURCE prResource
    )
    {
    SYM_ASSERT ( prResource );
    SYM_ASSERT ( prResource->bResourceInitialized );

#if defined(SYM_VXD)

    extern VOID SymVXDDestroyResource ( PSYMRESOURCE prResource );

    SymVXDDestroyResource ( prResource );

#elif defined(SYM_NTK)

    if ( NT_SUCCESS ( ExDeleteResourceLite ( &prResource->rResource ) ) )
        {
        MEMSET ( prResource, 0, sizeof(*prResource) );
        }
    else
        {
        SYM_ASSERT ( FALSE );
        }

#endif
    }

#endif

#endif // #ifndef _SYMSYNC_H_
