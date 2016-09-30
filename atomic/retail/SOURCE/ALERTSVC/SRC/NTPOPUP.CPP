/////////////////////////////////////////////////////////////////////
// NTPOPUP.CPP - Created - 11/13/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntpopup.cpv   1.5   07 Aug 1997 16:49:46   DALLEE  $
//
// Description:
//    This file contains the instructions for the LocalPopupNotification
//    functionality.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntpopup.cpv  $
// 
//    Rev 1.5   07 Aug 1997 16:49:46   DALLEE
// Call SetShowMsg() to reset already shown flag after displaying long 
// custom message. Used to display message only on the first call.
// 
//    Rev 1.4   16 Jul 1997 20:26:28   DSACKIN
// Where did the dwPacketSource variable come from???
//
//    Rev 1.3   19 Jun 1997 11:57:20   TIVANOV
// a new function call to bring the long custom alert
//
//    Rev 1.2   09 Jun 1997 01:57:08   DSACKIN
// We must clean up the lpcVirusAlert and lpcTarget when we are done.
//
//    Rev 1.1   18 May 1997 05:07:08   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:06   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:32:38   DSACKIN
// Changed the prototype of the ProcessWorkOrder function.
//
//    Rev 1.0   13 Mar 1997 15:06:10   RFULLER
// Initial revision
//
//    Rev 1.7   13 Feb 1997 21:12:04   TSmith
// Corrected test of packet source flags.
//
//    Rev 1.6   12 Feb 1997 17:08:34   TSmith
// Added code to get the packet source from packet & not pop up a message if
// the packet was sent by navw.
//
//    Rev 1.5   18 Jan 1997 07:01:18   DSACKIN
// Reworked the AlertSvc to use the CAlertQueueManager object to handle all
// alerting.
//
//    Rev 1.4   22 Dec 1996 15:53:48   TSmith
// Changed calls to CAlertNotification string retrieval functions to support
// that class's new function arguments.
//
//    Rev 1.3   17 Dec 1996 14:07:10   DSACKIN
// MB_SERVICE_NOTIFICATION flag is different between NT351 and NT40.
// We now reflect this.
//
//    Rev 1.2   26 Nov 1996 07:15:20   DSACKIN
// Added a LocalPopupManager
//
//    Rev 1.1   20 Nov 1996 22:13:40   DSACKIN
// Use the global BuildMessage function now instead of building
// a custom message each time.
//
//    Rev 1.0   13 Nov 1996 22:13:32   DSACKIN
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
#include "navutil.h"

/////////////////////////////////////////////////////////////////////
// Defines

                                        // These values were taken from
                                        // winuser.h.  Unfortunately, you can
                                        // only have one of the values defined
                                        // at one time, so I have appended the
                                        // version to the end

#define MB_SERVICE_NOTIFICATION_40     0x00200000L
#define MB_SERVICE_NOTIFICATION_35     0x00040000L

/////////////////////////////////////////////////////////////////////
// External Variables
extern NAVOPTIONS      grNavOptions;    // For use with our C functions



/////////////////////////////////////////////////////////////////////
// VOID CLocalPopupAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and displays a local popup
//    on the screen.
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
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID CLocalPopupAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
DWORD              dwMessageBoxFlags;
OSVERSIONINFO      rOSVersion;
BOOL               bNT40 = TRUE;
DWORD              dwPacketSrc;

CString            szMsg;

   LogMessage( _T("ProcessWorkOrder for LocalPopup.") );

                                        // Get & check the source of the packet.
   if (!lpcVirusAlert->GetPacketSource(
           dwPacketSrc) )
   {
      goto Cleanup;
   }

                                        // If the packet was originated by a copy
                                        // of NAVWNT running on the local system
                                        // we skip logging because it does it for
                                        // itself.
    if ( ( ( dwPacketSrc & PACKET_SOURCE_NAVW ) &&
           ( dwPacketSrc & PACKET_SOURCE_LOCAL ) ) )
    {
      goto Cleanup;
    }

                                        // Display long custom message.
                                        // Horrible mess with LCM(), need to
                                        // toggle already shown flag to make
                                        // it display a second time.
    LongCustomMessage(NULL);
    SetShowMsg();

                                            // We need to figure out what version
                                            // of NT we are running on in order to
                                            // put up the message box correctly

    dwMessageBoxFlags = MB_OK | MB_ICONEXCLAMATION;

    if ( GetVersionEx(&rOSVersion) &&
            (rOSVersion.dwMajorVersion >= 4) )
    {
        dwMessageBoxFlags |= MB_SERVICE_NOTIFICATION_40;
    }
    else
    {
                                            // If something fails, we should use
                                            // the old value for
                                            // MB_SERVICE_NOTIFICATION

        dwMessageBoxFlags |= MB_SERVICE_NOTIFICATION_35;
    }


                                            // First, we must interpret the Alert
                                            // before we can put up the message box.
    BuildVirusMessage(
        lpcVirusAlert,
        szMsg);


                                            // Check to see if we should beep
    if ( grNavOptions.alert.bBeep )
    {
                                            // Get the users attention
        MessageBeep( MB_ICONEXCLAMATION );
    }

                                            // Popup the message box. This will
                                            // appear even if no one is logged
                                            // in at the local system.
                                            //
                                            // If this behavior is not desired,
                                            // change MB_SERVICE_NOTIFICATION to
                                            // MB_DEFAULT_DESKTOP_ONLY and remove
                                            // the error check. The MessageBox
                                            // call will then fail if no on is
                                            // logged on.
    MessageBox(
        NULL,
        szMsg,
        _T( "Norton AntiVirus" ),
        dwMessageBoxFlags );


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


