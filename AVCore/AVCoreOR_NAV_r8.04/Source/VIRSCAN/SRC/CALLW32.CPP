#ifdef SYM_WIN32

#include "platform.h"
#include "disk.h"
#include "file.h"
#include "callbk15.h"
#include "xapi.h"
#include "symcfg.h"
#include "winioctl.h"
#include "pamapi.h"
#include "callw32.h"

#define MAX_PARTITIONS       4

BYTE NAVEX32_NTDeterminePartNum
(
    BYTE    byDrive,
    BYTE    byHead,
    WORD    wCylinder,
    BYTE    bySector,
    BYTE    bySectors,
    LPDWORD lpdwLinearOffset
);

BYTE NAVEX32_NTDetermineDriveLetter
(
    BYTE    byDrive,
    BYTE    byPartNum
);

BYTE NAVEX32_NTDiskWriteLogical
(
    BYTE    byDriveLetter,
    DWORD   dwLinearOffset,
    LPVOID  lpBuffer,
    LPDWORD lpdwSectors
);

//--------------------------------------------------------------------------
//
//  These CALLBACKREV2 functions are being temporarily declared here for
//  the WIN32 platform.  The UI callbacks need a resource which must 
//  be reside in a DLL (AVAPI1 makes a LIB).  A callback project will 
//  soon be created which replace these CALLBACKREV2 functions.
//
//--------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI ScanLockUnlockDiskP (
    UINT    uOperation,                 // [in] 0 - lock, 1 - unlock
    BYTE    byDrive,                    // [in] drive to lock/unlock
    UINT    uRequestedAccess            // [in] requested access right
    );
                                        // local defines
#define LOCK_DRIVE          0
#define UNLOCK_DRIVE        1

#define DISK_READ_WRITE     1
#define DISK_READ_ONLY      2


//--------------------------------------------------------------------
//--------------------------------------------------------------------
UINT SYM_EXPORT WINAPI NAVEX32SDiskReadPhysical(
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
    if (    (0 == stDiskRec.sectorsPerTrack)
         || (0 == stDiskRec.totalTracks)
         || (0 == stDiskRec.totalHeads) )
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

            stDiskRec.head          = bHead;
            stDiskRec.track         = wCylinder;
            stDiskRec.sector        = bSector;
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
    stDiskRec.head          = bHead;
    stDiskRec.track         = wCylinder;
    stDiskRec.sector        = bSector;
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

UINT SYM_EXPORT WINAPI NAVEX32SDiskWritePhysical (
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
        byPartNum = NAVEX32_NTDeterminePartNum( bDrive,
                                             bHead,
                                             wCylinder,
                                             bSector,
                                             bCount,
                                             &dwLinearOffset );

                                        // Is this partition currently mapped
                                        // to a drive letter?
         if ( MAX_PARTITIONS != byPartNum &&
              0 != ( byDriveLetter = NAVEX32_NTDetermineDriveLetter( bDrive,
                                                                  byPartNum )))
            {
                dwBytes = bCount * SECTOR_SIZE;

                                        // Write to the logical drive
                return ( NAVEX32_NTDiskWriteLogical ( byDriveLetter,
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
    if (    (0 == stDiskRec.sectorsPerTrack)
         || (0 == stDiskRec.totalTracks)
         || (0 == stDiskRec.totalHeads) )
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

    stDiskRec.head          = bHead;
    stDiskRec.track         = wCylinder;
    stDiskRec.sector        = bSector;
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



#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
                                        // These functions are helpers for
                                        // LocalDiskWritePhysical under NT

//--------------------------------------------------------------------
// NAVEX32_NTDeterminePartNum()
//
// Given a series of physical sectors (drive number, starting coordinates,
// and a sector count) this function returns the partition number (0-3) in
// which the specified sectors reside.
//
// If these sectors lies outside of all partitions, stradle a partition
// boundary, or if an error is encountered while determining the partition
// number, MAX_PARTITIONS (4) is returned.
//--------------------------------------------------------------------
BYTE NAVEX32_NTDeterminePartNum
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

    if (    (0 == stDiskRec.sectorsPerTrack)
         || (0 == stDiskRec.totalTracks)
         || (0 == stDiskRec.totalHeads) )
        {
        return ( MAX_PARTITIONS );
        }

    dwLinearOffset = (((DWORD) wCylinder *
                       stDiskRec.totalHeads *
                       stDiskRec.sectorsPerTrack ) +
                      ((DWORD) byHead * stDiskRec.sectorsPerTrack ) +
                      (bySector - 1));

    // Read MBR for this physical disk

    stDiskRec.head          = 0;
    stDiskRec.track         = 0;
    stDiskRec.sector        = 1;
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
// NAVEX32_NTDetermineDriveLetter()
//
// Given a drive and partition number, this function returns a drive
// letter ('C', 'D', etc.) that is currently mapped to this partition.
//
// If there is no drive letter mapped to this partition, or an error
// is encountered while determining the drive letter, 0 is returned.
//--------------------------------------------------------------------
BYTE NAVEX32_NTDetermineDriveLetter
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
// NAVEX32_NTDiskWriteLogical()
//
// Given a drive letter, a linear byte offset, some data, and a length,
// this function writes length bytes of the data to the logical drive
// starting at the linear byte offset.  The return values are identical
// to LocalDiskWritePhysical():
//
// 0 == success
// 1 == failure
//--------------------------------------------------------------------
BYTE NAVEX32_NTDiskWriteLogical
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


HFILE SYM_EXPORT WINAPI NAVEX32FileOpen 
(
    LPTSTR  lpName,
    UINT    uOpenMode 
)
{
    SYM_ASSERT ( lpName );

    return (FileOpen(lpName, uOpenMode));
}

HFILE SYM_EXPORT WINAPI NAVEX32FileClose 
(
    HFILE   hHandle 
)
{
    SYM_ASSERT ( hHandle );

    return (FileClose(hHandle));
}

UINT SYM_EXPORT WINAPI NAVEX32FileRead 
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

UINT SYM_EXPORT WINAPI NAVEX32FileWrite 
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

DWORD SYM_EXPORT WINAPI NAVEX32FileSeek 
(
    HFILE   hHandle,
    LONG    lOffset,
    int     nFlag 
)
{
    SYM_ASSERT ( hHandle );

    return (FileSeek(hHandle, lOffset, nFlag));
}

DWORD SYM_EXPORT WINAPI NAVEX32FileLength 
(
    HFILE   hHandle 
)
{
    return (FileLength(hHandle));
}

BOOL SYM_EXPORT WINAPI NAVEX32FileGetDateTime 
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

BOOL SYM_EXPORT WINAPI NAVEX32FileSetDateTime 
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

BOOL SYM_EXPORT WINAPI NAVEX32FileDelete 
(
    LPTSTR  lpFilename 
)
{
    SYM_ASSERT ( lpFilename );

    return (FileDelete(lpFilename) == NOERR ? TRUE : FALSE);
}

UINT SYM_EXPORT WINAPI NAVEX32FileGetAttr 
(
    LPTSTR  lpFilename,
    LPUINT  lpuAttr 
)
{
    SYM_ASSERT ( lpFilename );

    return (FileGetAttr(lpFilename, lpuAttr));
}

UINT SYM_EXPORT WINAPI NAVEX32FileSetAttr 
(
    LPTSTR  lpFilename,
    UINT    uAttr )
{
    SYM_ASSERT ( lpFilename );

    return (FileSetAttr(lpFilename, uAttr));
}

HFILE SYM_EXPORT WINAPI NAVEX32FileCreate 
(
    LPTSTR  lpFilename,
    UINT    uAttr 
)
{
    SYM_ASSERT ( lpFilename );

    return (FileCreate(lpFilename, uAttr));
}

LPVOID SYM_EXPORT WINAPI NAVEX32TempMemoryAlloc 
(
    DWORD    dwSize 
)
{
    return (MemAllocPtr(GHND, dwSize));
}

VOID SYM_EXPORT WINAPI NAVEX32TempMemoryFree 
(
    LPVOID  lpBuffer 
)
{
    MemFreePtr (lpBuffer);
}

LPVOID SYM_EXPORT WINAPI NAVEX32PermMemoryAlloc 
(
    DWORD   dwSize 
)
{
    return (MemAllocPtr(GHND, dwSize));
}

VOID SYM_EXPORT WINAPI NAVEX32PermMemoryFree 
(
    LPVOID  lpBuffer 
)
{
    MemFreePtr (lpBuffer);
}

UINT SYM_EXPORT  WINAPI NAVEX32CreateMutex
(
    LPLPVOID        lplpvMutexInfo
)
{
    return (TRUE);
}

UINT SYM_EXPORT  WINAPI NAVEX32DestroyMutex
(
    LPVOID          lpvMutexInfo
)
{
    return (TRUE);
}

UINT SYM_EXPORT  WINAPI NAVEX32WaitMutex
(
    LPVOID          lpvMutexInfo,
    DWORD           dwTimeoutMS
)
{
    return (TRUE);
}

UINT SYM_EXPORT  WINAPI NAVEX32ReleaseMutex
(
    LPVOID          lpvMutexInfo
)
{
    return (TRUE);
}

HFILE SYM_EXPORT  WINAPI NAVEX32GetTempFile
(
    LPTSTR          lpszFileName,
    LPVOID          lpvCookie
)
{

#if defined(SYM_DOS) || defined(SYM_WIN) 
 
    return (FileCreateTemporary(lpszFileName, FA_TEMPORARY));

#elif defined(SYM_VXD) || defined(SYM_NTK)

    auto   HFILE     hFile;    
    auto   TCHAR     szGeneratedFileName[13];    
    auto   TCHAR     szNum[5] = _T("0000");    
    auto   UINT      i,j, uNum;    

    STRCPY(lpszFileName, szNAVPath);    

    for (i = 0; i < 9999; i++)    
    {    
        for (j = STRLEN(szNum)-1, uNum = i; j >= 0 && uNum > 0; j--)    
        {    
            szNum[j] = (TCHAR)((uNum % 10) + '0');    
            uNum /= 10;    
        }    

        STRCPY(szGeneratedFileName, szNum);    
        STRCAT(szGeneratedFileName, _T("NAV~.TMP"));    
        NameAppendFile(lpszFileName, szGeneratedFileName);    

    //  hFile = RadCreateFile(lpszFileName);    
    //  if (hfile != HFILE_ERROR)    
    //  {    
    //      break;    
    //  }    
    }    

    return (hFile);

#endif

   return (HFILE_ERROR);
}

UINT SYM_EXPORT  WINAPI NAVEX32MessageBox
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

    return (AVAPIMessageBox32(dwBoxID,
                              lpszMessage,
                              dwButtonFlags,
                              lpdwResult,
                              dwMSTimeOut,
                              lpvCookie));
     

}

UINT SYM_EXPORT  WINAPI NAVEX32InfoBoxInit
(
    LPTSTR   lpszTitle, 
    DWORD    dwButtonFlags, 
    LPLPVOID lplpvInfoBoxInfo,
    LPVOID   lpvCookie
)
{
    if (!lpvCookie)
    {
        return (NAVEX_UI_TIMEOUT);
    }


    return (AVAPIInfoBoxInit32(lpszTitle,
                               dwButtonFlags,      
                               lplpvInfoBoxInfo,
                               lpvCookie));
}

UINT SYM_EXPORT  WINAPI NAVEX32InfoBoxUpdate
(
    LPVOID  lpvInfoBoxInfo, 
    LPTSTR  lpszText, 
    LPDWORD lpdwResultFlags,
    LPVOID  lpvCookie
)
{

    if (!lpvCookie)
    {
        return (NAVEX_UI_TIMEOUT);
    }

    return (AVAPIInfoBoxUpdate32(lpvInfoBoxInfo,
                                 lpszText,
                                 lpdwResultFlags,
                                 lpvCookie));
}

UINT SYM_EXPORT  WINAPI NAVEX32InfoBoxClose
(
    LPVOID lpvInfoBoxInfo,
    LPVOID lpvCookie
)
{
    if (!lpvCookie)
    {
        return (NAVEX_UI_TIMEOUT);
    }

    return (AVAPIInfoBoxClose32(lpvInfoBoxInfo,
                                lpvCookie));
}

UINT SYM_EXPORT  WINAPI NAVEX32TextEditBox
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
    if (!lpvCookie)
    {
        return (NAVEX_UI_TIMEOUT);
    }

    return (AVAPITextEditBox32(dwBoxID,
                               lpszPrompt,
                               lpszSeedText,
                               lpszPutHere,
                               nMaxLength,
                               dwMSTimeOut,
                               lpvCookie));
}

VOID SYM_EXPORT  WINAPI NAVEX32GetConfigInfo
(
    LPTSTR lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
)
{

    *(BOOL *)lpvResult = TRUE;

}

CALLBACKREV2 gstNAVEX32CallBacks =
{
    NAVEX32FileOpen,  // HFILE  (WINAPI FAR *FileOpen)(LPTSTR lpName, UINT uOpenMode);         
    NAVEX32FileClose, // HFILE  (WINAPI FAR *FileClose)(HFILE hHandle);     
    NAVEX32FileRead,  // UINT   (WINAPI FAR *FileRead)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);     
    NAVEX32FileWrite, // UINT   (WINAPI FAR *FileWrite)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);     
    NAVEX32FileSeek,  // DWORD  (WINAPI FAR *FileSeek)(HFILE hHandle, LONG dwOffset, int nFlag);     
    NAVEX32FileLength,// DWORD  (WINAPI FAR *FileSize)(HFILE hHandle);     
    NAVEX32FileGetDateTime,// BOOL (WINAPI FAR *FileGetDateTime)(HFILE hHandle, UINT uType, WORD FAR *lpuDate, WORD FAR *lpuTime);     
    NAVEX32FileSetDateTime,// BOOL (WINAPI FAR *FileSetDateTime)(HFILE hHandle, UINT uType, WORD uDate, WORD uTime);     
    NAVEX32FileDelete,// BOOL   (WINAPI FAR *FileDelete)(LPTSTR lpFilename);     
    NAVEX32FileGetAttr,// UINT  (WINAPI FAR *FileGetAttr)(LPTSTR lpFilename, UINT FAR *lpuAttr);     
    NAVEX32FileSetAttr,// UINT  (WINAPI FAR *FileSetAttr)(LPTSTR lpFilename, UINT uAttr);     
    NAVEX32FileCreate,// HFILE  (WINAPI FAR *FileCreate)(LPTSTR lpFilename, UINT uAttr);     
    NULL,           // BOOL   (WINAPI FAR *FileLock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);     
    NULL,           // BOOL   (WINAPI FAR *FileUnlock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);     
    NULL,           // BOOL   (WINAPI FAR *Progress)(WORD wProgress, DWORD dwCookie);     
    NAVEX32SDiskReadPhysical, // UINT   (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);     
    NAVEX32SDiskWritePhysical,// UINT   (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE  bSector, BYTE bHead,  BYTE bDrive);     
    NAVEX32TempMemoryAlloc,// LPVOID (WINAPI FAR *TempMemoryAlloc)(DWORD uSize);     
    NAVEX32TempMemoryFree, // VOID   (WINAPI FAR *TempMemoryFree)(LPVOID lpBuffer);     
    NAVEX32PermMemoryAlloc,// LPVOID (WINAPI FAR *PermMemoryAlloc)(DWORD uSize);     
    NAVEX32PermMemoryFree, // VOID   (WINAPI FAR *PermMemoryFree)(LPVOID lpBuffer);     
    NAVEX32CreateMutex,    // UINT   (WINAPI FAR *CreateMutex)(LPLPVOID lplpMutex);     
    NAVEX32DestroyMutex,   // UINT   (WINAPI FAR *DestroyMutex)(LPVOID lpMutex);     
    NAVEX32WaitMutex,      // UINT   (WINAPI FAR *WaitMutex)(LPVOID lpMutex, DWORD dwMSTicks);     
    NAVEX32ReleaseMutex,   // UINT   (WINAPI FAR *ReleaseMutex)(LPVOID lpMutex);     
    NAVEX32GetTempFile,    // UINT   (WINAPI FAR *GetTempFile)(LPTSTR lpszFileName);    
    NAVEX32MessageBox,     // UINT   (WINAPI FAR *MessageBox)(DWORD dwBoxID, LPTSTR lpszMessage, DWORD dwButtonFlags, LPDWORD lpdwResultFlags, DWORD dwMSTimeOut);    
    NAVEX32InfoBoxInit,    // UINT   (WINAPI FAR *InfoBoxInit)(LPTSTR lpszTitle, DWORD dwButtonFlags, LPLPVOID lplpvInfoBoxInfo);    
    NAVEX32InfoBoxUpdate,  // UINT   (WINAPI FAR *InfoBoxUpdate)(LPVOID lpvInfoBoxInfo, LPTSTR lpszText, LPDWORD lpdwResultFlags);    
    NAVEX32InfoBoxClose,   // UINT   (WINAPI FAR *InfoBoxClose)(LPVOID lpvInfoBoxInfo);    
    NAVEX32TextEditBox,    // UINT   (WINAPI FAR *TextEditBox)(DWORD dwBoxID, LPTSTR lpszPrompt, LPTSTR lpszSeedText, LPTSTR lpszPutHere, int nMaxLength, DWORD dwMSTimeOut);    
    NAVEX32GetConfigInfo,  // VOID   (WINAPI FAR *GetConfigInfo)(LPTSTR lpszID, LPVOID lpvResult, UINT uMaxResultLen, LPBOOL lpbImplemented, LPVOID lpvCookie);
    NULL,
};

#endif // SYM_WIN32

