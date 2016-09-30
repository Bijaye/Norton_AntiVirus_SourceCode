// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/SCANMEM.H_v   1.0   28 Feb 1997 11:33:38   MKEATIN  $
//
// Description:  Header for DCHI new memory scanning code. Scans with
//               INSCANM.DAT files that are larger than 64k.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/SCANMEM.H_v  $
// 
//    Rev 1.0   28 Feb 1997 11:33:38   MKEATIN
// Initial revision.
//************************************************************************

#ifndef _SCANMEM_H_

#define _SCANMEM_H_

#include "platform.h"

#define SIZE_MEM            15

#define MEMORY_413			1
#define MEMORY_ALL			2

#define MEM_SIG_LEN_CD      15
#define MEM_SIG_LEN_ALL     13      // Same for ALL and TOM signatures

#define MEM_PAGE_OVERLAP    SIZE_MEM
#define MEM_PAGE_SIZE       0x1000    // 4K pages

#define MAX_MEM_SIGS_PER_BLK    2048	// Power of 2

#define MEM_SIGS_BLK_SHIFT		11		// log of MAX_MEM_SIGS_PER_BLK
#define MEM_SIGS_BLK_ENTRY_MASK	(MAX_MEM_SIGS_PER_BLK-1)

#define MAX_MEM_SIG_BLKS    	(65536ul/2048ul)

#define ADDRESS_640K        0x0A0000
#define ADDRESS_1M          0x100000

typedef struct
{
    WORD            wVirusIndex;		// Index into virscan
    BYTE            bySig[SIZE_MEM];
} MEMSIG_T, FAR *LPMEMSIG;

typedef struct
{
    WORD            wNumSigs;
    LPMEMSIG        lpMemSigs;
} MEMSIGBLK_T, FAR *LPMEMSIGBLK;

typedef struct
{
    WORD            wNumSigs;
    BYTE            byQuickEliminate[256];
	LPBYTE			lpbyQuickWordEliminate;
    WORD            wNumSigBlks;
    MEMSIGBLK_T     stMemSigBlk[MAX_MEM_SIG_BLKS];
} MEMSIGGROUP_T, FAR *LPMEMSIGGROUP;

typedef struct
{
    MEMSIGGROUP_T   stCD;
    MEMSIGGROUP_T   stALL;
    MEMSIGGROUP_T   stTOM;
} MEMSIGS_T, FAR *LPMEMSIGS;

WORD VLScanMemScan
(
    LPMEMSIGS   lpMemSigs,
  	WORD		wFlags
);

#endif  // _SCANMEM_H_

