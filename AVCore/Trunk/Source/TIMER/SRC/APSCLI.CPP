/////////////////////////////////////////////////////////////////////
// APSCLI.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/apscli.cpv   1.0   09 May 1997 17:58:48   ksackin  $
//
//    Here is where I will define the interface for the CNAVAPSVCClient
// object.  Use this object to talk to the CNAVAPSVCServer
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/apscli.cpv  $
// 
//    Rev 1.0   09 May 1997 17:58:48   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:09:20   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:20   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:46:32   DKEEFER
// Initial revision
// 
//    Rev 1.1   22 Jul 1996 17:08:54   DSACKIN
// Included NavRpc.h
//
//    Rev 1.0   19 Jul 1996 00:43:46   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <rpc.h>
#include <tchar.h>

#include "apscomm.h"
#include "NavRpc.h"

#include "LinkVal.h"

#include "Linkcli.h"

#include "APSCli.h"

/////////////////////////////////////////////////////////////////////
// CNAVAPSVCClient::CNAVAPSVCClient
//
// This function will set default protocol for our client
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 07/18/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNAVAPSVCClient::CNAVAPSVCClient()
{
   SetProtocol(
      NAVAPSVC_PROTOCOL,
      NAVAPSVC_ADDRESS,
      NAVAPSVC_ENDPOINT);
}


/////////////////////////////////////////////////////////////////////
// CNAVAPSVCClient::~CNAVAPSVCClient
//
// This function is for our special cleanup.  (None needed at this time)
//
// Parameters:
//
// Returns:
//
/////////////////////////////////////////////////////////////////////
// 07/18/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CNAVAPSVCClient::~CNAVAPSVCClient()
{
}


/////////////////////////////////////////////////////////////////////
// CNAVAPSVCClient::SendPacket
//
// This function will send the packet to the server
// The only thing we really have to worry about is getting the correct
// return value.  The rest is taken care of on the server side.
//
// Parameters:
//    LPVOID - pPacket - [in] Packet to send to the server
//
// Returns:
//    DWORD - Return value from the server
//
/////////////////////////////////////////////////////////////////////
// 07/18/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD CNAVAPSVCClient::SendPacket(
   LPVOID pPacket)                      // [in] - Message to log
{
DWORD dwReturn;

   dwReturn = CLinkClient::SendPacket(
                 pPacket);

   if (LINK_ERROR == dwReturn)
      dwReturn = NAVAPSVC_ERROR;

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// CNAVAPSVCClient::SendCommand
//
// This function will create a packet and send the command to the CLinkServer
// The only thing we really have to worry about is getting the correct
// return value.  The rest is taken care of on the server side.
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
DWORD CNAVAPSVCClient::SendCommand(
   DWORD dwCommand)                     // [in] - Command to send to server
{
DWORD dwReturn;

   dwReturn = CLinkClient::SendCommand(
                 dwCommand);

   if (LINK_ERROR == dwReturn)
      dwReturn = NAVAPSVC_ERROR;

   return dwReturn;
}

