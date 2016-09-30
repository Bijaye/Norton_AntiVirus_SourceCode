/* $Header:   S:/SRC/NORTON/VCS/CMDALLOC.C_V   1.2   05 Nov 1992 14:41:20   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * CMDALLOC.C								*
 *									*
 * This file contains routines for allocating and freeing command	*
 * information records.							*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   S:/SRC/NORTON/VCS/CMDALLOC.C_V  $
 * 
 *    Rev 1.2   05 Nov 1992 14:41:20   SKURTZ
 * Made AllocCmdBuf() normalize the address that it returns.  This fixes ptrs
 * NU04320 and NU04793
 *
 *    Rev 1.1   17 Oct 1992 02:11:36   JOHN
 * General cleanup and update for New Loader
 *
 *    Rev 1.0   20 Aug 1990 15:19:34   DAVID
 * Initial revision.
 */

#include	<types.h>
#include	<vmm.h>
#include	<dialog.h>
#include	<file.h>
#include	<nuconfig.h>
#include	<lint.h>

#include	"defs.h"
#include	"proto.h"

MODULE_NAME;

/*----------------------------------------------------------------------*
 * This routine allcates a pool of up to MAX_COMMANDS CmdInfoRecs	*
 * from the far heap and marks all the buffers in the pool as free.	*
 *----------------------------------------------------------------------*/

void InitCmdBuf(void)
{
    extern	CmdInfoRec	huge *cmdBufBase;
    extern	Word 		numCmdBufs;
    extern	Boolean		cmdBufUsed[MAX_COMMANDS];
    auto	Ulong		totalBufs;
    auto	Word		i;

    totalBufs = (VMMAvailWithPurge() - 65536L) / sizeof(CmdInfoRec);
    if (totalBufs > MAX_COMMANDS)
        totalBufs = MAX_COMMANDS;

    cmdBufBase = VMMAllocFixed(totalBufs * sizeof(CmdInfoRec));
    numCmdBufs = (Word) totalBufs;

    // This step is redundant since the global
    // cmdBufUsed[] array is initialized to zero.

    for (i = 0; i < numCmdBufs; i++)
	cmdBufUsed[i] = FALSE;
}

/*----------------------------------------------------------------------*
 * This routine allocates a CmdInfoRec buffer from the pool.		*
 *									*
 * Returns:	Pointer to buffer if one is available			*
 *		NULL if out of buffers					*
 *----------------------------------------------------------------------*/

CmdInfoRec far *AllocCmdBuf (void)
{
    extern	CmdInfoRec	huge *cmdBufBase;
    extern	Word 		numCmdBufs;
    extern	Boolean		cmdBufUsed[MAX_COMMANDS];
    auto	Word		i;

    for (i = 0; i < numCmdBufs; i++)
        {
	if (cmdBufUsed[i] == FALSE)
	    {
	    cmdBufUsed[i] = TRUE;
	    return(MemoryNormalizeAddress(cmdBufBase + i));
	    }
	}

    return (NULL);
}

/*----------------------------------------------------------------------*
 * This routine returns a CmdInfoRec to the buffer pool.		*
 *----------------------------------------------------------------------*/

void FreeCmdBuf(CmdInfoRec huge *r)
{
    extern	CmdInfoRec	huge *cmdBufBase;
    extern	Boolean		cmdBufUsed[MAX_COMMANDS];
    auto	Word		bufNum;

    bufNum = r - cmdBufBase;
    cmdBufUsed[bufNum] == FALSE;
}