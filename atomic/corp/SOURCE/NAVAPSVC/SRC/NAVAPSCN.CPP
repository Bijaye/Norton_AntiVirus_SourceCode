//      Copyright 1996 Symantec Corporation
//*****************************************************************************
//      $Header:   S:/NAVAPSVC/VCS/navapscn.cpv   1.17   23 Dec 1998 15:05:56   RStanev  $
//
//      Description:
//              This source file contains the floppy scanning support for
//              NAVAP
//
//*****************************************************************************
//      $Log:   S:/NAVAPSVC/VCS/navapscn.cpv  $
// 
//    Rev 1.17   23 Dec 1998 15:05:56   RStanev
// Casted pvInBuffer to BOOL for CheckForDrive().
//
//    Rev 1.16   23 Dec 1998 13:56:08   RStanev
// If the VxD has requested a floppy to be scanned, we should not use the
// 3-second timeout.
//
//    Rev 1.15   24 Jun 1998 22:54:52   RStanev
// More quarantine fixes.
//
//    Rev 1.14   17 Feb 1998 12:40:54   mdunn
// Ported NEC code from the ROSNEC codebase.
//
//    Rev 1.13   02 Sep 1997 13:29:50   JBRENNA
// Use the Scanner options rather than the TSR options to determine if the
// user wants MBR and BR scanning off.
//
//    Rev 1.12   21 Aug 1997 02:41:44   DSACKIN
// Added a line to get the machine type before sending the packet
// over the wire.
//
//    Rev 1.11   19 Aug 1997 15:53:50   DALLEE
// ScanBoot() Added code to check for presence of a drive with timeout
// before calling VirusScanBoot(). Should fix problems shutting down services
// on laptops with disconnected floppies.
//
//    Rev 1.10   13 Aug 1997 22:35:50   DALLEE
// Added new NAVEX.INF location parameter to VirusScanInit().
// Changed scan MBR, scan boot checks on startup to query TSR options instead
// of SCANNER options.
//
//    Rev 1.9   07 Aug 1997 16:54:32   MKEATIN
// On startup scanning, check the options/scanning MBR and Boot record
// scanning options before scanning the MBR/Boot records.
//
//    Rev 1.8   11 Jul 1997 12:24:06   RStanev
// Added custom response support.
//
//    Rev 1.7   17 Jun 1997 18:38:40   MKEATIN
// VirusScanInit() now takes &gstNAVCallBacks as a parameter.
//
//    Rev 1.6   09 Jun 1997 09:13:22   JBRENNA
// Ops ... should of moved szDefDir declaration up when VirusScanSetPath moved.
//
//    Rev 1.5   09 Jun 1997 09:10:16   JBRENNA
// We must call VirusScanSetPath before calling VirusScanInit().
//
//    Rev 1.4   06 Jun 1997 11:19:18   JBRENNA
// Call VirusScanSetPath with the full path to the shared definitions
// directory.
//
//    Rev 1.3   31 May 1997 18:33:36   MKEATIN
// Passed NULL as the lpvCookie parameter in VirusScanRepairBoot().
//
//    Rev 1.2   16 May 1997 16:26:48   DSACKIN
// Switched from the old CAlertNotification to CVirusAlert.
//
//    Rev 1.1   18 Apr 1997 13:59:24   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:12:18   RFULLER
// Initial revision
//
//    Rev 1.6   07 Feb 1997 07:28:14   TSmith
// Modified 'ReportBootVirusScan' to always call 'NavGetDomainName' regardless
// of whether the floppy was scanned on startup/shutdown or on a mount request.
// This keeps the domain name that is reported consistent.
//
//    Rev 1.5   24 Jan 1997 19:50:08   TSmith
// Removed function GetDomainName & put in NAVXUTIL.
//
//    Rev 1.4   22 Jan 1997 15:21:46   TSmith
// Added nonmember function GetDomainName. Added 'pvInBuffer' argument to
// 'ScanBoot' to facilitate passing a NAVAP_SCANMOUNTEDDEVICEPACKET buffer
// through to 'ReportBootVirusEvent'. Added 'pvInBuffer' argument to that
// function along with logic to extract the user and domain names from that
// buffer when the service has scanned a boot record on behalf of the defice
// driver. For startup and shutdown BR scans, 'ReportBootVirusEvent' gets
// the domain and user names itself.
//
//    Rev 1.3   16 Jan 1997 03:36:56   DSACKIN
// Removed the endpoint from the Boot Sector RPC call to the Alert Service.
//
//    Rev 1.2   24 Dec 1996 18:53:52   DSACKIN
// Added include "afxtmpl.h"
//
//    Rev 1.1   08 Nov 1996 17:41:44   TSMITH
// Removed alerting, messaging and logging functionality from all NAVAPSVC
// modules. Removed string resources regarding virus found and virus action
// events. Added link to ALRTNTFY for NOTIFICATION_PACKET handling.
//
//    Rev 1.17   18 Oct 1996 18:23:34   TSmith
// Removed obsolete #ifdefs.
//
//    Rev 1.16   12 Sep 1996 11:02:08   jmillar
// Dan's fix to not knock empty SyQuest drives off-line
//
//    Rev 1.15   07 Aug 1996 15:06:44   PVENKAT
// Fixed #63616.  Avoided Action String on MBR/BR infection with
// Repair Automatically.  This case the Message itself contains the action.
//
//    Rev 1.14   05 Aug 1996 15:55:30   DSACKIN
// Removed all calls to ErrorHandlerID because ErrorHandler will work now.
// LogEvent has been re-worked to not need a NULL string at the end.
//
//    Rev 1.13   01 Aug 1996 09:02:22   PVENKAT
// Added Action string for Boot-Record Alerts.  #62612 fixed.
//
//
//    Rev 1.12   12 Jul 1996 16:47:46   DSACKIN
// Reworked the StartupScan to use QueryDosDevice.  This fixes the bug reported
// where if a user hasn't accessed a NetWare drive yet, we kick him offline.
//
//    Rev 1.11   28 Jun 1996 16:20:44   DSACKIN
// Fixed our call to ToBroadcast.
//
//    Rev 1.10   24 Jun 1996 23:08:42   DSACKIN
// Re-worked ScanBoot to handle our new logging scheme.
//
//    Rev 1.9   17 Jun 1996 19:59:12   DSACKIN
// Included APSCOMM.H for a change to NAVAPSVC.H
//
//    Rev 1.8   13 Jun 1996 17:24:10   DSACKIN
// Added DEBUG_PROFILE strings.
//
//    Rev 1.7   05 Jun 1996 22:36:08   DSACKIN
// Removed the StartupScan function.
//
//    Rev 1.6   05 Jun 1996 22:28:56   DSACKIN
// Since RepairBoot will only happen when we already have a SymStat lock
// on the drive, don't bother locking it again.
//
//    Rev 1.5   28 May 1996 15:00:22   DSACKIN
// Scan MBRs on 0x80 and 0x81 instead of only 0x80.
//
//    Rev 1.4   28 May 1996 13:33:08   DSACKIN
// If ScanBoot does a repair, we don't need to lock the drive twice.  Only
// lock the drive in ScanBoot.
//
//    Rev 1.3   23 May 1996 13:07:34   DSACKIN
// Added code for a startup scan.
//
//    Rev 1.2   22 May 1996 18:51:48   DSACKIN
// Set error handling mode to FailCritical so we don't get the Insert disk box.
//
//    Rev 1.1   21 May 1996 15:42:42   DSACKIN
// ScanBoot and RepairBoot return TRUE/FALSE if boot is STILL infected.
//
//    Rev 1.0   16 May 1996 11:10:04   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//      Includes

// Quake & System Include Files
#include <afxtempl.h>
#include <platform.h>
#include <process.h>
#include <xapi.h>
#include <nonav.h>
#include <navaplog.h>
#include "virscan.h"
#include "symstat.h"
#include "navstat.h"
#include "navver.h"
#include "uniutil.h"
#include "navcb.h"

#include "apscomm.h"                    // For apps to communicate with me.

#if !defined( __NAVRPC_H )
#   include <navrpc.h>
#endif

// Project Include files.
#include "service.h"
#include "navapsvc.h"
#include "apsvcstr.h"

/////////////////////////////////////////////////////////////////////
//      Constants
                                        // See VIRSCAN.H for ERR_VIRSCAN_*
                                        // and keep this different than all.
#define ERR_DISKLOCKED_SYMSTAT      100
#define ERR_DISKLOCKED_SYMSTAT_MBR  101
#define ERR_DISKLOCKED_SYMSTAT_BOOT 102

#define MAX_MESSAGE_LENGTH          (512)

/////////////////////////////////////////////////////////////////////
// Externs
extern CNavAPService APService;


/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.

static BOOL CheckForDrive( UINT uDrive, BOOL bLongTimeOut );
static unsigned __stdcall CheckForDriveProc( void *pvDrive );

/////////////////////////////////////////////////////////////////////
// Functions

/////////////////////////////////////////////////////////////////////
// ReportBootVirusEvent
//
//  This routine constructs a CVirusAlertobject with the
//  relevant virus detection information and forwards it to the
//  Alert Service.
//
// Parameters:
//   LPTSTR   - lpszVirusName     [in] - Virus name to report
//   UINT     - uDrive            [in] - Drive we are talking about
//   BOOL     - bIsInfected       [in] - IS it infected or WAS it?
//   BOOL     - bAttemptedRepair  [in] - Did we try to repair?
//   LPVOID   - pvInBuffer        [in] - Driver requested boot scan?
//                                       If this is != NULL then assume
//                                       the boot sector scan was requested
//                                       by the device driver & get user
//                                       and domain names from this buffer.
//                                       If this == NULL, then the boot
//                                       record was scanned due to service
//                                       startup or shutdown.
//
// Return Values:
//    none
//
// Notes:
//
/////////////////////////////////////////////////////////////////////
// 05/24/96 - DSACKIN - Function Created
// 01/21/97 - TSMITH  - Added 'pvInBuffer' argument and logic to
//                      extract user and domain names from this buffer
//                      assuming that when this pointer is != NULL,
//                      that the boot record was scanned due to a
//                      Scan Mounted Device request from the driver.
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::ReportBootVirusEvent(
   LPTSTR lpszVirusName,                // [in] - Virus name to report
   UINT   uDrive,                       // [in] - Drive we are talking about
   BOOL   bIsInfected,                  // [in] - IS it infected or WAS it?
   BOOL   bAttemptedRepair,             // [in] - Did we try to repair?
   LPVOID pvInBuffer)                   // [in] - Optional buffer from driver
{
NAVAP_SCANMOUNTEDDEVICEPACKET* pSMDP;   // Contains a NAVAP_USERINFOSUBPACKET
CVirusAlert  cVirusAlert;
TCHAR        szDrive[ 2 + 1 ];
TCHAR        szResStr[ MAX_RES_STR_LEN + 1 ];
TCHAR        szSystemName[ MAX_COMPUTERNAME_LENGTH + 1 ];
TCHAR        szDomainName[ MAX_COMPUTERNAME_LENGTH + 1 ];
TCHAR        szUserName[ 20 + 1 ];
DWORD        dwAction;
DWORD        dwSystemLength = sizeof( szSystemName ) / sizeof( TCHAR );
DWORD        dwUserLength = sizeof( szUserName ) / sizeof( TCHAR );
DWORD        dwDomainLength = sizeof( szDomainName ) / sizeof( TCHAR );
DWORD        dwRpcStatus;
PBYTE        pBuffer;
BOOL         bSuccess;
UINT         cbDomainSize = sizeof( szDomainName );
INT          nStatus;

   LogMessage( _T( "Entering ReportBootVirusEvent( )" ) );

   cVirusAlert.SetPacketSource(
      PACKET_SOURCE_LOCAL | PACKET_SOURCE_NAVAP);

   cVirusAlert.CreatePacketMachineType();

   pSMDP = (NAVAP_SCANMOUNTEDDEVICEPACKET*) pvInBuffer;

                                        // Insert UUID into packet
   cVirusAlert.CreateUniqueID();

                                        // Set the alert type for the kind
                                        // of boot record we're talking
                                        // about.
   cVirusAlert.SetAlertType(
      uDrive >= 0x80 ?
         MBR_KNOWN_VIRUS :
         BOOT_RECORD_KNOWN_VIRUS );

                                        // Compute actions that were taken.
   if ( bAttemptedRepair )
   {
      dwAction = ACTION_REPAIR;
      bSuccess = !bIsInfected;
   }
   else
   {
      dwAction = ACTION_NONE;
   }

                                        // Add actions to the packet
   cVirusAlert.SetAction(
      dwAction);
   cVirusAlert.SetActionSuccess(
      bSuccess);

                                        // Insert the SystemName next
   GetComputerName(
      szSystemName,
      &dwSystemLength);

   cVirusAlert.SetSystemName(
      szSystemName);


                                        // Now figure out the Domain.
   memset(
      szDomainName,
      0,
      sizeof(szDomainName) );

                                        // Get domain name that local system is
                                        // a member of.
   if ( NavGetDomainName(
           szSystemName,
           cbDomainSize,
           szDomainName) )
   {
      cVirusAlert.SetDomainName(
         szDomainName);
   }
   else
   {
        ErrorHandler(
           IDS_ERR_GETDOMAINNAME_FAILED);
   }

   if ( NULL != pSMDP)
   {
      cVirusAlert.SetTime(
         (SYSTEMTIME&) pSMDP->rSystemTime);

      DecodeNAVAPUserInfoSubPacket(
         &(pSMDP->rUser),
         szUserName,
         &dwUserLength,
         szDomainName,
         &dwDomainLength );

      cVirusAlert.SetUserName(
         szUserName);
   }
   else
   {
                                        // For startup & shutdown scans, no user
                                        // is associated with the scan event so
                                        // use our predefined user name string.
      nStatus = LoadString(
                   m_hInst,
                   IDS_NAVAP_BOOTSCAN_USER,
                   szResStr,
                   MAX_RES_STR_LEN );

      if ( nStatus > 0 )
      {
                                        // Plug our canned name into the packet.
         cVirusAlert.SetUserName(
            szResStr);
      }
      else
      {
         ErrorHandler(
            ID_ERR_LOADSTRING_FAILED);
      }

                                        // Use the member function to take down
                                        // the current time.
      cVirusAlert.StampTime();
   }


                                        // If drive # >= 0x80, we know its MBR
   if ( uDrive >= 0x80 )
   {
      SPRINTF(
         szDrive,                       // Now format the drive string so we
         _T( "%d" ),                    // can pass it to the ErrorHandler
         uDrive - 0x80 );               // Use drive 0, 1, 2 etc...

   }
   else
   {
      SPRINTF(
         szDrive,                       // Now format the drive string so we
         _T( "%c:" ),                   // can pass it to the ErrorHandler
         CharToUpper( uDrive ) );
   }

   cVirusAlert.SetObjectName(
      szDrive);

   cVirusAlert.SetVirusName(
      lpszVirusName);

                                        // Get the actual information that we
                                        // are going to send over the wire.
   pBuffer = cVirusAlert.GetPacket();

                                        // Wait for the Alert Service to become
                                        // ready or something bad happens in
                                        // Service Startup
   while (!m_bExitingService &&
          !m_bAlertSvcReady)
   {
      Sleep (500);
   }

                                        // Only try to send the Packet if the
                                        // Alert Service is Ready.
   if (m_bAlertSvcReady)
   {
                                        // It is important that we are only
                                        // allowed to send ONE RPC packet at a
                                        // time because all transmissions are
                                        // using the same UUID
      EnterCriticalSection(&m_RpcCritical);

      dwRpcStatus = NavRpcSendPacket(
                       ALERTSVC_PROTOCOL,
                       ALERTSVC_ADDRESS,
                       NULL,
                       (LPVOID) pBuffer );

                                        // Allow the next callback to attempt
                                        // sending his.
      LeaveCriticalSection(&m_RpcCritical);

      if ( dwRpcStatus != ERROR_SUCCESS )
      {
         ErrorHandler(
            ID_ERR_ALERT_SERVICE_RPC_XMT_FAILED |
            ID_ERR_FATAL );
      }
   }

                                        // Don't forget to unlock our packet
                                        // when we are done.
   cVirusAlert.ReleasePacket();
}


/////////////////////////////////////////////////////////////////////
// RepairBoot
//
// This routine will attempt to repair the boot sector of an infected
// floppy drive
//
// Parameters:
//    UINT    uDrive  - Which drive to repair
//                       - 'A'-'Z' (less than 0x80) for boot records
//                       - 0x80+  MBR of the physical drive
//
// Return Values:
//    TRUE  - Virus is still present
//    FALSE - Virus was eliminated.
//
// Notes:
//    All actions are taken care of here instead of sending back the
//    error code except if virus is present.
//
/////////////////////////////////////////////////////////////////////
// 05/15/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::RepairBoot (
   UINT uDrive,                         // [in] - Drive letter to repair
   UINT uVirusSigIndex)                 // [in] - What is the Virus ID?
{
BOOL        bStatus;
UINT        uRepairType = MBR_REPAIR;
BOOL        bReturn = TRUE;             // Assume there IS a virus!!!

   LogMessage(_T("Entering CNavAPService::RepairBoot()"));

   if (uDrive < 0x80)
      uRepairType = BOOT_REPAIR;

                                        // Attempt repair
   bStatus = VirusScanRepairBoot (
               uDrive,
               uVirusSigIndex,
               uRepairType,
               NULL);

                                        // If we couldn't repair, let the
                                        // user know this.
   if ( bStatus )
   {

      LogMessage(_T("CNavAPService::RepairBoot() - Unable to repair Boot/MBR virus."));

      bReturn = FALSE;                  // Virus was eliminated!!!
   }

   LogMessage(_T("Exiting CNavAPService::RepairBoot()"));

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// ScanBoot
//
// This routine processes requests from NAVAP device and scans the
// Boot sectors on request.
//
// Parameters:
//    UINT    uDrive  - Which drive to scan
//                       - 'A'-'Z' (less than 0x80) for boot records
//                       - 0x80+  MBR of the physical drive
//    LPVOID  pvInBuffer - Optional buffer from driver. If this is
//                         not NULL, then the request to scan the
//                         boot records is assumed to have come from
//                         the device driver and we can pass the buffer
//                         on to ReportBootVirusEvent so that user info
//                         can be included in the log entries. If NULL,
//                         we can assume the request is the result of
//                         the service being started or shutdown.
//    BOOL    bTryRepair - If infected, do we try to repair it?
//
// Return Values:
//    TRUE  - Boot Record is still infected upon exit.
//    FALSE - Boot Record is clean or an error occurred.
//
// Notes:
//    All actions are taken care of here instead of sending back the
//    error code except for if Boot Record is still infected.
//
/////////////////////////////////////////////////////////////////////
// 05/13/96 - DSACKIN - Function Created
// 05/14/96 - DSACKIN - Function revised to do MBRs and Boot records.
// 06/24/96 - DSACKIN - Changed logic to support new ReportBootVirusEvent function
// 01/21/97 - TSMITH  - Added pvInfBuffer argument to pass through to
//                      ReportBootVirusEvent.
// 10/14/97 Mike Dunn - Fixed #98410, AP not catching an infected floppy
//                      if the floppy drive has never been accessed before
//                      the scan takes place. (ported from ROSNEC)
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::ScanBoot (
   UINT   uDrive,                       // [in] - Drive letter to scan
   LPVOID pvInBuffer,                   // [in] - Optional buffer from driver
   BOOL   bTryRepair)                   // [in] - If infected, do we try repair?
{
N30NOTES rNotes;
STATUS   rStatus;
DWORD    dwWaitResult;
TCHAR    szVirusNameW[MAX_MESSAGE_LENGTH];
UINT     uVirusSigIndex;
UINT     uError;
UINT     uLen;
UINT     uOldErrorMode;
BOOL     bReturn = FALSE;               // Assume no infection
BOOL     bIsInfected = FALSE;
BOOL     bDrivePresent = TRUE;

   TCHAR szMessage[256];

   LogMessage(_T("Entering CNavAPService::ScanBoot()"));

   SPRINTF(
      szMessage,
      (uDrive >= 0x80) ?
         _T("Attempting to scan MBR of Drive 0x%X") :
         _T("Attempting to scan Boot Record of Drive %c:"),
      uDrive);

   LogMessage(szMessage);

                                        // Set error mode for our scan.
   uOldErrorMode = SetErrorMode (SEM_FAILCRITICALERRORS);

                                        // If floppy scan, check floppy drive
                                        // is present - fix for laptops w/
                                        // disconnected floppies.
   if ( ( uDrive < 0x80 ) &&
        ( FALSE == CheckForDrive( uDrive, (BOOL)pvInBuffer ) ) )
   {
      if ( HWIsNEC() )
      {
         // Try checking the drive again -- this works around a bug in NT
         // where no alert is generated if the floppy drive hasn't been
         // accessed in your current session, and you put in an infected
         // floppy and then start/stop the service.  The first
         // CheckForDrive() is returning FALSE for some reason.
         // Defect #98410.

         // ***** NOTE to Atomic/Maxwell folks *****  This might also fix
         // the bug where the AP won't detect an infected floppy when you
         // stop the service right after you boot the machine.  (see
         // defects 93428 (Ros/Tesla) and 97358 (Ros/Tesla NEC)).  I won't
         // mess with the AT behavior for now, though. 10/14/97 Mike Dunn

         LogMessage ( _T("First check for drive failed - trying again") );

         Sleep(3);

         if ( FALSE == CheckForDrive ( uDrive, (BOOL)pvInBuffer ) )
         {
            bDrivePresent = FALSE;
            // The failure will be logged below.
         }
         else
         {
            LogMessage ( _T("Drive found on second attempt.") );
         }
      }
      else
      {
         bDrivePresent = FALSE;         // On ATs, keep the old behavior of
                                        // just trying once.  But see note
                                        // above.
      }


      if ( !bDrivePresent )
      {
         LogMessage(_T("Skipping ScanBoot - drive not present"));
      }
   }

   if ( bDrivePresent )
   {
      dwWaitResult = WaitForSingleObject(
                        m_hNavStatMutex,// Handle to our mutex
                        3000L);         // Wait 3 seconds

      if (WAIT_OBJECT_0 == dwWaitResult)// Did we succeed?
      {
         char szDefDir[_MAX_PATH];
         char szNavexInf[ SYM_MAX_PATH + 1 ];

         LogMessage(_T("Calling VirusScanInit()"));

                                        // Point VirScan to our specific
                                        // shared defs directory. Must be
                                        // called before VirusScanInit().
         m_DefUtilsAppId1.GetCurrentDefs (szDefDir, sizeof (szDefDir));
         VirusScanSetPath (szDefDir);

         GetNavexInfFile( m_hInst, szNavexInf, SYM_MAX_PATH );

                                        // Initialize the Virus Library
         if ( ERR != VirusScanInit(&gstNAVCallBacks, szNavexInf) )
         {

            if ( uDrive >= 0x80)        // MBR?
            {
               LogMessage(_T("Calling VirusScanMasterBoot(0x%X)"), uDrive);

               uError = 0;

               uVirusSigIndex = VirusScanMasterBoot (
                                   uDrive,
                                   &rNotes,
                                   &uError);
            }
            else // (uDrive >= 0x80)    // Boot sector
            {

               LogMessage(_T("Attempting to lock the drive %c:"), uDrive);

               rStatus = NavStatSetDriveState (
                           uDrive,
                           0,
                           DRIVESTATUS_BACKGROUND,
                           NAV_APP_CLINIC);

               if ( ERR != rStatus )
               {

                  LogMessage(_T("Calling VirusScanBoot(%c:)"), uDrive);

                  uError = 0;
                                        // Here is where the actual scan happens
                  uVirusSigIndex = VirusScanBoot(
                                    uDrive,
                                    &rNotes,
                                    &uError);
               }
               else
                  ErrorHandler (ID_ERR_NAVSTATSETDRIVESTATE);
            }

            if ( 0 == uError)           // Did we scan OK?
            {
               if ( 0 != uVirusSigIndex )  // Do we have a virus?
               {
                  LogMessage(_T("Virus Found"));

                  bIsInfected = TRUE;

                                        // We DO have a virus at this point.
                  bReturn = TRUE;

                  if ( bTryRepair )
                  {
                                              // Try to repair it.
                     if (!RepairBoot (
                           uDrive,            // Which drive?
                           uVirusSigIndex))   // Which Virus?
                     {
                        bIsInfected = FALSE;  // Cleaned it!

                                              // RepairBoot will return FALSE if
                                              // the virus has been repaired.
                        bReturn = FALSE;
                     }
                  }

                  uLen = MultiByteToWideChar (
                           CP_ACP,
                           MB_PRECOMPOSED,
                           rNotes.lpVirName,
                           -1,
                           szVirusNameW,
                           sizeof(szVirusNameW) / sizeof(szVirusNameW[0]) );

                  SYM_ASSERT (0 != uLen);

                  if (0 != uLen)
                  {
                                        // Log our actions!
                     ReportBootVirusEvent(
                        szVirusNameW,
                        uDrive,
                        bIsInfected,
                        bTryRepair,
                        pvInBuffer);    // Pass buffer from driver through
                  }

               }
            }

            if ( (uDrive < 0x80) &&
               (ERR != rStatus) )       // BOOT Record need closing?
            {
               LogMessage(_T("Attempting to remove our lock on the drive %c:"), uDrive);
                                        // This must be done if NavStatSetDriveState
                                        // worked the first time.
               rStatus = NavStatSetDriveState (
                           uDrive,
                           0,
                           DRIVESTATUS_FREE,
                           NAV_APP_CLINIC );

               if ( ERR == rStatus )
                  ErrorHandler (ID_ERR_NAVSTATSETDRIVESTATE);
            }

            LogMessage(_T("Calling VirusScanEnd()"));

            if ( TRUE != VirusScanEnd() )
               ErrorHandler (ID_ERR_VIRUSSCANEND);
         }
         else // (ERR != VirusScanInit())
            ErrorHandler (ID_ERR_VIRUSSCANINIT);

                                        // At this point, we are done with NavStat
         if (!ReleaseMutex (m_hNavStatMutex))
         {
            bReturn = FALSE;
            SYM_ASSERT(0);
         }

      }  // if (WAIT_OBJECT_0 == dwWaitResult)
      else
      {
         ErrorHandler (ID_ERR_WAITFORMUTEXFAILED);
         bReturn = FALSE;
      }
   }
                                        // Restore the error mode
   SetErrorMode (uOldErrorMode);

   LogMessage(_T("Exiting CNavAPService::ScanBoot()"));

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CheckForDrive()
//
// This routine checks for the presence of a drive before boot scanning.
// Work around for some laptops w/ disconnected floppy drives.
// Time out for floppy drive may be as long as 40 seconds - need to
// shorten this up.
//
// Parameters:
//    UINT  uDrive;             Drive letter 'A' to 'Z' as in ScanBoot().
//    BOOL  bLongTimeOut;       0 if the function should allow the full 40
//                              seconds time out, non-0 if the function
//                              should time out in shorter time.
//
// Return Value:
//    TRUE if drive is present
//    FALSE otherwise.
/////////////////////////////////////////////////////////////////////
// 08/18/1997 DALLEE - Function Created
/////////////////////////////////////////////////////////////////////

static BOOL CheckForDrive( UINT uDrive, BOOL bLongTimeOut )
{
   auto  BOOL     bPresent = TRUE;
   auto  HANDLE   hThread;
   auto  unsigned uThread;

                                        // Begin separate thread to ping
                                        // drive.
   hThread = (HANDLE) _beginthreadex( NULL, 0, CheckForDriveProc, (void *)uDrive, 0, &uThread );

   if ( NULL != hThread )
   {
      if ( WAIT_TIMEOUT == WaitForSingleObject( hThread, bLongTimeOut ? 40000 : 3000 ) )
      {
         TerminateThread( hThread, 1 );
         bPresent = FALSE;
      }

      CloseHandle( hThread );
   }

   return ( bPresent );
}

/////////////////////////////////////////////////////////////////////
// CheckForDriveProc()
//
// Asynchronous routine used by CheckForDrive().
// Pings drive and must return within wait period specified in
// CheckForDrive().
/////////////////////////////////////////////////////////////////////
// 08/18/1997 DALLEE - Function Created
/////////////////////////////////////////////////////////////////////

static unsigned __stdcall CheckForDriveProc( void *pvDrive )
{
   auto  TCHAR    szDrive [] = _T("X:\\");

   szDrive[0] = (TCHAR) pvDrive;

   GetVolumeInformation( szDrive, NULL, 0, NULL, NULL, NULL, NULL, 0 );

   _endthreadex( 0 );

   return ( 0 );
}

/////////////////////////////////////////////////////////////////////
// StartupScan
//
// Here is where we will scan the drives upon startup.
//
// Parameters:
//    none.
//
// Return Value:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 05/07/1996 DSACKIN - Function Created
// 11/5/97 Mike Dunn -- Changed so that RecordFloppyDrives() is always
//                      called. It shouldn't have been NEC-specific.
//                      (ported from ROSNEC)
/////////////////////////////////////////////////////////////////////
#define MAX_DRIVE_TO_SCAN 0x81
VOID CNavAPService::StartupScan(VOID)
{
UINT   uDrive, uReturn;
DWORD  dwDriveMask, dwDriveBit;
TCHAR  szDrive[4] = _T("A:");
TCHAR  szDriveInfo[256];
TCHAR  szNetworkDevice[32] = _T("\\DEVICE\\NWRDR");
TCHAR  szHardDiskDevice[32] = _T("\\DEVICE\\HARDDISK");
BOOL   bTryRepair = FALSE;
BOOL   bReturn;
DWORD  dwSerialNumber;
DWORD  dwFlags;
DWORD  dwMaxFileNameLength;

   LogMessage(_T("Entering CNavAPService::StartupScan()"));
                                        // Check for the auto repair option!
   switch ( m_NavOptions.tsr.uActionKnown == TSR_CUSTOM_RESPONSE ?
               m_NavOptions.customAP.uchBoot :
               m_NavOptions.tsr.uActionKnown )
   {
      case TSR_REPAIR:
      case TSR_DELETE:
      case TSR_QUAR_CANT_REP:

         bTryRepair = TRUE;
   }

                                        // Start with MBRs
                                        // ...but only on ATs.
   if ( !HWIsNEC() )
   {
      if (!m_NavOptions.scanner.bMasterBootRec)
      {
          LogMessage(_T("CNavAPService::StartupScan() - MBR scanning turned off in option panel - not scanning Master Boot Records"));
      }
      else
      {
         for ( uDrive = 0x80;
                 ( (uDrive <= MAX_DRIVE_TO_SCAN) &&
                 (!m_bExitingService) );
                 uDrive++)
         {
            ScanBoot(uDrive, NULL, bTryRepair);    // Try them all.
         }
      }
   }

                                        // Now for the Boot Records

   if (!m_NavOptions.scanner.bBootRecs)
   {
      LogMessage(_T("CNavAPService::StartupScan() - Boot scanning turned off in option panel - not scanning Boot Records"));
   }
   else
   {

                                        // For logical drives, query the
                                        // available drive and only scan them.

      dwDriveMask = GetLogicalDrives();

                                        // Each bit represents the next
                                        // logical drive.
      for ( uDrive = 'A', dwDriveBit = 1;
            ( (uDrive <= 'Z') &&
              (!m_bExitingService) );
            uDrive++, dwDriveBit <<= 1)
      {
                                        // If the bit is set, scan the drive.
         if (0 != (dwDriveMask & dwDriveBit))
         {
                                        // Build the drive string
            szDrive[0] = uDrive;

            uReturn = QueryDosDevice(
                         szDrive,
                         szDriveInfo,
                         sizeof (szDriveInfo));

            LogMessage(_T("StartupScan() - QueryDosDevice [%s]"), szDriveInfo);

            if (0 == uReturn)
            {
               continue;
            }

            STRUPR (szDriveInfo);

                                        // Make sure they aren't trying to scan
                                        // something that uses NwRdr
            if ( !STRNCMP(
                     szDriveInfo,
                     szNetworkDevice,
                     STRLEN(szNetworkDevice)) )
            {
               continue;
            }

                                        // Add special handling for removable
                                        // hard disk devices.
            if ( !STRNCMP(
                     szDriveInfo,
                     szHardDiskDevice,
                     STRLEN(szHardDiskDevice)) )
            {
               if ( HWIsNEC() )
               {
                  continue;             // skip hard drives on NECs.
               }

                                        // Get The Drive type.
               uReturn = GetDriveType(
                            szDrive);

               if (DRIVE_REMOVABLE == uReturn)
               {
                  LogMessage(_T("StartupScan() - Found a removable HardDrive"));
                                        // For Removable HardDisk types, we
                                        // want to check the volume label to
                                        // make sure that there is a disk in
                                        // the dirve.  (Fix for SyQuest drives).

                  bReturn = GetVolumeInformation(
                              szDrive,          // Path to drive root
                              NULL,             // address of name of the volume
                              0,                // size of name of volume buffer
                              &dwSerialNumber,  // pointer to DWORD that will receive volume ID
                              &dwMaxFileNameLength, // Address of DWORD for max filename length
                              &dwFlags,         // Address of DWORD to receive flags
                              NULL,             // Buffer to receive name of file system
                              0);               // size of buffer to receive name of file system

                  if (!bReturn)
                  {
                     LogMessage(_T("StartupScan() - Found a removable HardDrive - Unable to get Volume Label."));
                     continue;
                  }
               }
            }

            ScanBoot (uDrive, NULL, bTryRepair);
         }
      }
   }

   RecordFloppyDrives();

   LogMessage(_T("Exiting CNavAPService::StartupScan()"));

   return;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:   CNavAPService::RecordFloppyDrives()
//
// Description:
//   Stores info about the floppy drives on the machine in the service's
//   member variables.
//
// Input:
//   Nothing.
//
// Returns:
//   Nothing.
//
//////////////////////////////////////////////////////////////////////////
// 9/11/97 Mike Dunn -- Code ported from Gryphon NEC and moved to its
//                      own function here.
//////////////////////////////////////////////////////////////////////////

void CNavAPService::RecordFloppyDrives()
{
TCHAR  szDrive[] = _T("x:");
TCHAR  szDriveInfo[256];
DWORD  fdwLogicalDrives, dwDriveBit;
                                        // **NOTE** this string must be uppercase
TCHAR  szFloppyDiskDevice[] = _T("\\DEVICE\\FLOPPY");
UINT   uDrive;

   LogMessage(_T("Entering CNavAPService::RecordFloppyDrives()"));

   fdwLogicalDrives = GetLogicalDrives();

   for ( uDrive = 'A', dwDriveBit = 1;
         uDrive <= 'Z'  &&  !m_bExitingService;
         uDrive++, dwDriveBit <<= 1 )
   {
                                        // If the bit is set, query the drive.
      if ( 0 != ( fdwLogicalDrives & dwDriveBit ) )
      {
                                        // Build the drive string
         szDrive[0] = uDrive;

         if ( 0 == QueryDosDevice ( szDrive,
                                    szDriveInfo,
                                    sizeof (szDriveInfo) ) )
         {
            continue;                   // move on if we couldn't query the drive
         }

         STRUPR ( szDriveInfo );

         // Check if the current drive is a floppy.

         if (! STRNCMP ( szDriveInfo,
                         szFloppyDiskDevice,
                         STRLEN(szFloppyDiskDevice) ) )
         {
            // Found a floppy!
            m_fdwFloppiesOnSystem |= dwDriveBit;

            // Now check if this is the first floppy we've come across.
            if ( m_cFirstFloppyDrive == 0 )
            {
               m_cFirstFloppyDrive = uDrive;

               LogMessage ( _T("First floppy drive found -- %s"), szDrive );
            }
         }
      }  // end if ( drive exists )
   }  // end for

   LogMessage ( _T("CNavAPService::m_cFirstFloppyDrive == %c."),
                m_cFirstFloppyDrive );

   LogMessage ( _T("CNavAPService::m_fdwFloppiesOnSystem == %lu."),
                m_fdwFloppiesOnSystem );

   LogMessage (_T("Exiting CNavAPService::RecordFloppyDrives()"));
}

