// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/vlcomm.cpv   1.2   17 Mar 1998 15:24:40   MKEATIN  $
//
// Description:
//      This file contains the callback structure and callbacks
//      requred by the NAVCORE library.
//
// Contains:
//      VLCBFileOpen()
//      VLCBFileClose()
//      VLCBFileRead()
//      VLCBFileWrite()
//      VLCBFileSeek()
//      VLCBFileSize()
//      VLCBFileGetDateTime()
//      VLCBFileSetDateTime()
//      VLCBFileDelete()
//      VLCBFileGetAttr()
//      VLCBFileSetAttr()
//      VLCBFileCreate()
//      VLCBSDiskReadPhysical()
//      VLCBSDiskWritePhysical()
//      VLCBTempMemoryAlloc()
//      VLCBTempMemoryFree()
//      VLCBPermMemoryAlloc()
//      VLCBPermMemoryFree()
//
// See Also:
//
// Note:
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/vlcomm.cpv  $
// 
//    Rev 1.2   17 Mar 1998 15:24:40   MKEATIN
// updated ABSDISKREC variables to match those in the new disk.h
// 
//    Rev 1.1   17 Feb 1998 22:09:50   RStanev
// Updated copyright.
//
//    Rev 1.0   09 Apr 1997 12:21:50   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "file.h"
#include "disk.h"
#include "navap.h"

//---------------------------------------------------------------------------
//
// Local function prototypes.  These will not be defined as static because
// the global symbols is the only thing I can use to see where I am during
// a debugging session.
//
//---------------------------------------------------------------------------
HFILE  WINAPI VLCBFileOpen ( LPTSTR lpName, UINT uOpenMode );
HFILE  WINAPI VLCBFileClose ( HFILE hHandle );
UINT   WINAPI VLCBFileRead ( HFILE hHandle, LPVOID lpBuffer, UINT uBytes );
UINT   WINAPI VLCBFileWrite ( HFILE hHandle, LPVOID lpBuffer, UINT uBytes );
DWORD  WINAPI VLCBFileSeek ( HFILE hHandle, LONG dwOffset, int nFlag );
DWORD  WINAPI VLCBFileSize ( HFILE hHandle );
BOOL   WINAPI VLCBFileGetDateTime ( HFILE hHandle, UINT uType, WORD FAR *lpuDate, WORD FAR *lpuTime );
BOOL   WINAPI VLCBFileSetDateTime ( HFILE hHandle, UINT uType, WORD uDate, WORD uTime );
BOOL   WINAPI VLCBFileDelete ( LPTSTR lpFilename );
UINT   WINAPI VLCBFileGetAttr ( LPTSTR lpFilename, UINT FAR *lpuAttr );
UINT   WINAPI VLCBFileSetAttr ( LPTSTR lpFilename, UINT uAttr );
HFILE  WINAPI VLCBFileCreate ( LPTSTR lpFilename, UINT uAttr );
BOOL   WINAPI VLCBFileLock ( HFILE hHandle, DWORD dwStart, DWORD dwLength );
BOOL   WINAPI VLCBFileUnlock ( HFILE hHandle, DWORD dwStart, DWORD dwLength );
BOOL   WINAPI VLCBProgress ( WORD wProgress, DWORD dwCookieMonster );
UINT   WINAPI VLCBSDiskReadPhysical ( LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE bSector, BYTE bHead, BYTE bDrive );
UINT   WINAPI VLCBSDiskWritePhysical ( LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE bSector, BYTE bHead, BYTE bDrive );
LPVOID WINAPI VLCBTempMemoryAlloc ( DWORD dwSize );
VOID   WINAPI VLCBTempMemoryFree ( LPVOID lpBuffer );
LPVOID WINAPI VLCBPermMemoryAlloc ( DWORD dwSize );
VOID   WINAPI VLCBPermMemoryFree ( LPVOID lpBuffer );

//---------------------------------------------------------------------------
//
// The following structure contains the callbacks for NAVCORE.
//
//---------------------------------------------------------------------------
CALLBACKREV1 rVLCallbacksRev1 =
    {
    &VLCBFileOpen,
    &VLCBFileClose,
    &VLCBFileRead,
    &VLCBFileWrite,
    &VLCBFileSeek,
    &VLCBFileSize,
    &VLCBFileGetDateTime,
    &VLCBFileSetDateTime,
    &VLCBFileDelete,
    &VLCBFileGetAttr,
    &VLCBFileSetAttr,
    &VLCBFileCreate,
    &VLCBFileLock,
    &VLCBFileUnlock,
    &VLCBProgress,
    &VLCBSDiskReadPhysical,
    &VLCBSDiskWritePhysical,
    &VLCBTempMemoryAlloc,
    &VLCBTempMemoryFree,
    &VLCBPermMemoryAlloc,
    &VLCBPermMemoryFree
    };

//---------------------------------------------------------------------------
//
// Local functions.
//
//---------------------------------------------------------------------------
HFILE WINAPI VLCBFileOpen ( LPTSTR lpName, UINT uOpenMode )
    {
    SYM_ASSERT ( lpName );

    return ( FileOpen ( lpName, uOpenMode ) );
    }

HFILE WINAPI VLCBFileClose ( HFILE hHandle )
    {
    SYM_ASSERT ( hHandle != HFILE_ERROR );

    return ( FileClose ( hHandle ) );
    }

UINT WINAPI VLCBFileRead ( HFILE hHandle, LPVOID lpBuffer, UINT uBytes )
    {
    SYM_ASSERT ( hHandle != HFILE_ERROR );
    SYM_ASSERT ( lpBuffer );
    SYM_ASSERT ( uBytes );

    return ( FileRead ( hHandle, lpBuffer, uBytes ) );
    }

UINT WINAPI VLCBFileWrite ( HFILE hHandle, LPVOID lpBuffer, UINT uBytes )
    {
    SYM_ASSERT ( hHandle != HFILE_ERROR );
    SYM_ASSERT ( lpBuffer );

    return ( FileWrite ( hHandle, lpBuffer, uBytes ) );
    }

DWORD WINAPI VLCBFileSeek ( HFILE hHandle, LONG dwOffset, int nFlag )
    {
    SYM_ASSERT ( hHandle != HFILE_ERROR );

    return ( FileSeek ( hHandle, dwOffset, nFlag ) );
    }

DWORD WINAPI VLCBFileSize ( HFILE hHandle )
    {
    SYM_ASSERT ( hHandle != HFILE_ERROR );

    return ( FileLength ( hHandle ) );
    }

BOOL WINAPI VLCBFileGetDateTime ( HFILE hHandle, UINT uType, WORD FAR *lpuDate, WORD FAR *lpuTime )
    {
    auto FILETIME rFileTime;
    auto BOOL     bResult;

    SYM_ASSERT ( hHandle != HFILE_ERROR );
    SYM_ASSERT ( lpuDate );
    SYM_ASSERT ( lpuTime );

    if ( FileGetTime ( hHandle, uType, &rFileTime ) == NOERR )
        {
        bResult = FileTimeToDosTime ( &rFileTime, lpuDate, lpuTime );
        }
    else
        {
        bResult = FALSE;
        }

    return ( bResult );
    }

BOOL WINAPI VLCBFileSetDateTime ( HFILE hHandle, UINT uType, WORD uDate, WORD uTime )
    {
    auto FILETIME rFileTime;
    auto BOOL     bResult;

    SYM_ASSERT ( hHandle != HFILE_ERROR );

    if ( DosTimeToFileTime ( uDate, uTime, &rFileTime ) )
        {
        bResult = FileSetTime ( hHandle, uType, &rFileTime ) == NOERR ?
                  TRUE : FALSE;
        }
    else
        {
        bResult = FALSE;
        }

    return ( bResult );
    }

BOOL WINAPI VLCBFileDelete ( LPTSTR lpFilename )
    {
    SYM_ASSERT ( lpFilename );

    return ( FileDelete ( lpFilename ) == NOERR ? TRUE : FALSE );
    }

UINT WINAPI VLCBFileGetAttr ( LPTSTR lpFilename, UINT FAR *lpuAttr )
    {
    SYM_ASSERT ( lpFilename );
    SYM_ASSERT ( lpuAttr );

    return ( FileGetAttr ( lpFilename, lpuAttr ) );
    }

UINT WINAPI VLCBFileSetAttr ( LPTSTR lpFilename, UINT uAttr )
    {
    SYM_ASSERT ( lpFilename );

    return ( FileSetAttr ( lpFilename, uAttr ) );
    }

HFILE WINAPI VLCBFileCreate ( LPTSTR lpFilename, UINT uAttr )
    {
    SYM_ASSERT ( lpFilename );

    return ( FileCreate ( lpFilename, uAttr ) );
    }

BOOL WINAPI VLCBFileLock ( HFILE hHandle, DWORD dwStart, DWORD dwLength )
    {
#if defined(SYM_VXD)
    SYM_ASSERT ( hHandle != HFILE_ERROR );

    return ( FileLockRegion ( hHandle, dwStart, dwLength ) == NOERR ? TRUE : FALSE );
#else
    SYM_ASSERT ( FALSE );

    return ( FALSE );
#endif
    }

BOOL WINAPI VLCBFileUnlock ( HFILE hHandle, DWORD dwStart, DWORD dwLength )
    {
#if defined(SYM_VXD)
    SYM_ASSERT ( hHandle != HFILE_ERROR );

    return ( FileUnlockRegion ( hHandle, dwStart, dwLength ) == NOERR ? TRUE : FALSE );
#else
    SYM_ASSERT ( FALSE );

    return ( FALSE );
#endif
    }

BOOL WINAPI VLCBProgress ( WORD wProgress, DWORD dwCookieMonster )
    {
                                        // No need of this function
    return ( TRUE );
    }

UINT WINAPI VLCBSDiskReadPhysical ( LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE bSector, BYTE bHead, BYTE bDrive )
    {
    auto UINT       uCount, uResult;
    auto ABSDISKREC absDisk;

    SYM_ASSERT ( lpBuffer );

    uCount = 2;

    do
        {
        MEMSET(&absDisk, 0, sizeof(absDisk));
        absDisk.dn = bDrive;

        uResult = DiskGetPhysicalInfo ( &absDisk );

        if ( uResult )
            {
            continue;
            }

        absDisk.dwHead = bHead;
        absDisk.dwTrack = wCylinder;
        absDisk.dwSector = bSector;
        absDisk.numSectors = bCount;
        absDisk.buffer = (HPBYTE) lpBuffer;
        uResult = DiskAbsOperation(READ_COMMAND, &absDisk);
        } while ( --uCount && uResult );

    return ( uResult );
    }

UINT WINAPI VLCBSDiskWritePhysical ( LPVOID lpBuffer, BYTE bCount, WORD wCylinder, BYTE bSector, BYTE bHead, BYTE bDrive )
    {
    auto UINT       uCount, uResult;
    auto ABSDISKREC absDisk;

    SYM_ASSERT ( lpBuffer );

    uCount = 2;

    do
        {
        MEMSET(&absDisk, 0, sizeof(absDisk));
        absDisk.dn = bDrive;

        uResult = DiskGetPhysicalInfo ( &absDisk );

        if ( uResult )
            {
            continue;
            }

        absDisk.dwHead = bHead;
        absDisk.dwTrack = wCylinder;
        absDisk.dwSector = bSector;
        absDisk.numSectors = bCount;
        absDisk.buffer = (HPBYTE) lpBuffer;
        uResult = DiskAbsOperation(WRITE_COMMAND, &absDisk);
        } while ( --uCount && uResult );

    return ( uResult );
    }

LPVOID WINAPI VLCBTempMemoryAlloc ( DWORD dwSize )
    {
    SYM_ASSERT ( dwSize );

    return ( MemAllocPtr ( GHND, dwSize ) );
    }

VOID WINAPI VLCBTempMemoryFree ( LPVOID lpBuffer )
    {
    SYM_ASSERT ( lpBuffer );

    MemFreePtr ( lpBuffer );
    }

LPVOID WINAPI VLCBPermMemoryAlloc ( DWORD dwSize )
    {
    SYM_ASSERT ( dwSize );

    return ( MemAllocPtr ( GHND, dwSize ) );
    }

VOID WINAPI VLCBPermMemoryFree ( LPVOID lpBuffer )
    {
    SYM_ASSERT ( lpBuffer );

    MemFreePtr ( lpBuffer );
    }
