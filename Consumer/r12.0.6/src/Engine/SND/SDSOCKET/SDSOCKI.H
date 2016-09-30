// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SDSOCKET/VCS/sdsocki.h_v   1.4   16 Jul 1998 17:25:58   CNACHEN  $
//
// Description:
//  Contains scan and deliver socket client implementation class declarations
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/SDSOCKET/VCS/sdsocki.h_v  $
// 
//    Rev 1.4   16 Jul 1998 17:25:58   CNACHEN
// Updated to:
// 
// #define MAX_HTTP_GET_ENCODE_LENGTH      1000
// #define MAX_HTTP_GET_LENGTH             1000
// 
// So we properly encode up to 16 hashes.. Used to be 256 and 512 which was not
// enough.
// 
//    Rev 1.3   08 Jun 1998 11:08:04   CNACHEN
// Added support for either numeric or symbolic internet addresses.
// 
//    Rev 1.2   27 May 1998 13:19:40   ECHIEN
// Added HTTP_VERSION
// 
//    Rev 1.1   07 May 1998 16:47:40   DCHI
// Changed GET_PATH definition.
// 
//    Rev 1.0   16 Mar 1998 18:01:26   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _SDSOCKI_H_

#define _SDSOCKI_H_

#define HTTP_PORT_NUM                   80
#define MAX_HTTP_GET_ENCODE_LENGTH      1000
#define MAX_HTTP_GET_LENGTH             1000
#define NUM_PRIORITIES                  3
#define MAX_HTTP_RESULT_LENGTH          512

#define MAX_RESPONSE_LINES              100

//GET /cgi-bin/sockcgi.exe?CRC=<BASE64-ENCODED-PACKAGE-DATA>&PRIORITY=PLATINUM  HTTP/1.0 [crlf]

#define GET_PATH "cgi-bin/sockcgi.exe"
#define HTTP_VERSION "HTTP/1.0"

#define HIGH_PRIORITY                   "PLATINUM"
#define MEDIUM_PRIORITY                 "CORPORATE"
#define LOW_PRIORITY                    "CUSTOMER"

// responses from the server

#define BACK_END_DATA_ERROR             1
#define BACK_END_SEND_PACKAGE           2   // via SMTP
#define BACK_END_FP_USE_LU              3   // false positive
#define BACK_END_BUSY_TRY_LATER         4   // busy!



// structure for submission of filter data

typedef struct
{
    DWORD                       dwType;
    DWORD                       dwVersion;
    DWORD                       dwSize;
} SUBMISSION_HEADER_T, FAR *LPSUBMISSION_HEADER;

class SocketClientImp : public SocketClient
{
    public:

        int GetBackEndStatus
        (
            LPTSTR              lpszServerName,
            int                 nPriorityCode,
            LPBYTE              lpbyPreSubmissionData
        );

        void Release
        (
            void
        );

    private:

        BOOL IsNumericAddress
        (
            LPTSTR      lpszServerName
        );

        void MIMEEncode64TriToQuad
        (
            LPBYTE              lpbyTri,
            LPTSTR              lpszQuad
        );

        BOOL ConvertDataToText
        (
            LPBYTE              lpbyData,
            LPTSTR              lpszText,
            int                 nMaxLength
        );

        BOOL SendLine
        (
            LPTSTR              lpszText
        );

        BOOL GetLine
        (
            LPTSTR              lpszResult,
            int                 nMaxLength
        );

        BOOL Connect
        (
            LPTSTR              lpszServerName
        );

        BOOL Disconnect
        (
            void
        );

        BOOL GetResponse
        (
            LPINT               lpnResponse
        );

        // data

        SOCKET                  m_socket;
};

//
//
//CLIENT connects to the server.
//
//CLIENT:
//
//GET /SCANANDDELIVER?CRC=<BASE64-ENCODED-PACKAGE-DATA>&PRIORITY=PLATINUM  HTTP/1.0 [crlf]
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
//    The following values can be returned by the SOCKI server:
//
//    00000001    BACK_END_DATA_ERROR
//    00000002    BACK_END_SEND_PACKAGE   (via SMTP)
//    00000003    BACK_END_FP_USE_LU  (false positive)
//    00000004    BACK_END_BUSY_TRY_LATER (busy)
//

#endif // #ifndef _SDSOCKI_H_
