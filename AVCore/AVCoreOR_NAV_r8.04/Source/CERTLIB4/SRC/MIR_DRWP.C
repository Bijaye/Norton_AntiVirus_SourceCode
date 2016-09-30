// Copyright 1993 Symantec Corporation
// ----------------------------------------------------------------------
//
// $Header:   S:\source\certlib4\vcs\mir_drwp.c_v   1.2   15 Mar 1994 10:55:34   BRUCE  $
//
// Description: OS/2 Mirrors replacement for INT13
//              physical disk read and write
//
// Contains:
//
// See Also:
//
// ----------------------------------------------------------------------
// $Log:   S:\source\certlib4\vcs\mir_drwp.c_v  $
// 
//    Rev 1.2   15 Mar 1994 10:55:34   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.1   11 Feb 1994 15:19:40   MARKL
// Qak 2 changes moved to trunk
// 
//    Rev 1.0.1.3   10 Nov 1993 14:18:26   keith
// OS/2 Mirrors - subtract 1 from the sector passed to these entry points
// so that the first drive sector is accessed by the value 1 rather than 
// a 0. OS/2 low level likes 0, DOS likes 1 as first sector.
// 
//    Rev 1.0.1.2   03 Nov 1993 19:40:14   keith
// Allow for reading floppy disks via the Int13 replacement routines.
// 
//    Rev 1.0.1.1   02 Nov 1993 17:16:18   KEITH
// Resolve a conflicting macro name with TOOLHELP.H
// 
//    Rev 1.0.1.0   29 Oct 1993 14:49:16   keith
// No change.
// 
//    Rev 1.0   29 Oct 1993 14:47:32   keith
// Initial revision.
//

#include "platform.h"

#pragma pack(1)
#include "os2disk.h"
#include "cts4.h"

// For logical drives only
// Will be called by NAV only for floppy diskettes

WORD SYM_EXPORT WINAPI OpenLogicalDisk(WORD wDriveNumber)
{
    BYTE    szName[3];
    WORD    hIOCtl;
    WORD    wAction;
    WORD    wRetVal;

    szName[0] = (BYTE)wDriveNumber + 'A';
    szName[1] = ':';
    szName[2] = EOS;

    wRetVal = DosOpen ( szName, 
                        &hIOCtl, 
                        &wAction, 
                        0L, 
                        0, 
                        FILE_OPEN,
                        OPEN_ACCESS_READONLY     | OPEN_SHARE_DENYNONE |
                        OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_DASD, 
                        0L);
    if ( wRetVal )
        return ( 0 );
    else
        return ( hIOCtl );
}

WORD SYM_EXPORT WINAPI CloseLogicalDisk(WORD hIOCtl)
{
    return ( DosClose( hIOCtl ) );
}

// For partitioned drives only
WORD SYM_EXPORT WINAPI OpenPhysicalDisk(WORD wDriveNumber)
{
    WORD    wRetVal;
    BYTE    szName[3];
    WORD    hIOCtl;
        
    szName[0] = (BYTE)wDriveNumber - 0x80 + '1'; // "1:" is first hard drive
    szName[1] = ':';
    szName[2] = EOS;

    wRetVal = DosPhysicalDisk(INFO_GETIOCTLHANDLE, &hIOCtl, 2, szName, 3);
        
    if(wRetVal)
        return (0);
    else
        return (hIOCtl);
}

WORD SYM_EXPORT WINAPI ClosePhysicalDisk(WORD hIOCtl)
{
    WORD    wRetVal;

    wRetVal = DosPhysicalDisk(INFO_FREEIOCTLHANDLE, NULL, 0, &hIOCtl, 2);
        
    if(wRetVal)
        return (ERR);
    else
        return (NOERR);
}

// For partitioned or logical drives only
WORD SYM_EXPORT WINAPI ReadDiskIOCtl (
    WORD    hIOCtl, 
    LPVOID  lpData, 
    WORD    wHead, 
    WORD    wCylinder, 
    WORD    wFirstSector, 
    WORD    wNumberOfSectors,
    WORD    wCommand)
{
    WORD            wRetVal;
    WORD            wSectorNumber;
    LPPHYSDISKPARAM lpPhysdiskparam;
    BYTE            Buffer[256];

    lpPhysdiskparam                   = (LPPHYSDISKPARAM)Buffer;

    lpPhysdiskparam->bCommandInfo     = 1;
    lpPhysdiskparam->wHead            = wHead;
    lpPhysdiskparam->wCylinder        = wCylinder;
    lpPhysdiskparam->wFirstSector     = wFirstSector;
    lpPhysdiskparam->wNumberOfSectors = wNumberOfSectors;

#if 1
    // &? this is questionable code - do not use it in anything real <<<<<LOOK>>>>>
    for(wSectorNumber = 0; wSectorNumber < 50; wSectorNumber++)
        {
        lpPhysdiskparam->TrackLayout[wSectorNumber].wSectorNumber = wSectorNumber+1;
        lpPhysdiskparam->TrackLayout[wSectorNumber].wSectorSize   = 512;
        }
    // end questionable code
#endif

    wRetVal = DosDevIOCtl ( lpData, 
                            lpPhysdiskparam, 
                            PDSK_READPHYSTRACK, 
                            wCommand, 
                            hIOCtl);
    if(wRetVal)
        return(ERR);
    else
        return(NOERR);
}


// For partitioned drives only
WORD SYM_EXPORT WINAPI WriteDiskIOCtl (
    WORD    hIOCtl, 
    LPVOID  lpData, 
    WORD    wHead, 
    WORD    wCylinder, 
    WORD    wFirstSector, 
    WORD    wNumberOfSectors,
    WORD    wCommand)
{
    WORD            wRetVal;
    WORD            wSectorNumber;
    LPPHYSDISKPARAM lpPhysdiskparam;
    BYTE            Buffer[256];

    lpPhysdiskparam                   = (LPPHYSDISKPARAM)Buffer;

    lpPhysdiskparam->bCommandInfo     = 1;
    lpPhysdiskparam->wHead            = wHead;
    lpPhysdiskparam->wCylinder        = wCylinder;
    lpPhysdiskparam->wFirstSector     = wFirstSector;
    lpPhysdiskparam->wNumberOfSectors = wNumberOfSectors;

#if 1
    // &? this is questionable code - do not use it in anything real <<<<<LOOK>>>>>
    for(wSectorNumber = 0; wSectorNumber < 50; wSectorNumber++)
        {
        lpPhysdiskparam->TrackLayout[wSectorNumber].wSectorNumber = wSectorNumber+1;
        lpPhysdiskparam->TrackLayout[wSectorNumber].wSectorSize   = 512;
        }
    // end questionable code
#endif

    wRetVal = DosDevIOCtl ( lpData, 
                            lpPhysdiskparam, 
                            PDSK_WRITEPHYSTRACK, 
                            wCommand, 
                            hIOCtl);

    if(wRetVal)
        return(ERR);
    else
        return(NOERR);
}


UINT WINAPI ReadLogicalDisk13(
    LPSTR lpBuffer,
    BYTE byCount,
    WORD wCylinder,
    BYTE bySector,
    BYTE byHead,
    BYTE byDrive)
{    
    WORD    hIOCtl;
    WORD    wRetVal;

    hIOCtl = OpenLogicalDisk( byDrive );

    if (hIOCtl)
        {
        wRetVal = ReadDiskIOCtl( hIOCtl, 
                                 lpBuffer,   
                                 byHead,  
                                 wCylinder,  
                                 bySector,  
                                 byCount,
                                 IOCTL_DISK );
        
        CloseLogicalDisk(hIOCtl);
        return ( wRetVal );
        }

    return ( ERR );
}


UINT WINAPI ReadPhysicalDisk13(
    LPSTR lpBuffer,
    BYTE byCount,
    WORD wCylinder,
    BYTE bySector,
    BYTE byHead,
    BYTE byDrive)                       // 80 = First hard drive
{    
    WORD    hIOCtl;
    WORD    wRetVal;

    hIOCtl = OpenPhysicalDisk( byDrive );

    if (hIOCtl)
        {
        wRetVal = ReadDiskIOCtl ( hIOCtl, 
                                  lpBuffer,   
                                  byHead,  
                                  wCylinder,  
                                  bySector,  
                                  byCount,
                                  IOCTL_PHYSICALDISK);
        
        ClosePhysicalDisk(hIOCtl);
        return ( wRetVal );
        }

    return ( ERR );
}


UINT WINAPI WriteLogicalDisk13(
    LPSTR lpBuffer,
    BYTE byCount,
    WORD wCylinder,
    BYTE bySector,
    BYTE byHead,
    BYTE byDrive)
{    
    WORD    hIOCtl;
    WORD    wRetVal;

    hIOCtl = OpenLogicalDisk( byDrive-0x80+1 );

    if (hIOCtl)
        {
        wRetVal = WriteDiskIOCtl( hIOCtl, 
                                  lpBuffer,   
                                  byHead,  
                                  wCylinder,  
                                  bySector,  
                                  byCount,
                                  IOCTL_DISK );
        
        CloseLogicalDisk(hIOCtl);
        return ( wRetVal );
        }

    return ( ERR );
}


UINT WINAPI WritePhysicalDisk13(
    LPSTR lpBuffer,
    BYTE byCount,
    WORD wCylinder,
    BYTE bySector,
    BYTE byHead,
    BYTE byDrive)                       // 80 = First hard drive
{    
    WORD    hIOCtl;
    WORD    wRetVal;

    hIOCtl = OpenPhysicalDisk( byDrive );

    if (hIOCtl)
        {
        wRetVal = WriteDiskIOCtl ( hIOCtl, 
                                   lpBuffer,   
                                   byHead,  
                                   wCylinder,  
                                   bySector,  
                                   byCount,
                                   IOCTL_PHYSICALDISK);
        
        ClosePhysicalDisk(hIOCtl);
        return ( wRetVal );
        }

    return ( ERR );
}


UINT WINAPI DIS4_READ_PHYSICAL(
    LPSTR lpBuffer,
    BYTE byCount,
    WORD wCylinder,
    BYTE bySector,
    BYTE byHead,
    BYTE byDrive)
{
    if( byDrive < 0x80 )
        return ( ReadLogicalDisk13 ( lpBuffer,
                                     byCount, 
                                     wCylinder,
                                     bySector-1,
                                     byHead,
                                     byDrive ));
    else        
        return ( ReadPhysicalDisk13 ( lpBuffer,
                                      byCount, 
                                      wCylinder,
                                      bySector-1,
                                      byHead,
                                      byDrive ));
}


UINT WINAPI DIS4_WRITE_PHYSICAL(
    LPSTR lpBuffer,
    BYTE byCount,
    WORD wCylinder,
    BYTE bySector,
    BYTE byHead,
    BYTE byDrive)
{
    if( byDrive < 0x80 )
        return ( WriteLogicalDisk13 ( lpBuffer,
                                      byCount, 
                                      wCylinder,
                                      bySector-1,
                                      byHead,
                                      byDrive ));
    else        
        return ( WritePhysicalDisk13 ( lpBuffer,
                                       byCount, 
                                       wCylinder,
                                       bySector-1,
                                       byHead,
                                       byDrive ));
}

