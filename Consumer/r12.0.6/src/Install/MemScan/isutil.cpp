#include "stdafx.h"
#include "isutil.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
//
//  DirectoryExists - Function to determine if a given directoryexists
//
//  Params: LPTSTR - Path to look for
//
//  Return value:   TRUE if dir is found
//
/////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DirectoryExists(LPTSTR szDir)
{
    BOOL                bRet = FALSE;
    HANDLE              hFind;
    WIN32_FIND_DATA     fd;

    hFind = FindFirstFile(szDir, &fd);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            bRet = TRUE;

        FindClose(hFind);
    }

    return(bRet);
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

DWORD WINAPI ISRemoveDirectory(LPTSTR lpDir)
{
    DWORD dwRet = IS_DIR_NOT_FOUND;
    DWORD dwError = 0;

    if ( lpDir != NULL && *lpDir )
    {
        if (RemoveDirectory(lpDir))
            dwRet = IS_DIR_REMOVED;
        else
        {
            dwError = GetLastError();

            if ( dwError != ERROR_PATH_NOT_FOUND &&
                 dwError != ERROR_FILE_NOT_FOUND )
            {
				dwRet = IS_DIR_NOT_EMPTY;                
            }	
        }
	}

    return( dwRet );
}



/////////////////////////////////////////////////////////////////////////////
//
//  ExtractFiles() - Function extracts the files from the resource.
//
//  Params:		HINSTANCE - handle to program.
//				LPTSTR - The temp directory that the files are copied to.
//
//  Return value:   TRUE - if files successfully extracted.
//                  FALSE - if files failed to extract.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ExtractFiles(HINSTANCE hInst, LPTSTR szTempFolder)
{
    BOOL bRet = TRUE;

	if(DirectoryExists(szTempFolder) == 0)
	{
		 CreateDirectory(szTempFolder, NULL);	 
	}

    if (CreateResourceFile(hInst, IDR_INSTSCAN_DLL, RESOURCE_TYPE, _T("instscan.dll"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }

    if (CreateResourceFile(hInst, IDR_N32CALL_DLL, RESOURCE_TYPE, _T("n32call.dll"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }

	if (CreateResourceFile(hInst, IDR_NAVEX32A_DLL, RESOURCE_TYPE, _T("navex32a.dll"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }
	
	if (CreateResourceFile(hInst, IDR_NAVKRNLO_VXD, RESOURCE_TYPE, _T("navkrnlo.vxd"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }
	if (CreateResourceFile(hInst, IDR_S32NAVO_DLL, RESOURCE_TYPE, _T("s32navo.dll"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }
	if (CreateResourceFile(hInst, IDR_VIRSCAN1_DAT, RESOURCE_TYPE, _T("virscan1.dat"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }
	if (CreateResourceFile(hInst, IDR_VIRSCAN2_DAT, RESOURCE_TYPE, _T("virscan2.dat"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }
	if (CreateResourceFile(hInst, IDR_VIRSCAN3_DAT, RESOURCE_TYPE, _T("virscan3.dat"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }
	if (CreateResourceFile(hInst, IDR_VIRSCAN4_DAT, RESOURCE_TYPE, _T("virscan4.dat"), szTempFolder ) != ERROR_SUCCESS)
    {
        bRet = FALSE;
    }
    return(bRet);
}



///////////////////////////////////////////////////////////////////////////
//
// CreateResourceFile
//
// Params:
//          HINSTANCE - Instance of the program
//          UINT - Resource ID
//          LPTSTR - Resource Type
//			LPTSTR - Resource Name
//          LPTSTR - Output File Name
//
// Returns: ERROR_SUCCESS or error code
//
///////////////////////////////////////////////////////////////////////////

DWORD WINAPI CreateResourceFile( HINSTANCE hInst, UINT uResourceId,
                                 LPTSTR lpType, LPTSTR szFileName, LPTSTR lpOutputFile )
{
    DWORD  dwReturn = 1;
    DWORD  dwSize;
    DWORD  dwWritten;
    HRSRC  hRes;
    LPBYTE lpData;
    HANDLE hFile;
	TCHAR  szFullPath[MAX_PATH]={0};

	
	wsprintf(szFullPath, _T("%s\\%s"), lpOutputFile, szFileName);

    hRes = FindResource( hInst, (LPCTSTR)uResourceId, lpType );

    if ( hRes == NULL )
        MessageBox(NULL, "Can't Find resource file", NULL, MB_OK);
    else    
    {
        dwSize = SizeofResource( hInst, hRes );

        lpData = (LPBYTE)LoadResource( hInst, hRes );

        if ( lpData == NULL )
            MessageBox(NULL, "Error - LoadResource failed on resource", NULL, MB_OK);
			
        else
        {
            hFile = CreateFile( szFullPath, GENERIC_WRITE, 0, NULL,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

            if ( hFile == INVALID_HANDLE_VALUE )
            {
                DWORD dwError = GetLastError();
				MessageBox(NULL, "Error - CreateFile failed. Unable to get valid handle", NULL, MB_OK);
				
            }
            else
            {
                if ( WriteFile( hFile, (LPVOID)lpData, dwSize,
                                &dwWritten, NULL ) )
                    dwReturn = ERROR_SUCCESS;
                else
                {
                    dwReturn = GetLastError();
					MessageBox(NULL, "Error - WriteFile failed", NULL, MB_OK);			
                }

                CloseHandle( hFile );
            }
        }
    }


    return(dwReturn);
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

DWORD WINAPI ISDeleteFile(LPTSTR szDir)
{
    DWORD	dwRet = IS_FILE_NOT_FOUND;
    DWORD   dwError = 0;
    HANDLE  hFind;
	TCHAR szMemScanFiles[MAX_PATH] = {0};
    TCHAR szFileName[MAX_PATH] = {0};
	

    WIN32_FIND_DATA     fd;

	wsprintf(szMemScanFiles,  _T("%s\\*.*"), szDir);

    if (szMemScanFiles != NULL && *szMemScanFiles)
    {
       
        hFind = FindFirstFile(szMemScanFiles, &fd);

        if ( hFind != INVALID_HANDLE_VALUE )
        {
            do
            {
                if ( fd.cFileName[0] != '.' )
                {

					wsprintf(szFileName, _T("%s\\%s"), szDir, fd.cFileName);
					

                    if (DeleteFile(szFileName))
                        dwRet = IS_FILE_DELETED;
                    else
                    {
                        dwError = GetLastError();

                        if (dwError != ERROR_FILE_NOT_FOUND)
                        {
                            if ( dwError == ERROR_SHARING_VIOLATION ||
                                 dwError == ERROR_ACCESS_DENIED )
                            {
                                dwRet = IS_FILE_IN_USE;
                            }
                        }
                    }
                }
            }
            while(FindNextFile( hFind, &fd));

            FindClose( hFind );
        }
    }

    return( dwRet );
}