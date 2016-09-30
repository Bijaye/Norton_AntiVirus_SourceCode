// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/ALGLOAD.CPv   1.1   08 Jul 1997 17:48:24   MKEATIN  $
//
// Description:
//  Functions for loading and unloading of algorithmic scanning
//  signatures.
//
// Contains:
//  EngLoadAlgScanData()
//  EngFreeAlgAux()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/ALGLOAD.CPv  $
// 
//    Rev 1.1   08 Jul 1997 17:48:24   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.0   18 Apr 1997 13:37:14   MKEATIN
// Initial revision.
// 
//    Rev 1.4   06 Jun 1996 18:07:44   DCHI
// Removed algorithmic signature skipping.
// 
//    Rev 1.3   17 May 1996 14:15:54   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
// 
//    Rev 1.2   16 May 1996 14:12:24   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.1   15 May 1996 14:06:32   DCHI
// Return value correction for EngLoadAlgScanData.
// 
//    Rev 1.0   13 May 1996 16:28:42   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

//********************************************************************
//
// Function:
//  EngAlgHash()
//
// Parameters:
//  lpPoolInfoStruct        Info on the location and contents of the alg pools
//  wNumHashBytes           Specifies 1-byte, 2-byte or 4-byte hash
//  wHashSize               Specifies the size of the hash table to create
//  lpAlgHashEntry          Pointer to the hash table of ALG_HASH_ENTRY structs
//  lpwMaxAlgsToHash        Maximum number of signatures to hash.  This is
//                          (IN/OUT) and is revised to reflect the actual
//                          number of signatures left to hash.  The value
//                          starts out as the total number of signatures.
//                          After all 1/2/4 signatures are hashed, the value
//                          of this variable will be equal to the number of no-
//                          hash signatures left.
//  lpwCurPool              The current pool where signatures are being pulled
//                          and hashed from.
//  lpwPoolOffset           The offset in the current pool where the next sig
//                          to be hashed resides.
//
// Description:
//  This function assumes that the alg pools contain all the algorithmic sigs
//  in the proper order.  These signatures are ordered by according to their
//  hash-ability.  The pools are ordered with all 4-byte hash signatures first,
//  then 2-byte hash signatures, then 1-byte hash signatures.  Finally, the
//  no-hash signatures follow.  These signatures have been ordered and sorted
//  such that the first N signatures will hash to hash cell 0, the next M
//  signatures will hash to hash cell 1, etc.  This routine will construct
//  the specified hash table from the provided pools of algorithmic signatures.
//  It does no memory allocation or other operations that would warrant a
//  return value.
//
// Returns:
//  nothing
//
//********************************************************************


void EngAlgHash
(
    LPPOOL_INFO_STRUCT  lpPoolInfoStruct,
    WORD                wNumHashBytes,
    WORD                wHashSize,
    LPALG_HASH_ENTRY    lpAlgHashEntry,
    LPWORD              lpwMaxAlgsToHash,
    LPWORD              lpwCurPool,
    LPWORD              lpwPoolOffset
)
{
    LPALG_SIG_DATA      lpAlgSigData;
    LPBYTE              lpbyCurPool;
    DWORD               dwValueToHash;
    WORD                wPoolOffset, wPoolSize, wCurPool, w, wMaxAlgsToHash;
    WORD                wHashValue;
    WORD                wCount;

    // store our current pool, current pool offset, and number of remaining
    // alg sigs to hash in local variables...

	wCurPool = *lpwCurPool;
	wPoolOffset = *lpwPoolOffset;
	wMaxAlgsToHash = *lpwMaxAlgsToHash;

    // reset the specified hash table so we can start inserting data into it.

	for (wCount=0;wCount<wHashSize;wCount++)
	{
        // this hash table entry does not point to any signatures

		lpAlgHashEntry[wCount].byPoolNum = INVALID_ALG_POOL_NUM;
		lpAlgHashEntry[wCount].wNumSigs = 0;
	}

    // Determine the size of the current pool of signatures and its location
    // in memory

    wPoolSize = lpPoolInfoStruct->AlgPoolArray[wCurPool].wBuffSize;
    lpbyCurPool = lpPoolInfoStruct->AlgPoolArray[wCurPool].lpbyAlgPoolPtr;

    // iterate through each signature.  the number of signatures we are
    // processing for any one hash will typically be less than wMaxAlgsToHash.
    // Therefore, wMaxAlgsToHash is an upper bound...

    for (wCount=0;wCount<wMaxAlgsToHash;wCount++)
    {
        // have we hit the end of the current pool?  If so, advance to the
        // next pool...

        if (wPoolOffset == wPoolSize)
        {
            wPoolSize = lpPoolInfoStruct->AlgPoolArray[++wCurPool].wBuffSize;
            lpbyCurPool = lpPoolInfoStruct->
                AlgPoolArray[wCurPool].lpbyAlgPoolPtr;
            wPoolOffset = 0;
        }

        // NOTE: if wPoolOffset > wPoolSize then we have some serious problems!

        // obtain a pointer to our next signature...

        lpAlgSigData = (LPALG_SIG_DATA)(lpbyCurPool + wPoolOffset);

        // update our offset in the current pool.  this skips over the
        // virus record number and signature length.

        wPoolOffset += sizeof(ALG_SIG_HDR_T);

        // One of first n=(4,2,1) bytes is function?  If so, we're done
        // hashing the current set of n byte-hashable signatures and need
        // to advance to n/2 byte-hashable signatures...

        for (w=0;w<wNumHashBytes;w++)
        {
            // decrypt the w'th byte and mask to check for functional
            // bytes (0xD0 through 0xDF).  If we find one, we have finished
            // hashing our n byte-hashable signatures and should go to the
            // n/2 byte-hashable signatures.

            if (((lpbyCurPool[wPoolOffset + w] - 1) & 0xF0) == 0xD0)
            {
                // back up so we're at the start of the signature...

                wPoolOffset -= sizeof(ALG_SIG_HDR_T);

                // record our current pool, offset in the pool, and number of
                // signatures left to hash...

                *lpwCurPool = wCurPool;
                *lpwPoolOffset = wPoolOffset;
                *lpwMaxAlgsToHash -= wCount;

                // finally, return

                return;
            }
		}

        // Time to hash a signature.  compute the hash value from the first
        // n bytes.

		dwValueToHash = (BYTE)(lpAlgSigData->bySigData[0]-1);
        for (w=1;w<wNumHashBytes;w++)
            dwValueToHash |=
                ((DWORD)(BYTE)(lpAlgSigData->bySigData[w]-1) << (8*w));

        // perform the proper modulo division against the hash table size

        wHashValue = (WORD)(dwValueToHash % wHashSize);

        // wHashValue has the index in our hash table for the next signature
        // if this wHashValue'th entry is unused, then initialize it to point
        // to the current signature we're hashing (pool, pool-offset). Also
        // set the number of signatures bucketed off this entry to 1.

        if (lpAlgHashEntry[wHashValue].wNumSigs == 0)
        {
            lpAlgHashEntry[wHashValue].byPoolNum = (BYTE)wCurPool;
			lpAlgHashEntry[wHashValue].wPoolOffset =
                wPoolOffset - sizeof(ALG_SIG_HDR_T);
            lpAlgHashEntry[wHashValue].wNumSigs = 1;
        }
        else
        {
            // This signature has the same hash value as the last signature,
            // so just increment the signature count but don't change any of
            // the pointers.

            lpAlgHashEntry[wHashValue].wNumSigs++;
        }

        // advance over the body of the signature...

        wPoolOffset += lpAlgSigData->bySigLength;
    }

    // report back on our progress

	*lpwCurPool = wCurPool;
	*lpwPoolOffset = wPoolOffset;
	*lpwMaxAlgsToHash -= wCount;
}


//********************************************************************
//
// Function:
//  EngFreeAlgAux()
//
// Parameters:
//  lpGeneralCallBacks      General callbacks for memory freeing
//  lpAlgScanInfo           Algorithmic scan data (points to dynamically
//                          allocated data structures that need to be freed)
//
// Description:
//  This function frees each of the hash tables (1, 2 and 4 byte) and then
//  frees the memory allocated for each of the alg signature pools.
//
// Returns:
//  nothing
//
//********************************************************************

ENGSTATUS EngFreeAlgAux
(
	LPGENERALCALLBACKS  lpGeneralCallBacks,
    LPALG_SCAN_INFO		lpAlgScanInfo
)
{
    WORD                wCount;

    // first free our hash tables...

    if (lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpFourByteTable) ==
        CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpTwoByteTable) ==
        CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpOneByteTable) ==
        CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

	// now free allocated pools

    for (wCount=0;
         wCount<lpAlgScanInfo->lpPoolInfoStruct->wNumAlgPools;
         wCount++)
    {
        if (lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpPoolInfoStruct->
                                        AlgPoolArray[wCount].lpbyAlgPoolPtr) ==
            CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);
    }

    // now we have to free our pool information structure...

    if (lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpPoolInfoStruct) ==
		CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    return(ENGSTATUS_OK);
}

//********************************************************************
//
// Function:
//  EngLoadAlgAux()
//
// Parameters:
//  lpDataFileCallBacks     Callbacks for accessing the data files
//  lpGeneralCallBacks      General callbacks for memory freeing and allocation
//  lpSectionInfo           Information on the section which contains the
//                          alg scanning data. (start offset in the data file,
//                          etc.)
//  lpvFileInfo             A file pointer for our data file
//  lpAlgScanInfo           This is a pointer to a structure that will be
//                          filled with the appropriate data after the alg
//                          sigs are loaded. It points to all of the pools
//                          used to store these alg signatures as well as the
//                          hash tables associated with these signatures
//
// Description:
//  This function loads the specified alg signatures into dynamically allocated
//  pools of memory and then calls upon EngAlgHash to create hash tables for
//  scanning.  Memory is allocated for each of the pools required to hold all
//  of the signatures.  Memory is also allocated for the hash tables.
//
// Returns:
//  ENGSTATUS_OK            On successful load (memory allocated and ready)
//  ENGSTATUS_ERROR         On unsuccessful load (all memory is freed)
//
//********************************************************************

ENGSTATUS EngLoadAlgAux
(
	LPDATAFILECALLBACKS  lpDataFileCallBacks,
	LPGENERALCALLBACKS   lpGeneralCallBacks,
	LPDATA_FILE_TABLE	 lpSectionInfo,
	LPVOID               lpvFileInfo,
	LPALG_SCAN_INFO      lpAlgScanInfo
)
{
    HASH_INFO_T         stHashInfo;
    LPBYTE              lpbyCurPool;
    WORD                wPoolOffset;
    WORD                wNumAlgs, wCount, wSigLen, wCurPool;
    DWORD               dwBytesRead, dwOff;

	// seek to the start of the section

	if (lpDataFileCallBacks->
			lpFileCallBacks->FileSeek(lpvFileInfo,
									 lpSectionInfo->dwStartOffset,
									 SEEK_SET,
									 &dwOff) == CBSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

	// read in the hash table description structure...

    if (lpDataFileCallBacks->
			lpFileCallBacks->FileRead(lpvFileInfo,
									 &stHashInfo,
									 sHASH_INFO,
									 &dwBytesRead) == CBSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

	// allocate the appropriate memory for the hash tables

	lpAlgScanInfo->wFourByteHashSize = WENDIAN(stHashInfo.wFourByteHashSize);
	if (lpGeneralCallBacks->
			MemoryAlloc(lpAlgScanInfo->wFourByteHashSize *
							sizeof(ALG_HASH_ENTRY_T),
						MEM_FIXME,
						(LPLPVOID)&lpAlgScanInfo->lpFourByteTable) ==
		CBSTATUS_ERROR)
	{
		// error allocating appropriate memory
		// we have not allocated any memory yet, so just return with error

		return(ENGSTATUS_ERROR);
	}

	lpAlgScanInfo->wTwoByteHashSize = WENDIAN(stHashInfo.wTwoByteHashSize);
	if (lpGeneralCallBacks->
			MemoryAlloc(lpAlgScanInfo->wTwoByteHashSize *
							sizeof(ALG_HASH_ENTRY_T),
						MEM_FIXME,
						(LPLPVOID)&lpAlgScanInfo->lpTwoByteTable) ==
		CBSTATUS_ERROR)
	{
		// error allocating appropriate memory
		// we must free our four byte hash table before returning

		lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpFourByteTable);

		return(ENGSTATUS_ERROR);
	}

	lpAlgScanInfo->wOneByteHashSize = WENDIAN(stHashInfo.wOneByteHashSize);
	if (lpGeneralCallBacks->
			MemoryAlloc(lpAlgScanInfo->wOneByteHashSize *
							sizeof(ALG_HASH_ENTRY_T),
						MEM_FIXME,
						(LPLPVOID)&lpAlgScanInfo->lpOneByteTable) ==
		CBSTATUS_ERROR)
	{
		// error allocating appropriate memory
		// we must free our four and two byte hash tables before returning

		lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpFourByteTable);
		lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpTwoByteTable);

		return(ENGSTATUS_ERROR);
	}

	// now we must allocate our pool information structure

	if (lpGeneralCallBacks->
			MemoryAlloc(sizeof(POOL_INFO_STRUCT_T),
						MEM_FIXME,
						(LPLPVOID)&lpAlgScanInfo->lpPoolInfoStruct) ==
		CBSTATUS_ERROR)
	{
		// error allocating appropriate memory for our pool structure
		// we must free our four/two/one byte hash tables before returning

		lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpFourByteTable);
		lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpTwoByteTable);
		lpGeneralCallBacks->MemoryFree(lpAlgScanInfo->lpOneByteTable);

		return(ENGSTATUS_ERROR);
	}

	// reset the number of pools to 0...

	lpAlgScanInfo->lpPoolInfoStruct->wNumAlgPools = 0;

	// read the appropriate number of sigs

	wNumAlgs = WENDIAN(stHashInfo.wNumSignatures);

	// starting in a fresh, new pool...

	wPoolOffset = 0;
	lpbyCurPool = NULL;

	for (wCount=0;wCount<wNumAlgs;wCount++)
	{
		// Need to advance to next pool?

		if (wPoolOffset >
			(WORD)(MAX_POOL_BYTE_SIZE - MAX_ALG_SIG_DATA_SIZE))
		{
			// Save pool pointer and advance count

			lpAlgScanInfo->lpPoolInfoStruct->AlgPoolArray[
				lpAlgScanInfo->lpPoolInfoStruct->wNumAlgPools].
				wBuffSize = wPoolOffset;

			lpAlgScanInfo->lpPoolInfoStruct->AlgPoolArray[
				lpAlgScanInfo->lpPoolInfoStruct->wNumAlgPools++].
				lpbyAlgPoolPtr = lpbyCurPool;

			// Set pool ptr to NULL for subsequent allocation of new buffer

			lpbyCurPool = NULL;
		}

		if (lpbyCurPool == NULL)
		{
			// Allocate pool with maximum size

			if (lpGeneralCallBacks->
					MemoryAlloc(MAX_POOL_BYTE_SIZE * sizeof(BYTE),
								MEM_FIXME,
								(LPLPVOID)&lpbyCurPool) ==
				CBSTATUS_ERROR)
			{
				// free all currently allocated pools, hash tables, and
				// pool information

				EngFreeAlgAux(lpGeneralCallBacks,lpAlgScanInfo);

				return(ENGSTATUS_ERROR);
			}

			// successful allocation of new pool.. set current offset and
			// continue

			wPoolOffset = 0;
		}

        // Read VirusRecordIndex and SigLen of our next signature

		if (lpDataFileCallBacks->
			lpFileCallBacks->FileRead(lpvFileInfo,
									 lpbyCurPool + wPoolOffset,
                                     sizeof(ALG_SIG_HDR_T),
									 &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != sizeof(ALG_SIG_HDR_T))
		{
			// free all currently allocated pools, hash tables, and
			// pool information

			EngFreeAlgAux(lpGeneralCallBacks,lpAlgScanInfo);

			return(ENGSTATUS_ERROR);
		}

        ((LPALG_SIG_DATA)(lpbyCurPool+wPoolOffset))->wVirusRecordIndex =
            WENDIAN(((LPALG_SIG_DATA)(lpbyCurPool+wPoolOffset))->
                wVirusRecordIndex);

		// determine our signature length...

        wSigLen = ((LPALG_SIG_DATA)(lpbyCurPool+wPoolOffset))->bySigLength;

		// Advance our pool index to where we're going to place our signature
		// data...

        wPoolOffset += sizeof(ALG_SIG_HDR_T);

		// Now read sig data

		if (lpDataFileCallBacks->
			lpFileCallBacks->FileRead(lpvFileInfo,
									 lpbyCurPool + wPoolOffset,
									 wSigLen,
									 &dwBytesRead) == CBSTATUS_ERROR ||
			wSigLen != dwBytesRead)
		{
			// free all currently allocated pools, hash tables, and
            // pool information

			EngFreeAlgAux(lpGeneralCallBacks,lpAlgScanInfo);

			return(ENGSTATUS_ERROR);
		}

		// Advance past sig data

		wPoolOffset += wSigLen;
	}

	// insert our last allocated pool into the pool information structure

	lpAlgScanInfo->lpPoolInfoStruct->AlgPoolArray[
		lpAlgScanInfo->lpPoolInfoStruct->wNumAlgPools].
		wBuffSize = wPoolOffset;

	lpAlgScanInfo->lpPoolInfoStruct->AlgPoolArray[
		lpAlgScanInfo->lpPoolInfoStruct->wNumAlgPools++].
		lpbyAlgPoolPtr = lpbyCurPool;

	// Generate the hash tables from our loaded data. EngAlgHash
	// will fill in the contents of the specified hash table and update
	// wCount, wCurPool and wPoolOffset appropriately (advancing over all
	// hashed signatures)

	wCurPool = 0;
	wPoolOffset = 0;

	wCount = wNumAlgs;

	// Do four byte hash

	EngAlgHash(lpAlgScanInfo->lpPoolInfoStruct,
			   4,
			   lpAlgScanInfo->wFourByteHashSize,
			   lpAlgScanInfo->lpFourByteTable,
			   &wCount,
			   &wCurPool,
			   &wPoolOffset);

	// Do two byte hash

	EngAlgHash(lpAlgScanInfo->lpPoolInfoStruct,
			   2,
			   lpAlgScanInfo->wTwoByteHashSize,
			   lpAlgScanInfo->lpTwoByteTable,
			   &wCount,
			   &wCurPool,
			   &wPoolOffset);

	// Do one byte hash

	EngAlgHash(lpAlgScanInfo->lpPoolInfoStruct,
			   1,
			   lpAlgScanInfo->wOneByteHashSize,
			   lpAlgScanInfo->lpOneByteTable,
			   &wCount,
			   &wCurPool,
			   &wPoolOffset);

	// set up the no-hash signatures

    lpAlgScanInfo->stNoHashInfo.byPoolNum = (BYTE)wCurPool;
	lpAlgScanInfo->stNoHashInfo.wPoolOffset = wPoolOffset;
	lpAlgScanInfo->stNoHashInfo.wNumSigs = wCount;

	return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EngLoadAlgScanData()
//
// Parameters:
//  lpDataFileCallBacks     Callbacks for accessing the data files
//  lpGeneralCallBacks      General callbacks for memory freeing and allocation
//  dwFileSection           The file section number where the alg scanning
//                          data is located.
//  lpAlgScanInfo           This is a pointer to a structure that will be
//                          filled with the appropriate data after the alg
//                          sigs are loaded. It points to all of the pools
//                          used to store these alg signatures as well as the
//                          hash tables associated with these signatures
//
// Description:
//  This function locates the specified alg section (dwFileSection) in the
//  proper data file (VIRSCAN4.DAT) and retrieves the scanning data.  This
//  data is loaded into a number of pools.  No signatures are split across
//  pools.  The signatures are also hashed.
//
// Returns:
//  ENGSTATUS_OK            On successful load (memory allocated and ready)
//  ENGSTATUS_ERROR         On unsuccessful load (all memory is freed)
//
//********************************************************************


ENGSTATUS EngLoadAlgScanData
(
    LPDATAFILECALLBACKS lpDataFileCallBacks,
    LPGENERALCALLBACKS  lpGeneralCallBacks,
    DWORD               dwFileSection,
    LPALG_SCAN_INFO     lpAlgScanInfo)
{
	LPVOID              lpvFileInfo;
	DATA_FILE_TABLE_T   stSectionInfo;
	BOOL                bSectionFound;
	ENGSTATUS           engStatus;

	// open our data file...

    if (EngOpenDataFile(lpDataFileCallBacks,
					   DATA_FILE_ID_VIRSCAN4DAT,
                       FILE_OPEN_READ_ONLY,
					   &lpvFileInfo) == DFSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

	// locate the proper section... (stSectionInfo is endian neutral)

	if (EngLookUp(lpDataFileCallBacks,
				 lpvFileInfo,
				 dwFileSection,
				 &stSectionInfo,
				 &bSectionFound) == DFSTATUS_ERROR)
	{
		// close and exit on error

        EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

		return(ENGSTATUS_ERROR);
	}

	if (bSectionFound == FALSE)
	{
		// specified section was not present in the file...
		// close and exit

		EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

		return(ENGSTATUS_ERROR);
	}

	// load those signatures

	engStatus = EngLoadAlgAux(lpDataFileCallBacks,
							  lpGeneralCallBacks,
							  &stSectionInfo,
							  lpvFileInfo,
							  lpAlgScanInfo);
	// close that data file

	if (EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo) == DFSTATUS_ERROR)
	{
		// error closing the data file? if we were able to load our signature
		// data then free it...

        if (engStatus != ENGSTATUS_ERROR)
		{
			EngFreeAlgAux(lpGeneralCallBacks, lpAlgScanInfo);
		}

		return(ENGSTATUS_ERROR);
    }

    return(engStatus);
}


