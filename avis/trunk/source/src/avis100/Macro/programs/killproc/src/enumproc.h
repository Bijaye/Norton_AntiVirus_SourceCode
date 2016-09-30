/*********************
 file: EnumProc.h
*********************/

#include <windows.h>

typedef BOOL (CALLBACK *PROCENUMPROC)( DWORD, DWORD, LPSTR,
      LPARAM ) ;

BOOL WINAPI EnumProcs( PROCENUMPROC lpProc, LPARAM lParam ) ;


typedef struct
{
      DWORD          dwPID ;
      PROCENUMPROC   lpProc ;
      DWORD          lParam ;
      BOOL           bEnd ;
} EnumInfoStruct ;

