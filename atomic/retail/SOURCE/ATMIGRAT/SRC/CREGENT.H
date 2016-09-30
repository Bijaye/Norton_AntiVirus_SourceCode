/////////////////////////////////////////////////////////////////////
// File: CRegent.h
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/ATMIGRAT/VCS/CREGENT.H_v   1.0   26 May 1998 14:36:08   SEDWARD  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/CREGENT.H_v  $
// 
//    Rev 1.0   26 May 1998 14:36:08   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


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

    // SCOTT
    HKEY GetCurrentHive( void )  { return  m_hHive; }
    HKEY GetCurrentKey( void )  { return  m_hKey; }
    LPTSTR GetCurrentSubKeyName( void )  { return  m_lpSubKey; }

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


VOID    CopyRegKey(CRegistryEntry*  oldKey, CRegistryEntry*  newKey);
DWORD   CopyRegkeyValues(CRegistryEntry*  oldKey, CRegistryEntry*  newKey);
VOID    RenameRegKey(CRegistryEntry*  oldKey, CRegistryEntry*  newKey);


#endif                                  // ifndef _CREGSTR_H_

