/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// FileNameParse.h: interface for the CFileNameParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILENAMEPARSE_H__F2081991_D73C_11D2_8F4C_3078302C2030__INCLUDED_)
#define AFX_FILENAMEPARSE_H__F2081991_D73C_11D2_8F4C_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileNameParse  
{
public:
	CFileNameParse();
	virtual ~CFileNameParse();

    static UINT GetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
    static UINT GetFilePath(LPCTSTR lpszPathName, LPTSTR lpszFilePath, UINT nMax);
    static UINT GetFileExtension( LPCTSTR lpszPathName, LPTSTR lpszExt, UINT nMax);
    static CString AppendFile( LPCTSTR szFolder, LPCTSTR szFile );

};

#endif // !defined(AFX_FILENAMEPARSE_H__F2081991_D73C_11D2_8F4C_3078302C2030__INCLUDED_)
