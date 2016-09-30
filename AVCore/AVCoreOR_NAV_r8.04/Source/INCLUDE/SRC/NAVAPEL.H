#if !defined(NAVAPEL_H)
#define NAVAPEL_H

typedef BOOL ( *fnAddCurrentThreadToPEL1 ) ( VOID );
typedef BOOL ( *fnForceCurrentThreadIntoPEL1 ) ( VOID );
typedef BOOL ( *fnIsCurrentThreadInPEL1 ) ( VOID );
typedef VOID ( *fnDeleteCurrentThreadFromPEL1 ) ( VOID );
typedef BOOL ( *fnAddCurrentProcessToPEL2 ) ( VOID );
typedef int  ( *fnGetCurrentProcessPEL2Count ) ( VOID );
typedef VOID ( *fnDeleteCurrentProcessFromPEL2 ) ( VOID );

#include <pshpack1.h>

typedef struct
{
    DWORD                          dwRev;
    fnAddCurrentThreadToPEL1       pAddCurrentThreadToPEL1;
    fnForceCurrentThreadIntoPEL1   pForceCurrentThreadIntoPEL1;
    fnIsCurrentThreadInPEL1        pIsCurrentThreadInPEL1;
    fnDeleteCurrentThreadFromPEL1  pDeleteCurrentThreadFromPEL1;
    fnAddCurrentProcessToPEL2      pAddCurrentProcessToPEL2;
    fnGetCurrentProcessPEL2Count   pGetCurrentProcessPEL2Count;
    fnDeleteCurrentProcessFromPEL2 pDeleteCurrentProcessFromPEL2;
} NAVAPEL_INTERFACE, *PNAVAPEL_INTERFACE;

#include <poppack.h>

typedef BOOL ( __cdecl *fnQueryNAVAPELInterface ) ( PNAVAPEL_INTERFACE pInterface );

#if defined(__cplusplus)
extern "C" {
#endif

BOOL __cdecl NAVAPELQueryInterface (
    PNAVAPEL_INTERFACE pInterface
    );

#if defined(__cplusplus)
}
#endif

#endif