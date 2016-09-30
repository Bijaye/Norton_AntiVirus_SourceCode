/////////////////////////////////////////////////////////////////////
// NTEVLOG.CPP - Created - 11/13/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntevlog.cpv   1.3   01 Jul 1997 16:13:44   DSACKIN  $
//
// Description:
//    This contains the code for NT Event Log Notifications
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntevlog.cpv  $
// 
//    Rev 1.3   01 Jul 1997 16:13:44   DSACKIN
// Had to do a little arithmatic with length of strings + NULL * sizeof (TCHAR)
// to get the actual buffer size for my CString objects being retrieved.
//
//    Rev 1.2   09 Jun 1997 01:57:02   DSACKIN
// We must clean up the lpcVirusAlert and lpcTarget when we are done.
//
//    Rev 1.1   18 May 1997 05:06:32   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:40:56   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:32:38   DSACKIN
// Changed the prototype of the ProcessWorkOrder function.
//
//    Rev 1.0   13 Mar 1997 15:06:08   RFULLER
// Initial revision
//
//    Rev 1.7   18 Feb 1997 13:36:04   TSmith
// Merged code from rev 1.1 of this file to retrieve a SID and send it
// to the LogEvent function. Originally, this code was used to retrieve the
// domain name and was removed when a different method to get this information
// was used. Now we use the SID only for passing through to the NT event log
// APIs so that the header of the event log entry will contain the proper info.
//
//    Rev 1.6   13 Feb 1997 21:11:02   TSmith
// Corrected test on packet source flags.
//
//    Rev 1.5   12 Feb 1997 17:12:30   TSmith
// Added code to get the packet source and skip the log entry if the packet
// was sent by navw.
//
//    Rev 1.4   21 Jan 1997 14:09:30   TSmith
// Added processing of domain name now stored in CAlertNotification objects
// to 'ProcessWorkOrder'. Removed code that retrieved domain name.
//
//    Rev 1.3   18 Jan 1997 07:00:06   DSACKIN
// Reworked the Alert Service to use the new CAlertQueueManager class for
// all alerting.
//
//    Rev 1.2   22 Dec 1996 15:50:14   TSmith
// Changed calls to CAlertNotification string retrieval functions to support
// that class's new arguments.
//
//    Rev 1.1   12 Dec 1996 19:12:34   TSmith
// Added code to retrieve a SID and send to the LogEvent function. This will
// allow the header information for the event log entry to be filled in.
//
//    Rev 1.0   13 Nov 1996 22:13:28   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//      Includes
#include "afxtempl.h"
#include "platform.h"
#include "process.h"
#include "winnetwk.h"
#include "lmcons.h"
#include "lmmsg.h"
#include "lmerr.h"
#include "servenfy.h"
#include "linksrv.h"
#include "service.h"
#include "svcctrl.h"
#include "navrpc.h"
#include "xapi.h"

                                        // These includes are for the
                                        // AlertManager objects
#include "AlertQ.h"
#include "Managers.h"

#include "alertsvc.h"
#include "alertstr.h"


/////////////////////////////////////////////////////////////////////
// VOID CEventLogAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and writes it to
//    the NT Event log
//
// Parameters:
//   CVirusAlert  *lpcVirusAlert - [in] - Information about the alert
//   CAlertTarget *lpcTarget     - [in] - Where to send it.
//
// Returns:
//   none
//
// Notes:
//   We are responsible here for deleting the lpcTarget when we
//   are done with it.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
// 01/21/97 - TSMITH  - Added processing of domain name now stored in
//                      CAlertNotification objects. Removed code that
//                      retrieved domain name.
// 02/10/97 - TSMITH  - Added code to get the packet source flags from
//                      packet and skip logging if packet from NAVW.
/////////////////////////////////////////////////////////////////////
VOID CEventLogAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
SID_NAME_USE SidUse;
CString      szObject;
CString      szVirus;
CString      szSystem;
CString      szUser;
CString      szAction;
CString      szDomain;
LPTSTR       pszDomain;
LPTSTR       lpszString;
DWORD        dwMsgId;
DWORD        dwPacketSrc;
DWORD        dwSid = 0;
DWORD        dwDomain = 0;
DWORD        dwBufSize;
BOOL         bState;
PSID         pSid = NULL;
DWORD        dwAlertType;
DWORD        dwAction;
BOOL         bActionSuccess;

                                        // Get & check the source of the packet.
   if (!lpcVirusAlert->GetPacketSource(
          dwPacketSrc))
   {
      goto Cleanup;
   }

                                        // If the packet was originated by a copy
                                        // of NAVWNT running on the local system
                                        // we skip logging because it does it for
                                        // itself.
   if ( ( ( dwPacketSrc & PACKET_SOURCE_NAVW ) &&
          ( dwPacketSrc & PACKET_SOURCE_LOCAL ) ) )
   {
      goto Cleanup;
   }

                                        // Get system and user names from packet
                                        // first because they're needed for
                                        // subsequent call to 'LookupAccountName'.
   if (!lpcVirusAlert->GetSystemNameLength(dwBufSize))
      goto Cleanup;

                                        // Adjust for the size of TCHAR and the
                                        // NULL terminator.
   dwBufSize = (dwBufSize+1) * sizeof (TCHAR);

   lpszString = szSystem.GetBufferSetLength(
                   dwBufSize);

   if (!lpcVirusAlert->GetSystemName(
           dwBufSize,
           lpszString ))
      goto Cleanup;

   szSystem.ReleaseBuffer();


   if (!lpcVirusAlert->GetUserNameLength(dwBufSize))
      goto Cleanup;

                                        // Adjust for the size of TCHAR and the
                                        // NULL terminator.
   dwBufSize = (dwBufSize+1) * sizeof (TCHAR);

   lpszString = szUser.GetBufferSetLength(
                   dwBufSize);

   if (!lpcVirusAlert->GetUserName(
           dwBufSize,
           lpszString))
      goto Cleanup;

   szUser.ReleaseBuffer();

                                        // Get access to CString's buffer
                                        // for LookupAccountName.
   pszDomain = szDomain.GetBuffer(dwDomain);

                                        // Get SID from user and computer name.
                                        // Make call with zero size buffers to
                                        // force NT to tell us what size it needs.
   LookupAccountName(
      szSystem,
      szUser,
      pSid,
      &dwSid,
      pszDomain,
      &dwDomain,
      &SidUse );

   szDomain.ReleaseBuffer();            // Release CString's buffer.
   pszDomain = NULL;                    // Pointer now invalid.

                                        // Allocate memory, throwing an
                                        // exception if allocation fails.
   pSid = ( PSID )new BYTE[ dwSid ];

   if ( ! pSid )
   {
      goto Cleanup;
   }
                                        // Get CString's buffer again, this
                                        // time enlarging it to meet the
                                        // requirements of LookupAccountName.
   pszDomain = szDomain.GetBufferSetLength(
                  dwDomain);

                                        // Now make the real call with
                                        // appropriately sized buffers.
   bState = LookupAccountName(
               szSystem,
               szUser,
               pSid,
               &dwSid,
               pszDomain,
               &dwDomain,
               &SidUse );

   szDomain.ReleaseBuffer();            // Release CString's buffer.
   pszDomain = NULL;                    // Pointer now invalid.

   if (!lpcVirusAlert->GetAlertType(
           dwAlertType))
      goto Cleanup;

   if (!lpcVirusAlert->GetAction(
           dwAction))
      goto Cleanup;

   if (!lpcVirusAlert->GetActionSuccess(
           bActionSuccess))
      goto Cleanup;

   ComposeActionString(
      dwAlertType,
      dwAction,
      bActionSuccess,
      &szAction);

                                        // Get the various variable length strings
                                        // out of the CAlertNotification object.
   if (!lpcVirusAlert->GetObjectNameLength(dwBufSize))
      goto Cleanup;

                                        // Adjust for the size of TCHAR and the
                                        // NULL terminator.
   dwBufSize = (dwBufSize+1) * sizeof (TCHAR);

   lpszString = szObject.GetBufferSetLength(
                   dwBufSize);

   if (!lpcVirusAlert->GetObjectName(
           dwBufSize,
           lpszString ))
      goto Cleanup;

   szObject.ReleaseBuffer();


   if (!lpcVirusAlert->GetVirusNameLength(dwBufSize))
      goto Cleanup;

                                        // Adjust for the size of TCHAR and the
                                        // NULL terminator.
   dwBufSize = (dwBufSize+1) * sizeof (TCHAR);

   lpszString = szVirus.GetBufferSetLength(
                   dwBufSize);

   if (!lpcVirusAlert->GetVirusName(
           dwBufSize,
           lpszString))
      goto Cleanup;

   szVirus.ReleaseBuffer();


   if (!lpcVirusAlert->GetDomainNameLength(dwBufSize))
      goto Cleanup;

                                        // Adjust for the size of TCHAR and the
                                        // NULL terminator.
   dwBufSize = (dwBufSize+1) * sizeof (TCHAR);

   lpszString = szDomain.GetBufferSetLength(
                   dwBufSize);

   if (!lpcVirusAlert->GetDomainName(
           dwBufSize,
           lpszString))
      goto Cleanup;

   szDomain.ReleaseBuffer();

                                        // Select which event log message to use
                                        // according to what kind of virus
                                        // event has occurred.
   switch (dwAlertType)
   {
      case FILE_KNOWN_VIRUS:
         dwMsgId = ID_EVENT_LOG_ENTRY_FILE;
         break;
      case BOOT_RECORD_KNOWN_VIRUS:
         dwMsgId = ID_EVENT_LOG_ENTRY_BR;
         break;
      case MBR_KNOWN_VIRUS:
         dwMsgId = ID_EVENT_LOG_ENTRY_MBR;
         break;
      default:
         SYM_ASSERT( FALSE );
         break;
   }

   m_lpService->LogEvent(
      dwMsgId,
      szObject.GetBuffer( szObject.GetLength() ),
      szVirus.GetBuffer( szVirus.GetLength() ),
      szDomain.GetBuffer( szDomain.GetLength() ),
      szSystem.GetBuffer( szSystem.GetLength() ),
      szUser.GetBuffer( szUser.GetLength() ),
      szAction.GetBuffer( szAction.GetLength() ),
      bState ? pSid : NULL );

   szObject.ReleaseBuffer();
   szVirus.ReleaseBuffer();
   szSystem.ReleaseBuffer();
   szDomain.ReleaseBuffer();
   szUser.ReleaseBuffer();
   szAction.ReleaseBuffer();

Cleanup:
                                        // If we had alerts or targets, this is
                                        // the place to delete them.
   if (lpcVirusAlert)
   {
      delete lpcVirusAlert;
      lpcVirusAlert = NULL;
   }

   if (lpcTarget)
   {
      delete lpcTarget;
      lpcTarget = NULL;
   }



   if ( pSid )                          // Release SID memory
   {
      delete[] pSid;
      pSid = NULL;
   }
}


