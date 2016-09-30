#include "scanmem.h"
#include "readsig.h"

#include <mem.h>
#include <stdlib.h>

#define SIZE_FIXED      64

extern unsigned _stklen = 22000u;

typedef struct
{
    WORD    wNumMEMs;        // Total number of memory signatures
    WORD    wNumCDs;         // # of CD signatures
    WORD    wNumTOMs;        // # of TOM signatures
    WORD    wNumALLs;        // # of ALL signatures
    WORD    wFixedIDs[SIZE_FIXED];  // N30 Record Numbers
} INSCANM_HEADER_T, FAR *LPINSCANM_HEADER;

BOOL VLScanMemCountMemSigs
(
    LPINSCANM_HEADER	lpHeader,
    HFILE           	hFile
);

LPMEMSIGGROUP VLScanMemAllocSigGroup
(
    WORD 				wNumSigs,
    LPMEMSIGGROUP 		lpMemSigGroup
);

void VLScanMemFreeSigGroup
(
    LPMEMSIGGROUP 		lpMemSigGroup
);

void VLScanMemFree
(
    LPMEMSIGS   		lpMemSigs
);

void VLScanMemSigShellSort
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wSigLen
);


//********************************************************************
//
// Function:
//  VLScanMemGetScanData()
//
// Parameters:
//	hFile			Handle to open INSCANM data file
//
// Description:
//	Allocates memory for INSCANM data and reads the data in.
//
// Returns:
//  NULL        	On failure
//  LPMEMSIGS   	On success
//
//********************************************************************

LPMEMSIGS VLScanMemGetScanData
(
    HFILE               hFile
)
{
    INSCANM_HEADER_T	stHeader;
    WORD    			i, j, k;
    WORD        		wCDBlk, wCDBlkEntry;
    WORD        		wALLBlk, wALLBlkEntry;
    WORD        		wTOMBlk, wTOMBlkEntry;
    LPBYTE      		lpbySig;
    MEMSIG_T    		stMemSig;
    WORD 				wByte, wBit, wWord;
    LPMEMSIGS 			lpMemSigs;

    ///////////////////////////////////////////////////////////////////
    //
    //  Read header
    //
    ///////////////////////////////////////////////////////////////////

    if (fread(&stHeader,sizeof(stHeader),1,hFile) != 1)
    {
        printf("fread() of header failed!\n");
        return (NULL);
    }

    ///////////////////////////////////////////////////////////////////
    //
    //  Read through file counting number of CDs, TOMs, and ALLs
    //
    ///////////////////////////////////////////////////////////////////

    if (VLScanMemCountMemSigs(&stHeader,hFile) == FALSE)
    {
        return (NULL);
    }

    ///////////////////////////////////////////////////////////////////
    //
    //  Allocate and initialize memory
    //
    ///////////////////////////////////////////////////////////////////

    // Allocate structure memory

    lpMemSigs = (LPMEMSIGS)calloc(1,sizeof(MEMSIGS_T));
    if (lpMemSigs == NULL)
    {
        return (NULL);
    }

    // Allocate quick eliminate and initialize to zeroes

    lpMemSigs->stALL.lpbyQuickWordEliminate = (LPBYTE)calloc(4096,1);
    if (lpMemSigs->stALL.lpbyQuickWordEliminate == NULL)
    {
        VLScanMemFree(lpMemSigs);
        return (NULL);
    }

    lpMemSigs->stTOM.lpbyQuickWordEliminate = (LPBYTE)calloc(4096,1);
    if (lpMemSigs->stTOM.lpbyQuickWordEliminate == NULL)
    {
        VLScanMemFree(lpMemSigs);
        return (NULL);
    }

    for (i=0;i<256;i++)
    {
	    lpMemSigs->stCD.byQuickEliminate[i] = 0;
	    lpMemSigs->stALL.byQuickEliminate[i] = 0;
	    lpMemSigs->stTOM.byQuickEliminate[i] = 0;
	}

    // Allocate memory for CDs

    if (VLScanMemAllocSigGroup(stHeader.wNumCDs,&lpMemSigs->stCD) == NULL)
    {
        VLScanMemFree(lpMemSigs);
        return (NULL);
    }

    // Allocate memory for TOMs

    if (VLScanMemAllocSigGroup(stHeader.wNumTOMs,&lpMemSigs->stTOM) == NULL)
    {
        VLScanMemFree(lpMemSigs);
        return (NULL);
    }

    // Allocate memory for ALLs

    if (VLScanMemAllocSigGroup(stHeader.wNumALLs,&lpMemSigs->stALL) == NULL)
    {
        VLScanMemFree(lpMemSigs);
        return (NULL);
    }

    ///////////////////////////////////////////////////////////////////
    //
    //  Read data
    //
    ///////////////////////////////////////////////////////////////////

    // Read data

    wCDBlk = wCDBlkEntry = 0;
    wALLBlk = wALLBlkEntry = 0;
    wTOMBlk = wTOMBlkEntry = 0;

    for (i=0;i<stHeader.wNumMEMs;i++)
    {
        if (fread(&stMemSig,sizeof(MEMSIG_T),1,hFile) != 1)
        {
            VLScanMemFree(lpMemSigs);
            return (NULL);
        }

        if (stMemSig.bySig[0] == 0)
        {
            switch (stMemSig.bySig[1])
            {
                case MEMORY_413:
                    lpMemSigs->stTOM.stMemSigBlk[wTOMBlk].
                        lpMemSigs[wTOMBlkEntry].wVirusIndex =
						stMemSig.wVirusIndex;

                    for (j=0;j<MEM_SIG_LEN_ALL;j++)
                        lpMemSigs->stTOM.stMemSigBlk[wTOMBlk].
                            lpMemSigs[wTOMBlkEntry].bySig[j] =
                            stMemSig.bySig[j+2];

					// Update quick eliminate

	    			lpMemSigs->stTOM.
						byQuickEliminate[(BYTE)(stMemSig.bySig[2]-1)] = 1;

			        wWord = (((BYTE)(stMemSig.bySig[3] - 1)) +
							(((WORD)((BYTE)(stMemSig.bySig[4] - 1))) << 8)) &
							0x7FFF;

			       	wByte = wWord >> 3;
			       	wBit = wWord & 0x7;

        			lpMemSigs->stTOM.lpbyQuickWordEliminate[wByte] |=
						1 << wBit;

					// Update index

                    if (++wTOMBlkEntry >= MAX_MEM_SIGS_PER_BLK)
                    {
                        ++wTOMBlk;
                        wTOMBlkEntry = 0;
                    }

                    break;

                case MEMORY_ALL:
                    lpMemSigs->stALL.stMemSigBlk[wALLBlk].
                        lpMemSigs[wALLBlkEntry].wVirusIndex =
						stMemSig.wVirusIndex;

                    for (j=0;j<MEM_SIG_LEN_ALL;j++)
                        lpMemSigs->stALL.stMemSigBlk[wALLBlk].
                            lpMemSigs[wALLBlkEntry].bySig[j] =
                            stMemSig.bySig[j+2];

					// Update quick eliminate

	    			lpMemSigs->stALL.
						byQuickEliminate[(BYTE)(stMemSig.bySig[2]-1)] = 1;

			        wWord = (((BYTE)(stMemSig.bySig[3] - 1)) +
							(((WORD)((BYTE)(stMemSig.bySig[4] - 1))) << 8)) &
							0x7FFF;

			       	wByte = wWord >> 3;
			       	wBit = wWord & 0x7;

        			lpMemSigs->stALL.lpbyQuickWordEliminate[wByte] |=
						1 << wBit;

					// Update index

                    if (++wALLBlkEntry >= MAX_MEM_SIGS_PER_BLK)
                    {
                        ++wALLBlk;
                        wALLBlkEntry = 0;
                    }

                    break;

                default:
                    lpMemSigs->stCD.stMemSigBlk[wCDBlk].
                        lpMemSigs[wCDBlkEntry++] = stMemSig;

					// Update quick eliminate

	    			lpMemSigs->stCD.
						byQuickEliminate[(BYTE)(stMemSig.bySig[0]-1)] = 1;

					// Update index

                    if (wCDBlkEntry >= MAX_MEM_SIGS_PER_BLK)
                    {
                        ++wCDBlk;
                        wCDBlkEntry = 0;
                    }

                    break;
            }
        }
        else
        {
            lpMemSigs->stCD.stMemSigBlk[wCDBlk].
            lpMemSigs[wCDBlkEntry++] = stMemSig;

			// Update quick eliminate

   			lpMemSigs->stCD.
				byQuickEliminate[(BYTE)(stMemSig.bySig[0]-1)] = 1;

			// Update index

            if (wCDBlkEntry >= MAX_MEM_SIGS_PER_BLK)
            {
                ++wCDBlk;
                wCDBlkEntry = 0;
            }

        }
    }

    // Now sort

    VLScanMemSigShellSort(&lpMemSigs->stCD,MEM_SIG_LEN_CD);
    VLScanMemSigShellSort(&lpMemSigs->stALL,MEM_SIG_LEN_ALL);
    VLScanMemSigShellSort(&lpMemSigs->stTOM,MEM_SIG_LEN_ALL);

    return (lpMemSigs);
}


//********************************************************************
//
// Function:
//  VLScanMemCountMemSigs()
//
// Parameters:
//	lpHeader		Pointer to INSCANM header to store counts
//	hFile			Handle to open INSCANM data file
//
// Returns:
//  FALSE			On failure
//  TRUE			On success
//
// Description:
//	Reads through INSCAM data file, counting number of CD sigs,
//	TOM sigs, and ALL sigs, storing the counts into the
//	INSCANM header structure.  The function reads lpHeader->wNumMEMs
//	signatures.
//
//	The function assumes that the file pointer points to the
//	first signature in the INSCANM file.  After reading all
//	signatures, the function returns the file pointer to point
//	to the first signature.
//
//********************************************************************

BOOL VLScanMemCountMemSigs
(
    LPINSCANM_HEADER	lpHeader,
    HFILE           	hFile
)
{
    WORD            	i;
    DWORD           	dwStartOffset;
    MEMSIG_T        	stMemSig;

    dwStartOffset = ftell(hFile);

    lpHeader->wNumCDs = lpHeader->wNumALLs = lpHeader->wNumTOMs = 0;

    for (i=0;i<lpHeader->wNumMEMs;i++)
    {
        if (fread(&stMemSig,sizeof(MEMSIG_T),1,hFile) != 1)
            return (FALSE);

        if (stMemSig.bySig[0] == 0)
        {
            switch (stMemSig.bySig[1])
            {
                case MEMORY_413:
                    lpHeader->wNumTOMs++;
                    break;
                case MEMORY_ALL:
                    lpHeader->wNumALLs++;
                    break;
                default:
                    lpHeader->wNumCDs++;
                    break;
            }
        }
        else
            lpHeader->wNumCDs++;
    }

    // Return to beginning of sigs

    if (fseek(hFile,dwStartOffset,SEEK_SET) == (DWORD)-1)
        return (FALSE);

    return (TRUE);
}


//********************************************************************
//
// Function:
//  VLScanMemAllocSigGroup()
//
// Parameters:
//	wNumSigs		Number of signatures to allocate memory for
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//
// Returns:
//  NULL			On failure
//  lpMemSigGroup	On success
//
// Description:
//	Determines the number of blocks of memory necessary by
//	calculating ceiling(wNumSigs/MAX_MEM_SIGS_PER_BLK).
//  All blocks except the last block have room for
//	MAX_MEM_SIGS_PER_BLK signatures, unless the division
//	is even.
//
//	The function then allocates the calculated number of memory
//	blocks.
//
//********************************************************************

LPMEMSIGGROUP VLScanMemAllocSigGroup
(
    WORD 				wNumSigs,
    LPMEMSIGGROUP 		lpMemSigGroup
)
{
    WORD            	wNumBlks, wSigsLastBlk;
    WORD    			i, j;

    // Calculate number of blocks

    lpMemSigGroup->wNumSigs = wNumSigs;
    wNumBlks = wNumSigs / MAX_MEM_SIGS_PER_BLK;
    if (wNumSigs % MAX_MEM_SIGS_PER_BLK != 0)
        ++wNumBlks;

    lpMemSigGroup->wNumSigBlks = wNumBlks;

    if (wNumBlks == 0)
        return (lpMemSigGroup);

    // Calculate sigs in last block

    wSigsLastBlk = wNumSigs % MAX_MEM_SIGS_PER_BLK;
    if (wSigsLastBlk == 0)
        wSigsLastBlk = MAX_MEM_SIGS_PER_BLK;

    // Allocate first wNumBlks - 1

    for (i=0;i<(wNumBlks-1);i++)
    {
    	WORD wTemp;

        wTemp = MAX_MEM_SIGS_PER_BLK * sizeof(MEMSIG_T);

        lpMemSigGroup->stMemSigBlk[i].wNumSigs = MAX_MEM_SIGS_PER_BLK;
        lpMemSigGroup->stMemSigBlk[i].lpMemSigs =
            (LPMEMSIG)malloc(wTemp);

        if (lpMemSigGroup->stMemSigBlk[i].lpMemSigs == NULL)
        {
		    lpMemSigGroup->wNumSigs = 0;
            lpMemSigGroup->wNumSigBlks = 0;
            lpMemSigGroup->stMemSigBlk[i].wNumSigs = 0;
            for (j=0;j<i;j++)
            {
                free(lpMemSigGroup->stMemSigBlk[j].lpMemSigs);
                lpMemSigGroup->stMemSigBlk[j].wNumSigs = 0;
                lpMemSigGroup->stMemSigBlk[j].lpMemSigs = NULL;
            }

            return (NULL);
        }
    }

    // Allocate last block

    lpMemSigGroup->stMemSigBlk[wNumBlks-1].wNumSigs = wSigsLastBlk;
    lpMemSigGroup->stMemSigBlk[wNumBlks-1].lpMemSigs =
        (LPMEMSIG)malloc(wSigsLastBlk * sizeof(MEMSIG_T));

    if (lpMemSigGroup->stMemSigBlk[wNumBlks-1].lpMemSigs == NULL)
    {
	    lpMemSigGroup->wNumSigs = 0;
        lpMemSigGroup->wNumSigBlks = 0;
        lpMemSigGroup->stMemSigBlk[wNumBlks-1].wNumSigs = 0;
        for (j=0;j<(wNumBlks-1);j++)
        {
            free(lpMemSigGroup->stMemSigBlk[j].lpMemSigs);
            lpMemSigGroup->stMemSigBlk[j].wNumSigs = 0;
            lpMemSigGroup->stMemSigBlk[j].lpMemSigs = NULL;
        }

        return (NULL);
    }

    return (lpMemSigGroup);
}


//********************************************************************
//
// Function:
//  VLScanMemFreeSigGroup()
//
// Parameters:
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//
// Returns:
//	nothing
//
// Description:
//	Frees all the memory blocks allocated for the signature group
//	pointed to by lpMemSigGroup and sets the corresponding wNumSigs
//	fields to zero and pointer fields to NULL.
//
//	The function also sets the signature group's wNumSigs field
//	to zero.
//
//********************************************************************

void VLScanMemFreeSigGroup
(
    LPMEMSIGGROUP 		lpMemSigGroup
)
{
    WORD    			i;

    for (i=0;i<lpMemSigGroup->wNumSigBlks;i++)
    {
        lpMemSigGroup->stMemSigBlk[i].wNumSigs = 0;
        if (lpMemSigGroup->stMemSigBlk[i].lpMemSigs != NULL)
        {
            free(lpMemSigGroup->stMemSigBlk[i].lpMemSigs);
            lpMemSigGroup->stMemSigBlk[i].lpMemSigs = NULL;
        }
    }

    lpMemSigGroup->wNumSigs = 0;
    lpMemSigGroup->wNumSigBlks = 0;
}


//********************************************************************
//
// Function:
//  VLScanMemFree()
//
// Parameters:
//	lpMemSigs		Pointer to MEMSIGS_T structure.
//
// Returns:
//	nothing
//
// Description:
//	Frees all the memory allocated for signatures and quick
//	elimination and then frees lpMemSigs.
//
//********************************************************************

void VLScanMemFree
(
    LPMEMSIGS   		lpMemSigs
)
{
	// Free sig groups

    VLScanMemFreeSigGroup(&lpMemSigs->stCD);
    VLScanMemFreeSigGroup(&lpMemSigs->stTOM);
    VLScanMemFreeSigGroup(&lpMemSigs->stALL);

	// Free quick eliminate buffers

    if (lpMemSigs->stCD.lpbyQuickWordEliminate != NULL)
    	free(lpMemSigs->stCD.lpbyQuickWordEliminate);

	lpMemSigs->stCD.lpbyQuickWordEliminate = NULL;

    if (lpMemSigs->stALL.lpbyQuickWordEliminate != NULL)
    	free(lpMemSigs->stALL.lpbyQuickWordEliminate);

	lpMemSigs->stALL.lpbyQuickWordEliminate = NULL;

    if (lpMemSigs->stTOM.lpbyQuickWordEliminate != NULL)
    	free(lpMemSigs->stTOM.lpbyQuickWordEliminate);

	lpMemSigs->stTOM.lpbyQuickWordEliminate = NULL;

    free(lpMemSigs);
}


//********************************************************************
//
// Function:
//  VLScanMemSigCmp()
//
// Parameters:
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//	wIndexA			Index of first signature
//	wIndexB			Index of second signature
//	wSigLen			Length of signature
//
// Returns:
//	< 0				Signature A < Signature B
//	= 0				Signature A = Signature B
//	> 0				Signature A > Signature B
//
// Description:
//	Compares the signatures at the two indices.
//
//	The function correctly crosses block boundaries.
//
//********************************************************************

int VLScanMemSigCmp
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wIndexA,
    WORD            	wIndexB,
    WORD            	wSigLen
)
{
    WORD            	wBlkA, wBlkB;

    // Find first sig

    wBlkA = wIndexA >> MEM_SIGS_BLK_SHIFT;
    wIndexA &= MEM_SIGS_BLK_ENTRY_MASK;

    // Find second sig

    wBlkB = wIndexB >> MEM_SIGS_BLK_SHIFT;
    wIndexB &= MEM_SIGS_BLK_ENTRY_MASK;

    // Return comparison result

    return memcmp(
		lpMemSigGroup->stMemSigBlk[wBlkA].lpMemSigs[wIndexA].bySig,
        lpMemSigGroup->stMemSigBlk[wBlkB].lpMemSigs[wIndexB].bySig,
        wSigLen);
}


//********************************************************************
//
// Function:
//  VLScanMemSigSwap()
//
// Parameters:
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//	wIndexA			Index of first signature
//	wIndexB			Index of second signature
//
// Returns:
//	nothing
//
// Description:
//	Swaps the signatures at the two indices, including the virus
//	indices.
//
//	The function correctly crosses block boundaries.
//
//********************************************************************

void VLScanMemSigSwap
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wIndexA,
    WORD            	wIndexB
)
{
    WORD            	wBlkA, wBlkB;
    MEMSIG_T        	sTmpMemSig;

    // Find first sig

    wBlkA = wIndexA >> MEM_SIGS_BLK_SHIFT;
    wIndexA &= MEM_SIGS_BLK_ENTRY_MASK;

    // Find second sig

    wBlkB = wIndexB >> MEM_SIGS_BLK_SHIFT;
    wIndexB &= MEM_SIGS_BLK_ENTRY_MASK;

    // Swap sigs

    sTmpMemSig = lpMemSigGroup->stMemSigBlk[wBlkA].lpMemSigs[wIndexA];
    lpMemSigGroup->stMemSigBlk[wBlkA].lpMemSigs[wIndexA] =
        lpMemSigGroup->stMemSigBlk[wBlkB].lpMemSigs[wIndexB];
    lpMemSigGroup->stMemSigBlk[wBlkB].lpMemSigs[wIndexB] = sTmpMemSig;
}


//********************************************************************
//
// Function:
//  VLScanMemSigShellSort()
//
// Parameters:
//	lpMemSigGroup	Pointer to MEMSIGGROUP_T structure.
//	wSigLen			Length of signature
//
// Returns:
//	nothing
//
// Description:
//	Sorts the signatures of the group using Shell's sorting
//	algorithm.
//
//********************************************************************

void VLScanMemSigShellSort
(
    LPMEMSIGGROUP   	lpMemSigGroup,
    WORD            	wSigLen
)
{
    WORD    			wGap;
    long    			i, j;

    // Shell's sorting algorithm

    for (wGap=lpMemSigGroup->wNumSigs/2;wGap>0;wGap/=2)
        for (i=wGap;i<lpMemSigGroup->wNumSigs;i++)
        {
            j = i - wGap;
            while (j>=0 &&
            	VLScanMemSigCmp(lpMemSigGroup,j,j+wGap,wSigLen) > 0)
			{
                VLScanMemSigSwap(lpMemSigGroup,j,j+wGap);
                j -= wGap;
            }
        }
}

