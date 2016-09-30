// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/memscan.cpv   1.2   14 Aug 1997 13:03:20   DCHI  $
//
// Description:
//  Functions for scanning memory and buffers.
//
// Contains:
//  EngScanMemoryAux()
//  EngScanBuffer()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/memscan.cpv  $
// 
//    Rev 1.2   14 Aug 1997 13:03:20   DCHI
// Removed unreferenced variables in EngScanBooBuffer().
// 
//    Rev 1.1   13 Aug 1997 13:55:26   CNACHEN
// Added new function EngScanBooBuffer to do old style boot scanning like
// certlib.
// 
//    Rev 1.0   18 Apr 1997 13:37:40   MKEATIN
// Initial revision.
// 
//    Rev 1.3   28 May 1996 14:31:16   DCHI
// Updated to support memory objects.
// 
//    Rev 1.2   20 May 1996 14:27:04   DCHI
// Added calls to ProgressInit(), ProgressUpdate(), and ProgressClose()
// to memory scanning.
// 
//    Rev 1.1   16 May 1996 14:12:50   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.0   13 May 1996 16:29:08   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

//*************************************************************************
//
// Function:
//  EngMemorySigSearch()
//
// Parameters:
//  lpMemSigGroup   Pointer to memory signature group
//  lpbyMemory      Pointer to block of memory of wSigLen bytes
//  wSigLen         Length of signature
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Searches through the signature group looking for a signature
//  matching the block of memory passed in.
//
// Returns:
//  FALSE           No virus
//  TRUE            If signature detected in memory
//
//*************************************************************************

BOOL EngMemorySigSearch
(
	LPMEMSIGGROUP   lpMemSigGroup,
	LPBYTE          lpbyMemory,
	WORD            wSigLen,
	LPHVIRUS        lphVirus
)
{
	LPMEMSIGPOOL    lpMemSigPool;
    BYTE            byMemBuf[SIZE_MEM];
	int             iLow, iHigh, iMid;
	int             i, iCmp;

	///////////////////////////////////////////////////////////////////
	//
	//  Get encrypted copy of memory
	//
	///////////////////////////////////////////////////////////////////

    for (i=0;i<(int)wSigLen;i++)
		byMemBuf[i] = *lpbyMemory++ + 1;

	///////////////////////////////////////////////////////////////////
	//
	// Binary search to find pool containing sig
	//
	///////////////////////////////////////////////////////////////////

	iLow = 0;
	iHigh = lpMemSigGroup->wNumSigPools - 1;
	while (iLow <= iHigh)
	{
		iMid = (iLow + iHigh) / 2;

		lpMemSigPool = lpMemSigGroup->stMemSigPool + iMid;

		// Compare memory and first signature of pool

        iCmp = MEMCMP(byMemBuf,
					  lpMemSigPool->lpMemSigs->bySig,
					  wSigLen);

		if (iCmp < 0)
		{
			// In lower half

			iHigh = iMid - 1;
			continue;
		}

		// Compare memory and last signature of pool

		iCmp = MEMCMP(byMemBuf,
					  lpMemSigPool->
						  lpMemSigs[lpMemSigPool->wNumSigs-1].bySig,
					  wSigLen);

		if (iCmp > 0)
		{
			// In upper half

			iLow = iMid + 1;
			continue;
		}

		// Possibly in this pool, iMid is the pool number

		break;
	}

	// Is it within range?

	if (iLow > iHigh)
	{
		// Not in any pool

		return(FALSE);
	}

	///////////////////////////////////////////////////////////////////
	//
	// Binary search through pool
	//
	///////////////////////////////////////////////////////////////////

	lpMemSigPool = lpMemSigGroup->stMemSigPool + iMid;
	iLow = 0;
	iHigh = lpMemSigPool->wNumSigs - 1;
	while (iLow <= iHigh)
	{
		iMid = (iLow + iHigh) / 2;

		// Compare memory and signature

		iCmp = MEMCMP(byMemBuf,
					  lpMemSigPool->lpMemSigs[iMid].bySig,
					  wSigLen);

		if (iCmp < 0)
		{
			// In lower half

			iHigh = iMid - 1;
			continue;
		}
		else
		if (iCmp > 0)
		{
			// In upper half

			iLow = iMid + 1;
			continue;
		}
		else
		{
			// Found a match

			*lphVirus = lpMemSigPool->lpMemSigs[iMid].wVirusRecordIndex;

            // if our *lphVirus is equal to 0xFFFF then this is a deleted
            // memory signature and we should ignore it...

            if (*lphVirus == INVALID_VIRUS_HANDLE)
                return(FALSE);
            else
                return(TRUE);
		}
	}

	// No match at all

	return(FALSE);
}


//*************************************************************************
//
// Function:
//  EngMemoryCD()
//
// Parameters:
//  lpbyMem         Pointer to block of memory to scan
//  dwMemSize       Size of block to scan
//  lpMemSigGroup   Pointer to CD memory signature group
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans the memory block looking for CD signatures using
//  the signatures in the memory group pointed to by lpMemSigGroup.
//
//  The function performs a first byte quick elimination test.
//
//  The function correctly limits scanning to within the region
//  of the memory block.
//
// Returns:
//  FALSE           No virus
//  TRUE            If signature detected in memory
//
//*************************************************************************

BOOL EngMemoryCD
(
    LPBYTE          lpbyMem,
    DWORD           dwMemSize,
    LPMEMSIGGROUP   lpMemSigGroup,
    LPHVIRUS        lphVirus
)
{
    BOOL            bVirusFound;

    // Check boundary condition

    if (dwMemSize < MEM_SIG_LEN_CD)
        return(FALSE);

	// Limit memory accesses to within memory block

	dwMemSize -= MEM_SIG_LEN_CD - 1;

	while (dwMemSize-- != 0)
	{
		// Search for a 0xCD

		if (0xCD == *lpbyMem++)
		{
			// Quick eliminate first

			if (lpMemSigGroup->byQuickEliminate[*lpbyMem] == 0)
			{
				lpbyMem++;
				continue;
			}

			bVirusFound = EngMemorySigSearch(lpMemSigGroup,
											 lpbyMem,
											 MEM_SIG_LEN_CD,
											 lphVirus);

			if (bVirusFound == TRUE)
				return(TRUE);
		}
	}

	// No virus

	return(FALSE);
}


//*************************************************************************
//
// Function:
//  EngMemoryALL()
//
// Parameters:
//  lpbyMem         Pointer to block of memory to scan
//  dwMemSize       Size of block to scan
//  lpMemSigGroup   Pointer to ALL or TOM memory signature group
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans the memory block looking for signatures using
//  the signatures in the memory group pointed to by lpMemSigGroup.
//
//  The function performs a first byte quick elimination test.
//
//  The function also performs a WORD after first byte quick
//  elimination test.
//
//  The function correctly limits scanning to within the region
//  of the memory block.
//
// Returns:
//  FALSE           No virus
//  TRUE            If signature detected in memory
//
//*************************************************************************

BOOL EngMemoryALL
(
    LPBYTE          lpbyMem,
    DWORD           dwMemSize,
    LPMEMSIGGROUP   lpMemSigGroup,
    LPHVIRUS        lphVirus
)
{
    BOOL            bVirusFound;
    WORD            wByte, wBit, wWord;

    // Check boundary condition

    if (dwMemSize < MEM_SIG_LEN_ALL)
        return(FALSE);

    // Limit memory accesses to within memory block

    dwMemSize -= MEM_SIG_LEN_ALL - 1;

    while (dwMemSize-- != 0)
    {
    	// Quick eliminate first

		if (lpMemSigGroup->byQuickEliminate[*lpbyMem] == 0)
        {
        	lpbyMem++;
            continue;
        }

        // Now try quick eliminate on WORD after first BYTE

        wWord = (*(LPWORD)(lpbyMem+1)) & 0x7FFF;
        wByte = wWord >> 3;
        wBit = wWord & 0x07;
        if ((lpMemSigGroup->lpbyQuickWordEliminate[wByte] & (1 << wBit))
			== 0)
        {
        	lpbyMem++;
            continue;
        }

        // Passed quick elimination test

        bVirusFound = EngMemorySigSearch(lpMemSigGroup,
                                         lpbyMem++,
                                         MEM_SIG_LEN_ALL,
                                         lphVirus);

        if (bVirusFound == TRUE)
            return (TRUE);
    }

    // No virus

    return(FALSE);
}


//*************************************************************************
//
// Function:
//  EngScanMemoryAux()
//
// Parameters:
//  hLEng           Handle to local context
//  dwStartAddress  Starting address to scan
//  dwEndAddress    Ending address to scan
//  dwFlags         Set SCAN_TOP_OF_MEMORY if TOM scanning should be done
//  dwMemCookie     Scan memory cookie
//  lpbFoundSig     Pointer to BOOL to store virus found status
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans bytes from dwStartAddress to dwEndAddress-1.
//  Sets *lpbFoundSig to TRUE if a match is found.
//  Otherwise, *lpbFoundSig is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngScanMemoryAux
(
    HLENG           hLEng,
    LPMEMORYOBJECT  lpMemoryObject,
    DWORD           dwStartAddress,
    DWORD           dwEndAddress,
    DWORD           dwFlags,
    DWORD           dwMemCookie,
    LPBOOL          lpbFoundSig,
    LPHVIRUS        lphVirus
)
{
    LPBYTE          lpbyMem;
    DWORD           dwMemAddress, dwEndMemAddress;
    DWORD           dwMemBlkSize;
    COOKIETRAY_T    stCookieTray;
    DWORD           dwTotalMemScanned;
    LPVOID          lpvData;
    CBSTATUS        cbStatus;

	// Set up cookie tray

    stCookieTray.dwGlobalCookie = hLEng->hGEng->dwGlobalCookie;
    stCookieTray.dwLocalCookie = hLEng->dwLocalCookie;
    stCookieTray.dwTransientCookie = dwMemCookie;

    // Initialize progress

    if (hLEng->hGEng->lpProgressCallBacks->
            ProgressInit(NULL,
                         0,
                         dwEndAddress - dwStartAddress,
                         &stCookieTray,
                         &lpvData) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // Assume no virus found

    *lpbFoundSig = FALSE;

    ///////////////////////////////////////////////////////////////////
    //
    //  Scan CD and ALL
    //
    ///////////////////////////////////////////////////////////////////

	dwMemBlkSize = TEMP_BUFFER_SIZE;
	dwMemAddress = dwStartAddress;
	dwEndMemAddress = dwEndAddress;

    dwTotalMemScanned = 0;

	while (dwMemAddress < dwEndMemAddress)
	{
		if (dwMemAddress + TEMP_BUFFER_SIZE >= dwEndMemAddress)
			dwMemBlkSize = dwEndMemAddress - dwMemAddress;

		// Get pointer to memory

        if (lpMemoryObject->lpCallBacks->
                MemGetPtrToMem(lpMemoryObject->lpvInfo,
                               dwMemAddress,
							   hLEng->byTempBuffer,
							   dwMemBlkSize,
							   &lpbyMem,
							   &stCookieTray) == CBSTATUS_ERROR)
		{
			// Error getting pointer to memory

            hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            return(ENGSTATUS_ERROR);
		}

        // Scan CD signatures

		*lpbFoundSig = EngMemoryCD(lpbyMem,dwMemBlkSize,
			&(hLEng->hGEng->lpMemoryScanInfo->stCD),lphVirus);

		if (*lpbFoundSig == TRUE)
		{
            cbStatus = hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            if (lpMemoryObject->lpCallBacks->
                    MemReleasePtrToMem(lpMemoryObject->lpvInfo,
                                       lpbyMem,
                                       &stCookieTray) == CBSTATUS_ERROR)
            {
                // Error releasing pointer to memory

                return(ENGSTATUS_ERROR);
            }

            if (cbStatus == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            return(ENGSTATUS_OK);
        }

        // Scan ALL signatures

		*lpbFoundSig = EngMemoryALL(lpbyMem,dwMemBlkSize,
			&(hLEng->hGEng->lpMemoryScanInfo->stALL),lphVirus);

		// Free pointer

        if (lpMemoryObject->lpCallBacks->
                MemReleasePtrToMem(lpMemoryObject->lpvInfo,
								   lpbyMem,
								   &stCookieTray) == CBSTATUS_ERROR)
		{
			// Error releasing pointer to memory

            hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            return(ENGSTATUS_ERROR);
		}

        if (*lpbFoundSig == TRUE)
        {
            cbStatus = hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            if (cbStatus == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            return(ENGSTATUS_OK);
        }

        dwMemAddress += TEMP_BUFFER_SIZE - MEM_PAGE_OVERLAP;

        // Update progress and abort scan if necessary

        if (dwMemAddress < dwEndMemAddress)
            dwTotalMemScanned = dwMemAddress - dwStartAddress;
        else
            dwTotalMemScanned = dwEndAddress - dwStartAddress;

        cbStatus = hLEng->hGEng->lpProgressCallBacks->
            ProgressUpdate(lpvData,dwTotalMemScanned);

        if (cbStatus == CBSTATUS_ERROR)
        {
            hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            return(ENGSTATUS_ERROR);
		}

        if (cbStatus == CBSTATUS_ABORT)
        {
            cbStatus = hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            if (cbStatus == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            return(ENGSTATUS_OK);
        }
    }

    ///////////////////////////////////////////////////////////////////
    //
    //  Scan TOM
    //
    ///////////////////////////////////////////////////////////////////

    if (!(dwFlags & LENG_SCAN_TOP_OF_MEMORY))
    {
        cbStatus = hLEng->hGEng->lpProgressCallBacks->
            ProgressClose(lpvData);

        if (cbStatus == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        return(ENGSTATUS_OK);
    }

    // Find out where the TOM is

    // Get pointer to TOM word

    if (lpMemoryObject->lpCallBacks->
            MemGetPtrToMem(lpMemoryObject->lpvInfo,
                           0x413,
                           hLEng->byTempBuffer,
						   2,
                           &lpbyMem,
                           &stCookieTray) == CBSTATUS_ERROR)
    {
        // Error getting pointer to memory

        hLEng->hGEng->lpProgressCallBacks->
            ProgressClose(lpvData);

        return(ENGSTATUS_ERROR);
    }

    dwMemAddress = ((DWORD)*((LPWORD)lpbyMem)) * 1024;

    // Release pointer to TOM word

    if (lpMemoryObject->lpCallBacks->
            MemReleasePtrToMem(lpMemoryObject->lpvInfo,
                               lpbyMem,
                               &stCookieTray) == CBSTATUS_ERROR)
    {
        // Error releasing pointer to memory

        hLEng->hGEng->lpProgressCallBacks->
            ProgressClose(lpvData);

        return(ENGSTATUS_ERROR);
    }

    // Make sure it is a legal value

    if (dwMemAddress == 0 || dwMemAddress >= ADDRESS_640K)
    {
        cbStatus = hLEng->hGEng->lpProgressCallBacks->
            ProgressClose(lpvData);

        if (cbStatus == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        return(ENGSTATUS_OK);
    }

    // Now scan it

    dwMemBlkSize = TEMP_BUFFER_SIZE;
	while (dwMemAddress < ADDRESS_640K)
	{
        if (dwMemAddress + TEMP_BUFFER_SIZE >= dwEndMemAddress)
            dwMemBlkSize = dwEndMemAddress - dwMemAddress;

        // Get pointer to memory

        if (lpMemoryObject->lpCallBacks->
                MemGetPtrToMem(lpMemoryObject->lpvInfo,
                               dwMemAddress,
                               hLEng->byTempBuffer,
                               dwMemBlkSize,
                               &lpbyMem,
                               &stCookieTray) == CBSTATUS_ERROR)
        {
            // Error getting pointer to memory

            hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            return(ENGSTATUS_ERROR);
        }

        // Scan TOM signatures

		*lpbFoundSig = EngMemoryALL(lpbyMem,dwMemBlkSize,
			&(hLEng->hGEng->lpMemoryScanInfo->stTOM),lphVirus);

		// Free pointer

        if (lpMemoryObject->lpCallBacks->
                MemReleasePtrToMem(lpMemoryObject->lpvInfo,
								   lpbyMem,
								   &stCookieTray) == CBSTATUS_ERROR)
		{
			// Error releasing pointer to memory

            hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

			return(ENGSTATUS_ERROR);
		}

		if (*lpbFoundSig == TRUE)
        {
            cbStatus = hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            if (cbStatus == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            return(ENGSTATUS_OK);
        }

		dwMemAddress += TEMP_BUFFER_SIZE - MEM_PAGE_OVERLAP;

        // Update progress and abort scan if necessary

        cbStatus = hLEng->hGEng->lpProgressCallBacks->
            ProgressUpdate(lpvData,dwTotalMemScanned);

        if (cbStatus == CBSTATUS_ERROR)
        {
            hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            return(ENGSTATUS_ERROR);
		}

        if (cbStatus == CBSTATUS_ABORT)
        {
            cbStatus = hLEng->hGEng->lpProgressCallBacks->
                ProgressClose(lpvData);

            if (cbStatus == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            return(ENGSTATUS_OK);
        }
    }

    cbStatus = hLEng->hGEng->lpProgressCallBacks->
        ProgressClose(lpvData);

    if (cbStatus == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngScanBuffer()
//
// Parameters:
//  lpbyMem         Pointer to block of memory to scan
//  dwMemSize       Size of block to scan
//  lpMemSigGroup   Pointer to signature group
//  wSigLen         Length of signature <= SIZE_MEM
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans the memory block looking for signatures using
//  the signatures in the memory group pointed to by lpMemSigGroup.
//
//  The function performs a first byte quick elimination test.
//
//  The function also performs a WORD after first byte quick
//  elimination test if the quick word eliminate array is non-NULL.
//
//  The function correctly limits scanning to within the region
//  of the memory block.
//
// Returns:
//  FALSE           No virus
//  TRUE            If signature detected in buffer
//
//*************************************************************************

BOOL EngScanBuffer
(
    LPBYTE          lpbyMem,
    DWORD           dwMemSize,
    LPMEMSIGGROUP   lpMemSigGroup,
    WORD            wSigLen,
    LPHVIRUS        lphVirus
)
{
    BOOL            bVirusFound;
    WORD            wByte, wBit, wWord;

    // Check boundary condition

    if (dwMemSize < wSigLen)
        return(FALSE);

    // Limit memory accesses to within memory block

    dwMemSize -= wSigLen - 1;

    while (dwMemSize-- != 0)
    {
    	// Quick eliminate first

		if (lpMemSigGroup->byQuickEliminate[*lpbyMem] == 0)
        {
        	lpbyMem++;
            continue;
        }

        if (lpMemSigGroup->lpbyQuickWordEliminate != NULL)
        {
            // Now try quick eliminate on WORD after first BYTE

            wWord = (*(LPWORD)(lpbyMem+1)) & 0x7FFF;
            wByte = wWord >> 3;
            wBit = wWord & 0x07;
            if ((lpMemSigGroup->lpbyQuickWordEliminate[wByte] & (1 << wBit))
                == 0)
            {
                lpbyMem++;
                continue;
            }
        }

        // Passed quick elimination test

        bVirusFound = EngMemorySigSearch(lpMemSigGroup,
                                         lpbyMem++,
                                         wSigLen,
                                         lphVirus);

        if (bVirusFound == TRUE)
            return (TRUE);
    }

    // No virus

    return(FALSE);
}



//*************************************************************************
//
// Function:
//  EngScanBooBuffer()
//
// Parameters:
//  lpbyMem         Pointer to block of memory to scan
//  dwMemSize       Size of block to scan
//  lpMemSigGroup   Pointer to signature group
//  wSigLen         Length of signature <= SIZE_MEM
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans the memory block looking for boot signatures using
//  the signatures in the memory group pointed to by lpMemSigGroup.
//  This searches for signatures by looking through the whole scan buffer for
//  each signature, 1 signature at a time, starting with the first
//  one in the table, going to the last.  The table is ordered just as it
//  was in the VIRSCAN.DAT.
//
//  The function correctly limits scanning to within the region
//  of the memory block.
//
//  This function encrypts the block pointed to by the lpbyMem pointer,
//  scans, and decrypts the block after the scan.
//
// Returns:
//  FALSE           No virus
//  TRUE            If signature detected in buffer
//
//*************************************************************************

BOOL EngScanBooBuffer
(
    LPBYTE          lpbyMem,
    DWORD           dwMemSize,
    LPMEMSIGGROUP   lpMemSigGroup,
    WORD            wSigLen,
    LPHVIRUS        lphVirus
)
{
    WORD            wSigIndex, wPoolIndex, wNumSigsInPool;
    LPBYTE          lpbyTemp;
    LPMEMSIGPOOL    lpMSP;
    LPMEMSIG        lpCurSig;
    DWORD           i, j;

    // Check boundary condition

    if (dwMemSize < wSigLen)
        return(FALSE);

    // encrypt the whole buffer before scanning it...

    for (i=0;i<dwMemSize;i++)
        lpbyMem[i]++;

    // Limit memory accesses to within memory block

    dwMemSize -= wSigLen - 1;

    for (wPoolIndex = 0;
         wPoolIndex < lpMemSigGroup->wNumSigPools;
         wPoolIndex++)
    {
        // iterate through each sig pool & sig, scanning the entire buffer for
        // that signature before going on to the next sig.
        // inefficient, I know, but need to support legacy sig ordering.

        lpMSP = &lpMemSigGroup->stMemSigPool[wPoolIndex];

        wNumSigsInPool = lpMSP->wNumSigs;

        lpCurSig = lpMSP->lpMemSigs;

        for (wSigIndex = 0; wSigIndex < wNumSigsInPool; wSigIndex++)
        {
            lpbyTemp = lpbyMem;

            for (j=0;j<dwMemSize;j++,lpbyTemp++)
            {
                if (MEMCMP(lpbyTemp,
                           lpCurSig->bySig,
                           wSigLen) == 0)
                {
                    // found a signature!

                    *lphVirus = (HVIRUS)lpCurSig->wVirusRecordIndex;

                    // decrypt the whole after scanning it...

                    for (i=0;i<dwMemSize + wSigLen - 1;i++)
                        lpbyMem[i]--;

                    return (TRUE);
                }
            }

            lpCurSig++;
        }
    }

    // No virus

    // decrypt the whole buffer after scanning it...

    for (i=0;i<dwMemSize + wSigLen - 1;i++)
        lpbyMem[i]--;

    return(FALSE);
}

