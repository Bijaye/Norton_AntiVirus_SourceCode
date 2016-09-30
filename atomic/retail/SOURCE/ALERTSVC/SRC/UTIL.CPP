/////////////////////////////////////////////////////////////////////
// UTIL.CPP - Created - 11/20/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/util.cpv   1.3   01 Apr 1998 18:09:16   DALLEE  $
//
// Description:
//    This file will contain utility functions for the AlertService
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/util.cpv  $
// 
//    Rev 1.3   01 Apr 1998 18:09:16   DALLEE
// Added handling for quaratine actions (ACTION_QUARANTINE).
// 
//    Rev 1.2   17 Jun 1997 15:28:42   DSACKIN
// In custom method, we need to be sending in the BYTE length of our buffer
// and NOT how many characters we need.  (UNICODE).
//
//    Rev 1.1   18 May 1997 05:07:12   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:12   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:34:04   DSACKIN
// Changed the BuildVirusMessage prototype.
//
//    Rev 1.0   13 Mar 1997 15:06:10   RFULLER
// Initial revision
//
//    Rev 1.6   28 Jan 1997 04:09:28   DSACKIN
// Added TrimAlertMessageToLength function.
//
//    Rev 1.5   23 Jan 1997 17:19:48   DSACKIN
// Removed some debug code.
//
//    Rev 1.4   23 Jan 1997 17:12:08   DSACKIN
// Fixed the appending of the custom alert message.
//
//    Rev 1.3   21 Jan 1997 13:59:12   TSmith
// Modified 'BuildVirusMessage' to process the DomainName now
// stored by CAlertNotification objects.
//
//    Rev 1.2   18 Jan 1997 07:01:20   DSACKIN
// Reworked the AlertSvc to use the CAlertQueueManager object to handle all
// alerting.
//
//    Rev 1.1   22 Dec 1996 15:53:20   TSmith
// Changed calls to CAlertNotification string retrieval functions to support
// that class's new function arguments.
//
//    Rev 1.0   20 Nov 1996 22:13:16   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////
//      Includes
#include <afxtempl.h>
#include <platform.h>
#include <process.h>
#include <winnetwk.h>
#include <lmcons.h>
#include <lmmsg.h>
#include <lmerr.h>
#include <servenfy.h>
#include <linksrv.h>
#include <service.h>
#include <svcctrl.h>
#include <navrpc.h>
#include <xapi.h>
#include "alertsvc.h"
#include "alertstr.h"

/////////////////////////////////////////////////////////////////////
//      Constants
#define MAX_RES_STR_LEN       (1024)


/////////////////////////////////////////////////////////////////////
// Externs
extern HINSTANCE ghInst;

/////////////////////////////////////////////////////////////////////
// Global variables
NAVOPTIONS      grNavOptions;           // For use with our C functions

/////////////////////////////////////////////////////////////////////
// CAlertService::LoadOptions
//
// Fills a NAVOPTIONS structure from NAVOPTS.DAT
//
// Parameters:
//      None.
//
// Return Value:
//      None.
/////////////////////////////////////////////////////////////////////
// 11/03/96 TSMITH  Function created.
/////////////////////////////////////////////////////////////////////
VOID CAlertService::LoadOptions( )
{
                                        // Load the options once for the Alert
                                        // Service
   ReadNavBin( &m_NavOptions );
   ReadNavStart( &m_NavOptions );

                                        // And again into the global structure
                                        // for the C functions
   ReadNavBin( &grNavOptions );
   ReadNavStart( &grNavOptions );
}




/////////////////////////////////////////////////////////////////////
// GetResourceString
//
// Private utility function to fill a CString object with a resource
// string. This is necessary because the CString::LoadString
// function never returns when called inside the service. This may
// or may not be a result of the fact that CString::LoadString requires
// there to be a CWinApp for it to get the module handle from.
//
// Parameters:
//      IN HINSTANCE hInst - Our processes instance handle.
//      IN UINT uResId - ID of resource string to retrieve.
//      OUT CString& szReturn - Reference to a CString object to fill.
//
// Return Value:
//      TRUE - Resource string successfully retrieved.
//      FALSE - LoadString failed or OUT var NULL.
//
/////////////////////////////////////////////////////////////////////
// 10/30/96 TSMITH  Function created.
// 01/16/97 - DSACKIN - Took this function out of CAlertService so
//                    - that it can be accessed by other objects
/////////////////////////////////////////////////////////////////////
VOID GetResourceString(
   HINSTANCE hInst,                     // [in]  - Our process instance handle
   UINT uResId,                         // [in]  - ID of string to retrieve
   CString& szReturn)                   // [out] - Reference to a CString to fill
{
TCHAR szResStr[ MAX_RES_STR_LEN ];      // Temporary buffer
INT   nStatus;                          // LoadString's return

                                        // Make sure buffer clear
   MEMSET( &szResStr, 0, sizeof( szResStr ) );

   if ( &szReturn != NULL )
   {
                                        // Load resource string via
                                        // Win32
      nStatus = LoadString(
                   hInst,
                   uResId,
                   szResStr,
                   MAX_RES_STR_LEN );

      if ( nStatus > 0 )              // If resource string retrieved
      {
                                        // Copy chars to out var
         szReturn = ( LPCTSTR )szResStr;
      }
      else
      {
         throw( ErrLoadString );     // Else throw exception
      }
   }
}


/////////////////////////////////////////////////////////////////////
// ComposeActionString
//
// Constructs the appropriate action string using the function
// arguments as criteria for selecting the proper string resources.
//
// Parameters:
//      IN DWORD dwAlertType - File, boot record, MBR infection.
//      IN DWORD dwAction - What action was attempted.
//      IN BOOL fSuccess - Was that action successful?
//      OUT CString* pszReturn - Pointer to CSting object. Optional.
//
//
// Notes:
//      Use this function to retrieve just the resource ID by passing
//      in a NULL for pszReturn.
//
//      Also, since the NAV line for WinNT doesn't currently support
//      any kind of Unknown virus checking, if any action of those
//      types come through, an "unknown action" message ID will be
//      returned.
//
// Return Value:
//      UINT - Resource ID of the appropriate action string.
/////////////////////////////////////////////////////////////////////
// 11/03/96 TSMITH  Function created.
/////////////////////////////////////////////////////////////////////
UINT ComposeActionString(
   DWORD    dwAlertType,                // [in]  - File, boot record or MBR infection
   DWORD    dwAction,                   // [in]  - What action was attempted
   BOOL     fSuccess,                   // [in]  - Were we successful
   CString* pszReturn)                  // [out] - Pointer to a CString object
{
CString szAction;
UINT    uResId;
                                        // Based on the action type reported
                                        // and whether it was successful,
                                        // compute the ID of, and retreive
                                        // a string resource to use for our
                                        // notification messages.
   switch ( dwAction )
   {
      case ACTION_NONE:
         uResId = IDS_VIR_ACT_NONE;
         break;

      case ACTION_MOVE:
         uResId = fSuccess ? IDS_VIR_ACT_MOVE : IDS_VIR_ACT_MOVE_FAIL;
         break;

      case ACTION_RENAME:
         uResId = fSuccess ? IDS_VIR_ACT_RENAME : IDS_VIR_ACT_RENAME_FAIL;
         break;

      case ACTION_REPAIR:
         switch ( dwAlertType )
         {
            case FILE_KNOWN_VIRUS:
               uResId = fSuccess ? IDS_VIR_ACT_REPAIRED : IDS_VIR_ACT_NOT_REPAIRED;
               break;

            case BOOT_RECORD_KNOWN_VIRUS:
               uResId = fSuccess ? IDS_VIR_ACT_BOOTREC_REPAIRED : IDS_VIR_ACT_BOOTREC_NOT_REPAIRED;
               break;

            case MBR_KNOWN_VIRUS:
               uResId = fSuccess ? IDS_VIR_ACT_MBR_REPAIRED : IDS_VIR_ACT_MBR_NOT_REPAIRED;
               break;

            default:
               uResId = IDS_VIR_ACT_UNDEFINED;
               break;
         }
         break;

      case ACTION_QUARANTINE:
         uResId = fSuccess ? IDS_VIR_ACT_QUARANTINED : IDS_VIR_ACT_NOT_QUARANTINED;
         break;

      case ACTION_DELETE:
         uResId = fSuccess ? IDS_VIR_ACT_DELETED : IDS_VIR_ACT_NOT_DELETED;
         break;

      case ACTION_EXCLUDE:
         switch ( dwAlertType )
         {
            case FILE_UNKNOWN_VIRUS:
               uResId = IDS_VIR_ACT_UNDEFINED;
               break;

            case FILE_KNOWN_VIRUS:
               uResId = fSuccess ? IDS_VIR_ACT_KNOWN_EXCL : IDS_VIR_ACT_KNOWN_NOT_EXCL;
               break;

            case BOOT_RECORD_UNKNOWN_VIRUS:
               uResId = IDS_VIR_ACT_UNDEFINED;
               break;

            case BOOT_RECORD_KNOWN_VIRUS:
               uResId = fSuccess ? IDS_VIR_ACT_BR_KNOWN_EXCL : IDS_VIR_ACT_BR_KNOWN_NOT_EXCL;
               break;

            case MBR_UNKNOWN_VIRUS:
               uResId = IDS_VIR_ACT_UNDEFINED;
               break;

            case MBR_KNOWN_VIRUS:
               uResId = fSuccess ? IDS_VIR_ACT_MBR_KNOWN_EXCL : IDS_VIR_ACT_MBR_KNOWN_NOT_EXCL;
               break;

            default:
               uResId = IDS_VIR_ACT_UNDEFINED;
               break;
         }
         break;

      case ACTION_SHUTDOWN:
         uResId = fSuccess ? IDS_VIR_ACT_COMPUTER_SHUTDOWN : IDS_VIR_ACT_SHUTDOWN_FAIL;
         break;

                                        // Since no unknown virus detection or
                                        // file inoculation is supported right
                                        // now, we return an "unknown action"
                                        // string & ID. The only time this
                                        // should happen is if a newer version
                                        // of NAV that does support these sends
                                        // a packet to an older alert service.
      case ACTION_INOCULATE:
         switch ( dwAlertType )
         {
            case FILE_UNKNOWN_VIRUS:
            case FILE_KNOWN_VIRUS:
            case BOOT_RECORD_UNKNOWN_VIRUS:
            case BOOT_RECORD_KNOWN_VIRUS:
            case MBR_UNKNOWN_VIRUS:
            case MBR_KNOWN_VIRUS:
            default:
               uResId = IDS_VIR_ACT_UNDEFINED;
               break;
         }
         break;

      case ACTION_DENY_ACCESS:
         uResId = IDS_VIR_ACT_ACCESS_DENIED;
         break;

      default:
         uResId = IDS_VIR_ACT_UNDEFINED;
         break;
   }

   if ( pszReturn != NULL )
   {
      GetResourceString(
         ghInst,
         uResId,
         szAction );

      *pszReturn = szAction;
   }

   return( uResId );
}


/////////////////////////////////////////////////////////////////////
// AppendCustomMessage
//
// Adds the text of the NAV custom alert message to its argument.
// If NAV is not currently setup to use a custom alert message,
// the CString referenced by the argument will be unchanged.
//
// Parameters:
//      IN CString& szReturn - String to append custom message to.
//
// Return Value:
//      None.
/////////////////////////////////////////////////////////////////////
// 11/04/96 TSMITH  Function created.
/////////////////////////////////////////////////////////////////////
VOID AppendCustomMessage(
   CString& szReturn)                   // [in] - String to append custom message to
{
TCHAR   szCustomMessage[SIZE_ALERT_MSG+1];

#ifdef  UNICODE
DWORD   dwReturn;
#endif // ifdef UNICODE

                                        // If there is no message or they don't
                                        // want it displayed, time to go!
   if ( (NULL == &szReturn) ||
        (!grNavOptions.alert.bDispAlertMsg) )
   {
      return;
   }



#ifdef UNICODE

                                        // If we are running in UNICODE, we are
                                        // going to have to convert the custom
                                        // message before appending it to the
                                        // rest of our message.

   dwReturn = MultiByteToWideChar (
                 CP_ACP,
                 MB_PRECOMPOSED,
                 grNavOptions.alert.szAlertMsg,
                 -1,
                 szCustomMessage,
                 sizeof(szCustomMessage) / sizeof(WCHAR) );

                                        // If we didn't get anything back,
                                        // there must not be anything to append.
   if (0 == dwReturn)
   {
      return;
   }


#else // ifdef UNICODE

                                        // If not UNICODE, then we are just
                                        // going to want to copy it across into
                                        // the same variable that the UNICODE
                                        // guy had to translate into.

   STRCPY (szCustomMessage, grNavOptions.alert.szAlertMsg);

#endif // ifdef UNICODE


                                        // OK... Now add them all together!
   szReturn = szReturn + CString(_T("\n\n")) + CString(szCustomMessage);

}


/////////////////////////////////////////////////////////////////////
// void TrimAlertMessageToLength
//
// Purpose:
//    Makes sure that every line of the message is complete.  If
//    a line needs to be shortened,
//
// Parameters:
//   CString& szMessage    - [out] - Message to use
//   DWORD    dwMaxLength  - [in]  - Maximum length of string.
//
// Returns:
//   The string that is chopped for display (if necessary)
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/27/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void TrimAlertMessageToLength (
   CString& szMessage,                  // [out] - Message to use
   DWORD    dwMaxLength)
{
LPTSTR lpszPtr, lpszSearch;
DWORD  dwCurrentLength = 0;

   lpszPtr = szMessage.GetBuffer(
                0);                     // minimum buffer length

                                        // Find the first carriage return.
   lpszSearch = wcschr(lpszPtr, '\n');

   while (lpszSearch)
   {
      dwCurrentLength += (DWORD) (lpszSearch - lpszPtr);

                                        // If the current length is longer than
                                        // the max length, then we are done and
                                        // kill the string at the first pointer.
      if (dwCurrentLength > dwMaxLength)
      {
                                        // Terminate the string
         *lpszPtr = EOS;
                                        // And bail out of the loop
         break;
      }

                                        // Move along our marker
      lpszPtr = lpszSearch + 1;

                                        // And find the next carriage return.
      lpszSearch = wcschr(lpszPtr, '\n');
   }

                                        // Now that we have cut down the
                                        // string, it is time to reset the
                                        // CString object
   szMessage.ReleaseBuffer();
}


/////////////////////////////////////////////////////////////////////
// VOID BuildVirusMessage
//
// Purpose:
//    This function will build the virus message from what is passed
//    in on the NotifyPacket
//
// Parameters:
//    CVirusAlert * - lpcVirusAlert - [in]  - Virus found information
//    CString&      - szMessage     - [out] - Composed message
//    DWORD         - dwMaxLength   - [in]  - Max size to put in szMessage
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/20/96 - DSACKIN - Function Created.
// 01/16/97 - DSACKIN - Took this function out of CAlertService so
//                    - that it can be accessed by other objects
// 01/21/97 - TSMITH  - Added processing for domain name.
/////////////////////////////////////////////////////////////////////
VOID BuildVirusMessage(
   CVirusAlert *lpcVirusAlert,          // [in]  - Alert to parse
   CString& szMessage,                  // [out] - Message to use
   DWORD    dwMaxLength)                // [in]  - Max size to put in szMessage
{
CString szVirus;
CString szObject;
CString szSystem;
CString szDomain;
CString szUser;
CString szAction;
CString szFmtStr;
LPTSTR  lpszString;
DWORD   dwBufSize;
DWORD   dwAlertType;
DWORD   dwAction;
BOOL    bActionSuccess;

   try
   {
      lpcVirusAlert->GetVirusNameLength(
         dwBufSize);
                                        // Convert characters to byte length
      dwBufSize = (dwBufSize+1) * sizeof(TCHAR);
      lpszString = szVirus.GetBufferSetLength(
                      dwBufSize);
      lpcVirusAlert->GetVirusName(
         dwBufSize,
         lpszString);
      szVirus.ReleaseBuffer();

      lpcVirusAlert->GetObjectNameLength(
         dwBufSize);
                                        // Convert characters to byte length
      dwBufSize = (dwBufSize+1) * sizeof(TCHAR);
      lpszString = szObject.GetBufferSetLength(
                      dwBufSize);
      lpcVirusAlert->GetObjectName(
         dwBufSize,
         lpszString);
      szObject.ReleaseBuffer();

      lpcVirusAlert->GetSystemNameLength(
         dwBufSize);
                                        // Convert characters to byte length
      dwBufSize = (dwBufSize+1) * sizeof(TCHAR);
      lpszString = szSystem.GetBufferSetLength(
                      dwBufSize);
      lpcVirusAlert->GetSystemName(
         dwBufSize,
         lpszString);
      szSystem.ReleaseBuffer();

      lpcVirusAlert->GetDomainNameLength(
         dwBufSize);
                                        // Convert characters to byte length
      dwBufSize = (dwBufSize+1) * sizeof(TCHAR);
      lpszString = szDomain.GetBufferSetLength(
                      dwBufSize);
      lpcVirusAlert->GetDomainName(
         dwBufSize,
         lpszString);
      szDomain.ReleaseBuffer();

      lpcVirusAlert->GetUserNameLength(
         dwBufSize);
                                        // Convert characters to byte length
      dwBufSize = (dwBufSize+1) * sizeof(TCHAR);
      lpszString = szUser.GetBufferSetLength(
                      dwBufSize);
      lpcVirusAlert->GetUserName(
         dwBufSize,
         lpszString);
      szUser.ReleaseBuffer();

      lpcVirusAlert->GetAlertType(
         dwAlertType);

      lpcVirusAlert->GetAction(
         dwAction);

      lpcVirusAlert->GetActionSuccess(
         bActionSuccess);

      ComposeActionString(
         dwAlertType,
         dwAction,
         bActionSuccess,
         &szAction );

                                      // Select which format string to use
                                      // according to what kind of virus
                                      // event has occurred.
      switch (dwAlertType)
      {
         case FILE_KNOWN_VIRUS:
            GetResourceString(
               ghInst,
               IDS_ADMIN_MSG_KNOWN_FILE_INFECTOR,
               szFmtStr );
            break;

         case BOOT_RECORD_KNOWN_VIRUS:
            GetResourceString(
               ghInst,
               IDS_ADMIN_MSG_KNOWN_BR_INFECTOR,
               szFmtStr );
            break;

         case MBR_KNOWN_VIRUS:
            GetResourceString(
               ghInst,
               IDS_ADMIN_MSG_KNOWN_MBR_INFECTOR,
               szFmtStr );
            break;

         case FILE_UNKNOWN_VIRUS:
         case BOOT_RECORD_UNKNOWN_VIRUS:
         case MBR_UNKNOWN_VIRUS:
         default:
             SYM_ASSERT( FALSE );
             break;
      }

      szMessage.Format(
         szFmtStr,
         szVirus,
         szObject,
         szDomain,
         szSystem,
         szUser,
         szAction );

      AppendCustomMessage( szMessage );

      TrimAlertMessageToLength(
         szMessage,
         dwMaxLength);
   }
   catch( CMemoryException e )
   {
      throw( ErrMemAlloc );             // Translate CString exception to our own.
   }

}


/////////////////////////////////////////////////////////////////////
//  LogWrite
//
//  To write to Nav-Activity Log file of Virus Detection
//  by the NT AP Driver.
//
//  Parameters:
//   LPNAVAP_LOG lpNavApLog - [in] - Info to write to the log
//
//  Returns:
//      WORD
/////////////////////////////////////////////////////////////////////
// 04/25/96 PVENKAT
// 10/22/96 TSMITH  Ported from NAVAPSVC.
// 01/17/97 - DSACKIN - Moved from NTALOG.CPP
/////////////////////////////////////////////////////////////////////
WORD LogWrite(
   LPNAVAP_LOG lpNavApLog )             // [in] - Info to write to the log
{
   return NavApDoCommandActivityLog( lpNavApLog );
}


