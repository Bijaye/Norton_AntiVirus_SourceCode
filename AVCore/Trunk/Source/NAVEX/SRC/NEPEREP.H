//************************************************************************
//
// $Header:   S:/NAVEX/VCS/neperep.h_v   1.7   02 Apr 1997 19:07:16   AOONWAL  $
//
// Description:
//      Header for Windows repair functions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/neperep.h_v  $
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

#endif

