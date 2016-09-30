/////////////////////////////////////////////////////////////////////
// MARCONI.CPP - Created - 05-20-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/marconi.cpv   1.1   21 Aug 1997 02:48:38   DSACKIN  $
//
// Description:
//    This file will hold the conversion routine from the old
//    style Marconi packets to the new style of TESLA packets.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/marconi.cpv  $
// 
//    Rev 1.1   21 Aug 1997 02:48:38   DSACKIN
// Added a line of code to specify that all Marconi Alerts came from NT boxes.
//
//    Rev 1.0   21 May 1997 13:42:14   DSACKIN
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
#include "apscomm.h"
#include "xapi.h"

#include "QueueMgr.h"
#include "AlertQ.h"
#include "MainMgr.h"
#include "Managers.h"

#include "AlerType.h"

#include "alertsvc.h"
#include "alertstr.h"

                                        // This header file is for the old
                                        // style of alerts from Marconi.
#include "marconi.h"



/////////////////////////////////////////////////////////////////////
// DWORD CAlertService::ProcessCAlertNotification(
//
// Purpose:
//    This function takes a packet from an old Marconi installation
//    and translates it into the new style of packet needed for
//    Tesla/Roswell.
//
// Parameters:
//   CAlert *lpcAlert - [in] - our old marconi style alert.
//
// Returns:
//    DWORD - Status from processing the alert.
//
/////////////////////////////////////////////////////////////////////
// 05-20-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CAlertService::ProcessCAlertNotification(
   CAlert *lpcAlert)                    // [in] - our old marconi style alert.
{
PMARCONI_PACKET         lpMarconiPacket;
PMARCONI_PACKET_STRING  lpPacketString;
CVirusAlert            *lpcVirusAlert;
LPWSTR                  lpszTemp;
DWORD                   dwReturn = ERROR_SUCCESS;


                                        // The scheme of moving things across
                                        // from Marconi to Tesla will be to
                                        // just grab the buffer and treat it as
                                        // a struct.  It is too cumbersom to
                                        // use the AlertNotification member
                                        // functions.

   lpcVirusAlert = new CVirusAlert;

                                        // Check for the validity of our packet.
   if (NULL == lpcVirusAlert)
   {
      LogMessage(_T("Failed to create the new CVirusAlert class."));
      goto Cleanup;
   }

                                        // OK.  Get the info from the old
                                        // packet!
   lpMarconiPacket = (PMARCONI_PACKET)
                       lpcAlert->GetPacket();


                                        // Now transfer.
   lpcVirusAlert->SetPacketType(
      lpMarconiPacket->PacketType);

   lpcVirusAlert->SetPacketSource(
      lpMarconiPacket->PacketSource);

                                        // We can assume that if we are
                                        // converting a packet that was sent by
                                        // Marconi that it is going to be of
                                        // type NT.
   lpcVirusAlert->SetPacketMachineType(
      PACKET_SOURCE_WINNT);

   lpcVirusAlert->SetUniqueID(
      lpMarconiPacket->UniqueID);

   lpcVirusAlert->SetTime(
      lpMarconiPacket->SystemTime);

   lpcVirusAlert->SetAction(
      lpMarconiPacket->Action);

   lpcVirusAlert->SetAlertType(
      lpMarconiPacket->AlertType);

   lpcVirusAlert->SetActionSuccess(
      lpMarconiPacket->ActionSuccess);

                                        // Now copy across the strings.
   if (0 != lpMarconiPacket->SystemNameOffset)
   {
      lpPacketString = (PMARCONI_PACKET_STRING)
                          (lpMarconiPacket + (lpMarconiPacket->SystemNameOffset));
      lpszTemp = &lpPacketString->String[0];
      lpcVirusAlert->SetSystemName(
         lpszTemp);
   }

   if (0 != lpMarconiPacket->UserNameOffset)
   {
      lpPacketString = (PMARCONI_PACKET_STRING)
                          (lpMarconiPacket + (lpMarconiPacket->UserNameOffset));
      lpszTemp = &lpPacketString->String[0];
      lpcVirusAlert->SetUserName(
         lpszTemp);
   }

   if (0 != lpMarconiPacket->ObjectNameOffset)
   {
      lpPacketString = (PMARCONI_PACKET_STRING)
                          (lpMarconiPacket + (lpMarconiPacket->ObjectNameOffset));
      lpszTemp = &lpPacketString->String[0];
      lpcVirusAlert->SetObjectName(
         lpszTemp);
   }

   if (0 != lpMarconiPacket->VirusNameOffset)
   {
      lpPacketString = (PMARCONI_PACKET_STRING)
                          (lpMarconiPacket + (lpMarconiPacket->VirusNameOffset));
      lpszTemp = &lpPacketString->String[0];
      lpcVirusAlert->SetVirusName(
         lpszTemp);
   }

   if (0 != lpMarconiPacket->DomainNameOffset)
   {
      lpPacketString = (PMARCONI_PACKET_STRING)
                          (lpMarconiPacket + (lpMarconiPacket->DomainNameOffset));
      lpszTemp = &lpPacketString->String[0];
      lpcVirusAlert->SetDomainName(
         lpszTemp);
   }


/////////////////////////////////////////////////////////////////////
// Add it to the queue and be done with it.


   if (cMainAlertQueueManager.AddAlert(
         lpcVirusAlert,                 // Send in the packet
         (CAlertTarget *) NULL))        // The Main Manager will handle this
   {
                                        // Get rid of our reference to it!  The
                                        // QueueManager will have to take care
                                        // of this when he is done.
      lpcVirusAlert = NULL;
   }
   else
   {
      LogMessage(_T("Failed call to cMainAlertQueueManager.AddAlert"));
   }

Cleanup:

                                        // Now release the packet.
   lpcAlert->ReleasePacket();

   if (NULL != lpcVirusAlert)
   {
      delete lpcVirusAlert;
      lpcVirusAlert = NULL;
   }

   return( dwReturn );

}

