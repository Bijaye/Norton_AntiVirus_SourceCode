//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/COPYDEFS/VCS/utils.cpv   1.0   19 Feb 1998 10:11:42   mdunn  $
//
// Description: Source file for miscellaneous utility functions used in
//              the CopyDefs applet.
//
//************************************************************************
//  $Log:   S:/COPYDEFS/VCS/utils.cpv  $
// 
//    Rev 1.0   19 Feb 1998 10:11:42   mdunn
// Initial revision.
// 
//    Rev 1.0   25 Sep 1997 19:10:00   mdunn
// Initial revision.
//************************************************************************

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include "utils.h"


/////////////////////////////////////////////////////////////////////////////
//
// Function:    LLoadString
//
// Description:
//  Loads a string resource with a given ID.
//
// Input:
//  nIDS: the ID of the string
//
// Returns:
//  A pointer to a local static buffer that holds the string.  If the
//  string ID was invalid, this fn throws an assert, and the static buffer
//  is set to a zero-length string.
//
// Notes:
//  Obviously this fn isn't too keen on error-checking the ID.  But it will
//  receive only IDS_* manifest constants so the compiler will catch any
//  bad IDs.
//
// ** WARNING **
//  This fn returns a pointer to a static buffer, so if you want to keep
//  the string around for a while, you should copy the string into a
//  local buffer.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

LPTSTR LLoadString ( int nIDS ) 
{
static HMODULE hModule = INVALID_HANDLE_VALUE;
static TCHAR   szRet [ 1024 ];          // static buffer!
int            nResult;

    if ( hModule == INVALID_HANDLE_VALUE )
        {
        hModule = GetModuleHandle ( NULL );
        }

    assert ( hModule != INVALID_HANDLE_VALUE );

    szRet[0] = 0;

    nResult = LoadString ( hModule, nIDS, szRet, 1024 );

    assert ( nResult != 0 );

    return szRet;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    LNameAppendFile
//
// Description:
//  Just like the Quake NameAppendFile()
//
// Input:
//  szPath: The string that will be changed - may contain a trailing 
//          backslash.
//  szFile: The filename to append - this is NOT checked for a leading
//          backslash!
//
// Returns:
//  A pointer to the changed szPath.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

LPTSTR LNameAppendFile ( LPTSTR szPath, LPCTSTR szFile )
{
    assert ( szPath != NULL );
    assert ( szFile != NULL  &&  szFile[0] != '\0' );

    // Tack on a backslash if szPath does not end in a backslash.
    
    if ( * _tcsdec ( szPath, _tcschr ( szPath, '\0' ) ) != '\\' )
//    if ( szPath [ _tcslen (szPath) - 1 ] != '\\' )
        {
        _tcscat ( szPath, _T("\\") );
        }

    _tcscat ( szPath, szFile );

    return szPath;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    LNameStripFile()
//
// Description:
//  Just like the Quake NameStripFile()
//
// Input:
//  szPath: the string to be changed
//
// Returns:
//  A pointer to the changed szPath.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

LPTSTR LNameStripFile ( LPTSTR szPath )
{
LPTSTR szLastBackslash;

    assert ( szPath != NULL );

    szLastBackslash = _tcsrchr ( szPath, '\\' );

    assert ( szLastBackslash != NULL );

    *szLastBackslash = '\0';

    return szPath;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    LNameReturnFile()
//
// Description:
//  Just like the Quake NameReturnFile().
//
// Input:
//  szPath: A full path to a file.
//
// Returns:
//  The last component of the path, or szPath if there was only one
//  component (ie, no backslashes).
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

LPTSTR LNameReturnFile ( LPTSTR szPath )
{
LPCTSTR szLastBackslash;

    assert ( szPath != NULL  &&  szPath[0] != '\0' );

    szLastBackslash = _tcsrchr ( szPath, '\\' );

    if ( NULL == szLastBackslash )
        {
        // No backslashes were in the path, so just return the path.
        return szPath;
        }
    else
        {
        return _tcsinc ( szLastBackslash );
        }
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    LNameStripExtension()
//
// Description:
//  Just like the Quake NameStripExtension()
//
// Input:
//  szPath: the filename or path to a file
//
// Returns:
//  A pointer to the changed string.
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

LPTSTR LNameStripExtension ( LPTSTR szPath )
{
LPTSTR szLastDot, szLastBackslash;

    szLastDot = _tcsrchr ( szPath, '.' );
    szLastBackslash = _tcsrchr ( szPath, '\\' );

    if ( szLastDot == NULL )
        {
        // No dot in the path, so nothing for us to do.
        return szPath;
        }

    if ( szPath != NULL  &&  szLastDot < szLastBackslash )
        {
        // The last dot wasn't in the last component of the path.
        return szPath;
        }

    *szLastDot = '\0';

    return szPath;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    PrintMessage()
//
// Description:
//  Prints a string with a given resource ID to stdout.
//
// Input:
//  nIDS: The string ID.
//
// Returns:
//  Nothing.
//
// *** WARNING! ***
//  This fn calls LLoadString() so it will wipe out the result of a previous
//  call to LLoadString().
//
/////////////////////////////////////////////////////////////////////////////
//  9/20/97 Mike Dunn -- function created
/////////////////////////////////////////////////////////////////////////////

void PrintMessage ( int nIDS )
{
    _tprintf ( LLoadString ( nIDS ) );
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    GetNAVDir()
//
// Description:
//  Attempts to read the NAV install directory from the registry.
//
// Input:
//  szDir: a buffer that will receive the directory if this fn is successful
//  dwSize: the buffer size in bytes.  This should be at least 
//          MAX_PATH * sizeof(TCHAR)
//
// Returns:
//  TRUE if successful, FALSE if not or if the buffer was too small.
//
//////////////////////////////////////////////////////////////////////////
//  9/23/97 Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL GetNAVDir ( LPTSTR szDir, DWORD dwSize )
{
BOOL bRetVal = FALSE;
HKEY hkeyInstalledApps;

    // First open the InstalledApps key.

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("Software\\Symantec\\InstalledApps"),
                                         0, KEY_READ,
                                         &hkeyInstalledApps ) )
        {
        // Next check for the NAV95 value.  If it's there, then NAV
        // really is installed.

        if ( ERROR_SUCCESS == RegQueryValueEx ( hkeyInstalledApps,
                                                _T("NAV95"), 0,
                                                NULL,
                                                (LPBYTE) szDir, &dwSize ) )
            {
            bRetVal = TRUE;
            }

        RegCloseKey ( hkeyInstalledApps );
        }

    return bRetVal;
}
