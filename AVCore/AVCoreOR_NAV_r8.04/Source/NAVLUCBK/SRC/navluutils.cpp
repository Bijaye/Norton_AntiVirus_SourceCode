
// Never define/include anything before this.
#include "StdAfx.h"
#include "NavLuUtils.h"
#include "strings.h"

//****************************************************************************
// Quake Replacement functions ***********************************************
//****************************************************************************

//////////////////////////////////////////////////////////////////////////
// Description:
//   This function appends the string specified by lpszAdd to the
//   lpszDest string. If lpszDest does not end in a \, one is automatically
//   added before the append occurs.
//
//   This function should correctly handle the following strings.
//
//      lpszDest (in)    lpszAdd     lpszDest (out)
//      ------------------------------------------------------------------
//      s:               abc         s:abc
//      s:.              abc         s:.\abc
//      s:..             abc         s:..\abc
//      s:dir            abc         s:dec\abc
//
// Parameters:
//   lpszDest     in/out Provides the string to append to. This string must
//                       already be allocated to included the number of
//                       characters from lpszAdd.
//   lpszAdd      in     String to append to lpszDest.
//////////////////////////////////////////////////////////////////////////
// 05/14/97 JBRENNA created.
//////////////////////////////////////////////////////////////////////////
void NavLuNameAppendFile (LPTSTR lpszDest, LPCTSTR lpszAdd)
{
    LPTSTR lpszDestCur;

    ASSERT (lpszDest);
    ASSERT (lpszAdd);

    // Find the next to the last character.
    for (lpszDestCur = lpszDest;
         *lpszDestCur != _T('\0') && *(lpszDestCur+1) != _T('\0');
         lpszDestCur = CharNext (lpszDestCur));

    // If we are not at the end of the string (I.E. lpszDestCur points
    // to the last character in lpszDest) ...
    if (*lpszDestCur != _T('\0'))
    {
        // And if the last character is not a : or \ character, append a \ at
        // the end of the destination string. The : character check ensures
        // we do not add \ to a "s:" destination string.
        if (*lpszDestCur != _T(':') && *lpszDestCur != _T('\\'))
        {
            *(lpszDestCur+1) = _T('\\');
            *(lpszDestCur+2) = _T('\0');

            // Move two characters forward. This skips the last character and
            // the newly appended \ character.
            lpszDestCur += 2;
        }
        // Else, the last character is a \. Increment past it.
        else
            lpszDestCur += 1;
    }

    _tcscpy (lpszDestCur, lpszAdd);
}

#if 0
//////////////////////////////////////////////////////////////////////////////
// Description:
//   Determines if the specified file exists on the system. Note that a
//   directory name does not count as a file name.
//
// Returns:
//   TRUE  - file found.
//   FALSE - file not found.
//////////////////////////////////////////////////////////////////////////////
BOOL NavLuFileExists (LPCTSTR lpszFileName)
{
    DWORD dwFileAttrs;

    // Ensure that file name is not NULL.
    ASSERT (lpszFileName);

    dwFileAttrs = GetFileAttributes (lpszFileName);

    if (0xFFFFFFFF == dwFileAttrs)
        return FALSE;
    else if (FILE_ATTRIBUTE_DIRECTORY & dwFileAttrs)
        return FALSE;
    else
        return TRUE;
}
#endif

//****************************************************************************
// External functions ********************************************************
//****************************************************************************

//////////////////////////////////////////////////////////////////////////////
// Description:
//   Determines the path to the S32LIVE1.DLL.
//
// Returns:
//   TRUE  - S32Live1.DLL found.
//   FALSE - S32Live1.DLL not found.
//////////////////////////////////////////////////////////////////////////////
BOOL NavLuGetLuPathName (LPTSTR lpszLuPathName, DWORD dwPathSize)
{
    LPCTSTR  szLuPathKey  = _T("SOFTWARE\\Symantec\\SharedUsage");
    LPCTSTR  szLuQueryKey = _T("LiveUpdate1");
    BOOL     bFoundPath   = FALSE;
    HKEY     hBaseKey;
    DWORD    dwValueType;

    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szLuPathKey, 0, KEY_READ, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szLuQueryKey, 0, &dwValueType, (LPBYTE)lpszLuPathName, &dwPathSize))
        {
            if (dwValueType == REG_SZ && *lpszLuPathName)
            {
                NavLuNameAppendFile (lpszLuPathName, "S32Live1.DLL");
                bFoundPath = TRUE;
            }
        }
        RegCloseKey (hBaseKey);
    }

    return bFoundPath;
}

#if 0
//////////////////////////////////////////////////////////////////////////
// Description:
//   Determine if the Shared Tech LiveUpdate component is on the machine.
//
// Parameters:
//   lpszLuPathName in  If present, provides a file name and path to verify
//                      the installation of NAVLU.  If that file does not
//                      exist then it returns LiveUpdate not on the machine.
// Returns:
//   TRUE   - LiveUpdate is on the machine.
//   FALSE  - LiveUpdate is not on the machine.
//////////////////////////////////////////////////////////////////////////
BOOL NavLuIsLuInstalled (LPTSTR lpszLuPathName)
{
    BOOL   bFoundPath = FALSE;
    TCHAR  szFile[ _MAX_PATH+1 ];

    // If there is a reference file, check to see if it is there.
    if( NULL != lpszLuPathName && !NavLuFileExists(lpszLuPathName) )
        return FALSE;

    bFoundPath = NavLuGetLuPathName (szFile, sizeof(szFile));

    return bFoundPath && NavLuFileExists (szFile);
}
#endif

#if 0
//////////////////////////////////////////////////////////////////////////
// Description:
//   Actually launch the Nav LiveUpdate program.
//
// Returns:
//   TRUE   - success.
//   FALSE  - failure.
//////////////////////////////////////////////////////////////////////////
BOOL NavLuLaunchLiveUpdate (void)
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    BOOL                bSuccess;
    TCHAR               szCmdLine[_MAX_PATH];

    memset (&si, 0, sizeof(si));
    si.cb = sizeof(si);

	// We're now calling LuAll which is located in LiveUpdate's
	// install area.  The install directory is specified in the 
	// SharedUsage key of the registry.
	HKEY hKey= 0;
	DWORD dwSize = sizeof( szCmdLine );

	if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
									   TEXT("SOFTWARE\\Symantec\\SharedUsage"), 0, 
									   KEY_READ, &hKey ) )
	{
		RegQueryValueEx( hKey, TEXT("LiveUpdate1"), 0, 0, (LPBYTE)szCmdLine, &dwSize );
		RegCloseKey(hKey);
	}

	// Concatenate a \ to the returned directory, if needed.
	size_t lLength = ( _tcslen( szCmdLine );
	if ( lLength > 0 )
	{
		// Check to see if there was a '\\'
		if ( _tcsrchr ( strCmdLine, '\\' ) != NULL )
			_tcscat(szCmdLine, _T("\\");
	}
    // Copy this into a non-const buffer. CreateProcess takes a non-const
    // command line.  Append the LuAll name to it full path as taken from
	// the SharedUsage registry key.
    _tcscat ( szCmdLine, g_szProgramName );

    // Start the child process.
    bSuccess = CreateProcess (NULL,             // Application name
                              szCmdLine,        // Command line to run.
                              NULL,             // Process handle not inheritable.
                              NULL,             // Thread handle not inheritable.
                              FALSE,            // Set handle inheritance to FALSE.
                              0,                // No creation flags.
                              NULL,             // Use parent's environment block.
                              NULL,             // Use parent's starting directory.
                              &si,              // Pointer to STARTUPINFO structure.
                              &pi);             // Pointer to PROCESS_INFORMATION structure.

    // Close handles returned immediately since we aren't using them.
    if ( bSuccess )
    {
        CloseHandle (pi.hProcess);
        CloseHandle (pi.hThread);
    }

    return bSuccess;
}
#endif