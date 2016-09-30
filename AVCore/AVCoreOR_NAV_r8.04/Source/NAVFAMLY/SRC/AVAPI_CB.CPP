// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVFAMLY/VCS/avapi_cb.cpv   1.0   28 Apr 1997 13:44:02   MKEATIN  $
//
// Description:
//  NAV callback implementation for AVAPI 1.0.
//
// Contains:
//  gstNAVFileCallBacks;
//  gstNAVGeneralCallBacks;
//  gstNAVProgressCallBacks;
//  gstNAVDataFileCallBacks;
//  gstNAVMemoryCallBacks;
//  gstNAVLinearCallBacks;
//  gstNAVEXCallBacks;
//
// See Also:
//
//************************************************************************
// $Log:   S:/NAVFAMLY/VCS/avapi_cb.cpv  $
// 
//    Rev 1.0   28 Apr 1997 13:44:02   MKEATIN
// Initial revision.
//************************************************************************

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "file.h"
#include "disk.h"
#include "xapi.h"
#include "symsync.h"

#include "pamapi.h"		
#include "avenge.h"
#include "callback.h"
#include "navcb.h"

char szNAVPath [SYM_MAX_PATH+1];        // szNavPath is initialized in
                                        // VLRegisterVirusDB() in avapi.cpp
CBSTATUS NAVFileOpen
(
    LPVOID      lpvFileInfo,
    WORD        wOpenMode
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    HFILE hFile;

    hFile = FileOpen(lpFileInfo->lpstrPathName,wOpenMode);
    if (hFile == HFILE_ERROR)
        return(CBSTATUS_ERROR);

    lpFileInfo->hFile = hFile;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileClose
(
    LPVOID      lpvFileInfo
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    if (FileClose(lpFileInfo->hFile) == HFILE_ERROR)
        return(CBSTATUS_ERROR);

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileSeek
(
    LPVOID      lpvFileInfo,
    long        lOffset,
    WORD        wWhence,
    LPDWORD     lpdwNewOffset
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    *lpdwNewOffset = FileSeek(lpFileInfo->hFile,lOffset,wWhence);

    if (*lpdwNewOffset == -1)
        return(CBSTATUS_ERROR);

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileRead
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    // Only attempt read if bytes to read is non-zero

    if (dwNumBytes != 0)
    {
        UINT    uiBytesRead = FileRead(lpFileInfo->hFile,
                                       lpvBuffer,
                                       (WORD)dwNumBytes);

        if (uiBytesRead == -1)
            return(CBSTATUS_ERROR);

        *lpdwNumActualBytes = uiBytesRead;
    }
    else
        *lpdwNumActualBytes = 0;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileWrite
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    ;

    // Only attempt write if bytes to write is non-zero

    if (dwNumBytes != 0)
    {
        UINT    uiBytesRead = FileWrite(lpFileInfo->hFile,
                                        lpvBuffer,
                                        (WORD)dwNumBytes);

        if (uiBytesRead == -1)
            return(CBSTATUS_ERROR);

        *lpdwNumActualBytes = uiBytesRead;
    }
    else
        *lpdwNumActualBytes = 0;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileTell
(
    LPVOID      lpvFileInfo,
    LPDWORD     lpdwOffset
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    DWORD       dwOffset;

    dwOffset = FileSeek(lpFileInfo->hFile,0,SEEK_CUR);

    if (dwOffset == -1)
        return(CBSTATUS_ERROR);

    *lpdwOffset = dwOffset;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileSetEOF
(
    LPVOID      lpvFileInfo
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    // Truncate

    if (FileSeek(lpFileInfo->hFile,0,SEEK_END) == -1)
        return(CBSTATUS_ERROR);
    
    return(CBSTATUS_OK);
}

CBSTATUS NAVFileEOF
(
    LPVOID lpvFileInfo,
    LPBOOL lpbEOF
)
{
//    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
//    int iStatus;
//
//    iStatus = _eof(lpFileInfo->hFile);
//
//    if (iStatus == -1)
//        return(CBSTATUS_ERROR);
//
//    *lpbEOF = iStatus;

//    return(CBSTATUS_OK);      

      return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileGetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPDWORD lpdwInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileSetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
	DWORD   dwInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileGetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
    return(CBSTATUS_OK);
}

CBSTATUS NAVFileSetDateTime
(
	LPVOID  lpvFileInfo,
	WORD    wType,
    WORD    wDate,
    WORD    wTime
)
{
    return(CBSTATUS_OK);
}

CBSTATUS NAVFileFullNameAndPath
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFileName;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileName
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFileName;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFilePath
(
	LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFilePath,
	WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrFilePath;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileLogicalDrive
(
	LPVOID  lpvFileInfo,
    LPWSTR  lpwstrDriveLetter,
	WORD    wBufferLen
)
{
	(void)lpvFileInfo;
    (void)lpwstrDriveLetter;
	(void)wBufferLen;

    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileLength
(
    LPVOID  lpvFileInfo,
    LPDWORD lpdwFileLength
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    DWORD       dwLength;

    dwLength = FileLength(lpFileInfo->hFile);

    if (dwLength == (DWORD)-1)
        return(CBSTATUS_ERROR);

    *lpdwFileLength = dwLength;

    return(CBSTATUS_OK);
}

CBSTATUS NAVFileDelete
(
	LPVOID  lpvFileInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileCopy
(
    LPVOID  lpvSrcFileInfo,
    LPVOID  lpvDstFileInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileRename
(
    LPVOID  lpvOldFileInfo,
    LPWSTR  lpwstrNewName
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileExists
(
    LPVOID  lpvFileInfo,
    LPBOOL	lpbExists
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVFileGetErrorCondition
(
    LPVOID  lpvFileInfo,
    LPDWORD lpdwErrorCondition
)
{
    (void)lpvFileInfo;

    *lpdwErrorCondition = 0;

    return(CBSTATUS_OK);
}


CBSTATUS NAVMemoryAlloc
(
    DWORD        dwSize,
    WORD         wMemType,
    LPLPVOID     lplpvBuffer
)
{
    LPVOID	lpvData;

    (void)wMemType;

    lpvData = MemAllocPtr(GHND, dwSize);

    if (lpvData == NULL)
    	return(CBSTATUS_ERROR);

    *lplpvBuffer = lpvData;

    return(CBSTATUS_OK);
}

CBSTATUS NAVMemoryFree
(
    LPVOID lpvBuffer
)
{
    MemFreePtr (lpvBuffer);

    return(CBSTATUS_OK);
}

CBSTATUS NAVGetDataFileInfo
(
    LPLPVOID        lplpvFileInfo,
    WORD            wDataFileNum
)
{
    LPFILEINFORMATION   lpFileInfo;

    lpFileInfo = (LPFILEINFORMATION)MemAllocPtr(GHND,sizeof(FILEINFO_T));
    if (lpFileInfo == NULL)
        return(CBSTATUS_ERROR);

    lpFileInfo->hFile = NULL;
    STRCPY(lpFileInfo->lpstrPathName,szNAVPath);

    switch (wDataFileNum)
    {
        case DATA_FILE_ID_VIRSCAN1DAT:
            NameAppendFile(lpFileInfo->lpstrPathName,"VIRSCAN1.DAT");
            break;
        case DATA_FILE_ID_VIRSCAN2DAT:
            NameAppendFile(lpFileInfo->lpstrPathName,"VIRSCAN2.DAT");
            break;
        case DATA_FILE_ID_VIRSCAN3DAT:
            NameAppendFile(lpFileInfo->lpstrPathName,"VIRSCAN3.DAT");
            break;
        case DATA_FILE_ID_VIRSCAN4DAT:
            NameAppendFile(lpFileInfo->lpstrPathName,"VIRSCAN4.DAT");
            break;
        default:
            return(CBSTATUS_ERROR);
    }

    *lplpvFileInfo = (LPVOID)lpFileInfo;
    return(CBSTATUS_OK);
}


CBSTATUS NAVCloseDataFileInfo
(
    LPVOID lpvFileInfo
)
{
    MemFreePtr(lpvFileInfo);

    return(CBSTATUS_OK);
}

CBSTATUS NAVMemGetPtrToMem
(
    LPVOID       lpvInfo,
    DWORD        dwMemAddress,
    LPBYTE       lpbyMemBuf,
    DWORD        dwMemSize,
    LPLPBYTE     lplpbyMemPtr,
    LPCOOKIETRAY lpCookieTray
)
{
    WORD        wSeg;
    WORD        wOff;
    DWORD       dwSegOffPtr;

    wSeg = (WORD)(dwMemAddress >> 4);
    wOff = (WORD)(dwMemAddress & 0xF);

    dwSegOffPtr = ((((DWORD)wSeg) << 16) | (DWORD)(wOff));

#if defined(SYM_DOSX) || defined(SYM_WIN32) || defined(SYM_VXD)

    *lplpbyMemPtr = (LPBYTE)GetProtModePtr((LPVOID)dwSegOffPtr);

    return(CBSTATUS_OK);

#else

    return(CBSTATUS_ERROR);

#endif
}

CBSTATUS NAVMemReleasePtrToMem
(
    LPVOID       lpvInfo,
    LPBYTE       lpbyMemPtr,
    LPCOOKIETRAY lpCookieTray
)
{

#if defined(SYM_DOSX) 

    SelectorFree(FP_SEG(lpbyMemPtr));

#endif

    return(CBSTATUS_OK);
}   

//********************************************************************
//
// Function:
//  NAVProgressInit()
//
// Parameters:
//  lptstrObjectName        String containing the object name being worked on
//  dwLevel                 What level of processing is this object?
//  dwProgressValueMax      What's our MAX progress value? (100%, etc?)
//  lpCookieTray            Pointer to a structure of our global, local, and
//                          transient cookies.
//  lplpvData               Set to point to the application's progress data
//
// Description:
//  This function is called to initialize a progress context.  This progress
//  context is used during scanning or any other task that takes noticable
//  time.  The progress context is passed along through the engine and back
//  to the ProgressUpdate and ProgressClose functions below to update the
//  application (and the user) as to the progress of the engine.
//
// Returns:
//  CBSTATUS_OK         On successful progress context creation
//  CBSTATUS_ERROR      If unable to create a progress context
//
//********************************************************************

CBSTATUS NAVProgressInit
(
    LPWSTR       lpwstrObjectName,
    DWORD        dwLevel,
    DWORD        dwProgressValueMax,
    LPCOOKIETRAY lpCookieTray,
    LPLPVOID     lplpvData
)
{
    return(CBSTATUS_OK);
}


//********************************************************************
//
// Function:
//  NAVProgressUpdate()
//
// Parameters:
//  lpvData             Point to the application's progress data
//  dwProgressValue     Value between 0 and dwProgressValueMax
//
// Description:
//  This function is called by the engine to update the application on the
//  progress of a given task.  The dwProgressValue is used to indicate how
//  much longer the engine must work before it has completed its task.
//
// Returns:
//  CBSTATUS_OK         If the engine should continue on its merry way
//  CBSTATUS_ABORT      If the application/user wants to abort the current
//                      engine taks.
//  CBSTATUS_ERROR      If there is an error (engine should close immediately)
//
//********************************************************************

CBSTATUS NAVProgressUpdate
(
    LPVOID       lpvData,
    DWORD        dwProgressValue
)
{
    return(CBSTATUS_OK);
}

//********************************************************************
//
// Function:
//  NAVProgressClose()
//
// Parameters:
//  lpvData             Point to the application's progress data
//
// Description:
//  This function is called by the engine once a task is completed. It frees
//  all context information and returns.
//
// Returns:
//  CBSTATUS_OK         If the engine should continue on its merry way
//  CBSTATUS_ERROR      If there is an error (engine should close immediately)
//
//********************************************************************


CBSTATUS NAVProgressClose
(
    LPVOID       lpvData
)
{
    return(CBSTATUS_OK);
}


CBSTATUS NAVLinearOpen
(
    LPVOID  lpvInfo,                // [in] Linear object info
    WORD    wOpenMode               // [in] filler
)
{
    return(CBSTATUS_OK);
}


CBSTATUS NAVLinearClose
(
    LPVOID  lpvInfo                 // [in] Linear object info
)
{
    return(CBSTATUS_OK);
}


CBSTATUS NAVLinearRead
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin reading
    DWORD   dwSectorHigh,           // [in] Sector's (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to read into
    DWORD   dwLength,               // [in] Sectors to read
    LPDWORD lpdwActualLength        // [out] Sectors read
)
{
    BYTE bySide;
    WORD wCylinder;
    BYTE bySector;

    UINT i, uResult;
    ABSDISKREC absDisk;

    if (NAVLinearConvertToPhysical (lpvInfo,
                                    dwSectorLow,
                                    &bySide,
                                    &wCylinder,
                                    &bySector ) == CBSTATUS_ERROR)
    {
        return(CBSTATUS_ERROR);
    }

    for ( i = 0; i < 2; i++)            // two attempts
    {
        MEMSET(&absDisk, 0, sizeof(absDisk));
        absDisk.dn = (BYTE)lpvInfo;

        uResult = DiskGetPhysicalInfo (&absDisk);

        if (uResult)                    // Failed - try again
        {
            continue;
        }

        absDisk.head       = bySide;
        absDisk.track      = wCylinder;
        absDisk.sector     = bySector;
        absDisk.numSectors = (BYTE)dwLength;
        absDisk.buffer     = (HPBYTE) lpbyBuffer;

        uResult = DiskAbsOperation (READ_COMMAND, &absDisk);

        if (!uResult)                   // Success
        {
            *lpdwActualLength = absDisk.numSectors;
            break;
        }
    }
 
    return (!uResult ? CBSTATUS_OK : CBSTATUS_ERROR);
}

CBSTATUS NAVLinearWrite
(
    LPVOID  lpvInfo,                // [in] Linear object info
    DWORD   dwSectorLow,            // [in] Sector to begin writing
    DWORD   dwSectorHigh,           // [in] Sector (high dword)
    LPBYTE  lpbyBuffer,             // [out] Buffer to write from
    DWORD   dwLength,               // [in] Sectors to write
    LPDWORD lpdwActualLength        // [in/out] Sectors written
)
{
    BYTE bySide;
    WORD wCylinder;
    BYTE bySector;

    UINT i, uResult;
    ABSDISKREC absDisk;

    if (NAVLinearConvertToPhysical (lpvInfo,
                                    dwSectorLow,
                                    &bySide,
                                    &wCylinder,
                                    &bySector ) == CBSTATUS_ERROR)
    {
        return(CBSTATUS_ERROR);
    }

    for ( i = 0; i < 2; i++)            // two attempts
    {
        MEMSET(&absDisk, 0, sizeof(absDisk));
        absDisk.dn = (BYTE)lpvInfo;

        uResult = DiskGetPhysicalInfo (&absDisk);

        if (uResult)                    // Failed - try again
        {
            continue;
        }

        absDisk.head       = bySide;
        absDisk.track      = wCylinder;
        absDisk.sector     = bySector;
        absDisk.numSectors = (BYTE)dwLength;
        absDisk.buffer     = (HPBYTE)lpbyBuffer;

        uResult = DiskAbsOperation (WRITE_COMMAND, &absDisk);

        if (!uResult)                   // Success
        {
            *lpdwActualLength = absDisk.numSectors;
            break;
        }
    }
 
    return (!uResult ? CBSTATUS_OK : CBSTATUS_ERROR);
}

CBSTATUS NAVLinearGetDimensions
(
    LPVOID  lpvInfo,                // [in] Linear object info
    LPBYTE  lpbySides,              // [out] Total Sides
    LPWORD  lpwCylinders,           // [out] Total Cylinders
    LPBYTE  lpbySectors             // [out] Sectors per cylinder
)
{
    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = (BYTE)lpvInfo;

    if (DiskGetPhysicalInfo (&stDiskRec))
    {
        return(CBSTATUS_ERROR);
    }

    *lpbySides    = stDiskRec.totalHeads;
    *lpwCylinders = stDiskRec.totalTracks;
    *lpbySectors  = stDiskRec.sectorsPerTrack; 

    return (CBSTATUS_OK);
}

CBSTATUS NAVLinearConvertFromPhysical
(
    LPVOID      lpvInfo,            // [in] Linear object info
    BYTE        bySide,             // [in] Physical side, ...
    WORD        wCylinder,          // [in] cylinder, and ...
    BYTE        bySector,           // [in] sector to translate
    LPDWORD     lpdwSectorLow       // [out] Translated sector low dword
)
{
    BYTE bySides;
    WORD wCylinders;
    BYTE bySectors;

    CBSTATUS cbResult;

    cbResult = NAVLinearGetDimensions (lpvInfo, 
                                       &bySides, 
                                       &wCylinders, 
                                       &bySectors);

    if (CBSTATUS_OK == cbResult)
    {
        *lpdwSectorLow = ((DWORD) wCylinder * bySectors * bySides ) +
                         ((DWORD) bySide * bySectors ) +
                         ((DWORD) bySector - 1 );
    }

    return(cbResult);
}

CBSTATUS NAVLinearConvertToPhysical
(
    LPVOID      lpvInfo,              // [in] Linear object info
    DWORD       dwSectorLow,          // [in] Linear sector low dword
    LPBYTE      lpbySide,             // [out] Translated side, ...
    LPWORD      lpwCylinder,          // [out] cylinder, and ...
    LPBYTE      lpbySector            // [out] sector
)
{
    BYTE bySides;
    WORD wCylinders;
    BYTE bySectors;

    CBSTATUS cbResult;

    cbResult = NAVLinearGetDimensions ( lpvInfo, &bySides, &wCylinders, &bySectors );

    if (CBSTATUS_OK == cbResult)
    {
        *lpwCylinder = (WORD) (dwSectorLow / ( (WORD) bySectors * (WORD) bySides ));
        dwSectorLow -= (DWORD) *lpwCylinder * ( (WORD) bySectors * (WORD) bySides );
        *lpbySide    = (BYTE) (dwSectorLow / bySectors);
        *lpbySector  = (BYTE) (dwSectorLow % bySectors) + 1;
    }

    return(cbResult);
}

CBSTATUS NAVLinearIsPartitionedMedia ( LPVOID lpvInfo, LPBOOL lpbPartitioned )
{
    *lpbPartitioned = (BYTE)lpvInfo < 0x80 ? FALSE : TRUE;

    return(CBSTATUS_OK);
}


// Mutex

typedef struct tagMUTEX_INFO
{
    DWORD           dwReserved;

#if defined(SYM_WIN32)
    HANDLE          hMutex;
#elif defined(SYM_VXD)
    SYMMUTEX        stMutex;
#endif

} MUTEX_INFO_T, FAR *LPMUTEX_INFO;


CBSTATUS NAVMutexCreate
(
    LPLPVOID        lplpvMutexInfo
)
{
    LPMUTEX_INFO    lpstMutexInfo;

    lpstMutexInfo = (LPMUTEX_INFO)MemAllocPtr(GHND,sizeof(MUTEX_INFO_T));
    if (lpstMutexInfo == NULL)
        return(CBSTATUS_OK);

#if defined(SYM_WIN32)

    lpstMutexInfo->hMutex = CreateMutex(NULL,   // Default security attributes
                                        FALSE,  // No initial owner
                                        NULL);  // Don't care about its name

    if (lpstMutexInfo->hMutex == NULL)
    {
        MemFreePtr(lpstMutexInfo);
        return(CBSTATUS_ERROR);
    }

#elif defined(SYM_VXD)

    if (!SymCreateMutex(&lpstMutexInfo->stMutex))
    {
        MemFreePtr(lpstMutexInfo);
        return(CBSTATUS_ERROR);
    }

#endif

    *lplpvMutexInfo = (LPVOID)lpstMutexInfo;

    return(CBSTATUS_OK);
}

CBSTATUS NAVMutexDestroy
(
    LPVOID          lpvMutexInfo
)
{
    CBSTATUS        cbStatus = CBSTATUS_OK;

#if defined(SYM_WIN32)

    if (CloseHandle(((LPMUTEX_INFO)lpvMutexInfo)->hMutex) == FALSE)
    {
        // Failed to close mutex

        cbStatus = CBSTATUS_ERROR;
    }

#elif defined(SYM_VXD)

    SymDestroyMutex (&(((LPMUTEX_INFO)lpvMutexInfo)->stMutex));

#endif
   

    MemFreePtr(lpvMutexInfo);
    return(CBSTATUS_OK);
}

CBSTATUS NAVMutexWait
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
#if defined(SYM_WIN32)

    if (WaitForSingleObject(((LPMUTEX_INFO)lpvMutexInfo)->hMutex,
                            dwTimeoutMS) != WAIT_OBJECT_0)
    {
        // Failure getting mutex

        return(CBSTATUS_ERROR);
    }

#elif defined(SYM_VXD)

    SymWaitForMutex (&(((LPMUTEX_INFO)lpvMutexInfo)->stMutex));

#endif

    return(CBSTATUS_OK);
}

CBSTATUS NAVMutexRelease
(
    LPVOID          lpvMutexInfo
)
{
#if defined(SYM_WIN32)

    if (ReleaseMutex(((LPMUTEX_INFO)lpvMutexInfo)->hMutex) == FALSE)
    {
        // Failure releasing mutex

        return(CBSTATUS_ERROR);
    }

#elif defined(SYM_VXD)

    SymReleaseMutex (&(((LPMUTEX_INFO)lpvMutexInfo)->stMutex));

#endif

    return(CBSTATUS_OK);
}

// Semaphore

CBSTATUS NAVSemaphoreCreate
(
    LPLPVOID        lplpvSemaphoreInfo,
    DWORD           dwInitialCount,
    DWORD           dwMaximumCount
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVSemaphoreDestroy
(
    LPVOID          lpvSemaphoreInfo
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVSemaphoreWait
(
    LPVOID          lpvSemaphoreInfo,
    DWORD           dwTimeoutMS
)
{
    return(CBSTATUS_ERROR);
}

CBSTATUS NAVSemaphoreRelease
(
    LPVOID          lpvSemaphoreInfo,
    LPDWORD         lpdwPrevCount
)
{
    return(CBSTATUS_ERROR);
}

FILECALLBACKS_T gstNAVFileCallBacks =
{
    NAVFileOpen,
    NAVFileClose,
    NAVFileSeek,
    NAVFileRead,
    NAVFileWrite,
    NAVFileTell,
    NAVFileSetEOF,
    NAVFileEOF,
    NAVFileGetAttr,
    NAVFileSetAttr,
    NAVFileGetDateTime,
    NAVFileSetDateTime,
    NAVFileFullNameAndPath,
    NAVFileName,
    NAVFilePath,
    NAVFileLogicalDrive,
    NAVFileLength,
    NAVFileDelete,
    NAVFileRename,
    NAVFileGetErrorCondition,
};

GENERALCALLBACKS_T gstNAVGeneralCallBacks =
{
    NULL,
    &gstNAVFileCallBacks,
    NAVMemoryAlloc,
    NAVMemoryFree,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

PROGRESSCALLBACKS_T gstNAVProgressCallBacks =
{
    NAVProgressInit,
    NAVProgressUpdate,
    NAVProgressClose,
    NULL
};

DATAFILECALLBACKS_T gstNAVDataFileCallBacks =
{
    &gstNAVFileCallBacks,
    NAVGetDataFileInfo,
    NAVCloseDataFileInfo
};

IPCCALLBACKS_T gstNAVIPCCallBacks =
{
    NAVMutexCreate,
    NAVMutexDestroy,
    NAVMutexWait,
    NAVMutexRelease,
    NAVSemaphoreCreate,
    NAVSemaphoreDestroy,
    NAVSemaphoreWait,
    NAVSemaphoreRelease
};

MEMORYCALLBACKS_T gstNAVMemoryCallBacks =
{
    NULL,
    NULL,
    NAVMemGetPtrToMem,
    NAVMemReleasePtrToMem
};

LINEARCALLBACKS gstNAVLinearCallBacks =
{
    NAVLinearOpen,
    NAVLinearClose,
    NAVLinearRead,
    NAVLinearWrite,
    NAVLinearGetDimensions,
    NAVLinearConvertFromPhysical,
    NAVLinearConvertToPhysical,
    NAVLinearIsPartitionedMedia
};

HFILE WINAPI NAVEXFileOpen(LPSTR lpName, UINT uOpenMode)
{
    return (FileOpen(lpName, uOpenMode));
}

HFILE WINAPI NAVEXFileClose(HFILE hHandle)
{
    return (FileClose(hHandle));
}

UINT WINAPI NAVEXFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    return (FileRead(hHandle,lpBuffer,uBytes));
}

UINT WINAPI NAVEXFileWrite(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    if (uBytes != 0)
    {
        return (FileWrite(hHandle,lpBuffer,uBytes));
    }
    else
    {
        if (FileSeek(hHandle,0,SEEK_END) == -1)
            return((UINT)-1);

        return(0);
    }
}

BOOL WINAPI NAVEXFileDelete(LPSTR lpFilename)
{
    FileDelete(lpFilename);

    return(0);
}

UINT WINAPI NAVEXFileGetAttr(LPSTR lpFilename, UINT FAR *lpuAttr)
{
    return (FileGetAttr (lpFilename, lpuAttr));
}

UINT WINAPI NAVEXFileSetAttr(LPSTR lpFilename, UINT uAttr)
{
    return (FileSetAttr (lpFilename, uAttr));
}


DWORD WINAPI NAVEXFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    return (FileSeek(hHandle, dwOffset,nFlag));
}

DWORD WINAPI NAVEXFileLength(HFILE hHandle)
{
    return (FileLength(hHandle));
}

HFILE WINAPI NAVEXFileCreate(LPSTR lpFileName, UINT uAttr)
{
    return (FileCreate(lpFileName,uAttr));
}


CALLBACKREV1    gstNAVEXCallBacks =
    {
        NAVEXFileOpen,  // HFILE   (WINAPI FAR *FileOpen)(LPTSTR lpName, UINT uOpenMode);
        NAVEXFileClose, // HFILE   (WINAPI FAR *FileClose)(HFILE hHandle);
        NAVEXFileRead,  // UINT    (WINAPI FAR *FileRead)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
        NAVEXFileWrite, // UINT    (WINAPI FAR *FileWrite)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
        NAVEXFileSeek,  // DWORD   (WINAPI FAR *FileSeek)(HFILE hHandle, LONG dwOffset, int nFlag);
        NAVEXFileLength,// DWORD   (WINAPI FAR *FileSize)(HFILE hHandle);
        NULL,           // BOOL    (WINAPI FAR *FileGetDateTime)(HFILE hHandle, UINT uType, WORD FAR *lpuDate, WORD FAR *lpuTime);
        NULL,           // BOOL    (WINAPI FAR *FileSetDateTime)(HFILE hHandle, UINT uType, WORD uDate, WORD uTime);
        NAVEXFileDelete,// BOOL    (WINAPI FAR *FileDelete)(LPTSTR lpFilename);
        NAVEXFileGetAttr,// UINT    (WINAPI FAR *FileGetAttr)(LPTSTR lpFilename, UINT FAR *lpuAttr);
        NAVEXFileSetAttr,// UINT    (WINAPI FAR *FileSetAttr)(LPTSTR lpFilename, UINT uAttr);
        NULL,           // HFILE   (WINAPI FAR *FileCreate)(LPTSTR lpFilename, UINT uAttr);
        NULL,           // BOOL    (WINAPI FAR *FileLock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
        NULL,           // BOOL    (WINAPI FAR *FileUnlock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
        NULL,           // BOOL    (WINAPI FAR *Progress)(WORD wProgress, DWORD dwCookie);
        NULL,           // UINT    (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
        NULL,           // UINT    (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
        NULL,           // LPVOID  (WINAPI FAR *TempMemoryAlloc)(DWORD uSize);
        NULL,           // VOID    (WINAPI FAR *TempMemoryFree)(LPVOID lpBuffer);
        NULL,           // LPVOID  (WINAPI FAR *PermMemoryAlloc)(DWORD uSize);
        NULL            // VOID    (WINAPI FAR *PermMemoryFree)(LPVOID lpBuffer);
    };


