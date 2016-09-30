/////////////////////////////////////////////////////////////////////
// NLMMsgr.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVSERVE/VCS/nlmmsgr.cpv   1.4   13 Feb 1998 18:39:06   MKEATIN  $
//
// Description:
//      This is the implementation file for the CNLMMessenger class.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVSERVE/VCS/nlmmsgr.cpv  $
// 
//    Rev 1.4   13 Feb 1998 18:39:06   MKEATIN
// Roswell Port.
// 
//    Rev 1.4   15 Jan 1998 11:45:22   CEATON
// Further changes to load Winsock DLL "just-in-time".
// 
// 
//    Rev 1.3   22 Dec 1997 17:29:28   CEATON
// Modified so that Winsock calls are made via function pointers.
// 
// 
//    Rev 1.2   18 Apr 1997 14:02:00   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:14:04   RFULLER
// Initial revision
// 
//    Rev 1.1   13 Dec 1996 13:53:36   DSACKIN
// Use the new NetWare server enumeration function to get the server names.
//
//    Rev 1.0   08 Nov 1996 11:53:34   DKEEFER
// Initial revision
//
//    Rev 1.3   12 Sep 1996 11:14:18   jmillar
// back of last fix - it breaks things.
//
//    Rev 1.2   05 Sep 1996 14:57:24   DSACKIN
// Close the handle that we receive from _beginthread when sending alerts.
// All of the other handles will be closed when we exit this DLL.  Technically
// they should be closed also, but we are too close to ship to re-build all
// of those project.
//
//    Rev 1.1   26 Aug 1996 00:43:26   DSACKIN
// Added code to wait until someone requests a new list before building one.
// We will still wait a whole hour between lists, but won't start the new
// one until someone requires it.  (Makes us MUCH more efficient).
//
//    Rev 1.0   20 Aug 1996 02:25:38   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "platform.h"

#include "process.h"
#include "winsock.h"
#include "nspapi.h"
#include "wsipx.h"
#include "svcguid.h"

#include "nxtw.h"                       // API prototypes
#include "nwbindry.h"       // ScanBinderyObject

#define _SERVENFY_C
#include "servenfy.h"

#include "symnet.h"
#include "netobj.h"

#include "NLMMsgr.h"


/////////////////////////////////////////////////////////////////////
// Local defines.

/////////////////////////////////////////////////////////////////////
// Local prototypes.

VOID MaintainServerListProc(
   LPVOID lpMessenger);

/////////////////////////////////////////////////////////////////////
// CNLMMessenger : Constructor
//
// This function initializes all of our member variables.
//
// Parameters:
//    none.
//
// Return Value:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CNLMMessenger::CNLMMessenger()
{
   m_bInitialized      = FALSE;
   m_bExitingMessenger = FALSE;

   m_pCurrentList      = NULL;
   m_pRemovingList     = NULL;
   m_dwRequestCount    = 0;

                                        // Set up our critical sections
   InitializeCriticalSection(&m_hListCritical);

}

HINSTANCE CNLMMessenger::InitializeWinsockLibrary()

{
   WORD    wVersion = MAKEWORD (1, 1);       // WinSock 1.1 required
   WSAData wsaData;

   m_hWinsockLibrary = NULL;

   if ( ( m_hWinsockLibrary = LoadLibrary( "WSOCK32.DLL" ) ) != NULL ) {
       lpclosesocket = ( CLOSESOCKET ) GetProcAddress( m_hWinsockLibrary,
                                      "closesocket" );
       lpWSAStartup = ( WSASTARTUP ) GetProcAddress( m_hWinsockLibrary,
                                      "WSAStartup" );
       lpWSACleanup = ( WSACLEANUP ) GetProcAddress( m_hWinsockLibrary,
                                      "WSACleanup" );
       lpWSAGetLastError = ( WSAGETLASTERROR ) GetProcAddress( m_hWinsockLibrary,
                                      "WSAGetLastError" );
       lpsendto  =( SENDTO )  GetProcAddress( m_hWinsockLibrary,
                                      "sendto" );
       lpsocket  =( _SOCKET )  GetProcAddress( m_hWinsockLibrary,
                                      "socket" );
       lpGetAddressByName  =( GETADDRESSBYNAME )  GetProcAddress( m_hWinsockLibrary,
                                      "GetAddressByNameA" );

                                        // Check to see if we have WINSOCK 1.1 or greater
       if ( ( ( *lpWSAStartup )(wVersion, &wsaData) ) != ERROR_SUCCESS ) {
           FreeLibrary( m_hWinsockLibrary );

           m_hWinsockLibrary = NULL;
           }
       }

   return( m_hWinsockLibrary );
}

VOID CNLMMessenger::FreeWinsockLibrary()

{
   if ( m_hWinsockLibrary != NULL ) {
       ( *lpWSACleanup )();

       FreeLibrary( m_hWinsockLibrary );
       }
}

/////////////////////////////////////////////////////////////////////
// CNLMMessenger : Destructor
//
// This function cleans up after we are done.
//
// We start by cleaning up the removing list if it exists.  Once that
// is done, we switch the current list to the removing list and then
// go through the process of removing the removing-list again.
//
// Parameters:
//    none.
//
// Return Value:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CNLMMessenger::~CNLMMessenger()
{
                                        // Wait for everyone to exit!
   while ( (m_pCurrentList != NULL) &&
           (m_pRemovingList != NULL) )
   {
                                        // Delete the list if no-one
                                        // is in it.
      DeleteRemovingList();

                                        // otherwise, wait for it to
                                        // go to NULL.
      while (m_pRemovingList != NULL)
      {
         Sleep (LIST_SLEEP_INTERVAL);
      }

                                        // Now switch the lists and make
                                        // everyone else exit

                                        // Only one person can change list
                                        // Info at a given time.
      EnterCriticalSection(&m_hListCritical);

      m_pRemovingList = m_pCurrentList;
      m_pCurrentList  = NULL;           // No new list!!

                                        // Now let the next guy through
      LeaveCriticalSection(&m_hListCritical);

   }

}



/////////////////////////////////////////////////////////////////////
// CNLMMessenger::Init()
//
// This function sets up our class.  If we don't succeed in setting
// up our class, we can't allow any NLM Server interaction.
//
// Parameters:
//    none.
//
// Return Value:
//    DWORD - ERROR_SUCCESS - successful call
//          - error code if error occurs.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNLMMessenger::Init(VOID)
{
BOOL    dwReturn = ERROR_SUCCESS;
HANDLE  hThread;

   if (m_bInitialized)
   {
      dwReturn = ERROR_SUCCESS;
      goto Cleanup;
   }

                                        // Fill in the GUID
                                        // NAV NLM uses GUID 0x0531
   m_guid.Data1    = MAKELONG (MAKEWORD (0x31, 0x05), 0x000B);
   m_guid.Data2    = 0;
   m_guid.Data3    = 0;
   m_guid.Data4[0] = 0xC0;
   m_guid.Data4[1] = 0;
   m_guid.Data4[2] = 0;
   m_guid.Data4[3] = 0;
   m_guid.Data4[4] = 0;
   m_guid.Data4[5] = 0;
   m_guid.Data4[6] = 0;
   m_guid.Data4[7] = 0x46;

                                        // zero out the protocols.
   MEMSET (m_anProtocols, 0, sizeof (m_anProtocols));

   m_anProtocols[0] = NSPROTO_IPX;      // only IPX
                                        // set size based on
                                        // SOCKADDR_IPX (of our interest)
   m_dwProtocolSize = MAX_PROTOCOLS * sizeof (SOCKADDR_IPX);

                                        // Start a thread that will take
                                        // care of updating our server list.
   hThread = (HANDLE)_beginthread (
                  MaintainServerListProc,  // start Addr
                  DEFAULT_STACK_SIZE,      // stack size
                  (LPVOID) this);          // Pass the 'this' into our thread

   if ((HANDLE)INVALID_HANDLE_VALUE == hThread)
   {
      goto Cleanup;
   }
                                        // Flag it OK to use the functions!
   m_bInitialized = TRUE;

Cleanup:

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Notify functions begin here!
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// CNLMMessenger::NotifyNLMServer()
//
// This function sends the Virus Notification to the selected
// NetWare server.
//
// A socket is declared for each entry into this function.  This is
// done to make this function threadsafe.  You should be able to call
// this function from as many threads as you wish, and they should all
// be able to send their information simultaneously.
//
// Parameters:
//   LPTSTR szVirus         - [in] Virus name that was found
//   LPTSTR szFile          - [in] File virus was found in
//   LPTSTR szUser          - [in] user that found the virus
//   SOCKADDR_IPX *pAddress - [in] address of server to notify
//
// Return Value:
//    DWORD - Error Code.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNLMMessenger::NotifyNLMServer(
   LPTSTR szVirus,                      // [in] Virus name that was found
   LPTSTR szFile,                       // [in] File virus was found in
   LPTSTR szUser,                       // [in] user that found the virus
   SOCKADDR_IPX *pAddress)              // [in] address of server to notify
{
char         szMsg[IPX_BODY_SIZE];      // Buffer to make msg
DWORD        dwLength;                  // to be the length of szMessage
DWORD        dwReturn = ERROR_SUCCESS;

DWORD        dwSize;
SOCKET       s = INVALID_SOCKET;        // Socket to use to send info

   if (!m_bInitialized)
   {
                                        // Don't let them use this function
                                        // until they call Init()!
      dwReturn = ERROR_INVALID_FUNCTION;
      goto Cleanup;
   }

   if ( InitializeWinsockLibrary() == NULL )
   {
      dwReturn = ERROR_INVALID_FUNCTION;
      goto Cleanup;
   }

                                        // Get a socket to send msg
   s =  ( *lpsocket )(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);

                                        // Check our socket creation
   if (INVALID_SOCKET == s)
   {
      dwReturn = ( DWORD ) ( *lpWSAGetLastError )();
      goto Cleanup;
   }

                                        // Prepare the message.
   MEMSET (szMsg, 0, sizeof (szMsg));

   szMsg[0] = (BYTE)IPX_WORKSTATION_VIRUS;
   STRCAT (szMsg, SZ_NAV);
   STRCAT (szMsg, SZ_SEPARATOR);
   STRCAT (szMsg, szVirus);
   STRCAT (szMsg, SZ_SEPARATOR);
   STRCAT (szMsg, szFile);
   STRCAT (szMsg, SZ_SEPARATOR);
   STRCAT (szMsg, szUser);
   STRCAT (szMsg, SZ_SEPARATOR);
   dwLength = STRLEN (szMsg);

   dwSize = sizeof (SOCKADDR_IPX);
                                        // send the message to the server
   dwReturn = ( *lpsendto )(
                 s,                     // Socket handle
                 szMsg,                 // message to send
                 dwLength,              // Number of bytes in szMsg
                 0,                     // flags?? What for??
                 (PSOCKADDR) pAddress,  // address to send to
                 dwSize);               // sizeof (sockaddr)

   if (dwReturn == SOCKET_ERROR)
   {
                                        // Use WSAGetLastError instead of just
                                        // GetLastError for sock errors!
      dwReturn = (DWORD) ( *lpWSAGetLastError )();

                                        // don't continue if we had an error
      goto Cleanup;
   }
   else
   {
                                        // At this point, dwReturn should
                                        // equal dwSize (bytes sent == bytes
                                        // attempted to send).  We aren't going
                                        // to check this because we got SOMETHING
                                        // off.  If this is a problem, we should
                                        // fix this later.
      dwReturn = ERROR_SUCCESS;
   }

Cleanup:

   if (INVALID_SOCKET != s)
   {
      ( *lpclosesocket )(s);
      s = INVALID_SOCKET;
   }

   FreeWinsockLibrary();
     
   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// CNLMMessenger::NotifyNLMServer()
//
// This function sends the Virus Notification to the selected
// NetWare server.  We are taking a server name in this function
// and getting the name using GetAddressByName.  Once we have the
// address, we will call to the NotifyNLMServer version that takes
// the address.
//
// Parameters:
//   LPTSTR szVirus        - [in] Virus name that was found
//   LPTSTR szFile         - [in] File virus was found in
//   LPTSTR szUser         - [in] user that found the virus
//   LPTSTR szServer       - [in] server name to notify
//
// Return Value:
//    DWORD - ERROR_SUCCESS or the Error Code.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNLMMessenger::NotifyNLMServer(
   LPTSTR szVirus,                      // [in] Virus name that was found
   LPTSTR szFile,                       // [in] File virus was found in
   LPTSTR szUser,                       // [in] user that found the virus
   LPTSTR szServer)                     // [in] server name to notify
{
CSADDR_INFO rAddress[MAX_ADDRESSES];    // Server Addresses
DWORD       dwSize = sizeof (rAddress);
DWORD       dwAddressCount;
DWORD       dwReturn;
DWORD       dwRet;

   if ( InitializeWinsockLibrary() == NULL )
   {
      dwReturn = ERROR_INVALID_FUNCTION;
      goto Cleanup;
   }

   dwRet = ( *lpGetAddressByName )(
              NS_SAP,                   // Name Space
              &m_guid,                  // NLM GUID Identifier
              szServer,                 // Server Name
              m_anProtocols,            // Protocols
              RES_SOFT_SEARCH,          // Resolution
              NULL,                     // Reserved
              &rAddress,                // Buffer
              &dwSize,                  // Bufsize - Returns number of byte received
              NULL,                     // Alias Buffer
              NULL);                    // Alias buf size

                                        // GetAddressByName returns its error
                                        // codes using SetLastError.
                                        // Actual Socket  calls should use
                                        // WSAGetLastError instead

   FreeWinsockLibrary();
     
   if (SOCKET_ERROR == dwRet)
   {
      dwReturn = GetLastError();
                                        // Return the error to caller
      goto Cleanup;
   }

   for (dwAddressCount = 0;
         dwAddressCount < dwRet;
         dwAddressCount++)
   {

                                        // Now send us in to our normal function
      NotifyNLMServer(
         szVirus,
         szFile,
         szUser,
         (SOCKADDR_IPX *)(rAddress[dwAddressCount].RemoteAddr.lpSockaddr));
   }

Cleanup:

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// CNLMMessenger::NotifyOurServers()
//
// This function starts a thread that will go through our current list
// of servers and send a message to each and every one of them.
//
// Special code has been added here to take care of the situation where
// we are building the list as we are sending alerts.
//
// Parameters:
//   LPTSTR szVirus   - [in] Virus name that was found
//   LPTSTR szFile    - [in] File virus was found in
//   LPTSTR szUser    - [in] user that found the virus
//
// Return Value:
//    DWORD - Error Code.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
// 08/26/96 DSACKIN - Now it is NOT an error if we get here and there is
//                  - no list started.  If this is the case, we make a
//                  - request for a new list by incrementing m_dwRequestCount,
//                  - and the maintenence thread will see this and start a new one.
/////////////////////////////////////////////////////////////////////
DWORD CNLMMessenger::NotifyOurServers(
   LPTSTR szVirus,                      // [in] Virus name that was found
   LPTSTR szFile,                       // [in] File virus was found in
   LPTSTR szUser)                       // [in] user that found the virus
{
DWORD             dwReturn = ERROR_SUCCESS;
PSERVERSTRINGLIST pList;
PSERVERSTRINGNODE pPtr;

   if ( !m_bInitialized )
   {
                                        // Don't let them use this function
                                        // until they call Init() and
                                        // a list has been started.
      dwReturn = ERROR_INVALID_FUNCTION;
      goto Cleanup;
   }

   if (NULL == m_pCurrentList)
   {
                                        // Start by incrementing the list
                                        // requester variable.
      m_dwRequestCount++;

      while ( (NULL == m_pCurrentList) &&
              (!m_bExitingMessenger) )
      {
                                        // Sleep until it is ready!
         Sleep(LIST_SLEEP_INTERVAL);
      }
                                        // If we increment this variable, we MUST
                                        // be responsible to decrement is when done
      m_dwRequestCount--;
   }

                                        // Only one person can change list
                                        // Info at a given time.
   EnterCriticalSection(&m_hListCritical);

                                        // Get the current list.
   pList = m_pCurrentList;

                                        // Increment the usage count
   pList->dwUsageCount++;

                                        // Now let the next guy through
   LeaveCriticalSection(&m_hListCritical);

                                        // This is for the case where we are
                                        // just starting to add strings to
                                        // the list, but we don't have any yet.
   while ( (NULL == pList->pList) &&
           (!pList->bDoneAdding) &&
           (!m_bExitingMessenger) )
   {
      Sleep(LIST_SLEEP_INTERVAL);
   }

                                        // Start at the top of our list.
   pPtr = pList->pList;

   while (pPtr && !m_bExitingMessenger)
   {

      NotifyNLMServer (
         szVirus,
         szFile,
         szUser,
         &(pPtr->rIPXAddress));

      while ( (NULL == (pPtr->pNext)) &&
              (!pList->bDoneAdding) )
      {
         Sleep(LIST_SLEEP_INTERVAL);
      }

                                        // If pNext is NULL then we must be
                                        // done adding and we will fall out
                                        // of this main loop.
      pPtr = pPtr->pNext;
   }

                                        // Only one person can change list
                                        // Info at a given time.
   EnterCriticalSection(&m_hListCritical);

   pList->dwUsageCount--;

                                        // Now let the next guy through
   LeaveCriticalSection(&m_hListCritical);

                                        // Now that we are done sending
                                        // messages, delete our list.
   if ( m_pRemovingList == pList )
   {
                                        // This function will only delete
                                        // our list if usage count is 0
      DeleteRemovingList();
   }


Cleanup:

   return dwReturn;
}

/////////////////////////////////////////////////////////////////////
// NotifyOurServersProc()
//
// This function is a passthrough that takes (as an LPVOID) a pointer
// to a CStartNotifyInfo class.  This class contains the information that
// will be used when going through the server list and telling them
// all about the virus.
//
// We are responsible for freeing the object that comes in because we
// are NOT going to return to the caller.
//
// Parameters:
//   LPVOID - lpNotifyInfo - [in] contains szVirus, szFile, and szUser
//                         - this is actually a CStartNotifyInfo class.
//
// Return Value:
//    doesn't return.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID NotifyOurServersProc(
   LPVOID lpStartInfo)                  // [in] contains szVirus, szFile and szUser
{
PSTARTNOTIFYINFO pStartInfo;
CNLMMessenger    *pThis;

   pStartInfo = (PSTARTNOTIFYINFO) lpStartInfo;

   pThis = pStartInfo->pThis;

                                        // Now call our object!
   pThis->NotifyOurServers(
      pStartInfo->szVirus,
      pStartInfo->szFile,
      pStartInfo->szUser);

                                        // Now clean up our memory.
   MemFreePtr (lpStartInfo);
                                        // Don't return.  We were spawned.
   _endthread();
}

/////////////////////////////////////////////////////////////////////
// CNLMMessenger::NotifyAllNLMServers()
//
// This function starts a thread that will go through our current list
// of servers and send a message to each and every one of them.
//
// Special code has been added here to take care of the situation where
// we are building the list as we are sending alerts.
//
// Parameters:
//   LPTSTR szVirus   - [in] Virus name that was found
//   LPTSTR szFile    - [in] File virus was found in
//   LPTSTR szUser    - [in] user that found the virus
//
// Return Value:
//    BOOL - TRUE  - Thread started successfully.
//           FALSE - Unable to start the thread.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNLMMessenger::NotifyAllNLMServers(
   LPTSTR szVirus,                      // [in] Virus name that was found
   LPTSTR szFile,                       // [in] File virus was found in
   LPTSTR szUser)                       // [in] user that found the virus
{
PSTARTNOTIFYINFO pInfo;
HANDLE           hThread = INVALID_HANDLE_VALUE;
BOOL             bReturn = TRUE;

   if (!m_bInitialized)
   {
                                        // Don't let them use this function
                                        // until they call Init()!
      bReturn = FALSE;
      goto Cleanup;
   }


                                        // Check for bogus information
   if ( (NULL == szVirus) ||
        (NULL == szFile) ||
        (NULL == szUser) )
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   pInfo = (PSTARTNOTIFYINFO) MemAlloc (GHND, sizeof (STARTNOTIFYINFO));

   if (NULL == pInfo)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

   STRCPY (pInfo->szVirus, szVirus);
   STRCPY (pInfo->szFile,  szFile);
   STRCPY (pInfo->szUser,  szUser);

   pInfo->pThis = this;

                                        // Start a thread that will take
                                        // care of sending our alerts.
   hThread = (HANDLE)_beginthread (
                  NotifyOurServersProc, // start Addr
                  DEFAULT_STACK_SIZE,   // stack size
                  (LPVOID) pInfo);      // We don't use the user parameter.

   if ((HANDLE)INVALID_HANDLE_VALUE == hThread)
   {
                                        // If we fail, we clean up our memory
                                        // if not, the spawned thread does.
      MemFreePtr (pInfo);
      bReturn = FALSE;
      goto Cleanup;
   }

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Server List functions begin here!
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// CNLMMessenger : StartNewServerList
//
// This function moves the m_pCurrentList to the m_pRemovingList vairable
// only if we are not in the process of removing a list.
//
// Parameters:
//    none.
//
// Return Value:
//    BOOL - TRUE  - New list has been started.
//           FALSE - m_pRemovingList hasn't finished yet or we were
//                   unable to allocate new memory.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNLMMessenger::StartNewServerList(VOID)
{
BOOL              bReturn = TRUE;
PSERVERSTRINGLIST m_pNewList;

   if (!m_bInitialized)
   {
                                        // Don't let them use this function
                                        // until they call Init()!
      bReturn = FALSE;
      goto Cleanup;
   }


   if ( (NULL != m_pRemovingList) &&
        (!DeleteRemovingList()) )
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // This next case should NEVER happen,
                                        // but if it does, we will take care of it.
   if ( (NULL != m_pCurrentList) &&
        (!m_pCurrentList->bDoneAdding) )
   {
                                        // We aren't done adding to the
                                        // current list, but want to create a
                                        // new one??? Can't do!
      bReturn = FALSE;
      goto Cleanup;
   }


   m_pNewList = (PSERVERSTRINGLIST) MemAllocPtr(GHND, sizeof (SERVERSTRINGLIST));

   if (NULL == m_pNewList)
   {
      bReturn = FALSE;
      goto Cleanup;
   }


   m_pNewList->dwUsageCount = 0;        // Start usage count at 0
   m_pNewList->pList = NULL;            // Nothin' in the list yet.

                                        // Now switch them!

                                        // Only one person can change list
                                        // Info at a given time.
   EnterCriticalSection(&m_hListCritical);

   m_pRemovingList = m_pCurrentList;
   m_pCurrentList  = m_pNewList;

                                        // Now let the next guy through
   LeaveCriticalSection(&m_hListCritical);

Cleanup:

   return bReturn;
}

/////////////////////////////////////////////////////////////////////
// CNLMMessenger : AddServer
//
// This function adds a server to our list of servers.
// All additions will be added to m_pCurrentList.
//
// Parameters:
//   LPTSTR       szServerName [in] server to notify
//   SOCKADDR_IPX *pAddress    [in] address of our server
//
//
// Return Value:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNLMMessenger::AddServer(
   LPTSTR       szServerName,           // [in] server to notify
   SOCKADDR_IPX *pAddress)               // [in] address of our server
{
BOOL              bReturn = TRUE;
PSERVERSTRINGNODE pNode, pPtr;

                                        // Not allowed until we are initialized
   if (!m_bInitialized)
   {
      bReturn = FALSE;
      goto Cleanup;
   }
                                        // Allocate our new node
   pNode = (PSERVERSTRINGNODE) MemAllocPtr(
                                  GHND,
                                  sizeof (SERVERSTRINGNODE));

   if (NULL == pNode)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // Set up this record
   pNode->pNext = NULL;
   STRCPY (pNode->szServerName, szServerName);
   MEMCPY (&(pNode->rIPXAddress), pAddress, sizeof (SOCKADDR_IPX));

                                        // If NULL, make this our new list
   if (NULL == m_pCurrentList->pList)
   {
      m_pCurrentList->pList = pNode;
      goto Cleanup;
   }

                                        // Otherwise, add it to the end

                                        // Start by kickin' down to the end
   for (pPtr = m_pCurrentList->pList;
        pPtr->pNext != NULL;
        pPtr = pPtr->pNext);

                                        // Then add our node
   pPtr->pNext = pNode;

Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CNLMMessenger : RemoveAllServers
//
// This function goes through our list and frees all allocated memory.
//
// Parameters:
//    PSERVERSTRINGNODE pList - [in] - Pointer to the list to free
//
// Return Value:
//    BOOL - TRUE  - Success
//           FALSE - failure
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNLMMessenger::RemoveAllServers(
   PSERVERSTRINGNODE pList)             // [in] - Pointer to the list to free
{
BOOL              bReturn = TRUE;
PSERVERSTRINGNODE pPtr;

                                        // Not allowed until we are initialized
   if (!m_bInitialized)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // If list is empty, return.
   if (NULL == pList)
   {
      goto Cleanup;
   }

   while (pList)
   {
                                        // Take a reference
      pPtr = pList;

                                        // Move to the next
      pList = pList->pNext;

                                        // Free this node.
      MemFreePtr (pPtr);
   }

                                        // At this point, m_pServerList
                                        // will already be set to NULL and
                                        // we are ready to exit.
Cleanup:

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CNLMMessenger : DeleteRemovingList
//
// This function checks the usage count on the m_pRemovingList guy.
// If it is at 0 and the list is still there, we take care of deleting
// the list and setting it to NULL.
//
// Parameters:
//    none.
//
// Return Value:
//    BOOL - TRUE  - List was deleted
//           FALSE - Someone was still using it
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CNLMMessenger::DeleteRemovingList(VOID)
{
PSERVERSTRINGLIST pTemp;

                                        // NO list is what we want.
   if (NULL == m_pRemovingList)
      return TRUE;

                                        // If the usage count is up,
                                        // we must assume that the guy in
                                        // the list will delete the list
                                        // when he is done because his
                                        // pList == m_pRemovingList now.
   if (0 != m_pRemovingList->dwUsageCount)
      return FALSE;

   pTemp = m_pRemovingList;

                                        // Only one person can change list
                                        // Info at a given time.
   EnterCriticalSection(&m_hListCritical);

                                        // First, set the RemovingList to
                                        // NULL so anyone waiting on it can
                                        // go on their merry way.. We can
                                        // clean the rest up from here.
   m_pRemovingList = NULL;

                                        // Now let the next guy through
   LeaveCriticalSection(&m_hListCritical);


                                        // Remove the servers
   RemoveAllServers( pTemp->pList );

                                        // Free the memory.
   MemFreePtr (pTemp);

   return TRUE;
}



/////////////////////////////////////////////////////////////////////
// BOOL CNLMMessenger::NLMServerEnum
//
// Purpose:
//    This is the callback from the enumeration function.
//
// Parameters:
//   LPSTR  lpszServerName - [in] - Server to be added
//   LPVOID lpAddress      - [in] - Optional address of server.
//
// Returns:
//    BOOL - TRUE  - Continue enumerating
//         - FALSE - STOP enumerating
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12-12-96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL CNLMMessenger::NLMServerEnum (
   LPSTR  lpszServerName,               // [in] - Name of server to add
   LPVOID lpAddress)                    // [in] - Address of server
{
                                        // Add server to our list.
   AddServer (
      lpszServerName,
      (SOCKADDR_IPX *)(lpAddress));

                                        // For this functionality, we don't
                                        // ever want to stop the enumeration.
   return (!m_bExitingMessenger);
}


/////////////////////////////////////////////////////////////////////
// BOOL NLMServerEnumProc (
//
// Purpose:
//    This is the callback from the enumeration function.
//
// Parameters:
//   LPSTR  lpszServerName - [in] - Server to be added
//   LPVOID lpAddress      - [in] - Optional address of server.
//   LPVOID lpExtra        - [in] - extra data (passthrough)
//
// Returns:
//    BOOL - TRUE  - Continue enumerating
//         - FALSE - STOP enumerating
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 12-12-96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
BOOL NLMServerEnumProc (
   LPSTR  lpszServerName,
   LPVOID lpAddress,
   LPVOID lpExtra)
{
CNLMMessenger *pThis;
BOOL          bReturn;

                                        // Take our this pointer from the
                                        // lpExtra variable
   pThis = (CNLMMessenger *) lpExtra;

                                        // Pass the info through to our class.
   bReturn = pThis->NLMServerEnum(
               lpszServerName,
               lpAddress);

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CNLMMessenger : BuildServerList
//
// This function builds our server list.
//
// Parameters:
//    none.
//
// Return Value:
//    BOOL - TRUE  - Success
//           FALSE - failure
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
#define MAX_SERVER_RECS   20             // Take 20 servers per query
BOOL CNLMMessenger::BuildNewServerList(VOID)
{
BOOL   bReturn = TRUE;

                                        // Not allowed until we are initialized
   if (!m_bInitialized)
   {
      bReturn = FALSE;
      goto Cleanup;
   }

                                        // All of our adding of servers is
                                        // going to happen in the callback
                                        // function that enumerates them for us.
   EnumNAVNLMServers(
      NLMServerEnumProc,                // Callback function
      TRUE,                             // We want the addresses
      (LPNETRESOURCE) NULL,             // Must always start at NULL!
      (LPVOID) this);                   // address of our current object.


Cleanup:

                                        // Only one person can change list
                                        // Info at a given time.
   EnterCriticalSection(&m_hListCritical);

                                        // At this point, we MUST be done
                                        // adding.  Adding only happens on
                                        // m_pCurrentList
   m_pCurrentList->bDoneAdding = TRUE;

                                        // Now let the next guy through
   LeaveCriticalSection(&m_hListCritical);


   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// MaintainServerList
//
// This function is responsible for rebuilding the server list every
// 30 minutes.
//
// We don't check the m_bInitialized because starting this thread is
// a part of the initialization process.
//
// Parameters:
//    none.
//
// Return Value:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
// 08/26/96 DSACKIN - Don't begin building the list until someone requests it.
//                  - The hour wait will be the same between building the
//                  - lists, but we won't start the new one unless someone
//                  - wants us to.
/////////////////////////////////////////////////////////////////////
VOID CNLMMessenger::MaintainServerList(VOID)
{
BOOL  bSuccess = TRUE;
DWORD dwTimeLeft, dwSleepTime;

   while (!m_bExitingMessenger && bSuccess)
   {
                                        // Before building the new server
                                        // list, we must successfully
                                        // create one!

                                        // 08/26/96 - Don't begin building
                                        // the list until someone requests it!
      while ( !m_bExitingMessenger &&
              (0 == m_dwRequestCount) )
      {
         Sleep (LIST_SLEEP_INTERVAL);
      }

                                        // First time through, we may not be
                                        // fully initialized yet.  If this is
                                        // the case, the sleep will allow
                                        // time for this to happen.
      while ( !m_bExitingMessenger &&
              !StartNewServerList() )
      {
         Sleep (LIST_SLEEP_INTERVAL);
      }

                                        // Attempt to build our new list
      if ( !m_bExitingMessenger &&
           BuildNewServerList())
      {
                                        // If we succeeded in building the
                                        // last list, we need to wait our
                                        // time out until we should build
                                        // another list.

                                        // Set up our timer
         dwTimeLeft = LIST_UPDATE_INTERVAL;

         while ( !m_bExitingMessenger &&
                 (dwTimeLeft > 0) )
         {
            dwSleepTime = (LIST_WAIT_INTERVAL < dwTimeLeft) ?
                           LIST_WAIT_INTERVAL :
                           dwTimeLeft;

            Sleep (dwSleepTime);

            dwTimeLeft -= dwSleepTime;
         }
      }
      else
      {
         bSuccess = FALSE;
      }
   }

                                        // At this point, we know we are
                                        // leaving for some reason

}


/////////////////////////////////////////////////////////////////////
// MaintainServerListProc
//
// This is a passthrough function to get to the MaintainServerList
// Member variable.  This should be called with _beginthread
//
// Parameters:
//    LPVOID lpMessenger - Our messenger object that we will be
//                         transferring control to.
//
// Return Value:
//    none.
//
/////////////////////////////////////////////////////////////////////
// 08/14/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID MaintainServerListProc(
   LPVOID lpMessenger)
{
                                        // Sending us a NULL pointer is
                                        // invalid and we must not use it!
   if (NULL != lpMessenger)
   {
      ((PCNLMMessenger) lpMessenger)->MaintainServerList();
   }

                                        // Don't return.  We were spawned.
   _endthread();
}


