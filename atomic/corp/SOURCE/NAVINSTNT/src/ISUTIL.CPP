/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1997 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/ISUTIL.CPv   1.8   03 Jun 1998 17:31:40   mdunn  $
//
//  ISUtil.CPP -    Module that holds various utility function for the
//                  InstallShield Uninstall of NAVNT
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/ISUTIL.CPv  $
// 
//    Rev 1.8   03 Jun 1998 17:31:40   mdunn
// Added Quarantine and .GID cleanup (and remembered to remove all the int 3's
// this time!)
// 
//    Rev 1.7   03 Aug 1997 20:25:46   SEDWARD
// Now uses the "Symantec Shared" global variable.
//
//    Rev 1.6   20 Jul 1997 19:06:08   SEDWARD
// Open reg keys with ALL_ACCESS instead of WRITE_ACCESS.
//
//    Rev 1.5   18 Jul 1997 18:57:50   JALLEE
// Ported Bruce H's update to the registration and usage count utilities from the
// BORG project.  Many changes, but should facilitate sharing bug fixes / updates.
//
//    Rev 1.4   30 Jun 1997 15:21:38   SEDWARD
// Moved GetUsageCount() and GetServicePath() to navuninst.cpp.
//
//    Rev 1.3   22 Jun 1997 23:00:10   SEDWARD
// Added GetServicePath() and GetUsageCount() functions.
//
//    Rev 1.2   09 Jun 1997 20:47:02   ksackin
// Updated variable length argument wvsprintf for DEC Alpha platform.
//
//    Rev 1.1   11 May 1997 21:30:40   SEDWARD
// Changed SI_MAX_PATH to SYM_MAX_PATH; moved global vars.
//
//    Rev 1.0   05 May 1997 19:24:54   sedward
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef SYM_WIN32
#define SYM_WIN32
#endif

#include <windows.h>
#include <windowsx.h>

#pragma hdrstop

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "NavInst.h"
#include "IsUtil.h"
#include "IsRegUti.h"
#include "resource.h"
#include "cwstring.h"
#include "cregent.h"
#include "cusgcnt.h"


/////////////////////////////////////////////////////////////////////////////
//
//  DeleteDirectoryContents() - Function that will remove all files in a
//                              directory and try to remove the directory
//
//  Params: LPTSTR - Pointer to the directory name
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI DeleteDirectoryContents( LPTSTR lpDirectory )
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    CWString            szFile( SYM_MAX_PATH );
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

                SetFileAttributes( szFile, FILE_ATTRIBUTE_NORMAL );
                ISDeleteFile( szFile );
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
//  GetBaseDir() - Function to retrieve the base path to the installation
//                 directory
//
//  Params: LPTSTR - Pointer to buffer to hold result
//          UINT   - Size of buffer
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI GetBaseDir(LPTSTR lpBuffer, UINT uBufSize)
{
    BOOL                bRet = FALSE;
    LPSTR               lpToken;
    LPTSTR              lpCmdLine;
    CWString            szTemp(SYM_MAX_PATH);

    lpCmdLine = GetCommandLine();
    lstrcpy( szTemp, lpCmdLine );

    lpToken = _tcsstr( szTemp, "-c" );

    if ( lpToken != NULL )
    {
        lpToken = CharNext( lpToken );
        lpToken = CharNext( lpToken );

        if (( IsDBCSLeadByte( *lpToken ) == 0 ) && ( *lpToken == '\"' ))
            ++lpToken;

        lstrcpy( lpBuffer, lpToken );
        int nLen = lstrlen( lpBuffer );

        if ( *(lpToken + (nLen - 1)) == '"' )
            *(lpToken + (nLen - 1)) = 0;

        lstrcpy( szTemp, lpToken );
        szTemp.StripFileName();
        lstrcpy( lpBuffer, szTemp );
        bRet = TRUE;
    }


    return( bRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  ProcessUsageCountsFile - Function to decrement the usage counts under the
//                           symantec sharedusage key
//
//  Params: LPTSTR - LogFile section name
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI ProcessUsageCountsFile( LPTSTR lpLogSect, LPTSTR lpLogFile  )
{
    int             nLen;
    BOOL            bRet = FALSE;
    FILE           *fpFile = NULL;
    LPTSTR          lpLine;
    CWString        szLogFile( SYM_MAX_PATH );
    CWString        szPath( SYM_MAX_PATH );
    CWString        szFile( SYM_MAX_PATH );
    CWString        szTemp( SYM_MAX_PATH );


    LTrace( "BEGIN ProcessUsageCountFile( %s )", lpLogSect );

    GetNavPaths(g_szTarget, g_szSharedDir, g_szSymSharedDir);
    lstrcpy( szLogFile, g_szTarget );
    szLogFile.AppendFile( lpLogFile );
    fpFile = fopen( szLogFile, "rt" );

    if ( fpFile != NULL && lpLogSect != NULL )
    {
        lpLine = LogFileFindSection( fpFile, lpLogSect );

        if ( lpLine != NULL )
        {
            do
            {
                LPSTR           lpTemp;

                lstrcpy( szPath, lpLine );
                lstrcpy( szTemp, szPath );

                szPath.StripFileName();
                nLen = szPath.StrLen();

                if ( nLen )
                {
                    lstrcpy( szTemp, lpLine );
                    lpTemp = szTemp + nLen;
                    lpTemp++;
                    szFile = lpTemp;
                }
                else
                    break;

                bRet = DecrementUsageCount( szFile, szPath, NAVNT_ID );

                lpLine = LogFileGetNextLine( fpFile );
            }                           // end of do
            while( lpLine != NULL );
        }

        fclose( fpFile );
    }
    else
        LTrace( "   Logfile open error" );

    LTrace( "END ProcessUsageCountFile( %d )\n", bRet );

    return( bRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  DecrementUsageCounts - Function to decrement a single usage count under
//                         both the symantec shared usage key and the ms
//                         shared dlls key
//
//  Params: LPTSTR - LogFile section name
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

int WINAPI DecrementUsageCount( LPTSTR lpFile, LPTSTR lpPath, LPTSTR lpId )
{
    int             nRet = -1;
    DWORD           dwUsage;
    DWORD           dwValues;
    CWString        szValue( SYM_MAX_PATH );
    CWString        szSymKey( SYM_MAX_PATH );
    CUsageCount    *pcUsg;
    CRegistryEntry *pcReg;

    if ( lpFile == NULL || lpPath == NULL || lpId == NULL )
        LTrace( "BEGIN DecrementUsageCount( Invalid Parameter Passed )" );
    else
    {
        LTrace( "BEGIN DecrementUsageCount( %s, %s, %s )", lpFile, lpPath, lpId );

                                        // Create the key name under the symantec
                                        // shared key
        szSymKey = SYM_SHAREDUSAGE_KEY;
        szSymKey.AppendFile( lpFile );

                                        // Create the value name under the MS shared
                                        // dlls key
        szValue = lpPath;
        szValue.AppendFile( lpFile );

                                        // Get current MS usage count

        pcUsg = new CUsageCount( HKEY_LOCAL_MACHINE, MS_DLLS_KEY, szValue );
        dwUsage = pcUsg->DecrementCount( TRUE );
        delete pcUsg;

                                        // Now handle the symantec sharedusage key

        pcReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, szSymKey, lpId );

        if ( pcReg->OpenKey( KEY_ALL_ACCESS ) == ERROR_SUCCESS )
        {
            pcReg->DeleteValue();
            dwValues = pcReg->GetValueCount();

            if ( dwValues <= 1 )
                pcReg->DeleteKey();
        }

        delete pcReg;

        if ( dwUsage == 0 )
            ISDeleteFile( szValue );

        nRet = (LONG)dwUsage;
    }

    LTrace( "END DecrementUsageCount( %d )\n", nRet );

    return( nRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  LogFileFindSection - Function to retrieve the first line of the given
//                       section in the uninstall log file
//
//  Params: FILE* - File pointer of the logfile
//          LPTSTR - Section name to look for
//
//  Return value:   LPTSTR to the line or NULL if EOF or end of section
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

LPTSTR WINAPI LogFileFindSection( FILE *fpFile, LPTSTR lpSection )
{
    BOOL            bSecFound = FALSE;
    LPSTR           lpToken = NULL;
    LPTSTR          lpRet = NULL;
    CWString        szCurLine( SYM_MAX_PATH );
    static char     szRetLine[ SYM_MAX_PATH ];

    LTrace( "BEGIN LogFileFindSection( %s )", lpSection );

    if ( fpFile != NULL )
    {
        rewind( fpFile );

        while( fgets( szCurLine, SYM_MAX_PATH - 1, fpFile ) != NULL )
        {
            int     nLen = lstrlen( szCurLine );

            if ( nLen > 0 )
            {
                nLen--;

                while( (nLen >= 0) &&
                       (szCurLine[nLen] == '\n' || szCurLine[nLen] == '\r') )
                {
                    szCurLine[nLen] = 0;

                    if ( nLen > 0 )
                        nLen--;
                    else
                        break;
                }

                nLen = lstrlen( szCurLine );

                if ( nLen > 0 )
                {
                    if ( *szCurLine == '[' )
                    {
                        char           *pPtr = szCurLine;

                        if ( bSecFound )
                            break;

                        pPtr++;
                        lpToken = strtok( pPtr, "]" );

                        if ( lpToken == NULL )
                            break;

                        if ( !lstrcmpi( lpToken, lpSection ) )
                        {
                            bSecFound = TRUE;
                            continue;
                        }
                    }
                    else
                    {
                        if ( bSecFound )
                        {
                            lstrcpy( szRetLine, szCurLine );
                            lpRet = szRetLine;
                            break;
                        }
                    }

                }
            }
        }                               // End of while loop
    }                                   // end of if fpFile != NULL

    LTrace( "END LogFileFindSection( %s )\n", lpRet == NULL ? "NULL" : lpRet );

    return( lpRet );
}



/////////////////////////////////////////////////////////////////////////////
//
//  LogFileGetNextLine - Function to retrieve the next line of the given
//                       section in the uninstall log file
//
//  Params: FILE* - File pointer of the logfile
//
//  Return value:   LPTSTR to the line or NULL if EOF or end of section
//
/////////////////////////////////////////////////////////////////////////////
//  02/13/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

LPSTR WINAPI LogFileGetNextLine( FILE *fpFile )
{
    LPTSTR          lpRet = NULL;
    CWString        szCurLine( SYM_MAX_PATH );
    static char     szRetLine[ SYM_MAX_PATH ];

    LTrace( "BEGIN LogFileGetNextLine()" );

    if ( fpFile != NULL )
    {
        while( fgets( szCurLine, SYM_MAX_PATH - 1, fpFile ) != NULL )
        {
            int     nLen = lstrlen( szCurLine );

            if ( nLen > 0 )
            {
                nLen--;

                while( (nLen >= 0) &&
                       (szCurLine[nLen] == '\n' || szCurLine[nLen] == '\r') )
                {
                    szCurLine[nLen] = 0;

                    if ( nLen > 0 )
                        nLen--;
                    else
                        break;
                }

                LTrace( "   Line Retrieved( %s )", (LPSTR)szCurLine );

                nLen = lstrlen( szCurLine );

                if ( nLen > 0 )
                {
                    if ( *szCurLine == '[' )
                    {
                        LTrace( "   New section found" );
                        break;
                    }
                    else
                    {
                        lstrcpy( szRetLine, szCurLine );
                        lpRet = szRetLine;
                        break;
                    }
                }
            }
        }
    }

    LTrace( "END LogFileGetNextLine( %s )\n", lpRet == NULL ? "NULL" : lpRet );
    return( lpRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  DirectoryExists - Function to determine if a given directoryexists
//
//  Params: LPTSTR - Path to look for
//
//  Return value:   TRUE if dir is found
//
/////////////////////////////////////////////////////////////////////////////
//  06/05/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DirectoryExists( LPTSTR lpPath )
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    WIN32_FIND_DATA     fd;

    hFind = FindFirstFile( lpPath, &fd );

    if ( hFind != INVALID_HANDLE_VALUE )
    {
        if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            bRet = TRUE;

        FindClose( hFind );
    }

    return( bRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  FileExists - Function to determine if a given file exists
//
//  Params: LPTSTR - Path and filename to look for
//
//  Return value:   TRUE if file is found
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI FileExists( LPTSTR lpFile )
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    WIN32_FIND_DATA     fd;

    hFind = FindFirstFile( lpFile, &fd );

    if ( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            if ( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                 *fd.cFileName != '.' && *fd.cAlternateFileName != '.' )
            {
                bRet = TRUE;
                break;
            }
        }
        while( FindNextFile( hFind, &fd ) );

        FindClose( hFind );
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
    CWString            szDir( SYM_MAX_PATH );
    CWString            szTemp( SYM_MAX_PATH );
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
//  LaunchAndWait() - Function to launch a process and wait for it to finish
//
//  Params: LPSTR - App to execute
//          LPSTR - Command line params
//
//  Return value:   TRUE if app successfully launched
//
/////////////////////////////////////////////////////////////////////////////
//  02/16/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI LaunchAndWait( LPSTR lpExe, LPSTR lpParams )
{
    BOOL                    bRet = FALSE;
    CWString                szLaunch( SYM_MAX_PATH * 2 );
    STARTUPINFO             sInfo;
    PROCESS_INFORMATION     piInfo;

    LTrace( "BEGIN LaunchAndWait( %s, %s )", lpExe == NULL ? "NULL" : lpExe,
            lpParams == NULL ? "NULL" : lpParams );

    memset( &sInfo, 0, sizeof( STARTUPINFO ) );
    memset( &piInfo, 0, sizeof( PROCESS_INFORMATION ) );

    sInfo.cb = sizeof( STARTUPINFO );

    if ( lpExe != NULL && *lpExe )
    {
        lstrcpy( szLaunch, lpExe );
        GetShortPathName( szLaunch, szLaunch, SYM_MAX_PATH - 1 );

        if ( lpParams != NULL && *lpParams )
        {
            lstrcat( szLaunch, " " );
            lstrcat( szLaunch, lpParams );
        }
    }
    else
        lstrcpy( szLaunch, lpParams );


    if ( CreateProcess( NULL, szLaunch, NULL, NULL, FALSE,
                        NORMAL_PRIORITY_CLASS,
                        NULL, NULL, &sInfo, &piInfo ) )
    {
        WaitForSingleObject( piInfo.hProcess, INFINITE );
        bRet = TRUE;
    }
    else
    {
        LTrace( "   CreateProcess failed %08X", GetLastError() );
    }

    LTrace( "END LaunchAndWait( %d )\n", bRet );

    return( bRet );
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

BOOL WINAPI FileInUseHandler( LPTSTR lpName )
{
    BOOL                    bRet = FALSE;
    CWString                szShort( SYM_MAX_PATH );

    GetShortPathName( lpName, szShort, SYM_MAX_PATH - 1 );

//    if ( CheckOS() == OS_ID_WINNT )
//    {
        MoveFileEx( szShort, NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
        bRet = TRUE;
//    }
//    else // win95
    return( bRet );
}


/////////////////////////////////////////////////////////////////////////////
//
//  LTrace() - Function to trace output to a log fle put in the windows dir
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
    FILE           *fp = NULL;
    DWORD           dwNumBytes = 0L;
    CWString        szBuffer( SI_MAXSTR );
    CWString        szLogFile( SYM_MAX_PATH );
    va_list         lpArgs;

    va_start( lpArgs, lpszFmt );

    wvsprintf( (LPSTR)szBuffer, lpszFmt, lpArgs );
    lstrcat( szBuffer, "\n" );


#ifdef _LOGFILE_TRACE_
    GetWindowsDirectory( szLogFile, SI_MAXSTR - 1 );
    szLogFile.AppendFile( "SYMINST.LOG" );

    fp = fopen( szLogFile, "at" );

    if ( fp != NULL )
    {
        fputs( szBuffer, fp );
        fflush( fp );
        fclose( fp );
    }
#else
    OutputDebugString( szBuffer );
#endif

    return;
}
