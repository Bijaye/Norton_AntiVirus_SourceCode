/////////////////////////////////////////////////////////////////////
// STRING.CPP - Created - 05-13-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CALERT/VCS/string.cpv   1.4   11 Jun 1997 14:31:12   DSACKIN  $
//
// Description:
//    This is where our string functions will go.  This version
//    of CAlert will only handle UNICODE strings.  Any ANSI
//    strings that are sent to the ANSI functions will be converted
//    and stored in our class in UNICODE.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CALERT/VCS/string.cpv  $
// 
//    Rev 1.4   11 Jun 1997 14:31:12   DSACKIN
// Needed an extra set of parenthases to make sure that we weren't doubling
// the string length.
//
//    Rev 1.3   05 Jun 1997 18:38:28   DSACKIN
// When doing pointer arithmetic, we should always be treating m_pPacket as
// a LPBYTE instead of what it really is.
//
//    Rev 1.2   03 Jun 1997 15:29:52   DSACKIN
// Fixed a couple of minor bugs in returning success and specifying buffer length.
//
//    Rev 1.1   18 May 1997 00:17:18   DSACKIN
// Added a string length function
//
//    Rev 1.0   14 May 1997 13:40:12   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include <assert.h>
#include "calert.h"


/////////////////////////////////////////////////////////////////////
// Since we are not going to allow the strings to be stored in ANSI,
// there is no point in creating a generic TCHAR version of the
// string functions.
//
// A UNICODE version and a NON-UNICODE version of each function will
// be created, but there will be no generic "cover both cases" functions.
//

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetString(
//
// Purpose:
//    This function will store a UNICODE string into our packet.
//
// Parameters:
//   DWORD  dwStringNumber - [in] - String to set
//   LPWSTR lpszString     - [in] - Value to assign to the string
//
// Returns:
//   TRUE  - String was successfully stored in the packet.
//   FALSE - The String was not stored successfully in the packet.
//
/////////////////////////////////////////////////////////////////////
// 05-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetString(
   DWORD  dwStringNumber,               // [in] - String to set
   LPWSTR lpszString)                   // [in] - Value to assign to the string
{
BOOL  bReturn = FALSE;
DWORD dwStringLength;

                                        // Check to see if the string is OK.
   if (NULL == lpszString)
      goto Cleanup;
                                        // Calculate how much custom data is
                                        // going to go into the buffer and use
                                        // the SetBinaryData function to set
                                        // the info

   dwStringLength = (wcslen (lpszString) + 1) * sizeof(WCHAR);

                                        // Set the data!
   bReturn = SetBinaryData(
                dwStringNumber,
                dwStringLength,
                (LPBYTE) lpszString);

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// BOOL CAlert::SetString(
//
// Purpose:
//   This function will put a variable length (ANSI) string into
//   our packet.  The string will be converted and stored in
//   UNICODE.
//
// Parameters:
//   DWORD dwStringNumber - [in] - String to set
//   LPSTR lpszString     - [in] - Value to assign to the string
//
// Returns:
//   TRUE  - String was set successfully
//   FALSE - String was not set successfully.
//
/////////////////////////////////////////////////////////////////////
// 05-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::SetString(
   DWORD dwStringNumber,                // [in] - String to set
   LPSTR lpszString)                    // [in] - Value to assign to the string
{
BOOL   bReturn = FALSE;
DWORD  dwStringLength;
DWORD  dwReturn;
LPWSTR lpwszString = NULL;

   if (NULL == lpszString)
      goto Cleanup;
                                        // First, calculate the amount of data
                                        // we need to allocate for the UNICODE
                                        // string.
   dwStringLength = strlen (lpszString) + 1;

                                        // Attempt to allocate a new buffer.
   lpwszString = new WCHAR [dwStringLength];

   if (NULL == lpwszString)
   {
      goto Cleanup;
   }


                                        // Convert if we have strings.
   dwReturn = MultiByteToWideChar (
                 CP_ACP,
                 MB_PRECOMPOSED,
                 lpszString,
                 -1,
                 lpwszString,
                 dwStringLength);

   if (0 == dwReturn)
   {
      goto Cleanup;
   }

                                        // OK.  Call the UNICODE version to
                                        // actually put the info into the
                                        // packet. Take his return value
   bReturn = SetString(
                dwStringNumber,
                lpwszString);


Cleanup:

   if (NULL != lpwszString)
   {
      delete lpwszString;
      lpwszString = NULL;
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetString(
//
// Purpose:
//   This function gets a UNICODE string from the packet.
//
// Parameters:
//   DWORD  dwStringNumber - [in]  - String to get
//   DWORD  dwBufferSize   - [in]  - Maximum size (BYTES) of string to get
//   LPWSTR lpszString     - [out] - The String
//
// Returns:
//   TRUE  - String was successfully retrieved.
//   FALSE - The String was NOT successfully retrieved.
//
/////////////////////////////////////////////////////////////////////
// 05-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetString(
   DWORD  dwStringNumber,               // [in]  - String to get
   DWORD  dwBufferSize,                 // [in]  - Maximum size of string to get
   LPWSTR lpszString)                   // [out] - The String
{
BOOL  bReturn = FALSE;

                                        // Check to see if the string is OK.
   if (NULL == lpszString)
      goto Cleanup;

                                        // Get the data!
   bReturn = GetBinaryData(
                dwStringNumber,
                dwBufferSize,
                (LPBYTE) lpszString);

Cleanup:
   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetString(
//
// Purpose:
//   This function gets an ANSI string out of our packet.  All
//   strings are stored in the packet in UNICODE at this point
//   in time, so the string will be coverted from UNICODE to
//   ANSI and then returned in the buffer.
//
// Parameters:
//   DWORD  dwStringNumber - [in]  - String to get
//   DWORD  dwBufferSize   - [in]  - Maximum size (BYTES) of string to get
//   LPSTR  lpszString     - [out] - The String
//
// Returns:
//   TRUE  - String was retrieved successfully.
//   FALSE - The string was not retrieved successfully.
//
/////////////////////////////////////////////////////////////////////
// 05-13-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetString(
   DWORD  dwStringNumber,               // [in]  - String to get
   DWORD  dwBufferSize,                 // [in]  - Maximum size of string to get
   LPSTR  lpszString)                   // [out] - The String
{
BOOL   bReturn = FALSE;
DWORD  dwReturn;
LPWSTR lpwszString = NULL;

   if (NULL == lpszString)
      goto Cleanup;

                                        // Attempt to allocate a new buffer.
   lpwszString = new WCHAR [dwBufferSize];

   if (NULL == lpwszString)
   {
      goto Cleanup;
   }

                                        // Call the UNICODE version to get the
                                        // string.
   if( !GetString(
           dwStringNumber,
           dwBufferSize * sizeof (WCHAR),
           lpwszString) )
   {
      goto Cleanup;
   }

                                        // Convert the string.

   dwReturn = WideCharToMultiByte (
                 CP_ACP,
                 WC_COMPOSITECHECK,
                 lpwszString,           // UNICODE string
                 -1,
                 lpszString,            // non-UNICODE string
                 dwBufferSize,
                 NULL,
                 NULL );

   if (0 == dwReturn)
   {
      goto Cleanup;
   }

                                        // Indicate our success state at this
                                        // point.
   bReturn = TRUE;

Cleanup:

   if (NULL != lpwszString)
   {
      delete lpwszString;
      lpwszString = NULL;
   }

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// BOOL CAlert::GetStringLength(
//
// Purpose:
//    Gets the string length of the buffer.  This function assumes
//    that the strings are all stored in UNICODE.
//
// Parameters:
//    DWORD dwStringNumber   - [in]  - Number for string length
//    DWORD & dwStringLength - [out] - String length in characters.
//
// Returns:
//    TRUE  - Successful retrieval of the string length
//    FALSE - String length was not retrieved correctly.
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CAlert::GetStringLength(
   DWORD dwStringNumber,                // [in]  - Number for string length
   DWORD & dwStringLength)              // [out] - String length in characters.
{
BOOL   bReturn = FALSE;
LPWSTR lpString;
                                        // If there isn't a packet don't allow
                                        // this to happen.
   if (NULL == m_pPacket)
   {
      goto Cleanup;
   }

                                        // First check the subscript
   if (dwStringNumber >= CUSTOM_BUFFERS)
   {
      goto Cleanup;
   }

                                        // Check to see if this buffer is being
                                        // used
   if (0 == m_pPacket->cBufferEntries[dwStringNumber].dwBufferOffset)
   {
      goto Cleanup;
   }

   lpString = (LPWSTR) (((LPBYTE) m_pPacket) +
               m_pPacket->cBufferEntries[dwStringNumber].dwBufferOffset);

                                        // Get the string length using the
                                        // UNICODE version ONLY!
   dwStringLength = wcslen(
                       lpString);

                                        // Indicate success.
   bReturn = TRUE;

Cleanup:

   return bReturn;

}


