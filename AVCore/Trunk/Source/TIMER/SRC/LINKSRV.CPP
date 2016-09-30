/////////////////////////////////////////////////////////////////////
// LinkSrv.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/linksrv.cpv   1.0   09 May 1997 17:58:44   ksackin  $
//
//    Here is where I will put the implementation for the CLinkServer
// object.  Use this object to listen for CLinkClient s
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/linksrv.cpv  $
// 
//    Rev 1.0   09 May 1997 17:58:44   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:09:18   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:20   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:46:28   DKEEFER
// Initial revision
// 
//    Rev 1.2   28 Aug 1996 14:37:24   DSACKIN
// Added the ability to remove all security for the CLinkServer RPC connecter.
// This can be accomplished by calling SetProtocol with FALSE as an additional
// parameter.
//
//    Rev 1.1   22 Jul 1996 17:02:34   DSACKIN
// Added SendString
//
//    Rev 1.0   18 Jul 1996 23:41:52   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <rpc.h>
#include <tchar.h>

#include "Link.h"
#include "LinkCbk.h"
#include "LinkSrv.h"

/////////////////////////////////////////////////////////////////////
// CLinkServer::CLinkServer
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
CLinkServer::CLinkServer()
{
   m_bListening = FALSE;

   m_szProtocol[0] = 0;
   m_szEndPoint[0] = 0;

   m_bUseSecurity        = TRUE;
   m_pSecurityDescriptor = NULL;
}


/////////////////////////////////////////////////////////////////////
// CLinkServer::~CLinkServer
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
CLinkServer::~CLinkServer()
{
   if (m_bListening)
      StopListen();
}


/////////////////////////////////////////////////////////////////////
// CLinkServer::SetCallbacks
//
// This function will set our callbacks for the RPC function(s)
//
// Parameters:
//    PLINKCALLBACKS - pCallbacks - [in] - List of callbacks to set
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 06/09/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CLinkServer::SetCallbacks(
      PLINKCALLBACKS pCallbacks)        // [in] - List of callbacks to set
{
   ::SetCallbacks (pCallbacks);         // Call directly into the RPC C function
}

/////////////////////////////////////////////////////////////////////
// CLinkServer::SetProtocol
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
BOOL CLinkServer::SetProtocol(
   LPWSTR lpszProtocol,                 // [in] - New protocol
   LPWSTR lpszEndPoint,                 // [in] - New endpoint
   BOOL   bUseSecurity)                 // [in] - Use our context security?
{

   if ( m_bListening )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszEndPoint) )
      return FALSE;

   wcscpy (m_szProtocol, lpszProtocol);
   wcscpy (m_szEndPoint, lpszEndPoint);

   m_bUseSecurity = bUseSecurity;

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CLinkServer::SetProtocol
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
BOOL CLinkServer::SetProtocol(
   LPSTR lpszProtocol,                  // [in] - New protocol
   LPSTR lpszEndPoint,                  // [in] - New endpoint
   BOOL  bUseSecurity)                  // [in] - Use our context security?
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
               szEndPointW,
               bUseSecurity) );
}



/////////////////////////////////////////////////////////////////////
// CLinkServer::StartListen
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
BOOL CLinkServer::StartListen(VOID)
{
RPC_STATUS rStatus;
BOOL       bReturn = TRUE;

   RpcTryExcept
   {
      if (m_bUseSecurity)
      {
         if (NULL != m_pSecurityDescriptor)
         {
                                        // If we want to use the default
                                        // security, we are going to have to
                                        // free our old descriptor if it is
                                        // not NULL.
            GlobalFree(m_pSecurityDescriptor);
            m_pSecurityDescriptor = NULL;
         }
      }
      else
      {
         if (m_pSecurityDescriptor)
         {
            GlobalFree(m_pSecurityDescriptor);
            m_pSecurityDescriptor = NULL;
         }

         m_pSecurityDescriptor = GlobalAlloc(
                                    GHND,
                                    SECURITY_DESCRIPTOR_MIN_LENGTH);

         if (NULL == m_pSecurityDescriptor)
         {
            bReturn = FALSE;
            goto Cleanup;
         }
                                        // If we don't want to use security,
                                        // we need to create an open descriptor so
                                        // that anyone may connect.

         if (!InitializeSecurityDescriptor(
                 m_pSecurityDescriptor,             // Our allocated SD
                 SECURITY_DESCRIPTOR_REVISION) )    // Selects what authority for our SID
         {
            GlobalFree(m_pSecurityDescriptor);
            m_pSecurityDescriptor = NULL;

            bReturn = FALSE;
            goto Cleanup;
         }

         if (!SetSecurityDescriptorDacl (
                 m_pSecurityDescriptor, // Pointer to our SID
                 TRUE,                  // Are we adding a Dacl?
                 (PACL) NULL,           // What Dacl? One that allows all access
                 FALSE))                // bDaclDefaulted - Specified by user
         {
            GlobalFree(m_pSecurityDescriptor);
            m_pSecurityDescriptor = NULL;

            bReturn = FALSE;
            goto Cleanup;
         }
      }

                                        // use the specified protocol
                                        // and endpoint
      rStatus = RpcServerUseProtseqEp(
                   m_szProtocol,
                   RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
                   m_szEndPoint,
                   m_pSecurityDescriptor); // Here is our pre-setup decriptor

      if ( RPC_S_OK != rStatus )
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // register the interface
      if (RpcServerRegisterIf(
            Link_v1_0_s_ifspec,
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
// CLinkServer::StopListen
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
BOOL CLinkServer::StopListen(VOID)
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
      else
      {
         m_bListening = FALSE;

                                        // Cleanup a SecurityDescriptor if
                                        // we have it allocated.
         if (NULL != m_pSecurityDescriptor)
         {
            GlobalFree(m_pSecurityDescriptor);
            m_pSecurityDescriptor = NULL;
         }
      }
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
// CLinkServer::SendPacket
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
DWORD CLinkServer::SendPacket(
   LPVOID pPacket)                      // [in] - packet received
{
   return 0;
}


/////////////////////////////////////////////////////////////////////
// CLinkServer::SendString
//
// This function will receive the string from the client guy.
//
// Parameters:
//    LPSTR - lpszString - String received from the client
//
//
// Returns:
//    DWORD - return value from our function
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CLinkServer::SendString(
   LPSTR lpszString)                    // [in] - string received
{
   return 0;
}


/////////////////////////////////////////////////////////////////////
// CLinkServer::SendString
//
// This function will receive the string from the client guy.
// UNICODE version.
//
// Parameters:
//    LPWSTR - lpszStringW - UNICODE String received from the client
//
//
// Returns:
//    DWORD - return value from our function
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CLinkServer::SendString(
   LPWSTR lpszStringW)                  // [in] - string received
{
   return 0;
}


