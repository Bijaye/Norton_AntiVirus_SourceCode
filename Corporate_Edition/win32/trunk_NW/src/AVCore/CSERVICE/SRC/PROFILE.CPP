/////////////////////////////////////////////////////////////////////
// Profile.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/CSERVICE/VCS/profile.cpv   1.0   09 May 1997 17:50:36   ksackin  $
//
// Description:
//      This is the implementation file for the CService Profiling functions.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/CSERVICE/VCS/profile.cpv  $
// 
//    Rev 1.0   09 May 1997 17:50:36   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 13:48:58   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:09:28   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:46:56   DKEEFER
// Initial revision
// 
//    Rev 1.4   25 Oct 1996 09:50:20   TSmith
// Removed obsolete #ifdef DEBUG_PROFILE block delimiters.
// 
//    Rev 1.3   24 Oct 1996 01:08:14   DSACKIN
// Added full UNICODE/NON-UNICODE support.
//
//    Rev 1.2   20 Jun 1996 02:20:50   DSACKIN
// Included profiler.h that will define (or not) DEBUG_PROFILE.
// if DEBUG_PROFILE is not defined, none of the code in this file will be
// added to the project.
//
//    Rev 1.1   19 Jun 1996 13:48:12   DSACKIN
// All Profiler buffers have been doubled in size.
//
//    Rev 1.0   13 Jun 1996 17:15:12   DSACKIN
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"

#include "SymSaferStrings.h"
#include <new>
#include "profiler.h"

#include "service.h"


/////////////////////////////////////////////////////////////////////
// CService :: InitializeProfiler
//
// This function initializes our timer and gets info out of the
// registry as to who to connect to
//
// Parameters:
//    none.
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/10/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
void CService::InitializeProfiler(
   LPTSTR lpszServiceName)           // [in] - The name of the service
{
TCHAR  szFilename[SYM_MAX_PATH+1];
TCHAR  szEXEExt[5] = TEXT(".EXE");
TCHAR  szDefault[SYM_MAX_PATH+1] = TEXT("disabled");
TCHAR  szAddress[SYM_MAX_PATH+1];
TCHAR  szProtocol[SYM_MAX_PATH+1];
TCHAR  szEndpoint[SYM_MAX_PATH+1];
LPTSTR lpszExt;
DWORD dwReturn;

                                        // First build our ini filename
   dwReturn = GetModuleFileName(
                 m_hInst,
                 szFilename,
                 sizeof (szFilename) );

   if (0 == dwReturn)                   // Return if an error ocurred.
      return;

   STRUPR (szFilename);

   lpszExt = STRSTR (szFilename, szEXEExt);

   if (NULL != lpszExt)
      *lpszExt = EOS;

   STRCAT (szFilename, TEXT(".INI"));

                                        // Now that we have the filename,
                                        // lets get our info.

   GetPrivateProfileString(
      lpszServiceName,                  // Section
      TEXT("Address"),                  // key
      szDefault,                        // default return
      szAddress,                        // Where to put it
      sizeof (szAddress),               // Max number of bytes
      szFilename);                      // INI filename

   GetPrivateProfileString(
      lpszServiceName,                  // Section
      TEXT("Protocol"),                 // key
      szDefault,                        // default return
      szProtocol,                       // Where to put it
      sizeof (szProtocol),              // Max number of bytes
      szFilename);                      // INI filename

   GetPrivateProfileString(
      lpszServiceName,                  // Section
      TEXT("Endpoint"),                 // key
      szDefault,                        // default return
      szEndpoint,                       // Where to put it
      sizeof (szEndpoint),              // Max number of bytes
      szFilename);                      // INI filename

                                        // Check for any info back
   if ( STRCMP (szAddress,  szDefault) &&
        STRCMP (szProtocol, szDefault) &&
        STRCMP (szEndpoint, szDefault) )
   {
      m_cTimer.SetProtocol (
                  szProtocol,
                  szAddress,
                  szEndpoint);

      if (m_cTimer.Connect())
         LogMessage(TEXT("Connection established."));
   }
}


/////////////////////////////////////////////////////////////////////
// CService :: InitializeProfiler
//
// This function is the NON-UNICODE version that converts a string
// passed in NON-UNICODE format to UNICODE and calls the correct
// version of the function.
//
// Parameters:
//    LPSTR - lpszServiceName - non-UNICODE string with service name in it
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 09/30/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#ifdef UNICODE

void CService::InitializeProfiler(
   LPSTR lpszServiceName)                 // [in] - The name of the service
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
   if (0 != dwReturn)
   {
                                        // Call the UNICODE version.
      InitializeProfiler(
         szServiceNameW);
   }
}


/////////////////////////////////////////////////////////////////////
// CService :: InitializeProfiler
//
// This function is the NON-UNICODE version that converts a string
// passed in NON-UNICODE format to UNICODE and calls the correct
// version of the function.
//
// Parameters:
//    LPSTR - szServiceName - non-UNICODE string with service name in it
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 09/30/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#else // ifdef UNICODE

void CService::InitializeProfiler(
   LPWSTR lpszServiceName)              // [in] - The name of the service
{
DWORD dwReturn;
char  szServiceNameA[MAX_SERVICE_NAME_LENGTH + 1];

                                        // Convert the string.

   dwReturn = WideCharToMultiByte (
                 CP_ACP,
                 WC_COMPOSITECHECK,
                 lpszServiceName,
                 -1,
                 szServiceNameA,
                 sizeof (szServiceNameA),  // * sizeof(char)
                 NULL,
                 NULL );

   if (0 != dwReturn)
   {
      InitializeProfiler(
         szServiceNameA);
   }

}

#endif // ifdef UNICODE


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This function will take a message, tack out ID string to it
// and call to the member function to take care of it.
//
// Parameters:
//    LPTSTR - lpszMessage - [in] String to long
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/10/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
void CService::LogMessage(
   LPTSTR lpszMessage)                  // [in] - Message to send
{
TCHAR  szLogMessage[2*SYM_MAX_PATH+1];
TCHAR  szFormat[2*SYM_MAX_PATH+1] = TEXT("DEBUG [%s] - %s");

#ifndef UNICODE
char   szServiceNameA [MAX_SERVICE_NAME_LENGTH+1];
DWORD  dwReturn;

                                        // Now run the conversion
   dwReturn = WideCharToMultiByte (
                 CP_ACP,
                 WC_COMPOSITECHECK,
                 m_szServiceName,
                 -1,
                 szServiceNameA,
                 sizeof (szServiceNameA),
                 NULL,
                 NULL );

   if (0 == dwReturn)
   {
      goto Cleanup;
   }
#endif // ifndef UNICODE

   SPRINTF (
      szLogMessage,
      szFormat,
#ifdef UNICODE
      m_szServiceName,
#else // ifdef UNICODE
      szServiceNameA,
#endif // ifdef UNICODE
      lpszMessage);

   m_cTimer.LogMessage (szLogMessage);

#ifndef UNICODE
Cleanup:
#endif

   return;
}


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This is the UNICODE function that will get a NON-UNICODE string,
// convert and sent to the UNICODE version.
//
// Parameters:
//    LPSTR - lpszMessage - [in] String to log
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 09/30/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#ifdef UNICODE

void CService::LogMessage(
   LPSTR lpszMessage)                     // [in] - Message to send
{
DWORD  dwReturn;
DWORD  dwLength;
LPWSTR lpszMessageW = NULL;

                                        // Get the length
   dwLength = strlen (lpszMessage);

                                        // Allocate a UNICODE string
   try
   {
	lpszMessageW = new WCHAR [dwLength + 1];
   }
   catch(std::bad_alloc &)
   {
		
   }

   if (NULL == lpszMessageW)
   {
      goto Cleanup;
   }


                                        // Now run the conversion
   dwReturn = MultiByteToWideChar (
                 CP_ACP,
                 MB_PRECOMPOSED,
                 lpszMessage,
                 -1,
                 lpszMessageW,
                 (dwLength + 1) / sizeof(WCHAR) );

   if (0 == dwReturn)
   {
      goto Cleanup;
   }

                                        // Now call the non-UNICODE version.
   LogMessage (
      lpszMessageW);

Cleanup:

   if (NULL != lpszMessageW)
   {
      delete[] lpszMessageW;
      lpszMessageW = NULL;
   }
}

/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This is the non-UNICODE function that will get a UNICODE string,
// convert it, and forward it on to the non-UNICODE function.
//
// Parameters:
//    LPWSTR - lpszMessage - [in] String to log
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 09/30/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#else // ifdef UNICODE

void CService::LogMessage(
   LPWSTR lpszMessage)                    // [in] - Message to send
{
DWORD  dwReturn;
DWORD  dwLength;
LPSTR  lpszMessageA = NULL;

                                        // Get the length
   dwLength = wcslen (lpszMessage);

                                        // Allocate a non-UNICODE string
   try
   {
	lpszMessageA = new char [dwLength + 1];
   }
   catch(std::bad_alloc &)
   {
   }

   if (NULL == lpszMessageA)
   {
      goto Cleanup;
   }


   if (0 == dwReturn)
   {
      goto Cleanup;
   }

                                        // Now run the conversion
   dwReturn = WideCharToMultiByte (
                 CP_ACP,
                 WC_COMPOSITECHECK,
                 lpszMessage,
                 -1,
                 lpszMessageA,
                 dwLength + 1,  // * sizeof(char)
                 NULL,
                 NULL );

                                        // Now call the non-UNICODE version.
   LogMessage (
      lpszMessageA);

Cleanup:

   if (NULL != lpszMessageA)
   {
      delete lpszMessageA;
      lpszMessageA = NULL;
   }
}

#endif


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This function will take a message and a string, put them together
// and the LogMessage function
//
// Parameters:
//    LPTSTR - lpszMessage - [in] Format string
//    LPTSTR - lpszString  - [in] Supporting string
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/12/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
void CService::LogMessage(
   LPTSTR lpszMessage,                  // [in] - Message to send
   LPTSTR lpszString)                   // [in] - Supporting string
{
TCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   SPRINTF (
      szLogMessage,
      lpszMessage,
      lpszString);

   LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This is a NON-UNICODE version of LogMessage that takes a message
// and a string and combines them using NON-UNICODE functions.  Once
// the string has been built, we send it to the non-UNICODE version
// of the string based LogMessage function where it will be converted
// to UNICODE and logged.
//
// Parameters:
//    LPSTR - lpszMessage - [in] Format string
//    LPSTR - lpszString  - [in] Supporting string
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 10/01/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#ifdef UNICODE

void CService::LogMessage(
   LPSTR lpszMessage,                   // [in] - Message to send
   LPSTR lpszString)                    // [in] - Supporting string
{
char   szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output using a
                                        // NON-UNICODE version of the function.
   sssnprintf (
      szLogMessage,
      sizeof(szLogMessage),
      lpszMessage,
      lpszString);

   LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This function is a UNICODE version of LogMessage that takes a message
// and a support string and puts them together using UNICODE functions.
// The result is then passed into the UNICODE version of the string based
// LogMessage function where it will be converted and sent.
//
// Parameters:
//    LPWSTR - lpszMessage - [in] Format string
//    LPWSTR - lpszString  - [in] Supporting string
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 10/01/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#else // ifdef UNICODE

void CService::LogMessage(
   LPWSTR lpszMessage,                  // [in] - Message to send
   LPWSTR lpszString)                   // [in] - Supporting string
{
WCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output using a
                                        // UNICODE version of the function.
   sssnprintf (
      szLogMessage,
      sizeof(szLogMessage),
      lpszMessage,
      lpszString);

   LogMessage (szLogMessage);
}

#endif // ifdef UNICODE


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This function will take a message and a string, put them together
// and the LogMessage function
//
// Parameters:
//    LPTSTR - lpszMessage - [in] Format string
//    DWORD  - dwValue     - [in] Supporting DWORD
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/12/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
void CService::LogMessage(
   LPTSTR lpszMessage,                  // [in] - Message to send
   DWORD  dwValue)                      // [in] - Supporting DWORD
{
TCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   SPRINTF (
      szLogMessage,
      lpszMessage,
      dwValue);

   LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// Here we have the non-UNICODE version of LogMessage taking an lpszMessage
// and a DWORD.  We will perform the functions using non-UNICODE functions
// and then pass it on to the string based non UNICODE function to
// convert it and log it.
//
// Parameters:
//    LPSTR  - lpszMessage - [in] Format string
//    DWORD  - dwValue     - [in] Supporting DWORD
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/12/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#ifdef UNICODE

void CService::LogMessage(
   LPSTR  lpszMessage,                  // [in] - Message to send
   DWORD  dwValue)                      // [in] - Supporting DWORD
{
char  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   sssnprintf (
      szLogMessage,
      sizeof(szLogMessage),
      lpszMessage,
      dwValue);

   LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CService :: LogMessage
//
// This is the UNICODE version that will be supplied when we are
// compiled without UNICODE defined.  We will explicitly use the
// UNICODE versions of functions here and let the string based
// LogMessage do the conversion before the log.
//
// Parameters:
//    LPTSTR - lpszMessage - [in] Format string
//    DWORD  - dwValue     - [in] Supporting DWORD
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 10/01/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#else // ifdef UNICODE

void CService::LogMessage(
   LPWSTR lpszMessage,                  // [in] - Message to send
   DWORD  dwValue)                      // [in] - Supporting DWORD
{
WCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   sssnprintf (
      szLogMessage,
      sizeof(szLogMessage),
      lpszMessage,
      dwValue);

   LogMessage (szLogMessage);
}
#endif // ifdef UNICODE


