// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/infc.cpv   1.1   08 Jul 1997 17:49:30   MKEATIN  $
//
// Description:
//  Functions for loading information recs many at a time for fast
//  virus list loading.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/infc.cpv  $
// 
//    Rev 1.1   08 Jul 1997 17:49:30   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.0   20 May 1997 13:17:56   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "avenge.h"

ENGSTATUS EngInitInfRecLoad
(
    HGENG                       hGEng,
    LPVOID                      lpvFileInfo,
    LPINFREC_LOAD_INFO          lpstNLI
)
{
    DATA_FILE_TABLE_T       stDataFileSection;
    BOOL                    bSectionFound = FALSE;
    WORD                    wSize, wNumRecords;
    DWORD                   dwBytesRead, dwBytes, dwOff;

    if (lpvFileInfo == NULL)
    {
        if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                           DATA_FILE_ID_VIRSCAN1DAT,
                           FILE_OPEN_READ_ONLY,
                           &lpstNLI->lpvFileInfo) == DFSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        lpstNLI->bNoFileHandle = FALSE;
    }
    else
    {
        lpstNLI->lpvFileInfo = lpvFileInfo;
        lpstNLI->bNoFileHandle = TRUE;
    }

    if (EngLookUp(hGEng->lpDataFileCallBacks,
                 lpstNLI->lpvFileInfo,
                 SECTION_ID_VIRINFO,
                 &stDataFileSection,
                 &bSectionFound) != DFSTATUS_OK)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpstNLI->lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

    lpstNLI->dwSectionStart = stDataFileSection.dwStartOffset +
        sizeof(WORD) + sizeof(WORD);

    // now read in the size & # of records

    if (hGEng->lpDataFileCallBacks->
                lpFileCallBacks->FileSeek(lpstNLI->lpvFileInfo,
                                          stDataFileSection.dwStartOffset,
                                          SEEK_SET,
                                          &dwOff) == CBSTATUS_ERROR)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpstNLI->lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }


    // read in # of records

    if (hGEng->lpDataFileCallBacks->
        lpFileCallBacks->FileRead(lpstNLI->lpvFileInfo,
                                  &wNumRecords,
                                  sizeof(WORD),
                                  &dwBytesRead) == CBSTATUS_ERROR)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpstNLI->lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }
    
	lpstNLI->dwTotalRecsLeft = wNumRecords;

    // read in size/record

    if (hGEng->lpDataFileCallBacks->
            lpFileCallBacks->FileRead(lpstNLI->lpvFileInfo,
                                      &wSize,
                                      sizeof(WORD),
                                      &dwBytesRead) == CBSTATUS_ERROR)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpstNLI->lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

	lpstNLI->dwRecSize = wSize;

    dwBytes = (DWORD)wSize * (DWORD)MAX_RECS_TO_HOLD;

    if (dwBytes > MAX_INFREC_BUFFER_SIZE)
        dwBytes = (MAX_INFREC_BUFFER_SIZE / wSize) * wSize;

    lpstNLI->dwRecsInBuffer = dwBytes / wSize;

    if (hGEng->lpGeneralCallBacks->
            MemoryAlloc(dwBytes,
                        MEM_FIXME,
                        (LPLPVOID)&lpstNLI->lpbyInfRecBuffer) == CBSTATUS_ERROR)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpstNLI->lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

    // get start offset of the section

    // reset current offset in buffer, etc.

    lpstNLI->dwTotalIndex = 0;
    lpstNLI->dwBufferRecsLeft = 0;
    lpstNLI->dwBufferIndex = 0;

    return(ENGSTATUS_OK);
}

ENGSTATUS EngGetNextInfRec
(
    HGENG                       hGEng,
    LPINFREC_LOAD_INFO          lpstNLI,
    LPVIRUS_INFO                lpstVirusInfo,
    LPBOOL                      lpbDone
)
{
    DWORD                   dwRecsToRead, dwOff, dwBytesRead;

    *lpbDone = FALSE;

    // time to refill our pool

    if (lpstNLI->dwBufferRecsLeft == 0)
    {
        dwRecsToRead = lpstNLI->dwTotalRecsLeft;
        if (dwRecsToRead > lpstNLI->dwRecsInBuffer)
            dwRecsToRead = lpstNLI->dwRecsInBuffer;

        if (dwRecsToRead == 0)
        {
            *lpbDone = TRUE;

            return(ENGSTATUS_OK);
        }

        // seek to the next offset to read data

        if (hGEng->lpDataFileCallBacks->
                lpFileCallBacks->FileSeek(lpstNLI->lpvFileInfo,
                                          lpstNLI->dwSectionStart +
                                           lpstNLI->dwTotalIndex *
                                           lpstNLI->dwRecSize,
                                          SEEK_SET,
                                          &dwOff) == CBSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        // read in next chunk

        if (hGEng->lpDataFileCallBacks->
                lpFileCallBacks->FileRead(lpstNLI->lpvFileInfo,
                                          lpstNLI->lpbyInfRecBuffer,
                                          dwRecsToRead * lpstNLI->dwRecSize,
                                          &dwBytesRead) == CBSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        lpstNLI->dwBufferRecsLeft = dwRecsToRead;
        lpstNLI->dwBufferIndex = 0;
    }

    // now get that InfRec

    *lpstVirusInfo = *(LPVIRUS_INFO)(lpstNLI->lpbyInfRecBuffer +
                                     (lpstNLI->dwRecSize *
                                      lpstNLI->dwBufferIndex));

    // reduce amount of InfRec records left in the stream

    lpstNLI->dwTotalRecsLeft--;
    lpstNLI->dwTotalIndex++;
    lpstNLI->dwBufferRecsLeft--;
    lpstNLI->dwBufferIndex++;

    // OK

    return(ENGSTATUS_OK);
}

ENGSTATUS EngCloseInfRecLoad
(
    HGENG                       hGEng,
    LPINFREC_LOAD_INFO          lpstNLI
)
{
    ENGSTATUS               engStatus = ENGSTATUS_OK;

    if (lpstNLI->bNoFileHandle == FALSE &&
        EngCloseDataFile(hGEng->lpDataFileCallBacks,
                        lpstNLI->lpvFileInfo) == DFSTATUS_ERROR)
        engStatus = ENGSTATUS_ERROR;

    if (hGEng->lpGeneralCallBacks->MemoryFree(lpstNLI->lpbyInfRecBuffer) !=
        CBSTATUS_OK)
        engStatus = ENGSTATUS_ERROR;

    return(engStatus);
}


