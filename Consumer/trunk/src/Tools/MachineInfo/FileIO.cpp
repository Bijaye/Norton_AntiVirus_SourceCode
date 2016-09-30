////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// FileIO.cpp: implementation of the CFileIO class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "FileIO.h"
#include "FileEnum.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileIO::CFileIO()
{

}

CFileIO::~CFileIO()
{

}


bool CFileIO::CopyFiles ( LPCTSTR lpcszSourcePath,
                          LPCTSTR lpcszFileMask,
                          LPCTSTR lpcszTargetPath,
                          LPCTSTR lpcszException /*NULL*/)
{
    CFileEnum FileEnum ( lpcszSourcePath, lpcszFileMask );
    TCHAR szNewFileName [MAX_PATH] = {0};
    TCHAR szOriginalFileName [MAX_PATH] = {0};
    LPTSTR lpszOriginalFileName = szOriginalFileName;

    while ( FileEnum.NextFile ( szOriginalFileName ))
    {
        // Don't copy the Activity log
        //
        if ( lpcszException == NULL || 0 != _tcsicmp ( FileEnum.GetCurrentFileName (), lpcszException ))
        {
            _stprintf ( szNewFileName, "%s\\%s", lpcszTargetPath, FileEnum.GetCurrentFileName ());
    
            CopyFile ( szOriginalFileName, szNewFileName, FALSE );
        }
    }

    return true;
}


DWORD CFileIO::GetVersionInfo(LPCTSTR lpszFileName, LPTSTR lpszVersion, UINT uBufferSize)
{
    if ( lpszFileName[0] == '\0' ||
         uBufferSize <1 )
         return FALSE;
    
    DWORD dwVersionInfoHandle = 0;
    DWORD dwResourceSize = 0;
    
    dwResourceSize = GetFileVersionInfoSize ( (LPTSTR) lpszFileName, &dwVersionInfoHandle );

    if ( dwResourceSize > 0 )
    {
        LPVOID pResourceData = new BYTE [dwResourceSize];
        ZeroMemory ( pResourceData, dwResourceSize );

        if ( pResourceData )
        {
            if ( GetFileVersionInfo ( (LPTSTR) lpszFileName,
                                      dwVersionInfoHandle,
                                      dwResourceSize,
                                      pResourceData))
            {
                DWORD dwBufferSize = sizeof(MAX_PATH) * sizeof (TCHAR);
                UINT uiLen = 0;
                VS_FIXEDFILEINFO* lpVersion;
            
                // Get version string.
                //
                VerQueryValue( pResourceData,
                               _T("\\"), //StringFileInfo\\040904B0\\FileVersion
                               (LPVOID *)(&lpVersion),
                               &uiLen);

                if ( uiLen > uBufferSize )
                    return ERROR_BUFFER_OVERFLOW; 

                if ( uiLen == 0 )
                    _tcscpy ( lpszVersion, _T(""));
                else
                    _stprintf ( lpszVersion, "%d.%d.%d.%d", HIWORD(lpVersion->dwFileVersionMS),
                                                            LOWORD(lpVersion->dwFileVersionMS),
                                                            HIWORD(lpVersion->dwFileVersionLS),
                                                            LOWORD(lpVersion->dwFileVersionLS));
             
                delete [] pResourceData;
                pResourceData = NULL;

                return TRUE;
            }
        }

    }
    return FALSE;

}
