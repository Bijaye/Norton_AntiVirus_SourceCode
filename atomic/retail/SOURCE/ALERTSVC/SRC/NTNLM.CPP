/////////////////////////////////////////////////////////////////////
// NTNLM.CPP - Created - 11/13/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntnlm.cpv   1.3   19 Jun 1997 14:16:12   DSACKIN  $
//
// Description:
//    This file contains the code necessary to notify the NLM product.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntnlm.cpv  $
// 
//    Rev 1.3   19 Jun 1997 14:16:12   DSACKIN
// When sending up a char * buffer to GetUserName, we must explicitly call
// the GetUserNameA function.
//
//    Rev 1.2   09 Jun 1997 01:57:08   DSACKIN
// We must clean up the lpcVirusAlert and lpcTarget when we are done.
//
//    Rev 1.1   18 May 1997 05:07:08   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:04   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:32:42   DSACKIN
// Changed the prototype of the ProcessWorkOrder function.
//
//    Rev 1.0   13 Mar 1997 15:06:08   RFULLER
// Initial revision
//
//    Rev 1.4   16 Feb 1997 05:42:18   DSACKIN
// Added code to remove a double backslash from the target if it is
// at the start of the machine name.  The NLM alerting code needs this
// removed in order to function correctly.
//
//    Rev 1.3   18 Jan 1997 07:00:40   DSACKIN
// Reworked the AlertSvc to use the CAlertQueueManager object to handle all
// alerting.
//
//    Rev 1.2   22 Dec 1996 15:52:28   TSmith
// Changed calls to CAlertNotification string retrieval functions to support
// that class's new function arguments.
//
//    Rev 1.1   18 Nov 1996 12:33:12   DSACKIN
// Changed the actual NLM call to a function that takes the server we are
// trying to notify.  This gets us away from having the NLM notification
// guy pull the actual server name out of the standard DAT file.
//
//    Rev 1.0   13 Nov 1996 22:13:30   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


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
// BOOL CNavNlmAlertQueueManager::StartUp(void)
//
// Purpose:
//    This function takes care of starting up the NLM Alerting
//    manager.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Startup was successful
//    FALSE - Startup failed.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/17/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNavNlmAlertQueueManager::StartUp(void)
{
BOOL bReturn;

   bReturn = InitNLMMessenger();

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// VOID CNavNlmAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and alerts a Nav NLM target
//
// Parameters:
//   CVirusAlert  *lpcVirusAlert  - [in] - Information about the alert
//   CAlertTarget *lpcTarget      - [in] - Where to send it.
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
// 01/17/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CNavNlmAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
char    szUser[ SIZE_NET_USER_LIST + 1 ];
char    szVirus[ SIZE_VIRUSNAME + 1 ];
char    szPath[ SYM_MAX_PATH + 1 ];
char    szServer[ SYM_MAX_PATH + 1 ];
CString szTarget;

   LogMessage( _T("ProcessWorkOrder for Nav NLM Target.") );

   MEMSET( &szUser, 0, sizeof( szUser ) );
   MEMSET( &szVirus, 0, sizeof( szVirus ) );
   MEMSET( &szPath, 0, sizeof( szPath ) );

                                        // Explicitly call the A version of the
                                        // function!
   lpcVirusAlert->GetUserNameA(
      sizeof(szUser),
      szUser );

   lpcVirusAlert->GetVirusName(
      sizeof(szVirus),
      szVirus);

   lpcVirusAlert->GetObjectName(
      sizeof(szPath),
      szPath);

   szTarget = ((CNLMTarget *)lpcTarget)->GetAddress();
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


                                        // We need ASCII String for Server name
   WideCharToMultiByte(
      CP_ACP,
      WC_COMPOSITECHECK,
      szTarget,
      -1,
      szServer,
      sizeof (szServer),
      NULL,
      NULL );

   OemToAnsi(
      szVirus,
      szVirus );

   NotifyNAVNLMServer(
      szVirus,
      szPath,
      szUser,
      szServer );


                                        // If we had alerts or targets, this is
                                        // the place to delete them.
   if (lpcVirusAlert)
   {
      delete lpcVirusAlert;
      lpcVirusAlert = NULL;
   }

   if (lpcTarget)
   {
      delete lpcTarget;
      lpcTarget = NULL;
   }



}


