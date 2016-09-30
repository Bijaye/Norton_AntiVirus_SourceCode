/////////////////////////////////////////////////////////////////////
// TapsCLI.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/include/VCS/tapscli.h_v   1.0   06 Feb 1997 20:51:46   RFULLER  $
//
//    Here we will declare all of the functions that will be used by
// the client into Tapsc.lib
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/include/VCS/tapscli.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:46   RFULLER
// Initial revision
// 
//    Rev 1.0   18 Jul 1996 18:48:46   DSACKIN
// Initial revision.
// 
//    Rev 1.0   18 Jul 1996 18:48:20   DSACKIN
// Initial revision.
//
//    Rev 1.0   13 Jun 1996 00:36:20   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__TAPSCLI_H)
#define __TAPSCLI_H

#define MAX_STRING_LENGTH   (256)
/////////////////////////////////////////////////////////////////////
// Function declarations

class CTapsClient
{

private:
BOOL    m_bConnected;
WCHAR   m_szProtocol[MAX_STRING_LENGTH];
WCHAR   m_szAddress[MAX_STRING_LENGTH];
WCHAR   m_szEndPoint[MAX_STRING_LENGTH];

public:
   CTapsClient();
   ~CTapsClient();

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

   DWORD SendPacket(
      LPVOID pPacket);                  // [in] - Packet to send

   DWORD SendCommand(
      DWORD dwCommand);                 // [in] - Command to send to server

};

#endif // if !defined(__TAPSCLI_H)

