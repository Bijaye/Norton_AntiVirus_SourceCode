// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/REPARAUX.CPv   1.4   08 Jul 1997 17:55:56   MKEATIN  $
//
// Description:
//  Functions for obtaining repair records.
//
// Contains:
//  GetRepairRecord()
//
// See Also:
//  filerep.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/REPARAUX.CPv  $
// 
//    Rev 1.4   08 Jul 1997 17:55:56   MKEATIN
// Changed DFCloseDataFile() to EngCloseDataFile().
// 
//    Rev 1.3   08 Jul 1997 17:19:56   MKEATIN
// Passed NULL as the lpvProvidedFileInfo parameters in various functions.
// The old default NULL in the old prototype won't compile for the NLM
// C compiler.
// 
//    Rev 1.2   15 May 1997 12:19:46   MKEATIN
// Made iterating though records faster by providing an open file info
// pointer (handle) to info routines.
// 
//    Rev 1.1   13 May 1997 18:27:00   MKEATIN
// Ported Carey's boot caching code.
// 
//    Rev 1.0   18 Apr 1997 13:37:52   MKEATIN
// Initial revision.
// 
//    Rev 1.4   05 Jun 1996 13:29:36   CNACHEN
// Removed all reference of the GetBOORepairRecord function.
// 
//    Rev 1.3   05 Jun 1996 12:41:52   CNACHEN
// Removed unused GetBOORepairRecord
// 
//    Rev 1.2   17 May 1996 14:16:04   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
// 
//    Rev 1.1   16 May 1996 14:12:58   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.0   13 May 1996 16:29:20   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

//*************************************************************************
//
// Function:
//  EndianizeVirusRepair()
//
// Parameters:
//  lpVirusRepairInfo   Pointer to VIRUS_REPAIR_T structure to endianize.
//
// Description:
//  Endianizes all fields of the VIRUS_REPAIR_T struct.
//
// Returns:
//  Nothing
//
//*************************************************************************

void EndianizeVirusRepair
(
    LPVIRUS_REPAIR    lpVirusRepairInfo
)
{
    lpVirusRepairInfo->wCRC =
        WENDIAN(lpVirusRepairInfo->wCRC);
    lpVirusRepairInfo->wTagSize =
        WENDIAN(lpVirusRepairInfo->wTagSize);
    lpVirusRepairInfo->wHostJmp =
        WENDIAN(lpVirusRepairInfo->wHostJmp);
    lpVirusRepairInfo->wJmpLen =
        WENDIAN(lpVirusRepairInfo->wJmpLen);
    lpVirusRepairInfo->wSS_reg =
        WENDIAN(lpVirusRepairInfo->wSS_reg);
    lpVirusRepairInfo->wSP_reg =
        WENDIAN(lpVirusRepairInfo->wSP_reg);
    lpVirusRepairInfo->wCS_reg =
        WENDIAN(lpVirusRepairInfo->wCS_reg);
    lpVirusRepairInfo->wIP_reg =
        WENDIAN(lpVirusRepairInfo->wIP_reg);
    lpVirusRepairInfo->wHeader =
        WENDIAN(lpVirusRepairInfo->wHeader);
    lpVirusRepairInfo->wHeadLen =
        WENDIAN(lpVirusRepairInfo->wHeadLen);
    lpVirusRepairInfo->wBookLocation =
        WENDIAN(lpVirusRepairInfo->wBookLocation);
    lpVirusRepairInfo->wBookMark =
        WENDIAN(lpVirusRepairInfo->wBookMark);
    lpVirusRepairInfo->wInfSize =
        WENDIAN(lpVirusRepairInfo->wInfSize);
    lpVirusRepairInfo->dwReserved =
        DWENDIAN(lpVirusRepairInfo->dwReserved);
    lpVirusRepairInfo->dwRepairFlags =
        DWENDIAN(lpVirusRepairInfo->dwRepairFlags);
    lpVirusRepairInfo->wMemSigIndex =
		WENDIAN(lpVirusRepairInfo->wMemSigIndex);
}


//*************************************************************************
//
// Function:
//  GetRepairRecordAux()
//
// Parameters:
//  lpDataFileCallBacks Pointer to data file callbacks
//  lpvFileInfo         Pointer to file info for file containing repair
//  lpRepSec            Repair section information
//  lpMemSec            Repair memory information
//  wRepairIndex        Repair index
//  lpVirusRepairInfo   Pointer to VIRUS_REPAIR_T structure
//  lpbyDecryptData     Pointer to buffer to store decryption data
//
// Description:
//  Retrieves the repair record at wRepairIndex.
//
// Returns:
//  Nothing
//
//*************************************************************************

ENGSTATUS GetRepairRecordAux
(
    LPDATAFILECALLBACKS     lpDataFileCallBacks,
    LPVOID                  lpvFileInfo,
    LPDATA_FILE_TABLE       lpRepSec,
    LPDATA_FILE_TABLE       lpMemSec,
    WORD                    wRepairIndex,
    LPVIRUS_REPAIR          lpVirusRepairInfo,
    LPBYTE                  lpbyDecryptData
)
{
    DWORD                   dwOff, dwBytesRead;
    WORD                    wNumRecords, wRecordSize;

    // seek to the start of the repair section

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileSeek(lpvFileInfo,
                                     lpRepSec->dwStartOffset,
                                     SEEK_SET,
                                     &dwOff) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // read in the # of records in this section

    if (lpDataFileCallBacks->
        lpFileCallBacks->FileRead(lpvFileInfo,
                                  &wNumRecords,
                                  sizeof(WORD),
                                  &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(WORD))
    {
        return(ENGSTATUS_ERROR);
    }

    wNumRecords = WENDIAN(wNumRecords);

    // determine how large each repair record is...

    if (lpDataFileCallBacks->
        lpFileCallBacks->FileRead(lpvFileInfo,
                                  &wRecordSize,
                                  sizeof(WORD),
                                  &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(WORD))
    {
        return(ENGSTATUS_ERROR);
    }

    wRecordSize = WENDIAN(wRecordSize);

    // now read the specified record

    dwOff = lpRepSec->dwStartOffset +
            sizeof(wNumRecords) +
            sizeof(wRecordSize) +
            (DWORD)wRecordSize * (DWORD)wRepairIndex;

    // seek to the appropriate record

    if (lpDataFileCallBacks->
        lpFileCallBacks->FileSeek(lpvFileInfo,
                                  dwOff,
                                  SEEK_SET,
                                  &dwOff) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // read in the repair record...

    if (lpDataFileCallBacks->
        lpFileCallBacks->FileRead(lpvFileInfo,
                                  lpVirusRepairInfo,
                                  sizeof(VIRUS_REPAIR_T),
                                  &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(VIRUS_REPAIR_T))
    {
        return(ENGSTATUS_ERROR);
    }

    EndianizeVirusRepair(lpVirusRepairInfo);

    // now locate the specified memory signature for decryption if necessary

    if (lpVirusRepairInfo->dwRepairFlags & CTRL_DECRYPT)
    {
        dwOff = lpMemSec->dwStartOffset + sizeof(WORD) +
                (DWORD)lpVirusRepairInfo->wMemSigIndex * DECRYPT_MEM_SIG_SIZE;

        // seek to the signature

        if (lpDataFileCallBacks->
            lpFileCallBacks->FileSeek(lpvFileInfo,
                                     dwOff,
                                     SEEK_SET,
                                     &dwOff) == CBSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        // read in the 8-byte signature

        if (lpDataFileCallBacks->
            lpFileCallBacks->FileRead(lpvFileInfo,
                                      lpbyDecryptData,
                                      DECRYPT_MEM_SIG_SIZE,
                                      &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != DECRYPT_MEM_SIG_SIZE)
        {
            return(ENGSTATUS_ERROR);
        }
    }

    return(ENGSTATUS_OK);
}


ENGSTATUS GetRepairRecord
(
    HLENG                   hLEng,
    LPVIRUS_INFO            lpstVirusInfo,
    LPVIRUS_REPAIR          lpVirusRepairInfo,
    LPBYTE                  lpbyDecryptSig
)
{
    LPVOID              lpvFileInfo;
    DATA_FILE_TABLE_T   stRepSection, stMemSection;
    ENGSTATUS           status;
    LPBOOT_INFO_LIST    lpstBIL;

    if (hLEng->hGEng->dwInitStatus & GENG_FLAG_BOOT_INFO_LOADED)
    {
        lpstBIL = hLEng->hGEng->lpBootScanInfo->lpstBootInfoList;

        while (lpstBIL != NULL)
        {
            if (lpstBIL->stVirusInfo.wVirusID == lpstVirusInfo->wVirusID)
            {
                memcpy(lpbyDecryptSig,
                       lpstBIL->byDecryptSig,
                       DECRYPT_MEM_SIG_SIZE);

                *lpVirusRepairInfo = lpstBIL->stVirusRepair;

                return(ENGSTATUS_OK);
            }

            lpstBIL = lpstBIL->lpstNext;
        }
    }

    // hit the disk!

    if (EngLocateSections (hLEng->hGEng,
                           lpstVirusInfo,
                           &lpvFileInfo,
                           &stRepSection,
                           &stMemSection,
                           NULL) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }    


    status = GetRepairRecordAux (hLEng->hGEng->lpDataFileCallBacks,
                                 lpvFileInfo,
                              	 &stRepSection,
                              	 &stMemSection,
                              	 lpstVirusInfo->wRepairIndex,
                              	 lpVirusRepairInfo,
                              	 lpbyDecryptSig );

    EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

    return(status);
}

