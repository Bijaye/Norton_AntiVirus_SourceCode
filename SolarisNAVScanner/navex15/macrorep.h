//************************************************************************
//
// $Header:   S:/NAVEX/VCS/macrorep.h_v   1.6   17 Jan 1997 11:01:46   DCHI  $
//
// Description:
//      Contains WordDocument macro repair function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrorep.h_v  $
// 
//    Rev 1.6   17 Jan 1997 11:01:46   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.5   21 Nov 1996 17:13:38   AOONWAL
// No change.
// 
//    Rev 1.4   29 Oct 1996 13:00:16   AOONWAL
// No change.
// 
//    Rev 1.3   28 Aug 1996 16:16:06   DCHI
// Changed comment on required work buffer size.
// 
//    Rev 1.2   06 May 1996 17:19:32   DCHI
// Modified prototype for WordMacroVirusRepair() to support Macintosh.
// 
//    Rev 1.1   15 Apr 1996 20:33:46   RSTANEV
// TCHAR support.
//
//    Rev 1.0   03 Jan 1996 17:12:14   DCHI
// Initial revision.
//
//************************************************************************

#ifndef _MACROREPH_

#define _MACROREPH_

#include "platform.h"
#include "callback.h"

#include "ctsn.h"
#include "navex.h"

#if defined(SYM_DOSX) || !defined(SYM_DOS)

EXTSTATUS WordMacroVirusRepair
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
	WORD 			wVID,					// ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
#ifdef __MACINTOSH__
    HFILE           hFile,                  // Handle to file to repair
#endif
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 2048 bytes
);

#else

// Plain DOS below

#define LPF

typedef struct
{
	WORD			wVID;
	BYTE LPF * LPF *ppszMacroNames;
} MACRO_REPAIR_T;

typedef MACRO_REPAIR_T LPF *	LPMACRO_REPAIR_T;

EXTSTATUS WordMacroVirusRepair
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	WORD 			wVID,					// ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
#ifdef __MACINTOSH__
    HFILE           hFile,                  // Handle to file to repair
#endif
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 1024 bytes
);

// Plain DOS above

#endif

#endif

