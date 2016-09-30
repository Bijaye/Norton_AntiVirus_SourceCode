// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Intent: Definitions for Windows base data types.

#ifndef _winBaseTypes_h_
#define _winBaseTypes_h_

#ifndef SYM_LIBC_NLM
    #include <nwthread.h>
    #include <nwadv.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#ifdef NLM

#define MAX_DEBUGPRINT_STR			1024
#define MAX_PATH						 255

#define __stdcall
#define  _stdcall
#define __cdecl
#define __fastcall

#define FAR
#define APIENTRY
#define WINAPI
#define WINADVAPI
#define PASCAL
#define IN
#define OUT

#ifndef VOID
#define VOID void
#endif

#ifndef BOOL
#define BOOL  int
#endif

typedef BOOL* LPBOOL;

typedef const char* LPCSTR;
typedef const char* LPCTSTR;
typedef char* PSTR;
typedef char* LPSTR;
typedef char* LPTSTR;

#ifdef SYM_LIBC_NLM
typedef const wchar_t* LPCWSTR;
typedef wchar_t *LPWSTR;
#endif

#define STR(s) ((s) ? (s) : "(null)")

#ifndef __ws2defs_h__
typedef char CHAR;	//conflicts with scssock.h
#endif

#ifndef BYTE
#define BYTE unsigned char								//	 8 bits
#endif

#ifndef WORD
#define WORD unsigned short							// 16 bits
#endif

#ifndef DWORD
#define DWORD unsigned long							// 32 bits
#endif

// Not sure why this is unsigned on Netware when it's signed on Windows. jjm 11/29/2004
#ifndef LONG
#define LONG unsigned long
#endif

#ifndef ULONG
typedef unsigned long ULONG;
#endif

typedef long LARGE_INTEGER;

typedef int INT;
typedef unsigned int UINT;

typedef DWORD *PDWORD;
typedef DWORD FAR *LPDWORD;

typedef BYTE *PBYTE;
#ifndef _LPBYTE
typedef BYTE FAR *LPBYTE;
#endif

typedef DWORD HKEY;
typedef HKEY *PHKEY;

#ifndef HANDLE						  // Note that `HANDLE' is defined as `void*' in Windows
#define HANDLE long
#endif

typedef HANDLE * PHANDLE;
typedef HANDLE * LPHANDLE;

#define INVALID_HANDLE_VALUE 0xffffffff

#ifndef _PLATNLM_H
	typedef LONG* LPLONG;
	typedef LPLONG LPSECURITY_ATTRIBUTES;

	typedef DWORD FILETIME;			  // this is smaller than the windows FILETIME

	typedef HANDLE HINSTANCE;
	typedef HINSTANCE HMODULE;
#endif // ! _PLATNLM_H

typedef void *PFILETIME;

typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK REGSAM;

typedef void * PVOID;
typedef void * LPVOID;
typedef LONG NTSTATUS;
typedef HANDLE HDROP;

typedef unsigned long MSIHANDLE;

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct
{
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;
#endif
typedef GUID UUID;
typedef GUID CLSID;
typedef GUID IID;

// Definition != MS definition because their definition is not portable (uses __uuidof)
GUID GUID_NULL = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0} };

// /usr/include/wine/windows/winerror.h

#define STATUS_PENDING 0x00000103L
#define STILL_ACTIVE	  STATUS_PENDING

#define WAIT_OBJECT_0				0x00000000UL
#define WAIT_TIMEOUT					0x00000102UL
#define WAIT_ABANDONED				0x00000080UL
#define WAIT_FAILED					0xFFFFFFFFUL
#define INFINITE						0xFFFFFFFFUL

// The following are from WINERROR.H, for NetWare
#define NO_ERROR                         0L    // dderror
#define ERROR_SUCCESS                         0L
#define ERROR_INVALID_FUNCTION           1L    // dderror
#define ERROR_FILE_NOT_FOUND             2L
#define ERROR_PATH_NOT_FOUND             3L
#define ERROR_TOO_MANY_OPEN_FILES        4L
#define ERROR_INVALID_HANDLE             6L
#define ERROR_NOT_ENOUGH_MEMORY           8L
#define ERROR_INVALID_DATA               13L
#define ERROR_OUTOFMEMORY                14L
#define ERROR_OUT_OF_PAPER               28L
#define ERROR_CANNOT_MAKE                82L
#define ERROR_INVALID_PARAMETER          87L    // dderror
#define ERROR_OPEN_FAILED                110L
#define ERROR_BUFFER_OVERFLOW            111L
#define ERROR_INSUFFICIENT_BUFFER        122L    // dderror
#define ERROR_DIR_NOT_EMPTY              145L
#define ERROR_BUSY                            170L
#define ERROR_ALREADY_EXISTS             183L
#define ERROR_MORE_DATA                  234L   // dderror
#define ERROR_NO_MORE_ITEMS               259L
#define ERROR_FUNCTION_FAILED                 1627L
#define ERROR_CREATE_FAILED              1631L

#define LOAD_WITH_ALTERED_SEARCH_PATH   0x00000008

#define LoadLibraryEx(x,y,flags) LoadLibrary(x)
#define LoadLibrary(x) _LoadLibrary(x,1)
HANDLE _LoadLibrary(const char *szName, bool bLoad);

#define FreeLibrary(x) nlm_FreeLibrary(x)
void nlm_FreeLibrary(HANDLE hNLM);

#ifndef SYM_LIBC_NLM
#define GetProcAddress(x,y) ImportSymbol(GetNLMHandle(),y)
#endif

DWORD GetLastError();
void  SetLastError(DWORD dwErrCode);

int WSAGetLastError();

#endif // NLM

//
// These types are platforms specific since Windows uses "HANDLE" for many purposes, yet
// the implementation of all functions that use HANDLEs may need to treat HANDLE as either
// a pointer type (e.g. void*) or a scalar type (e.g. LONG).
//
#ifdef _WINDOWS
typedef HANDLE MUTEX;
#elif defined(NLM)
typedef HANDLE MUTEX;
#endif

#ifdef __cplusplus
}
#endif

#endif // _winBaseTypes_h_
