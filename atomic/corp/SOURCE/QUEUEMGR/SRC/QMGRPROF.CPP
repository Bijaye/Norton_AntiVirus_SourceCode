/////////////////////////////////////////////////////////////////////
// QMGRPROF.CPP - Created - 01/16/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/QUEUEMGR/VCS/qmgrprof.cpv   1.0   18 Apr 1997 14:05:08   DSACKIN  $
//
// Description:
//    This will contain the Profiler functions for our CAlertQueueManager
//    object.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/QUEUEMGR/VCS/qmgrprof.cpv  $
// 
//    Rev 1.0   18 Apr 1997 14:05:08   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   24 Mar 1997 16:44:42   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:06:12   RFULLER
// Initial revision
//
//    Rev 1.0   18 Jan 1997 06:58:50   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////
//      Includes

#include "afxtempl.h"
#include "platform.h"
#include "process.h"

#include "QueueMgr.h"


/////////////////////////////////////////////////////////////////////
// void CQueueManager::InitializeProfiler(
//
// Purpose:
//    This function initializes our timer and gets info out of the
//    ini file as to who to connect to.
//
// Parameters:
//   LPCTSTR lpszQueueManagerName - [in] - The name of the QueueManager
//
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CQueueManager::InitializeProfiler(
   LPCTSTR lpszQueueManagerName)        // [in] - The name of the QueueManager
{
TCHAR  szFilename[SYM_MAX_PATH+1];
TCHAR  szEXEExt[5] = TEXT(".EXE");
TCHAR  szDefault[SYM_MAX_PATH+1] = TEXT("disabled");
TCHAR  szAddress[SYM_MAX_PATH+1];
TCHAR  szProtocol[SYM_MAX_PATH+1];
TCHAR  szEndpoint[SYM_MAX_PATH+1];
LPTSTR lpszExt;
DWORD dwReturn;
HINSTANCE hInst;

TCHAR szTemp[SYM_MAX_PATH+1];
LPTSTR lpszCommandLine;

   STRCPY(m_szQueueManagerName, TEXT("QueueManager "));
   STRCAT(m_szQueueManagerName, lpszQueueManagerName);

   lpszCommandLine = GetCommandLine (); // Returns a pointer to the command line.
   STRCPY (szTemp, lpszCommandLine);    // Make a copy to work with

                                        // I am going to assume we don't have
                                        // to kill the other command line
                                        // parameters for GetModuleHandle to work.

   hInst = GetModuleHandle (szTemp);    // Now set our hInst.
                                        // First build our ini filename
   dwReturn = GetModuleFileName(
                 hInst,
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
      m_szQueueManagerName,             // Section
      TEXT("Address"),                  // key
      szDefault,                        // default return
      szAddress,                        // Where to put it
      sizeof (szAddress),               // Max number of bytes
      szFilename);                      // INI filename

   GetPrivateProfileString(
      m_szQueueManagerName,             // Section
      TEXT("Protocol"),                 // key
      szDefault,                        // default return
      szProtocol,                       // Where to put it
      sizeof (szProtocol),              // Max number of bytes
      szFilename);                      // INI filename

   GetPrivateProfileString(
      m_szQueueManagerName,             // Section
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
// void CQueueManager::InitializeProfiler(
//
// Purpose:
//    This function is the NON-UNICODE version that converts a string
//    passed in NON-UNICODE format to UNICODE and calls the correct
//    version of the function.
//
// Parameters:
//    LPCSTR lpszQueueManagerName - [in] - The name of the CQueueManager
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////

#ifdef UNICODE

void CQueueManager::InitializeProfiler(
   LPCSTR lpszQueueManagerName)         // [in] - The name of the service
{
DWORD  dwReturn;
WCHAR  szQueueManagerNameW[SYM_MAX_PATH + 1];

   dwReturn = MultiByteToWideChar (
                 CP_ACP,
                 MB_PRECOMPOSED,
                 lpszQueueManagerName,
                 -1,
                 szQueueManagerNameW,
                 sizeof(szQueueManagerNameW) / sizeof(WCHAR) );

                                        // If we dwReturn is zero, just return
                                        // it to the caller.
   if (0 != dwReturn)
   {
                                        // Call the UNICODE version.
      InitializeProfiler(
         szQueueManagerNameW);
   }
}


/////////////////////////////////////////////////////////////////////
// void CQueueManager::InitializeProfiler(
//
// Purpose:
//    This function is the NON-UNICODE version that converts a string
//    passed in NON-UNICODE format to UNICODE and calls the correct
//    version of the function.
//
// Parameters:
//    LPCWSTR lpszQueueManagerName - [in] - The name of the CQueueManager
//
// Returns:
//    none
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////

#else // ifdef UNICODE

void CQueueManager::InitializeProfiler(
   LPCWSTR lpszQueueManagerName)        // [in] - The name of the service
{
DWORD dwReturn;
char  szQueueManagerNameA[SYM_MAX_PATH + 1];

                                        // Convert the string.

   dwReturn = WideCharToMultiByte (
                 CP_ACP,
                 WC_COMPOSITECHECK,
                 lpszQueueManagerName,
                 -1,
                 szQueueManagerNameA,
                 sizeof (szQueueManagerNameA),  // * sizeof(char)
                 NULL,
                 NULL );

   if (0 != dwReturn)
   {
      InitializeProfiler(
         szQueueManagerNameA);
   }

}

#endif // ifdef UNICODE


/////////////////////////////////////////////////////////////////////
// CQueueManager :: LogMessage
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
void CQueueManager::LogMessage(
   LPTSTR lpszMessage)                  // [in] - Message to send
{
TCHAR  szLogMessage[2*SYM_MAX_PATH+1];
TCHAR  szFormat[2*SYM_MAX_PATH+1] = TEXT("DEBUG [%s] - %s");

#ifndef UNICODE
char   szQueueManagerNameA [SYM_MAX_PATH+1];
DWORD  dwReturn;

                                        // Now run the conversion
   dwReturn = WideCharToMultiByte (
                 CP_ACP,
                 WC_COMPOSITECHECK,
                 m_szQueueManagerName,
                 -1,
                 szQueueManagerNameA,
                 sizeof (szQueueManagerNameA),
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
      m_szQueueManagerName,
#else // ifdef UNICODE
      szQueueManagerNameA,
#endif // ifdef UNICODE
      lpszMessage);

   m_cTimer.LogMessage (szLogMessage);

#ifndef UNICODE
Cleanup:
#endif

   return;
}


/////////////////////////////////////////////////////////////////////
// CQueueManager :: LogMessage
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

void CQueueManager::LogMessage(
   LPSTR lpszMessage)                   // [in] - Message to send
{
DWORD  dwReturn;
DWORD  dwLength;
LPWSTR lpszMessageW = NULL;

                                        // Get the length
   dwLength = strlen (lpszMessage);

                                        // Allocate a UNICODE string
   lpszMessageW = new WCHAR [dwLength + 1];

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
      delete lpszMessageW;
      lpszMessageW = NULL;
   }
}

/////////////////////////////////////////////////////////////////////
// CQueueManager :: LogMessage
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

void CQueueManager::LogMessage(
   LPWSTR lpszMessage)                  // [in] - Message to send
{
DWORD  dwReturn;
DWORD  dwLength;
LPSTR  lpszMessageA = NULL;

                                        // Get the length
   dwLength = wcslen (lpszMessage);

                                        // Allocate a non-UNICODE string
   lpszMessageA = new char [dwLength + 1];

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
                 dwLength + 1,          // * sizeof(char)
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
// CQueueManager :: LogMessage
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
void CQueueManager::LogMessage(
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
// CQueueManager :: LogMessage
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

void CQueueManager::LogMessage(
   LPSTR lpszMessage,                   // [in] - Message to send
   LPSTR lpszString)                    // [in] - Supporting string
{
char   szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output using a
                                        // NON-UNICODE version of the function.
   sprintf (
      szLogMessage,
      lpszMessage,
      lpszString);

   LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CQueueManager :: LogMessage
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

void CQueueManager::LogMessage(
   LPWSTR lpszMessage,                  // [in] - Message to send
   LPWSTR lpszString)                   // [in] - Supporting string
{
WCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output using a
                                        // UNICODE version of the function.
   swprintf (
      szLogMessage,
      lpszMessage,
      lpszString);

   LogMessage (szLogMessage);
}

#endif // ifdef UNICODE


/////////////////////////////////////////////////////////////////////
// CQueueManager :: LogMessage
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
void CQueueManager::LogMessage(
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
// CQueueManager :: LogMessage
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

void CQueueManager::LogMessage(
   LPSTR  lpszMessage,                  // [in] - Message to send
   DWORD  dwValue)                      // [in] - Supporting DWORD
{
char  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   sprintf (
      szLogMessage,
      lpszMessage,
      dwValue);

   LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CQueueManager :: LogMessage
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

void CQueueManager::LogMessage(
   LPWSTR lpszMessage,                  // [in] - Message to send
   DWORD  dwValue)                      // [in] - Supporting DWORD
{
WCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   swprintf (
      szLogMessage,
      lpszMessage,
      dwValue);

   LogMessage (szLogMessage);
}
#endif // ifdef UNICODE



