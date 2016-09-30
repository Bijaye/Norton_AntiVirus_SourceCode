// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/SDSOCKET.H_v   1.3   27 May 1998 14:22:34   jtaylor  $
//
// Description:
//  Contains scan and deliver socket client class declarations
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/SDSOCKET.H_v  $
// 
//    Rev 1.3   27 May 1998 14:22:34   jtaylor
// Undoing changes.
// 
//    Rev 1.1   14 May 1998 15:20:32   CNACHEN
// Added BACK_END_ALL_CLEAN_STATUS.
// 
//    Rev 1.0   16 Mar 1998 18:01:04   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _SDSOCKET_H_

#define _SDSOCKET_H_

#define BACK_END_CONNECTION_ERROR   0
#define BACK_END_DATA_ERROR         1
#define BACK_END_SEND_PACKAGE       2
#define BACK_END_FP_USE_LU          3
#define BACK_END_BUSY_TRY_LATER     4
#define BACK_END_ALL_CLEAN          5

#define PLATINUM_PRIORITY           0
#define CORPORATE_PRIORITY          1
#define CUSTOMER_PRIORITY           2

class SocketClient
{
    public:

//************************************************************************
// int GetBackEndStatus(lpszServerName, nPriorityCode, lpbyPreSubmissionData)
//
// Purpose
//
//  This function connects to the SARC HTTP server to query the back end's
//  status.
//
// Parameters:
//
//  lpszServerName: www.sarcbackstatus.com or similar
//  nPriorityCode: PLATINUM_PRIORITY, CORPORATE_PRIORITY or CUSTOMER_PRIORITY
//  lpbyResSubmissionData: Provided by the packaging API
//
// Returns:
//
//  BACK_END_CONNECTION_ERROR if there is an error connecting to the back end
//  BACK_END_DATA_ERROR       if there is a data error during xmission
//  BACK_END_SEND_PACKAGE     if the back end is up and is accepting packages
//  BACK_END_FP_USE_LU        if the back end has determined that the
//                            submission is definitely an FP; client app should
//                            tell the user to use LiveUpdate within the next
//                            week.
//  BACK_END_BUSY_TRY_LATER   if the back end needs to throttle submissions
//
//************************************************************************
// 3/16/98 Carey created.
//************************************************************************

        virtual int GetBackEndStatus
        (
            LPTSTR              lpszServerName,
            int                 nPriorityCode,
            LPBYTE              lpbyPreSubmissionData
        ) = 0;

        virtual void Release
        (
            void
        ) = 0;
};

#ifdef _SDSOCKET_CPP_
#define SDSocketDLLImportExport __declspec(dllexport)
#else
#define SDSocketDLLImportExport __declspec(dllimport)
#endif

//************************************************************************
// SocketClient *CreateInstance(void)
//
// Purpose
//
//  Returns a new instance of the socket client class.
//
// Parameters:
//
//  none
//
// Returns:
//
//  A pointer to a socket client or NULL.
//
//************************************************************************

class SDSocketDLLImportExport SocketClientFactory
{
    public:

        static SocketClient *CreateInstance();
};


//
//
//CLIENT connects to the server.
//
//CLIENT:
//
//GET /SCANANDDELIVER?CRCDATA=<BASE64-ENCODED-PACKAGE-DATA>&PRIORITY=PLATINUM  HTTP/1.0 [crlf]
//[crlf]
//
//SERVER:
//
//HTTP/1.0 200 OK         [crlf]
//Content-type: text/html         [crlf]
//[crlf]
//<ASCII response - "00001" for example>  [crlf]
//
//SERVER closes connection.
//
//    The following values can be returned by the socket server:
//
//    00000001    BACK_END_DATA_ERROR
//    00000002    BACK_END_SEND_PACKAGE   (via SMTP)
//    00000003    BACK_END_FP_USE_LU  (false positive)
//    00000004    BACK_END_BUSY_TRY_LATER (busy)
//    00000005    BACK_END_ALL_CLEAN (no infections)
//

#endif // #ifndef _SDSOCKI_H_
