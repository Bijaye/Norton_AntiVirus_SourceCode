// Copyright (C) 1992-1993 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/INCLUDE/VCS/repair.h_v   1.12   02 Feb 1996 20:08:10   RSTANEV  $
//
// Description: C prototypes for file and boot repair functions.
//
//**********************************************************************
// $Log:   S:/INCLUDE/VCS/repair.h_v  $
// 
//    Rev 1.12   02 Feb 1996 20:08:10   RSTANEV
// Changed the prototypes of some APIs to be Unicode compatible.
//
//    Rev 1.11   29 Nov 1994 14:50:10   DALLEE
// Changes to include from .CPP
//
//    Rev 1.10   29 Nov 1994 14:28:08   DALLEE
// Reget from QAK2 post NAV 305.
//
//    Rev 1.4.3.0   19 Oct 1993 11:27:30   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.4   21 Jul 1993 17:13:44   MFALLEN
// Added generic boot repair code.
//**********************************************************************

#ifndef __REPAIR_H
#define __REPAIR_H

#define REP_MBR_GENERIC     ((BYTE)4)

#ifdef __cplusplus
extern "C" {
#endif

//;-------------------------------------------------------------------------
//;                        FILE REPAIR
//;-------------------------------------------------------------------------
UINT WINAPI REP4_ME  (WORD, UINT, LPTSTR, LPBYTE, WORD, LPTSTR);
//;-------------------------------------------------------------------------
//;                         BOOT REPAIR
//;-------------------------------------------------------------------------
UINT WINAPI REP4_BOOT (WORD VID, LPTSTR lpDatabase, LPBYTE lpBuffer, WORD uBuffer, BYTE byDrive);

UINT WINAPI DIS4_ANALYZE_FLOP (WORD wDriveWord, LPSTR lpDos, LPSTR lpWin);

BOOL WINAPI REP4_BOOT_GENERIC (LPSTR lpWin, LPSTR lpDos, WORD wDriveWord,
                               UINT uDiskType);

BOOL WINAPI REP4_MBR_GENERIC (LPSTR lpWin, LPSTR lpDos, BYTE, BYTE);

//;------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif  // !__REPAIR_H
