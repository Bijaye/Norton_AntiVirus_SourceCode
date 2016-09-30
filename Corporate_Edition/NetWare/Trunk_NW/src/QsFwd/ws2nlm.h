#ifndef __ws2nlm_h__
#define __ws2nlm_h__
/*============================================================================
=  Novell Software Development Kit
=
=  Copyright (C) 1999-2001 Novell, Inc. All Rights Reserved.
=
=  This work is subject to U.S. and international copyright laws and treaties.
=  Use and redistribution of this work is subject  to  the  license  agreement
=  accompanying  the  software  development kit (SDK) that contains this work.
=  However, no part of this work may be revised and/or  modified  without  the
=  prior  written consent of Novell, Inc. Any use or exploitation of this work
=  without authorization could subject the perpetrator to criminal  and  civil
=  liability. 
=
=  Source(s): Novell Header
=
=  ws2nlm.h
==============================================================================
*/
#include <ws2defs.h>

#if !defined(N_PLAT_NLM)
# define N_PLAT_NLM
#endif


#define _INC_WINDOWS 

/* socket() 'flags'... */
#define SKTS_RAWCALLBACKS        0x40000000L
#define SKTS_WORKTODOCALLBACKS   0x20000000L

/* ioctl() 'flags' arguments... */
#define SIO_RAWCALLBACKS         0x1ADD0002L
#define SIO_WORKTODOCALLBACKS    0x1ADD0004L
#define SIO_FASTACCEPTCALLBACK   0x1ADD0008L
#define SIO_FASTRECVCALLBACK     0x1ADD000C

/* affects blocking mode as well as overlapped callbacks */
#define SIO_RCVFULLMSG           0x1ADD0010 

/* NetWare specific socket options */
#define SO_CONNTIMEO             0x1009 /* connect timeout */

/* some system flags not defined in NetWare... */
#define INFINITE                 0xFFFFFFFFUL
#define WAIT_FAILED              0xFFFFFFFFUL
#define WAIT_OBJECT_0            0x00000000UL
#define WAIT_ABANDONED           0x00000080UL
#define WAIT_IO_COMPLETION       0x000000C0UL
#define WAIT_TIMEOUT             0x00000102UL
#define MAXIMUM_WAIT_OBJECTS     64UL

#ifndef USE_WINSOCK_DIRECTLY
# define USE_WINSOCK_DIRECTLY	1
#endif /* USE_WINSOCK_DIRECTLY */

#if USE_WINSOCK_DIRECTLY
/*
** Bring in WinSock 2 naked and use it straight without intervention by the C
** runtime library. The descriptors passed back from socket() won't be unified
** with those returned by open(), etc.
*/
# define INCL_WINSOCK_API_TYPEDEFS     1
# define INCL_WINSOCK_API_PROTOTYPES   1
/*
** BSD sockets clashes that must first be maped to WS2_32.NLM calls before
** they can be converted to Winsock 2.
*/  
# define htonl              WS2_32_htonl 
# define ntohl              WS2_32_ntohl 
# define htons              WS2_32_htons 
# define ioctlsocket        WS2_32_ioctlsocket
# define ntohs              WS2_32_ntohs 
# define send               WS2_32_send
# define recv               WS2_32_recv
# define bind               WS2_32_bind
# define listen             WS2_32_listen
# define closesocket        WS2_32_closesocket
# define getpeername        WS2_32_getpeername
# define getsockname        WS2_32_getsockname
# define getsockopt         WS2_32_getsockopt
# define recvfrom           WS2_32_recvfrom
# define select             WS2_32_select
# define sendto             WS2_32_sendto
# define setsockopt         WS2_32_setsockopt
# define socket             WS2_32_socket
# define shutdown           WS2_32_shutdown
# define inet_addr          WS2_32_inet_addr
# define inet_ntoa          WS2_32_inet_ntoa
# define gethostbyaddr      WS2_32_gethostbyaddr
# define gethostbyname      WS2_32_gethostbyname
# define gethostname        WS2_32_gethostname
# define getprotobyname     WS2_32_getprotobyname
# define getprotobynumber   WS2_32_getprotobynumber
# define getservbyname      WS2_32_getservbyname
# define getservbyport      WS2_32_getservbyport
#else
/*
** Bring in WinSock 2, but not its prototypes and typedefs. This option is had
** by including <sys/socket.h> to get Berkeley interfaces out of the C runtime
** library. The descriptors passed back from socket() are unified with those
** returned by open() and can be used within that framwork. By not including
** <sys/socket.h>, WinSock is consumed directly and the descriptors cannot be
** unified with those coming out of the library (from open(), for example).
*/
# define INCL_WINSOCK_API_TYPEDEFS     0
# define INCL_WINSOCK_API_PROTOTYPES   0
#endif /* USE_WINSOCK_DIRECTLY */

#ifdef WIN32
# include <winsock2.h>
#else
# define WIN32
# include <winsock2.h>
# undef WIN32
#endif /* WIN32 */

#if USE_WINSOCK_DIRECTLY
/* BSD sockets clashes that can be maped directly to WSA calls. */
# define connect(s,name,namelen) WSAConnect(s,name,namelen, 0,0,0,0)
# define accept(s,addr,addrlen)  WSAAccept(s,addr,addrlen,0,0)
#else
/* The following two defines come from sys/filio.h for Berkley implementation */
# undef FIONREAD
# undef FIONBIO
/* The following define comes from sys/sockio.h for Berkley implementation */
# undef SIOCATMARK
#endif /* USE_WINSOCK_DIRECTLY */

/*
** NetWare Fast Accept and Recv option structures
** Fast Recv also has a cleanup routine returned.
*/

typedef	
int (CALLBACK *LPFASTACCEPT_COMPLETION_ROUTINE)
(
    SOCKET      acceptSkt, 
    LPSOCKADDR  peerAddr, 
    int         peerAddrLen,
    void        *arg
);

typedef	
int (CALLBACK *LPFASTRECV_COMPLETION_ROUTINE)
(
    SOCKET      s, 
    void        *recvBuf,
    LPWSABUF    wsBuf,
    DWORD       wsBufCnt,
    DWORD       recvLen,
    void        *arg
);
 
typedef struct FASTACCEPT_OP
{
    LPFASTACCEPT_COMPLETION_ROUTINE acceptHandler;
    void        *arg;
}*LPFAST_ACCEPT_OPT, FAST_ACCEPT_OPT;

typedef struct FASTRECV_OP
{
    LPFASTRECV_COMPLETION_ROUTINE   recvHandler;
    void        *arg;
}*LPFAST_RECV_OPT, FAST_RECV_OPT;


/* New function Prototypes	*/
int WSAOpenHosts(void);
int WSAOpenProtocols(void);
int WSAOpenServices(void);
int WSAOpenResolveCfg(void);


#ifdef WS_SSL
# include <sslnlm.h>
#endif /* WS_SSL */

#ifdef WS_TLS
# include <tlsnlm.h>
#endif /* WS_TLS */

#endif /*__ws2nlm_h__ */
