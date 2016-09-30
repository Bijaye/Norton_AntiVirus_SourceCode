/////////////////////////////////////////////////////////////////////
// ISLINK.CPP - Created - 12/02/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/QUARSVC/VCS/islink.cpv   1.2   22 Jun 1998 21:52:54   jtaylor  $
//
// Description:
//    This is where the entry points from the RPC calls should go.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/QUARSVC/VCS/islink.cpv  $
// 
//    Rev 1.2   22 Jun 1998 21:52:54   jtaylor
// Removed UpdateServerQuarantineHeader since it is no longer needed.
//
//    Rev 1.1   10 May 1998 22:42:18   SEDWARD
// Added BOOL argument to Server_CreateQuarantineFile().
//
//    Rev 1.0   22 Apr 1998 22:00:16   DBuches
// Initial revision.
//
//    Rev 1.5   20 Apr 1998 22:50:24   SEDWARD
// Disabled LOGFILE_TRACE.
//
//    Rev 1.4   12 Apr 1998 20:51:26   SEDWARD
// Added Serve_DeleteFile().
//
//    Rev 1.3   07 Apr 1998 17:46:14   SEDWARD
// Use 'NameReturnFile'.
//
//    Rev 1.2   06 Apr 1998 14:52:54   SEDWARD
// Added Server_UpdateQuarantineFileHeader.
//
//    Rev 1.1   23 Mar 1998 14:27:06   SEDWARD
// Added 'Server_CreateQuarantineFile', 'Server_WriteFile', now include
// 'xapi.h' and 'QuarAdd.h'.
//
//    Rev 1.0   17 Mar 1998 19:42:22   SEDWARD
// Initial revision.
//
//    Rev 1.0   27 Jan 1998 12:24:30   KSACKIN
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "xapi.h"
#include "QuarAdd.h"
#include "ole2.h"
#include "Quar32.h"
#include "quarsvc.h"
#define INITGUID
#include "IQuaran.h"            // COM interfaces
#include "ISLink.h"             // Generated at compile time by MIDL


extern CQuarantineService QuarService;


// SCOTT
// #define _LOGFILE_TRACE_
VOID CDECL LTrace( LPCSTR lpszFmt, ... );
BOOL    FileExists(LPTSTR  lpszFile);


/////////////////////////////////////////////////////////////////////
// This is for general purpose use.

DWORD Server_Ping(void)
    {
    return ERROR_SUCCESS;
    }



/////////////////////////////////////////////////////////////////////
// These functions are for the file copy process when the Agent is
// getting files from the Server.

/////////////////////////////////////////////////////////////////////
// DWORD Server_CreateFile(
//
// Purpose:
//   This function is to be used by the Agent to open a file on the
//   Server machine.
//
// Parameters:
//    LPDWORD   lphFile                - [out] - This is the return value
//                                               from CreateFile
//
//    LPCTSTR   lpFileName             - [in]  - These parameters
//    DWORD     dwDesiredAccess        - [in]  - fit one to one with
//    DWORD     dwShareMode            - [in]  - the Win32 function
//    DWORD     dwSecurityAttributes   - [in]  - CreateFile
//    BYTE*     lpSecurityAttributes   - [in]  -
//    DWORD     dwCreationDistribution - [in]  -
//    DWORD     dwFlagsAndAttributes   - [in]  -
//    DWORD     hTemplateFile          - [in]  -
//    LPDWORD   lpError                - [out] - If an error occurrs, this
//                                               will be GetLastError()
//
// Returns:
//    ERROR_SUCCESS - We will always return this indicating that
//                    we received the function call.
//
// Notes:
//    We will NOT allow write access on the server from a remote
//    process.  We will return ERROR_ACCESS_DENIED
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD Server_CreateFile(
    LPDWORD         lphFile,
    LPCTSTR         lpFileName,
    DWORD           dwDesiredAccess,
    DWORD           dwShareMode,
    DWORD           dwSecurityAttributes,
    BYTE*           lpSecurityAttributes,
    DWORD           dwCreationDistribution,
    DWORD           dwFlagsAndAttributes,
    DWORD           hTemplateFile,
    LPDWORD         lpError)
    {

    if (GENERIC_WRITE == (dwDesiredAccess & GENERIC_WRITE))
        {
        *lpError = ERROR_ACCESS_DENIED;
        goto Cleanup;
        }

    *lphFile = (DWORD) CreateFile(
                           lpFileName,
                           dwDesiredAccess,
                           dwShareMode,
                           (0 == dwSecurityAttributes) ?
                               NULL :
                               (LPSECURITY_ATTRIBUTES) lpSecurityAttributes,
                           dwCreationDistribution,
                           dwFlagsAndAttributes,
                           (HANDLE) hTemplateFile);

    if (INVALID_HANDLE_VALUE == ((HANDLE)*lphFile))
        {
        *lpError = GetLastError();
        }
    else
        {
        // Add handle to open file handle list
        QuarService.m_handleMonitor.AddHandleConnection( (HANDLE) *lphFile );
        }

Cleanup:
    return ERROR_SUCCESS;
    }



/////////////////////////////////////////////////////////////////////
// DWORD Server_CreateQuarantineFile(
//
// Purpose:
//   This function creates a new, unique file in the Quarantine directory
//   on the Server machine.
//
// Parameters:
//    BOOL*     lpbReturn              - [out] - TRUE if all goes well, FALSE
//                                               if not
//
//    LPDWORD   lphFile                - [out] - This is the return value
//                                               from CreateFile
//
//    LPCTSTR   lpFileName             - [in]  - The original filename
//                                               (sans path info)
//
//    DWORD     dwBufSize              - [in]  - The size of the output buffer
//
//    LPTSTR    lpszBuf                - [out] - The output buffer that will store
//                                               the name of the server Quarantine
//                                               file
//
//    LPDWORD   lpError                - [out] - If an error occurrs, this
//                                               will be GetLastError()
//
// Returns:
//    ERROR_SUCCESS - We will always return this indicating that
//                    we received the function call.
//
/////////////////////////////////////////////////////////////////////
// 03/18/98 - SEDWARD - Function Created
/////////////////////////////////////////////////////////////////////
DWORD Server_CreateQuarantineFile(
    BOOL*           lpbReturn,
    LPDWORD         lphFile,
    LPCTSTR         lpFileName,
    DWORD           dwBufSize,
    BYTE*           lpszBuf,
    LPDWORD         lpError)
    {
    auto    BOOL        bResult = TRUE;
    auto    HANDLE      newFileHandle = INVALID_HANDLE_VALUE;
    auto    TCHAR       szNewFilename[MAX_PATH];
    auto    TCHAR       szQuarPath[MAX_PATH];
    auto    TCHAR       szFileExtension[MAX_PATH];


    LTrace("BEGIN Server_CreateQuarantineFile");

    // initialize our BOOLean return value
    *lpbReturn = TRUE;

    // get the path to the Quarantine directory
    bResult = GetQuarantinePath(szQuarPath, MAX_PATH, TARGET_QUARANTINE_PATH);
    if (FALSE == bResult)
        {
        LTrace("  Failed to get Quarantine path");
        *lpbReturn = FALSE;
        *lpError = ERROR_PATH_NOT_FOUND;
        goto Cleanup;
        }
    LTrace("  szQuarPath = %s", szQuarPath);


    // get the extension from the original filename, we want to use the same
    // extension for the Quarantined file
    NameReturnExtensionEx(lpFileName, szFileExtension, MAX_QUARANTINE_EXTENSION_LENGTH);
    LTrace("  lpFileName = %s, szFileExtension = %s", lpFileName, szFileExtension);

    // create a new file in the Quarantine directory
    *lphFile = (DWORD)CreateUniqueFile(szQuarPath, szFileExtension, szNewFilename);
    LTrace("  szNewFilename = %s", szNewFilename);

    // if any errors occurred, get the last error code
    if (INVALID_HANDLE_VALUE == ((HANDLE)*lphFile))
        {
        LTrace("  Got an INVALID_HANDLE_VALUE");
        *lpbReturn = FALSE;
        *lpError = GetLastError();
        goto Cleanup;
        }
    else
        {
        // Add handle to open file handle list
        QuarService.m_handleMonitor.AddHandleConnection( (HANDLE) *lphFile );
        }

    // save the name of the new server Quarantine file
    if (NULL == lstrcpyn((LPTSTR)lpszBuf, szNewFilename, dwBufSize))
        {
        LTrace("  lstrcpyn(%s, %s, %ld) failed", lpszBuf, szNewFilename, dwBufSize);
        *lpbReturn = FALSE;
        *lpError = GetLastError();
        }


Cleanup:
    LTrace("END Server_CreateQuarantineFile");
    return ERROR_SUCCESS;

    }  // end of "Server_CreateQuarantineFile"



/////////////////////////////////////////////////////////////////////
// DWORD Server_DeleteFile(
//
// Purpose:
//   This function deletes a file from the Quarantine directory on the
//   Server machine.
//
// Parameters:
//    BOOL*     lpbReturn              - [out] - Return value from
//                                               DeleteFile()
//
//    LPCTSTR   lpFileName             - [in]  - The target filename
//                                               (sans path info)
//
//    LPDWORD   lpError                - [out] - If an error occurrs, this
//                                               will be GetLastError()
//
// Returns:
//    ERROR_SUCCESS - We will always return this indicating that
//                    we received the function call.
//
/////////////////////////////////////////////////////////////////////
// 04/12/98 - SEDWARD - Function Created
/////////////////////////////////////////////////////////////////////
DWORD Server_DeleteFile(
    BOOL*           lpbReturn,
    LPCTSTR         lpFileName,
    LPDWORD         lpError)
    {

    LTrace("BEGIN Server_DeleteFile");

    if (0 != DeleteFile(lpFileName))
        {
        *lpbReturn = TRUE;
        }
    else
        {
        LTrace("  DeleteFile(%s), failed", lpFileName);
        *lpbReturn = FALSE;
        }

    LTrace("END Server_DeleteFile");
    return ERROR_SUCCESS;

    }  // end of "Server_DeleteFile"



/////////////////////////////////////////////////////////////////////
// DWORD Server_ReadFile(
//
// Purpose:
//    This function will allow the agent to read the file that
//    he has open on the server machine.
//
// Parameters:
//    BOOL*   lpbReturn            - [out] - Return value from ReadFile
//
//    DWORD   hFile                - [in]  - Handle of file to read from
//    DWORD   dwBufferSize         - [in]  - Size of lpBuffer
//    BYTE*   lpBuffer             - [out] - Information back to caller
//    DWORD   nNumberOfBytesToRead - [in]  - Number of bytes to read
//    LPDWORD lpNumberOfBytesRead  - [out] - Number of bytes actually read
//    LPDWORD lpError              - [out] - If an error occurrs, GetLastError()
//
// Returns:
//    ERROR_SUCCESS - We will always return this indicating that
//                    we received the function call.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD Server_ReadFile(
    BOOL*           lpbReturn,
    DWORD           hFile,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer,
    DWORD           nNumberOfBytesToRead,
    LPDWORD         lpNumberOfBytesRead,
    LPDWORD         lpError)
    {

    *lpbReturn = ReadFile(
                     (HANDLE) hFile,
                     (LPVOID) lpBuffer,
                     nNumberOfBytesToRead,
                     lpNumberOfBytesRead,
                     (LPOVERLAPPED) NULL);  // Don't allow overlapped I/O

    if (FALSE == *lpbReturn)
        *lpError = GetLastError();
    else
        {
        // Update last access time for this file
        QuarService.m_handleMonitor.UpdateConnectionTime( (HANDLE) hFile );
        }

    return ERROR_SUCCESS;
    }


/////////////////////////////////////////////////////////////////////
// DWORD Server_WriteQuarantineFile(
//
// Purpose:
//    This function will allow the agent to write to the file that
//    is open on the server machine.
//
// Parameters:
//    BOOL*   lpbReturn            - [out] - Return value from WriteFile
//
//    DWORD   hFile                - [in]  - Handle of file to read from
//    DWORD   dwBufferSize         - [in]  - Size of lpBuffer
//    BYTE*   lpBuffer             - [out] - Information back to caller
//    DWORD   nNumberOfBytesToWrite- [in]  - Number of bytes to write
//    LPDWORD lpNumberOfBytesWrite - [out] - Number of bytes actually written
//    LPDWORD lpError              - [out] - If an error occurrs, GetLastError()
//
// Returns:
//    ERROR_SUCCESS - We will always return this indicating that
//                    we received the function call.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 03/18/98 - SEDWARD - Function Created (thanks Dan!)
/////////////////////////////////////////////////////////////////////
DWORD Server_WriteQuarantineFile(
    BOOL*           lpbReturn,
    DWORD           hFile,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer,
    DWORD           nNumberOfBytesToWrite,
    LPDWORD         lpNumberOfBytesWrite,
    LPDWORD         lpError)
    {

    *lpbReturn = WriteFile(
                     (HANDLE) hFile,
                     (LPVOID) lpBuffer,
                     nNumberOfBytesToWrite,
                     lpNumberOfBytesWrite,
                     (LPOVERLAPPED) NULL);  // Don't allow overlapped I/O

    if (FALSE == *lpbReturn)
        {
        *lpError = GetLastError();
        }
    else
        {
        // Update last access time for this file
        QuarService.m_handleMonitor.UpdateConnectionTime( (HANDLE) hFile );
        }

    return ERROR_SUCCESS;
    }


/////////////////////////////////////////////////////////////////////
// DWORD Server_GetFileTime(
//
// Purpose:
//    This function will allow the Agent Service to get the file
//    time associated with the file that it is trying to copy.
//
// Parameters:
//    BOOL *     lpbReturn        - [out] - Return value of GetFileTime()
//
//    DWORD      hFile            - [in]  - File handle to get times of
//    LPFILETIME lpCreationTime   - [out] - Information requested
//    LPFILETIME lpLastAccessTime - [out] - Information requested
//    LPFILETIME lpLastWriteTime  - [out] - Information requested
//    LPDWORD    lpError          - [out] - If an error occurrs, GetLastError()
//
// Returns:
//    ERROR_SUCCESS - We will always return this indicating that
//                    we received the function call.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD Server_GetFileTime(
    BOOL *     lpbReturn,
    DWORD      hFile,
    LPFILETIME lpCreationTime,
    LPFILETIME lpLastAccessTime,
    LPFILETIME lpLastWriteTime,
    LPDWORD    lpError)
    {

    *lpbReturn = GetFileTime(
                     (HANDLE) hFile,
                     lpCreationTime,
                     lpLastAccessTime,
                     lpLastWriteTime);

    if (FALSE == *lpbReturn)
        *lpError = GetLastError();

    return ERROR_SUCCESS;
    }


/////////////////////////////////////////////////////////////////////
// DWORD Server_CloseHandle(
//
// Purpose:
//    This allows the Agent to close file handles on the Server.
//
// Parameters:
//    BOOL *  lpbReturn  - [out] - Return value from CloseHandle
//
//    DWORD   hObject    - [in]  - Handle to close
//    LPDWORD lpError    - [out] - If an error occurrs, GetLastError()
//
// Returns:
//    ERROR_SUCCESS - We will always return this indicating that
//                    we received the function call.
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD Server_CloseHandle(
    BOOL *  lpbReturn,
    DWORD   hObject,
    LPDWORD lpError)
    {

    *lpbReturn = CloseHandle(
                    (HANDLE) hObject);

    if (FALSE == *lpbReturn)
        *lpError = GetLastError();
    else
        {
        // Update last access time for this file
        QuarService.m_handleMonitor.RemoveHandleConnection( (HANDLE) hObject );
        }


    return ERROR_SUCCESS;
    }



