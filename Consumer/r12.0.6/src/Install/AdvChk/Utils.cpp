#include "stdafx.h"
#include "advchk.h"
#include "cwstring.h"
#include "utils.h"


extern HINSTANCE ghInstance;  //AdvChk.cpp

//Checks to see if NU is installed
BOOL IsNUInstalled(LPTSTR szPath /* = NULL*/, DWORD& dwSize)
{
	DWORD item = 0;

	while(NULL != NUInstalledAppList[item])
	{
		if( TRUE == IsAppInstalled(NUInstalledAppList[item], szPath, dwSize))
			return TRUE;
		
		item++;
	}
	return FALSE;
}

BOOL IsAppInstalled(LPCTSTR szAPPID, LPTSTR szPath, DWORD &dwSize)
{
	// Find InstalledApps key and return value if they want it
    
    CRegKey key;
    if(REG_SUCCEEDED(key.Open(HKEY_LOCAL_MACHINE,REGKEY_INSTALLEDAPPS)))
    {
        if(REG_SUCCEEDED(RegQueryValueEx(key, szAPPID, NULL, NULL, LPBYTE(szPath), &dwSize)))
        {
            return TRUE;
        }
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
//
//  LTrace() - Function to trace output to a log file put in the windows dir
//
//  Params: LPCSTR - Format string
//          Variable Args - To be substituted into format string
//
//  Return value:   VOID
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

VOID CDECL LTrace( LPCSTR lpszFmt, ... )
{
    FILE *fp = NULL;
    DWORD dwNumBytes = 0L;
    TCHAR szBuffer[SI_MAXSTR] = {0};
    TCHAR szLogFile[SI_MAX_PATH] = {0};

    LPSTR lpArgs = (LPSTR)&lpszFmt + sizeof( lpszFmt );
    wvsprintf( (LPSTR)szBuffer, lpszFmt, lpArgs );
    lstrcat( szBuffer, "\n" );

    GetWindowsDirectory( szLogFile, SI_MAXSTR - 1 );
	_tcscat(szLogFile, _T("\\MININU.LOG"));

    fp = fopen( szLogFile, "at" );

    if ( fp != NULL )
    {
        fputs( szBuffer, fp );
        fflush( fp );
        fclose( fp );
    }
    
    return;
}


BOOL IsOSNT()
{
	OSVERSIONINFO OsVersion;
	ZeroMemory(&OsVersion, sizeof(OSVERSIONINFO));
	OsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if(FALSE == GetVersionEx(&OsVersion))
	{
		OsVersion.dwPlatformId = VER_PLATFORM_WIN32_NT;
	}

	return((OsVersion.dwPlatformId == VER_PLATFORM_WIN32_NT) ? TRUE : FALSE);
}


BOOL GetModuleDirectory(LPTSTR szPath)
{

	TCHAR szModName[MAX_PATH]	= {0};
	TCHAR szDir[MAX_PATH]	= {0};

	if(0 == GetModuleFileName(ghInstance, szModName, MAX_PATH))
		return(FALSE);

	_tsplitpath(szModName, szPath, szDir, NULL, NULL);
	_tcscat(szPath, szDir);

	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//
//  FileInUseHandler() - Function to handle a file-in-use properly under the
//                       appropriate OS for file/directory deletions
//
//  Params: LPSTR - File or directory to delete
//
//  Return value:   TRUE if successfully scheduled for deletion
//
/////////////////////////////////////////////////////////////////////////////
//  05/19/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////
//NOTE:  this function was copied directly from the working NU install
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI FileInUseHandler( LPTSTR lpName )
{
    BOOL                    bRet = FALSE;
    FILE                    *fpFile;
    LPTSTR                  lpValue = "NSI";
    LPTSTR                  lpSection = "Rename";
    TCHAR					szFile[MAX_PATH ] = {0};
    TCHAR					szLine[MAX_PATH * 2 ] = {0};
    TCHAR					szShort[MAX_PATH ] = {0};

    
	if( NULL == GetShortPathName( lpName, szShort, SI_MAX_PATH - 1 ) )
	{
		return FALSE;
	}

    if (IsOSNT())
	{	
		if(0 != MoveFileEx( szShort, NULL, MOVEFILE_DELAY_UNTIL_REBOOT ))
			bRet = TRUE;
	}
	else
    {
        GetWindowsDirectory( szFile, SI_MAX_PATH - 1 );
        _tcscat(szFile, _T("\\WININIT.INI"));

        WritePrivateProfileString( lpSection, lpValue, szShort, szFile );
        WritePrivateProfileString( NULL, NULL, NULL, szFile );


        fpFile = fopen( szFile, "rt" );

        if ( fpFile != NULL )
        {
            fpos_t          fpos;

            do
            {
                fgetpos( fpFile, &fpos );
                fgets( szLine, SI_MAX_PATH * 2, fpFile );

                if ( !_strnicmp( szLine, lpValue, 3 ) )
                {
                    fflush( fpFile );
                    fclose( fpFile );

                    fpFile = fopen( szFile, "r+t" );

                    if ( fpFile != NULL )
                    {
                        fsetpos( fpFile, &fpos );
                        fprintf( fpFile, "NUL=%s\n", (LPTSTR)szShort );
                        fflush( fpFile );
                        fclose( fpFile );
                        fpFile = NULL;
                    }
                    break;
                }
            }
            while( !feof( fpFile ) );

        if ( fpFile != NULL )
            fclose( fpFile );

        bRet = TRUE;
        }
    }

    return( bRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  ISDeleteFile() - Function to try and delete a file which will queue it
//                   up for processing after rebooting
//
//  Params: LPSTR - File to delete
//
//  Return value:   IS_FILE_DELETED or
//                  IS_FILE_IN_USE or
//                  IS_FILE_NOT_FOUND
//
/////////////////////////////////////////////////////////////////////////////
//  02/16/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ISDeleteFile( LPTSTR lpFile )
{
    DWORD               dwRet = IS_FILE_NOT_FOUND;
    DWORD               dwError = 0;
    HANDLE              hFind;
    CWString            szDir( SI_MAX_PATH );
    CWString            szTemp( SI_MAX_PATH );
    WIN32_FIND_DATA     fd;

    LTrace( "BEGIN ISDeleteFile( %s )", lpFile );

    if ( lpFile != NULL && *lpFile )
    {
        szDir = lpFile;
        szDir.StripFileName();

        hFind = FindFirstFile( lpFile, &fd );

        if ( hFind != INVALID_HANDLE_VALUE )
        {
            do
            {
                if ( fd.cFileName[0] != '.' )
                {
                    szTemp = szDir;
                    szTemp.AppendFile( fd.cFileName );

                    if ( DeleteFile( szTemp ) )
                        dwRet = IS_FILE_DELETED;
                    else
                    {
                        dwError = GetLastError();

                        LTrace( "   Error deleting file %08X", dwError );

                        if ( dwError != ERROR_FILE_NOT_FOUND )
                        {
                            if ( dwError == ERROR_SHARING_VIOLATION ||
                                 dwError == ERROR_ACCESS_DENIED )
                            {
                                dwRet = IS_FILE_IN_USE;
                                FileInUseHandler( lpFile );
                                LTrace( "   File queued for deletion" );
                            }
                        }
                    }
                }
            }
            while( FindNextFile( hFind, &fd ) );

            FindClose( hFind );
        }
    }

    LTrace( "END ISDeleteFile( %08X )\n", dwRet );

    return( dwRet );
}


/////////////////////////////////////////////////////////////////////////////
//
//  ISRemoveDirectory() - Function that will remove a directory or queue it
//                        for deletion after rebooting
//
//  Params: LPSTR - Directory to remove
//
//  Return value:   IS_DIR_REMOVED
//                  IS_DIR_NOT_FOUND
//                  IS_DIR_NOT_EMPTY
//
/////////////////////////////////////////////////////////////////////////////
//  02/16/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI ISRemoveDirectory( LPTSTR lpDir )
{
    DWORD       dwRet = IS_DIR_NOT_FOUND;
    DWORD       dwError = 0;

    LTrace( "BEGIN ISRemoveDirectory( %s )", lpDir );

    if ( lpDir != NULL && *lpDir )
    {
        if ( RemoveDirectory( lpDir ) )
            dwRet = IS_DIR_REMOVED;
        else
        {
            dwError = GetLastError();

            LTrace( "   Error removing directory %08X", dwError );

            if ( dwError != ERROR_PATH_NOT_FOUND &&
                 dwError != ERROR_FILE_NOT_FOUND )
            {
                dwRet = IS_DIR_NOT_EMPTY;
                FileInUseHandler( lpDir );
                LTrace( "   Directory queued for removal" );
            }
        }
    }

    LTrace( "END ISRemoveDirectory( %08X )\n", dwRet );

    return( dwRet );
}
/////////////////////////////////////////////////////////////////////////////
//
//  DeleteDirectoryContents() - Function that will remove all files in a
//                              directory and try to remove the directory
//
//  Params: LPTSTR - Pointer to the directory name
//			BOOL - Boolean specifying whether subdirectories should be deleted
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DeleteDirectoryContents( LPTSTR lpDirectory, BOOL bRecursive)
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    CWString            szFile( SI_MAX_PATH );
    WIN32_FIND_DATA     fd;

    LTrace( "BEGIN DeleteDirectoryContents( %s )", lpDirectory );

    SetFileAttributes( lpDirectory, FILE_ATTRIBUTE_NORMAL );

    lstrcpy( szFile, lpDirectory );
    szFile.AppendFile( "*.*" );

    hFind = FindFirstFile( szFile, &fd );

    if ( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            lstrcpy( szFile, lpDirectory );

            if ( fd.cFileName[0] != '.' &&
                 fd.cAlternateFileName[0] != '.' )
            {				
					szFile.AppendFile( fd.cFileName[0] ? fd.cFileName :
									fd.cAlternateFileName );
	
					if (bRecursive && fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
						DeleteDirectoryContents( szFile, TRUE);
					else
					{					
						SetFileAttributes( szFile, FILE_ATTRIBUTE_NORMAL );
						ISDeleteFile( szFile );
					}
            }
        }
        while( FindNextFile( hFind, &fd ) );

        FindClose( hFind );
    }

    ISRemoveDirectory( lpDirectory );

    bRet = TRUE;

    LTrace( "END DeleteDirectoryContents( %d )", bRet );

    return( bRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  RemoveNProtectDirectory - Function to remove all the NPROTECT directories
//                            from the hard drives
//
//  Params: NONE
//
//  Return value:  TRUE if at least one NPROTECT directory is removed
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI RemoveNProtectDirectory( VOID )
{
    char                szDriveRoot[5];
    BOOL                bRet = TRUE;
    CWString            szBuffer( SI_MAX_PATH );

    LTrace( "BEGIN - RemoveNprotectDirectory()" );

    for(char cDrive = 'A'; cDrive <= 'Z'; cDrive++)
    {
        lstrcpy( szDriveRoot," :\\" );
        szDriveRoot[0] = cDrive;

        switch( GetDriveType(  szDriveRoot ) )
        {
            case DRIVE_FIXED:
                _tcscpy( szBuffer, UNERASE_PROT_DIR);
                szBuffer[0] = cDrive;
                DeleteDirectoryContents( szBuffer );

            default:
                break;
        }
    }

//    FuncTrace( "RemoveNProtectDirectory", bRet );

    return( bRet );
}
