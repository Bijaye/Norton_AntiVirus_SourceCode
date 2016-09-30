////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "cconfigmanifests.h"

#include <shlobj.h>
#include ".\cconfigmanifests.h"

CConfigManifests::CConfigManifests()
{
}

CConfigManifests::~CConfigManifests()
{
}

bool CConfigManifests::Initialize()
{
    return true;
}

HRESULT CConfigManifests::CopyManifest(const wchar_t* szSource)
{
    // Make sure the source file ends in ".spm"
    CString strSource(szSource);
    if( strSource.Right(4).MakeLower() != ".spm" )
        return E_INVALIDARG;

    // Look up the "Common Files" folder
    wchar_t szDestination[MAX_PATH];
    HRESULT hr = SHGetFolderPath(NULL, 
                    CSIDL_PROGRAM_FILES_COMMON,
                    NULL,
                    SHGFP_TYPE_DEFAULT,
                    szDestination);
    if( FAILED(hr) )
        return hr;

    // Add the relative path of the manifests folder
    PathAppend(szDestination, L"Symantec Shared\\SPManifests\\");

    int iFileNameLocation = strSource.ReverseFind(L'\\');
    CStringW strSourceDir = strSource.Left(iFileNameLocation);
    CStringW strBaseFileName(strSource.Mid(iFileNameLocation + 1).SpanExcluding(L"."));

    // Copy the .spm, .grd, .sig files
    wchar_t* szExtensions[] = {L"spm", L"grd", L"sig"};
    for( int i = 0; i < CCDIMOF(szExtensions); ++i )
    {
        hr = CopyManifestFile(strSourceDir, szDestination, 
                              strBaseFileName, szExtensions[i]);
        if( FAILED(hr) )
            return hr;
    }

    return S_OK;
}

HRESULT CConfigManifests::CopyManifestFile(const wchar_t* szSourceDir, 
                                           const wchar_t* szDestDir, 
                                           const wchar_t* szFileBaseName, 
                                           const wchar_t* szExtension)
{
    CStringW strSource(szSourceDir);
    CStringW strDest(szDestDir);

    // Build the file name
    CStringW strFile(szFileBaseName);
    if( szExtension[0] == L'.' )
        ++szExtension;
    strFile.AppendFormat(L".%s", szExtension);

    // Append trailing slashes to the directories if necessary
    if( strSource.Right(1) != L"\\" )
        strSource.AppendChar(L'\\');
    if( strDest.Right(1) != L"\\" )
        strDest.AppendChar(L'\\');

    strSource += strFile;
    strDest += strFile;

    if( !CopyFile(strSource, strDest, FALSE) )
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}
