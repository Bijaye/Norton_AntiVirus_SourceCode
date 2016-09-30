// DecPlat.h : Decomposer platform-specific global includes
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

//
// Define 'bool' if needed
//
#if defined(NO_NATIVE_BOOL)
	typedef int bool;
	#define false 0
	#define true  1
#endif

#if defined(_WINDOWS)
//
// Windows
//
	#include <windows.h>
#elif defined(UNIX)
//
// Unix
//
	#if defined(SYM_FREEBSD)
		#if !defined(SYM_DARWIN)
			// Include curses.h to pick up wint_t definition.
			#include <curses.h>
		#endif
	#endif

	#if defined(SYM_DARWIN)
		// Mac OS X does not have localtime_r or gmtime_r functions.
		#define localtime_r(a,b)	localtime(a)
		#define gmtime_r(a,b)		gmtime(a)
		#define readdir_r(a,b,c)	readdir(a)

		// Since we can not use curses.h on Mac OS X due to STL conflicts, we must define
		// wint_t here.
		#ifndef wint_t
			typedef long int wint_t;
		#endif
	#endif
	#if !defined(SYM_NLM)
		#include "platunix.h"
	#endif
#elif defined(SYM_NLM)
//
// Netware
//
#if defined( SAV_CERTIFY) 
	#include <stdlib.h>
	#include <malloc.h>
#endif
	#include "platnlm.h"

#if defined( SAV_CERTIFY) 
	#define malloc(s) zalloc(s)
	#define realloc(o, s) xealloc(o, s)
	#define free(p) MyNLMFree(p, __FILE__, __LINE__)
	#define calloc(count,size)  yalloc( count, size )

#ifdef __cplusplus
extern "C"
{
#endif
	void FAR *zalloc( size_t size );
	void FAR *yalloc( int count, size_t size );
	void FAR *xealloc(void *old, int size);
	void MyNLMFree(void *p, char *file, int line);
#ifdef __cplusplus
}
#endif

	#define Pfree MyNLMFree

	#ifndef FALSE
	#define FALSE 0
	#endif
	#ifndef TRUE
	#define TRUE 1
	#endif
#endif

#else
//
// Unknown platform
//
	#error("Platform not supported!")
#endif

//
// Define any pointer-to-pointer types that are needed.  External clients
// who don't need these should define SYM_NO_LPLPxxxx.
//
#if !defined(SYM_NO_LPLPSTR)
	typedef char **LPLPSTR;
#endif

#if !defined(SYM_NO_LPLPVOID)
	typedef void **LPLPVOID;
#endif

#if !defined(SYM_NO_LPLPDWORD)
	typedef DWORD **LPLPDWORD;
#endif


//////////////////////////////////////////////////////////////////////
// Endianization definitions
//////////////////////////////////////////////////////////////////////

#if !defined(WENDIAN) || !defined(DWENDIAN)
#ifdef CPU_BIG_ENDIAN
#define WENDIAN(w) ((WORD)((WORD)(w) >> 8) | (WORD)((WORD)(w) << 8))
#define DWENDIAN(dw) ((DWORD)((DWORD)(dw) >> 24) | (DWORD)((DWORD)(dw) << 24) \
	| (DWORD)(((DWORD)(dw) >> 8) & 0xFF00U) | (DWORD)(((DWORD)(dw) << 8) & \
	0xFF0000UL))
#else
#define WENDIAN(w) (w)
#define DWENDIAN(dw) (dw)
#endif
#endif
