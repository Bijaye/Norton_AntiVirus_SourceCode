/////////////////////////////////////////////////////////////////////
// LinkCLI.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/CLINK/VCS/linkcli.cpv   1.1   09 Jun 1997 17:40:12   DALLEE  $
//
//    Here is where I will define the interface for the CLinkClient
// object.  Use this object to talk to the CLinkServer
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/CLINK/VCS/linkcli.cpv  $
// 
//    Rev 1.1   09 Jun 1997 17:40:12   DALLEE
// Win95 RPC workaround in Connect(). Substitute NULL for address if passing
// an empty string to RpcStringBindingCompose().
// Empty string should default to local machine, but fails in Win 95.
// NULL correctly defaults to local machine for both Win 95 and NT 
// implementations of RPC.
// 
//    Rev 1.0   09 May 1997 18:35:12   ksackin
// Initial revision.
// 
//    Rev 1.1   06 May 1997 19:42:46   TSmith
// Added member variable 'm_pBindingHandle' to ClinkClient class. Also added
// declarations for overloaded 'Connect' & 'DisConnect' functions to accept
// an RPC_BINDING_HANDLE* as an argument. This will enable connecting to a
// user specified RPC interface.
//
//    Rev 1.0   13 Mar 1997 15:08:56   RFULLER
// Initial revision
//
//    Rev 1.0   08 Nov 1996 14:54:34   DSACKIN
// Initial revision.
//
//    Rev 1.0   07 Nov 1996 14:50:10   DSACKIN
// Initial revision.
//
//    Rev 1.2   28 Jul 1996 23:28:36   DSACKIN
// Wrap ALL of our RPC calls in RpcTryExcept and RpcExcept.
//
//    Rev 1.1   22 Jul 1996 17:02:12   DSACKIN
// Added SendString.
//
//    Rev 1.0   18 Jul 1996 23:41:12   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <rpc.h>
#include <tchar.h>

#include "LinkVal.h"

#include "Link.h"                       // Generated at compile time
#include "Linkcli.h"

/////////////////////////////////////////////////////////////////////
// CLinkClient::CLinkClient
//
// This function will set defaults for all of our values
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to CLink
// 04/22/97 TSMITH  - Initialize new member variable m_pBindingHandle
/////////////////////////////////////////////////////////////////////
CLinkClient::CLinkClient()
{
   m_pBindingHandle       = NULL;
   m_bUseDynamicEndpoints = FALSE;
   m_szProtocol[0]        = 0;
   m_szAddress[0]         = 0;
   m_szEndPoint[0]        = 0;
   m_bConnected           = FALSE;
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::~CLinkClient
//
// This function will clean up for us
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to CLink
// 04/22/97 TSMITH  - Replaced hard coded binding handle with member
//                    variable.
/////////////////////////////////////////////////////////////////////
CLinkClient::~CLinkClient()
{
   if ( m_bConnected )
      DisConnect( m_pBindingHandle );
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPWSTR - lpszProtocol - String containing which protocol
//    LPWSTR - lpszAddress  - String containing address
//    LPWSTR - lpszEndPoint - String containing EndPoint

//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Client is connected or a null sting was entered
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to CLink
/////////////////////////////////////////////////////////////////////
BOOL CLinkClient::SetProtocol(
   LPCWSTR lpszProtocol,                 // [in] - New protocol
   LPCWSTR lpszAddress,                  // [in] - New address
   LPCWSTR lpszEndPoint)                 // [in] - New endpoint
{

   if ( m_bConnected )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszAddress) )
   {
      return FALSE;
   }

                                        // Are they going to use Dynamic
                                        // EndPoints?
   if (NULL == lpszEndPoint)
   {
      m_bUseDynamicEndpoints = TRUE;
   }

   wcscpy (m_szProtocol, lpszProtocol);
   wcscpy (m_szAddress,  lpszAddress);

   if (m_bUseDynamicEndpoints)
   {
      m_szEndPoint[0] = 0;
   }
   else
   {
      wcscpy (m_szEndPoint, lpszEndPoint);
   }

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPTSTR - lpszProtocol - String containing which protocol
//    LPTSTR - lpszAddress  - String containing address
//    LPTSTR - lpszEndPoint - String containing EndPoint
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Client is connected or a null sting was entered
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 06/11/96 DSACKIN - Added UNICODE support
// 07/17/96 DSACKIN - Function Reworked from Timer to CLink
/////////////////////////////////////////////////////////////////////
BOOL CLinkClient::SetProtocol(
   LPCSTR lpszProtocol,                  // [in] - New protocol
   LPCSTR lpszAddress,                   // [in] - New address
   LPCSTR lpszEndPoint)                  // [in] - New endpoint
{
WCHAR szProtocolW[MAX_STRING_LENGTH];
WCHAR szAddressW[MAX_STRING_LENGTH];
WCHAR szEndPointW[MAX_STRING_LENGTH];
BOOL  bReturn;

   if ( m_bConnected )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszAddress) )
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
      lpszAddress,
      -1,
      szAddressW,
      sizeof(szAddressW) / sizeof(szAddressW[0]) );

   if (NULL == lpszEndPoint)
   {
      bReturn = SetProtocol(
                   szProtocolW,
                   szAddressW,
                   (LPWSTR) NULL);
   }
   else
   {
      MultiByteToWideChar (
         CP_ACP,
         MB_PRECOMPOSED,
         lpszEndPoint,
         -1,
         szEndPointW,
         sizeof(szEndPointW) / sizeof(szEndPointW[0]) );

      bReturn = SetProtocol(
                   szProtocolW,
                   szAddressW,
                   szEndPointW);
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::Connect( RPC_BINDING_HANDLE* pBindingHandle )
//
// This function will connect us to the server
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - Server is contacted
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to CLink
// 04/22/97 TSMITH  - Added binding handle argument
/////////////////////////////////////////////////////////////////////
BOOL CLinkClient::Connect( RPC_BINDING_HANDLE* pBindingHandle )
{
BOOL   bReturn = TRUE;
LPTSTR stringBinding = NULL;
LPWSTR lpszAddress;

   if ( m_bConnected )
      return FALSE;

   RpcTryExcept
   {
                                        // Slight klugery for Win95.
                                        // Empty string in address fails in
                                        // RpcStringBindingCompose().
                                        // NULL will default to local
                                        // machine correctly for W95 and NT.

      lpszAddress = *m_szAddress ? m_szAddress : NULL;

                                        // Put together binding

      if ( RpcStringBindingCompose(
            NULL,                       // No UUID for us
            m_szProtocol,
            lpszAddress,
            (m_bUseDynamicEndpoints) ?  // Do they need dynamic endpoints?
               NULL :
               m_szEndPoint,
            NULL,                       // No special network stuff
            &stringBinding) )           // This is returned
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // bind to server using string binding info

      if (RpcBindingFromStringBinding(
            stringBinding,
            pBindingHandle))
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // OK.. Now we know we are connected.
      m_bConnected = TRUE;
      m_pBindingHandle = pBindingHandle;

Cleanup:

      if ( stringBinding )
         RpcStringFree(&stringBinding);

   }
   RpcExcept(RpcExceptionCode())
   {
                                        // If we get here, an exception ocurred
      bReturn = FALSE;
      m_pBindingHandle = NULL;
   }
   RpcEndExcept

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::Connect(VOID)
//
//    Overloaded version calls regular version with default handle.
//
// Parameters:
//    none
//
// Returns:
//
//    See 'CLinkClient::Connect(RPC_BINDING_HANDLE* pBindingHandle)
//
/////////////////////////////////////////////////////////////////////
// 05/06/97 TSMITH  - Function created.
/////////////////////////////////////////////////////////////////////
BOOL CLinkClient::Connect( VOID )
{
    return( Connect( &Linksrv_handle ) );
}



/////////////////////////////////////////////////////////////////////
// CLinkClient::DisConnect( RPC_BINDING_HANDLE* pBindingHandle )
//
// This function will disconnect us from the server
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - Connection is broken
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to CLink.
// 04/22/97 TSMITH  - Added binding handle argument.
/////////////////////////////////////////////////////////////////////
BOOL CLinkClient::DisConnect( RPC_BINDING_HANDLE* pBindingHandle )
{
BOOL bReturn = TRUE;

   if ( !m_bConnected )
      return FALSE;

   RpcTryExcept
   {
                                        // release binding to server
      if (RpcBindingFree(pBindingHandle))
         bReturn = FALSE;
   }
   RpcExcept(RpcExceptionCode())
   {
                                        // If we get here, an exception ocurred
      bReturn = FALSE;
   }
   RpcEndExcept

   m_pBindingHandle = NULL;
   m_bConnected = FALSE;
   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// CLinkClient::DisConnect( VOID )
//
//    Overloaded version calls regular version with default handle.
//
// Parameters:
//    none
//
// Returns:
//
//    See 'CLinkClient::DisConnect( RPC_BINDING_HANDLE* pBindingHandle )'
//
/////////////////////////////////////////////////////////////////////
// 05/06/97 TSMITH  - Function created.
/////////////////////////////////////////////////////////////////////

BOOL CLinkClient::DisConnect( VOID )
{
    return( DisConnect( &Linksrv_handle ) );
}

/////////////////////////////////////////////////////////////////////
// CLinkClient::SendPacket
//
// This function will send the packet to the server
//
// Parameters:
//    LPVOID - pPacket - [in] Packet to send to the server
//
// Returns:
//    DWORD - Return value from the server
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CLinkClient::SendPacket(
   LPVOID pPacket)                      // [in] - Message to log
{
DWORD dwReturn = LINK_ERROR;
DWORD dwSize;

   if ( !m_bConnected )
      return dwReturn;

   if ( NULL == pPacket )
      return dwReturn;

   RpcTryExcept
   {
                                        // First DWORD is ALWAYS the size
      dwSize =  *( (DWORD *) pPacket );

      dwReturn = Link_SendCommand (
                    dwSize,
                    (byte *) pPacket);
   }
   RpcExcept(RpcExceptionCode())
   {
                                        // If we get here, an exception ocurred
      dwReturn = LINK_ERROR;
   }
   RpcEndExcept

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::SendCommand
//
// This function will create a packet and send the command to the CLinkServer
//
// Parameters:
//    DWORD - dwCommand - [in] Command to send to server
//
// Returns:
//    DWORD - Return value from the server
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CLinkClient::SendCommand(
   DWORD dwCommand)                     // [in] - Command to send to server
{
DWORD dwReturn = LINK_ERROR;
DWORD dwCmdPacket[2];

   if ( !m_bConnected )
      goto Cleanup;

                                        // First, how much to send.
   dwCmdPacket[0] = sizeof(dwCmdPacket);
                                        // Second DWORD is the command
   dwCmdPacket[1] = dwCommand;

   RpcTryExcept
   {
                                        // Now send the packet and
                                        // collect the return value
      dwReturn = SendPacket ((LPVOID) dwCmdPacket);

   }
   RpcExcept(RpcExceptionCode())
   {
                                        // Some exception ocurred
      dwReturn = LINK_ERROR;
   }
   RpcEndExcept

Cleanup:
   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::SendString
//
// This function will send a string to the remote machine
//
// Parameters:
//    LPSTR - lpszString - [in] String to send to the server
//
// Returns:
//    DWORD - Return value from the server
//
/////////////////////////////////////////////////////////////////////
// 07/22/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CLinkClient::SendString(
   LPSTR lpszString)                    // [in] - Command to send to server
{
DWORD dwReturn = LINK_ERROR;

   if ( !m_bConnected )
      goto Cleanup;

                                        // Make sure we have a string
   if (NULL == lpszString)
      goto Cleanup;

   RpcTryExcept
   {
                                        // Now send the string and
                                        // collect the return value
      dwReturn = Link_SendStringA (
                  (unsigned char *) lpszString);

   }
   RpcExcept(RpcExceptionCode())
   {
                                        // Some exception ocurred
      dwReturn = LINK_ERROR;
   }
   RpcEndExcept

Cleanup:
   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// CLinkClient::SendString
//
// This function will send a string to the remote machine
//
// Parameters:
//    LPWSTR - lpszStringW - [in] UNICODE String to send to the server
//
// Returns:
//    DWORD - Return value from the server
//
/////////////////////////////////////////////////////////////////////
// 07/22/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CLinkClient::SendString(
   LPWSTR lpszStringW)                  // [in] - Command to send to server
{
DWORD dwReturn = LINK_ERROR;

   if ( !m_bConnected )
      goto Cleanup;

                                        // Make sure we have a string
   if (NULL == lpszStringW)
      goto Cleanup;

   RpcTryExcept
   {
                                        // Now send the packet and
                                        // collect the return value
      dwReturn = Link_SendStringW (lpszStringW);

   }
   RpcExcept(RpcExceptionCode())
   {
                                        // Some exception ocurred
      dwReturn = LINK_ERROR;
   }
   RpcEndExcept


Cleanup:
   return dwReturn;
}

