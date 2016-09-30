// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/DELENTRY.CPv   1.2   08 Jul 1997 17:49:28   MKEATIN  $
//
// Description:
//  Functions for deleting a virus entry from the data files such that
//  the virus will no longer be detected.
//
// Contains:
//  EngDeleteVirusEntry()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/DELENTRY.CPv  $
// 
//    Rev 1.2   08 Jul 1997 17:49:28   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.1   08 Jul 1997 17:20:00   MKEATIN
// Passed NULL as the lpvProvidedFileInfo parameters in various functions.
// The old default NULL in the old prototype won't compile for the NLM
// C compiler.
// 
//    Rev 1.0   18 Apr 1997 13:37:56   MKEATIN
// Initial revision.
// 
//    Rev 1.2   17 May 1996 14:15:52   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
// 
//    Rev 1.1   16 May 1996 14:13:04   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.0   13 May 1996 16:29:26   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

//********************************************************************
//
// Function:
//  EngDeleteMEMSig()
//
// Parameters:
//  hGEng           Handle to global engine context
//  hVirus          Handle of virus MEM sig to delete
//
// Description:
//  Opens File:VIRSCAN4.DAT Section:MEMORY_SCAN_DATA_SECTION
//  and deletes the memory signature for hVirus.  If no memory
//  signature for hVirus exists, then the function still returns
//  success.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngDeleteMEMSig
(
    HGENG               hGEng,
    HVIRUS              hVirus
)
{
    LPVOID              lpvFileInfo;
    WORD                i, w, wNumRecords;
    DWORD               dwBytesWritten;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwDefOffset, dwBytesRead;
    MEMSIG_T            stMemSig;

    ///////////////////////////////////////////////////////////////////
	//
    //  Open and seek to MEMORY sigs section
	//
	///////////////////////////////////////////////////////////////////

	if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN4DAT,
                       FILE_OPEN_READ_WRITE,
					   &lpvFileInfo) == DFSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_MEM_SIG,
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
    //  Search CD mem sig section
	//
	///////////////////////////////////////////////////////////////////

    // Read number of CD sigs

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

    wNumRecords = WENDIAN(wNumRecords);

    dwDefOffset = stDataFileTableEntry.dwStartOffset + sizeof(WORD);

    // Search through mem CD sigs

    for (w=0;w<wNumRecords;w++)
    {
        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileRead(lpvFileInfo,
                         (LPVOID)&stMemSig,
                         sizeof(MEM_CD_SIG_T),
                         &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != sizeof(MEM_CD_SIG_T))
        {
            // Read of CD sig failed

            EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
            return(ENGSTATUS_ERROR);
        }

        if (WENDIAN(stMemSig.wVirusRecordIndex) == (WORD)hVirus)
        {
            stMemSig.wVirusRecordIndex = WENDIAN(INVALID_VIRUS_HANDLE);

            // Found a match

            // Seek back to wVirusRecordIndex of sig

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileSeek(lpvFileInfo,
                             dwDefOffset,
                             SEEK_SET,
                             &dwOffset) == CBSTATUS_ERROR)
            {
                // Seek failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            // Write mem sig with INVALID_VIRUS_HANDLE out

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileWrite(lpvFileInfo,
                              (LPVOID)&stMemSig,
                              sizeof(MEM_CD_SIG_T),
                              &dwBytesWritten) == CBSTATUS_ERROR ||
                dwBytesWritten != sizeof(MEM_CD_SIG_T))
            {
                // Write of mem sig with INVALID_VIRUS_HANDLE failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            return(ENGSTATUS_OK);
        }

        dwDefOffset += sizeof(MEM_CD_SIG_T);
    }


    ///////////////////////////////////////////////////////////////////
	//
    //  Search ALL and TOM mem sig section
	//
	///////////////////////////////////////////////////////////////////

    for (i=0;i<2;i++)
    {
        // Read number of ALL or TOM sigs

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

        dwDefOffset += sizeof(WORD);

        // Search through mem ALL sigs

        for (w=0;w<wNumRecords;w++)
        {
            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileRead(lpvFileInfo,
                             (LPVOID)&stMemSig,
                             sizeof(MEM_ALL_SIG_T),
                             &dwBytesRead) == CBSTATUS_ERROR ||
                dwBytesRead != sizeof(MEM_ALL_SIG_T))
            {
                // Read of ALL or TOM sig failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            if (WENDIAN(stMemSig.wVirusRecordIndex) == (WORD)hVirus)
            {
                stMemSig.wVirusRecordIndex = WENDIAN(INVALID_VIRUS_HANDLE);

                // Found a match

                // Seek back to wVirusRecordIndex of sig

                if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                        FileSeek(lpvFileInfo,
                                 dwDefOffset,
                                 SEEK_SET,
                                 &dwOffset) == CBSTATUS_ERROR)
                {
                    // Seek failed

                    EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                    return(ENGSTATUS_ERROR);
                }

                // Write mem sig with INVALID_VIRUS_HANDLE out

                if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                        FileWrite(lpvFileInfo,
                                  (LPVOID)&stMemSig,
                                  sizeof(MEM_ALL_SIG_T),
                                  &dwBytesWritten) == CBSTATUS_ERROR ||
                    dwBytesWritten != sizeof(MEM_ALL_SIG_T))
                {
                    // Write of mem sig with INVALID_VIRUS_HANDLE failed

                    EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                    return(ENGSTATUS_ERROR);
                }

                return(ENGSTATUS_OK);
            }

            dwDefOffset += sizeof(MEM_ALL_SIG_T);
        }
    }

    // If not found, just return OK

    return(EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo));
}


//********************************************************************
//
// Function:
//  EngDeleteBOOSig()
//
// Parameters:
//  hGEng           Handle to global engine context
//  hVirus          Handle of virus BOO sig to delete
//
// Description:
//  Opens File:VIRSCAN4.DAT Section:BOO_SCAN_DATA_SECTION
//  and deletes the boot signature for hVirus.  If no boot
//  signature for hVirus exists, then the function still returns
//  success.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngDeleteBOOSig
(
    HGENG               hGEng,
    HVIRUS              hVirus
)
{
    LPVOID              lpvFileInfo;
    WORD                w, wNumRecords;
    DWORD               dwBytesWritten;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwDefOffset, dwBytesRead;
    BOO_SIG_DATA_T      stBOOSigData;

    ///////////////////////////////////////////////////////////////////
	//
    //  Open and seek to BOO sigs section
	//
	///////////////////////////////////////////////////////////////////

	if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN4DAT,
                       FILE_OPEN_READ_WRITE,
					   &lpvFileInfo) == DFSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_BOO_SIG,
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
    //  Search BOO sig section
	//
	///////////////////////////////////////////////////////////////////

    // Read number of BOO sigs

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

    wNumRecords = WENDIAN(wNumRecords);

    dwDefOffset = stDataFileTableEntry.dwStartOffset + sizeof(WORD);

    // Search through BOO sigs

    for (w=0;w<wNumRecords;w++)
    {
        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileRead(lpvFileInfo,
                         (LPVOID)&stBOOSigData,
                         sizeof(BOO_SIG_DATA_T),
                         &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != sizeof(BOO_SIG_DATA_T))
        {
            // Read of BOO sig failed

            EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
            return(ENGSTATUS_ERROR);
        }

        if (WENDIAN(stBOOSigData.wVirusRecordIndex) == (WORD)hVirus)
        {
            stBOOSigData.wVirusRecordIndex = WENDIAN(INVALID_VIRUS_HANDLE);

            // Found a match

            // Seek back to wVirusRecordIndex of sig

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileSeek(lpvFileInfo,
                             dwDefOffset,
                             SEEK_SET,
                             &dwOffset) == CBSTATUS_ERROR)
            {
                // Seek failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            // Write boo sig with INVALID_VIRUS_HANDLE out

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileWrite(lpvFileInfo,
                              (LPVOID)&stBOOSigData,
                              sizeof(BOO_SIG_DATA_T),
                              &dwBytesWritten) == CBSTATUS_ERROR ||
                dwBytesWritten != sizeof(BOO_SIG_DATA_T))
            {
                // Write of INVALID_VIRUS_HANDLE failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            return(ENGSTATUS_OK);
        }

        dwDefOffset += sizeof(BOO_SIG_DATA_T);
    }

    // If not found, just return OK

    return(EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo));
}


//********************************************************************
//
// Function:
//  EngDeleteCRCSig()
//
// Parameters:
//  hGEng           Handle to global engine context
//  hVirus          Handle of virus CRC sig to delete
//
// Description:
//  Opens File:VIRSCAN4.DAT Section:CRC_SCAN_DATA_SECTION
//  and deletes the CRC signature for hVirus.  If no CRC
//  signature for hVirus exists, then the function still returns
//  success.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngDeleteCRCSig
(
    HGENG               hGEng,
    HVIRUS              hVirus
)
{
    LPVOID              lpvFileInfo;
    WORD                w, wNumRecords;
    DWORD               dwBytesWritten;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwDefOffset, dwBytesRead;
    CRC_SIG_DATA_T      stCRCSigData;

    ///////////////////////////////////////////////////////////////////
	//
    //  Open and seek to CRC sigs section
	//
	///////////////////////////////////////////////////////////////////

	if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN4DAT,
                       FILE_OPEN_READ_WRITE,
					   &lpvFileInfo) == DFSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_CRC_SIG,
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
    //  Search CRC sig section
	//
	///////////////////////////////////////////////////////////////////

    // Read number of CRC sigs

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

    wNumRecords = WENDIAN(wNumRecords);

    dwDefOffset = stDataFileTableEntry.dwStartOffset + sizeof(WORD);

    // Search through CRC sigs

    for (w=0;w<wNumRecords;w++)
    {
        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileRead(lpvFileInfo,
                         (LPVOID)&stCRCSigData,
                         sizeof(CRC_SIG_DATA_T),
                         &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != sizeof(CRC_SIG_DATA_T))
        {
            // Read of CRC sig failed

            EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
            return(ENGSTATUS_ERROR);
        }

        if (WENDIAN(stCRCSigData.wVirusRecordIndex) == (WORD)hVirus)
        {
            stCRCSigData.wVirusRecordIndex = WENDIAN(INVALID_VIRUS_HANDLE);

            // Found a match

            // Seek back to wVirusRecordIndex of sig

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileSeek(lpvFileInfo,
                         dwDefOffset,
                         SEEK_SET,
                         &dwOffset) == CBSTATUS_ERROR)
            {
                // Seek failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            // Write CRC sig with INVALID_VIRUS_HANDLE out

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileWrite(lpvFileInfo,
                              (LPVOID)&stCRCSigData,
                              sizeof(CRC_SIG_DATA_T),
                              &dwBytesWritten) == CBSTATUS_ERROR ||
                dwBytesWritten != sizeof(CRC_SIG_DATA_T))
            {
                // Write of CRC sig with INVALID_VIRUS_HANDLE failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            return(ENGSTATUS_OK);
        }

        dwDefOffset += sizeof(CRC_SIG_DATA_T);
    }

    // If not found, just return OK

    return(EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo));
}


//********************************************************************
//
// Function:
//  EngDeleteALGSig()
//
// Parameters:
//  hGEng           Handle to global engine context
//  hVirus          Handle of virus ALG sig to delete
//
// Description:
//  Opens File:VIRSCAN4.DAT Section:ALG_WILD_SCAN_DATA_SECTION
//  and deletes the wild ALG signature for hVirus.  If no wild ALG
//  signature for hVirus exists, then the function opens
//  File:VIRSCAN4.DAT Section:ALG_ZOO_SCAN_DATA_SECTION
//  and deletes the zoo ALG signature for hVirus.  If no wild ALG
//  and no zoo ALG signature for hVirus exists, then the function
//  still returns success.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngDeleteALGSig
(
    HGENG               hGEng,
    HVIRUS              hVirus
)
{
    LPVOID              lpvFileInfo;
    WORD                w;
    DWORD               dwBytesWritten;
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwDefOffset, dwBytesRead;
    HASH_INFO_T         stHashInfo;
    ALG_SIG_HDR_T       stAlgSigHdr;

    ///////////////////////////////////////////////////////////////////
	//
    //  Open and seek to ALG_WILD
	//
	///////////////////////////////////////////////////////////////////

	if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                       DATA_FILE_ID_VIRSCAN4DAT,
                       FILE_OPEN_READ_WRITE,
					   &lpvFileInfo) == DFSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_ALG_ITW_SIG,
                 &stDataFileTableEntry,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // Error during lookup and/or section not found

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Seek to beginning of ALG_WILD section

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
    //  Search ALG_WILD sig section
	//
	///////////////////////////////////////////////////////////////////

    // Read number of ALG_WILD sigs

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)&stHashInfo,
                     sizeof(stHashInfo),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(stHashInfo))
	{
        // Read of # of records failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
		return(ENGSTATUS_ERROR);
	}

    stHashInfo.wNumSignatures = WENDIAN(stHashInfo.wNumSignatures);

    dwDefOffset = stDataFileTableEntry.dwStartOffset + sizeof(stHashInfo);

    // Search through ALG_WILD sigs

    for (w=0;w<stHashInfo.wNumSignatures;w++)
    {
        // Read the header for this ALG sig

        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileSeek(lpvFileInfo,
                         dwDefOffset,
                         SEEK_SET,
                         &dwOffset) == CBSTATUS_ERROR)
        {
            // Seek failed

            EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
            return(ENGSTATUS_ERROR);
        }

        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileRead(lpvFileInfo,
                         (LPVOID)&stAlgSigHdr,
                         sizeof(ALG_SIG_HDR_T),
                         &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != sizeof(ALG_SIG_HDR_T))
        {
            // Read of ALG header failed

            EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
            return(ENGSTATUS_ERROR);
        }

        if (WENDIAN(stAlgSigHdr.wVirusRecordIndex) == (WORD)hVirus)
        {
            stAlgSigHdr.wVirusRecordIndex = WENDIAN(INVALID_VIRUS_HANDLE);

            // Found a match

            // Seek back to wVirusRecordIndex of sig

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileSeek(lpvFileInfo,
                             dwDefOffset,
                             SEEK_SET,
                             &dwOffset) == CBSTATUS_ERROR)
            {
                // Seek failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            // Write of alg sig hdr with INVALID_VIRUS_HANDLE out

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileWrite(lpvFileInfo,
                              (LPVOID)&stAlgSigHdr,
                              sizeof(ALG_SIG_HDR_T),
                              &dwBytesWritten) == CBSTATUS_ERROR ||
                dwBytesWritten != sizeof(ALG_SIG_HDR_T))
            {
                // Write of alg sig hdr with INVALID_VIRUS_HANDLE failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            return(ENGSTATUS_OK);
        }

        dwDefOffset += sizeof(ALG_SIG_HDR_T) + stAlgSigHdr.bySigLength;
    }


    ///////////////////////////////////////////////////////////////////
	//
    //  Seek to ALG_ZOO sig section
	//
	///////////////////////////////////////////////////////////////////

    // Find ALG_ZOO section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_ALG_ZOO_SIG,
                 &stDataFileTableEntry,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // Error during lookup and/or section not found

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
        return(ENGSTATUS_ERROR);
    }

    // Seek to beginning of ALG_ZOO section

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
    //  Search ALG_ZOO sig section
	//
	///////////////////////////////////////////////////////////////////

    // Read number of ALG_ZOO sigs

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     (LPVOID)&stHashInfo,
                     sizeof(stHashInfo),
                     &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(stHashInfo))
	{
        // Read of # of records failed

        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
		return(ENGSTATUS_ERROR);
	}

    stHashInfo.wNumSignatures = WENDIAN(stHashInfo.wNumSignatures);

    dwDefOffset = stDataFileTableEntry.dwStartOffset + sizeof(stHashInfo);

    // Search through ALG_ZOO sigs

    for (w=0;w<stHashInfo.wNumSignatures;w++)
    {
        // Read the header for this ALG sig

        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileSeek(lpvFileInfo,
                         dwDefOffset,
                         SEEK_SET,
                         &dwOffset) == CBSTATUS_ERROR)
        {
            // Seek failed

            EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
            return(ENGSTATUS_ERROR);
        }

        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileRead(lpvFileInfo,
                         (LPVOID)&stAlgSigHdr,
                         sizeof(ALG_SIG_HDR_T),
                         &dwBytesRead) == CBSTATUS_ERROR ||
            dwBytesRead != sizeof(ALG_SIG_HDR_T))
        {
            // Read of ALG header failed

            EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
            return(ENGSTATUS_ERROR);
        }

        if (WENDIAN(stAlgSigHdr.wVirusRecordIndex) == (WORD)hVirus)
        {
            stAlgSigHdr.wVirusRecordIndex = WENDIAN(INVALID_VIRUS_HANDLE);

            // Found a match

            // Seek back to wVirusRecordIndex of sig

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileSeek(lpvFileInfo,
                             dwDefOffset,
                             SEEK_SET,
                             &dwOffset) == CBSTATUS_ERROR)
            {
                // Seek failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            // Write alg sig hdr with INVALID_VIRUS_HANDLE out

            if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                    FileWrite(lpvFileInfo,
                              (LPVOID)&stAlgSigHdr,
                              sizeof(ALG_SIG_HDR_T),
                              &dwBytesWritten) == CBSTATUS_ERROR ||
                dwBytesWritten != sizeof(ALG_SIG_HDR_T))
            {
                // Write of alg sig hdr INVALID_VIRUS_HANDLE failed

                EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo);
                return(ENGSTATUS_ERROR);
            }

            return(ENGSTATUS_OK);
        }

        dwDefOffset += sizeof(ALG_SIG_HDR_T) + stAlgSigHdr.bySigLength;
    }

    // If not found, just return OK

    return(EngCloseDataFile(hGEng->lpDataFileCallBacks,lpvFileInfo));
}


//********************************************************************
//
// Function:
//  EngDeleteVirusEntry()
//
// Parameters:
//  hGEng           Handle to global engine context
//  hVirus          Handle of virus ALG sig to delete
//
// Description:
//  This function deletes the virus information and signatures
//  for hVirus so that the given virus is no longer detected.
//  It performs the following actions:
//      1. Deletes the memory signature by setting the virus
//          record index field to INVALID_VIRUS_HANDLE (0xFFFF)
//      2. Deletes either the CRC, ALG, or BOO signature depending
//          on the wFlags field of the virus info structure by
//          setting the virus record index field to
//          INVALID_VIRUS_HANDLE (0xFFFF).
//      3. Sets AVENGE_TYPE_DELETED in the wFlags field of the virus
//          info entry for hVirus.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

ENGSTATUS EngDeleteVirusEntry
(
    HGENG       hGEng,
    HVIRUS      hVirus
)
{
    VIRUS_INFO_T    stVirusInfo;

    ///////////////////////////////////////////////////////////////////
	//
    //  Get the virus information to determine what type of signature
	//
	///////////////////////////////////////////////////////////////////

    if (EngGetVirusInfoAux(hGEng,hVirus,&stVirusInfo,NULL) == ENGSTATUS_ERROR)
    {
        // Failure getting virus information

        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
	//
    //  Delete memory signature
	//
	///////////////////////////////////////////////////////////////////

    if (EngDeleteMEMSig(hGEng,hVirus) == ENGSTATUS_ERROR)
    {
        // Failure deleting memory signature

        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
	//
    //  Delete either CRC, ALG, or BOO signature
	//
	///////////////////////////////////////////////////////////////////

    if ((stVirusInfo.wFlags & AVENGE_TYPE_CRC) != 0)
    {
        if (EngDeleteCRCSig(hGEng,hVirus) == ENGSTATUS_ERROR)
        {
            // Failure deleting CRC signature

            return(ENGSTATUS_ERROR);
        }
    }
    else
    if ((stVirusInfo.wFlags & AVENGE_TYPE_ALG) != 0)
    {
        if (EngDeleteALGSig(hGEng,hVirus) == ENGSTATUS_ERROR)
        {
            // Failure deleting ALG signature

            return(ENGSTATUS_ERROR);
        }
    }
    else
    if ((stVirusInfo.wFlags & AVENGE_TYPE_BOO) != 0)
    {
        if (EngDeleteBOOSig(hGEng,hVirus) == ENGSTATUS_ERROR)
        {
            // Failure deleting BOO signature

            return(ENGSTATUS_ERROR);
        }
    }

    ///////////////////////////////////////////////////////////////////
	//
    //  Now modify the flags in the virus info struct to mark deleted
    //  and write out new flags
	//
	///////////////////////////////////////////////////////////////////

    stVirusInfo.wFlags |= AVENGE_TYPE_DELETED;

    return (EngPutVirusInfoAux(hGEng,hVirus,&stVirusInfo));
}

