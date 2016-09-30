// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/avenge.cpv   1.8   15 Sep 1997 14:33:22   THOLMES  $
//
// Description:
//  Contains most of the functions implementing the avenge API.
//
// Contains:
//  EngGlobalInit()
//  EngGlobalClose()
//  EngGlobalMemoryInit()
//  EngGlobalMemoryClose()
//  EngGlobalBootInit()
//  EngGlobalBootClose()
//  EngGlobalFileInit()
//  EngGlobalFileClose()
//  EngLocalInit()
//  EngLocalClose()
//  EngScanFile()
//  EngScanMemory()
//  EngScanBoot()
//  EngRepairFile()
//  EngRepairBoot()
//
// See Also:
//  delentry.c, getinfo.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/avenge.cpv  $
// 
//    Rev 1.8   15 Sep 1997 14:33:22   THOLMES
// Moved variables in EngGlobalInit to the top of the function so it will
// build for WatCom.
// 
//    Rev 1.7   12 Sep 1997 13:47:18   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.6   13 Aug 1997 13:56:30   CNACHEN
// Added new function EngScanBooBuffer to do old style boot scanning like
// certlib.
//    Rev 1.5.1.4   27 Aug 1997 18:12:38   MKEATIN
// Make sure any files we attemp to close have been opened first.
//
//    Rev 1.5.1.3   27 Aug 1997 18:04:10   MKEATIN
// Close any open dat files after pinging.
//
//    Rev 1.5.1.2   27 Aug 1997 17:45:34   MKEATIN
// We now ping virscan1-4 to verify that they exist in EngGlobalInit().
//
//    Rev 1.5.1.1   13 Aug 1997 14:11:04   CNACHEN
// Added EngScanBooBuffer to scan the way certlib used to...
//
//    Rev 1.5   17 Jul 1997 17:46:56   DDREW
// Moved some "ifndef SYM_NLM's" around
//
//    Rev 1.4   08 Jul 1997 17:19:58   MKEATIN
// Passed NULL as the lpvProvidedFileInfo parameters in various functions.
// The old default NULL in the old prototype won't compile for the NLM
// C compiler.
//
//    Rev 1.3   05 Jun 1997 18:35:18   MKEATIN
// Turned off the GENG_FLAG_BOOT_INFO_LOADED bit after calling
// EngFreeResidentBootInfo().
//
//    Rev 1.2   14 May 1997 15:27:52   MKEATIN
// Run EngBootLoad() before EngLoadResidentBootInfo() for allocation of
// structers.
//
//    Rev 1.1   13 May 1997 18:26:56   MKEATIN
// Ported Carey's boot caching code.
//
//    Rev 1.0   18 Apr 1997 13:37:54   MKEATIN
// Initial revision.
//
//    Rev 1.17   23 Oct 1996 11:19:04   DCHI
// Added IPC callback support.
//
//    Rev 1.16   01 Oct 1996 17:04:54   CNACHEN
// More PC way of handling errors in closing.
//
//    Rev 1.15   09 Sep 1996 11:54:22   DCHI
// Added return of error in case of failure on CRC load.
//
//    Rev 1.14   17 Jun 1996 15:09:28   RAY
// Various bug fixes to Boot Repair stuff.
//
//    Rev 1.13   04 Jun 1996 17:40:50   DCHI
// Changes to conform with new header files in global include.
//
//    Rev 1.12   04 Jun 1996 13:43:30   RAY
// Reorganized most of BootRep and added support for SECTORs.
//
//    Rev 1.11   29 May 1996 18:07:56   DCHI
// Changed lpvFileInfo to lpvInfo for file objects.
//
//    Rev 1.10   28 May 1996 14:31:02   DCHI
// Updated to support memory objects.
//
//    Rev 1.9   23 May 1996 18:02:00   DCHI
// Added code for initializing file cache for ALG scanning.
//
//    Rev 1.8   21 May 1996 16:51:40   RAY
// Changed from Physical to Linear Objects and Callbacks.
//
//    Rev 1.7   17 May 1996 15:51:14   CNACHEN
// Changed RepairFile to indicate that repair had failed as default.
//
//    Rev 1.6   17 May 1996 15:37:28   CNACHEN
// File repair gets/set date and time outside of inner repair.  Also fixed
// file close bug (on repair error).
//
//
//    Rev 1.5   17 May 1996 14:15:50   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
//
//    Rev 1.4   17 May 1996 13:43:54   RAY
// Added EngRepairBoot()
//
//    Rev 1.3   17 May 1996 12:19:28   DCHI
// Added MEMSETting of scanning buffers to 0 on local close.
//
//    Rev 1.2   16 May 1996 14:12:58   CNACHEN
// Changed to use new AVENGE?.H headers.
//
//    Rev 1.1   15 May 1996 16:11:02   DCHI
// Corrected EngGlobalFileInit() to always attempt to load Wild ALG sigs.
//
//    Rev 1.0   13 May 1996 16:29:22   DCHI
// Initial revision.
//
//************************************************************************

#include "avengel.h"

//*************************************************************************
//
// Function:
//  EngGlobalInit()
//
// Parameters:
//  lphGEng             Pointer to handle to global engine context
//  lpGeneralCallBacks  Pointer to general callback structure
//  lpProgressCallBacks Pointer to progress callback structure
//  lpDataFileCallBacks Pointer to data file callback structure
//  lpIPCCallBacks      Pointer to IPC callback structure
//  dwGlobalCookie      Global cookie
//
// Description:
//  Allocates and initializes a global engine context, including
//  initialization of the virus information cache.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalInit
(
    LPHGENG             lphGEng,
    LPGENERALCALLBACKS  lpGeneralCallBacks,
    LPPROGRESSCALLBACKS lpProgressCallBacks,
    LPDATAFILECALLBACKS lpDataFileCallBacks,
    LPIPCCALLBACKS      lpIPCCallBacks,
    DWORD               dwGlobalCookie
)
{
    HGENG hGEng = (HGENG) 0;

    BOOL bDatFilesExist = FALSE;

    LPVOID lpvFileInfo1 = (LPVOID) 0;

    LPVOID lpvFileInfo2 = (LPVOID) 0;

    LPVOID lpvFileInfo3 = (LPVOID) 0;

    LPVOID lpvFileInfo4 = (LPVOID) 0;

    if (lpGeneralCallBacks->MemoryAlloc(sizeof(GENG),
                                        MEM_FIXME,
                                        (LPLPVOID)lphGEng) ==
        CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // set up our callbacks

    hGEng = *lphGEng;

    hGEng->lpDataFileCallBacks = lpDataFileCallBacks;
    hGEng->lpProgressCallBacks = lpProgressCallBacks;
    hGEng->lpGeneralCallBacks = lpGeneralCallBacks;
    hGEng->lpIPCCallBacks = lpIPCCallBacks;

    // record our global cookie

    hGEng->dwGlobalCookie = dwGlobalCookie;

    // set all flags to uninitialized

    hGEng->dwInitStatus = GENG_FLAG_UNINITIALIZED;

    // reset the info caches

    if (EngCacheInit(&hGEng->stVirusInfoCache,lpIPCCallBacks) != ENGSTATUS_OK)
    {
        // error. free our memory;

        lpGeneralCallBacks->MemoryFree(hGEng);

        return(ENGSTATUS_ERROR);
    }

    // Ping VIRSCAN?.DAT 1 thru 4 for some limited validation

    if ((EngOpenDataFile(hGEng->lpDataFileCallBacks,
                         DATA_FILE_ID_VIRSCAN1DAT,
                         FILE_OPEN_READ_ONLY,
                         &lpvFileInfo1) == DFSTATUS_ERROR) ||

        (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                         DATA_FILE_ID_VIRSCAN2DAT,
                         FILE_OPEN_READ_ONLY,
                         &lpvFileInfo2) == DFSTATUS_ERROR) ||

        (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                         DATA_FILE_ID_VIRSCAN3DAT,
                         FILE_OPEN_READ_ONLY,
                         &lpvFileInfo3) == DFSTATUS_ERROR) ||

        (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                         DATA_FILE_ID_VIRSCAN4DAT,
                         FILE_OPEN_READ_ONLY,
                         &lpvFileInfo4) == DFSTATUS_ERROR))
    {
        bDatFilesExist = FALSE;
    }
    else
    {
        bDatFilesExist = TRUE;
    }

    if (lpvFileInfo1)
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo1);
    if (lpvFileInfo2)
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo2);
    if (lpvFileInfo3)
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo3);
    if (lpvFileInfo4)
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo4);

    if (!bDatFilesExist)
        return (ENGSTATUS_ERROR);

    return (ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGlobalClose()
//
// Parameters:
//  hGEng               Handle to global engine context
//
// Description:
//  Closes global engine context and frees any memory allocated
//  for it.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalClose
(
    HGENG               hGEng
)
{
    ENGSTATUS           engStatus = ENGSTATUS_OK;

    // Close the virus info cache

    if (EngCacheClose(&hGEng->stVirusInfoCache) != ENGSTATUS_OK)
    {
        // error.

        engStatus = ENGSTATUS_ERROR;
    }

    // free our local context data

    if (hGEng->lpGeneralCallBacks->MemoryFree(hGEng) == CBSTATUS_ERROR)
    {
        engStatus = ENGSTATUS_ERROR;
    }

    return(engStatus);
}


//*************************************************************************
//
// Function:
//  EngGlobalMemoryInit()
//
// Parameters:
//  hGEng               Handle to global engine context
//
// Description:
//  Initializes global engine context with memory scanning data.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalMemoryInit
(
    HGENG               hGEng
)
{
    if (EngMemoryLoad(hGEng) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // successful initialization of memory scanning data

    hGEng->dwInitStatus |= GENG_FLAG_MEMORY_LOADED;

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGlobalMemoryClose()
//
// Parameters:
//  hGEng               Handle to global engine context
//
// Description:
//  Unloads memory scanning data.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalMemoryClose
(
    HGENG   hGEng
)
{
    if (!(hGEng->dwInitStatus & GENG_FLAG_MEMORY_LOADED))
        return(ENGSTATUS_ERROR);

    if (EngMemoryUnload(hGEng) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // reset memory loaded flag after we unload

    hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_MEMORY_LOADED;

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGlobalFileInit()
//
// Parameters:
//  hGEng               Handle to global engine context
//
// Description:
//  Initializes global engine context with file scanning data.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalFileInit
(
    HGENG   hGEng,
    DWORD   dwFlags
)
{
    if (dwFlags & GENG_INIT_FILE_FLAG_LOAD_ALL)
    {
        if (EngLoadAlgScanData(hGEng->lpDataFileCallBacks,
                               hGEng->lpGeneralCallBacks,
                               SECTION_ID_ALG_ZOO_SIG,
                               &hGEng->stGeneralALGScanInfo) ==
            ENGSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        hGEng->dwInitStatus |= GENG_FLAG_FILE_ALL_LOADED;
    }

    if (EngLoadAlgScanData(hGEng->lpDataFileCallBacks,
                           hGEng->lpGeneralCallBacks,
                           SECTION_ID_ALG_ITW_SIG,
                           &hGEng->stWildALGScanInfo) ==
        ENGSTATUS_ERROR)
    {
        // free any allocated general alg sigs...

        if (hGEng->dwInitStatus & GENG_FLAG_FILE_ALL_LOADED)
        {
            if (EngFreeAlgAux(hGEng->lpGeneralCallBacks,
                          &hGEng->stGeneralALGScanInfo) == ENGSTATUS_OK)
                hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_FILE_ALL_LOADED;
        }

        return(ENGSTATUS_ERROR);
    }
    else
        hGEng->dwInitStatus |= GENG_FLAG_FILE_WILD_LOADED;

    // load CRCs at this time...

    if (EngCRCLoad(hGEng) == ENGSTATUS_ERROR)
    {
        if (hGEng->dwInitStatus & GENG_FLAG_FILE_ALL_LOADED)
        {
            if (EngFreeAlgAux(hGEng->lpGeneralCallBacks,
                              &hGEng->stGeneralALGScanInfo) == ENGSTATUS_OK)
                hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_FILE_ALL_LOADED;
        }

        if (hGEng->dwInitStatus & GENG_FLAG_FILE_WILD_LOADED)
        {
            if (EngFreeAlgAux(hGEng->lpGeneralCallBacks,
                              &hGEng->stWildALGScanInfo) == ENGSTATUS_OK)
                hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_FILE_WILD_LOADED;
        }

        return(ENGSTATUS_ERROR);
    }

    hGEng->dwInitStatus |= GENG_FLAG_FILE_CRC_LOADED;

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGlobalFileClose()
//
// Parameters:
//  hGEng               Handle to global engine context
//
// Description:
//  Unloads file scanning data.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalFileClose
(
    HGENG   hGEng
)
{
    ENGSTATUS   engStatus1, engStatus2, engStatus3;

    engStatus1 = engStatus2 = engStatus3 = ENGSTATUS_OK;

    if (hGEng->dwInitStatus & GENG_FLAG_FILE_ALL_LOADED)
    {
        engStatus1 = EngFreeAlgAux(hGEng->lpGeneralCallBacks,
                                   &hGEng->stGeneralALGScanInfo);

        if (engStatus1 == ENGSTATUS_OK)
            hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_FILE_ALL_LOADED;
    }

    if (hGEng->dwInitStatus & GENG_FLAG_FILE_WILD_LOADED)
    {
        engStatus2 = EngFreeAlgAux(hGEng->lpGeneralCallBacks,
                                   &hGEng->stWildALGScanInfo);

        if (engStatus2 == ENGSTATUS_OK)
            hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_FILE_WILD_LOADED;
    }

    // now free the CRCs...

    if (hGEng->dwInitStatus & GENG_FLAG_FILE_CRC_LOADED)
    {
        engStatus3 = EngCRCUnload(hGEng);

        if (engStatus3 == ENGSTATUS_OK)
            hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_FILE_CRC_LOADED;
    }

    if (engStatus1 == ENGSTATUS_ERROR ||
        engStatus2 == ENGSTATUS_ERROR ||
        engStatus3 == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngLocalInit()
//
// Parameters:
//  hGEng               Handle to global engine context
//  lphLEng             Pointer to handle to local engine context
//  dwLocalCookie       Local cookie
//
// Description:
//  Allocates and initializes a local engine context.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngLocalInit
(
    HGENG       hGEng,
    LPHLENG     lphLEng,
    DWORD       dwLocalCookie
)
{
    HLENG       hLEng;

    if (hGEng->lpGeneralCallBacks->MemoryAlloc(sizeof(LENG),
                                               MEM_FIXME,
                                               (LPLPVOID)lphLEng)
        == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    hLEng = *lphLEng;

    // remember our global context handle

    hLEng->hGEng = hGEng;

    // set up our cookie

    hLEng->dwLocalCookie = dwLocalCookie;

    //////////////////////////////////////////////////////////////////
    //
    // Initialize alg scanner cache buffer of zeroes
    //
    //////////////////////////////////////////////////////////////////

    // Initialize the buffer of zeroes

    MEMSET(hLEng->stAlgScanData.stEngCache.byEOFCache + ENG_EOF_CACHE_SIZE,
           0,
           FSTART_BUFFER_SIZE);

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngLocalClose()
//
// Parameters:
//  hLEng               Handle to local engine context
//
// Description:
//  Closes local engine context and frees any memory allocated
//  for it.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngLocalClose
(
    HLENG hLEng
)
{
    LPGENERALCALLBACKS  lpGeneralCallBacks =
        hLEng->hGEng->lpGeneralCallBacks;

    // Zero buffers

    MEMSET(hLEng,0,sizeof(LENG));

    // free our local context data

    if (lpGeneralCallBacks->MemoryFree(hLEng) ==
        CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngScanFile()
//
// Parameters:
//  hLEng               Handle to local engine context
//  lpFileObject        Pointer to file object to scan
//  dwFlags             Flags
//                          GENG_SCAN_FILE_FLAG_SCAN_ONLY_WILD
//                          GENG_SCAN_FILE_FLAG_SCAN_ALL
//  dwCookie            File scan cookie
//  lpbFoundSig         Pointer to BOOL to store virus found result
//  lphVirus            Pointer to HVIRUS to store virus handle
//
// Description:
//  Scans the file object for file viruses.  Sets *lpbFoundSig to TRUE
//  and *lphVirus to a virus handle if a match is found.  Otherwise,
//  *lpbFoundSig is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

// the file is already open...

ENGSTATUS EngScanFile
(
    HLENG               hLEng,
    LPFILEOBJECT        lpFileObject,
    DWORD               dwFlags,
    DWORD               dwCookie,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
)
{
    int                 nCurEntryPoint = 0, nTotalEntryPoint = 1;
    LPENG_CACHE         lpEngCache;

    // for the time being, don't do anything with the cookie

    (void)dwCookie;

    // no viruses detected to start out with...

    *lphVirus = INVALID_VIRUS_HANDLE;
    *lpbFoundSig = FALSE;

    // zero our NAVEX request buffer...

    hLEng->stAlgScanData.wNAVEXRequestCount = 0;

    //////////////////////////////////////////////////////////////////
    //
    // Initialize alg scanner cache
    //
    //////////////////////////////////////////////////////////////////

    lpEngCache = &(hLEng->stAlgScanData.stEngCache);

    // Initialize file length field

    if (lpFileObject->lpCallBacks->
            FileLength(lpFileObject->lpvInfo,
                       &(lpEngCache->dwFileLength)) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // Initialize rest of cache

    lpEngCache->wNumBytesInEOFCache = 0;
    lpEngCache->wNumOtherFilled = 0;
    lpEngCache->wNextFreeEntry = 0;
    if (lpEngCache->dwFileLength < ENG_EOF_CACHE_SIZE)
        lpEngCache->dwEOFCacheStartOffset = 0;
    else
        lpEngCache->dwEOFCacheStartOffset =
            lpEngCache->dwFileLength - ENG_EOF_CACHE_SIZE;

    // perform proper # of scans

    for (nCurEntryPoint=0;nCurEntryPoint < nTotalEntryPoint;nCurEntryPoint++)
    {
        // get our FStart information

        if (GetFStart(lpFileObject,
                      (WORD)nCurEntryPoint,
                      &hLEng->stAlgScanData.stFStartData,
                      &hLEng->stFStartInfo) == ENGSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        // check to see if we're working on a SYS file.  If so, bump up
        // the # of entry-points

        if (hLEng->stFStartInfo.wFileType == FSTART_SYS_FILE_TYPE &&
            nTotalEntryPoint == 1)
        {
            // bump up the number of entry-points so we go through the scan
            // loop once more...

            nTotalEntryPoint = 2;
        }

        // apply CRC signatures

        if (CRCApplySigs(&hLEng->stAlgScanData.stFStartData,
                         &hLEng->stFStartInfo,
                         &hLEng->hGEng->stCRCScanInfo,
                         lpbFoundSig,
                         lphVirus) == ENGSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        if (*lpbFoundSig)
        {
            return(ENGSTATUS_OK);
        }

        // then apply wild algs

        if (hLEng->hGEng->dwInitStatus & GENG_FLAG_FILE_WILD_LOADED)
        {
            if (AlgApplyAllSigs(lpFileObject,
                                &hLEng->stAlgScanData,      // target file contents
                                &hLEng->stFStartInfo,
                                &hLEng->hGEng->stWildALGScanInfo,
                                lpbFoundSig,
                                lphVirus) == ENGSTATUS_ERROR)
            {
                return(ENGSTATUS_ERROR);
            }

            if (*lpbFoundSig)
                return(ENGSTATUS_OK);
        }

        // check to see if we want to (or can) scan with all non-wild
        // ALG defs...

        if ((hLEng->hGEng->dwInitStatus & GENG_FLAG_FILE_ALL_LOADED) &&
            (dwFlags & LENG_SCAN_FILE_FLAG_SCAN_ALL))
        {
            if (AlgApplyAllSigs(lpFileObject,
                                &hLEng->stAlgScanData,
                                &hLEng->stFStartInfo,
                                &hLEng->hGEng->stGeneralALGScanInfo,
                                lpbFoundSig,
                                lphVirus) == ENGSTATUS_ERROR)
            {
                return(ENGSTATUS_ERROR);
            }

            if (*lpbFoundSig)
            {
                return(ENGSTATUS_OK);
            }
        }
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngScanMemory()
//
// Parameters:
//  hLEng           Handle to local engine context
//  lpMemoryObject  Pointer to memory object to scan
//  dwStartAddress  Starting address to scan
//  dwEndAddress    Ending address to scan
//  dwFlags         Set SCAN_TOP_OF_MEMORY if TOM scanning should be done
//  dwMemCookie     Scan memory cookie
//  lpbFoundSig     Pointer to BOOL to store virus found status
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans memory for memory resident virus signatures.  Sets *lpbFoundSig
//  to TRUE and *lphVirus to a virus handle if a match is found.
//  Otherwise, *lpbFoundSig is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngScanMemory
(
    HLENG               hLEng,
    LPMEMORYOBJECT      lpMemoryObject,
    DWORD               dwStartAddress,
    DWORD               dwEndAddress,
    DWORD               dwFlags,
    DWORD               dwMemCookie,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
)
{
    return(EngScanMemoryAux(hLEng,
                            lpMemoryObject,
                            dwStartAddress,
                            dwEndAddress,
                            dwFlags,
                            dwMemCookie,
                            lpbFoundSig,
                            lphVirus));
}


//*************************************************************************
//
// Function:
//  EngRepairFile()
//
// Parameters:
//  hLEng               Handle to local engine context
//  hVirus              Handle to virus to repair
//  lpFileObject        Pointer to file object to scan
//  lpbFileRepaired     Pointer to BOOL to store repair result
//
// Description:
//  Attempts to repair the file object.  Sets *lpbFileRepaired to TRUE
//  if the file was successfully repaired.  Otherwise, *lpbFileRepaired
//  is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS   EngRepairFile
(
    HLENG               hLEng,              // temporary buffers and callbacks
    HVIRUS              hVirus,
    LPFILEOBJECT        lpFileObject,       // file object to repair
    LPBOOL              lpbFileRepaired     // OUT: was the file repaired?
)
{
    VIRUS_INFO_T        stVirusInfo;
    char                sVirusName[MAX_VIRUS_NAME_LENGTH];
    WORD                wDate, wTime;

    *lpbFileRepaired = FALSE;               // make sure!

    if (EngGetInfoHVIRUS(hLEng->hGEng,
                         hVirus,
                         &stVirusInfo,
                         sVirusName,
                         NULL) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    if (lpFileObject->lpCallBacks->
            FileOpen(lpFileObject->lpvInfo,FILE_OPEN_READ_WRITE) !=
        CBSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    if (AVFileGetDateTime(lpFileObject->lpvInfo,
                          DATE_TIME_FIXME,
                          &wDate,
                          &wTime) == CBSTATUS_ERROR)
    {
        // unable to get the date and time.. close and return


        lpFileObject->lpCallBacks->FileClose(lpFileObject->lpvInfo);

        return(ENGSTATUS_ERROR);
    }

    if (EngRepairFileAux(hLEng,
                         &stVirusInfo,
                         lpFileObject,
                         lpbFileRepaired) != ENGSTATUS_OK)
    {
        // error repairing the file; reset time, close and return!

        AVFileSetDateTime(lpFileObject->lpvInfo,
                          DATE_TIME_FIXME,
                          wDate,
                          wTime);

        lpFileObject->lpCallBacks->FileClose(lpFileObject->lpvInfo);

        return(ENGSTATUS_ERROR);
    }

    if (AVFileSetDateTime(lpFileObject->lpvInfo,
                          DATE_TIME_FIXME,
                          wDate,
                          wTime) == CBSTATUS_ERROR)
    {
        // unable to set the date and time.. close and return

        lpFileObject->lpCallBacks->FileClose(lpFileObject->lpvInfo);

        return(ENGSTATUS_ERROR);
    }


    if (lpFileObject->lpCallBacks->
            FileClose(lpFileObject->lpvInfo) !=
        CBSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}

#ifndef SYM_NLM

//*************************************************************************
//
// Function:
//  EngGlobalBootInit()
//
// Parameters:
//  hGEng               Handle to global engine context
//
// Description:
//  Initializes global engine context with boot scanning data.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalBootInit
(
    HGENG               hGEng,
    DWORD               dwFlags
)
{
    if (EngBootLoad(hGEng) == ENGSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    if (dwFlags & BOOT_INIT_NO_DISK_ACCESS)
    {
        if (EngLoadResidentBootInfo(hGEng) != ENGSTATUS_OK)
            return(ENGSTATUS_ERROR);

        hGEng->dwInitStatus |= GENG_FLAG_BOOT_INFO_LOADED;
    }

    // successful initialization of boot scanning data

    hGEng->dwInitStatus |= GENG_FLAG_BOOT_LOADED;

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGlobalBootClose()
//
// Parameters:
//  hGEng               Handle to global engine context
//
// Description:
//  Unloads boot scanning data.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGlobalBootClose
(
    HGENG   hGEng
)
{
    ENGSTATUS status = ENGSTATUS_OK;

    if (!(hGEng->dwInitStatus & GENG_FLAG_BOOT_LOADED))
        return(ENGSTATUS_ERROR);

    if (hGEng->dwInitStatus & GENG_FLAG_BOOT_INFO_LOADED)
    {
        if (EngFreeResidentBootInfo(hGEng) != ENGSTATUS_OK)
            status = ENGSTATUS_ERROR;
        else
            hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_BOOT_INFO_LOADED;
    }

    if (EngBootUnload(hGEng) == ENGSTATUS_ERROR)
        status = ENGSTATUS_ERROR;

    // reset boot loaded flag after we unload

    hGEng->dwInitStatus &= (DWORD)~GENG_FLAG_BOOT_LOADED;

    return (status);
}


//*************************************************************************
//
// Function:
//  EngScanBoot()
//
// Parameters:
//  hLEng           Handle to local engine context
//  lpbyBootBuffer  Pointer to buffer containing boot sector data
//  dwBufSize       Size of the boot buffer to scan
//  lpbFoundSig     Pointer to BOOL to store virus found status
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Scans buffer for boot virus signatures.  Sets *lpbFoundSig
//  to TRUE and *lphVirus to a virus handle if a match is found.
//  Otherwise, *lpbFoundSig is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngScanBoot
(
    HLENG       hLEng,
    LPBYTE      lpbyBootBuffer,
    DWORD       dwBufSize,
    LPBOOL      lpbFoundSig,
    LPHVIRUS    lphVirus
)
{
    *lpbFoundSig = EngScanBooBuffer(lpbyBootBuffer,
                                    dwBufSize,
                                    &(hLEng->hGEng->lpBootScanInfo->stBOO),
                                    BOO_SIG_LEN,
                                    lphVirus);

    return(ENGSTATUS_OK);
}

//*************************************************************************
//
// Function:
//  EngRepairBoot()
//
// Parameters:
//  hLocalEng           Handle to local engine context
//  hVirus              Handle to virus to repair
//  lprLinear           Pointer to linear object to repair
//  lpbRepaired         Pointer to BOOL to store repair result
//
// Description:
//  Attempts to repair the linear object.  Sets *lpbFileRepaired to TRUE
//  if the object was successfully repaired.  Otherwise, *lpbFileRepaired
//  is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngRepairBoot
(
    HLENG               hLocalEng,          // temporary buffers and callbacks
    HVIRUS              hVirus,
    LPLINEAROBJECT      lprLinear,          // linear object to repair
    BYTE                byPartNumber,       // partition to repair
    LPBOOL              lpbRepaired         // OUT: was the object repaired?
)
{
    VIRUS_INFO_T        rVirusInfo;
    char                sVirusName[MAX_VIRUS_NAME_LENGTH];

    *lpbRepaired = FALSE;

    if (EngGetInfoHVIRUS(hLocalEng->hGEng,
                         hVirus,
                         &rVirusInfo,
                         sVirusName,
                         NULL) != ENGSTATUS_OK)
        {
        return(ENGSTATUS_ERROR);
        }

    if (EngRepairBootAux(hLocalEng,
                         &rVirusInfo,
                         lprLinear,
                         byPartNumber,
                         lpbRepaired) != ENGSTATUS_OK)
        {
        return(ENGSTATUS_ERROR);
        }

    return(ENGSTATUS_OK);
}



VOID EngParseMBRPartition
(
    LPBYTE      lpbyMBR,                // [in] Entire MBR
    BYTE        byPartNumber,           // [in] Partition to parse (0 - 3)
    LPPARTENTRY lprPartEntry            // [out] filled out PARTENTRY structure
)
{
    WORD        wEncodedSectorCylinder;

    lpbyMBR += PARTITION_OFFSET + ( byPartNumber * PARTENTRY_SIZE );

    if ( *lpbyMBR++ == PARTITION_ACTIVE )
        {
        lprPartEntry->bActive = TRUE;
        }
    else
        {
        lprPartEntry->bActive = FALSE;
        }

    lprPartEntry->byStartSide = *lpbyMBR++;

    wEncodedSectorCylinder = WENDIAN (*(LPWORD) lpbyMBR);
    lpbyMBR += 2;
    EngDecodeSectorCylinder ( wEncodedSectorCylinder,
                             (LPBYTE) &lprPartEntry->byStartSector,
                             (LPWORD) &lprPartEntry->wStartCylinder );

    lprPartEntry->byFileSystem = *lpbyMBR++;

    lprPartEntry->byEndSide = *lpbyMBR++;

    wEncodedSectorCylinder = WENDIAN (*(LPWORD) lpbyMBR);
    lpbyMBR += 2;
    EngDecodeSectorCylinder ( wEncodedSectorCylinder,
                             (LPBYTE) &lprPartEntry->byEndSector,
                             (LPWORD) &lprPartEntry->wEndCylinder );

    lprPartEntry->dwRelativeSectors = DWENDIAN ( *(LPDWORD) lpbyMBR );
    lpbyMBR += 4;

    lprPartEntry->dwTotalSectors = DWENDIAN ( *(LPDWORD) lpbyMBR );
    lpbyMBR += 4;
}


ENGSTATUS EngGetDiskType
(
    LPLINEAROBJECT      lprLinear,      // [in] Linear object to analyze
    LPWORD              lpwDiskType     // [out] Disk Type
)
{
    ENGSTATUS   esResult = ENGSTATUS_OK;
    BYTE        abySector [ SECTOR_SIZE ];
    DWORD       dwActual;
    BOOL        bIsPartitionedMedia;

    if (lprLinear->lpCallBacks->
            LinearIsPartitionedMedia(lprLinear->lpvInfo,
                                     &bIsPartitionedMedia) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if ( bIsPartitionedMedia )
        {
        *lpwDiskType = DISK_FIXED;
        }
    else
        {
                                        // Read Floppy Boot Sector
        esResult = lprLinear->lpCallBacks->LinearRead ( lprLinear->lpvInfo,
                                                        0,
                                                        0,
                                                        abySector,
                                                        1,
                                                        &dwActual );

        if ( ENGSTATUS_OK == esResult && dwActual != 1 )
            {
            esResult = ENGSTATUS_ERROR;
            }

        if ( ENGSTATUS_OK == esResult )
            {
                                        // Call internal Floppy type function
            esResult = EngGetFloppyType ( lprLinear, abySector, lpwDiskType );
            }
        }
    return ( esResult );
}

#endif   // ifndef SYM_NLM
