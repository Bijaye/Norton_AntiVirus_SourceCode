// Copyright 1993 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/SYMKRNL.H_v   1.0   26 Jan 1996 20:21:46   JREARDON  $
//
// Description:
//      Contains typedefs and prototypes for use within SymKrnl.DLL
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/SYMKRNL.H_v  $
// 
//    Rev 1.0   26 Jan 1996 20:21:46   JREARDON
// Initial revision.
// 
//    Rev 1.15   19 Apr 1995 20:10:24   DAVID
// Cleaned up the management of the Symkrnl VxD
// 
//    Rev 1.14   21 Mar 1995 14:41:18   MFALLEN
// Create a SymKRNLAttach() and SymKRNLDetach() function for 16-bit platform also.
// 
//    Rev 1.13   02 Mar 1995 11:44:54   BRIANF
// Made changes that are necessary to build custom Quake DLLs.
//
//    Rev 1.12   02 Feb 1995 17:56:46   BRAD
// Added SYMKRNL VxD changes from QAK6
//
//    Rev 1.11   21 Oct 1994 11:50:46   BRAD
// Removed _SetInternalDBCSTable
//
//    Rev 1.10   20 Oct 1994 18:28:40   BRAD
// Added _SetInternalDBCSTable()
//
//    Rev 1.9   11 May 1994 21:12:58   BRAD
// Added 2 Convert* functions between seconds and FILETIMEs
//
//    Rev 1.8   03 May 1994 22:57:22   BRAD
// Remove functions NFileOpen() and NFileCreate()
//
//    Rev 1.7   07 Apr 1994 11:58:52   BRUCE
// Added extern C stuff
//
//    Rev 1.6   23 Mar 1994 16:25:40   BRAD
// Removed SHELL function ptrs.  No longer used
//
//    Rev 1.5   15 Mar 1994 12:35:22   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.4   25 Feb 1994 12:23:40   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.3   23 Sep 1993 12:40:52   ED
// #ifs for masking multiple inclusion and for excluding SHELL items
// without SHELLAPI.
//
//    Rev 1.2   23 Sep 1993 12:35:00   ED
// Added StealthCopy
//
//    Rev 1.1   21 Jun 1993 16:39:28   DLEVITON
//
//    Rev 1.0   17 Jun 1993 19:56:56   MARKL
// Initial revision.
//***************************************************************************

#ifndef INCL_SYMKRNL
#define INCL_SYMKRNL

#include "file.h"
#include <time.h>

#ifdef __cplusplus                      // Compiling for C++?
extern "C" {
#endif
//***************************************************************************
// Typedefs
//***************************************************************************

//***************************************************************************
// Prototypes
//***************************************************************************

#ifdef SYM_WIN32

BOOL WINAPI SymKRNLAttach (
    HINSTANCE   hInstance,
    DWORD       dwReason,
    LPVOID      lpReserved
    );

#else

BOOL WINAPI SymKRNLAttach (
    HINSTANCE   hInstance
    );

BOOL WINAPI SymKRNLDetach (
    VOID
    );

#endif


void    WINAPI VxDServicesTerminate();  
BOOL    WINAPI StealthCopy (LPVOID lpWorkPtr, LPVOID lpSrcPtr, UINT wLen);
LPSTR   WINAPI ExpandAndConvertFile (LPCSTR lpOriginal, LPSTR lpConverted);
time_t  WINAPI ConvertFileTimeToSeconds(LPFILETIME lpFileTime);
VOID    WINAPI ConvertSecondsToFileTime(LPFILETIME lpFileTime, time_t seconds);

#ifdef __cplusplus                      // Compiling for C++?
}
#endif
#endif

