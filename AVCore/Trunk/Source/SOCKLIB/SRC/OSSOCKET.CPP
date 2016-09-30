// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SOCKLIB/VCS/OSSOCKET.CPv   1.1   23 Mar 1998 11:27:00   mlele  $
//
// Description:
//  Source file for socket implementation.
//
// Contains:
//  SocketAddressFill()
//  SocketAddressGetIP()
//  SocketAddressCmpIP()
//  SocketCreate()
//  SocketCreateForListen()
//  SocketSetNonBlockingMode()
//  SocketListen()
//  SocketAccept()
//  SocketConnect()
//  SocketSelectWriteable()
//  SocketSelectReadable()
//  SocketDestroy()
//  SocketReceive()
//  SocketSend()
//  SocketReadLine()
//  SocketReadLineB()
//  SocketWriteLine()
//  SocketSendDWORD()
//  SocketSendWORD()
//  SocketReceiveDWORD()
//  SocketReceiveWORD()
//  SocketReceiveBuffer()
//
// See Also:
//
//************************************************************************
// $Log:   S:/SOCKLIB/VCS/OSSOCKET.CPv  $
// 
//    Rev 1.1   23 Mar 1998 11:27:00   mlele
// Modified function SocketReceiveBuffer() -- Removed unused variable
// nBytesRead. The actual one used is: lpnBytesRead.
// 
//    Rev 1.0   25 Apr 1997 11:51:44   ksackin
// Initial revision.
// 
//    Rev 1.13   25 Mar 1997 18:31:56   DJesch
// Added a SocketReceiveBuffer() function.
//
//    Rev 1.12   04 Feb 1997 16:25:08   RAY
// Fixed ReadLineB to recognize closed sockets
//
//    Rev 1.11   31 Jan 1997 22:51:54   DJesch
// When returning an error status from CreateSocket(), now freeing the memory
// block for the socket and setting the returned handle to HINVALID_SOCKET.
//
//    Rev 1.9   23 Dec 1996 14:02:54   RAY
// Implemented buffered socket i/o.  Hope it works!
//
//    Rev 1.8   05 Nov 1996 14:51:10   RAY
// SocketAddressFill() now uses INADDR_ANY when passed a NULL or empty
// lpszInternetAddress.
//
//    Rev 1.7   05 Nov 1996 13:22:20   RAY
// Added SocketSelectReadable().
//
//    Rev 1.6   31 Oct 1996 11:36:32   CNACHEN
// Added SocketWriteLine
//
//    Rev 1.5   24 Oct 1996 18:24:38   DCHI
// Added SocketSelectWriteable() function.
//
//    Rev 1.4   08 Oct 1996 17:21:52   DCHI
// Added SocketReadLineB() function that does not strip the CR/LF.
//
//    Rev 1.3   04 Oct 1996 12:14:02   DCHI
// Added SetNonBlockingMode() to header comments.
//
//    Rev 1.2   04 Oct 1996 12:12:50   DCHI
// Added SetNonBlockingMode function.
//
//    Rev 1.1   24 Sep 1996 19:50:38   DJesch
// Updated version from DCHI
//
//    Rev 1.1   23 Sep 1996 15:08:48   DCHI
// Added SocketAddressCmpIP().
//
//    Rev 1.0   20 Sep 1996 18:20:42   DCHI
// Initial revision.
//
//************************************************************************

#include "ossocket.h"
#include "stdlib.h"
#include "memory.h"

//*************************************************************************
//
// SOCKET_STATUS SocketAddressFill()
//
// Parameters:
//  lpstSocketAddress       Ptr to a SOCKET_ADDRESS_T structure
//  lpszInternetAddress     Either an internet name or dot separated
//                          IP address
//  wPort                   The port
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Performs a DNS lookup if the address is a name.
//
//  The resulting IP address combined with the port is used to create
//  a SOCKET_ADDRESS_T structure for connections.
//
//  This function does not need a corresponding destroy.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketAddressFill
(
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPSTR               lpszInternetAddress,
    WORD                wPort,
    LPDWORD             lpdwError
)
{
    if( NULL == lpszInternetAddress || 0 == strlen( lpszInternetAddress ))
    {
        lpstSocketAddress->stSockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
        lpstSocketAddress->stSockAddr.sin_family  = AF_INET;
    }

    else
    {
        struct hostent *    pHostEntry;     // pointer to host entry
        int                 i;
        BOOL                bIsINETAddr;

        // Is it an INET address?

        bIsINETAddr = TRUE;

        for (i=strlen(lpszInternetAddress)-1;i>=0;i--)
        {
            if (lpszInternetAddress[i] != '.' &&
                isdigit(lpszInternetAddress[i]) == 0)
            {
                bIsINETAddr = FALSE;
                break;
            }
        }

        if (bIsINETAddr == FALSE)
        {
            // Map the server name to an address

            pHostEntry = gethostbyname(lpszInternetAddress);
            if (pHostEntry == NULL)
            {
                *lpdwError = (DWORD)WSAGetLastError();
                return(SOCKET_STATUS_ERROR);
            }

            memcpy(&(lpstSocketAddress->stSockAddr.sin_addr),
                   pHostEntry->h_addr_list[0],
                   pHostEntry->h_length);

            lpstSocketAddress->stSockAddr.sin_family = pHostEntry->h_addrtype;
        }
        else
        {
            unsigned long   ulINETAddr;

            // It must be an IP address string

            ulINETAddr = inet_addr(lpszInternetAddress);
            if (ulINETAddr == INADDR_NONE)
            {
                *lpdwError = (DWORD)WSAGetLastError();
                return(SOCKET_STATUS_ERROR);
            }

            lpstSocketAddress->stSockAddr.sin_addr.S_un.S_addr = ulINETAddr;
            lpstSocketAddress->stSockAddr.sin_family  = AF_INET;
        }
    }

    lpstSocketAddress->stSockAddr.sin_port = htons(wPort);

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketAddressGetIP()
//
// Parameters:
//  lpstSocketAddress       Ptr to a SOCKET_ADDRESS_T structure
//  lpbyIP                  Ptr to four byte array to receive IP
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Stores the four components of the IP address at the given four bytes.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketAddressGetIP
(
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPBYTE              lpbyIP,
    LPDWORD             lpdwError
)
{
    lpbyIP[0] = lpstSocketAddress->stSockAddr.sin_addr.S_un.S_un_b.s_b1;
    lpbyIP[1] = lpstSocketAddress->stSockAddr.sin_addr.S_un.S_un_b.s_b2;
    lpbyIP[2] = lpstSocketAddress->stSockAddr.sin_addr.S_un.S_un_b.s_b3;
    lpbyIP[3] = lpstSocketAddress->stSockAddr.sin_addr.S_un.S_un_b.s_b4;

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketAddressCmpIP()
//
// Parameters:
//  lpstSocketAddress0      Ptr to a SOCKET_ADDRESS_T structure
//  lpstSocketAddress1      Ptr to a SOCKET_ADDRESS_T structure
//  lpbEqual                Ptr to BOOL to hold equality status of IPs
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Compares the IP portions of the two addresses and returns TRUE
//  in *lpbEqual if they are equal, and FALSE otherwise.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketAddressCmpIP
(
    LPSOCKET_ADDRESS    lpstSocketAddress0,
    LPSOCKET_ADDRESS    lpstSocketAddress1,
    LPBOOL              lpbEqual,
    LPDWORD             lpdwError
)
{
    if ((lpstSocketAddress0->stSockAddr.sin_addr.S_un.S_un_b.s_b1 ==
         lpstSocketAddress1->stSockAddr.sin_addr.S_un.S_un_b.s_b1) &&
        (lpstSocketAddress0->stSockAddr.sin_addr.S_un.S_un_b.s_b2 ==
         lpstSocketAddress1->stSockAddr.sin_addr.S_un.S_un_b.s_b2) &&
        (lpstSocketAddress0->stSockAddr.sin_addr.S_un.S_un_b.s_b3 ==
         lpstSocketAddress1->stSockAddr.sin_addr.S_un.S_un_b.s_b3) &&
        (lpstSocketAddress0->stSockAddr.sin_addr.S_un.S_un_b.s_b4 ==
         lpstSocketAddress1->stSockAddr.sin_addr.S_un.S_un_b.s_b4))
    {
        *lpbEqual = TRUE;
    }
    else
    {
        *lpbEqual = FALSE;
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketCreate()
//
// Parameters:
//  lphSocket               A ptr to a socket handle
//  bNonBlocking            TRUE if socket is to be non-blocking
//  bLinger                 TRUE if the socket should linger on close
//  nLingerTimeSec          Linger time in seconds
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Creates an unbound socket.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketCreate
(
    LPHSOCKET           lphSocket,
    BOOL                bNonBlocking,
    BOOL                bLinger,
    int                 nLingerTimeSec,
    LPDWORD             lpdwError
)
{
    u_long              ulNonBlock;
    struct linger       stLinger;
    LPOSSOCKET          lprSocket;

    *lphSocket = (HSOCKET) malloc (sOSSOCKET);

    if (NULL == *lphSocket)
    {
        *lpdwError = SOCKET_ERROR_NOBUFS;
        *lphSocket = HINVALID_SOCKET;
        return(SOCKET_STATUS_ERROR);
    }

    lprSocket = (LPOSSOCKET) *lphSocket;

    lprSocket->rSocket = socket (PF_INET,SOCK_STREAM,0);

    if (lprSocket->rSocket == INVALID_SOCKET)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        free(lprSocket);
        *lphSocket = HINVALID_SOCKET;
        return(SOCKET_STATUS_ERROR);
    }

    lprSocket->wRecvBufferOffset = 0;
    lprSocket->wRecvBufferCount = 0;

    // Set non-blocking status

    if (bNonBlocking == TRUE)
        ulNonBlock = 1;
    else
        ulNonBlock = 0;

    if (ioctlsocket(lprSocket->rSocket,FIONBIO,&ulNonBlock) == SOCKET_ERROR)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        closesocket(lprSocket->rSocket);
        free(lprSocket);
        *lphSocket = HINVALID_SOCKET;
        return(SOCKET_STATUS_ERROR);
    }

    // Set linger status

    if (bLinger == TRUE)
    {
        stLinger.l_onoff = 1;
        stLinger.l_linger = nLingerTimeSec;
    }
    else
    {
        stLinger.l_onoff = 0;
        stLinger.l_linger = 0;
    }

    if (setsockopt(lprSocket->rSocket,
                   SOL_SOCKET,
                   SO_LINGER,
                   (char FAR *)&stLinger,
                   sizeof(stLinger)) == SOCKET_ERROR)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        closesocket(lprSocket->rSocket);
        free(lprSocket);
        *lphSocket = HINVALID_SOCKET;
        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketCreateForListen()
//
// Parameters:
//  lphSocket               A ptr to a socket handle
//  bNonBlocking            TRUE if socket is to be non-blocking
//  bLinger                 TRUE if the socket should linger on close
//  nLingerTimeSec          Linger time in seconds
//  lpszInternetAddress     The IP address in string form
//  lpwPort                 Set to zero if one should be assigned,
//                              otherwise set to desired port
//  lpstSocketAddress       Ptr to structure to store resulting
//                              socket address
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Creates an unbound socket and binds it to the IP address and port
//  given.
//
//  If *lpwPort is passed in with a value of zero and the function
//  completes successfully, *lpwPort will return with the assigned
//  port.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketCreateForListen
(
    LPHSOCKET           lphSocket,
    BOOL                bNonBlocking,
    BOOL                bLinger,
    int                 nLingerTimeSec,
    LPSTR               lpszInternetAddress,
    LPWORD              lpwPort,
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPDWORD             lpdwError
)
{
    int                 nNameLen;
    LPOSSOCKET          lprSocket;

    // Create a socket

    if (SocketCreate(lphSocket,
                     bNonBlocking,
                     bLinger,
                     nLingerTimeSec,
                     lpdwError) != SOCKET_STATUS_OK)
    {
        return(SOCKET_STATUS_ERROR);
    }

    lprSocket = (LPOSSOCKET) *lphSocket;

    // Create an address

    if (SocketAddressFill(lpstSocketAddress,
                          lpszInternetAddress,
                          *lpwPort,
                          lpdwError) != SOCKET_STATUS_OK)
    {
        SocketDestroy(lprSocket,lpdwError);
        return(SOCKET_STATUS_ERROR);
    }

    lpstSocketAddress->stSockAddr.sin_family = AF_INET;
    lpstSocketAddress->stSockAddr.sin_port = htons(*lpwPort);
    if (bind(lprSocket->rSocket,
             (const struct sockaddr FAR *)&lpstSocketAddress->stSockAddr,
             sizeof(lpstSocketAddress->stSockAddr)) == SOCKET_ERROR)
    {
        DWORD dwError;
        *lpdwError = (DWORD)WSAGetLastError();

        // Don't return an error from SocketDestroy()
        SocketDestroy(lprSocket, &dwError);
        return(SOCKET_STATUS_ERROR);
    }

    // Get the socket address

    nNameLen = sizeof(lpstSocketAddress->stSockAddr);

    if (getsockname(lprSocket->rSocket,
                    (struct sockaddr FAR *)&lpstSocketAddress->stSockAddr,
                    &nNameLen) == SOCKET_ERROR)
    {
        DWORD dwError;
        *lpdwError = (DWORD)WSAGetLastError();

        // Don't return an error from SocketDestroy()
        SocketDestroy(lprSocket, &dwError);
        return(SOCKET_STATUS_ERROR);
    }

    *lpwPort = ntohs(lpstSocketAddress->stSockAddr.sin_port);

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketSetNonBlockingMode()
//
// Parameters:
//  hSocket                 A bound socket handle
//  bNonBlocking            Non-blocking mode enable/disable
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Disables non-blocking mode if bNonBlocking is FALSE.  Otherwise,
//  enables non-blocking mode.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketSetNonBlockingMode
(
    HSOCKET             hSocket,
    BOOL                bNonBlocking,
    LPDWORD             lpdwError
)
{
    unsigned long       ulNonBlocking;
    LPOSSOCKET          lprSocket = (LPOSSOCKET) hSocket;

    if (bNonBlocking == FALSE)
    {
        // Disable non-blocking mode

        ulNonBlocking = 0;
    }
    else
    {
        // Enable non-blocking mode

        ulNonBlocking = 1;
    }

    if (ioctlsocket(lprSocket->rSocket,FIONBIO,&ulNonBlocking) == SOCKET_ERROR)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketListen()
//
// Parameters:
//  hSocket                 A bound socket handle
//  nBacklog                Backlog queue size
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  To accept connections, a socket is first created with
//  SocketCreateForListen(), a backlog for incoming connections is
//  specified with SocketListen(), and then the connections are accepted
//  with SocketAccept().  The socket is put into "passive" mode where
//  incoming connections are acknowledged and queued pending acceptance
//  by the process.
//
//  This function is typically used by servers that could have more than
//  one connection request at a time: if a connection request arrives
//  with the queue full, the client will receive an error with an
//  indication of SOCKET_ERROR_CONNREFUSED.
//
//  SocketListen() attempts to continue to function rationally when
//  there are no available descriptors.  It will accept connections
//  until the queue is emptied.  If descriptors become available, a
//  later call to SocketListen() or SocketAccept() will re-fill the
//  queue to the current or most recent "backlog", if possible, and
//  resume listening for incoming connections.
//
//  Compatibility: backlog is currently limited (silently) to 5. As in
//  4.3BSD, illegal values (less than 1 or greater than 5) are replaced
//  by the nearest legal value.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketListen
(
    HSOCKET             hSocket,
    int                 nBacklog,
    LPDWORD             lpdwError
)
{
    LPOSSOCKET  lprSocket = (LPOSSOCKET) hSocket;
    if (listen(lprSocket->rSocket,nBacklog) == SOCKET_ERROR)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketAccept()
//
// Parameters:
//  hSocket                 A bound socket handle
//  lpSocketAddress         Ptr to SOCKET_ADDRESS_T to store address
//                          of socket for which connection is accepted
//  lphNewSocket            Ptr to a socket handle to store handle
//                          of new socket
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This routine extracts the first connection on the queue of pending
//  connections on hSocket, creates a new socket with the same properties
//  as hSocket and returns a handle to the new socket.  If no pending
//  connections are present on the queue, and the socket is not marked
//  as non-blocking, SocketAccept() blocks the caller until a connection
//  is present.  If the socket is marked non-blocking and no pending
//  connections are present on the queue, SocketAccept() returns an
//  error.  The accepted socket may not be used to accept more
//  connections.  The original socket remains open.
//
//  On error, *lphNewSocket is set to HINVALID_SOCKET.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketAccept
(
    HSOCKET             hSocket,
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPHSOCKET           lphNewSocket,
    LPDWORD             lpdwError
)
{
    LPOSSOCKET lprSocket = (LPOSSOCKET) hSocket;
    LPOSSOCKET lprNewSocket;

    int nAddrLen = sizeof(lpstSocketAddress->stSockAddr);

    *lphNewSocket = (HSOCKET) malloc (sOSSOCKET);

    if (NULL == *lphNewSocket)
    {
        *lpdwError = SOCKET_ERROR_NOBUFS;
        return(SOCKET_STATUS_ERROR);
    }

    lprNewSocket = (LPOSSOCKET) *lphNewSocket;

    lprNewSocket->wRecvBufferOffset = 0;
    lprNewSocket->wRecvBufferCount = 0;

    lprNewSocket->rSocket =
                accept( lprSocket->rSocket,
                        (struct sockaddr *)&lpstSocketAddress->stSockAddr,
                        &nAddrLen);

    if( lprNewSocket->rSocket == INVALID_SOCKET)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        free( lprNewSocket );
        *lphNewSocket = HINVALID_SOCKET;
        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketConnect()
//
// Parameters:
//  hSocket                 A socket handle
//  lpstSocketAddress       The socket address to connect to
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Connects a socket to an address.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketConnect
(
    HSOCKET             hSocket,
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPDWORD             lpdwError
)
{
    LPOSSOCKET lprSocket = (LPOSSOCKET) hSocket;

    if (connect(lprSocket->rSocket,
                (struct sockaddr FAR *)&lpstSocketAddress->stSockAddr,
                sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketSelectWriteable()
//
// Parameters:
//  hSocket                 A socket handle
//  dwTimeLimitMS           Time to wait in milliseconds
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  The function checks the given socket for writeability.  If the
//  socket is connecting (nonblocking), writability means that the
//  connection establishment successfully completed. If the socket
//  is not in the process of connecting, writability means that a
//  send or sendto will complete without blocking. [It is not specified
//  how long this guarantee can be assumed to be valid, particularly
//  in a multithreaded environment.]
//
//  The parameter dwTimeLimitMS controls how long the select may take
//  to complete.  The dwTimeLimitMS parameter specifies the maximum time
//  that select should wait before returning.  If its value is 0, the
//  select will return immediately; this is used to "poll" the state
//  of the socket.  If this is the case, then the select call is
//  considered nonblocking and the standard assumptions for nonblocking
//  calls apply.  For example, the blocking hook must not be called,
//  and the Windows Sockets implementation must not yield.
//
//  On timeout, the function returns SOCKET_STATUS_ERROR and sets
//  *lpdwError = SOCKET_ERROR_TIMEDOUT.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketSelectWriteable
(
    HSOCKET             hSocket,
    DWORD               dwTimeLimitMS,
    LPDWORD             lpdwError
)
{
    int                 nReturnValue;
    fd_set              stFDset;
    struct timeval      stTimeVal;
    LPOSSOCKET          lprSocket = (LPOSSOCKET) hSocket;

    // Add socket to FD set

    FD_ZERO(&stFDset);
    FD_SET(lprSocket->rSocket,&stFDset);

    // Set time limit

    stTimeVal.tv_sec = dwTimeLimitMS / 1000;
    stTimeVal.tv_usec = (dwTimeLimitMS % 1000) * 1000;

    // Perform select

    nReturnValue = select(1,NULL,&stFDset,NULL,&stTimeVal);

    if (nReturnValue == 1)
    {
        // Successful

        return(SOCKET_STATUS_OK);
    }

    if (nReturnValue == 0)
    {
        // Call timed out

        *lpdwError = SOCKET_ERROR_TIMEDOUT;
    }
    else
    {
        *lpdwError = (DWORD)WSAGetLastError();
    }

    return(SOCKET_STATUS_ERROR);
}


//*************************************************************************
//
// SOCKET_STATUS SocketSelectReadable()
//
// Parameters:
//  hSocket                 A socket handle
//  dwTimeLimitMS           Time to wait in milliseconds
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  The function checks the given socket for readability.  WinSock
//  documentation describes readability as follows:
//
//      If the socket is currently listening, it will be marked as readable
//      if an incoming connection request has been received, so that an accept
//      is guaranteed to complete without blocking.
//
//      For other sockets, readability means that queued data is available
//      for reading or, for sockets of type SOCK_STREAM, that the virtual
//      socket corresponding to the socket has been closed, so that a recv or
//      recvfrom is guaranteed to complete without blocking.
//
//  The parameter dwTimeLimitMS controls how long the select may take
//  to complete.  The dwTimeLimitMS parameter specifies the maximum time
//  that select should wait before returning.  If its value is 0, the
//  select will return immediately; this is used to "poll" the state
//  of the socket.  If this is the case, then the select call is
//  considered nonblocking and the standard assumptions for nonblocking
//  calls apply.  For example, the blocking hook must not be called,
//  and the Windows Sockets implementation must not yield.
//
//  On timeout, the function returns SOCKET_STATUS_ERROR and sets
//  *lpdwError = SOCKET_ERROR_TIMEDOUT.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketSelectReadable
(
    HSOCKET             hSocket,
    DWORD               dwTimeLimitMS,
    LPDWORD             lpdwError
)
{
    int                 nReturnValue;
    fd_set              stFDset;
    struct timeval      stTimeVal;
    LPOSSOCKET          lprSocket = (LPOSSOCKET) hSocket;

    // Add socket to FD set

    FD_ZERO(&stFDset);
    FD_SET(lprSocket->rSocket,&stFDset);

    // Set time limit

    stTimeVal.tv_sec = dwTimeLimitMS / 1000;
    stTimeVal.tv_usec = (dwTimeLimitMS % 1000) * 1000;

    // Perform select

    nReturnValue = select(1,&stFDset,NULL,NULL,&stTimeVal);

    if (nReturnValue == 1)
    {
        // Successful

        return(SOCKET_STATUS_OK);
    }

    if (nReturnValue == 0)
    {
        // Call timed out

        *lpdwError = SOCKET_ERROR_TIMEDOUT;
    }
    else
    {
        *lpdwError = (DWORD)WSAGetLastError();
    }

    return(SOCKET_STATUS_ERROR);
}


//*************************************************************************
//
// SOCKET_STATUS SocketDestroy()
//
// Parameters:
//  hSocket                 A socket handle
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Frees resources allocated for a socket.
//
//  The semantics of this function are affected by the linger socket
//  options as follows:
//
//      Linger Option   Interval    Type of close   Wait for close?
//      FALSE (OFF)     Don't care  Graceful        No
//      TRUE (ON)       Zero        Hard            No
//      TRUE (ON)       Non-zero    Graceful        Yes
//
//  If linger is on with a zero timeout interval, the function is not
//  blocked even if queued data has not yet been sent or acknowledged.
//  This is called a "hard" or "abortive" close, because the socket's
//  virtual circuit is reset immediately, and any unsent data is lost.
//  Any receive call on the remote side of the circuit will fail with
//  SOCKET_ERROR_CONNRESET.
//
//  If linger is on with a non-zero timeout interval, the call blocks
//  until the remaining data has been sent or until the timeout expires.
//  This is called a graceful disconnect.  Note that if the socket is
//  set to non-blocking and linger is on with a non-zero timeout, the
//  call fails with an error of SOCKET_ERROR_WOULDBLOCK.
//
//  If linger if off the call will return immediately.  However, any data
//  queued for transmission will be sent if possible before the underlying
//  socket is closed. This is also called a graceful disconnect. Note that
//  in this case the sockets implementation may not release the socket and
//  other resources for an arbitrary period, which may affect applications
//  which expect to use all available sockets.
//
//  hSocket can equal to HINVALID_SOCKET.  The function will not
//  attempt to close the socket if this is so, but will return
//  SOCKET_STATUS_OK.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketDestroy
(
    HSOCKET             hSocket,
    LPDWORD             lpdwError
)
{
    LPOSSOCKET lprSocket = (LPOSSOCKET) hSocket;
    SOCKET_STATUS ssResult = SOCKET_STATUS_OK;

    // For HINVALID_SOCKET, the function returns OK
    if (lprSocket != HINVALID_SOCKET)
    {
        if (closesocket(lprSocket->rSocket) == SOCKET_ERROR)
        {
            *lpdwError = (DWORD)WSAGetLastError();
            ssResult = SOCKET_STATUS_ERROR;
        }
        free( lprSocket );
    }
    return ssResult;
}


//*************************************************************************
//
// SOCKET_STATUS SocketReceive()
//
// Parameters:
//  hSocket                 A socket handle
//  lpvBuf                  Buffer for receiving data
//  nBufLenBytes            Size of buffer in bytes
//  lpnBytesReceived        Ptr to int to receive # bytes received
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  As much information as is currently available up to the size
//  of the buffer supplied is returned.
//
//  If no incoming data is available at the socket, the call waits
//  for data to arrive unless the socket is non-blocking.  In this case
//  a value of SOCKET_STATUS_ERROR is returned with the error code set
//  to SOCKET_ERROR_WOULDBLOCK.
//
//  If the remote side has shut down the connection gracefully, this call
//  will complete immediately with 0 bytes received.  If the connection
//  has been reset, the call will fail with the error
//  SOCKET_ERROR_CONNRESET.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketReceive
(
    HSOCKET             hSocket,
    LPVOID              lpvBuf,
    int                 nBufLenBytes,
    LPINT               lpnBytesReceived,
    LPDWORD             lpdwError
)
{
    WORD wBytesAvailable;
    LPOSSOCKET lprSocket = (LPOSSOCKET) hSocket;

    // start with zero bytes read
    *lpnBytesReceived = 0;

    // if there is anything in the buffer already
    if( lprSocket->wRecvBufferCount > 0 )
    {
        if( nBufLenBytes <= lprSocket->wRecvBufferCount )
        {
            // there's enough in the buffer to complete this request
            memcpy( lpvBuf,
                    lprSocket->abyRecvBuffer + lprSocket->wRecvBufferOffset,
                    nBufLenBytes );

            // reset buffer stats
            lprSocket->wRecvBufferCount -= nBufLenBytes;
            lprSocket->wRecvBufferOffset += nBufLenBytes;

            *lpnBytesReceived = nBufLenBytes;
            return SOCKET_STATUS_OK;
        }

        else
        {
            DWORD   dwError;

            // start with what's in the buffer
            memcpy( lpvBuf,
                    lprSocket->abyRecvBuffer + lprSocket->wRecvBufferOffset,
                    lprSocket->wRecvBufferCount );

            // set these now in case we don't receive any more below
            *lpnBytesReceived = lprSocket->wRecvBufferCount;

            lprSocket->wRecvBufferCount = 0;
            lprSocket->wRecvBufferOffset = 0;

            // should we return now or should we recv() some more?
            if( SOCKET_STATUS_OK != SocketSelectReadable( lprSocket,
                                                          0,
                                                          &dwError ))
            {
                // There are no more bytes available on the socket so return
                // with a partially filled buffer (which is legal for winsock)

                return SOCKET_STATUS_OK;
            }

            // we only have this much space left to fill
            nBufLenBytes -= *lpnBytesReceived;
        }
    }

    // at this point, the buffer is empty ... try filling it up
    lprSocket->wRecvBufferOffset = 0;
    lprSocket->wRecvBufferCount = recv( lprSocket->rSocket,
                                        (char FAR *) lprSocket->abyRecvBuffer,
                                        OSSOCKET_BUFFER_SIZE,
                                        0 );

    if( lprSocket->wRecvBufferCount == ((WORD) SOCKET_ERROR ))
    {
        // uh-oh!
        lprSocket->wRecvBufferCount = 0;
        *lpdwError = (DWORD) WSAGetLastError();
        return SOCKET_STATUS_ERROR;
    }

    // how many (more) bytes can we return?
    wBytesAvailable = __min( lprSocket->wRecvBufferCount, nBufLenBytes );

    memcpy( (LPVOID) ( (LPBYTE) lpvBuf + *lpnBytesReceived ),
            lprSocket->abyRecvBuffer,
            wBytesAvailable );

    // total bytes including any we previously got from our buffer
    *lpnBytesReceived += wBytesAvailable;

    // recalculate buffer stats
    lprSocket->wRecvBufferCount -= wBytesAvailable;
    lprSocket->wRecvBufferOffset += wBytesAvailable;

    return SOCKET_STATUS_OK;
}


//*************************************************************************
//
// SOCKET_STATUS SocketSend()
//
// Parameters:
//  hSocket                 A socket handle
//  lpvBuf                  Buffer containing data to send
//  nBufLenBytes            Size of buffer in bytes
//  lpnBytesReceived        Ptr to int to receive # bytes sent
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Note that the successful completion of a send does not indicate that
//  the data was successfully delivered.
//
//  If no buffer space is available within the transport system to hold
//  the data to be transmitted, send will block unless the socket has
//  been placed in a non-blocking I/O mode.  On non-blocking sockets,
//  the number of bytes written may be between 1 and the requested
//  length, depending on buffer availability on both the local and
//  foreign hosts.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketSend
(
    HSOCKET             hSocket,
    LPVOID              lpvBuf,
    int                 nBufLenBytes,
    LPINT               lpnBytesSent,
    LPDWORD             lpdwError
)
{
    LPOSSOCKET lprSocket = (LPOSSOCKET) hSocket;
    *lpnBytesSent = send(lprSocket->rSocket,(char FAR *)lpvBuf,nBufLenBytes,0);

    if (*lpnBytesSent == SOCKET_ERROR)
    {
        *lpdwError = (DWORD)WSAGetLastError();
        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketReadLine()
//
// Parameters:
//  hSocket                 A socket handle
//  lpszBuf                 Buffer where line will be placed; the CR/LF will
//                          be stripped from the returned line.
//  nBufLen                 Maximum size of the receive buffer
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function gets the next line of text (delimited by a CR/LF or LF) from
//  the input socket.  It does so by reading one byte at a time from the
//  specified socket.  If the line being read is greater than the buffer size,
//  then the nBufLen-1 bytes are stored in the buffer.  A successive call to
//  SocketReadLine will retrieve the rest of the bytes.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketReadLine
(
    HSOCKET             hSocket,
    LPSTR               lpszBuf,
    int                 nBufLen,
    LPDWORD             lpdwError
)
{
    WORD wStrLen;
    LPOSSOCKET lprSocket = (LPOSSOCKET) hSocket;

    if( SOCKET_STATUS_OK != SocketReadLineB( lprSocket,
                                             lpszBuf,
                                             nBufLen,
                                             lpdwError ))
    {
        return SOCKET_STATUS_ERROR;
    }

    wStrLen = strlen( lpszBuf );

    if( wStrLen > 0 && '\n' == lpszBuf[wStrLen - 1])
    {
        if( wStrLen > 1 && '\r' == lpszBuf[wStrLen - 2])
        {
            lpszBuf[wStrLen - 2] = '\0';
        }
        else
        {
            lpszBuf[wStrLen - 1] = '\0';
        }
    }
    return SOCKET_STATUS_OK;
}


//*************************************************************************
//
// SOCKET_STATUS SocketReadLineB()
//
// Parameters:
//  hSocket                 A socket handle
//  lpszBuf                 Buffer where line will be placed; the CR/LF will
//                          NOT be stripped from the returned line.
//  nBufLen                 Maximum size of the receive buffer
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function gets the next line of text (delimited by a CR/LF or LF) from
//  the input socket.  It does so by reading one byte at a time from the
//  specified socket.  If the line being read is greater than the buffer size,
//  then the nBufLen-1 bytes are stored in the buffer.  A successive call to
//  SocketReadLine will retrieve the rest of the bytes.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketReadLineB
(
    HSOCKET             hSocket,
    LPSTR               lpszBuf,
    int                 nBufLen,
    LPDWORD             lpdwError
)
{
    WORD    wBytesReceived;
    WORD    wBytesAvailable;
    LPVOID  lpvCR;
    LPOSSOCKET lprSocket = (LPOSSOCKET) hSocket;

    // start with zero bytes read
    wBytesReceived = 0;

    while(1)
    {
        // if the buffer is empty
        if( lprSocket->wRecvBufferCount == 0 )
        {
            // fill it
            lprSocket->wRecvBufferCount = recv( lprSocket->rSocket,
                                                (char FAR *) lprSocket->abyRecvBuffer,
                                                OSSOCKET_BUFFER_SIZE,
                                                0 );

            if( lprSocket->wRecvBufferCount == 0 )
            {
                // Socket is closed.
                lprSocket->wRecvBufferCount = 0;
                *lpdwError = (DWORD) SOCKET_ERROR_CONNRESET;
                return SOCKET_STATUS_ERROR;
            }

            if( lprSocket->wRecvBufferCount == (WORD) SOCKET_ERROR )
            {
                // uh-oh!
                lprSocket->wRecvBufferCount = 0;
                *lpdwError = (DWORD) WSAGetLastError();
                return SOCKET_STATUS_ERROR;
            }

            lprSocket->wRecvBufferOffset = 0;
        }

        // if there's a <CR> in the buffer
        lpvCR = memchr( lprSocket->abyRecvBuffer + lprSocket->wRecvBufferOffset,
                        '\n',
                        lprSocket->wRecvBufferCount );

        if( NULL != lpvCR )
        {
            // entire readline handled by buffer

            // ++lpvCR to include the '\n'
            wBytesAvailable = __min(( 1 + (LPBYTE) lpvCR ) -
                                    ( lprSocket->abyRecvBuffer +
                                      lprSocket->wRecvBufferOffset ),
                                      nBufLen - 1 );

            memcpy( lpszBuf + wBytesReceived,
                    lprSocket->abyRecvBuffer + lprSocket->wRecvBufferOffset,
                    wBytesAvailable );

            lpszBuf[wBytesAvailable + wBytesReceived] = '\0';

            // reset buffer stats
            lprSocket->wRecvBufferCount -= wBytesAvailable;
            lprSocket->wRecvBufferOffset += wBytesAvailable;

            return SOCKET_STATUS_OK;
        }
        else
        {
            // start with what's in the buffer
            wBytesAvailable = __min( lprSocket->wRecvBufferCount, nBufLen - 1 );
            memcpy( lpszBuf + wBytesReceived,
                    lprSocket->abyRecvBuffer + lprSocket->wRecvBufferOffset,
                    wBytesAvailable );

            // keep track of buffer size
            wBytesReceived += wBytesAvailable;
            lprSocket->wRecvBufferCount -= wBytesAvailable;
            lprSocket->wRecvBufferOffset += wBytesAvailable;

            // we only have this much space left
            nBufLen -= wBytesAvailable;

            if( 1 == nBufLen )
            {
                lpszBuf[wBytesReceived] = '\0';
                return SOCKET_STATUS_OK;
            }
        }
    }
}


//*************************************************************************
//
// SOCKET_STATUS SocketWriteLine()
//
// Parameters:
//  hSocket                 A socket handle
//  lpszBuf                 0-terminated buffer to send to the socket
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function sends a 0-terminated string to the specified socket and
//  send sends a CR LF afterwards.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketWriteLine
(
    HSOCKET             hSocket,
    LPSTR               lpszBuf,
    LPDWORD             lpdwError
)
{
    int         nBytesSent, nStrLen;
    LPOSSOCKET  lprSocket = (LPOSSOCKET) hSocket;

    nStrLen = strlen(lpszBuf);


    if (SocketSend(lprSocket,
                   lpszBuf,
                   nStrLen,
                   &nBytesSent,
                   lpdwError) != SOCKET_STATUS_OK ||
        nBytesSent != nStrLen)
    {
        return(SOCKET_STATUS_ERROR);
    }

    if (SocketSend(lprSocket,
                   "\r\n",
                   2,
                   &nBytesSent,
                   lpdwError) != SOCKET_STATUS_OK ||
        nBytesSent != 2)
    {
        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketSendDWORD()
//
// Parameters:
//  hSocket                 A socket handle
//  dwValue                 DWORD value to send
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function converts the DWORD value from host byte order
//  to big endian byte order and then sends it on the given socket.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketSendDWORD
(
    HSOCKET             hSocket,
    DWORD               dwValue,
    LPDWORD             lpdwError
)
{
    int                 nBytesSent;
    BYTE                byBigEndianDWORD[4];
    LPOSSOCKET          lprSocket = (LPOSSOCKET) hSocket;


    // Convert to big endian byte order

    byBigEndianDWORD[0] = (BYTE)(dwValue >> 24);
    byBigEndianDWORD[1] = (BYTE)(dwValue >> 16);
    byBigEndianDWORD[2] = (BYTE)(dwValue >> 8);
    byBigEndianDWORD[3] = (BYTE)(dwValue);

    if (SocketSend(lprSocket,
                   byBigEndianDWORD,
                   sizeof(DWORD),
                   &nBytesSent,
                   lpdwError) != SOCKET_STATUS_OK ||
        nBytesSent != sizeof(DWORD))
    {
        // Error sending

        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketSendWORD()
//
// Parameters:
//  hSocket                 A socket handle
//  wValue                  WORD value to send
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function converts the WORD value from host byte order
//  to big endian byte order and then sends it on the given socket.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketSendWORD
(
    HSOCKET             hSocket,
    WORD                wValue,
    LPDWORD             lpdwError
)
{
    int                 nBytesSent;
    BYTE                byBigEndianWORD[2];
    LPOSSOCKET          lprSocket = (LPOSSOCKET) hSocket;

    // Convert to big endian byte order

    byBigEndianWORD[0] = (BYTE)(wValue >> 8);
    byBigEndianWORD[1] = (BYTE)(wValue);

    if (SocketSend(lprSocket,
                   byBigEndianWORD,
                   sizeof(WORD),
                   &nBytesSent,
                   lpdwError) != SOCKET_STATUS_OK ||
        nBytesSent != sizeof(WORD))
    {
        // Error sending

        return(SOCKET_STATUS_ERROR);
    }

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketReceiveDWORD()
//
// Parameters:
//  hSocket                 A socket handle
//  lpdwValue               Ptr to DWORD variable to store value received
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function receives a DWORD value from the given socket
//  and converts the value from big endian byte order to host byte order.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketReceiveDWORD
(
    HSOCKET             hSocket,
    LPDWORD             lpdwValue,
    LPDWORD             lpdwError
)
{
    int                 nBytesReceived;
    LPOSSOCKET          lprSocket = (LPOSSOCKET) hSocket;

    if (SocketReceive(lprSocket,
                      lpdwValue,
                      sizeof(DWORD),
                      &nBytesReceived,
                      lpdwError) != SOCKET_STATUS_OK ||
        nBytesReceived != sizeof(DWORD))
    {
        // Error receiving

        return(SOCKET_STATUS_ERROR);
    }

    // Convert to host byte order

    *lpdwValue = (((DWORD)((LPBYTE)lpdwValue)[0]) << 24) |
                 (((DWORD)((LPBYTE)lpdwValue)[1]) << 16) |
                 (((DWORD)((LPBYTE)lpdwValue)[2]) << 8) |
                 ((DWORD)((LPBYTE)lpdwValue)[3]);

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketReceiveWORD()
//
// Parameters:
//  hSocket                 A socket handle
//  lpwValue                Ptr to WORD variable to store value received
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function receives a WORD value from the given socket
//  and converts the value from big endian byte order to host byte order.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketReceiveWORD
(
    HSOCKET             hSocket,
    LPWORD              lpwValue,
    LPDWORD             lpdwError
)
{
    int                 nBytesReceived;
    LPOSSOCKET          lprSocket = (LPOSSOCKET) hSocket;

    if (SocketReceive(lprSocket,
                      lpwValue,
                      sizeof(WORD),
                      &nBytesReceived,
                      lpdwError) != SOCKET_STATUS_OK ||
        nBytesReceived != sizeof(WORD))
    {
        // Error receiving

        return(SOCKET_STATUS_ERROR);
    }

    // Convert to host byte order

    *lpwValue = (((WORD)((LPBYTE)lpwValue)[0]) << 8) |
                ((WORD)((LPBYTE)lpwValue)[1]);

    return(SOCKET_STATUS_OK);
}


//*************************************************************************
//
// SOCKET_STATUS SocketReceiveBuffer()
//
// Parameters:
//  hSocket                 A socket handle
//  lpvBuffer               Ptr to buffer to read socket data into
//  nBufLen                 Size of buffer to read
//  lpbBytesRead            Number of bytes read into the buffer
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  This function reads data from the socket into the buffer specified for
//  a maximum of nBufLen bytes.  It returns the number of bytes actually
//  read (it may be less than nBufLen if the socket read failed) and any
//  error that it may have encountered.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketReceiveBuffer
(
    HSOCKET             hSocket,
    LPVOID              lpvBuffer,
    int                 nBufLen,
    LPINT               lpnBytesRead,
    LPDWORD             lpdwError
)
{
    int                 nBytesReceived;
    int                 nBytesLeft;
    LPSTR               lpBuffer;

    *lpnBytesRead = 0;

    lpBuffer = (LPSTR) lpvBuffer;
    nBytesLeft = nBufLen;

    for (; nBytesLeft > 0;)
    {
        if (*lpnBytesRead > nBufLen)
            return (SOCKET_STATUS_OK);

        // fetch a byte from the socket

        if (SocketReceive(hSocket,
                          (LPVOID) lpBuffer,
                          nBytesLeft,
                          &nBytesReceived,
                          lpdwError) != SOCKET_STATUS_OK)
        {
            return (SOCKET_STATUS_ERROR);
        }

        // check for an error
        if (nBytesReceived == SOCKET_ERROR)
        {
            *lpdwError = SOCKET_ERROR_SHUTDOWN;
            return (SOCKET_STATUS_ERROR);
        }
        else if (nBytesReceived == 0)
        {
            return (SOCKET_STATUS_OK);
        }

        lpBuffer += nBytesReceived;
        nBytesLeft -= nBytesReceived;

        *lpnBytesRead += nBytesReceived;
    }

    return (SOCKET_STATUS_OK);
}

#if 0                                   // this doesn't seem very reliable,
                                        // especially when it returns 8192!
//*************************************************************************
//
// SOCKET_STATUS SocketGetReceivableCount()
//
// Parameters:
//  hSocket                 A bound socket handle
//  lpdwCount               Ptr to DWORD returns with count of bytes ready to
//                          be SocketReceived()
//  lpdwError               Ptr to DWORD to store error code on error
//
// Description:
//  Returns the number of bytes available.
//
// Returns:
//  SOCKET_STATUS_OK        On success
//  SOCKET_STATUS_ERROR     On error
//
//*************************************************************************

SOCKET_STATUS SocketGetReceivableCount
(
    HSOCKET             hSocket,
    LPDWORD             lpdwCount,
    LPDWORD             lpdwError
)
{
    LPOSSOCKET  lprSocket = (LPOSSOCKET) hSocket;
    if (ioctlsocket(hSocket->rSocket,FIONREAD,lpdwCount) == SOCKET_ERROR)
        {
        *lpdwError = (DWORD)WSAGetLastError();
        return(SOCKET_STATUS_ERROR);
        }
    *lpdwCount += hSocket->wRecvBufferCount;
    return(SOCKET_STATUS_OK);
}
#endif                                  // 0
