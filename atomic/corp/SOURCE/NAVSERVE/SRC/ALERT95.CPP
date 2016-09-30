/////////////////////////////////////////////////////////////////////
// ALERT95.CPP - Created - 06-18-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVSERVE/VCS/alert95.cpv   1.1   19 Jun 1997 17:11:24   DSACKIN  $
//
// Description:
//    This file will contain the code that will allow the
//    Windows 95 platform to alert to NT servers as well as NLM Servers.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVSERVE/VCS/alert95.cpv  $
// 
//    Rev 1.1   19 Jun 1997 17:11:24   DSACKIN
// For the forward packets, strip the double backslash on target addresses.
//
//    Rev 1.0   18 Jun 1997 21:18:44   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"

#include "navutil.h"
#include "servenfy.h"

#include "at_opts.h"

#include "CAlert.h"
#include "CVAlert.h"


/////////////////////////////////////////////////////////////////////
// VOID ProcessNLMTarget(
//
// Purpose:
//    This will take a CVirusAlert and send an NLM Alert to the
//    requested CAlertTarget
//
// Parameters:
//    CVirusAlert  *lpcVirusAlert - [in] - Information about the alert
//    CAlertTarget *lpcTarget     - [in] - Where to send it.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 06-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID ProcessNLMTarget(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
char    szUser[ SIZE_NET_USER_LIST + 1 ];
char    szVirus[ SIZE_VIRUSNAME + 1 ];
char    szPath[ SYM_MAX_PATH + 1 ];
CString szServer;
LPTSTR  lpszServer;

   lpcVirusAlert->GetUserName(
      sizeof(szUser),
      szUser );

   lpcVirusAlert->GetVirusName(
      sizeof(szVirus),
      szVirus);

   lpcVirusAlert->GetObjectName(
      sizeof(szPath),
      szPath);

   szServer = ((CNLMTarget *)lpcTarget)->GetAddress();

                                        // Now make sure that the server name
                                        // that is coming through does NOT have
                                        // double backslashes on it.
   if (0 == szServer.Find ( _T("\\\\") ) )
   {
                                        // If we found the double backslash at
                                        // space 0, we need to trim it down to
                                        // just the server name.
      szServer = szServer.Right ( szServer.GetLength() - 2 );
   }


   OemToAnsi(
      szVirus,
      szVirus );

   lpszServer = szServer.GetBuffer (szServer.GetLength() +1);

   NotifyNAVNLMServer(
      szVirus,
      szPath,
      szUser,
      lpszServer );

   szServer.ReleaseBuffer();
}


/////////////////////////////////////////////////////////////////////
// VOID ProcessForwardTarget(
//
// Purpose:
//    This will take a CVirusAlert and send a Forward Alert to the
//    requested CAlertTarget
//
// Parameters:
//    CVirusAlert  *lpcVirusAlert - [in] - Information about the alert
//    CAlertTarget *lpcTarget     - [in] - Where to send it.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 06-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
VOID ProcessForwardTarget(
   CVirusAlert  *lpcVirusAlert,         // [in] - Information about the alert
   CAlertTarget *lpcTarget)             // [in] - Where to send it.
{
CForwardTarget    *lpcForwardTarget;
CString            cszProtocol;
CString            cszAddress;
LPTSTR             lpszProtocol;
LPTSTR             lpszAddress;
PBYTE              pBuffer;
DWORD              dwPacketSrc;

                                        // Set the packet source to FORWARD,
                                        // preserving the low word.
   if (!lpcVirusAlert->GetPacketSource(
           dwPacketSrc))
   {
      goto Cleanup;
   }

                                        // Zero the top word of the new
                                        // PacketSource and OR in the flag that
                                        // we want.
   dwPacketSrc = LOWORD (dwPacketSrc) | PACKET_SOURCE_FORWARD;

                                        // Plug the mofified source into packet.
   lpcVirusAlert->SetPacketSource( dwPacketSrc );

                                        // Now get the info out of the Forward
                                        // target that we need.
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


                                        // Now we must get the pointers to work
                                        // with in NavRpcSendPacket
   lpszAddress  = cszAddress.GetBuffer  (cszAddress.GetLength()  +1);
   lpszProtocol = cszProtocol.GetBuffer (cszProtocol.GetLength() +1);

                                        // Get access to raw bytes
   pBuffer = lpcVirusAlert->GetPacket();

                                        // Use dynamic endpoint resolution
   NavRpcSendPacket(
      lpszProtocol,
      lpszAddress,
      NULL,
      ( LPVOID )pBuffer );

                                        // Locked buffers must always be
                                        // released.
   cszAddress.ReleaseBuffer();
   cszProtocol.ReleaseBuffer();

                                        // Don't forget to let go of the buffer
                                        // when we are done.
   lpcVirusAlert->ReleasePacket();

Cleanup:
   return;
}


/////////////////////////////////////////////////////////////////////
// BOOL SYM_EXPORT WINAPI ProcessCVirusAlert(
//
// Purpose:
//    This function will process an alert from either NAVW32 or
//    NAVAPW32 for the 95 platform
//
// Parameters:
//    CVirusAlert *lpcVirusAlert - [in] - Virus Alert to process
//
// Returns:
//    TRUE  - Alerts were sent
//    FALSE - Alerts were not sent.
//
/////////////////////////////////////////////////////////////////////
// 06-18-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI ProcessCVirusAlert(
   CVirusAlert *lpcVirusAlert)          // [in] - Alert Information
{
CAlertOptions       cAlertOptions;
CAlertTarget       *lpcTarget;
DWORD               dwTargetType;
BOOL                bReturn = FALSE;

                                        // We can't do our work if there is no
                                        // work to do.
   if (NULL == lpcVirusAlert)
   {
      goto Cleanup;
   }

                                        // Ok... Now find out what to do with
                                        // the alert.
   if (!cAlertOptions.LoadOptions())
   {
      goto Cleanup;
   }

   while (NULL != (lpcTarget = cAlertOptions.GetNextTarget()))
   {

      if (!lpcTarget->GetActive())
      {
                                        // If this target isn't active, we
                                        // won't process it.
         continue;
      }

                                        // OK, which manager are we looking for?
      dwTargetType = lpcTarget->GetTargetType();


                                        // Now we know that we are going to
                                        // process this target.
      switch (dwTargetType)
      {
         case TARGET_NLM:

            ProcessNLMTarget(
               lpcVirusAlert,
               lpcTarget);

            break;

         case TARGET_FORWARD:

            ProcessForwardTarget(
               lpcVirusAlert,
               lpcTarget);

            break;

         default:
            break;
      }

   }

                                        // Must assume that if we got the
                                        // options and there was an alert that
                                        // all of the targets went through OK.
   bReturn = TRUE;

Cleanup:

   return bReturn;
}

