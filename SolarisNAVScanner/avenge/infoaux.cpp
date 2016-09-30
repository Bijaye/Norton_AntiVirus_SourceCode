// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/INFOAUX.CPv   1.3   08 Jul 1997 17:49:20   MKEATIN  $
//
// Description:
//  Functions providing read/write access to virus information records
//  in VIRSCAN1.DAT.
//
// Contains:
//  EngGetVirusInfoAux()
//  EngPutVirusInfoAux()
//  EngGetVirusInfoVIDAux()
//
// See Also:
//  getinfo.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/INFOAUX.CPv  $
// 
//    Rev 1.3   08 Jul 1997 17:49:20   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.2   15 May 1997 12:19:48   MKEATIN
// Made iterating though records faster by providing an open file info
// pointer (handle) to info routines.
// 
//    Rev 1.1   13 May 1997 18:27:08   MKEATIN
// Ported Carey's boot caching code.
// 
//    Rev 1.0   18 Apr 1997 13:37:34   MKEATIN
// Initial revision.
// 
//    Rev 1.3   17 May 1996 14:15:26   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
// 
//    Rev 1.2   16 May 1996 14:12:48   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.1   15 May 1996 18:28:28   DCHI
// Fixed a couple of WORDS misdeclared as DWORDS.
// 
//    Rev 1.0   13 May 1996 16:29:02   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"


//*************************************************************************
//
// Function:
//  EndianizeVirusInfo()
//
// Parameters:
//  lpVirusInfo     Pointer to VIRUS_INFO_T structure to endianize.
//
// Description:
//  Endianizes all fields of the VIRUS_INFO_T struct.
//
// Returns:
//  Nothing
//
//*************************************************************************

void EndianizeVirusInfo
(
    LPVIRUS_INFO    lpVirusInfo
)
{
    // Endian convert the structure

    lpVirusInfo->wVirusID = WENDIAN(lpVirusInfo->wVirusID);
    lpVirusInfo->dwInfo = DWENDIAN(lpVirusInfo->dwInfo);
    lpVirusInfo->wInfSize = WENDIAN(lpVirusInfo->wInfSize);
    lpVirusInfo->wRepairIndex = WENDIAN(lpVirusInfo->wRepairIndex);
    lpVirusInfo->wFlags = WENDIAN(lpVirusInfo->wFlags);
}


//********************************************************************
//
// Function:
//  EngGetVirusInfoAux()
//
// Parameters:
//  hGEng           Handle to global engine context
//  hVirus          Handle of virus information to retrieve
//  lpVirusInfo     Pointer to VIRUS_INFO_T struct to store info
//
// Description:
//  Opens File:VIRSCAN1.DAT Section:VIRUS_RECORD_DATA_SECTION
//  and retrieves virus record information for hVirus into the
//  structure pointed to by lpVirusInfo.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngGetVirusInfoAux
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPVOID              lpvProvidedFileInfo
)
{
    LPVOID              lpvFileInfo;
    WORD                wNumRecords;
    WORD                wRecordSize;
    DWORD               dwBytesToRead;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwBytesRead;
    LPBOOT_INFO_LIST    lpstBIL;

    // Initialize structure to zero

    dwBytesToRead = sizeof(VIRUS_INFO_T);
    MEMSET(lpVirusInfo,0,sizeof(VIRUS_INFO_T));

    // check in our in-memory boot cache, if applicable

    if (hGEng->dwInitStatus & GENG_FLAG_BOOT_INFO_LOADED)
    {
        lpstBIL = hGEng->lpBootScanInfo->lpstBootInfoList;

        while (lpstBIL != NULL)
        {
            if (lpstBIL->hVirus == hVirus)
            {
                *lpVirusInfo = lpstBIL->stVirusInfo;

                return(ENGSTATUS_OK);
            }

            lpstBIL = lpstBIL->lpstNext;
        }
    }

    // now hit the files...

    ///////////////////////////////////////////////////////////////////
    //
    //  Open VIRSCAN1.DAT section VIRUS_RECORD_DATA_SECTION
    //
    ///////////////////////////////////////////////////////////////////

    // open the virus information data file...

    if (lpvProvidedFileInfo == NULL)
    {
        if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                           DATA_FILE_ID_VIRSCAN1DAT,
                           FILE_OPEN_READ_ONLY,
                           &lpvFileInfo) == DFSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }
    }
    else
    {
       lpvFileInfo = lpvProvidedFileInfo;
    }

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_VIRINFO,
                 &stDataFileTableEntry,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // Error during lookup and/or section not found

        goto Cleanup;
    }

    // Seek to beginning of section

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileSeek(lpvFileInfo,
                     stDataFileTableEntry.dwStartOffset,
                     SEEK_SET,
                     &dwOffset) == CBSTATUS_ERROR)
    {
        // Seek failed

        goto Cleanup;
    }


    ///////////////////////////////////////////////////////////////////
    //
    //  Read number of virus information records and record size
    //
    ///////////////////////////////////////////////////////////////////

    // Read number of records

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)&wNumRecords,
                     sizeof(wNumRecords),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(wNumRecords))
    {
        // Read of # of records failed

        goto Cleanup;
    }

    // Read record size

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)&wRecordSize,
                     sizeof(wRecordSize),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(wRecordSize))
    {
        // Read of record size failed

        goto Cleanup;
    }

    // Endian enable read values

    wNumRecords = WENDIAN(wNumRecords);
    wRecordSize = WENDIAN(wRecordSize);


    ///////////////////////////////////////////////////////////////////
    //
    //  Bounds checking
    //
    ///////////////////////////////////////////////////////////////////

    // Check to see that the virus handle is within bounds

    if ((WORD)hVirus >= wNumRecords)
    {
        // Out of bounds virus handle

        goto Cleanup;
    }

    // Seek to the entry

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileSeek(lpvFileInfo,
                     stDataFileTableEntry.dwStartOffset +
                        sizeof(WORD) + sizeof(WORD) +
                        (DWORD)wRecordSize * (DWORD)hVirus,
                     SEEK_SET,
                     &dwOffset) == CBSTATUS_ERROR)
    {
        // Seek failed

        goto Cleanup;
    }

    // Take the minimum of sizeof(VIRUS_INFO_T) and wRecordSize

    if (wRecordSize < dwBytesToRead)
        dwBytesToRead = wRecordSize;


    ///////////////////////////////////////////////////////////////////
    //
    //  Read the entry and endian-enable it
    //
    ///////////////////////////////////////////////////////////////////

    // Read the entry

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)lpVirusInfo,
                     dwBytesToRead,
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != dwBytesToRead)
    {
        // Read of entry failed

        goto Cleanup;
    }

    // Endian convert the structure

    EndianizeVirusInfo(lpVirusInfo);

    if (lpvProvidedFileInfo == NULL)
        return (EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo));
    
    return (ENGSTATUS_OK);

Cleanup:

    if (lpvProvidedFileInfo == NULL)
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
    return (ENGSTATUS_ERROR);
}


//********************************************************************
//
// Function:
//  EngPutVirusInfoAux()
//
// Parameters:
//  hGEng           Handle to global engine context
//  hVirus          Handle of virus information to retrieve
//  lpVirusInfo     Pointer to VIRUS_INFO_T struct containing info
//                  to write
//
// Description:
//  Opens File:VIRSCAN1.DAT Section:VIRUS_RECORD_DATA_SECTION
//  and writes virus record information for hVirus from the
//  structure pointed to by lpVirusInfo.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngPutVirusInfoAux
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo
)
{
    LPVOID              lpvFileInfo;
    WORD                wNumRecords;
    WORD                wRecordSize;
    DWORD               dwBytesToWrite;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwBytesRead, dwBytesWritten;

    ///////////////////////////////////////////////////////////////////
    //
    //  Open VIRSCAN1.DAT section VIRUS_RECORD_DATA_SECTION
    //
    ///////////////////////////////////////////////////////////////////

    // open the virus information data file...

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN1DAT,
                       FILE_OPEN_READ_WRITE,
                       &lpvFileInfo) == DFSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_VIRINFO,
                 &stDataFileTableEntry,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // Error during lookup and/or section not found

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Seek to beginning of section

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileSeek(lpvFileInfo,
                     stDataFileTableEntry.dwStartOffset,
                     SEEK_SET,
                     &dwOffset) == CBSTATUS_ERROR)
    {
        // Seek failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    //  Read number of virus information records and record size
    //
    ///////////////////////////////////////////////////////////////////

    // Read number of records

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)&wNumRecords,
                     sizeof(wNumRecords),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(wNumRecords))
    {
        // Read of # of records failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Read record size

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)&wRecordSize,
                     sizeof(wRecordSize),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(wRecordSize))
    {
        // Read of record size failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Endian enable read values

    wNumRecords = WENDIAN(wNumRecords);
    wRecordSize = WENDIAN(wRecordSize);


    ///////////////////////////////////////////////////////////////////
    //
    //  Bounds checking
    //
    ///////////////////////////////////////////////////////////////////

    // Check to see that the virus handle is within bounds

    if ((WORD)hVirus >= wNumRecords)
    {
        // Out of bounds virus handle

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Seek to the entry

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileSeek(lpvFileInfo,
                     stDataFileTableEntry.dwStartOffset +
                        sizeof(WORD) + sizeof(WORD) +
                        (DWORD)wRecordSize * (DWORD)hVirus,
                     SEEK_SET,
                     &dwOffset) == CBSTATUS_ERROR)
    {
        // Seek failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Take the minimum of sizeof(VIRUS_INFO_T) and wRecordSize

    if (wRecordSize < sizeof(VIRUS_INFO_T))
        dwBytesToWrite = wRecordSize;
    else
        dwBytesToWrite = sizeof(VIRUS_INFO_T);


    ///////////////////////////////////////////////////////////////////
    //
    //  Little-endianize the structure and then write the entry
    //
    ///////////////////////////////////////////////////////////////////

    // Endian convert the structure

    EndianizeVirusInfo(lpVirusInfo);

    // Write the entry

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileWrite(lpvFileInfo,
                      (LPVOID)lpVirusInfo,
                      dwBytesToWrite,
                      &dwBytesWritten) == CBSTATUS_ERROR ||
        dwBytesWritten != dwBytesToWrite)
    {
        // Write of entry failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    return (EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo));
}


//********************************************************************
//
// Function:
//  EngGetVirusInfoVIDAux()
//
// Parameters:
//  hGEng           Handle to global engine context
//  wVID            VID of virus information to retrieve
//  lphVirus        Pointer to handle to store handle to virus
//  lpVirusInfo     Pointer to VIRUS_INFO_T struct to store info
//  lpbFound        Pointer to BOOL to store found status
//
// Description:
//  Opens File:VIRSCAN1.DAT Section:VIRUS_RECORD_DATA_SECTION
//  and retrieves virus record information for VID into the
//  structure pointed to by lpVirusInfo.
//
//  The function performs a binary search through the virus
//  record information section of VIRSCAN1.DAT using wVID
//  as the key.
//
//  *lpbFound is set to TRUE if a record with a matching VID was found.
//  Otherwise, *lpbFound is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngGetVirusInfoVIDAux
(
    HGENG               hGEng,
    WORD                wVID,
    LPHVIRUS            lphVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPBOOL              lpbFound
)
{
    LPVOID              lpvFileInfo;
    DATA_FILE_TABLE_T   stInfo;
    BOOL                bSectionFound;
    WORD				wNumRecords, wRecordSize;
    DWORD               dwOff;
    DWORD               dwBytesRead, dwBytesToRead;
    long                lMin, lMax, lCur, lResult;
    LPDATAFILECALLBACKS lpDataFileCallBacks;

    // assume we can't find what we're looking for...

    *lpbFound = FALSE;

    lpDataFileCallBacks = hGEng->lpDataFileCallBacks;

    ///////////////////////////////////////////////////////////////////
    //
    //  Open VIRSCAN1.DAT section VIRINFO section
    //
    ///////////////////////////////////////////////////////////////////

    // open our data file...

    if (EngOpenDataFile(lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN1DAT,
                       FILE_OPEN_READ_ONLY,
                       &lpvFileInfo) == DFSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // locate the virus database section

    if (EngLookUp(lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_VIRINFO,
                 &stInfo,
                 &bSectionFound) == DFSTATUS_ERROR)
    {
        // close and exit on error

        EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

    // seek to the section...

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileSeek(lpvFileInfo,
                                      stInfo.dwStartOffset,
                                      SEEK_SET,
                                      &dwOff) == CBSTATUS_ERROR)
    {
        // close and exit on error

        EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    //  Read number of records and record size
    //
    ///////////////////////////////////////////////////////////////////

    // read in the number of records

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileRead(lpvFileInfo,
                                     &wNumRecords,
                                     sizeof(WORD),
                                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(WORD))
    {
        // close and exit on error

        EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

    // read in the size of each record

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileRead(lpvFileInfo,
                                     &wRecordSize,
                                     sizeof(WORD),
                                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(WORD))
    {
        // close and exit on error

        EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

    wRecordSize = WENDIAN(wRecordSize);

    if (wRecordSize < sizeof(VIRUS_INFO_T))
        dwBytesToRead = wRecordSize;
    else
        dwBytesToRead = sizeof(VIRUS_INFO_T);


    ///////////////////////////////////////////////////////////////////
    //
    //  Binary search for our data
    //
    ///////////////////////////////////////////////////////////////////

    lMin = 0;                               // set minimum possible record
    lMax = WENDIAN(wNumRecords) - 1;        // set maximum possible record


    lCur = (lMin + lMax) >> 1;             /* divide by 2 */

    while (lMin <= lMax)
    {
        // calculate the offset of our virus info record

        dwOff = stInfo.dwStartOffset + 2*sizeof(WORD) +
                lCur * wRecordSize;

        // seek and read in the record from VIRSCAN1.DAT

        if (lpDataFileCallBacks->
                lpFileCallBacks->FileSeek(lpvFileInfo,
                                          dwOff,
                                          SEEK_SET,
                                          &dwOff) == CBSTATUS_ERROR)
        {
            // close and exit on error

            EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

            return(ENGSTATUS_ERROR);
        }

        if (lpDataFileCallBacks->
                lpFileCallBacks->FileRead(lpvFileInfo,
                                         (LPVOID)lpVirusInfo,
                                         dwBytesToRead,
                                         &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != dwBytesToRead)
        {
            // close and exit on error

            EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

            return(ENGSTATUS_ERROR);
        }

        // make sure to endianize our virus ID.  its the only field we
        // examine during the binary search...

        lResult = (long)(WORD)wVID - (long)WENDIAN(lpVirusInfo->wVirusID);

        if (!lResult)
        {
            // We found our record.  Return it even if its deleted.  Let
            // the application worry about the display, etc.

            EndianizeVirusInfo(lpVirusInfo);
            *lpbFound = TRUE;

            // report the index into the database

            *lphVirus = (HVIRUS)lCur;

            EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

            return(ENGSTATUS_OK);
        }

        if (lResult < 0)
            lMax = lCur - 1;
        else
            lMin = lCur + 1;

        lCur = (lMin + lMax) >> 1;
    }

    EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

    return(ENGSTATUS_OK);
}


