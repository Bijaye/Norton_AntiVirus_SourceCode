// FileNameParse.cpp: implementation of the CFileNameParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileNameParse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileNameParse::CFileNameParse()
{

}

CFileNameParse::~CFileNameParse()
{

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFileNameParse::GetFileName
//
// Description   : This routine will return the file name for a fully qualified path
//
// Return type   : UINT 
//
// Argument      : LPCTSTR lpszPathName - full path to parse
// Argument      : LPTSTR lpszTitle     - destination buffer
// Argument      : UINT nMax            - destination buffer size.
//
///////////////////////////////////////////////////////////////////////////////
// 2/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
UINT CFileNameParse::GetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax)
{
	ASSERT(lpszTitle == NULL ||
		AfxIsValidAddress(lpszTitle, _MAX_FNAME));
	ASSERT(AfxIsValidString(lpszPathName));

	// always capture the complete file name including extension (if present)
	LPTSTR lpszTemp = (LPTSTR)lpszPathName;
	for (LPCTSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// remember last directory/drive separator
		if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
			lpszTemp = (LPTSTR)_tcsinc(lpsz);
	}

	// lpszTitle can be NULL which just returns the number of bytes
	if (lpszTitle == NULL)
		return lstrlen(lpszTemp)+1;

	// otherwise copy it into the buffer provided
	lstrcpyn(lpszTitle, lpszTemp, nMax);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CItemData::GetFilePath
//
// Description   : This routine will return the path component of a given file name
//
// Return type   : UINT 
//
// Argument      : LPCTSTR lpszPathName - full path to parse
// Argument      : LPTSTR lpszFilePath  - destination buffer
// Argument      : UINT nMax            - destination buffer size.
//
///////////////////////////////////////////////////////////////////////////////
// 2/1/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
UINT CFileNameParse::GetFilePath(LPCTSTR lpszPathName, LPTSTR lpszFilePath, UINT nMax)
{
	ASSERT(lpszFilePath == NULL ||
		AfxIsValidAddress(lpszFilePath, _MAX_FNAME));
	ASSERT(AfxIsValidString(lpszPathName));

    _tcsncpy( lpszFilePath, lpszPathName, nMax );

	// always capture the complete file name including extension (if present)
	LPTSTR lpszTemp = NULL;
	for (LPTSTR lpsz = lpszFilePath; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// remember last directory/drive separator
		if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
			lpszTemp = lpsz;//(LPTSTR)_tcsinc(lpsz);
	}
    
    if( lpszTemp )
        {
        // 
        // Special case for items in root folder
        // 
        if( lpszTemp == &lpszFilePath[2] )
            {
            lpszTemp = _tcsinc( lpszTemp );
            }

        *lpszTemp = _T('\0');
        }

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CFileNameParse::GetFileExtension
//
// Description   : 
//
// Return type   : UINT 
//
// Argument      : LPCTSTR lpszPathName
// Argument      : LPTSTR lpszExt
// Argument      : UINT nMax
//
///////////////////////////////////////////////////////////////////////////////
// 2/10/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
UINT CFileNameParse::GetFileExtension( LPCTSTR lpszPathName, LPTSTR lpszExt, UINT nMax)
{
	ASSERT(AfxIsValidString(lpszPathName));

	// always capture the complete file name including extension (if present)
	LPTSTR lpszTemp = NULL;
	for (LPCTSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// remember last period
		if (*lpsz == '.' )
			lpszTemp = (LPTSTR)_tcsinc(lpsz);
	}

    if( lpszTemp )
        {
        if( lpszExt == NULL )
            return _tcslen( lpszTemp ) + 1;

        _tcsncpy( lpszExt, lpszTemp, nMax );
        }

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: CFileNameParse::AppendFile
//
// Description  : 
//
// Return type  : CString 
//
// Argument     :  LPCTSTR szFolder
// Argument     : LPCTSTR szFile
//
///////////////////////////////////////////////////////////////////////////////
// 5/3/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CString CFileNameParse::AppendFile( LPCTSTR szFolder, LPCTSTR szFile )
{
    CString sRet;

    // 
    // Check to see if the last character is a \
    // 
    sRet = szFolder;
    if( sRet.Right(1) != _T('\\') )
        {
        // 
        // Add backslash.
        // 
        sRet += _T("\\");
        }

    // 
    // Add file name.
    // 
    sRet += szFile;

    return sRet;
}
