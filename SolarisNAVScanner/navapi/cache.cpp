// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/cache.cpv   1.1   21 May 1998 20:31:14   MKEATIN  $
//
// Description:
//
//  This file contains the functions for managing the early out cache
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/cache.cpv  $
// 
//    Rev 1.1   21 May 1998 20:31:14   MKEATIN
// Changed pamapi.h to pamapi_l.h
//
//    Rev 1.0   21 May 1998 19:23:54   MKEATIN
// Initial revision.
//
//    Rev 1.4   01 Jul 1996 18:03:22   GDZIECI
// If USE_CACHE is not defined, don't even include SYMSYNC.H.
//
//    Rev 1.3   22 May 1996 10:30:22   CNACHEN
// Added #ifndef BORLAND around #include of symsync.h
//
//    Rev 1.2   04 May 1996 13:33:36   RSTANEV
// Changed the caching synchronization to use SymInterlockedExchange()
// instead of local functions.
//
//    Rev 1.1   27 Mar 1996 15:38:58   RSTANEV
// Fixed a compilation error in SYM_NTK.
//
//    Rev 1.0   04 Mar 1996 16:01:42   CNACHEN
// Initial revision.
//
//************************************************************************

#include "pamapi_l.h"

#ifdef USE_CACHE

#ifndef BORLAND
#include "symsync.h"
#endif

#define CACHE_MUTEX_UNLOCKED    0
#define CACHE_MUTEX_LOCKED      1


//************************************************************************
// void CacheInit(LPCACHE lpCache,
//                LPLONG  lplMutex)
//
// Purpose
//
//      This function initializes the cache, invalidating each entry.
//
// Parameters:
//
//      LPCACHE lpCache         Pointer to the cache structure
//      LPLONG  lplMutex        Pointer to object of type LONG, managed
//                              by the client.  This object will be used
//                              by SymInterlockedExchange() and should be
//                              allocated as documented in SYMSYNC.H.
//
// Returns:
//
//      Nothing
//
//************************************************************************
// 3/1/96 Carey created.
//************************************************************************

void CacheInit(LPCACHE lpCache,
               LPLONG  lplMutex)
{
    int i;

    // Clear mutex state to unlocked

    lpCache->lplMutex = lplMutex;

    *lplMutex = CACHE_MUTEX_UNLOCKED;

    for (i=0;i<CACHE_NUM_ENTRIES;i++)
        lpCache->aCache[i].dwLRUIter = CACHE_INVALID_ITER;

    lpCache->dwLRUIter = 1;
}


//************************************************************************
// void CacheInsertItem(LPCACHE            lpCache,
//                      LPCPU_STATE_T      lpEntry)
//
// Purpose
//
//      This function looks for the LRU entry and replaces it
//  with the new entry pointed to by lpEntry.
//
// Parameters:
//
//      LPCACHE       lpCache       Pointer to the cache
//      LPCPU_STATE_T lpEntry       Pointer to the new entry
//
// Returns:
//
//      Nothing
//
//************************************************************************
// 3/1/96 Carey created.
//************************************************************************

void CacheInsertItem(LPCACHE            lpCache,
                     LPCPU_STATE_T      lpEntry)
{
    int         i, nEntryNum;
    DWORD       dwOldest;

    // Obtain mutex

    if (SymInterlockedExchange(lpCache->lplMutex,CACHE_MUTEX_LOCKED) ==
        CACHE_MUTEX_LOCKED)
        return;

    // Search for least recently used entry

    dwOldest = lpCache->dwLRUIter;
    nEntryNum = 0;

    for (i=0;i<CACHE_NUM_ENTRIES;i++)
    {
        if (lpCache->aCache[i].dwLRUIter < dwOldest)
        {
            dwOldest = lpCache->aCache[i].dwLRUIter;
            nEntryNum = i;
        }
    }

    if (CACHE_NUM_ENTRIES)
    {
        lpCache->aCache[nEntryNum].stCPUState = *lpEntry;
        lpCache->aCache[nEntryNum].dwLRUIter = lpCache->dwLRUIter++;
    }

    // Release mutex

    SymInterlockedExchange(lpCache->lplMutex,CACHE_MUTEX_UNLOCKED);
}


//************************************************************************
// CACHESTATUS CacheLookupItem(LPCACHE         lpCache,
//                             LPCPU_STATE_T   lpCPUState)
//
// Purpose
//
//      This function searches the cache for an entry matching
//  the state pointed to by lpCPUState.  If the mutex fails
//  the function does not perform the lookup.
//
// Parameters:
//
//      LPCACHE       lpCache           Pointer to the cache
//      LPCPU_STATE_T lpCPUState        Pointer to the CPU state to lookup
//
// Returns:
//
//      CACHESTATUS_ERROR           If the mutex failed.
//      CACHESTATUS_ENTRY_FOUND     If the cache has such an entry.
//      CACHESTATUS_ENTRY_NOT_FOUND If the cache has no such entry.
//
//************************************************************************
// 3/1/96 Carey created.
//************************************************************************

CACHESTATUS CacheLookupItem(LPCACHE         lpCache,
                            LPCPU_STATE_T   lpCPUState)
{
    int         i;

    // Obtain mutex

    if (SymInterlockedExchange(lpCache->lplMutex,CACHE_MUTEX_LOCKED) ==
        CACHE_MUTEX_LOCKED)
        return(CACHESTATUS_ERROR);

    // Search for the LRU entry

    for (i=0;i<CACHE_NUM_ENTRIES;i++)
    {
        if (lpCache->aCache[i].dwLRUIter &&
            !MEMCMP(lpCPUState,
                    &lpCache->aCache[i].stCPUState,
                    sizeof(CPU_STATE_T)))
        {
            // Make this the most recently used entry

            lpCache->aCache[i].dwLRUIter = lpCache->dwLRUIter++;

            SymInterlockedExchange(lpCache->lplMutex,CACHE_MUTEX_UNLOCKED);
            return(CACHESTATUS_ENTRY_FOUND);
        }
    }

    // Release mutex

    SymInterlockedExchange(lpCache->lplMutex,CACHE_MUTEX_UNLOCKED);
    return(CACHESTATUS_ENTRY_NOT_FOUND);
}


//************************************************************************
// void StoreCPUStateIntoTemp(PAMLHANDLE hLocal,
//                            LPCPU_STATE_T lpTempState)
//
// Purpose:
//
//      This function stores the current CPU state into the structure
//  pointed to by lpTempState.  It saves this information:
//
//      - CS:IP
//      - CACHE_SAVE_CODE_LEN bytes at CS:IP
//      - Image size
//      - EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP
//      - Number of writes
//
//  In addition, it also updates the LRU count.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      LPCPU_STATE_T lpTempState   Pointer to structure to store state
//
// Returns:
//
//      Nothing
//
//************************************************************************
// 3/1/96 Carey created.
//************************************************************************

void StoreCPUStateIntoTemp(PAMLHANDLE hLocal, LPCPU_STATE_T lpTempState)
{
    int i;

    lpTempState->wCS = hLocal->CPU.CS;
    lpTempState->wIP = hLocal->CPU.IP;

    for (i=0;i<CACHE_SAVE_CODE_LEN;i++)
        lpTempState->byCode[i] = get_byte(hLocal,
                                          hLocal->CPU.CS,
                                          (WORD)(hLocal->CPU.IP+i));

    hLocal->stTempEntry.dwLRUIter = hLocal->CPU.iteration;
    lpTempState->dwImageSize = hLocal->CPU.ulImageSize;
    lpTempState->dwEAX = hLocal->CPU.preg.D.EAX;
    lpTempState->dwEBX = hLocal->CPU.preg.D.EBX;
    lpTempState->dwECX = hLocal->CPU.preg.D.ECX;
    lpTempState->dwEDX = hLocal->CPU.preg.D.EDX;
    lpTempState->dwESI = hLocal->CPU.ireg.D.ESI;
    lpTempState->dwEDI = hLocal->CPU.ireg.D.EDI;
    lpTempState->dwEBP = hLocal->CPU.ireg.D.EBP;
    lpTempState->dwESP = hLocal->CPU.ireg.D.ESP;
    lpTempState->dwNumWrites = hLocal->CPU.num_writes;
}


#endif // USE_CACHE
