/////////////////////////////////////////////////////////////////////
// LinkSrv.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CLINK/VCS/LINKSRV.CPv   1.5   09 Dec 1997 14:44:22   DSACKIN  $
//
//    Here is where I will put the implementation for the CLinkServer
// object.  Use this object to listen for CLinkClient s
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CLINK/VCS/LINKSRV.CPv  $
// 
//    Rev 1.5   09 Dec 1997 14:44:22   DSACKIN
// RpcEpRegister is now changed to RpcEpRegisterA for Win95 support.  The
// UNICODE version of this function is NON-Functional on 95!  Both work
// under NT.
//
//    Rev 1.4   14 Aug 1997 19:23:54   DSACKIN
// Ported from the Trunk.
//
//    Rev 1.3   14 Aug 1997 18:08:48   DSACKIN
// On server shutdown, we now unregister our interface and also
// wait until the server stops listening.
//
//    Rev 1.2   12 Aug 1997 20:30:32   DSACKIN
// Added the ability to turn off SAPing.
//
//    Rev 1.1   13 May 1997 16:44:16   DSACKIN
// The FreeBindingVector function call needs to be called within the
// RpcExcption handling braces.
//
//    Rev 1.0   09 May 1997 18:35:14   ksackin
// Initial revision.
//
//    Rev 1.1   06 May 1997 20:21:46   TSmith
// Added overloaded function 'StartListen' to accept an RPC_IF_HANDLE array and
// an array element count to enable the registration of multiple interfaces
// with the RPC service.
//
//    Rev 1.0   13 Mar 1997 15:08:56   RFULLER
// Initial revision
//
//    Rev 1.2   11 Feb 1997 16:26:32   DSACKIN
// YES we want a DACL if we are not using any security!!!
//
//    Rev 1.1   05 Feb 1997 16:04:54   DSACKIN
// Now we are registering each protocol independantly of the others because
// if some of the fail to register, but we get some of the protocols to
// listen, then we are going to want to call that a success.
//
//    Rev 1.0   08 Nov 1996 14:54:36   DSACKIN
// Initial revision.
//
//    Rev 1.0   07 Nov 1996 14:52:34   DSACKIN
// Initial revision.
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

#include "RPCProt.h"

#include "Link.h"
#include "LinkCbk.h"
#include "LinkSrv.h"


/////////////////////////////////////////////////////////////////////
// Global Variables

LPTSTR szProtocols [NUMBER_OF_PROTOCOLS] = {
          _T(PROTOCOL_NCALRPC),
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

BOOL gbEnabledProtocols[NUMBER_OF_PROTOCOLS] = {
          TRUE,                         // _T(PROTOCOL_NCALRPC),
          TRUE,                         // _T(PROTOCOL_NCACN_IP_TCP),
          TRUE,                         // _T(PROTOCOL_NCACN_NB_NB),
          TRUE,                         // _T(PROTOCOL_NCACN_NB_IPX),
          TRUE,                         // _T(PROTOCOL_NCACN_NB_TCP),
          TRUE,                         // _T(PROTOCOL_NCACN_SPX),
          TRUE,                         // _T(PROTOCOL_NCADG_IP_UDP),
          TRUE,                         // _T(PROTOCOL_NCADG_IPX),
          TRUE,                         // _T(PROTOCOL_NCACN_NP),
          TRUE,                         // _T(PROTOCOL_NCACN_DNET_NSP),
          TRUE,                         // _T(PROTOCOL_NCACN_VNS_SPP),
          TRUE                          // _T(PROTOCOL_NCACN_AT_DSP)
};

#define PROTOCOL_NUMBER_NCACN_NB_IPX (3)
#define PROTOCOL_NUMBER_NCACN_SPX    (5)
#define PROTOCOL_NUMBER_NCADG_IPX    (7)

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

   m_bUseAllProtocols = FALSE;
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
// This function will set our protocol strings.  If we didn't receive
// a string for either Protocol or Endpoint, we will set ourselves into
// all protocol mode which will in turn use dynamic endpoints.
//
// Parameters:
//    LPTSTR - lpszProtocol  - String containing which protocol - "ncacn_ip_tcp"
//    LPTSTR - lpszEndPoint  - String containing EndPoint - "4444"
//    BOOL   - bUseSecurity  - Security is used by default.  If you specify
//                             FALSE here, we create a blank security
//                             descriptor and use that.
//    BOOL   - bEnableIPXSPX - This flag was added to take care of the
//                           - problems that some of our customers have
//                           - reported with RPC Servers SAPing when IPX/SPX
//                           - are enabled as protocols.
//                           - This flag is only valid when using all protocols.
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Server is listening
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 - DSACKIN - Function Created.
// 10/30/96 - DSACKIN - Added Dynamic Endpoint / All Protocol support
/////////////////////////////////////////////////////////////////////
BOOL CLinkServer::SetProtocol(
   LPWSTR lpszProtocol,                 // [in] - New protocol
   LPWSTR lpszEndPoint,                 // [in] - New endpoint
   BOOL   bUseSecurity,                 // [in] - Use our context security?
   BOOL   bEnableIPXSPX)                // [in] - Enable/Disable IPX/SPX
{

   if ( m_bListening )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszEndPoint) )
   {
                                        // If Either is NULL, use all protocols
      m_szProtocol[0] = m_szEndPoint[0] = 0;
      m_bUseAllProtocols = TRUE;
   }
   else
   {
      wcscpy (m_szProtocol, lpszProtocol);
      wcscpy (m_szEndPoint, lpszEndPoint);
   }

   m_bUseSecurity = bUseSecurity;

                                        // Handle the flag to disable IPX/SPX
   gbEnabledProtocols[PROTOCOL_NUMBER_NCACN_NB_IPX] = bEnableIPXSPX;
   gbEnabledProtocols[PROTOCOL_NUMBER_NCACN_SPX]    = bEnableIPXSPX;
   gbEnabledProtocols[PROTOCOL_NUMBER_NCADG_IPX]    = bEnableIPXSPX;

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CLinkServer::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPSTR - lpszProtocol   - String containing which protocol - "ncacn_ip_tcp"
//    LPSTR - lpszEndPoint   - String containing EndPoint - "4444"
//    BOOL   - bUseSecurity  - Security is used by default.  If you specify
//                             FALSE here, we create a blank security
//                             descriptor and use that.
//    BOOL   - bEnableIPXSPX - This flag was added to take care of the
//                           - problems that some of our customers have
//                           - reported with RPC Servers SAPing when IPX/SPX
//                           - are enabled as protocols.
//                           - This flag is only valid when using all protocols.
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Server is listening
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 - DSACKIN - Function Created.
// 06/11/96 - DSACKIN - Added UNICODE support
// 10/30/96 - DSACKIN - Added Dynamic Endpoint / All Protocol support
/////////////////////////////////////////////////////////////////////
BOOL CLinkServer::SetProtocol(
   LPSTR lpszProtocol,                  // [in] - New protocol
   LPSTR lpszEndPoint,                  // [in] - New endpoint
   BOOL  bUseSecurity,                  // [in] - Use our context security?
   BOOL  bEnableIPXSPX)                 // [in] - Enable/Disable IPX/SPX
{
WCHAR szProtocolW[MAX_STRING_LENGTH];
WCHAR szEndPointW[MAX_STRING_LENGTH];
BOOL  bReturn = FALSE;

   if ( m_bListening )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszEndPoint) )
   {
                                        // If the user doesn't have a protocol
                                        // or endpoint, we should send them to
                                        // the real function as NULL.
      bReturn = SetProtocol (
                   (LPWSTR) NULL,
                   (LPWSTR) NULL,
                   bUseSecurity,
                   bEnableIPXSPX);
   }
   else
   {

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

      bReturn = SetProtocol(
                   szProtocolW,
                   szEndPointW,
                   bUseSecurity,
                   bEnableIPXSPX);
   }

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// CLinkServer::StartListen(RPC_IF_HANDLE IfSpec)
//
// This function will start listening for clients
//
// Parameters:
//   RPC_IF_HANDLE aIfHandle[] - Array of interface handles
//                               to register.
//   DWORD dwHandles           - Number of elements in array.
//
// Returns:
//    TRUE  - Server is listening
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 - DSACKIN - Function Created.
// 10/30/96 - DSACKIN - Added Dynamic Endpoint / All Protocol support
// 04/24/97 - TSMITH  - Added capability to register multiple
//                      interface handles with RPC service.
/////////////////////////////////////////////////////////////////////
BOOL CLinkServer::StartListen(
   RPC_IF_HANDLE * aIfHandle,
   DWORD           dwHandles)
{
RPC_STATUS          rStatus;
BOOL                bReturn = TRUE;
RPC_BINDING_VECTOR *pBindingVector = NULL;
DWORD               dwUsedProtocols;
DWORD               dwCount;

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

                                        // Use all protocols or a specific one?
      if (m_bUseAllProtocols)
      {

                                        // OK.  The idea is to go through all
                                        // of the protocols and try to register
                                        // each one of them.  We look at it as
                                        // a success if we can even register
                                        // only one protocol.

         dwUsedProtocols = 0;

         for (dwCount = 0; dwCount < NUMBER_OF_PROTOCOLS; dwCount++)
         {
                                        // Here, we check to see if the given
                                        // protocol is enabled.  If it is NOT,
                                        // we skip registering this protocol.

            if (!gbEnabledProtocols[dwCount])
            {
                                        // Don't add this protocol to the list
                                        // that these interfaces are going to
                                        // listen on.
               continue;
            }

            rStatus = RpcServerUseProtseq(
                         szProtocols[dwCount],
                         RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
                         m_pSecurityDescriptor); // Here is our pre-setup decriptor

            if (RPC_S_OK == rStatus)
               dwUsedProtocols++;
         }

                                        // Now check to see if we were able to
                                        // register any protocols
         if (0 == dwUsedProtocols)
         {
                                        // Nope! Time to Go!
            bReturn = FALSE;
            goto Cleanup;
         }

                                        // Now collect our binding vector
                                        // information
         rStatus = RpcServerInqBindings(
                      &pBindingVector);

         if (RPC_S_NO_BINDINGS == rStatus)
         {
            bReturn = FALSE;
            goto Cleanup;
         }

                                        // Register each interface with the
                                        // endpoint map database.
         for ( dwCount = 0, rStatus = RPC_S_OK;
              (dwCount < dwHandles) && (RPC_S_OK == rStatus);
               dwCount++ )
         {
                                        // ******************************
                                        // This function MUST be the ANSI
                                        // version.  The UNICODE version of
                                        // this function is NOT supported on
                                        // Win95.  The ANSI version however
                                        // works just fine on the NT side.  We
                                        // will leave this as ANSI from now on.
                                        // ******************************

            rStatus = RpcEpRegisterA(
                         aIfHandle[ dwCount ],
                         pBindingVector,
                         NULL,
                         (LPBYTE) NULL);
         }
                                        // This frees any memory allocated with
                                        // our binding vector.
         RpcBindingVectorFree (
            &pBindingVector);

                                        // Only continue on if we can register
                                        // our endpoints
                                        // This rStatus if from the
                                        // RpcEpRegister call(s).
         if (RPC_S_OK != rStatus)
         {
            bReturn = FALSE;
            goto Cleanup;
         }


      }
      else
      {
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
      }

                                        // Register each interface specified in
                                        // the array of interface handles.
      for ( dwCount = 0, rStatus = RPC_S_OK;
            (dwCount < dwHandles) && (RPC_S_OK == rStatus);
            dwCount++ )
      {
         rStatus = RpcServerRegisterIf(
                      aIfHandle[ dwCount ],
                      NULL,
                      NULL);

      }

                                        // Check to make sure that the
                                        // registration of the interfactes was
                                        // successful
      if (RPC_S_OK != rStatus)
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // listen for and service RPC requests
      rStatus = RpcServerListen(
                   1,
                   RPC_C_LISTEN_MAX_CALLS_DEFAULT,
                   TRUE);               // return immediately

      if ( (RPC_S_OK != rStatus) &&
           (RPC_S_ALREADY_LISTENING != rStatus) )
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
// CLinkServer::StartListen(void)
//
// Overloaded version to retain backward compatibility with earlier
// use of the class. This simply calls the real 'StartListen' with
// the default interface handle.
//
// Parameters:
//    none
//
// Returns:
//    See 'CLinkServer::StartListen(void)'.
//
/////////////////////////////////////////////////////////////////////
// 04/24/97 TSmith  - Function created.
/////////////////////////////////////////////////////////////////////
BOOL CLinkServer::StartListen( )
{
   return( StartListen(
              &Link_v1_1_s_ifspec,
              1 ));
}



/////////////////////////////////////////////////////////////////////
// CLinkServer::StopListen
//
// This function will stop our server from listening
//
// Parameters:
//   RPC_IF_HANDLE aIfHandle[] - Array of interface handles
//                               to register.
//   DWORD dwHandles           - Number of elements in array.
//
// Returns:
//    TRUE  - Server is stopped.
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 08/13/97 - DSACKIN - We now will accept a list of interface
//                    - handles to unregister.
// 08/14/97 - DSACKIN - We will also wait until the server is no
//                    - longer listening before exiting this function.
/////////////////////////////////////////////////////////////////////
BOOL CLinkServer::StopListen(
   RPC_IF_HANDLE * aIfHandle,           // [in] - Array of interfaces to unregister
   DWORD           dwHandles)           // [in] - Number of elements in array.
{
RPC_STATUS rStatus;
BOOL       bReturn = TRUE;
DWORD      dwCount;

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

                                        // Don't forget to unregister our
                                        // interfaces.
         for ( dwCount = 0, rStatus = RPC_S_OK;
               (dwCount < dwHandles) && (RPC_S_OK == rStatus);
               dwCount++)
         {
            rStatus = RpcServerUnregisterIf(
                        aIfHandle[dwCount],
                        NULL,
                        NULL);
         }

         if (RPC_S_OK != rStatus)
         {
            bReturn = FALSE;
         }
         else
         {



            rStatus = RpcMgmtWaitServerListen();

            if ( (RPC_S_OK == rStatus) ||
                 (RPC_S_NOT_LISTENING == rStatus) )
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
            else
            {
               bReturn = FALSE;
            }
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
// 08/13/97 - DSACKIN - This will now pass our default interface
//                    - pointer to the main function
/////////////////////////////////////////////////////////////////////
BOOL CLinkServer::StopListen( )
{
   return ( StopListen(
               &Link_v1_1_s_ifspec,
               1) );
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


