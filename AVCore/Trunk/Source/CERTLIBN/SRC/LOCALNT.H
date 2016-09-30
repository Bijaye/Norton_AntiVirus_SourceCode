//--------------------------------------------------------------------
// File Name: LocalNT.h
//
// Description: Prototypes and Macros for NT Low Level I/O helpers
//
// See Also: NEWVIR/CERTLIBN/avglob.cpp
//--------------------------------------------------------------------
// $Header:   S:/CERTLIBN/VCS/localnt.h_v   1.0   17 Jul 1996 21:20:46   RAY  $
//--------------------------------------------------------------------
// $Log:   S:/CERTLIBN/VCS/localnt.h_v  $
// 
//    Rev 1.0   17 Jul 1996 21:20:46   RAY
// Initial revision.
// 
//--------------------------------------------------------------------
#if !defined( __LOCALNT_H )
#   define __LOCALNT_H

#   if defined( SYM_WIN32 ) && !defined( SYM_VXD ) && !defined( SYM_NTK )

#   if !defined( MAX_PARTITIONS )
#       define MAX_PARTITIONS       4
#   endif

BYTE LocalNTDeterminePartNum
(
    BYTE    byDrive,
    BYTE    byHead,
    WORD    wCylinder,
    BYTE    bySector,
    BYTE    bySectors,
    LPDWORD lpdwLinearOffset
);

BYTE LocalNTDetermineDriveLetter
(
    BYTE    byDrive,
    BYTE    byPartNum
);

BYTE LocalNTDiskWriteLogical
(
    BYTE    byDriveLetter,
    DWORD   dwLinearOffset,
    LPVOID  lpBuffer,
    LPDWORD lpdwSectors
);


#   endif                               // SYM_WIN32 && !SYM_VXD && !SYM_NTK

#endif                                  // __LOCALNT_H
