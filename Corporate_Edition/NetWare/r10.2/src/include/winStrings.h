// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: NetWare implementation of Windows string handling functions

#ifndef _winStrings_h_
#define _winStrings_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>

#if defined SYM_LIBC_NLM
	#include <utf8.h>

	#include "libcnlm.h"

#elif defined NLM
	#include <nwlocale.h>

	#include "ClibNLM.h"
#endif

// Tell the SymSaferStrings.h library that Netware wants MBCS string processing
#define SYMSAFERSTRINGS_DO_MBCS 1

#if defined _UNICODE
	#ifdef SYM_LIBC_NLM
	   #include <wchar.h> // collides w/ MetroWerk <string> -> <iosfwd>
	#elif NLM
	   #include <unicode.h>
	#endif

	#define TextMapping(wideCharacter,multiByte,singleByte) wideCharacter
#elif defined _MBCS
	#define TextMapping(wideCharacter,multiByte,singleByte) multiByte
#else
	#define TextMapping(wideCharacter,multiByte,singleByte) singleByte
#endif

#ifndef TCHAR
	#define TCHAR TextMapping(wchar_t,char,char)
#endif

#ifndef _TEXT
	#define _TEXT(s) TextMapping(L ## s,s,s)
#endif
#ifndef _T
	#define _T(s) _TEXT(s)
#endif
#ifndef __T
	#define __T(s) _TEXT(s)
#endif

#ifndef _TEOF
	#define _TEOF TextMapping(WEOF,EOF,EOF)
#endif

// ================== libc ==============================================================

#ifdef SYM_LIBC_NLM
	#define _tcslen TextMapping(wcslen,strlen,strlen)

	#define _tcsclen TextMapping(wcslen,_mbslen,strlen)
	#define _mbslen(s) utf8len((const utf8_t*) s)

	#define _tcschr TextMapping(wcschr,_mbschr,strchr)
	char* _mbschr(const char* string,char character);

	#define _tcsrchr TextMapping(wcsrchr,_mbsrchr,strrchr)
	char* _mbsrchr(const char* string,char character);

	#define _tcsncmp TextMapping(wcsncmp,_mbsnbcmp,strncmp)
	int _mbsnbcmp(const char* s1,const char* s2,size_t n);

	#define _tcsnicmp TextMapping(wcsnicmp,_mbsnbicmp,strnicmp)
	int _mbsnbicmp(const char* s1,const char* s2,size_t n);

	#define _tcsncpy TextMapping(wcsncpy,strncpy,strncpy)

    #define _tcsncat TextMapping(wcsncat,strncat,strncat)

	#define _sntprintf TextMapping(swprintf,snprintf,snprintf)
    #define _snprintf snprintf

	#define _vsntprintf TextMapping(vswprintf,vsnprintf,vsnprintf)
    #define _vsnprintf vsnprintf

    #define _stscanf TextMapping(wsscanf,sscanf,sscanf)

	#define CharNext(s)  ((char*) utf8next((const utf8_t*) s))
	#define CharNextA(s) CharNext(s)
	#define AnsiNext(s)  CharNext(s)

// ================== Clib ==============================================================

#elif defined NLM
	#define _tcslen TextMapping(unilen,strlen,strlen)

	#define _tcsclen TextMapping(unilen,_mbslen,strlen)
	#define _mbslen(s) NWLmbslen((const unsigned char*) s)

	#define _tcschr TextMapping(unichr,NWLstrchr,strchr)

	#define _tcsrchr TextMapping(unirchr,NWLstrrchr,strrchr)

	#define _tcsstr TextMapping(unistr,strstr,strstr)

	#define _tcscmp TextMapping(unicmp,NWLstrcoll,strcmp)

	#define _tcsncmp TextMapping(unincmp,_mbsnbcmp,strncmp)
	int _mbsnbcmp(const char* s1,const char* s2,size_t n);

	#define _tcsicmp TextMapping(uniicmp,NWLstricmp,stricmp)

	#define _tcsnicmp TextMapping(uninicmp,_mbsnbicmp,strnicmp)
	int _mbsnbicmp(const char* s1,const char* s2,size_t n);

	#define _tcscpy TextMapping(unicpy,strcpy,strcpy)
	#define lstrcpy strcpy

	#define _tcsncpy TextMapping(unincpy,strncpy,strncpy)

	#define _tcscat TextMapping(unicat,strcat,strcat)
	#define lstrcat strcat

	#define _tcsncat TextMapping(unincat,strncat,strncat)

	// The `uni' functions don't exist -- leave for later when we switch over to use
	// Unicode.

	#define _tcslwr TextMapping(unilwr,_mbslwr,strlwr)
	char* _mbslwr(char* string);

	#define _tcstoul TextMapping(unitoul,strtoul,strtoul)
    #define _tcstol TextMapping(unitol,strtol,strtol)


	#define _ttoi TextMapping(unitoi,atoi,atoi)

	#define _itot TextMapping(uniitoa,itoa,itoa)

	#define _stprintf TextMapping(unisprintf,NWsprintf,NWsprintf)
	#define wsprintf NWsprintf

	#define _sntprintf TextMapping(unisnprintf,snprintf,snprintf)

// From LIBC's stdio.h with B and DML added (to avoid conflict with CLib)
int      snprintf ( char * restrict, size_t n, const char * restrictB, ... );
typedef char   *va_listLIBC;
#define _ARGWIDTHLIBC(n)       ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_startLIBC(ap, parm) (void) (ap = (va_listLIBC) &parm + _ARGWIDTHLIBC(parm))
#define va_endLIBC(ap)         (void) (ap = (va_listLIBC) 0)
#define va_argLIBC(ap, type)   (*(type *)((ap += _ARGWIDTHLIBC(type))-_ARGWIDTHLIBC(type)))
int      vsnprintf( char * restrict, size_t n, const char * restrictB, va_listLIBC args );

	#define _vstprintf TextMapping(univsprintf,NWvsprintf,NWvsprintf)

    #define _vsntprintf TextMapping(univsnprintf,_vsnprintf,_vsnprintf)
    int _vsnprintf( char* mbString, size_t bufferSize, const char* mbFormat, va_list args );

   #define _stscanf TextMapping(unisscanf,sscanf,sscanf)

    #define _tcsftime TextMapping(unistrftime,NWLstrftime,NWLstrftime)

	#define AnsiNext NWNextChar
	#define AnsiPrev NWPrevChar

	#define CharNext  NWNextChar
	#define CharNextA NWNextChar
    #define CharPrev(x, y) NWPrevChar(x, (LPTSTR) y)
	#define CharPrevA NWPrevChar

	#define OutputDebugString(s) printf("%s",s)

	// From RPC.H
	#define __RPC_FAR
	typedef long RPC_STATUS;
	#define RPCRTAPI
	#define RPC_ENTRY

	// From RpcDce.h
	RPCRTAPI
	RPC_STATUS
	RPC_ENTRY
	UuidToString (
		 IN UUID __RPC_FAR * Uuid,
		 OUT unsigned char __RPC_FAR * __RPC_FAR * StringUuid
		 );

	/* client/server */
	RPCRTAPI
	RPC_STATUS
	RPC_ENTRY
	UuidFromString (
		 IN unsigned char __RPC_FAR * StringUuid,
		 OUT UUID __RPC_FAR * Uuid
		 );

	RPCRTAPI
	RPC_STATUS
	RPC_ENTRY
	RpcStringFree (
		 IN OUT unsigned char __RPC_FAR * __RPC_FAR * String
		 );


	// From RpcNtErr.h
	#define RPC_S_OK                          ERROR_SUCCESS
	#define RPC_S_INVALID_ARG                 ERROR_INVALID_PARAMETER
	#define RPC_S_OUT_OF_MEMORY               ERROR_OUTOFMEMORY
	#define RPC_S_INVALID_LEVEL               ERROR_INVALID_PARAMETER
	#define RPC_S_BUFFER_TOO_SMALL            ERROR_INSUFFICIENT_BUFFER
	#define RPC_S_ACCESS_DENIED               ERROR_ACCESS_DENIED
	#define RPC_S_TIMEOUT                     ERROR_TIMEOUT

	#define HIWORD(l)   ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
	#define LOWORD(l)   ((WORD)(DWORD)l)
	#define LOBYTE(w)   ((BYTE)w)
	#define HIBYTE(w)   ((BYTE)w >> 8)

#endif // SYM_LIBC_NLM || NLM

char* PathAddBackslash(char* lpszPath);

char* mkWindowsPath(char* path);

#ifdef __cplusplus
}
#endif

#endif // _winStrings_h_
