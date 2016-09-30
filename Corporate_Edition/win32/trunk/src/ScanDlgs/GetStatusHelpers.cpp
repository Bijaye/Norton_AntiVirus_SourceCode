
#include "GetStatusHelpers.h" 
#include "resource.h" // for IDS_ macros
// 
// Determine if we're trying to restore to a remote machine by
// comparing the local machine name with the logged one.
//
// @param - sRemoteComputer - Remote computer name
//
BOOL IsLocalMachine( CString sRemoteComputer )
{
	TCHAR szLocalComputer[MAX_PATH];
	DWORD dwSize = sizeof(szLocalComputer);
	BOOL bReturn = FALSE;

	if( GetComputerName( szLocalComputer, &dwSize ) )
	{
		if( 0 ==_tcscmp( szLocalComputer, sRemoteComputer.GetBuffer(0) ) )
		{
			bReturn = TRUE;
		}
	}

	return bReturn;
}

// 
// Display a warning to the user that the file will be restored locally.
//
BOOL WarnAndRestore()
{
	CString sMsg;
	DWORD dwRtn = 0;
	BOOL bReturn = FALSE;
	sMsg.LoadString( IDS_WARN_AND_RESTORE );

	dwRtn = AfxMessageBox( sMsg, MB_YESNO|MB_ICONINFORMATION );

	if( IDYES == dwRtn )
	{
		bReturn = TRUE;
	}

	return bReturn;
}

// 
// Create the directory that does not exist
//
// @param - lpFullPath - Full path string
//
BOOL CreateMissingDirectory(LPCTSTR lpFullPath)
{
    int     iDelim = 0;
    CString sFullPath = lpFullPath;
    CString sDir;
    BOOL    bRet = FALSE;
    DWORD   dwError;

    // Strip the file name
    iDelim = sFullPath.ReverseFind( TCHAR('\\') );
    if ( iDelim != -1 )
    {
        sDir = sFullPath.Left( iDelim );
        bRet = ::CreateDirectory( lpFullPath, NULL );
        if ( !bRet )
        {
            dwError = GetLastError();
            if ( dwError == ERROR_PATH_NOT_FOUND )
            {
                if ( CreateMissingDirectory((char *)LPCTSTR(sDir)) )
                {
                    bRet = ::CreateDirectory( lpFullPath, NULL );
                }
            }
        }
    }
    return bRet;
}