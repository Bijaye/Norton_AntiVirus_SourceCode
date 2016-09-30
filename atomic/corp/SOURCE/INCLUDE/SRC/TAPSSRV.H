/////////////////////////////////////////////////////////////////////
// TapsSRV.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/include/VCS/tapssrv.h_v   1.0   06 Feb 1997 20:51:48   RFULLER  $
//
//    Here we will declare all of the functions that will be used by
// the server into Tapss.lib
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/include/VCS/tapssrv.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:48   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Jul 1996 18:50:04   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Jun 1996 00:36:28   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined (__TAPSSRV_H)
#define __TAPSSRV_H

#define MAX_STRING_LENGTH   (256)

#if !defined (__TAPSCBK_H)
#  include "TapsCBK.h"
#endif

/////////////////////////////////////////////////////////////////////
// Function declarations

/////////////////////////////////////////////////////////////////////
// CTapsServer class

class CTapsServer
{
private:
   BOOL    m_bListening;
   WCHAR   m_szProtocol[MAX_STRING_LENGTH];
   WCHAR   m_szEndPoint[MAX_STRING_LENGTH];

public:
   CTapsServer();
   ~CTapsServer();

   void SetCallbacks(
      PTAPSCALLBACKS pCallbacks);       // [in] - List of callbacks to set


   BOOL SetProtocol(
      LPWSTR lpszProtocol,              // [in] - New protocol
      LPWSTR lpszEndPoint);             // [in] - New endpoint

   BOOL SetProtocol(
      LPSTR lpszProtocol,               // [in] - New protocol
      LPSTR lpszEndPoint);              // [in] - New endpoint

   BOOL StartListen(VOID);

   BOOL StopListen(VOID);

   virtual DWORD SendPacket(
      LPVOID pPacket);                  // [in] - packet to send

};


#endif // !defined (__TAPSSRV_H)
