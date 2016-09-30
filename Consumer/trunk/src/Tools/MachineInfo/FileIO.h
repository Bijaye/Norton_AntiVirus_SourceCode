////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// FileIO.h: interface for the CFileIO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEIO_H__2284AA70_05FA_4295_8BC7_D51BC2EF74BC__INCLUDED_)
#define AFX_FILEIO_H__2284AA70_05FA_4295_8BC7_D51BC2EF74BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileIO  
{
public:
	DWORD GetVersionInfo ( LPCTSTR lpszFileName, LPTSTR lpszVersion, UINT uBufferSize );
	CFileIO();
	virtual ~CFileIO();
    bool CopyFiles ( LPCTSTR lpcszSourcePath,
                     LPCTSTR lpcszFileMask,
                     LPCTSTR lpcszTargetPath,
                     LPCTSTR lpcszException = NULL );
};

#endif // !defined(AFX_FILEIO_H__2284AA70_05FA_4295_8BC7_D51BC2EF74BC__INCLUDED_)
