/////////////////////////////////////////////////////////////////////
// STRINGS.CPP - Created - 05-15-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CVALERT/VCS/strings.cpv   1.2   18 Jun 1997 18:55:24   DSACKIN  $
//
// Description:
//    This is where we will take care of getting the strings into
//    and out of the base CALert class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CVALERT/VCS/strings.cpv  $
// 
//    Rev 1.2   18 Jun 1997 18:55:24   DSACKIN
// GetUserName is a Win32 API call so we need to mangle the GetUserNameA and
// GetUserNameW functions.
//
//    Rev 1.1   18 May 1997 00:33:26   DSACKIN
// Added string length functions.
//
//    Rev 1.0   16 May 1997 13:30:08   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include <assert.h>
#include "CValert.h"


/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetSystemName(
//
// Purpose:
//
//
// Parameters:
//    LPSTR lpszSystemName - [in] - SystemName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetSystemName(
   LPSTR lpszSystemName)                // [in] - SystemName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_SYSTEM_NAME,
                lpszSystemName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetUserName(
//
// Purpose:
//
//
// Parameters:
//    LPSTR lpszUserName - [in] - UserName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetUserName(
   LPSTR lpszUserName)                  // [in] - UserName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_USER_NAME,
                lpszUserName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetObjectName(
//
// Purpose:
//
//
// Parameters:
//   LPSTR lpszObjectName - [in] - ObjectName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetObjectName(
   LPSTR lpszObjectName)                // [in] - ObjectName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_OBJECT_NAME,
                lpszObjectName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetVirusName(
//
// Purpose:
//
//
// Parameters:
//   LPSTR lpszVirusName - [in] - VirusName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetVirusName(
   LPSTR lpszVirusName)                 // [in] - VirusName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_VIRUS_NAME,
                lpszVirusName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetDomainName(
//
// Purpose:
//
//
// Parameters:
//    LPSTR lpszDomainName - [in] - DomainName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetDomainName(
   LPSTR lpszDomainName)                // [in] - DomainName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_DOMAIN_NAME,
                lpszDomainName);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetSystemName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize  - [in]  - Size of output buffer
//    LPSTR lpszSystemName - [out] - SystemName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetSystemName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPSTR lpszSystemName)                // [out] - SystemName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_SYSTEM_NAME,
                dwBufferSize,
                lpszSystemName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetUserNameA(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize - [in]  - Size of output buffer
//    LPSTR lpszUserName  - [out] - UserName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetUserNameA(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPSTR lpszUserName)                  // [out] - UserName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_USER_NAME,
                dwBufferSize,
                lpszUserName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetObjectName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize  - [in]  - Size of output buffer
//    LPSTR lpszObjectName - [out] - ObjectName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetObjectName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPSTR lpszObjectName)                // [out] - ObjectName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_OBJECT_NAME,
                dwBufferSize,
                lpszObjectName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetVirusName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize - [in]  - Size of output buffer
//    LPSTR lpszVirusName - [out] - VirusName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetVirusName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPSTR lpszVirusName)                 // [out] - VirusName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_VIRUS_NAME,
                dwBufferSize,
                lpszVirusName);

   return bReturn;

}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetDomainName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize  - [in]  - Size of output buffer
//    LPSTR lpszDomainName - [out] - DomainName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetDomainName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPSTR lpszDomainName)                // [out] - DomainName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_DOMAIN_NAME,
                dwBufferSize,
                lpszDomainName);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetSystemName(
//
// Purpose:
//
//
// Parameters:
//    LPWSTR lpszSystemName - [in] - SystemName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetSystemName(
   LPWSTR lpszSystemName)               // [in] - SystemName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_SYSTEM_NAME,
                lpszSystemName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetUserName(
//
// Purpose:
//
//
// Parameters:
//    LPWSTR lpszUserName - [in] - UserName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetUserName(
   LPWSTR lpszUserName)                 // [in] - UserName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_USER_NAME,
                lpszUserName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetObjectName(
//
// Purpose:
//
//
// Parameters:
//    LPWSTR lpszObjectName - [in] - ObjectName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetObjectName(
   LPWSTR lpszObjectName)               // [in] - ObjectName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_OBJECT_NAME,
                lpszObjectName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetVirusName(
//
// Purpose:
//
//
// Parameters:
//    LPWSTR lpszVirusName - [in] - VirusName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetVirusName(
   LPWSTR lpszVirusName)                // [in] - VirusName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_VIRUS_NAME,
                lpszVirusName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::SetDomainName(
//
// Purpose:
//
//
// Parameters:
//    LPWSTR lpszDomainName - [in] - DomainName to set
//
//
// Returns:
//    TRUE  - String was Set correctly.
//    FALSE - Possibly the packet was locked or the string was NULL.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::SetDomainName(
   LPWSTR lpszDomainName)               // [in] - DomainName to set
{
BOOL bReturn;

   bReturn = SetString (
                STR_DOMAIN_NAME,
                lpszDomainName);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetSystemName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize   - [in]  - Size of output buffer
//    LPWSTR lpszSystemName - [out] - SystemName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetSystemName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPWSTR lpszSystemName)               // [out] - SystemName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_SYSTEM_NAME,
                dwBufferSize,
                lpszSystemName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetUserNameW(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize - [in]  - Size of output buffer
//    LPWSTR lpszUserName - [out] - UserName returned.
//
//
// Returns:
//
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetUserNameW(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPWSTR lpszUserName)                 // [out] - UserName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_USER_NAME,
                dwBufferSize,
                lpszUserName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetObjectName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize   - [in]  - Size of output buffer
//    LPWSTR lpszObjectName - [out] - ObjectName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetObjectName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPWSTR lpszObjectName)               // [out] - ObjectName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_OBJECT_NAME,
                dwBufferSize,
                lpszObjectName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetVirusName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize  - [in]  - Size of output buffer
//    LPWSTR lpszVirusName - [out] - VirusName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetVirusName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPWSTR lpszVirusName)                // [out] - VirusName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_VIRUS_NAME,
                dwBufferSize,
                lpszVirusName);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetDomainName(
//
// Purpose:
//
//
// Parameters:
//    DWORD  dwBufferSize   - [in]  - Size of output buffer
//    LPWSTR lpszDomainName - [out] - DomainName returned.
//
//
// Returns:
//    TRUE  - String retrieved successfully.
//    FALSE - Probably not enough buffer to return the string.
//
/////////////////////////////////////////////////////////////////////
// 05-15-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetDomainName(
   DWORD  dwBufferSize,                 // [in]  - Size of output buffer
   LPWSTR lpszDomainName)               // [out] - DomainName returned.
{
BOOL bReturn;

   bReturn = GetString (
                STR_DOMAIN_NAME,
                dwBufferSize,
                lpszDomainName);

   return bReturn;
}




/////////////////////////////////////////////////////////////////////
// String Length Functions.


/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetSystemNameLength(
//
// Purpose:
//
//
// Parameters:
//    DWORD & dwLength - [out] - SystemNameLength
//
// Returns:
//    TRUE  - Got the String Length Successfully.
//    FALSE - Probably an invalid string.
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetSystemNameLength(
   DWORD & dwLength)                  // [out] - SystemNameLength
{
BOOL bReturn;

   bReturn = GetStringLength(
                STR_SYSTEM_NAME,
                dwLength);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetUserNameLength(
//
// Purpose:
//
//
// Parameters:
//    DWORD & dwLength - [out] - UserNameLength
//
// Returns:
//    TRUE  - Got the String Length Successfully.
//    FALSE - Probably an invalid string.
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetUserNameLength(
   DWORD & dwLength)                 // [out] - UserNameLength
{
BOOL bReturn;

   bReturn = GetStringLength(
                STR_USER_NAME,
                dwLength);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetObjectNameLength(
//
// Purpose:
//
//
// Parameters:
//    DWORD & dwLength - [out] - ObjectNameLength
//
//
// Returns:
//    TRUE  - Got the String Length Successfully.
//    FALSE - Probably an invalid string.
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetObjectNameLength(
   DWORD & dwLength)                 // [out] - ObjectNameLength
{
BOOL bReturn;

   bReturn = GetStringLength(
                STR_OBJECT_NAME,
                dwLength);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetVirusNameLength(
//
// Purpose:
//
//
// Parameters:
//    DWORD & dwLength - [out] - VirusNameLength
//
//
// Returns:
//    TRUE  - Got the String Length Successfully.
//    FALSE - Probably an invalid string.
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetVirusNameLength(
   DWORD & dwLength)                 // [out] - VirusNameLength
{
BOOL bReturn;

   bReturn = GetStringLength(
                STR_VIRUS_NAME,
                dwLength);

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CVirusAlert::GetDomainNameLength(
//
// Purpose:
//
//
// Parameters:
//    DWORD & dwLength - [out] - DomainNameLength
//
//
// Returns:
//    TRUE  - Got the String Length Successfully.
//    FALSE - Probably an invalid string.
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CVirusAlert::GetDomainNameLength(
   DWORD & dwLength)                 // [out] - DomainNameLength
{
BOOL bReturn;

   bReturn = GetStringLength(
                STR_DOMAIN_NAME,
                dwLength);

   return bReturn;
}



