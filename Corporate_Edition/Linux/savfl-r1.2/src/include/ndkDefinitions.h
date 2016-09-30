// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Behzad Moaddeli <Behzad@Symantec.com>
//			  Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Definitions for Linux implementation of NDK functions

#ifndef _ndkDefinitions_h_
#define _ndkDefinitions_h_


#include <sys/types.h> /* for off_t */

#include <assert.h>
#include <limits.h>

#include "winBaseTypes.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NWCONN_HANDLE
#define NWCONN_HANDLE nuint
#endif

#ifndef INFINITE
#define INFINITE		 0xFFFFFFFFUL
#endif
#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0x00000000UL
#endif
#ifndef WAIT_TIMEOUT
#define WAIT_TIMEOUT	 0x00000102UL
#endif
#ifndef WAIT_FAILED
#define WAIT_FAILED	 0xFFFFFFFFUL
#endif

	// Tools/NDK1004/libc/include/errno.h Tools/NDK1004/nwsdk/include/nlm/errno.h

#ifndef ESUCCESS
#define ESUCCESS	0
#endif
#ifndef EBADHNDL
#define EBADHNDL 22
#endif
#ifndef EFAILURE
#define EFAILURE -1
#endif

#ifndef ERR_INSUFFICIENT_SPACE
#define ERR_INSUFFICIENT_SPACE					1
#endif
#ifndef ERR_NETWORK_DISK_IO
#define ERR_NETWORK_DISK_IO					 131
#endif
#ifndef ERR_INVALID_FILE_HANDLE
#define ERR_INVALID_FILE_HANDLE				 136
#endif
#ifndef ERR_NO_READ_PRIVILEGE
#define ERR_NO_READ_PRIVILEGE					 147
#endif
#ifndef ERR_NO_WRITE_PRIVILEGE_OR_READONLY
#define ERR_NO_WRITE_PRIVILEGE_OR_READONLY 148
#endif
#ifndef ERR_FILE_DETACHED
#define ERR_FILE_DETACHED						 149
#endif
#ifndef ERR_IO_LOCKED
#define ERR_IO_LOCKED							 162
#endif
#ifndef ERR_BAD_PARAMETER
#define ERR_BAD_PARAMETER						 0xFF	 /* 255 */
#endif

#ifndef IMAX_PATH
#define IMAX_PATH PATH_MAX
#endif
#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif
#ifndef _MAX_PATH
#define _MAX_PATH PATH_MAX
#endif

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX		64
#endif

#define VPstrncpy _VPstrncpy

typedef unsigned long nuint;

#ifndef MAX_PACKET_DATA
#define MAX_PACKET_DATA (CBA_MAX_DATA-sizeof(HEADER)-((6)*sizeof(DWORD)))
#endif


#ifndef	ACCESS_RD
#define	ACCESS_RD R_OK
#endif
#ifndef	ACCESS_WR
#define	ACCESS_WR W_OK
#endif

#ifndef _strncpy
#define _strncpy strncpy
#endif

#ifndef MINUS_ONE
#define MINUS_ONE                ((DWORD)-1)
#endif

#ifdef GLUE


#define FAILED(Status) ((HRESULT)(Status) < 0 )


#if defined(NLM) && !defined(LINUX)
#ifndef NextChar
#define NextChar NWNextChar
#endif
#ifndef CharNext
#define CharNext NextChar
#endif
#ifndef AnsiNext
#define AnsiNext NWNextChar
#endif
#ifndef AnsiPrev
#define AnsiPrev NWPrevChar
#endif
#ifndef charUpper
#define CharUpper NWLstrupr
#endif
#ifndef charLower
#define CharLower NWLstrlwr
#endif
#ifndef StrRChr
#define StrRChr NWLstrrchr
#endif
#ifndef StrChr
#define StrChr NWLstrchr
#endif
#endif // defined(NLM) && !defined(LINUX)


//following is from dec3 platform implementation: platunix.h

/* This kind of declaration makes HMODULES and HINSTANCES strongly typed--
that is, not compatible with any other native types. This will help us catch
errors at compile time. */

/*
struct _HMODULE_STRUCT {
	int unused;
};
typedef struct _HMODULE_STRUCT *HMODULE;
typedef HMODULE HINSTANCE;
*/

#ifndef MYVERIFY
#define MYVERIFY(expr)	assert (expr)
#endif

#endif //GLUE



#ifdef __cplusplus
}
#endif

#endif // _ndkDefinitions_h_
