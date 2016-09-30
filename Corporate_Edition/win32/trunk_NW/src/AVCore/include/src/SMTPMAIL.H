// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/SMTPMAIL.H_v   1.0   25 Apr 1997 12:11:56   JBRENNA  $
//
// Description:
//  Header file for SMTP mail module.
//
// Contains:
//  AVISInitializeStats()
//  AVISClosesStats()
//  AVISSubmitSessionStats()
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/SMTPMAIL.H_v  $
// 
//    Rev 1.0   25 Apr 1997 12:11:56   JBRENNA
// Initial revision.
// 
//    Rev 1.1   13 Dec 1996 17:25:54   RAY
// added prototype
// 
//    Rev 1.0   20 Nov 1996 11:37:38   AOONWAL
// Initial revision.
// 
//    Rev 1.2   04 Nov 1996 14:03:12   CNACHEN
// Added error checking in greeting
// 
// 
//    Rev 1.1   22 Oct 1996 12:11:16   AOONWAL
// No change.
// 
//    Rev 1.0   24 Sep 1996 20:05:04   DJesch
// Initial revision.
// 
//    Rev 1.0   24 Sep 1996 17:34:40   DJesch
// Initial revision.
// 
//************************************************************************

#ifndef _SMTPMAIL_H_

#define _SMTPMAIL_H_

#include "ossocket.h"

// Function return values

typedef int SMTP_STATUS;

#define SMTP_STATUS_OK          0
#define SMTP_STATUS_ERROR       1

// Error code definitions
//  An error code is only set if the function
//      returns SMTP_STATUS_ERROR.

#define SMTP_ERROR_DNS_NAME_CONVERSION              1L
#define SMTP_ERROR_INET_ADDR_CONVERSION             2L
#define SMTP_ERROR_SOCKET_CREATION                  3L
#define SMTP_ERROR_SERVER_CONNECTION                4L
#define SMTP_ERROR_GREETING_RECEIPT                 5L
#define SMTP_ERROR_GREETING_SEND                    6L
#define SMTP_ERROR_GREETING_REPLY                   7L
#define SMTP_ERROR_MAILFROM_SEND                    8L
#define SMTP_ERROR_MAILFROM_REPLY                   9L
#define SMTP_ERROR_RECIPIENT_BEFORE_SENDER          10L
#define SMTP_ERROR_RCPTTO_SEND                      11L
#define SMTP_ERROR_RCPTTO_REPLY                     12L
#define SMTP_ERROR_DATA_BEFORE_SENDER_OR_RECIPIENT  13L
#define SMTP_ERROR_DATA_SEND                        14L
#define SMTP_ERROR_DATA_REPLY                       15L
#define SMTP_ERROR_DATA_BEFORE_DATA_START           16L
#define SMTP_ERROR_SENDING_DATA                     17L
#define SMTP_ERROR_INCOMPLETE_MSG_BEFORE_FINISH     18L
#define SMTP_ERROR_END_OF_DATA_SEND                 19L
#define SMTP_ERROR_CLOSE_SOCKET                     20L
#define SMTP_ERROR_BAD_GREETING                     21L

// SMTP connection structure

#define SMTP_CONNECTION_FLAG_SENDER_SPECIFIED       0x00000001UL
#define SMTP_CONNECTION_FLAG_RECIPIENT_SPECIFIED    0x00000002UL
#define SMTP_CONNECTION_FLAG_DATA_SPECIFIED         0x00000004UL
#define SMTP_CONNECTION_FLAG_AT_BEGINNING_OF_LINE   0x00000008UL

typedef struct tagSMTP_CONNECTION
{
    DWORD           dwError;

    DWORD           dwFlags;    // State flags
    HSOCKET         hSocket;     // Socket connection to SMTP server
} SMTP_CONNECTION_T, FAR *LPSMTP_CONNECTION;

// Internet email address list

#define MAX_EMAIL_ADDR          256

typedef struct tagINTERNET_EMAIL_LIST   INTERNET_EMAIL_LIST_T,
    FAR *LPINTERNET_EMAIL_LIST;

typedef struct tagINTERNET_EMAIL_LIST
{
    LPINTERNET_EMAIL_LIST   lpNext;
    char                    szAddress[MAX_EMAIL_ADDR];
} INTERNET_EMAIL_LIST_T, FAR *LPINTERNET_EMAIL_LIST;

//////////////////////////////////////////////////////////////////////
// Function prototypes
//////////////////////////////////////////////////////////////////////

// Actually sending mail

SMTP_STATUS SMTPInitiateMailConnection
(
    LPSMTP_CONNECTION       lpstSMTPConnection,
    LPSOCKET_ADDRESS        lpstSMTPServerAddress,
    LPSTR                   lpszSender
);

SMTP_STATUS SMTPConnectionAddRecipient
(
    LPSMTP_CONNECTION       lpstSMTPConnection,
    LPSTR                   lpszRecipient
);

SMTP_STATUS SMTPConnectionStartData
(
    LPSMTP_CONNECTION       lpstSMTPConnection
);

SMTP_STATUS SMTPConnectionSendData
(
    LPSMTP_CONNECTION       lpstSMTPConnection,
    LPSTR                   lpszBuf
);

SMTP_STATUS SMTPFinishMailConnection
(
    LPSMTP_CONNECTION       lpstSMTPConnection
);

// This function is used for sending a series of data parts
//  separately using SMTPConnectionSendData().  The connection
//  should be completed by calling SMTPFinishMailConnection().

SMTP_STATUS SMTPSendMailStart
(
    LPSMTP_CONNECTION       lpstSMTPConnection,
    LPSOCKET_ADDRESS        lpstSMTPServerAddress,
    LPSTR                   lpszSender,
    LPINTERNET_EMAIL_LIST   lpstRecipientList,
    LPSTR                   lpszSubject
);

SMTP_STATUS SMTPSendMailStartEx
(
    LPSMTP_CONNECTION       lpstSMTPConnection,
    LPSOCKET_ADDRESS        lpstSMTPServerAddress,
    LPSTR                   lpszSender,
    LPINTERNET_EMAIL_LIST   lpstRecipientList,
    LPSTR                   lpszSubject,
    LPSTR                   lpszAddlHdrs
);
// Top level send mail function

SMTP_STATUS SMTPSendMail
(
    LPSOCKET_ADDRESS   lpstSMTPServerAddress,
    LPSTR                   lpszSender,
    LPINTERNET_EMAIL_LIST   lpstRecipientList,
    LPSTR                   lpszSubject,
    LPSTR                   lpszMessage,
    LPDWORD                 lpdwError
);

// Function prototypes

#endif // #ifndef _SMTPMAIL_H_

