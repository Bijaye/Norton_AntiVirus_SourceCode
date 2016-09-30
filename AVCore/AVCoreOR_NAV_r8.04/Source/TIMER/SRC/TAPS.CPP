/////////////////////////////////////////////////////////////////////
// Taps.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/TAPS.CPv   1.0   09 May 1997 17:58:28   ksackin  $
//
// This is where the functions declared by our RPC clients go.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/TAPS.CPv  $
// 
//    Rev 1.0   09 May 1997 17:58:28   ksackin
// Initial revision.
// 
//    Rev 1.0   06 Feb 1997 21:09:18   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Jul 1996 18:45:24   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Jun 1996 00:34:00   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>

#include <rpc.h>
#include <rpcndr.h>

#include "Taps.h"                      // Generated header file

#include "Tapscbk.h"

#include "apscomm.h"

TAPSCALLBACKS rCbk = { NULL };

/////////////////////////////////////////////////////////////////////
// SetCallbacks
//
//    This function will set up the callbacks for our RPC functions
//
// Parameters:
//    PTAPSCALLBACKS - pCallbacks  - List of callbacks to set
//
/////////////////////////////////////////////////////////////////////
void SetCallbacks(
   PTAPSCALLBACKS pCallbacks)           // [in] - List of callbacks to set
{
   memcpy (&rCbk, pCallbacks, sizeof (TAPSCALLBACKS));
}

/////////////////////////////////////////////////////////////////////
// Start of our RPC functions

/////////////////////////////////////////////////////////////////////
// NAVAPService_SendCommand
//
//    RPC function on the server machine that receives our commands
//
// Parameters:
//    unsigned long - dwSize  - size of the structure coming through
//    byte *        - pPacket - reference to our packet being received
//
/////////////////////////////////////////////////////////////////////
unsigned long NAVAPService_SendCommand(
   unsigned long  dwSize,
   byte *         pPacket)
{
unsigned long dwReturn;

   if (NULL == rCbk.lpfnSendCommandProc)
      dwReturn = NAVAPSVC_ERROR;
   else
   {
      dwReturn = (rCbk.lpfnSendCommandProc) (
                    pPacket);
   }

   return dwReturn;
}


