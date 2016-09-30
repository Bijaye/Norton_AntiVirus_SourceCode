// Copyright 1996-1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVSDK/VCS/engcbk.cpv   1.4   19 Mar 1998 11:42:58   DALLEE  $
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
// $Log:   S:/NAVSDK/VCS/engcbk.cpv  $
// 
//    Rev 1.4   19 Mar 1998 11:42:58   DALLEE
// Updated for wider DWORD-sized ABSDISKREC fields.
// 
//    Rev 1.3   18 Dec 1997 13:24:30   DALLEE
// Added memory progress callback.
// 
//    Rev 1.2   18 Dec 1997 02:08:34   DALLEE
// Klugery to combat buggy NAVENG. If the config callback is not implemented
// and there is no .INF engine config file, then heuristic scanning is
// disabled (the engine gets an empty extension list).
// Added engine config NAVSDK.INF (szNavexInf) path to per thread data.
// In callback, if this doesn't exist, manually set scan level to 3.
// 
//    Rev 1.1   04 Dec 1997 16:20:42   DALLEE
// When necessary, get per scan context info from thread local storage -
// this isn't available from the engine in cases where we need it.
// Config callback: set heuristic scan level based on our options.
// Data file callback: get location based on defs used by this instance.
// 
//    Rev 1.0   02 Dec 1997 14:30:20   DALLEE
// Initial revision.
//************************************************************************
// From AVCORE-CALLBACK CALLBACK.CPV r1.21
//
//    Rev 1.21   14 Nov 1997 12:32:20   DDREW
// Changed the file open mode for NAVEX callbacks to conform with Avenge
//************************************************************************

#include "platform.h"
#include "file.h"
#include "disk.h"
#include "xapi.h"
#include "symsync.h"
#include "symcfg.h"

#include "stdio.h"
#include "conio.h"

#include "avtypes.h"
#include "avcb.h"
#include "pamapi.h"
#include "avenge.h"
#include "callbk15.h"
//#include "callw32.h"

#define BUILDING_CALLBACK

#include "navcb.h"
#include "avapi.h"

#ifdef SYM_NLM
#include "navnlmex.h"
#include "symserve.h"
#endif


#if defined(SYM_WIN32)
  #include "winioctl.h"
#elif defined (SYM_VXD)
  #include "symevent.h"
#endif

#include "navsdk.h"
#include "oemapi.h"


#ifdef SYM_DOS
  #define DOS_LOADDS    LOADDS
#else
  #define DOS_LOADDS
#endif


//=======================================================================
//
//  Local Prototypes
//
//=======================================================================
CBSTATUS NAVLinearConvertToPhysical
(
    LPVOID      lpvInfo,                // [in] Linear object info
    DWORD       dwSectorLow,            // [in] Linear sector low dword
    LPBYTE      lpbySide,               // [out] Translated side, ...
    LPWORD      lpwCylinder,            // [out] cylinder, and ...
    LPBYTE      lpbySector              // [out] sector
);

BOOL WINAPI ScanLockUnlockDiskP
(
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
);
                                        // local defines
#define LOCK_DRIVE          0
#define UNLOCK_DRIVE        1

#define DISK_READ_WRITE     1
#define DISK_READ_ONLY      2

// NT Partition info helpers

#if defined(SYM_WIN32)

#if !defined( MAX_PARTITIONS )
    #define MAX_PARTITIONS       4
#endif

BYTE NAVEX_NTDeterminePartNum
(
    BYTE    byDrive,
    BYTE    byHead,
    WORD    wCylinder,
    BYTE    bySector,
    BYTE    bySectors,
    LPDWORD lpdwLinearOffset
);

BYTE NAVEX_NTDetermineDriveLetter
(
    BYTE    byDrive,
    BYTE    byPartNum
);

BYTE NAVEX_NTDiskWriteLogical
(
    BYTE    byDriveLetter,
    DWORD   dwLinearOffset,
    LPVOID  lpBuffer,
    LPDWORD lpdwSectors
);

#endif // SYM_WIN32

//=======================================================================
//
//  Callbacks
//
//=======================================================================

CBSTATUS NAVFileOpen
(
    LPVOID      lpvFileInfo,
    WORD        wOpenMode
)
{
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    HFILE hFile;

#ifndef SYM_NLM
    hFile = FileOpen(lpFileInfo->lpstrPathName,wOpenMode);
#else
    hFile = NV_FileOpen(lpFileInfo->lpstrPathName,wOpenMode);
#endif
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
    HFILE hFile;
    LPFILEINFORMATION  lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;

    hFile = FileClose(lpFileInfo->hFile);
    if (hFile == HFILE_ERROR)
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

#if defined(SYM_WIN32)                  // truncate the file

    if (!SetEndOfFile((HANDLE) lpFileInfo->hFile))
    {
        return(CBSTATUS_ERROR);
    }
                                        // writing 0 bytes will truncate
#else                                   // the file

    if (FileWrite(lpFileInfo->hFile, "", 0) == HFILE_ERROR)
    {
        return(CBSTATUS_ERROR);
    }

#endif

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
    LPBOOL  lpbExists
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
    LPVOID  lpvData;

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

#ifdef SYM_DOS                          // MAJOR Kludge to support wild set for DOS
    extern "C" BOOL PASCAL AreDefsWild(VOID);
#endif

CBSTATUS NAVGetDataFileInfo
(
    LPLPVOID        lplpvFileInfo,
    WORD            wDataFileNum
)
{
    LPOEMCONTEXT        lpOEMContext;
    LPFILEINFORMATION   lpFileInfo;
    BOOL                bWildDefs = FALSE;

#ifdef SYM_DOS
    bWildDefs = AreDefsWild();
#endif

    // Get per scan context information.

    lpOEMContext = GetScanContext();

    if ( ( NULL == lpOEMContext ) ||
         ( OEM_SIGNATURE != lpOEMContext->dwOEMSignature ) )
        return ( CBSTATUS_ERROR );

    lpFileInfo = (LPFILEINFORMATION)MemAllocPtr(GHND,sizeof(FILEINFO_T));
    if (lpFileInfo == NULL)
        return(CBSTATUS_ERROR);

    lpFileInfo->hFile = NULL;

    STRCPY( lpFileInfo->lpstrPathName, lpOEMContext->szVirusDefDir );

    switch (wDataFileNum)
    {
        case DATA_FILE_ID_VIRSCAN1DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD1.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN1.DAT"));
            break;
        case DATA_FILE_ID_VIRSCAN2DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD2.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN2.DAT"));
            break;
        case DATA_FILE_ID_VIRSCAN3DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD3.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN3.DAT"));
            break;
        case DATA_FILE_ID_VIRSCAN4DAT:
            if (bWildDefs)
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRWILD4.DAT"));
            else
                NameAppendFile(lpFileInfo->lpstrPathName,_T("VIRSCAN4.DAT"));
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

    return( CBSTATUS_OK );
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
    LPOEMCONTEXT        lpOEMContext;

    // Get per scan context information.

    lpOEMContext = GetScanContext();

    if ( ( NULL == lpOEMContext ) ||
         ( OEM_SIGNATURE != lpOEMContext->dwOEMSignature ) )
    {
        return ( CBSTATUS_OK );
    }

    // Only relay progress messages while we're scanning memory
    // and the user has provided a callback.

    if ( ( FALSE == lpOEMContext->bScanningMem ) ||
         ( NULL  == lpOEMContext->lpfMemProgress ) )
    {
        return ( CBSTATUS_OK );
    }

    // Save max progress and call user's progress function.

    lpOEMContext->dwMemProgressMax = dwProgressValueMax;
        
    lpOEMContext->lpfMemProgress( dwLevel, 
                                  lpOEMContext->dwMemProgressMax,
                                  lpOEMContext->lpvMemUserData );

    return( CBSTATUS_OK );
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
//                      engine task.
//  CBSTATUS_ERROR      If there is an error (engine should close immediately)
//
//********************************************************************

CBSTATUS NAVProgressUpdate
(
    LPVOID       lpvData,
    DWORD        dwProgressValue
)
{
    LPOEMCONTEXT        lpOEMContext;

    // Get per scan context information.

    lpOEMContext = GetScanContext();

    if ( ( NULL == lpOEMContext ) ||
         ( OEM_SIGNATURE != lpOEMContext->dwOEMSignature ) )
    {
        return ( CBSTATUS_OK );
    }

    // Only relay progress messages while we're scanning memory
    // and the user has provided a callback.

    if ( ( FALSE == lpOEMContext->bScanningMem ) ||
         ( NULL  == lpOEMContext->lpfMemProgress ) )
    {
        return ( CBSTATUS_OK );
    }

    // Call user's progress function.

    if ( FALSE == lpOEMContext->lpfMemProgress( dwProgressValue,
                                                lpOEMContext->dwMemProgressMax,
                                                lpOEMContext->lpvMemUserData ) )
    {
        return ( CBSTATUS_ABORT );
    }    
    else
    {
        return ( CBSTATUS_OK );
    }    
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


#ifndef SYM_NLM

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

    for (i = 0; i < 2; i++)             // two attempts
    {
        MEMSET(&absDisk, 0, sizeof(absDisk));
        absDisk.dn = (BYTE)lpvInfo;

        uResult = DiskGetPhysicalInfo (&absDisk);

        if (uResult)                    // Failed - try again
        {
            continue;
        }

        absDisk.dwHead     = bySide;
        absDisk.dwTrack    = wCylinder;
        absDisk.dwSector   = bySector;
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
    LPVOID  lpvInfo,                    // [in]  Linear object info
    DWORD   dwSectorLow,                // [in]  Sector to begin writing
    DWORD   dwSectorHigh,               // [in]  Sector (high dword)
    LPBYTE  lpbyBuffer,                 // [out] Buffer to write from
    DWORD   dwLength,                   // [in]  Sectors to write
    LPDWORD lpdwActualLength            // [in/out] Sectors written
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

    for (i = 0; i < 2; i++)             // two attempts
    {
        MEMSET(&absDisk, 0, sizeof(absDisk));
        absDisk.dn = (BYTE)lpvInfo;

        uResult = DiskGetPhysicalInfo (&absDisk);

        if (uResult)                    // Failed - try again
        {
            continue;
        }

        absDisk.dwHead     = bySide;
        absDisk.dwTrack    = wCylinder;
        absDisk.dwSector   = bySector;
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
    LPVOID  lpvInfo,                    // [in]  Linear object info
    LPBYTE  lpbySides,                  // [out] Total Sides
    LPWORD  lpwCylinders,               // [out] Total Cylinders
    LPBYTE  lpbySectors                 // [out] Sectors per cylinder
)
{
    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = (BYTE)lpvInfo;

    if (DiskGetPhysicalInfo (&stDiskRec))
    {
        return(CBSTATUS_ERROR);
    }

    *lpbySides    = (BYTE) stDiskRec.dwTotalHeads;
    *lpwCylinders = (WORD) stDiskRec.dwTotalTracks;
    *lpbySectors  = (BYTE) stDiskRec.dwSectorsPerTrack;

    return (CBSTATUS_OK);
}

CBSTATUS NAVLinearConvertFromPhysical
(
    LPVOID      lpvInfo,                // [in] Linear object info
    BYTE        bySide,                 // [in] Physical side, ...
    WORD        wCylinder,              // [in] cylinder, and ...
    BYTE        bySector,               // [in] sector to translate
    LPDWORD     lpdwSectorLow           // [out] Translated sector low dword
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
    LPVOID      lpvInfo,                // [in] Linear object info
    DWORD       dwSectorLow,            // [in] Linear sector low dword
    LPBYTE      lpbySide,               // [out] Translated side, ...
    LPWORD      lpwCylinder,            // [out] cylinder, and ...
    LPBYTE      lpbySector              // [out] sector
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

CBSTATUS NAVLinearIsPartitionedMedia
(
    LPVOID lpvInfo,
    LPBOOL lpbPartitioned
)
{
    *lpbPartitioned = (BYTE)lpvInfo < 0x80 ? FALSE : TRUE;

    return(CBSTATUS_OK);
}

#endif    // #ifndef SYM_NLM


// Mutex

typedef struct tagMUTEX_INFO
{
    DWORD           dwReserved;

#if defined(SYM_WIN32)

    HANDLE          hMutex;

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SYMMUTEX        stMutex;

#endif

} MUTEX_INFO_T, FAR *LPMUTEX_INFO;


CBSTATUS NAVMutexCreate
(
    LPLPVOID        lplpvMutexInfo
)
{
    LPMUTEX_INFO    lpstMutexInfo;

    lpstMutexInfo = (LPMUTEX_INFO)MemAllocPtr(GMEM_ZEROINIT | GMEM_FIXED,
                                              sizeof(MUTEX_INFO_T));
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

#elif defined(SYM_VXD) || defined(SYM_NTK)

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
    CBSTATUS        cbStatus;
    cbStatus = CBSTATUS_OK;

#if defined(SYM_WIN32)

    if (CloseHandle(((LPMUTEX_INFO)lpvMutexInfo)->hMutex) == FALSE)
    {
        // Failed to close mutex

        cbStatus = CBSTATUS_ERROR;
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

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

#elif defined(SYM_VXD) || defined(SYM_NTK)

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

#elif defined(SYM_VXD) || defined(SYM_NTK)

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

LINEARCALLBACKS gstNAVLinearCallBacks =
{
#ifndef SYM_NLM
    NAVLinearOpen,
    NAVLinearClose,
    NAVLinearRead,
    NAVLinearWrite,
    NAVLinearGetDimensions,
    NAVLinearConvertFromPhysical,
    NAVLinearConvertToPhysical,
    NAVLinearIsPartitionedMedia
#else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
};

GENERALCALLBACKS_T gstNAVGeneralCallBacks =
{
    &gstNAVLinearCallBacks,
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
#ifndef SYM_NLM
    NAVProgressInit,
    NAVProgressUpdate,
    NAVProgressClose,
    NULL
#else
    NV_ProgressInit,
    NV_ProgressUpdate,
    NV_ProgressClose,
    NULL
#endif
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


//=========================================================================//
//                                                                         //
//      NAVEX Callbacks - taken from the old avglob.cpp in Certlibn        //
//                                                                         //
//=========================================================================//

#ifndef SYM_NLM

/*@API:*************************************************************************
@Declaration:

BOOL WINAPI ScanLockUnlockDiskP ( // Locks / unlock physical drive
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

@Description:
This function is used to prepare a physical drive for disk read/write.

Note: this function does very little in SYM_DOS platform.

@Parameters:
$uOperation$ This parameter should contain 0 or 1.  If 0, the function will
             lock the drive, if 1, the function will unlock the drive.
$byDrive$ Should be an int-13 drive ID
$uRequestedAccess$ The possible settings can be:
        DISK_READ_WRITE If the code should lock/unlock write access
        DISK_READ_ONLY  If the code should lock/unlock read only access

@Returns:
    TRUE    if the operation was successful
    FALSE   if somebody has a lock on the drive for the requested access, or
            error.

@Include: virscan.h

*******************************************************************************/
#if defined(SYM_DOS)

BOOL WINAPI ScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    return (TRUE);
}

#elif defined(SYM_WIN32) && !defined(SYM_VXD)

BOOL WINAPI ScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
        return(TRUE);

    switch ( uRequestedAccess )
        {
        case DISK_READ_ONLY:

            switch ( uOperation )
                {
                case 0:

                    if ( DiskLockPhysicalVolume ( byDrive, 1, 1 ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockPhysicalVolume ( byDrive, 2, 1 ) )
                        {
                        DiskUnlockPhysicalVolume ( byDrive );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockPhysicalVolume ( byDrive );
                    DiskUnlockPhysicalVolume ( byDrive );

                    break;

                default:
                    return(FALSE);
                }

            break;

        case DISK_READ_WRITE:

            switch ( uOperation )
                {
                case 0:

                    if ( !ScanLockUnlockDiskP ( 0, byDrive, DISK_READ_ONLY ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockPhysicalVolume ( byDrive, 3, 1 ) )
                        {
                        ScanLockUnlockDiskP ( 1, byDrive, DISK_READ_ONLY );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockPhysicalVolume ( byDrive );

                    ScanLockUnlockDiskP ( 1, byDrive, DISK_READ_ONLY );

                    break;

                default:
                    return(FALSE);
                }

            break;

        default:
            return ( FALSE );
        }

    return ( TRUE );
}
#endif

/*@API:*************************************************************************
@Declaration:

BOOL WINAPI ScanLockUnlockDiskL ( // Locks / unlock logical drive
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );

@Description:
This function is used to prepare a logical drive for disk read/write.

Note: this function does very little in SYM_DOS platform.

@Parameters:
$uOperation$ This parameter should contain 0 or 1.  If 0, the function will
             lock the drive, if 1, the function will unlock the drive.
$byDrive$ Should be a drive ID - A, B, C, etc
$uRequestedAccess$ The possible settings can be:
        DISK_READ_WRITE If the code should lock/unlock write access
        DISK_READ_ONLY  If the code should lock/unlock read only access

@Returns:
    TRUE    if the operation was successful
    FALSE   if somebody has a lock on the drive for the requested access, or
            error.

@Include: virscan.h

*******************************************************************************/
#if defined(SYM_DOS)

BOOL WINAPI ScanLockUnlockDiskL (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    return (TRUE);
}

#elif defined(SYM_WIN32) && !defined(SYM_VXD)

BOOL WINAPI ScanLockUnlockDiskL (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    )
{
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_95)
        return(TRUE);

    switch ( uRequestedAccess )
        {
        case DISK_READ_ONLY:

            switch ( uOperation )
                {
                case 0:

                    if ( DiskLockLogicalVolume ( byDrive, 1, 1 ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockLogicalVolume ( byDrive, 2, 1 ) )
                        {
                        DiskUnlockLogicalVolume ( byDrive );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockLogicalVolume ( byDrive );
                    DiskUnlockLogicalVolume ( byDrive );

                    break;

                default:
                    return(FALSE);
                }

            break;

        case DISK_READ_WRITE:

            switch ( uOperation )
                {
                case 0:

                    if ( !ScanLockUnlockDiskL ( 0, byDrive, DISK_READ_ONLY ) )
                        {
                        return ( FALSE );
                        }

                    if ( DiskLockLogicalVolume ( byDrive, 3, 1 ) )
                        {
                        ScanLockUnlockDiskL ( 1, byDrive, DISK_READ_ONLY );
                        return ( FALSE );
                        }

                    break;

                case 1:

                    DiskUnlockLogicalVolume ( byDrive );

                    ScanLockUnlockDiskL ( 1, byDrive, DISK_READ_ONLY );

                    break;

                default:
                    return(FALSE);
                }

            break;

        default:
            return ( FALSE );
        }

    return ( TRUE );
}
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
UINT DOS_LOADDS WINAPI NAVEXSDiskReadPhysical(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive )
{
    BYTE    byResult;
    BYTE    byCounter = 0;

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = bDrive;

#if defined(SYM_DOS) || defined(SYM_WIN32)
    byResult = DiskGetPhysicalInfo(&stDiskRec);
                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( 1 );
        }
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0))  // don't lock first partition table
        {
        if ( ScanLockUnlockDiskP ( LOCK_DRIVE,
                                        bDrive,
                                        DISK_READ_ONLY ) == FALSE)
            {
            return(1);                  // error!
            }
        }
#endif

#ifdef SYM_WIN32
    // Win32 platform needs special handling for Promise EIDE 4030+ controller.
    // Buffer must not straddle two 4K memory pages.
    // To keep things simple, we'll only allow reads of up to 4K.

    byResult = SYM_ERROR_INVALID_FUNCTION;  // assume failure.

    if (bCount * PHYSICAL_SECTOR_SIZE <= 4096)
        {
        auto    LPBYTE  lpLocalBuffer;

        lpLocalBuffer = (LPBYTE)MemAllocPtr(GHND, 4096 * 2);

        if (NULL != lpLocalBuffer)
            {
                                        // Get 4K aligned buffer.
            stDiskRec.buffer = (LPBYTE)((DWORD)lpLocalBuffer + 4096
                                        - ((DWORD)lpLocalBuffer % 4096));

            stDiskRec.dwHead        = bHead;
            stDiskRec.dwTrack       = wCylinder;
            stDiskRec.dwSector      = bSector;
            stDiskRec.numSectors    = bCount;

            do
                {
                byResult = DiskAbsOperation(READ_COMMAND, &stDiskRec);
                byCounter++;
                }
            while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

                                    // On success, copy these sectors over.
            if (byResult == SYM_ERROR_NO_ERROR)
                {
                MEMCPY(lpBuffer, stDiskRec.buffer, bCount * PHYSICAL_SECTOR_SIZE);
                }

            MemFreePtr(lpLocalBuffer);
            }
         }
#else
    stDiskRec.dwHead        = bHead;
    stDiskRec.dwTrack       = wCylinder;
    stDiskRec.dwSector      = bSector;
    stDiskRec.numSectors    = bCount;
    stDiskRec.buffer        = (BYTE huge *) lpBuffer;

    do
        {
        byResult = DiskAbsOperation(READ_COMMAND, &stDiskRec);
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0))
        {
        ScanLockUnlockDiskP ( UNLOCK_DRIVE,
                              bDrive,
                              DISK_READ_ONLY );
        }
#endif

    return( (byResult == SYM_ERROR_NO_ERROR) ? 0 : 1 );
}

UINT DOS_LOADDS WINAPI NAVEXSDiskWritePhysical (
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive )
{

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

                                        // The following block fixes the
                                        // overwrites that were happening
                                        // on NT FAT partitions.
    if ( SystemGetWindowsType() == SYM_SYSTEM_WIN_NT &&
         bDrive >= FIRST_HD )
        {
        BYTE    byDriveLetter;
        BYTE    byPartNum;
        DWORD   dwLinearOffset;
        DWORD   dwBytes;

                                        // Do these sectors lie within a
                                        // valid partition?
        byPartNum = NAVEX_NTDeterminePartNum( bDrive,
                                              bHead,
                                              wCylinder,
                                              bSector,
                                              bCount,
                                              &dwLinearOffset );

                                        // Is this partition currently mapped
                                        // to a drive letter?
         if ( MAX_PARTITIONS != byPartNum &&
              0 != ( byDriveLetter = NAVEX_NTDetermineDriveLetter( bDrive,
                                                                  byPartNum )))
            {
                dwBytes = bCount * SECTOR_SIZE;

                                        // Write to the logical drive
                return ( NAVEX_NTDiskWriteLogical ( byDriveLetter,
                                                    dwLinearOffset,
                                                    lpBuffer,
                                                    &dwBytes ));
            }
        }
                                        // Otherwise, continue as before...
#endif  // SYM_WIN32 && !SYM_VXD && !SYM_NTK


    BYTE    byResult;
    BYTE    byCounter = 0;

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = bDrive;

#if defined(SYM_DOS) || defined(SYM_WIN32)
    byResult = DiskGetPhysicalInfo(&stDiskRec);

                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( 1 );
        }
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0 && bSector == 1))  // don't lock MBR
        {
        if ( ScanLockUnlockDiskP ( LOCK_DRIVE,
                                        bDrive,
                                        DISK_READ_WRITE ) == FALSE)
            {
            return(1);                  // error!
            }
        }
#endif

    stDiskRec.dwHead        = bHead;
    stDiskRec.dwTrack       = wCylinder;
    stDiskRec.dwSector      = bSector;
    stDiskRec.numSectors    = bCount;
    stDiskRec.buffer        = (BYTE huge *) lpBuffer;

    do
        {
        byResult = DiskAbsOperation(WRITE_COMMAND, &stDiskRec);
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

#if defined(SYM_WIN32) && !defined(SYM_VXD)
    if (!(wCylinder == 0 && bHead == 0 && bSector == 1))  // don't lock MBR
        {
        ScanLockUnlockDiskP ( UNLOCK_DRIVE,
                                   bDrive,
                                   DISK_READ_WRITE );
        }
#endif

    return( (byResult == SYM_ERROR_NO_ERROR) ? 0 : 1 );
}



#if defined(SYM_WIN32)
                                        // These functions are helpers for
                                        // LocalDiskWritePhysical under NT

//--------------------------------------------------------------------
// NAVEX_NTDeterminePartNum()
//
// Given a series of physical sectors (drive number, starting coordinates,
// and a sector count) this function returns the partition number (0-3) in
// which the specified sectors reside.
//
// If these sectors lies outside of all partitions, stradle a partition
// boundary, or if an error is encountered while determining the partition
// number, MAX_PARTITIONS (4) is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDeterminePartNum
(
    BYTE    byDrive,
    BYTE    byHead,
    WORD    wCylinder,
    BYTE    bySector,
    BYTE    bySectors,
    LPDWORD lpdwLinearOffset
)
{
    BYTE    byResult;
    BYTE    byCounter = 0;
    DWORD   dwLinearOffset;
    WORD    wMBROffset;
    BYTE    byPartNum;
    BYTE    abyMBR [ SECTOR_SIZE ];

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = byDrive;

    byResult = DiskGetPhysicalInfo(&stDiskRec);

    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( MAX_PARTITIONS );
        }

    dwLinearOffset = ( ( (DWORD) wCylinder *
                         stDiskRec.dwTotalHeads *
                         stDiskRec.dwSectorsPerTrack ) +
                       ( (DWORD) byHead * stDiskRec.dwSectorsPerTrack ) +
                       ( bySector - 1 ) );

    // Read MBR for this physical disk

    stDiskRec.dwHead        = 0;
    stDiskRec.dwTrack       = 0;
    stDiskRec.dwSector      = 1;
    stDiskRec.numSectors    = 1;
    stDiskRec.buffer        = (BYTE huge *) abyMBR;

    do
        {
        byResult = DiskAbsOperation( READ_COMMAND, &stDiskRec );
        byCounter++;
        }
    while ( (byResult != SYM_ERROR_NO_ERROR) && (byCounter < 3) );

    if ( byResult != SYM_ERROR_NO_ERROR )
        return ( MAX_PARTITIONS );

    for ( byPartNum = 0, wMBROffset = 0x1BE;
          byPartNum < MAX_PARTITIONS;
          ++byPartNum, wMBROffset += 0x10 )
        {
        if ( 0 != *((LPBYTE) abyMBR + wMBROffset + 4 ) &&
             dwLinearOffset >= *((LPDWORD) ( abyMBR + wMBROffset + 8 )) &&
             ( dwLinearOffset + bySectors ) <= *((LPDWORD) ( abyMBR + wMBROffset + 8 ))
                               + *((LPDWORD) ( abyMBR + wMBROffset + 12 )))
            {
            // byPartNum now indicates the first partition entry with a
            // valid non-zero filesystem byte which contains (inclusively)
            // the sector to be written.

            break;
            }
        }

    if ( NULL != lpdwLinearOffset )
        {
        dwLinearOffset -= *((LPDWORD) ( abyMBR + wMBROffset + 8 ));
        dwLinearOffset *= SECTOR_SIZE;
        *lpdwLinearOffset = dwLinearOffset;
        }

    return ( byPartNum );
}



//--------------------------------------------------------------------
// NAVEX_NTDetermineDriveLetter()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDetermineDriveLetter
(
    BYTE    byDrive,
    BYTE    byPartNum
)
{
    BYTE   byResult = 0;
    BYTE   abyDevices[ SYM_MAX_PATH * 2 ];
    BYTE   abyDIOCBuffer[ SYM_MAX_PATH * 2 ];
    BOOL   bDriveLetterFound = FALSE;
    TCHAR  tszThisDevice[] = _T( "\\Device\\HarddiskX\\PartitionX" );
    LPTSTR lptstrDevice;

    // create the string we're trying to match
    tszThisDevice[16] = '0' + ( byDrive - FIRST_HD );
    tszThisDevice[27] = '0' + byPartNum + 1;

    // get a list of devices
    if ( 0 == QueryDosDevice( NULL, (char *) abyDevices, SYM_MAX_PATH * 2 ))
        {
        return( 0 );
        }

    lptstrDevice = (LPTSTR) abyDevices;

    while( lptstrDevice[0] != 0 )
        {
        if( lptstrDevice[1] == ':' &&
            0 != QueryDosDevice( lptstrDevice, (char *) abyDIOCBuffer, SYM_MAX_PATH * 2 ) &&
            !STRICMP( (char *) abyDIOCBuffer, tszThisDevice ))
            {
            bDriveLetterFound = TRUE;
            break;
            }
        lptstrDevice += STRLEN( lptstrDevice ) + 1;
        }

    if( TRUE == bDriveLetterFound )
        {
        byResult =(BYTE) lptstrDevice[0];
        }

    return( byResult );
}


//--------------------------------------------------------------------
// NAVEX_NTDiskWriteLogical()
//
// Given a drive letter, a linear byte offset, some data, and a length,
// this function writes length bytes of the data to the logical drive
// starting at the linear byte offset.  The return values are identical
// to LocalDiskWritePhysical():
//
// 0 == success
// 1 == failure
//--------------------------------------------------------------------
BYTE NAVEX_NTDiskWriteLogical
(
    BYTE    byDriveLetter,
    DWORD   dwLinearOffset,
    LPVOID  lpvBuffer,
    LPDWORD lpdwSectors
)
{
    TCHAR  tszThisLogicalDrive[] = _T( "\\\\.\\X:" );
    HANDLE hLogicalDrive;
    BOOL   bLocked;
    BOOL   bDismounted;
    BOOL   bWritten;
    DWORD  dwDummy;

    tszThisLogicalDrive[4] =(TCHAR) byDriveLetter;

    hLogicalDrive = CreateFile( tszThisLogicalDrive,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL );

    if( INVALID_HANDLE_VALUE == hLogicalDrive )
        {
        return( 1 );
        }

    dwDummy = SetFilePointer( hLogicalDrive, dwLinearOffset, NULL, FILE_BEGIN );

    if( dwDummy != dwLinearOffset )
        {
        CloseHandle( hLogicalDrive );
        return( 1 );
        }

    bLocked = DeviceIoControl ( hLogicalDrive,
                                FSCTL_LOCK_VOLUME,
                                NULL,
                                0,
                                NULL,
                                0,
                                &dwDummy,
                                NULL );

    bWritten = WriteFile( hLogicalDrive, lpvBuffer, *lpdwSectors, lpdwSectors, NULL );

    if( bLocked )
        {
        bDismounted = DeviceIoControl ( hLogicalDrive,
                                        FSCTL_DISMOUNT_VOLUME,
                                        NULL,
                                        0,
                                        NULL,
                                        0,
                                        &dwDummy,
                                        NULL );

        bLocked = !DeviceIoControl ( hLogicalDrive,
                                     FSCTL_UNLOCK_VOLUME,
                                     NULL,
                                     0,
                                     NULL,
                                     0,
                                     &dwDummy,
                                     NULL );
        }

    CloseHandle( hLogicalDrive );

    if ( !bWritten )
        {
        return( 1 );
        }

    return( 0 );
}

#endif // SYM_WIN32

#endif // ifndef SYM_NLM


HFILE DOS_LOADDS WINAPI NAVEXFileOpen
(
    LPTSTR  lpName,
    UINT    uOpenMode
)
{
    SYM_ASSERT ( lpName );

#ifndef SYM_NLM
    return (FileOpen(lpName, uOpenMode));
#else
    return (NV_FileOpen(lpName, uOpenMode));
#endif

}

HFILE DOS_LOADDS WINAPI NAVEXFileClose
(
    HFILE   hHandle
)
{
    SYM_ASSERT ( hHandle );

    return (FileClose(hHandle));
}

UINT DOS_LOADDS WINAPI NAVEXFileRead
(
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    SYM_ASSERT ( lpBuffer );
    SYM_ASSERT ( uBytes );
    SYM_ASSERT ( hHandle );

    return (FileRead(hHandle, lpBuffer, uBytes));
}

UINT DOS_LOADDS WINAPI NAVEXFileWrite
(
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    SYM_ASSERT ( lpBuffer || (0 == uBytes) );
    SYM_ASSERT ( hHandle );

    return (FileWrite(hHandle, lpBuffer, uBytes));
}

DWORD DOS_LOADDS WINAPI NAVEXFileSeek
(
    HFILE   hHandle,
    LONG    lOffset,
    int     nFlag
)
{
    SYM_ASSERT ( hHandle );

    return (FileSeek(hHandle, lOffset, nFlag));
}

DWORD DOS_LOADDS WINAPI NAVEXFileLength
(
    HFILE   hHandle
)
{
    return (FileLength(hHandle));
}

BOOL DOS_LOADDS WINAPI NAVEXFileGetDateTime
(
    HFILE   hHandle,
    UINT    uType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
    WORD        wDosDate = 0;
    WORD        wDosTime = 0;
    FILETIME    FileTime = { 0 };

    (void) uType;

    SYM_ASSERT ( hHandle );
    SYM_ASSERT ( lpwDate );
    SYM_ASSERT ( lpwTime );

    FileGetTime (hHandle, TIME_LASTWRITE, &FileTime);
    FileTimeToDosTime(&FileTime, &wDosDate, &wDosTime);

    *lpwDate = wDosDate;
    *lpwTime = wDosTime;

    return (TRUE);
}

BOOL DOS_LOADDS WINAPI NAVEXFileSetDateTime
(
    HFILE   hHandle,
    UINT    uType,
    WORD    wDate,
    WORD    wTime
)
{
    FILETIME    FileTime = { 0 };

    (void) uType;

    SYM_ASSERT ( hHandle );

    DosTimeToFileTime (wDate, wTime, &FileTime);
    FileSetTime (hHandle, TIME_LASTWRITE, &FileTime);
    return (TRUE);
}


//========================================================================
//
// Function:
//  NAVEXFileDelete()
//
// Parameters:
//  lpFilename          Full Path of the file to Delete.
//
// Description:
//  This function:
//                 1. Overwrites the entire file with zeros.
//                 2. Truncates the file to 0 byes in size.
//                 3. Deletes the file.
//
// Returns:
//  TRUE if successful, FALSE otherwise.
//
//========================================================================
BOOL DOS_LOADDS WINAPI NAVEXFileDelete
(
    LPTSTR  lpFilename
)
{
    SYM_ASSERT (lpFilename);

#ifndef SYM_NLM
    return (FileWipe(lpFilename, 1) == NOERR);
#else
    // for the time enabling FileWipe() in CORE opens too many problems
    return (FileDelete(lpFilename) == NOERR);
#endif

}

UINT DOS_LOADDS WINAPI NAVEXFileGetAttr
(
    LPTSTR  lpFilename,
    LPUINT  lpuAttr
)
{
    SYM_ASSERT ( lpFilename );

    return (FileGetAttr(lpFilename, lpuAttr));
}

UINT DOS_LOADDS WINAPI NAVEXFileSetAttr
(
    LPTSTR  lpFilename,
    UINT    uAttr )
{
    SYM_ASSERT ( lpFilename );

    return (FileSetAttr(lpFilename, uAttr));
}

HFILE DOS_LOADDS WINAPI NAVEXFileCreate
(
    LPTSTR  lpFilename,
    UINT    uAttr
)
{
    SYM_ASSERT ( lpFilename );

    return (FileCreate(lpFilename, uAttr));
}

LPVOID DOS_LOADDS WINAPI NAVEXTempMemoryAlloc
(
    DWORD    dwSize
)
{
    return (MemAllocPtr(GHND, dwSize));
}

VOID DOS_LOADDS WINAPI NAVEXTempMemoryFree
(
    LPVOID  lpBuffer
)
{
    MemFreePtr (lpBuffer);
}

LPVOID DOS_LOADDS WINAPI NAVEXPermMemoryAlloc
(
    DWORD   dwSize
)
{
    return (MemAllocPtr(GHND, dwSize));
}

VOID DOS_LOADDS WINAPI NAVEXPermMemoryFree
(
    LPVOID  lpBuffer
)
{
    MemFreePtr (lpBuffer);
}

UINT DOS_LOADDS WINAPI NAVEXCreateMutex
(
    LPLPVOID        lplpvMutexInfo
)
{
    return (NAVMutexCreate(lplpvMutexInfo) == CBSTATUS_OK);
}

UINT DOS_LOADDS WINAPI NAVEXDestroyMutex
(
    LPVOID          lpvMutexInfo
)
{
    return (NAVMutexDestroy(lpvMutexInfo) == CBSTATUS_OK);
}

UINT DOS_LOADDS WINAPI NAVEXWaitMutex
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
    return (NAVMutexWait(lpvMutexInfo, dwTimeoutMS) == CBSTATUS_OK);
}

UINT DOS_LOADDS WINAPI NAVEXReleaseMutex
(
    LPVOID          lpvMutexInfo
)
{
    return (NAVMutexRelease(lpvMutexInfo) == CBSTATUS_OK);
}

HFILE DOS_LOADDS WINAPI NAVEXGetTempFile
(
    LPTSTR          lpszFileName,
    LPVOID          lpvCookie
)
{

#if defined(SYM_DOS) || defined(SYM_WIN)

    return (FileCreateTemporary(lpszFileName, FA_NORMAL));

#elif defined(SYM_VXD) || defined(SYM_NTK)

    auto   HFILE     hFile;
    auto   TCHAR     szGeneratedFileName[13];
    auto   int       i,j, iNum;

                                        // use the defs dir for our temp
                                        // path

    for (i = 0; i < 9999; i++)
    {
        for ( j = 3, iNum = i;
              j >= 0;
              szGeneratedFileName[j--] = _T('0') + (iNum % 10), iNum /= 10 );

        STRCPY(&szGeneratedFileName[4], _T("NAV~.TMP"));
        STRCPY(lpszFileName, s_szNAVPath);
        NameAppendFile(lpszFileName, szGeneratedFileName);

        hFile = FileCreate(lpszFileName,FA_NORMAL|0x80000000L);
        if (hFile != HFILE_ERROR)
        {
            return (hFile);
        }
    }

#endif

   return (HFILE_ERROR);
}

BOOL DOS_LOADDS WINAPI NAVEXProgress
(
    WORD            wProgress,
    DWORD           dwCookie )
{
#ifdef SYM_NLM

    RelinquishControl();

#endif
    return(0);
}

UINT DOS_LOADDS WINAPI NAVEXMessageBox
(
    DWORD   dwBoxID,
    LPTSTR  lpszMessage,
    DWORD   dwButtonFlags,
    LPDWORD lpdwResult,
    DWORD   dwMSTimeOut,
    LPVOID  lpvCookie
)
{
                                        // if lpCookie is NULL we don't
                                        // display any UI - just return
                                        // NAVEX_UI_TIMEOUT so the caller
                                        // will perform the default action.

    if (!lpvCookie)
    {
        return (NAVEX_UI_TIMEOUT);
    }

#if defined(SYM_WIN32)

    return ( NAVEX_UI_TIMEOUT );

/*&? Implement me! ***
    return (NAVEXMessageBox32(dwBoxID,
                              lpszMessage,
                              dwButtonFlags,
                              lpdwResult,
                              dwMSTimeOut,
                              lpvCookie));
*********************/

#elif defined(SYM_DOSX)

    DWORD dwButtonArray[6];
    WORD  wButtonCount = 0, ch;

    extern char SZ_OK [];
    extern char SZ_CANCEL [];
    extern char SZ_CONTINUE [];
    extern char SZ_YES [];
    extern char SZ_NO [];
    extern char SZ_PREVIOUS [];
    extern char SZ_ENTER_A_NUMBER [];

                                        // display the provided message

    printf("\n\n%s\n\n", lpszMessage);

                                        // dynamically build a user menu and
                                        // a corresponding array of possible
                                        // results

    if (dwButtonFlags & BUTTON_FLAG_OK)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_OK;
        printf("%3d. %s\n", ++wButtonCount, SZ_OK);
    }

    if (dwButtonFlags & BUTTON_FLAG_CANCEL)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_CANCEL;
        printf("%3d. %s\n", ++wButtonCount, SZ_CANCEL);
    }

    if (dwButtonFlags & BUTTON_FLAG_CONTINUE)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_CONTINUE;
        printf("%3d. %s\n", ++wButtonCount, SZ_CONTINUE);
    }

    if (dwButtonFlags & BUTTON_FLAG_YES)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_YES;
        printf("%3d. %s\n", ++wButtonCount, SZ_YES);
    }

    if (dwButtonFlags & BUTTON_FLAG_NO)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_NO;
        printf("%3d. %s\n", ++wButtonCount, SZ_NO);
    }

    if (dwButtonFlags & BUTTON_FLAG_PREVIOUS)
    {
        dwButtonArray[wButtonCount] = BUTTON_FLAG_PREVIOUS;
        printf("%3d. %s\n", ++wButtonCount, SZ_PREVIOUS);
    }

    printf("\n%s: ", SZ_ENTER_A_NUMBER);

                                        // wait for user input

    while ((ch = getch()) < '1' || ch > (wButtonCount + '0'));

    printf("\n");
                                        // assign user's the selection to
                                        // *lpdwResult

    *lpdwResult = dwButtonArray[ch - '0' - 1];

#elif defined(SYM_NTK)

    return (NAVEX_UI_TIMEOUT);

#elif defined(SYM_VXD)

    RECT rMessageBoxRect =
    {
        5, 8, 75, 18
    };

    auto   TCHAR szButtonString[128];
    auto   UINT  uResult;
    extern TCHAR *apszNAVEXButtons[];
    extern TCHAR apszNAVEXMessageBoxTitle[];
    szButtonString[0] = EOS;
    PMESSAGE_BOX_COOKIE pstMessageBoxCookie = (PMESSAGE_BOX_COOKIE)lpvCookie;

                                        // dynamically build a ButtonString
                                        // for AsyncMsgBox

    if (!pstMessageBoxCookie->bUserInterventionAllowed)
    {
        return (NAVEX_UI_TIMEOUT);
    }

    if (dwButtonFlags & BUTTON_FLAG_OK)
    {
        STRCAT(szButtonString, _T("1"));
        STRCAT(szButtonString, apszNAVEXButtons[1]);
    }

    if (dwButtonFlags & BUTTON_FLAG_CANCEL)
    {
        STRCAT(szButtonString, _T("2"));
        STRCAT(szButtonString, apszNAVEXButtons[2]);
    }

    if (dwButtonFlags & BUTTON_FLAG_CONTINUE)
    {
        STRCAT(szButtonString, _T("3"));
        STRCAT(szButtonString, apszNAVEXButtons[3]);
    }

    if (dwButtonFlags & BUTTON_FLAG_YES)
    {
        STRCAT(szButtonString, _T("4"));
        STRCAT(szButtonString, apszNAVEXButtons[4]);
    }

    if (dwButtonFlags & BUTTON_FLAG_NO)
    {
        STRCAT(szButtonString, _T("5"));
        STRCAT(szButtonString, apszNAVEXButtons[5]);
    }

    if (dwButtonFlags & BUTTON_FLAG_PREVIOUS)
    {
        STRCAT(szButtonString, _T("6"));
        STRCAT(szButtonString, apszNAVEXButtons[6]);
    }

    uResult = AsyncMsgBox(&rMessageBoxRect,
                          pstMessageBoxCookie->uTimeOut,
                          0,
                          AMB_COLOR_BLUE,
                          apszNAVEXMessageBoxTitle,
                          lpszMessage,
                          NULL,
                          szButtonString);

                                        // assign the user's selection to
                                        // *lpdwResult

    switch (uResult)
    {
        case 1:
            *lpdwResult = BUTTON_FLAG_OK;
            break;
        case 2:
            *lpdwResult = BUTTON_FLAG_CANCEL;
            break;
        case 3:
            *lpdwResult = BUTTON_FLAG_CONTINUE;
            break;
        case 4:
            *lpdwResult = BUTTON_FLAG_YES;
            break;
        case 5:
            *lpdwResult = BUTTON_FLAG_NO;
            break;
        case 6:
            *lpdwResult = BUTTON_FLAG_PREVIOUS;
            break;
        default:
            *lpdwResult = 0;
            return (NAVEX_UI_TIMEOUT);
    }

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxInit
(
    LPTSTR   lpszTitle,
    DWORD    dwButtonFlags,
    LPLPVOID lplpvInfoBoxInfo,
    LPVOID   lpvCookie
)
{

#if defined(SYM_WIN32)

    return ( NAVEX_UI_OK );

/*&? Implement me ***
    return (NAVEXInfoBoxInit32(lpszTitle,
                               dwButtonFlags,
                               lplpvInfoBoxInfo,
                               lpvCookie));
********************/

#elif defined(SYM_DOSX)

    printf("\n\n");

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxUpdate
(
    LPVOID  lpvInfoBoxInfo,
    LPTSTR  lpszText,
    LPDWORD lpdwResultFlags,
    LPVOID  lpvCookie
)
{

#if defined(SYM_WIN32)

    return ( NAVEX_UI_OK );

/*&? Implement me ***
    return (NAVEXInfoBoxUpdate32(lpvInfoBoxInfo,
                                 lpszText,
                                 lpdwResultFlags,
                                 lpvCookie));
********************/

#elif defined(SYM_DOSX)

                                        // erase the old text with 80 spaces
                                        // (This limits the text length to
                                        // 80 characters). Then display the
                                        // the new text.

    printf("\r                                                                                ");
    printf("\r%s", lpszText);

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxClose
(
    LPVOID lpvInfoBoxInfo,
    LPVOID lpvCookie
)
{

#if defined(SYM_WIN32)

    return ( NAVEX_UI_OK );

/*&? Implement me ***
    return (NAVEXInfoBoxClose32(lpvInfoBoxInfo,
                                lpvCookie));
********************/

#elif defined(SYM_DOSX)

    printf("\n\n");

#endif

    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXTextEditBox
(
    DWORD  dwBoxID,
    LPTSTR lpszPrompt,
    LPTSTR lpszSeedText,
    LPTSTR lpszPutHere,
    UINT   nMaxLength,
    DWORD  dwMSTimeOut,
    LPVOID lpvCookie
)
{

#if defined(SYM_WIN32)

    return ( NAVEX_UI_TIMEOUT );

/*&? Implement me ***
    return (NAVEXTextEditBox32(dwBoxID,
                               lpszPrompt,
                               lpszSeedText,
                               lpszPutHere,
                               nMaxLength,
                               dwMSTimeOut,
                               lpvCookie));
********************/

#else

    return (NAVEX_UI_TIMEOUT);

#endif

}

VOID DOS_LOADDS WINAPI NAVEXGetConfigInfo
(
    LPTSTR lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
)
{
    LPOEMCONTEXT        lpOEMContext;

    // Default to not implemented.

    *lpbImplemented = FALSE;

    // Get per scan context information.

    lpOEMContext = GetScanContext();

    if ( ( NULL != lpOEMContext ) &&
         ( OEM_SIGNATURE == lpOEMContext->dwOEMSignature ) )
    {
        if ( !STRCMP( lpszID, _T("heur") ) ||
             !STRCMP( lpszID, _T("macroheur") ) )
        {
            if ( VIRLIB_HEURISTICS_DEFAULT == lpOEMContext->wHeuristicScanningLevel )
            {
                // Klugery for bad NAVENG. First version that shipped will
                // disable heuristics (by scanning no extensions) if this 
                // callback is not implemented and the backup .INF file is not 
                // present. Default should be to enable at level 3.
            
                if ( !FileExists( lpOEMContext->szNavexInf ) )
                {
                    *lpbImplemented = TRUE;
                    STRCPY( (LPTSTR)lpvResult, _T("3") );
                }
            }
            else
            {    
                *lpbImplemented = TRUE;
                SPRINTF( (LPTSTR)lpvResult, _T("%d"), lpOEMContext->wHeuristicScanningLevel - 1 );
            }
        }
    }
}


VOID DOS_LOADDS WINAPI NAVEXRelinquishControl(VOID)

{
#ifdef SYM_NLM

    RelinquishControl();

#endif
}


CALLBACKREV2 gstNAVEXCallBacks =
{
    NAVEXFileOpen,  // HFILE  (WINAPI FAR *FileOpen)(LPTSTR lpName, UINT uOpenMode);
    NAVEXFileClose, // HFILE  (WINAPI FAR *FileClose)(HFILE hHandle);
    NAVEXFileRead,  // UINT   (WINAPI FAR *FileRead)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    NAVEXFileWrite, // UINT   (WINAPI FAR *FileWrite)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    NAVEXFileSeek,  // DWORD  (WINAPI FAR *FileSeek)(HFILE hHandle, LONG dwOffset, int nFlag);
    NAVEXFileLength,// DWORD  (WINAPI FAR *FileSize)(HFILE hHandle);
    NAVEXFileGetDateTime,// BOOL (WINAPI FAR *FileGetDateTime)(HFILE hHandle, UINT uType, WORD FAR *lpuDate, WORD FAR *lpuTime);
    NAVEXFileSetDateTime,// BOOL (WINAPI FAR *FileSetDateTime)(HFILE hHandle, UINT uType, WORD uDate, WORD uTime);
    NAVEXFileDelete,// BOOL   (WINAPI FAR *FileDelete)(LPTSTR lpFilename);
    NAVEXFileGetAttr,// UINT  (WINAPI FAR *FileGetAttr)(LPTSTR lpFilename, UINT FAR *lpuAttr);
    NAVEXFileSetAttr,// UINT  (WINAPI FAR *FileSetAttr)(LPTSTR lpFilename, UINT uAttr);
    NAVEXFileCreate,// HFILE  (WINAPI FAR *FileCreate)(LPTSTR lpFilename, UINT uAttr);
    NULL,           // BOOL   (WINAPI FAR *FileLock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    NULL,           // BOOL   (WINAPI FAR *FileUnlock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    NAVEXProgress,  // BOOL   (WINAPI FAR *Progress)(WORD wProgress, DWORD dwCookie);
#ifndef SYM_NLM
    NAVEXSDiskReadPhysical, // UINT   (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
    NAVEXSDiskWritePhysical,// UINT   (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
#else
    NULL,                // UINT   (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
    NULL,                // UINT   (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);
#endif
    NAVEXTempMemoryAlloc,// LPVOID (WINAPI FAR *TempMemoryAlloc)(DWORD uSize);
    NAVEXTempMemoryFree, // VOID   (WINAPI FAR *TempMemoryFree)(LPVOID lpBuffer);
    NAVEXPermMemoryAlloc,// LPVOID (WINAPI FAR *PermMemoryAlloc)(DWORD uSize);
    NAVEXPermMemoryFree, // VOID   (WINAPI FAR *PermMemoryFree)(LPVOID lpBuffer);
    NAVEXCreateMutex,    // UINT   (WINAPI FAR *CreateMutex)(LPLPVOID lplpMutex);
    NAVEXDestroyMutex,   // UINT   (WINAPI FAR *DestroyMutex)(LPVOID lpMutex);
    NAVEXWaitMutex,      // UINT   (WINAPI FAR *WaitMutex)(LPVOID lpMutex, DWORD dwMSTicks);
    NAVEXReleaseMutex,   // UINT   (WINAPI FAR *ReleaseMutex)(LPVOID lpMutex);
    NAVEXGetTempFile,    // UINT   (WINAPI FAR *GetTempFile)(LPTSTR lpszFileName);
    NAVEXMessageBox,     // UINT   (WINAPI FAR *MessageBox)(DWORD dwBoxID, LPTSTR lpszMessage, DWORD dwButtonFlags, LPDWORD lpdwResultFlags, DWORD dwMSTimeOut);
    NAVEXInfoBoxInit,    // UINT   (WINAPI FAR *InfoBoxInit)(LPTSTR lpszTitle, DWORD dwButtonFlags, LPLPVOID lplpvInfoBoxInfo);
    NAVEXInfoBoxUpdate,  // UINT   (WINAPI FAR *InfoBoxUpdate)(LPVOID lpvInfoBoxInfo, LPTSTR lpszText, LPDWORD lpdwResultFlags);
    NAVEXInfoBoxClose,   // UINT   (WINAPI FAR *InfoBoxClose)(LPVOID lpvInfoBoxInfo);
    NAVEXTextEditBox,    // UINT   (WINAPI FAR *TextEditBox)(DWORD dwBoxID, LPTSTR lpszPrompt, LPTSTR lpszSeedText, LPTSTR lpszPutHere, int nMaxLength, DWORD dwMSTimeOut);
    NAVEXGetConfigInfo,  // VOID   (WINAPI FAR *GetConfigInfo)(LPTSTR lpszID, LPVOID lpvResult, UINT uMaxResultLen, LPBOOL lpbImplemented, LPVOID lpvCookie);
    NAVEXRelinquishControl, // VOID   (WINAPI FAR *RelinquishControl)(VOID);
};

EXTERNC AVGLOBALCALLBACKS g_AVCallbacks =
{
    &gstNAVGeneralCallBacks,
    &gstNAVProgressCallBacks,
    &gstNAVDataFileCallBacks,
    &gstNAVIPCCallBacks,
    &gstNAVMemoryCallBacks,
    &gstNAVEXCallBacks,
};

