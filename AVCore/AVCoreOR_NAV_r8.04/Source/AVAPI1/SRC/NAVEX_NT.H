//--------------------------------------------------------------------
// File Name: NAVEX_NT.h
//
// Description: Prototypes and Macros for NT Low Level I/O helpers
//
// See Also: NEWVIR/CERTLIBN/avglob.cpp
//--------------------------------------------------------------------
// $Header:   S:/AVAPI1/VCS/NAVEX_NT.h_v   1.0   09 May 1997 18:12:02   MKEATIN  $
//--------------------------------------------------------------------
// $Log:   S:/AVAPI1/VCS/NAVEX_NT.h_v  $
// 
//    Rev 1.0   09 May 1997 18:12:02   MKEATIN
// Initial revision.
// 
//    Rev 1.0   08 May 1997 16:05:28   MKEATIN
// Initial revision.
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


#   endif                               // SYM_WIN32 && !SYM_VXD && !SYM_NTK

#endif                                  // __LOCALNT_H
