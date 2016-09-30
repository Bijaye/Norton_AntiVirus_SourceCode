/////////////////////////////////////////////////////////////////////
// N_ALERT.CPP - Created - 12/04/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVWORK/VCS/n_alert.cpv   1.6   27 Apr 1998 17:48:20   TCASHIN  $
//
// Description:
//    This file will hold all of the advaced alerting features
//    that are going to be in nav starting with the Marconi project.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVWORK/VCS/n_alert.cpv  $
// 
//    Rev 1.6   27 Apr 1998 17:48:20   TCASHIN
// Handle quarantine alert actions.
// 
//    Rev 1.5   21 Aug 1997 02:26:12   DSACKIN
// Added code to stamp the current machine type into the CAlert packet.
//
//    Rev 1.4   15 Aug 1997 17:35:04   DSACKIN
// Moved the NetAlert function call so that we can actually tell if any
// automatic (or from prompt) actions were taken instead of "No Action Taken"
//
//    Rev 1.3   16 Jul 1997 15:05:42   DBUCHES
// Added support for getting a workstation's domain name for Win95 platforms
//
//    Rev 1.2   30 Jun 1997 19:21:32   DSACKIN
// Added NT forward alerting.
//
//    Rev 1.1   18 May 1997 05:50:44   DSACKIN
// Switched from CAlertNotification to CVirusAlert
//
//    Rev 1.0   24 Apr 1997 11:28:50   JBRENNA
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:15:16   RFULLER
// Initial revision
//
//    Rev 1.8   14 Feb 1997 17:48:20   TSmith
// Replaced use of CAlertOptions with GetPrivateProfileInt.
//
//    Rev 1.7   13 Feb 1997 21:17:40   TSmith
// Uncommented checks on sending alerts from NAVW.
//
//    Rev 1.6   06 Feb 1997 21:33:32   TSmith
// Modified calls to 'NavGetDomainName' to account for that functions
// new argument types. Included header file 'UNIUTIL.H' where that function
// now resides.
//
//    Rev 1.5   06 Feb 1997 03:36:28   KSACKIN
// Fixed up some incorrect message processing when sending messages to the
// AlertSvc for the advanced alerting options.   We were reporting that
// everything was an infected file, and not checking to see if it is a boot
// record or master boot record.
//
//    Rev 1.4   24 Jan 1997 18:48:50   TSmith
// Fixed #79150: Alerts & log entries now contain the name of the infected
// compressed file (along with the parent zip file name) rather than the
// temporary disk file that was actually scanned by the virus engine.
// Also put a check to blow off any alert requests that are not for know
// virus detections.
//
//
//    Rev 1.3   23 Jan 1997 11:48:54   TSmith
// Added code to insert virus name and domain name in alert packet.
// Reference defect #78159.
//
//    Rev 1.2   26 Dec 1996 14:14:10   TSmith
// Modified calls to the CAlertNotification object's string insertion functions
// to use TCHARs instead of WCHARs & removed MBCS to Wide string conversion
// functions (The CAlertNotification class now properly accepts either Ansi or
// Unicode strings).
//
//    Rev 1.1   20 Dec 1996 14:08:28   DSACKIN
// Wrap our RPC call in a critical section.
//
//    Rev 1.0   05 Dec 1996 03:38:34   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////
#include <platform.h>
#include "security.h"

#include "nonav.h"
#include "servenfy.h"
#include "nunavstb.h"
#include "nunavdlg.h"
#include "CAlert.h"
#include "CVAlert.h"
#include "NetAlert.h"
#include "uniutil.h"
#include "xapi.h"

                                        // For the alerting options file
                                        // handling
#include "navrpc.h"

// Here we need a static (Global) critical section to insure that we
// aren't going to send more than one packet to the alert service at
// any given time.

CRITICAL_SECTION rRpcCritical;
CVirusAlert gcVirusAlert;
extern HINSTANCE hInstance;
BOOL gbCancelSelected;

DWORD AlertThreadFunc();
static DWORD WINAPI AlertThreadLinkFunc(LPVOID pParam);
BOOL WINAPI CancelDlgProc(HWND hDlg, UINT    uMessage, WPARAM  wParam, LPARAM  lParam );

/////////////////////////////////////////////////////////////////////
// Local function prototypes

#ifdef SYM_WIN32
BOOL LOCAL Win95GetDomainName( LPTSTR szDomainName, DWORD cbDomainSize );
#endif

/////////////////////////////////////////////////////////////////////
// Defines
                                        // Makes it easier to determine
                                        // appropriate code paths:
#if defined (WIN32)
    #define IS_WIN32 TRUE
#else
    #define IS_WIN32 FALSE
#endif

#define IS_NT()      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)


#ifdef SYM_WIN32


/////////////////////////////////////////////////////////////////////
// VOID NetAlertGetAlertActions(
//
// Purpose:
//   This function will examine the infection information and
//   decide what action was taken and if it was successful or
//   not.
//
// Parameters:
//   LPINFECTEDFILE lpInfected,
//   CVirusAlert   *lpcVirusAlert)
//
// Returns:
//   none.
//
// Notes:
//   All information returned will be in the CVirusAlert that
//   is passed into this function.
//
/////////////////////////////////////////////////////////////////////
// 08/15/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID NetAlertGetAlertActions(
   LPINFECTEDFILE lpInfected,
   CVirusAlert   *lpcVirusAlert)
{
DWORD dwActionTaken;
                                        // Assume that the operation failed.
BOOL  bActionSuccess = FALSE;


                                        // This code will group the actions
                                        // based on what action was taken, and
                                        // then if that action was successful,
                                        // they are in the top half and a
                                        // successful status is set.  The
                                        // switch statement lets items of the
                                        // same action with success, fall
                                        // through and get the action type
                                        // along with the failures.
   switch (lpInfected->wStatus)
   {
                                        // First, handle all successful repair
                                        // actions.
      case FILESTATUS_REPAIRED:
      case FILESTATUS_BOOTREPAIRED:
      case FILESTATUS_MASTERBOOTREPAIRED:
      case FILESTATUS_INTEG_MBREPAIRED:
      case FILESTATUS_INTEG_BOOTREPAIRED:
      case FILESTATUS_INTEG_SYSFILEREPAIRED:
         bActionSuccess = TRUE;

                                        // Now for the unsuccessful repairs
      case FILESTATUS_NOTREPAIRED:
      case FILESTATUS_BOOTNOTREPAIRED:
      case FILESTATUS_MASTERBOOTNOTREPAIRED:
      case FILESTATUS_INTEG_MBREPAIRFAILED:
      case FILESTATUS_INTEG_BOOTREPAIRFAILED:
      case FILESTATUS_INTEG_SYSFILEREPAIRFAILED:
         dwActionTaken = ACTION_REPAIR;
         break;

                                        // Now handle delete success
      case FILESTATUS_DELETED:
      case FILESTATUS_INOCULATEDELETED:
      case FILESTATUS_INOC_CHANGED_DELETED:
         bActionSuccess = TRUE;

                                        // Now handle delete failures
      case FILESTATUS_NOTDELETED:
      case FILESTATUS_INOCULATEDELETE_FAILED:
         dwActionTaken = ACTION_DELETE;
         break;


                                        // Now handle excluded success
      case FILESTATUS_EXCLUDE_INOCULATION:
      case FILESTATUS_EXCLUDE_VIRUSDETECTION:
         bActionSuccess = TRUE;

                                        // Now handle excluded failures
      case FILESTATUS_EXCLUDE_FAILED:
         dwActionTaken = ACTION_EXCLUDE;
         break;


                                        // Now handle quarantine success
      case FILESTATUS_QUARANTINE_INFECTED:
         bActionSuccess = TRUE;

                                        // Now handle quarantine failures
      case FILESTATUS_QUARANTINE_INFECTED_FAILED:
         dwActionTaken = ACTION_QUARANTINE;
         break;


      default:
         dwActionTaken = ACTION_NONE;
         break;
   }

                                        // Now that we have an action and a
                                        // success, we should update the alert
                                        // information
   lpcVirusAlert->SetAction(
      dwActionTaken);

   lpcVirusAlert->SetActionSuccess(
      bActionSuccess);
}

/////////////////////////////////////////////////////////////////////
// VOID WINAPI NetAlert (LPNETALERT lpAlert)
//
// Purpose:
//    This function is where we are going to hook into the advanced
//    alerting options that are becoming present in the Marconi project.
//
// Parameters:
//    LPNETALERT lpAlert - [in] - information about the alert
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12/04/96 - DSACKIN - Function taken from stub.cpp
/////////////////////////////////////////////////////////////////////
VOID WINAPI NetAlert( LPNETALERT lpAlert, LPZIPDETECT lpZipDetect )
{

LPINFECTEDFILE lpInfected = (LPINFECTEDFILE)lpAlert->lpInfected;
LPALERTOPT     lpAlertOpt = (LPALERTOPT) lpAlert->lpAlertOpt;
extern         HINSTANCE  hInstance;
TCHAR          szComputerName[ MAX_COMPUTERNAME_LENGTH + 1 ] = { 0 };
TCHAR          szDomainName[ MAX_COMPUTERNAME_LENGTH + 1 ] = { 0 };
TCHAR          szUserName[SYM_MAX_PATH + 1] = { 0 };
TCHAR          szObject[SYM_MAX_PATH + 1] = { 0 };
TCHAR          szVirus[SYM_MAX_PATH + 1] = { 0 };
TCHAR          szFmtStr[SYM_MAX_PATH + 1] = { 0 };
TCHAR          szTempStr[SYM_MAX_PATH+1] = { 0 };
TCHAR          szForwardStr[SYM_MAX_PATH+1] = { 0 };
TCHAR          szNLMStr[SYM_MAX_PATH+1] = { 0 };
DWORD          dwUserLen = sizeof (szUserName) / sizeof (szUserName[0]);
DWORD          dwComputerLen = sizeof (szComputerName) / sizeof (szComputerName[0]);
DWORD          dwDomainLength = sizeof(szDomainName) / sizeof( TCHAR );
DWORD          dwPacketSource = PACKET_SOURCE_LOCAL | PACKET_SOURCE_NAVW;
BOOL           bScannerRemoteAlertsEnabled;
UINT           cbDomainSize = sizeof( szDomainName );
DWORD          lpAlertExitCode;
DWORD          dwAlertThreadID;
BOOL           bDone = FALSE;
HANDLE         hAlertThread;
HWND           hwndCancelDlg;
MSG            msg;
BOOL           bRet;

                                        // Only handle known virus detections
   if ( lpAlert->uTypeOfEvent != EV_ALERT_KNOWN_VIRUS )
       return;

    if ( 0 != GetModuleFileName( hInstance, szTempStr, SYM_MAX_PATH ) )
    {
    NameStripFile( szTempStr );

                            // Tack on the default file name to the
                            // path of this module.
    STRCAT( szTempStr, "\\NAVALERT.DAT" );
    }

   bScannerRemoteAlertsEnabled =
    GetPrivateProfileInt( "General Options",
                          "ScannerRemoteAlertsEnabled",
                          1,
                          szTempStr );

   if ( !bScannerRemoteAlertsEnabled )
       return;

                                        // Tell alert service who sent it
   gcVirusAlert.SetPacketSource(
      dwPacketSource);

                                        // Stamp the packet with our current
                                        // machine type.
   gcVirusAlert.CreatePacketMachineType();

                                        // Start by getting all of the
                                        // information that we are going to
                                        // need for our packet
   GetUserName (szUserName, &dwUserLen);
   GetComputerName( szComputerName, &dwComputerLen );

                                        // Now we need to set up the string
                                        // that says what was infected.
                                        // If we're scanning a zip file, we
                                        // need to use the name of the archive
                                        // rather than the temp file that was
                                        // actually scanned.
   if ( ( lpZipDetect != NULL ) && ( lpZipDetect->bInZip == TRUE ) )
   {
       LoadString( hInstance,
                   IDS_PRINTZIPFILE,
                   szFmtStr,
                   sizeof( szFmtStr ) );

       SPRINTF( szObject,
                szFmtStr,
                lpZipDetect->szZipChildFile,
                lpZipDetect->lpZipFileName );
                                        // Currently we are only handling Known
                                        // virus detections.
       gcVirusAlert.SetAlertType( FILE_KNOWN_VIRUS );
   }
   else if ( lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED )
   {
                                        // Item is Master Boot Record.
                                        // Format our string accordingly.
      *szObject = lpInfected->szFullPath[0] + _T('0');
      *(CharNext(szObject)) = EOS;


                                        // Currently we are only handling Known
                                        // virus detections.
      gcVirusAlert.SetAlertType( MBR_KNOWN_VIRUS );
   }
   else if ( lpInfected->wStatus == FILESTATUS_BOOTINFECTED )
   {
                                        // Item is a boot record.
      STRCPY( szObject, lpInfected->szFullPath );
                                        // Currently we are only handling Known
                                        // virus detections.
      gcVirusAlert.SetAlertType( BOOT_RECORD_KNOWN_VIRUS );
   }
   else if (lpInfected->szFullPath[0] == EOS)
   {
                                        // No string?  Must be memory.
      LoadString (hInstance,
               IDS_MEMORY,
               szObject,
               sizeof (szObject));

                                        // This is memory virus, but setting to
                                        // file unil we get an ID for memory.
      gcVirusAlert.SetAlertType( FILE_KNOWN_VIRUS );
   }
   else
   {
                                        // OK.  Only thing left is the file
                                        // name.
      STRCPY (szObject, lpInfected->szFullPath);

                                        // Currently we are only handling Known
                                        // virus detections.
      gcVirusAlert.SetAlertType( FILE_KNOWN_VIRUS );
   }

                                        // Creating a Unique ID and the Time
                                        // stamp were moved to CAlert.
   gcVirusAlert.CreateUniqueID();
   gcVirusAlert.StampTime();

                                        // Fill out the information about what
                                        // happened.
   NetAlertGetAlertActions(
      lpInfected,
      &gcVirusAlert);

                                        // Set up the computer and user names
   gcVirusAlert.SetSystemName(
      szComputerName);
   gcVirusAlert.SetUserName(
      szUserName);
   gcVirusAlert.SetObjectName(
      szObject);

                                        // Only thing left is the name of the
                                        // virus that infected the system.
   STRCPY( szVirus, lpInfected->Notes.lpVirName );
   gcVirusAlert.SetVirusName( szVirus );

#ifdef NAVWNT

                                        // This function call is only valid for
                                        // the NAVWNT platform because it uses
                                        // netapi32.dll which is a Windows NT
                                        // only DLL.

    if ( NavGetDomainName( NULL, cbDomainSize, szDomainName ) )
#else
                                        // Call this routine for Win95 systems
                                        // This routine gets the domain name
                                        // from the registry. -- I don't know
                                        // how kosher this is, but it's better
                                        // than thunking to the 16-bit
                                        // NETAPI.DLL!
    if ( Win95GetDomainName( szDomainName, cbDomainSize ) )
#endif // #ifdef NAVWNT

    gcVirusAlert.SetDomainName( szDomainName );

    hAlertThread = CreateThread(NULL, 0, AlertThreadLinkFunc, NULL, 0, &dwAlertThreadID);

    gbCancelSelected = FALSE;

    if (NULL != hAlertThread)
    {
        bRet = GetPrivateProfileSection( "Forward", szForwardStr, SYM_MAX_PATH, szTempStr);
        bRet = GetPrivateProfileSection( "NLM", szNLMStr, SYM_MAX_PATH, szTempStr);

        if (szForwardStr[0] == '1' || szNLMStr[0] == '1')
        {
            hwndCancelDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CANCEL_DIALOG), NULL, CancelDlgProc);
        }

        while (!bDone)
        {
            GetExitCodeThread(hAlertThread, &lpAlertExitCode);

            if (STILL_ACTIVE != lpAlertExitCode)
                bDone = TRUE;

            if (gbCancelSelected)
                bDone = TRUE;

            // Handle dialog messages
            while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if(!IsDialogMessage(hwndCancelDlg, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }

        CloseHandle(hAlertThread);
        DestroyWindow(hwndCancelDlg);
    }
}

DWORD WINAPI AlertThreadLinkFunc(LPVOID pParam)
{
    return (AlertThreadFunc());
}

DWORD AlertThreadFunc()
{
   PBYTE          pBuffer;
   DWORD          status;

   if (IS_NT())
   {
      pBuffer = gcVirusAlert.GetPacket( );
                                        // It is important that we are only
                                        // allowed to send ONE RPC packet at a
                                        // time because all transmissions are
                                        // using the same UUID
      EnterCriticalSection(&rRpcCritical);

                                        // Use dynamic endpoint resolution
      status = NavRpcSendPacket(
                  ALERTSVC_PROTOCOL,
                  ALERTSVC_ADDRESS,
                  NULL,
                  ( LPVOID )pBuffer );
                                        // Allow the next callback to attempt
                                        // sending his.
      LeaveCriticalSection(&rRpcCritical);


                                        // Don't forget to release our packet
                                        // when we are done!
      gcVirusAlert.ReleasePacket();
   }
   else // if (IS_NT()) - This is for the 95 product.
   {
      ProcessCVirusAlert(&gcVirusAlert);
   }

   return 0;
}


BOOL WINAPI CancelDlgProc (
    HWND    hDlg,
    UINT    uMessage,
    WPARAM  wParam,
    LPARAM  lParam )
{
    BOOL        bReturn = FALSE;

    switch(uMessage)
    {
        case WM_INITDIALOG:
            bReturn = TRUE;
            break;
                                        // WM_ACTIVATE:  Set help context
                                        // for dialog so F1 works.
        case WM_ACTIVATE:
            break;

        case WM_DESTROY:
            break;

        case WM_PAINT:
            break;

        case WM_COMMAND:
            switch ( GET_WM_COMMAND_ID(wParam,lParam) )
            {
                case IDCANCEL:
                    gbCancelSelected = TRUE;
                    EndDialog(hDlg, 0);
            }

    }
    return ( bReturn );
}

/////////////////////////////////////////////////////////////////////
//
// BOOL LOCAL Win95GetDomainName( LPTSTR szDomainName, DWORD cbDomainSize )
//
// Purpose: Retrieves this workstation's domain name by peeking at
// the following registry key:
//              HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\
//                          MSNP32\NetworkProvider\AuthenticatingAgent
//
// Parameters:
//    LPSTR szDomainName - buffer to recieve domain name
//    int cbDomainSize - size of buffer
//
// Returns:
//    TRUE if successfull
//    FALSE otherwise
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 7/16/97 DBUCHES - Function created
/////////////////////////////////////////////////////////////////////
BOOL LOCAL Win95GetDomainName( LPTSTR szDomainName, DWORD cbDomainSize )
{
    HKEY hKey;
    LONG lResult;
    DWORD dwType = REG_SZ;
    BOOL bRet = FALSE;

    // Clear buffer.
    ZeroMemory( szDomainName, cbDomainSize );

    // Open MSNetwork key.
    lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                    _T("System\\CurrentControlSet\\Services\\MSNP32\\NetworkProvider"),
                    0,
                    KEY_READ,
                    &hKey );

    if( ERROR_SUCCESS == lResult )
        {
        // Read data.
        lResult = RegQueryValueEx( hKey,
                                    _T("AuthenticatingAgent"),
                                    NULL,
                                    &dwType,
                                    (LPBYTE) szDomainName,
                                    &cbDomainSize );

        if( ERROR_SUCCESS == lResult )
            bRet = TRUE;

        // Cleanup.
        RegCloseKey(hKey);
        }

    // All done.  Return results.
    return bRet;
}
#endif

