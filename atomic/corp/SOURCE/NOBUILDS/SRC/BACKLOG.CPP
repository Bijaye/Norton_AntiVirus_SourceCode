///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NOBUILDS/VCS/Backlog.cpv   1.0   15 Oct 1998 16:29:04   mdunn  $
//
// BACKLOG.CPP   - Application to backup the borg uninstall log file
//
//
///////////////////////////////////////////////////////////////////////////
//  Modified so it works fine on NT as well as 9X.  The added code removes
//  a value from the run key.  The original version was launched from
//  the RunOnce key.  This key does not work under NT.  The new version
//  is launched from the Run key, and it removes itself when run.
//
//  Modified 9/22/98 by Vance Gloster.
///////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#define BL_MAX_PATH         264
#define UNINST_LOGFILE      "UNINST.ISU"
#define VALUE_NAME			"BACKLOG"
#define KEY_NAME			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"


BOOL WINAPI FileCat( LPTSTR lpPath, LPTSTR lpFile );
BOOL WINAPI FileExists( LPTSTR lpFile );
BOOL RemoveKeyValue( LPCTSTR szKeySource, LPCTSTR szValue );

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    int             nCtr = 0;
    int             nRet = 0;
    TCHAR           szFile[ BL_MAX_PATH ];
    TCHAR           szBackup[ BL_MAX_PATH ];
//    TCHAR           szName[ 25 ];
    TCHAR           *pFilename, *pExt;
    LPTSTR          lpFmt = ".%03d";

	// Remove the run key value
	RemoveKeyValue( KEY_NAME, VALUE_NAME );    

    if ( lpCmdLine != NULL && *lpCmdLine )
    {
        lstrcpy( szFile, lpCmdLine );
//        FileCat( szFile, UNINST_LOGFILE );
        
        if ( FileExists( szFile ) )
        {
            lstrcpy ( szBackup, szFile );
            pFilename = _tcsrchr ( szBackup, '\\' );
            pExt = _tcsrchr ( szBackup, '.' );

            if ( pExt < pFilename )
                {
                pExt = _tcschr ( szBackup, '\0' );
                }

            wsprintf ( pExt, lpFmt, nCtr );

//            wsprintf( szName, lpFmt, nCtr );
//            lstrcpy( szBackup, lpCmdLine );
//            FileCat( szBackup, szName );
            
            while( FileExists( szBackup ) )
            {
                nCtr++;
                wsprintf( pExt, lpFmt, nCtr );
//                lstrcpy( szBackup, lpCmdLine );
//                FileCat( szBackup, szName );
            }
            
            if ( !CopyFile( szFile, szBackup, FALSE ) )
                nRet = 3;
        }
        else
            nRet = 2;
    }
    else
        nRet = 1;
        
        
        
    return( nRet );
}
    
    
/////////////////////////////////////////////////////////////////////////////
//
//  FileCat - Concatenates a filename to the given path
//
//  Params: LPTSTR - Path to append to
//          LPTSTR - Filename to append
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  11/05/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////
    
BOOL WINAPI FileCat( LPTSTR lpPath, LPTSTR lpFile )
{
    BOOL            bRet = TRUE;
    LPTSTR          lpPtr;
    
    lpPtr = lpPath;
                        
    while( *lpPtr != 0 )
        lpPtr = CharNext( lpPtr );
        
    lpPtr = CharPrev( lpPath, lpPtr );
    
    if ( *lpPtr != '\\' )
        lstrcat( lpPath, "\\" );
        
    lstrcat( lpPath, lpFile );
    
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

/**********************************************************
* FUNCTION: 	RemoveKeyValue
*
* PARAMETERS:	LPCTSTR szKeySource
*				LPCTSTR szValue
* RETURNS:		BOOL
* AUTHOR:		Vance Gloster
* DESCRIPTION:	Removes a value from a key
* 				
***********************************************************/
BOOL RemoveKeyValue(LPCTSTR szKeySource, LPCTSTR szValue)
{	BOOL  bRes = FALSE;
	LONG  lRes;
	HKEY  hKey;

	lRes = RegOpenKey(HKEY_LOCAL_MACHINE, szKeySource, &hKey);
	if(lRes != ERROR_SUCCESS)
		goto exit;
	lRes = RegDeleteValue(hKey, szValue);
	if(lRes == ERROR_SUCCESS)
		bRes = TRUE;
	RegCloseKey(hKey);
exit:
	return bRes;
}

    

