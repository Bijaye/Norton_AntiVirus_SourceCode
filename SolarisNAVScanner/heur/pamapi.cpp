// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/pamapi.cpv   1.5   22 Aug 1997 16:53:44   CNACHEN  $
//
// Description:
//
//  This file contains the top-level PAM API functions which can be used
//  to initialize the PAM system, scan for viruses and repair virus infected
//  files and boot records.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/pamapi.cpv  $
// 
//    Rev 1.5   22 Aug 1997 16:53:44   CNACHEN
// Removed mutexes for cache, since there is no cache in heur.
// 
//    Rev 1.4   09 Jul 1997 16:34:58   CNACHEN
// Added support for 3 config levels for all major config options.
// 
// 
//    Rev 1.3   06 Jun 1997 15:32:02   CNACHEN
// Does not terminate after finding INT 21, AH=4C now; continues emulating until
// (1) we start executing the host or (2) we start searching for new regions to
// execute that were never branched to.
// 
//    Rev 1.2   29 May 1997 13:30:32   CNACHEN
// Added support for heuristic sensitivity level
// 
//    Rev 1.1   23 May 1997 12:21:54   CNACHEN
// Added check to see if we want to stop emulating right after
// local_init_cpu is done.
// 
//    Rev 1.0   14 May 1997 17:23:26   CNACHEN
// Initial revision.
// 
//    Rev 1.32   21 Mar 1997 14:04:08   CNACHEN
// Updated to support BIG endian and picky non-Intel machines...
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

#include "heurapi.h"

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

    /* now its time to load up the low memory data area into the read-only
       32K buffer */

    dfTemp = DFLookUp(hDataFile,
                      ID_BLOOD_LOW_DATA_AREA,
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

    /* now its time to load the PAM configuration options */


    dfTemp = DFLookUp(hDataFile,
                      ID_BLOOD_CONFIG_OPTIONS,
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
        PAMFileRead(hFile,&(pTemp->config_info[0]),sizeof(PAMConfigType)) !=
        sizeof(PAMConfigType))
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

#ifdef BIG_ENDIAN

    // endianize our config data...
    {
        pTemp->config_info[0].wFillWord =
            WENDIAN(pTemp->config_info[0].wFillWord);

        pTemp->config_info[0].wMOVSWriteThreshold =
            WENDIAN(pTemp->config_info[0].wMOVSWriteThreshold);
        pTemp->config_info[0].wIPEOFThreshold =
            WENDIAN(pTemp->config_info[0].wIPEOFThreshold);
        pTemp->config_info[0].wReadWriteBaitThreshold =
            WENDIAN(pTemp->config_info[0].wReadWriteBaitThreshold);
        pTemp->config_info[0].wSuspiciousOpcodeThreshold =
            WENDIAN(pTemp->config_info[0].wSuspiciousOpcodeThreshold);
        pTemp->config_info[0].wSuspiciousIntThreshold =
            WENDIAN(pTemp->config_info[0].wSuspiciousIntThreshold);
        pTemp->config_info[0].wDisreguardCleanBehaviorInstrCount =
            WENDIAN(pTemp->config_info[0].wDisreguardCleanBehaviorInstrCount); // NEW_UNIX
        pTemp->config_info[0].wAssumedVirusSize =
            WENDIAN(pTemp->config_info[0].wAssumedVirusSize);
        pTemp->config_info[0].wMinEXEVirusSize =
            WENDIAN(pTemp->config_info[0].wMinEXEVirusSize);
        pTemp->config_info[0].wMaxEncryptedVirusIterations =
            WENDIAN(pTemp->config_info[0].wMaxEncryptedVirusIterations);
        pTemp->config_info[0].wMinEncryptedVirusIterations =
            WENDIAN(pTemp->config_info[0].wMinEncryptedVirusIterations);
        pTemp->config_info[0].wMinModifiedByteRange  =
            WENDIAN(pTemp->config_info[0].wMinModifiedByteRange);
        pTemp->config_info[0].wMaxIterForEPAdjust =
            WENDIAN(pTemp->config_info[0].wMaxIterForEPAdjust);
        pTemp->config_info[0].wMaxIterForRangeAdjust =
            WENDIAN(pTemp->config_info[0].wMaxIterForRangeAdjust);

        pTemp->config_info[0].wNormalInterruptVectorMax =
            WENDIAN(pTemp->config_info[0].wNormalInterruptVectorMax);
        pTemp->config_info[0].wNormalLegitInt21SubFuncMax =
            WENDIAN(pTemp->config_info[0].wNormalLegitInt21SubFuncMax);
        pTemp->config_info[0].wMinCallSubroutineSize =
            WENDIAN(pTemp->config_info[0].wMinCallSubroutineSize);

        pTemp->config_info[0].wMOVSWriteClosePercent =
            WENDIAN(pTemp->config_info[0].wMOVSWriteClosePercent);
        pTemp->config_info[0].wIPEOFWriteClosePercent =
            WENDIAN(pTemp->config_info[0].wIPEOFWriteClosePercent);
    }
#endif



    if (PAMFileRead(hFile,&(pTemp->config_info[1]),sizeof(PAMConfigType)) !=
        sizeof(PAMConfigType))
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }


#ifdef BIG_ENDIAN

    // endianize our config data...
    {
        pTemp->config_info[1].wFillWord =
            WENDIAN(pTemp->config_info[1].wFillWord);

        pTemp->config_info[1].wMOVSWriteThreshold =
            WENDIAN(pTemp->config_info[1].wMOVSWriteThreshold);
        pTemp->config_info[1].wIPEOFThreshold =
            WENDIAN(pTemp->config_info[1].wIPEOFThreshold);
        pTemp->config_info[1].wReadWriteBaitThreshold =
            WENDIAN(pTemp->config_info[1].wReadWriteBaitThreshold);
        pTemp->config_info[1].wSuspiciousOpcodeThreshold =
            WENDIAN(pTemp->config_info[1].wSuspiciousOpcodeThreshold);
        pTemp->config_info[1].wSuspiciousIntThreshold =
            WENDIAN(pTemp->config_info[1].wSuspiciousIntThreshold);
        pTemp->config_info[1].wDisreguardCleanBehaviorInstrCount =
            WENDIAN(pTemp->config_info[1].wDisreguardCleanBehaviorInstrCount); // NEW_UNIX
        pTemp->config_info[1].wAssumedVirusSize =
            WENDIAN(pTemp->config_info[1].wAssumedVirusSize);
        pTemp->config_info[1].wMinEXEVirusSize =
            WENDIAN(pTemp->config_info[1].wMinEXEVirusSize);
        pTemp->config_info[1].wMaxEncryptedVirusIterations =
            WENDIAN(pTemp->config_info[1].wMaxEncryptedVirusIterations);
        pTemp->config_info[1].wMinEncryptedVirusIterations =
            WENDIAN(pTemp->config_info[1].wMinEncryptedVirusIterations);
        pTemp->config_info[1].wMinModifiedByteRange  =
            WENDIAN(pTemp->config_info[1].wMinModifiedByteRange);
        pTemp->config_info[1].wMaxIterForEPAdjust =
            WENDIAN(pTemp->config_info[1].wMaxIterForEPAdjust);
        pTemp->config_info[1].wMaxIterForRangeAdjust =
            WENDIAN(pTemp->config_info[1].wMaxIterForRangeAdjust);

        pTemp->config_info[1].wNormalInterruptVectorMax =
            WENDIAN(pTemp->config_info[1].wNormalInterruptVectorMax);
        pTemp->config_info[1].wNormalLegitInt21SubFuncMax =
            WENDIAN(pTemp->config_info[1].wNormalLegitInt21SubFuncMax);
        pTemp->config_info[1].wMinCallSubroutineSize =
            WENDIAN(pTemp->config_info[1].wMinCallSubroutineSize);

        pTemp->config_info[1].wMOVSWriteClosePercent =
            WENDIAN(pTemp->config_info[1].wMOVSWriteClosePercent);
        pTemp->config_info[1].wIPEOFWriteClosePercent =
            WENDIAN(pTemp->config_info[1].wIPEOFWriteClosePercent);
    }
#endif

    if (PAMFileRead(hFile,&(pTemp->config_info[2]),sizeof(PAMConfigType)) !=
        sizeof(PAMConfigType))
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }


#ifdef BIG_ENDIAN

    // endianize our config data...
    {
        pTemp->config_info[2].wFillWord =
            WENDIAN(pTemp->config_info[2].wFillWord);

        pTemp->config_info[2].wMOVSWriteThreshold =
            WENDIAN(pTemp->config_info[2].wMOVSWriteThreshold);
        pTemp->config_info[2].wIPEOFThreshold =
            WENDIAN(pTemp->config_info[2].wIPEOFThreshold);
        pTemp->config_info[2].wReadWriteBaitThreshold =
            WENDIAN(pTemp->config_info[2].wReadWriteBaitThreshold);
        pTemp->config_info[2].wSuspiciousOpcodeThreshold =
            WENDIAN(pTemp->config_info[2].wSuspiciousOpcodeThreshold);
        pTemp->config_info[2].wSuspiciousIntThreshold =
            WENDIAN(pTemp->config_info[2].wSuspiciousIntThreshold);
        pTemp->config_info[2].wDisreguardCleanBehaviorInstrCount =
            WENDIAN(pTemp->config_info[2].wDisreguardCleanBehaviorInstrCount); // NEW_UNIX
        pTemp->config_info[2].wAssumedVirusSize =
            WENDIAN(pTemp->config_info[2].wAssumedVirusSize);
        pTemp->config_info[2].wMinEXEVirusSize =
            WENDIAN(pTemp->config_info[2].wMinEXEVirusSize);
        pTemp->config_info[2].wMaxEncryptedVirusIterations =
            WENDIAN(pTemp->config_info[2].wMaxEncryptedVirusIterations);
        pTemp->config_info[2].wMinEncryptedVirusIterations =
            WENDIAN(pTemp->config_info[2].wMinEncryptedVirusIterations);
        pTemp->config_info[2].wMinModifiedByteRange  =
            WENDIAN(pTemp->config_info[2].wMinModifiedByteRange);
        pTemp->config_info[2].wMaxIterForEPAdjust =
            WENDIAN(pTemp->config_info[2].wMaxIterForEPAdjust);
        pTemp->config_info[2].wMaxIterForRangeAdjust =
            WENDIAN(pTemp->config_info[2].wMaxIterForRangeAdjust);

        pTemp->config_info[2].wNormalInterruptVectorMax =
            WENDIAN(pTemp->config_info[2].wNormalInterruptVectorMax);
        pTemp->config_info[2].wNormalLegitInt21SubFuncMax =
            WENDIAN(pTemp->config_info[2].wNormalLegitInt21SubFuncMax);
        pTemp->config_info[2].wMinCallSubroutineSize =
            WENDIAN(pTemp->config_info[2].wMinCallSubroutineSize);

        pTemp->config_info[2].wMOVSWriteClosePercent =
            WENDIAN(pTemp->config_info[2].wMOVSWriteClosePercent);
        pTemp->config_info[2].wIPEOFWriteClosePercent =
            WENDIAN(pTemp->config_info[2].wIPEOFWriteClosePercent);
    }
#endif


    //////////////////////////////////////////////////////////////////////////
    // now load up our pcode
    //////////////////////////////////////////////////////////////////////////

    dfTemp = DFLookUp(hDataFile,
                      ID_BLOOD_PCODE,
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
        PAMFileRead(hFile,&pTemp->wHeurStreamLen,sizeof(WORD)) != sizeof(WORD))
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

    pTemp->wHeurStreamLen = WENDIAN(pTemp->wHeurStreamLen); // NEW_UNIX


    pTemp->lpbyHeurCode = (LPBYTE)PAMMemoryAlloc(pTemp->wHeurStreamLen);
    if (pTemp->lpbyHeurCode == NULL)
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_MEM_ERROR);
    }


    if (PAMFileRead(hFile,
                    pTemp->lpbyHeurCode,
                    pTemp->wHeurStreamLen) != pTemp->wHeurStreamLen)
    {
        DFCloseDataFile(hDataFile);
        PAMMemoryFree(pTemp->lpbyHeurCode);
        PAMMemoryFree(pTemp);

        return(PAMSTATUS_FILE_ERROR);
    }

   

    /* success. break out the champaigne (is that spelled right?) */

    DFCloseDataFile(hDataFile);

    /* now load up the signature-based exclusions (we weren't quite done
       before) */

    pamTemp = PExcludeInit(&hExclude,szDataFile);


	if (pamTemp != PAMSTATUS_OK)
    {
        PAMMemoryFree(pTemp->lpbyHeurCode);
        PAMMemoryFree(pTemp);

        return(pamTemp);
    }


    pTemp->sig_exclude_info = hExclude;

    // finally set our handle up...

    *hPtr = pTemp;

    return(PAMSTATUS_OK);
}


/* this can be called multiple times to provide many threads with emulator
   capabilities */

PAMSTATUS PAMLocalInit
(
    PAMGHANDLE hGPAM,
    LPPAMLHANDLE hLPtr
)
{
	PAMLHANDLE          hTemp;
	PAMSTATUS           pamStatus;

	*hLPtr = NULL;

	/* allocate memory for the local pam context info */

    hTemp = (PAMLHANDLE)PAMMemoryAlloc(sizeof(LocalPAMType));

    if (NULL == hTemp)
        return(PAMSTATUS_MEM_ERROR);

	/* initialize all paging stuff */

    pamStatus = global_init_paging(NULL,hTemp);

	if (pamStatus != PAMSTATUS_OK)
	{
		PAMMemoryFree(hTemp);

		return(pamStatus);
	}

	/* initialize our local exclusion bitfields */

	hTemp->hGPAM = hGPAM;

    // the low memory area has been read into memory and does not need to
    // be accessed via the file handle!

    hTemp->CPU.low_stream = (HFILE)-1;

    //////////////////////////////////////////////////////////////////////////
	// Heuristics
	//////////////////////////////////////////////////////////////////////////

    EMInit(&hTemp->hGPAM->config_info[0],
           &hTemp->stEM);
    BMReset(&hTemp->hGPAM->config_info[0],
            &hTemp->stBM,
            0);             // 0 means lowest heuristic level as default
                            // this is actually reset later when PAMScanFileH
                            // is called

	//////////////////////////////////////////////////////////////////////////
	// Heuristics
    //////////////////////////////////////////////////////////////////////////

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

    //////////////////////////////////////////////////////////////////////////
    // Heuristics
    //////////////////////////////////////////////////////////////////////////

    EMReset(&hLocal->stEM);

    //////////////////////////////////////////////////////////////////////////
    // Heuristics
    //////////////////////////////////////////////////////////////////////////


    PAMMemoryFree(hLocal);

    return(PAMSTATUS_OK);
}


PAMSTATUS PAMGlobalClose(PAMGHANDLE hGlobal)
{
	PExcludeClose(hGlobal->sig_exclude_info);
    PAMMemoryFree(hGlobal->lpbyHeurCode);
    PAMMemoryFree(hGlobal);

	return(PAMSTATUS_OK);
}



PAMSTATUS PAMScanFileH(PAMLHANDLE   hLocal,
                       HFILE        hFile,
                       int          nHeuristicLevel,
                       LPBOOL       lpbVirusFound,
                       LPWORD       lpwVirusID)
{
    BOOL            bReset, bContinue;

    if (nHeuristicLevel < 1)
        hLocal->nCurHeurLevel = 0;
    else if (nHeuristicLevel > 3)
        hLocal->nCurHeurLevel = 2;
    else
        hLocal->nCurHeurLevel = nHeuristicLevel - 1;

    *lpbVirusFound = FALSE;

    // set up prefetch stuff...

    hLocal->CPU.prefetch.wCurrentRequest = PREFETCH_32_BYTE;
    hLocal->CPU.prefetch.wNextRequest = PREFETCH_NO_REQUEST;

    // reset emulation manager/behavior manager

    EMReset(&hLocal->stEM);
    BMReset(&hLocal->hGPAM->config_info[hLocal->nCurHeurLevel],
            &hLocal->stBM,
            nHeuristicLevel);

    if (local_init_cpu(hLocal, hFile) != PAMSTATUS_OK)
        return(PAMSTATUS_FILE_ERROR);

    // see if we want to stop emulating...

    if (BMStopEmulating(&hLocal->stBM) == TRUE)
    {
        local_close_cpu(hLocal);

        return(PAMSTATUS_OK);
    }

    /* emulate that sample! */

    bContinue = HeuristicDecryptInterpret(hLocal, &bReset);

    if (bReset == TRUE)
    {
        local_close_cpu(hLocal);

        // reset emulation manager/behavior manager

        EMReset(&hLocal->stEM);
        BMReset(&hLocal->hGPAM->config_info[hLocal->nCurHeurLevel],
                &hLocal->stBM,
                nHeuristicLevel);

        if (local_init_cpu(hLocal,
                           hFile) != PAMSTATUS_OK)
            return(PAMSTATUS_FILE_ERROR);

        BMSubmitOriginalEntry(&hLocal->stBM,TRUE);
    }

    if (bContinue == TRUE)
    {
        if (HeuristicInterpret(hLocal,
                               lpbVirusFound,
                               lpwVirusID) == FALSE)
            return(PAMSTATUS_FILE_ERROR);
    }
    else
        *lpbVirusFound = FALSE;

    /* check to see if an error occured */

    if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
    {
        local_close_cpu(hLocal);

        if (hLocal->dwFlags & LOCAL_FLAG_MEM_ERROR)
            return (PAMSTATUS_MEM_ERROR);

        return (PAMSTATUS_FILE_ERROR);
    }

    local_close_cpu(hLocal);

	return(PAMSTATUS_OK);
}

