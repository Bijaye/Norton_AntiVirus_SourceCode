///////////////////////////////////////////////////////////////////////////
// Copyright (c)1996,1997 - Bruce Hellstrom, All rights reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/IsRegUti.h_v   1.0   14 May 1997 17:11:22   jtaylor  $
//
//  CWSTRING.CPP - This file contains member functions for the CWString
//                 class
//
///////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/IsRegUti.h_v  $
// 
//    Rev 1.0   14 May 1997 17:11:22   jtaylor
// Initial revision.
// 
//    Rev 1.0   05 May 1997 19:26:04   sedward
// Initial revision.
//
///////////////////////////////////////////////////////////////////////////

#ifndef IS_REG_UTIL_HEADER
#define IS_REG_UTIL_HEADER


/////////////////////////////////////////////////////////////////////////////
//  Function prototypes from ISRegUtil.CPP
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI IRegDBDeleteKey( HKEY hHive, LPTSTR lpSubKey );

BOOL WINAPI IRegDBDeleteValue( HKEY hKey, LPTSTR lpSubkey, LPTSTR lpValue );

BOOL WINAPI IRegDBSetValue( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue,
                            DWORD dwType, LPBYTE lpData, int nDataSize );

BOOL WINAPI IRegDBGetValue( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue,
                            LPDWORD lpdwType, LPBYTE *lplpData, LPDWORD lpdwDataSize );

BOOL WINAPI IRegDBGetKeyInfo( HKEY hKey, LPSTR lpszSubKey, LPDWORD lpdwKeys,
                              LPDWORD lpdwValues );

#endif  // IS_REG_UTIL_HEADER