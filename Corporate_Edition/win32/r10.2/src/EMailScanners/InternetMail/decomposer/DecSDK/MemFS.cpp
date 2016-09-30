//------------------------------------------------------------------------
// MemFS.cpp
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

#include <errno.h>
#include <stddef.h>
#include "dec_assert.h"
#include "SymSaferStrings.h"

typedef struct tagMemFind
{
	int		index;
} MEMFIND, *PMEMFIND;
						
						//
						// Initialize any needed base class members...
						//
CMemFile1::CMemFile1() : CDecSDKFileBase(FILE_SYSTEM_MEM)
{
	iGetc = EOF;
	ulPos = 0;
	iMode = 0;
	pdiskfile = NULL;
	pdir = NULL;
}


CMemFile1::~CMemFile1()
{
	dec_assert(pdiskfile == NULL);
}


CMemDir::CMemDir()
{
	index = 0;
	uiFlags = 0;
	uiOpenCount = 0;
	uiHash = 0;
	ulSize = 0;
	pStore = NULL;
	ulStoreSize = 0;
	szName[0] = 0x00;
	szDiskName[0] = 0x00;
	szDiskNameW[0] = 0x00;
	ftCreate = 0;
	ftLastAccess = 0;
	ftLastWrite = 0;
	dwAttrs = 0x00000000;
}


CMemDir::~CMemDir()
{
	if (pStore)
		free(pStore);
}


bool CDecFiles::InitMemoryFileSystem()
{
	// Have we initialized the in-memory file-system yet?
	if (m_bInitDone)
		return true;

	// Begin critical section.
	if (m_bMultiThread)
		m_CriticalDir.In();

	if (!m_ppdir)
	{
		// Allocate array of CMemDir pointers.
		m_ppdir = (CMemDir **)malloc(DECFS_MAX_FILES * sizeof(CMemDir *));
		if (!m_ppdir)
		{
			// End critical section.
			if (m_bMultiThread)
				m_CriticalDir.Out();
			return false;
		}

		memset(m_ppdir, 0, DECFS_MAX_FILES * sizeof(CMemDir *));
	}

	m_bInitDone = true;

	// End critical section.
	if (m_bMultiThread)
		m_CriticalDir.Out();

	return true;
}


void CDecFiles::AdjustReopenMode(char *pszMode)
{
	const char	*pmode;
	char		szNewMode[12];

	if (!pszMode)
		return;

	// This function adjusts the original open mode such that it will not
	// destroy the data in an existing file.
	// First mode character must be 'r', 'w', or 'a'.
	pmode = pszMode;
	if (!strncmp(pmode, "r+", 2))
	{
		strcpy(szNewMode, "r+");
		pmode += 2;
	}
	else if (!strncmp(pmode, "w+", 2))
	{
		strcpy(szNewMode, "r+");
		pmode += 2;
	}
	else if (!strncmp(pmode, "a+", 2))
	{
		strcpy(szNewMode, "a");
		pmode += 2;
	}
	else if (*pmode == 'r')
	{
		strcpy(szNewMode, "r");
		pmode++;
	}
	else if (*pmode == 'w')
	{
		strcpy(szNewMode, "r+");
		pmode++;
	}
	else if (*pmode == 'a')
	{
		strcpy(szNewMode, "a");
		pmode++;
	}
	else
		return;	// The mode is bad, but this should never happen.

	// Just append the rest of the input mode to the new mode string.
	strcat(szNewMode, pmode);

	// Return the adjusted mode string.
	strcpy(pszMode, szNewMode);
}


X_FILE *CDecFiles::NewFile(void)
{
	X_FILE		*pxfile = NULL;
	CMemFile1	*pfile;

	pfile = new CMemFile1;
	if (pfile)
		pxfile = (X_FILE *)pfile;

	return(pxfile);
}


X_FILE *CDecFiles::AddDirEntry(const char *pszName, const char *pszMode)
{
	X_FILE		*pxfile;
	CMemDir		*pdir;
	CMemFile1	*pfile;
	size_t		len;

	if (!pszName)
		return(NULL);

	// Get the length of the name.
	len = strlen(pszName);
	if (len == 0 || len >= sizeof(pdir->szName))
		return(NULL);

	// Get an empty directory slot.
	pdir = GetEmptyEntry();
	if (!pdir)
		return(NULL);

	pxfile = NewFile();
	if (!pxfile)
		return(NULL);

	pfile = GET_MEMFILE(pxfile);
	dec_assert(pfile);

	pfile->uiFlags &= ~FILE_FLAG_EOF;
	pfile->ulPos = 0;
	pfile->pdir = pdir;

	strcpy(pdir->szName, pszName);
	pdir->ulSize = 0;
	pdir->pStore = NULL;
	pdir->ulStoreSize = 0;
	pdir->szDiskName[0] = 0x00;
	pdir->szDiskNameW[0] = 0x0000;
	pdir->ftCreate = 0;
	pdir->ftLastAccess = 0;
	pdir->ftLastWrite = 0;
	pdir->dwAttrs = 0x00000000;

	// Directory entry and associated file object created OK.
	return(pxfile);
}


// Locate an unused directory entry.
// Return NULL if all slots are in use.
CMemDir *CDecFiles::GetEmptyEntry(void)
{
	CMemDir	*pdir;
	int		index = 0;
	int		iFound = 0;

	if (!m_bInitDone)
		return(NULL);

	// Begin critical section.
	if (m_bMultiThread)
		m_CriticalDir.In();

	// See if we have saved off an index that is known to be available.
	if (m_iCachedDirIndex)
	{
		// Yes, we have.  So, just use it.
		index = m_iCachedDirIndex;
		m_iCachedDirIndex = 0;
		iFound = 1;

		// Double-check that the entry really is available.
		dec_assert(m_ppdir[index] == NULL);
	}

	if (!iFound)
	{
		for (index = 0; index < DECFS_MAX_FILES; index++)
		{
			pdir = m_ppdir[index];
			if (!pdir)
			{
				iFound = 1;
				break;
			}
		}
	}

	if (iFound)
	{
		pdir = new CMemDir;
		if (pdir)
		{
			if (m_iCachedDirIndex == index)
				m_iCachedDirIndex = 0;
			pdir->index = index;
			m_ppdir[index] = pdir;
		}
	}
	else
	{
		pdir = NULL;
	}

	// End critical section.
	if (m_bMultiThread)
		m_CriticalDir.Out();
	return(pdir);
}


CMemDir *CDecFiles::FindEntry(const char *pszName)
{
	CMemDir *pdir = NULL;
	int		index;
	int		iFound = 0;

	if (!pszName)
		return(NULL);

	if (!m_bInitDone)
		return(NULL);

	// Begin critical section.
	if (m_bMultiThread)
		m_CriticalDir.In();

	for (index = 0; index < DECFS_MAX_FILES; index++)
	{
		pdir = m_ppdir[index];
		if (pdir)
		{
			// The following comparison is a performance optimization.
			// We know that all names stored in the m_ppdir array
			// start with "mem://nnnnnnnn" where nnnnnnnn is a decimal
			// number that gets continuously incremented.  Thus, we can
			// avoid making strcmp calls 9 out of every 10 by looking at
			// the least significant digit at offset 13.
			// This optimization generally shaves about 1% off total run-time.
			if (pszName[13] == pdir->szName[13])
			{
				if (!strcmp(pszName, pdir->szName))
				{
					iFound = 1;
					break;
				}
			}
		}
		else
		{
			// Performance optimization.  Save off the index for
			// this empty slot if we have not already found one.
			// This will get used on the next call to GetEmptyEntry.
			if (!m_iCachedDirIndex)
				m_iCachedDirIndex = index;
		}
	}

	// End critical section.
	if (m_bMultiThread)
		m_CriticalDir.Out();

	if (!iFound)
		pdir = NULL;

	return(pdir);
}


CMemDir *CDecFiles::FindFirstEntry(unsigned long int *pulHandle)
{
	MEMFIND *pfind = NULL;

	if (!pulHandle)
		return(NULL);

	if (!m_bInitDone)
		return(NULL);

	// Allocate FF/FN tracking object.
	pfind = (MEMFIND *)malloc(sizeof(MEMFIND));
	if (!pfind)
		return(NULL);

	// Start search at index 0.
	pfind->index = 0;
	*pulHandle = (unsigned long int)pfind;

	return (FindNextEntry(*pulHandle));
}


CMemDir *CDecFiles::FindNextEntry(unsigned long int ulHandle)
{
	CMemDir *pdir = NULL;
	MEMFIND *pfind = NULL;
	int		index;
	int		iFound = 0;

	if (!ulHandle)
		return(NULL);

	if (!m_bInitDone)
		return(NULL);

	pfind = (MEMFIND *)ulHandle;
	if (pfind->index >= DECFS_MAX_FILES)
		return(NULL);

	// Begin critical section.
	if (m_bMultiThread)
		m_CriticalDir.In();

	for (index = pfind->index; index < DECFS_MAX_FILES; index++)
	{
		pdir = m_ppdir[index];
		if (pdir)
		{
			iFound = 1;
			break;
		}
	}

	// End critical section.
	if (m_bMultiThread)
		m_CriticalDir.Out();

	if (iFound)
		pfind->index = index + 1;
	else
		pdir = NULL;

	return(pdir);
}


void CDecFiles::FindEntryClose(unsigned long int ulHandle)
{
	MEMFIND *pfind;
	
	pfind = (MEMFIND *)ulHandle;
	if (pfind)
		free(pfind);
}


bool CDecFiles::SwitchMemoryFileToDisk(X_FILE *stream, CMemDir *pdir)
{
	size_t		len;
	size_t		ulOldPos;
	FILE		*fp;
	CMemFile1	*pfile;
	char		*pszTempFile = NULL;
#if defined(_WINDOWS)
	wchar_t		*pszTempFileW = NULL;
#endif
	bool		rc;
	char		szNewMode[12];

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_INTERNAL_REQUEST(false);

	pfile = GET_MEMFILE(stream);
	dec_assert(pfile);

	// This function takes an in-memory file and dumps it into a temporary
	// on-disk file.
	// 1: Create an on-disk temporary file.
	// 2: Dump whatever data for the file we already have in memory
	//    into the temporary on-disk file.
	// 3: Flush any data currently being held in the virtual I/O buffer
	//    out to the temporary on-disk file.
	// 4: Seek to the correct position in the on-disk file to match
	//    what is the current position in the in-memomry file.
	// 5: Change the directory entry from in-memory to on-disk.
	if (m_iUseWideTempPath)
	{
#if defined(_WINDOWS)
		wchar_t szMode[12];

		pszTempFileW = (wchar_t *)malloc(2048);
		if (!pszTempFileW)
		{
			errno = ENOMEM;
			return false;
		}

		// FreeBSD does not have wcscpy or wcslen.
		wcscpy(pszTempFileW, GetTempFilePathW());
		len = wcslen(pszTempFileW);
		if (!GetTempFileW(&pszTempFileW[len], 1023 - len))
		{
			errno = ENOMEM;
			goto Switch_Error;
		}

		mbstowcs(szMode, pfile->szMode, strlen(pfile->szMode));
		fp = _wfopen(pszTempFileW, szMode);
		wcscpy(pdir->szDiskNameW, pszTempFileW);
#else
		fp = NULL;	// No support for Unicode except on Windows.
#endif
	}
	else
	{
		pszTempFile = (char *)malloc(MAX_PATH);
		if (!pszTempFile)
		{
			errno = ENOMEM;
			return false;
		}

		if (!GetTempFile(pszTempFile, MAX_PATH - 1))
		{
			errno = ENOMEM;
			goto Switch_Error;
		}

		// Open the on-disk file in create/truncate/write/binary mode.
		fp = fopen(pszTempFile, "wb");
		strcpy(pdir->szDiskName, pszTempFile);
	}

	if (!fp)
	{
		// Return errno from the above fopen call.
		goto Switch_Error;
	}

	// Save off the in-memory file's current position.
	ulOldPos = pfile->ulPos;

	// Now copy the in-memory file to the on-disk file.
	rc = CopyMemoryFileToDisk(stream, pdir, fp);
	if (!rc)
	{
		// Failed to copy the in-memory file to disk.  Return an error.
		// Don't need to check this fclose since we're already in an
		// error state.
		fclose(fp);
		errno = ENOMEM;
		goto Switch_Error;
	}

	if (EOF == fclose(fp))
		goto Switch_Error;

	// Now re-open the on-disk file with the correct mode.
	// But before we re-open the on-disk file, adjust the mode to take into
	// account the fact that we do NOT want the file to be created or
	// truncated because that would destroy the data that we just dumped
	// into the new on-disk file.
	strcpy(szNewMode, pfile->szMode);
	AdjustReopenMode(szNewMode);
	fp = fopen(pszTempFile, szNewMode);
	if (!fp)
	{
		// Failed to re-open the on-disk file for some reason.  Return an error.
		// Return errno from the above fopen call.
		goto Switch_Error;
	}

	// Update the on-disk file's current position to match the current
	// position of the in-memory file.
	if (fseek(fp, ulOldPos, SEEK_SET))
	{
		// The seek call failed for some odd reason.  Well, that
		// means we can not guarantee the in-memory file has been
		// correctly swapped to disk.  So, clean everything up and
		// return an error.
		// Don't need to check this fclose since we're already in an
		// error state.
		fclose(fp);
		errno = EINVAL;
		goto Switch_Error;
	}

	pfile->pdiskfile = fp;

	// Free any allocation associated with this file.
	if (pdir->pStore)
	{
		free(pdir->pStore);
		pdir->pStore = NULL;

		// Begin critical section.  Only one thread allowed through here
		// at a time.
		if (m_bMultiThread)
			m_CriticalFile.In();

		// Remove the amount of storage that we are letting go from our
		// running tab of the in-memory file system size.
		dec_assert(pdir->ulStoreSize <= m_ulFSSize);
		m_ulFSSize -= pdir->ulStoreSize;

		// End critical section.
		if (m_bMultiThread)
			m_CriticalFile.Out();
	}

	pdir->uiFlags = 0;
	pdir->ulSize = 0;
	pdir->ulStoreSize = 0;

	if (m_iUseWideTempPath)
		pdir->uiFlags |= MEMDIR_FLAG_SWAPPED_W;
	else
		pdir->uiFlags |= MEMDIR_FLAG_SWAPPED;

	// Free the temporary filename buffer allocated for the on-disk file.
	if (pszTempFile)
		free(pszTempFile);

#if defined(_WINDOWS)
	if (pszTempFileW)
		free(pszTempFileW);
#endif

	return true;

Switch_Error:
	if (pszTempFile)
	{
		remove(pszTempFile);
		free(pszTempFile);
		pdir->szDiskName[0] = 0x00;
	}

#if defined(_WINDOWS)
	if (pszTempFileW)
	{
		_wremove(pszTempFileW);
		free(pszTempFileW);
		pdir->szDiskNameW[0] = 0x0000;
	}
#endif

	return false;
}


#define XFER_BUF_SIZE	32 * 1024

bool CDecFiles::CopyMemoryFileToDisk(X_FILE *stream, CMemDir *pdir, FILE *fp)
{
	unsigned long int	ulOldPos;
	unsigned char		*pbuffer;
	size_t				numread;
	size_t				sizeFileLen;
	CMemFile1			*pfile;
	bool				bRet = true;

	pfile = GET_MEMFILE(stream);
	dec_assert(pfile);

	sizeFileLen = (size_t)pdir->ulSize;

	// Save current file position
	ulOldPos = pfile->ulPos;

	// Seek back to the beginning of the in-memory file.
	pfile->ulPos = 0;

	// Copy data from the in-memory file to the on-disk file.
	pbuffer = pdir->pStore;
	while (sizeFileLen > 0)
	{
		numread = XFER_BUF_SIZE;
		if (numread > sizeFileLen)
			numread = sizeFileLen;

		if (fwrite(pbuffer, 1, numread, fp) != numread)
			break;

		sizeFileLen -= numread;
		pbuffer += numread;
	}

	//
	// Check for errors...
	//
	if (ferror(fp))
		bRet = false;

	return bRet;
}


//*************************************************************************************************
// X_FILE *SetEndOfFile(X_FILE *stream,
//                      size_t size)
//
// Purpose
//  This function is used to extend or truncate an existing file.  Currently, it uses either
//  the chsize() API (on Windows and Netware) or the ftruncate() API (on AIX, Linux, Solaris,
//  S390, and AS400).  
// 
//  Since these API's are not POSIX compliant, we may find in the future that some platforms
//  do not offer these API's.  At that time, we'll need to 'roll our own' (see Support_SetEndOfFile
//  in Dec2Lib for an example), but in the meantime, we can save a lot of work and gain 
//  performance by doing it this way.
//
// Parameters:
//
//  stream      - Pointer to file to truncate
//  size        - Desired new file size
//
// Returns:
//
//  Upon success, this function returns stream.
//  Upon failure, this function returns NULL.
//
//*************************************************************************************************

X_FILE *CDecFiles::SetEndOfFile(X_FILE *stream, size_t size)
{
	CMemFile1	*pfile;

	if (!stream)
		return NULL;

	pfile = GET_MEMFILE(stream);
	dec_assert(pfile);

	if (!pfile->pdiskfile)
		return NULL;

#if defined(_WINDOWS) || defined(SYM_NLM)
	if(-1 == chsize(fileno(pfile->pdiskfile), size))
		return NULL;
#else
	if(-1 == ftruncate(fileno(pfile->pdiskfile), size))
		return NULL;
#endif

	return (stream);
}


bool CDecFiles::AttachMemoryBlock(char *pszName, size_t ulNameBufSize, void *ptr, size_t sizeOfPtr)
{
	CMemDir *pdir;
	char	*pszFileName;
	size_t	len;
	bool bRet = false;

	if (!pszName || !ptr || (ulNameBufSize == 0) || (sizeOfPtr == 0))
		return(false);

	if (!GetNextTempFileName(pszName, (int)ulNameBufSize, false))
		return(false);

	// Get the length of the name.
	len = strlen(pszName);
	if (!len || len + NAMESPACE_MAX_LEN + 1 > ulNameBufSize)
		return(false);
	
	// Get an empty directory slot.
	pdir = GetEmptyEntry();
	if (!pdir)
		return(false);

	// Allocate a buffer to store the name.
	len += NAMESPACE_MAX_LEN;  // Add some slack space for the name and its NULL terminator.
	pszFileName = (char *)malloc(len);
	if (!pszFileName)
		goto bailOut;

	sssnprintf(pszFileName, len, "%s%s", NAMESPACE_MEMORY, pszName);
	strcpy(pszName, pszFileName);

	strcpy(pdir->szName, pszFileName);
	pdir->ulSize = sizeOfPtr;
	pdir->pStore = (unsigned char *)ptr;
	pdir->ulStoreSize = sizeOfPtr;
	pdir->uiFlags |= MEMDIR_FLAG_ATTACHED;

	bRet = true;

bailOut:
	if (pszFileName)
		free(pszFileName);

	// File created OK.
	return(bRet);
}


void CDecFiles::SetSingleThreadMode(bool bMode)
{
	m_bMultiThread = !bMode;
}


unsigned long int CDecFiles::GetFileSystemSize()
{
	return m_ulMaxFSSize;
}


void CDecFiles::SetFileSystemSize(unsigned long int ulSize)
{
	m_ulMaxFSSize = ulSize;
}

unsigned long int CDecFiles::GetFileSizeThreshold()
{
	return m_ulFileSizeThreshold;
}


void CDecFiles::SetFileSizeThreshold(unsigned long int ulSize)
{
	m_ulFileSizeThreshold = ulSize;
}


const char *CDecFiles::GetTempFilePath()
{
	return m_pszTempPath;
}


void CDecFiles::SetTempPath(const char *path)
{
	size_t len;

	m_iUseWideTempPath = false;

	if (m_pszTempPath)
		delete [] m_pszTempPath;

	if (!path)
		len = 0;
	else
		len = strlen(path);

	if (len == 0)
	{
		// No path has been supplied so use some default platform-specific path.
		m_pszTempPath = new char[MAX_PATH];
		if (m_pszTempPath)
		{
			memset(m_pszTempPath, 0, MAX_PATH);

#if defined(_WINDOWS)
			GetTempPath(MAX_PATH, m_pszTempPath);
#elif defined(UNIX)
			strcpy(m_pszTempPath, "/tmp");
#elif defined(SYM_NLM)
			#pragma message("Support_GetTempPath(): Confirm this is correct for Netware!")
			strcpy(m_pszTempPath, "SYS:tmp");
#endif
		}
	}
	else
	{
		len = strlen(path) + 2;
		m_pszTempPath = new char[len];
		if (m_pszTempPath)
		{
			memset(m_pszTempPath, 0, len);
			strcpy(m_pszTempPath, path);
		}
	}
}


const wchar_t *CDecFiles::GetTempFilePathW()
{
	return m_pszTempPathW;
}


void CDecFiles::SetTempPathW(const wchar_t *path)
{
#if !defined(SYM_FREEBSD)
	// FreeBSD does not have wcscpy.

	size_t len;

	m_iUseWideTempPath = true;

	if (m_pszTempPathW)
		delete [] m_pszTempPathW;

	if (!path)
		len = 0;
	else
		len = wcslen(path);

	if (len == 0)
	{
		// No path has been supplied so use some default platform-specific path.
		m_pszTempPathW = new wchar_t[MAX_PATH];
		if (m_pszTempPathW)
		{
			memset(m_pszTempPathW, 0, MAX_PATH * sizeof(wchar_t));

#if defined(_WINDOWS)
			GetTempPathW(MAX_PATH, m_pszTempPathW);
#endif

#if defined(UNIX)
			wcscpy(m_pszTempPathW, L"/tmp");
#endif
		}
	}
	else
	{
		len = wcslen(path) + 2;
		m_pszTempPathW = new wchar_t[len];
		if (m_pszTempPathW)
		{
			memset(m_pszTempPathW, 0, len * sizeof(wchar_t));
			wcscpy(m_pszTempPathW, path);
		}
	}
#endif
}


bool CDecFiles::GetTempFile(char *pszFilename, int iSize)
{
	return (CreateTempFile(pszFilename, iSize));
}


bool CDecFiles::GetTempFileW(wchar_t *pszFilename, int iSize)
{
	char	*pszFile;

	pszFile = new char[iSize];
	if (!pszFile)
		return false;

	if (!CreateTempFile(pszFile, iSize))
	{
		delete [] pszFile;
		return false;
	}

#if defined(SYM_LINUX) || defined(SYM_AIX)
	mbstate_t state;
	memset(&state,'\0',sizeof(state));
	const char* tempvar = pszFile;
	mbsrtowcs(pszFilename, &tempvar, strlen(pszFile),&state);
#else
	mbstowcs(pszFilename, pszFile, strlen(pszFile));
#endif
	delete [] pszFile;
	return true;
}


bool CDecFiles::SwitchFileToDisk(const char *filename, char *pszondiskname, int iBufferSize)
{
	bool		rc;
	X_FILE		*stream;
	CMemFile1	*pfile;

	//
	// Handle alternate namespaces
	//
	if(IS_RAPI_NAMESPACE(filename))
	{
		// 
		// Not needed on RAPI filesystem
		//
		strcpy(pszondiskname, filename);
		return true;
	}

	stream = x_fopen(filename, "rb");
	if (!stream)
		return false;

	pfile = GET_MEMFILE(stream);
	dec_assert(pfile);
	
	if (pfile->pdiskfile)
	{
		// The file is already on-disk.  See if the file has already been
		// swapped to disk.  If so, return its on-disk filename.
		if (pfile->pdir)
		{
			if (pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
			{
				strncpy(pszondiskname, pfile->pdir->szDiskName, iBufferSize);
				
				if(EOF == x_fclose(stream))
					return false;

				return true;
			}

			if (pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W)
			{
#if defined(SYM_LINUX) || defined(SYM_AIX)
				mbstate_t state;
				memset(&state,'\0',sizeof(state));
				const wchar_t *tempvar = pfile->pdir->szDiskNameW;
				wcsrtombs(pszondiskname, &tempvar, iBufferSize, &state);
#else
				wcstombs(pszondiskname, pfile->pdir->szDiskNameW, iBufferSize);
#endif
				
				if(EOF == x_fclose(stream))
					return false;

				return true;
			}
		}

		// This case handles files that are not in any namespaces.
		// So, the on-disk name is the same as the input name.
		if(EOF == x_fclose(stream))
			return false;

		strncpy(pszondiskname, filename, iBufferSize);
		return true;
	}

	rc = false;	// Assume failure.
	if (pfile->pdir)
	{
		if (!(pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED) &&
			!(pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W))
		{
			// Re-direct this in-memory file to be on-disk instead.
			rc = SwitchMemoryFileToDisk(stream, pfile->pdir);
			if (rc)
				strncpy(pszondiskname, pfile->pdir->szDiskName, iBufferSize);
		}
	}

	if(EOF == x_fclose(stream))
		return false;

	return rc;
}


bool CDecFiles::SwitchFileToDiskW(const char *filename, wchar_t *pszondiskname, int iBufferSize)
{
#if !defined(SYM_FREEBSD)
	// FreeBSD does not have wcscpy or wcslen or wcsncpy.

	bool		rc = false;
	X_FILE		*stream;
	CMemFile1	*pfile;

	stream = x_fopen(filename, "rb");
	if (!stream)
		return true;

	pfile = GET_MEMFILE(stream);
	dec_assert(pfile);
	if (pfile->pdiskfile)
	{
		// The file is already on-disk.  See if the file has already been
		// swapped to disk.  If so, return its on-disk filename.
		if (pfile->pdir)
		{
			if (pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
			{
#if defined(SYM_LINUX) || defined(SYM_AIX)
				mbstate_t state;
				memset(&state,'\0',sizeof(state));
				const char* tempvar = pfile->pdir->szDiskName;
				mbsrtowcs(pszondiskname, &tempvar, iBufferSize, &state);
#else
				mbstowcs(pszondiskname, pfile->pdir->szDiskName, iBufferSize);
#endif
				
				if(EOF == x_fclose(stream))
					return false;

				return true;
			}

			if (pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W)
			{
				wcsncpy(pszondiskname, pfile->pdir->szDiskNameW, iBufferSize);
				
				if(EOF == x_fclose(stream))
					return false;

				return true;
			}
		}

		// This case handles files that are not in any namespaces.
		// So, the on-disk name is the same as the input name.
		if(EOF == x_fclose(stream))
			return false;

#if defined(SYM_LINUX) || defined(SYM_AIX)
		mbstate_t state;
		memset(&state,'\0',sizeof(state));
		mbsrtowcs(pszondiskname, &filename, iBufferSize, &state);
#else
		mbstowcs(pszondiskname, filename, iBufferSize);
#endif
		return true;
	}

	if (pfile->pdir)
	{
		if (!(pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED) &&
			!(pfile->pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W))
		{
			// Re-direct this in-memory file to be on-disk instead.
			rc = SwitchMemoryFileToDisk(stream, pfile->pdir);
		}
	}

	if(EOF == x_fclose(stream))
		return false;

	return rc;
#else
	return false;
#endif
}


bool CDecFiles::CreateTempFile(char *pszFile, int iSize)
{
	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_INTERNAL_REQUEST(false);

	if (pszFile == NULL)
		return false;

	// If we get a name collision, retry up to 5 times.
	for (int i = 0; i < 5; i++)
	{
		bool bFileExists;

		// Calculate the next temporary filename.
		GetNextTempFileName(pszFile, iSize);

		// Try to create the file.
		// If successful, return immediately.
		if (CreateTempFile(pszFile, &bFileExists))
		{
//            printf("Temp file: %s\n", pszFile);
			return true;
		}

		// If error other than "name already exists",
		// give up immediately.
		if (!bFileExists)
			return false;
	}

	return false;
}


bool CDecFiles::CreateTempFile(const char *szFile, bool *pbFileExists)
{
	FILE *pfile;

	if (szFile == NULL || pbFileExists == NULL)
		return false;

	// Create the specified file, but fail if the name already exists.
	if (access(szFile, 0) == 0)
	{
		*pbFileExists = true;
		return false;
	}

	pfile = fopen(szFile, "w+b");
	if (!pfile)
	{
		*pbFileExists = false;
		return false;
	}

	*pbFileExists = true;
	
	if(EOF == fclose(pfile))
		return false;

	return true;
}


bool CDecFiles::GetNextTempFileName(char *pszName, int iSize, bool bAddNamespace)
{
	const char *pszLastBackslash;
	size_t	len;
	int		rc;
	char	szBetween[2];
	char	szHex[12];

	if (pszName == NULL)
		return false;

	m_CriticalTempFile.In();

	if(bAddNamespace)
	{

		// If the directory is non-empty and doesn't end in a backslash,
		// we need to add one.
		pszLastBackslash = strrchr(m_pszTempPath, '/');
		if (!pszLastBackslash)
			pszLastBackslash = strrchr(m_pszTempPath, '\\');

		szBetween[0] = '\0';
		if (m_pszTempPath[0] &&
			(pszLastBackslash == NULL || pszLastBackslash[1]))
		{
	#if defined(UNIX)
			strcpy(szBetween, "/");
	#else
			strcpy(szBetween, "\\");
	#endif
		}

		// Build a string of the form "tempdir\TMppnnnn" or just "TMppnnnn".
		// the 'pp' is a two character code based on the process ID
		// the 'nnnn' is a four digit code for unique file names.
		rc = -1;  // Assume failure
		len = strlen(m_pszTempPath) + strlen(szBetween) + 9;
		if (len <= (size_t)iSize)
		{
			strcpy(pszName, m_pszTempPath);
			strcat(pszName, szBetween);
			sssnprintf(szHex, sizeof(szHex), "%08X", m_dwUnique);
			strcat(pszName, szHex);
			m_dwUnique++;
			if (m_dwUnique > 0x0FFFFFFF)
				m_dwUnique = 0x00000000;
			rc = 0;
		}
	}

	else
	{
		sssnprintf(szHex, sizeof(szHex), "%08X", m_dwUnique);
		strcpy(pszName, szHex);
		m_dwUnique++;
		if (m_dwUnique > 0x0FFFFFFF)
			m_dwUnique = 0x00000000;
		rc = 0;
	}

	m_CriticalTempFile.Out();

	if (rc < 0)
		return false;

	return true;
}

