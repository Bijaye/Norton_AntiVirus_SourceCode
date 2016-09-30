// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Intent: Definitions for Windows base data types.

#ifndef _winBaseTypes_h_
#define _winBaseTypes_h_

#if !(defined SYM_LIBC_NLM || defined LINUX)
    #include <nwthread.h>
    #include <nwadv.h>
#endif

#if defined(__cplusplus) && defined(SYM_LIBC_NLM)
extern "C"
{
#endif // defined(__cplusplus) && defined(SYM_LIBC_NLM)
#include <stdarg.h>
#if !defined(NLM) || defined(SYM_LIBC_NLM)
#include <wchar.h>
#endif // !defined(NLM) || defined(SYM_LIBC_NLM)
#if defined(__cplusplus) && defined(SYM_LIBC_NLM)
}
#endif // defined(__cplusplus) && defined(SYM_LIBC_NLM)

#if defined(LINUX)
#include <stdint.h>
#include <sys/types.h>
#endif // defined(LINUX)

#if defined(__cplusplus) && defined(SYM_LIBC_NLM)
extern "C"
{
#endif // defined(__cplusplus) && defined(SYM_LIBC_NLM)
#include <dirent.h>
#if defined(__cplusplus) && defined(SYM_LIBC_NLM)
}
#endif // defined(__cplusplus) && defined(SYM_LIBC_NLM)


#ifdef __cplusplus
extern "C" {
#endif



//////////////////////////////////////////////////////////////////
#ifndef SYM_WINDOWS_TYPES_DEFINED
#define SYM_WINDOWS_TYPES_DEFINED


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

#if defined(LINUX)
#define LINUX_CHECK_PREDEFINED
#else
#undef LINUX_CHECK_PREDEFINED
#endif // defined(LINUX)

#ifdef LINUX_CHECK_PREDEFINED
#if defined(UINT)
#error "UINT already defined"
#define UINT double
#endif

#if defined(BYTE)
#error "BYTE already defined"
#define BYTE double
#endif

#if defined(BOOL)
#error "BOOL already defined"
#define BOOL double
#endif

#if defined(WORD)
#error "WORD already defined"
#define WORD double
#endif

#undef DWORD
#if defined(DWORD)
#error "DWORD already defined"
#define DWORD uint64_t
#endif

#if defined(LONG)
#error "LONG already defined"
#define LONG char
#endif

#if defined(ULONG)
#error "ULONG already defined"
#define ULONG char
#endif

#if defined(TCHAR)
#error "TCHAR already defined"
#define TCHAR double
#endif


#endif // LINUX_CHECK_PREDEFINED


#ifndef VOID
#define VOID void
#endif

#undef BOOL
typedef int BOOL;
typedef BOOL* LPBOOL;

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#if !defined(NLM)
#define CONST const

#define NO_WCHAR_T
#undef HAS_WCHAR_T

typedef wchar_t WCHAR;
typedef CONST WCHAR *LPCWSTR;
typedef WCHAR *LPWSTR;
typedef CONST WCHAR *PCWSTR;
typedef WCHAR *PWSTR;
typedef WCHAR *PWCHAR;
#endif // !defined(NLM)

#if defined(NLM) && !defined(LINUX)
//conflicts with Netware's ws2defs.h
#ifndef __ws2defs_h__
typedef char CHAR;
#endif // __ws2defs_h__
#else
#define _CHAR
typedef char CHAR;
#endif // defined(NLM) && !defined(LINUX)

// Note: types here down using 'char' should use 'CHAR', except for the potential conflict with Netware

typedef const char* LPCSTR;
typedef const char* LPCTSTR;

typedef char *PSTR;
typedef char *LPSTR;

//---------------------------------
#if defined(LINUX)

#ifndef _TCHAR_DEFINED
	#define _TCHAR_DEFINED
#endif

#define STR(s) ((s) ? (s) : "(null)")

/* #ifndef TCHAR */
/* 	#define TCHAR TextMapping(wchar_t,char,char) */
/* #endif */
#if defined _UNICODE
    #error "Not yet supported"
#elif defined _MBCS
  typedef char TCHAR;
#else
  typedef char TCHAR;
#endif

#endif // defined(LINUX)
//---------------------------------


#ifdef UNICODE
typedef LPWSTR LPTSTR;
#else
typedef LPSTR LPTSTR;
#endif //  UNICODE

#if !defined(NLM) || defined(SYM_LIBC_NLM)
typedef unsigned char BYTE;							// 8 bits
#endif // defined(SYM_LIBC_NLM) || !defined(NLM)

#if !defined(LINUX) || !defined(LINUX_DEFINED_WORD)
#if !defined(NLM) || defined(SYM_LIBC_NLM)
typedef unsigned short WORD;						// 16 bits
#endif // !defined(NLM) || defined(SYM_LIBC_NLM)
#ifdef LINUX
#define LINUX_DEFINED_WORD
#endif
#endif

#if !defined(LINUX) || !defined(LINUX_DEFINED_DWORD)
typedef unsigned long DWORD;						// 32 bits
#ifdef LINUX
#define LINUX_DEFINED_DWORD
#endif
#endif

#if defined NLM || defined SYM_LIBC_NLM
   #ifndef __int64
      #define __int64 long long
   #endif
#endif // NLM

#if defined(LINUX)
  typedef uint64_t ULONGLONG;
#elif !defined(_M_IX86)
typedef unsigned __int64 ULONGLONG;
#else
typedef double ULONGLONG
#endif // defined(LINUX)

typedef ULONGLONG DWORDLONG;

#if !defined(NLM) || defined(SYM_LIBC_NLM)
// Not sure why this is unsigned on Netware when it's signed on Windows. jjm 11/29/2004
typedef unsigned long LONG;							// 32 bits
#endif // !defined(NLM) || defined(SYM_LIBC_NLM)

typedef unsigned long ULONG;						// 32 bits

typedef unsigned short USHORT;

// NOTE:  LONG is currently incorrectly defined as unsigned for NetWare
typedef long HRESULT;

typedef long LARGE_INTEGER;

typedef int INT;
typedef unsigned int UINT;

typedef WORD *PWORD;
typedef WORD FAR *LPWORD;

typedef DWORD *PDWORD;
typedef DWORD FAR *LPDWORD;

typedef BYTE *PBYTE;
#ifndef _LPBYTE
#define _LPBYTE
typedef BYTE FAR *LPBYTE;
#endif


#undef PVOID
#undef LPVOID
#undef HANDLE

typedef VOID * PVOID;
typedef VOID * LPVOID;

typedef PVOID HANDLE;

typedef HANDLE HKEY;
typedef HKEY *PHKEY;

typedef HANDLE * PHANDLE;
typedef HANDLE * LPHANDLE;


#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct
{
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;
#ifdef NLM
typedef unsigned char   nuint8;
typedef struct
{
	nuint8						 GUID[16];
} NW_GUID;
#endif
#endif // GUID_DEFINED
typedef GUID UUID;
typedef GUID CLSID;
typedef GUID IID;

#if !defined(LINUX)
// Definition != MS definition because their definition is not portable (uses __uuidof)
GUID GUID_NULL = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0} };
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(-1))
#endif


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
#define ERROR_BAD_PATHNAME               161L
#define ERROR_BUSY                            170L
#define ERROR_ALREADY_EXISTS             183L
#define ERROR_MORE_DATA                  234L   // dderror
#define ERROR_NO_MORE_ITEMS               259L
#define ERROR_FUNCTION_FAILED                 1627L
#define ERROR_CREATE_FAILED              1631L

// from vpcommon.h
#define ERROR_MEMORY                   0x20000002
#define ERROR_OPEN_FAIL                0x2000000a

typedef LONG NTSTATUS;

typedef HANDLE HDROP;

#ifndef HIBYTE
#define HIBYTE(w) ((BYTE) (((WORD) (w) >> 8) & 0xFF))
#endif
#define HIWORD(l)   ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#define LOWORD(l)   ((WORD)(DWORD)l)
#define LOBYTE(w)   ((BYTE)w)


#endif // SYM_WINDOWS_TYPES_DEFINED
//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
#if defined NLM

#define MAX_DEBUGPRINT_STR			1024
#define MAX_PATH						 255

#ifdef SYM_LIBC_NLM
typedef const wchar_t* LPCWSTR;
typedef wchar_t *LPWSTR;
#endif

#ifndef __ws2defs_h__
typedef char CHAR;	//conflicts with scssock.h
#endif

#define INVALID_HANDLE_VALUE 0xffffffff

#ifndef _PLATNLM_H
	typedef LONG* LPLONG;
	typedef LPLONG LPSECURITY_ATTRIBUTES;

	typedef DWORD FILETIME;			  // this is smaller than the windows FILETIME

	typedef HANDLE HINSTANCE;
	typedef HINSTANCE HMODULE;
#endif // ! _PLATNLM_H

typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK REGSAM;

typedef void *PFILETIME;

typedef unsigned long MSIHANDLE;

#define LOAD_WITH_ALTERED_SEARCH_PATH   0x00000008

#define LoadLibraryEx(x,y,flags) LoadLibrary(x)
#define LoadLibrary(x) _LoadLibrary(x,1)
HANDLE _LoadLibrary(const char *szName, bool bLoad);

#define FreeLibrary(x) nlm_FreeLibrary(x)
void nlm_FreeLibrary(HANDLE hNLM);

#ifndef SYM_LIBC_NLM
#define GetProcAddress(x,y) ImportSymbol(GetNLMHandle(),y)
#endif

int WSAGetLastError();

typedef FILETIME *LPFILETIME;

#elif defined LINUX
//////////////////////////////////////////////////////////////////////////////////////

typedef LONG* LPLONG;

//typedef struct _FILETIME {
//	DWORD dwLowDateTime;
//	DWORD dwHighDateTime;
//} FILETIME;

typedef DWORD FILETIME;			  // this is smaller than the windows FILETIME


typedef FILETIME *LPFILETIME;
typedef FILETIME *PFILETIME;


#define HINSTANCE_DEFINED
typedef HANDLE HINSTANCE;
typedef HINSTANCE HMODULE;


// //depot/Tools/NDK0403/nwsdk/include/nlm/nwfattr.h

#ifndef _A_NORMAL
#define _A_NORMAL 0x00000000 // Normal (read/write) file
#endif
#ifndef _A_RDONLY
#define _A_RDONLY 0x00000001 // Read-only file
#endif
#ifndef _A_EXECUTE
#define _A_EXECUTE 0x00000008 // Execute only file
#endif
#ifndef _A_SUBDIR
#define _A_SUBDIR 0x00000010 // Subdirectory
#endif

#ifndef FA_NORMAL
#define FA_NORMAL 0
#endif

#if !defined(__RPC_FAR)
// From RPC.H
#define __RPC_FAR
typedef long RPC_STATUS;
#define RPCRTAPI
#define RPC_ENTRY


// From RpcDce.h
RPCRTAPI RPC_STATUS RPC_ENTRY UuidToString( IN UUID __RPC_FAR * Uuid,
					    OUT unsigned char __RPC_FAR * __RPC_FAR * StringUuid
					    );

/* client/server */
RPCRTAPI RPC_STATUS RPC_ENTRY UuidFromString( IN unsigned char __RPC_FAR * StringUuid,
					      OUT UUID __RPC_FAR * Uuid
					      );

RPCRTAPI RPC_STATUS RPC_ENTRY RpcStringFree( IN OUT unsigned char __RPC_FAR * __RPC_FAR * String );


// From RpcNtErr.h
#define RPC_S_OK                          ERROR_SUCCESS
#define RPC_S_INVALID_ARG                 ERROR_INVALID_PARAMETER
#define RPC_S_OUT_OF_MEMORY               ERROR_OUTOFMEMORY
#define RPC_S_INVALID_LEVEL               ERROR_INVALID_PARAMETER
#define RPC_S_BUFFER_TOO_SMALL            ERROR_INSUFFICIENT_BUFFER
#define RPC_S_ACCESS_DENIED               ERROR_ACCESS_DENIED
#define RPC_S_TIMEOUT                     ERROR_TIMEOUT

/* #define HIWORD(l)   ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF)) */
/* #define LOWORD(l)   ((WORD)(DWORD)l) */
/* #define LOBYTE(w)   ((BYTE)w) */
/* #define HIBYTE(w)   ((BYTE)w >> 8) */

#endif // ! __RPC_FAR


// Intercept FindFile and friends
#define CopyFile savCopyFile
#define FindClose savFindClose
#define FindFirstFile savFindFirstFile
#define FindNextFile savFindNextFile

#endif // NLM

#ifndef IMAX_PATH
#define IMAX_PATH 384
#endif

#ifndef STRUCT_WIN32_FIND_DATA_DEFINED
#define STRUCT_WIN32_FIND_DATA_DEFINED
typedef struct {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
	char cFileName[IMAX_PATH];  // porting from changelist 134119 from the trunk
	char cAlternateFileName[14];
	DIR *dir;
	} WIN32_FIND_DATA;
#define LPWIN32_FIND_DATA WIN32_FIND_DATA *
#endif // STRUCT_WIN32_FIND_DATA_DEFINED


DWORD GetLastError();
void  SetLastError(DWORD dwErrCode);

#ifndef NLM
#if defined(REGISTRY_CLIENT_IPC_EXPLICIT)
#define RegOpenKey				vpRegOpenKey
#define RegCreateKey			vpRegCreateKey
#define RegCreateKeyEx			vpRegCreateKeyEx
#define RegDeleteKey			vpRegDeleteKey
#define RegDeleteValue			vpRegDeleteValue
#define RegCloseKey				vpRegCloseKey
#define RegSetValueEx			vpRegSetValueEx
#define RegQueryValueEx			vpRegQueryValueEx
#define RegEnumValue			vpRegEnumValue
#define RegEnumKeyEx			vpRegEnumKeyEx
#define RegEnumKey				vpRegEnumKey
#define RegFlushKey				vpRegFlushKey
#define RegNotifyChangeKeyValue	vpRegNotifyChangeKeyValue
#define DuplicateHandle			vpDuplicateHandle
#define RegOpenKeyEx			vpRegOpenKeyEx
#else
#define RegOpenKey              ipcRegOpenKey
#define RegOpenKeyEx            ipcRegOpenKeyEx
#define RegCreateKey            ipcRegCreateKey
#define RegCreateKeyEx          ipcRegCreateKeyEx
#define RegDeleteKey            ipcRegDeleteKey
#define RegDeleteValue          ipcRegDeleteValue
#define RegCloseKey             ipcRegCloseKey
#define RegSetValueEx           ipcRegSetValueEx
#define RegQueryValueEx         ipcRegQueryValueEx
#define RegEnumValue            ipcRegEnumValue
#define RegEnumKeyEx            ipcRegEnumKeyEx
#define RegEnumKey              ipcRegEnumKey
#define RegFlushKey             ipcRegFlushKey
#define RegNotifyChangeKeyValue ipcRegNotifyChangeKeyValue
#define DuplicateHandle         ipcDuplicateHandle
#define RegOpenKeyEx            ipcRegOpenKeyEx
#endif //defined(REGISTRY_CLIENT_IPC_EXPLICIT)
#endif // NLM

// Types of data values:
#define REG_NONE 				( 0 )   // No value type
#define REG_SZ 					( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ 			( 2 )   // Unicode nul terminated string (with environment variable references)
#define REG_BINARY 				( 3 )   // Free form binary
#define REG_DWORD 				( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN	( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN	( 5 )   // 32-bit number
#define REG_LINK				( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ 			( 7 )   // Multiple Unicode strings
#define REG_NOTIFY_CHANGE_LAST_SET ( 100 )

#define HKEY_LOCAL_MACHINE          (( HKEY ) 0x80000002 )
#define HKEY_CURRENT_USER        (( HKEY ) 0x80000002 )

#if !defined(_UNICODE)
// TODO_LINUX This is backwards for now. Need to correct.
#define RegQueryValueExA RegQueryValueEx

#endif // !defined(_UNICODE)

DWORD RegCloseKey(HKEY hkey);
DWORD RegQueryValueEx(HKEY hkey,LPCSTR value,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD cbdata);

#if defined LINUX
LPTSTR CharLower(LPTSTR lpsz);


//#define StrRChar _StrRChar
#define StrRChr    NWLstrrchr
#ifndef StrRChar
#define StrRChar   NWLstrrchr
#endif
#define CharNext   NWNextChar
#define CharPrev   NWPrevChar
#define CharLower  NWLstrlwr

#ifndef StrChr
#define StrChr NWLstrchr
#endif

#if !defined(_UNICODE)
#define AnsiNext CharNext
#define AnsiPrev CharPrev
#endif // !defined(_UNICODE)

#endif // LINUX

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
