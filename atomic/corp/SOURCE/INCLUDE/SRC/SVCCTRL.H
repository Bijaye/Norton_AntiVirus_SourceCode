// Copyright 1996-1997 Symantec Corporation
//***************************************************************************
// 
// $Header:   S:/INCLUDE/VCS/svcctrl.h_v   1.1   24 Apr 1997 20:05:12   JBRENNA  $ 
// 
// Description:
//   This header file will define the interface for the CServiceControl
//   object that will be used to start, stop and control services.
// 
//*************************************************************************** 
// $Log:   S:/INCLUDE/VCS/svcctrl.h_v  $ 
// 
//    Rev 1.1   24 Apr 1997 20:05:12   JBRENNA
// Tesla Port: DSACKIN r1.11:
// Removed obsolete #ifdef DEBUG_PROFILE block delimeters.
// 
//*************************************************************************** 

#if !defined(__SVCCTRL_H)
#define __SVCCTRL_H

#if !defined(__PROFILER_H)
#include "profiler.h"
#endif // if !defined(__PROFILER_H)

#if !defined(__TIMERCLI_H)
#  include "timercli.h"
#endif // if !defined(__TIMERCLI_H)


#define MAX_SERVICENAME_SIZE 256

#define ERROR_SVCCTRL_OPEN_SCMGR        512
#define ERROR_SVCCTRL_CREATEFILE        513
#define ERROR_SVCCTRL_SERVICEOPEN       514
#define ERROR_SVCCTRL_STOPSERVICE       515
#define ERROR_SVCCTRL_INVALIDSERVICE    516
#define ERROR_SVCCTRL_STARTSERVICE      517
#define ERROR_SVCCTRL_SERVICESTATUS     518
#define ERROR_SVCCTRL_FAILEDSTATE       519
#define ERROR_SVCCTRL_STRINGCONVERSION  520
#define ERROR_SVCCTRL_ALLOCATE          521
#define ERROR_SVCCTRL_CHANGECONFIG      522
#define ERROR_SVCCTRL_QUERYCONFIG       523
#define ERROR_SVCCTRL_INVALIDARGUMENT   524
#define ERROR_SVCCTRL_SERVICECREATE     525
#define ERROR_SVCCTRL_DELETESERVICE     526

class CServiceControl
{
private:
   SC_HANDLE  m_hSCMgr;                 // Handle to the ServiceControl Manager
   SC_HANDLE  m_hService;               // Handle to our actual service
   HANDLE     m_hDevice;                // Use this to keep usage count

   BOOL       m_bStarted;               // Is the function Started?
   BOOL       m_bLocked;

   WCHAR      m_szServiceName[MAX_SERVICENAME_SIZE + 1];
   WCHAR      m_szMachineName[MAX_SERVICENAME_SIZE + 1];

   LPQUERY_SERVICE_CONFIG m_lpConfig;   // Use this for querying service configuration

   WCHAR      m_szLogName[MAX_SERVICENAME_SIZE + 1];
   CTimerClient m_cTimer;

public:

   CServiceControl();
   ~CServiceControl();

   DWORD Open (
      PCSTR lpszServiceName = NULL,     // [in] - Service Name (non-unicode)
      PCSTR lpszMachineName = NULL);    // [in] - Machine where service is (non-unicode)

   DWORD Open (
      PCWSTR lpszServiceName = NULL,    // [in] - Service Name
      PCWSTR lpszMachineName = NULL);   // [in] - Machine where service is

   DWORD Start (
      BOOL  bLock = FALSE);             // [in] - Lock the Service?

   BOOL ControlService(
      DWORD            dwControlCode,            // What control code to send
      LPSERVICE_STATUS lpServiceStatus = NULL);  // Return the status?

   DWORD Stop  (VOID);
   VOID Close (VOID);                   // Cleans up member variables.

   DWORD  GetName (                     // returns number of bytes copied
      LPSTR   lpszServiceName);         // [out] - Name of the service.

   DWORD  GetName (                     // returns number of bytes copied
      LPWSTR  lpszServiceName);         // [out] - Name of the service.

   DWORD WaitForState(
      DWORD   dwState);                 // [in] - State to wait for.

   BOOL QueryServiceStatus (
      LPSERVICE_STATUS lpServiceStatus);   // [out] - Return the status struct

   BOOL QueryServiceStatus (
      LPDWORD  lpdwStatus);             // [out] - Current Status


   // The following 2 functions are declared in the Win32 API to have a
   // 'A' version and a 'W' version of the functions.  If either
   // CreateService or ChangeServiceConfig are used, the appropriate 'A'
   // or 'W' will automatically be added to the end of the function name.
   // because of this, If one will be automatically defined, I am manually
   // defining the other one for our use in UNICODE apps that call non-UNICODE
   // functions or non-UNICODE apps that call UNICODE functions.

                                        // If NULL or SERVICE_NO_CHANGE are
                                        // specified, that part of the config
                                        // is not going to be changed.

#ifdef UNICODE
                                        // declare a non-UNICODE version
                                        // if UNICODE is defined
   DWORD ChangeServiceConfigA (
      DWORD   dwServiceType = SERVICE_NO_CHANGE, // [in] type of service
      DWORD   dwStartType = SERVICE_NO_CHANGE,   // [in] when to start service
      DWORD   dwErrorControl = SERVICE_NO_CHANGE,// [in] severity if service fails to start
      LPCSTR lpBinaryPathName = NULL,            // [in] pointer to service binary file name
      LPCSTR lpLoadOrderGroup = NULL,            // [in] pointer to load ordering group name
      LPDWORD lpdwTagId = NULL,                  // [out] pointer to variable to get tag identifier
      LPCSTR lpDependencies = NULL,              // [in] pointer to array of dependency names
      LPCSTR lpServiceStartName = NULL,          // [in] pointer to account name of service
      LPCSTR lpPassword = NULL,                  // [in] pointer to password for service account
      LPCSTR lpDisplayName = NULL ) ;            // [in] pointer to display name

#else // ifdef UNICODE

                                        // declare a UNICODE version
                                        // if UNICODE is NOT defined
   DWORD ChangeServiceConfigW (
      DWORD   dwServiceType = SERVICE_NO_CHANGE, // [in] type of service
      DWORD   dwStartType = SERVICE_NO_CHANGE,   // [in] when to start service
      DWORD   dwErrorControl = SERVICE_NO_CHANGE,// [in] severity if service fails to start
      LPCWSTR lpBinaryPathName = NULL,           // [in] pointer to service binary file name
      LPCWSTR lpLoadOrderGroup = NULL,           // [in] pointer to load ordering group name
      LPDWORD lpdwTagId = NULL,                  // [out] pointer to variable to get tag identifier
      LPCWSTR lpDependencies = NULL,             // [in] pointer to array of dependency names
      LPCWSTR lpServiceStartName = NULL,         // [in] pointer to account name of service
      LPCWSTR lpPassword = NULL,                 // [in] pointer to password for service account
      LPCWSTR lpDisplayName = NULL ) ;           // [in] pointer to display name

#endif // ifdef UNICODE
                                        // gets defined as either _W or _A
                                        // based on UNICODE
   DWORD ChangeServiceConfig (
      DWORD dwServiceType = SERVICE_NO_CHANGE,   // [in] type of service
      DWORD dwStartType = SERVICE_NO_CHANGE,     // [in] when to start service
      DWORD dwErrorControl = SERVICE_NO_CHANGE,  // [in] severity if service fails to start
      LPCTSTR lpBinaryPathName = NULL,           // [in] pointer to service binary file name
      LPCTSTR lpLoadOrderGroup = NULL,           // [in] pointer to load ordering group name
      LPDWORD lpdwTagId = NULL,                  // [out] pointer to variable to get tag identifier
      LPCTSTR lpDependencies = NULL,             // [in] pointer to array of dependency names
      LPCTSTR lpServiceStartName = NULL,         // [in] pointer to account name of service
      LPCTSTR lpPassword = NULL,                 // [in] pointer to password for service account
      LPCTSTR lpDisplayName = NULL ) ;           // [in] pointer to display name

#ifdef UNICODE
                                        // declare a non-UNICODE version
                                        // if UNICODE is defined
   DWORD CreateServiceA (
      LPCSTR lpszServiceName,              // [in] name of service to create
      LPCSTR lpszDisplayName,              // [in] pointer to display name
      DWORD   dwDesiredAccess,             // [in] type of access to service
      DWORD   dwServiceType,               // [in] type of service
      DWORD   dwStartType,                 // [in] when to start service
      DWORD   dwErrorControl,              // [in] severity if service fails to start
      LPCSTR lpszBinaryPathName,           // [in] pointer to name of binary fail
      LPCSTR lpszLoadOrderGroup = NULL,    // [in] pointer to name of load ordering group
      LPDWORD lpdwTagId = NULL,            // [out] pointer to variable to get tag identifier
      LPCSTR lpszDependencies = NULL,      // [in] pointer to array of dependency names
      LPCSTR lpszServiceStartName = NULL,  // [in] pointer to account name of service
      LPCSTR lpszPassword = NULL);         // [in] pointer to password for service account

#else // ifdef UNICODE

                                        // declare a UNICODE version
                                        // if UNICODE is NOT defined
   DWORD CreateServiceW (
      LPCWSTR lpszServiceName,             // [in] name of service to create
      LPCWSTR lpszDisplayName,             // [in] pointer to display name
      DWORD   dwDesiredAccess,             // [in] type of access to service
      DWORD   dwServiceType,               // [in] type of service
      DWORD   dwStartType,                 // [in] when to start service
      DWORD   dwErrorControl,              // [in] severity if service fails to start
      LPCWSTR lpszBinaryPathName,          // [in] pointer to name of binary fail
      LPCWSTR lpszLoadOrderGroup = NULL,   // [in] pointer to name of load ordering group
      LPDWORD lpdwTagId = NULL,            // [out] pointer to variable to get tag identifier
      LPCWSTR lpszDependencies = NULL,     // [in] pointer to array of dependency names
      LPCWSTR lpszServiceStartName = NULL, // [in] pointer to account name of service
      LPCWSTR lpszPassword = NULL);        // [in] pointer to password for service account

#endif // ifdef UNICODE
                                        // gets defined as either _W or _A
                                        // based on UNICODE
   DWORD CreateService (
      LPCTSTR lpszServiceName,             // [in] name of service to create
      LPCTSTR lpszDisplayName,             // [in] pointer to display name
      DWORD   dwDesiredAccess,             // [in] type of access to service
      DWORD   dwServiceType,               // [in] type of service
      DWORD   dwStartType,                 // [in] when to start service
      DWORD   dwErrorControl,              // [in] severity if service fails to start
      LPCTSTR lpszBinaryPathName,          // [in] pointer to name of binary fail
      LPCTSTR lpszLoadOrderGroup = NULL,   // [in] pointer to name of load ordering group
      LPDWORD lpdwTagId = NULL,            // [out] pointer to variable to get tag identifier
      LPCTSTR lpszDependencies = NULL,     // [in] pointer to array of dependency names
      LPCTSTR lpszServiceStartName = NULL, // [in] pointer to account name of service
      LPCTSTR lpszPassword = NULL);        // [in] pointer to password for service account

   BOOL  Delete(VOID);                  // Use this to delete a service.

   DWORD ChangeServiceConfig_StartType(
      DWORD dwStartType);               // Change service start type

private:
   DWORD Initialize(VOID);              // Used for internal initialization

                                        // These are the profiling functions
   VOID InitializeProfiler(
      LPTSTR lpszLogName);              // [in] - The name of the service

   void LogMessage(
      LPTSTR szMessage);                // [in] - Message to send

   void LogMessage(
      LPTSTR szMessage,                 // [in] - Message to send
      LPTSTR szString);                 // [in] - Support string for message

   void LogMessage(
      LPTSTR szMessage,                 // [in] - Message to send
      DWORD  dwValue);                  // [in] - Support value for message

};

#endif // __SVCCTRL_H
