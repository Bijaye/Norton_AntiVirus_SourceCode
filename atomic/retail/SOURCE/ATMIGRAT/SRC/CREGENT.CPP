/////////////////////////////////////////////////////////////////////
// File: CRegent.cpp
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGRAT/VCS/CREGENT.CPv   1.0   26 May 1998 14:31:14   SEDWARD  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/CREGENT.CPv  $
// 
//    Rev 1.0   26 May 1998 14:31:14   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include <windows.h>

#include "cregent.h"


#ifndef _WIN32
#error This implementation of CRegEnt requires Win32
#endif


///////////////////////////////////////////////////////////////////////////
// Constructors/Destructors
///////////////////////////////////////////////////////////////////////////

CRegistryEntry::CRegistryEntry()
{
    InitMembers();
    return;
}


CRegistryEntry::CRegistryEntry( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue )
{
    InitMembers();
    m_hHive = hKey;

    if ( lpSubKey != NULL )
    {
        m_lpSubKey = new TCHAR[ lstrlen( lpSubKey ) + 1 ];
        lstrcpy( m_lpSubKey, lpSubKey );
    }

    if ( lpValue != NULL )
    {
        m_lpValue = new TCHAR[ lstrlen( lpValue ) + 1 ];
        lstrcpy( m_lpValue, lpValue );
    }

    return;
}


CRegistryEntry::~CRegistryEntry()
{
    if ( IsOpen() )
        CloseKey();

    if ( m_lpSubKey != NULL )
        delete m_lpSubKey;

    if ( m_lpValue != NULL )
        delete m_lpValue;

    return;
}



///////////////////////////////////////////////////////////////////////////
// Member functions
///////////////////////////////////////////////////////////////////////////

void CRegistryEntry::SetHive( HKEY hKey )
{
    if ( IsOpen() )
        CloseKey();

    m_hHive = hKey;

    return;
}



void CRegistryEntry::SetSubkey( LPTSTR lpSubKey )
{
    if ( IsOpen() )
        CloseKey();

    if ( m_lpSubKey != NULL )
    {
        delete m_lpSubKey;
        m_lpSubKey = NULL;
    }

    if ( lpSubKey != NULL )
    {
        m_lpSubKey = new TCHAR[ lstrlen( lpSubKey ) + 1 ];
        lstrcpy( m_lpSubKey, lpSubKey );
    }

    return;
}



void CRegistryEntry::SetValueName( LPTSTR lpValueName )
{
    if ( m_lpValue != NULL )
    {
        delete m_lpValue;
        m_lpValue = NULL;
    }

    if ( lpValueName != NULL )
    {
        m_lpValue = new TCHAR[ lstrlen( lpValueName ) + 1 ];
        lstrcpy( m_lpValue, lpValueName );
    }


    return;
}



void CRegistryEntry::ResetKey( HKEY hKey, LPTSTR lpSubKey, LPTSTR lpValue )
{
    if ( IsOpen() )
        CloseKey();

    m_hHive = hKey;

    if ( m_lpSubKey != NULL )
    {
        delete m_lpSubKey;
        m_lpSubKey = NULL;
    }

    if ( lpSubKey != NULL )
    {
        m_lpSubKey = new TCHAR[ lstrlen( lpSubKey ) + 1 ];
        lstrcpy( m_lpSubKey, lpSubKey );
    }

    if ( m_lpValue != NULL )
    {
        delete m_lpValue;
        m_lpValue = NULL;
    }

    if ( lpValue != NULL )
    {
        m_lpValue = new TCHAR[ lstrlen( lpValue ) + 1 ];
        lstrcpy( m_lpValue, lpValue );
    }

    return;
}



LONG CRegistryEntry::OpenKey( DWORD dwAccess )
{
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( m_hHive != NULL && m_lpSubKey != NULL )
    {
        lReturn = RegOpenKeyEx( m_hHive, m_lpSubKey, 0, dwAccess, &m_hKey );
    }

    if ( lReturn == ERROR_SUCCESS && m_hKey != NULL )
        m_bOpen = TRUE;
    else
    {
        m_hKey = NULL;
        m_bOpen = FALSE;
    }

    return( lReturn );
}



LONG CRegistryEntry::OpenCreateKey( DWORD dwAccess )
{
    LONG            lReturn = ERROR_INVALID_DATA;
    DWORD           dwDisp = 0;

    if ( m_hHive != NULL && m_lpSubKey != NULL )
    {
        lReturn = RegCreateKeyEx( m_hHive, m_lpSubKey, 0, NULL,
                                  REG_OPTION_NON_VOLATILE,
                                  dwAccess, NULL, &m_hKey, &dwDisp );
    }

    if ( lReturn == ERROR_SUCCESS && m_hKey != NULL )
        m_bOpen = TRUE;
    else
    {
        m_hKey = NULL;
        m_bOpen = FALSE;
    }

    return( lReturn );
}



LONG CRegistryEntry::GetValue( LPDWORD lpdwType, LPBYTE lpBuffer, LPDWORD lpdwSize )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ );
        bClose = TRUE;
    }

    lReturn = RegQueryValueEx( m_hKey, m_lpValue, NULL,
                               lpdwType, lpBuffer, lpdwSize );

    if ( bClose )
        CloseKey();

    return( lReturn );
}



LONG CRegistryEntry::GetValueInfo( LPDWORD lpdwType, LPDWORD lpdwSize )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ );
        bClose = TRUE;
    }

    if ( lpdwType != NULL && lpdwSize != NULL )
        lReturn = RegQueryValueEx( m_hKey, m_lpValue, NULL,
                                   lpdwType, NULL, lpdwSize );

    if ( bClose )
        CloseKey();

    return( lReturn );
}



LONG CRegistryEntry::SetValue( DWORD dwType, LPBYTE lpData, DWORD dwSize )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenCreateKey( KEY_WRITE );
        bClose = TRUE;
    }

    if ( lpData != NULL )
        lReturn = RegSetValueEx( m_hKey, m_lpValue, 0, dwType, lpData, dwSize );

    if ( bClose )
        CloseKey();

    return( lReturn );
}


LONG CRegistryEntry::DeleteValue( void )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;

    if ( !IsOpen() )
    {
        OpenKey( KEY_WRITE );
        bClose = TRUE;
    }

    lReturn = RegDeleteValue( m_hKey, m_lpValue );

    if ( bClose )
        CloseKey();

    return( lReturn );
}


LONG CRegistryEntry::DeleteAllValues( void )
{
    BOOL            bClose = FALSE;
    LONG            lReturn = ERROR_INVALID_DATA;
    DWORD           dwIndex = 0;
    DWORD           dwSize = REG_MAX_STR;
    TCHAR           szValueName[ REG_MAX_STR ];

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ | KEY_WRITE );
        bClose = TRUE;
    }

    while( RegEnumValue( m_hKey, dwIndex, szValueName, &dwSize, NULL,
                         NULL, NULL, NULL ) == ERROR_SUCCESS )
    {
        RegDeleteValue( m_hKey, szValueName );
        dwIndex++;
        dwSize = REG_MAX_STR;
        lReturn = ERROR_SUCCESS;
    }

    if ( bClose )
        CloseKey();

    return( lReturn );
}



LONG CRegistryEntry::IsKeyEmpty( void )
{
    LONG        lRet = FALSE;
    DWORD       dwSubKeys = 0;
    DWORD       dwValues = 0;

    lRet = GetKeyInfo( &dwSubKeys, &dwValues );

    if ( lRet == ERROR_SUCCESS )
        lRet = !dwSubKeys && !dwValues;

    return( lRet );
}



DWORD CRegistryEntry::GetSubkeyCount( void )
{
    DWORD       dwReturn = ERR_INVALID_COUNT;
    DWORD       dwSubKeys = 0;
    DWORD       dwValues = 0;

    if ( GetKeyInfo( &dwSubKeys, &dwValues ) == ERROR_SUCCESS )
        dwReturn = dwSubKeys;

    return( dwReturn );
}



DWORD CRegistryEntry::GetValueCount( void )
{
    DWORD       dwReturn = ERR_INVALID_COUNT;
    DWORD       dwSubKeys = 0;
    DWORD       dwValues = 0;

    if ( GetKeyInfo( &dwSubKeys, &dwValues ) == ERROR_SUCCESS )
        dwReturn = dwValues;

    return( dwReturn );
}



BOOL CRegistryEntry::DeleteKey( BOOL bSubKeys )
{
    BOOL            bRet = FALSE;
    BOOL            bWinNT = FALSE;
    OSVERSIONINFO   osInfo;

    memset( &osInfo, 0, sizeof( OSVERSIONINFO ) );
    osInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

    GetVersionEx( &osInfo );

    if ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
        bWinNT = TRUE;

    if ( m_lpSubKey && bWinNT && bSubKeys )
        DeleteSubkeys();

    if ( IsOpen() )
        CloseKey();

    RegDeleteKey( m_hHive, m_lpSubKey );
    bRet = TRUE;

    return( bRet );
}



void CRegistryEntry::CloseKey( void )
{
    if ( m_hKey != NULL )
    {
        RegCloseKey( m_hKey );
        m_hKey = NULL;
        m_bOpen = FALSE;
    }

    return;
}



BOOL CRegistryEntry::DeleteSubkeys( void )
{
    BOOL                bClose = FALSE;
    BOOL                bRet = FALSE;
    DWORD               dwIndex = 0;
    DWORD               dwKeySize = REG_MAX_STR - 1;
    LPTSTR              lpSubKey;
    FILETIME            ftWriteTime;
    CRegistryEntry     *pCRegEnt;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ | KEY_WRITE );
        bClose = TRUE;
    }

    if ( m_lpSubKey != NULL )
    {
        lpSubKey = new TCHAR[ REG_MAX_STR ];

        while( RegEnumKeyEx( m_hKey, dwIndex, lpSubKey, &dwKeySize,
                             NULL, NULL, NULL, &ftWriteTime) == ERROR_SUCCESS)
        {
            pCRegEnt = new CRegistryEntry( m_hKey, lpSubKey );
            pCRegEnt->DeleteKey();
            delete pCRegEnt;
            dwKeySize = REG_MAX_STR - 1;
        }

        bRet = TRUE;
        delete lpSubKey;
    }

    if ( bClose )
        CloseKey();

    return( bRet );
}


LONG CRegistryEntry::GetKeyInfo( LPDWORD lpdwKeys, LPDWORD lpdwValues )
{
    BOOL        bClose = FALSE;
    LONG        lReturn = ERROR_INVALID_DATA;
    TCHAR       szClass[ REG_MAX_STR ];
    DWORD       dwClass = 0;
    DWORD       dwMaxSubkey = 0;
    DWORD       dwMaxClass = 0;
    DWORD       dwMaxValueName = 0;
    DWORD       dwMaxValueData = 0;
    DWORD       dwSecurityDescriptor = 0;
    FILETIME    ftWriteTime;

    if ( !IsOpen() )
    {
        OpenKey( KEY_READ );
        bClose = TRUE;
    }

    dwClass = REG_MAX_STR;

    lReturn = RegQueryInfoKey( m_hKey, szClass, &dwClass,
                               NULL, lpdwKeys, &dwMaxSubkey,
                               &dwMaxClass,
                               lpdwValues, &dwMaxValueName, &dwMaxValueData,
                               &dwSecurityDescriptor, &ftWriteTime );

    if ( bClose )
        CloseKey();

    return( lReturn );
}




// ==== RenameRegKey ======================================================
//
//  This function renames a registry key by first copying (along with all
//  its subkeys and values) to the new location, then deleting the
//  original key.
//
//  Input:
//      oldKeyPtr   -- a pointer to a CRegistryEntry object; the subkey
//                     member should contain the "source" registry branch
//
//      newKeyPtr   -- a pointer to a CRegistryEntry object; the subkey
//                     member should contain the "destination" registry branch
//
//  Output:
//      nothing
//
// ========================================================================
//  Created by SEDWARD, 11/16/97
// ========================================================================

VOID    RenameRegKey(CRegistryEntry*  oldKeyPtr, CRegistryEntry*  newKeyPtr)
{
    CopyRegKey(oldKeyPtr, newKeyPtr);
    oldKeyPtr->DeleteKey(TRUE);

}  // end of "RenameRegKey"



// ==== CopyRegKey ========================================================
//
//  This recursive function copies the entire contents of a registry key.
//  The target registry tree branch is descended until a "leaf" is reached
//  (ie, depth-first).  Then the key is duplicated under the desired target
//  branch. If the key has any associated values, then those are copied over
//  as well.
//
//  NOTE: it is the responsibility of the caller of this function to
//        remove the original source registry key (if so desired)
//
//
//  Input:
//      oldKeyPtr   -- a pointer to a CRegistryEntry object; the subkey
//                     member should contain the "source" registry branch
//
//      newKeyPtr   -- a pointer to a CRegistryEntry object; the subkey
//                     member should contain the "destination" registry branch
//
//  Output:
//      nothing
//
// ========================================================================
//  Created by SEDWARD, 11/16/97
// ========================================================================

VOID    CopyRegKey(CRegistryEntry*  oldKeyPtr, CRegistryEntry*  newKeyPtr)
{
    auto    BOOL                bClose = FALSE;
    auto    CRegistryEntry*     cOldReg = NULL;
    auto    CRegistryEntry*     cNewReg = NULL;
    auto    DWORD               dwIndex = 0;
    auto    DWORD               dwNumSubKeys = 0;
    auto    DWORD               dwNumValues = 0;
    auto    DWORD               dwSizeSubkeyBuffer = REG_MAX_STR;
    auto    FILETIME            fileTime;
    auto    LONG                lResult;
    auto    LPTSTR              lpSubKeyName;
    auto    TCHAR               subkeyName[ REG_MAX_STR ];
    auto    TCHAR               szRootBuf[ REG_MAX_STR ];

    // open the key if necessary
    if (FALSE == oldKeyPtr->IsOpen())
        {
        oldKeyPtr->OpenKey(KEY_ALL_ACCESS);
        bClose = TRUE;
        }

    // set the pointer to point to the subkey name buffer
    lpSubKeyName = subkeyName;

    // if there are any subkeys, we need to descend down the tree
    dwNumSubKeys = oldKeyPtr->GetSubkeyCount();
    if (dwNumSubKeys > 0)
        {
        do  {
            lResult = RegEnumKeyEx(oldKeyPtr->GetCurrentKey(), dwIndex, lpSubKeyName
                                                                    , &dwSizeSubkeyBuffer
                                                                    , NULL
                                                                    , NULL
                                                                    , NULL
                                                                    , &fileTime);
            if (ERROR_SUCCESS == lResult)
                {
                // initialize the root string buffer with the current key path
                // and create a new registry object for the recursive call
                // (this will be the "old key" argument)
                lstrcpy(szRootBuf, oldKeyPtr->GetCurrentSubKeyName());
                lstrcat(szRootBuf, "\\");
                lstrcat(szRootBuf, subkeyName);
                cOldReg = new CRegistryEntry(oldKeyPtr->GetCurrentHive(), szRootBuf, NULL);

                // do the same for the "new key" argument
                lstrcpy(szRootBuf, newKeyPtr->GetCurrentSubKeyName());
                lstrcat(szRootBuf, "\\");
                lstrcat(szRootBuf, subkeyName);
                cNewReg = new CRegistryEntry(newKeyPtr->GetCurrentHive(), szRootBuf, NULL);

                // create the "new key"
                cNewReg->OpenCreateKey(KEY_ALL_ACCESS);

                // pass the new registry objects to the recursive call
                CopyRegKey(cOldReg, cNewReg);
                delete  cOldReg;
                delete  cNewReg;

                // update vars for the next iteration
                dwSizeSubkeyBuffer = REG_MAX_STR;
                ++dwIndex;
                }

            } while (ERROR_SUCCESS == lResult);
        }

    // if there are any values in the "old key", copy them over to the "new key"
    dwNumValues = oldKeyPtr->GetValueCount();
    if (dwNumValues > 0)
        {
        CopyRegkeyValues(oldKeyPtr, newKeyPtr);
        }

    if (bClose)
        {
        oldKeyPtr->CloseKey();
        }

}  // end of "CopyRegKey"



// ==== CopyRegkeyValues ==================================================
//
//  This function copies all the current values under the soure registry
//  key to the destination registry key.
//
//  Input:
//      sourceKeyPtr    -- a pointer to a CRegistryEntry object
//
//      destKeyPtr      -- a pointer to a CRegistryEntry object
//
//  Output:
//      If all goes well, the total number of values copied is returned;
//      else, an invalid value (-1) is returned.
//
// ========================================================================
//  Created by SEDWARD, 11/16/97
// ========================================================================

DWORD   CopyRegkeyValues(CRegistryEntry*  sourceKeyPtr
                                                , CRegistryEntry*  destKeyPtr)
{
    auto    BOOL        bCloseOld = FALSE;
    auto    BOOL        bCloseNew = FALSE;
    auto    DWORD       dwIndex = 0;
    auto    DWORD       dwMaxValueDataSize = 0;
    auto    DWORD       dwMaxValueNameSize = 0;
    auto    DWORD       dwTempDataSize = 0;
    auto    DWORD       dwTempNameSize = 0;
    auto    DWORD       dwTypeCode = 0;
    auto    LONG        lResult = 0;
    auto    LPBYTE      valueDataBuf;
    auto    TCHAR       *valueName;

    // open the keys if necessary
    if (FALSE == sourceKeyPtr->IsOpen())
        {
        sourceKeyPtr->OpenKey(KEY_READ);
        bCloseOld = TRUE;
        }

    if (FALSE == destKeyPtr->IsOpen())
        {
        destKeyPtr->OpenKey(KEY_ALL_ACCESS);
        bCloseNew = TRUE;
        }

    // we only want to fetch the maximum sizes for the value names and value data
    lResult = RegQueryInfoKey(sourceKeyPtr->GetCurrentKey(), NULL, NULL, NULL, NULL, NULL
                                                                    , NULL, NULL
                                                                    , &dwMaxValueNameSize
                                                                    , &dwMaxValueDataSize
                                                                    , NULL, NULL);
    // allocate buffers for the value names and data
    valueDataBuf = (LPBYTE)malloc(dwMaxValueDataSize);
    if (NULL == valueDataBuf)
        {
        dwIndex = lResult = -1;
        goto   CleanUp;
        }
    else
        {
        valueName = new TCHAR[dwMaxValueNameSize + 1];
        if (NULL == valueName)
            {
            dwIndex = lResult = -1;
            goto   CleanUp;
            }
        }

    // now enumerate through the values
    do  {
        dwTempNameSize = dwMaxValueNameSize + 1;
        dwTempDataSize = dwMaxValueDataSize;
        lResult = RegEnumValue(sourceKeyPtr->GetCurrentKey(), dwIndex, valueName
                                                                , &dwTempNameSize
                                                                , 0, &dwTypeCode
                                                                , valueDataBuf
                                                                , &dwTempDataSize);
        if (ERROR_SUCCESS == lResult)
            {
            // copy the "old key" value over to the "new key", based on data type
            destKeyPtr->SetValueName(valueName);
            destKeyPtr->SetValue(dwTypeCode, valueDataBuf, dwTempDataSize);

            ++dwIndex;
            }

        } while (ERROR_SUCCESS == lResult);


CleanUp:

    // release allocated memory and return
    if (NULL != valueDataBuf)
        {
        free(valueDataBuf);
        }

    if (NULL != valueName)
        {
        delete  valueName;
        }

    // return the number of copied values
    return  (dwIndex-1);

}  // end of "CopyRegkeyValues"