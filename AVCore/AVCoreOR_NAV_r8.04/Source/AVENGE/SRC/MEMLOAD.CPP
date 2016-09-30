// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/MEMLOAD.CPv   1.1   08 Jul 1997 17:49:22   MKEATIN  $
//
// Description:
//  Functions for loading signatures in form scannable by memory
//  scanning subengine.
//
// Contains:
//  EngMemoryLoad()
//  EngMemoryUnload()
//  EngMemorySigShellSort()
//  EngMemoryFreeSigGroup()
//  EngMemoryAllocSigGroup()
//  EngMemoryReadSigGroup()
//
// See Also:
//  booload.c, memscan.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/MEMLOAD.CPv  $
// 
//    Rev 1.1   08 Jul 1997 17:49:22   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.0   18 Apr 1997 13:37:36   MKEATIN
// Initial revision.
// 
//    Rev 1.2   17 May 1996 14:16:02   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
// 
//    Rev 1.1   16 May 1996 14:12:50   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.0   13 May 1996 16:29:06   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

//*************************************************************************
//
// Function:
//  EngMemorySigCmp()
//
// Parameters:
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//	wIndexA			Index of first signature
//	wIndexB			Index of second signature
//	wSigLen			Length of signature
//
// Description:
//	Compares the signatures at the two indices.
//
//  The function correctly crosses pool boundaries.
//
// Returns:
//  < 0             Signature A < Signature B
//  = 0             Signature A = Signature B
//  > 0             Signature A > Signature B
//
//*************************************************************************

int EngMemorySigCmp
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wIndexA,
    WORD            	wIndexB,
    WORD            	wSigLen
)
{
    WORD                wPoolA, wPoolB;
    LPBYTE              lpbySigA, lpbySigB;

    // Find first sig

    wPoolA = wIndexA >> MEM_SIGS_POOL_SHIFT;
    wIndexA &= MEM_SIGS_POOL_ENTRY_MASK;

    // Find second sig

    wPoolB = wIndexB >> MEM_SIGS_POOL_SHIFT;
    wIndexB &= MEM_SIGS_POOL_ENTRY_MASK;

    // Return comparison result

    lpbySigA = lpMemSigGroup->stMemSigPool[wPoolA].lpMemSigs[wIndexA].bySig;
    lpbySigB = lpMemSigGroup->stMemSigPool[wPoolB].lpMemSigs[wIndexB].bySig;

    while (wSigLen-- != 0)
    {
        if (*lpbySigA != *lpbySigB)
        {
            if (*lpbySigA < *lpbySigB)
                return(-1);

            return(1);
        }

        ++lpbySigA;
        ++lpbySigB;
    }

    return(0);
}


//*************************************************************************
//
// Function:
//  EngMemorySigSwap()
//
// Parameters:
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//	wIndexA			Index of first signature
//	wIndexB			Index of second signature
//
// Description:
//	Swaps the signatures at the two indices, including the virus
//	indices.
//
//  The function correctly crosses pool boundaries.
//
// Returns:
//  nothing
//
//*************************************************************************

void EngMemorySigSwap
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wIndexA,
    WORD            	wIndexB
)
{
    WORD                wPoolA, wPoolB;
    MEMSIG_T        	sTmpMemSig;

    // Find first sig

    wPoolA = wIndexA >> MEM_SIGS_POOL_SHIFT;
    wIndexA &= MEM_SIGS_POOL_ENTRY_MASK;

    // Find second sig

    wPoolB = wIndexB >> MEM_SIGS_POOL_SHIFT;
    wIndexB &= MEM_SIGS_POOL_ENTRY_MASK;

    // Swap sigs

    sTmpMemSig = lpMemSigGroup->stMemSigPool[wPoolA].lpMemSigs[wIndexA];
    lpMemSigGroup->stMemSigPool[wPoolA].lpMemSigs[wIndexA] =
        lpMemSigGroup->stMemSigPool[wPoolB].lpMemSigs[wIndexB];
    lpMemSigGroup->stMemSigPool[wPoolB].lpMemSigs[wIndexB] = sTmpMemSig;
}


//*************************************************************************
//
// Function:
//  EngMemorySigShellSort()
//
// Parameters:
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//	wSigLen			Length of signature
//
// Description:
//	Sorts the signatures of the group using Shell's sorting
//	algorithm.
//
// Returns:
//  nothing
//
//*************************************************************************

void EngMemorySigShellSort
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wSigLen
)
{
    WORD    			wGap;
    long    			i, j;

    // Shell's sorting algorithm

    for (wGap=lpMemSigGroup->wNumSigs/2;wGap>0;wGap/=2)
        for (i=wGap;i<(int)lpMemSigGroup->wNumSigs;i++)
        {
            j = i - wGap;
            while (j>=0 &&
                EngMemorySigCmp(lpMemSigGroup,
                    (WORD)j,(WORD)(j+wGap),wSigLen) > 0)
			{
                EngMemorySigSwap(lpMemSigGroup,(WORD)j,(WORD)(j+wGap));
                j -= wGap;
            }
        }
}


//*************************************************************************
//
// Function:
//  EngMemoryFreeSigGroup()
//
// Parameters:
//  hGEng           Handle to global engine context
//  lpMemSigGroup   Pointer to MEMSIGGROUP_T structure.
//
// Description:
//  Frees the memory allocated for a memory signature group.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngMemoryFreeSigGroup
(
    HGENG           hGEng,
    LPMEMSIGGROUP   lpMemSigGroup
)
{
	if (lpMemSigGroup->wNumSigPools > 0)
    {
        WORD j;

		for (j=0;j<lpMemSigGroup->wNumSigPools;j++)
        {
            hGEng->lpGeneralCallBacks->
                MemoryFree(lpMemSigGroup->stMemSigPool[j].lpMemSigs);
            lpMemSigGroup->stMemSigPool[j].wNumSigs = 0;
            lpMemSigGroup->stMemSigPool[j].lpMemSigs = NULL;
        }
    }

    if (lpMemSigGroup->lpbyQuickWordEliminate != NULL)
    {
        hGEng->lpGeneralCallBacks->
            MemoryFree(lpMemSigGroup->lpbyQuickWordEliminate);
        lpMemSigGroup->lpbyQuickWordEliminate = NULL;
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngMemoryFreeInfo()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Frees memory allocated for memory signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngMemoryFreeInfo
(
    HGENG       hGEng
)
{
    // Free CD memory

	EngMemoryFreeSigGroup(hGEng,&(hGEng->lpMemoryScanInfo->stCD));

	// Free ALL memory

	EngMemoryFreeSigGroup(hGEng,&(hGEng->lpMemoryScanInfo->stALL));

	// Free TOM memory

	EngMemoryFreeSigGroup(hGEng,&(hGEng->lpMemoryScanInfo->stTOM));

    // Free MEMORY_SCAN_INFO_T structure

    hGEng->lpGeneralCallBacks->MemoryFree(hGEng->lpMemoryScanInfo);
    hGEng->lpMemoryScanInfo = NULL;

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngMemoryAllocSigGroup()
//
// Parameters:
//  hGEng           Handle to global engine context
//  wNumSigs        Number of signatures to allocate for
//  lpMemSigGroup   Pointer to MEMSIGGROUP_T structure
//
// Description:
//  Allocates enough memory for wNumSigs memory signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngMemoryAllocSigGroup
(
	HGENG           hGEng,
	WORD            wNumSigs,
	LPMEMSIGGROUP   lpMemSigGroup
)
{
	WORD                wNumSigPools, wSigsLastPool;
	WORD    			i, j;

	// Calculate number of pools

	lpMemSigGroup->wNumSigs = wNumSigs;
	wNumSigPools = wNumSigs / MAX_MEM_SIGS_PER_POOL;
	if (wNumSigs % MAX_MEM_SIGS_PER_POOL != 0)
		++wNumSigPools;

	lpMemSigGroup->wNumSigPools = wNumSigPools;

	if (wNumSigPools == 0)
		return(ENGSTATUS_OK);

	// Calculate sigs in last block

	wSigsLastPool = wNumSigs % MAX_MEM_SIGS_PER_POOL;
	if (wSigsLastPool == 0)
		wSigsLastPool = MAX_MEM_SIGS_PER_POOL;

	// Allocate first wNumSigPools - 1

	for (i=0;i<(wNumSigPools-1);i++)
	{
		lpMemSigGroup->stMemSigPool[i].wNumSigs = MAX_MEM_SIGS_PER_POOL;

		if (hGEng->lpGeneralCallBacks->
				MemoryAlloc(MAX_MEM_SIGS_PER_POOL * sizeof(MEMSIG_T),
							MEM_FIXME,
							(LPLPVOID)&(lpMemSigGroup->stMemSigPool[i].
								lpMemSigs))
			== CBSTATUS_ERROR)
		{
			lpMemSigGroup->wNumSigs = 0;
			lpMemSigGroup->wNumSigPools = 0;
			lpMemSigGroup->stMemSigPool[i].wNumSigs = 0;
			for (j=0;j<i;j++)
			{
				hGEng->lpGeneralCallBacks->
					MemoryFree(lpMemSigGroup->stMemSigPool[j].lpMemSigs);
				lpMemSigGroup->stMemSigPool[j].wNumSigs = 0;
				lpMemSigGroup->stMemSigPool[j].lpMemSigs = NULL;
			}

			return(ENGSTATUS_ERROR);
		}
	}

	// Allocate last block

	lpMemSigGroup->stMemSigPool[wNumSigPools-1].wNumSigs = wSigsLastPool;

	if (hGEng->lpGeneralCallBacks->
			MemoryAlloc(wSigsLastPool * sizeof(MEMSIG_T),
						MEM_FIXME,
						(LPLPVOID)&(lpMemSigGroup->
							stMemSigPool[wNumSigPools-1].
							lpMemSigs))
		== CBSTATUS_ERROR)
	{
		lpMemSigGroup->wNumSigs = 0;
		lpMemSigGroup->wNumSigPools = 0;
		lpMemSigGroup->stMemSigPool[wNumSigPools-1].wNumSigs = 0;
		for (j=0;j<(wNumSigPools-1);j++)
		{
			hGEng->lpGeneralCallBacks->
                MemoryFree(lpMemSigGroup->stMemSigPool[j].lpMemSigs);
            lpMemSigGroup->stMemSigPool[j].wNumSigs = 0;
            lpMemSigGroup->stMemSigPool[j].lpMemSigs = NULL;
        }

        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngMemoryReadSigGroup()
//
// Parameters:
//  hGEng                   Handle to global engine context
//  lpvFileInfo             File info for data file
//  wMemSigSize             Size of each memory signature in data file
//  bQuickWordEliminate     Whether to allocate quick word eliminate mem
//  lpMemSigGroup           Pointer to MEMSIGGROUP_T structure
//
// Description:
//  Reads a memory signature group.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngMemoryReadSigGroup
(
    HGENG           hGEng,
    LPVOID          lpvFileInfo,
    WORD            wMemSigSize,
    BOOL            bQuickWordEliminate,
    LPMEMSIGGROUP   lpMemSigGroup
)
{
    LPMEMSIG        lpMemSig;
    DWORD           dwBytesRead;
    WORD            wNumSigs, w;
    WORD            wPool, wEntry;
    WORD            wByte, wBit, wWord;

    ///////////////////////////////////////////////////////////////////
    //
    // Read number of signatures
    //
    ///////////////////////////////////////////////////////////////////

    // Get count of sigs

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     &wNumSigs,
                     sizeof(wNumSigs),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(wNumSigs))
    {
        // Read of # of TOMs failed

        return(ENGSTATUS_ERROR);
    }

    // Endian enable wNumSigs

    wNumSigs = WENDIAN(wNumSigs);

    // Initialize quick eliminate

    for (w=0;w<256;w++)
        lpMemSigGroup->byQuickEliminate[w] = 0;


    ///////////////////////////////////////////////////////////////////
    //
    // Allocate memory for quick word eliminate
    //
    ///////////////////////////////////////////////////////////////////

    if (bQuickWordEliminate == TRUE)
    {
        // Allocate quick eliminate and initialize to zeroes

        if (hGEng->lpGeneralCallBacks->
                MemoryAlloc(4096 * sizeof(BYTE),
                            MEM_FIXME,
							(LPLPVOID)&(lpMemSigGroup->
								lpbyQuickWordEliminate))
            == CBSTATUS_ERROR)
        {
            // Allocation of memory failed

            return(ENGSTATUS_ERROR);
        }

        for (w=0;w<4096;w++)
            lpMemSigGroup->lpbyQuickWordEliminate[w] = 0;
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Allocate memory for signatures
    //
    ///////////////////////////////////////////////////////////////////

    if (EngMemoryAllocSigGroup(hGEng,
                               wNumSigs,
                               lpMemSigGroup) == ENGSTATUS_ERROR)
    {
        // Allocation of memory failed

        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Read signatures
    //
    ///////////////////////////////////////////////////////////////////

    wPool = wEntry = 0;
    lpMemSig = lpMemSigGroup->stMemSigPool[0].lpMemSigs;

    for (w=0;w<wNumSigs;w++)
    {
        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileRead(lpvFileInfo,
                         lpMemSig,
                         wMemSigSize,
                         &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != wMemSigSize)
        {
            // Read of sig failed

            return(ENGSTATUS_ERROR);
        }

        // Endian enable wVirusRecordIndex field

        lpMemSig->wVirusRecordIndex =
            WENDIAN(lpMemSig->wVirusRecordIndex);

        // Update quick eliminate

        lpMemSigGroup->byQuickEliminate[(BYTE)(lpMemSig->bySig[0]-1)] = 1;

        if (bQuickWordEliminate == TRUE)
        {
            wWord = (((BYTE)(lpMemSig->bySig[1] - 1)) +
                    (((WORD)((BYTE)(lpMemSig->bySig[2] - 1))) << 8)) &
                    0x7FFF;

            wByte = wWord >> 3;
            wBit = wWord & 0x7;

            lpMemSigGroup->lpbyQuickWordEliminate[wByte] |=
                1 << wBit;
        }

        if (++wEntry >= MAX_MEM_SIGS_PER_POOL)
        {
            wEntry = 0;
            lpMemSig = lpMemSigGroup->stMemSigPool[++wPool].lpMemSigs;
        }
        else
            ++lpMemSig;
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngMemoryLoadAux()
//
// Parameters:
//  hGEng           Handle to global engine context
//  lpvFileInfo     Pointer to file info for VIRSCAN4.DAT
//
// Description:
//  Looks up SECTION_ID_MEM_SIG in VIRSCAN4.DAT and loads the memory
//  signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngMemoryLoadAux
(
    HGENG       hGEng,
    LPVOID      lpvFileInfo
)
{
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset;

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_MEM_SIG,
                 &stDataFileTableEntry,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // Error during lookup and/or section not found

        return(ENGSTATUS_ERROR);
    }

    // Seek to beginning of memory signatures section

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileSeek(lpvFileInfo,
                     stDataFileTableEntry.dwStartOffset,
                     SEEK_SET,
                     &dwOffset) == CBSTATUS_ERROR ||
        dwOffset != stDataFileTableEntry.dwStartOffset)
    {
        // Seek failed

        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Allocate MEMORY_SCAN_INFO_T structure memory
    //
    ///////////////////////////////////////////////////////////////////

    if (hGEng->lpGeneralCallBacks->
            MemoryAlloc(sizeof(MEMORY_SCAN_INFO_T),
                        MEM_FIXME,
						(LPLPVOID)&(hGEng->lpMemoryScanInfo))
		== CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // Initialize fields

    hGEng->lpMemoryScanInfo->stCD.lpbyQuickWordEliminate = NULL;
    hGEng->lpMemoryScanInfo->stCD.wNumSigPools = 0;

    hGEng->lpMemoryScanInfo->stALL.lpbyQuickWordEliminate = NULL;
    hGEng->lpMemoryScanInfo->stALL.wNumSigPools = 0;

    hGEng->lpMemoryScanInfo->stTOM.lpbyQuickWordEliminate = NULL;
    hGEng->lpMemoryScanInfo->stTOM.wNumSigPools = 0;


    ///////////////////////////////////////////////////////////////////
    //
    // Read CD signatures
    //
    ///////////////////////////////////////////////////////////////////

    if (EngMemoryReadSigGroup(hGEng,
                              lpvFileInfo,
                              sizeof(MEM_CD_SIG_T),
                              FALSE,
                              &(hGEng->lpMemoryScanInfo->stCD))
        == ENGSTATUS_ERROR)
    {
        EngMemoryFreeInfo(hGEng);
        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Read ALL signatures
    //
    ///////////////////////////////////////////////////////////////////

    if (EngMemoryReadSigGroup(hGEng,
                              lpvFileInfo,
                              sizeof(MEM_ALL_SIG_T),
                              TRUE,
                              &(hGEng->lpMemoryScanInfo->stALL))
        == ENGSTATUS_ERROR)
    {
        EngMemoryFreeInfo(hGEng);
        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Read TOM signatures
    //
    ///////////////////////////////////////////////////////////////////

    if (EngMemoryReadSigGroup(hGEng,
                              lpvFileInfo,
                              sizeof(MEM_ALL_SIG_T),
                              TRUE,
                              &(hGEng->lpMemoryScanInfo->stTOM))
        == ENGSTATUS_ERROR)
    {
		EngMemoryFreeInfo(hGEng);
        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Sort signatures
    //
    ///////////////////////////////////////////////////////////////////

	EngMemorySigShellSort(&(hGEng->lpMemoryScanInfo->stCD),
		MEM_SIG_LEN_CD);

	EngMemorySigShellSort(&(hGEng->lpMemoryScanInfo->stALL),
		MEM_SIG_LEN_ALL);

	EngMemorySigShellSort(&(hGEng->lpMemoryScanInfo->stTOM),
		MEM_SIG_LEN_ALL);

	return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngMemoryUnload()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Frees memory allocated for memory signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngMemoryUnload
(
	HGENG       hGEng
)
{
	return EngMemoryFreeInfo(hGEng);
}


//*************************************************************************
//
// Function:
//  EngMemoryLoad()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Opens VIRSCAN4.DAT and calls EngMemoryLoadAux() to load the memory
//  signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngMemoryLoad
(
    HGENG       hGEng
)
{
    LPVOID      lpvFileInfo;
    ENGSTATUS   engStatus;

    // open our data file...

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
					   DATA_FILE_ID_VIRSCAN4DAT,
                       FILE_OPEN_READ_ONLY,
                       &lpvFileInfo) == DFSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    engStatus = EngMemoryLoadAux(hGEng,lpvFileInfo);

    if (EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvFileInfo) ==
        DFSTATUS_ERROR)
    {
        // error closing the data file? if we were able to load our signature
        // data then free it...

        if (engStatus != DFSTATUS_ERROR)
        {
            EngMemoryUnload(hGEng);
        }

        return(ENGSTATUS_ERROR);
    }

    return(engStatus);
}

