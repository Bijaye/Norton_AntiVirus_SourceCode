// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/EXEC.CPv   1.16   14 Oct 1998 11:57:38   MKEATIN  $
//
// Description:
//
//  This file deals with the top level emulation of programs.  It contains:
//
//  1. The main emulator interpretation subroutine: interpret()
//  2. A routine which determines which pages were modified during emulation
//     and scans those pages using the string scanning system (SEARCH.CPP),
//  3. Instruction fault code.
//
//  This file can also be build with the BUILD_SET define in order to create
//  a standalone program which can be used to obtain virus information instead
//  of just scanning for viruses.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/EXEC.CPv  $
// 
//    Rev 1.16   14 Oct 1998 11:57:38   MKEATIN
// Carey's RelinquishControl calls for NLM.
// 
//    Rev 1.15   05 Jun 1998 15:19:06   CNACHEN
// Updated to properly decrypt viruses that decrypt the last part of their
// decryption loop and then execute through it.
// 
//    Rev 1.14   03 Nov 1997 17:11:32   CNACHEN
// Updated to support Win16
// 
//    Rev 1.13   13 Oct 1997 13:29:36   CNACHEN
// Relinquish control even more!
// 
//    Rev 1.12   13 Oct 1997 11:15:02   CNACHEN
// Modified relinquish control iterations.
// 
//    Rev 1.11   08 Sep 1997 11:08:02   CNACHEN
// Added DRelinquishControl calls/prototypes
// 
//    Rev 1.10   02 Sep 1997 13:59:32   CNACHEN
// Re-ordered bool declaration before code for non C++ platforms (NLM)
// around line 505
// 
//    Rev 1.9   09 Jul 1997 16:34:42   CNACHEN
// Added support for 3 config levels for all major config options.
// 
// 
//    Rev 1.8   07 Jul 1997 18:09:54   CNACHEN
// Make sure not to check for jmp to middle of file if we're not at TOF in
// HeuristicInterpret.
// 
//    Rev 1.7   03 Jul 1997 17:06:56   CNACHEN
// Fixed bug where we have a prefetch queue crash when CS:IP > FFFFF
// (CS=FFAE, IP=FFF9 or so causes our EA to be too high and causes
// problems)
// 
//    Rev 1.6   01 Jul 1997 13:49:12   CNACHEN
// Added heavy hitter support.
// 
//    Rev 1.5   06 Jun 1997 15:31:58   CNACHEN
// Does not terminate after finding INT 21, AH=4C now; continues emulating until
// (1) we start executing the host or (2) we start searching for new regions to
// execute that were never branched to.
// 
//    Rev 1.4   04 Jun 1997 11:33:32   CNACHEN
// 
//    Rev 1.3   04 Jun 1997 11:30:54   CNACHEN
// #ifdefed external global variables from code
// 
//    Rev 1.2   23 May 1997 18:13:00   CNACHEN
// Added support for new header found in virus body code.
// 
//    Rev 1.1   22 May 1997 15:06:30   CNACHEN
// Added support for: FILE_READ/WRITE_E9/MZ, WRITE_AFTER_E9/MZ, PUSH_RET,
//                    PUSH_RET100
// 
//    Rev 1.0   14 May 1997 17:23:04   CNACHEN
// Initial revision.
// 
//    Rev 1.32   21 Mar 1997 14:03:54   CNACHEN
// Updated to support BIG endian and picky non-Intel machines...
// 
//    Rev 1.31   05 Jun 1996 12:31:44   CNACHEN
// Fixed CSIP queue lookup
// 
//    Rev 1.30   20 May 1996 14:38:12   CNACHEN
// Added GETSIG change. NOTE: this is commented for normal scanner
// operation and will not affect emulation other than getsig.
// This fixes a prefetch-queue problem during getsig operation.
// Old behavior: getsig detects prefetch queue fumbling and also finds
// the BUILD.SIG signature.  When it returns, PAMAPI detects the prefetch
// queue modification and re-emulates with a smaller prefetch queue even
// though the signature was already found.  The new version does not do this.
// 
// 
//    Rev 1.29   09 Apr 1996 12:17:26   CNACHEN
// Fixed string searching so we are guaranteed to search the entire 1K buffer.
// This is done by adding an extra 24 bytes to the end of the buffer and zeroing
// this data.
// 
//    Rev 1.28   04 Mar 1996 16:01:24   CNACHEN
// Added #IFDEF'd cache support.
// 
//    Rev 1.27   27 Feb 1996 15:59:36   CNACHEN
// Commented this file and fixed a bug with STOP_16 and STOP_1K exclusion calls.
// 
//    Rev 1.26   26 Feb 1996 18:25:46   CNACHEN
// Commented the first 2 functions.  Lots more work to do...
// 
//    Rev 1.25   16 Feb 1996 13:23:24   CNACHEN
// Fixed handling of 16 and 1024 exclusions when the target program uses REPMOVS
// 
//    Rev 1.24   13 Feb 1996 12:00:32   CNACHEN
// Chnaged memset to MEMSET.
// 
//    Rev 1.23   09 Feb 1996 16:03:44   CNACHEN
// fixed no-fault bug...
// 
//    Rev 1.22   02 Feb 1996 11:44:20   CNACHEN
// Added new dwFlags and revamped all exclusion checking...
// 
// 
//    Rev 1.21   24 Jan 1996 13:34:18   DCHI
// Typecast fix.
// 
//    Rev 1.20   23 Jan 1996 14:43:32   DCHI
// Addition of CSIP queue, ARPL repair query, and modification of immunity
// counter to deal with nested immunity activations.
// 
//    Rev 1.19   22 Jan 1996 17:25:30   DCHI
// Added incrementing of instructionFetchCount.
// 
//    Rev 1.18   22 Jan 1996 13:40:10   CNACHEN
// Added new fault support.
// 
//    Rev 1.17   04 Jan 1996 10:15:48   CNACHEN
// Added two new PAM opcodes for PrefetchQ->VM and VM->PrefetchQ...
// 
//    Rev 1.16   21 Dec 1995 19:08:56   CNACHEN
// Added GETSIG stuff.
// 
//    Rev 1.15   19 Dec 1995 19:08:00   CNACHEN
// Added prefetch queue support!
// 
// 
//    Rev 1.14   14 Dec 1995 15:21:22   CNACHEN
// Added control relinquishing in between memory scans of 1K buffers.
// 
//    Rev 1.13   14 Dec 1995 13:02:44   CNACHEN
// Added support for control relinquishing under NLM...
// 
//    Rev 1.12   14 Dec 1995 10:49:32   CNACHEN
// No change.
// 
//    Rev 1.11   15 Nov 1995 17:45:34   CNACHEN
// Added additional handling for GETSIG.
// 
//    Rev 1.10   03 Nov 1995 11:33:24   CNACHEN
// Fixed iteration checkpoints.
// 
//    Rev 1.9   19 Oct 1995 18:23:42   CNACHEN
// Initial revision... with comment header :)
// 
//************************************************************************


#include "heurapi.h"

#ifdef PRINT_DEBUG
extern int g_nFileNum;
extern DWORD g_dwNumIter;
#endif

#ifdef SYM_WIN16
#define memcmp _fmemcmp
#endif

/////////////////////////////////////////////////////////////////////////////

void CPURestoreState
(
    PAMLHANDLE              hLocal,
    LPCPU_CACHE_STATE       lpstState
)
{
    WORD                    i;

    hLocal->CPU.preg = lpstState->preg;
    hLocal->CPU.ireg = lpstState->ireg;
    hLocal->CPU.IP = lpstState->IP;

    hLocal->CPU.CS = lpstState->CS;
    hLocal->CPU.DS = lpstState->DS;
    hLocal->CPU.ES = lpstState->ES;
    hLocal->CPU.SS = lpstState->SS;
    hLocal->CPU.FS = lpstState->FS;
    hLocal->CPU.GS = lpstState->GS;
    hLocal->CPU.FLAGS = lpstState->FLAGS;

    for (i=0;i<STACK_SAVE_WORDS;i++)
        put_word(hLocal,
                 hLocal->CPU.SS,
                 hLocal->CPU.ireg.X.SP +
                  STACK_SAVE_WORDS*sizeof(WORD) +
                  i * sizeof(WORD),
                 lpstState->wStackValues[i]);

    reset_prefetch_queue(hLocal);
}

void CPUSaveState
(
    PAMLHANDLE              hLocal,
	LPCPU_CACHE_STATE       lpstState
)
{
    WORD                    i;

    lpstState->preg = hLocal->CPU.preg;
    lpstState->ireg = hLocal->CPU.ireg;
    lpstState->IP = hLocal->CPU.IP  ;

    lpstState->CS = hLocal->CPU.CS  ;
    lpstState->DS = hLocal->CPU.DS  ;
    lpstState->ES = hLocal->CPU.ES  ;
    lpstState->SS = hLocal->CPU.SS  ;
    lpstState->FS = hLocal->CPU.FS  ;
    lpstState->GS = hLocal->CPU.GS  ;
    lpstState->FLAGS = hLocal->CPU.FLAGS;

    for (i=0;i<STACK_SAVE_WORDS;i++)
        lpstState->wStackValues[i] = get_word(hLocal,
                                              hLocal->CPU.SS,
                                              hLocal->CPU.ireg.X.SP +
                                               STACK_SAVE_WORDS*sizeof(WORD) +
                                               i * sizeof(WORD));
}



void ObtainBuffers
(
    PAMLHANDLE              hLocal,
    LPBYTE                  lpbyBuffer,
    LPDWORD                 lpdwBufferLen
)
{
    int                     i;
    WORD                    wSeg, wStartOff, wEndOff, wDist;

    EMGetRange(&hLocal->stEM,&wSeg, &wStartOff,&wEndOff);

    wDist = wEndOff - wStartOff;

    if (wDist > hLocal->hGPAM->
                    config_info[hLocal->nCurHeurLevel].wAssumedVirusSize )
    {
        // make sure we don't get too many bytes

        wDist = hLocal->hGPAM->
                    config_info[hLocal->nCurHeurLevel].wAssumedVirusSize ;
    }

    for (i=0;i<wDist;i++)
    {
        lpbyBuffer[i] = get_byte(hLocal,wSeg,(WORD)(wStartOff+i));

#ifdef SYM_NLM
        if ((i & 0xFF) == 0)
            DRelinquishControl();
#endif

    }

    MEMSET(lpbyBuffer+i,
           0,
           hLocal->hGPAM->
            config_info[hLocal->nCurHeurLevel].wAssumedVirusSize -i);

	*lpdwBufferLen = wDist;
}


void CheckBuffers
(
    PAMLHANDLE          hLocal,
    LPBYTE              lpbyVirusBuffers,
    DWORD               dwBufferLen
)
{
    WORD                wLen, i, wSearchLen, wStart;

    if (hLocal->byTOF[0] == 'M' && hLocal->byTOF[1] == 'Z')
    {
        wStart = 0;
        wLen = 0x1c;
    }
    else
        wStart = wLen = 3;

    wSearchLen = (WORD)(dwBufferLen-wLen+1);

    for (i=wStart;i<wSearchLen;i++)
    {
#ifdef SYM_NLM
        if ((i & 0xFF) == 0)
            DRelinquishControl();
#endif

        if (memcmp(hLocal->byTOF,lpbyVirusBuffers+i,wLen) == 0)
        {
            BMSubmitFoundHeaderInBody(&hLocal->stBM);
            return;
        }
    }
}

#ifdef BUILD_HEAVY_HITTER

DWORD   gdwLastIterations = 0;

#endif


BOOL HeuristicInterpret
(
    PAMLHANDLE          hLocal,
    LPBOOL              lpbFoundVirus,
    LPWORD              lpwVirusID
)
{
    BYTE                opcode;
    PAMConfigType FAR   *pConfig;
    BOOL                bHitYet;
    BOOL                bUpdateRange;
    WORD                wOrigCS, wOrigIP;

    DWORD               dwBufferLen, dwMaxIter;

    // zero all flags and virus size information
	    					 
    pConfig = &(hLocal->hGPAM->config_info[hLocal->nCurHeurLevel]);

    // we are now in flood/exploration mode.

    EMSetExploreMode(&hLocal->stEM,TRUE);

    // clear our segment write map so nothing is initially marked as written
    // to.  This map was used to determine whether or not self-modifying code
    // was being executed so we could know when to stop the decryption phase.
    // Now it will be used to redirect writes to an alternate segment to
    // prevent the virus from overwriting itself when its running in place.

    EMClearWriteMap(&hLocal->stEM);

    // Determine whether or not we need to establish the likely viral region
    // still, or whether that has already been established.

    if (EMGetFileType(&hLocal->stEM) == EXEC_FILE_TYPE_COM &&
        EMGetRangeFinalStatus(&hLocal->stEM) == FALSE)
        bUpdateRange = TRUE;
    else
    {
        bUpdateRange = FALSE;

        // exe check of buffers

        ObtainBuffers(hLocal,hLocal->byVirusBuffers,&dwBufferLen);
        CheckBuffers(hLocal,hLocal->byVirusBuffers,dwBufferLen);
    }

    // When hLocal->bReset is set to TRUE, we want to stop emulating at the
    // current instruction and look in our queue for another execution path.
    // Essentially, we've hit a dead end.

    hLocal->bReset = FALSE;

    // reset our detection of self relocating programs

    hLocal->wMOVSDelta = 0;

    // reset the flags

    hLocal->dwFlags = 0;

    // reset the prefetch queue

    reset_prefetch_queue(hLocal);

    ///////////////////////////////////////////////////////////////////////
    // Heuristics
    ///////////////////////////////////////////////////////////////////////

    if (hLocal->CPU.IP == 0x100 && hLocal->CPU.CS == 0x7f0 &&
        get_byte(hLocal,hLocal->CPU.CS,hLocal->CPU.IP) == 0xE9)
    {
        BMSubmitE9AtTOF(&hLocal->stBM,
                        get_word(hLocal,
                                 hLocal->CPU.CS,
                                 (WORD)(hLocal->CPU.IP+1)));
    }

    ///////////////////////////////////////////////////////////////////////
    // Heuristics
    ///////////////////////////////////////////////////////////////////////

    dwMaxIter = hLocal->hGPAM->
                    config_info[hLocal->nCurHeurLevel].wAssumedVirusSize;
    
    for (hLocal->dwIteration = 0;
         hLocal->dwIteration < dwMaxIter;
         hLocal->dwIteration++)
	{

#ifdef PRINT_DEBUG
        g_dwNumIter++;
#endif

#ifdef BUILD_HEAVY_HITTER

        gdwLastIterations++;

#endif

#ifdef SYM_NLM

        if ((hLocal->dwIteration & 0xF) == 0)
        {
            DRelinquishControl();
        }
#endif

        if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
            return(FALSE);                      // error for whatever reason

        // see if we've hit CS:100 again.  If so, record it; this may be
        // happening because a virus returned control to the host.

        if (hLocal->CPU.IP == 0x100 && hLocal->CPU.CS == 0x7f0)
		{
            BMSubmitIP100(&hLocal->stBM,hLocal->dwIteration);

            if (hLocal->dwIteration > 0 &&
                EMGetFileType(&hLocal->stEM) == EXEC_FILE_TYPE_COM)
            {
                BMSubmitOriginalEntry(&hLocal->stBM, FALSE);
            }
		}

        ///////////////////////////////////////////////////////////////////////
        // Heuristics
        ///////////////////////////////////////////////////////////////////////

        // 10 is a good guess of how long before we're pretty sure we're in
        // our virus body
        // submit CS:IP so the BM can determine the distance to EOF from
        // each CS:IP; this set of up to 10 distances can then be compared
        // to file writes the program does.  If the number of bytes between
        // the EP and EOF is the same as the file write, this is suspicious.

        if (hLocal->dwIteration <
                hLocal->
                 hGPAM->
                  config_info[hLocal->nCurHeurLevel].wMaxIterForEPAdjust)
        {
            BMSubmitCurCSIP(&hLocal->stBM,
                            hLocal->CPU.CS,
                            hLocal->CPU.IP);
        }

        // now see if we want to update our viral flood region...
        // This only takes place in COM files.  We only want to do
        // this once, between 10 and 128 instructions...

        if (hLocal->dwIteration <
            hLocal->hGPAM->
                config_info[hLocal->nCurHeurLevel].wMaxIterForRangeAdjust &&
            hLocal->dwIteration >=
            hLocal->hGPAM->
                config_info[hLocal->nCurHeurLevel].wMaxIterForEPAdjust &&
            bUpdateRange == TRUE)
        {
            if (EMUpdateRangeByCSIP(&hLocal->stEM,
                                    hLocal->CPU.CS,
                                    hLocal->CPU.IP,
                                    TRUE) == EXEC_STATUS_OK)
            {
                bUpdateRange = FALSE;
                ObtainBuffers(hLocal,hLocal->byVirusBuffers,&dwBufferLen);
                CheckBuffers(hLocal,hLocal->byVirusBuffers,dwBufferLen);
            }
        }

        // Test and set the execution status of the next instruction that
        // we're going to execute.  See if we've been here yet
        // (bHitYet == TRUE)

        if (hLocal->bReset == FALSE)
            EMTSByteExecStatus(&hLocal->stEM,
                               hLocal->CPU.CS,
                               hLocal->CPU.IP,
                               &bHitYet);

        // if we've been here before, or we need to reset (hLocal->bReset
        // is set when certain conditions or instructions are executed), then
        // look in our queue for a new execution path.  Also look for a new
        // execution path if we execute outside of our flood region.

        if (bHitYet == TRUE ||
            hLocal->bReset == TRUE ||
            EMInRange(&hLocal->stEM,
                      hLocal->CPU.CS,
                      hLocal->CPU.IP) == FALSE ||
            (DWORD)hLocal->CPU.CS * PARAGRAPH + hLocal->CPU.IP >=
                EFFECTIVE_ADDRESS_MASK
            )

        {
            CPU_CACHE_STATE_T       stState;
            BOOL                    bAnyLeft;

            // make sure to reset call-pop and pushf status here!
            // otherwise we may jump from one piece of code to another
            // and get confused

            EMDequeue(&hLocal->stEM,&stState,&bAnyLeft);
            BMResetContext(&hLocal->stBM);

            // did we find anything enqueued?

            if (bAnyLeft == TRUE)
            {
                CPURestoreState(hLocal,&stState);
                reset_prefetch_queue(hLocal);
                hLocal->bReset = FALSE;
                continue;
            }
            else
            {
                BOOL            bGotBlock;
                
                // we're done exploring all paths that can be reached from
                // the orig. EP...

                BMSubmitOriginalEntry(&hLocal->stBM,FALSE);

                // done going through all paths.
                // try to find open regions within the flood area
                // that we haven't hit yet.

                EMGetIPOfNextClearBlock(&hLocal->stEM,
                                        &hLocal->CPU.CS,
                                        &hLocal->CPU.IP,
                                        &bGotBlock);

                //printf("\n\nFinding next clear block (%04X:%04X)\n\n",hLocal->CPU.CS,hLocal->CPU.IP);

                // reset the critical registers and continue executing.

                if (bGotBlock == TRUE)
                {
                    hLocal->CPU.preg.X.AX = 0;
                    hLocal->CPU.preg.X.BX = 0;
                    hLocal->CPU.preg.X.CX = 0;
                    hLocal->CPU.preg.X.DX = 0;
                    hLocal->CPU.ireg.X.SI = 0;
                    hLocal->CPU.ireg.X.DI = 0;

                    reset_prefetch_queue(hLocal);
                    hLocal->bReset = FALSE;
                    continue;
                }

                break;
            }
        }

        // remember our CS:IP for the about-to-be-fetched instruction

        wOrigCS = hLocal->CPU.CS;
        wOrigIP = hLocal->CPU.IP;

        ///////////////////////////////////////////////////////////////////////
        // Heuristics
        ///////////////////////////////////////////////////////////////////////

        opcode = get_byte_at_csip(hLocal);

        hLocal->CPU.reset_seg = TRUE;

		large_switch(hLocal,opcode);

        if (TRUE == hLocal->CPU.reset_seg)
		{
            reset_seg_over(hLocal);     /* in the case of segment prefixes */
										/* this is not reset. */

            reset_rep_over(hLocal);     /* reset REPNZ/REPZ prefix overrides */

            reset_32_bit_over(hLocal);  /* forget about 32 bit stuff */
        }

        ///////////////////////////////////////////////////////////////////////
        // Heuristics
        ///////////////////////////////////////////////////////////////////////

        // submit the opcode that we just executed

        BMSubmitOpcode(&hLocal->stBM,
                       opcode,
                       wOrigCS,
                       wOrigIP,
                       hLocal->CPU.CS,
                       hLocal->CPU.IP,
                       hLocal->dwIteration);

        ///////////////////////////////////////////////////////////////////////
        // Heuristics
        ///////////////////////////////////////////////////////////////////////
    }

    // done flooding/emulating/etc.  Time to scan for strings within the
    // flood region...

    ObtainBuffers(hLocal,hLocal->byVirusBuffers,&dwBufferLen);
    BMSubmitBuffers(&hLocal->stBM,hLocal->byVirusBuffers,dwBufferLen);
    //BMPrintBehavior(&hLocal->stBM,g_nFileNum);
   
    // now see if we have a virus!

    if (BADetectVirus(hLocal->hGPAM->wHeurStreamLen,
                      hLocal->hGPAM->lpbyHeurCode,
                      &hLocal->stBM,
                      lpbFoundVirus,
                      lpwVirusID) == FALSE)
    {
        // error

        return(FALSE);
    }

    // TRUE means success; *lpbFoundVirus indicates infection
		 
    return(TRUE);
}

// true means continue, false means stop

BOOL HeuristicDecryptInterpret
(
    PAMLHANDLE          hLocal,
    BOOL                *bReset
)
{
    BYTE                opcode;
    PAMConfigType FAR   *pConfig;
    DWORD               dwTemp;
    BOOL                bHitYet;
    BOOL                bUpdateRange, bCOMFile;
    WORD                wOrigCS, wOrigIP;
    DWORD               dwMaxIter;

    *bReset = FALSE;

    // see if we should not emulate for any reason (we already determined
    // that this file can't have a virus)
	   
    if (BMStopEmulating(&hLocal->stBM) == TRUE)
    {
//        printf("Early out:\n");
        //BMPrintBehavior(&hLocal->stBM,g_nFileNum);
        return(FALSE);
    }
    
    pConfig = &(hLocal->hGPAM->config_info[hLocal->nCurHeurLevel]);

    // not exploring just yet; we are trying to decrypt the file if it is
    // encrypted.

    EMSetExploreMode(&hLocal->stEM,FALSE);

    // if our file type is COM, we want to later update our "likely virus"
    // region in the program.  If it is an EXE, use the entry-point to
    // determine the likely virus region (it will sandwitch the entry-point).

    if (EMGetFileType(&hLocal->stEM) == EXEC_FILE_TYPE_COM)
        bCOMFile = bUpdateRange = TRUE;
    else
        bCOMFile = bUpdateRange = FALSE;

    ///////////////////////////////////////////////////////////////////////
    // Heuristics
    ///////////////////////////////////////////////////////////////////////

    if (get_byte(hLocal,hLocal->CPU.CS,hLocal->CPU.IP) == 0xE9)
    {
        BMSubmitE9AtTOF(&hLocal->stBM,
                        get_word(hLocal,
                                 hLocal->CPU.CS,
                                 (WORD)(hLocal->CPU.IP+1)));
    }

    ///////////////////////////////////////////////////////////////////////
    // Heuristics
    ///////////////////////////////////////////////////////////////////////

    BMSubmitOriginalEntry(&hLocal->stBM,TRUE);

    dwMaxIter = hLocal->
                 hGPAM->
                  config_info[hLocal->nCurHeurLevel].wMaxEncryptedVirusIterations;

    for (hLocal->dwIteration = 0;
         hLocal->dwIteration < dwMaxIter;
         hLocal->dwIteration++)
	{
#if PRINT_DEBUG
        g_dwNumIter++;
#endif

#ifdef BUILD_HEAVY_HITTER

        gdwLastIterations++;

#endif

#ifdef SYM_NLM

        if ((hLocal->dwIteration & 0xF) == 0)
        {
            DRelinquishControl();
        }
#endif

        if ((DWORD)hLocal->CPU.CS * PARAGRAPH + hLocal->CPU.IP >=
            EFFECTIVE_ADDRESS_MASK)
            return(TRUE);

        if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
            return(FALSE);                  // error for whatever reason

        if (bCOMFile == TRUE &&
            hLocal->CPU.IP == 0x100 && hLocal->CPU.CS == 0x7f0)
		{
            // we hit CS:100 at least once...

            BMSubmitIP100(&hLocal->stBM,hLocal->dwIteration);

            if (hLocal->dwIteration != 0)
            {
                // executing host, if anything; no need to continue
                // general emulation... transfer control to heuristic
                // interpret to flood-emulate likely viral area...

                BMSubmitOriginalEntry(&hLocal->stBM, FALSE);

                EMGetMaxWriteRangeSize(&hLocal->stEM,&dwTemp);

                if (dwTemp <
                    hLocal->hGPAM->
                        config_info[hLocal->nCurHeurLevel].wMinModifiedByteRange)
                {
                    BMSubmitOriginalEntry(&hLocal->stBM,TRUE);

                    *bReset = TRUE;     // not encrypted!

                    return(TRUE);       // continue normally
                }
            }
        }

		// if we see a repmovs during decryption, exit out and start doing
        // flooding; this isn't a decryptor.

        if (hLocal->stBM.m_byBehaviorArray[BF5_FOUND_REPMOVS])
		{
			EMGetMaxWriteRangeSize(&hLocal->stEM,&dwTemp);

            if (dwTemp <
                hLocal->
                 hGPAM->
                  config_info[hLocal->nCurHeurLevel].wMinModifiedByteRange)
            {
                BMSubmitOriginalEntry(&hLocal->stBM,TRUE);

				*bReset = TRUE;     // not encrypted!

                return(TRUE);       // continue normally
			}
		}

        // see if we should stop emulating for any reason
	   
        if (BMStopEmulating(&hLocal->stBM) == TRUE)
        {
   			return(FALSE);
        }

        if (hLocal->dwIteration ==
            hLocal->
             hGPAM->
              config_info[hLocal->nCurHeurLevel].wMinEncryptedVirusIterations)
        {
            EMGetMaxWriteRangeSize(&hLocal->stEM,&dwTemp);

            if (dwTemp <
                hLocal->
                 hGPAM->
                  config_info[hLocal->nCurHeurLevel].wMinModifiedByteRange)
            {
                *bReset = TRUE;      // not encrypted!

                BMSubmitOriginalEntry(&hLocal->stBM,TRUE);

                return(TRUE);
            }
        }

        // for ip to eof dist to compare against file writes...

        if (hLocal->dwIteration <
            hLocal->
             hGPAM->
              config_info[hLocal->nCurHeurLevel].wMaxIterForEPAdjust )
        {
            // record all CS:IP's so we can determine if a file write
            // matches the number of bytes from any of the recorded
            // CS:IP's to the EOF.

            BMSubmitCurCSIP(&hLocal->stBM,
                            hLocal->CPU.CS,
                            hLocal->CPU.IP);
        }

        // now see if we want to update our viral flood region...
        // This only takes place in COM files.  We only want to do
        // this once, between 10 and 128 instructions...

        if (hLocal->dwIteration <
            hLocal->
             hGPAM->
              config_info[hLocal->nCurHeurLevel].wMaxIterForRangeAdjust &&
            hLocal->dwIteration >=
            hLocal->
             hGPAM->
              config_info[hLocal->nCurHeurLevel].wMaxIterForEPAdjust &&
            bUpdateRange == TRUE)
        {
            if (EMUpdateRangeByCSIP(&hLocal->stEM,
                                    hLocal->CPU.CS,
                                    hLocal->CPU.IP,
                                    TRUE) == EXEC_STATUS_OK)
				bUpdateRange = FALSE;				
        }

        // we've hit this byte before for execution... remember!

        EMSetByteExecStatus(&hLocal->stEM,
                            hLocal->CPU.CS,
                            hLocal->CPU.IP,
                            TRUE);

        // see if we've modified this byte before... if so, we're probably
        // executing decrypted code.

        EMGetByteWriteStatus(&hLocal->stEM,
							 hLocal->CPU.CS,
							 hLocal->CPU.IP,
							 &bHitYet);

        if (bHitYet == TRUE)
        {
            if (hLocal->dwIteration <
                hLocal->hGPAM->
                  config_info[hLocal->nCurHeurLevel].wMinEncryptedVirusIterations)
            {
                // we've got some self-modifiying code, but probably haven't
                // decrypted everything yet - likely we decrypted the last
                // couple bytes of the decryption loop and we need to
                // execute the modified instructions to decrypt the
                // virus ...  Reset this particular byte write status and
                // continue...

                EMSetByteWriteStatus(&hLocal->stEM,
                                     hLocal->CPU.CS,
                                     hLocal->CPU.IP,
                                     FALSE);
            }
            else
            {
                // self-modifying code.. done!

                BMSubmitSelfModifyingCode(&hLocal->stBM);

                // we haven't yet executed this modified code, but do need to do so!
                // update the status so it looks as if we haven't executed it yet.

                EMSetByteExecStatus(&hLocal->stEM,
                                    hLocal->CPU.CS,
                                    hLocal->CPU.IP,
                                    FALSE);

                // Note: bReset == FALSE at this point, so we stay decrypted.

                return(TRUE);
            }
        }


        // remember where the current instruction starts

        wOrigCS = hLocal->CPU.CS;
        wOrigIP = hLocal->CPU.IP;

        // fetch the instruction

        opcode = get_byte_at_csip(hLocal);

		


        // make sure we allow instruction faults now.

        hLocal->CPU.reset_seg = TRUE;

		large_switch(hLocal,opcode);

        if (TRUE == hLocal->CPU.reset_seg)
		{
            reset_seg_over(hLocal);     /* in the case of segment prefixes */
										/* this is not reset. */

            reset_rep_over(hLocal);     /* reset REPNZ/REPZ prefix overrides */

            reset_32_bit_over(hLocal);  /* forget about 32 bit stuff */
        }

        ///////////////////////////////////////////////////////////////////////
        // Heuristics
        ///////////////////////////////////////////////////////////////////////

        BMSubmitOpcode(&hLocal->stBM,
                       opcode,
                       wOrigCS,
                       wOrigIP,
                       hLocal->CPU.CS,
                       hLocal->CPU.IP,
                       hLocal->dwIteration);

        ///////////////////////////////////////////////////////////////////////
        // Heuristics
        ///////////////////////////////////////////////////////////////////////
    }

    //BMPrintBehavior(&hLocal->stBM,g_nFileNum);

    return(TRUE);
}


