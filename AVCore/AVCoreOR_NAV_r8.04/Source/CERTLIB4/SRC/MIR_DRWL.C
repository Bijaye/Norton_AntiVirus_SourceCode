// Copyright 1993 Symantec Corporation
// ----------------------------------------------------------------------
//
// $Header:   S:\source\certlib4\vcs\mir_drwl.c_v   1.2   15 Mar 1994 10:55:38   BRUCE  $
//
// Description: OS/2 Mirrors replacement for INT 25/26
//              logical disk read and write
//
// Contains:
//
// See Also:
//
// ----------------------------------------------------------------------
// $Log:   S:\source\certlib4\vcs\mir_drwl.c_v  $
// 
//    Rev 1.2   15 Mar 1994 10:55:38   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.1   11 Feb 1994 15:19:42   MARKL
// Qak 2 changes moved to trunk
// 
//    Rev 1.0.1.1   03 Nov 1993 19:40:58   keith
// Allow critical errors to return an error to DosOpen rather than
// call the critical error routine.
// 
//    Rev 1.0.1.0   29 Oct 1993 14:49:12   keith
// No change.
// 
//    Rev 1.0   29 Oct 1993 14:47:30   keith
// Initial revision.
//

// OS/2 Mirrors replacement for INT25 and INT26 logical disk read and write

#include "platform.h"

#include "os2disk.h"
#include "cts4.h"

#if 0
WORD DosBytesPerDiskSector(WORD drive_handle)
{
    BIOSPARAMETERBLOCK bpb;
    short              command = 1;   // current medium

    if (DosDevIOCtl(&bpb, &command, DSK_GETDEVICEPARAMS, IOCTL_DISK,
      drive_handle) == 0)
        return (bpb.wBytesPerSector);
    else
        return 0;
}
#endif


UINT WINAPI DIS4_READ_LOGICAL(
    LPSTR   lpBuffer,
    BYTE    byDriveNumber,              // 0 = A:
    DWORD   dwSector,
    WORD    wCount)
{
    BYTE    szName[3];
    WORD    wHandle;
    WORD    wAction;
    WORD    wRetVal;
    WORD    wBytesRead;
    WORD    wSectorSize;
    DWORD   dwDistance;
    DWORD   dwNewPosition;

    szName[0] = byDriveNumber + 'A';
    szName[1] = ':';
    szName[2] = EOS;

    wRetVal = DosOpen ( szName, 
                        &wHandle, 
                        &wAction, 
                        0L, 
                        0, 
                        FILE_OPEN,
                        OPEN_ACCESS_READONLY     | OPEN_SHARE_DENYNONE |
                        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_DASD, 
                        0L);
    if ( wRetVal )
        return( wRetVal );

    wSectorSize = 512;                  // DosBytesPerDiskSector(wHandle);
    dwDistance = (DWORD)wSectorSize * dwSector;

    wRetVal = DosChgFilePtr( wHandle, 
                             dwDistance, 
                             FILE_BEGIN, 
                             &dwNewPosition );

    if ( !wRetVal )
        {
        wRetVal = DosRead( wHandle, 
                           lpBuffer, 
                           wSectorSize*wCount, 
                           &wBytesRead );
        }

    DosClose( wHandle );
    return( wRetVal );
}

UINT WINAPI DIS4_WRITE_LOGICAL(
    LPSTR   lpBuffer,
    BYTE    byDriveNumber,              // 0 = A:
    DWORD   dwSector,
    WORD    wCount)
{
    BYTE    szName[3];
    WORD    wHandle;
    WORD    wAction;
    WORD    wRetVal;
    WORD    wBytesWritten;
    WORD    wSectorSize;
    DWORD   dwDistance;
    DWORD   dwNewPosition;

    szName[0] = byDriveNumber + 'A';
    szName[1] = ':';
    szName[2] = EOS;

    wRetVal = DosOpen( szName, 
                       &wHandle, 
                       &wAction, 
                       0L, 
                       0, 
                       FILE_OPEN,
                       OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE 
                                            | OPEN_FLAGS_DASD, 
                       0L);
    if ( wRetVal )
        return( wRetVal );

    wRetVal = DosChgFilePtr( wHandle, 
                             dwDistance, 
                             FILE_BEGIN, 
                             &dwNewPosition );

    wSectorSize = 512;                  // DosBytesPerDiskSector(wHandle);
    dwDistance = (DWORD)wSectorSize * dwSector;

    if ( !wRetVal )
        {
        wRetVal = DosWrite( wHandle, 
                            lpBuffer, 
                            wSectorSize*wCount, 
                            &wBytesWritten );
        }

    DosClose( wHandle );
    return( wRetVal );
}
