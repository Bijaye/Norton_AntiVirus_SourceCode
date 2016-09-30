/* Copyright 1994 Symantec Corporation                                  */
//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/SYMKRNL/VCS/cfg_reg.cpv   1.5   11 May 1998 17:14:40   jbelden  $
//
//////////////////////////////////////////////////////////////////////////
//
// $Log:   S:/SYMKRNL/VCS/cfg_reg.cpv  $
// 
//    Rev 1.5   11 May 1998 17:14:40   jbelden
// Changed the KEY_ALL_ACCESS in ConfigOpenRegistryKey to KEY_READ | KEY_WRITE
// so that non-admin. user can read the registry under NT.
//
//    Rev 1.4   29 Aug 1996 18:15:34   DHERTEL
// Merged changes from Quake E:
// Fixed #65244.  Changed  ConfigRegQueryValue to return ERROR_ACCESS_DENIED if
// the user is not an administrator.
//
//    Rev 1.3   24 Jun 1996 19:03:02   RSTANEV
// Now using the Zv...() wraps instead of the Zw...() APIs.
//
//    Rev 1.2   13 Jun 1996 10:13:58   RSTANEV
// ConfigRegQueryValue() should fail if lpCfgRegKey->lpszSwitch is not
// NULL for platforms where having this parameter does not make sense.
//
//    Rev 1.1   04 Apr 1996 09:17:30   RSTANEV
// Added SYM_NTK support.
//
//    Rev 1.0   26 Jan 1996 20:22:38   JREARDON
// Initial revision.
//
//    Rev 1.13   14 Mar 1995 14:40:58   RSTANEV
// VxDs don't have command lines.
//
//    Rev 1.12   14 Mar 1995 13:27:12   RSTANEV
// "SOFTWARE" key should have all cars uppercased.  Don't know if it matters,
// but MS uses it that way.
//
//    Rev 1.11   06 Mar 1995 10:46:12   HENRI
// Added ConfigOpenRegistryKey & ConfigCloseRegistryKey
//
//    Rev 1.10   02 Feb 1995 10:30:44   HENRI
// Fixed RegistryDeleteTree
//
//    Rev 1.9   04 Jan 1995 09:31:40   RSTANEV
// Now supports VxD platform.
//
//    Rev 1.8   08 Nov 1994 17:39:42   BRAD
// Wouldn't build under WIN16.
//
//    Rev 1.7   05 Nov 1994 12:00:16   MARKK
// Fixed error in ConfigEnumValue
//
//    Rev 1.6   05 Nov 1994 11:17:38   MARKK
// Added ConfigRegEnumValue
//
//    Rev 1.5   09 Oct 1994 16:07:34   DLEVITON
// Check command line switches in ConfigRegQueryValue.
//
//    Rev 1.4   05 Oct 1994 09:28:14   DLEVITON
// ConfigRegDeleteTree
//
//    Rev 1.3   04 Oct 1994 15:05:22   DLEVITON
// ConfigRegDeleteKey and ConfigRegDeleteValue.
//
//    Rev 1.2   30 Aug 1994 14:31:48   BRIANF
// Cleaned up Geologist comments.
//
//    Rev 1.1   25 Aug 1994 14:23:44   DLEVITON
// Use LPVOID for data in Reg calls.
//
//    Rev 1.0   24 Aug 1994 15:01:54   DLEVITON
// Initial revision.
//
//////////////////////////////////////////////////////////////////////////
/*@Library: SYMKRNL */

#include "platform.h"
#ifdef SYM_WIN32
#include <winreg.h>
#endif
#ifdef SYM_VXD
#include <vmmreg.h>
#endif
#include "xapi.h"
#include "symcfg.h"

#if defined(SYM_WIN16) || defined(SYM_DOS)
// Needs to be defined, since its in WIN32's WINERROR.H
#define REGDB_E_KEYMISSING               0x80040152L
#endif

MODULE_NAME;

#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
TCHAR* SOFTWARE = _T("SOFTWARE");
TCHAR* SYMANTEC = _T("Symantec");

LONG DeleteRegistryTree( HKEY hKey );
#endif // SYM_WIN32 || SYM_VXD || SYM_NTK


/*@API:*************************************************************************
@Declaration:  LONG ConfigRegQueryValue( LPCFGREGKEY lpCfgRegKey,
    LPDWORD lpdwReserved, LPDWORD lpdwType, LPBYTE lpbData, LPDWORD lpcbData)

@Description:
This function adds value to the WIN32 API RegQueryValueEx. It returns the data
associated with a Symantec standard registry key. If lpCfgRegKey->lpszSwitch is
provided, the command line is inspected for an override of the setting. \n

Under DOS and WIN16, this calls acts as a cover for ConfigReadString or
ConfigReadValue--depending on the value of *lpdwType. Only REG_DWORD and REG_SZ
are supported.

When Symkrnl constructs a key from a CFGREGKEY, it prepends "Software\Symantec"
if hkey is HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE. Then it concatenates lpszProduct,
lpszVersion, lpszProject, lpszSection, and lpszEntry, separated by backslashes.
Any of these may be NULL or empty, and such entries and their corresponding
backslashes are omitted.

@Parameters:
$lpCfgRegKey$ Identifies the data entry to return.

$lpdwReserved$  Reserved; must be NULL.

$lpdwType$  Points to a variable that receives the key's value type. The value
    returned through this parameter will be one of the following:
~REG_BINARY~    Binary data in any form.
~REG_DWORD~ A 32-bit number.
~REG_DWORD_LITTLE_ENDIAN~   A 32-bit number in little-endian format (same as
    REG_DWORD). In little-endian format, the most significant byte of a word is
    the high-order word.
~REG_DWORD_BIG_ENDIAN~  A 32-bit number in big-endian format. In big-endian format,
    the most significant byte of a word is the low-order word.
~REG_EXPAND_SZ~ A null-terminated string that contains unexpanded references to
    environment variables (for example, "%PATH%"). It will be a UNICODE or ANSI
    string depending on whether you use the UNICODE or ANSI functions.
~REG_LINK~  A Unicode symbolic link.
~REG_MULTI_SZ~  An array of null-terminated strings, terminated by two null characters.
~REG_NONE~  No defined value type.
~REG_RESOURCE_LIST~ A device-driver resource list.
~REG_SZ~    A null-terminated string. It will be a UNICODE or ANSI string depending
    on whether you use the UNICODE or ANSI functions.

The lpdwType parameter can be NULL if the type is not required.

$lpbData$ Points to a buffer that receives the value's data. This parameter can
    be NULL if the data is not required.

$lpcbData$ Points to a variable that specifies the size, in bytes, of the buffer
    pointed to by the lpbData parameter. When the function returns, this variable
    contains the size of the data copied to lpbData.

    If the buffer specified by lpbData parameter is not large enough to hold the
    data, the function returns the value ERROR_MORE_DATA, and stores the required
    buffer size, in bytes, into the variable pointed to by lpcbData.
    If lpbData is NULL, and lpcbData is non-NULL, the function returns ERROR_SUCCESS,
    and stores the size of the data, in bytes, into the variable pointed to by
    lpcbData. This lets an application determine the best way to allocate a buffer
    for the value key's data.

    If the data has the REG_SZ, REG_MULTI_SZ or REG_EXPAND_SZ type, then lpData will
    also include the size of the terminating NULL character.

    The lpcbData parameter may be NULL only if lpbData is NULL.

@Returns:
The return value is passed through from RegQueryValueEx.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: DOS, Win16, Win32, NTK and VxD
*******************************************************************************/

LONG SYM_EXPORT WINAPI ConfigRegQueryValue (
    LPCFGREGKEY lpCfgRegKey,            // specifies data entry to query
    LPDWORD lpdwReserved,               // reserved
    LPDWORD lpdwType,                   // address of buffer for value type
    LPVOID  lpbData,                    // address of data buffer
    LPDWORD  lpcbData )                 // address of data buffer size
{
    LONG    rtn = ERROR_SUCCESS;

#if defined(SYM_WIN16) || defined(SYM_DOS)
    CFGTEXTREC  Config;

    Config.lpSwitch = (LPSTR)lpCfgRegKey->lpszSwitch;
    Config.wFlags = CFG_SW_MINIMUM;
    Config.lpSection = (LPSTR)lpCfgRegKey->lpszSection;
    Config.lpEntry = (LPSTR)lpCfgRegKey->lpszEntry;
    Config.wSource = 0;

    if (lpdwType && *lpdwType == REG_SZ)
        rtn = (LONG)ConfigReadString( &Config, (LPSTR)lpbData, (WORD)(*lpcbData) );
    else
        rtn = (LONG)ConfigReadValue( &Config, (UINT FAR *)lpbData, NULL );
#endif // SYM_WIN16, SYM_DOS


#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
    if (lpCfgRegKey->lpszSwitch)
        {
#if defined(SYM_WIN32)
        CFGTEXTREC  Config;

        Config.lpSwitch = (LPSTR)lpCfgRegKey->lpszSwitch;
        Config.wFlags = CFG_SW_MINIMUM;
        Config.lpSection = (LPSTR)lpCfgRegKey->lpszSection;
        Config.lpEntry = (LPSTR)lpCfgRegKey->lpszEntry;
        Config.wSource = 0;
        if (lpdwType && *lpdwType == REG_SZ)
            {
            if (ConfigSwitchGetText(&Config, (LPSTR)lpbData, (WORD)(*lpcbData)))
                return rtn;
            }
        else
            {
            if (ConfigSwitchGetLongValue(&Config, (DWORD FAR *)lpbData))
                return rtn;
            }
#else
                                        // Setting lpszSwitch for drivers
                                        // doesn't make sense - they don't
                                        // have regular "command lines."

        SYM_ASSERT ( !lpCfgRegKey->lpszSwitch );
        return ( ERROR_INVALID_PARAMETER );
#endif
        }

    HKEY hKey = ConfigOpenRegistryKey( lpCfgRegKey, TRUE );
    
    //
    // ConfigOpenRegistryKey, Calls RegCreateKeyEx to create the key if doesnot exist or
    // get an handle if exist.  If the Current user is not an Administrator then this call
    // will fail with ERROR_ACCESS_DENIED (5) and the return value is NULL.
    //
    
    if (hKey != NULL)
    {
        rtn = RegQueryValueEx( hKey, (LPTSTR)lpCfgRegKey->lpszName, lpdwReserved,
            lpdwType, (LPBYTE)lpbData, lpcbData );

        ConfigCloseRegistryKey( hKey );
    }
    else
        rtn = ERROR_ACCESS_DENIED;  // GetLastError returns ERROR_INVALID_HANDLE (6)!!
#endif // SYM_WIN32 || SYM_VXD || SYM_NTK

    return rtn;
}   // ConfigRegQueryValue


/*@API:*************************************************************************
@Declaration:  LONG ConfigRegEnumValue( LPCFGREGKEY lpCfgRegKey,
    LPDWORD lpdwReserved, DWORD dwIndex, LPDWORD lpdwType, LPBYTE lpbData, LPDWORD lpcbData)

@Description:
This function adds value to the WIN32 API RegQueryValueEx. It returns the data
associated with a Symantec standard registry key. If lpCfgRegKey->lpszSwitch is
provided, the command line is inspected for an override of the setting. \n

Under DOS and WIN16, this calls acts as a cover for ConfigReadString or
ConfigReadValue--depending on the value of *lpdwType. Only REG_DWORD and REG_SZ
are supported.

When Symkrnl constructs a key from a CFGREGKEY, it prepends "Software\Symantec"
if hkey is HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE. Then it concatenates lpszProduct,
lpszVersion, lpszProject, lpszSection, and lpszEntry, separated by backslashes.
Any of these may be NULL or empty, and such entries and their corresponding
backslashes are omitted.

@Parameters:
$lpCfgRegKey$ Identifies the data entry to return.

$lpdwReserved$  Reserved; must be NULL.

$dwIndex$ Index to enumerate, starts at 0.

$lpdwType$  Points to a variable that receives the key's value type. The value
    returned through this parameter will be one of the following:
~REG_BINARY~    Binary data in any form.
~REG_DWORD~ A 32-bit number.
~REG_DWORD_LITTLE_ENDIAN~   A 32-bit number in little-endian format (same as
    REG_DWORD). In little-endian format, the most significant byte of a word is
    the high-order word.
~REG_DWORD_BIG_ENDIAN~  A 32-bit number in big-endian format. In big-endian format,
    the most significant byte of a word is the low-order word.
~REG_EXPAND_SZ~ A null-terminated string that contains unexpanded references to
    environment variables (for example, "%PATH%"). It will be a UNICODE or ANSI
    string depending on whether you use the UNICODE or ANSI functions.
~REG_LINK~  A Unicode symbolic link.
~REG_MULTI_SZ~  An array of null-terminated strings, terminated by two null characters.
~REG_NONE~  No defined value type.
~REG_RESOURCE_LIST~ A device-driver resource list.
~REG_SZ~    A null-terminated string. It will be a UNICODE or ANSI string depending
    on whether you use the UNICODE or ANSI functions.

The lpdwType parameter can be NULL if the type is not required.

$lpbData$ Points to a buffer that receives the value's data. This parameter can
    be NULL if the data is not required.

$lpcbData$ Points to a variable that specifies the size, in bytes, of the buffer
    pointed to by the lpbData parameter. When the function returns, this variable
    contains the size of the data copied to lpbData.

    If the buffer specified by lpbData parameter is not large enough to hold the
    data, the function returns the value ERROR_MORE_DATA, and stores the required
    buffer size, in bytes, into the variable pointed to by lpcbData.
    If lpbData is NULL, and lpcbData is non-NULL, the function returns ERROR_SUCCESS,
    and stores the size of the data, in bytes, into the variable pointed to by
    lpcbData. This lets an application determine the best way to allocate a buffer
    for the value key's data.

    If the data has the REG_SZ, REG_MULTI_SZ or REG_EXPAND_SZ type, then lpData will
    also include the size of the terminating NULL character.

    The lpcbData parameter may be NULL only if lpbData is NULL.

@Returns:
The return value is passed through from RegQueryValueEx.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: DOS, Win16, Win32, NTK and VxD
*******************************************************************************/

LONG SYM_EXPORT WINAPI ConfigRegEnumValue (
    LPCFGREGKEY lpCfgRegKey,            // specifies data entry to query
    LPDWORD lpdwReserved,               // reserved
    DWORD   dwIndex,			// Index
    LPDWORD lpdwType,                   // address of buffer for value type
    LPVOID  lpbData,                    // address of data buffer
    LPDWORD  lpcbData )                 // address of data buffer size
{
    LONG    rtn = ERROR_SUCCESS;

#if defined(SYM_WIN16) || defined(SYM_DOS)
    rtn = REGDB_E_KEYMISSING;		// Only implemented under Win32
#endif // SYM_WIN16, SYM_DOS


#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)

    HKEY hKey = ConfigOpenRegistryKey(lpCfgRegKey, TRUE);
    DWORD dwSize = SYM_MAX_PATH;

    rtn = RegEnumValue( hKey, dwIndex, (LPTSTR)lpCfgRegKey->lpszName, &dwSize, lpdwReserved,
            lpdwType, (LPBYTE)lpbData, lpcbData );

    ConfigCloseRegistryKey( hKey );
#endif // SYM_WIN32 || SYM_VXD || SYM_NTK

    return rtn;
}   // ConfigRegEnumValue




/*@API:*************************************************************************
@Declaration:  LONG ConfigRegSetValue( LPCFGREGKEY lpCfgRegKey,
    DWORD dwReserved, DWORD fdwType, CONST BYTE * lpbData, DWORD cbData)

@Description:
This function adds value to the WIN32 API RegSetValue. It stores data
associated with a Symantec standard registry key. \n

Under DOS and WIN16, this calls acts as a cover for ConfigWriteString and
ConfigWriteValue. Only REG_DWORD and REG_SZ are supported.

When Symkrnl constructs a key from a CFGREGKEY, it prepends "Software\Symantec"
if hkey is HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE. Then it concatenates lpszProduct,
lpszVersion, lpszProject, lpszSection, and lpszEntry, separated by backslashes.
Any of these may be NULL or empty, and such entries and their corresponding
backslashes are omitted.

@Parameters:
$lpCfgRegKey$ Identifies the data entry to return.

$dwReserved$  Reserved; must be 0.

$fdwType$  The key's value type. The value of this
    parameter should be one of the following:
~REG_BINARY~    Binary data in any form.
~REG_DWORD~ A 32-bit number.
~REG_DWORD_LITTLE_ENDIAN~   A 32-bit number in little-endian format (same as
    REG_DWORD). In little-endian format, the most significant byte of a word is
    the high-order word.
~REG_DWORD_BIG_ENDIAN~  A 32-bit number in big-endian format. In big-endian format,
    the most significant byte of a word is the low-order word.
~REG_EXPAND_SZ~ A null-terminated string that contains unexpanded references to
    environment variables (for example, "%PATH%"). It will be a UNICODE or ANSI
    string depending on whether you use the UNICODE or ANSI functions.
~REG_LINK~  A Unicode symbolic link.
~REG_MULTI_SZ~  An array of null-terminated strings, terminated by two null characters.
~REG_NONE~  No defined value type.
~REG_RESOURCE_LIST~ A device-driver resource list.
~REG_SZ~    A null-terminated string. It will be a UNICODE or ANSI string depending
    on whether you use the UNICODE or ANSI functions.

$lpbData$ Points to a buffer that contains the value's data.

$cbData$ The size, in bytes, of the buffer pointed to by the lpbData parameter.  If
    the data is of type REG_SZ, REG_EXPAND_SZ, or REG_MULTI_SZ, cbData must include
    the size of the terminating null character.

@Returns:
The return value is passed through from RegSetValueEx.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: DOS, Win16, Win32, NTK and VxD
*******************************************************************************/
LONG SYM_EXPORT WINAPI ConfigRegSetValue (
    LPCFGREGKEY lpCfgRegKey,            // specifies data entry to query
    DWORD       dwReserved,             // reserved
    DWORD       fdwType,                // address of buffer for value type
    CONST LPVOID lpbData,               // address of value data
    DWORD       cbData )                // size of value data
{
    LONG        rtn = ERROR_SUCCESS;

#if defined(SYM_WIN16) || defined(SYM_DOS)
    CFGTEXTREC  Config;

    Config.lpSwitch = NULL;
    Config.wFlags = 0;
    Config.lpSection = (LPSTR)lpCfgRegKey->lpszSection;
    Config.lpEntry = (LPSTR)lpCfgRegKey->lpszEntry;
    Config.wSource = 0;

    if (fdwType == REG_SZ)
        rtn = (LONG)ConfigWriteString( &Config, (LPSTR)lpbData );
    else
        rtn = (LONG)ConfigWriteValue( &Config, *(UINT FAR *)lpbData );
#endif // SYM_WIN16, SYM_DOS


#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
    HKEY hKey = ConfigOpenRegistryKey( lpCfgRegKey, TRUE );

    rtn = RegSetValueEx( hKey, lpCfgRegKey->lpszName, dwReserved,
            fdwType, (CONST LPBYTE)lpbData, cbData );

    ConfigCloseRegistryKey( hKey );
#endif // SYM_WIN32 || SYM_VXD || SYM_NTK

    return rtn;
}   // ConfigRegSetValue


/*@API:*************************************************************************
@Declaration:  LONG ConfigRegDeleteValue( LPCFGREGKEY lpCfgRegKey )

@Description:
This function adds value to the WIN32 API RegDeleteValue. It deletes data
associated with a Symantec standard registry key. \n

Under DOS and WIN16, this calls acts as a cover for ConfigRegSetValue.

@Parameters:
$lpCfgRegKey$ Identifies the data entry to delete.

@Returns:
The return value is passed through from RegDeleteValue.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: DOS, Win16, Win32, NTK and VxD
*******************************************************************************/
LONG SYM_EXPORT WINAPI ConfigRegDeleteValue (
    LPCFGREGKEY lpCfgRegKey )           // specifies data entry to delete
{
    LONG        rtn = ERROR_SUCCESS;

#if defined(SYM_WIN16) || defined(SYM_DOS)
    rtn = ConfigRegSetValue( lpCfgRegKey, 0, REG_SZ, NULL, 0 );
#endif // SYM_WIN16, SYM_DOS


#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
    HKEY hKey = ConfigOpenRegistryKey( lpCfgRegKey, TRUE );

    rtn = RegDeleteValue( hKey, (LPTSTR)lpCfgRegKey->lpszName );

    ConfigCloseRegistryKey( hKey );
#endif // SYM_WIN32 || SYM_VXD || SYM_NTK

    return rtn;
}   // ConfigRegDeleteValue


/*@API:*************************************************************************
@Declaration:  LONG ConfigRegDeleteKey( LPCFGREGKEY lpCfgRegKey )

@Description:
This function adds value to the WIN32 API RegDeleteKey. It deletes a Symantec
standard registry key. \n

Under DOS and WIN16, this calls acts as a cover for ConfigRegSetValue.

@Parameters:
$lpCfgRegKey$ Identifies the data entry to delete.

@Returns:
The return value is passed through from RegDeleteKey.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: DOS, Win16, Win32, NTK and VxD
*******************************************************************************/
LONG SYM_EXPORT WINAPI ConfigRegDeleteKey (
    LPCFGREGKEY lpCfgRegKey )           // specifies data entry to delete
{
    LONG        rtn = ERROR_SUCCESS;

#if defined(SYM_WIN16) || defined(SYM_DOS)
    rtn = ConfigRegSetValue( lpCfgRegKey, 0, REG_SZ, NULL, 0 );
#endif // SYM_WIN16, SYM_DOS


#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
    HKEY hKey = ConfigOpenRegistryKey( lpCfgRegKey, FALSE );

    rtn = RegDeleteKey( hKey, lpCfgRegKey->lpszEntry );

    ConfigCloseRegistryKey( hKey );
#endif // SYM_WIN32 || SYM_VXD || SYM_NTK

    return rtn;
}   // ConfigRegDeleteKey


/*@API:*************************************************************************
@Declaration:  LONG ConfigRegDeleteTree( LPCFGREGKEY lpCfgRegKey )

@Description:
This function adds value to the WIN32 API RegDeleteKey. It deletes a Symantec
standard registry key, including all its subkeys and all of their values. \n

Under DOS and WIN16, this call does nothing, because only one level of keys is
available. Use ConfigRegDeleteKey or ConfigRegSetValue.

@Parameters:
$lpCfgRegKey$ Identifies the data entry to delete.

@Returns:
The return value is passed through from RegDeleteKey.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: DOS, Win16, Win32, NTK and VxD
*******************************************************************************/
LONG SYM_EXPORT WINAPI ConfigRegDeleteTree (
    LPCFGREGKEY lpCfgRegKey )           // specifies data entry to delete
{
    LONG        rtn = ERROR_SUCCESS;

#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
    HKEY hKey = ConfigOpenRegistryKey( lpCfgRegKey, TRUE );

    rtn = DeleteRegistryTree( hKey );
    ConfigCloseRegistryKey( hKey );

    if (rtn == ERROR_SUCCESS)
        {
        hKey = ConfigOpenRegistryKey( lpCfgRegKey, FALSE );
        rtn = RegDeleteKey( hKey, lpCfgRegKey->lpszEntry );
        ConfigCloseRegistryKey( hKey );
        }
#endif // SYM_WIN32 || SYM_VXD || SYM_NTK

    return rtn;
}   // ConfigRegDeleteTree


#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)

/*@API:*************************************************************************
@Declaration:  LONG ConfigCloseRegistryKey( HKEY hKey )

@Description:
This function closes a registry key opened with ConfigOpenRegistryKey.\n

Under DOS and WIN16, this call does nothing, because only one level of keys is
available.

@Parameters:
$hKey$ Identifies the handle to the registry entry to close.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: Win32, NTK and VxD
*******************************************************************************/
VOID SYM_EXPORT WINAPI ConfigCloseRegistryKey( HKEY hKey )
{
    if (hKey != HKEY_USERS &&
        hKey != HKEY_CURRENT_USER &&
        hKey != HKEY_CLASSES_ROOT &&
        hKey != HKEY_LOCAL_MACHINE)
        RegCloseKey( hKey );
    return;
}   // ConfigCloseRegistryKey


// DeleteRegistryTree deletes all subkeys of an open parent key, but
// not the parent key itself. It works by calling itself recursively
// on all subkeys of the parent, deleting a key after its subtree has
// been wiped out.
LONG DeleteRegistryTree( HKEY hKeyParent )
{
    TCHAR       szName[MAX_PATH];
    DWORD       dwSize;
#if !defined(SYM_VXD)
    FILETIME    ftLastWrite;
#endif
    HKEY        hKey;
    LONG        rtn = ERROR_SUCCESS;

    do {
        dwSize = sizeof(szName);
                                        // Always ask for the first subkey,
                                        // since deleting may cause reordering.
        rtn = RegEnumKeyEx( hKeyParent, 0, szName, &dwSize, NULL,
                NULL, NULL, &ftLastWrite );
        if (rtn == ERROR_SUCCESS)
            {
            rtn = RegOpenKeyEx( hKeyParent, szName, 0, KEY_ALL_ACCESS, &hKey );
            if (rtn == ERROR_SUCCESS)
                {
                rtn = DeleteRegistryTree( hKey );
                RegCloseKey( hKey );
                if (rtn == ERROR_SUCCESS)
                    rtn = RegDeleteKey( hKeyParent, szName );
                }
            }
    } while (rtn == ERROR_SUCCESS);

    if (rtn == ERROR_NO_MORE_ITEMS)
        rtn = ERROR_SUCCESS;
    return rtn;
}   // DeleteRegistryTree


/*@API:*************************************************************************
@Declaration:  LONG ConfigOpenRegistryKey( LPCFGREGKEY lpCfgRegKey, BOOL fWithEntry )

@Description:
This function opens a handle to a registry tree or entry.\n

Under DOS and WIN16, this call does nothing, because only one level of keys is
available. Use ConfigRegDeleteKey or ConfigRegSetValue.

@Parameters:
$lpCfgRegKey$ Identifies the data entry to delete.
$fWithEntry$  If the key contains an entry, then get the handle to the entry.

@Returns:
A handle to the registry key.

@See: CFGREGKEY

@Include: symcfg.h

@Compatibility: Win32, NTK and VxD
*******************************************************************************/
HKEY SYM_EXPORT WINAPI ConfigOpenRegistryKey( LPCFGREGKEY lpCfgRegKey, BOOL fWithEntry )
{
#if !defined(SYM_VXD)
    DWORD       dwDisp;
#endif
    HKEY        hKey = NULL;
    HKEY        hKeyPrev = lpCfgRegKey->hkey;
    LONG        rtn = ERROR_SUCCESS;

    if (lpCfgRegKey->hkey == HKEY_CURRENT_USER ||
        lpCfgRegKey->hkey == HKEY_LOCAL_MACHINE)
        {
        rtn = RegCreateKeyEx( hKeyPrev, SOFTWARE, NULL, _T(""),
             NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisp );
        if (rtn != ERROR_SUCCESS)
            goto Exit;
        hKeyPrev = hKey;
        rtn = RegCreateKeyEx( hKeyPrev, SYMANTEC, NULL, _T(""),
             NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisp );
        if (rtn != ERROR_SUCCESS)
            goto Exit;
        ConfigCloseRegistryKey( hKeyPrev );
        hKeyPrev = hKey;
        }

    if (lpCfgRegKey->lpszProduct && *lpCfgRegKey->lpszProduct)
        {
        rtn = RegCreateKeyEx( hKeyPrev, lpCfgRegKey->lpszProduct, NULL, _T(""),
             NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisp );
        if (rtn != ERROR_SUCCESS)
            goto Exit;
        ConfigCloseRegistryKey( hKeyPrev );
        hKeyPrev = hKey;
        }
    if (lpCfgRegKey->lpszVersion && *lpCfgRegKey->lpszVersion)
        {
        rtn = RegCreateKeyEx( hKeyPrev, lpCfgRegKey->lpszVersion, NULL, _T(""),
             NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisp );
        if (rtn != ERROR_SUCCESS)
            goto Exit;
        ConfigCloseRegistryKey( hKeyPrev );
        hKeyPrev = hKey;
        }
    if (lpCfgRegKey->lpszProject && *lpCfgRegKey->lpszProject)
        {
        rtn = RegCreateKeyEx( hKeyPrev, lpCfgRegKey->lpszProject, NULL, _T(""),
             NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisp );
        if (rtn != ERROR_SUCCESS)
            goto Exit;
        ConfigCloseRegistryKey( hKeyPrev );
        hKeyPrev = hKey;
        }
    if (lpCfgRegKey->lpszSection && *lpCfgRegKey->lpszSection)
        {
        rtn = RegCreateKeyEx( hKeyPrev, lpCfgRegKey->lpszSection, NULL, _T(""),
             NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisp );
        if (rtn != ERROR_SUCCESS)
            goto Exit;
        ConfigCloseRegistryKey( hKeyPrev );
        hKeyPrev = hKey;
        }
    if (fWithEntry && lpCfgRegKey->lpszEntry && *lpCfgRegKey->lpszEntry)
        {
        rtn = RegCreateKeyEx( hKeyPrev, lpCfgRegKey->lpszEntry, NULL, _T(""),
             NULL, KEY_READ | KEY_WRITE, NULL, &hKey, &dwDisp );
        if (rtn != ERROR_SUCCESS)
            goto Exit;
        ConfigCloseRegistryKey( hKeyPrev );
        hKeyPrev = hKey;
        }

Exit:
    if (rtn != ERROR_SUCCESS)
        {
        if (hKeyPrev != lpCfgRegKey->hkey)
            ConfigCloseRegistryKey( hKeyPrev );
        hKey = NULL;
        }
 return(hKey);
}   // ConfigOpenRegistryKey

#endif // SYM_WIN32 || SYM_VXD || SYM_NTK

#if defined(SYM_NTK) && defined(UNICODE)
//---------------------------------------------------------------------------
//
// This part of the file defines registry functions for SYM_NTK platform.
//
//---------------------------------------------------------------------------
static HANDLE FASTCALL OpenPredefinedKey (
    HKEY hKey,
    REGSAM samDesired
    )
    {
    auto HANDLE            hResult;
    auto UNICODE_STRING    Key;
    auto OBJECT_ATTRIBUTES ObjectAttributes;

    SYM_ASSERT ( hKey );

    switch ( hKey )
        {
        case HKEY_CLASSES_ROOT:
            SYM_ASSERT ( FALSE );
            return ( NULL );

        case HKEY_CURRENT_USER:
            SYM_ASSERT ( FALSE );
            return ( NULL );

        case HKEY_LOCAL_MACHINE:
            RtlInitUnicodeString ( &Key, L"\\Registry\\Machine" );
            break;

        case HKEY_USERS:
            SYM_ASSERT ( FALSE );
            return ( NULL );

        default:
            SYM_ASSERT ( sizeof(HANDLE) == sizeof(HKEY) );
            return ( (HANDLE) hKey );
        }

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &Key,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

    if ( NT_SUCCESS ( ZvOpenKey ( &hResult,
                                  samDesired,
                                  &ObjectAttributes ) ) )
        {
        return ( hResult );
        }

    return ( NULL );
    }

static VOID FASTCALL ClosePredefinedKey (
    HKEY hKey,
    HANDLE hPredefined
    )
    {
    SYM_ASSERT ( hKey );

    if ( hPredefined )
        {
        switch ( hKey )
            {
            case HKEY_CLASSES_ROOT:
            case HKEY_CURRENT_USER:
            case HKEY_LOCAL_MACHINE:
            case HKEY_USERS:
                ZvClose ( hPredefined );
            }
        }
    }

LONG RegOpenKeyExW (
    HKEY hKey,
    LPCTSTR lpSubKey,
    DWORD ulOptions,
    REGSAM samDesired,
    PHKEY phkResult
    )
    {
    auto LONG              lResult = ERROR_SUCCESS;
    auto HANDLE            hPredefined;
    auto UNICODE_STRING    SubKey;
    auto OBJECT_ATTRIBUTES ObjectAttributes;

    SYM_ASSERT ( hKey );
    SYM_ASSERT ( !ulOptions );
    SYM_ASSERT ( phkResult );
    SYM_VERIFY_BUFFER ( phkResult, sizeof(HANDLE) );

    hPredefined = OpenPredefinedKey ( hKey, samDesired );

    if ( !lpSubKey || !*lpSubKey )
        {
        lpSubKey = _T("\\");
        }

    RtlInitUnicodeString ( &SubKey, lpSubKey );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &SubKey,
                                 OBJ_CASE_INSENSITIVE,
                                 hPredefined,
                                 NULL );

    SYM_ASSERT ( sizeof(HANDLE) == sizeof(HKEY) );

    if ( !NT_SUCCESS ( ZvOpenKey ( (PHANDLE)phkResult,
                                   samDesired,
                                   &ObjectAttributes ) ) )
        {
        *phkResult = NULL;
        lResult = ERROR_REGISTRY_CORRUPT;
        }

    ClosePredefinedKey ( hKey, hPredefined );

    return ( lResult );
    }

LONG RegCloseKey (
    HKEY hKey
    )
    {
    SYM_ASSERT ( hKey );

    SYM_ASSERT ( sizeof(HANDLE) == sizeof(HKEY) );

    if ( !NT_SUCCESS ( ZvClose ( (HANDLE)hKey ) ) )
        {
        SYM_ASSERT ( FALSE );
        return ( ERROR_INVALID_HANDLE );
        }

    return ( ERROR_SUCCESS );
    }

LONG RegCreateKeyExW (
    HKEY hKey,
    LPCTSTR lpSubKey,
    DWORD Reserved,
    LPTSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
    )
    {
    auto LONG              lResult = ERROR_SUCCESS;
    auto HANDLE            hPredefined;
    auto UNICODE_STRING    SubKey, Class;
    auto OBJECT_ATTRIBUTES ObjectAttributes;

    SYM_ASSERT ( hKey );
    SYM_ASSERT ( lpSubKey );
    SYM_ASSERT ( *lpSubKey != '\\' );
    SYM_VERIFY_STRING ( lpSubKey );
    SYM_ASSERT ( !Reserved );
    SYM_ASSERT ( !lpSecurityAttributes );
    SYM_ASSERT ( phkResult );
    SYM_VERIFY_BUFFER ( phkResult, sizeof(HANDLE) );
    SYM_ASSERT ( lpdwDisposition );
    SYM_VERIFY_BUFFER ( lpdwDisposition, sizeof(*lpdwDisposition) );

    hPredefined = OpenPredefinedKey ( hKey, samDesired );

    RtlInitUnicodeString ( &SubKey, lpSubKey );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &SubKey,
                                 OBJ_CASE_INSENSITIVE,
                                 hPredefined,
                                 NULL );

    RtlInitUnicodeString ( &Class, lpClass ? lpClass : _T("") );

    SYM_ASSERT ( sizeof(HANDLE) == sizeof(HKEY) );

    if ( !NT_SUCCESS ( ZvCreateKey ( (PHANDLE)phkResult,
                                     samDesired,
                                     &ObjectAttributes,
                                     0,
                                     &Class,
                                     dwOptions,
                                     lpdwDisposition ) ) )
        {
        *phkResult = NULL;
        lResult = ERROR_REGISTRY_CORRUPT;
        }

    ClosePredefinedKey ( hKey, hPredefined );

    return ( lResult );
    }

LONG RegQueryValueExW (
    HKEY hKey,
    LPCTSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    )
    {
    auto LONG                          lResult = ERROR_SUCCESS;
    auto UNICODE_STRING                ValueName;
    auto DWORD                         dwDummy;
    auto KEY_VALUE_PARTIAL_INFORMATION Info, *pInfo;

    SYM_ASSERT ( hKey );
    SYM_ASSERT ( lpValueName );
    SYM_VERIFY_STRING ( lpValueName );
    SYM_ASSERT ( !lpReserved );

    RtlInitUnicodeString ( &ValueName, lpValueName );

    SYM_ASSERT ( sizeof(HANDLE) == sizeof(HKEY) );

    switch ( ZvQueryValueKey ( (HANDLE)hKey,
                               &ValueName,
                               KeyValuePartialInformation,
                               &Info,
                               sizeof(Info),
                               &dwDummy ) )
        {
        case STATUS_OBJECT_NAME_NOT_FOUND:
            return ( ERROR_CANTREAD );

        case STATUS_BUFFER_OVERFLOW:
            break;

        default:
            dwDummy = sizeof(Info);
        }

    if ( pInfo = (PKEY_VALUE_PARTIAL_INFORMATION) MemAllocPtr ( GHND, dwDummy ) )
        {
        if ( NT_SUCCESS ( ZvQueryValueKey ( (HANDLE)hKey,
                                            &ValueName,
                                            KeyValuePartialInformation,
                                            pInfo,
                                            dwDummy,
                                            &dwDummy ) ) )
            {
            lpType && ( *lpType = pInfo->Type );

            if ( lpData )
                {
                SYM_ASSERT ( lpcbData );

                if ( *lpcbData >= pInfo->DataLength )
                    {
                    MEMCPY ( lpData, &pInfo->Data, pInfo->DataLength );
                    }
                else
                    {
                    lResult = ERROR_BUFFER_OVERFLOW;
                    }

                *lpcbData = pInfo->DataLength;
                }
            else
                {
                lpcbData && ( *lpcbData = dwDummy - sizeof(Info) + 1 );
                }
            }
        else
            {
            SYM_ASSERT ( FALSE );
            lResult = ERROR_REGISTRY_CORRUPT;
            }

        MemFreePtr ( pInfo );
        }
    else
        {
        SYM_ASSERT ( FALSE );
        lResult = ERROR_OUTOFMEMORY;
        }

    return ( lResult );
    }

LONG RegSetValueExW (
    HKEY hKey,
    LPCTSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    CONST BYTE* lpData,
    DWORD cbData
    )
    {
    auto UNICODE_STRING ValueName;

    SYM_ASSERT ( hKey );
    SYM_ASSERT ( !Reserved );
    SYM_ASSERT ( lpData );
    SYM_ASSERT ( cbData );
    SYM_VERIFY_BUFFER ( lpData, cbData );

    if ( lpValueName )
        {
        RtlInitUnicodeString ( &ValueName, lpValueName );
        }

    SYM_ASSERT ( sizeof(HANDLE) == sizeof(HKEY) );

    if ( !NT_SUCCESS ( ZvSetValueKey ( (HANDLE)hKey,
                                       lpValueName ? &ValueName : NULL,
                                       0,
                                       dwType,
                                       (PVOID)lpData,
                                       cbData ) ) )
        {
        return ( ERROR_REGISTRY_CORRUPT );
        }

    return ( ERROR_SUCCESS );
    }

LONG RegEnumValueW (
    HKEY hKey,
    DWORD dwIndex,
    LPTSTR lpValueName,
    LPDWORD lpcbValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
    )
    {
    SYM_ASSERT ( FALSE );
    return ( ERROR_REGISTRY_CORRUPT );
    }

LONG RegEnumKeyExW (
    HKEY hKey,
    DWORD dwIndex,
    LPTSTR lpName,
    LPDWORD lpcbName,
    LPDWORD lpReserved,
    LPTSTR lpClass,
    LPDWORD lpcbClass,
    PFILETIME lpftLastWriteTime
    )
    {
    SYM_ASSERT ( FALSE );
    return ( ERROR_REGISTRY_CORRUPT );
    }

LONG RegDeleteValueW (
    HKEY hKey,
    LPCTSTR lpValueName
    )
    {
    SYM_ASSERT ( FALSE );
    return ( ERROR_REGISTRY_CORRUPT );
    }

LONG RegDeleteKeyW (
    HKEY hKey,
    LPCTSTR lpSubKey
    )
    {
    SYM_ASSERT ( FALSE );
    return ( ERROR_REGISTRY_CORRUPT );
    }

#endif // SYM_NTK
