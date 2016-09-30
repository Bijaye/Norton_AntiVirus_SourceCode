/////////////////////////////////////////////////////////////////////
// NTMAIL.CPP - Created - 11/17/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntmail.cpv   1.2   09 Jun 1997 01:57:06   DSACKIN  $
//
// Description:
//    This will contain the code for the e-mail alerting for the
//    ALERTSVC program.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntmail.cpv  $
// 
//    Rev 1.2   09 Jun 1997 01:57:06   DSACKIN
// We must clean up the lpcVirusAlert and lpcTarget when we are done.
//
//    Rev 1.1   18 May 1997 05:06:52   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:40:52   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:32:20   DSACKIN
// Changed the prototype of the ProcessWorkOrder function.
//
//    Rev 1.0   13 Mar 1997 15:06:08   RFULLER
// Initial revision
//
//    Rev 1.4   18 Jan 1997 07:00:08   DSACKIN
// Reworked the Alert Service to use the new CAlertQueueManager class for
// all alerting.
//
//    Rev 1.3   26 Nov 1996 06:42:02   DSACKIN
// Added SMTP PostOffice Configuration.
//
//    Rev 1.2   21 Nov 1996 18:14:16   DSACKIN
// Loading strings for Subject and Sender
//
//    Rev 1.1   21 Nov 1996 01:27:44   DSACKIN
// ReWorked the SendMailTarget function to be an EMail setup function.
//
//    Rev 1.0   18 Nov 1996 12:32:20   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////
//      Includes
// #include <afx.h>
// #include <afxwin.h>
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

#include "AlertQ.h"
#include "Managers.h"

#include "alertsvc.h"
#include "alertstr.h"
                                        // Include for SMTP Mail
#include "CSMTP.h"

/////////////////////////////////////////////////////////////////////
// Defines
#define MAX_RES_STR_LEN       (1024)


/////////////////////////////////////////////////////////////////////
// VOID CSMTPAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and sends an e-mail to the
//    sepcified recipient
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
// 01/18/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CSMTPAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
CAlertOptions   AlertOptions;
CSMTPMail       cSMTPMail;
CString         szMessage;

   LogMessage( _T("ProcessWorkOrder for SMTP Alert.") );

                                        // We will need the options to continue
   if (!AlertOptions.LoadOptions())
   {
      LogMessage (_T("Failed to load the options."));
                                        // Failed to get the options.
      goto Cleanup;
   }

                                        // Start by zapping the EMail guy and
                                        // building the new information into him
   cSMTPMail.Reset();

   BuildVirusMessage(
      lpcVirusAlert,
      szMessage);

                                        // First off, we have to tell him which
                                        // postoffice to use to send the mail
   cSMTPMail.SetPostOffice(
                AlertOptions.GetSMTPPostOffice());

                                        // Now we tell him what message we want
                                        // sent
   cSMTPMail.SetMessage(
      szMessage);

                                        // Who is it From?
   cSMTPMail.SetSender(
      AlertOptions.GetSMTPAuthor());

                                        // What is the subject?
   cSMTPMail.SetSubject(
      AlertOptions.GetSMTPSubject());

                                        // Who do se send it to?
   cSMTPMail.AddRecipient(
      ((CEMailTarget *)lpcTarget)->GetAddress());

                                        // OK! Now Send it!
   cSMTPMail.Send();

Cleanup:

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


   return;
}


