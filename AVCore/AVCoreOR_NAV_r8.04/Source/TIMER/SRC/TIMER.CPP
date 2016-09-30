/////////////////////////////////////////////////////////////////////
// Timer.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/timer.cpv   1.0   09 May 1997 17:58:20   ksackin  $
//
// This is where the functions declared by our RPC clients go.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/timer.cpv  $
// 
//    Rev 1.0   09 May 1997 17:58:20   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:08:54   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:16   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:46:04   DKEEFER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:34:00   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>

#include <rpc.h>
#include <rpcndr.h>

#include "timer.h"                      // Generated header file

#include "timercbk.h"

SERVERCALLBACKS rCbk = { NULL, NULL };

/////////////////////////////////////////////////////////////////////
// SetCallbacks
//
//    This function will set up the callbacks for our RPC functions
//
// Parameters:
//    PSERVERCALLBACKS - pCallbacks  - List of callbacks to set
//
/////////////////////////////////////////////////////////////////////
void SetCallbacks(
   PSERVERCALLBACKS pCallbacks)         // [in] - List of callbacks to set
{
   memcpy (&rCbk, pCallbacks, sizeof (SERVERCALLBACKS));
}

/////////////////////////////////////////////////////////////////////
// Start of our RPC functions

/////////////////////////////////////////////////////////////////////
// Server_LogMessageA
//
//    RPC function on the server machine that calls our Log Callback
//
// Parameters:
//    unsigned long - dwMilliseconds - Time stamp to put on our logged event
//    byte *        - lpszMessage    - Message to log
//
/////////////////////////////////////////////////////////////////////
boolean Server_LogMessageA(
   unsigned long  dwMilliseconds,
   byte *         lpszMessage)
{
BOOL bReturn;

   if (NULL == rCbk.lpfnLogMessageA)
      bReturn = FALSE;
   else
   {
      bReturn = (rCbk.lpfnLogMessageA) (
                  dwMilliseconds,
                  (LPSTR) lpszMessage);
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// Server_LogMessageW
//
//    RPC function on the server machine that calls our Log Callback
//    with UNICODE support
//
// Parameters:
//    unsigned long - dwMilliseconds - Time stamp to put on our logged event
//    byte *        - lpszMessage    - Message to log
//
/////////////////////////////////////////////////////////////////////
boolean Server_LogMessageW(
   unsigned long  dwMilliseconds,
   wchar_t *      lpszMessage)
{
BOOL bReturn;

   if (NULL == rCbk.lpfnLogMessageW)
      bReturn = FALSE;
   else
   {
      bReturn = (rCbk.lpfnLogMessageW) (
                  dwMilliseconds,
                  (LPWSTR) lpszMessage);
   }

   return bReturn;
}


