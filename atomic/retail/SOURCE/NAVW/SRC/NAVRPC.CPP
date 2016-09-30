// Copyright 1990 Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NAVW/VCS/navrpc.cpv   1.1   13 Mar 1998 15:33:20   TCASHIN  $
//
// Description:
//    This file contains the code necessary to get our RPC server running.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVW/VCS/navrpc.cpv  $
// 
//    Rev 1.1   13 Mar 1998 15:33:20   TCASHIN
// Pass along the UWM_DISPLAY_DIALOG message.
// 
//    Rev 1.0   06 Feb 1997 21:01:26   RFULLER
// Initial revision
// 
//    Rev 1.1   22 Jul 1996 14:44:44   DSACKIN
// Include NAVRPC.H instead of NAVW.H
//
//    Rev 1.0   19 Jul 1996 13:56:28   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"

#include "navrpc.h"
#include "msgreg.h"                     // For UWM_NAVAP_ACTIVATING

#include "LinkSrv.h"                    // For RPC Server.

/////////////////////////////////////////////////////////////////////
// Globals

CLinkServer  g_NavwServer;              // Declare our RPC server object.


/////////////////////////////////////////////////////////////////////
// Externs

extern HWND         ghWndMain;          // Main window / dialog box handle


/////////////////////////////////////////////////////////////////////
// NavSendCommandProc
//
// This takes the command from the RPC guy and processes it.
// Normally this would call back into our server object, but we can do
// everything we need to here in this C function
//
// Parameters:
//    LPVOID - lpPacket - Packet from client guy
//
// Return Value:
//    DWORD - TRUE - valid packet
//    DWORD - FALSE - invalid packet
//
/////////////////////////////////////////////////////////////////////
// 07/18/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD NavSendCommandProc(
   LPVOID pPacket)                      // [in] - Packet from Client
{
DWORD *lpdwInfo;

                                        // Check for invalid pointer
   if (NULL == pPacket)
      return FALSE;

                                        // Set up our pointer
   lpdwInfo = (DWORD *) pPacket;

                                        // Make sure we got enough data
   if (*lpdwInfo < (2 * sizeof (DWORD)))
      return FALSE;

                                        // Bump to the next DWORD (our command)
   lpdwInfo++;

   switch (*lpdwInfo)
   {
                                        // If we get the right message,
                                        // pass on to the main window
      case UWM_NAVAP_ACTIVATING:
         PostMessage (ghWndMain, UWM_NAVAP_ACTIVATING, 0, 0);
         break;

      case UWM_DISPLAY_OPTIONS:
         PostMessage (ghWndMain, UWM_DISPLAY_OPTIONS, 0, 0);
         break;
   }

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// NavStartRPCServer
//
// Description:
//    This gets our RPC Server up and running.
//    No error checking because there is nothing we can do if we fail,
//    and the only thing that will happen is NAVAPSVC can't talk to us.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 07/18/96 - DSACKIN - Function created
/////////////////////////////////////////////////////////////////////
VOID WINAPI NavStartRPCServer (VOID)
{
LINKCALLBACKS rCbk = { NavSendCommandProc };

                                        // Initialize our server.
   g_NavwServer.SetProtocol(
      NAVW_PROTOCOL,
      NAVW_ENDPOINT);

                                        // Set up the callback
   g_NavwServer.SetCallbacks(
      &rCbk);
                                        // Now Create start listening
   g_NavwServer.StartListen();
}


/////////////////////////////////////////////////////////////////////
// NavStopRPCServer
//
// Description:
//    This function stops our RPC Server.
//
// Parameters:
//    none.
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 07/18/96 - DSACKIN - Function created
/////////////////////////////////////////////////////////////////////
VOID WINAPI NavStopRPCServer (VOID)
{
                                        // Stop our server!
   g_NavwServer.StopListen();
}

