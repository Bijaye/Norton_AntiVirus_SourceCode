/////////////////////////////////////////////////////////////////////
// FILELINK.H - Created - 12/03/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/FILELINK.H_v   1.3   20 Apr 1998 22:05:56   SEDWARD  $
//
// Description:
//    This file will define the interface for the file over the wire
//    implementation.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/FILELINK.H_v  $
// 
//    Rev 1.3   20 Apr 1998 22:05:56   SEDWARD
// Removed SendQuarantineFile().
//
//    Rev 1.2   12 Apr 1998 20:46:26   SEDWARD
// Added DeleteServerFile().
//
//    Rev 1.1   23 Mar 1998 14:07:18   SEDWARD
// Added 'SendQuarantineFile'.
//
//    Rev 1.0   17 Mar 1998 19:49:04   SEDWARD
// Initial revision.
//
//    Rev 1.3   28 Jan 1998 15:46:58   KSACKIN
// Removed the CLinkCli member from the CRemoteFileTransfer class.  We do not
// need this as our linking and sending of files happens outside of this class,
// and the link client will already be declared.
//
//    Rev 1.2   27 Jan 1998 12:38:46   KSACKIN
// Updated my export tag from EXPORT to FILELINK_API to avoid confusion.
//
//    Rev 1.1   04 Dec 1997 10:43:22   KSACKIN
// Added the EXPORT term to CRemoteFileTransfer and to all of my exported
// API's.  Also, I added an ifdef to determine if we are building the DLL, or
// if we are including this header file for a client application.  If we are
// building the dll, we define this as dllexport, otherwise, dllimport.
//
//    Rev 1.0   03 Dec 1997 23:23:36   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__FILELINK_H)
#define __FILELINK_H

#ifdef BUILDING_FILELINK_DLL
    #define FILELINK_API __declspec(dllexport)
#else
    #define FILELINK_API __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////
// Definitions.

#define FILE_TRANSFER_STATUS DWORD  // See header for values.

#define MAX_TARGET_LENGTH 262       // Maximum target length.

/////////////////////////////////////////////////////////////////////
// Class Definitions.


class FILELINK_API CRemoteFileTransfer
{
public:
    CRemoteFileTransfer();
    ~CRemoteFileTransfer();

    void SetTargetName(             // Set the name of the target machine.
        LPCTSTR szTargetName);      // [IN] Name of machine to send files to.

    void SetTargetName(             // Set the name of the target machine.
         LPCTSTR szProtocol,        // [IN] Protocol to use.
         LPCTSTR szAddress,         // [IN] Name of machine to send files to.
         LPCTSTR szEndpoint);       // [IN] Endpoint to connect to.

    LPCTSTR GetTargetName( void );  // Get the name of the current target.

    FILE_TRANSFER_STATUS SendFile(  // Send file to a remote machine.
        LPCTSTR szLocalFileName,    // [IN] Local file to send.
        LPCTSTR szRemoteFileName);  // [IN] Remote file name to copy file to.

    FILE_TRANSFER_STATUS DeleteServerFile(  // Delete a file from the server machine.
        LPCTSTR  lpszServerFileName);       // [IN] Full path to remote file to delete.

    FILE_TRANSFER_STATUS GetFile(   // Get a file from a remote machine.
        LPCTSTR szRemoteFileName,   // [IN] Remote file to retrieve.
        LPCTSTR szLocalFileName);   // [IN] Local file name for incoming file.

protected:
    TCHAR m_szProtocol[MAX_TARGET_LENGTH];
    TCHAR m_szAddress[MAX_TARGET_LENGTH];
    TCHAR m_szEndpoint[MAX_TARGET_LENGTH];
};

/////////////////////////////////////////////////////////////////////
// Function Prototypes.

FILELINK_API HANDLE RemoteCreateFile(
    LPCTSTR   lpFileName,
    DWORD     dwDesiredAccess,
    DWORD     dwShareMode,
    DWORD     cbSecurityAttributes,
    BYTE*     lpSecurityAttributes,
    DWORD     dwCreationDistribution,
    DWORD     dwFlagsAndAttributes,
    DWORD     hTemplateFile);

FILELINK_API BOOL RemoteWriteFile(
    HANDLE    hFile,
    DWORD     cbBufferSize,
    BYTE*     lpBuffer,
    DWORD     nNumberOfBytesToWrite,
    LPDWORD   lpNumberOfBytesWritten);

FILELINK_API BOOL RemoteReadFile(
    HANDLE    hFile,
    DWORD     cbBufferSize,
    BYTE*     lpBuffer,
    DWORD     nNumberOfBytesToRead,
    LPDWORD   lpNumberOfBytesRead);

FILELINK_API BOOL RemoteCloseHandle(
    HANDLE    hObject);

FILELINK_API BOOL RemoteGetDiskFreeSpace(
    LPDWORD   lpError,
    LPCTSTR   lpRootPathName,
    LPDWORD   lpSectorsPerCluster,
    LPDWORD   lpBytesPerSector,
    LPDWORD   lpNumberOfFreeClusters,
    LPDWORD   lpTotalNumberOfClusters);

FILELINK_API BOOL RemoteCreateDirectoryTree(
    LPTSTR    lpszDirectoryTree);

FILELINK_API BOOL RemoteRemoveDirectoryTree(
    LPTSTR    lpszDirectoryTree);

#endif // if !defined(__FILELINK_H)