/////////////////////////////////////////////////////////////////////
// NTMSNGR.CPP - Created - 11/13/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntmsngr.cpv   1.1   18 May 1997 05:07:04   DSACKIN  $
//
// Description:
//    This is where we will have the NetMessnger handling.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntmsngr.cpv  $
// 
//    Rev 1.1   18 May 1997 05:07:04   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:02   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:33:14   DSACKIN
// Changed the prototype of the ProcessWorkOrder function.
//
//    Rev 1.0   13 Mar 1997 15:06:08   RFULLER
// Initial revision
//
//    Rev 1.4   15 Feb 1997 18:05:52   DSACKIN
// If we have a double backslash at the beginning of out target name, we
// need to strip it off as it is not valid for the net messenger.
//
//    Rev 1.3   28 Jan 1997 04:08:52   DSACKIN
// We now restrict the Network Broadcast messages to 122 characters long
//
//    Rev 1.2   18 Jan 1997 07:00:10   DSACKIN
// Reworked the Alert Service to use the new CAlertQueueManager class for
// all alerting.
//
//    Rev 1.1   20 Nov 1996 22:14:14   DSACKIN
// Moved the message building into a function called BuildMessage in util.cpp
//
//    Rev 1.0   13 Nov 1996 22:13:30   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////
//      Includes
#include "afxtempl.h"
#include "platform.h"
#include "process.h"
#include "winnetwk.h"
#include "lmcons.h"
#include "lmmsg.h"
#include "lmerr.h"
#include "servenfy.h"
#include "linksrv.h"
#include "service.h"
#include "svcctrl.h"
#include "navrpc.h"
#include "xapi.h"
                                        // These includes are for the
                                        // AlertManager objects
#include "AlertQ.h"
#include "Managers.h"

#include "alertsvc.h"
#include "alertstr.h"

/////////////////////////////////////////////////////////////////////
// Externs
extern HINSTANCE ghInst;


/////////////////////////////////////////////////////////////////////
// BOOL CRemotePopupAlertQueueManager::InitializeNetMessenger(
//
// Purpose:
//    This function does the work to load the NetMessenger function
//    from the DLL.  This must be done before we can send NetMessenger
//    alerts.
//
// Parameters:
//    none.
//
// Returns:
//   TRUE  - NetMessenger function has been loaded successfully.
//   FALSE - Something didn't work as expected
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/17/97 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CRemotePopupAlertQueueManager::StartUp(void)
{
TCHAR szNetApiDLL[SYM_MAX_PATH + 1];
BOOL  bReturn = TRUE;
DWORD dwReturn;
DWORD dwSize;

   LogMessage( _T( "InitializeNetMessenger - Starting initialization" ) );

   dwSize = sizeof (m_szComputer);

   if (!GetComputerName (m_szComputer, &dwSize))
   {
      LogMessage (_T("InitializeNetMessenger - *** Failed call to GetComputerName"));

                                        // *** Failed call to GetComputerName
      bReturn = FALSE;

      m_szComputer[0] = EOS;

      goto Cleanup;
   }

                                        // Now load the NetApi DLL for use
                                        // while servicing calls.
   dwReturn = LoadString (
                 ghInst,
                 IDS_NAV_ALERT_NETAPI,
                 szNetApiDLL,
                 sizeof (szNetApiDLL));

   if (0 == dwReturn)
   {
      LogMessage ( _T("InitializeNetMessenger - *** Failed call to LoadString"));

                                        // *** Failed call to LoadString
      bReturn = FALSE;

      goto Cleanup;
   }
                                        // Load the actual DLL
   m_hNetApi32 = LoadLibrary (
                    szNetApiDLL);

   if (NULL == m_hNetApi32)
   {
      LogMessage ( _T("InitializeNetMessenger - *** Failed call to LoadLibrary") );

      bReturn = FALSE;

      goto Cleanup;
   }

                                       // There is no UNICODE version
                                       // of GetProcAddress
   m_lpfnNetMessageBufferSend = (LPNETMESSAGEBUFFERSEND_FCN) GetProcAddress (
                                   m_hNetApi32,
                                   "NetMessageBufferSend");

   if (NULL == m_lpfnNetMessageBufferSend)
   {
      LogMessage (_T("InitializeNetMessenger - *** Failed call to GetProcAddress") );

      bReturn = FALSE;

      goto Cleanup;
   }

Cleanup:

   LogMessage(_T("InitializeNetMessenger - Exiting with status of %d"), (DWORD) bReturn);

   return bReturn;

}


/////////////////////////////////////////////////////////////////////
// BOOL CRemotePopupAlertQueueManager::UnloadNetMessenger(void)
//
// Purpose:
//    This cleans up when it is time to exit
//
// Parameters:
//    none
//
// Returns:
//    none
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/17/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CRemotePopupAlertQueueManager::ShutDown(void)
{
   LogMessage( _T("UnloadNetMessenger - Unloading") );

                                        // First off, set the function pointer
                                        // to NULL so that it can't be used any
                                        // more
   m_lpfnNetMessageBufferSend = NULL;

                                        // Only free the library if we have a
                                        // handle to it.
   if (NULL != m_hNetApi32)
   {
                                        // Get rid of the Library
      FreeLibrary(m_hNetApi32);

                                        // Clean up our pointer.
      m_hNetApi32 = NULL;
   }

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// VOID CRemotePopupAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and calls NetMessenger to display
//    a message on a remote machine.
//
// Parameters:
//   CVirusAlert  *lpcAlert  - [in] - Information about the alert
//   CAlertTarget *lpcTarget - [in] - Where to send it.
//
// Returns:
//   none
//
// Notes:
//   We are responsible here for deleting the lpcTarget when we
//   are done with it.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////

#define MAX_BROADCAST_LENGTH 122

VOID CRemotePopupAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
NET_API_STATUS naStatus;
CString        szMsg;
LPTSTR         pszTarget;
TCHAR          szErrVal[ 10 + 1 ];
CString        szTarget;
LPTSTR         lpszTemp;
DWORD          dwMaxLength;

   LogMessage( _T("ProcessWorkOrder for NetMessenger Target.") );

   if (NULL == m_lpfnNetMessageBufferSend)
   {
      LogMessage ( _T("m_lpfnNetMessageBufferSend was not initialized.") );
      goto Cleanup;
   }

   MEMSET( szErrVal, 0, sizeof( szErrVal ) );

                                        // Save a local copy of the target
                                        // system name so we can provide a
                                        // LPTSTR rather than a LPCTSTR to
                                        // NetMessageBufferSend.
   szTarget = (LPCTSTR) (((CNetMsgTarget *)lpcTarget)->GetAddress());

                                        // Now make sure that the server name
                                        // that is coming through does NOT have
                                        // double backslashes on it.
   if (0 == szTarget.Find ( _T("\\\\") ) )
   {
                                        // If we found the double backslash at
                                        // space 0, we need to trim it down to
                                        // just the server name.
      szTarget = szTarget.Right ( szTarget.GetLength() - 2 );
   }


   lpszTemp = szTarget.GetBuffer(0);

   if (wcschr(lpszTemp, '*'))
      dwMaxLength = MAX_BROADCAST_LENGTH;
   else
      dwMaxLength = 0xFFFFFFFF;

   szTarget.ReleaseBuffer();

   BuildVirusMessage(
      lpcVirusAlert,
      szMsg,
      dwMaxLength);

   pszTarget = szTarget.GetBuffer( szTarget.GetLength( ) );

   naStatus = m_lpfnNetMessageBufferSend(
                  NULL,
                  pszTarget,
                  m_szComputer,
                  ( LPBYTE )szMsg.GetBuffer( szMsg.GetLength( ) ),
                  ( szMsg.GetLength( ) + 1 ) * sizeof( TCHAR ) );

   szTarget.ReleaseBuffer( );
   szMsg.ReleaseBuffer( );
   pszTarget = NULL;

                                        // Check status of message transmission
                                        // to determine an error message if
                                        // necessary. These error codes are
                                        // defined in LMERR.H
   switch ( naStatus )
   {
      case NERR_Success:
         LogMessage (_T("NetMessenger message sent successfully."));
         break;

      case NERR_NameNotFound:
         LogMessage (_T("NetMessenger - Name Not Found"));
         break;

      case NERR_PausedRemote:
         LogMessage (_T("NetMessenger - Remote NetMessenger paused."));
         break;

      case NERR_BadReceive:
         LogMessage (_T("NetMessenger - Bad Receive"));
         break;

      default:
         LogMessage (_T("NetMessenger - Error Code %d"), naStatus);
         break;
   }

Cleanup:

   return;
}


