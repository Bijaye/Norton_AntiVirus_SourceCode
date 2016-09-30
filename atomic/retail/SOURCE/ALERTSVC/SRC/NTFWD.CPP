/////////////////////////////////////////////////////////////////////
// NTFWD.CPP - Created - 11/17/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/ntfwd.cpv   1.2   09 Jun 1997 01:57:04   DSACKIN  $
//
// Description:
//    This file will contain the forwarding code that will be used
//    to get an alert packet from the current machine to the next one.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/ntfwd.cpv  $
// 
//    Rev 1.2   09 Jun 1997 01:57:04   DSACKIN
// We must clean up the lpcVirusAlert and lpcTarget when we are done.
//
//    Rev 1.1   18 May 1997 05:06:34   DSACKIN
// Switched over from CAlertNotification to CVirusAlert
//
//    Rev 1.0   18 Apr 1997 13:41:00   DSACKIN
// Ported from Tesla
//
//    Rev 1.1   24 Mar 1997 20:32:40   DSACKIN
// Changed the prototype of the ProcessWorkOrder function.
//
//    Rev 1.0   13 Mar 1997 15:06:08   RFULLER
// Initial revision
//
//    Rev 1.8   13 Feb 1997 20:59:14   TSmith
// Set flag in packets to be forwarded to PACKET_SOURCE_LOCAL, preserving
// the low word which indicates the originating app.
//
//    Rev 1.7   11 Feb 1997 15:51:50   DSACKIN
// On the forward if we have a double backslash on the front of the server
// name, we now will strip it out because it was failing the DNS lookup.
//
//    Rev 1.6   05 Feb 1997 15:40:08   DSACKIN
// Changed the number of used protocols to 11 instead of 12 because we
// are not going to use ncalrpc to try to send packets to remote machines.
//
//    Rev 1.5   05 Feb 1997 15:20:38   DSACKIN
// Added the ncacn_at_dsp protocol sequence.
//
//    Rev 1.4   23 Jan 1997 18:59:58   DSACKIN
// Don't forget to clean up our forwarding address list.
//
//    Rev 1.3   18 Jan 1997 07:00:08   DSACKIN
// Reworked the Alert Service to use the new CAlertQueueManager class for
// all alerting.
//
//    Rev 1.2   26 Nov 1996 06:21:36   DSACKIN
// Added dynamic protocol selection for the forwarding.
//
//    Rev 1.1   18 Nov 1996 18:02:32   DSACKIN
// Implemented basic forwarding.
//
//    Rev 1.0   18 Nov 1996 12:31:52   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#define _WIN32_WINNT 0x0400

/////////////////////////////////////////////////////////////////////
//      Includes
// #include <afx.h>
// #include <afxwin.h>
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

#include "AlertQ.h"
#include "Managers.h"

#include "alertsvc.h"
#include "alertstr.h"

#include "linkval.h"


/////////////////////////////////////////////////////////////////////
// Externs
extern HINSTANCE ghInst;


/////////////////////////////////////////////////////////////////////
// Define the protocol sequence - Unfortunately due to the fact that
// we are going to use the NAVXUTIL function to send our commands, we
// must use the protocol strings in a NON-UNICODE format.


                                        // Note the lack of ncalrpc.  This is
                                        // the local procedure call and would
                                        // not be useful in network transfer of
                                        // information
#define NUMBER_OF_USED_PROTOCOLS 11

CString szProtocols [NUMBER_OF_USED_PROTOCOLS] = {
          _T(PROTOCOL_NCACN_IP_TCP),
          _T(PROTOCOL_NCACN_NB_NB),
          _T(PROTOCOL_NCACN_NB_IPX),
          _T(PROTOCOL_NCACN_NB_TCP),
          _T(PROTOCOL_NCACN_SPX),
          _T(PROTOCOL_NCADG_IP_UDP),
          _T(PROTOCOL_NCADG_IPX),
          _T(PROTOCOL_NCACN_NP),
          _T(PROTOCOL_NCACN_DNET_NSP),
          _T(PROTOCOL_NCACN_VNS_SPP),
          _T(PROTOCOL_NCACN_AT_DSP)
};


/////////////////////////////////////////////////////////////////////
// BOOL CForwardAlertQueueManager::StartUp(
//
// Purpose:
//    This function gets us ready to accept forward work orders.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Successful Startup
//    FALSE - Startup was not successful.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/16/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CForwardAlertQueueManager::StartUp(void)
{
BOOL  bReturn = TRUE;
DWORD dwReturn;

                                        // Now load the dynamic keyword
   dwReturn = LoadString (
                 ghInst,
                 IDS_NAV_FORWARD_DYNAMIC,
                 m_szDynamic,
                 MAX_DYNAMIC_STRING_LENGTH );

   if (0 == dwReturn)
   {
      bReturn = FALSE;
   }

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// BOOL CForwardAlertQueueManager::ShutDown(
//
// Purpose:
//    This function removes all of our address pairs from our list.
//
// Parameters:
//    none.
//
// Returns:
//    TRUE  - Successful ShutDown
//    FALSE - ShutDown was not successful.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/23/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CForwardAlertQueueManager::ShutDown(void)
{
CForwardAddressProtocol *lpcAddressProtocol;

   while (!m_oblAddressProtocol.IsEmpty())
   {
      lpcAddressProtocol = (CForwardAddressProtocol *)
                              m_oblAddressProtocol.RemoveTail();

      if (NULL != lpcAddressProtocol)
         delete lpcAddressProtocol;
   }

   return TRUE;
}



/////////////////////////////////////////////////////////////////////
// DWORD CForwardAlertQueueManager::TryProtocol(
//
// Purpose:
//
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/18/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CForwardAlertQueueManager::TryProtocol(
   CVirusAlert *lpcVirusAlert,          // [in] - Alert to send
   CString&     cszAddress,             // [in] - Where to send it
   CString&     cszProtocol)            // [in] - What protocol to try
{
char  szAddress [ SYM_MAX_PATH + 1 ];
char  szProtocol[ SYM_MAX_PATH + 1 ];
PBYTE pBuffer;
DWORD dwReturn;
DWORD dwPacketSrc;

                                        // Set the packet source to FORWARD,
                                        // preserving the low word.
   if (!lpcVirusAlert->GetPacketSource(
           dwPacketSrc))
   {
      goto Cleanup;
   }

   dwPacketSrc = LOWORD (dwPacketSrc) | PACKET_SOURCE_FORWARD;

                                        // Plug the mofified source into packet.
   lpcVirusAlert->SetPacketSource( dwPacketSrc );

                                        // We need ASCII String for the Target
   WideCharToMultiByte(
      CP_ACP,
      WC_COMPOSITECHECK,
      cszAddress,
      -1,
      szAddress,
      sizeof (szAddress),
      NULL,
      NULL );

                                        // We need ASCII String for the Protocol
   WideCharToMultiByte(
      CP_ACP,
      WC_COMPOSITECHECK,
      cszProtocol,
      -1,
      szProtocol,
      sizeof (szProtocol),
      NULL,
      NULL );


                                        // Get access to raw bytes
   pBuffer = lpcVirusAlert->GetPacket();

                                        // Use dynamic endpoint resolution
   dwReturn = NavRpcSendPacket(
                 szProtocol,
                 szAddress,
                 NULL,
                 ( LPVOID )pBuffer );

                                        // Don't forget to let go of the buffer
                                        // when we are done.
   lpcVirusAlert->ReleasePacket();

Cleanup:

   return dwReturn;
}



/////////////////////////////////////////////////////////////////////
// DWORD CForwardAlertQueueManager::TryDynamic(
//
// Purpose:
//
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 01/18/97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CForwardAlertQueueManager::TryDynamic(
   CVirusAlert *lpcVirusAlert,          // [in] - Alert to send
   CString&     cszAddress)             // [in] - Where to send it
{
CForwardAddressProtocol *pcAddressProtocol;
CString                  cszTempAddress;
POSITION                 rPos;
DWORD                    dwReturn;
BOOL                     bAddRecord = TRUE;
int                      nCurrentProtocol;

                                        // First thing we need to do is see if
                                        // we have the address already in the
                                        // list.

                                        // First check for an empty list
   if (!m_oblAddressProtocol.IsEmpty())
   {
                                        // Now iterate through looking for the
                                        // correct one
      for ( rPos = m_oblAddressProtocol.GetHeadPosition();
            (rPos != NULL);
            )
      {
         pcAddressProtocol = (CForwardAddressProtocol *) (m_oblAddressProtocol.GetNext( rPos ));

         pcAddressProtocol->GetAddress(cszTempAddress);

                                        // If we find it, set the add flag to
                                        // false and skip out of this loop
         if (cszTempAddress == cszAddress)
         {
            bAddRecord = FALSE;
            break;
         }
      }

   }

   if (bAddRecord)
   {
      pcAddressProtocol = new CForwardAddressProtocol (cszAddress);
      m_oblAddressProtocol.AddHead(pcAddressProtocol);
   }

                                        // At this point, pcAddressProtocol is
                                        // going to be pointing at a record we
                                        // need to examine for dynamic protocol
   nCurrentProtocol = pcAddressProtocol->GetProtocol();
   pcAddressProtocol->GetAddress(cszTempAddress);

   do
   {
      dwReturn = TryProtocol(
                    lpcVirusAlert,
                    cszTempAddress,
                    szProtocols[nCurrentProtocol]);

                                        // Increment to the next protocol if we
                                        // fail.
      if (LINK_ERROR == dwReturn)
      {
         nCurrentProtocol = (nCurrentProtocol + 1) % NUMBER_OF_USED_PROTOCOLS;
      }
   }
   while ( (LINK_ERROR == dwReturn) &&
           (nCurrentProtocol != pcAddressProtocol->GetProtocol()) );

                                        // Now if we were able to send the
                                        // command, save our new protocol again.
   if (LINK_ERROR != dwReturn)
   {
      LogMessage (_T("Packet was forwarded using the protocol number:%d."), nCurrentProtocol);

      pcAddressProtocol->SetProtocol(nCurrentProtocol);
   }
   else
   {
      LogMessage (_T("Unable to forward packet."));
   }

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// VOID CForwardAlertQueueManager::ProcessWorkOrder(
//
// Purpose:
//    This function takes a work order and forwards the needed information
//    to the remote machine.
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
VOID CForwardAlertQueueManager::ProcessWorkOrder(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
CForwardTarget    *lpcForwardTarget;
CString            cszProtocol;
CString            cszAddress;
DWORD              dwReturn;

   LogMessage( _T("ProcessWorkOrder for Forward Target.") );

   lpcForwardTarget = (CForwardTarget *)lpcTarget;

   cszAddress  = lpcForwardTarget->GetAddress();
   cszProtocol = lpcForwardTarget->GetProtocol();

                                        // Now make sure that the server name
                                        // that is coming through does NOT have
                                        // double backslashes on it.
   if (0 == cszAddress.Find ( _T("\\\\") ) )
   {
                                        // If we found the double backslash at
                                        // space 0, we need to trim it down to
                                        // just the server name.
      cszAddress = cszAddress.Right ( cszAddress.GetLength() - 2 );
   }


                                        // Check to see if they have a specific
                                        // protocol
   if (STRCMP(cszProtocol, m_szDynamic))
   {
                                        // If they have a specific protocol,
                                        // only try to use that protocol and no
                                        // others.
      dwReturn = TryProtocol(
                    lpcVirusAlert,
                    cszAddress,
                    cszProtocol);

      if (LINK_ERROR == dwReturn)
      {
         LogMessage(
            _T("Unable to alert to the machine - non-dynamic protocol"));
      }
   }
   else
   {
                                        // OK ... Here we know that we want to
                                        // try the dynamic protocol stuff
      TryDynamic(
         lpcVirusAlert,
         cszAddress);
   }

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




/////////////////////////////////////////////////////////////////////
// First take care of all of the member functions that we are going
// to need for the CForwardItem class


/////////////////////////////////////////////////////////////////////
// Now for the CForwardAddressProtocol object definitions

/////////////////////////////////////////////////////////////////////
// CForwardAddressProtocol::CForwardAddressProtocol
//
// Purpose:
//    Constructor for our object
//
// Parameters:
//    cszAddress - [in] - Address we want to associate
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/26/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CForwardAddressProtocol::CForwardAddressProtocol(
         CString cszAddress)
{
   m_cszAddress = cszAddress;

                                        // Now start the protocol selection at
                                        // the top
   m_nProtocol  = 0;
}


/////////////////////////////////////////////////////////////////////
// CForwardAddressProtocol::GetAddress
//
// Purpose:
//    Get the current Address
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/26/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CForwardAddressProtocol::SetAddress(
   CString cszAddress)
{
   m_cszAddress = cszAddress;
}

/////////////////////////////////////////////////////////////////////
// void CForwardAddressProtocol::GetAddress
//
// Purpose:
//    This allows us to get the address of each member.
//
// Parameters:
//    CString& cszAddress - [out] - address is returned
//
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/26/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CForwardAddressProtocol::GetAddress(
      CString& cszAddress)
{
   cszAddress = m_cszAddress;
}



/////////////////////////////////////////////////////////////////////
// void CForwardAddressProtocol::SetProtocol (
//
// Purpose:
//    This allows us to save the new protocol
//
// Parameters:
//    int nProtocol - [in] - new protocol
//
// Returns:
//    none.
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/26/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CForwardAddressProtocol::SetProtocol (
      int nProtocol)
{
   m_nProtocol = nProtocol;
}

/////////////////////////////////////////////////////////////////////
// void CForwardAddressProtocol::GetProtocol (
//
// Purpose:
//    Gets the current a
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 11/26/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
int CForwardAddressProtocol::GetProtocol (void)
{
   return m_nProtocol;
}


