// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/namec.cpv   1.1   08 Jul 1997 17:49:32   MKEATIN  $
//
// Description:
//  Functions for loading virus names many at a time for fast
//  virus list loading.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/namec.cpv  $
// 
//    Rev 1.1   08 Jul 1997 17:49:32   MKEATIN
// Changed DFFunctions() to EngFunctions().
// 
//    Rev 1.0   20 May 1997 13:18:10   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "avenge.h"

ENGSTATUS EngInitNameLoad
(
    HGENG                   hGEng,
    LPVOID                  lpvFileInfo,
    LPNAME_LOAD_INFO        lpstNLI
)
{
    DATA_FILE_TABLE_T       stDataFileSection;
    BOOL                    bSectionFound = FALSE;

    if (hGEng->lpGeneralCallBacks->
            MemoryAlloc(NAME_BUFFER_SIZE,
                        MEM_FIXME,
                        (LPLPVOID)&lpstNLI->lpbyNameBuffer) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    if (lpvFileInfo == NULL)
    {
        if (EngOpenDataFile(hGEng->lpDataFileCallBacks,
                           DATA_FILE_ID_VIRSCAN1DAT,
                           FILE_OPEN_READ_ONLY,
                           &lpstNLI->lpvFileInfo) == DFSTATUS_ERROR)
        {
            hGEng->lpGeneralCallBacks->MemoryFree(lpstNLI->lpbyNameBuffer);
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
                 SECTION_ID_NAMES,
                 &stDataFileSection,
                 &bSectionFound) != DFSTATUS_OK)
    {
        EngCloseDataFile(hGEng->lpDataFileCallBacks,lpstNLI->lpvFileInfo);
        hGEng->lpGeneralCallBacks->MemoryFree(lpstNLI->lpbyNameBuffer);

        return(ENGSTATUS_ERROR);
    }

    // get length of all string data

    lpstNLI->dwDataLeft = stDataFileSection.dwLength - sizeof(WORD);

    // get start offset of the section

    lpstNLI->dwSectionStart = stDataFileSection.dwStartOffset + sizeof(WORD);

    // reset current offset in buffer, etc.

    lpstNLI->dwBufferLeft = 0;
    lpstNLI->dwBufferOffset = 0;
    lpstNLI->dwSectionOffset = 0;

    return(ENGSTATUS_OK);
}

ENGSTATUS EngGetNextName
(
    HGENG                   hGEng,
    LPNAME_LOAD_INFO        lpstNLI,
    LPSTR                   lpszName
)
{
    DWORD                   dwBytesToRead, dwOff, dwBytesRead;
    DWORD                   dwNameLen;

    // time to refill our pool

    if (lpstNLI->dwBufferLeft == 0)
    {
        dwBytesToRead = (lpstNLI->dwDataLeft > NAME_BUFFER_SIZE) ?
                        NAME_BUFFER_SIZE :
                        lpstNLI->dwDataLeft;

        if (dwBytesToRead == 0)
        {
            *lpszName = 0;

            return(ENGSTATUS_OK);
        }

        // seek to the next offset to read data

        if (hGEng->lpDataFileCallBacks->
                lpFileCallBacks->FileSeek(lpstNLI->lpvFileInfo,
                                          lpstNLI->dwSectionStart +
                                           lpstNLI->dwSectionOffset,
                                          SEEK_SET,
                                          &dwOff) == CBSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        // read in next chunk

        if (hGEng->lpDataFileCallBacks->
                lpFileCallBacks->FileRead(lpstNLI->lpvFileInfo,
                                          lpstNLI->lpbyNameBuffer,
                                          dwBytesToRead,
                                          &dwBytesRead) == CBSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        lpstNLI->dwBufferLeft = dwBytesToRead;
        lpstNLI->dwBufferOffset = 0;
    }

    // now get that name

    dwNameLen = lpstNLI->lpbyNameBuffer[lpstNLI->dwBufferOffset];

    // copy the name from the pool

    strncpy(lpszName,
            (LPSTR)lpstNLI->lpbyNameBuffer+lpstNLI->dwBufferOffset+1,
            dwNameLen > N30_SIZE_VIRUSNAME ? N30_SIZE_VIRUSNAME : dwNameLen);
    if (dwNameLen < N30_SIZE_VIRUSNAME)
        lpszName[dwNameLen] = 0;

    // reduce amount of name bytes left in the stream

    lpstNLI->dwDataLeft -= (dwNameLen + 1);
    lpstNLI->dwBufferLeft -= (dwNameLen + 1);
    lpstNLI->dwSectionOffset += (dwNameLen + 1);
    lpstNLI->dwBufferOffset += (dwNameLen + 1);

    // now see if we have any data left or need to re-fill the pool

    if (lpstNLI->dwBufferLeft == 0)
        return(ENGSTATUS_OK);

    if (lpstNLI->lpbyNameBuffer[lpstNLI->dwBufferOffset] >=
        lpstNLI->dwBufferLeft)
    {
        lpstNLI->dwBufferLeft = 0;
    }

    // fall through

    return(ENGSTATUS_OK);
}

ENGSTATUS EngCloseNameLoad
(
    HGENG                   hGEng,
    LPNAME_LOAD_INFO        lpstNLI
)
{
    ENGSTATUS               engStatus = ENGSTATUS_OK;

    if (lpstNLI->bNoFileHandle == FALSE &&
        EngCloseDataFile(hGEng->lpDataFileCallBacks,
                        lpstNLI->lpvFileInfo) == DFSTATUS_ERROR)
        engStatus = ENGSTATUS_ERROR;

    if (hGEng->lpGeneralCallBacks->MemoryFree(lpstNLI->lpbyNameBuffer) !=
        CBSTATUS_OK)
        engStatus = ENGSTATUS_ERROR;

    return(engStatus);
}


