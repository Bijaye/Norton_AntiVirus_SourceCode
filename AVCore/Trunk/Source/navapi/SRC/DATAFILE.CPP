// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/datafile.cpv   1.3   11 Jun 1998 14:38:14   MKEATIN  $
//
// Description:
//
//  This source file contains support routines for acessing PAM's indexed
//  data files.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVAPI/VCS/datafile.cpv  $
//
//    Rev 1.3   11 Jun 1998 14:38:14   MKEATIN
// We now use PAMFilexxx routines for ALL file operations.
//
//    Rev 1.2   21 May 1998 20:30:34   MKEATIN
// Changed pamapi.h to pamapi_l.h
//
//    Rev 1.1   21 May 1998 19:37:54   MKEATIN
// Changed PMFilexxxx to Filexxxx
//
//    Rev 1.0   21 May 1998 19:23:32   MKEATIN
// Initial revision.
//
//    Rev 1.11   29 May 1996 17:28:30   CNACHEN
// Changed code to look for "SYMC" signature or 0xDEADBEEFUL (for legacy
// purposes).
//
//    Rev 1.10   28 Feb 1996 11:35:22   CNACHEN
// close file if memory alloc fails..
//
//    Rev 1.9   28 Feb 1996 10:10:26   CNACHEN
// Fixed a memory free bug...
//
//
//    Rev 1.8   21 Feb 1996 10:59:56   CNACHEN
// Added comments...
//
//    Rev 1.7   20 Feb 1996 11:26:56   CNACHEN
// Changed all LPSTRs to LPTSTRs.
//
//
//    Rev 1.6   13 Dec 1995 11:57:50   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
//
//    Rev 1.5   19 Oct 1995 18:23:34   CNACHEN
// Initial revision... with comment header :)
//
//************************************************************************


#include "avendian.h"
#include "pamapi_l.h"


//************************************************************************
// DFSTATUS DFOpenDataFile(lpDataFile, uMode, lphInfo)
//
// Purpose
//
//      This function is called to open an indexed database.  If the database
//  is successfully opened, the function will provide the caller with a handle
//  to access the database.  This handle will need to be closed with
//  DFCloseDataFile to properly shut down.  This function verifies the
//  integrity of the database by checking for a signature at the top of the
//  file header.  While a data file is open, one file handle is used.
//
// Parameters:
//
//      LPTSTR          lpDataFile  Filename of the data file to open
//      UINT            uMode       Open mode (usually some form of READ_ONLY)
//      DATAFILEHANDLE  *lphInfo    *lphInfo is set to the handle on success
//
// Returns:
//
//      DFSTATUS_OK                 On success
//      DFSTATUS_FILE_ERROR         On file error
//                                  (DFCloseDataFile need not be called)
//      DFSTATUS_MEM_ERROR          On memory allocation error
//                                  (DFCloseDataFile need not be called)
//
//************************************************************************
// 2/21/96 Carey created.
//************************************************************************

DFSTATUS DFOpenDataFile(LPTSTR lpDataFile,
                        UINT uMode,
                        DATAFILEHANDLE *lphInfo)
{
    HFILE                   hStream;
    UINT                    uBytesRead;
    DATAFILEHANDLE          lpTempInfo;

    *lphInfo = NULL;



    hStream = PAMFileOpen(lpDataFile,uMode);
    if ((HFILE)-1 == hStream)
        return(DFSTATUS_FILE_ERROR);

    lpTempInfo = (DATAFILEHANDLE)PMemoryAlloc(sizeof(DataFileHandleType));
    if (NULL == lpTempInfo)
    {
        PAMFileClose(hStream);

        return(DFSTATUS_MEM_ERROR);
    }

    memset (lpTempInfo, 0, sizeof(DataFileHandleType));

    uBytesRead = PAMFileRead(hStream,&(lpTempInfo->sHeader),sHEADERTYPE);
    if (uBytesRead != sHEADERTYPE)
    {
        PMemoryFree(lpTempInfo);
        PAMFileClose(hStream);
        return(DFSTATUS_FILE_ERROR);
    }

#ifdef BIG_ENDIAN
    lpTempInfo->sHeader.dwTableOffset =
        DWENDIAN(lpTempInfo->sHeader.dwTableOffset);
    lpTempInfo->sHeader.dwSignature =
        DWENDIAN(lpTempInfo->sHeader.dwSignature);
    lpTempInfo->sHeader.dwVersionNumber =
        DWENDIAN(lpTempInfo->sHeader.dwVersionNumber);
    lpTempInfo->sHeader.wNumSections =
        WENDIAN(lpTempInfo->sHeader.wNumSections);
    lpTempInfo->sHeader.dwCRC =
        DWENDIAN(lpTempInfo->sHeader.dwCRC);
#endif

    // the DEADBEEF below is legacy code to support QA tools, etc...

    if (SIG_VALUE != lpTempInfo->sHeader.dwSignature &&
        0xDEADBEEFUL != lpTempInfo->sHeader.dwSignature)
    {
        PMemoryFree(lpTempInfo);
        PAMFileClose(hStream);
        return(DFSTATUS_INVALID_FORMAT);
    }

    /* check CRC here if we get real anal */

	lpTempInfo->hHandle = hStream;

    *lphInfo = lpTempInfo;

    return(DFSTATUS_OK);
}


//************************************************************************
// HFILE DFGetHandle(hInfo)
//
// Purpose
//
//      This function is called to obtain the file handle that is maintained
//  by a DATAFILEHANDLE object.  It allows the caller to directly read the
//  contents of a datafile.  Basically, after calling DFLookUp to find where
//  a given record is in the datafile, DFGetHandle can be called to obtain a
//  file handle to read the contents of this record from the datafile.
//
// Parameters:
//
//      DATAFILEHANDLE  hInfo     hInfo is a valid data file handle
//
// Returns:
//
//      HFILE of the datafile.
//
//************************************************************************
// 2/21/96 Carey created.
//************************************************************************

HFILE DFGetHandle(DATAFILEHANDLE hInfo)
{
    return(hInfo->hHandle);
}


//************************************************************************
// DFSTATUS DFLookUp(hInfo, dwIdent, lpdwVersionNumber, lpdwStartOffset,
//                   lpdwLength, lpbyMon, lpbyDay, lpbyYear)
//
// Purpose
//
//      This function is called to locate a section (record) in the database.
//  Each section in the database is identified by a DWORD identifier (dwIdent).
//  This function searches through the index in the datafile until it locates
//  the appropriate record.  It will then store information about the section
//  into the *lp parameters.  If a section can not be located, then the
//  function returns DFSTATUS_NO_ENTRY_FOUND is returned, and the *lp values
//  are invalid.
//
//  Currently, the lpdwVersionNumber, lpbyMon, lpbyDay and lpbyYear are unused
//  in the datafile.  E.g. they are all set to 0.
//
//
//  Data file format:
//
//                  (File header)
//                  (Section table - entry 1 (TableType))
//                  (Section table - entry 2 (TableType))
//                  ...
//                  (Section table - entry n (TableType))
//                  (Record/section #1 - raw data)
//                  ...
//                  (Record/section #2 - raw data)
//                  ...
//                  ...
//                  ...
//                  ...
//                  ...
//                  (Record/section #3 - raw data)
//
// Parameters:
//
//      DATAFILEHANDLE  hInfo       hInfo is a valid data file handle
//      DWORD           dwIdent     Section identifier - key for the section
//                                  we want to locate
//
//      DWORD           *lpdwVersionNumber  Version number of the section
//                                          (User defined)
//      DWORD           *lpdwStartOffset    Absolute offset from TOF to section
//      DWORD           *lpdwLength         Size in bytes of the section
//      BYTE            *lpbyMon            Month date stamp of section
//      BYTE            *lpbyDay            Day date stamp of section
//      BYTE            *lpbyYear           Year date stamp of section
//                                          (relative to 1900)
//
// Returns:
//
//      DFSTATUS_OK             If a section was successfully located.
//      DFSTATUS_FILE_ERROR     If there was an error accessing the database.
//      DFSTATUS_NO_ENTRY_FOUND If the specified entry was not located.
//
//************************************************************************
// 2/21/96 Carey created.
//************************************************************************

DFSTATUS DFLookUp(DATAFILEHANDLE hInfo,
                  DWORD dwIdent,
                  DWORD *lpdwVersionNumber,
                  DWORD *lpdwStartOffset,
				  DWORD *lpdwLength,
                  BYTE  *lpbyMon, BYTE *lpbyDay, BYTE *lpbyYear)
{
    DWORD           dwOff;
    TableType       sTableEntry;
    WORD            wCurSection;
    UINT            uBytes;

    dwOff = PAMFileSeek(hInfo->hHandle,hInfo->sHeader.dwTableOffset,SEEK_SET);

	if (dwOff != hInfo->sHeader.dwTableOffset)
        return (DFSTATUS_FILE_ERROR);

    /* search in the table for the proper ID so we can find where the
       section is in our data file */

	for (wCurSection=0;wCurSection<hInfo->sHeader.wNumSections;wCurSection++)
    {
        uBytes = PAMFileRead(hInfo->hHandle,&sTableEntry,sTABLETYPE);
        if (sTABLETYPE != uBytes)
			return(DFSTATUS_FILE_ERROR);

#ifdef BIG_ENDIAN
        sTableEntry.dwIdent = DWENDIAN(sTableEntry.dwIdent);
        sTableEntry.dwVersionNumber = DWENDIAN(sTableEntry.dwVersionNumber);
        sTableEntry.dwStartOffset = DWENDIAN(sTableEntry.dwStartOffset);
        sTableEntry.dwLength = DWENDIAN(sTableEntry.dwLength);
#endif

        /* found the specified table entry.  report to the user */

        if (sTableEntry.dwIdent == dwIdent)
        {
            if (lpdwVersionNumber)
                *lpdwVersionNumber = sTableEntry.dwVersionNumber;
            if (lpdwStartOffset)
				*lpdwStartOffset = sTableEntry.dwStartOffset;
            if (lpdwLength)
                *lpdwLength = sTableEntry.dwLength;
            if (lpbyMon)
                *lpbyMon = sTableEntry.byMon;
            if (lpbyDay)
                *lpbyDay = sTableEntry.byDay;
            if (lpbyYear)
                *lpbyYear = sTableEntry.byYear;

            return(DFSTATUS_OK);
        }
    }

    /* the specified Identifier was not found in the table of contents */

    return(DFSTATUS_NO_ENTRY_FOUND);
}



//************************************************************************
// void DFCloseDataFile(hInfo)
//
// Purpose
//
//      This function is called to close the specified data file and free any
//  memory required by the datafile handle.  It should be called after
//  processing of the datafile is complete, and only when DFOpenDataFile
//  returns DFSTATUS_OK.
//
// Parameters:
//
//      DATAFILEHANDLE  hInfo       hInfo is a valid data file handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/21/96 Carey created.
//************************************************************************

void DFCloseDataFile(DATAFILEHANDLE hInfo)
{
    PAMFileClose(hInfo->hHandle);
    PMemoryFree(hInfo);
}


