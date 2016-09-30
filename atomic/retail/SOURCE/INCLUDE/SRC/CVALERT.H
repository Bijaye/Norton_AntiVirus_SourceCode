/////////////////////////////////////////////////////////////////////
// CVALERT.H - Created - 05-14-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/cvalert.h_v   1.7   01 Apr 1998 18:06:34   DALLEE  $
//
// Description:
//    This is where our CVirusAlert class is going to be declared.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/cvalert.h_v  $
// 
//    Rev 1.7   01 Apr 1998 18:06:34   DALLEE
// Added ACTION_QUARANTINE action flag.
// 
//    Rev 1.6   01 Jul 1997 12:21:32   ksackin
// Added a prototype for the operator "=".  This would not compile for DEC alpha without it.  It would complain about a 
// body being already defined for the function.
//
//    Rev 1.5   18 Jun 1997 18:55:50   DSACKIN
// GetUserName is a Win32 API call, so we need to mangle it into GetUserNameA and
// GetUserNameW function calls.
//
//    Rev 1.4   18 May 1997 05:00:58   DSACKIN
// Fixed a prototype
//
//    Rev 1.3   18 May 1997 00:33:36   DSACKIN
// Added string length functions.
//
//    Rev 1.2   16 May 1997 16:15:18   DSACKIN
// Added AlertType definitions
// Added Action flags.
//
//    Rev 1.1   16 May 1997 15:21:28   DSACKIN
// Added the AlertType functions.
//
//    Rev 1.0   16 May 1997 13:29:46   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__CVALERT_H)
#define __CVALERT_H


#if !defined(__CALERT_H)
#   include "CAlert.h"
#endif // #if !defined(__CALERT_H)


///////////////////////////////////////////////////////////////////////////////
// Packet type definitions.
//
// Only one defined now but different types could be designed for subsequent
// versions to enable different kinds of packets to be sent to the alert
// service. This could facilitate testing, diagnostics, configuration, etc.

#define PACKET_TYPE_VIRUS_PACKET 0x00000002


///////////////////////////////////////////////////////////////////////////////
// 'AlertType' definitions

#define FILE_KNOWN_VIRUS          0x00000001
#define FILE_UNKNOWN_VIRUS        0x00000002
#define BOOT_RECORD_KNOWN_VIRUS   0x00000003
#define BOOT_RECORD_UNKNOWN_VIRUS 0x00000004
#define MBR_KNOWN_VIRUS           0x00000005
#define MBR_UNKNOWN_VIRUS         0x00000006


///////////////////////////////////////////////////////////////////////////////
// 'Action' flags

#define ACTION_NONE               0x00000001
#define ACTION_MOVE               0x00000002
#define ACTION_RENAME             0x00000004
#define ACTION_REPAIR             0x00000008
#define ACTION_DELETE             0x00000010
#define ACTION_EXCLUDE            0x00000020
#define ACTION_SHUTDOWN           0x00000040
#define ACTION_INOCULATE          0x00000080
#define ACTION_DENY_ACCESS        0x00000100
#define ACTION_UNDEFINED          0x00000200
#define ACTION_QUARANTINE         0x00000400

///////////////////////////////////////////////////////////////////////////////
// CVirusAlert class
//
// The CVirusAlert class is derived from the CAlertClass, and will incorporate
// functions to build a packet that will be specifically for virus alert
// notifications.
//


class CVirusAlert : public CAlert
{
private:
/////////////////////////////////////////////////////////////////////
// Here is where we decide what buffers get what

   enum BUFFER_NUMBERS
   {
      STR_SYSTEM_NAME,
      STR_USER_NAME,
      STR_OBJECT_NAME,
      STR_VIRUS_NAME,
      STR_DOMAIN_NAME
   };

   enum DWORD_VALUE_NUMBERS
   {
      DWORD_ACTION,
      DWORD_ALERT_TYPE
   };

   enum BOOL_VALUE_NUMBERS
   {
      BOOL_ACTION_SUCCESS
   };

public:

/////////////////////////////////////////////////////////////////////
// Class management & minipulation functions

                                        // Default constructor
   CVirusAlert();

                                        // Construction with an external buffer
   CVirusAlert(
      LPBYTE lpBuffer);                 // [in] - Buffer to assign

                                        // Copy constructor
   CVirusAlert(
      CVirusAlert & lpVirusAlert);

                                        // Default destructor
   ~CVirusAlert();

   CVirusAlert& operator =(
      CVirusAlert& pRightHandObject);   // [in] - Object to copy

/////////////////////////////////////////////////////////////////////
// String Access Functions

   BOOL SetSystemName(
      LPTSTR lpszSystemName);           // [in] - SystemName to set

   BOOL SetUserName(
      LPTSTR lpszUserName);             // [in] - UserName to set

   BOOL SetObjectName(
      LPTSTR lpszObjectName);           // [in] - ObjectName to set

   BOOL SetVirusName(
      LPTSTR lpszVirusName);            // [in] - VirusName to set

   BOOL SetDomainName(
      LPTSTR lpszDomainName);           // [in] - DomainName to set


   BOOL GetSystemName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPTSTR lpszSystemName);           // [out] - SystemName returned.

   BOOL GetUserName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPTSTR lpszUserName);             // [out] - UserName returned.

   BOOL GetObjectName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPTSTR lpszObjectName);           // [out] - ObjectName returned.

   BOOL GetVirusName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPTSTR lpszVirusName);            // [out] - VirusName returned.

   BOOL GetDomainName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPTSTR lpszDomainName);           // [out] - DomainName returned.


#ifdef UNICODE

/////////////////////////////////////////////////////////////////////
// If UNICODE is defined, the above functions are for the UNICODE
// definitions, and here we must define the NON-UNICODE versions.

   BOOL SetSystemName(
      LPSTR lpszSystemName);            // [in] - SystemName to set

   BOOL SetUserName(
      LPSTR lpszUserName);              // [in] - UserName to set

   BOOL SetObjectName(
      LPSTR lpszObjectName);            // [in] - ObjectName to set

   BOOL SetVirusName(
      LPSTR lpszVirusName);             // [in] - VirusName to set

   BOOL SetDomainName(
      LPSTR lpszDomainName);            // [in] - DomainName to set


   BOOL GetSystemName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPSTR lpszSystemName);            // [out] - SystemName returned.

   BOOL GetUserNameA(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPSTR lpszUserName);              // [out] - UserName returned.

   BOOL GetObjectName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPSTR lpszObjectName);            // [out] - ObjectName returned.

   BOOL GetVirusName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPSTR lpszVirusName);             // [out] - VirusName returned.

   BOOL GetDomainName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPSTR lpszDomainName);            // [out] - DomainName returned.


#else // #ifdef UNICODE

/////////////////////////////////////////////////////////////////////
// If UNICODE was NOT defined for this module, the prototypes at the
// top become the ANSI definitions, and here we must declare the
// UNICODE versions for all of those functions

   BOOL SetSystemName(
      LPWSTR lpszSystemName);           // [in] - SystemName to set

   BOOL SetUserName(
      LPWSTR lpszUserName);             // [in] - UserName to set

   BOOL SetObjectName(
      LPWSTR lpszObjectName);           // [in] - ObjectName to set

   BOOL SetVirusName(
      LPWSTR lpszVirusName);            // [in] - VirusName to set

   BOOL SetDomainName(
      LPWSTR lpszDomainName);           // [in] - DomainName to set


   BOOL GetSystemName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPWSTR lpszSystemName);           // [out] - SystemName returned.

   BOOL GetUserNameW(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPWSTR lpszUserName);             // [out] - UserName returned.

   BOOL GetObjectName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPWSTR lpszObjectName);           // [out] - ObjectName returned.

   BOOL GetVirusName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPWSTR lpszVirusName);            // [out] - VirusName returned.

   BOOL GetDomainName(
      DWORD  dwBufferSize,              // [in]  - Size of output buffer
      LPWSTR lpszDomainName);           // [out] - DomainName returned.


#endif // #ifdef UNICODE

/////////////////////////////////////////////////////////////////////
// String Length Functions.

   BOOL GetSystemNameLength(
      DWORD & dwLength);                // [out] - SystemNameLength

   BOOL GetUserNameLength(
      DWORD & dwLength);                // [out] - UserNameLength

   BOOL GetObjectNameLength(
      DWORD & dwLength);                // [out] - ObjectNameLength

   BOOL GetVirusNameLength(
      DWORD & dwLength);                // [out] - VirusNameLength

   BOOL GetDomainNameLength(
      DWORD & dwLength);                // [out] - DomainNameLength


/////////////////////////////////////////////////////////////////////
// DWORD value functions

   BOOL SetAction(
      DWORD dwAction);                  // [in] - Action taken

   BOOL GetAction(
      DWORD & dwAction);                // [out] - Action taken

   BOOL SetAlertType(
      DWORD dwAlertType);               // [in] - Alert type to set

   BOOL GetAlertType(
      DWORD & dwAlertType);             // [out] - Alert type from packet

/////////////////////////////////////////////////////////////////////
// BOOL Value functions

   BOOL SetActionSuccess(
      BOOL bActionSuccess);             // [in] - Was the action successful

   BOOL GetActionSuccess(
      BOOL & bActionSuccess);           // [out] - Was the action successful

};

#endif // if !defined(__CVALERT_H)
