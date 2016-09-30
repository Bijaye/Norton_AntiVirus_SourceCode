//************************************************************************
//
// $Header:   S:/NAVEX/VCS/neperep.h_v   1.22   14 Jan 1999 16:02:40   DKESSNE  $
//
// Description:
//      Header for Windows repair functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/neperep.h_v  $
// 
//    Rev 1.22   14 Jan 1999 16:02:40   DKESSNE
// added prototype for EXTRepairFileMemorial
// 
//    Rev 1.21   24 Dec 1998 23:04:02   DCHI
// Added EXTRepairFileRemoteXCorrupt() prototype.
// 
//    Rev 1.20   23 Dec 1998 18:13:12   relniti
// EXTRepairFileRemoteExpore
// 
//    Rev 1.19   10 Dec 1998 16:40:04   DKESSNE
// added prototype for EXTRepairFileK32
// 
//    Rev 1.18   24 Nov 1998 11:42:54   DKESSNE
// added prototype for W95.Parvo repair
// 
//    Rev 1.17   17 Nov 1998 17:46:36   DKESSNE
// added prototype for W95.Padania repair
// 
//    Rev 1.16   12 Nov 1998 18:30:04   DKESSNE
// added prototype for HPS repair
// 
//    Rev 1.15   14 Oct 1998 14:06:26   DKESSNE
// added prototype for Cabanas repair
// 
//    Rev 1.14   30 Sep 1998 15:58:08   DKESSNE
// added prototype for W95.Inca repair function
// 
//    Rev 1.13   01 Sep 1998 12:49:32   DKESSNE
// removed ifdefs from W95.MarkJ repair prototype (wasn't building right)
// 
//    Rev 1.12   01 Sep 1998 12:35:02   DKESSNE
// added prototype for W95.MarkJ repair
// 
//    Rev 1.11   24 Jul 1998 16:17:40   DKESSNE
// added prototype for Marburg repair function
// 
//    Rev 1.10   29 Jun 1998 14:58:10   DKESSNE
// added declaration for W95.CIH 0x617e
// 
//    Rev 1.9   12 Mar 1998 20:13:36   RELNITI
// ADD function prototype EXTScanFileKlunky
// 
//    Rev 1.8   03 Nov 1997 20:49:40   JWILBER
// Added function proto for EXTRepairFilePoppy, to repair
// Poppy.1052, VID 6071, for CI 417.
//
//    Rev 1.7   02 Apr 1997 19:07:16   AOONWAL
// Added LPN30 (structure) parameter to all the repair function
//
//    Rev 1.6   10 Dec 1996 13:18:22   AOONWAL
// Added function prototype for WinTpvo.3783 repair
//
//    Rev 1.5   21 Nov 1996 17:13:20   AOONWAL
// No change.
//
//    Rev 1.4   29 Oct 1996 13:00:12   AOONWAL
// No change.
//
//    Rev 1.3   13 Jun 1996 15:18:36   JWILBER
// Added prototype for EXTRepairFileNewTent().
//
//    Rev 1.2   15 May 1996 00:29:46   JWILBER
// Added prototype for Tentacle repair.
//
//    Rev 1.1   15 Apr 1996 20:38:58   RSTANEV
// TCHAR support.
//
//    Rev 1.0   08 Feb 1996 10:39:30   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _NEPEREPH_
#define _NEPEREPH_

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"
#include "navexshr.h"

WORD EXTRepairFileBoza
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileTent
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileNewTent
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileTpvo
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFilePoppy
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileKlunky
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileCIH
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileMarburg
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileMarkJ
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileInca
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileCabanas
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileHPS
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFilePadania
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileParvo
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileK32
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileRemoteExplore
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileRemoteXCorrupt
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);

WORD EXTRepairFileMemorial
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
);


#endif
