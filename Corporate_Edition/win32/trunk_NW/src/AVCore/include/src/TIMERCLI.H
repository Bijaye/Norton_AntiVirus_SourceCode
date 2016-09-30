/////////////////////////////////////////////////////////////////////
// TimerCLI.h
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1992 - 2003, 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/timercli.h_v   1.0   11 Dec 1996 16:11:46   DSACKIN  $
//
//    Here we will declare all of the functions that will be used by
// the client into timerc.lib
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/timercli.h_v  $
// 
//    Rev 1.0   11 Dec 1996 16:11:46   DSACKIN
// Initial revision.
// 
//    Rev 1.0   08 Nov 1996 11:35:42   DKEEFER
// Initial revision
// 
//    Rev 1.0   13 Jun 1996 00:36:20   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__TIMERCLI_H)
#define __TIMERCLI_H

#define MAX_STRING_LENGTH   (256)
/////////////////////////////////////////////////////////////////////
// Function declarations

class CTimerClient
{

private:
BOOL    m_bConnected;
WCHAR   m_szProtocol[MAX_STRING_LENGTH];
WCHAR   m_szAddress[MAX_STRING_LENGTH];
WCHAR   m_szEndPoint[MAX_STRING_LENGTH];

public:
   CTimerClient();
   ~CTimerClient();

   BOOL SetProtocol(
      LPWSTR lpszProtocol,              // [in] - New protocol
      LPWSTR lpszAddress,               // [in] - New address
      LPWSTR lpszEndPoint);             // [in] - New endpoint

   BOOL SetProtocol(
      LPSTR lpszProtocol,               // [in] - New protocol
      LPSTR lpszAddress,                // [in] - New address
      LPSTR lpszEndPoint);              // [in] - New endpoint

   BOOL Connect(VOID);

   BOOL DisConnect(VOID);

   BOOL LogMessage(
      LPTSTR lpszMessage);              // [in] - Message to log

#ifdef _UNICODE

   BOOL LogMessage(
      LPSTR lpszMessage);               // [in] - Message to log

#else // ifdef _UNICODE

   BOOL LogMessage(
      LPWSTR lpszMessage);              // [in] - Message to log

#endif // ifdef _UNICODE
};

#endif // if !defined(__TIMERCLI_H)

