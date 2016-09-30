// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
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
#ifdef _USE_CCW
BOOL DECLSPEC_NAVNUTIL GetFileSecurityDescW( LPCWSTR lpszFileName, LPBYTE *pDesc );
BOOL DECLSPEC_NAVNUTIL SetFileSecurityDescW( LPWSTR lpszFileName, LPBYTE pDesc );
#endif
BOOL DECLSPEC_NAVNUTIL SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc );
BOOL DECLSPEC_NAVNUTIL FreeFileSecurityDesc( LPBYTE pDesc );

#ifdef __cplusplus
}
#endif

#endif
