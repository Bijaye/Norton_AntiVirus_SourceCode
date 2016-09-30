/////////////////////////////////////////////////////////////////////
// FSLINK.H - Created - 12/01/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/Fslink.h_v   1.7   07 May 1998 20:24:04   SEDWARD  $
//
// Description:
//    This is where the Server Link information will go.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/Fslink.h_v  $
// 
//    Rev 1.7   07 May 1998 20:24:04   SEDWARD
// Added BOOL argument to ServerCreateQuarantineFile and ServerDeleteFile.
//
//    Rev 1.6   21 Apr 1998 19:24:26   SEDWARD
// Added 'platform.h'.
//
//    Rev 1.5   14 Apr 1998 12:49:30   dwilson
// Re-enabled the function prototype for LTrace (oops!).
//
//    Rev 1.4   14 Apr 1998 12:29:16   SEDWARD
// Disabled LOGFILE_TRACE.
//
//    Rev 1.3   12 Apr 1998 20:47:10   SEDWARD
// Added ServerDeleteFile().
//
//    Rev 1.2   06 Apr 1998 14:26:44   SEDWARD
// ServerCreateQuarantineFile now takes an [out] buffer to save the name of the
// Quarantine file created on the server; added ServerUpdateQuarantineFileHeader
//
//    Rev 1.1   23 Mar 1998 14:08:08   SEDWARD
// Added 'ServerCreateQuarantineFile'.
//
//    Rev 1.0   17 Mar 1998 19:47:08   SEDWARD
// Initial revision.
//
//    Rev 1.0   28 Jan 1998 15:18:28   KSACKIN
// Initial revision.
//
//    Rev 1.1   27 Jan 1998 13:12:08   KSACKIN
// Updated an LPCSTR to an LPCTSTR so that we can compile this module in
// UNICODE.
//
//    Rev 1.0   27 Jan 1998 12:52:02   KSACKIN
// Initial revision.
//
//    Rev 1.7   15 Dec 1997 16:09:22   DSACKIN
// Added ServerReloadOptions
//
//    Rev 1.6   11 Dec 1997 15:13:32   DSACKIN
// Added dwJobID and dwTargetID for all of the copy file commands.
//
//    Rev 1.5   05 Dec 1997 12:32:24   DSACKIN
// Added ServerCopyFileFromServer
//
//    Rev 1.4   04 Dec 1997 13:37:08   DSACKIN
// Added a prototype for SelectProtocol
//
//    Rev 1.3   03 Dec 1997 19:20:44   DSACKIN
// Added ServerGetJobCount
//
//    Rev 1.2   03 Dec 1997 12:38:02   DSACKIN
// Added copy file functions.
//
//    Rev 1.1   02 Dec 1997 14:47:08   DSACKIN
// Added new functions for StartJob StopJob UpdateStatus
//
//    Rev 1.0   02 Dec 1997 10:41:42   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__FSLINK_H)
#define __FSLINK_H

#include "stdio.h"
#include "platform.h"
// #define _LOGFILE_TRACE_
VOID CDECL LTrace( LPCSTR lpszFmt, ... );

#include "linkinfo.h"

/////////////////////////////////////////////////////////////////////
// Function Prototypes


/////////////////////////////////////////////////////////////////////
// Multi-purpose functions

DWORD __declspec(dllexport) WINAPI ServerPing(
    PRPC_LINK_INFO     pLinkInfo);

BOOL __declspec(dllexport) WINAPI SelectProtocol(
    PRPC_LINK_INFO     pLinkInfo,
    LPFNPINGFUNCTION   lpfnPing);



/////////////////////////////////////////////////////////////////////
// File copy functions

DWORD __declspec(dllexport) WINAPI ServerCreateFile(
    PRPC_LINK_INFO  pLinkInfo,
    LPDWORD         lphFile,
    LPCTSTR         lpFileName,
    DWORD           dwDesiredAccess,
    DWORD           dwShareMode,
    DWORD           dwSecurityAttributes,
    BYTE*           lpSecurityAttributes,
    DWORD           dwCreationDistribution,
    DWORD           dwFlagsAndAttributes,
    DWORD           hTemplateFile,
    LPDWORD         lpError);

DWORD __declspec(dllexport) WINAPI ServerCreateQuarantineFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    LPDWORD         lphFile,
    LPCTSTR         lpFileName,
    DWORD           dwBufSize,
    LPSTR           lpServerFileName,
    LPDWORD         lpError);

DWORD __declspec(dllexport) WINAPI ServerDeleteFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    LPCTSTR         lpszServerFileName,
    LPDWORD         lpError);

DWORD __declspec(dllexport) WINAPI ServerReadFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    DWORD           hFile,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer,
    DWORD           nNumberOfBytesToRead,
    LPDWORD         lpNumberOfBytesRead,
    LPDWORD         lpError);

DWORD __declspec(dllexport) WINAPI ServerWriteQuarantineFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    DWORD           hFile,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer,
    DWORD           nNumberOfBytesToRead,
    LPDWORD         lpNumberOfBytesRead,
    LPDWORD         lpError);

DWORD __declspec(dllexport) WINAPI ServerGetFileTime(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL *     lpbReturn,
    DWORD      hFile,
    LPFILETIME lpCreationTime,
    LPFILETIME lpLastAccessTime,
    LPFILETIME lpLastWriteTime,
    LPDWORD    lpError);

DWORD __declspec(dllexport) WINAPI ServerCloseHandle(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL *  lpbReturn,
    DWORD   hObject,
    LPDWORD lpError);

DWORD __declspec(dllexport) WINAPI ServerCopyFileFromServer(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL *  lpbReturn,
    LPCTSTR lpszLocalFileName,
    LPCTSTR lpszServerFileName,
    LPDWORD lpError);

DWORD __declspec(dllexport) WINAPI ServerUpdateQuarantineFileHeader(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    LPCTSTR         lpFileName,
    LPDWORD         lpError);

#endif // if !defined(__FSLINK_H)