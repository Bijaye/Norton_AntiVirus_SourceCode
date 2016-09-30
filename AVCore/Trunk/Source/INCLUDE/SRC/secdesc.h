////////////////////////////////////////////////////////////////////////////
// $Header:  $
////////////////////////////////////////////////////////////////////////////
//
// SecDesc.h - 
//
////////////////////////////////////////////////////////////////////////////
// $Log:  $
// 
////////////////////////////////////////////////////////////////////////////

#include <windows.h>


#ifndef _SECDESC_H_
#define _SECDESC_H_

#ifdef BUILDING_NAVNUTIL
#define DECLSPEC_NAVNUTIL __declspec( dllexport )
#else
#define DECLSPEC_NAVNUTIL __declspec( dllimport )
#endif

#ifdef __cplusplus
extern "C" {
#endif

BOOL DECLSPEC_NAVNUTIL GetFileSecurityDesc( LPSTR lpszFileName, LPBYTE *pDesc );
BOOL DECLSPEC_NAVNUTIL SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc );
BOOL DECLSPEC_NAVNUTIL FreeFileSecurityDesc( LPBYTE pDesc );

#ifdef __cplusplus
}
#endif

#endif
