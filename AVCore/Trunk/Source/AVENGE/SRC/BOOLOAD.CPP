// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/BOOLOAD.CPv   1.7   22 Aug 1997 20:10:04   RStanev  $
//
// Description:
//  Functions for loading boot scanning signatures in form suitable
//  for scanning with memory scanning subengine.
//
// Contains:
//  EngBootLoad()
//  EngBootUnload()
//
// See Also:
//  memload.c, memscan.c, avenge.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/BOOLOAD.CPv  $
// 
//    Rev 1.7   22 Aug 1997 20:10:04   RStanev
// Fixed a call to MemFree with an uninitialized auto pointer, and fixed
// a double call to MemFree with the same pointer.
//
//    Rev 1.6   13 Aug 1997 13:55:02   CNACHEN
// no longer sorts boot sigs after loading for compatibility with old
// style boot scanning.
//
//    Rev 1.5   08 Jul 1997 17:49:14   MKEATIN
// Changed DFFunctions() to EngFunctions().
//
//    Rev 1.4   19 May 1997 19:10:44   MKEATIN
// Fixed comments
//
//    Rev 1.3   19 May 1997 19:03:44   MKEATIN
// EngLoadResidentBootInfo() now iterates only through virusus into BOOTLIST
// section.
//
//    Rev 1.2   15 May 1997 12:17:14   MKEATIN
// Made iterating though records faster by providing an open file info
// pointer (handle) to info routines.
//
//    Rev 1.1   13 May 1997 18:26:38   MKEATIN
// Ported Carey's boot caching code.
//
//    Rev 1.0   18 Apr 1997 13:37:20   MKEATIN
// Initial revision.
//
//    Rev 1.2   17 May 1996 14:15:56   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
//
//    Rev 1.1   16 May 1996 14:12:40   CNACHEN
// Changed to use new AVENGE?.H headers.
//
//    Rev 1.0   13 May 1996 16:28:48   DCHI
// Initial revision.
//
//************************************************************************

#include "avengel.h"

//*************************************************************************
//
// Function:
//  EngBootFreeInfo()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Frees memory allocated for boot signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngBootFreeInfo
(
    HGENG       hGEng
)
{
    // Free BOO memory

    EngMemoryFreeSigGroup(hGEng,&(hGEng->lpBootScanInfo->stBOO));

    // Free BOOT_SCAN_INFO_T structure

    hGEng->lpGeneralCallBacks->MemoryFree(hGEng->lpBootScanInfo);
    hGEng->lpBootScanInfo = NULL;

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngBootLoadAux()
//
// Parameters:
//  hGEng           Handle to global engine context
//  lpvFileInfo     Pointer to file info for VIRSCAN4.DAT
//
// Description:
//  Looks up SECTION_ID_BOO_SIG in VIRSCAN4.DAT and loads the memory
//  signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngBootLoadAux
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
                 SECTION_ID_BOO_SIG,
                 &stDataFileTableEntry,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // Error during lookup and/or section not found

        return(ENGSTATUS_ERROR);
    }

    // Seek to beginning of boot signatures section

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
    // Allocate BOOT_SCAN_INFO_T structure memory
    //
    ///////////////////////////////////////////////////////////////////

    if (hGEng->lpGeneralCallBacks->
            MemoryAlloc(sizeof(BOOT_SCAN_INFO_T),
                        MEM_FIXME,
                        (LPLPVOID)&(hGEng->lpBootScanInfo))		== CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // Initialize fields

    hGEng->lpBootScanInfo->stBOO.lpbyQuickWordEliminate = NULL;
    hGEng->lpBootScanInfo->stBOO.wNumSigPools = 0;


    ///////////////////////////////////////////////////////////////////
    //
    // Read BOO signatures
    //
    ///////////////////////////////////////////////////////////////////

    if (EngMemoryReadSigGroup(hGEng,
                              lpvFileInfo,
                              sizeof(BOO_SIG_DATA_T),
                              FALSE,
                              &(hGEng->lpBootScanInfo->stBOO))
        == ENGSTATUS_ERROR)
    {
        EngBootFreeInfo(hGEng);
        return(ENGSTATUS_ERROR);
    }


	return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngBootUnload()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Frees memory allocated for boot signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngBootUnload
(
	HGENG       hGEng
)
{
    return EngBootFreeInfo(hGEng);
}


//*************************************************************************
//
// Function:
//  EngBootLoad()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Opens VIRSCAN4.DAT and calls EngBootLoadAux() to load the boot
//  signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngBootLoad
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

    engStatus = EngBootLoadAux(hGEng,lpvFileInfo);

    if (EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvFileInfo) ==
        DFSTATUS_ERROR)
    {
        // error closing the data file? if we were able to load our signature
        // data then free it...

        if (engStatus != DFSTATUS_ERROR)
        {
            EngBootUnload(hGEng);
        }

        return(ENGSTATUS_ERROR);
    }

    return(engStatus);
}

ENGSTATUS EngLoadResidentBootInfo
(
    HGENG       hGEng
)
{
    WORD                wIndex, wNumRecords;
    HVIRUS              hVirus;
    VIRUS_INFO_T        stVirusInfo;
    char                szVirusName[MAX_VIRUS_NAME_LENGTH+1];
    LPBOOT_INFO_LIST    lpstBIL = NULL;
    LPVOID              lpvVirscan1FileInfo, lpv2ndVirscan1FileInfo;
    LPVOID              lpvVirscan2FileInfo, lpvVirscan3FileInfo;
    DATA_FILE_TABLE_T   stRepSection, stMemSection, stBootSection;
    BOOL                bSectionFound;
    DWORD               dwOff, dwBytesRead;

    hGEng->lpBootScanInfo->lpstBootInfoList = NULL;

    // Open data files

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN1DAT,
                       FILE_OPEN_READ_ONLY,
                       &lpvVirscan1FileInfo) == DFSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN1DAT,
                       FILE_OPEN_READ_ONLY,
                       &lpv2ndVirscan1FileInfo) == DFSTATUS_ERROR)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan1FileInfo);

        return(ENGSTATUS_ERROR);
    }

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN2DAT,
                       FILE_OPEN_READ_ONLY,
                       &lpvVirscan2FileInfo) == DFSTATUS_ERROR)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan1FileInfo);
        EngCloseDataFile(hGEng->lpDataFileCallBacks, lpv2ndVirscan1FileInfo);

        return(ENGSTATUS_ERROR);
    }

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN3DAT,
                       FILE_OPEN_READ_ONLY,
                       &lpvVirscan3FileInfo) == DFSTATUS_ERROR)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan1FileInfo);
        EngCloseDataFile(hGEng->lpDataFileCallBacks, lpv2ndVirscan1FileInfo);
        EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan2FileInfo);

        return(ENGSTATUS_ERROR);
    }

    // locate boot list section

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvVirscan1FileInfo,
                 SECTION_ID_BOOTLIST,
                 &stBootSection,
                 &bSectionFound) == DFSTATUS_ERROR || bSectionFound == FALSE)
    {
        goto Cleanup;
    }

    // seek to the start of the boot list section

    if (hGEng->lpDataFileCallBacks->
               lpFileCallBacks->FileSeek(lpvVirscan1FileInfo,
                                         stBootSection.dwStartOffset,
                                         SEEK_SET,
                                         &dwOff) == CBSTATUS_ERROR)
    {
        goto Cleanup;
    }

    // read in the # of records in this section

    if (hGEng->lpDataFileCallBacks->
               lpFileCallBacks->FileRead(lpvVirscan1FileInfo,
                                         &wNumRecords,
                                         sizeof(WORD),
                                         &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(WORD))
    {
        goto Cleanup;
    }

    wNumRecords = WENDIAN(wNumRecords);

    // iterate through the records and load them...

    for (wIndex = 0; wIndex < wNumRecords; wIndex++)
    {

        // get the next hVirus

        if (hGEng->lpDataFileCallBacks->
                   lpFileCallBacks->
                   FileRead(lpvVirscan1FileInfo,
                            &hVirus,
                            sizeof(WORD),
                            &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != sizeof(WORD))
        {
            goto Cleanup;
        }

        // below uses lpv2ndVirscan1FileInfo so we don't have to waste time
        // FileSeek()-ing to the next hVirus (just before reading it above)

        if (EngGetInfoHVIRUS(hGEng,
                             hVirus,
                             &stVirusInfo,
                             szVirusName,
                             lpv2ndVirscan1FileInfo) != ENGSTATUS_OK)
        {
            goto Cleanup;
        }

        // see if we've got a boot def - if so, store it away!

        if (stVirusInfo.wFlags & AVENGE_TYPE_BOO)
        {
            if (hGEng->
                lpGeneralCallBacks->
                MemoryAlloc(sizeof(BOOT_INFO_LIST_T),
                            MEM_FIXME,
                            (LPLPVOID)&lpstBIL) != ENGSTATUS_OK)
            {
                goto Cleanup;
            }

            // store what we know so far...

            lpstBIL->hVirus = hVirus;
            lpstBIL->stVirusInfo = stVirusInfo;

            // including the virus name...

            if (hGEng->
                lpGeneralCallBacks->
                MemoryAlloc(strlen(szVirusName)+1,
                            MEM_FIXME,
                            (LPLPVOID)&lpstBIL->lpszVirusName) != ENGSTATUS_OK)
            {
                goto Cleanup;
            }

            strcpy(lpstBIL->lpszVirusName,szVirusName);

            if (stVirusInfo.wFlags & AVENGE_TYPE_VIRSCAN2_REPAIR)
            {
                // now locate the repair information

                if (EngLocateSections (hGEng,
                                       &stVirusInfo,
                                       NULL,
                                       &stRepSection,
                                       &stMemSection,
                                       lpvVirscan2FileInfo) != ENGSTATUS_OK)
                {
                    goto Cleanup;
                }

                // and fetch it...

                if (GetRepairRecordAux (hGEng->lpDataFileCallBacks,
                                        lpvVirscan2FileInfo,
                                        &stRepSection,
                                        &stMemSection,
                                        stVirusInfo.wRepairIndex,
                                        &lpstBIL->stVirusRepair,
                                        lpstBIL->byDecryptSig ) != ENGSTATUS_OK)
                {
                    goto Cleanup;
                }
            }
            else if (stVirusInfo.wFlags & AVENGE_TYPE_HAS_REPAIR)
            {
                // now locate the repair information

                if (EngLocateSections (hGEng,
                                       &stVirusInfo,
                                       NULL,
                                       &stRepSection,
                                       &stMemSection,
                                       lpvVirscan3FileInfo) != ENGSTATUS_OK)
                {
                    goto Cleanup;
                }

                // and fetch it...

                if (GetRepairRecordAux (hGEng->lpDataFileCallBacks,
                                        lpvVirscan3FileInfo,
                                        &stRepSection,
                                        &stMemSection,
                                        stVirusInfo.wRepairIndex,
                                        &lpstBIL->stVirusRepair,
                                        lpstBIL->byDecryptSig ) != ENGSTATUS_OK)
                {
                    goto Cleanup;
                }
            }


            lpstBIL->lpstNext = hGEng->lpBootScanInfo->lpstBootInfoList;
            hGEng->lpBootScanInfo->lpstBootInfoList = lpstBIL;
            lpstBIL = NULL;
        }
    }

    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan1FileInfo);
    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpv2ndVirscan1FileInfo);
    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan2FileInfo);
    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan3FileInfo);

    return(ENGSTATUS_OK);

Cleanup:

    if (lpstBIL != NULL)
        hGEng->lpGeneralCallBacks->MemoryFree(lpstBIL);

    EngFreeResidentBootInfo(hGEng);

    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan1FileInfo);
    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpv2ndVirscan1FileInfo);
    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan2FileInfo);
    EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvVirscan3FileInfo);

    return(ENGSTATUS_ERROR);
}

ENGSTATUS EngFreeResidentBootInfo
(
    HGENG       hGEng
)
{
    LPBOOT_INFO_LIST    lpstBIL, lpstNext;

    lpstBIL = hGEng->lpBootScanInfo->lpstBootInfoList;

    while (lpstBIL != NULL)
    {
        hGEng->lpGeneralCallBacks->MemoryFree(lpstBIL->lpszVirusName);
        lpstNext = lpstBIL->lpstNext;

        hGEng->lpGeneralCallBacks->MemoryFree(lpstBIL);
        lpstBIL = lpstNext;
    }

    return(ENGSTATUS_OK);
}




