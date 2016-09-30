// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// Simple type definitions, a la windows.h
//
// Provides platform dependent definitions for the platform independent
// types BOOL, BYTE, WORD, DWORD and miscellaneous other Windows types.
// In this case the platform is the Tenberry 16 bit DOS Extender.
//
//************************************************************************

#define NEAR               _near
#define FAR                _far
#define PASCAL             _pascal
#define CDECL              _cdecl
#define WINAPI             _far _pascal
#define CALLBACK           _far _pascal

#define FALSE              0
#define TRUE               1

#ifndef BOOL
typedef int                BOOL;       // 16-bit signed
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
typedef signed long        LONG;       // 32-bit
#endif

#ifndef UINT
typedef unsigned int       UINT;       // UINT floats with compiler
#endif

typedef BOOL FAR *         LPBOOL;
typedef void FAR *         LPVOID;
typedef BYTE FAR *         LPBYTE;
typedef WORD FAR *         LPWORD;
typedef DWORD FAR *        LPDWORD;
typedef long FAR *         LPLONG;
typedef UINT FAR *         LPUINT;
typedef int FAR *          LPINT;

typedef const char FAR *   LPCSTR;
typedef char FAR *         LPSTR;

typedef char               TCHAR;
typedef LPSTR              LPTSTR;
typedef LPCSTR             LPCTSTR;

#define _TCHAR_DEFINED

                           
