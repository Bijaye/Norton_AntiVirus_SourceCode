//************************************************************************
//
// $Header:   S:/NAVEX/VCS/macrscan.h_v   1.4   17 Jan 1997 11:01:50   DCHI  $
//
// Description:
//      Contains WordDocument macro delete function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrscan.h_v  $
// 
//    Rev 1.4   17 Jan 1997 11:01:50   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.3   21 Nov 1996 17:14:04   AOONWAL
// No change.
// 
//    Rev 1.2   29 Oct 1996 13:00:22   AOONWAL
// No change.
// 
//    Rev 1.1   28 Aug 1996 16:18:32   DCHI
// Changed comment on required work buffer size.
// 
//    Rev 1.0   03 Jan 1996 17:12:12   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MACRSCANH_

#define _MACRSCANH_

#include "platform.h"
#include "callback.h"

#include "ctsn.h"
#include "navex.h"

#if defined(SYM_DOSX) || !defined(SYM_DOS)

EXTSTATUS EXTScanFileMacro
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE          lpbyWorkBuffer,         // Work buffer >= 2048 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
);

#else

// Plain DOS below

#define LPF

typedef struct
{
	BYTE LPF *pszMacroName;
	BYTE LPF *pbySig;
} WORD_MACRO_SIG_T;

typedef WORD_MACRO_SIG_T LPF *LPWORD_MACRO_SIG_T;

typedef struct
{
	WORD				wVID;		// VID returned on match
	LPWORD_MACRO_SIG_T	pwmsSig;	// NULL-terminated array of
									// macro signatures.
} WORD_SIG_T;

typedef WORD_SIG_T LPF *LPWORD_SIG_T;

EXTSTATUS EXTScanFileMacro
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE          lpbyWorkBuffer,         // Work buffer >= 1024 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
);

// Plain DOS above

#endif

#endif

