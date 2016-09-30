/////////////////////////////////////////////////////////////////////
// SVCCTRL.CPP
//
// This is the implementation file for the CServiceControl object.
//
// This file MUST be compiled using UNICODE and _UNICODE.  This object
// however may be used in either type of executable.
//

#include "platform.h"

#include "svcctrl.h"


/////////////////////////////////////////////////////////////////////
// CServiceControl : Constructor
//
// This function initializes our member variables.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CServiceControl::CServiceControl()
{
                                        // Initialize all of our member vars.
   m_hSCMgr   = NULL;
   m_hService = NULL;
   m_hDevice  = INVALID_HANDLE_VALUE;

   m_bStarted = FALSE;                  // Set to true when we are running.
   m_bLocked  = FALSE;                  // Set to true if we lock the service.

   m_szServiceName[0] = EOS;
   m_szMachineName[0] = EOS;

   m_lpConfig = NULL;                   // This is allocated info.

   m_szLogName[0] = EOS;
}


/////////////////////////////////////////////////////////////////////
// CServiceControl : Destructor
//
// This function cleans up anything leftover in our service control
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CServiceControl::~CServiceControl()
{
   Close();
}


/////////////////////////////////////////////////////////////////////
// CServiceControl : Initialize
//
// This function allocates memory for our two strings that we need
// to keep locally.
//
// Parameters:
//    none
//
// Return Value:
//    ERROR_SUCCESS - Initialization OK
//
//    ERROR_SVCCTRL_ALLOCATE - Failed to allocate memory
//
/////////////////////////////////////////////////////////////////////
// 04/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::Initialize(VOID)
{
DWORD     dwReturn = ERROR_SUCCESS;

                                        // First, make sure that if we
                                        // have already initialized that
                                        // we clean up previously allocated
                                        // memory.
   Close();

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// CServiceControl : Open
//
// This function sets up our communications with the device or service.
// It applies a lock to the service so other people can't unload it.
//
// Parameters:
//    lpszServiceName - [in] - Service name  ( "SymEvent" ), ( "NAVAP" )
//    lpszMachineName - [in] - Machine were service is (NULL = Local)
//
// Return Value:
//    ERROR_SUCCESS - Initialization OK
//
//    ERROR_SVCCTRL_OPEN_SCMGR  - Failed to open SCMgr
//    ERROR_SVCCTRL_SERVICEOPEN - Failed to open the Service
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::Open(
   PCWSTR lpszServiceName,              // [in] - Service Name
   PCWSTR lpszMachineName)              // [in] - Machine where service is
{
DWORD     dwReturn = ERROR_SUCCESS;

   dwReturn = Initialize();

   if (ERROR_SUCCESS == dwReturn)
   {

                                        // Take a copy of the Service Name
      if (NULL == lpszServiceName)
         m_szServiceName[0] = EOS;
      else
         wcscpy (m_szServiceName, lpszServiceName);

      InitializeProfiler(
         (NULL != lpszServiceName) ?
            m_szServiceName :
            TEXT("<no name>"));
                                        // And machine name if NOT NULL!
      if (NULL == lpszMachineName)
         m_szMachineName[0] = EOS;
      else
         wcscpy (m_szMachineName, lpszMachineName);


      m_hSCMgr = OpenSCManager(
                  (EOS == m_szMachineName[0]) ?   // If no machine name,
                     NULL :                // Send in NULL for local machine
                     m_szMachineName,      // or m_szMachineName for name.
                  NULL,                    // database to access (NULL = ServicesActive)
                  SC_MANAGER_ALL_ACCESS);  // we want access to everything

      if (NULL != m_hSCMgr)                // Success in opening the database
      {
                                           // Only open service if we have a name
         if (NULL != lpszServiceName)
         {
            m_hService = OpenService(
                        m_hSCMgr,           // Handle to ServiceManager
                        m_szServiceName,    // Name of the Service to open
                        SERVICE_ALL_ACCESS);// requested access clearance

            if (NULL == m_hService)
            {                              // Failed ServiceOpen
               dwReturn = ERROR_SVCCTRL_SERVICEOPEN;
            }
         }
      }
      else                              // Failed OpenSCManager
         dwReturn = ERROR_SVCCTRL_OPEN_SCMGR;
   }

                                        // No need to clean up since this will
                                        // happen in our destructor.

   return dwReturn;
}



/////////////////////////////////////////////////////////////////////
// CServiceControl : Open
//
// This function takes a standard String for arguments and then
// calls into our normal Open function.
//
// Parameters:
//    lpszServiceName - [in] - Service name  ( "SymEvent" ), ( "NAVAP" )
//    lpszMachineName - [in] - Machine were service is (NULL = Local)
//
// Return Value:
//    ERROR_SUCCESS - Initialization OK
//
//    ERROR_SVCCTRL_OPEN_SCMGR - Failed to open SCMgr
//    ERROR_SVCCTRL_SERVICEOPEN - Failed to open the Service
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

DWORD CServiceControl::Open(
   PCSTR lpszServiceName,               // [in] - Service Name (non-unicode)
   PCSTR lpszMachineName)               // [in] - Machine where service is (non-unicode)
{
WCHAR szServiceNameW[MAX_SERVICENAME_SIZE];
WCHAR szMachineNameW[MAX_SERVICENAME_SIZE];
DWORD dwReturn = ERROR_SUCCESS;
int   nLen;

                                        // Initialize our local guys.
   szServiceNameW[0] = szMachineNameW[0] = EOS;

                                        // Convert if we have strings.
   if (NULL != lpszServiceName)
   {
      nLen = MultiByteToWideChar (
                CP_ACP,
                MB_PRECOMPOSED,
                lpszServiceName,
                -1,
                szServiceNameW,
                sizeof(szServiceNameW) / sizeof(szServiceNameW[0]) );

                                        // Make sure that we got our conversion
      if (0 == nLen)
         dwReturn = ERROR_SVCCTRL_STRINGCONVERSION;
   }

   if ( ERROR_SUCCESS == dwReturn &&
       (NULL != lpszMachineName))
   {
      nLen = MultiByteToWideChar (
                CP_ACP,
                MB_PRECOMPOSED,
                lpszMachineName,
                -1,
                szMachineNameW,
                sizeof(szMachineNameW) / sizeof(szMachineNameW[0]) );

                                        // Make sure that we got our conversion
      if (0 == nLen)
         dwReturn = ERROR_SVCCTRL_STRINGCONVERSION;
   }

   if ( ERROR_SUCCESS == dwReturn )
   {
                                        // Now open it with UNICODE functions.
                                        // if we don't have a machine name,
                                        // send in NULL.
      dwReturn = Open (
                    szServiceNameW,
                    (EOS != szMachineNameW[0]) ?
                       szMachineNameW :
                       NULL );
   }


   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// CServiceControl : Close
//
// This function closes our communications with the device or service.
//
// Parameters:
//    none.
//
// Return Value:
//    ERROR_SUCCESS - DeInitialization Successful.
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CServiceControl::Close(VOID)
{
                                        // Don't leave any locks if we
                                        // are closing our connection with
                                        // a service/driver.
   if (m_bLocked)
   {
      if (INVALID_HANDLE_VALUE != m_hDevice)
      {
         if (!CloseHandle (m_hDevice))
         {
            SYM_ASSERT (FALSE);
         }
         m_hDevice = INVALID_HANDLE_VALUE; // Re-Initialize
      }
   }

   if (NULL != m_hService)
   {
      if (!CloseServiceHandle(m_hService))
      {                                 // This shouldn't fail.
         SYM_ASSERT(FALSE);
      }
      m_hService = NULL;                // Re-Initialize
   }

   if (NULL != m_hSCMgr)
   {
      if (!CloseServiceHandle (m_hSCMgr))
      {                                 // Can't close it? What now?
         SYM_ASSERT (FALSE);
      }
      m_hSCMgr = NULL;                  // Re-Initialize
   }
}



/////////////////////////////////////////////////////////////////////
// CServiceControl : Start
//
// This function starts the service specified in Open
//
// Parameters:
//    none
//
// Return Value:
//    ERROR_SUCCESS - Started OK
//
//    ERROR_SVCCTRL_INVALIDSERVICE - m_hService == NULL
//    ERROR_SVCCTRL_CREATEFILE     - Unable to perform requested lock
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
// 04/30/96 DSACKIN - Added Service Locking here.
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::Start(
   BOOL   bLock)                        // [in] - Lock the service?
{
WCHAR     szDeviceName[MAX_SERVICENAME_SIZE];
DWORD     dwReturn = ERROR_SUCCESS;

   if (NULL == m_hService)
   {
      dwReturn = ERROR_SVCCTRL_INVALIDSERVICE;
   }
   else
   {
      LogMessage(TEXT("Attempting to Start Service/Device"));

      if (StartService(
             m_hService,                // Handle of service to start.
             0,                         // No parameters
             NULL))                     // No parameters
      {
         Sleep(100);                    // Give him 1/10 of a second to get
                                        // the Service Handler installed.
                                        // (1st thing to do!!!)

         dwReturn = WaitForState(SERVICE_RUNNING);

         if (ERROR_SUCCESS == dwReturn)
            m_bStarted = TRUE;          // Service running.
      }
      else
      {
                                        // While the call didn't finish
                                        // successfully, was it because
                                        // the service was already running?
         if (ERROR_SERVICE_ALREADY_RUNNING == GetLastError())
         {
            m_bStarted = TRUE;          // Already running
            LogMessage(TEXT("Service was found to be already running."));
         }
         else
         {
            LogMessage(TEXT("*** Failed StartService Call."));
                                        // Report the Error
            dwReturn = ERROR_SVCCTRL_STARTSERVICE;
         }
      }
   }

   if ( ERROR_SUCCESS == dwReturn )
   {
      if ( bLock )
      {
         LogMessage(TEXT("Attempting to lock service."));

                                        // Next step: bump usage count so
                                        // people can't unload it if requested

                                        // Build the device name
         wcscpy(szDeviceName, TEXT("\\\\.\\") );
         wcscat(szDeviceName, m_szServiceName);

                                        // Now try to open a handle to the
                                        // Device.
         m_hDevice = CreateFile (
                        szDeviceName,   // Name of device to Open.
                        GENERIC_READ | GENERIC_WRITE,        // Access Mode
                        FILE_SHARE_READ | FILE_SHARE_WRITE,  // Share mode
                        NULL,           // Default security
                        OPEN_EXISTING,  // dwCreation Attributes
                        0,              // dwAttributes
                        NULL );         // Template File for attributes

         if (INVALID_HANDLE_VALUE == m_hDevice)  // Failed CreateFile
            dwReturn = ERROR_SVCCTRL_CREATEFILE;
         else
            m_bLocked = TRUE;         // This signals how we clean up.
      }
   }
   return dwReturn;
}



/////////////////////////////////////////////////////////////////////
// CServiceControl : ControlService
//
// This function sends the control codes to the services
//
// Parameters:
//    dwControlCode   - [in]  Code to send to the service
//    lpServiceStatus - [out] If you want the service status, send in
//                            a SERVICE_STATUS struct here.  If NULL,
//                            this parameter is ignored.
//
// Return Value:
//    TRUE  - Service control successful
//    FALSE - Service conrtol Unsuccessful
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CServiceControl::ControlService(
   DWORD            dwControlCode,            // What control code to send
   LPSERVICE_STATUS lpServiceStatus)          // Return the status here
{
SERVICE_STATUS   rServiceStatus;        // If one isn't supplied, use this one
BOOL             bReturn = TRUE;

   LogMessage(TEXT("Attempting to send control [%d] to service."), dwControlCode);

   if (NULL == lpServiceStatus)
      lpServiceStatus = &rServiceStatus; // Point to us if one isn't supplied

   if (NULL == m_hService)
   {
      bReturn = FALSE;
      SYM_ASSERT (NULL != m_hService);  // m_hService must be valid!
   }
   else
   {
      bReturn = ::ControlService(
                   m_hService,          // Opened in Open()
                   dwControlCode,
                   lpServiceStatus);
   }

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// CServiceControl : Stop
//
// This function sends the SERVICE_CONTROL_STOP code to the service
//
// Parameters:
//    none
//
// Return Value:
//    TRUE  - Service Stopped successfully
//    FALSE - Service Not Stopped Successfully
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
// 04/30/96 DSACKIN - Added unlock code before driver is unloaded.
// 07/24/96 DSACKIN - Always allow attempt to stop even if we didn't
//                  - start it.
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::Stop(VOID)
{
DWORD dwReturn = ERROR_SUCCESS;

   LogMessage(TEXT("Attempting to Stop Service"));

   if (NULL == m_hService)
   {
      dwReturn = ERROR_SVCCTRL_INVALIDSERVICE;
      goto Cleanup;
   }

                                        // Before shutting it down, we need
                                        // to release our lock on it.
   if (m_bLocked)
   {
      if (INVALID_HANDLE_VALUE != m_hDevice)
      {
         if (!CloseHandle (m_hDevice))
         {
            SYM_ASSERT (FALSE);
         }
         m_hDevice = INVALID_HANDLE_VALUE; // Re-Initialize
      }
   }

   if (!ControlService(
           SERVICE_CONTROL_STOP))       // Control to send to Service
   {                                    // Report the Error.
      dwReturn = ERROR_SVCCTRL_STOPSERVICE;
   }
   else
   {
                                        // Now wait for the stopped state
      dwReturn = WaitForState(SERVICE_STOPPED);

      if (ERROR_SUCCESS == dwReturn)
         m_bStarted = FALSE;            // Service Stopped successfully.
   }

Cleanup:

   return dwReturn;
}



/////////////////////////////////////////////////////////////////////
// CServiceControl : WaitForState
//
// This function waits for the specified state to happen in the service.
//
// Parameters:
//    none
//
// Return Value:
//    ERROR_SUCCESS - Started OK
//
//    ERROR_SVCCTRL_INVALIDSERVICE - m_hService == NULL
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
#define MAX_FAILURES_ALLOWED  4
#define SLEEP_STEP_MS         5000
DWORD CServiceControl::WaitForState(
   DWORD   dwState)                     // [in] - State to wait for.
{
SERVICE_STATUS   ssStatus;
DWORD            dwReturn = ERROR_SUCCESS;
DWORD            dwOldCheckPoint = 0;
DWORD            dwFailures = 0;
DWORD            dwSleepTime, dwCheckPointTime;

   LogMessage(TEXT("CServiceControl::WaitForState(%d)"), dwState);

   if (NULL == m_hService)
   {
      dwReturn = ERROR_SVCCTRL_SERVICESTATUS;
      goto Cleanup;
   }


   if ( !::QueryServiceStatus(          // Query Status
           m_hService,                  // Handle to service/device
           &ssStatus) )                 // Status to be filled out.
   {
      dwReturn = ERROR_SVCCTRL_SERVICESTATUS;
      dwFailures++;
   }

   while ( (dwFailures < MAX_FAILURES_ALLOWED) &&
           (dwState != ssStatus.dwCurrentState) )
   {
   LogMessage(TEXT("CServiceControl::WaitForState() - CheckPoint: %d"), ssStatus.dwCheckPoint);

                                        // Make sure progress is being made
      dwOldCheckPoint = ssStatus.dwCheckPoint;

                                        // Don't allow a wait hint of 0.  This
                                        // will automatically make us fail
      dwCheckPointTime = (ssStatus.dwWaitHint > 0) ?
                             ssStatus.dwWaitHint :
                             100;

      while ( (dwCheckPointTime > 0) &&
              (dwState != ssStatus.dwCurrentState) )
      {
                                        // TO make us more responsive to
                                        // the system, we check every
                                        // SLEEP_STEP_MS for the desired
                                        // state.
         dwSleepTime = (dwCheckPointTime <= SLEEP_STEP_MS) ?
                          dwCheckPointTime :
                          SLEEP_STEP_MS;

         LogMessage(TEXT("CServiceControl::WaitForState() - Sleeping %d milliseconds"), dwSleepTime);

         Sleep (dwSleepTime);

         dwCheckPointTime -= dwSleepTime;

         if ( !::QueryServiceStatus(    // the next checkpoint
                   m_hService,          // Handle to service/device
                   &ssStatus) )         // Status to be filled out.
         {
                                        // Keep track of our failures.
            dwFailures++;

            if (dwFailures >= MAX_FAILURES_ALLOWED)
               dwReturn = ERROR_SVCCTRL_SERVICESTATUS;
         }

      }

                                        // Check if they have got to the state
                                        // they were looking for or the next checkpoint
      if ( (dwState != ssStatus.dwCurrentState) &&
            (ssStatus.dwCheckPoint <= dwOldCheckPoint) )
      {
         LogMessage(TEXT("CServiceControl::WaitForState() - New Checkpoint: %d"), ssStatus.dwCheckPoint);

         dwFailures++;               // Add another failure

         if (dwFailures >= MAX_FAILURES_ALLOWED)
            dwReturn = ERROR_SVCCTRL_FAILEDSTATE;
      }
      else
      {
                                       // Reseting our failure count
         dwFailures = 0;
      }
   }

Cleanup:

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// CServiceControl : QueryServiceStatus
//
// This function gets the last status as it was reported to the SCMGR.
//
// Parameters:
//    LPSERVICE_STATUS  lpServiceStatus - Only use if you want the entire
//                                        status structure returned.
//
// Return Value:
//    FALSE  - Couldn't get Status
//    TRUE   - Status is being returned.
//
/////////////////////////////////////////////////////////////////////
// 05/06/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CServiceControl::QueryServiceStatus ( // Returns Current State
   LPSERVICE_STATUS lpServiceStatus)       // [out] - Return the status?
{
BOOL             bReturn = TRUE;

   SYM_ASSERT (lpServiceStatus);
   SYM_ASSERT (m_hService);

   if (NULL != lpServiceStatus)
   {
      if (NULL != m_hService)
      {
         bReturn = ::QueryServiceStatus(
                     m_hService,          // Opened in Open()
                     lpServiceStatus);
      }
      else // (NULL != m_hService)
         bReturn = FALSE;
   }
   else // (NULL != lpServiceStatus)
      bReturn = FALSE;

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CServiceControl : QueryServiceStatus
//
// This function gets the last status as it was reported to the SCMGR.
//
// Parameters:
//    LPDWORD  lpdwStatus - Returns only Current state of the Service.
//
// Return Value:
//    FALSE  - Couln't get Status
//    TRUE   - Status is being returned.
//
/////////////////////////////////////////////////////////////////////
// 05/06/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CServiceControl::QueryServiceStatus ( // Returns Current State
   LPDWORD  lpdwStatus)                    // [out] - Return the status?
{
SERVICE_STATUS   rServiceStatus;        // If one isn't supplied, use this one
BOOL             bReturn;

   SYM_ASSERT (lpdwStatus);

   bReturn = QueryServiceStatus (&rServiceStatus);

   if ( bReturn )
      *lpdwStatus = rServiceStatus.dwCurrentState;

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// CServiceControl : GetName
//
// This function copies the name of our service into a buffer.
//
// Parameters:
//    none.
//
// Return Value:
//    DWORD - Number of bytes copied.
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::GetName(
   LPWSTR  lpszServiceName)             // Place to return the name
{
DWORD dwReturn = 0;

   if (NULL == lpszServiceName)
   {
      SYM_ASSERT(FALSE);
   }
   else
   {
      wcscpy (lpszServiceName, m_szServiceName);
      dwReturn = STRLEN (m_szServiceName) * sizeof (WCHAR);
   }

   return dwReturn;
}





/////////////////////////////////////////////////////////////////////
// CServiceControl : GetName
//
// This function copies the name of our service into a buffer.
//
// Parameters:
//    none.
//
// Return Value:
//    ERROR_SUCCESS - DeInitialization Successful.
//
/////////////////////////////////////////////////////////////////////
// 04/23/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

DWORD CServiceControl::GetName(
   LPSTR  lpszServiceName)              // Place to return the name
{
DWORD dwReturn = 0;

   if (NULL == lpszServiceName)
   {
      SYM_ASSERT(FALSE);
   }
   else
   {
                                        // Convert the string.

    dwReturn = WideCharToMultiByte (
                  CP_ACP,
                  WC_COMPOSITECHECK,
                  m_szServiceName,
                  -1,
                  lpszServiceName,
                  sizeof (*lpszServiceName) * MAX_SERVICENAME_SIZE,
                  NULL,
                  NULL );
   }

   return dwReturn;
}


#ifdef UNICODE
/////////////////////////////////////////////////////////////////////
// CServiceControl : ChangeServiceConfigA
//
// This function changes the configuration of the specified service.
// If UNICODE is defined the regular ChangeServiceConfig will get
// defined as ChangeServiceConfigW.  Since this is the case, we need to
// make ChangeServiceConfigA manually.
//
// Parameters:
//    DWORD   dwServiceType = SERVICE_NO_CHANGE,  // type of service
//    DWORD   dwStartType = SERVICE_NO_CHANGE,    // when to start service
//    DWORD   dwErrorControl = SERVICE_NO_CHANGE, // severity if service fails to start
//    LPCSTR  lpBinaryPathName = NULL,            // pointer to service binary file name
//    LPCSTR  lpLoadOrderGroup = NULL,            // pointer to load ordering group name
//    LPDWORD lpdwTagId = NULL,                   // pointer to variable to get tag identifier
//    LPCSTR  lpDependencies = NULL,              // pointer to array of dependency names
//    LPCSTR  lpServiceStartName = NULL,          // pointer to account name of service
//    LPCSTR  lpPassword = NULL,                  // pointer to password for service account
//    LPCSTR  lpDisplayName = NULL )              // pointer to display name
//
// Return Value:
//    ERROR_SUCCESS                 - Successful.
//    ERROR_SVCCTRL_CHANGESTARTTYPE - Fail
/////////////////////////////////////////////////////////////////////
// 06/14/96 RPULINT - Function Created
// 07/01/96 DSACKIN -
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::ChangeServiceConfigA (
   DWORD   dwServiceType,               // type of service
   DWORD   dwStartType,                 // when to start service
   DWORD   dwErrorControl,              // severity if service fails to start
   LPCSTR  lpBinaryPathName,            // pointer to service binary file name
   LPCSTR  lpLoadOrderGroup,            // pointer to load ordering group name
   LPDWORD lpdwTagId,                   // pointer to variable to get tag identifier
   LPCSTR  lpDependencies,              // pointer to array of dependency names
   LPCSTR  lpServiceStartName,          // pointer to account name of service
   LPCSTR  lpPassword,                  // pointer to password for service account
   LPCSTR  lpDisplayName)               // pointer to display name
{
DWORD   dwReturn = ERROR_SUCCESS;

   SYM_ASSERT (m_hService);

                                        // For now only support non-unicode
                                        // Pass through to our service.
   if ( !::ChangeServiceConfigA (
              m_hService,
              dwServiceType,
              dwStartType,
              dwErrorControl,
              lpBinaryPathName,
              lpLoadOrderGroup,
              lpdwTagId,
              lpDependencies,
              lpServiceStartName,
              lpPassword,
              lpDisplayName ) )
   {
      dwReturn = ERROR_SVCCTRL_CHANGECONFIG;
   }

    return ( dwReturn );
}

#else // ifdef UNICODE
/////////////////////////////////////////////////////////////////////
// CServiceControl : ChangeServiceConfigW
//
// This function changes the configuration of the specified service.
// If UNICODE is not defined the regular ChangeServiceConfig will get
// defined as ChangeServiceConfigA.  Since this is the case, we need to
// make ChangeServiceConfigW manually.
//
// Parameters:
//    DWORD   dwServiceType = SERVICE_NO_CHANGE,  // type of service
//    DWORD   dwStartType = SERVICE_NO_CHANGE,    // when to start service
//    DWORD   dwErrorControl = SERVICE_NO_CHANGE, // severity if service fails to start
//    LPCWSTR lpBinaryPathName = NULL,            // pointer to service binary file name
//    LPCWSTR lpLoadOrderGroup = NULL,            // pointer to load ordering group name
//    LPDWORD lpdwTagId = NULL,                   // pointer to variable to get tag identifier
//    LPCWSTR lpDependencies = NULL,              // pointer to array of dependency names
//    LPCWSTR lpServiceStartName = NULL,          // pointer to account name of service
//    LPCWSTR lpPassword = NULL,                  // pointer to password for service account
//    LPCWSTR lpDisplayName = NULL )              // pointer to display name
//
// Return Value:
//    ERROR_SUCCESS                 - Successful.
//    ERROR_SVCCTRL_CHANGESTARTTYPE - Fail
/////////////////////////////////////////////////////////////////////
// 06/14/96 RPULINT - Function Created
// 07/01/96 DSACKIN -
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::ChangeServiceConfigW (
   DWORD   dwServiceType,               // type of service
   DWORD   dwStartType,                 // when to start service
   DWORD   dwErrorControl,              // severity if service fails to start
   LPCWSTR lpBinaryPathName,            // pointer to service binary file name
   LPCWSTR lpLoadOrderGroup,            // pointer to load ordering group name
   LPDWORD lpdwTagId,                   // pointer to variable to get tag identifier
   LPCWSTR lpDependencies,              // pointer to array of dependency names
   LPCWSTR lpServiceStartName,          // pointer to account name of service
   LPCWSTR lpPassword,                  // pointer to password for service account
   LPCWSTR lpDisplayName)               // pointer to display name
{
DWORD   dwReturn = ERROR_SUCCESS;

   SYM_ASSERT (m_hService);

                                        // For now only support non-unicode
                                        // Pass through to our service.
   if ( !::ChangeServiceConfigW (
              m_hService,
              dwServiceType,
              dwStartType,
              dwErrorControl,
              lpBinaryPathName,
              lpLoadOrderGroup,
              lpdwTagId,
              lpDependencies,
              lpServiceStartName,
              lpPassword,
              lpDisplayName ) )
   {
      dwReturn = ERROR_SVCCTRL_CHANGECONFIG;
   }

    return ( dwReturn );
}

#endif // ifdef UNICODE


/////////////////////////////////////////////////////////////////////
// CServiceControl : ChangeServiceConfig
//
// This function changes the configuration of the specified service.
// This function will actually be named either ChangeServiceConfigW
// or ChangeServiceConfigA based on if UNICODE is defined or not.  This
// happens because ChangeServiceConfig is actually #define'd in the
// Win32 API headers to be one or the other.
//
// Parameters:
//    DWORD   dwServiceType = SERVICE_NO_CHANGE,  // type of service
//    DWORD   dwStartType = SERVICE_NO_CHANGE,    // when to start service
//    DWORD   dwErrorControl = SERVICE_NO_CHANGE, // severity if service fails to start
//    LPCTSTR lpBinaryPathName = NULL,            // pointer to service binary file name
//    LPCTSTR lpLoadOrderGroup = NULL,            // pointer to load ordering group name
//    LPDWORD lpdwTagId = NULL,                   // pointer to variable to get tag identifier
//    LPCTSTR lpDependencies = NULL,              // pointer to array of dependency names
//    LPCTSTR lpServiceStartName = NULL,          // pointer to account name of service
//    LPCTSTR lpPassword = NULL,                  // pointer to password for service account
//    LPCTSTR lpDisplayName = NULL )              // pointer to display name
//
// Return Value:
//    ERROR_SUCCESS                 - Successful.
//    ERROR_SVCCTRL_CHANGESTARTTYPE - Fail
/////////////////////////////////////////////////////////////////////
// 06/14/96 RPULINT - Function Created
// 07/01/96 DSACKIN -
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::ChangeServiceConfig (
   DWORD   dwServiceType,               // type of service
   DWORD   dwStartType,                 // when to start service
   DWORD   dwErrorControl,              // severity if service fails to start
   LPCTSTR lpBinaryPathName,            // pointer to service binary file name
   LPCTSTR lpLoadOrderGroup,            // pointer to load ordering group name
   LPDWORD lpdwTagId,                   // pointer to variable to get tag identifier
   LPCTSTR lpDependencies,              // pointer to array of dependency names
   LPCTSTR lpServiceStartName,          // pointer to account name of service
   LPCTSTR lpPassword,                  // pointer to password for service account
   LPCTSTR lpDisplayName)               // pointer to display name
{
DWORD   dwReturn = ERROR_SUCCESS;

   SYM_ASSERT (m_hService);

                                        // Pass through to our service.
   if ( !::ChangeServiceConfig (
            m_hService,
            dwServiceType,
            dwStartType,
            dwErrorControl,
            lpBinaryPathName,
            lpLoadOrderGroup,
            lpdwTagId,
            lpDependencies,
            lpServiceStartName,
            lpPassword,
            lpDisplayName ) )
   {
      dwReturn = ERROR_SVCCTRL_CHANGECONFIG;
   }

    return ( dwReturn );
}

/////////////////////////////////////////////////////////////////////
// CServiceControl::ChangeServiceConfig_StartType
//
// This function changes the start type of the specified service.
//
// Parameters:
//    DWORD dwStartType = SERVICE_AUTO_START,	SERVICE_DEMAND_START,
//                        SERVICE_BOOT_START,SERVICE_SYSTEM_START, SERVICE_DISABLED
//
// Return Value:
//    ERROR_SUCCESS                 - Successful.
//    ERROR_SVCCTRL_CHANGESTARTTYPE - Fail
/////////////////////////////////////////////////////////////////////
// 06/14/96 RPULINT - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::ChangeServiceConfig_StartType(
    DWORD dwStartType )
{
DWORD dwReturn;

                                        // We must leave all parameters
                                        // in tact down to the place where
                                        // we can specify UNICODE/NONUNICODE
                                        // version of ChangeServiceConfig or
                                        // the compiler will complain about us
                                        // being ambiguous.
                                        // We will end up using whatever the
                                        // end application is using.

   dwReturn = ChangeServiceConfig (
                 SERVICE_NO_CHANGE,     // Don't change Service Type!
                 dwStartType,           // Only the start type
                 SERVICE_NO_CHANGE,
                 (LPCTSTR) NULL );

   return dwReturn;
}

#ifdef UNICODE
/////////////////////////////////////////////////////////////////////
// CServiceControl : CreateServiceA
//
// This function creates the service in the service control manager.
// If UNICODE is defined the regular CreateService will get
// defined as CreateServiceW.  Since this is the case, we need to
// make CreateServiceA manually.
//
// Parameters:
//    LPCSTR  lpszServiceName,          // name of service to create
//    LPCSTR  lpszDisplayName,          // pointer to display name
//    DWORD   dwDesiredAccess,          // type of access to service
//    DWORD   dwServiceType,            // type of service
//    DWORD   dwStartType,              // when to start service
//    DWORD   dwErrorControl,           // severity if service fails to start
//    LPCSTR  lpszBinaryPathName,       // pointer to name of binary fail
//    LPCSTR  lpszLoadOrderGroup = NULL,// pointer to name of load ordering group
//    LPDWORD lpdwTagId = NULL,         // pointer to variable to get tag identifier
//    LPCSTR  lpszDependencies = NULL,  // pointer to array of dependency names
//    LPCSTR  lpszServiceStartName = NULL, // pointer to account name of service
//    LPCSTR  lpszPassword = NULL);     // pointer to password for service account
//
// Return Value:
//    ERROR_SUCCESS                 - Successful.
//    ERROR_SVCCTRL_CHANGESTARTTYPE - Fail
/////////////////////////////////////////////////////////////////////
// 07/29/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::CreateServiceA (
   LPCSTR  lpszServiceName,             // name of service to create
   LPCSTR  lpszDisplayName,             // pointer to display name
   DWORD   dwDesiredAccess,             // type of access to service
   DWORD   dwServiceType,               // type of service
   DWORD   dwStartType,                 // when to start service
   DWORD   dwErrorControl,              // severity if service fails to start
   LPCSTR  lpszBinaryPathName,          // pointer to name of binary fail
   LPCSTR  lpszLoadOrderGroup,          // pointer to name of load ordering group
   LPDWORD lpdwTagId,                   // pointer to variable to get tag identifier
   LPCSTR  lpszDependencies,            // pointer to array of dependency names
   LPCSTR  lpszServiceStartName,        // pointer to account name of service
   LPCSTR  lpszPassword)                // pointer to password for service account
{
DWORD     dwReturn = ERROR_SUCCESS;
int       nLen;

   if (NULL == m_hSCMgr)
   {
                                        // return that they must call Open
                                        // prior to accessing this function
      dwReturn = ERROR_SVCCTRL_OPEN_SCMGR;

      goto Cleanup;
   }

   SYM_ASSERT (lpszServiceName);

                                        // Take a copy of the service name
                                        // we must convert because this is
                                        // a non-unicode function
   nLen = MultiByteToWideChar (
             CP_ACP,
             MB_PRECOMPOSED,
             lpszServiceName,
             -1,
             m_szServiceName,
             MAX_SERVICENAME_SIZE);

   if (0 == nLen)
   {
      dwReturn = ERROR_SVCCTRL_STRINGCONVERSION;

      goto Cleanup;
   }

   m_hService = ::CreateServiceA (
                   m_hSCMgr,
                   lpszServiceName,
                   lpszDisplayName,
                   dwDesiredAccess,
                   dwServiceType,
                   dwStartType,
                   dwErrorControl,
                   lpszBinaryPathName,
                   lpszLoadOrderGroup,
                   lpdwTagId,
                   lpszDependencies,
                   lpszServiceStartName,
                   lpszPassword);

   if (NULL == m_hService)
   {
                                        // If it didn't work, report the error,
                                        // but don't clean up.  This will happen
                                        // in our destructor.
      dwReturn = ERROR_SVCCTRL_SERVICECREATE;
   }

Cleanup:

   return ( dwReturn );
}


#else // ifdef UNICODE

/////////////////////////////////////////////////////////////////////
// CServiceControl : CreateServiceW
//
// This function creates the service in the service control manager.
// If UNICODE is not defined the regular CreateService will get
// defined as CreateServiceA.  Since this is the case, we need to
// make CreateServiceW manually.
//
// Parameters:
//    LPCWSTR lpszServiceName,          // name of service to create
//    LPCWSTR lpszDisplayName,          // pointer to display name
//    DWORD   dwDesiredAccess,          // type of access to service
//    DWORD   dwServiceType,            // type of service
//    DWORD   dwStartType,              // when to start service
//    DWORD   dwErrorControl,           // severity if service fails to start
//    LPCWSTR lpszBinaryPathName,       // pointer to name of binary fail
//    LPCWSTR lpszLoadOrderGroup = NULL,// pointer to name of load ordering group
//    LPDWORD lpdwTagId = NULL,         // pointer to variable to get tag identifier
//    LPCWSTR lpszDependencies = NULL,  // pointer to array of dependency names
//    LPCWSTR lpszServiceStartName = NULL, // pointer to account name of service
//    LPCWSTR lpszPassword = NULL);     // pointer to password for service account
//
// Return Value:
//    ERROR_SUCCESS                 - Successful.
//    ERROR_SVCCTRL_CHANGESTARTTYPE - Fail
/////////////////////////////////////////////////////////////////////
// 07/29/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::CreateServiceW (
   LPCWSTR lpszServiceName,             // name of service to create
   LPCWSTR lpszDisplayName,             // pointer to display name
   DWORD   dwDesiredAccess,             // type of access to service
   DWORD   dwServiceType,               // type of service
   DWORD   dwStartType,                 // when to start service
   DWORD   dwErrorControl,              // severity if service fails to start
   LPCWSTR lpszBinaryPathName,          // pointer to name of binary fail
   LPCWSTR lpszLoadOrderGroup,          // pointer to name of load ordering group
   LPDWORD lpdwTagId,                   // pointer to variable to get tag identifier
   LPCWSTR lpszDependencies,            // pointer to array of dependency names
   LPCWSTR lpszServiceStartName,        // pointer to account name of service
   LPCWSTR lpszPassword)                // pointer to password for service account
{
DWORD     dwReturn = ERROR_SUCCESS;

   if (NULL == m_hSCMgr)
   {
                                        // return that they must call Open
                                        // prior to accessing this function
      dwReturn = ERROR_SVCCTRL_OPEN_SCMGR;

      goto Cleanup;
   }

   SYM_ASSERT (lpszServiceName);

                                        // Take a copy of the service name
   wcscpy (m_szServiceName, lpszServiceName);

   m_hService = ::CreateServiceW (
                   m_hSCMgr,
                   lpszServiceName,
                   lpszDisplayName,
                   dwDesiredAccess,
                   dwServiceType,
                   dwStartType,
                   dwErrorControl,
                   lpszBinaryPathName,
                   lpszLoadOrderGroup,
                   lpdwTagId,
                   lpszDependencies,
                   lpszServiceStartName,
                   lpszPassword);

   if (NULL == m_hService)
   {
                                        // If it didn't work, report the error,
                                        // but don't clean up.  This will happen
                                        // in our destructor.
      dwReturn = ERROR_SVCCTRL_SERVICECREATE;
   }

Cleanup:

   return ( dwReturn );
}

#endif // ifdef UNICODE

/////////////////////////////////////////////////////////////////////
// CServiceControl : CreateService
//
// This function creates the service in the service control manager.
// If UNICODE is defined this version will be named CreateServiceW.
// If UNICODE is NOT defined, this version will be named CreateServiceA.
// Appropriate string handling must be used in both cases.
//
// Parameters:
//    LPCTSTR lpszServiceName,          // name of service to create
//    LPCTSTR lpszDisplayName,          // pointer to display name
//    DWORD   dwDesiredAccess,          // type of access to service
//    DWORD   dwServiceType,            // type of service
//    DWORD   dwStartType,              // when to start service
//    DWORD   dwErrorControl,           // severity if service fails to start
//    LPCTSTR lpszBinaryPathName,       // pointer to name of binary fail
//    LPCTSTR lpszLoadOrderGroup = NULL,// pointer to name of load ordering group
//    LPDWORD lpdwTagId = NULL,         // pointer to variable to get tag identifier
//    LPCTSTR lpszDependencies = NULL,  // pointer to array of dependency names
//    LPCTSTR lpszServiceStartName = NULL, // pointer to account name of service
//    LPCTSTR lpszPassword = NULL);     // pointer to password for service account
//
// Return Value:
//    ERROR_SUCCESS                 - Successful.
//    ERROR_SVCCTRL_CHANGESTARTTYPE - Fail
/////////////////////////////////////////////////////////////////////
// 07/29/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CServiceControl::CreateService (
   LPCTSTR lpszServiceName,             // name of service to create
   LPCTSTR lpszDisplayName,             // pointer to display name
   DWORD   dwDesiredAccess,             // type of access to service
   DWORD   dwServiceType,               // type of service
   DWORD   dwStartType,                 // when to start service
   DWORD   dwErrorControl,              // severity if service fails to start
   LPCTSTR lpszBinaryPathName,          // pointer to name of binary fail
   LPCTSTR lpszLoadOrderGroup,          // pointer to name of load ordering group
   LPDWORD lpdwTagId,                   // pointer to variable to get tag identifier
   LPCTSTR lpszDependencies,            // pointer to array of dependency names
   LPCTSTR lpszServiceStartName,        // pointer to account name of service
   LPCTSTR lpszPassword)                // pointer to password for service account
{
DWORD     dwReturn = ERROR_SUCCESS;

#if !defined(UNICODE)
int       nLen;                         // variable only used in NON-UNICODE
#endif // ifdef UNICODE

   if (NULL == m_hSCMgr)
   {
                                        // return that they must call Open
                                        // prior to accessing this function
      dwReturn = ERROR_SVCCTRL_OPEN_SCMGR;

      goto Cleanup;
   }

   SYM_ASSERT (lpszServiceName);

#ifdef UNICODE
   wcscpy (m_szServiceName, lpszServiceName);
#else // ifdef UNICODE
                                        // Take a copy of the service name
                                        // we must convert because this is
                                        // a non-unicode function
   nLen = MultiByteToWideChar (
             CP_ACP,
             MB_PRECOMPOSED,
             lpszServiceName,
             -1,
             m_szServiceName,
             MAX_SERVICENAME_SIZE);

   if (0 == nLen)
   {
      dwReturn = ERROR_SVCCTRL_STRINGCONVERSION;

      goto Cleanup;
   }

#endif // ifdef UNICODE

   m_hService = ::CreateService (
                   m_hSCMgr,
                   lpszServiceName,
                   lpszDisplayName,
                   dwDesiredAccess,
                   dwServiceType,
                   dwStartType,
                   dwErrorControl,
                   lpszBinaryPathName,
                   lpszLoadOrderGroup,
                   lpdwTagId,
                   lpszDependencies,
                   lpszServiceStartName,
                   lpszPassword);

   if (NULL == m_hService)
   {
                                        // If it didn't work, report the error,
                                        // but don't clean up.  This will happen
                                        // in our destructor.
      dwReturn = ERROR_SVCCTRL_SERVICECREATE;
   }

Cleanup:

   return ( dwReturn );
}


/////////////////////////////////////////////////////////////////////
// CServiceControl : Delete
//
// This function will delete the specified service.  The service must
// have a valid handle to it within the class.
//
// Parameters:
//    none.
//
// Return Value:
//    DWORD - ERROR_SUCCESS - Service Marked for deletion
//
/////////////////////////////////////////////////////////////////////
// 07/29/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CServiceControl::Delete(VOID)
{
DWORD dwReturn = ERROR_SUCCESS;
BOOL  bReturn;

   if (NULL == m_hService)
   {
      dwReturn = ERROR_SVCCTRL_INVALIDSERVICE;
      goto Cleanup;
   }

                                        // Try the delete
   bReturn = DeleteService(
                m_hService);

   if (!bReturn)
   {
                                        // Something happened.  Lets see if
                                        // it was already marked for deletion
      if (ERROR_SERVICE_MARKED_FOR_DELETE != GetLastError())
         dwReturn = ERROR_SVCCTRL_DELETESERVICE;
   }

Cleanup:

   return dwReturn;
}


