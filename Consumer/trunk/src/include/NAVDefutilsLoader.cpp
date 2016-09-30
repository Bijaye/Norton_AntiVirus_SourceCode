////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVDefutilsLoader.cpp

#include "NAVDefUtilsLoader.h"

const LPCTSTR sz_DefUtils_dll = _T("DefUtDCD.dll");

// for use by any product components: looks for defutils dll in the cc directory
bool CccPathProvider::GetPath(LPTSTR szPath, size_t& nSize)
{
    ccLib::CString sPath;
    if(ccSym::CCommonClientInfo::GetCCDirectory(sPath))
    {
        if(size_t(sPath.GetLength() + 1) <= nSize)
            _tcsncpy(szPath, sPath, sPath.GetLength() + 1);

        return true;
    }
	else
    {
        return false;
    }
}

// for use by symsetup: looks for defutils relative to symsetup on the cd
bool CSymSetupPathProvider::GetPath(LPTSTR szPath, size_t& dwSize)
{
	GetModuleFileName(NULL, szPath, static_cast<DWORD>(dwSize));
	TCHAR *pszBackSlash = _tcsrchr(szPath, _T('\\'));
	if(pszBackSlash)
		*pszBackSlash = _T('\0');
	
	return true;
}


#ifdef MSI_DEFUTILS

MSIHANDLE g_hDefsInstall = NULL;

// for use by msi: looks for defutils relative to nav.msi on the cd
bool CMSIPathProvider::GetPath(LPTSTR szPath, size_t& nSize)
{
	// cc might not be installed yet, so we need to look for the file in the CD layout
    DWORD dwSize = nSize;
	if(ERROR_SUCCESS == MsiGetProperty(g_hDefsInstall, _T("SOURCEDIR"), szPath, &dwSize))
	{	
		TCHAR szTestPath[MAX_PATH] = {0};
		PathAppend(szPath, _T("..\\"));
			return true;
	}

	// we couldn't find it
	return false;
}

#endif