//************************************************************************
//
// $Header:   S:/NAVEX/VCS/macrodel.h_v   1.4   17 Jan 1997 11:01:40   DCHI  $
//
// Description:
//      Contains WordDocument macro delete function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrodel.h_v  $
// 
//    Rev 1.4   17 Jan 1997 11:01:40   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.3   21 Nov 1996 17:13:30   AOONWAL
// No change.
// 
//    Rev 1.2   29 Oct 1996 13:00:14   AOONWAL
// No change.
// 
//    Rev 1.1   06 May 1996 17:18:40   DCHI
// Added prototype for WordDocChangeToDocument() function.
// 
//    Rev 1.0   03 Jan 1996 17:12:12   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MACRODELH_

#define _MACRODELH_

#include "worddoc.h"

#if defined(SYM_DOSX) || !defined(SYM_DOS)

// Decryption key enhanced functions below

WORD WordDocDeleteMacro
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPMACRO_INFO_T      pMacroInfo,         // Macro info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
);

WORD WordDocChangeToDocument
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
);

// Decryption key enhanced functions above

#else

// Plain DOS below

WORD WordDocDeleteMacro
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPMACRO_TABLE_INFO  pMacroTableInfo,	// Macro table info
	LPMACRO_INFO_T		pMacroInfo,			// Macro info
	LPBYTE 				lpbyWorkBuffer		// Work buffer >= 512 bytes
);

WORD WordDocChangeToDocument
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
);

// Plain DOS above

#endif

#endif

