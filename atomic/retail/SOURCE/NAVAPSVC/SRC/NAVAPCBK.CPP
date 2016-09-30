// Copyright 1996 Symantec Corporation
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVAPSVC/VCS/navapcbk.cpv   1.17   09 Jun 1998 20:18:58   RStanev  $
//
// Description:
//    This source file contains routines for NAV Alert Service (for NT)
//
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVAPSVC/VCS/navapcbk.cpv  $
// 
//    Rev 1.17   09 Jun 1998 20:18:58   RStanev
// Processing quarantine's incoming directory for every successful action by AP.
//
//    Rev 1.16   28 May 1998 03:58:04   RStanev
// TSR_QUAR_CANT_REPAIR should be considered as repair when it comes down
// to boot records.
//
//    Rev 1.15   21 May 1998 11:59:54   tcashin
// Removed INITGUID. It's now in NAVAPSVC.CPP.
//
//    Rev 1.14   19 May 1998 09:54:02   mdunn
// Worked a little magic with the preprocessor directives so it'd get the
// DEFINE_GUID stuff right.
//
//    Rev 1.13   13 May 1998 20:18:34   DALLEE
// Added translation of AVACTION_QUARANTINE to ACTION_QUARANTINE for
// alerting. Alert service should now have info to display proper messages, etc.
//
//    Rev 1.12   13 May 1998 16:03:32   DALLEE
// AP Service now performs final processing on driver's quarantine items:
//     Set m_bIncomingQuarantineItems on event progress if quarantine action.
//     Notification queue calls ProcessAllIncomingFiles() later if set.
//
//    Rev 1.11   23 Feb 1998 17:18:22   DALLEE
// Removed handling of AVACTION_INOCULATE -- file inoculation
// handling and associated defines has been removed from the driver.
//
//    Rev 1.10   17 Feb 1998 12:40:32   mdunn
// Ported NEC code from the ROSNEC codebase.
//
//    Rev 1.9   20 Nov 1997 10:08:46   mdunn
// In LaunchNavw(), put the CreateProcess() and related stuff in a critical
// section to avoid starting up multiple instances of NAVW.
//
//    Rev 1.8   18 Nov 1997 13:50:12   mdunn
// Fixed to handle double-byte strings correctly.
//
//    Rev 1.7   17 Nov 1997 15:28:44   mdunn
// Put in Vance's changes (with a couple of my own tweaks) for AP prompting.
//
//    Rev 1.6   21 Aug 1997 02:25:52   DSACKIN
// Added code for inserting the current machine type into the CAlert packet.
//
//    Rev 1.5   11 Jul 1997 12:23:32   RStanev
// Added custom response support.
//
//    Rev 1.4   02 Jul 1997 15:56:20   DSACKIN
// ALWAYS send packets to the Alert Service now for logging even though
// we aren't supposed to process AutoProtect alerts.  The Alert Service
// will now take care of not sending alerts if we aren't supposed to do so.
//
//    Rev 1.3   16 May 1997 16:25:58   DSACKIN
// Switched from the old CAlertNotification to CVirusAlert.
//
//    Rev 1.2   30 Apr 1997 12:26:38   RStanev
// Updated to match the new APCOMM functionality.
//
//    Rev 1.1   18 Apr 1997 13:59:16   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:12:16   RFULLER
// Initial revision
//
//    Rev 1.11   07 Feb 1997 07:44:02   TSmith
// Modified 'CBKDecodePacket' to always call 'NavGetDomainName' to retrieve
// the domain name.
//
//    Rev 1.10   23 Jan 1997 22:44:28   DSACKIN
// Display our new RPC transmission failure message and then put the alert
// back into the queue.  Once the Alert Service comes back on line, then we
// will begin sending the alerts over to it.
//
//    Rev 1.9   22 Jan 1997 17:16:54   DSACKIN
// Stop processing outgoing packets when we get a SERVICE_CONTROL_STOP message.
//
//    Rev 1.8   22 Jan 1997 15:46:02   TSmith
// Modified calls to ScanBoot to  accomodate that function's new argument.
// Modified usage of CAlertNotification objects throughout to insert new
// packet source flags and domin name.
//
//    Rev 1.7   13 Jan 1997 22:23:16   DSACKIN
// Added code to make NAVAPSVC wait until the Alert Service tells it that
// it has finished starting up before we send over the first alert.
//
//    Rev 1.6   30 Dec 1996 11:05:36   TSmith
// Fixed access violation exception caused by invalid pointer access in
// 'NotificationQueue'. Also fixed defect where all alerts would contain the
// same file/infection information as the first (also in 'NotificationQueue').
//
//    Rev 1.5   24 Dec 1996 18:52:58   DSACKIN
// Changed this module to use a queue for events that are to be sent across
// the wire using RPC.
//
//    Rev 1.4   24 Dec 1996 16:45:54   DSACKIN
// Added critical section for our RPC call
//
//    Rev 1.3   12 Dec 1996 17:56:44   TSmith
// Modified 'CBKDecodePacket' to put the system name in the packet rather
// than the domain name as reported by NAVAP.
//
//    Rev 1.2   05 Dec 1996 03:58:06   DSACKIN
// We now check the option to see if we should process any alerts.
//
//    Rev 1.1   08 Nov 1996 17:40:20   TSMITH
// Removed alerting, messaging and logging functionality from all NAVAPSVC
// modules. Removed string resources regarding virus found and virus action
// events. Added link to ALRTNTFY for NOTIFICATION_PACKET handling.
//
//    Rev 1.15   18 Oct 1996 18:22:50   TSmith
// Removed obsolete #ifdefs.
//
//    Rev 1.14   07 Aug 1996 15:15:32   DSACKIN
// Removed some commented out code.
//
//    Rev 1.13   07 Aug 1996 15:06:24   PVENKAT
// Added SID for Log Event.
//
//
//    Rev 1.12   05 Aug 1996 15:54:46   DSACKIN
// Removed all calls to ErrorHandlerID because ErrorHandler will work now.
// LogEvent has been re-worked to not need a NULL string at the end.
//
//    Rev 1.11   01 Aug 1996 09:01:42   PVENKAT
// Added Action string in NT Event Log.  #62612 Fixed.
//
//    Rev 1.9   12 Jul 1996 19:43:06   pvenkat
// #57230 fixed.  Now Action taken will also be displayed as part of alert.
//
//    Rev 1.8   28 Jun 1996 16:21:12   DSACKIN
// Fixed our call to ToBroadcast.
//
//    Rev 1.7   13 Jun 1996 17:23:36   DSACKIN
// Added DEBUG_PROFILE strings.
//
//    Rev 1.6   06 Jun 1996 14:34:06   PVENKAT
// NotifyNavNLM is modified to take Alert packet instead of LogPacket.
//
//    Rev 1.5   20 May 1996 22:48:48   DSACKIN
// Fixed the Mount Callback to use the Packet being sent by the AP Driver.
//
//    Rev 1.4   17 May 1996 16:57:00   PVENKAT
// Uncommented NotifyNLM, which calls NotifyServersOfVirus to inform NW Server.
//
//    Rev 1.3   17 May 1996 16:32:08   DSACKIN
// Added code for the drive mount notification.
//
//    Rev 1.2   16 May 1996 20:32:08   DSACKIN
// Re-organized the callback function for better performance.
//
//    Rev 1.1   16 May 1996 15:30:12   DSACKIN
// Switched the order of drvcomm.h and apcomm.h
//
//    Rev 1.0   16 May 1996 11:09:12   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//      Includes

// Quake & System Include Files
#include <afxtempl.h>
#include <platform.h>
#include <process.h>

#include <nonav.h>
#include <navalog.h>
#include <navaplog.h>
#include <navbin.h>
#include <actions.h>
#include <symalog.h>
#include <navutil.h>                    // this #includes objbase.h & iquaran.h
#include <xapi.h>
#include <symstat.h>
#include <navstat.h>

#include <servenfy.h>

// NAV & AP Driver Include Files
#include <drvcomm.h>
#include <apcomm.h>
#include "apscomm.h"                    // For apps to communicate with me.

// Project Include files.
#include "service.h"
#include "navapsvc.h"
#include "apsvcstr.h"

#if !defined( __NAVRPC_H )
#   include <navrpc.h>
#endif

// Zoso!
// #if !defined( __ALRTNTFY_H )
// #   include "alrtntfy.h"                // CAlertNotificaton class
// #endif

#include "CVAlert.h"

#include "at_opts.h"                    // For CAlertOptions class
#include "uniutil.h"

//***************************************************************************/
//      Constants
//***************************************************************************/
#define MAX_THREAD_STACK_SIZE (10240)  // Max Stack size
#define MAX_AP_BUFSIZE        (10240)
#define MAX_SERVICE_NAME      (256)

                                        // These class names copied from
                                        // navw.cpp and navw.h (resp.) and
                                        // used in NavwRunning()
#define NAV_SCANWINDOW_CLASS _T("ScanWindow")
#define NAV_MAIN_WND_CLASS   _T("nav")

/////////////////////////////////////////////////////////////////////
// Globals

/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.

BOOL NavwRunning();

/////////////////////////////////////////////////////////////////////
// Functions


/////////////////////////////////////////////////////////////////////
// CPacketItem functions

/////////////////////////////////////////////////////////////////////
// CPacketItem::CPacketItem()
//
// Purpose:
//    Default constructor for our packet item.
//
// Parameters:
//    LPVOID - lpPacket - [in] - Packet from the AutoProtect.
//    DWORD  - dwSize   - [in] - Size of the packet coming in.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/24/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CPacketItem::CPacketItem(
   LPVOID lpPacket,
   DWORD  dwSize)
{
   SetPacket(lpPacket, dwSize);
}

/////////////////////////////////////////////////////////////////////
// CPacketItem::~CPacketItem()
//
// Purpose:
//    Default destructor for our CPacketItemClass
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/24/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CPacketItem::~CPacketItem()
{
                                        // Check for things to clean up.
   if (m_lpPacket)
   {
                                        // Get rid of any memory that we may
                                        // have allocated.
      delete m_lpPacket;
      m_lpPacket = NULL;
      m_dwSize = 0;
   }
}


/////////////////////////////////////////////////////////////////////
// CPacketItem::SetPacket()
//
// Purpose:
//    Sets the current packet.
//
// Parameters:
//    LPVOID lpPacket - [in] - Packet to be contained.
//    DWORD  dwSize   - [in] - Size of the packet coming in.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/24/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CPacketItem::SetPacket(
   LPVOID lpPacket,                     // [in] - Packet to be set.
   DWORD  dwSize)
{
   m_lpPacket   = new BYTE[dwSize];

   if (NULL != m_lpPacket)
   {
      MEMCPY (m_lpPacket, lpPacket, dwSize);
      m_dwSize   = dwSize;
   }
   else
      m_dwSize = 0;
}

/////////////////////////////////////////////////////////////////////
// CPacketItem::GetPacket()
//
// Purpose:
//    Gets the currently set up packet.
//
// Parameters:
//    LPVOID *lppPacket - [out] - Returns the pointer to our packet.
//    DWORD  *lpdwSize  - [out] - Returns the size of the packet.
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/24/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CPacketItem::GetPacket(
   LPVOID * lppPacket,
   DWORD  * lpdwSize)
{
                                        // Copy out our member variables.
   *lppPacket = m_lpPacket;
   *lpdwSize  = m_dwSize;
}


/////////////////////////////////////////////////////////////////////
// CBKDecodePacket
//
//	Description:
//    Decodes a NAVAP_EVENTPROGRESSREPORTPACKET and fills a
//    CVirusAlert object in preparation for transmission to
//    the alert service.
//
// Parameters:
//    LPVOID pvInBuffer - Data for the operation.
//
//    DWORD  dwInBufferSize - Buffer Size.
//
//    CVirusAlert & NotifyPacket - Reference to a notification packet object.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 04/18/96 PVENKAT
// 05/16/96 DSACKIN Extracted from CallBack
// 10/31/96 TSMITH  Renamed from CBKAlert to CBKDecodePacket and
//                  heavily modified to perform the limited function-
//                  ality necessary to decode a
//                  NAVAP_EVENTPROGRESSREPORTPACKET and fill a
//                  CAlertNotification object.
// 01/21/97 TSMITH  Modified usage of CAlertNotification to set packet
//                  source flags and insert domain name.
// 05-16-97 - DSACKIN - Changed from CAlertNotification to CVirusAlert
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::CBKDecodePacket(
   LPVOID        pvInBuffer,            // [in] - buffer sent from driver
   DWORD         dwInBufferSize,        // [in] - size of the buffer
   CVirusAlert & cVirusAlert)           // [out] - Packet object reference
{
PNAVAP_EVENTPROGRESSREPORTPACKET lpAlert;
TCHAR             szComputer[ MAX_COMPUTERNAME_LENGTH + 1 ];
TCHAR             szUserName[ SYM_MAX_PATH ];
TCHAR             szDomainName[ SYM_MAX_PATH ];
DWORD             dwComputerLen = sizeof( szComputer ) / sizeof( szComputer[ 0 ] );
DWORD             dwUserLength = sizeof( szUserName ) / sizeof( TCHAR );
DWORD             dwDomainLength = sizeof( szDomainName ) / sizeof( TCHAR );
DWORD             dwPacketSource = PACKET_SOURCE_LOCAL | PACKET_SOURCE_NAVAP;
DWORD             dwDomainSize = sizeof( szDomainName );

   cVirusAlert.SetPacketSource(
      dwPacketSource);

                                        // Set the current machine type into
                                        // the packet.
   cVirusAlert.CreatePacketMachineType();

   GetComputerName(
      szComputer,
      &dwComputerLen);

   lpAlert = (PNAVAP_EVENTPROGRESSREPORTPACKET) pvInBuffer;

                                        // Insert UUID into packet
   cVirusAlert.CreateUniqueID();

   cVirusAlert.SetTime (
      (SYSTEMTIME&)lpAlert->rSystemTime);

                                        // The NAVAP driver only handles file
                                        // type events right now so this the
                                        // only alert type possible here.
   cVirusAlert.SetAlertType(
      FILE_KNOWN_VIRUS);

                                        // What action was taken?
   cVirusAlert.SetAction(
      ConvertLastAPAction(lpAlert));

                                        // Did it work?
   cVirusAlert.SetActionSuccess(
      lpAlert->dwActionCompleted);


                                        // Decode the User Info Sub-Packet
   DecodeNAVAPUserInfoSubPacket(
      &(lpAlert->rUser),
      szUserName,
      &dwUserLength,
      szDomainName,
      &dwDomainLength );


                                        // Now insert our text information into
                                        // the packet.
   cVirusAlert.SetSystemName(
      szComputer);

   cVirusAlert.SetUserName(
      szUserName);

   cVirusAlert.SetObjectName(
      &lpAlert->szStringArray[lpAlert->dwFileDeviceInTroubleIndex] );

   cVirusAlert.SetVirusName(
      &lpAlert->szStringArray[lpAlert->dwVirusNameIndex] );


                                        // Get domain name that local system is
                                        // a member of.
   if ( NavGetDomainName(
           szComputer,
           dwDomainSize,
           szDomainName ) )
   {
      cVirusAlert.SetDomainName(
         szDomainName);
   }
   else
   {
        ErrorHandler( IDS_ERR_GETDOMAINNAME_FAILED );
   }
}


/////////////////////////////////////////////////////////////////////
// CBKMount
//
//	Description:
//    This function does the scan of a mounted drive.
//
// Parameters:
//    LPVOID  - pvInBuffer     - Data for the operation
//    ULONG   - uInBufferSize  - Buffer Size
//
// Returns:
//    VOID
/////////////////////////////////////////////////////////////////////
// 05/17/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::CBKMount (
   LPVOID   pvInBuffer,                 // [in] - buffer sent from driver
   UINT     uInBufferSize)              // [in] - size of the buffer
{
PNAVAP_SCANMOUNTEDDEVICEPACKET  pMount;
UINT   uDrive;
BOOL   bAttemptRepair = FALSE;
TCHAR  szDrive[2] = _T(" ");

                                        // Convert packet to our struct.
   pMount = (PNAVAP_SCANMOUNTEDDEVICEPACKET) pvInBuffer;

   uDrive = (UINT) pMount->szDevice[0]; // Get the drive letter

   LogMessage(TEXT("Mounting device %c:"), uDrive);

                                        // Check for the auto repair option!
      switch ( m_NavOptions.tsr.uActionKnown == TSR_CUSTOM_RESPONSE ?
                  m_NavOptions.customAP.uchBoot :
                  m_NavOptions.tsr.uActionKnown )
      {
         case TSR_REPAIR:
         case TSR_DELETE:
         case TSR_QUAR_CANT_REP:

            bAttemptRepair = TRUE;
      }

   if ( HWIsNEC() )
   {
      // If we're on a NEC, scan the newly-mounted drive only if it's a
      // floppy drive.

      szDrive[0] = uDrive;
      STRUPR ( szDrive );

      if ( m_fdwFloppiesOnSystem & ( 1 << (szDrive[0] - 'A') ) )
         {
         LogMessage( _T("Scanning mounted floppy on NEC -- drive %c:"),
                     szDrive[0] );

         ScanBoot (uDrive, pMount, bAttemptRepair);   // Scan the drive!
         }
      else
         {
         LogMessage ( _T("Not scanning mounted drive on NEC -- drive %c:"),
                      szDrive[0] );
         }
   }    // end if ( HWIsNEC )
   else
   {
      ScanBoot (uDrive, pMount, bAttemptRepair);   // Scan the drive!
   }
}

/////////////////////////////////////////////////////////////////////
//
// BOOL NavwRunning()
//
// Purpose:
//    Tests to see if NAVWNT.exe is running.  Added for prompting.
//
// Parameters:
//    None.
//
// Returns:
//    TRUE if NAVWNT.exe is running, FALSE if not.
//
/////////////////////////////////////////////////////////////////////
// 11/17/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////

BOOL NavwRunning()
{
    if ( NULL != ::FindWindow ( NAV_MAIN_WND_CLASS, NULL )  ||
         NULL != ::FindWindow ( NAV_SCANWINDOW_CLASS, NULL )   )
        {
        return TRUE;
        }
    else
        {
        return FALSE;
        }
}

/////////////////////////////////////////////////////////////////////
// VOID CNavAPService::LaunchNavw
//
// Purpose:
//    This takes events out of the Navw queue and sends them off to
//    Navw so it can do prompting.
//
// Parameters:
//
// Returns: TRUE for success
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 11/17/97 Mike Dunn -- function created.  Mostly Vance G.'s code, with
//                       my fixes for double-byte string handling.
// 11/18/97 Mike Dunn -- added a critical section to try and avoid
//                       multiple instance problems.
/////////////////////////////////////////////////////////////////////
BOOL CNavAPService::LaunchNavw()
{
CString             csItem, csBuffer;
CString             csCommand, csFile;
LPVOID              pMMF;
LPVOID              pNow;
SECURITY_ATTRIBUTES sa;
DWORD               dwPassedVal;
CPacketItem*        lpcPacketItem;
CObList             oblTemp;
CVirusAlert         cva;
char*               pMBFilenameList;    // multibyte string
int                 nMBBufferSize;      // calculated size of the string
STARTUPINFO         rSI;
PROCESS_INFORMATION rPI;
BOOL                bLaunched;
static HANDLE       hMMF = NULL;
static const TCHAR  s_szShareDataName[] = _T(WNDD_SHAREMEM_DATA_NAME);
static const TCHAR  s_szCommand[]       = _T(NAVNT_COMMAND);

    ZeroMemory ( &rSI, sizeof ( rSI ) );
    ZeroMemory ( &rPI, sizeof ( rPI ) );

    rSI.cb = sizeof ( rSI );


    if(hMMF != NULL)
        CloseHandle(hMMF);

    // Find an unused memory mapped file name
    dwPassedVal = WNDD_START_VALUE;

    do
        {
        ++dwPassedVal;
        csFile.Format(s_szShareDataName, dwPassedVal);

        hMMF = OpenFileMapping(FILE_MAP_READ, FALSE, csFile);

        if(hMMF != NULL)
            CloseHandle(hMMF);
        } while(hMMF != NULL);


    // Create the data for the file.  We'll keep any filenames pulled from
    // the prompt list in a temporary list, so we can restore the prompt list
    // if necessary (such as if we can't create the memory-mapped file).
    // csBuffer holds the filenames that we retrieve from the prompt list.

    do
        {
        LPVOID lpData;
        DWORD  dwSize;
        TCHAR  szName [MAX_PATH];

        lpcPacketItem = (CPacketItem *) m_oblPromptList.RemoveTail();

        oblTemp.AddHead(lpcPacketItem);

        lpcPacketItem->GetPacket(&lpData, &dwSize);

        CBKDecodePacket(lpData, dwSize, cva);

        if(cva.GetObjectName(MAX_PATH, szName))
            {
                                        // Add on the next filename, quoted in
                                        // case it's an LFN with spaces.
            csBuffer += _T("\"") + CString(szName) + _T("\" ");
            }
        } while(!m_oblPromptList.IsEmpty());


    // Create the memory mapped file

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

                                        // ***************************************
                                        // NOTE: I'm passing the max length as
                                        // sizeof(TCHAR) * buffer length since the
                                        // whole string might be double-byte
                                        // characters.  The  MFC docs for
                                        // CString::GetLength() say that it
                                        // returns the number of bytes
                                        // in the string, but that's wrong!  In
                                        // Unicode, it returns the number of
                                        // characters (and each character is 2
                                        // bytes long, thus the multplication by
                                        // sizeof(TCHAR), which is 2).
                                        // ***************************************

    hMMF = CreateFileMapping ( (HANDLE)0xFFFFFFFF, &sa,
                               PAGE_READWRITE | SEC_COMMIT, 0,
                               sizeof(TCHAR) * (csBuffer.GetLength() + 1) + sizeof(DWORD),
                               csFile);

    if(hMMF == NULL)
        goto LaunchExit;

                                        // We need to convert the filename
                                        // list from Unicode to ANSI so that
                                        // NAVW can read it.

                                        // First calculate the buffer size
                                        // needed to hold the MB string.

    nMBBufferSize = WideCharToMultiByte ( CP_ACP,
                                          WC_COMPOSITECHECK,
                                          (LPCWSTR) csBuffer, -1,
                                          NULL, 0,
                                          NULL, NULL );

    pMBFilenameList = new char [ nMBBufferSize ];

    if ( pMBFilenameList == NULL )
        {
        LogMessage ( _T("Memory allocation error when converting filename list to a multibyte string.") );
        goto LaunchExit;
        }

                                        // Convert the filename list!
    WideCharToMultiByte ( CP_ACP, WC_COMPOSITECHECK,
                          (LPCWSTR) csBuffer, -1,
                          pMBFilenameList, nMBBufferSize,
                          NULL, NULL );


    pMMF = MapViewOfFile(hMMF, FILE_MAP_WRITE, 0, 0, 0);

    if(pMMF == NULL)
        {
        CloseHandle(hMMF);
        hMMF = NULL;
        delete [] pMBFilenameList;
        goto LaunchExit;
        }

                                        // Copy the filename list to the
                                        // shared memory.

    memcpy ( pMMF, &nMBBufferSize, sizeof(int) );

    pNow = (LPBYTE) pMMF + sizeof(DWORD);

    memcpy ( pNow, (LPVOID) pMBFilenameList, nMBBufferSize );

    delete [] pMBFilenameList;
    pMBFilenameList = NULL;

    UnmapViewOfFile(pMMF);


//  Time to launch NAVW.

    csCommand.Format(s_szCommand, dwPassedVal);

//  If the launch fails, try next time

    EnterCriticalSection ( &m_PromptingCritical );

                                        // Check if NAV is running here since
                                        // any threads that were blocked at
                                        // the EnterCriticalSection() should
                                        // bail if the thread that was just in
                                        // the critical section launched NAVW
                                        // successfully.

    if ( NavwRunning() )
        {
        goto LaunchExit;
        }

    bLaunched = CreateProcess ( NULL, csCommand.GetBuffer(nMBBufferSize),
                                NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS,
                                NULL, NULL, &rSI, &rPI );

    csCommand.ReleaseBuffer();

                                        // Sit here until NAVW has finished
                                        // initializing.
    if ( bLaunched )
        {
                                        // Close unneeded handles.
        CloseHandle ( rPI.hProcess );
        CloseHandle ( rPI.hThread );

                                        // This is the important part! Since
                                        // NavwRunning() will return FALSE
                                        // if the NAVW process is running but
                                        // NAV is still initializing itself,
                                        // we wait here 'till NavwRunning()
                                        // finally returns TRUE.  This also
                                        // keeps other threads from entering
                                        // the critical section and launching
                                        // more instances of NAVW.
        while ( !NavwRunning() )
            {
            Sleep(0);                   // Give up the rest of our time slice
            }
        }
    else
        {
        goto LaunchExit;
        }

                                        // Clean up our temporary list.
    while(!oblTemp.IsEmpty())
        {
        lpcPacketItem = (CPacketItem *) oblTemp.RemoveHead();
        delete lpcPacketItem;
        }

    LeaveCriticalSection ( &m_PromptingCritical );

    return TRUE;

LaunchExit:

                                        // We get here if an error occured during
                                        // processing here and we couldn't
                                        // launch NAVW.  In this case, put back
                                        // any filenames we pulled from the
                                        // prompt queue.
    while(!oblTemp.IsEmpty())
        {
        lpcPacketItem = (CPacketItem *) oblTemp.RemoveHead();
        m_oblPromptList.AddTail(lpcPacketItem);
        }

    LeaveCriticalSection ( &m_PromptingCritical );

    return FALSE;
}

/////////////////////////////////////////////////////////////////////
// VOID CNavAPService::NotificationQueue
//
// Purpose:
//    This takes events out of our queue and sends them off to the
//    AlertService.
//
// Parameters:
//
// Returns:
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/24/96 - DSACKIN - Function Created.
// 12/28/96 - TSMITH  - Moved deletion of 'lpcPacketItem' to end of
//                      inner while loop. This fixed 'lpPacket' from
//                      being invalidated too early which caused
//                      'lpcPacketItem' data to be overwritten by
//                      'AlertOptions.LoadOptions'. Corrupt data was
//                      then inserted into 'NotifyPacket' which caused
//                      an Access Violation exception when sent to the
//                      Alert Service.
// 07-02-97 - DSACKIN - We now send all packets to the alert service
//                    - for logging even though the AP Alerts are off.
//
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::NotificationQueue (VOID)
{
CVirusAlert     cVirusAlert;
CPacketItem*    lpcPacketItem;
LPVOID          lpPacket;
PBYTE           pBuffer;
DWORD           dwSize;
DWORD           Status;
BOOL            bTransmitted;
IQuarantineDLL  *pIQuarantine;

   while ( !m_bExitingService )
   {
                                        // We must wait until we get a
                                        // notification from the AlertService
                                        // that it is ready to handle our
                                        // requests before we start processing
                                        // them.

      while (  m_bAlertSvcReady &&
              !m_oblPacketList.IsEmpty( ) &&
              !m_bExitingService )
      {
         LogMessage(_T("Processing the next Packet Forward"));

                                        // Get next packet from the queue
         lpcPacketItem = (CPacketItem *) m_oblPacketList.RemoveTail();

         if (lpcPacketItem)
         {
                                        // Set this to TRUE once the packet has
                                        // been transmitted.  If the packet
                                        // isn't transmitted, we will assume
                                        // that it has been added back into our
                                        // queue.
            bTransmitted = FALSE;

                                        // Get the packet
            lpcPacketItem->GetPacket(
                              &lpPacket,// Packet we are getting
                              &dwSize); // Size of the packet we received.


                                        // If we aren't enabled, we are going
                                        // to want to just ignore the packets.
                                        // This state will be checked every
                                        // time we get a packet.
            if (NULL != lpPacket)
            {
                                        // Fill up a notifiation packet
                                        // with info from NAVAP
               CBKDecodePacket(
                  lpPacket,
                  dwSize,
                  cVirusAlert );

                                        // Get access to raw bytes
               pBuffer = cVirusAlert.GetPacket();

                                        // It is important that we are only
                                        // allowed to send ONE RPC packet at a
                                        // time because all transmissions are
                                        // using the same UUID
               EnterCriticalSection(&m_RpcCritical);

                                        // Use dynamic endpoint resolution
               Status = NavRpcSendPacket(
                           ALERTSVC_PROTOCOL,
                           ALERTSVC_ADDRESS,
                           NULL,
                           (LPVOID) pBuffer );

                                        // Allow the next callback to attempt
                                        // sending his.
               LeaveCriticalSection(&m_RpcCritical);

                                        // Don't forget to unlock the buffer!
               cVirusAlert.ReleasePacket();

               if ( Status == ERROR_SUCCESS )
               {
                  bTransmitted = TRUE;
               }
               else
               {
                  ErrorHandler(
                     ID_ERR_ALERT_SERVICE_RPC_XMT_FAILED );

                                        // Now that we have failed to transmit
                                        // the packet, lets set ourselves up so
                                        // that when the Alert Service comes
                                        // back online that we will continue to
                                        // transmit all of the packets.


                                        // First, lets start by putting the
                                        // item we couldn't transmit back into
                                        // the queue.
                  m_oblPacketList.AddTail(
                     lpcPacketItem);

                                        // Now set our flag so that we will
                                        // know when to start processing again.
                  m_bAlertSvcReady = FALSE;
               }
            }

                                        // Clean up our memory.
                                        // 'lpPacket' now becomes invalid if
                                        // it has been transmitted.
            if (bTransmitted)
               delete lpcPacketItem;

         } // while ( !m_oblPacketList.IsEmpty( ) )

      } // if (m_bAlertSvcReady)

                                        // Process the INCOMING items in Quarantine.

      if ( m_bIncomingQuarantineItems )
      {
         CoInitialize( NULL );

         if ( SUCCEEDED( CoCreateInstance( CLSID_QuarantineDLL,
                                           NULL,
                                           CLSCTX_INPROC_SERVER,
                                           IID_QuarantineDLL,
                                           ( LPVOID * ) &pIQuarantine ) ) )
         {
            if ( SUCCEEDED( pIQuarantine->Initialize() ) )
            {
               m_bIncomingQuarantineItems = FALSE;

               pIQuarantine->ProcessAllIncomingFiles();
            }

            pIQuarantine->Release();
         }

         CoUninitialize();
      }

                                        // Code added to process prompting

      if ( m_NavOptions.tsr.uActionKnown == TSR_PROMPT &&
           !m_oblPromptList.IsEmpty() &&
           !m_bExitingService &&
           !NavwRunning())
      {
         LaunchNavw();
      }

                                        // Sleep while we wait for the next
                                        // packet to come in.
      Sleep( 500 );
   } // while (!m_bExitingService)


}


/////////////////////////////////////////////////////////////////////
// NotificationQueueProc
//
// Purpose:
//    This is a callthrough to our NotificationQueue member function
//
// Parameters:
//    LPVOID lpObject - The This pointer to get us back into our class.
//
// Returns:
//    Doesn't return.  Terminates with _endthread.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/24/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID NotificationQueueProc(
   LPVOID lpObject)                     // [in] - This pointer from caller
{
CNavAPService *pThis;

   pThis = (CNavAPService *) lpObject;

   pThis->NotificationQueue();

   _endthread();                        // Don't just return!!!
}

/////////////////////////////////////////////////////////////////////
// Callback
//
//	Description:
//      NAV AP Driver calls this function whenever it finds an virus,
//      or a volume mount event occurs on the system.
//
//      This function will either fill and send a CAlertNotification
//      to the alert service or will do a boot sector scan of a
//      a volume as reported by the NAVAP driver.
//
//  Parameters:
//		ulIoControlCode	ULONG	- Log/Alert operation
//		pvInBuffer	VOID *	- Data for the operation
//		uInBufferSize	ULONG	- Buffer Size
//
//  Returns:
//      VOID
/////////////////////////////////////////////////////////////////////
// 04/18/96 PVENKAT
// 11/01/96 TSMITH  Modified to fill and send a NAV_NOTIFICATION_PACKET
//                  rather than perform the logging and alerting
//                  directly.
/////////////////////////////////////////////////////////////////////
VOID CNavAPService::Callback (
    ULONG  ulIoControlCode,             // [in] - Control code from Device
    LPVOID pvInBuffer,                  // [in] - Input buffer
    UINT   uInBufferSize                // [in] - Bytes in the Input buffer.
    )
    {
    CPacketItem                     *lpPacketItem;

    switch ( ulIoControlCode )
        {
        case NAVAP_COMM_EVENTPROGRESSREPORT:
            {
                                        // If the AP driver just moved a file to
                                        // Quarantine's INCOMING folder, set a flag
                                        // so the notification queue can complete the
                                        // move into the legit Quarantine area.

            if ( ((NAVAP_EVENTPROGRESSREPORTPACKET *)pvInBuffer)->dwActionCompleted )
                {
                                        // No synchronization on m_bIncomingQuarantineItems.
                                        // Only set TRUE here after AP driver sends items to
                                        // Quarantine's INCOMING folder.
                                        // Set FALSE in notification queue prior to actual
                                        // call to ProcessAllIncomingFiles().
                m_bIncomingQuarantineItems = TRUE;
                }

                                        // Save event progress info for notification queue.

            lpPacketItem = new CPacketItem (
                                  pvInBuffer,
                                  uInBufferSize);

            if (NULL == lpPacketItem)
                {
                LogMessage(_T("Memory allocation error in creating a new CPacketItem"));
                                        // This is a memory allocation error
                }
            else
                {
                                        // Add it to the list so that our
                                        // processing function can handle it.
                m_oblPacketList.AddHead(lpPacketItem);

                // Code added to do Prompting

                if ( m_NavOptions.tsr.uActionKnown == TSR_PROMPT )
                    {
                    CPacketItem *lpPromptPacket;

                    lpPromptPacket = new CPacketItem ( pvInBuffer,
                                                       uInBufferSize );

                    if (lpPromptPacket)
                        {
                        m_oblPromptList.AddHead(lpPromptPacket);
                        }
                    }
                }   // end else
            }   // end case NAVAP_COMM_EVENTPROGRESSREPORT

            break;

        case NAVAP_COMM_SCANMOUNTEDDEVICE:
            CBKMount( pvInBuffer, uInBufferSize);
            break;

        default:
            LogMessage(TEXT("*** Unknown callback ***"));
            break;
        }
}

//***************************************************************************/
//  ConvertLastAPAction
//
//	Description:
//      Converts the action value used specifically by NAVAP to a general
//      value used by CAlertNotification.
//
//  Parameters:
//      PNAVAP_EVENTPROGRESSREPORTPACKET lpLog - Pointer to data packet from
//                                               NAVAP driver.
//
//  Returns:
//      DWORD - Generic action value for alert notifications.
//***************************************************************************/
// 04/25/96 PVENKAT
// 11/02/96 TSMITH  Modified heavily to remove any logic for selecting IDs
//                  for string resources, action success status, etc.
//***************************************************************************/
DWORD CNavAPService::ConvertLastAPAction( PNAVAP_EVENTPROGRESSREPORTPACKET lpLog )
    {
    DWORD dwAction;

    // Each case reported by Driver needs to be processed separately.
    switch ( lpLog->dwAction )
        {
        case AVACTION_OK:
            dwAction = ACTION_NONE;
            break;

        case AVACTION_STOP:
            dwAction = ACTION_DENY_ACCESS;
            break;

        case AVACTION_CONTINUE:
            dwAction = ACTION_NONE;
            break;

        case AVACTION_REPAIR:
            dwAction = ACTION_REPAIR;
            break;

        case AVACTION_DELETE:
            dwAction = ACTION_DELETE;
            break;

        case AVACTION_EXCLUDE:
            dwAction = ACTION_EXCLUDE;
            break;

        case AVACTION_SHUTDOWN:
            dwAction = ACTION_SHUTDOWN;
            break;

        case AVACTION_MOVE:
            dwAction = ACTION_MOVE;
            break;

        case AVACTION_RENAME:
            dwAction = ACTION_RENAME;
            break;

        case AVACTION_QUARANTINE:
            dwAction = ACTION_QUARANTINE;
            break;

        default:
            dwAction = ACTION_UNDEFINED;
            break;
       }

    return( dwAction );
    }



