/////////////////////////////////////////////////////////////////////
// Profile.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/CSERVICE/VCS/svcprof.cpv   1.0   09 May 1997 17:50:36   ksackin  $
//
// Description:
//      This is the implementation file for the CServiceControl Profiling functions.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/CSERVICE/VCS/svcprof.cpv  $
// 
//    Rev 1.0   09 May 1997 17:50:36   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 13:49:02   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:09:28   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:47:00   DKEEFER
// Initial revision
// 
//    Rev 1.3   25 Oct 1996 09:49:08   TSmith
// Removed obsolete #ifdef DEBUG_PROFILE block delimiters.
// 
//    Rev 1.2   01 Aug 1996 13:09:02   DSACKIN
// Changed the CServiceControl to use static strings instead of allocated strings.
//
//    Rev 1.1   20 Jun 1996 02:19:06   DSACKIN
// Included profiler.h that will define (or not) DEBUG_PROFILE.
// if DEBUG_PROFILE is not defined, none of the code in this file will be
// added to the project.
//
//    Rev 1.0   20 Jun 1996 01:52:20   DSACKIN
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"

#include "profiler.h"

#include "svcctrl.h"


/////////////////////////////////////////////////////////////////////
// CServiceControl :: InitializeProfiler
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
void CServiceControl::InitializeProfiler(
   LPTSTR lpszLogName)                  // [in] - The name of the service
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

   STRCPY(m_szLogName, TEXT("SvcCtrl "));
   STRCAT(m_szLogName, lpszLogName);

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
      m_szLogName,                      // Section
      TEXT("Address"),                  // key
      szDefault,                        // default return
      szAddress,                        // Where to put it
      sizeof (szAddress),               // Max number of bytes
      szFilename);                      // INI filename

   GetPrivateProfileString(
      m_szLogName,                      // Section
      TEXT("Protocol"),                 // key
      szDefault,                        // default return
      szProtocol,                       // Where to put it
      sizeof (szProtocol),              // Max number of bytes
      szFilename);                      // INI filename

   GetPrivateProfileString(
      m_szLogName,                      // Section
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
// CServiceControl :: LogMessage
//
// This function will take a message, tack out ID string to it
// and call to the member function to take care of it.
//
// Parameters:
//    LPTSTR - szMessage - [in] String to long
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/10/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
void CServiceControl::LogMessage(
   LPTSTR szMessage)                    // [in] - Message to send
{
TCHAR  szLogMessage[2*SYM_MAX_PATH+1];
TCHAR  szFormat[2*SYM_MAX_PATH+1] = TEXT("DEBUG [%s] - %s");

   SPRINTF (
      szLogMessage,
      szFormat,
      (EOS != m_szLogName[0]) ?
         m_szLogName :
         TEXT("<no log name>"),
      szMessage);

   m_cTimer.LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CServiceControl :: LogMessage
//
// This function will take a message and a string, put them together
// and the LogMessage function
//
// Parameters:
//    LPTSTR - szMessage - [in] Format string
//    LPTSTR - szString  - [in] Supporting string
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/12/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
void CServiceControl::LogMessage(
   LPTSTR szMessage,                    // [in] - Message to send
   LPTSTR szString)                     // [in] - Supporting string
{
TCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   SPRINTF (
      szLogMessage,
      szMessage,
      szString);

   LogMessage (szLogMessage);
}


/////////////////////////////////////////////////////////////////////
// CServiceControl :: LogMessage
//
// This function will take a message and a string, put them together
// and the LogMessage function
//
// Parameters:
//    LPTSTR - szMessage - [in] Format string
//    DWORD  - dwValue   - [in] Supporting DWORD
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 06/12/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
void CServiceControl::LogMessage(
   LPTSTR szMessage,                    // [in] - Message to send
   DWORD  dwValue)                      // [in] - Supporting DWORD
{
TCHAR  szLogMessage[2*SYM_MAX_PATH+1];

                                        // Format the output
   SPRINTF (
      szLogMessage,
      szMessage,
      dwValue);

   LogMessage (szLogMessage);
}
