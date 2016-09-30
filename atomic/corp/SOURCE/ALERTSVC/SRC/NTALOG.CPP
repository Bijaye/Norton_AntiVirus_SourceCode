/////////////////////////////////////////////////////////////////////
// NTALOG.CPP - Created - 11/13/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntalog.cpv   1.5   21 May 1998 16:37:14   rpulint  $
//
// Description:
//    This contains the NAV Activity Log Notification code
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntalog.cpv  $
// 
//    Rev 1.5   21 May 1998 16:37:14   rpulint
// Increased the number of chars to be read for GetSystemName()
// to hold 15 chars + null terminator in ProcessWorkOrder()
// 
//    Rev 1.4   01 Apr 1998 18:09:34   DALLEE
// Added handling for quaratine actions (ACTION_QUARANTINE).
// 
//    Rev 1.3   19 Jun 1997 14:15:46   DSACKIN
// When sending up a char * buffer to GetUserName, we must explicitly call
// the GetUserNameA function.
//
//    Rev 1.2   09 Jun 1997 01:56:46   DSACKIN
// We must clean up the lpcVirusAlert and lpcTarget when we are done.
//
//    Rev 1.1   18 May 1997 05:06:30   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:40:54   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:30:34   DSACKIN
// Changed the ProcessWorkOrder prototype.
//
//    Rev 1.0   13 Mar 1997 15:06:08   RFULLER
// Initial revision
//
//    Rev 1.5   13 Feb 1997 21:03:44   TSmith
// Corrected test on packet source flags.
//
//    Rev 1.4   12 Feb 1997 17:06:00   TSmith
// Added code to get source of packet and return without making a log entry
// if the packet was sent by NAVW.
//
//    Rev 1.3   18 Jan 1997 07:00:04   DSACKIN
// Reworked the Alert Service to use the new CAlertQueueManager class for
// all alerting.
//
//    Rev 1.2   22 Dec 1996 15:47:32   TSmith
// Changed calls to CAlertNotification string retrieval functions to support
// those functions new arguments.
//
//    Rev 1.1   16 Dec 1996 10:14:10   TSmith
// Added code to 'LocalActivityLogNotification' to insert the system name an
// event occurred on into the NAVAP_LOG structure.
//
//    Rev 1.0   13 Nov 1996 22:13:26   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// Includes

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
// VOID CActivityLogAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and displays a writes it to
//    the activity log
//
// Parameters:
//   CVirusAlert  *lpcVirusAlert  - [in] - Information about the alert
//   CAlertTarget *lpcTarget      - [in] - Where to send it.
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
/////////////////////////////////////////////////////////////////////
VOID CActivityLogAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
TIME_ZONE_INFORMATION rTz;              // For local time zone
SYSTEMTIME            rSystem;
SYSTEMTIME            rLocal;           // For local time.
NAVAP_LOG             NavApLog;         // For activity Log Write
BOOL                  bSuccess;
DWORD                 dwAction;
DWORD                 dwAlertType;
DWORD                 dwPacketSrc;


   LogMessage( _T("ProcessWorkOrder for ActivityLog.") );

   if (!lpcVirusAlert->GetActionSuccess(
           bSuccess))
   {
      goto Cleanup;
   }

   if (!lpcVirusAlert->GetAction(
           dwAction))
   {
      goto Cleanup;
   }

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

                                        // Clean up.
   MEMSET( &NavApLog, 0, sizeof( NAVAP_LOG ) );
   MEMSET( &rTz, 0, sizeof( TIME_ZONE_INFORMATION ) );
   MEMSET( &rLocal, 0, sizeof( SYSTEMTIME ) );

                                        // Fill in the Log structure.
   NavApLog.wSize = sizeof( NAVAP_LOG );        // Size
   NavApLog.byRecType = ALOG_FI_KNOWNVIRUS;
   lpcVirusAlert->GetTime( rSystem );

                                        // Note that the CVirusAlert
                                        // class can store strings much longer
                                        // than those defined in the NAVAP_LOG
                                        // structure.  If the strings in the
                                        // packet are longer than what
                                        // NAVAP_LOG can hold, we'll get
                                        // truncated versions of the strings
                                        // here.

   lpcVirusAlert->GetSystemName(
//      MAX_COMPUTERNAME_LENGTH,
      MAX_COMPUTERNAME_LENGTH+1,
      NavApLog.szSystem);

                                        // Explicitly call the A version of the
                                        // function!
   lpcVirusAlert->GetUserNameA(
      SYM_MAX_PATH,
      NavApLog.szUser);

   lpcVirusAlert->GetObjectName(
      SYM_MAX_PATH + 1,
      NavApLog.szFile );

   lpcVirusAlert->GetVirusName(
      SIZE_VIRUSNAME,
      NavApLog.szVirus);

   if (!lpcVirusAlert->GetAlertType(
           dwAlertType))
   {
      goto Cleanup;
   }

   switch (dwAlertType)
   {
      case FILE_KNOWN_VIRUS:
         if ( bSuccess && ( ( dwAction == ACTION_REPAIR ) ||
                            ( dwAction == ACTION_EXCLUDE) ||
                            ( dwAction == ACTION_DELETE ) ||
                            ( dwAction == ACTION_QUARANTINE ) ) )
            NavApLog.wVirMessageID = IDS_VIR_MSG_WAS_INFECTED;
         else
            NavApLog.wVirMessageID = IDS_VIR_MSG_IS_INFECTED;
         break;

      case BOOT_RECORD_KNOWN_VIRUS:
         if ( bSuccess && ( ( dwAction == ACTION_REPAIR ) ||
                            ( dwAction == ACTION_EXCLUDE) ||
                            ( dwAction == ACTION_DELETE ) ) )
            NavApLog.wVirMessageID = IDS_VIR_MSG_BOOT_WAS_INFECTED;
         else
            NavApLog.wVirMessageID = IDS_VIR_MSG_BOOT_IS_INFECTED;
         break;

      case MBR_KNOWN_VIRUS:
         if ( bSuccess && ( ( dwAction == ACTION_REPAIR ) ||
                            ( dwAction == ACTION_EXCLUDE) ||
                            ( dwAction == ACTION_DELETE ) ) )
            NavApLog.wVirMessageID = IDS_VIR_MSG_MBR_WAS_INFECTED;
         else
            NavApLog.wVirMessageID = IDS_VIR_MSG_MBR_IS_INFECTED;
         break;

      default:
         NavApLog.wVirMessageID = 0;
         break;
   }

   NavApLog.wVirActionID = ComposeActionString(
                              dwAlertType,
                              dwAction,
                              bSuccess,
                              NULL );

   *( NavApLog.szProgram ) = EOS;       // Leave this empty (Inherited!!)

                                        // Get the local Time zone info
   GetTimeZoneInformation (&rTz);

                                        // Convert the given system time into local time.
   SystemTimeToTzSpecificLocalTime(
      &rTz,
      &rSystem,
      &rLocal );

                                        // Convert the local date into DWORD
   NavApLog.dwEncodedDate = ConvertDateToEncoded(
                              rLocal.wMonth,
                              rLocal.wDay,
                              rLocal.wYear );

                                        // Convert the local time into DWORD
   NavApLog.dwEncodedTime = ConvertTimeToEncoded( rLocal.wHour,
                              rLocal.wMinute,
                              rLocal.wSecond );

                                        // Log it to the Activity Log file.
   LogWrite( &NavApLog );


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


   return;
}


