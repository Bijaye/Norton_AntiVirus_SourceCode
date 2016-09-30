/////////////////////////////////////////////////////////////////////
// QUARLINK.H - Created - 12/03/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/INCLUDE/VCS/QuarLink.h_v   1.1   08 Jun 1998 19:42:42   SEDWARD  $
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/QuarLink.h_v  $
// 
//    Rev 1.1   08 Jun 1998 19:42:42   SEDWARD
// Added support for callback function for the progress bar when forwarding
// files from client to server.
//
//    Rev 1.0   20 Apr 1998 22:33:34   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#ifndef __QUARLINK_H
#define __QUARLINK_H

#include "IQuaran.h"    // for FPFORWARDPROGRESS definition
#include "FileLink.h"


class FILELINK_API CQuarantineFileTransfer : public CRemoteFileTransfer
{
public:
    CQuarantineFileTransfer();
    ~CQuarantineFileTransfer();

    FILE_TRANSFER_STATUS SendQuarantineFile(        // Send file to a remote Quarantine
        LPCTSTR szLocalFileName                     // [IN] Local file to send.
        , FPFORWARDPROGRESS  lpfnForwardProgress    // [IN] Callback function for progress bar
        , DWORD*  dwPtrBytesSent                    // [IN] Pointer to number of bytes sent
        , DWORD*  dwPtrTotalBytes);                 // [IN] Pointer to total number of bytes to send
};


#endif // #ifndef __QUARLINK_H