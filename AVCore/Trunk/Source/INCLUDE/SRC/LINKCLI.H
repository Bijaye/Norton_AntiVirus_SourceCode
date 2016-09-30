/////////////////////////////////////////////////////////////////////
// LinkCli.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/LINKCLI.H_v   1.0   09 May 1997 18:40:58   ksackin  $
//
//    Here we will declare all of the functions that will be used by
// the CLinkClient side
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/LINKCLI.H_v  $
// 
//    Rev 1.0   09 May 1997 18:40:58   ksackin
// Initial revision.
// 
//    Rev 1.2   06 May 1997 19:46:18   TSmith
// Added 'm_pBindingHandle' to class. Also overloaded the 'Connect' and
// 'Disconnect' functions to accept a RPC_BINDING_HANDLE pointer to enable
// binding to a user specified RPC interface.
//
//    Rev 1.0   13 Mar 1997 15:10:40   RFULLER
// Initial revision
//
//    Rev 1.0   08 Nov 1996 11:36:02   DKEEFER
// Initial revision
//
//    Rev 1.2   07 Nov 1996 14:51:22   DSACKIN
// Added support for dynamic endpoints.
//
//    Rev 1.1   22 Jul 1996 17:02:24   DSACKIN
// Added SendString
//
//    Rev 1.0   18 Jul 1996 23:41:22   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__LINKCLI_H)
#define __LINKCLI_H

#define MAX_STRING_LENGTH   (256)
/////////////////////////////////////////////////////////////////////
// Function declarations

class CLinkClient
{

private:
RPC_BINDING_HANDLE* m_pBindingHandle;
BOOL                m_bConnected;
WCHAR               m_szProtocol[MAX_STRING_LENGTH];
WCHAR               m_szAddress[MAX_STRING_LENGTH];
WCHAR               m_szEndPoint[MAX_STRING_LENGTH];
BOOL                m_bUseDynamicEndpoints;
public:
   CLinkClient();
   ~CLinkClient();

   BOOL SetProtocol(
      LPCWSTR lpszProtocol,              // [in] - New protocol
      LPCWSTR lpszAddress,               // [in] - New address
      LPCWSTR lpszEndPoint);             // [in] - New endpoint

   BOOL SetProtocol(
      LPCSTR lpszProtocol,               // [in] - New protocol
      LPCSTR lpszAddress,                // [in] - New address
      LPCSTR lpszEndPoint);              // [in] - New endpoint

   BOOL Connect( VOID );
   BOOL Connect( RPC_BINDING_HANDLE* pBindingHandle );

   BOOL DisConnect( VOID );
   BOOL DisConnect( RPC_BINDING_HANDLE* pBindingHandle );

   virtual DWORD SendPacket(
      LPVOID pPacket);                  // [in] - Packet to send

   virtual DWORD SendCommand(
      DWORD dwCommand);                 // [in] - Command to send to server

   DWORD SendString(
      LPSTR lpszString);                // [in] - Command to send to server

   DWORD SendString(
      LPWSTR lpszStringW);              // [in] - Command to send to server

};

#endif // if !defined(__LINKCLI_H)

