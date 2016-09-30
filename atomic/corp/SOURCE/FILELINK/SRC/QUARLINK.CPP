/////////////////////////////////////////////////////////////////////
// QUARLINK.cpp - Created - 12/03/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/FILELINK/VCS/QuarLink.cpv   1.6   30 Jun 1998 20:40:24   sedward  $
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/FILELINK/VCS/QuarLink.cpv  $
// 
//    Rev 1.6   30 Jun 1998 20:40:24   sedward
// Moved DWORD vars to top of function in 'SendQuarantineFile'.
//
//    Rev 1.5   30 Jun 1998 20:38:50   sedward
// Dynamically allocate the Quarantine file transfer buffer.
//
//    Rev 1.4   22 Jun 1998 21:45:06   jtaylor
// Removed the call to update server quarantine header.  Due to a change in the quarantineItem, it is no longer needed.
//
//    Rev 1.3   08 Jun 1998 19:42:12   SEDWARD
// Added support for callback function for the progress bar when forwarding
// files from client to server.
//
//    Rev 1.2   07 May 1998 20:26:10   SEDWARD
// Fixed CodeReview/ET1 bugs.
//
//    Rev 1.1   20 Apr 1998 22:08:04   SEDWARD
//
/////////////////////////////////////////////////////////////////////


#include "platform.h"
#include "LinkInfo.h"
#include "FSLink.h"
#include "FileLink.h"
#include "QuarLink.h"
#include "QuarAdd.h"



FILELINK_API CQuarantineFileTransfer::CQuarantineFileTransfer()
{
}

FILELINK_API CQuarantineFileTransfer::~CQuarantineFileTransfer()
{
}


// ==== CQuarantineFileTransfer::SendQuarantineFile =======================
//
//  This function is called by a client Quarantine to forward a file to a
//  server Quarantine.
//
//  Input:
//      lpszLocalFileName   -- the name of the Quarantine file in the client
//                             Quarantine
//
//      lpfnForwardProgress -- a pointer to an optional callback function;
//                             the callback function can be used to display
//                             the progress of files being forwarded
//
//      dwPtrBytesSent      -- supports the callback; keeps a count of the
//                             number of bytes sent so far
//
//      dwPtrTotalBytes     -- supports the callback; stores the total number
//                             of bytes to send
//
// ========================================================================
//  Function created: 4/98, SEDWARD
//  06/22/98 - JTAYLOR - Removed ServerUpdateQuarantineFileHeader no longer
//                       needed.
// ========================================================================

FILELINK_API FILE_TRANSFER_STATUS CQuarantineFileTransfer::SendQuarantineFile(
                                            LPCTSTR  lpszLocalFileName
                                            , FPFORWARDPROGRESS  lpfnForwardProgress
                                            , DWORD*  dwPtrBytesSent
                                            , DWORD*  dwPtrTotalBytes)
{
    auto    BOOL                bReturn = FALSE;
    auto    BOOL                bResult = TRUE;
    auto    BOOL                bTempBool;
    auto    LPBYTE              ptrTempBuf = NULL;
    auto    DWORD               dwBufSize = MAX_PATH;
    auto    DWORD               dwBytesRead = 0;
    auto    DWORD               dwBytesWritten = 0;
    auto    DWORD               dwError = ERROR_SUCCESS;
    auto    HANDLE              hLocalFile = INVALID_HANDLE_VALUE;
    auto    HANDLE              hServerFile = INVALID_HANDLE_VALUE;
    auto    RPC_LINK_INFO       rLinkInfo;
    auto    TCHAR               szServerFilename[MAX_PATH];


    LTrace("BEGIN CQuarantineFileTransfer::SendQuarantineFile");
    LTrace("  lpszLocalFileName = %s", lpszLocalFileName);

    // allocate memory for the file transfer buffer
    ptrTempBuf = new BYTE[QUARANTINE_TRANSFER_BUFFER_SIZE];
    if(NULL == ptrTempBuf)
        {
        bReturn = FALSE;
        LTrace("  Memory allocation for file transfer buffer failed");
        goto  Cleanup;
        }
    memset(ptrTempBuf, 0, QUARANTINE_TRANSFER_BUFFER_SIZE);

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

    // create the Quarantine file on the server
    ServerCreateQuarantineFile(&rLinkInfo, &bResult, (LPDWORD)&hServerFile
                                                        , lpszLocalFileName
                                                        , dwBufSize
                                                        , szServerFilename
                                                        , &dwError);
    if (FALSE == bResult)
        {
        bReturn = FALSE;
        LTrace("  ServerCreateQuarantineFile failed");
        goto  Cleanup;
        }

    // send the local file data to the server file
    if (INVALID_HANDLE_VALUE == hServerFile)
        {
        bReturn = FALSE;
        LTrace("  ServerCreateQuarantineFile failed returned an invalid SERVER file handle");
        goto  Cleanup;
        }


    // open up the local file
    hLocalFile = CreateFile(lpszLocalFileName, GENERIC_READ, 0, NULL
                                                    , OPEN_EXISTING
                                                    , FILE_FLAG_SEQUENTIAL_SCAN
                                                    , NULL);
    if (INVALID_HANDLE_VALUE == hLocalFile)
        {
        bReturn = FALSE;
        ServerCloseHandle(&rLinkInfo, &bTempBool, (DWORD)hServerFile, &dwError);
        hServerFile = INVALID_HANDLE_VALUE;
        DeleteServerFile(szServerFilename);
        LTrace("  ServerCreateQuarantineFile failed returned an invalid LOCAL file handle");
        goto  Cleanup;
        }

    do  {
        bResult = ReadFile(hLocalFile, ptrTempBuf, QUARANTINE_TRANSFER_BUFFER_SIZE
                                                            , &dwBytesRead, NULL);
        if ((FALSE != bResult)  &&  (dwBytesRead > 0))
            {
            // write the file to the server
            ServerWriteQuarantineFile(&rLinkInfo, &bReturn
                                                , (DWORD)hServerFile
                                                , QUARANTINE_TRANSFER_BUFFER_SIZE
                                                , ptrTempBuf
                                                , dwBytesRead
                                                , &dwBytesWritten
                                                , &dwError);
            if (FALSE == bReturn)
                {
                bResult = FALSE;
                }
            else if (NULL != lpfnForwardProgress)
                {
                // if the caller has supplied a callback function, update the
                // byte counters; the callback's return value indicates if the
                // user has canceled the forward operation
                *dwPtrBytesSent += dwBytesWritten;
                bResult = lpfnForwardProgress(dwPtrBytesSent, dwPtrTotalBytes);
                }
            }

        } while ((FALSE != bResult)  &&  (dwBytesRead > 0));

    // close file handles
    ServerCloseHandle(&rLinkInfo, &bTempBool, (DWORD)hServerFile, &dwError);
    CloseHandle(hLocalFile);
    hServerFile = INVALID_HANDLE_VALUE;
    hLocalFile = INVALID_HANDLE_VALUE;

Cleanup:

    // free the buffer
    if(NULL != ptrTempBuf)
        {
        delete []ptrTempBuf;
        ptrTempBuf = NULL;
        }

    // close file handles
    if (INVALID_HANDLE_VALUE != hServerFile)
        {
        ServerCloseHandle(&rLinkInfo, &bTempBool, (DWORD)hServerFile, &dwError);
        }

    if (INVALID_HANDLE_VALUE != hLocalFile)
        {
        CloseHandle(hLocalFile);
        }


    LTrace("END CQuarantineFileTransfer::SendQuarantineFile");
    return bReturn;

}  // end of "CQuarantineFileTransfer::SendQuarantineFile"