// Copyright (C) 1997 Symantec Corporation
//*****************************************************************************
// $Header:   S:/FILELINK/VCS/FSLINK.CPv   1.7   22 Jun 1998 21:45:06   jtaylor  $
//
// Description:
//
//*****************************************************************************
// $Log:   S:/FILELINK/VCS/FSLINK.CPv  $
// 
//    Rev 1.7   22 Jun 1998 21:45:06   jtaylor
// Removed the call to update server quarantine header.  Due to a change in the quarantineItem, it is no longer needed.
//
//    Rev 1.6   14 May 1998 10:35:40   SEDWARD
// In ServerCreateQuarantineFile(), if cLink.Connect() fails we set the BOOL
// argument to FALSE.
//
//    Rev 1.5   07 May 1998 20:25:22   SEDWARD
// Fixed CodeReview/ET1 bugs.f
//
//    Rev 1.4   14 Apr 1998 12:49:46   dwilson
// Disabled body for LTrace so we could build the DEC Alpha (for now).
//
//    Rev 1.3   12 Apr 1998 20:47:00   SEDWARD
// Added ServerDeleteFile().
//
//    Rev 1.2   06 Apr 1998 14:25:54   SEDWARD
// ServerCreateQuarantineFile now takes an [out] buffer to save the name of the
// Quarantine file created on the server; added ServerUpdateQuarantineFileHeader
//
//    Rev 1.1   23 Mar 1998 14:07:40   SEDWARD
// Added 'ServerCreateQuarantineFile'.
//
//    Rev 1.0   17 Mar 1998 19:45:20   SEDWARD
// Initial revision.
//
//    Rev 1.0   28 Jan 1998 15:20:24   KSACKIN
// Initial revision.
//
//    Rev 1.1   27 Jan 1998 13:11:14   KSACKIN
// Updated a LPCSTR to an LPCTSTR so that we can compile this module in
// UNICODE.
//
//    Rev 1.0   27 Jan 1998 12:51:12   KSACKIN
// Initial revision.
//
//*****************************************************************************
#include <platform.h>

#include "LinkCli.h"
#include "LinkVal.h"

#include "FSLINK.h"
#include "ISLink.h"


#ifdef _DEBUG
#define MY_ASSERT(xP) if( (LPVOID)xP == NULL ) DebugBreak();
#else
#define MY_ASSERT(xP)
#endif



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// If you have not worked in this file before, please read the
// comment in Serial.h about the CLinkSerializer class
//  BEFORE modifying any of the functions in this file.
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#include "Serial.h"


/////////////////////////////////////////////////////////////////////
// Defines

#define COPY_BUFFER_SIZE  (0x20000)

/////////////////////////////////////////////////////////////////////
// External variables

extern CRITICAL_SECTION  gLinkCriticalSection;



/////////////////////////////////////////////////////////////////////
// Disable warning regarding SEH not calling object destructors.
// This is safe here because this is being handled explicitly in the
// exception handlers via deletion of dynamic objects.
//

#pragma warning(disable:4509)



/////////////////////////////////////////////////////////////////////
// Multi-use Functions


/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerPing(
//
// Purpose:
//    This function is to be used by anyone that wants to ping for
//    the presense of the Server Service.
//
// Parameters:
//    PRPC_LINK_INFO pLinkInfo - [in] - Information on how to contact the server
//
// Returns:
//    LINK_ERROR - Couldn't contact the service
//    TRUE       - Server was contacted
//
// Notes:
//
//
/////////////////////////////////////////////////////////////////////
// 12/02/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerPing(
    PRPC_LINK_INFO     pLinkInfo)
    {
                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

                                        // Validate incoming strings
    if (NULL == pLinkInfo)
        {

        }
    MY_ASSERT(pLinkInfo);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        dwReturn = LINK_ERROR;
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
        dwReturn = Server_Ping();
        }
    RpcExcept( RpcExceptionCode() )
        {
        SetLastError( RpcExceptionCode() );
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    return dwReturn;

    }  // end of "ServerPing"



/////////////////////////////////////////////////////////////////////
// Functions called by the Agent to get files and update status


/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerCreateFile(
//
// Purpose:
//    Create a file on the server machine.
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo              - [in]  - Where to connect to
//
//   LPDWORD         lphFile                - [out] - The return from the function
//
//   LPCTSTR         lpFileName             - [in]  -
//   DWORD           dwDesiredAccess        - [in]  -  The following parameters
//   DWORD           dwShareMode            - [in]  -  match that of the WIN32
//   DWORD           dwSecurityAttributes   - [in]  -  SDK for file creation.
//   BYTE*           lpSecurityAttributes   - [in]  -
//   DWORD           dwCreationDistribution - [in]  -
//   DWORD           dwFlagsAndAttributes   - [in]  -
//   DWORD           hTemplateFile          - [in]  -
//
//   LPDWORD         lpError                - [out] - Any error codes
//
// Returns:
//   ERROR_SUCCESS  - Function was executed on the remote machine
//   LINK_ERROR     - Couldn't contact the remote machine.
//
//   lphFile        - The returned file handle
//
// Notes:
//   For security reasons, the server will deny ANY requests
//   on the server machine for write access.  If write access is
//   requested, lpError will contain the value: ERROR_ACCESS_DENIED
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
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
    LPDWORD         lpError)
    {
                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

                                        // Validate incoming pointers
    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lphFile);
    MY_ASSERT(lpFileName);
    MY_ASSERT(lpError);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
                                        // Now create the file on the remote
                                        // machine.
        dwReturn = Server_CreateFile(
                       lphFile,
                       lpFileName,
                       dwDesiredAccess,
                       dwShareMode,
                       dwSecurityAttributes,
                       lpSecurityAttributes,
                       dwCreationDistribution,
                       dwFlagsAndAttributes,
                       hTemplateFile,
                       lpError);
        }
    RpcExcept( RpcExceptionCode() )
        {
        SetLastError( RpcExceptionCode() );
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

   return dwReturn;

}  // end of "ServerCreateFile"



/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerCreateQuarantineFile(
//
// Purpose:
//    Create a file on the server machine.
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo              - [in]  - Where to connect to
//
//   BOOL*           lpbReturn              - [out] - Function's return value
//
//   LPDWORD         lphFile                - [out] - The return from the function
//
//   LPCTSTR         lpFileName             - [in]  - The name of the source file
//                                                    on the client machine
//
//   DWORD           dwBufSize              - [in]  - The size of the output
//                                                    buffer
//
//   LPTSTR          lpServerFileName       - [out] - An output buffer that
//                                                    will contain the name
//                                                    of the Quarantine file
//                                                    created on the server
//
//   LPDWORD         lpError                - [out] - Any error codes
//
// Returns:
//   ERROR_SUCCESS  - Function was executed on the remote machine
//   LINK_ERROR     - Couldn't contact the remote machine.
//
//   lphFile        - The returned file handle
//
// Notes:
//
/////////////////////////////////////////////////////////////////////
// 03/18/98 - SEDWARD - Function Created
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerCreateQuarantineFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    LPDWORD         lphFile,
    LPCTSTR         lpFileName,
    DWORD           dwBufSize,
    LPTSTR          lpServerFileName,
    LPDWORD         lpError)
    {

    LTrace("BEGIN ServerCreateQuarantineFile");
    LTrace("  lpFileName = %s", lpFileName);


                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

                                        // Validate incoming pointers
    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lphFile);
    MY_ASSERT(lpFileName);
    MY_ASSERT(lpError);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        LTrace("  cLink.Connect() failed");
        *lpbReturn = FALSE;
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
        LTrace("  About to call Server_CreateQuarantineFile()");

                                        // Now create the file in the remote
                                        // machine's Quarantine directory
                                        // Note that this function always returns
                                        // ERROR_SUCCESS, this is where we get the
                                        // chance to return success
        dwReturn = Server_CreateQuarantineFile(
                       lpbReturn,
                       lphFile,
                       lpFileName,
                       dwBufSize,
                       (BYTE*)lpServerFileName,
                       lpError);
        if (FALSE == *lpbReturn)
            {
            dwReturn = *lpError ;
            }
        }
    RpcExcept( RpcExceptionCode() )
        {
        LTrace("  RPC Exception");
        SetLastError( RpcExceptionCode() );
        *lpbReturn = FALSE;
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    LTrace("END ServerCreateQuarantineFile");
    return dwReturn;

    }  // end of "ServerCreateQuarantineFile"



/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerDeleteFile(
//
// Purpose:
//    Delete a file on the server machine.
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo              - [in]  - Where to connect to
//
//   BOOL*           lpbReturn              - [out] - Function's return value
//
//   LPCTSTR         lpszServerFileName     - [in]  - The name of the source file
//                                                    on the client machine
//
//   LPDWORD         lpError                - [out] - Any error codes
//
// Returns:
//   ERROR_SUCCESS  - Function was executed on the remote machine
//   LINK_ERROR     - Couldn't contact the remote machine.
//
// Notes:
//
/////////////////////////////////////////////////////////////////////
// 04/12/98 - SEDWARD - Function Created
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerDeleteFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    LPCTSTR         lpszServerFileName,
    LPDWORD         lpError)
    {
    LTrace("BEGIN ServerDeleteFile");
    LTrace("  lpszServerFileName = %s", lpszServerFileName);

                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

                                        // Validate incoming pointers
    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lpszServerFileName);
    MY_ASSERT(lpError);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        LTrace("  cLink.Connect() failed");
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
        LTrace("  About to call Server_DeleteFile()");

                                        // Now delete the file from the remote server
                                        // machine's Quarantine directory
                                        // Note that this function always returns
                                        // ERROR_SUCCESS, this is where we get the
                                        // chance to return success
        dwReturn = Server_DeleteFile(
                       lpbReturn,
                       lpszServerFileName,
                       lpError);
        if (FALSE == *lpbReturn)
            {
            dwReturn = *lpError ;
            }
        }
    RpcExcept( RpcExceptionCode() )
        {
        LTrace("  RPC Exception");
        SetLastError( RpcExceptionCode() );
        *lpbReturn = FALSE;
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    LTrace("END ServerDeleteFile");
    return dwReturn;

    }  // end of "ServerDeleteFile"


/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerReadFile(
//
// Purpose:
//    This function allows us to Read a file from the server
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo              - [in]  - Where to connect to
//
//   BOOL*           lpbReturn              - [out] - Function's return value
//
//   DWORD           hFile                  -
//   DWORD           dwBufferSize           - The rest of the API matches
//   BYTE*           lpBuffer               - What the WIN32 WriteFile function
//   DWORD           nNumberOfBytesToRead   - does.
//   LPDWORD         lpNumberOfBytesRead    -
//
//   LPDWORD         lpError                - [out] - Any errors on remote machine
//
// Returns:
//   ERROR_SUCCESS  -  Remote function was executed successfully
//   LINK_ERROR     -  Server was inaccessible
//
//   lpbReturn -
//     TRUE  - Read was successful
//     FALSE - Read was NOT successful.
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerReadFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    DWORD           hFile,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer,
    DWORD           nNumberOfBytesToRead,
    LPDWORD         lpNumberOfBytesRead,
    LPDWORD         lpError)
    {
                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lpbReturn);
    MY_ASSERT(lpBuffer);
    MY_ASSERT(lpNumberOfBytesRead);
    MY_ASSERT(lpError);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
                                        // Read from the file!
                                        // Note that this function always returns
                                        // ERROR_SUCCESS, this is where we get the one
                                        // chance to return success
        dwReturn = Server_ReadFile(
                       lpbReturn,
                       hFile,
                       dwBufferSize,
                       lpBuffer,
                       nNumberOfBytesToRead,
                       lpNumberOfBytesRead,
                       lpError);
        if (FALSE == *lpbReturn)
            {
            dwReturn = *lpError ;
            }
        }
    RpcExcept( RpcExceptionCode() )
        {
        SetLastError( RpcExceptionCode() );
        *lpbReturn = FALSE;
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    return dwReturn;
    }



/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerWriteQuarantineFile(
//
// Purpose:
//    This function allows us to write a file to the server
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo              - [in]  - Where to connect to
//
//   BOOL*           lpbReturn              - [out] - Function's return value
//
//   DWORD           hFile                  - [in]  - The handle to the open file
//                                                    on the server
//
//   DWORD           dwBufferSize           - The rest of the API matches
//   BYTE*           lpBuffer               - What the WIN32 WriteFile function
//   DWORD           nNumberOfBytesToRead   - does.
//   LPDWORD         lpNumberOfBytesRead    -
//
//   LPDWORD         lpError                - [out] - Any errors on remote machine
//
// Returns:
//   ERROR_SUCCESS  -  Remote function was executed successfully
//   LINK_ERROR     -  Server was inaccessible
//
//   lpbReturn -
//     TRUE  - Read was successful
//     FALSE - Read was NOT successful.
//
/////////////////////////////////////////////////////////////////////
// 04/03/98 - SEDWARD - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerWriteQuarantineFile(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL*           lpbReturn,
    DWORD           hFile,
    DWORD           dwBufferSize,
    BYTE*           lpBuffer,
    DWORD           nNumberOfBytesToRead,
    LPDWORD         lpNumberOfBytesRead,
    LPDWORD         lpError)
    {
                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lpbReturn);
    MY_ASSERT(lpBuffer);
    MY_ASSERT(lpNumberOfBytesRead);
    MY_ASSERT(lpError);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
                                        // Write to the file!
                                        // Note that this function always returns
                                        // ERROR_SUCCESS, this is where we get the one
                                        // chance to return success
        dwReturn = Server_WriteQuarantineFile(
                       lpbReturn,
                       hFile,
                       dwBufferSize,
                       lpBuffer,
                       nNumberOfBytesToRead,
                       lpNumberOfBytesRead,
                       lpError);
        if (FALSE == *lpbReturn)
            {
            dwReturn = *lpError;
            }
        }
    RpcExcept( RpcExceptionCode() )
        {
        SetLastError( RpcExceptionCode() );
        *lpbReturn = FALSE;
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    return dwReturn;

    }  // end of "ServerWriteQuarantineFile"



/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerGetFileTime(
//
// Purpose:
//    This function allows us to get the time settings on a file
//    on the server machine.
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo              - [in]  - Where to connect to
//
//   BOOL*           lpbReturn              - [out] - Function's return value
//
//   DWORD           hFile                  -
//   LPFILETIME      lpCreationTime         - [out] - Time information
//   LPFILETIME      lpLastAccessTime       - [out] - about the file in
//   LPFILETIME      lpLastWriteTime        - [out] - question
//
//   LPDWORD         lpError                - [out] - Any errors on remote machine
//
// Returns:
//   ERROR_SUCCESS  -  Remote function was executed successfully
//   LINK_ERROR     -  Server was inaccessible
//
//   lpbReturn -
//     TRUE  - Query was successful
//     FALSE - Query was NOT successfule
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerGetFileTime(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL *     lpbReturn,
    DWORD      hFile,
    LPFILETIME lpCreationTime,
    LPFILETIME lpLastAccessTime,
    LPFILETIME lpLastWriteTime,
    LPDWORD lpError)
    {
                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lpbReturn);
    MY_ASSERT(lpCreationTime);
    MY_ASSERT(lpLastAccessTime);
    MY_ASSERT(lpLastWriteTime);
    MY_ASSERT(lpError);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
                                        // Get the File Time
        dwReturn = Server_GetFileTime(
                       lpbReturn,
                       hFile,
                       lpCreationTime,
                       lpLastAccessTime,
                       lpLastWriteTime,
                       lpError);
        }
    RpcExcept( RpcExceptionCode() )
        {
        SetLastError( RpcExceptionCode() );
        *lpbReturn = FALSE;
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    return dwReturn;
    }





/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerCloseHandle(
//
// Purpose:
//    This function allows us to close our handle on the server
//    machine.
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo  - [in]  - Where to connect to
//
//   BOOL*   lpbReturn          - [out] - Return from the function
//
//   DWORD   hObject            - [in]  - Handle to close
//
//   LPDWORD lpError            - [out] - GetLastError on remote machine.
//
// Returns:
//   ERROR_SUCCESS  -  Remote function was executed successfully
//   LINK_ERROR     -  Server was inaccessible
//
//   lpbReturn -
//     TRUE  - CloseHandle was successful
//     FALSE - CloseHandle was NOT successfule
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerCloseHandle(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL *  lpbReturn,
    DWORD   hObject,
    LPDWORD lpError)
    {
                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;

    DWORD dwReturn = LINK_ERROR;

    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lpbReturn);
    MY_ASSERT(lpError);

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        goto Cleanup;
        }

    RpcTryExcept
        {
                                        // Close the handle
                                        // Note that this function always returns
                                        // ERROR_SUCCESS, this is where we get the one
                                        // chance to return success
        dwReturn = Server_CloseHandle(
                                lpbReturn,
                                hObject,
                                lpError);
        if (FALSE == *lpbReturn)
            {
            dwReturn = *lpError;
            }
        }
    RpcExcept( RpcExceptionCode() )
        {
        SetLastError( RpcExceptionCode() );
        *lpbReturn = FALSE;
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    return dwReturn;
    }



/////////////////////////////////////////////////////////////////////
// DWORD __declspec(dllexport) WINAPI ServerCopyFileFromServer(
//
// Purpose:
//   This function will copy a file from the server machine to the
//   local machine
//
// Parameters:
//   PRPC_LINK_INFO  pLinkInfo    - [in]  - Where to connect to
//
//   BOOL*   lpbReturn            - [out] - Return from the function
//
//   LPCTSTR   lpszLocalFileName  - [in]  - Name of the local file to create
//   LPCTSTR   lpszServerFileName - [in]  - Name of the file on the server
//   LPDWORD lpError              - [out] - GetLastError()
//
// Returns:
//   dwReturn is used in this function as the return from ALL
//   RPC calls.  If we get a LINK_ERROR on any of the calls, we
//   immediately bail out of the function.
//
//   ERROR_SUCCESS  -  Remote function was executed successfully
//   LINK_ERROR     -  Server was inaccessible
//
//   lpbReturn -
//     TRUE  - File Copied Successfully
//     FALSE - File was NOT copied successfully.
//
/////////////////////////////////////////////////////////////////////
// 12/03/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD __declspec(dllexport) WINAPI ServerCopyFileFromServer(
    PRPC_LINK_INFO  pLinkInfo,
    BOOL *  lpbReturn,
    LPCTSTR lpszLocalFileName,
    LPCTSTR lpszServerFileName,
    LPDWORD lpError)
    {

    LTrace("BEGIN ServerCopyFileFromServer");
    LTrace("  lpszLocalFileName = %s, lpszServerFileName = %s", lpszLocalFileName, lpszServerFileName);


                                        // MUST HAVE THIS VARIABLE IN ALL RPC
                                        // FUNCTIONS!
    CLinkSerializer cLink;


    DWORD dwReturn = LINK_ERROR;

    HANDLE hLocalFile  = INVALID_HANDLE_VALUE;
    HANDLE hServerFile = INVALID_HANDLE_VALUE;

    BOOL  bReturn;

    LPBYTE lpDataBuffer = NULL;

    DWORD  dwBytesRead;
    DWORD  dwBytesWritten;

    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;

                                        // Verify Data

    MY_ASSERT(pLinkInfo);
    MY_ASSERT(lpbReturn);
    MY_ASSERT(lpszLocalFileName);
    MY_ASSERT(lpszServerFileName);
    MY_ASSERT(lpError);

                                        // Initialize the outgoing data
    *lpbReturn = FALSE;

                                        // Make the connection.
    if (!cLink.Connect(
             pLinkInfo,
             &ISLink_Handle))
        {
        SetLastError( RPC_S_SERVER_UNAVAILABLE );
        LTrace("  cLink.Connect FAILED");
        goto Cleanup;
        }

    RpcTryExcept
        {


/////////////////////////////////////////////////////////////////////
// Start by opening both of the files and setting up the copy buffer

        lpDataBuffer = new BYTE[COPY_BUFFER_SIZE];

        if (NULL == lpDataBuffer)
            {
            LTrace("  failed to allocate lpDataBuffer");
            *lpbReturn = FALSE;
            goto CopyCleanup;
            }

        hLocalFile = CreateFile (
                         lpszLocalFileName,
                         GENERIC_WRITE,
                         0,
                         NULL,
                         CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL );

        if (INVALID_HANDLE_VALUE == hLocalFile)
            {
            LTrace("  failed to open local file -- %s", lpszLocalFileName);
            *lpbReturn = FALSE;
            *lpError = GetLastError();
            goto CopyCleanup;
            }

        dwReturn = Server_CreateFile(
                       (LPDWORD) &hServerFile,
                       lpszServerFileName,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       0,               // sizeof SecurityAttributes
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL |
                       FILE_FLAG_SEQUENTIAL_SCAN,
                       NULL,
                       lpError);

                                        // Check the RPC return value
        if (LINK_ERROR == dwReturn)
            {
            LTrace("  Server_CreateFile(%s) failed (RPC value)", lpszServerFileName);
            goto CopyCleanup;
            }

                                        // Check the function return value
        if (INVALID_HANDLE_VALUE == hServerFile)
            {
            LTrace("  Server_CreateFile(%s) failed (bad file handle)", lpszServerFileName);
            goto CopyCleanup;
            }


/////////////////////////////////////////////////////////////////////
// Next, start the Read/Write cycle to get all of the remote
// information.

        do
            {

                                        // Try to get data from the remote
                                        // machine
            dwReturn = Server_ReadFile(
                              &bReturn,
                              (DWORD) hServerFile,
                              COPY_BUFFER_SIZE,
                              lpDataBuffer,
                              COPY_BUFFER_SIZE,
                              &dwBytesRead,
                              lpError);

                                        // Check the link first
            if (LINK_ERROR == dwReturn)
                {
                LTrace("  Server_ReadFile() has failed link");
                goto CopyCleanup;
                }

                                        // Check the read for success
            if (!bReturn)
                {
                LTrace("  Server_ReadFile() has failed read access");
                goto CopyCleanup;
                }

                                        // Check to see if we got any data.
            if (dwBytesRead > 0)
                {
                LTrace("  dwBytesRead = %ld", dwBytesRead);

                                        // If we did, lets write it locally!
                bReturn = WriteFile(
                             hLocalFile,
                             lpDataBuffer,
                             dwBytesRead,
                             &dwBytesWritten,
                             NULL);     // Overlapped structure

                                        // Check the write
                if (!bReturn)
                    {
                    LTrace("  WriteFile() failed");
                    *lpError = GetLastError();
                    goto CopyCleanup;
                    }

                                        // Check how much we wrote
                if (dwBytesWritten != dwBytesRead)
                    {
                    LTrace("  dwBytesWritten != dwBytesRead");
                    goto CopyCleanup;
                    }

                }

            }
        while (dwBytesRead > 0);


/////////////////////////////////////////////////////////////////////
// Now that we have the file copied, we need to update FILETIMEs and
// ATTRIBUTES

                                        // Get the File Time
        dwReturn = Server_GetFileTime(
                       &bReturn,
                       (DWORD) hServerFile,
                       &ftCreationTime,
                       &ftLastAccessTime,
                       &ftLastWriteTime,
                       lpError);

                                        // First, Check the link
        if (LINK_ERROR == dwReturn)
            {
            goto CopyCleanup;
            }

                                        // Then check the return value
        if (!bReturn)
            {
            goto CopyCleanup;
            }

                                        // Now set the local file time
        bReturn = SetFileTime(
                      hLocalFile,
                      &ftCreationTime,
                      &ftLastAccessTime,
                      &ftLastWriteTime);

                                        // Check to see if our local guy failed
        if (!bReturn)
            {
            *lpError = GetLastError();
            goto CopyCleanup;
            }

                                        // If we get here, we know that the
                                        // copy file command was successful.
        *lpbReturn = TRUE;

CopyCleanup:

        if (NULL != lpDataBuffer)
            {
            delete [] lpDataBuffer;
            lpDataBuffer = NULL;
            }

        if (INVALID_HANDLE_VALUE != hLocalFile)
            {
            CloseHandle (hLocalFile);
            hLocalFile = INVALID_HANDLE_VALUE;
            }

        if (INVALID_HANDLE_VALUE != hServerFile)
            {
            Server_CloseHandle(
                          &bReturn,
                          (DWORD) hServerFile,
                          lpError);
            if (FALSE == bReturn)
                {
                dwReturn = *lpError;
                }

            hServerFile = INVALID_HANDLE_VALUE;
            }

        }
    RpcExcept( RpcExceptionCode() )
        {
        SetLastError( RpcExceptionCode() );
        *lpbReturn = FALSE;
        dwReturn = LINK_ERROR;
        }
    RpcEndExcept

Cleanup:

    LTrace("END ServerCopyFileFromServer");
    return dwReturn;

    }  // end of "ServerCopyFileFromServer"







/////////////////////////////////////////////////////////////////////
// BOOL APIENTRY DllMain( HANDLE hModule, DWORD ulReason, LPVOID lpReserved )
//
// Purpose:
//    Entry point for the DLL
//
// Parameters:
//    HANDLE hModule    - Basically the instance handle
//    DWORD  dwReason   - Reason this function is being called
//    LPVOID lpReserved - No idea... Provided by Windows.
//
// Returns:
//    TRUE  - Success
//    FALSE - Look at the code... We will never fail.
//
/////////////////////////////////////////////////////////////////////
// 07-29-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, DWORD dwReason, LPVOID lpReserved )
    {
    switch( dwReason )
        {
        case DLL_PROCESS_ATTACH:
            InitializeCriticalSection(&gLinkCriticalSection);
            break;

        case DLL_PROCESS_DETACH:
            DeleteCriticalSection(&gLinkCriticalSection);
            break;
        }

    return( TRUE );
    }










/////////////////////////////////////////////////////////////////////////////
//
//  LTrace() - Function to trace output to a log fle put in the windows dir
//
//  Params: LPCSTR - Format string
//          Variable Args - To be substituted into format string
//
//  Return value:   VOID
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

VOID CDECL LTrace( LPCSTR lpszFmt, ... )
{
#if     0
    FILE           *fp = NULL;
    DWORD           dwNumBytes = 0L;
    TCHAR           szBuffer[MAX_PATH];
    TCHAR           szLogFile[MAX_PATH];


    LPSTR lpArgs = (LPSTR)&lpszFmt + sizeof( lpszFmt );
    wvsprintf( (LPSTR)szBuffer, lpszFmt, lpArgs );
    lstrcat( szBuffer, "\n" );

#ifdef _LOGFILE_TRACE_
    GetWindowsDirectory( szLogFile, MAX_PATH - 1 );
    lstrcat( szLogFile, "\\FILELINK.LOG");
    fp = fopen( szLogFile, "at" );

    if ( fp != NULL )
    {
        fputs( szBuffer, fp );
        fflush( fp );
        fclose( fp );
    }
#else
    OutputDebugString( szBuffer );
#endif
#endif

    return;
}
