// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/OSSOCKET.H_v   1.0   25 Apr 1997 12:09:30   JBRENNA  $
//
// Description:
//  Header file for socket implementation.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/OSSOCKET.H_v  $
// 
//    Rev 1.0   25 Apr 1997 12:09:30   JBRENNA
// Initial revision.
// 
//    Rev 1.10   25 Mar 1997 18:32:42   DJesch
// Added a SocketReceiveBuffer() function prototype.
//
//    Rev 1.9   23 Dec 1996 14:03:40   RAY
// Made changes.
//
//    Rev 1.8   20 Nov 1996 11:37:28   AOONWAL
// No change.
//
//    Rev 1.7   05 Nov 1996 13:23:10   RAY
// Added SocketSelectReadable().
//
//    Rev 1.6   31 Oct 1996 11:36:44   CNACHEN
// Added SocketWriteLine
//
//    Rev 1.5   24 Oct 1996 18:24:26   DCHI
// Added SocketSelectWriteable() prototype.
//
//    Rev 1.4   22 Oct 1996 12:11:48   AOONWAL
// No change.
//
//    Rev 1.3   08 Oct 1996 17:22:06   DCHI
// Added prototype for SocketReadLineB().
//
//    Rev 1.2   04 Oct 1996 12:13:06   DCHI
// Added SetNonBlockingMode() function prototype.
//
//    Rev 1.1   24 Sep 1996 20:18:50   DJesch
// Updated based on changes from DCHI
//
//    Rev 1.0   24 Sep 1996 20:09:20   DJesch
// Initial revision.
//
//    Rev 1.0   24 Sep 1996 17:34:00   DJesch
// Initial revision.
//
//************************************************************************

#ifndef _OSSOCKET_H_

#define _OSSOCKET_H_

//////////////////////////////////////////////////////////////////////
// SOCKET_ERROR definitions
//////////////////////////////////////////////////////////////////////

// SOCKET_ERROR_WOULDBLOCK          Operation would block
// SOCKET_ERROR_INPROGRESS          Operation now in progress
// SOCKET_ERROR_ALREADY             Operation already in progress
// SOCKET_ERROR_NOTSOCK             Socket operation on non-socket
// SOCKET_ERROR_DESTADDRREQ         Destination address required
// SOCKET_ERROR_MSGSIZE             Message too long
// SOCKET_ERROR_PROTOTYPE           Protocol wrong type for socket
// SOCKET_ERROR_NOPROTOOPT          Protocol not available
// SOCKET_ERROR_PROTONOSUPPORT      Protocol not supported
// SOCKET_ERROR_SOCKTNOSUPPORT      Socket type not supported
// SOCKET_ERROR_OPNOTSUPP           Operation not supported on transport endpoint
// SOCKET_ERROR_PFNOSUPPORT         Protocol family not supported
// SOCKET_ERROR_AFNOSUPPORT         Address family not supported by protocol
// SOCKET_ERROR_ADDRINUSE           Address already in use
// SOCKET_ERROR_ADDRNOTAVAIL        Cannot assign requested address
// SOCKET_ERROR_NETDOWN             Network is down
// SOCKET_ERROR_NETUNREACH          Network is unreachable
// SOCKET_ERROR_NETRESET            Network dropped connection because of reset
// SOCKET_ERROR_CONNABORTED         Software caused connection abort
// SOCKET_ERROR_CONNRESET           Connection reset by peer
// SOCKET_ERROR_NOBUFS              No buffer space available
// SOCKET_ERROR_ISCONN              Transport endpoint is already connected
// SOCKET_ERROR_NOTCONN             Transport endpoint is not connected
// SOCKET_ERROR_SHUTDOWN            Cannot send after transport endpoint shutdown
// SOCKET_ERROR_TOOMANYREFS         Too many references: cannot splice
// SOCKET_ERROR_TIMEDOUT            Connection timed out
// SOCKET_ERROR_CONNREFUSED         Connection refused
// SOCKET_ERROR_LOOP                Too many symbolic links encountered
// SOCKET_ERROR_NAMETOOLONG         File name too long
// SOCKET_ERROR_HOSTDOWN            Host is down
// SOCKET_ERROR_HOSTUNREACH         No route to host
// SOCKET_ERROR_NOTEMPTY            Directory not empty
// SOCKET_ERROR_PROCLIM             ????????
// SOCKET_ERROR_USERS               Too many users
// SOCKET_ERROR_DQUOT               Quota exceeded
// SOCKET_ERROR_STALE               Stale NFS file handle
// SOCKET_ERROR_REMOTE              Object is remote

#ifdef SYM_UNIX

#include <errno.h>
#include <sys/socket.h>

#define SOCKET_ERROR_WOULDBLOCK             ((DWORD)(int)(EWOULDBLOCK))
#define SOCKET_ERROR_INPROGRESS             ((DWORD)(int)(EINPROGRESS))
#define SOCKET_ERROR_ALREADY                ((DWORD)(int)(EALREADY))
#define SOCKET_ERROR_NOTSOCK                ((DWORD)(int)(ENOTSOCK))
#define SOCKET_ERROR_DESTADDRREQ            ((DWORD)(int)(EDESTADDRREQ))
#define SOCKET_ERROR_MSGSIZE                ((DWORD)(int)(EMSGSIZE))
#define SOCKET_ERROR_PROTOTYPE              ((DWORD)(int)(EPROTOTYPE))
#define SOCKET_ERROR_NOPROTOOPT             ((DWORD)(int)(ENOPROTOOPT))
#define SOCKET_ERROR_PROTONOSUPPORT         ((DWORD)(int)(EPROTONOSUPPORT))
#define SOCKET_ERROR_SOCKTNOSUPPORT         ((DWORD)(int)(ESOCKTNOSUPPORT))
#define SOCKET_ERROR_OPNOTSUPP              ((DWORD)(int)(EOPNOTSUPP))
#define SOCKET_ERROR_PFNOSUPPORT            ((DWORD)(int)(EPFNOSUPPORT))
#define SOCKET_ERROR_AFNOSUPPORT            ((DWORD)(int)(EAFNOSUPPORT))
#define SOCKET_ERROR_ADDRINUSE              ((DWORD)(int)(EADDRINUSE))
#define SOCKET_ERROR_ADDRNOTAVAIL           ((DWORD)(int)(EADDRNOTAVAIL))
#define SOCKET_ERROR_NETDOWN                ((DWORD)(int)(ENETDOWN))
#define SOCKET_ERROR_NETUNREACH             ((DWORD)(int)(ENETUNREACH))
#define SOCKET_ERROR_NETRESET               ((DWORD)(int)(ENETRESET))
#define SOCKET_ERROR_CONNABORTED            ((DWORD)(int)(ECONNABORTED))
#define SOCKET_ERROR_CONNRESET              ((DWORD)(int)(ECONNRESET))
#define SOCKET_ERROR_NOBUFS                 ((DWORD)(int)(ENOBUFS))
#define SOCKET_ERROR_ISCONN                 ((DWORD)(int)(EISCONN))
#define SOCKET_ERROR_NOTCONN                ((DWORD)(int)(ENOTCONN))
#define SOCKET_ERROR_SHUTDOWN               ((DWORD)(int)(ESHUTDOWN))
#define SOCKET_ERROR_TOOMANYREFS            ((DWORD)(int)(ETOOMANYREFS))
#define SOCKET_ERROR_TIMEDOUT               ((DWORD)(int)(ETIMEDOUT))
#define SOCKET_ERROR_CONNREFUSED            ((DWORD)(int)(ECONNREFUSED))
#define SOCKET_ERROR_LOOP                   ((DWORD)(int)(ELOOP))
#define SOCKET_ERROR_NAMETOOLONG            ((DWORD)(int)(ENAMETOOLONG))
#define SOCKET_ERROR_HOSTDOWN               ((DWORD)(int)(EHOSTDOWN))
#define SOCKET_ERROR_HOSTUNREACH            ((DWORD)(int)(EHOSTUNREACH))
#define SOCKET_ERROR_NOTEMPTY               ((DWORD)(int)(ENOTEMPTY))
#define SOCKET_ERROR_PROCLIM                ((DWORD)(int)(EPROCLIM))
#define SOCKET_ERROR_USERS                  ((DWORD)(int)(EUSERS))
#define SOCKET_ERROR_DQUOT                  ((DWORD)(int)(EDQUOT))
#define SOCKET_ERROR_STALE                  ((DWORD)(int)(ESTALE))
#define SOCKET_ERROR_REMOTE                 ((DWORD)(int)(EREMOTE))

#else // !SYM_UNIX

#include <winsock.h>

#define SOCKET_ERROR_WOULDBLOCK             ((DWORD)(int)(WSAEWOULDBLOCK))
#define SOCKET_ERROR_INPROGRESS             ((DWORD)(int)(WSAEINPROGRESS))
#define SOCKET_ERROR_ALREADY                ((DWORD)(int)(WSAEALREADY))
#define SOCKET_ERROR_NOTSOCK                ((DWORD)(int)(WSAENOTSOCK))
#define SOCKET_ERROR_DESTADDRREQ            ((DWORD)(int)(WSAEDESTADDRREQ))
#define SOCKET_ERROR_MSGSIZE                ((DWORD)(int)(WSAEMSGSIZE))
#define SOCKET_ERROR_PROTOTYPE              ((DWORD)(int)(WSAEPROTOTYPE))
#define SOCKET_ERROR_NOPROTOOPT             ((DWORD)(int)(WSAENOPROTOOPT))
#define SOCKET_ERROR_PROTONOSUPPORT         ((DWORD)(int)(WSAEPROTONOSUPPORT))
#define SOCKET_ERROR_SOCKTNOSUPPORT         ((DWORD)(int)(WSAESOCKTNOSUPPORT))
#define SOCKET_ERROR_OPNOTSUPP              ((DWORD)(int)(WSAEOPNOTSUPP))
#define SOCKET_ERROR_PFNOSUPPORT            ((DWORD)(int)(WSAEPFNOSUPPORT))
#define SOCKET_ERROR_AFNOSUPPORT            ((DWORD)(int)(WSAEAFNOSUPPORT))
#define SOCKET_ERROR_ADDRINUSE              ((DWORD)(int)(WSAEADDRINUSE))
#define SOCKET_ERROR_ADDRNOTAVAIL           ((DWORD)(int)(WSAEADDRNOTAVAIL))
#define SOCKET_ERROR_NETDOWN                ((DWORD)(int)(WSAENETDOWN))
#define SOCKET_ERROR_NETUNREACH             ((DWORD)(int)(WSAENETUNREACH))
#define SOCKET_ERROR_NETRESET               ((DWORD)(int)(WSAENETRESET))
#define SOCKET_ERROR_CONNABORTED            ((DWORD)(int)(WSAECONNABORTED))
#define SOCKET_ERROR_CONNRESET              ((DWORD)(int)(WSAECONNRESET))
#define SOCKET_ERROR_NOBUFS                 ((DWORD)(int)(WSAENOBUFS))
#define SOCKET_ERROR_ISCONN                 ((DWORD)(int)(WSAEISCONN))
#define SOCKET_ERROR_NOTCONN                ((DWORD)(int)(WSAENOTCONN))
#define SOCKET_ERROR_SHUTDOWN               ((DWORD)(int)(WSAESHUTDOWN))
#define SOCKET_ERROR_TOOMANYREFS            ((DWORD)(int)(WSAETOOMANYREFS))
#define SOCKET_ERROR_TIMEDOUT               ((DWORD)(int)(WSAETIMEDOUT))
#define SOCKET_ERROR_CONNREFUSED            ((DWORD)(int)(WSAECONNREFUSED))
#define SOCKET_ERROR_LOOP                   ((DWORD)(int)(WSAELOOP))
#define SOCKET_ERROR_NAMETOOLONG            ((DWORD)(int)(WSAENAMETOOLONG))
#define SOCKET_ERROR_HOSTDOWN               ((DWORD)(int)(WSAEHOSTDOWN))
#define SOCKET_ERROR_HOSTUNREACH            ((DWORD)(int)(WSAEHOSTUNREACH))
#define SOCKET_ERROR_NOTEMPTY               ((DWORD)(int)(WSAENOTEMPTY))
#define SOCKET_ERROR_PROCLIM                ((DWORD)(int)(WSAEPROCLIM))
#define SOCKET_ERROR_USERS                  ((DWORD)(int)(WSAEUSERS))
#define SOCKET_ERROR_DQUOT                  ((DWORD)(int)(WSAEDQUOT))
#define SOCKET_ERROR_STALE                  ((DWORD)(int)(WSAESTALE))
#define SOCKET_ERROR_REMOTE                 ((DWORD)(int)(WSAEREMOTE))

#endif // #ifdef SYM_UNIX

//////////////////////////////////////////////////////////////////////
// Structure and type definitions
//////////////////////////////////////////////////////////////////////

#define OSSOCKET_BUFFER_SIZE  8192

typedef struct tagOSSOCKET
{
    SOCKET  rSocket;
    BYTE    abyRecvBuffer[OSSOCKET_BUFFER_SIZE];
    WORD    wRecvBufferOffset;         // Where "unread" data starts
    WORD    wRecvBufferCount;          // Number of "unread" bytes
} OSSOCKET, FAR * LPOSSOCKET;
#define sOSSOCKET ( sizeof ( OSSOCKET ))

typedef int SOCKET_STATUS;

#define SOCKET_STATUS_OK        0
#define SOCKET_STATUS_ERROR     1

typedef HANDLE HSOCKET, FAR *LPHSOCKET;

#define HINVALID_SOCKET ((HANDLE) INVALID_SOCKET)

#ifdef SYM_UNIX

struct in_addr
{
    union
    {
        struct
        {
            BYTE        s_b1;
            BYTE        s_b2;
            BYTE        s_b3;
            BYTE        s_b4;
        } S_un_b;

        struct
        {
            WORD        s_w1;
            WORD        s_w2;
        } S_un_w;

        DWORD           S_addr;
    } S_un;
};

struct sockaddr_in
{
    short               sin_family;
    WORD                sin_port;
    struct in_addr      sin_addr;
    char                sin_zero[8];
};

#endif // SYM_UNIX

#define MAX_INET_ADDR_LEN           255

typedef struct tagSOCKET_ADDRESS
{
    struct sockaddr_in  stSockAddr;
} SOCKET_ADDRESS_T, FAR *LPSOCKET_ADDRESS;

typedef LPSOCKET_ADDRESS FAR *LPLPSOCKET_ADDRESS;

//////////////////////////////////////////////////////////////////////
// Function prototypes
//////////////////////////////////////////////////////////////////////

SOCKET_STATUS SocketAddressFill
(
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPSTR               lpszInternetAddress,
    WORD                wPort,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketAddressGetIP
(
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPBYTE              lpbyIP,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketAddressCmpIP
(
    LPSOCKET_ADDRESS    lpstSocketAddress0,
    LPSOCKET_ADDRESS    lpstSocketAddress1,
    LPBOOL              lpbEqual,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketCreate
(
    LPHSOCKET           lphSocket,
    BOOL                bNonBlocking,
    BOOL                bLinger,
    int                 nLingerTimeSec,
    LPDWORD             lpdwError
);

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
);

SOCKET_STATUS SocketSetNonBlockingMode
(
    HSOCKET             hSocket,
    BOOL                bNonBlocking,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketListen
(
    HSOCKET             hSocket,
    int                 nBacklog,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketAccept
(
    HSOCKET             hSocket,
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPHSOCKET           lphNewSocket,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketConnect
(
    HSOCKET             hSocket,
    LPSOCKET_ADDRESS    lpstSocketAddress,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketSelectWriteable
(
    HSOCKET             hSocket,
    DWORD               dwTimeLimitMS,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketSelectReadable
(
    HSOCKET             hSocket,
    DWORD               dwTimeLimitMS,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketDestroy
(
    HSOCKET             hSocket,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketReceive
(
    HSOCKET             hSocket,
    LPVOID              lpvBuf,
    int                 nBufLenBytes,
    LPINT               lpnBytesReceived,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketSend
(
    HSOCKET             hSocket,
    LPVOID              lpvBuf,
    int                 nBufLenBytes,
    LPINT               lpnBytesSent,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketReadLine
(
    HSOCKET             hSocket,
    LPSTR               lpszBuf,
    int                 nBufLen,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketReadLineB
(
    HSOCKET             hSocket,
    LPSTR               lpszBuf,
    int                 nBufLen,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketWriteLine
(
    HSOCKET             hSocket,
    LPSTR               lpszBuf,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketSendDWORD
(
    HSOCKET             hSocket,
    DWORD               dwValue,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketSendWORD
(
    HSOCKET             hSocket,
    WORD                wValue,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketReceiveDWORD
(
    HSOCKET             hSocket,
    LPDWORD             lpdwValue,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketReceiveWORD
(
    HSOCKET             hSocket,
    LPWORD              lpwValue,
    LPDWORD             lpdwError
);

SOCKET_STATUS SocketReceiveBuffer
(
    HSOCKET             hSocket,
    LPVOID              lpvBuffer,
    int                 nBufLen,
    LPINT               lpnBytesRead,
    LPDWORD             lpdwError
);

#if 0
SOCKET_STATUS SocketGetReceivableCount
(
    HSOCKET             hSocket,
    LPDWORD             lpdwCount,
    LPDWORD             lpdwError
);
#endif  // 0

#endif  // #ifndef _OSSOCKET_H_

