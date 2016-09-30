// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/pamapi.cpv   1.31   25 Jun 1996 10:29:36   CNACHEN  $
//
// Description:
//
//  This file contains the top-level PAM API functions which can be used
//  to initialize the PAM system, scan for viruses and repair virus infected
//  files and boot records.
//
// Contains:
//
//  PAMSTATUS PAMGlobalInit(LPTSTR szDataFile,
//                          LPPAMGHANDLE hPtr)
//  PAMGlobalInit: Called once during general initialization.
//
//  PAMSTATUS PAMLocalInit(LPTSTR szDataFile,
//                         LPTSTR szSwapFile,
//                         PAMGHANDLE hGPAM,
//                         LPPAMLHANDLE hLPtr)
//  PAMLocalInit: Called once per virus-scanning-thread.
//                szSwapFile = NULL -> use MemoryAlloc instead of swap file...
//
//  PAMSTATUS PAMLocalClose(PAMLHANDLE hLocal)
//  PAMLocalClose: Called once per virus-scanning-thread.
//
//  PAMSTATUS PAMGlobalClose(PAMGHANDLE hGlobal)
//  PAMGlobalClose: Called once during general closedown.
//
//  PAMSTATUS PAMScanFile(PAMLHANDLE hLocal,
//                        HFILE hFile,
//                        WORD wFlags,
//                        LPWORD lpVirusID)
//  PAMScanFile: Called for each target program.
//
//PAMSTATUS PAMRepairFile(PAMLHANDLE hLocal,
//                        LPTSTR szDataFile,
//                        LPTSTR lpszFileName,
//                        LPTSTR lpszWorkFileName,
//                        WORD wVirusID,
//                        WORD wFlags)
//  PAMRepairFile: Called for each target program.
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/pamapi.cpv  $
// 
//    Rev 1.31   25 Jun 1996 10:29:36   CNACHEN
// Added debugger to ERS.
// 
//    Rev 1.30   06 May 1996 10:14:52   CNACHEN
// Removed RAD's comment and verified that the cleanup is correct in the
// global init function if more than one global init is called by a product
// under NTK platform.  NTK platform only supports one global context.
// 
//    Rev 1.29   04 May 1996 14:44:02   RSTANEV
// Exclamation, sklackamation, mistaketion...
//
//    Rev 1.28   04 May 1996 13:34:24   RSTANEV
// Now properly allocating the mutex used with SymInterlockedExchange().
//
//    Rev 1.27   27 Mar 1996 10:29:32   CNACHEN
// Re-fixed IP bug in repair.
//
//    Rev 1.26   25 Mar 1996 16:50:12   CNACHEN
// Fixed bug in ERS: If we don't have any stop signatures, then don't decrement
// ES:DI value (CS:IP).
//
//    Rev 1.25   08 Mar 1996 10:51:02   CNACHEN
// Added NOT_IN_TSR support.
//
//    Rev 1.24   05 Mar 1996 14:01:38   CNACHEN
// Added error checking for repair stuff...
//
//    Rev 1.23   04 Mar 1996 16:01:40   CNACHEN
// Added #IFDEF'd cache support.
//
//    Rev 1.22   01 Mar 1996 12:22:22   CNACHEN
// After scanning for signatures (outside of interpret), check for errors
// and return an error condition from PAMScanFile if we have any..
//
//    Rev 1.21   20 Feb 1996 11:27:08   CNACHEN
// Changed all LPSTRs to LPTSTRs.
//
//
//    Rev 1.20   14 Feb 1996 12:42:16   CNACHEN
// Fixed file closing bug for paging swap file in LocalInit
//
//    Rev 1.19   12 Feb 1996 13:29:58   CNACHEN
// Changed *'s to LP's.
//
//
//    Rev 1.18   02 Feb 1996 11:44:30   CNACHEN
// Added new dwFlags and exclusion checking.  Also modified the API for
// PAMRepairFile to take an additional argument: szDataFile...
//
//    Rev 1.17   01 Feb 1996 10:15:40   CNACHEN
// Changed char * to LPTSTR...
//
//    Rev 1.16   19 Dec 1995 19:08:02   CNACHEN
// Added prefetch queue support!
//
//
//    Rev 1.15   15 Dec 1995 18:59:02   CNACHEN
// low memory can now be read in during global init so we don't need open file
// handles...
//
//    Rev 1.14   14 Dec 1995 10:49:30   CNACHEN
// Fixed repair stuff...
//
//    Rev 1.13   13 Dec 1995 11:58:02   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
//
//    Rev 1.12   11 Dec 1995 17:33:12   CNACHEN
// Changed ERS to contain status of repeat bytes, segment prefixes, and
// addr/operand overrides within the flags word at F000:FEFE
//
//    Rev 1.11   11 Dec 1995 14:16:28   CNACHEN
// Made sure to reset all prefixes before starting ERS...
//
//    Rev 1.10   26 Oct 1995 14:17:54   CNACHEN
// Oopsie...
//
//    Rev 1.9   26 Oct 1995 14:10:52   CNACHEN
// Updated documentation of PAMLocalInit
//
//    Rev 1.8   24 Oct 1995 17:10:58   CNACHEN
// changed PAMRepairFile to pass in proper &stSigList to repair_interpret
// function...
//
//    Rev 1.7   19 Oct 1995 18:23:40   CNACHEN
// Initial revision... with comment header :)
//
//************************************************************************


/* this file contains the PAM interface code for allocating context handles */

#include "pamapi.h"

#include "ident.h"

#if defined(SYM_NTK)

// SYM_NTK requires that the PAM cache synchronization mutex is allocated in
// locked memory (xref: SYMSYNC.H)  Since the memory allocation callbacks
// in AVAPI do not support any flags, we have to find an alternative method
// of allocating the mutex in locked memory.  Currently this is solved by
// defining it in driver's locked data segment.  The problem with this
// solution is that the client can't initialize multiple PAM handles without
// them sharing the same mutex.  Since we don't want this to happen, SYM_NTK
// will not allow the creation of multiple PAM handles.

#include "symsync.h"

#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static LONG lPamGlobalMutexUsage = -1;
static LONG lPamGlobalMutex = 0;

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#endif

PAMSTATUS PAMGlobalInit(LPTSTR szDataFile,LPPAMGHANDLE hPtr)
{
    PAMGHANDLE          pTemp;
    DFSTATUS            dfTemp;
	DWORD               dwOffset;
    PAMSTATUS           pamTemp;
    DATAFILEHANDLE      hDataFile;
    HFILE               hFile;
    ExcludeContext      hExclude;

    /* point our hPtr to NULL so if we return an error its taken care of */

    *hPtr = NULL;

    pTemp = (PAMGHANDLE)PAMMemoryAlloc(sizeof(GlobalPAMType));
    if (NULL == pTemp)
		return(PAMSTATUS_MEM_ERROR);

    dfTemp = DFOpenDataFile(szDataFile,
                            READ_ONLY_FILE,
                            &hDataFile);

    /* error opening our DATAFILE - free our PAMGHANDLE memory and exit */

    if (DFSTATUS_OK != dfTemp)
    {
        PAMMemoryFree(pTemp);

        if (DFSTATUS_MEM_ERROR == dfTemp)
            return(PAMSTATUS_MEM_ERROR);
        else
            return(PAMSTATUS_FILE_ERROR);
    }

    /* now obtain the file handle into the data file for quick accessing */

    hFile = DFGetHandle(hDataFile);

#ifdef LOW_IN_RAM

    /* now its time to load up the low memory data area into the read-only
       32K buffer */

    dfTemp = DFLookUp(hDataFile,
                      ID_LOW_DATA_AREA,
                      NULL,
                      &dwOffset,
                      NULL, NULL, NULL, NULL);

    if (DFSTATUS_OK != dfTemp)
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

    if ((DWORD)PAMFileSeek(hFile,dwOffset,SEEK_SET) != dwOffset ||
		PAMFileRead(hFile,pTemp->low_mem_area,LOW_MEM_SIZE) !=
		LOW_MEM_SIZE)
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

#endif

    /* now its time to load the PAM configuration options */


    dfTemp = DFLookUp(hDataFile,
                      ID_PAM_CONFIG_OPTIONS,
                      NULL,
					  &dwOffset,
                      NULL, NULL, NULL, NULL);


    /* error reading config options? */

    if (DFSTATUS_OK != dfTemp)
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

    if ((DWORD)PAMFileSeek(hFile,dwOffset,SEEK_SET) != dwOffset ||
        PAMFileRead(hFile,&(pTemp->config_info),sizeof(PAMConfigType)) !=
        sizeof(PAMConfigType))
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }


    /* we now have our config options loaded up!  now its time to load the
        global exclusion data */

    dfTemp = DFLookUp(hDataFile,
                      ID_STATIC_DYN_EXCLUDE,
                      NULL,
					  &dwOffset,
                      NULL, NULL, NULL, NULL);


    if (DFSTATUS_OK != dfTemp)
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

	pamTemp = global_init_exclude(hFile,
								  dwOffset,
								  &(pTemp->exclude_info));

    if (PAMSTATUS_OK != pamTemp)
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(pamTemp);
    }


    /* now its time to load the global string searching data */

    dfTemp = DFLookUp(hDataFile,
                      ID_STRING_SEARCH,
                      NULL,
					  &dwOffset,
                      NULL, NULL, NULL, NULL);

    if (DFSTATUS_OK != dfTemp)
    {
        global_close_exclude(&(pTemp->exclude_info));
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

	pamTemp = global_init_search(hFile,dwOffset,pTemp);

    if (PAMSTATUS_OK != pamTemp)
    {
        global_close_exclude(&(pTemp->exclude_info));
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(pamTemp);
    }


    /* success. break out the champaigne (is that spelled right?) */

    DFCloseDataFile(hDataFile);

    /* now load up the signature-based exclusions (we weren't quite done
       before) */

    pamTemp = PExcludeInit(&hExclude,szDataFile);

	if (pamTemp != PAMSTATUS_OK)
    {
        global_close_search(pTemp);
        global_close_exclude(&(pTemp->exclude_info));
        PAMMemoryFree(pTemp);

        return(pamTemp);
    }

    // now our signature exclusion have been set up

    pTemp->sig_exclude_info = hExclude;


#ifdef USE_CACHE

    // now initialize our autoprotect cache

#if defined(SYM_NTK)

    // due to the global mutex, only one global instance is allowed to
    // initialize on the NTK platform

    if ( SymInterlockedIncrement ( &lPamGlobalMutexUsage ) )
        {
        SymInterlockedDecrement ( &lPamGlobalMutexUsage );

        PExcludeClose(pTemp->sig_exclude_info);
        global_close_search(pTemp);
        global_close_exclude(&(pTemp->exclude_info));
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_MEM_ERROR);
        }

    CacheInit(&pTemp->stCache, &lPamGlobalMutex);

#else

    CacheInit(&pTemp->stCache, &pTemp->stCache.lLocalMutex);

#endif

#endif // USE_CACHE

    // finally set our handle up...

    *hPtr = pTemp;

    return(PAMSTATUS_OK);
}


/* this can be called multiple times to provide many threads with emulator
   capabilities */

PAMSTATUS PAMLocalInit(LPTSTR szDataFile,
                       LPTSTR szSwapFile,
                       PAMGHANDLE hGPAM,
                       LPPAMLHANDLE hLPtr)
{
	PAMLHANDLE          hTemp;

#ifndef LOW_IN_RAM
	DFSTATUS            dfTemp;
	DATAFILEHANDLE      hDataFile;
#endif

	PAMSTATUS           pamStatus;

#ifdef LOW_IN_RAM
	(void)szDataFile;
#endif

	*hLPtr = NULL;

	/* allocate memory for the local pam context info */

    hTemp = (PAMLHANDLE)PAMMemoryAlloc(sizeof(LocalPAMType));

    if (NULL == hTemp)
        return(PAMSTATUS_MEM_ERROR);

	/* initialize all paging stuff */

	pamStatus = global_init_paging(szSwapFile,hTemp);

	if (pamStatus != PAMSTATUS_OK)
	{
		PAMMemoryFree(hTemp);

		return(pamStatus);
	}

	/* initialize our local exclusion bitfields */

	hTemp->hGPAM = hGPAM;

	pamStatus = context_init_exclude(hTemp);

	if (pamStatus != PAMSTATUS_OK)
	{
        global_close_paging(hTemp);
		PAMMemoryFree(hTemp);

		return(pamStatus);
	}


#ifndef LOW_IN_RAM

	/* open the LOW memory swap file */

	dfTemp = DFOpenDataFile(szDataFile,
							READ_ONLY_FILE,
							&hDataFile);

	if (DFSTATUS_OK != dfTemp)
	{
        global_close_paging(hTemp);
		context_close_exclude(hTemp);

		PAMMemoryFree(hTemp);

		if (DFSTATUS_FILE_ERROR == dfTemp)
            return(PAMSTATUS_FILE_ERROR);
        else
            return(PAMSTATUS_MEM_ERROR);
    }

    hTemp->CPU.hDataFile  = hDataFile;
    hTemp->CPU.low_stream = DFGetHandle(hDataFile);

    dfTemp = DFLookUp(hDataFile,
                      ID_LOW_DATA_AREA,
                      NULL,
					  &(hTemp->CPU.ulLowStartOffset),
                      NULL, NULL, NULL, NULL);


    if (DFSTATUS_OK != dfTemp)
    {
        global_close_paging(hTemp);
        context_close_exclude(hTemp);
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(hTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

#else

    // the low memory area has been read into memory and does not need to
    // be accessed via the file handle!

    hTemp->CPU.hDataFile = NULL;
    hTemp->CPU.low_stream = (HFILE)-1;

#endif

    /* reset all states and stuff... */

    hTemp->dwFlags = 0;

	*hLPtr = hTemp;

    return (PAMSTATUS_OK);
}



PAMSTATUS PAMLocalClose(PAMLHANDLE hLocal)
{
    /* close the paging system down.  this does *NOT* delete the temporary
       swap files */

    global_close_paging(hLocal);

    /* DFCloseDataFile closes the low_stream handle used for the low memory
       data file */

#ifndef LOW_IN_RAM

    // no need to close down if everything was read into ram...

	DFCloseDataFile(hLocal->CPU.hDataFile);

#endif

    context_close_exclude(hLocal);

    PAMMemoryFree(hLocal);

    return(PAMSTATUS_OK);
}


PAMSTATUS PAMGlobalClose(PAMGHANDLE hGlobal)
{
#if defined(SYM_NTK)

    SymInterlockedDecrement ( &lPamGlobalMutexUsage );

#endif

	PExcludeClose(hGlobal->sig_exclude_info);
	global_close_exclude(&(hGlobal->exclude_info));
	global_close_search(hGlobal);

    PAMMemoryFree(hGlobal);

	return(PAMSTATUS_OK);
}

PAMSTATUS PAMScanFile(PAMLHANDLE hLocal,
                      HFILE hFile,
					  WORD wFlags,
                      LPWORD lpVirusID)
{
	WORD            wNumEntry, wFoundString, i;
    DWORD           dwNumIterations;

#ifdef USE_CACHE

    DWORD           dwMaxIter;

#endif // USE_CACHE

    /* check to see if we're scanning a SYS file.  If so, it has two entry
       points instead of the usual 1.

       NOTE: Currently, memory is reset after each entry-point execution.
             This may miss some viri?
    */

    if (wFlags & FLAG_SYS_FILE)
        wNumEntry = 2;
	else
        wNumEntry = 1;

    *lpVirusID = NOT_FOUND;
    wFoundString = NOT_FOUND;

#ifdef USE_CACHE

    dwMaxIter = 0;                      // store max iterations for cache stuff

    // reset locacl CPU state storage for cache.  Placing 0 in dwLRUIter
    // indicates that we have not yet stored the CPU state for this file...

    hLocal->stTempEntry.dwLRUIter = 0;  // for LRU CPU state cache

#endif

    // set up prefetch stuff...

    hLocal->CPU.prefetch.wCurrentRequest = PREFETCH_32_BYTE;


	do
    {
        hLocal->CPU.prefetch.wNextRequest = PREFETCH_NO_REQUEST;

        for (i=0;i<wNumEntry && wFoundString == NOT_FOUND;i++)
        {
            // wNumEntry is passed in in case we detect a sys file by looking
            // at the header.  If so, we bump up wNumEntry to two so we scan
            // both entrypoints

            if (local_init_cpu(hLocal, hFile, wFlags, i, &wNumEntry) != PAMSTATUS_OK)
                return(PAMSTATUS_FILE_ERROR);

            /* emulate that sample! */

#if defined(IN_AUTOPROTECT)
            hLocal->CPU.max_iteration = hLocal->hGPAM->config_info.dwAPMaxIter;
#else
            hLocal->CPU.max_iteration = hLocal->hGPAM->config_info.dwMaxIter;
#endif

            hLocal->CPU.max_immune_iteration =
                hLocal->hGPAM->config_info.ulMaxImmuneIter;

            dwNumIterations = interpret(hLocal, &wFoundString);

            // only add to our cache if we exceed the max # of iterations
            // during any of our emulation attempts...

#ifdef USE_CACHE

            if (dwNumIterations > dwMaxIter)    // exceeded max iter
                dwMaxIter = dwNumIterations;

#endif // USE_CACHE

#ifdef BORLAND
            printf("dwNumIter = %ld\n",dwNumIterations);
            if (dwNumIterations > 128)
                fprintf(stderr,"\n\niterations = %ld\n",dwNumIterations);
#endif

            /* check to see if an error occured */

            if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
            {
                local_close_cpu(hLocal);

                if (hLocal->dwFlags & LOCAL_FLAG_MEM_ERROR)
                    return (PAMSTATUS_MEM_ERROR);

                return (PAMSTATUS_FILE_ERROR);
            }

            if (dwNumIterations && wFoundString == NOT_FOUND)
            {
                // perform string scan.  reason to believe there may be a virus

                wFoundString = search_buffers_for_string(hLocal);
            }

            local_close_cpu(hLocal);

            // check for errors during search_buffers_for_string...

            if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
            {
                if (hLocal->dwFlags & LOCAL_FLAG_MEM_ERROR)
                    return (PAMSTATUS_MEM_ERROR);

                return (PAMSTATUS_FILE_ERROR);
            }
        }

        hLocal->CPU.prefetch.wCurrentRequest =
            hLocal->CPU.prefetch.wNextRequest;
    }
    while (hLocal->CPU.prefetch.wCurrentRequest != PREFETCH_NO_REQUEST &&
           NOT_FOUND == wFoundString);

    if (wFoundString != NOT_FOUND)
    {
        *lpVirusID = wFoundString;
        return(PAMSTATUS_VIRUS_FOUND);
    }

#ifdef USE_CACHE

    // if we exceeded the acceptable number of iterations, store our temp
    // entry into the cache...

    if (dwMaxIter > hLocal->hGPAM->config_info.dwCacheStoreIter &&
        hLocal->stTempEntry.dwLRUIter != 0)
    {
        // Only want to store if a real lookup in the interpret
        // loop did not fail.

        if (CacheLookupItem(&hLocal->hGPAM->stCache,
                            &hLocal->stTempEntry.stCPUState) ==
            CACHESTATUS_ENTRY_NOT_FOUND)
        {
            // Absolutely no such entry

            CacheInsertItem(&hLocal->hGPAM->stCache,
                            &hLocal->stTempEntry.stCPUState);
        }
    }

#endif // USE_CACHE

	return(PAMSTATUS_OK);
}

void PAMStoreRegisters(PAMLHANDLE hLocal)
{
	WORD wFlags;

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_EAX_OFF,
              hLocal->CPU.preg.D.EAX);

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_EBX_OFF,
              hLocal->CPU.preg.D.EBX);

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_ECX_OFF,
              hLocal->CPU.preg.D.ECX);

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_EDX_OFF,
              hLocal->CPU.preg.D.EDX);

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_ESI_OFF,
              hLocal->CPU.ireg.D.ESI);

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_EDI_OFF,
              hLocal->CPU.ireg.D.EDI);

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_EBP_OFF,
              hLocal->CPU.ireg.D.EBP);

    put_dword(hLocal,
              REPAIR_SEG,
              REPAIR_ESP_OFF,
              hLocal->CPU.ireg.D.ESP);

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_CS_OFF,
             hLocal->CPU.CS);

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_DS_OFF,
             hLocal->CPU.DS);

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_ES_OFF,
             hLocal->CPU.ES);

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_SS_OFF,
             hLocal->CPU.SS);

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_FS_OFF,
             hLocal->CPU.FS);

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_GS_OFF,
             hLocal->CPU.GS);

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_IP_OFF,
             hLocal->CPU.IP);

    wFlags = 0;

    if (hLocal->CPU.FLAGS.O)
        wFlags |= REPAIR_FLAG_O;

    if (hLocal->CPU.FLAGS.D)
        wFlags |= REPAIR_FLAG_D;

    if (hLocal->CPU.FLAGS.I)
        wFlags |= REPAIR_FLAG_I;

    if (hLocal->CPU.FLAGS.T)
        wFlags |= REPAIR_FLAG_T;

    if (hLocal->CPU.FLAGS.S)
        wFlags |= REPAIR_FLAG_S;

    if (hLocal->CPU.FLAGS.Z)
        wFlags |= REPAIR_FLAG_Z;

    if (hLocal->CPU.FLAGS.A)
        wFlags |= REPAIR_FLAG_A;

    if (hLocal->CPU.FLAGS.P)
        wFlags |= REPAIR_FLAG_P;

    if (hLocal->CPU.FLAGS.C)
        wFlags |= REPAIR_FLAG_C;

    // special CPU state info

    if (hLocal->CPU.rep_prefix == PREFIX_REPZ)
        wFlags |= REPAIR_FLAG_REPZ;

    if (hLocal->CPU.rep_prefix == PREFIX_REPNZ)
        wFlags |= REPAIR_FLAG_REPNZ;

    if (hLocal->CPU.op_size_over == TRUE)
        wFlags |= REPAIR_FLAG_OP_OVER;

    if (hLocal->CPU.addr_size_over == TRUE)
        wFlags |= REPAIR_FLAG_ADDR_OVER;

    switch (hLocal->CPU.seg_over)
    {
        case CS_OVER:
            wFlags |= REPAIR_CS_SEG_PREFIX;
            break;

        case DS_OVER:
            wFlags |= REPAIR_DS_SEG_PREFIX;
            break;

        case ES_OVER:
            wFlags |= REPAIR_ES_SEG_PREFIX;
            break;

        case FS_OVER:
            wFlags |= REPAIR_FS_SEG_PREFIX;
            break;

        case GS_OVER:
            wFlags |= REPAIR_GS_SEG_PREFIX;
            break;

        case SS_OVER:
            wFlags |= REPAIR_SS_SEG_PREFIX;
            break;
    }

    put_word(hLocal,
             REPAIR_SEG,
             REPAIR_FLAGS_OFF,
             wFlags);
}

PAMSTATUS PAMRepairFile(PAMLHANDLE hLocal,
                        LPTSTR szDataFile,
                        LPTSTR lpszFileName,
                        LPTSTR lpszWorkFileName,
						WORD wVirusID,
                        WORD wFlags)
{
	WORD            wSigNum, wFoundSig, wEntryToRepair;
	WORD            wResult, wOrigCS, wOrigIP;
	PAMSTATUS       pamStatus;
	DWORD           dwFileSize;
	sig_list_type   stSigList;
    HFILE           hFile;
    DFSTATUS        dfTemp;
    DATAFILEHANDLE  hDataFile;


    // first open our MASTER data file

    dfTemp = DFOpenDataFile(szDataFile,
                            READ_ONLY_FILE,
                            &hDataFile);

    // error opening our DATAFILE

    if (DFSTATUS_OK != dfTemp)
    {
        if (DFSTATUS_MEM_ERROR == dfTemp)
            return(PAMSTATUS_MEM_ERROR);
        else
            return(PAMSTATUS_FILE_ERROR);
	}

	// first try to load up repair information!

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Locating repair information for ERS index %04X\n",
                   wVirusID);
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

    pamStatus = load_repair_info(hLocal,
                                 hDataFile,
                                 wVirusID,
                                 &stSigList);

	if (pamStatus != PAMSTATUS_OK)
    {

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Failed to locate repair information\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

        DFCloseDataFile(hDataFile);

		return(pamStatus);
    }

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Located repair information\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

    // set up the entry point number (strategy or interrupt) regardless of
    // whether or not we're working on a SYS file.  It has no effect for
    // COM or EXE files, and it needs to be initialized for SYS files...

    if (stSigList.repair_bundle_info.wFlags & REPAIR_STRATEGY)
        wEntryToRepair = 0;
    else
        wEntryToRepair = 1;

    if (stSigList.repair_bundle_info.wFlags & REPAIR_PREFETCH_8)
        hLocal->CPU.prefetch.wCurrentRequest = PREFETCH_8_BYTE;
    else if (stSigList.repair_bundle_info.wFlags & REPAIR_PREFETCH_16)
        hLocal->CPU.prefetch.wCurrentRequest = PREFETCH_16_BYTE;
    else if (stSigList.repair_bundle_info.wFlags & REPAIR_PREFETCH_32)
        hLocal->CPU.prefetch.wCurrentRequest = PREFETCH_32_BYTE;
    else
        hLocal->CPU.prefetch.wCurrentRequest = PREFETCH_0_BYTE;

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            if (wEntryToRepair == 0)
                printf("ERS: Main(Strategy) entrypoint selected\n");
            else
                printf("ERS: Main(Interrupt) entrypoint selected\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

	hFile = PAMFileOpen(lpszFileName, READ_ONLY_FILE);
	if (hFile == (HFILE)-1)
	{
		DFCloseDataFile(hDataFile);

		return(PAMSTATUS_FILE_ERROR);
	}

	if (local_init_cpu(hLocal,
					   hFile,
					   wFlags,
					   wEntryToRepair,
					   NULL) != PAMSTATUS_OK)
	{
		DFCloseDataFile(hDataFile);

		PAMFileClose(hFile);
		return(PAMSTATUS_FILE_ERROR);
	}

	// emulate that sample!  But first, save the start CS & IP of the sample
	// so we can provide it to our repair code

	wOrigCS = hLocal->CPU.CS;
	wOrigIP = hLocal->CPU.IP;

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Initial CS:IP = %04X:%04X\n",wOrigCS, wOrigIP);
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

	// make sure to set REPAIR_DECRYPT status after calling local_init_cpu
	// as this resets the in_repair flag to FALSE itself!

    // reset in_repair status

    hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_NO_CAND_LEFT;
    hLocal->dwFlags |= (LOCAL_FLAG_REPAIR_DECRYPT |
                        LOCAL_FLAG_IMMUNE_EXCLUSION_PERM);

    if (stSigList.repair_bundle_info.wNumSigs != 0)
    {
		wFoundSig = repair_interpret(hLocal, &stSigList, &wSigNum);

        if (wFoundSig)
            hLocal->CPU.IP--;

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            if (wFoundSig == TRUE)
                printf("ERS: Hit stopper signature #0x%X.  CS:IP = %04X:%04X\n",
                       wSigNum,
                       hLocal->CPU.CS,
                       hLocal->CPU.IP);
            else
                printf("ERS: Never hit stopper signature\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////
    }
	else
	{
		wFoundSig = TRUE;               // no sigs, automatically go into
		wSigNum = 0xFFFFU;              // repair code emulation

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: No stop signatures specified\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////
	}

	/* check to see if an error occured */

	if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
	{
		// all errors which would occur at this level are file errors

		DFCloseDataFile(hDataFile);

		local_close_cpu(hLocal);
		PAMFileClose(hFile);

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Error during decryption-emulation phase\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

        if (hLocal->dwFlags & LOCAL_FLAG_MEM_ERROR)
            return (PAMSTATUS_MEM_ERROR);

        return (PAMSTATUS_FILE_ERROR);
	}

	if (FALSE == wFoundSig)             // no repair information found!
		wSigNum = 0xFFFFU;              // indicate this with FFFF sig num

	// load our repair code now

    pamStatus = load_repair_code(hLocal, hDataFile, &stSigList);

    // done with our data file...

    DFCloseDataFile(hDataFile);

	if (pamStatus != PAMSTATUS_OK)
	{
///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Error during foundation/overlay loading\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

		local_close_cpu(hLocal);
        PAMFileClose(hFile);
		return(pamStatus);
	}

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Foundation/overlay successfully loaded\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

	hLocal->repair_result = 0;
    hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_WRITE_THRU;   /* no write default */

	// set up the registers for the repair code to use!
	//
	// ES:DI = CS:IP where signature was found
	// DX:AX = size of infected host file on DISK
	// CX    = signature number which was located (FFFF means no signature)
	// SS:SP = F000:FF00 (right at end of overlay code)
	// FS:SI = CS:IP entrypoint of program (originally)

	// F000:FED0 = regs: EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP
	//                   CS, DS, ES, SS, FD, GS, IP, FLAGS

	PAMStoreRegisters(hLocal);

	hLocal->CPU.ES          = hLocal->CPU.CS;
    hLocal->CPU.ireg.X.DI   = hLocal->CPU.IP;

    // Make sure to only decrement our IP if we interpreted.  If we did so then
    // we had to fetch the first byte of the last instruction which would place
    // our IP 1 byte too far.

    dwFileSize = PAMFileLength(hLocal->CPU.stream);

    // check for error...

    if (dwFileSize == (DWORD)-1)
    {
		local_close_cpu(hLocal);
        PAMFileClose(hFile);
		return(PAMSTATUS_FILE_ERROR);
    }

	hLocal->CPU.preg.X.DX   = (WORD)(dwFileSize >> 16);
	hLocal->CPU.preg.X.AX   = (WORD)(dwFileSize & 0xFFFFU);

	hLocal->CPU.preg.X.CX   = wSigNum;

	// set the stack up right below the first 256 bytes from the TOF of the
	// infected host file.

	hLocal->CPU.SS          = REPAIR_SEG;
	hLocal->CPU.ireg.X.SP   = REPAIR_STACK_OFF;

    hLocal->CPU.CS          = REPAIR_SEG;
	hLocal->CPU.IP          = REPAIR_OVERLAY_OFF;

	hLocal->CPU.FS          = wOrigCS;
	hLocal->CPU.ireg.X.SI	= wOrigIP;

    // make sure to reset all segment and address and rep overrides!

    reset_seg_over(hLocal);     /* in the case of segment prefixes */
										/* this is not reset. */

    reset_rep_over(hLocal);     /* reset REPNZ/REPZ prefix overrides */

    reset_32_bit_over(hLocal);  /* forget about 32 bit stuff */

	reset_prefetch_queue(hLocal);		/* reset the queue! */

    // default: no prefetch queue when executing repair (overlay/foundation)
    // code

    hLocal->CPU.prefetch.wCurrentRequest = PREFETCH_0_BYTE;

    // now copy our file's first 256 bytes into the VM...
	// throw away wResult, pamStatus has what we need to know

    pamStatus = copy_bytes_to_vm(hLocal,
                                 256,
                                 0,
                                 hLocal->CPU.stream,
                                 REPAIR_SEG,
                                 REPAIR_HOST_TOF_OFF,
                                 &wResult);

    if (pamStatus != PAMSTATUS_OK ||
        (hLocal->dwFlags & LOCAL_FLAG_ERROR))
    {
        local_close_cpu(hLocal);
        PAMFileClose(hFile);

        if (hLocal->dwFlags & LOCAL_FLAG_MEM_ERROR)
            return (PAMSTATUS_MEM_ERROR);
        else
            return (PAMSTATUS_FILE_ERROR);
    }

	// now open our host so we can repair it

    hLocal->hRepairFile = PAMFileOpen(lpszWorkFileName,READ_WRITE_FILE);

	if ((HFILE)-1 == hLocal->hRepairFile)
	{
///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Error opening backup repair file\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

        local_close_cpu(hLocal);
        PAMFileClose(hFile);
		return(PAMSTATUS_FILE_ERROR);
	}

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Loaded backup repair file\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

	// emulate our repair code now

    hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_REPAIR_DECRYPT;
    hLocal->dwFlags |= (LOCAL_FLAG_REPAIR_REPAIR |
                        LOCAL_FLAG_IMMUNE_EXCLUSION_PERM);

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.D = hLocal->CPU.FLAGS.I =
    hLocal->CPU.FLAGS.T = hLocal->CPU.FLAGS.S = hLocal->CPU.FLAGS.Z =
    hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.P = hLocal->CPU.FLAGS.C = FALSE;

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            printf("ERS: Starting overlay execution\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

    repair_interpret(hLocal, &stSigList, &wResult);

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
        extern  BOOL    gbDebugRepair;

        if (gbDebugRepair == TRUE)
        {
            if (wResult == REPAIR_FAILURE)
                printf("ERS: Repair failed\n");
            else
                printf("ERS: Repair successful\n");
        }
    }
#endif
///////////////////////////////////////////////////////////////////////////////

	// done emulating repair code

    hLocal->dwFlags &= ~(DWORD)(LOCAL_FLAG_REPAIR_REPAIR |
                                LOCAL_FLAG_IMMUNE_EXCLUSION_PERM);

	// now close our repaired program since we're done with it.

    PAMFileClose(hLocal->hRepairFile);

	// now close the virtual CPU...

	local_close_cpu(hLocal);
    PAMFileClose(hFile);

    if (hLocal->dwFlags & LOCAL_FLAG_MEM_ERROR)
        return (PAMSTATUS_MEM_ERROR);

    if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
        return (PAMSTATUS_FILE_ERROR);

	if (wResult == REPAIR_FAILURE)
		return(PAMSTATUS_NO_REPAIR);

	return(PAMSTATUS_OK);
}
