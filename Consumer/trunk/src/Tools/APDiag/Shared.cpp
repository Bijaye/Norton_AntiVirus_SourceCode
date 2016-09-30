////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Shared.cpp - defines functions that are common to the diagnostic tool
//
#include "StdAfx.h"

// Signature checking functions
#include "ccTrust.h"

bool FileExistsAndSigned(LPTSTR szFileName)
{
	if( -1 == GetFileAttributes(szFileName) )
	{
		g_log.Log(_T("Error: The file: %s does not exist!"), szFileName);
		return false;
	}

	if( !cct_IsSymantecImage(szFileName) )
	{
		g_log.Log(_T("Error: The file: %s does not have a valid symantec signature!"), szFileName);
		return false;
	}

	return true;
}

bool RegisterComDll(LPTSTR lpszModulePath)
{
	HRESULT hrCoinit = CoInitialize(NULL);
	if (FAILED(hrCoinit))
	{
		g_log.Log( _T("Error: Cannot register %s because CoInitialize failed with error: 0x%x"),lpszModulePath, hrCoinit);
		return false;
	}

	TCHAR szDllRegSvr[] = _T("DllRegisterServer");

    // Make sure the module exists before installing
    //
    if ( -1 == GetFileAttributes ( lpszModulePath ))
    {
        g_log.Log ( _T("Error: Cannot register %s because it was not found"),lpszModulePath );
		CoUninitialize();
        return false;
    }
    
    HINSTANCE hInst;

    hInst = ::LoadLibraryEx(lpszModulePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if ( !hInst )
    {
        g_log.Log (_T("Error: Failed to load %s"), lpszModulePath);
		CoUninitialize();
        return false;
    }
    
    HRESULT (FAR STDAPICALLTYPE * lpDllEntryPoint)(void);
    LPTSTR pszDllEntryPoint = szDllRegSvr;  

    (FARPROC&)lpDllEntryPoint = GetProcAddress(hInst, pszDllEntryPoint);
	
	if (lpDllEntryPoint == NULL)   
	{
        // Can't find the exported function
        //
        g_log.Log (_T("Error: Can't find the exported DllRegisterServer function in %s"),lpszModulePath);
		if (hInst)
			FreeLibrary ( hInst );
		CoUninitialize();
        return false;
    }
    else
    {
        HRESULT hrExtendedError = (*lpDllEntryPoint)();
           
        if (FAILED(hrExtendedError))
        {
            // Failed during DllRegisterServer
            //
            g_log.Log (_T("Error: Failed during DllRegisterServer function in %s. Error code: 0x%x"),lpszModulePath,hrExtendedError);
			if (hInst)
				FreeLibrary ( hInst );
			CoUninitialize();
            return false;
        }
    }

    if (hInst)
        FreeLibrary ( hInst );

	g_log.Log(_T("Registration of %s succeeded."), lpszModulePath);

	CoUninitialize();
    return true;
}

// Retrieves the version number of a file
bool GetFileVersion(LPTSTR lpszFile, LPTSTR lpszBuf, DWORD dwBufSize)
{
	BOOL bOk;
	DWORD FileVerInfoSize = 0;
	DWORD DummyHandle = 0;

	// verify and initialize return string
	if( !lpszBuf || dwBufSize <= 0 )
	{
		g_log.Log(_T("Error: GetFileVersion - The buffer passed into GetFileVersion is not valid."));
		return false;
	}
    *lpszBuf = 0;

	// get the size needed for the buffer to hold the file version data
	FileVerInfoSize = GetFileVersionInfoSize(lpszFile, &DummyHandle);
	if (FileVerInfoSize == 0)
	{
		g_log.Log(_T("Error: GetFileVersion - Could not get file version info size."));
		return false;
	}

	// create the buffer to hold the file version info
	LPBYTE pFileVerData;
	pFileVerData = new BYTE[FileVerInfoSize];
	ZeroMemory(pFileVerData,FileVerInfoSize);
	if (!pFileVerData)
	{
		g_log.Log(_T("Error: GetFileVersion - Could not allocate a buffer to store the file version information."));
		return false;
	}

	// get the data
	bOk = GetFileVersionInfo(lpszFile, 0, FileVerInfoSize, pFileVerData);
	if (!bOk)
	{
		delete[] pFileVerData;
		g_log.Log(_T("Error: GetFileVersion - Call to GetFileVersionInfo failed."));
		return false;
	}

	// This will point to the ProductVersion string in the pFileVerData buffer
	LPTSTR lpszProductVersion;

    // query the data for the language-character set strings
    // (use the first one, version info should be same in all languages)
    DWORD *pdwLangCharsetCode;
	UINT uLenJunk;
	bOk = VerQueryValue(pFileVerData, "\\VarFileInfo\\Translation", (void**)&pdwLangCharsetCode, &uLenJunk);

	//query the data for ProductVersion
	TCHAR szSubBlock[255] = {0};
	UINT uProductVersionLen;
	_stprintf(
        szSubBlock, _T("\\StringFileInfo\\%04x%04x\\ProductVersion"),
        LOWORD(*pdwLangCharsetCode),
        HIWORD(*pdwLangCharsetCode)
    );
	bOk = VerQueryValue(pFileVerData, szSubBlock, (void**)&lpszProductVersion, &uProductVersionLen);
	if (!bOk)
	{
		delete[] pFileVerData;
		g_log.Log(_T("Warning: GetFileVersion - VerQueryValue() failed to get the product version for %s (This is ok for files that do not have versions like Naveng.sys and Navex15.sys."), lpszFile);
		return false;
	}

	// create the string to return to caller
	if (dwBufSize < uProductVersionLen)
	{
		g_log.Log(_T("Error: GetFileVersion - The buffer for the file version info passed in is too small."));
		delete[] pFileVerData;
        return false;       // return buffer is too small
	}
    
    _tcscpy(lpszBuf, lpszProductVersion);
	delete[] pFileVerData;
	return true;
}
