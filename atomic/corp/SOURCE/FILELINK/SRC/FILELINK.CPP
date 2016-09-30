/////////////////////////////////////////////////////////////////////
// FILELINK.CPP - Created - 12/03/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/FILELINK/VCS/Filelink.cpv   1.5   07 May 1998 20:24:58   SEDWARD  $
//
// Description:
//    This will be the implementation file for the FileLink library.
//    Within this file will be the definiton of the
//    CRemoteFileTransfer class as well as exported functions for
//    remote IO through a CLink connection.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/FILELINK/VCS/Filelink.cpv  $
// 
//    Rev 1.5   07 May 1998 20:24:58   SEDWARD
// Updated call to ServerDeleteFile().
//
//    Rev 1.4   20 Apr 1998 22:06:08   SEDWARD
// Removed SendQuarantineFile().
//
//    Rev 1.3   12 Apr 1998 20:44:58   SEDWARD
// SendQuarantineFile now deletes the file on the server if the local file
// cannot be found (cleanup stuff); added DeleteServerFile().
//
//    Rev 1.2   06 Apr 1998 14:25:24   SEDWARD
// Enhanced SendQuarantineFile.
//
//    Rev 1.1   23 Mar 1998 14:06:56   SEDWARD
// Added 'SendQuarantineFile'.
//
//    Rev 1.0   17 Mar 1998 19:45:14   SEDWARD
// Initial revision.
//
//    Rev 1.3   28 Jan 1998 15:21:12   KSACKIN
// Updated a header file dependency from slink.h to fslink.h.  This is the new
// header file that we are going to use to connect to the File Server RPC
// Server.  We renamed the header file to not collide with the NSDU files.
//
//    Rev 1.2   27 Jan 1998 12:37:46   KSACKIN
// Updated the EXPORT tag on my exported function to be FILELINK_EXPORT.  This
// is to avoid any unneccessary confision about my export tag.
//
//    Rev 1.1   04 Dec 1997 10:45:42   KSACKIN
// Exported all class functions, as well as the remote file IO API.
//
//    Rev 1.0   03 Dec 1997 23:25:02   KSACKIN
// Initial revision.
//
//    Rev 1.0   03 Dec 1997 23:23:56   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "filelink.h"
#include "fslink.h"
#include "QuarAdd.h"



/////////////////////////////////////////////////////////////////////
// CRemoteFileTransfer Class.


FILELINK_API CRemoteFileTransfer::CRemoteFileTransfer()
{
}

FILELINK_API CRemoteFileTransfer::~CRemoteFileTransfer()
{
}

FILELINK_API void CRemoteFileTransfer::SetTargetName(
    LPCTSTR szTargetName
    )
{
    _tcscpy( m_szAddress,  szTargetName );
    _tcscpy( m_szProtocol, _T("") );
    _tcscpy( m_szEndpoint, _T("") );
}

void SafeCopyString( LPTSTR szDest, LPCTSTR szSrc )
{
    if ( NULL == szSrc )
        {
        _tcscpy( szDest, _T("") );
        }
    else
        {
        _tcscpy( szDest, szSrc );
        }
}

FILELINK_API void CRemoteFileTransfer::SetTargetName(
    LPCTSTR szProtocol,
    LPCTSTR szAddress,
    LPCTSTR szEndpoint
    )
{
    SafeCopyString( m_szAddress,  szAddress );
    SafeCopyString( m_szProtocol, szProtocol );
    SafeCopyString( m_szEndpoint, szEndpoint );
}


FILELINK_API LPCTSTR CRemoteFileTransfer::GetTargetName( void )
{
    return m_szAddress;
}



// ==== CRemoteFileTransfer::DeleteServerFile =============================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

FILELINK_API FILE_TRANSFER_STATUS CRemoteFileTransfer::DeleteServerFile(
                                                    LPCTSTR  lpszServerFileName)
{
    auto    BOOL                bReturn = TRUE;
    auto    DWORD               dwError = 0;
    auto    RPC_LINK_INFO       rLinkInfo;

    LTrace("BEGIN CRemoteFileTransfer::DeleteServerFile");
    LTrace("  lpszServerFileName = %s", lpszServerFileName);

    // initialize the RPC information structure
    rLinkInfo.pszAddress  = m_szAddress;
    rLinkInfo.pszProtocol = m_szProtocol;
    if (0 == _tcslen(m_szEndpoint))
        {
        LTrace("  m_szEndpoint = NULL");
        rLinkInfo.pszEndpoint = NULL;
        }
    else
        {
        LTrace("  m_szEndpoint = %s", m_szEndpoint);
        rLinkInfo.pszEndpoint = m_szEndpoint;
        }

    if (ERROR_SUCCESS != ServerDeleteFile(&rLinkInfo, &bReturn
                                                    , lpszServerFileName
                                                    , &dwError))
        {
        bReturn = FALSE;
        LTrace("  ServerDeleteFile failed");
        }

    LTrace("END CRemoteFileTransfer::DeleteServerFile");
    return (bReturn);

}  // end of "CRemoteFileTransfer::DeleteServerFile"





FILELINK_API FILE_TRANSFER_STATUS CRemoteFileTransfer::SendFile(
    LPCTSTR szLocalFileName,
    LPCTSTR szRemoteFileName
    )
{

    return 0;

}  // end of "CRemoteFileTransfer::SendFile"



FILELINK_API FILE_TRANSFER_STATUS CRemoteFileTransfer::GetFile(
    LPCTSTR szRemoteFileName,
    LPCTSTR szLocalFileName
    )
{
    RPC_LINK_INFO   rLinkInfo;
    DWORD           dwError;
    BOOL            bReturn = FALSE;


    LTrace("BEGIN CRemoteFileTransfer::GetFile");

    rLinkInfo.pszAddress  = m_szAddress;
    rLinkInfo.pszProtocol = m_szProtocol;

    LTrace("  m_szAddress = %s, m_szProtocol = %s", m_szAddress, m_szProtocol);
    LTrace("  rLinkInfo.pszAddress = %s, rLinkInfo.pszProtocol = %s", rLinkInfo.pszAddress, rLinkInfo.pszProtocol);

    if ( 0 == _tcslen( m_szEndpoint ) )
        {
        LTrace("  rLinkInfo.pszEndpoint = NULL");
        rLinkInfo.pszEndpoint = NULL;
        }
    else
        {
        LTrace("  rLinkInfo.pszEndpoint = %s", rLinkInfo.pszEndpoint);
        rLinkInfo.pszEndpoint = m_szEndpoint;
        }

    if ( ERROR_SUCCESS != ServerCopyFileFromServer(
        &rLinkInfo,
        &bReturn,
        szLocalFileName,
        szRemoteFileName,
        &dwError ) )
        {
        LTrace("  ServerCopyFileFromServer() returned FALSE");
        bReturn = FALSE;
        }
    else
        {
        LTrace("  ServerCopyFileFromServer() returned TRUE");
        }

    LTrace("END CRemoteFileTransfer::GetFile");
    return bReturn;
}


/////////////////////////////////////////////////////////////////////
// Remote W32 API's

FILELINK_API HANDLE RemoteCreateFile(
    LPCTSTR   lpFileName,
    DWORD     dwDesiredAccess,
    DWORD     dwShareMode,
    DWORD     cbSecurityAttributes,
    BYTE*     lpSecurityAttributes,
    DWORD     dwCreationDistribution,
    DWORD     dwFlagsAndAttributes,
    DWORD     hTemplateFile
    )
{
    return NULL;
}

FILELINK_API BOOL RemoteWriteFile(
    HANDLE    hFile,
    DWORD     cbBufferSize,
    BYTE*     lpBuffer,
    DWORD     nNumberOfBytesToWrite,
    LPDWORD   lpNumberOfBytesWritten
    )
{
    return TRUE;
}

FILELINK_API BOOL RemoteReadFile(
    HANDLE    hFile,
    DWORD     cbBufferSize,
    BYTE*     lpBuffer,
    DWORD     nNumberOfBytesToRead,
    LPDWORD   lpNumberOfBytesRead
    )
{
    return TRUE;
}

FILELINK_API BOOL RemoteCloseHandle(
    HANDLE    hObject
    )
{
    return TRUE;
}

FILELINK_API BOOL RemoteGetDiskFreeSpace(
    LPDWORD   lpError,
    LPCTSTR   lpRootPathName,
    LPDWORD   lpSectorsPerCluster,
    LPDWORD   lpBytesPerSector,
    LPDWORD   lpNumberOfFreeClusters,
    LPDWORD   lpTotalNumberOfClusters
    )
{
    return TRUE;
}

FILELINK_API BOOL RemoteCreateDirectoryTree(
    LPTSTR    lpszDirectoryTree
    )
{
    return TRUE;
}

FILELINK_API BOOL RemoteRemoveDirectoryTree(
    LPTSTR    lpszDirectoryTree
    )
{
    return TRUE;
}



