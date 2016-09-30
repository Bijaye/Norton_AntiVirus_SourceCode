// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/GETINFO.CPv   1.6   08 Jul 1997 17:49:26   MKEATIN  $
//
// Description:
//  Functions for accessing virus information from data files.
//
// Contains:
//  EngGetNumVirusRecords()
//  EngGetUserInfoIndex()
//  EngGetUserInfoHVIRUS()
//  EngGetUserInfoVID()
//  EngGetN30HVIRUS()
//
// See Also:
//  infoaux.c, filerep.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/GETINFO.CPv  $
// 
//    Rev 1.6   08 Jul 1997 17:49:26   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.5   08 Jul 1997 17:19:54   MKEATIN
// Passed NULL as the lpvProvidedFileInfo parameters in various functions.
// The old default NULL in the old prototype won't compile for the NLM
// C compiler.
// 
//    Rev 1.4   03 Jun 1997 18:08:04   MKEATIN
// Fixed a bug in EngGetClaimedNumViruses().
// 
//    Rev 1.3   03 Jun 1997 17:43:14   MKEATIN
// Ported EngGetNumClaimedViruses() from NEWVIR.
// 
//    Rev 1.2   15 May 1997 12:19:40   MKEATIN
// Made iterating though records faster by providing an open file info
// pointer (handle) to info routines.
// 
//    Rev 1.1   13 May 1997 18:27:02   MKEATIN
// Ported Carey's boot caching code.
// 
//    Rev 1.0   18 Apr 1997 13:37:48   MKEATIN
// Initial revision.
// 
//    Rev 1.12   01 Nov 1996 17:04:40   DCHI
// Corrected casting problem in get virus name function for calculating offset.
// 
//    Rev 1.11   29 Aug 1996 14:28:08   DCHI
// Added DFDataFileClose to end of GetNumVirusRecords and added check
// on error for DFDataFileClose to GetVirusName.
// 
//    Rev 1.10   13 Aug 1996 12:29:06   CNACHEN
// Added support for new Macro, Agent and Windows flags.
// 
//    Rev 1.9   17 Jul 1996 16:16:30   CNACHEN
// Corrected EngGetInfoVID to use the bool variable pointer passed in.
// 
//    Rev 1.8   06 Jun 1996 18:52:20   DCHI
// Code now checks wFlags and ORs in appropriate flags into Ctrl and Ctrl2 of N30.
// 
//    Rev 1.7   05 Jun 1996 18:08:00   CNACHEN
// Fixed lpbFound == FALSE comparison to *lpbFound == FALSE...
// 
//    Rev 1.6   05 Jun 1996 17:36:18   CNACHEN
// Fixed a bug in N30 getting function.
// 
//    Rev 1.5   05 Jun 1996 13:28:20   CNACHEN
// Added new EngGetN30HVIRUS() function
// 
// 
//    Rev 1.4   17 May 1996 14:16:02   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
// 
//    Rev 1.3   16 May 1996 14:12:56   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.2   15 May 1996 16:53:40   DCHI
// Fixed GetUserInfoIndex() which was not retrieving the VID.
// 
//    Rev 1.1   15 May 1996 11:19:44   DCHI
// Modified EngVIDtoHVIRUS() to report error only on real errors.
// 
//    Rev 1.0   13 May 1996 16:29:18   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"
#include "n30type.h"

//*************************************************************************
//
// Function:
//  EngGetVirusNameAux()
//
// Parameters:
//  lpDataFileCallBacks Pointer to data file callback structure
//  hVirus              Handle to virus whose name to get
//  lpvFileInfo         VIRSCAN1.DAT file info pointer
//  lpIndexInfo         Name index section information
//  lpDataInfo          Names section information
//  lpsVirusName        Pointer to buffer to store name
//
// Description:
//  This function retrieves to lpstrVirusName the name of hVirus.
//  The function first finds the index of the name in the name index
//  section of VIRSCAN1.DAT and then reads the name at that index
//  from the names section of VIRSCAN1.DAT.  The index is actually
//  a byte offset.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetVirusNameAux
(
    LPDATAFILECALLBACKS     lpDataFileCallBacks,
    HVIRUS                  hVirus,
    LPVOID                  lpvFileInfo,
    LPDATA_FILE_TABLE       lpIndexInfo,
    LPDATA_FILE_TABLE       lpDataInfo,
    LPSTR                   lpstrVirusName
)
{
    DWORD                   dwOff, dwBytesRead, dwNameDataOff;
    BYTE                    byData[NAME_INDEX_POINTER_SIZE];
    BYTE                    byLen;

    ///////////////////////////////////////////////////////////////////
    //
    //  Calculate offset into names section
    //
    ///////////////////////////////////////////////////////////////////

    // determine where our index entry is and read the 3 byte value there.
    // zero extend this 3 byte value into a DWORD and use this as an index
    // into our name data section.

    dwOff = lpIndexInfo->dwStartOffset + sizeof(WORD) + sizeof(WORD) +
            (DWORD)NAME_INDEX_POINTER_SIZE * (DWORD)hVirus;

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileSeek(lpvFileInfo,
                                      dwOff,
                                      SEEK_SET,
                                      &dwOff) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileRead(lpvFileInfo,
                                     byData,
                                     NAME_INDEX_POINTER_SIZE,
                                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != NAME_INDEX_POINTER_SIZE)
    {
        return(ENGSTATUS_ERROR);
    }

    dwNameDataOff = byData[0] | ((DWORD)byData[1] << 8) |
                    ((DWORD)byData[2] << 16);

    // calculate the absolute offset in the data file where we can find our
    // data.  the sizeof(WORD) is to account for the WORD count of names at
    // the top of the data table

    dwOff = dwNameDataOff + sizeof(WORD) + lpDataInfo->dwStartOffset;


    ///////////////////////////////////////////////////////////////////
    //
    //  Read name
    //
    ///////////////////////////////////////////////////////////////////

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileSeek(lpvFileInfo,
                                      dwOff,
                                      SEEK_SET,
                                      &dwOff) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // First read the length of the name

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileRead(lpvFileInfo,
                                      (LPVOID)&byLen,
                                      sizeof(BYTE),
                                      &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(BYTE))
    {
        return(ENGSTATUS_ERROR);
    }

    // read in the virus name.  The other bytes are the name data,
    //  in OEM format.

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileRead(lpvFileInfo,
                                     lpstrVirusName,
                                     byLen,
                                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != byLen)
    {
        return(ENGSTATUS_ERROR);
    }

    // Null terminate the string.

    lpstrVirusName[byLen] = 0;

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGetVirusName()
//
// Parameters:
//  hGEng               Handle to global engine context
//  hVirus              Handle to virus whose name to get
//  lpstrVirusName      Pointer to buffer to store name
//
// Description:
//  This function retrieves to lpstrVirusName the name of hVirus.
//  The function calls EngGetVirusNameAux() after opening VIRSCAN1.DAT
//  and locating the name index and names sections.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS   EngGetVirusName
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPSTR               lpstrVirusName,
    LPVOID              lpvProvidedFileInfo
)
{
    LPVOID              lpvFileInfo;
    DATA_FILE_TABLE_T   stIndexInfo, stDataInfo;
    BOOL                bSectionFound;
    LPBOOT_INFO_LIST    lpstBIL;

    // if we're dealing with VxD and have stuff cached, look in memory first
    // then fall thru to disk (which should error out if we have no file system)

    if (hGEng->dwInitStatus & GENG_FLAG_BOOT_INFO_LOADED)
    {
        lpstBIL = hGEng->lpBootScanInfo->lpstBootInfoList;

        while (lpstBIL != NULL)
        {
            if (lpstBIL->hVirus == hVirus)
            {
                strcpy(lpstrVirusName,lpstBIL->lpszVirusName);

                return(ENGSTATUS_OK);
            }

            lpstBIL = lpstBIL->lpstNext;
        }
    }

    ///////////////////////////////////////////////////////////////////
    //
    //  Open VIRSCAN1.DAT and locate name index and names sections
    //
    ///////////////////////////////////////////////////////////////////

    // open our data file...

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

    // locate the index section

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_NAMESIDX,
                 &stIndexInfo,
                 &bSectionFound) == DFSTATUS_ERROR)
    {
        // close and exit on error

        goto Cleanup;
    }

    if (bSectionFound == FALSE)
    {
        // specified section was not present in the file...
        // close and exit

        goto Cleanup;
    }

    // locate the data section

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_NAMES,
                 &stDataInfo,
                 &bSectionFound) == DFSTATUS_ERROR)
    {
        // close and exit on error

        goto Cleanup;
    }

    if (bSectionFound == FALSE)
    {
        // specified section was not present in the file...
        // close and exit

        goto Cleanup;
    }

    ///////////////////////////////////////////////////////////////////
    //
    //  Call EngGetVirusNameAux() to retrieve the name
    //
    ///////////////////////////////////////////////////////////////////

    if (EngGetVirusNameAux(hGEng->lpDataFileCallBacks,
                           hVirus,
                           lpvFileInfo,
                           &stIndexInfo,
                           &stDataInfo,
                           lpstrVirusName) != ENGSTATUS_OK)
    {
        goto Cleanup;
    }

    if (lpvProvidedFileInfo == NULL)
        return (EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo));

    return(ENGSTATUS_OK);

Cleanup:

    if (lpvProvidedFileInfo == NULL)
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);

    return(ENGSTATUS_ERROR);
}


//*************************************************************************
//
// Function:
//  EngGetNumVirusRecords()
//
// Parameters:
//  hGEng               Handle to global engine context
//  lpwNumVirusRecords  Ptr to WORD to store number of virus records
//
// Description:
//  Call this function to get the number of virus information
//  records in the database.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetNumVirusRecords
(
    HGENG               hGEng,
    LPWORD              lpwNumVirusRecords
)
{
    LPVOID              lpvFileInfo;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwBytesRead;

    ///////////////////////////////////////////////////////////////////
    //
    //  Open VIRSCAN1.DAT section VIRINFO section
    //
    ///////////////////////////////////////////////////////////////////

    // open the virus information data file...

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN1DAT,
                       FILE_OPEN_READ_ONLY,
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
    //  Read number of virus information records
    //
    ///////////////////////////////////////////////////////////////////

    // Read number of records

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)lpwNumVirusRecords,
                     sizeof(WORD),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(WORD))
    {
        // Read of # of records failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Close

    if (EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo) !=
        DFSTATUS_OK)
        return(ENGSTATUS_ERROR);

    *lpwNumVirusRecords = WENDIAN(*lpwNumVirusRecords);

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGetNumClaimedViruses()
//
// Parameters:
//  hGEng               Handle to global engine context
//  lpdwNumClaimedViruses Ptr to DWORD to store claimed number of viruses
//
// Description:
//  Call this function to get the claimed number of viruses
//  detected with this definition set.  This is not the same
//  as the actual number of virus records.  This value is
//  typically displayed to the user.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetNumClaimedViruses
(
    HGENG               hGEng,
    LPDWORD             lpdwNumClaimedViruses
)
{
    LPVOID              lpvFileInfo;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwBytesRead;

    ///////////////////////////////////////////////////////////////////
    //
    //  Open VIRSCAN1.DAT file SECTION_ID_CLAIMED_NUM_VIRUSES section
    //
    ///////////////////////////////////////////////////////////////////

    // open the virus information data file...

    if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN1DAT,
                       FILE_OPEN_READ_ONLY,
                       &lpvFileInfo) == DFSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_CLAIMED_NUM_VIRUSES,
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
    //  Read claimed number of viruses detected by this def set
    //
    ///////////////////////////////////////////////////////////////////

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)lpdwNumClaimedViruses,
                     sizeof(DWORD),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(DWORD))
    {
        // Read failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Close

    if (EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo) !=
        DFSTATUS_OK)
        return(ENGSTATUS_ERROR);

    *lpdwNumClaimedViruses = DWENDIAN(*lpdwNumClaimedViruses);

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGetUserInfoIndex()
//
// Parameters:
//  hGEng               Handle to global engine context
//  wIndex              Index of record to get
//  lpVirusUserInfo     Pointer to VIRUS_USER_INFO_T structure to store
//                      virus information record
//
// Description:
//  Call this function to get the virus information record for the
//  virus at wIndex
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetUserInfoIndex
(
    HGENG               hGEng,
    WORD                wIndex,
    LPVIRUS_USER_INFO   lpVirusUserInfo
)
{
    VIRUS_INFO_T        stVirusInfo;

    if (EngGetVirusInfoAux(hGEng,
                           wIndex,
                           &stVirusInfo,
                           NULL) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // get the virus name from VIRSCAN1.DAT

    if (EngGetVirusName(hGEng,
                        wIndex,
                        lpVirusUserInfo->sVirusName,
                        NULL) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // lets put it into the proper structure for the user to get...
    // the lpVirusInfo structure should be endian neutral at this time
    // (EngGetVirusNameInfoAux should do the conversion when it reads the data)

    lpVirusUserInfo->hVirus = wIndex;
    lpVirusUserInfo->wVirusID = stVirusInfo.wVirusID;
    lpVirusUserInfo->dwInfo = stVirusInfo.dwInfo;
    lpVirusUserInfo->wInfSize = stVirusInfo.wInfSize;
    lpVirusUserInfo->wFlags = stVirusInfo.wFlags; // BOO, CRC, ALG, etc

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGetInfoHVIRUS()
//
// Parameters:
//  hGEng               Handle to global engine context
//  hVirus              Index of record to get
//  lpVirusInfo         Pointer to VIRUS_INFO_T structure to store
//                      virus information record
//  lpstrVirusName      Pointer to buffer to store virus name
//
// Description:
//  Call this function to get the virus information record for the
//  virus at wIndex
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetInfoHVIRUS
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPSTR               lpstrVirusName,
    LPVOID              lpvProvidedFileInfo
)
{
    HVIRUS              hVirusFound;
    BOOL                bFound;

    // assume we can't locate the record...

    bFound = FALSE;

    // if we have an error with the cache, something's really wrong
    // return with error.  Note: if we don't find the record in the cache,
    // this is *NOT* an error.

    // Note: Don't waste time on the cache if we've provided an vFileInfo.
    // We may be iterating through the def set.

    if ((lpvProvidedFileInfo == NULL) &&
        (EngCacheLocateInfoHVIRUS(hVirus,
                                 &hGEng->stVirusInfoCache,
                                 lpVirusInfo,
                                 &hVirusFound,
                                 lpstrVirusName,
                                 &bFound) != ENGSTATUS_OK))
    {
        return(ENGSTATUS_ERROR);
    }

    // found the element in the cache?

    if (bFound == FALSE)
    {
        // not in the cache.  get the information record from VIRSCAN1.DAT.
        // The EngGetVirusInfoAux function returns ENGSTATUS_ERROR if its
        // unable to locate the proper record.

        if (EngGetVirusInfoAux(hGEng,
                               hVirus,
                               lpVirusInfo,
                               lpvProvidedFileInfo) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // get the virus name from VIRSCAN1.DAT

        if (EngGetVirusName(hGEng,
                            hVirus,
                            lpstrVirusName,
                            lpvProvidedFileInfo) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // insert the name and info record into the cache...

        // Note: Don't waste time on the cache if we've provided an vFileInfo.
        // We may be iterating through the def set.

        if ((lpvProvidedFileInfo == NULL) &&
            (EngCacheInsertItem(&hGEng->stVirusInfoCache,
                                hVirus,
                                lpVirusInfo,
                                lpstrVirusName) == ENGSTATUS_ERROR))
            return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGetUserInfoHVIRUS()
//
// Parameters:
//  hGEng               Handle to global engine context
//  hVirus              Index of record to get
//  lpVirusUserInfo     Pointer to VIRUS_USER_INFO_T structure to store
//                      virus information record
//
// Description:
//  Call this function to get the virus information record for the
//  virus with handle hVirus
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetUserInfoHVIRUS
(
    HGENG               hGEng,
    HVIRUS              hVirus,
    LPVIRUS_USER_INFO   lpVirusUserInfo
)
{
    VIRUS_INFO_T        stVirusInfo;

    if (EngGetInfoHVIRUS(hGEng,
                         hVirus,
                         &stVirusInfo,
                         lpVirusUserInfo->sVirusName,
                         NULL) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    // now that we have our data from either the cache or the VIRSCAN1.DAT file
    // lets put it into the proper structure for the user to get...
    // the lpVirusInfo structure should be endian neutral at this time
    // (EngGetVirusNameInfoAux should do the conversion when it reads the data)

    lpVirusUserInfo->hVirus = hVirus;
    lpVirusUserInfo->wVirusID = stVirusInfo.wVirusID;
    lpVirusUserInfo->dwInfo = stVirusInfo.dwInfo;
    lpVirusUserInfo->wInfSize = stVirusInfo.wInfSize;
    lpVirusUserInfo->wFlags = stVirusInfo.wFlags; // BOO, CRC, ALG, etc

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGetInfoVID()
//
// Parameters:
//  hGEng               Handle to global engine context
//  wVID                VID of virus record to get
//  lphVirus            Pointer to HVIRUS to store handle to virus
//  lpVirusInfo         Pointer to VIRUS_INFO_T structure to store
//                      virus information record
//  lpstrVirusName      Pointer to buffer to store virus name
//  lpbFound        Pointer to BOOL to store found status
//
// Description:
//  Call this function to get the virus information record for the
//  virus with virus ID wVID.
//
//  *lpbFound is set to TRUE if a record with a matching VID was found.
//  Otherwise, *lpbFound is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetInfoVID
(
    HGENG               hGEng,
    WORD                wVID,
    LPHVIRUS            lphVirus,
    LPVIRUS_INFO        lpVirusInfo,
    LPSTR               lpstrVirusName,
    LPBOOL              lpbFound
)
{
    LPBOOT_INFO_LIST    lpstBIL;

    // assume we can't locate the record...

    *lpbFound = FALSE;

    // try looking in our in-memory cache

    if (hGEng->dwInitStatus & GENG_FLAG_BOOT_INFO_LOADED)
    {
        lpstBIL = hGEng->lpBootScanInfo->lpstBootInfoList;

        while (lpstBIL != NULL)
        {
            if (lpstBIL->stVirusInfo.wVirusID == wVID)
            {
                *lphVirus = lpstBIL->hVirus;
                *lpVirusInfo = lpstBIL->stVirusInfo;
                strcpy(lpstrVirusName,lpstBIL->lpszVirusName);

                *lpbFound = TRUE;

                return(ENGSTATUS_OK);
            }

            lpstBIL = lpstBIL->lpstNext;
        }
    }

    // if we have an error with the cache, something's really wrong
    // return with error.  Note: if we don't find the record in the cache,
    // this is *NOT* an error.

    if (EngCacheLocateInfoVID(wVID,
                              &hGEng->stVirusInfoCache,
                              lpVirusInfo,
                              lphVirus,
                              lpstrVirusName,
                              lpbFound) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    // found the element in the cache?

    if (*lpbFound == FALSE)
    {
        // not in the cache.  get the information record from VIRSCAN1.DAT.
        // The EngGetVirusInfoAux function returns ENGSTATUS_ERROR if its
        // unable to locate the proper record.

        if (EngGetVirusInfoVIDAux(hGEng,
                                  wVID,
                                  lphVirus,
                                  lpVirusInfo,
                                  lpbFound) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (*lpbFound == FALSE)
        {
            // No such entry

            return(ENGSTATUS_OK);
        }

        // get the virus name from VIRSCAN1.DAT

        if (EngGetVirusName(hGEng,
                            *lphVirus,
                            lpstrVirusName,
                            NULL) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // insert the name and info record into the cache...

        if (EngCacheInsertItem(&hGEng->stVirusInfoCache,
                               *lphVirus,
                               lpVirusInfo,
                               lpstrVirusName) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngGetUserInfoVID()
//
// Parameters:
//  hGEng               Handle to global engine context
//  wVID                VID of record to get
//  lpVirusUserInfo     Pointer to VIRUS_USER_INFO_T structure to store
//                      virus information record
//
// Description:
//  Call this function to get the virus information record for the
//  virus with virus ID wVID
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetUserInfoVID
(
    HGENG               hGEng,
    WORD                wVID,
    LPVIRUS_USER_INFO   lpVirusUserInfo
)
{
    HVIRUS              hVirus;
    VIRUS_INFO_T        stVirusInfo;
    BOOL                bFound;

    if (EngGetInfoVID(hGEng,
                      wVID,
                      &hVirus,
                      &stVirusInfo,
                      lpVirusUserInfo->sVirusName,
                      &bFound) != ENGSTATUS_OK ||
        bFound == FALSE)
    {
        return(ENGSTATUS_ERROR);
    }

    // now that we have our data from either the cache or the VIRSCAN1.DAT file
    // lets put it into the proper structure for the user to get...
    // the lpVirusInfo structure should be endian neutral at this time
    // (EngGetVirusNameInfoAux should do the conversion when it reads the data)

    lpVirusUserInfo->hVirus = hVirus;
    lpVirusUserInfo->wVirusID = stVirusInfo.wVirusID;
    lpVirusUserInfo->dwInfo = stVirusInfo.dwInfo;
    lpVirusUserInfo->wInfSize = stVirusInfo.wInfSize;
    lpVirusUserInfo->wFlags = stVirusInfo.wFlags; // BOO, CRC, ALG, etc

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngVIDToHVIRUS()
//
// Parameters:
//  hGEng           Handle to global engine context
//  wVID            Virus ID
//  lphVirus        Pointer to HVIRUS to store virus handle
//  lpbFound        Pointer to BOOL to store found status
//
// Description:
//  Call this function to translate a VID to an HVIRUS.
//
//  *lpbFound is set to TRUE if a translation could be performed (e.g.,
//  a record with a matching VID was found).  Otherwise, *lpbFound is
//  set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngVIDToHVIRUS
(
    HGENG       hGEng,
    WORD        wVID,
    LPHVIRUS    lphVirus,
    LPBOOL      lpbFound
)
{
    VIRUS_INFO_T        stVirusInfo;
    char                sVirusName[MAX_VIRUS_NAME_LENGTH];

    return EngGetInfoVID(hGEng,
                         wVID,
                         lphVirus,
                         &stVirusInfo,
                         sVirusName,
                         lpbFound);
}



//*************************************************************************
//
// Function:
//  EngGetN30HVIRUS()
//
// Parameters:
//  hGEng               Handle to global engine context
//  hVirus              Index of record to get
//  lpstN30             Pointer to N30 structure to store
//                      old-style virus information record
// Description:
//  Call this function to get an old-style N30 structure from an avenge
//  virus record.  This structure will not have its memory signature or file
//  signature filled in.  If the definition does not have any repair flags,
//  then all repair information will also be zeroed, regardless of the initial
//  contents of the N30 record.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngGetN30HVIRUS
(
    HLENG               hLEng,
    HVIRUS              hVirus,
    LPN30               lpstN30
)
{
    VIRUS_INFO_T        stVirusInfo;
    VIRUS_REPAIR_T      stVirusRepairInfo = {0};
    char                byTempName[256];
    N30                 stTempN30 = {0};
    ENGSTATUS           esResult;
    BYTE                byDecryptSig[DECRYPT_MEM_SIG_SIZE];

    // obtain our virus information

    if (EngGetInfoHVIRUS(hLEng->hGEng, 
                         hVirus, 
                         &stVirusInfo, 
                         byTempName, 
                         NULL) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    // set up the fields one at a time

    // first fill in virus ID

    stTempN30.wVirID = stVirusInfo.wVirusID;

    // copy over the virus size

    stTempN30.wInfSize = stVirusInfo.wInfSize;

    // next fill in the def type

    if (stVirusInfo.wFlags & AVENGE_TYPE_CRC)
        stTempN30.wType = CRCtype;
    if (stVirusInfo.wFlags & AVENGE_TYPE_ALG)
        stTempN30.wType = ALGtype;
    if (stVirusInfo.wFlags & AVENGE_TYPE_BOO)
        stTempN30.wType = BOOtype;
    if (stVirusInfo.wFlags & AVENGE_TYPE_MAC)
        stTempN30.wType = MACtype;

    // now retrieve the repair record for all kinds of cool info!  We only
    // fetch the record if it exists.  The stVirusRepairInfo is initialized
    // to zero, so if the current def doesn't have repair information then
    // all fields will be 0 when we copy them into our N30 structure.

    if (stVirusInfo.wFlags &
        (AVENGE_TYPE_HAS_REPAIR | AVENGE_TYPE_VIRSCAN2_REPAIR))
    {
        esResult = GetRepairRecord (hLEng,
                                    &stVirusInfo,
                                    &stVirusRepairInfo,
                                    byDecryptSig );

        if (esResult != ENGSTATUS_OK)
            return(ENGSTATUS_ERROR);
    }

    // next copy info from the Avenge repair structure to the N30 structure
    // (if there is a repair information structure)

    stTempN30.wCrc              = stVirusRepairInfo.wCRC;
    stTempN30.wTagSize          = stVirusRepairInfo.wTagSize;
    stTempN30.wHostJmp          = stVirusRepairInfo.wHostJmp;
    stTempN30.wJmpLen           = stVirusRepairInfo.wJmpLen;
    stTempN30.wSS_reg           = stVirusRepairInfo.wSS_reg;
    stTempN30.wSP_reg           = stVirusRepairInfo.wSP_reg;
    stTempN30.wCS_reg           = stVirusRepairInfo.wCS_reg;
    stTempN30.wIP_reg           = stVirusRepairInfo.wIP_reg;
    stTempN30.wHeader           = stVirusRepairInfo.wHeader;
    stTempN30.wHeadLen          = stVirusRepairInfo.wHeadLen;
    stTempN30.wBookLocation     = stVirusRepairInfo.wBookLocation;
    stTempN30.wBookMark         = stVirusRepairInfo.wBookMark;
    stTempN30.dwReserved        = stVirusRepairInfo.dwReserved;

    // Avenge dwRepairFlags -> N30 wControl and N30 wControl2

    stTempN30.wControl = (WORD)(stVirusRepairInfo.dwRepairFlags & 0xFFFFU);
    stTempN30.wControl2 = (WORD)(stVirusRepairInfo.dwRepairFlags >> 16);

    // The following flags are in wFlags and need to be added
    //  to the N30 structure that we return

    if (stVirusInfo.wFlags & AVENGE_TYPE_NOCLEAN)
        stTempN30.wControl |= N30_CTRL_NOCLEAN;

    if (stVirusInfo.wFlags & AVENGE_TYPE_NOT_IN_TSR)
        stTempN30.wControl2 |= N30_CTRL2_NOT_IN_TSR;

    if (stVirusInfo.wFlags & AVENGE_TYPE_NO_GENERIC)
        stTempN30.wControl2 |= N30_CTRL2_NO_GENERIC;

    // the next two items set up support for Macro, Agent and Windows
    // virus types, which are encoded in the two high bits of the wControl2
    // field of the N30 structure.  (They are contained in the flags word of
    // the avenge info structure.)

    if (stVirusInfo.wFlags & AVENGE_TYPE_EXTENSION_LOW)
        stTempN30.wControl2 |= N30_CTRL2_BIT_FLAG2I;

    if (stVirusInfo.wFlags & AVENGE_TYPE_EXTENSION_HIGH)
        stTempN30.wControl2 |= N30_CTRL2_BIT_FLAG2J;

    // now set up our info flags (only the low word is from N30)

    stTempN30.wInfo = (WORD)(stVirusInfo.dwInfo & 0xFFFFU);

    // now copy the virus name into the record

    MEMCPY(stTempN30.lpVName,
           byTempName,
           N30_SIZE_VIRUSNAME);

    *lpstN30 = stTempN30;

    return(ENGSTATUS_OK);
}

