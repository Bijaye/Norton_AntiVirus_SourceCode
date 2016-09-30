//************************************************************************
//
// $Header:   $
//
// Description:
//
//************************************************************************
// $Log:   $
// 
//************************************************************************

#ifndef _PLATFORMH_

#define _PLATFORMH_

#include <stdio.h>
#include <stdlib.h>

#define FAR

typedef int             BOOL;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef signed char     SBYTE;
typedef signed short    SWORD;
typedef signed long     SDWORD;
typedef unsigned int    UINT;
typedef char            TCHAR;
typedef long            LONG;
typedef WORD FAR        *LPWORD;
typedef int FAR         *LPINT;
typedef char FAR        *LPSTR;
typedef char FAR        *LPTSTR;
typedef const LPTSTR    LPCTSTR;
typedef BYTE FAR        *LPBYTE;
typedef void FAR        *LPVOID;
typedef DWORD FAR       *LPDWORD;
typedef BOOL FAR        *LPBOOL;
typedef long FAR        *LPLONG;
typedef LPSTR FAR       *LPLPSTR;
typedef void FAR        *LPVOID;

typedef LPBYTE FAR      *LPLPBYTE;
typedef LPVOID FAR      *LPLPVOID;

#define FALSE			0
#define TRUE			1

#endif

