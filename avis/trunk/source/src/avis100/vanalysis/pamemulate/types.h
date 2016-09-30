#ifndef _TYPES_H

#define _TYPES_H
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned int    UINT;
typedef unsigned long	DWORD;
typedef int             BOOL;
typedef WORD            *LPWORD;
typedef long            LONG;
typedef char            *LPSTR;
typedef char            *LPTSTR;
typedef BYTE            *LPBYTE;
typedef void            *LPVOID;
typedef long            *LPLONG;

typedef FILE            *HFILE;

#define FAR             far

#define READ_ONLY_FILE      0
#define READ_WRITE_FILE     2

#define SYM_MAX_PATH        256

#endif
