// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// Notifications.cpp:
//
// Implementation of the CIMailStorage notifications
//      Insertion of warning messages
//      Email alerts
//*************************************************************************


#include "stdafx.h"
#include <new>
#include <assert.h>

#include "vpcommon.h"
#include "clientreg.h"

#include "MailComm.h"
#include "IMailStorage.h"
#include "ModifyMail.h"

#include "smtpmail.h"
#include "resource.h"

#include <exception>

//***************************************************************************
// Externals
//***************************************************************************

#ifdef __cplusplus
extern "C" 
{
#endif

extern HINSTANCE g_hInstance;

#ifdef __cplusplus
}
#endif

//*************************************************************************
// Static helpers
//*************************************************************************

static bool SendMail ( LPCTSTR     lpszMailServer,
                       WORD        wMailPort,
                       LPCTSTR     lpszReversePath,
                       LPCTSTR     lpszSender,
                       LPCTSTR     lpszRecipients,
                       LPCTSTR     lpszSubject,
                       LPCTSTR     lpszMessage,
                       LPDWORD     lpdwSMTPError );
static void BuildRecipientList( LPCTSTR lpszRecipients, LPINTERNET_EMAIL_LIST *lppRecipientList );
static void DeleteRecipientList( LPINTERNET_EMAIL_LIST *lppRecipientList );

typedef enum _ALERT_MESSAGE_TYPE {
    ALERT_WARNING = 0,
    ALERT_SENDER,
    ALERT_SELECTED
} ALERT_MESSAGE_TYPE;

static LPCTSTR BuildAlertMessageBody( LPCTSTR lpszMessage, PIEM_ATTACHMENT_INFO_LIST pAttachmentInfo, ALERT_MESSAGE_TYPE nType );
static void DeleteAlertMessageBody( LPCTSTR lpszMessage );


//*************************************************************************
// CIMailStorage::doNotifications()
//
// After scanning for a node (email message) is completed, this routine
// checks options and makes required warning message insertions and
// sends alert emails.
//
// DWORD CIMailStorage::doNotifications( PSNODE pSNode )
//
// Parameters:
//      pSNode
//
// Return Values:
//
// Remarks:
//      Called by BeginRTSWatch() after node is scanned in call to 
//      ProcessRTSNode()
//*************************************************************************
// 2003.08.24 DALLEE - created.
//*************************************************************************

DWORD CIMailStorage::doNotifications( PSNODE pSNode )
{
    auto    PIEM_NODE_CONTEXT   pNodeContext;
    auto    PIEM_MAIL_INFO      pMailInfo;

    auto    LPCTSTR lpszMessageBody;
    auto    LPTSTR  lpszEncodedMessageBody;
    auto    int     cchEncodedMessageBody;
    auto    DWORD   dwSMTPError;
    auto    DWORD   dwResult;

    auto    TCHAR   szSymantecAntiVirus [ 100 ];
    auto    char    szEncodedSubject [ MAX_RFC821_LINE ];

    lpszEncodedMessageBody = NULL;
    dwResult = ERROR_SUCCESS;

    pNodeContext = static_cast<PIEM_NODE_CONTEXT>( pSNode->Context );
    pMailInfo = s_pStorage->m_commBuffer.GetMailInfoBlockFromIndex( pNodeContext->dwMailInfoBufferIndex );

    if ( NULL == pNodeContext->pAttachmentInfo )
    {
        // No infection information...
        return ( ERROR_SUCCESS );
    }

    // Message modifications...

    if ( s_pStorage->m_options.bInsertWarning )
    {
        lpszMessageBody = BuildAlertMessageBody( pNodeContext->szWarningMessage, pNodeContext->pAttachmentInfo, ALERT_WARNING );

        InsertWarningMessage( pSNode->InternalPath,
                              lpszMessageBody ? lpszMessageBody : pNodeContext->szWarningMessage,
                              s_pStorage->m_options.bChangeMessageSubject ? pNodeContext->szWarningSubject : NULL );

        DeleteAlertMessageBody( lpszMessageBody );
    }

    // Get message body sender information ready...

    szSymantecAntiVirus[0] = _T('\0');
    LoadString( g_hInstance, IDS_FROM_SYMANTEC_ANTIVIRUS, szSymantecAntiVirus, sizeof( szSymantecAntiVirus ) / sizeof( *szSymantecAntiVirus ) );

    // If we're set to do email alerts to sender....

    if ( pNodeContext->bAlertSender && pMailInfo && 
         pMailInfo->szSender[0] &&
         s_pStorage->m_options.SenderMailServer[0] )
    {
        // Create message body from warning message and per attachment infection messages

        lpszMessageBody = BuildAlertMessageBody( pNodeContext->szSenderMessage, pNodeContext->pAttachmentInfo, ALERT_SENDER );
        EncodeSubject( szEncodedSubject, pNodeContext->szSenderSubject, sizeof( szEncodedSubject ) );

        // Convert warning message to default email character encoding

        if ( lpszMessageBody )
        {
            cchEncodedMessageBody = 2 * _tcslen( lpszMessageBody ) + 1;
			try
			{
	            lpszEncodedMessageBody = new TCHAR [ cchEncodedMessageBody ];
			}
			catch (std::bad_alloc &) {}

            if ( lpszEncodedMessageBody )
            {
                if ( 0 == ConvertWindowsString( CP_ACP, GetDefaultEmailCodePage(), lpszMessageBody, lpszEncodedMessageBody, cchEncodedMessageBody ) )
                {
                    // Just use the unencoded info on error....
                    _tcscpy( lpszEncodedMessageBody, lpszMessageBody );
                }
            }
        }
        
        SendMail( s_pStorage->m_options.SenderMailServer,
                  s_pStorage->m_options.SenderMailPort,
                  s_pStorage->m_options.SenderReversePath,
                  szSymantecAntiVirus,
                  pMailInfo->szSender,
                  szEncodedSubject,
                  lpszEncodedMessageBody ? lpszEncodedMessageBody : pNodeContext->szSenderMessage,
                  &dwSMTPError );

        delete [] lpszEncodedMessageBody;
        lpszEncodedMessageBody = NULL;

        DeleteAlertMessageBody( lpszMessageBody );
    }

    // If we're set to do email alerts to selected recipients....

    if ( s_pStorage->m_options.bEmailSelected &&
         s_pStorage->m_options.SelectedRecipients[0] &&
         s_pStorage->m_options.SelectedMailServer[0] )
    {
        // Create message body from warning message and per attachment infection messages

        lpszMessageBody = BuildAlertMessageBody( pNodeContext->szSelectedMessage, pNodeContext->pAttachmentInfo, ALERT_SELECTED );
        EncodeSubject( szEncodedSubject, pNodeContext->szSelectedSubject, sizeof( szEncodedSubject ) );
        
        // Convert warning message to default email character encoding

        if ( lpszMessageBody )
        {
            cchEncodedMessageBody = 2 * _tcslen( lpszMessageBody ) + 1;
			try
			{
				lpszEncodedMessageBody = new TCHAR [ cchEncodedMessageBody ];
			}
			catch (std::bad_alloc &) {}

            if ( lpszEncodedMessageBody )
            {
                if ( 0 == ConvertWindowsString( CP_ACP, GetDefaultEmailCodePage(), lpszMessageBody, lpszEncodedMessageBody, cchEncodedMessageBody ) )
                {
                    // Just use the unencoded info on error....
                    _tcscpy( lpszEncodedMessageBody, lpszMessageBody );
                }
            }
        }
        
        SendMail( s_pStorage->m_options.SelectedMailServer,
                  s_pStorage->m_options.SelectedMailPort,
                  s_pStorage->m_options.SelectedReversePath,
                  szSymantecAntiVirus,
                  s_pStorage->m_options.SelectedRecipients,
                  szEncodedSubject,
                  lpszEncodedMessageBody ? lpszEncodedMessageBody : pNodeContext->szSelectedMessage,
                  &dwSMTPError );

        delete [] lpszEncodedMessageBody;
        lpszEncodedMessageBody = NULL;

        DeleteAlertMessageBody( lpszMessageBody );
    }

    return ( dwResult );
} // CIMailStorage::doNotifications()


//*************************************************************************
// SendMail()
//
// static bool SendMail ( LPCTSTR     lpszMailServer,
//                        DWORD       dwMailPort,
//                        LPCTSTR     lpszReversePath,
//                        LPCTSTR     lpszSender,
//                        LPCTSTR     lpszRecipients,
//                        LPCTSTR     lpszSubject,
//                        LPCTSTR     lpszMessage,
//                        LPDWORD     lpdwSMTPError )
//
// Parameters:
//      lpszMailServer
//      lpszSender
//      lpszRecipients
//      lpszSubject
//      lpszMessage
//      *lpdwSMTPError
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.18 DALLEE - from CSMTPMail::Send() csmtp.cpp
// 2003.09.03 DALLEE - added configurable mail port, and SMTP MAIL FROM:<reverse-path>
//*************************************************************************

static bool SendMail ( LPCTSTR     lpszMailServer,
                       WORD        wMailPort,
                       LPCTSTR     lpszReversePath,
                       LPCTSTR     lpszSender,
                       LPCTSTR     lpszRecipients,
                       LPCTSTR     lpszSubject,
                       LPCTSTR     lpszMessage,
                       LPDWORD     lpdwSMTPError )
{
    auto    SOCKET_ADDRESS_T    sSockAddress;
    auto    SOCKET_STATUS       sStatus;
    auto    WSADATA             wsaData;
    auto    bool                bReturn = true;
    auto    bool                bWinSockOpen = false;
    auto    int                 nError;

    auto    LPINTERNET_EMAIL_LIST   lpRecipientList;

    if ( NULL != lpdwSMTPError )
    {
        *lpdwSMTPError = 0;
    }

    // Reformat recipients from ';' delimited to SMTPSendMail's list format

    BuildRecipientList( lpszRecipients, &lpRecipientList );

    if (NULL == lpRecipientList)
    {
        bReturn = false;
        goto Cleanup;
    }

    // First, initialize WinSock.

    nError = WSAStartup( MAKEWORD( 1, 1 ), &wsaData );

    // Check for an error

    if ( 0 != nError )
    {
        bReturn = false;
        goto Cleanup;
    }

    // Set a flag to make sure we clean up

    bWinSockOpen = true;

    ZeroMemory( &sSockAddress, sizeof( sSockAddress ) );

    // Now that WinSock is ready for us, we
    // need to look up the address of the
    // Mail Server.

    sStatus = SocketAddressFill( &sSockAddress,
                                 const_cast<LPSTR>( lpszMailServer ),
                                 wMailPort,
                                 lpdwSMTPError );

    // Check to see if we got our address correctly

    if ( SOCKET_STATUS_OK != sStatus )
    {
        bReturn = false;
        goto Cleanup;
    }

    // OK... Now after all the busy work,
    // we can send the mail

    sStatus = SMTPSendMail( &sSockAddress,
                            const_cast<LPSTR>( lpszReversePath ),
                            const_cast<LPSTR>( lpszSender ),
                            lpRecipientList,
                            const_cast<LPSTR>( lpszSubject ),
                            const_cast<LPSTR>( lpszMessage ),
                            lpdwSMTPError );

    if (SOCKET_STATUS_OK != sStatus)
    {
        bReturn = false;
        goto Cleanup;
    }

Cleanup:

    if ( bWinSockOpen )
    {
        // Close down our hooks into winsock.

        WSACleanup();
    }

    // Clean up any allocated strings

    if ( NULL != lpRecipientList )
    {
        DeleteRecipientList( &lpRecipientList );
    }

    return ( bReturn );
} // SendMail()


//*************************************************************************
// BuildRecipientList()
//
// Build a list of recipients as understood by SMTPSendMail() from
// ';' delimited list in SAVCE's options format.
//
// static void BuildRecipientList( LPCTSTR lpszRecipients, LPINTERNET_EMAIL_LIST *lppRecipientList )
//
// Parameters:
//      lpszRecipients          Sequence of ';' delimited bare addresses
//      *lppRecipientList       [out] Receives list of addresses in SMTPSendMail format
//                                    or NULL on error.
//
// Return Values:
//      None
//          
// Remarks:
//      When finished with the list, DeleteRecipientList() must be called
//      to free the list data.
//*************************************************************************
// 2003.08.18 DALLEE - very loosely from CSMTPMail::BuildRecipientList() csmtp.cpp
//*************************************************************************

static void BuildRecipientList( LPCTSTR lpszRecipients, LPINTERNET_EMAIL_LIST *lppRecipientList )
{
    auto    LPCTSTR     lpszDelimiter;

    auto    INTERNET_EMAIL_LIST_T   rListHead;
    auto    LPINTERNET_EMAIL_LIST   pListEnd;
    auto    LPINTERNET_EMAIL_LIST   pNewNode = NULL;


    // rListHead is an empty node just to keep track of the list we're creating here.

    memset( &rListHead, 0, sizeof( rListHead ) );
    pListEnd = &rListHead;

    // Pull each ';' delimited recipient address out of lpszRecipients

    while ( _T('\0') != *lpszRecipients )
    {
        // Find end of next recipient address

        lpszDelimiter = _tcschr( lpszRecipients, _T(';') );

        if ( NULL == lpszDelimiter )
        {
            // If no more tokens, make delimiter the end of string.

            lpszDelimiter = lpszRecipients + _tcslen( lpszRecipients );
        }

        // Crude check to make sure we have an address here to copy...

        if ( ( lpszDelimiter - lpszRecipients > 0 ) &&
             ( lpszDelimiter - lpszRecipients < sizeof( pNewNode->szAddress ) / sizeof( *pNewNode->szAddress ) ) )
        {
			try
			{
				pNewNode = new INTERNET_EMAIL_LIST_T;
			}
			catch (std::bad_alloc &) 
			{
                DeleteRecipientList( &rListHead.lpNext );
                break;
			}


            memset( pNewNode, 0, sizeof( *pNewNode ) );

            // Copy recipient address
            // As far as CRT string routines go, strncat isn't so bad... it guarantees null
            // termination. Will append up to n characters from source + 1 more for EOS.

            _tcsncat( pNewNode->szAddress, lpszRecipients, lpszDelimiter - lpszRecipients );

            // Add new node to end of list

            pListEnd->lpNext = pNewNode;
            pListEnd = pNewNode;

            // Advance to next entry in our recipients list...

            lpszRecipients = lpszDelimiter;
            while ( _T(';') == *lpszRecipients )
            {
                lpszRecipients = _tcsinc( lpszRecipients );
            }
        }
    }

    *lppRecipientList = rListHead.lpNext;
} // BuildRecipientList()


//*************************************************************************
// DeleteRecipientList()
//
// static void DeleteRecipientList( LPINTERNET_EMAIL_LIST *lppRecipientList )
//
// Parameters:
//      *lppRecipientList
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.18 DALLEE - loosely from CSMTPMail::DeleteRecipientList() csmtp.cpp
//*************************************************************************

static void DeleteRecipientList( LPINTERNET_EMAIL_LIST *lppRecipientList )
{
    auto    LPINTERNET_EMAIL_LIST   lpTemp;

    // Iterate through and delete all of the records.
    // Also sets *lppRecipientList = NULL at end.

    while ( *lppRecipientList )
    {
        lpTemp = *lppRecipientList;

        *lppRecipientList = (*lppRecipientList)->lpNext;

        delete lpTemp;
    }
} // DeleteRecipientList()


//*************************************************************************
// BuildAlertMessageBody()
//
// static LPCTSTR BuildAlertMessageBody( LPCTSTR lpszMessage, PIEM_ATTACHMENT_INFO_LIST pAttachmentInfo, ALERT_MESSAGE_TYPE nType )
//
// Parameters:
//      lpszMessage
//      pAttachmentInfo
//      nType
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.24 DALLEE - created.
//*************************************************************************

static LPCTSTR BuildAlertMessageBody( LPCTSTR lpszMessage, PIEM_ATTACHMENT_INFO_LIST pAttachmentInfo, ALERT_MESSAGE_TYPE nType )
{
    auto    PIEM_ATTACHMENT_INFO_LIST   pCurrentAttachment;
    auto    LPCTSTR                     lpszCurrentInfectionInfo;

    auto    LPTSTR                      lpszFullMessage = NULL;
    auto    size_t                      nMessageLength;

    const   TCHAR   szCRLF [] = _T("\r\n");

    // Calculate size for full alert message -- warning message plus
    // infection information for each attachment.

    nMessageLength = _tcslen( lpszMessage ) + 2 + 1;    // +2 for CRLF + 1 for EOS

    for ( pCurrentAttachment = pAttachmentInfo;
          pCurrentAttachment;
          pCurrentAttachment = pCurrentAttachment->pNext )
    {
        switch ( nType )
        {
        default:
        case ALERT_WARNING:
            lpszCurrentInfectionInfo = pCurrentAttachment->szWarningInfectionInfo;
            break;
        case ALERT_SENDER:
            lpszCurrentInfectionInfo = pCurrentAttachment->szSenderInfectionInfo;
            break;
        case ALERT_SELECTED:
            lpszCurrentInfectionInfo = pCurrentAttachment->szSelectedInfectionInfo;
            break;
        }

        nMessageLength += _tcslen( lpszCurrentInfectionInfo ) + 2; // +2 for CRLF
    }

    // Allocate buffer for full message

	try
	{
		lpszFullMessage = new TCHAR [ nMessageLength ];
	}
	catch (std::bad_alloc &) {}

    // Construct full message

    if ( lpszFullMessage )
    {
        _tcscpy( lpszFullMessage, lpszMessage );
        _tcscat( lpszFullMessage, szCRLF );

        for ( pCurrentAttachment = pAttachmentInfo;
              pCurrentAttachment;
              pCurrentAttachment = pCurrentAttachment->pNext )
        {
            switch ( nType )
            {
            default:
            case ALERT_WARNING:
                lpszCurrentInfectionInfo = pCurrentAttachment->szWarningInfectionInfo;
                break;
            case ALERT_SENDER:
                lpszCurrentInfectionInfo = pCurrentAttachment->szSenderInfectionInfo;
                break;
            case ALERT_SELECTED:
                lpszCurrentInfectionInfo = pCurrentAttachment->szSelectedInfectionInfo;
                break;
            }

            _tcscat( lpszFullMessage, lpszCurrentInfectionInfo );
            _tcscat( lpszFullMessage, szCRLF );
        }
    }

    return ( lpszFullMessage );
} // BuildAlertMessageBody()


//*************************************************************************
// DeleteAlertMessageBody()
//
// static void DeleteAlertMessageBody( LPCTSTR lpszMessage )
//
// Parameters:
//      lpszMessage
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.24 DALLEE - created.
//*************************************************************************

static void DeleteAlertMessageBody( LPCTSTR lpszMessage )
{
    delete [] const_cast<LPTSTR>( lpszMessage );
} // DeleteAlertMessageBody()


