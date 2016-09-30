// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/navcb.cpv   1.15   02 Sep 1998 13:00:20   dhertel  $
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
// $Log:   S:/NAVAPI/VCS/navcb.cpv  $
//
//    Rev 1.15   02 Sep 1998 13:00:20   dhertel
// Added NLM critical section calls.
//
//    Rev 1.14   28 Aug 1998 15:04:08   dhertel
//
//    Rev 1.13   27 Aug 1998 14:50:58   MKEATIN
// Copy the changes dhertel made on TempMemoryAlloc/free to PermMemoryAlloc/
// free.
//
//    Rev 1.12   25 Aug 1998 13:51:20   dhertel
//
//    Rev 1.11   17 Aug 1998 16:44:14   MKEATIN
// Changed a SYM_WIN32 to SYM_WIN in MemGetPtrToMem().
//
//    Rev 1.10   11 Aug 1998 20:22:50   DHERTEL
// Fixed bug in NLM NAVEXGetConfigInfo()
//
//    Rev 1.9   10 Aug 1998 21:51:04   MKEATIN
// One more DiskGetPhyscialInfo was ifdefed out on Win16.
//
//    Rev 1.8   10 Aug 1998 21:48:16   MKEATIN
// Wasn't making a required call to DiskGetPhysicalInfo on Win16.
//
//    Rev 1.7   04 Aug 1998 15:47:56   MKEATIN
// Changed a SYM_WIN32 to SYM_WIN in GetConfigInfo.
//
//    Rev 1.6   30 Jul 1998 21:51:54   DHERTEL
// Changes for NLM packaging of NAVAPI.
//
//    Rev 1.5   29 Jul 1998 18:34:04   DHERTEL
// DX and NLM changes
//
//    Rev 1.4   20 Jul 1998 14:28:38   MKEATIN
// Fixed comments.
//
//    Rev 1.3   20 Jul 1998 14:26:12   MKEATIN
// Removed SYM_EXPORT from AVGLOBALCALLBACKS.
//
//    Rev 1.2   11 Jun 1998 17:37:12   DALLEE
// Route all file IO through VirtFileXXX() routines.
// These correctly switch between client-defined virtual file IO
// and standard file IO.
//
//    Rev 1.1   27 May 1998 17:39:02   MKEATIN
// changed NAVSetDefsPath to NAVSetStaticData.  This now set's the
// defs path and the heuristic level.
//
//    Rev 1.0   22 May 1998 20:28:50   MKEATIN
// Initial revision.
//
// Pulled from AVCORE CALLBACK callback.cpp r1.25
//        Rev 1.25   04 May 1998 20:21:04   DBuches
//     Ported changes from branch.
//************************************************************************

#include "platform.h"
#include "file.h"
#if !defined(SYM_UNIX)
#include "disk.h"
#endif
#include "xapi.h"
#include "symsync.h"
#if !defined(SYM_UNIX)
#include "symcfg.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#if !defined(SYM_UNIX)
#include "conio.h"
#endif

#include "avtypes.h"
#include "avcb.h"
#include "pamapi.h"
#include "avenge.h"
#include "callbk15.h"

#include "navcb_l.h"
#include "avapi.h"
#include "nlm_nav.h"
#include "navapi.h"
#include "virtcb.h"

#ifdef SYM_NLM
#include "navnlmex.h"
#include "symserve.h"
#endif

#if defined(SYM_UNIX)
#include <pthread.h>
#include <ctype.h>
#endif

#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK) && !defined(SYM_UNIX)
  #include "winioctl.h"
  #include "NAVEX_NT.h"
#elif defined (SYM_VXD)
  #include "symevent.h"
#endif

// Static data maintained for engine callbacks

static TCHAR     s_szDefsPath [ SYM_MAX_PATH + 1 ];
static WORD      s_wHeuristicLevel = 2;     // default to 2 -- on, medium
static NAVFILEIO s_stFileIoCallbacks;

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

#if !defined(SYM_NLM) && !defined(SYM_UNIX)
UINT DOS_LOADDS WINAPI NAVEXSDiskReadPhysical
(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive
);

UINT DOS_LOADDS WINAPI NAVEXSDiskWritePhysical
(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive
);
#endif

                                        // local defines
#define LOCK_DRIVE          0
#define UNLOCK_DRIVE        1

#define DISK_READ_WRITE     1
#define DISK_READ_ONLY      2


//************************************************************************
//************************************************************************

//*************************************************************************
// NAVSetStaticData()
//
// VOID NAVSetStaticData (
//      LPCTSTR     lpszDefsPath
//      WORD        wHeuristicLevel
//      LPNAVFILEIO lpFileCallbacks )
//
// This routine saves data required by NAVAPI's engine callbacks.
// Some engine callbacks have no way of obtaining any context information
// from NAVAPI and we're forced to keep static data for them.
//
// Returns:
//      Nothing.
//*************************************************************************
// 06/09/98 DALLEE, added pointer to file callbacks.
// 03/02/99 Jim Brennan, allow NULL for lpFileCallbacks.
//*************************************************************************

VOID SetStaticCallbackData
(
    LPCTSTR     lpszDefsPath,
    WORD        wHeuristicLevel,
    LPNAVFILEIO lpFileCallbacks
)
{
    extern NAVFILEIO g_stDefaultFileIoCallbacks;

    STRNCPY( s_szDefsPath, lpszDefsPath, SYM_MAX_PATH);
    s_wHeuristicLevel = wHeuristicLevel;

    if (lpFileCallbacks)
        s_stFileIoCallbacks = *lpFileCallbacks;
    else
        s_stFileIoCallbacks = g_stDefaultFileIoCallbacks;

} // NAVSetStaticData()


//*************************************************************************
// GetFileIoCallbacks()
//
// LPNAVFILEIO GetFileIoCallbacks ( )
//
// This routine returns a pointer to the stored NAVAPI client file
// IO callbacks.
//
// Returns:
//      Pointer to NAVAPI client file IO callbacks.
//*************************************************************************
// 6/9/98 DALLEE, created.
//*************************************************************************

LPNAVFILEIO GetFileIoCallbacks()
{
    return ( &s_stFileIoCallbacks );
} // GetFileIoCallbacks()


//*************************************************************************
// GetDefsPath()
//
// LPCTSTR GetDefsPath( )
//
// This routine returns a pointer to the stored definitions path.
//
// Returns:
//      Pointer to definitions path.
//*************************************************************************
// 6/9/98 DALLEE, created.
//*************************************************************************

LPCTSTR GetDefsPath()
{
    return ( s_szDefsPath );
} // GetDefsPath()


CBSTATUS NAVFileOpen
(
    LPVOID      lpvFileInfo,
    WORD        wOpenMode
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    auto    HFILE               hFile;
    auto    BOOL                bSuccess;

    bSuccess = VirtFileOpen( lpFileInfo->lpstrPathName, wOpenMode, &hFile );

    if ( !bSuccess )
        return ( CBSTATUS_ERROR );

    lpFileInfo->hFile = hFile;

    return ( CBSTATUS_OK );
}


CBSTATUS NAVFileClose
(
    LPVOID      lpvFileInfo
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION)lpvFileInfo;
    auto    HFILE               hFile;

    hFile = VirtFileClose( lpFileInfo->hFile );

    if (hFile == HFILE_ERROR)
        return ( CBSTATUS_ERROR );

    return ( CBSTATUS_OK );
}


CBSTATUS NAVFileSeek
(
    LPVOID      lpvFileInfo,
    long        lOffset,
    WORD        wWhence,
    LPDWORD     lpdwNewOffset
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION) lpvFileInfo;

    *lpdwNewOffset = VirtFileSeek( lpFileInfo->hFile, lOffset, wWhence );

    if ( (DWORD) -1 == *lpdwNewOffset )
        return ( CBSTATUS_ERROR );

    return ( CBSTATUS_OK );
}


CBSTATUS NAVFileRead
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION) lpvFileInfo;
    auto    UINT                uBytesRead;

    // Only attempt read if bytes to read is non-zero

    if ( 0 != dwNumBytes )
    {
        uBytesRead = VirtFileRead( lpFileInfo->hFile,
                                   lpvBuffer,
                                   (UINT) dwNumBytes );

        if ( (UINT) -1 == uBytesRead )
            return( CBSTATUS_ERROR );

        *lpdwNumActualBytes = uBytesRead;
    }
    else
    {
        *lpdwNumActualBytes = 0;
    }

    return ( CBSTATUS_OK );
}


CBSTATUS NAVFileWrite
(
    LPVOID      lpvFileInfo,
    LPVOID      lpvBuffer,
    DWORD       dwNumBytes,
    LPDWORD     lpdwNumActualBytes
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION) lpvFileInfo;
    auto    UINT                uBytesWritten;

    // Only attempt write if bytes to write is non-zero

    if ( 0 != dwNumBytes )
    {
        uBytesWritten = VirtFileWrite( lpFileInfo->hFile,
                                       lpvBuffer,
                                       (UINT) dwNumBytes );

        if ( (UINT) -1 == uBytesWritten )
            return( CBSTATUS_ERROR );

        *lpdwNumActualBytes = uBytesWritten;
    }
    else
    {
        *lpdwNumActualBytes = 0;
    }

    return ( CBSTATUS_OK );
}


CBSTATUS NAVFileTell
(
    LPVOID      lpvFileInfo,
    LPDWORD     lpdwOffset
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION) lpvFileInfo;
    auto    DWORD               dwOffset;

    dwOffset = VirtFileSeek( lpFileInfo->hFile, 0, SEEK_CUR );

    if ( -1 == dwOffset )
        return( CBSTATUS_ERROR );

    *lpdwOffset = dwOffset;

    return ( CBSTATUS_OK );
}


CBSTATUS NAVFileSetEOF
(
    LPVOID      lpvFileInfo
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION) lpvFileInfo;

    if ( (UINT) -1 == VirtFileWrite( lpFileInfo->hFile, (LPVOID)"", 0 ) )
    {
        return ( CBSTATUS_ERROR );
    }

    return ( CBSTATUS_OK );
}


CBSTATUS NAVFileEOF
(
    LPVOID lpvFileInfo,
    LPBOOL lpbEOF
)
{
      return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileGetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPDWORD lpdwInfo
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileSetAttr
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    DWORD   dwInfo
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileGetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
    return ( CBSTATUS_OK );
}

CBSTATUS NAVFileSetDateTime
(
    LPVOID  lpvFileInfo,
    WORD    wType,
    WORD    wDate,
    WORD    wTime
)
{
    return ( CBSTATUS_OK );
}

CBSTATUS NAVFileFullNameAndPath
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileName
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFileName,
    WORD    wBufferLen
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFilePath
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrFilePath,
    WORD    wBufferLen
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileLogicalDrive
(
    LPVOID  lpvFileInfo,
    LPWSTR  lpwstrDriveLetter,
    WORD    wBufferLen
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileLength
(
    LPVOID  lpvFileInfo,
    LPDWORD lpdwFileLength
)
{
    auto    LPFILEINFORMATION   lpFileInfo = (LPFILEINFORMATION) lpvFileInfo;
    auto    DWORD               dwLength;

    dwLength = VirtFileLength( lpFileInfo->hFile );

    if ( (DWORD) -1 == dwLength )
        return ( CBSTATUS_ERROR );

    *lpdwFileLength = dwLength;

    return ( CBSTATUS_OK );
}

CBSTATUS NAVFileDelete
(
    LPVOID  lpvFileInfo
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileCopy
(
    LPVOID  lpvSrcFileInfo,
    LPVOID  lpvDstFileInfo
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileRename
(
    LPVOID  lpvOldFileInfo,
    LPWSTR  lpwstrNewName
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileExists
(
    LPVOID  lpvFileInfo,
    LPBOOL  lpbExists
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVFileGetErrorCondition
(
    LPVOID  lpvFileInfo,
    LPDWORD lpdwErrorCondition
)
{
    (void)lpvFileInfo;

    *lpdwErrorCondition = 0;

    return ( CBSTATUS_OK );
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
        return ( CBSTATUS_ERROR );

    *lplpvBuffer = lpvData;

    return ( CBSTATUS_OK );
}

CBSTATUS NAVMemoryFree
(
    LPVOID lpvBuffer
)
{
    MemFreePtr (lpvBuffer);

    return ( CBSTATUS_OK );
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
    LPFILEINFORMATION   lpFileInfo;
    BOOL                bWildDefs = FALSE;
    LPTSTR              lpFileName = NULL;
#if defined(SYM_UNIX)
    int                 i;
    TCHAR               szFileName[13];
#endif

#ifdef SYM_DOS
    bWildDefs = AreDefsWild();
#endif

    lpFileInfo = (LPFILEINFORMATION)MemAllocPtr(GHND,sizeof(FILEINFO_T));
    if (lpFileInfo == NULL)
	return ( CBSTATUS_ERROR );

    lpFileInfo->hFile = NULL;

    STRCPY(lpFileInfo->lpstrPathName, s_szDefsPath);

    switch (wDataFileNum)
    {
	case DATA_FILE_ID_VIRSCAN1DAT:
	    if (bWildDefs)
		lpFileName = _T("VIRWILD1.DAT");
	    else
		lpFileName = _T("VIRSCAN1.DAT");
	    break;
	case DATA_FILE_ID_VIRSCAN2DAT:
	    if (bWildDefs)
		lpFileName = _T("VIRWILD2.DAT");
	    else
		lpFileName = _T("VIRSCAN2.DAT");
	    break;
	case DATA_FILE_ID_VIRSCAN3DAT:
	    if (bWildDefs)
		lpFileName = _T("VIRWILD3.DAT");
	    else
		lpFileName = _T("VIRSCAN3.DAT");
	    break;
	case DATA_FILE_ID_VIRSCAN4DAT:
	    if (bWildDefs)
		lpFileName = _T("VIRWILD4.DAT");
	    else
		lpFileName = _T("VIRSCAN4.DAT");
	    break;
	default:
	    return ( CBSTATUS_ERROR );
    }

#if defined(SYM_UNIX)
	// Make the data file name lowercase.
    for (i = 0; lpFileName[i] != '\0'; ++i)
	    szFileName[i] = tolower(lpFileName[i]);
	szFileName[i] = '\0';
	lpFileName = szFileName;
#endif

    NameAppendFile(lpFileInfo->lpstrPathName, lpFileName);

    *lplpvFileInfo = (LPVOID)lpFileInfo;
    return ( CBSTATUS_OK );
}

CBSTATUS NAVCloseDataFileInfo
(
    LPVOID lpvFileInfo
)
{
    MemFreePtr(lpvFileInfo);

    return ( CBSTATUS_OK );
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

#if defined(SYM_DOSX) || defined(SYM_WIN) || defined(SYM_VXD)

    *lplpbyMemPtr = (LPBYTE)GetProtModePtr((LPVOID)dwSegOffPtr);

    return ( CBSTATUS_OK );

#else

    return ( CBSTATUS_ERROR );

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

    return ( CBSTATUS_OK );
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
#if defined(SYM_NLM)
    RelinquishControl();
#endif
    return ( CBSTATUS_OK );
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
#ifdef SYM_NLM

    RelinquishControl();

#endif
    return ( CBSTATUS_OK );
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
    return ( CBSTATUS_OK );
}


#if !defined(SYM_NLM) && !defined(SYM_UNIX)

CBSTATUS NAVLinearOpen
(
    LPVOID  lpvInfo,                // [in] Linear object info
    WORD    wOpenMode               // [in] filler
)
{
    return ( CBSTATUS_OK );
}


CBSTATUS NAVLinearClose
(
    LPVOID  lpvInfo                 // [in] Linear object info
)
{
    return ( CBSTATUS_OK );
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

    if (NAVLinearConvertToPhysical (lpvInfo,
                                    dwSectorLow,
                                    &bySide,
                                    &wCylinder,
                                    &bySector ) == CBSTATUS_ERROR)
    {
        return ( CBSTATUS_ERROR );
    }

    for (i = 0; i < 2; i++)             // two attempts
    {
        // This NAVEXSDiskReadPhysical aligns lpbyBuffer on a 4k bounder before calling
        // DiskAbsOperation().  This is necessary for Promise and MO drives.

        uResult = NAVEXSDiskReadPhysical(lpbyBuffer, 1, wCylinder, bySector, bySide, (BYTE)lpvInfo);

        if (!uResult)                   // Success
        {
            *lpdwActualLength = dwLength;
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

    if (NAVLinearConvertToPhysical (lpvInfo,
                                    dwSectorLow,
                                    &bySide,
                                    &wCylinder,
                                    &bySector ) == CBSTATUS_ERROR)
    {
        return ( CBSTATUS_ERROR );
    }

    for (i = 0; i < 2; i++)             // two attempts
    {
        // This NAVEXSDiskReadPhysical aligns lpbyBuffer on a 4k bounder before calling
        // DiskAbsOperation().  This is necessary for Promise and MO drives.

        uResult = NAVEXSDiskWritePhysical(lpbyBuffer, 1, wCylinder, bySector, bySide, (BYTE)lpvInfo);

        if (!uResult)                   // Success
        {
            *lpdwActualLength = dwLength;
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

    if (!HWIsNEC() || (HWIsNEC() && (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)))
    {
        if (DiskGetPhysicalInfo (&stDiskRec))
        {
            return ( CBSTATUS_ERROR );
        }

        *lpbySides    = (BYTE)stDiskRec.dwTotalHeads;
        *lpwCylinders = (WORD)stDiskRec.dwTotalTracks;
        *lpbySectors  = (BYTE)stDiskRec.dwSectorsPerTrack;
    }
    else                                // this IS a NEC PC98xx
    {                                   // NOTE: floppy disk is assumed!
        BPBBootRec *lpBPBBootRec;
        // Increasing this buffer to 4KB caused stack checking (__chkstk) in IRUK platform
        // which caused an unresolved external error when building NAVAP.SYS. Since this is
        // for NEC floppy, 2KB will suffice.
        BYTE       BootSector[4 * SECTOR_SIZE];

        stDiskRec.dwHead     = 0;
        stDiskRec.dwTrack    = 0;
        stDiskRec.dwSector   = 1;
        stDiskRec.numSectors = 1;
        stDiskRec.buffer     = (HPBYTE)&BootSector;

                                        // Read the Boot record

        if (DiskAbsOperation(READ_COMMAND, &stDiskRec))
        {
            return ( CBSTATUS_ERROR );
        }

        lpBPBBootRec = (BPBBootRec *)&BootSector;

                                        // Determine the disk dimensions
                                        // from the floppy type

        switch (lpBPBBootRec->BPB.mediaDescriptor)
        {
            case 0xFD:
                                        // 360k diskette
                *lpwCylinders = 40;
                *lpbySectors  = 9;
                *lpbySides    = 2;

                break;

            case 0xF9:
                                        // 720k or 1.2M diskette
                *lpwCylinders = 80;
                *lpbySectors  = (BYTE)lpBPBBootRec->BPB.sectorsPerTrack;
                *lpbySides    = 2;

                break;

            case 0xF0:
                                        // 1.44M or 2.88M diskette
                *lpwCylinders = 80;
                *lpbySectors  = (BYTE)lpBPBBootRec->BPB.sectorsPerTrack;
                *lpbySides    = 2;

                break;

            case 0xFE:
                                        // 1.2M (1k/sector) diskette
                *lpwCylinders = 80;
                *lpbySectors  = 8;
                *lpbySides    = 2;

                break;

            case 0xFC:
                                        // 180K diskette
                *lpwCylinders = 40;
                *lpbySectors  = 9;
                *lpbySides    = 1;

                break;

            case 0xFF:
                                        // 320K diskette
                *lpwCylinders = 40;
                *lpbySectors  = 8;
                *lpbySides    = 2;

                break;

            case 0xFB:
                                        // 640K diskette
                *lpwCylinders = 80;
                *lpbySectors  = 8;
                *lpbySides    = 2;

                break;

            default:

                return ( CBSTATUS_ERROR );
        }
    }

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
    if (HWIsNEC())
    {
        *lpbPartitioned = FALSE;
        return CBSTATUS_OK;
    }

    // if this is a floppy drive, it definitely is not partitioned media.

    if ((BYTE)lpvInfo < 0x80)
    {
        *lpbPartitioned = FALSE;
        return CBSTATUS_OK;
    }

    // we may still be not be partitionable even though we are greater
    // than drive 0x80 - but we must be careful before assuming
    // we're not paritionable at this point.

    // NOTE: if we err in the function, it should err on the side of
    // assuming paritionable media.  This function always gets called when
    // repairing boot records.  If we assume that the boot record is
    // at physical sector zero, when an MBR is really threre, we will
    // likely corrupt the MBR and render the user's hard drive unusable

    // allocate a buffer

    LPVOID lpMBRBuffer = MemAllocPtr(GHND, 8 * PHYSICAL_SECTOR_SIZE);

    if (!lpMBRBuffer)
    {
        return CBSTATUS_ERROR;
    }

    // read the first sector of the drive

    if (NAVEXSDiskReadPhysical(lpMBRBuffer, 1, 0, 1, 0, (BYTE)lpvInfo))
    {
        MemFreePtr(lpMBRBuffer);
        return CBSTATUS_ERROR;
    }

    //  A invalid BPB means that we assume we are partitionable
    //  since this is is not a boot reocord at physical sector zero

    FAT32BPBREC BPB;

    //  Get the possible BPB

    BPB = *((FAT32BPBREC *)((LPBYTE)lpMBRBuffer + 11));

    if (!DiskIsValidBPB((LPBPBREC)&BPB))
    {
        *lpbPartitioned = TRUE;
        MemFreePtr(lpMBRBuffer);

        return CBSTATUS_OK;
    }

    // this looks like a boot record at this point - but we want to be
    // absolutely sure this is not an MBR before setting *lpbParitioned to FALSE.
    // Remember that the Intruder and Newboot viruses setup valid BPB records
    // in the MBR of a hard drive.

    // if we can be assured that this does not have a valid partition table
    // we'll allow a FALSE value in *lpbParitioned

    // look for overlapping partitions

    LPPARTENTRYREC lpPartEntryRecI;
    LPPARTENTRYREC lpPartEntryRecJ;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == j)
            {
                continue;
            }

            lpPartEntryRecI = (LPPARTENTRYREC)((LPSTR)lpMBRBuffer + PART_TABLE_OFFSET) + i;
            lpPartEntryRecJ = (LPPARTENTRYREC)((LPSTR)lpMBRBuffer + PART_TABLE_OFFSET) + j;

            // only check for overlapping partitions on used entries

            if (lpPartEntryRecI->system == SYSTEM_NOT_USED || lpPartEntryRecJ->system == SYSTEM_NOT_USED)
            {
                continue;
            }

            if ((lpPartEntryRecJ->relativeSectors >= lpPartEntryRecI->relativeSectors) &&
                (lpPartEntryRecJ->relativeSectors < lpPartEntryRecI->relativeSectors +
                                                     lpPartEntryRecI->numSectors))
            {
                // woops - we found an overlapping partition.  This is not
                // a valid partition table

                *lpbPartitioned = FALSE;
                MemFreePtr(lpMBRBuffer);

                return CBSTATUS_OK;
            }
        }
    }

    // no overlapping partitions
    // assume *lpbPartitoned is TRUE at this point until proven FALSE

    *lpbPartitioned = TRUE;

    WORD  wActivePartitions  = 0;
    WORD  wUsedPartitions    = 0;

    LPPARTENTRYREC lpPartEntryRec;
    for (int index = 0; index < 4; ++index)
    {
        lpPartEntryRec = (LPPARTENTRYREC)((LPSTR)lpMBRBuffer + PART_TABLE_OFFSET) + index;

        if (lpPartEntryRec->system == SYSTEM_NOT_USED)
        {
            continue;
        }

        ++wUsedPartitions;

        // it is not valid to have more that one active partition

        if (lpPartEntryRec->bootable && (++wActivePartitions > 1))
        {
            *lpbPartitioned = FALSE;
            break;
        }
    }

    // we must have at least one used parition to be valid

    if (!wUsedPartitions)
    {
        *lpbPartitioned = FALSE;
    }

    MemFreePtr(lpMBRBuffer);

    return CBSTATUS_OK;
}

#endif    // #if !defined(SYM_NLM) && !defined(SYM_UNIX)

// Mutex

typedef struct tagMUTEX_INFO
{
    DWORD           dwReserved;

#if defined(SYM_WIN32)

    HANDLE          hMutex;

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SYMMUTEX        stMutex;

#elif defined(SYM_UNIX)

    pthread_mutex_t mutex;

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
	return ( CBSTATUS_OK );

#if defined(SYM_WIN32)

    lpstMutexInfo->hMutex = CreateMutex(NULL,   // Default security attributes
					FALSE,  // No initial owner
					NULL);  // Don't care about its name

    if (lpstMutexInfo->hMutex == NULL)
    {
	MemFreePtr(lpstMutexInfo);
	return ( CBSTATUS_ERROR );
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    if (!SymCreateMutex(&lpstMutexInfo->stMutex))
    {
	MemFreePtr(lpstMutexInfo);
	return ( CBSTATUS_ERROR );
    }

#elif defined(SYM_UNIX)

    pthread_mutex_init(&lpstMutexInfo->mutex, NULL);

#endif

    *lplpvMutexInfo = (LPVOID)lpstMutexInfo;

    return ( CBSTATUS_OK );
}

CBSTATUS NAVMutexDestroy
(
    LPVOID          lpvMutexInfo
)
{
    CBSTATUS        cbStatus;
    LPMUTEX_INFO    lpstMutexInfo;

    cbStatus = CBSTATUS_OK;
    lpstMutexInfo = (LPMUTEX_INFO)lpvMutexInfo;

#if defined(SYM_WIN32)

    if (CloseHandle(lpstMutexInfo->hMutex) == FALSE)
    {
	// Failed to close mutex

	cbStatus = CBSTATUS_ERROR;
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SymDestroyMutex (&lpstMutexInfo->stMutex);

#elif defined(SYM_UNIX)

    pthread_mutex_destroy(&lpstMutexInfo->mutex);

#endif

    MemFreePtr(lpvMutexInfo);
    return ( CBSTATUS_OK );
}

CBSTATUS NAVMutexWait
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
    LPMUTEX_INFO    lpstMutexInfo;

    lpstMutexInfo = (LPMUTEX_INFO)lpvMutexInfo;

#if defined(SYM_WIN32)

    if (WaitForSingleObject(lpstMutexInfo->hMutex,
			    dwTimeoutMS) != WAIT_OBJECT_0)
    {
	// Failure getting mutex

	return ( CBSTATUS_ERROR );
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SymWaitForMutex (&lpstMutexInfo->stMutex);

#elif defined(SYM_NLM)

    EnterCritSec();

#elif defined(SYM_UNIX)

    pthread_mutex_lock(&lpstMutexInfo->mutex);

#endif

    return ( CBSTATUS_OK );
}

CBSTATUS NAVMutexRelease
(
    LPVOID          lpvMutexInfo
)
{
    LPMUTEX_INFO    lpstMutexInfo;

    lpstMutexInfo = (LPMUTEX_INFO)lpvMutexInfo;

#if defined(SYM_WIN32)

    if (ReleaseMutex(lpstMutexInfo->hMutex) == FALSE)
    {
	// Failure releasing mutex

	return ( CBSTATUS_ERROR );
    }

#elif defined(SYM_VXD) || defined(SYM_NTK)

    SymReleaseMutex (&pstMutexInfo->stMutex);

#elif defined(SYM_NLM)

    ExitCritSec();

#elif defined(SYM_UNIX)

    pthread_mutex_unlock(&lpstMutexInfo->mutex);

#endif

    return ( CBSTATUS_OK );
}

// Semaphore

CBSTATUS NAVSemaphoreCreate
(
    LPLPVOID        lplpvSemaphoreInfo,
    DWORD           dwInitialCount,
    DWORD           dwMaximumCount
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVSemaphoreDestroy
(
    LPVOID          lpvSemaphoreInfo
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVSemaphoreWait
(
    LPVOID          lpvSemaphoreInfo,
    DWORD           dwTimeoutMS
)
{
    return ( CBSTATUS_ERROR );
}

CBSTATUS NAVSemaphoreRelease
(
    LPVOID          lpvSemaphoreInfo,
    LPDWORD         lpdwPrevCount
)
{
    return ( CBSTATUS_ERROR );
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
#if !defined(SYM_NLM) && !defined(SYM_UNIX)
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

//=========================================================================//
//                                                                         //
//      Default File IO Callbacks                                          //
//                                                                         //
//=========================================================================//

BOOL NAVCALLBACK NAVDefaultFileIoOpen (LPVOID lpvFileInfo, DWORD dwOpenMode,
                                       LPVOID FAR *lplpvHandle)
{
    return VirtFileOpen ((LPTSTR)lpvFileInfo, (WORD)dwOpenMode, (HFILE FAR*)lplpvHandle);
}

BOOL NAVCALLBACK NAVDefaultFileIoClose (LPVOID lpvHandle)
{
    return (VirtFileClose ((HFILE)lpvHandle) != HFILE_ERROR);
}

DWORD NAVCALLBACK NAVDefaultFileIoSeek (LPVOID lpvHandle, LONG lOffset, int nFrom)
{
    return VirtFileSeek ((HFILE)lpvHandle, lOffset, nFrom);
}

UINT NAVCALLBACK NAVDefaultFileIoRead (LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToRead)
{
    return VirtFileRead ((HFILE)lpvHandle, lpvBuffer, uBytesToRead);
}

UINT NAVCALLBACK NAVDefaultFileIoWrite (LPVOID lpvHandle, LPVOID lpvBuffer, UINT uBytesToWrite)
{
    return VirtFileWrite ((HFILE)lpvHandle, lpvBuffer, uBytesToWrite);
}

BOOL NAVCALLBACK NAVDefaultFileIoDelete (LPVOID lpvFileInfo)
{
    return VirtFileDelete ((LPTSTR) lpvFileInfo);
}

BOOL NAVCALLBACK NAVDefaultFileIoGetDateTime (LPVOID lpvHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
    return VirtFileGetDateTime ((HFILE)lpvHandle, uType, lpwDate, lpwTime);
}

BOOL NAVCALLBACK NAVDefaultFileIoSetDateTime (LPVOID lpvHandle, UINT uType, WORD wDate, WORD wTime)
{
    return VirtFileSetDateTime ((HFILE)lpvHandle, uType, wDate, wTime);
}

NAVFILEIO g_stDefaultFileIoCallbacks =
{
    NAVDefaultFileIoOpen,
    NAVDefaultFileIoClose,
    NAVDefaultFileIoSeek,
    NAVDefaultFileIoRead,
    NAVDefaultFileIoWrite,
    NAVDefaultFileIoDelete,
    NAVDefaultFileIoGetDateTime,
    NAVDefaultFileIoSetDateTime,
};


//=========================================================================//
//                                                                         //
//      NAVEX Callbacks - taken from the old avglob.cpp in Certlibn        //
//                                                                         //
//=========================================================================//

#if !defined(SYM_NLM) && !defined(SYM_UNIX)

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

#if !defined(SYM_VXD)

    if ( HWIsNEC() && SystemGetWindowsType() != SYM_SYSTEM_WIN_NT)
        {
            // don't call DiskGetPhysicalInfo on NEC WIN95
        }
    else
        {
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

            stDiskRec.dwHead      = bHead;
            stDiskRec.dwTrack     = wCylinder;
            stDiskRec.dwSector    = bSector;
            stDiskRec.numSectors  = bCount;

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

        // Is this partition currently mapped to a drive letter?
        // Note that NAVEX_NTDetermineDriveLetter will return 0 for
        // floppy drives. In which case, floppy writes will occur through
        // the DiskAbsOperation below.
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

#if !defined(SYM_VXD)

    if ( HWIsNEC() && (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT))
        {
            ;// don't call DiskGetPhysicalInfo on NEC WIN95
        }
    else
        {
        byResult = DiskGetPhysicalInfo(&stDiskRec);

                                        // These MUST be filled in for
                                        // W32 DiskAbsOperation() to calculate
                                        // the correct offset.
        if (   (0 == stDiskRec.dwSectorsPerTrack)
            || (0 == stDiskRec.dwTotalTracks)
            || (0 == stDiskRec.dwTotalHeads) )
            {
            return ( 1 );
            }
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

    stDiskRec.dwHead       = bHead;
    stDiskRec.dwTrack      = wCylinder;
    stDiskRec.dwSector     = bSector;
    stDiskRec.numSectors   = bCount;
    stDiskRec.buffer       = (BYTE huge *) lpBuffer;

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



#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
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
    BYTE    abyMBR [ 8 * SECTOR_SIZE ];

    ABSDISKREC  stDiskRec = { 0 };

    stDiskRec.dn = byDrive;

    byResult = DiskGetPhysicalInfo(&stDiskRec);

    if (    (0 == stDiskRec.dwSectorsPerTrack)
         || (0 == stDiskRec.dwTotalTracks)
         || (0 == stDiskRec.dwTotalHeads) )
        {
        return ( MAX_PARTITIONS );
        }

    dwLinearOffset = (((DWORD) wCylinder *
                       stDiskRec.dwTotalHeads *
                       stDiskRec.dwSectorsPerTrack ) +
                      ((DWORD) byHead * stDiskRec.dwSectorsPerTrack ) +
                      (bySector - 1));

    // Read MBR for this physical disk

    stDiskRec.dwHead      = 0;
    stDiskRec.dwTrack     = 0;
    stDiskRec.dwSector    = 1;
    stDiskRec.numSectors  = 1;
    stDiskRec.buffer      = (BYTE huge *) abyMBR;

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
        WORD        wSectorSize = 128 << stDiskRec.sectorLenCode;

        dwLinearOffset -= *((LPDWORD) ( abyMBR + wMBROffset + 8 ));
        dwLinearOffset *= wSectorSize;
        *lpdwLinearOffset = dwLinearOffset;
        }

    return ( byPartNum );
}


//--------------------------------------------------------------------
// NAVEX_NTDetermineDriveLetterWin2000Plus()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
// It only works for hard drives. This function works only on
// Windows 2000 and post-Windows 2000 systems.
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDetermineDriveLetterWin2000Plus (BYTE    byDrive,
                                              BYTE    byPartNum)
{
    // Translate our input parameters a little:
    // 1. Translate the physical drive number to an NT based device number.
    //    Generatally, byDrive could by 0x80 and the NT device number is 0.
    // 2. Translate the specified partition number to a one based
    //    partition number as specified in the STORAGE_DEVICE_NUMBER struct.
    const BYTE    byNeedNtDeviceNum    = byDrive - FIRST_HD;
    const BYTE    byNeedStoragePartNum = byPartNum + 1;

    const TCHAR   c_szDeviceHarddrive[] = "\\Device\\Harddisk";
    const DWORD   c_dwDrivesChars       = 256;
    auto  TCHAR   szzDrives[c_dwDrivesChars+1];
    auto  LPTSTR  lpszCurDrive          = szzDrives;
    auto  BYTE    byFoundDriveLtr       = 0 /*failure*/;

    // This function does not work when byDrive is less than FIRST_HD.
    // See byNeedNtDeviceNum variable initialization.
    if (byDrive < FIRST_HD)
        goto FuncEnd;

    // Get all of the possible logical drive letters.
    if (0 == GetLogicalDriveStrings (c_dwDrivesChars, szzDrives))
        goto FuncEnd;

    // Walk through all of the logical drives. For each one, determine
    // if the physical drive number and partition number match the
    // caller's.
    while (byFoundDriveLtr == 0 && lpszCurDrive[0] != _T('\0'))
    {
        const DWORD    c_dwDIOCBufferChars   = SYM_MAX_PATH*2;
        auto  TCHAR    szDrive[16];
        auto  TCHAR    szDIOCBuffer[c_dwDIOCBufferChars];
        auto  BOOL     bSuccess;

        // Copy only the drive specification. Eg. "C:".
        szDrive[0] = lpszCurDrive[0];
        szDrive[1] = lpszCurDrive[1];
        szDrive[2] = '\0';

        // First query for a DOS name for this device.
        bSuccess = (0 != QueryDosDevice (szDrive, szDIOCBuffer, c_dwDIOCBufferChars));

        // Only process devices that are hard drives.
        if (0 == STRNICMP (c_szDeviceHarddrive, szDIOCBuffer, sizeof (c_szDeviceHarddrive)-1))
        {
            auto  TCHAR                 szDevice[16];
            auto  DWORD                 dwBytesReturned;
            auto  STORAGE_DEVICE_NUMBER stStorageDeviceInfo;
            auto  HANDLE                hDevice;

            SPRINTF (szDevice, "\\\\.\\%c:", lpszCurDrive[0]);

            // Open the device for query.
            hDevice = CreateFile (szDevice, 0 /*Device Query Only*/,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            // Fail out of this function when the CreateFile fails.
            if (hDevice == INVALID_HANDLE_VALUE)
                goto FuncEnd;

            // Attempt to read the storage information.
            bSuccess = DeviceIoControl (hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER,
                                        NULL, 0, &stStorageDeviceInfo,
                                        sizeof (stStorageDeviceInfo), &dwBytesReturned,
                                        NULL);

            // Only attempt to match when we where able to get the storage
            // information.
            if (bSuccess)
            {
                // This this logical drive has a matching physical device number
                // and partition number, we have found the logical drive the
                // caller is looking for.
                if (stStorageDeviceInfo.DeviceNumber    == byNeedNtDeviceNum &&
                    stStorageDeviceInfo.PartitionNumber == byNeedStoragePartNum)
                {
                    byFoundDriveLtr = lpszCurDrive[0];
                }
            }

            // Remember to close our drive handle.
            CloseHandle (hDevice);
            hDevice = NULL;
        }

        // Skip to the next drive listing in this SZZ string.
        lpszCurDrive += STRLEN (lpszCurDrive) + 1;
    }

FuncEnd:
    return byFoundDriveLtr;
}

//--------------------------------------------------------------------
// NAVEX_NTDetermineDriveLetterPreWin2000()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
// It only works for hard drives. This function works only on
// pre-Windows 2000 systems.
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE NAVEX_NTDetermineDriveLetterPreWin2000
(
    BYTE    byDrive,
    BYTE    byPartNum
)
{
    const DWORD   c_dwDrivesChars       = 256;
    auto  TCHAR   szzDrives[c_dwDrivesChars+1];
    auto  TCHAR   szThisDevice[SYM_MAX_PATH];
    auto  BYTE    byResult = 0;
    auto  BYTE    abyDIOCBuffer[ SYM_MAX_PATH * 2 ];
    auto  BOOL    bDriveLetterFound = FALSE;
    auto  LPTSTR  lptstrDevice;
    auto  TCHAR   szDrive[8];

    // This function does not work when byDrive is less than FIRST_HD.
    // See SPRINTF below.
    if (byDrive < FIRST_HD)
        return 0;

    // create the string we're trying to match
    SPRINTF (szThisDevice, "\\Device\\Harddisk%c\\Partition%c",
             '0' + (byDrive - FIRST_HD), '0' + byPartNum + 1);

    // Get all of the possible logical drive letters.
    if (0 == GetLogicalDriveStrings (c_dwDrivesChars, szzDrives))
        return 0;

    lptstrDevice = szzDrives;

    szDrive[1] = ':';
    szDrive[2] = '\0';

    while (lptstrDevice[0] != 0)
    {
        if (lptstrDevice[1] == ':')
        {
            // Copy the drive letter to szDrive.
            szDrive[0] = lptstrDevice[0];

            if (0 != QueryDosDevice (szDrive, (TCHAR*) abyDIOCBuffer, SYM_MAX_PATH*2) &&
                !STRICMP ((TCHAR*) abyDIOCBuffer, szThisDevice))
            {
                bDriveLetterFound = TRUE;
                break;
            }
        }
        lptstrDevice += STRLEN( lptstrDevice ) + 1;
    }

    if (TRUE == bDriveLetterFound)
    {
        byResult = (BYTE)lptstrDevice[0];
    }

    return byResult;
}

//--------------------------------------------------------------------
// NAVEX_NTDetermineDriveLetter()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
// It only works for hard drives.
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
    OSVERSIONINFO   stVersion;
    BOOL            bSuccess;
    BYTE            byResult = 0;

    memset (&stVersion, 0, sizeof (stVersion));
    stVersion.dwOSVersionInfoSize = sizeof(stVersion);

    bSuccess = GetVersionEx (&stVersion);
    if (!bSuccess)
        goto FuncEnd;

    // This function only works for NT based OSes.
    SYM_ASSERT (stVersion.dwPlatformId == VER_PLATFORM_WIN32_NT);

    // Windows 2000 does its drive letter mapping differently than
    // Windows NT 4. Call the Windows 2000 version when we are on a
    // Windows 2000 or newer OS. In all other cases, we revert to the
    // pre-Windows 2000 method of mapping. This works for NT4.
    if (stVersion.dwMajorVersion >= 5)
        byResult = NAVEX_NTDetermineDriveLetterWin2000Plus (byDrive, byPartNum);
    else
        byResult = NAVEX_NTDetermineDriveLetterPreWin2000 (byDrive, byPartNum);

FuncEnd:
    return byResult;
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

#endif // SYM_WIN32 && !SYM_VXD && !SYM_NTK

#endif // if !defined(SYM_NLM) && !defined(SYM_UNIX)


HFILE DOS_LOADDS WINAPI NAVEXFileOpen
(
    LPTSTR  lpName,
    UINT    uOpenMode
)
{
    auto   BOOL   bSuccess;
    auto   HFILE  hFile;

    SYM_ASSERT( lpName );

    bSuccess = VirtFileOpen( lpName, uOpenMode, &hFile );
    if (bSuccess)
        return hFile;
    else
        return HFILE_ERROR;
}

HFILE DOS_LOADDS WINAPI NAVEXFileClose
(
    HFILE   hHandle
)
{
    SYM_ASSERT( hHandle );

    return ( VirtFileClose( hHandle ) );
}

UINT DOS_LOADDS WINAPI NAVEXFileRead
(
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    SYM_ASSERT( lpBuffer );
    SYM_ASSERT( uBytes );
    SYM_ASSERT( hHandle );

    return ( VirtFileRead( hHandle, lpBuffer, uBytes ) );
}

UINT DOS_LOADDS WINAPI NAVEXFileWrite
(
    HFILE   hHandle,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    SYM_ASSERT( lpBuffer || (0 == uBytes) );
    SYM_ASSERT( hHandle );

    return ( VirtFileWrite( hHandle, lpBuffer, uBytes ) );
}

DWORD DOS_LOADDS WINAPI NAVEXFileSeek
(
    HFILE   hHandle,
    LONG    lOffset,
    int     nFlag
)
{
    SYM_ASSERT( hHandle );

    return ( VirtFileSeek( hHandle, lOffset, nFlag ) );
}

DWORD DOS_LOADDS WINAPI NAVEXFileLength
(
    HFILE   hHandle
)
{
    return( VirtFileLength( hHandle ) );
}

BOOL DOS_LOADDS WINAPI NAVEXFileGetDateTime
(
    HFILE   hHandle,
    UINT    uType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
    return ( VirtFileGetDateTime( hHandle, uType, lpwDate, lpwTime ) );
}

BOOL DOS_LOADDS WINAPI NAVEXFileSetDateTime
(
    HFILE   hHandle,
    UINT    uType,
    WORD    wDate,
    WORD    wTime
)
{
    return ( VirtFileSetDateTime( hHandle, uType, wDate, wTime ) );
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
    SYM_ASSERT( lpFilename );

    return ( VirtFileDelete( lpFilename ) );
}

UINT DOS_LOADDS WINAPI NAVEXFileGetAttr
(
    LPTSTR  lpFilename,
    LPUINT  lpuAttr
)
{
    SYM_ASSERT( lpFilename );

    return ( VirtFileGetAttr( lpFilename, lpuAttr ) );
}

UINT DOS_LOADDS WINAPI NAVEXFileSetAttr
(
    LPTSTR  lpFilename,
    UINT    uAttr )
{
    SYM_ASSERT ( lpFilename );

    return ( VirtFileSetAttr( lpFilename, uAttr ) );
}

HFILE DOS_LOADDS WINAPI NAVEXFileCreate
(
    LPTSTR  lpFilename,
    UINT    uAttr
)
{
    SYM_ASSERT( lpFilename );

    return ( VirtFileCreate( lpFilename, uAttr ) );
}

LPVOID DOS_LOADDS WINAPI NAVEXTempMemoryAlloc
(
    DWORD    dwSize
)
{
#if defined(SYM_WIN16)
    // We use malloc() instead of MemAllocPtr() because
    // this function is used to allocate many blocks simultaneously
    // for the virus defintion table, and if we use a selector
    // for each block we will run out.  malloc() will allocate
    // a few large blocks using selectors and then suballocate from them.
    // (This might fix similar problems on the DX platform also.)
    // The block must be initialize to zero to match the behavior of
    // MemAllocPtr() with the GHND flags.
    LPVOID p = malloc(dwSize);
    if (p != NULL)
        MEMSET(p, 0, dwSize);
    return p;
#else
    return ( MemAllocPtr( GHND, dwSize ) );
#endif
}

VOID DOS_LOADDS WINAPI NAVEXTempMemoryFree
(
    LPVOID  lpBuffer
)
{
#if defined(SYM_WIN16)
    free(lpBuffer);
#else
    MemFreePtr( lpBuffer );
#endif
}

LPVOID DOS_LOADDS WINAPI NAVEXPermMemoryAlloc
(
    DWORD   dwSize
)
{
#if defined(SYM_WIN16)
    // The function used to allocate memory here must be the same
    // as the one used in NAVEXTempMemoryAlloc() because there are
    // currently some "unbalanced" calls where pointers allocated
    // with one function are freed by the other function's deallocator.
    // The block must be initialize to zero to match the behavior of
    // MemAllocPtr() with the GHND flags.
    LPVOID p = malloc(dwSize);
    if (p != NULL)
        MEMSET(p, 0, dwSize);
    return p;
#else
    return ( MemAllocPtr( GHND, dwSize ) );
#endif
}

VOID DOS_LOADDS WINAPI NAVEXPermMemoryFree
(
    LPVOID  lpBuffer
)
{
#if defined(SYM_WIN16)
    free(lpBuffer);
#else
    MemFreePtr( lpBuffer );
#endif
}

UINT DOS_LOADDS WINAPI NAVEXCreateMutex
(
    LPLPVOID        lplpvMutexInfo
)
{
    return ( NAVMutexCreate( lplpvMutexInfo ) == CBSTATUS_OK );
}

UINT DOS_LOADDS WINAPI NAVEXDestroyMutex
(
    LPVOID          lpvMutexInfo
)
{
    return ( NAVMutexDestroy( lpvMutexInfo ) == CBSTATUS_OK );
}

UINT DOS_LOADDS WINAPI NAVEXWaitMutex
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
    return ( NAVMutexWait( lpvMutexInfo, dwTimeoutMS ) == CBSTATUS_OK );
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
    return ( VirtGetTempFile( lpszFileName, lpvCookie ) );
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
    return (NAVEX_UI_TIMEOUT);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxInit
(
    LPTSTR   lpszTitle,
    DWORD    dwButtonFlags,
    LPLPVOID lplpvInfoBoxInfo,
    LPVOID   lpvCookie
)
{
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
    return (NAVEX_UI_OK);
}

UINT DOS_LOADDS WINAPI NAVEXInfoBoxClose
(
    LPVOID lpvInfoBoxInfo,
    LPVOID lpvCookie
)
{
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
    return (NAVEX_UI_TIMEOUT);
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

#if defined(SYM_DOSX) || defined (SYM_WIN)

    CFGTEXTREC rSwitch = {lpszID, CFG_SW_EXACT,};
    TCHAR szLevel[2];

    *lpbImplemented = TRUE;

    if (ConfigSwitchSet(&rSwitch, NULL))
    {
        ConfigSwitchGetText(&rSwitch, (LPSTR)lpvResult, uMaxResultLen);
    }
    else
    {
        szLevel[0] = (TCHAR) (s_wHeuristicLevel + '0');
        szLevel[1] = EOS;
        STRNCPY((LPTSTR)lpvResult, szLevel, uMaxResultLen);
    }

#elif defined (SYM_NLM)

    TCHAR szLevel[2];

    // ##IPE Do Not translate
    if( !(STRCMP( lpszID, "heur" )) )
    {
        szLevel[0] = s_wHeuristicLevel + '0';
        szLevel[1] = EOS;
        STRNCPY(lpvResult, szLevel, uMaxResultLen);
        *lpbImplemented = TRUE;
    }
    else
    {
        *lpbImplemented = FALSE;
        lpvResult = NULL;
    }

#elif 0 // OLD NLM CODE

    EXTERN BYTE byHeuristicLevel[];

    // ##IPE Do Not translate
    if( !(STRCMP( lpszID, "heur" )) )
        {
        *lpbImplemented = TRUE;
        STRCPY( lpvResult, &byHeuristicLevel );
        }
    else
        {
        *lpbImplemented = FALSE;
        lpvResult = NULL;
        }
#else

    *lpbImplemented = FALSE;

#endif
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
#if !defined(SYM_NLM) && !defined(SYM_UNIX)
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

//#if defined(SYM_DOSX) || defined (SYM_NLM)
//EXTERNC
//#endif
AVGLOBALCALLBACKS gstNAVCallBacks =
{
    &gstNAVGeneralCallBacks,
    &gstNAVProgressCallBacks,
    &gstNAVDataFileCallBacks,
    &gstNAVIPCCallBacks,
    &gstNAVMemoryCallBacks,
    &gstNAVEXCallBacks,
};


// In Roswell, AreDefsWild() is defined in NAVC, SEARCH.CPP.
// It returns TRUE if the normal def files (VIRSCAN1.DAT etc.) were NOT
// found in the defs directory at the time when the location of the
// defs directory was determined.
// For this project, we just stub out this function.
// Perhaps this should be a flag passed to NAVEngineInit()?

#ifdef SYM_DOS
BOOL PASCAL AreDefsWild(VOID)
{
    return FALSE;
}
#endif
