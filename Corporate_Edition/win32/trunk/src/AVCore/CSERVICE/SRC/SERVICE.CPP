/////////////////////////////////////////////////////////////////////
// Service.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/CSERVICE/VCS/service.cpv   1.0   09 May 1997 17:50:26   ksackin  $
//
// Description:
//      This is the implementation file for the CService class.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/CSERVICE/VCS/service.cpv  $
// 
//    Rev 1.0   09 May 1997 17:50:26   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 13:48:48   DSACKIN
// Ported from Tesla
// 
//    Rev 1.2   19 Mar 1997 14:49:50   DSACKIN
// Fixed a comment
//
//    Rev 1.1   19 Mar 1997 14:47:08   DSACKIN
// Added a WIN_VERSION definition.
//
//    Rev 1.0   13 Mar 1997 15:09:26   RFULLER
// Initial revision
//
//    Rev 1.1   21 Jan 1997 13:24:10   TSmith
// Added another string argument to 'LogEvent' and 'ErrorHandler' functions.
//
//    Rev 1.0   08 Nov 1996 11:46:46   DKEEFER
// Initial revision
//
//    Rev 1.25   06 Nov 1996 09:02:20   TSmith
// Modified 'LogEvent' and 'ErrorHandler' functions to accept up to 5 strings.
//
//    Rev 1.24   25 Oct 1996 09:43:22   TSmith
// Removed obsolete #ifdef DEBUG_PROFILE block delimiters.
//
//    Rev 1.23   24 Oct 1996 01:06:08   DSACKIN
// Added full UNICODE/NON-UNICODE support
//
//    Rev 1.22   05 Aug 1996 17:59:46   DSACKIN
// Removed m_pSid from the member variable list.  We really need to be passing
// this in to the ErrorHandler and LogEvent functions to be thread safe.
//
//    Rev 1.21   05 Aug 1996 15:41:28   DSACKIN
// Re-Worked LogEvent and ErrorHandler to take a fixed number of strings.
// Additional strings will be taken care of by C++ default values.
//
//    Rev 1.20   02 Aug 1996 08:44:26   PVENKAT
// Added PSID member variable to be used on NT Event Log.
//
//    Rev 1.19   25 Jul 1996 18:49:56   DSACKIN
// Set the AllowStop default behavior.
//
//    Rev 1.18   25 Jul 1996 18:48:02   DSACKIN
// Added AllowStop() function.
//
//    Rev 1.17   18 Jul 1996 20:33:18   DSACKIN
// Don't automatically assume service started correctly.
//
//    Rev 1.16   27 Jun 1996 17:07:06   DSACKIN
// Fatal errors will now STOP the service.
//
//    Rev 1.15   14 Jun 1996 14:51:26   DSACKIN
// Added DEBUG_PROFILE to ReportStatus
//
//    Rev 1.14   13 Jun 1996 17:14:42   DSACKIN
// Added code for DEBUG_PROFILE strings.
//
//    Rev 1.13   05 Jun 1996 22:18:52   DSACKIN
// Added the AllowPauseContinue function.
//
//    Rev 1.12   26 Apr 1996 17:44:36   DSACKIN
// When stopping, we should give an initial estimate of a few seconds.
//
//    Rev 1.11   24 Apr 1996 17:27:04   DSACKIN
// SetEvent before calling ServiceControlStop.  Also make it the Service's
// responsibility to report when done leaving.
//
//    Rev 1.10   19 Apr 1996 18:52:26   DSACKIN
// Updated the Error Handling routine to call LogEvent.
// Updated the LogEvent routine to log events correctly.
//
//    Rev 1.9   17 Apr 1996 14:47:06   DSACKIN
// Added WaitForSingleObject if we are running as an executable.
//
//    Rev 1.8   16 Apr 1996 14:07:00   DSACKIN
// Reworked the ErrorHandler function to handle flags for what type of error
// handling is requred for the specified error.
//
//    Rev 1.7   12 Apr 1996 17:20:54   DSACKING
// Added a new ErrorHandlerID function... Commented more code.
//
//    Rev 1.6   11 Apr 1996 16:41:02   DSACKING
// Removed error codes and changed a couple of return values so that we don't
// require a resource in this object.
//
//    Rev 1.5   11 Apr 1996 15:46:44   DSACKING
// Removed the SYM_EXPORT and WINAPI to make it a LIB again.
//
//    Rev 1.4   11 Apr 1996 14:28:24   DSACKING
// Added include "srvcres.h"
//
//    Rev 1.3   11 Apr 1996 13:26:12   DSACKING
// Made ghInst a member variable (m_hInst) that gets set at construction.
//
//    Rev 1.2   10 Apr 1996 18:35:02   PVENKAT
// Added Resource/Message file, modified ErrorHandler for NT Log
//
//    Rev 1.1   10 Apr 1996 16:29:36   DSACKING
// Added SYM_EXPORT WINAPI to public functions.
//
//    Rev 1.0   09 Apr 1996 11:21:00   DSACKING
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

                                        // This gets us the
                                        // MB_SERVICE_NOTIFICATION definition
#define _WIN32_WINNT 0x0400

#include "platform.h"

#include "SymSaferStrings.h"            // for sssnprintf

#include "service.h"
#include <new>

/////////////////////////////////////////////////////////////////////
// Local file defines

#define EMPTY_STRING TEXT("")          // For use with wsprintf

/////////////////////////////////////////////////////////////////////
// CService : Constructor
//
// This function initializes all of our member variables.
//
// Parameters:
//    LPTSTR szService Name - Name of our service
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CService::CService(
   LPSERVICE_MAIN_FUNCTIONW pMain,      // [in] - The name of our Main callback
   LPHANDLER_FUNCTION       pHandler)   // [in] - The name of our Handler callback
{
OSVERSIONINFO osInfo;
TCHAR szTemp[SYM_MAX_PATH+1];
LPTSTR szCommandLine;

   szCommandLine = GetCommandLine ();   // Returns a pointer to the command line.
   STRCPY (szTemp, szCommandLine);      // Make a copy to work with

                                        // I am going to assume we don't have
                                        // to kill the other command line
                                        // parameters for GetModuleHandle to work.

   m_hInst = GetModuleHandle (szTemp);  // Now set our hInst.

                                        // First, set up platform information
   memset (&osInfo, 0, sizeof (osInfo));
   osInfo.dwOSVersionInfoSize = sizeof (osInfo);

   GetVersionEx (&osInfo);
   m_wPlatform = (osInfo.dwPlatformId == VER_PLATFORM_WIN32s) ?  PLAT_WIN31 :
                 (osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) ? PLAT_WINNT : PLAT_WIN95;

   m_pMain    = pMain;                  // Set our Main callback
   m_pHandler = pHandler;               // Set our Handler callback

   m_bExe = FALSE;                      // Assume service functionality

   m_hEvent = NULL;                     // Initialize Handles to NULL

   m_dwErrorCode = NO_ERROR;            // Start with No error code set.
   m_dwStep = 0;                        // Start at step 0

                                        // Zero out our structures to begin.
   MEMSET(&m_hStatus, 0, sizeof(m_hStatus));
   MEMSET(&m_sStatus, 0, sizeof(m_sStatus));

   m_bPauseContinue = TRUE;             // On by default
   m_bAllowStop     = TRUE;             // On by default

   m_bExitingService = FALSE;           // Set this when we go to exit.
}


/////////////////////////////////////////////////////////////////////
// CService : Destructor
//
// This function does our cleanup
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CService::~CService()
{
}


/////////////////////////////////////////////////////////////////////
// Create
//
// This function startus our service
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CService::Create(                 // Starts the service running
   LPWSTR szServiceName)                // [in] - The name of the service
{
DWORD dwReturn = ERROR_SUCCESS;
                                        // Must use the Unicode version here.
SERVICE_TABLE_ENTRYW tbl[2] = {
    {NULL, NULL},
    {NULL, NULL} };

                                        // Set our service name
   wcscpy(m_szServiceName, szServiceName);

   InitializeProfiler(szServiceName);

                                        // Start by creating our event
                                        // that will be used to signal exit
   m_hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
                                        // m_hEvent will be cleaned up
                                        // when InitService exits

   if (m_hEvent)
   {
                                        // First, fill in the blanks on
                                        // our Service Table

      tbl[0].lpServiceName = m_szServiceName;
      tbl[0].lpServiceProc = m_pMain;

      LogMessage(TEXT("StartServiceCtrlDispatcher()"));

                                        // We must call the UNICODE version of
                                        // StartServiceCtrlDispatcher so we can
                                        // use a UNICODE ServiceMain function
      if (!StartServiceCtrlDispatcherW(tbl))
      {
         if (PLAT_WINNT != m_wPlatform) // Not WinNT?
         {
            m_bExe = TRUE;              // Must be Executable.
            InitService ();             // Call InitService Manually.

                                        // Now since we aren't ever going
                                        // to get to the ServiceMain, we need
                                        // to hold our ground here.
            WaitForSingleObject (m_hEvent, INFINITE);
         }
         else
            dwReturn = ERROR_STARTSERVICECTRLDISPATCHER;
      }
      LogMessage(TEXT("Exiting CService.Create()"));
   }

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// DWORD CService::Create - non-UNICODE version
//
// Purpose:
//    Take a non-UNICODE string and pass it in to our UNICODE version
//    of the service stuff.
//
// Parameters:
//    LPSTR lpszServiceName - Name of the service (Will be converted)
//
// Returns:
//    ERROR_INVALID_DATA - Couldn't convert the string
//
//    otherwise it will be the return value from the UNICODE version
//
// See Also:
//    Create (UNICODE version)
//
/////////////////////////////////////////////////////////////////////
// 09/30/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CService::Create(                 // Starts the service running
   LPSTR lpszServiceName)               // [in] - The name of the service
{
DWORD  dwReturn;
WCHAR  szServiceNameW[MAX_SERVICE_NAME_LENGTH + 1];

   dwReturn = MultiByteToWideChar (
                 CP_ACP,
                 MB_PRECOMPOSED,
                 lpszServiceName,
                 -1,
                 szServiceNameW,
                 sizeof(szServiceNameW) / sizeof(WCHAR) );

                                        // If we dwReturn is zero, just return
                                        // it to the caller.
   if (0 == dwReturn)
   {
      dwReturn = ERROR_INVALID_DATA;
      goto Cleanup;
   }

                                        // Call the UNICODE version.
   dwReturn = Create (szServiceNameW);

Cleanup:

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// -- Utility Functions --


/////////////////////////////////////////////////////////////////////
// ReportStatusToSCMgr
//
// This function reports our current status to the Service Control Manager
//
// Parameters:
//    [in] DWORD dwCurrentState  - What state is the service in at the moment
//    [in] DWORD dwWin32ExitCode - If exiting, what is the exit code
//    [in] DWORD dwWaitHint      - How many milliseconds until next check point?
//
// Return Value:
//    TRUE  - Reported successfully
//    FALSE - Report failed
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CService::ReportStatusToSCMgr(
   DWORD dwCurrentState,
   DWORD dwWin32ExitCode,
   DWORD dwWaitHint)
{
BOOL bResult = TRUE;                    // Start with success

                                        // SCM to be called only
                                        // if invoked as Service.
   if (!m_bExe)
   {
      {
         TCHAR szFormat[256] = TEXT("Reporting Status:%s");

         switch (dwCurrentState)
         {
            case SERVICE_STOPPED:
               LogMessage(szFormat, TEXT("SERVICE_STOPPED"));
               break;
            case SERVICE_START_PENDING:
               LogMessage(szFormat, TEXT("SERVICE_START_PENDING"));
               break;
            case SERVICE_STOP_PENDING:
               LogMessage(szFormat, TEXT("SERVICE_STOP_PENDING"));
               break;
            case SERVICE_RUNNING:
               LogMessage(szFormat, TEXT("SERVICE_RUNNING"));
               break;
            case SERVICE_CONTINUE_PENDING:
               LogMessage(szFormat, TEXT("SERVICE_CONTINUE_PENDING"));
               break;
            case SERVICE_PAUSE_PENDING:
               LogMessage(szFormat, TEXT("SERVICE_PAUSE_PENDING"));
               break;
            case SERVICE_PAUSED:
               LogMessage(szFormat, TEXT("SERVICE_PAUSED"));
               break;
            default:
               LogMessage(szFormat, TEXT("UNKNOWN_STATE!"));
               break;
         }
      }

                                        // Fill the status with the
                                        // provided parameters
      if (dwCurrentState == SERVICE_START_PENDING)
         m_sStatus.dwControlsAccepted = 0;
      else
      {
         m_sStatus.dwControlsAccepted = 0;

         if (m_bAllowStop)
            m_sStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP;

         if (m_bPauseContinue)
            m_sStatus.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
      }

      m_sStatus.dwCurrentState  = dwCurrentState;
      m_sStatus.dwWin32ExitCode = dwWin32ExitCode;
      m_sStatus.dwCheckPoint    = m_dwStep;
      m_sStatus.dwWaitHint      = dwWaitHint;

      m_dwStep++;                       // Make sure the next step gets the
                                        // next number.

                                        // Inform the SCM of this status.
      bResult = SetServiceStatus (m_hStatus,
                                 &m_sStatus);
   }

   return bResult;
}


/////////////////////////////////////////////////////////////////////
// LogEventW
//
// This function logs a string to the System Event Log
//
// Parameters:
//    [in] DWORD - ID of event to log.
//    [in] [optional] LPTSTR - Supplimentary string 1
//    [in] [optional] LPTSTR - Supplimentary string 2
//    [in] [optional] LPTSTR - Supplimentary string 3
//    [in] [optional] LPTSTR - Supplimentary string 4
//    [in] [optional] LPTSTR - Supplimentary string 5
//    [in] [optional] LPTSTR - Supplimentary string 6
//    [in] [optional] PSID - This SID is sent to the function if we need to
//                         - log using someone else's SID.  If WE are logging
//                         - something, just leave this NULL and default
//                         - security will be used.
//
// Return Value:
//    TRUE  - Logged successfully
//    FALSE - Log failed
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 - DSACKIN - Function Created
// 09/30/96 - DSACKIN - Reworked UNICODE support
// 01/21/97 - TSMITH  - Added sixth supplimentary string.
/////////////////////////////////////////////////////////////////////
BOOL CService::LogEventW(
   DWORD  dwErrID,                      // [in] - ID of event to log
   LPWSTR lpszString1,                  // [in] [optional] supplimentary string
   LPWSTR lpszString2,                  // [in] [optional] supplimentary string
   LPWSTR lpszString3,                  // [in] [optional] supplimentary string
   LPWSTR lpszString4,                  // [in] [optional] supplimentary string
   LPWSTR lpszString5,                  // [in] [optional] supplimentary string
   LPWSTR lpszString6,                  // [in] [optional] supplimentary string
   PSID   pSid)                         // [in] [optional] SID to use in logging
{
LPWSTR  lpszStr[MAX_STRINGS + 1];       // List of strings for ReportEvent
DWORD   dwMsgID;
WORD    wCount, wResourceID, wCatID;
HANDLE  hEventSource;
BOOL    bReturn = TRUE;                 // Assume success


   wResourceID = LOWORD(dwErrID);       // This will be our resource ID base


                                        // Set up our string list for the
                                        // call to ReportEvent
   lpszStr[0] = lpszString1;
   lpszStr[1] = lpszString2;
   lpszStr[2] = lpszString3;
   lpszStr[3] = lpszString4;
   lpszStr[4] = lpszString5;
   lpszStr[5] = lpszString6;
   lpszStr[6] = NULL;

                                        // Now count the number of strings.
   for (wCount = 0;
        (wCount < MAX_STRINGS) && (lpszStr[wCount]);
        wCount++);

                                        // Decide what message category we have.
   if (0 != (dwErrID & ID_ERR_FATAL))
      wCatID = ID_ERR_CAT_ERROR;
   else if (0 != (dwErrID & ID_ERR_MESSAGEBOX))
      wCatID = ID_ERR_CAT_WARNING;
   else
      wCatID = ID_ERR_CAT_INFORMATION;

                                        // Build our MC Resource ID.
   dwMsgID = wResourceID | ID_ERR_MCBASE;

                                        // NT Event Log:
                                        // Get the Handle and log the error.
   hEventSource = RegisterEventSourceW (NULL, m_szServiceName);

   if (hEventSource)
   {
      if (!ReportEventW (
              hEventSource,             // Handle to Event Source
              EVENTLOG_ERROR_TYPE,      // Event type to log
              wCatID,                   // Event category
              dwMsgID,                  // Event ID (from the MC file)
              pSid,                     // Security ID
              wCount,                   // Number of strings
              0,
              (LPCWSTR * )lpszStr,      // Array of strings to merge with MSG
              NULL))                    // Address of RAW data (not used)
      {
         LogMessage(TEXT("*** Couldn't Report the Event."));
         bReturn = FALSE;
      }

      DeregisterEventSource (hEventSource);
   }
   else
      bReturn = FALSE;

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// LogEventA
//
// This function logs a string to the System Event Log
//
// Parameters:
//    [in] DWORD - ID of event to log.
//    [in] [optional] LPSTR - Supplimentary string 1
//    [in] [optional] LPSTR - Supplimentary string 2
//    [in] [optional] LPSTR - Supplimentary string 3
//    [in] [optional] LPSTR - Supplimentary string 4
//    [in] [optional] LPSTR - Supplimentary string 5
//    [in] [optional] LPSTR - Supplimentary string 6
//    [in] [optional] PSID - This SID is sent to the function if we need to
//                         - log using someone else's SID.  If WE are logging
//                         - something, just leave this NULL and default
//                         - security will be used.
//
// Return Value:
//    TRUE  - Logged successfully
//    FALSE - Log failed
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 - DSACKIN - Function Created
// 09/30/96 - DSACKIN - Reworked UNICODE support
// 01/21/97 - TSMITH  - Added sixth supplimentary string.
/////////////////////////////////////////////////////////////////////
BOOL CService::LogEventA(
   DWORD  dwErrID,                      // [in] - ID of event to log
   LPSTR  lpszString1,                  // [in] [optional] supplimentary string
   LPSTR  lpszString2,                  // [in] [optional] supplimentary string
   LPSTR  lpszString3,                  // [in] [optional] supplimentary string
   LPSTR  lpszString4,                  // [in] [optional] supplimentary string
   LPSTR  lpszString5,                  // [in] [optional] supplimentary string
   LPSTR  lpszString6,                  // [in] [optional] supplimentary string
   PSID   pSid)                         // [in] [optional] SID to use in logging
{
BOOL   bReturn;
DWORD  dwLength;
DWORD  dwReturn;
LPSTR  lpszStrings[ MAX_STRINGS + 1 ];  // holders for non-UNICODE strings
LPWSTR lpszStringsW[ MAX_STRINGS + 1 ]; // holders for UNICODE strings
int    nCount;


                                        // First set the string holders.
   lpszStrings[0] = lpszString1;
   lpszStrings[1] = lpszString2;
   lpszStrings[2] = lpszString3;
   lpszStrings[3] = lpszString4;
   lpszStrings[4] = lpszString5;
   lpszStrings[5] = lpszString6;
   lpszStrings[6] = NULL;

                                        // Second, allocate memory and convert
                                        // the strings

   for (nCount = 0; nCount < MAX_STRINGS; nCount++)
   {
      if (NULL != lpszStrings[nCount])
      {
                                        // Get the length
         dwLength = strlen (lpszStrings[nCount]);

                                        // Allocate a UNICODE string
		 try
		 {
			lpszStringsW[nCount] = new WCHAR [dwLength + 1];
		}
		 catch(std::bad_alloc &){}

         if (NULL == lpszStringsW[nCount])
         {
            bReturn = FALSE;
            goto Cleanup;
         }

         dwReturn = MultiByteToWideChar (
                       CP_ACP,
                       MB_PRECOMPOSED,
                       lpszStrings[nCount],
                       -1,
                       lpszStringsW[nCount],
                       (dwLength+1) / sizeof (WCHAR) );
      }
      else                              // It was a NULL string
         lpszStringsW[nCount] = NULL;
   }

                                        // Now pass along our converted
                                        // parameters to the UNICODE version of
                                        // this function to do all of the work.
   bReturn = LogEventW(
                dwErrID,
                lpszStringsW[0],
                lpszStringsW[1],
                lpszStringsW[2],
                lpszStringsW[3],
                lpszStringsW[4],
                lpszStringsW[5],
                pSid);

Cleanup:

   for (nCount = 0; nCount < MAX_STRINGS; nCount++)
   {
      if (NULL != lpszStringsW[nCount])
      {
         delete lpszStringsW[nCount];
         lpszStringsW[nCount] = NULL;
      }
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// ErrorHandler
//
// This function takes a message ID and extracts from it a STRING ID
// and also a MSG ID.  The string ID is extracted from the String table
// in the .rc file while the MSG ID is extracted from the .mc source by
// the ReportEvent function.
//
// Parameters:
//    [in] DWORD dwErrID - ID of the Message to send to the Log
//    [in] [optional] LPTSTR - Supplimentary string 1
//    [in] [optional] LPTSTR - Supplimentary string 2
//    [in] [optional] LPTSTR - Supplimentary string 3
//    [in] [optional] LPTSTR - Supplimentary string 4
//    [in] [optional] LPTSTR - Supplimentary string 5
//    [in] [optional] LPTSTR - Supplimentary string 6
//    [in] [optional] PSID - This SID is sent to the function if we need to
//                         - log using someone else's SID.  If WE are logging
//                         - something, just leave this NULL and default
//                         - security will be used.
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
// 04/10/96 PVENKAT - Reworked
// 09/30/96 DSACKIN - Added UNICODE support
// 01/21/97 TSMITH  - Added sixth supplimentary string.
/////////////////////////////////////////////////////////////////////
VOID CService::ErrorHandlerW (
   DWORD  dwErrID,                      // [in] - ID of event to log
   LPWSTR lpszString1,                  // [in] [optional] supplimentary string
   LPWSTR lpszString2,                  // [in] [optional] supplimentary string
   LPWSTR lpszString3,                  // [in] [optional] supplimentary string
   LPWSTR lpszString4,                  // [in] [optional] supplimentary string
   LPWSTR lpszString5,                  // [in] [optional] supplimentary string
   LPWSTR lpszString6,                  // [in] [optional] supplimentary string
   PSID   pSid)                         // [in] [optional] SID to use in logging
{
DWORD   dwLen, dwStrID;
WORD    wCount, wResourceID;
LPWSTR  lpszStr[ MAX_STRINGS + 1 ];     // List of strings for ReportEvent
WCHAR   szTemplate[MAX_MESSAGE_LEN + 1];// This is where we get our template
WCHAR   szErrMsg[MAX_MESSAGE_LEN + 1];  // Build our error message here.

                                        // Get the various ID from error-code.
   m_dwErrorCode = GetLastError ();     // Last known error

   wResourceID = LOWORD(dwErrID);       // This will be our resource ID base


                                        // Set up our string list for the
                                        // call to ReportEvent
   lpszStr[0] = lpszString1;
   lpszStr[1] = lpszString2;
   lpszStr[2] = lpszString3;
   lpszStr[3] = lpszString4;
   lpszStr[4] = lpszString5;
   lpszStr[5] = lpszString6;
   lpszStr[6] = NULL;

                                        // Now count the number of strings.
   for (dwLen = 0, wCount = 0;
        (wCount < MAX_STRINGS) && (lpszStr[wCount]);
        wCount++)
   {
      dwLen += wcslen (lpszStr[wCount]);
   }

   dwLen *= sizeof (WCHAR);             // Mulitply Length by size of our CHAR

                                        // Put it into the NT Event log?
   if (0 != (dwErrID & ID_ERR_LOGEVENT))
   {
      LogEventW(                        // Use our WCHAR LogEvent function
         dwErrID,
         lpszStr[0],
         lpszStr[1],
         lpszStr[2],
         lpszStr[3],
         lpszStr[4],
         lpszStr[5],
         pSid
         );
   }

                                        // Display error as a message box?
   if (0 != (dwErrID & ID_ERR_MESSAGEBOX))
   {
                                        // Set up our string ID.
      dwStrID = wResourceID;

                                        // Call the WCHAR version of LoadString
      if (0 != LoadStringW (
                  m_hInst,
                  dwStrID,
                  szTemplate,
                  sizeof (szTemplate) / sizeof (WCHAR)) )
      {
                                        // WCHAR version of sprintf
         sssnprintf (
            szErrMsg,                   // Build the error here.
            sizeof(szErrMsg),
            szTemplate,                 // using this template.
            lpszStr[0],                 // Strings have been initialized to
            lpszStr[1],                 // EMPTY_STRING so if they are not
            lpszStr[2],                 // used, everything will still work.
            lpszStr[3],
            lpszStr[4],
            lpszStr[5]
            );

                                        // Here I must specifically set the
                                        // first parameter to a UNICODE string.
         LogMessage(L"CService::ErrorHandler() \"%s\"", szErrMsg);
                                        // Display the error Message.
         MessageBoxW (
            NULL,
            szErrMsg,
            m_szServiceName,
            MB_OK | MB_SERVICE_NOTIFICATION);
      }
      else
      {
         SYM_ASSERT (FALSE);            // Can't report loadstring failure.
      }
   }

   if (0 != (dwErrID & ID_ERR_FATAL))   // Abort our service??
   {
                                        // If we are already aborting, don't
                                        // recurse into the shutdown code.
      if (!m_bExitingService)
      {
         m_bExitingService = TRUE;

         ReportStatusToSCMgr (SERVICE_STOP_PENDING, NO_ERROR, 3000);

         ServiceControlStop();          // Allow user to do what they need

         SetEvent (m_hEvent);           // Signal to cleanup and exit.
      }
   }
}


/////////////////////////////////////////////////////////////////////
// ErrorHandlerA
//
// This function converts all of the support strings to UNICODE and
// passes them into the UNICODE version of the same function.
//
// Parameters:
//    [in] DWORD dwErrID - ID of the Message to send to the Log
//    [in] [optional] LPSTR - Supplimentary string 1
//    [in] [optional] LPSTR - Supplimentary string 2
//    [in] [optional] LPSTR - Supplimentary string 3
//    [in] [optional] LPSTR - Supplimentary string 4
//    [in] [optional] LPSTR - Supplimentary string 5
//    [in] [optional] LPSTR - Supplimentary string 6
//    [in] [optional] PSID - This SID is sent to the function if we need to
//                         - log using someone else's SID.  If WE are logging
//                         - something, just leave this NULL and default
//                         - security will be used.
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
// 04/10/96 PVENKAT - Reworked
// 09/30/96 DSACKIN - Added non-UNICODE support
/////////////////////////////////////////////////////////////////////
VOID CService::ErrorHandlerA(
   DWORD  dwErrID,                      // [in] - ID of event to log
   LPSTR  lpszString1,                  // [in] [optional] supplimentary string
   LPSTR  lpszString2,                  // [in] [optional] supplimentary string
   LPSTR  lpszString3,                  // [in] [optional] supplimentary string
   LPSTR  lpszString4,                  // [in] [optional] supplimentary string
   LPSTR  lpszString5,                  // [in] [optional] supplimentary string
   LPSTR  lpszString6,                  // [in] [optional] supplimentary string
   PSID   pSid)                         // [in] [optional] SID to use in logging
{
DWORD  dwLength;
DWORD  dwReturn;
LPSTR  lpszStrings[ MAX_STRINGS + 1 ];  // holders for non-UNICODE strings
LPWSTR lpszStringsW[ MAX_STRINGS + 1 ]; // holders for UNICODE strings
int    nCount;


                                        // First set the string holders.
   lpszStrings[0] = lpszString1;
   lpszStrings[1] = lpszString2;
   lpszStrings[2] = lpszString3;
   lpszStrings[3] = lpszString4;
   lpszStrings[4] = lpszString5;
   lpszStrings[5] = lpszString6;
   lpszStrings[6] = NULL;


                                        // Second, allocate memory and convert
                                        // the strings

   for (nCount = 0; nCount < MAX_STRINGS; nCount++)
   {
      if (NULL != lpszStrings[nCount])
      {
                                        // Get the length
         dwLength = strlen (lpszStrings[nCount]);

                                        // Allocate a UNICODE string
		 try
		 {
			lpszStringsW[nCount] = new WCHAR [dwLength + 1];
		 }
		 catch(std::bad_alloc &) { }


         if (NULL == lpszStringsW[nCount])
         {
            goto Cleanup;
         }

         dwReturn = MultiByteToWideChar (
                       CP_ACP,
                       MB_PRECOMPOSED,
                       lpszStrings[nCount],
                       -1,
                       lpszStringsW[nCount],
                       (dwLength+1) / sizeof (WCHAR) );
      }
      else                              // It was a NULL string
         lpszStringsW[nCount] = NULL;
   }

                                        // Now pass along our converted
                                        // parameters to the UNICODE version of
                                        // this function to do all of the work.
   ErrorHandlerW(
      dwErrID,
      lpszStringsW[0],
      lpszStringsW[1],
      lpszStringsW[2],
      lpszStringsW[3],
      lpszStringsW[4],
      lpszStringsW[5],
      pSid);

Cleanup:

   for (nCount = 0; nCount < MAX_STRINGS; nCount++)
   {
      if (NULL != lpszStringsW[nCount])
      {
         delete lpszStringsW[nCount];
         lpszStringsW[nCount] = NULL;
      }
   }

}



/////////////////////////////////////////////////////////////////////
// Handler
//
// This function calls the ServiceControlxxxxx functions based on
// the control code that comes in from the Service Manager
//
// Parameters:
//    [in] DWORD dwCtrlCode  - Message from Service Manager
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::Handler(
   DWORD dwCtrlCode)
{
DWORD dwState = SERVICE_RUNNING;

   switch(dwCtrlCode)
   {
      case SERVICE_CONTROL_PAUSE:

         LogMessage(TEXT("CService::Handler() SERVICE_CONTROL_PAUSE"));

         ServiceControlPause();

         if (SERVICE_RUNNING == m_sStatus.dwCurrentState)
            dwState = SERVICE_PAUSED;   // Set our paused state
         break;

      case SERVICE_CONTROL_CONTINUE:

         LogMessage(TEXT("CService::Handler() SERVICE_CONTROL_CONTINUE"));

         ServiceControlContinue();

         if (SERVICE_PAUSED == m_sStatus.dwCurrentState)
            dwState = SERVICE_RUNNING;  // Set us back to running
         break;

      case SERVICE_CONTROL_STOP:

         LogMessage(TEXT("CService::Handler() SERVICE_CONTROL_STOP"));

                                        // Don't allow recursion!
         if (!m_bExitingService)
         {
            m_bExitingService = TRUE;

            dwState = SERVICE_STOP_PENDING;
            ReportStatusToSCMgr (dwState, NO_ERROR, 3000);

            ServiceControlStop();          // Allow user to do what then need

            SetEvent (m_hEvent);           // Signal to cleanup and exit.
         }
         break;

      case SERVICE_CONTROL_INTERROGATE:
         LogMessage(TEXT("CService::Handler() SERVICE_CONTROL_INTERROGATE"));
                                        // Interrogation always occurs at the
                                        // end of a Control Call
         break;

      default:

         LogMessage(TEXT("CService::Handler() User Service Control Code Received."));
                                        // If we don't know what it is,
                                        // perhaps it is a User Code
         ServiceControlUser(dwCtrlCode);
         break;
   }

   if (SERVICE_STOP_PENDING != dwState)
   {
      ReportStatusToSCMgr (dwState,     // Now report our status
                           NO_ERROR,    // No error
                           0);          // No wait hint
   }
}


/////////////////////////////////////////////////////////////////////
// -- Virtual Functions --


/////////////////////////////////////////////////////////////////////
// ServiceMain
//
// This function startus our service
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CService::ServiceMain(
   DWORD dwArgc,                        // [in] - Number of parameters
   LPWSTR *lpszArgv)                    // [in] - Actual parameters.
{
BOOL bResult = TRUE;

   LogMessage(TEXT("Entering CService::ServiceMain."));
                                        // Set our Service Status Flags
   m_sStatus.dwServiceType      = SERVICE_WIN32_OWN_PROCESS;
   m_sStatus.dwCurrentState     = SERVICE_START_PENDING;
   m_sStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                  SERVICE_ACCEPT_PAUSE_CONTINUE;
   m_sStatus.dwWin32ExitCode           = 0;
   m_sStatus.dwServiceSpecificExitCode = 0;
   m_sStatus.dwCheckPoint              = 0;
   m_sStatus.dwWaitHint                = 0;

                                        // Register the Handler function with
                                        // SCM to receive control requests
   m_hStatus = RegisterServiceCtrlHandlerW (
                  m_szServiceName,      // Service Name
                  m_pHandler);          // Function Name (Set in Constructor)

                                        // If succeeded, do the
                                        // Initialization, or exit.
   if (m_hStatus)
   {
      LogMessage(TEXT("Successful call to RegisterServiceCtrlHandler"));
                                        // Report to SCM to wait for some
                                        // time until we do the init.
      ReportStatusToSCMgr (SERVICE_START_PENDING,
                           NO_ERROR,
                           3000);

      LogMessage(TEXT("Before InitService"));

      InitService ();                   // Initialization: Start the WorkerThread

      LogMessage(TEXT("After InitService"));

                                        // Now that the service is running,
                                        // we need to wait until told to
                                        // stop it.
      if (!m_bExitingService)
      {
         WaitForSingleObject (m_hEvent, INFINITE);
      }

      LogMessage(TEXT("Event has been set.  Exiting ServiceMain"));

      if (m_hEvent)
         CloseHandle (m_hEvent);

                                        // At this point, the actual Service
                                        // needs to report when it is shut down.

   }
   else
      bResult = FALSE;

   LogMessage(TEXT("Exiting CService::ServiceMain()"));

   return bResult;                      // Error handler will be called in
                                        // our passthrough function so the
                                        // end class can make the error ID.
}


/////////////////////////////////////////////////////////////////////
// InitService
//
// This function does our service initialization
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::InitService (VOID)
{
}


/////////////////////////////////////////////////////////////////////
// ServiceConrtolPause
//
// This function is where you can over-ride to get access when the
// Service Conrtoller issues a Pause Command.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::ServiceControlPause(VOID)
{
}


/////////////////////////////////////////////////////////////////////
// ServiceConrtolContinue
//
// This function is where you can over-ride to get access when the
// Service Conrtoller issues a Continue Command.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::ServiceControlContinue(VOID)
{
}


/////////////////////////////////////////////////////////////////////
// ServiceConrtolStop
//
// This function is where you can over-ride to get access when the
// Service Conrtoller issues a Stop Command.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::ServiceControlStop(VOID)
{
}


/////////////////////////////////////////////////////////////////////
// ServiceConrtolUser
//
// This function is where you can over-ride to get access when the
// Service Conrtoller issues a User defined Command.
//
// Parameters:
//    DWORD dwCtrlCode - the code from the Service Conrtoller
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::ServiceControlUser(DWORD dwCtrlCode)
{
}

/////////////////////////////////////////////////////////////////////
// AllowPauseContinue
//
// This function is used to set wether a service will accept the
// Pause and continue commands.
//
// Parameters:
//    BOOL - bPauseContinue - Allow pause/continue?
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::AllowPauseContinue(
   BOOL bPauseContinue)
{
   m_bPauseContinue = bPauseContinue;
}


/////////////////////////////////////////////////////////////////////
// AllowStop
//
// This function is used to set wether a service will accept the
// Stop command.
//
// Parameters:
//    BOOL - bAllowStop
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CService::AllowStop(
   BOOL bAllowStop)
{
   m_bAllowStop = bAllowStop;
}



