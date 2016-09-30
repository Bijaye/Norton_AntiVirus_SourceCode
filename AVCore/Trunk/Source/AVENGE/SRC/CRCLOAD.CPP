// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/CRCLOAD.CPv   1.1   08 Jul 1997 17:49:18   MKEATIN  $
//
// Description:
//  Functions for loading and unloading of CRC signature data.
//
// Contains:
//  EngCRCLoad()
//  EngCRCUnload()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/CRCLOAD.CPv  $
// 
//    Rev 1.1   08 Jul 1997 17:49:18   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.0   18 Apr 1997 13:37:22   MKEATIN
// Initial revision.
// 
//    Rev 1.2   17 May 1996 14:15:56   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
// 
//    Rev 1.1   16 May 1996 14:12:40   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.0   13 May 1996 16:28:50   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

//*************************************************************************
//
// Function:
//  EngCRCLoadAux()
//
// Parameters:
//  hGEng           Handle to global engine context
//  lpvFileInfo     Pointer to file info for VIRSCAN4.DAT
//
// Description:
//  Looks up SECTION_ID_CRC_SIG in VIRSCAN4.DAT and loads the CRC
//  signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCRCLoadAux
(
    HGENG               hGEng,
    LPVOID              lpvFileInfo
)
{
    DATA_FILE_TABLE_T   stDataFileTableEntry;
    BOOL                bSectionFound;
    DWORD               dwOffset, dwBytesRead;
    WORD                wNumSigs;
    int                 i;
    LPCRC_SIG_DATA      lpCRCSigData;

    // Find section in data file

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpvFileInfo,
                 SECTION_ID_CRC_SIG,
                 &stDataFileTableEntry,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // Error during lookup and/or section not found

        return(ENGSTATUS_ERROR);
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Determine number of CRC sigs
    //
    ///////////////////////////////////////////////////////////////////

    // Seek to number of CRCs

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileSeek(lpvFileInfo,
                     stDataFileTableEntry.dwStartOffset,
                     SEEK_SET,
                     &dwOffset) == CBSTATUS_ERROR)
    {
        // Seek failed

        return(ENGSTATUS_ERROR);
    }

    // Read number of CRCs

    if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
            FileRead(lpvFileInfo,
                     &wNumSigs,
                     sizeof(wNumSigs),
                     &dwBytesRead) == CBSTATUS_ERROR ||
		dwBytesRead != sizeof(wNumSigs))
	{
		// Read of # of CRCs failed

		return(ENGSTATUS_ERROR);
	}

	// Endian enable wNumCRCSigs

	wNumSigs = WENDIAN(wNumSigs);


	///////////////////////////////////////////////////////////////////
	//
	// Allocate memory
	//
	///////////////////////////////////////////////////////////////////

	// Set number of CRC sigs field

	hGEng->stCRCScanInfo.wNumSigs = wNumSigs;

    // Determine number of pools needed

	hGEng->stCRCScanInfo.wNumCRCPools =
        (wNumSigs + MAX_CRC_SIGS_PER_POOL - 1) /
        MAX_CRC_SIGS_PER_POOL;

    // Now allocate pool memory

    for (i=0;i<(int)hGEng->stCRCScanInfo.wNumCRCPools;i++)
    {
        // Determine how many sigs for this pool

        if (i == (int)(hGEng->stCRCScanInfo.wNumCRCPools - 1) &&
            wNumSigs % MAX_CRC_SIGS_PER_POOL != 0)
			hGEng->stCRCScanInfo.CRCPoolArray[i].wNumSigs =
                wNumSigs % MAX_CRC_SIGS_PER_POOL;
        else
			hGEng->stCRCScanInfo.CRCPoolArray[i].wNumSigs =
                MAX_CRC_SIGS_PER_POOL;

        // Allocate pool memory

        if (hGEng->lpGeneralCallBacks->
				MemoryAlloc(hGEng->stCRCScanInfo.CRCPoolArray[i].wNumSigs *
                                sizeof(CRC_SIG_DATA_T),
                            MEM_FIXME,
							(LPLPVOID)&(hGEng->stCRCScanInfo.
								CRCPoolArray[i].
                                lpCRCSigData)) == CBSTATUS_ERROR)
        {
            // Free pool memory

            while (--i >= 0)
            {
                hGEng->lpGeneralCallBacks->
					MemoryFree(hGEng->stCRCScanInfo.CRCPoolArray[i].
                        lpCRCSigData);
            }

			return(ENGSTATUS_ERROR);
        }
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Read CRC sigs into pools
    //
    ///////////////////////////////////////////////////////////////////

    for (i=0;i<(int)hGEng->stCRCScanInfo.wNumCRCPools;i++)
    {
        // Read CRCs

        if (hGEng->lpDataFileCallBacks->lpFileCallBacks->
                FileRead(lpvFileInfo,
						 hGEng->stCRCScanInfo.CRCPoolArray[i].lpCRCSigData,
						 hGEng->stCRCScanInfo.CRCPoolArray[i].wNumSigs *
                            sizeof(CRC_SIG_DATA_T),
                         &dwBytesRead) == CBSTATUS_ERROR ||
			dwBytesRead != hGEng->stCRCScanInfo.CRCPoolArray[i].wNumSigs *
                              sizeof(CRC_SIG_DATA_T))
        {
            // Read of pool of CRCs failed

            // Free all pool memory

            for (i=0;i<(int)hGEng->stCRCScanInfo.wNumCRCPools;i++)
            {
                hGEng->lpGeneralCallBacks->
					MemoryFree(hGEng->stCRCScanInfo.CRCPoolArray[i].
                        lpCRCSigData);
            }

			return(ENGSTATUS_ERROR);
        }
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Endian enable necessary wVirusRecordIndex and wCRC
    //  fields of CRC sigs
    //
    ///////////////////////////////////////////////////////////////////

    for (i=0;i<(int)hGEng->stCRCScanInfo.wNumCRCPools;i++)
    {
		lpCRCSigData = hGEng->stCRCScanInfo.CRCPoolArray[i].lpCRCSigData;

        for (wNumSigs=0;
			 wNumSigs<hGEng->stCRCScanInfo.CRCPoolArray[i].wNumSigs;
             wNumSigs++,lpCRCSigData++)
        {
            lpCRCSigData->wVirusRecordIndex =
                WENDIAN(lpCRCSigData->wVirusRecordIndex);

            lpCRCSigData->wCRC = WENDIAN(lpCRCSigData->wCRC);
        }
    }

	// Success

	return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngCRCUnload()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Frees memory allocated for CRC signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCRCUnload
(
	HGENG   hGEng
)
{
	int i;

	// Free all pool memory

    for (i=0;i<(int)hGEng->stCRCScanInfo.wNumCRCPools;i++)
	{
		hGEng->lpGeneralCallBacks->
			MemoryFree(hGEng->stCRCScanInfo.CRCPoolArray[i].
				lpCRCSigData);
	}

	hGEng->stCRCScanInfo.wNumCRCPools = 0;

	return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  EngCRCLoad()
//
// Parameters:
//  hGEng           Handle to global engine context
//
// Description:
//  Opens VIRSCAN4.DAT and calls EngCRCLoadAux() to load the CRC
//  signatures.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS EngCRCLoad
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

	engStatus = EngCRCLoadAux(hGEng,lpvFileInfo);

	if (EngCloseDataFile(hGEng->lpDataFileCallBacks, lpvFileInfo) ==
		DFSTATUS_ERROR)
	{
		// error closing the data file? if we were able to load our signature
		// data then free it...

		if (engStatus != DFSTATUS_ERROR)
		{
			EngCRCUnload(hGEng);
		}

		return(ENGSTATUS_ERROR);
	}

	return(ENGSTATUS_OK);
}

