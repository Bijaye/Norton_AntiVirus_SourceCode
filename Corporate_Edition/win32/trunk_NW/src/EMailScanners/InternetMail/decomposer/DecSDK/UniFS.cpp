//------------------------------------------------------------------------
// UniFS.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2001, 2005. All rights reserved.
//------------------------------------------------------------------------

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

// Project headers
#define DAPI_CPP
#include "DecPlat.h"
#include "DAPI.h"
#include "SDKMacros.h"
#include "PortableTm.h"

#include <errno.h>
#include <stddef.h>
#include "dec_assert.h"

int CDecFiles::uni_access(const char *path, int mode)
{
#if !defined(_WINDOWS)
	dec_assert(0);
	return EOF;
#else
	int		rc = EOF;
	CUNIMO	*pMapObject;

	pMapObject = GetUnicodeMappingObject(path);
	if (pMapObject)
	{
		rc = _waccess(pMapObject->pszUniName, mode);
	}

	// Delete mapping object once we're done with it
	DeleteUnicodeMappingObject(pMapObject);

	return(rc);
#endif
}


int CDecFiles::uni_chmod(const char *filename, int pmode)
{
#if !defined(_WINDOWS)
	dec_assert(0);
	return EOF;
#else
	int		rc = EOF;
	CUNIMO	*pMapObject;

	pMapObject = GetUnicodeMappingObject(filename);
	if (pMapObject)
	{
		rc = _wchmod(pMapObject->pszUniName, pmode);
	}

	// Delete mapping object once we're done with it
	DeleteUnicodeMappingObject(pMapObject);

	return(rc);
#endif
}


X_FILE *CDecFiles::uni_fopen(const char *filename, const char *mode)
{
#if !defined(_WINDOWS)
	dec_assert(0);
	return NULL;
#else
	X_FILE		*pxfile = NULL;
	CUNIMO		*pMapObject;
	wchar_t		szModeW[6];
	CMemFile1	*pfile;

	// This is an on-disk file, but with a wide-character filename,
	// so use the C-runtime Posix _wfopen() call to open the on-disk file.
	// Get a new X_FILE structure from our pool.
	pxfile = NewFile();
	if (!pxfile)
	{
		errno = ENOENT;
		return(NULL);
	}

	// Yes, this is a Unicode mapped file.
	// Get the Unicode filename from the mapping object.
	pMapObject = GetUnicodeMappingObject(filename);
	if (!pMapObject)
	{
		// *** DEBUG ***
		// Destroy the object created by the NewFile call above.
		// *************
		errno = ENOENT;
		if (pxfile) delete pxfile;
		return(NULL);
	}

	pfile = GET_MEMFILE(pxfile);
	dec_assert(pfile);

	mbstowcs(szModeW, mode, strlen(mode));
	pfile->pdiskfile = _wfopen(pMapObject->pszUniName, szModeW);
	
	// Delete mapping object once we're done with it
	DeleteUnicodeMappingObject(pMapObject);

	if (!pfile->pdiskfile)
	{
		// *** DEBUG ***
		// Destroy the object created by the NewFile call above.
		// *************
		if (pxfile) delete pxfile;
		return(NULL);
	}

	// OK, opened the on-disk file.
	return(pxfile);
#endif
}


int CDecFiles::uni_remove(const char *path)
{
#if !defined(_WINDOWS)
	dec_assert(0);
	return EOF;
#else
	int		rc = EOF;
	CUNIMO	*pMapObject;

	pMapObject = GetUnicodeMappingObject(path);
	if (pMapObject)
	{
		rc = _wremove(pMapObject->pszUniName);
	}

	// Delete mapping object once we're done with it
	DeleteUnicodeMappingObject(pMapObject);

	return(rc);
#endif
}


int CDecFiles::uni_rename(const char *oldname, const char *newname)
{
#if !defined(_WINDOWS)
	dec_assert(0);
	return EOF;
#else
	int		rc = EOF;
	CUNIMO	*pMapObject1;
	CUNIMO	*pMapObject2;

	pMapObject1 = GetUnicodeMappingObject(oldname);
	pMapObject2 = GetUnicodeMappingObject(newname);
	if (pMapObject1 && pMapObject2)
		rc = _wrename(pMapObject1->pszUniName, pMapObject2->pszUniName);

	// Delete mapping objects once we're done with it
	DeleteUnicodeMappingObject(pMapObject1);
	DeleteUnicodeMappingObject(pMapObject2);

	return(rc);
#endif
}


#if defined(_WINDOWS)
//
// Hack:  MSDN says that GetFileAttributes will return 
// INVALID_FILE_ATTRIBUTES if the call fails.  However,
// you have to have the Platform SDK installed to get
// the new winbase.h that includes this definition. Since
// we only need this one definition, I'm just defining it
// myself rather that requiring the Platform SDK for the 
// Decomposer.
//
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)

//
// From MSDOS.H (which can't be directly included since
// it's part of the CRT...ugh)
//
#define A_RO			0x1		/* read only */
#define A_H				0x2		/* hidden */
#define A_S				0x4		/* system */
#define A_V				0x8		/* volume id */
#define A_D				0x10	/* directory */
#define A_A				0x20	/* archive */

//
// _wstat doesn't accept names like \\?\<somepath>
// so we have to implement our own '_wstat' function
// using the Win32 Unicode API's.
//
int CDecFiles::uni_stat(const char *path, struct _stat *buffer)
{
	CUNIMO	*pMapObject;
	int		rc = EOF;

	pMapObject = GetUnicodeMappingObject(path);
	if (pMapObject)
	{
		WIN32_FIND_DATAW findData;
		HANDLE hFile = NULL;

		//
		// Initialize buffer and set any non-zero defaults. These values 
		// never change on Windows systems and come from the documentation
		// in MSDN.
		//
		memset(buffer, 0, sizeof(struct _stat));
		buffer->st_nlink = 1;

		hFile = FindFirstFileW(pMapObject->pszUniName, &findData);
		if(INVALID_HANDLE_VALUE != hFile)
		{
			//
			// Convert FILETIME to time_t and save in stat buffer
			//
			MyFileTimeToUnixTime(&findData.ftLastAccessTime, &buffer->st_atime);
			MyFileTimeToUnixTime(&findData.ftLastWriteTime, &buffer->st_mtime); 
			MyFileTimeToUnixTime(&findData.ftCreationTime, &buffer->st_ctime); 

			//
			// Save file size - we can only save the low DWORD since the stat
			// buffer's size field is a 'long'.
			//
			buffer->st_size = (int)findData.nFileSizeLow;

			//
			// Map Win32 attribs to UNIX style attribs
			//
			const wchar_t *p = NULL;
			unsigned dosmode = findData.dwFileAttributes & 0xFF;

			// Directory?
			buffer->st_mode = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? _S_IFDIR : _S_IFREG;

			// Read only?
			buffer->st_mode |= (dosmode & A_RO) ? _S_IREAD : (_S_IREAD | _S_IWRITE);

			// Propagate user read/write/execute bits to group/other fields
			buffer->st_mode |= (findData.dwFileAttributes & 0700) >> 3;
			buffer->st_mode |= (findData.dwFileAttributes & 0700) >> 6;
								  
			if (p = wcsrchr(pMapObject->pszUniName, L'.')) 
			{
				if (!wcsicmp(p, L".exe") ||
					!wcsicmp(p, L".cmd") ||
					!wcsicmp(p, L".bat") ||
					!wcsicmp(p, L".com"))
				{
					buffer->st_mode |= _S_IEXEC;
				}
			}
			
			// Success
			rc = 0;

			// Cleanup
			FindClose(hFile);
		}
					
		// Delete mapping object once we're done with it
		DeleteUnicodeMappingObject(pMapObject);
	}

	return(rc);
}
#else
int CDecFiles::uni_stat(const char *path, struct stat *buffer)
{
	dec_assert(0);
	return EOF;
}
#endif


#if defined(_WINDOWS)
int CDecFiles::uni_utime(const char *filename, struct _utimbuf *times)
{
	CUNIMO	*pMapObject;
	int		rc = EOF;

	pMapObject = GetUnicodeMappingObject(filename);
	if (pMapObject)
	{
		rc = _wutime(pMapObject->pszUniName, times);
	}

	// Delete mapping object once we're done with it
	DeleteUnicodeMappingObject(pMapObject);

	return(rc);
}
#else
int CDecFiles::uni_utime(const char *filename, struct utimbuf *times)
{
	dec_assert(0);
	return EOF;
}
#endif
