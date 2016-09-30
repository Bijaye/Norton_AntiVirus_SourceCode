//////////////////////////////////////////////////////////////////////
// LinkSrv.h
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/linksrv.h_v   1.3   14 Aug 1997 19:24:12   DSACKIN  $
//
//    Here we declare the interface for the CLinkServer object
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/linksrv.h_v  $
// 
//    Rev 1.3   14 Aug 1997 19:24:12   DSACKIN
// Ported from the Trunk.
// 
//    Rev 1.2   14 Aug 1997 18:09:30   DSACKIN
// StopListening now takes an interface handle to unregister.
//
//    Rev 1.1   12 Aug 1997 20:31:04   DSACKIN
// Added the ability to turn off SAPing.
//
//    Rev 1.0   09 May 1997 18:41:00   ksackin
// Initial revision.
//
//    Rev 1.2   06 May 1997 20:07:52   TSmith
// Added overloaded 'StartListen' function declaration that accepts an
// RPC_IF_HANDLE array and an array element count.
//
//    Rev 1.0   13 Mar 1997 15:10:36   RFULLER
// Initial revision
//
//    Rev 1.0   08 Nov 1996 11:35:36   DKEEFER
// Initial revision
//
//    Rev 1.3   07 Nov 1996 14:51:32   DSACKIN
// Added support for dynamic endpoints.
//
//    Rev 1.2   28 Aug 1996 14:36:20   DSACKIN
// Added a m_bUseSecurity flag and a couple of parameters in SetProtocol
// to go with it.
//
//    Rev 1.1   22 Jul 1996 17:02:44   DSACKIN
// Added SendString
//
//    Rev 1.0   18 Jul 1996 23:42:02   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined (__LINKSRV_H)
#define __LINKSRV_H

#if !defined(__RPC_H__)
#  include "rpc.h"                      // For RPC_IF_HANDLE
#endif

#if !defined (__LINKCBK_H)
#  include "LinkCbk.h"
#endif

#define MAX_STRING_LENGTH   (256)

/////////////////////////////////////////////////////////////////////
// Function declarations

/////////////////////////////////////////////////////////////////////
// CLinkServer class

class CLinkServer
{
private:
   BOOL    m_bListening;
   WCHAR   m_szProtocol[MAX_STRING_LENGTH];
   WCHAR   m_szEndPoint[MAX_STRING_LENGTH];
   BOOL    m_bUseSecurity;
   BOOL    m_bUseAllProtocols;

   PSECURITY_DESCRIPTOR m_pSecurityDescriptor;

public:
   CLinkServer();

   ~CLinkServer();

   void SetCallbacks(
      PLINKCALLBACKS pCallbacks);       // [in] - List of callbacks to set


   BOOL SetProtocol(
      LPWSTR lpszProtocol,              // [in] - New protocol
      LPWSTR lpszEndPoint,              // [in] - New endpoint
      BOOL   bUseSecurity = TRUE,       // [in] - Use our context security?
      BOOL   bEnableIPXSPX = TRUE);     // [in] - Use to disable IPX/SPX SAP

   BOOL SetProtocol(
      LPSTR lpszProtocol,               // [in] - New protocol
      LPSTR lpszEndPoint,               // [in] - New endpoint
      BOOL  bUseSecurity = TRUE,        // [in] - Use our context security?
      BOOL  bEnableIPXSPX = TRUE);      // [in] - Use to disable IPX/SPX SAP

   BOOL StartListen(
     RPC_IF_HANDLE aIfHandle[],         // [in] - Array of interface handles.
     DWORD         dwHandles);          // [in] - Count of array elements.


   BOOL StartListen( );                 // Uses default interface (Linksrv_handle)

   BOOL StopListen(
     RPC_IF_HANDLE aIfHandle[],         // [in] - Array of interface handles.
     DWORD         dwHandles);          // [in] - Count of array elements.

   BOOL StopListen( );

   virtual DWORD SendPacket(
      LPVOID pPacket);                  // [in] - packet to send

   virtual DWORD SendString(
      LPSTR lpszString);                // [in] - string received

   virtual DWORD SendString(
      LPWSTR lpszStringW);              // [in] - string received

};


#endif // !defined (__LINKSRV_H)
