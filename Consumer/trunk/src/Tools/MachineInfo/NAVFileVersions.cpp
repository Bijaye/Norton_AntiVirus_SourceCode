////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVFileVersions.cpp: implementation of the CNAVFileVersions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "NAVFileVersions.h"
#include "FileEnum.h"
#include "NAVInfo.h"
#include "FileIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVFileVersions::CNAVFileVersions()
{
    CNAVInfo NAVInfo;
    if ( NAVInfo.GetNAVDir ()[0] != '\0' )
        m_bIsAvailable = true;
}

CNAVFileVersions::~CNAVFileVersions()
{

}

bool CNAVFileVersions::CreateReport ( LPCTSTR lpcszFolderPath )
{
    FILE* pOutputFile;
    TCHAR szOutput [MAX_PATH] = {0};

    _stprintf ( szOutput, "%s\\%s", lpcszFolderPath, _T("NAV File Info.txt"));
    pOutputFile = _tfopen ( szOutput, "w+" );

    if ( !pOutputFile )
        return false;

    CNAVInfo NAVInfo;
    CFileIO FileIO;
    CFileEnum FileEnumDLL ( NAVInfo.GetNAVDir (), _T("*.dll") );

    TCHAR szFileName [MAX_PATH] = {0};
    LPTSTR lpszFileName = szFileName;

    TCHAR szVersion [MAX_PATH] = {0};
    
    while ( FileEnumDLL.NextFile ( lpszFileName ))
    {
        if ( FileIO.GetVersionInfo ( lpszFileName, szVersion, MAX_PATH ))
        {
            _ftprintf ( pOutputFile, "%s\t%s\n", FileEnumDLL.GetCurrentFileName (), szVersion );
        }
    }

    CFileEnum FileEnumEXE ( NAVInfo.GetNAVDir (), _T("*.exe") );

    while ( FileEnumEXE.NextFile ( lpszFileName ))
    {
        if ( FileIO.GetVersionInfo ( lpszFileName, szVersion, MAX_PATH ))
        {
            _ftprintf ( pOutputFile, "%s\t%s\n", FileEnumEXE.GetCurrentFileName (), szVersion );
        }
    }

    fclose ( pOutputFile );

    return true;
}

