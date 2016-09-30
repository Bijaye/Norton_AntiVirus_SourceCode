/////////////////////////////////////////////////////////////////////
// TapsSRV.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/TAPSSRV.CPv   1.0   09 May 1997 17:58:32   ksackin  $
//
//    Here is where I will define the interface for the CTapsServer
// object.  Use this object to list for CTapsClient s
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/TAPSSRV.CPv  $
// 
//    Rev 1.0   09 May 1997 17:58:32   ksackin
// Initial revision.
// 
//    Rev 1.0   06 Feb 1997 21:09:20   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Jul 1996 18:49:54   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Jun 1996 00:37:42   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <rpc.h>
#include <tchar.h>

#include <iostream.h>

#include "apscomm.h"

#include "Taps.h"
#include "TapsCBK.h"
#include "TapsSrv.h"

/////////////////////////////////////////////////////////////////////
// CTapsServer::CTapsServer
//
// This function will set defaults for all of our values
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CTapsServer::CTapsServer()
{
   m_bListening = FALSE;

   _tcscpy (m_szProtocol, NAVAPSVC_PROTOCOL );
   _tcscpy (m_szEndPoint, NAVAPSVC_ENDPOINT );
}


/////////////////////////////////////////////////////////////////////
// CTapsServer::~CTapsServer
//
// This function will clean up for us
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CTapsServer::~CTapsServer()
{
}


/////////////////////////////////////////////////////////////////////
// CTapsServer::SetCallbacks
//
// This function will set our callbacks for the RPC function(s)
//
// Parameters:
//    PTAPSCALLBACKS - pCallbacks - [in] - List of callbacks to set
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 06/09/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CTapsServer::SetCallbacks(
      PTAPSCALLBACKS pCallbacks)        // [in] - List of callbacks to set
{
   ::SetCallbacks (pCallbacks);         // Call directly into the RPC C function
}

/////////////////////////////////////////////////////////////////////
// CTapsServer::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPTSTR - lpszProtocol - String containing which protocol - "ncacn_ip_tcp"
//    LPTSTR - lpszEndPoint - String containing EndPoint - "4444"
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Server is listening
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTapsServer::SetProtocol(
   LPWSTR lpszProtocol,                 // [in] - New protocol
   LPWSTR lpszEndPoint)                 // [in] - New endpoint
{

   if ( m_bListening )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszEndPoint) )
      return FALSE;

   wcscpy (m_szProtocol, lpszProtocol);
   wcscpy (m_szEndPoint, lpszEndPoint);

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CTapsServer::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPSTR - lpszProtocol - String containing which protocol - "ncacn_ip_tcp"
//    LPSTR - lpszEndPoint - String containing EndPoint - "4444"
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Server is listening
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 06/11/96 DSACKIN - Added UNICODE support
/////////////////////////////////////////////////////////////////////
BOOL CTapsServer::SetProtocol(
   LPSTR lpszProtocol,                  // [in] - New protocol
   LPSTR lpszEndPoint)                  // [in] - New endpoint
{
WCHAR szProtocolW[MAX_STRING_LENGTH];
WCHAR szEndPointW[MAX_STRING_LENGTH];

   if ( m_bListening )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszEndPoint) )
      return FALSE;

   MultiByteToWideChar (
      CP_ACP,
      MB_PRECOMPOSED,
      lpszProtocol,
      -1,
      szProtocolW,
      sizeof(szProtocolW) / sizeof(szProtocolW[0]) );

   MultiByteToWideChar (
      CP_ACP,
      MB_PRECOMPOSED,
      lpszEndPoint,
      -1,
      szEndPointW,
      sizeof(szEndPointW) / sizeof(szEndPointW[0]) );

   return ( SetProtocol(
               szProtocolW,
               szEndPointW) );
}



/////////////////////////////////////////////////////////////////////
// CTapsServer::StartListen
//
// This function will start listening for clients
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - Server is listening
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTapsServer::StartListen(VOID)
{
RPC_STATUS rStatus;
BOOL       bReturn = TRUE;

   RpcTryExcept
   {
                                        // use the specified protocol
                                        // and endpoint
      rStatus = RpcServerUseProtseqEp(
                   m_szProtocol,
                   RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
                   m_szEndPoint,
                   NULL);               // No security descriptor

      if ( RPC_S_OK != rStatus )
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // register the interface

      if (RpcServerRegisterIf(
             TalkAPService_v1_0_s_ifspec,
             NULL,
             NULL) )
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // listen for and service RPC requests
      if (RpcServerListen(
             1,
             RPC_C_LISTEN_MAX_CALLS_DEFAULT,
             TRUE))                     // return immediately
      {
         bReturn = FALSE;
         goto Cleanup;
      }

      m_bListening = TRUE;

Cleanup:
      ;                                 // Must have SOME instruction for a label

   }
   RpcExcept(RpcExceptionCode())
   {
                                        // If we get here, an exception ocurred
      bReturn = FALSE;
   }
   RpcEndExcept

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CTapsServer::StopListen
//
// This function will stop our server from listening
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - Server is stopped.
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTapsServer::StopListen(VOID)
{
RPC_STATUS rStatus;
BOOL bReturn = TRUE;

   if ( !m_bListening )
      return FALSE;

   RpcTryExcept
   {
                                        // Shut down the RPC server
    rStatus = RpcMgmtStopServerListening(NULL);

    if (RPC_S_OK != rStatus)
       bReturn = FALSE;
   }
   RpcExcept(RpcExceptionCode())
   {
                                        // If we get here, an exception ocurred
      bReturn = FALSE;
   }
   RpcEndExcept

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CTapsServer::SendPacket
//
// This function will receive the packets from the client guy and
// take care of unpacking them and calling the correct function.
//
// Parameters:
//    LPVOID - pPacket - Pointer to our received packet
//
//
// Returns:
//    DWORD - return value from our function
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CTapsServer::SendPacket(
   LPVOID pPacket)                      // [in] - packet to send
{
   return 0;
}


