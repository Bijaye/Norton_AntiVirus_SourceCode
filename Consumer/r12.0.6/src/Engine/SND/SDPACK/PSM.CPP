// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SDPACK/VCS/psm.cpv   1.1   07 Apr 1998 12:56:14   DCHI  $
//
// Description:
//  Contains code to generate presubmission info for a file
//
//************************************************************************
// $Log:   S:/SDPACK/VCS/psm.cpv  $
// 
//    Rev 1.1   07 Apr 1998 12:56:14   DCHI
// Changes to use filthash.* and addition of remaining callbacks returning fail.
// 
//    Rev 1.0   13 Mar 1998 12:51:42   DCHI
// Initial revision.
// 
//************************************************************************

#include <string.h>

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "winconst.h"
#include "navex.h"

#include "psm.h"

#include "filthash.h"

//////////////////////////////////////////////////////////////////////
// Callback implementation
//////////////////////////////////////////////////////////////////////

HFILE WINAPI FAR FileOpen(LPTSTR lpName, UINT uOpenMode)
{
    (void)lpName;
    (void)uOpenMode;

    return((HFILE)-1);
}

HFILE WINAPI FAR FileClose(HFILE hHandle)
{
    (void)hHandle;

    return((HFILE)-1);
}

UINT WINAPI FileRead
(
    HFILE       hHandle,
    LPVOID      lpBuffer,
    UINT        uBytes
)
{
    DWORD       dwBytesRead;

    if (ReadFile(HANDLE(hHandle),
                 lpBuffer,
                 uBytes,
                 &dwBytesRead,
                 NULL) == FALSE)
        return((UINT)-1);

    return((UINT)dwBytesRead);
}

UINT WINAPI FAR FileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    (void)hHandle;
    (void)lpBuffer;
    (void)uBytes;

    return((UINT)-1);
}

DWORD WINAPI FileSeek
(
    HFILE       hHandle,
    LONG        dwOffset,
    int         nFlag
)
{
    DWORD       dwMoveMethod;

    switch (nFlag)
    {
        case 0:
            dwMoveMethod = FILE_BEGIN;
            break;
        case 1:
            dwMoveMethod = FILE_CURRENT;
            break;
        case 2:
            dwMoveMethod = FILE_END;
            break;
        default:
            return((DWORD)-1);
    }

    dwOffset = SetFilePointer((HANDLE)hHandle,
                              dwOffset,
                              NULL,
                              dwMoveMethod);

    return(dwOffset);
}

DWORD WINAPI FileSize(HFILE hHandle)
{
    DWORD       dwSize;

    dwSize = GetFileSize((HANDLE)hHandle,NULL);
    return(dwSize);
}

BOOL WINAPI FAR FileGetDateTime(HFILE hHandle, UINT uType, WORD FAR *lpuDate, WORD FAR *lpuTime)
{
    (void)hHandle;
    (void)uType;
    (void)lpuDate;
    (void)lpuTime;

    return(FALSE);
}

BOOL WINAPI FAR FileSetDateTime(HFILE hHandle, UINT uType, WORD uDate, WORD uTime)
{
    (void)hHandle;
    (void)uType;
    (void)uDate;
    (void)uTime;

    return(FALSE);
}

BOOL WINAPI FAR FileDelete(LPTSTR lpFilename)
{
    (void)lpFilename;

    return(FALSE);
}

UINT WINAPI FAR FileGetAttr(LPTSTR lpFilename, UINT FAR *lpuAttr)
{
    (void)lpFilename;
    (void)lpuAttr;

    return((UINT)-1);
}

UINT WINAPI FAR FileSetAttr(LPTSTR lpFilename, UINT uAttr)
{
    (void)lpFilename;
    (void)uAttr;

    return((UINT)-1);
}

HFILE WINAPI FAR FileCreate(LPTSTR lpFilename, UINT uAttr)
{
    (void)lpFilename;
    (void)uAttr;

    return((HFILE)-1);
}

BOOL WINAPI FAR FileLock(HFILE hHandle, DWORD dwStart, DWORD dwLength)
{
    (void)hHandle;
    (void)dwStart;
    (void)dwLength;

    return(FALSE);
}

BOOL WINAPI FAR FileUnlock(HFILE hHandle, DWORD dwStart, DWORD dwLength)
{
    (void)hHandle;
    (void)dwStart;
    (void)dwLength;

    return(FALSE);
}

BOOL WINAPI FAR Progress(WORD wProgress, DWORD dwCookie)
{
    (void)wProgress;
    (void)dwCookie;

    return(FALSE);
}

UINT WINAPI FAR SDiskReadPhysical(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive)
{
    (void)lpBuffer;
    (void)bCount;
    (void)wCylinder;
    (void)bSector;
    (void)bHead;
    (void)bDrive;

    return((UINT)-1);
}

UINT WINAPI FAR SDiskWritePhysical(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive)
{
    (void)lpBuffer;
    (void)bCount;
    (void)wCylinder;
    (void)bSector;
    (void)bHead;
    (void)bDrive;

    return((UINT)-1);
}

LPVOID WINAPI FAR TempMemoryAlloc(DWORD uSize)
{
    (void)uSize;

    return(NULL);
}

VOID WINAPI FAR TempMemoryFree(LPVOID lpBuffer)
{
    (void)lpBuffer;
}

LPVOID WINAPI FAR PermMemoryAlloc(DWORD uSize)
{
    (void)uSize;

    return(NULL);
}

VOID WINAPI FAR PermMemoryFree(LPVOID lpBuffer)
{
    (void)lpBuffer;
}

CALLBACKREV1 gstCallBacks =
{
    FileOpen,
    FileClose,
    FileRead,
    FileWrite,
    FileSeek,
    FileSize,
    FileGetDateTime,
    FileSetDateTime,
    FileDelete,
    FileGetAttr,
    FileSetAttr,
    FileCreate,

    FileLock,
    FileUnlock,
    Progress,

    SDiskReadPhysical,
    SDiskWritePhysical,

    TempMemoryAlloc,
    TempMemoryFree,
    PermMemoryAlloc,
    PermMemoryFree
};

//********************************************************************
//
// Function:
//  PreSubmitDataFill
//
// Parameters:
//  lpszFileName        Name of file to calculate presubmission data
//  lpbySubmitBuffer    Ptr to buffer for presubmission data
//  dwSubmitBufferSize  Size of presubmission buffer
//
// Description:
//  Fills the given buffer with the presubmission data.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL PreSubmitDataFill
(
    LPTSTR              lpszFileName,
    LPBYTE              lpbySubmitBuffer,
    DWORD               dwSubmitBufferSize
)
{
    BYTE                abyFileHdr[64];
    BYTE                byWorkBuf[2048];
    FILTER_DATA_T       stData;
    HANDLE              hFile;
    DWORD               dwBytesRead;

    // Validate parameters

    if (lpszFileName == NULL ||
        lpbySubmitBuffer == NULL ||
        dwSubmitBufferSize < sizeof(FILTER_DATA_T))
        return(FALSE);

    // Open the file

    hFile = CreateFile(lpszFileName,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return(FALSE);

    // Read the first 64 bytes

    if (ReadFile(hFile,
                 abyFileHdr,
                 64,
                 &dwBytesRead,
                 NULL) == FALSE)
    {
        CloseHandle(hFile);
        return(FALSE);
    }

    // Get the presubmission information

    if (ObtainInformation(&gstCallBacks,
                          (HFILE)hFile,
                          BIT_FILL_DOS_HEADER |
                           BIT_FILL_WIN_HEADER |
                           BIT_FILL_FILE_SIZE |
                           BIT_FILL_FILE_CRC,
                          abyFileHdr,
                          byWorkBuf,
                          &stData) == FALSE)
    {
        CloseHandle(hFile);
        return(FALSE);
    }

    // Close the file

    CloseHandle(hFile);

    // Copy data to buffer

    memcpy(lpbySubmitBuffer,&stData,sizeof(FILTER_DATA_T));
    return(TRUE);
}

