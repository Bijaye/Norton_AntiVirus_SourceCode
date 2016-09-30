// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef _scssock_h_
#define _scssock_h_

////////////////////////////////////////////////////////////////////////////////////
//from windows ws2tctip.h	identical to the same header in netware

/* Structure used in getaddrinfo() call */

typedef struct addrinfo
{
	 int						ai_flags;		 // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
	 int						ai_family;		 // PF_xxx
	 int						ai_socktype;	 // SOCK_xxx
	 int						ai_protocol;	 // 0 or IPPROTO_xxx for IPv4 and IPv6
	 size_t					ai_addrlen;		 // Length of ai_addr
	 char *					ai_canonname;	 // Canonical name for nodename
	 struct sockaddr *	ai_addr;			 // Binary address
	 struct addrinfo *	ai_next;			 // Next structure in linked list
}
ADDRINFOA, *PADDRINFOA;

//	 RFC standard definition for getaddrinfo()

typedef ADDRINFOA			ADDRINFO, FAR * LPADDRINFO;

/* Flags used in "hints" argument to getaddrinfo() */

#define AI_PASSIVE	  0x1	 /* Socket address will be used in bind() call */
#define AI_CANONNAME	  0x2	 /* Return canonical name in first ai_canonname */
#define AI_NUMERICHOST 0x4	 /* Nodename must be a numeric address string */

////////////////////////////////////////////////////////////////////////////////
//from netware ws2tcpip:
#ifndef wsnchar
# define _WSNCHAR
#define wsnchar unsigned char
#endif

// This typedef results in a conflicting definition of CHAR in winBaseTypes.h.
// So wsnchar is used natively below instead.
//typedef wsnchar			  CHAR;

#ifndef WCHAR
# define WCHAR				wchar_t
#endif

#ifndef NLM
extern "C" int WSAGetLastError();
#endif

#ifndef WSABASEERR

# define WSABASEERR					10000
/*
** Windows Sockets definitions of regular Microsoft C error constants
*/
# define WSAEFAULT					(WSABASEERR+14)
# define WSAEINVAL					(WSABASEERR+22)

/*
** Windows Sockets definitions of regular Berkeley error constants
*/
# define WSAEWOULDBLOCK				(WSABASEERR+35)
# define WSAENOTSOCK					(WSABASEERR+38)
# define WSAEAFNOSUPPORT			(WSABASEERR+47)
# define WSAEADDRINUSE				(WSABASEERR+48)
# define WSAEADDRNOTAVAIL			(WSABASEERR+49)
# define WSAENETDOWN					(WSABASEERR+50)
# define WSAENETUNREACH				(WSABASEERR+51)
# define WSAENOBUFS					(WSABASEERR+55)
# define WSAETIMEDOUT				(WSABASEERR+60)
# define WSAECONNREFUSED			(WSABASEERR+61)
/* Non-recoverable errors, FORMERR, REFUSED, NOTIMP */
# define WSANO_RECOVERY				(WSABASEERR+1003)

#endif

#ifndef ERROR_NOT_ENOUGH_MEMORY
# define ERROR_NOT_ENOUGH_MEMORY				  8L			  /* dderror */
#endif

#define WSA_NOT_ENOUGH_MEMORY	  (ERROR_NOT_ENOUGH_MEMORY)

#define TIMEVAL timeval

/*
** Socket address, internet style.
*/
#define WSADESCRIPTION_LEN		  256
#define WSASYS_STATUS_LEN		  128

typedef struct WSAData
{
	WORD				wVersion;
	WORD				wHighVersion;
#ifdef _WIN64
	unsigned short iMaxSockets;
	unsigned short iMaxUdpDg;
	char			  *lpVendorInfo;
	char				szDescription[WSADESCRIPTION_LEN+1];
	char				szSystemStatus[WSASYS_STATUS_LEN+1];
#else
	char				szDescription[WSADESCRIPTION_LEN+1];
	char				szSystemStatus[WSASYS_STATUS_LEN+1];
	unsigned short iMaxSockets;
	unsigned short iMaxUdpDg;
	char			  *lpVendorInfo;
#endif
} WSADATA, * LPWSADATA;

#ifndef IN
# define IN
#endif

#ifndef OUT
# define OUT
#endif

extern "C" int WSAStartup(		IN WORD wVersionRequested,
	 OUT LPWSADATA lpWSAData	 );

extern "C" int WSACleanup(		void	  );

#define MAX_PROTOCOL_CHAIN 7

#define BASE_PROTOCOL		1
#define LAYERED_PROTOCOL	0

typedef struct _WSAPROTOCOLCHAIN
{
	int	 ChainLen;								 /* the length of the chain,		*/
														 /* == 0 means layered protocol, */
														 /* == 1 means base protocol,		*/
														 /*  > 1 means protocol chain		*/
	 DWORD ChainEntries[MAX_PROTOCOL_CHAIN];/* a list of dwCatalogEntryIds	*/
} WSAPROTOCOLCHAIN, *LPWSAPROTOCOLCHAIN;

#define WSAPROTOCOL_LEN	 255

typedef struct _WSAPROTOCOL_INFOA
{
	DWORD				  dwServiceFlags1;
	DWORD				  dwServiceFlags2;
	DWORD				  dwServiceFlags3;
	DWORD				  dwServiceFlags4;
	DWORD				  dwProviderFlags;
	GUID					 ProviderId;
	DWORD				  dwCatalogEntryId;
	WSAPROTOCOLCHAIN	 ProtocolChain;
	int					iVersion;
	int					iAddressFamily;
	int					iMaxSockAddr;
	int					iMinSockAddr;
	int					iSocketType;
	int					iProtocol;
	int					iProtocolMaxOffset;
	int					iNetworkByteOrder;
	int					iSecurityScheme;
	DWORD				  dwMessageSize;
	DWORD				  dwProviderReserved;
	wsnchar			  szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOA, * LPWSAPROTOCOL_INFOA;

typedef struct _WSAPROTOCOL_INFOW
{
	DWORD				  dwServiceFlags1;
	DWORD				  dwServiceFlags2;
	DWORD				  dwServiceFlags3;
	DWORD				  dwServiceFlags4;
	DWORD				  dwProviderFlags;
	GUID					 ProviderId;
	DWORD				  dwCatalogEntryId;
	WSAPROTOCOLCHAIN	 ProtocolChain;
	int					iVersion;
	int					iAddressFamily;
	int					iMaxSockAddr;
	int					iMinSockAddr;
	int					iSocketType;
	int					iProtocol;
	int					iProtocolMaxOffset;
	int					iNetworkByteOrder;
	int					iSecurityScheme;
	DWORD				  dwMessageSize;
	DWORD				  dwProviderReserved;
	WCHAR				  szProtocol[WSAPROTOCOL_LEN+1];
} WSAPROTOCOL_INFOW, *LPWSAPROTOCOL_INFOW;

#ifdef UNICODE
typedef WSAPROTOCOL_INFOW WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOW LPWSAPROTOCOL_INFO;
#else
typedef WSAPROTOCOL_INFOA WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOA LPWSAPROTOCOL_INFO;
#endif

#define SO_PROTOCOL_INFOA 0x2004		  /* WSAPROTOCOL_INFOA structure */
#define SO_PROTOCOL_INFOW 0x2005		  /* WSAPROTOCOL_INFOW structure */
#ifdef UNICODE
# define SO_PROTOCOL_INFO SO_PROTOCOL_INFOW
#else
# define SO_PROTOCOL_INFO SO_PROTOCOL_INFOA
#endif



#if !defined(MAKEWORD)
#define MAKEWORD(low,high) \
		  ((WORD)(((BYTE)(low)) | ((WORD)((BYTE)(high))) << 8))
#endif

///////////////////////////////////////////////////////////////////////////////////
#endif //_scssock_h_
