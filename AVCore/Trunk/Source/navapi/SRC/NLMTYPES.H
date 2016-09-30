// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// Simple type definitions, a la windows.h
//
// Provides platform dependent definitions for the platform independent
// types BOOL, BYTE, WORD, DWORD and miscellaneous other Windows types.
// In this case the platform is NetWare 386 and WATCOM C/386
//
//************************************************************************

#define NEAR
#define FAR
#define PASCAL             _pascal
#define CDECL              _cdecl
#define WINAPI             _pascal
#define CALLBACK           _pascal

#define FALSE              0
#define TRUE               1

#ifndef BOOL
typedef short int          BOOL;       // 16-bit signed
#endif

#ifndef VOID
typedef void               VOID;
#endif

#ifndef BYTE
typedef unsigned char      BYTE;       // 8-bit
#endif

#ifndef WORD
typedef unsigned short int WORD;       // 16-bit
#endif

#ifndef DWORD
typedef unsigned long int  DWORD;      // 32-bit
#endif

#ifndef LONG
typedef signed long	      LONG;       // 32-bit
#endif

#ifndef UINT
typedef unsigned int       UINT;       // UINT floats with compiler
#endif

typedef BOOL *             LPBOOL;
typedef void *             LPVOID;
typedef BYTE *             LPBYTE;
typedef WORD *             LPWORD;
typedef DWORD *            LPDWORD;
typedef long *             LPLONG;
typedef UINT *             LPUINT;
typedef int *              LPINT;

typedef const char *       LPCSTR;
typedef char *             LPSTR;

typedef char               TCHAR;
typedef LPSTR              LPTSTR;
typedef LPCSTR             LPCTSTR;

#define _TCHAR_DEFINED

