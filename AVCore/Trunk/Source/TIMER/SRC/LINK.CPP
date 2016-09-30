/////////////////////////////////////////////////////////////////////
// Link.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/link.cpv   1.0   09 May 1997 17:58:38   ksackin  $
//
// This is where the functions declared by our RPC clients go.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/link.cpv  $
// 
//    Rev 1.0   09 May 1997 17:58:38   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:09:08   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:18   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:46:20   DKEEFER
// Initial revision
// 
//    Rev 1.1   22 Jul 1996 17:03:06   DSACKIN
// Added Link_SendStringA and Link_SendStringW
//
//    Rev 1.0   18 Jul 1996 23:40:30   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>

#include <rpc.h>
#include <rpcndr.h>

#include "LinkVal.h"                    // CLink constants

#include "Link.h"                       // Generated header file

#include "Linkcbk.h"

LINKCALLBACKS rCbk = { NULL, NULL, NULL };

/////////////////////////////////////////////////////////////////////
// SetCallbacks
//
//    This function will set up the callbacks for our RPC functions
//
// Parameters:
//    PLINKCALLBACKS - pCallbacks  - List of callbacks to set
//
/////////////////////////////////////////////////////////////////////
void SetCallbacks(
   PLINKCALLBACKS pCallbacks)           // [in] - List of callbacks to set
{
   memcpy (&rCbk, pCallbacks, sizeof (LINKCALLBACKS));
}

/////////////////////////////////////////////////////////////////////
// Start of our RPC functions

/////////////////////////////////////////////////////////////////////
// Link_SendCommand
//
//    RPC function on the server machine that receives our commands
//
// Parameters:
//    unsigned long - dwSize  - size of the structure coming through
//    byte *        - pPacket - reference to our packet being received
//
/////////////////////////////////////////////////////////////////////
unsigned long Link_SendCommand(
   unsigned long  dwSize,
   byte *         pPacket)
{
unsigned long dwReturn;

   if (NULL == rCbk.lpfnSendCommandProc)
      dwReturn = LINK_ERROR;
   else
   {
      dwReturn = (rCbk.lpfnSendCommandProc) (
                    pPacket);
   }

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// Link_SendStringA
//
//    RPC function on the server machine that receives our string
//
// Parameters:
//    unsigned long - dwSize  - size of the structure coming through
//    byte *        - pPacket - reference to our packet being received
//
/////////////////////////////////////////////////////////////////////
// 07/22/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
unsigned long Link_SendStringA(
   byte *         lpszString)
{
unsigned long dwReturn;

   if (NULL == rCbk.lpfnSendStringProcA)
      dwReturn = LINK_ERROR;
   else
   {
      dwReturn = (rCbk.lpfnSendStringProcA) (
                    lpszString);
   }

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// Link_SendStringW
//
//    RPC function on the server machine that receives our string
//    (UNICODE VERSION)
//
// Parameters:
//    wchar_t *     - lpszString - UNICODE string to send
//
/////////////////////////////////////////////////////////////////////
// 07/22/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
unsigned long Link_SendStringW(
   wchar_t * lpszString)
{
unsigned long dwReturn;

   if (NULL == rCbk.lpfnSendStringProcW)
      dwReturn = LINK_ERROR;
   else
   {
      dwReturn = (rCbk.lpfnSendStringProcW) (
                    lpszString);
   }

   return dwReturn;
}


