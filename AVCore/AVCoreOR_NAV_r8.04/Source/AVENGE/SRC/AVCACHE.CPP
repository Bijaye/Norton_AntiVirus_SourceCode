// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/AVCACHE.CPv   1.0   18 Apr 1997 13:37:18   MKEATIN  $
//
// Description:
//  Functions implementing virus information cache.
//
// Contains:
//  EngCacheInit()
//  EngCacheClose()
//  EngCacheLocateInfoHVIRUS()
//  EngCacheLocateInfoVID()
//  EngCacheInsertItem()
//
// See Also:
//  avenge.c, getinfo.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/AVCACHE.CPv  $
// 
//    Rev 1.0   18 Apr 1997 13:37:18   MKEATIN
// Initial revision.
// 
//    Rev 1.5   23 Oct 1996 11:19:02   DCHI
// Added IPC callback support.
// 
//    Rev 1.4   12 Aug 1996 17:47:04   DCHI
// Added SYM_UNIX.
// 
//    Rev 1.3   17 Jul 1996 16:15:16   CNACHEN
// Added break statement when entry is found in cache.
// 
//    Rev 1.2   04 Jun 1996 10:33:50   DCHI
// Modification for separate compilation under VC20.
// 
//    Rev 1.1   16 May 1996 14:12:36   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.0   13 May 1996 16:28:44   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

#define CACHE_MUTEX_WAIT_MS     60000

//*************************************************************************
//
// Function:
//  EngCacheInit()
//
// Parameters:
//  lpInfoCache         Pointer to virus info cache
//  lpIPCCallBacks      Pointer to IPC callback structure
//
// Description:
//  Initializes the virus information cache by invalidating all entries
//  and creates a mutex for accessing the cache.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCacheInit
(
    LPVIRUS_INFO_CACHE  lpInfoCache,
    LPIPCCALLBACKS      lpIPCCallBacks
)
{
    int                 i;

    // reset the LRU counter

    lpInfoCache->dwLRUValue = 1;

    // reset each LRU counter to 0 so they can be reused

    for (i=0;i<VIRUS_INFO_CACHE_SIZE;i++)
		lpInfoCache->stData[i].dwLRUValue = INVALID_INFO_CACHE_LRU;

    // remember the mutex callbacks

    lpInfoCache->lpIPCCallBacks = lpIPCCallBacks;

    // Create the mutex

    if (lpIPCCallBacks->MutexCreate(&lpInfoCache->lpvMutexInfo) !=
        CBSTATUS_OK)
        return(ENGSTATUS_ERROR);

	return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngCacheClose()
//
// Parameters:
//  lpInfoCache         Pointer to virus info cache
//
// Description:
//  Closes the virus info cache by destroying the access mutex.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCacheClose
(
	LPVIRUS_INFO_CACHE  lpInfoCache
)
{
    // Destroy the mutex

    if (lpInfoCache->lpIPCCallBacks->
            MutexDestroy(lpInfoCache->lpvMutexInfo) != CBSTATUS_OK)
    {
        // Failed to destroy mutex

        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngCacheLocateInfoHVIRUS()
//
// Parameters:
//  hVirus              Handle to virus to look up
//  lpInfoCache         Pointer to virus info cache
//  lpVirusInfo         Pointer to VIRUS_INFO_T to store virus info
//  lphVirus            Pointer to virus handle to
//  lpszVirusName       Pointer to buffer to store virus name
//  lpbFoundEntry       Pointer to BOOL to set to found result
//
// Description:
//  Searches for virus info for the virus with handle hVirus.
//  Sets *lpbFoundEntry to TRUE if an entry was found.
//  Otherwise, *lpbFoundEntry is set to FALSE.
//
//  If any of lpVirusInfo, lphVirus, or lpszVirusName are NULL, then
//  that information is not retrieved from the cache.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCacheLocateInfoHVIRUS
(
	HVIRUS              hVirus,
	LPVIRUS_INFO_CACHE  lpInfoCache,
	LPVIRUS_INFO        lpVirusInfo,
    LPHVIRUS            lphVirus,
    LPSTR               lpszVirusName,
    LPBOOL              lpbFoundEntry
)
{
    int                 i;

    // Assume we're going to fail...

    *lpbFoundEntry = FALSE;

    // wait until we can lock our cache information

    if (lpInfoCache->lpIPCCallBacks->
            MutexWait(lpInfoCache->lpvMutexInfo,
                      CACHE_MUTEX_WAIT_MS) != CBSTATUS_OK)
    {
        // Failed to obtain mutex on cache

        return(ENGSTATUS_ERROR);
    }

    // now perform our search

    for (i=0;i<VIRUS_INFO_CACHE_SIZE;i++)
        if (lpInfoCache->stData[i].dwLRUValue != INVALID_INFO_CACHE_LRU &&
            lpInfoCache->stData[i].hVirus == hVirus)
        {
            // update our LRU count for this cache record...

            lpInfoCache->stData[i].dwLRUValue = lpInfoCache->dwLRUValue++;

            // fetch the virus info

            if (lpVirusInfo != NULL)
                *lpVirusInfo = lpInfoCache->stData[i].stVirusInfo;

            if (lphVirus != NULL)
                *lphVirus = lpInfoCache->stData[i].hVirus;

            if (lpszVirusName != NULL)
                MEMCPY(lpszVirusName,
                       lpInfoCache->stData[i].sVirusName,
                       MAX_VIRUS_NAME_LENGTH);

            // we found our entry

            *lpbFoundEntry = TRUE;
            break;
        }

    // Release mutex on cache

    if (lpInfoCache->lpIPCCallBacks->
            MutexRelease(lpInfoCache->lpvMutexInfo) != CBSTATUS_OK)
    {
        // Failed to release mutex on cache

        return(ENGSTATUS_ERROR);
    }

	return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngCacheLocateInfoVID()
//
// Parameters:
//  hVirus              Handle to virus to look up
//  lpInfoCache         Pointer to virus info cache
//  lpVirusInfo         Pointer to VIRUS_INFO_T to store virus info
//  lphVirus            Pointer to virus handle to
//  lpszVirusName       Pointer to buffer to store virus name
//  lpbFoundEntry       Pointer to BOOL to set to found result
//
// Description:
//  Searches for virus info for the virus with VID wVirusID.
//  Sets *lpbFoundEntry to TRUE if an entry was found.
//  Otherwise, *lpbFoundEntry is set to FALSE.
//
//  If any of lpVirusInfo, lphVirus, or lpszVirusName are NULL, then
//  that information is not retrieved from the cache.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCacheLocateInfoVID
(
	WORD                wVirusID,
	LPVIRUS_INFO_CACHE  lpInfoCache,
	LPVIRUS_INFO        lpVirusInfo,
	LPHVIRUS            lphVirus,
    LPSTR               lpszVirusName,
    LPBOOL              lpbFoundEntry
)
{
	int i;

	// Assume we're going to fail...

    *lpbFoundEntry = FALSE;

    // wait until we can lock our cache information

    if (lpInfoCache->lpIPCCallBacks->
            MutexWait(lpInfoCache->lpvMutexInfo,
                      CACHE_MUTEX_WAIT_MS) != CBSTATUS_OK)
    {
        // Failed to obtain mutex on cache

        return(ENGSTATUS_ERROR);
    }

    // now perform our search

	for (i=0;i<VIRUS_INFO_CACHE_SIZE;i++)
		if (lpInfoCache->stData[i].dwLRUValue != INVALID_INFO_CACHE_LRU &&
			lpInfoCache->stData[i].stVirusInfo.wVirusID == wVirusID)
	{
		// update our LRU count for this cache record...

		lpInfoCache->stData[i].dwLRUValue = lpInfoCache->dwLRUValue++;

		// fetch the virus info

        if (lpVirusInfo != NULL)
            *lpVirusInfo = lpInfoCache->stData[i].stVirusInfo;

        if (lphVirus != NULL)
            *lphVirus = lpInfoCache->stData[i].hVirus;

        if (lpszVirusName != NULL)
            MEMCPY(lpszVirusName,
                   lpInfoCache->stData[i].sVirusName,
                   MAX_VIRUS_NAME_LENGTH);

		// we found our entry

        *lpbFoundEntry = TRUE;
	}

    // Release mutex on cache

    if (lpInfoCache->lpIPCCallBacks->
            MutexRelease(lpInfoCache->lpvMutexInfo) != CBSTATUS_OK)
    {
        // Failed to release mutex on cache

        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngCacheInsertItem()
//
// Parameters:
//  lpInfoCache         Pointer to virus info cache
//  hVirus              Handle to virus to look up
//  lpVirusInfo         Pointer to VIRUS_INFO_T to store to cache
//  lpszVirusName       Pointer to buffer containing virus name
//
// Description:
//  Inserts virus info into the virus info cache using LRU eviction.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCacheInsertItem
(
	LPVIRUS_INFO_CACHE  lpInfoCache,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPSTR               lpszVirusName
)
{
    int                 i, nEntryNum;
    DWORD               dwOldest;

    // wait until we can lock our cache information

    if (lpInfoCache->lpIPCCallBacks->
            MutexWait(lpInfoCache->lpvMutexInfo,
                      CACHE_MUTEX_WAIT_MS) != CBSTATUS_OK)
    {
        // Failed to obtain mutex on cache

        return(ENGSTATUS_ERROR);
    }

    // Search for least recently used entry

    dwOldest = lpInfoCache->dwLRUValue;
    nEntryNum = 0;

    for (i=0;i<VIRUS_INFO_CACHE_SIZE;i++)
    {
        if (lpInfoCache->stData[i].dwLRUValue < dwOldest)
        {
            dwOldest = lpInfoCache->stData[i].dwLRUValue;
            nEntryNum = i;
        }
    }

    // insert the new entry into the cache

    if (VIRUS_INFO_CACHE_SIZE)      // true when non-zero sized cache
	{
		lpInfoCache->stData[nEntryNum].hVirus = hVirus;

		lpInfoCache->stData[nEntryNum].stVirusInfo = *lpVirusInfo;
        lpInfoCache->stData[nEntryNum].dwLRUValue = lpInfoCache->dwLRUValue++;

		MEMCPY(lpInfoCache->stData[nEntryNum].sVirusName,
			   lpszVirusName,
			   MAX_VIRUS_NAME_LENGTH);
	}

    // Release mutex on cache

    if (lpInfoCache->lpIPCCallBacks->
            MutexRelease(lpInfoCache->lpvMutexInfo) != CBSTATUS_OK)
    {
        // Failed to release mutex on cache

        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}

