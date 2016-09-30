// Copyright 1993-1994 Symantec Corporation
//===========================================================================
//
// $Header:   S:\source\include\vcs\repairn.h_v   1.4   27 May 1994 15:24:12   MFALLEN  $
//
// Description:  Declares function prototypes, macros, equates and types
//               for file and boot repair functions in CERTLIBN project.
//
// See Also:
//
//---------------------------------------------------------------------------
// $Log:   S:\source\include\vcs\repairn.h_v  $
// 
//    Rev 1.4   27 May 1994 15:24:12   MFALLEN
// Fixed protos.
// 
//    Rev 1.3   26 May 1994 10:33:36   RSTANEV
// Added new prototype.
//
//    Rev 1.2   24 May 1994 13:59:38   RSTANEV
// Using extern "C" {} when compiling with C++.
//
//    Rev 1.1   24 May 1994 13:42:34   RSTANEV
// Added REP4_ME.
//
//    Rev 1.0   05 May 1994 12:49:18   RSTANEV
// Initial revision.
//
//---------------------------------------------------------------------------

#define REP_MBR_GENERIC     ((BYTE)4)

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
//                         FILE REPAIR
//---------------------------------------------------------------------------
BOOL WINAPI REP4_ME (WORD, UINT, LPSTR, LPSTR, UINT, LPSTR);

//---------------------------------------------------------------------------
//                         BOOT REPAIR
//---------------------------------------------------------------------------

BOOL WINAPI REP4_BOOT (
    WORD    VID,
    LPSTR   lpDatabase,
    LPSTR   lpBuffer,
    UINT    uBuffer,
    BYTE    byDrive );

BYTE WINAPI DIS4_ANALYZE_FLOP (
    BYTE    byDriveWord, 
    LPSTR   lpMem );

BOOL WINAPI REP4_BOOT_GENERIC (
    LPSTR   lpWin, 
    BYTE    byDriveWord,
    BYTE    byDiskType );

BOOL WINAPI REP4_MBR_GENERIC (
    LPSTR   lpMem, 
    BYTE    byDrive, 
    BYTE    byRepairType );

//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
