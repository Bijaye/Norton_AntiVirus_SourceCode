/////////////////////////////////////////////////////////////////////
// TimerSRV.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/include/VCS/timersrv.h_v   1.0   06 Feb 1997 20:51:40   RFULLER  $
//
//    Here we will declare all of the functions that will be used by
// the server into Timers.lib
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/include/VCS/timersrv.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:40   RFULLER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:36:28   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined (__TIMERSRV_H)
#define __TIMERSRV_H

#define MAX_STRING_LENGTH   (256)

#if !defined (__TIMERCBK_H)
#  include "TimerCBK.h"
#endif

/////////////////////////////////////////////////////////////////////
// Function declarations

/////////////////////////////////////////////////////////////////////
// CTimerServer class

class CTimerServer
{
private:
   BOOL    m_bListening;
   WCHAR   m_szProtocol[MAX_STRING_LENGTH];
   WCHAR   m_szEndPoint[MAX_STRING_LENGTH];

public:
   CTimerServer();
   ~CTimerServer();

   void SetCallbacks(
      PSERVERCALLBACKS pCallbacks);     // [in] - List of callbacks to set


   BOOL SetProtocol(
      LPWSTR lpszProtocol,              // [in] - New protocol
      LPWSTR lpszEndPoint);             // [in] - New endpoint

   BOOL SetProtocol(
      LPSTR lpszProtocol,               // [in] - New protocol
      LPSTR lpszEndPoint);              // [in] - New endpoint

   BOOL StartListen(VOID);

   BOOL StopListen(VOID);

   virtual BOOL LogMessage(
      DWORD  dwMilliseconds,            // [in] - Time stamp
      LPTSTR lpszMessage);              // [in] - Message to log

#ifdef _UNICODE

   virtual BOOL LogMessage(
      DWORD dwMilliseconds,
      LPSTR lpszMessage);

#else // ifdef _UNICODE

   virtual BOOL LogMessage(
      DWORD  dwMilliseconds,
      LPWSTR lpszMessage);

#endif // ifdef _UNICODE

};


#endif // !defined (__TIMERSRV_H)
