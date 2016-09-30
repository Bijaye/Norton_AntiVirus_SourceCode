///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/CRegent.h_v   1.0   18 Jul 1997 11:23:26   JALLEE  $
//
//  CREGENT.H     - This file contains class definitions for the
//                  CRegistryEntry Class Objects.
//
///////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/CRegent.h_v  $
// 
//    Rev 1.0   18 Jul 1997 11:23:26   JALLEE
// Initial revision.
///////////////////////////////////////////////////////////////////////////

#ifndef _CREGENT_H_
#define _CREGENT_H_


#define REG_MAX_STR         264
#define ERR_INVALID_COUNT   0xFFFFFFFF



///////////////////////////////////////////////////////////////////////////
//  CRegistryEntry class definition
///////////////////////////////////////////////////////////////////////////

class CRegistryEntry
{
public:

                                        // Construction

    CRegistryEntry();
    CRegistryEntry( HKEY hKey, LPTSTR lpSubKey = NULL, LPTSTR lpValue = NULL );

                                        // Destruction

    ~CRegistryEntry();

                                        // User Interface

    void SetHive( HKEY hKey );
    void SetSubkey( LPTSTR lpSubKey );
    void SetValueName( LPTSTR lpValueName );
    void ResetKey( HKEY hKey, LPTSTR lpSubKey = NULL, LPTSTR lpValue = NULL );
    LONG OpenKey( DWORD dwAccess = KEY_ALL_ACCESS );
    LONG OpenCreateKey( DWORD dwAccess = KEY_ALL_ACCESS );
    LONG GetValue( LPDWORD lpdwType, LPBYTE lpBuffer, LPDWORD lpdwSize );
    LONG GetValueInfo( LPDWORD lpdwType, LPDWORD lpdwSize );
    LONG SetValue( DWORD dwType, LPBYTE lpData, DWORD dwSize );
    LONG DeleteValue( void );
    LONG DeleteAllValues( void );
    LONG IsKeyEmpty( void );
    DWORD GetSubkeyCount( void );
    DWORD GetValueCount( void );
    BOOL DeleteKey( BOOL bSubKeys = TRUE );

    BOOL IsOpen()
        { return( m_bOpen ); }

    void CloseKey( void );


protected:

    void InitMembers( void )
        { m_hHive = NULL;
          m_hKey = NULL;
          m_lpSubKey = NULL;
          m_lpValue = NULL;
          m_bOpen = FALSE; }

    LONG GetKeyInfo( LPDWORD lpdwKeys, LPDWORD lpdwValues );
    BOOL DeleteSubkeys( void );

                                        // Member variables

    HKEY            m_hHive;
    HKEY            m_hKey;
    BOOL            m_bOpen;
    LPTSTR          m_lpSubKey;
    LPTSTR          m_lpValue;
};


#endif                                  // ifndef _CREGSTR_H_



