/////////////////////////////////////////////////////////////////////
// TimerCLI.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992-1996, 2005 Symantec, Peter Norton Product Group. All rights reserved.
/////////////////////////////////////////////////////////////////////
// $Header:   S:/TIMER/VCS/timercli.cpv   1.0   09 May 1997 17:58:22   ksackin  $
//
//    Here is where I will define the interface for the CTimerClient
// object.  Use this object to talk to the RPC server.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/TIMER/VCS/timercli.cpv  $
// 
//    Rev 1.0   09 May 1997 17:58:22   ksackin
// Initial revision.
// 
//    Rev 1.1   18 Apr 1997 14:08:56   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:19:16   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 11:46:06   DKEEFER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:37:36   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <rpc.h>
#include <tchar.h>

#include "timer.h"
#include "Timercli.h"
#include "timercbk.h"

handle_t Timersrv_handle;
SERVERCALLBACKS rCbk = { NULL, NULL };

/////////////////////////////////////////////////////////////////////
// CTimerClient::CTimerClient
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
CTimerClient::CTimerClient()
{
   m_bConnected = FALSE;

   _tcscpy (m_szProtocol, TEXT("ncacn_ip_tcp"));
   _tcscpy (m_szAddress,  TEXT("mule"));
   _tcscpy (m_szEndPoint, TEXT("4444"));
}


/////////////////////////////////////////////////////////////////////
// CTimerClient::~CTimerClient
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
CTimerClient::~CTimerClient()
{
   if ( m_bConnected )
      DisConnect();
}


/////////////////////////////////////////////////////////////////////
// CTimerClient::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPWSTR - lpszProtocol - String containing which protocol - "ncacn_ip_tcp"
//    LPWSTR - lpszAddress  - String containing address - "mule"
//    LPWSTR - lpszEndPoint - String containing EndPoint - "4444"
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Client is connected or a null sting was entered
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTimerClient::SetProtocol(
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
// CTimerClient::SetProtocol
//
// This function will set our protocol strings
//
// Parameters:
//    LPTSTR - lpszProtocol - String containing which protocol - "ncacn_ip_tcp"
//    LPTSTR - lpszAddress  - String containing address - "mule"
//    LPTSTR - lpszEndPoint - String containing EndPoint - "4444"
//
// Returns:
//    TRUE  - Protocols were set
//    FALSE - Client is connected or a null sting was entered
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
// 06/11/96 DSACKIN - Added UNICODE support
/////////////////////////////////////////////////////////////////////
BOOL CTimerClient::SetProtocol(
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
// CTimerClient::Connect
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
/////////////////////////////////////////////////////////////////////
BOOL CTimerClient::Connect(VOID)
{
BOOL   bReturn = TRUE;
RPC_WSTR rpcwstrStringBinding = NULL;

   if ( m_bConnected )
      return FALSE;

   RpcTryExcept
   {
                                        // Put together binding

      if ( RpcStringBindingCompose(
            NULL,                       // No UUID for us
            (RPC_WSTR)m_szProtocol,
            (RPC_WSTR)m_szAddress,
            (RPC_WSTR)m_szEndPoint,
            NULL,                       // No special network stuff
            &rpcwstrStringBinding) )           // This is returned
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // bind to server using string binding info

      if (RpcBindingFromStringBinding(
            rpcwstrStringBinding,
            &Timersrv_handle))
      {
         bReturn = FALSE;
         goto Cleanup;
      }

                                        // OK.. Now we know we are connected.
      m_bConnected = TRUE;

Cleanup:

      if ( rpcwstrStringBinding )
         RpcStringFree(&rpcwstrStringBinding);

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
// CTimerClient::DisConnect
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
/////////////////////////////////////////////////////////////////////
BOOL CTimerClient::DisConnect(VOID)
{
BOOL bReturn = TRUE;

   if ( !m_bConnected )
      return FALSE;

   RpcTryExcept
   {
                                        // release binding to server
      if (RpcBindingFree(&Timersrv_handle))
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
// CTimerClient::LogMessage
//
// This function will take the system time and log the event
//
// Parameters:
//    LPWSTR - lpszMessage - [in] UNICODE message to log
//
// Returns:
//    TRUE  - Event logged correctly
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTimerClient::LogMessage(
   LPWSTR lpszMessage)                  // [in] - Message to log
{
DWORD dwMilliseconds;
BOOL  bReturn = TRUE;

   if ( !m_bConnected )
      return FALSE;

   if ( NULL == lpszMessage )
      return FALSE;

   dwMilliseconds = GetTickCount();

   RpcTryExcept
   {
      bReturn = Server_LogMessageW (
                  dwMilliseconds,
                  lpszMessage);
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
// CTimerClient::LogMessage
//
// This function will take the system time and log the event
//
// Parameters:
//    LPSTR - lpszMessage - [in] NONUNICODE message to log
//
// Returns:
//    TRUE  - Event logged correctly
//    FALSE - Some kind of failure
//
/////////////////////////////////////////////////////////////////////
// 06/07/96 DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CTimerClient::LogMessage(
   LPSTR lpszMessage)                   // [in] - Message to log
{
DWORD dwMilliseconds;
BOOL  bReturn = TRUE;

   if ( !m_bConnected )
      return FALSE;

   if ( NULL == lpszMessage )
      return FALSE;

   dwMilliseconds = GetTickCount();

   RpcTryExcept
   {
      bReturn = Server_LogMessageA (
                  dwMilliseconds,
                  (UCHAR *) lpszMessage);
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

