/////////////////////////////////////////////////////////////////////
// TimerSRV.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/timersrv.cpv   1.0   09 May 1997 17:58:24   ksackin  $
//
//    Here is where I will define the interface for the CTimerServer
// object.  Use this object to list for CTimerClient s
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/timersrv.cpv  $
// 
//    Rev 1.0   09 May 1997 17:58:24   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:08:58   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:16   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:46:08   DKEEFER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:37:42   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <rpc.h>
#include <tchar.h>

#include <iostream.h>

#include "timer.h"
#include "TimerCBK.h"
#include "Timersrv.h"

/////////////////////////////////////////////////////////////////////
// CTimerServer::CTimerServer
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
CTimerServer::CTimerServer()
{
   m_bListening = FALSE;

   _tcscpy (m_szProtocol, TEXT("ncacn_ip_tcp") );
   _tcscpy (m_szEndPoint, TEXT("4444") );
}


/////////////////////////////////////////////////////////////////////
// CTimerServer::~CTimerServer
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
CTimerServer::~CTimerServer()
{
}


/////////////////////////////////////////////////////////////////////
// CTimerServer::SetCallbacks
//
// This function will set our callbacks for the RPC function(s)
//
// Parameters:
//    PSERVERCALLBACKS - pCallbacks - [in] - List of callbacks to set
//
// Returns:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 06/09/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CTimerServer::SetCallbacks(
      PSERVERCALLBACKS pCallbacks)      // [in] - List of callbacks to set
{
   ::SetCallbacks (pCallbacks);         // Call directly into the RPC C function
}

/////////////////////////////////////////////////////////////////////
// CTimerServer::SetProtocol
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
BOOL CTimerServer::SetProtocol(
   LPWSTR lpszProtocol,                 // [in] - New protocol
   LPWSTR lpszEndPoint)                 // [in] - New endpoint
{

   if ( m_bListening )
      return FALSE;

   if ( (NULL == lpszProtocol) ||
        (NULL == lpszEndPoint) )
      return FALSE;

   wcscpy (m_szProtocol, lpszProtocol);
   wcscpy (m_szEndPoint, lpszEndPoint);

   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CTimerServer::SetProtocol
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
BOOL CTimerServer::SetProtocol(
   LPSTR lpszProtocol,                  // [in] - New protocol
   LPSTR lpszEndPoint)                  // [in] - New endpoint
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
               szEndPointW) );
}



/////////////////////////////////////////////////////////////////////
// CTimerClient::StartListen
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
BOOL CTimerServer::StartListen(VOID)
{
RPC_STATUS rStatus;
BOOL       bReturn = TRUE;

   RpcTryExcept
   {
                                        // use the specified protocol
                                        // and endpoint
      rStatus = RpcServerUseProtseqEp(
                   m_szProtocol,
                   RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
                   m_szEndPoint,
                   NULL);               // No security descriptor

      if ( RPC_S_OK != rStatus )
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // register the interface

      if (RpcServerRegisterIf(
             Timer_v1_0_s_ifspec,
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
// CTimerServer::StopListen
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
BOOL CTimerServer::StopListen(VOID)
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
// CTimerServer::LogMessage
//
// This function will log the messages that are coming in.
// UNICODE version
//
// Parameters:
//    DWORD  - dwMilliseconds - [in] Time stamp
//    LPWSTR - lpszMessage    - [in] Message to log (UNICODE)
//
//
// Returns:
//    TRUE  - Server is stopped.
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 06/11/96 DSACKIN - Added UNICODE support
/////////////////////////////////////////////////////////////////////
BOOL CTimerServer::LogMessage(
   DWORD  dwMilliseconds,               // [in] - Time stamp
   LPWSTR lpszMessage)                  // [in] - Message to log
{
   return TRUE;
}


/////////////////////////////////////////////////////////////////////
// CTimerServer::LogMessage
//
// This function will log the messages that are coming in.
// NON-UNICODE version
//
// Parameters:
//    DWORD  - dwMilliseconds - [in] Time stamp
//    LPSTR  - lpszMessage    - [in] Message to log (NON-UNICODE)
//
//
// Returns:
//    TRUE  - Server is stopped.
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 06/11/96 DSACKIN - Added UNICODE support
/////////////////////////////////////////////////////////////////////
BOOL CTimerServer::LogMessage(
   DWORD  dwMilliseconds,               // [in] - Time stamp
   LPSTR lpszMessage)                   // [in] - Message to log
{
   return TRUE;
}


