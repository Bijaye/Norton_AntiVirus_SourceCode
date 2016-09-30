/////////////////////////////////////////////////////////////////////
// TapsCLI.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/TAPSCLI.CPv   1.0   09 May 1997 17:58:30   ksackin  $
//
//    Here is where I will define the interface for the CTapsClient
// object.  Use this object to talk to the RPC server.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/TAPSCLI.CPv  $
// 
//    Rev 1.0   09 May 1997 17:58:30   ksackin
// Initial revision.
// 
//    Rev 1.0   06 Feb 1997 21:09:18   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Jul 1996 18:48:02   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Jun 1996 00:37:36   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <rpc.h>
#include <tchar.h>

#include "Taps.h"
#include "Tapscli.h"

#include "apscomm.h"

/////////////////////////////////////////////////////////////////////
// CTapsClient::CTapsClient
//
// This function will set defaults for all of our values
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to Taps
/////////////////////////////////////////////////////////////////////
CTapsClient::CTapsClient()
{
   m_bConnected = FALSE;

   _tcscpy (m_szProtocol, NAVAPSVC_PROTOCOL);
   _tcscpy (m_szAddress,  NAVAPSVC_ADDRESS);
   _tcscpy (m_szEndPoint, NAVAPSVC_ENDPOINT);
}


/////////////////////////////////////////////////////////////////////
// CTapsClient::~CTapsClient
//
// This function will clean up for us
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 07/17/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to Taps
/////////////////////////////////////////////////////////////////////
CTapsClient::~CTapsClient()
{
   if ( m_bConnected )
      DisConnect();
}


/////////////////////////////////////////////////////////////////////
// CTapsClient::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPWSTR - lpszProtocol - String containing which protocol - "ncalrpc"
//    LPWSTR - lpszAddress  - String containing address - ""
//    LPWSTR - lpszEndPoint - String containing EndPoint - "Navapsvc"
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Client is connected or a null sting was entered
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 07/17/96 DSACKIN - Function Reworked from Timer to Taps
/////////////////////////////////////////////////////////////////////
BOOL CTapsClient::SetProtocol(
   LPWSTR lpszProtocol,                 // [in] - New protocol
   LPWSTR lpszAddress,                  // [in] - New address
   LPWSTR lpszEndPoint)                 // [in] - New endpoint
{

   if ( m_bConnected )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszAddress)  ||
        (NULL == lpszEndPoint) )
      return FALSE;

   wcscpy (m_szProtocol, lpszProtocol);
   wcscpy (m_szAddress,  lpszAddress);
   wcscpy (m_szEndPoint, lpszEndPoint);

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CTapsClient::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPTSTR - lpszProtocol - String containing which protocol - "ncalrpc"
//    LPTSTR - lpszAddress  - String containing address - ""
//    LPTSTR - lpszEndPoint - String containing EndPoint - "Navapsvc"
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Client is connected or a null sting was entered
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 06/11/96 DSACKIN - Added UNICODE support
// 07/17/96 DSACKIN - Function Reworked from Timer to Taps
/////////////////////////////////////////////////////////////////////
BOOL CTapsClient::SetProtocol(
   LPSTR lpszProtocol,                  // [in] - New protocol
   LPSTR lpszAddress,                   // [in] - New address
   LPSTR lpszEndPoint)                  // [in] - New endpoint
{
WCHAR szProtocolW[MAX_STRING_LENGTH];
WCHAR szAddressW[MAX_STRING_LENGTH];
WCHAR szEndPointW[MAX_STRING_LENGTH];

   if ( m_bConnected )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszAddress)  ||
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
      lpszAddress,
      -1,
      szAddressW,
      sizeof(szAddressW) / sizeof(szAddressW[0]) );

   MultiByteToWideChar (
      CP_ACP,
      MB_PRECOMPOSED,
      lpszEndPoint,
      -1,
      szEndPointW,
      sizeof(szEndPointW) / sizeof(szEndPointW[0]) );

   return ( SetProtocol(
               szProtocolW,
               szAddressW,
               szEndPointW) );
}


/////////////////////////////////////////////////////////////////////
// CTapsClient::Connect
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
// 07/17/96 DSACKIN - Function Reworked from Timer to Taps
/////////////////////////////////////////////////////////////////////
BOOL CTapsClient::Connect(VOID)
{
BOOL   bReturn = TRUE;
LPTSTR stringBinding = NULL;

   if ( m_bConnected )
      return FALSE;

   RpcTryExcept
   {
                                        // Put together binding

      if ( RpcStringBindingCompose(
            NULL,                       // No UUID for us
            m_szProtocol,
            m_szAddress,
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
            &TalkAPServicesrv_handle))
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // OK.. Now we know we are connected.
      m_bConnected = TRUE;

Cleanup:

      if ( stringBinding )
         RpcStringFree(&stringBinding);

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
// CTapsClient::DisConnect
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
// 07/17/96 DSACKIN - Function Reworked from Timer to Taps
/////////////////////////////////////////////////////////////////////
BOOL CTapsClient::DisConnect(VOID)
{
BOOL bReturn = TRUE;

   if ( !m_bConnected )
      return FALSE;

   RpcTryExcept
   {
                                        // release binding to server
      if (RpcBindingFree(&TalkAPServicesrv_handle))
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
// CTapsClient::SendPacket
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
DWORD CTapsClient::SendPacket(
   LPVOID pPacket)                      // [in] - Message to log
{
DWORD dwReturn = NAVAPSVC_ERROR;
DWORD dwSize;

   if ( !m_bConnected )
      return dwReturn;

   if ( NULL == pPacket )
      return dwReturn;

   RpcTryExcept
   {
                                        // First DWORD is ALWAYS the size
      dwSize =  *( (DWORD *) pPacket );

      dwReturn = NAVAPService_SendCommand (
                    dwSize,
                    (byte *) pPacket);
   }
   RpcExcept(RpcExceptionCode())
   {
                                        // If we get here, an exception ocurred
      dwReturn = NAVAPSVC_ERROR;
   }
   RpcEndExcept

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// CTapsClient::SendCommand
//
// This function will create a packet and send the command to NAVAPSVC
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
DWORD CTapsClient::SendCommand(
   DWORD dwCommand)                     // [in] - Command to send to server
{
DWORD dwReturn = NAVAPSVC_ERROR;
DWORD dwCmdPacket[2];

   if ( !m_bConnected )
      goto Cleanup;

                                        // First, how much to send.
   dwCmdPacket[0] = sizeof(dwCmdPacket);
                                        // Second DWORD is the command
   dwCmdPacket[1] = dwCommand;

                                        // Now send the packet and
                                        // collect the return value
   dwReturn = SendPacket ((LPVOID) dwCmdPacket);

Cleanup:
   return dwReturn;
}

