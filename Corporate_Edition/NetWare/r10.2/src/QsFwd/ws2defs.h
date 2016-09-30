#ifndef __ws2defs_h__
#define __ws2defs_h__
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
=  ws2defs.h
==============================================================================
*/

/* types that may not already be defined... */
#ifndef FARPROC
# define FARPROC        void *
#endif

#ifndef FAR
# define FAR		                /* (ignore: address space is flat on NetWare) */
#endif

#ifndef PASCAL
# define PASCAL
#endif

#ifndef CALLBACK
# define CALLBACK
#endif

#ifndef _UNICODE_TYPE_DEFINED
#define _UNICODE_TYPE_DEFINED
#define  unicode unsigned short    /* Unicode data must be 16 bits   */
#endif

#ifndef BYTE
#define _BYTE
#define BYTE unsigned char
#endif

#ifndef wsnchar
# define _WSNCHAR
#define wsnchar unsigned char
#endif

#ifndef BOOL
# define _BOOL
#define BOOL unsigned int
#endif

#ifndef WORD
# define _WORD
#define  WORD unsigned short
#endif

#ifndef DWORD
# define _DWORD
#define DWORD	unsigned int
#endif

#ifndef LPDWORD
# define _LPDWORD
typedef unsigned int *LPDWORD;
#endif

#ifndef ULONG
# define _ULONG
#define ULONG unsigned long
#endif

#ifndef UCHAR
# define UCHAR          unsigned char
#endif

#ifndef WPARAM
# define _WPARAM
typedef unsigned int WPARAM;
#endif

#ifndef LPARAM
# define _LPARAM
typedef unsigned int LPARAM;
#endif

#ifdef UNICODE
typedef unsigned short *LPTSTR;
#else
typedef char *LPTSTR;
#endif

//#ifndef _WCHAR_T_DEFINED
//typedef unsigned short wchar_t;
//#define _WCHAR_T_DEFINED
//#endif

#ifndef  _WCHAR_T
# define _WCHAR_T
typedef unsigned short  wchar_t;

# ifdef __STDC_ISO_10646__
#  undef __STDC_ISO_10646__      /* estimate conformance at this date */
#  define __STDC_ISO_10646__  200103L
# endif
#endif

#ifndef WCHAR
# define WCHAR          wchar_t
#endif

#ifndef _LPWSTR
# define _LPWSTR
typedef WCHAR           *LPWSTR;
#endif
#ifndef _LPBYTE
# define _LPBYTE
typedef char            *LPBYTE;
#endif
 
#ifndef _INT
# define _INT
typedef int             INT;            /* transmit */
#endif
#ifndef _LPSTR
# define _LPSTR
typedef char            *LPSTR;         /* string */
#endif
 
#ifndef _LPINT
# define _LPINT
typedef int             *LPINT;
#endif
#ifndef _LPVOID
# define _LPVOID
typedef void            *LPVOID;
#endif
#ifndef _VOID
# define _VOID
typedef void            VOID;
#endif
#ifndef _CHAR
# define _CHAR
typedef wsnchar         CHAR;
#endif
 
#ifndef _HWND
# define _HWND
#define HWND            void *
#endif
#ifndef _HANDLE
# define _HANDLE
#define HANDLE          void *
#endif
#ifndef _LPHANDLE
# define _LPHANDLE
typedef HANDLE          *LPHANDLE;
#endif

#ifndef MAKEWORD
# define MAKEWORD(lo, hi)   ((WORD)((BYTE)(lo)) | (((WORD)(BYTE)(hi))<<8))
#endif

#ifndef HIBYTE
#define HIBYTE(a16)	((uint8_t)((uint16_t)(a16) >> 8))
#endif

#ifndef LOBYTE
#define LOBYTE(a16) ((uint8_t)((uint16_t)(a16) & 0xFF))
#endif

#ifndef IN
# define IN
#endif

#ifndef OUT
# define OUT
#endif

#include <pshpack1.h>

typedef struct _OVERLAPPED
{
   DWORD       Internal;
   DWORD       InternalHigh;
   DWORD       Offset;
   DWORD       OffsetHigh;
   HANDLE      hEvent;
} OVERLAPPED, *LPOVERLAPPED;

#include <poppack.h>


/* Microsoft errors and definitions used in winsock2.h... */
#ifndef ERROR_INVALID_HANDLE 
# define ERROR_INVALID_HANDLE               6L
#endif
#ifndef ERROR_NOT_ENOUGH_MEMORY
# define ERROR_NOT_ENOUGH_MEMORY            8L          /* dderror */
#endif
#ifndef ERROR_INVALID_PARAMETER
# define ERROR_INVALID_PARAMETER            87L         /* dderror */
#endif
#ifndef ERROR_IO_PENDING
# define ERROR_IO_PENDING                   997L        /* dderror */
#endif
#ifndef ERROR_OPERATION_ABORTED
# define ERROR_OPERATION_ABORTED            995L
#endif
#ifndef ERROR_IO_INCOMPLETE
# define ERROR_IO_INCOMPLETE                996L
#endif

/* predefined value types... */
#ifndef REG_NONE
# define REG_NONE                           0           /* no value type */
#endif                       

#ifndef REG_SZ
# define REG_SZ                             1           /* Unicode null-terminated string*/
#endif

#ifndef REG_EXPAND_SZ
# define REG_EXPAND_SZ                      2           /* Unicode null-terminated string*/
#endif                                                  /* ... with variable references) */
#ifndef REG_BINARY
# define REG_BINARY                         3           /* free-form binary */
#endif
#ifndef REG_DWORD
# define REG_DWORD                          4           /* 32-bit value */
#endif
#ifndef REG_DWORD_LITTLE_ENDIAN
# define REG_DWORD_LITTLE_ENDIAN            4           /* 32-bit value (REG_DWORD) */
#endif
#ifndef REG_DWORD_BIG_ENDIAN
# define REG_DWORD_BIG_ENDIAN               5           /* 32-bit value */
#endif
#ifndef REG_LINK
# define REG_LINK                           6           /* symbolic link (Unicode) */
#endif
#ifndef REG_MULTI_SZ
# define REG_MULTI_SZ                       7           /* multiple Unicode strings */
#endif
#ifndef REG_RESOURCE_LIST
# define REG_RESOURCE_LIST                  8           /* resource list in map */
#endif
#ifndef REG_FULL_RESOURCE_DESCRIPTOR
# define REG_FULL_RESOURCE_DESCRIPTOR       9           /* resource list in description */
#endif
#ifndef REG_RESOURCE_REQUIREMENTS_LIST
# define REG_RESOURCE_REQUIREMENTS_LIST     10          /* resource list in requirements */
#endif

/* New Address Types */

#define AF_INET_ACP                         25
#define AF_IPX_ACP                          26
#define AF_ACP                              27

/* SSL-related ioctl() 'flags' arguments and notes... */
/* placed here because needed for both ssl/tls programming */

#define SECURITY_PROTOCOL_SSL       1
#define SECURITY_PROTOCOL_TLS       2
#define SIO_SSL_CRYPTFILE           0x1ADD0010UL  
	/*
	** Note: 'optval' is a zero terminated ASCII string, 'optlen' is set to
	** length of string and zero termination.
	*/

#define SIO_SSL_AUTHTYPE            0x1ADD0020UL
	/*
	** There are three interesting authentication types:
	** CLIENT -	Client initiates a SSL connection
	** SERVER - Listener set up to listen for incoming SSL conns, (server sends
	**				its cert during authentication)
	** MUTUAL	is SSL server requesting client authentication, (server asks for
	**				client cert in addition to sending its own)
	** 'optval' is a DWORD defined as:
	*/
#define MUTUAL                      0x00000002UL

#define SIO_SSL_CONVERT             0x1ADD0040UL  
	/*
	** 'optval' is a BOOL; if TRUE, then convert socket to SSL; if FALSE, then
	** convert socket from SSL.
	*/


#endif  /* __ws2defs_h__ */
