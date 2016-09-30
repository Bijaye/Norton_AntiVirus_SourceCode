/////////////////////////////////////////////////////////////////////
// NLMMsgr.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVSERVE/VCS/nlmmsgr.h_v   1.4   13 Feb 1998 18:39:10   MKEATIN  $
//
// Description:
//      This is the interface header file for our CNLMMessenger object.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVSERVE/VCS/nlmmsgr.h_v  $
// 
//    Rev 1.4   13 Feb 1998 18:39:10   MKEATIN
// Roswell Port.
// 
//    Rev 1.4   12 Jan 1998 16:48:28   CEATON
// Modified Winsock to be loaded only when server is notified.
// 
// 
//    Rev 1.3   22 Dec 1997 17:30:18   CEATON
// Added function pointers for Winsock calls.
// 
// 
//    Rev 1.2   18 Apr 1997 14:01:58   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:14:04   RFULLER
// Initial revision
// 
//    Rev 1.1   13 Dec 1996 13:58:54   DSACKIN
// Added a function used for the new NLM Enumeration callback function
//
//    Rev 1.0   08 Nov 1996 11:53:32   DKEEFER
// Initial revision
//
//    Rev 1.1   26 Aug 1996 00:44:10   DSACKIN
// Added m_dwRequestCout variable for new alert threads to request a list
// be started if there isn't one already existing.
//
//    Rev 1.0   20 Aug 1996 02:25:08   DSACKIN
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////

#if !defined(__NLMMSGR_H)

#define __NLMMSGR_H

/////////////////////////////////////////////////////////////////////

typedef int ( PASCAL FAR *CLOSESOCKET )(SOCKET s);
typedef int ( PASCAL FAR *WSASTARTUP )(WORD wVersionRequired, LPWSADATA lpWSAData);
typedef int ( PASCAL FAR *WSACLEANUP )();
typedef int ( PASCAL FAR *WSAGETLASTERROR )();
typedef int ( PASCAL FAR *SENDTO  )(SOCKET s, const char FAR * buf, int len, int flags,
                              const struct sockaddr FAR *to, int tolen);
typedef SOCKET ( PASCAL FAR *_SOCKET  )(int af, int type, int protocol);
typedef INT ( APIENTRY *GETADDRESSBYNAME )(
    IN     DWORD                dwNameSpace,
    IN     LPGUID               lpServiceType,
    IN     LPSTR              lpServiceName OPTIONAL,
    IN     LPINT                lpiProtocols OPTIONAL,
    IN     DWORD                dwResolution,
    IN     LPSERVICE_ASYNC_INFO lpServiceAsyncInfo OPTIONAL,
    IN OUT LPVOID               lpCsaddrBuffer,
    IN OUT LPDWORD              lpdwBufferLength,
    IN OUT LPSTR              lpAliasBuffer OPTIONAL,
    IN OUT LPDWORD              lpdwAliasBufferLength OPTIONAL
    );

// defines used by this class

#define SZ_SEPARATOR ";"
#define SZ_NAV       "NAV"
#define SZ_NETWARE   "NetWare"

                                        // sleep time in ms between times
                                        // we look for new stuff on the list
#define LIST_SLEEP_INTERVAL  250
                                        // Wait a whole second before checking
                                        // if we are done sleeping our long time
#define LIST_WAIT_INTERVAL   1000
                                        // Update our list every hour or 3600000ms
#define LIST_UPDATE_INTERVAL 3600000

                                        // Used in calls to _beginthread
#define DEFAULT_STACK_SIZE   8192

                                        // Used in WinSock setup
#define MAX_PROTOCOLS        10

                                        // Support a max of 10 addresses
                                        // per lookup.
#define MAX_ADDRESSES        10

                                        // Used in querying network types
#define MAX_NETWORK_TYPES    10

/////////////////////////////////////////////////////////////////////
// Struct used in our linked list.

typedef struct tagSERVERSTRINGNODE
{
   TCHAR        szServerName[NETMAXSERVERNAME + 1];
   SOCKADDR_IPX rIPXAddress;

   struct tagSERVERSTRINGNODE *pNext;

} SERVERSTRINGNODE, * PSERVERSTRINGNODE;

typedef struct tagSERVERSTRINGLIST
{
   PSERVERSTRINGNODE pList;
   DWORD             dwUsageCount;
   BOOL              bDoneAdding;       // Done adding servers to this list?
} SERVERSTRINGLIST, * PSERVERSTRINGLIST;

/////////////////////////////////////////////////////////////////////
// CNLMMessenger class

class CNLMMessenger
{
private:
   BOOL              m_bInitialized;
   BOOL              m_bExitingMessenger;

                                        // Constants used for sending
                                        // to a specific NAVNLM server
   GUID              m_guid;
   int               m_anProtocols[MAX_PROTOCOLS];
   DWORD             m_dwProtocolSize;
   HINSTANCE         m_hWinsockLibrary;

   /* Explicitly linked Winsock functions */
   CLOSESOCKET lpclosesocket;
   WSASTARTUP lpWSAStartup;
   WSACLEANUP lpWSACleanup;
   WSAGETLASTERROR lpWSAGetLastError;
   SENDTO lpsendto;
   _SOCKET lpsocket;
   GETADDRESSBYNAME lpGetAddressByName;

/////////////////////////////////////////////////
// Server List Variables

   PSERVERSTRINGLIST m_pCurrentList;

   PSERVERSTRINGLIST m_pRemovingList;

   DWORD             m_dwRequestCount;  // Used for threads waiting for a list.

   CRITICAL_SECTION  m_hListCritical;   // Variable for our semaphore stuff

public:

   CNLMMessenger();
   ~CNLMMessenger();

                                        // Must be initialized before calling
                                        // other member functions
   DWORD Init(VOID);

/////////////////////////////////////////////////
// Notification functions

   DWORD NotifyNLMServer(
      LPTSTR szVirus,                   // [in] Virus name that was found
      LPTSTR szFile,                    // [in] File virus was found in
      LPTSTR szUser,                    // [in] user that found the virus
      LPTSTR szServer);                 // [in] server to notify

   DWORD NotifyNLMServer(
      LPTSTR szVirus,                   // [in] Virus name that was found
      LPTSTR szFile,                    // [in] File virus was found in
      LPTSTR szUser,                    // [in] user that found the virus
      SOCKADDR_IPX *pAddress);          // [in] address of server to notify

                                        // Application people can call
                                        // this function to wait for the
                                        // return.
   DWORD NotifyOurServers(
      LPTSTR szVirus,                   // [in] Virus name that was found
      LPTSTR szFile,                    // [in] File virus was found in
      LPTSTR szUser);                   // [in] user that found the virus

                                        // Application people should call
                                        // this function for an immediate
                                        // return.
   BOOL NotifyAllNLMServers(
      LPTSTR szVirus,                   // [in] Virus name that was found
      LPTSTR szFile,                    // [in] File virus was found in
      LPTSTR szUser);                   // [in] user that found the virus


/////////////////////////////////////////////////
// Server List functions
public:
                                        // This function needs to remain public
                                        // becuase it is going to be called
                                        // from a callback function
   BOOL NLMServerEnum (
      LPSTR  lpszServerName,               // [in] - Name of server to add
      LPVOID lpAddress);                   // [in] - Address of server

                                        // Internal functions
private:
   HINSTANCE InitializeWinsockLibrary();
   VOID FreeWinsockLibrary();

   BOOL StartNewServerList(VOID);

   BOOL AddServer(
      LPTSTR       szServerName,        // [in] server to notify
      SOCKADDR_IPX *pAddress);          // [in] address of our server

   BOOL RemoveAllServers(
      PSERVERSTRINGNODE pList);         // [in] - Pointer to the list to free

   BOOL DeleteRemovingList(VOID);

   BOOL BuildNewServerList(VOID);

                                        // The following is an entry point
                                        // called from _beginthread and MUST
                                        // remain public.
public:
   VOID MaintainServerList(VOID);


};

typedef CNLMMessenger * PCNLMMessenger;

/////////////////////////////////////////////////////////////////////
// Notify Thread information class

typedef struct tagSTARTNOTIFYINFO
{
                                        // Pointer to the object to call
   PCNLMMessenger pThis;
                                        // The following three strings will
                                        // be allocated and sent into a thread
   TCHAR     szVirus[SYM_MAX_PATH + 1];
   TCHAR     szFile[SYM_MAX_PATH + 1];
   TCHAR     szUser[SIZE_NET_USER_LIST + 1];

} STARTNOTIFYINFO, * PSTARTNOTIFYINFO;

#endif // if !defined(__NLMMSGR_H)


