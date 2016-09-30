/////////////////////////////////////////////////////////////////////
// NAVRPC.CPP - Created - 06-20-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVRPC/VCS/navrpc.cpv   1.0   23 Jun 1997 01:18:02   DSACKIN  $
//
// Description:
//    This file will contain the NavRpc functions that can be used
//    as a communications DLL for different applications within NAV
//    to talk to each other.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NAVRPC/VCS/navrpc.cpv  $
// 
//    Rev 1.0   23 Jun 1997 01:18:02   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"

#include "navrpc.h"                     // For NAVW RPC endpoints
#include "linkcli.h"                    // For CLinkClient
#include "linkval.h"                    // For LINK_ERROR


/////////////////////////////////////////////////////////////////////
// DWORD SYM_EXPORT WINAPI NavRpcSendPacket(
//
// Purpose:
//    This function will send a block of data to the specified
//    RPC server.  The only stipulation of the data is that the
//    first DWORD of the LPVOID packet is the size of the
//    data that is to be transferred.
//
// Parameters:
//    LPSTR  lpszProtocol  - Protocol to use to send the command.
//    LPSTR  lpszAddress   - Address to contact.
//    LPSTR  lpszEndpoint  - Endpoint to look for at the Address.
//    LPVOID lpPacket      - Pointer to data block to transmit.
//
// Returns:
//   DWORD - Whatever the remote function returns.
//         - or LINK_ERROR if we fail to connect.
//
/////////////////////////////////////////////////////////////////////
// 06-20-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD SYM_EXPORT WINAPI NavRpcSendPacket(
   LPSTR    lpszProtocol,              // [in] - Protocol to use
   LPSTR    lpszAddress,               // [in] - Address to contact
   LPSTR    lpszEndpoint,              // [in] - Endpoint to look for.
   LPVOID   lpPacket)                  // [in] - Data block to send to endpoint
{
DWORD         dwReturn = LINK_ERROR;
CLinkClient   client;

   client.SetProtocol(
      lpszProtocol,
      lpszAddress,
      lpszEndpoint);

   if ( client.Connect() )
   {
      dwReturn = client.SendPacket(
                    lpPacket);

      client.DisConnect();
   }

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// DWORD SYM_EXPORT WINAPI NavRpcSendCommand (
//
// Purpose:
//    This function sends a command to the specified RPC server.
//
// Parameters:
//   LPSTR - lpszProtocol - Protocol to use to send the command
//   LPSTR - lpszAddress  - Address to contact
//   LPSTR - lpszEndpoint - Endpoint to look for at the Address
//   DWORD - dwCmd        - Command to send to the Address.
//
// Returns:
//   DWORD - Whatever the remote function returns.
//
/////////////////////////////////////////////////////////////////////
// 06-20-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD SYM_EXPORT WINAPI NavRpcSendCommand (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   DWORD    dwCmd)                      // [in] - Code to send to the endpoint
{

DWORD         dwReturn = LINK_ERROR;
CLinkClient   client;

   client.SetProtocol(
      lpszProtocol,
      lpszAddress,
      lpszEndpoint);

   if (client.Connect())
   {
      dwReturn = client.SendCommand(
                    dwCmd);

      client.DisConnect();
   }

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// DWORD SYM_EXPORT WINAPI NavRpcSendString (
//
// Purpose:
//    This function sends a string to the specified RPC Server
//
// Parameters:
//   LPSTR - lpszProtocol - Protocol to use to send the command
//   LPSTR - lpszAddress  - Address to contact
//   LPSTR - lpszEndpoint - Endpoint to look for at the Address
//   LPSTR - lpszString   - String to send to the Address.
//
// Returns:
//   DWORD - Whatever the remote function returns.
//
/////////////////////////////////////////////////////////////////////
// 06-20-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD SYM_EXPORT WINAPI NavRpcSendString (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   LPSTR    lpszString)                 // [in] - String to send to the endpoint
{
DWORD         dwReturn = LINK_ERROR;
CLinkClient   client;

   client.SetProtocol(
      lpszProtocol,
      lpszAddress,
      lpszEndpoint);

   if (client.Connect())
   {
      dwReturn = client.SendString(
                    lpszString);

      client.DisConnect();
   }

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// DWORD SYM_EXPORT WINAPI NavRpcSendStringW (
//
// Purpose:
//    This functino sends a UNICODE string to the specified RPC Server
//
// Parameters:
//   LPSTR  - lpszProtocol - Protocol to use to send the command
//   LPSTR  - lpszAddress  - Address to contact
//   LPSTR  - lpszEndpoint - Endpoint to look for at the Address
//   LPWSTR - lpszStringW  - UNICODE String to send to the Address.
//
// Returns:
//   DWORD - Whatever the remote RPC Server returns.
//
/////////////////////////////////////////////////////////////////////
// 06-20-97 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD SYM_EXPORT WINAPI NavRpcSendStringW (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   LPWSTR   lpszStringW)                // [in] - UNICODE String to send
{
DWORD         dwReturn = LINK_ERROR;
CLinkClient   client;

   client.SetProtocol(
      lpszProtocol,
      lpszAddress,
      lpszEndpoint);

   if (client.Connect())
   {
      dwReturn = client.SendString(
                    lpszStringW);

      client.DisConnect();
   }

   return dwReturn;
}


