/////////////////////////////////////////////////////////////////////
// SMTP.CPP - Created - 11/19/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CMAIL/VCS/csmtp.cpv   1.1   03 Nov 1997 09:50:26   TSMITH  $
//
// Description:
//    This file will contain the implementation of the CSMTPMail class.
//    It will be derived from the CMail base class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CMAIL/VCS/csmtp.cpv  $
// 
//    Rev 1.1   03 Nov 1997 09:50:26   TSMITH
// Ported the following by MDUNN from RosNec;
// 
// Fixed memory allocations in CSMTPMail::Send() to allocate enough memory
// to handle double-byte characters. 
// 
// 
//    Rev 1.0   12 Sep 1997 17:31:50   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Apr 1997 13:47:20   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:09:00   RFULLER
// Initial revision
// 
//    Rev 1.4   25 Nov 1996 14:54:54   DSACKIN
// Changed the hard coded port 25 to a defined IPPORT_SMTP.
//
//    Rev 1.3   21 Nov 1996 18:01:00   DSACKIN
// ALWAYS initialize local variables!!!
//
//    Rev 1.2   20 Nov 1996 22:09:40   DSACKIN
// Check number of recipients before sending e-mails
//
//    Rev 1.1   20 Nov 1996 21:55:12   DSACKIN
// Add one to the length of the strings for the NULL terminator.
//
//    Rev 1.0   20 Nov 1996 14:28:02   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "afx.h"

                                        // Includes from NEWVIR
#include "ossocket.h"
#include "smtpmail.h"

                                        // Class definitions
#include "CMail.h"
#include "CSMTP.h"


/////////////////////////////////////////////////////////////////////
// CSMTPMail::CSMTPMail()
//
// Purpose:
//    Constructor for our CSMTPMail object
//
// Parameters:
//    none
//
// Returns:
//    none
//
// See Also:
//    CMail constructor
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CSMTPMail::CSMTPMail()
{

}


/////////////////////////////////////////////////////////////////////
// CSMTPMail::~CSMTPMail()
//
// Purpose:
//    Destructor for our CSMTPMail object
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
// See Also:
//    CMail destructor
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CSMTPMail::~CSMTPMail()
{
}


/////////////////////////////////////////////////////////////////////
// CSMTPMail::BuildRecipientList()
//
// Purpose:
//    Constructs an e-mail list from the internal list of
//    e-mail recipients.
//
// Parameters:
//    LPINTERNET_EMAIL_LIST *lpTop - [out] Pointer to use in creating our list
//
// Returns:
//    lpTop - NULL  - no recipients
//            Value - link list to recipients
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CSMTPMail::BuildRecipientList(
   LPINTERNET_EMAIL_LIST *lpTop)
{
LPINTERNET_EMAIL_LIST lpNew = NULL;
POSITION   pList;
CString    cszName;
BOOL       bSuccess = TRUE;

                                        // Start will a NULL list and then add
                                        // the nodes
   *lpTop = NULL;


                                        // Go through our internal list to add
                                        // the individual names
   for ( pList = m_szlistRecipients.GetTailPosition();
         (pList != NULL); )
   {
      cszName = m_szlistRecipients.GetPrev( pList );

      lpNew = new INTERNET_EMAIL_LIST_T;

                                        // Check for a memory allocation error
      if (NULL == lpNew)
      {
                                        // Get rid of any of the list we may
                                        // have created so far
         DeleteRecipientList(
            lpTop);

                                        // Now break out so that we don't try
                                        // to add any new ones
         break;
      }

                                        // Convert the name to multibyte
      WideCharToMultiByte(
         CP_ACP,
         WC_COMPOSITECHECK,
         ( LPCTSTR )cszName,
         -1,
         lpNew->szAddress,
         sizeof (lpNew->szAddress),
         NULL,
         NULL );

                                        // Now link it into our chain
      lpNew->lpNext = *lpTop;

                                        // Now move our list top to the new node
      *lpTop = lpNew;

   }

}



/////////////////////////////////////////////////////////////////////
// CSMTPMail::DeleteRecipientList()
//
// Purpose:
//    Removes all nodes from a LPINTERNET_EMAIL_LIST
//
// Parameters:
//    LPINTERNET_EMAIL_LIST *lpTop - Pointer to list to be deleted
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CSMTPMail::DeleteRecipientList(
   LPINTERNET_EMAIL_LIST *lpTop)
{
LPINTERNET_EMAIL_LIST lpTemp;


                                        // Iterate through and delete all of
                                        // the records
   while (*lpTop)
   {
      lpTemp = *lpTop;

      *lpTop = (*lpTop)->lpNext;

      delete lpTemp;
   }

}

/////////////////////////////////////////////////////////////////////
// CSMTPMail::Send()
//
// Purpose:
//    This will actuall send an SMTP mail.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - mail sent successfully
//    FALSE - mail was not sent successfully
//
// Note:
//    All information used to send the actual mail is handled
//    in the CMail base class.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/19/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CSMTPMail::Send(VOID)
{
SOCKET_ADDRESS_T sSockAddress;
SOCKET_STATUS    sStatus;
WORD             wVersionRequested;
WSADATA          wsaData;
BOOL             bReturn = FALSE;
BOOL             bWinSockOpen = FALSE;
DWORD            dwStatus;
int              nError, nRet;

LPINTERNET_EMAIL_LIST lpRecipientList;
LPSTR                 lpszPostOffice = NULL;
LPSTR                 lpszSender = NULL;
LPSTR                 lpszSubject = NULL;
LPSTR                 lpszMessage = NULL;
DWORD                 dwSizePostOffice;
DWORD                 dwSizeSender;
DWORD                 dwSizeSubject;
DWORD                 dwSizeMessage;

   if (0 == m_dwRecipients)
   {
      goto Cleanup;
   }
                                        // Try to convert our internal internet
                                        // address into something that the
                                        // e-mail guy will understand?
   BuildRecipientList(
      &lpRecipientList);

                                        // Were we successful?
   if (NULL == lpRecipientList)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // Grab the sizes of all of our strings
                                        // for allocating non-unicode guys
                                        // Always add 1 for the NULL
                                        // 
                                        // I multiply GetLength() return by
                                        // 2 because in Unicode, GetLength()
                                        // returns the number of Unicode
                                        // characters, which might all be
                                        // double-byte once converted to the
                                        // multibyte character set.  The extra
                                        // memory that's allocated as a
                                        // result shouldn't be a big deal.

   dwSizePostOffice = (2 * m_szPostOffice.GetLength()) + 1;
   dwSizeSender     = (2 * m_szSender.GetLength()) + 1;
   dwSizeSubject    = (2 * m_szSubject.GetLength()) + 1;
   dwSizeMessage    = (2 * m_szMessage.GetLength()) + 1;

                                        // Now attempt to build NON-UNICODE
                                        // strings that the e-mailer guy will
                                        // understand
   lpszPostOffice = new char [dwSizePostOffice + 1];
   lpszSender     = new char [dwSizeSender + 1];
   lpszSubject    = new char [dwSizeSubject + 1];
   lpszMessage    = new char [dwSizeMessage + 1];

   if ( (NULL == lpszPostOffice) ||
        (NULL == lpszSender) ||
        (NULL == lpszSubject) ||
        (NULL == lpszMessage) )
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // Convert the name to multibyte
   nRet = WideCharToMultiByte(
             CP_ACP,
             WC_COMPOSITECHECK,
             ( LPCTSTR )m_szPostOffice,
             -1,
             lpszPostOffice,
             dwSizePostOffice,
             NULL,
             NULL );
                                        // Convert the name to multibyte
   if (nRet > 0)
   {
      nRet = WideCharToMultiByte(
                CP_ACP,
                WC_COMPOSITECHECK,
                ( LPCTSTR )m_szSender,
                -1,
                lpszSender,
                dwSizeSender,
                NULL,
                NULL );
   }
                                        // Convert the name to multibyte
   if (nRet > 0)
   {
      nRet = WideCharToMultiByte(
                CP_ACP,
                WC_COMPOSITECHECK,
                ( LPCTSTR )m_szSubject,
                -1,
                lpszSubject,
                dwSizeSubject,
                NULL,
                NULL );
   }
                                        // Convert the name to multibyte
   if (nRet > 0)
   {
      nRet = WideCharToMultiByte(
                CP_ACP,
                WC_COMPOSITECHECK,
                ( LPCTSTR )m_szMessage,
                -1,
                lpszMessage,
                dwSizeMessage,
                NULL,
                NULL );
   }

                                        // Check to make sure all of our
                                        // conversions happened correctly.
   if (0 == nRet)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // First, initialize WinSock
   wVersionRequested = MAKEWORD(1, 1);
   nError = WSAStartup (
               wVersionRequested,
               &wsaData);

                                        // Check for an error
   if (0 != nError)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // Set a flag to make sure we clean up
   bWinSockOpen = TRUE;


                                        // Now that WinSock is ready for us, we
                                        // need to look up the address of the
                                        // Mail Server.

   sStatus = SocketAddressFill (
                &sSockAddress,
                lpszPostOffice,
                IPPORT_SMTP,            // 25 is standard SMTP mail port
                &dwStatus);

                                        // Check to see if we got our address
                                        // correctly
   if (SOCKET_STATUS_OK != sStatus)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // OK... Now after all the busy work,
                                        // we can send the mail

   sStatus = SMTPSendMail(
                &sSockAddress,
                lpszSender,
                lpRecipientList,
                lpszSubject,
                lpszMessage,
                &dwStatus);

   if (SOCKET_STATUS_OK != sStatus)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

Cleanup:

   if (bWinSockOpen)
   {
      WSACleanup();                     // Close down our hooks into winsock.
   }

                                        // Clean up any allocated strings
   if (NULL != lpszPostOffice)
   {
      delete lpszPostOffice;
      lpszPostOffice = NULL;
   }

   if (NULL != lpszSender)
   {
      delete lpszSender;
      lpszSender = NULL;
   }

   if (NULL != lpszSubject)
   {
      delete lpszSubject;
      lpszSubject = NULL;
   }

   if (NULL != lpszMessage)
   {
      delete lpszMessage;
      lpszMessage = NULL;
   }

   return bReturn;
}
