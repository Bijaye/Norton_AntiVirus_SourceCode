//------------------------------------------------------------------------
// DecFS.cpp
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
#include "RapiFs.h"
#include "time.h"

#if defined(UNIX)
	#include <pthread.h>
	#include <stdarg.h>
#endif

#if defined(SYM_AIX)
	// Need S_IREAD and S_IWRITE definitions
	#undef _H_MODE
	#include <sys/mode.h>
#endif

#include <errno.h>
#include <stddef.h>
#include <limits.h>

#if !defined(_WINDOWS)
	#include <unistd.h>		// For ftruncate on UNIX and chsize on Netware
#endif

#if !defined(SYM_NLM) && !defined(SYM_FREEBSD)
	#include <wchar.h>
#endif

#include "SymSaferStrings.h"

#if defined(SYM_FAIL_FILESYSTEM_CALLS)
	int g_nFailWhichFSCalls = FAIL_FILESYSTEM_NONE;
#endif
	
//
// If we're statically linking, then this file has already been included in 
// Dec2.cpp.  If we're building DLL's or .SO's then we need to include it
// here.
//
#if defined(SYM_PLATFORM_HAS_DLLS) || defined(SYM_PLATFORM_HAS_SHAREDOBJS)
	#include "../CharUtil/bstricmp.c"
#endif

CDecFiles::CDecFiles()
{
	m_bInitDone = false;
	m_bMultiThread = true;	// Default to multi-threaded operation.

	m_iCachedDirIndex = 0;

	// Start swapped-to-disk temporary filenames at "00000000".
	m_dwUnique = 0x00000000;

	// Set the temporary file path to its default setting.
	m_pszTempPath = NULL;
	m_pszTempPathW = NULL;
	SetTempPath(NULL);

	m_pnet = NULL;
	m_ppdir = NULL;

	// Set default maximum in-memory file system size.
	m_ulMaxFSSize = (unsigned long int)DECFS_DEFAULT_FS_SIZE;

	// Set amount of data currently held in the file system.
	m_ulFSSize = 0;

	// Set in-memory file size threshold.
	m_ulFileSizeThreshold = (unsigned long int)DECFS_DEFAULT_FILE_SIZE_THRESHOLD;

	// 
	// Initialize RAPI file system pointer.  The call to
	// m_pRapiFS->Init() will try to load RAPI.DLL and get
	// some function pointers.  This may fail, but we don't
	// care at this point.  If this fails, then any use of
	// the RapiFS file system calls will also fail.
	//
	m_pRapiFS = new CRapiFS();
	if(!m_pRapiFS)
	{
		dec_assert(0);
	}

	//
	// Disable until we really want to use it...
	//
	// m_pRapiFS->Init();
}


CDecFiles::~CDecFiles()
{
	if (m_pszTempPath)
	{
		delete [] m_pszTempPath;
		m_pszTempPath = NULL;
	}

	if (m_ppdir)
	{
		int		index;
		CMemDir *pdir;

		for (index = 0; index < DECFS_MAX_FILES; index++)
		{
			pdir = m_ppdir[index];
			if (pdir)
			{
				delete pdir;
				pdir = NULL;
			}
		}

		free (m_ppdir);
		m_ppdir = NULL;
	}

	if (m_pnet)
		m_pnet->Close();

	if (m_pRapiFS)
	{
		delete m_pRapiFS;
		m_pRapiFS = NULL;
	}
}


int CDecFiles::x_access(const char *path, int mode)
{
	CMemDir		*pdir;
	int			rc = EOF;  // Assume failure (i.e. file does not exist)

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	// mode values:
	// 00 - Existence only
	// 02 - Write permission
	// 04 - Read permission
	// 06 - Read and write permission

	if (!path)
		return(EOF);

	if (!(mode == 0 || mode == 2 || mode == 4 || mode == 6))
		return(EOF);

	// Determine which namespace this file is in and delegate if we have to...
	if (!IS_MEM_NAMESPACE(path))
	{
		if (IS_UNI_NAMESPACE(path))
			return (uni_access(path, mode));
		else if (IS_RAPI_NAMESPACE(path))
			return (m_pRapiFS->x_access(path, mode));

		// Default to an on-disk file.
		return(access(path, mode));
	}

	// Locate the named stream in our directory.
	pdir = FindEntry(path);
	if (pdir)
	{
		// First, see if this file has been swapped to disk.
		if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
			return (access(pdir->szDiskName, mode));

		if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W)
		{
#if defined(_WINDOWS)
			return (_waccess(pdir->szDiskNameW, mode));
#else
			return (EOF);
#endif
		}

		// Yes, the file exists.
		// Assume success at this point.
		rc = 0;

		if (mode == 2 || mode == 6)
		{
			if (pdir->dwAttrs & FILE_ATTRIBUTE_READONLY)
				rc = EOF;
		}
	}

	return (rc);
}


int CDecFiles::x_chmod(const char *filename, int pmode)
{
	CMemDir	  *pdir;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!filename)
	{
		errno = ENOENT;
		return(EOF);
	}

	// Determine which namespace this file is in and delegate if we have to...
	if (!IS_MEM_NAMESPACE(filename))
	{
		if (IS_UNI_NAMESPACE(filename))
			return (uni_chmod(filename, pmode));
		else if (IS_RAPI_NAMESPACE(filename))
			return (m_pRapiFS->x_chmod(filename, pmode));

		// Default to an on-disk file.
		return(chmod(filename, pmode));
	}

	// Locate the named stream in our directory.
	// Note that if this returns a non-NULL pointer that the
	// directory system is still locked.
	pdir = FindEntry(filename);
	if (!pdir)
	{
		errno = ENOENT;
		return(-1);
	}

	// First, see if this file has been swapped out to disk.
	if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
		return (chmod(pdir->szDiskName, pmode));

	if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W)
	{
#if defined(_WINDOWS)
		return (_wchmod(pdir->szDiskNameW, pmode));
#else
		return(EOF);
#endif
	}

	if (pmode & _S_IWRITE)
		pdir->dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
	else if (pmode & _S_IREAD)
		pdir->dwAttrs |= FILE_ATTRIBUTE_READONLY;

	return(0);
}


void CDecFiles::x_clearerr(X_FILE *stream)
{
	CMemFile1	*pfile;

	if (!stream)
		return;

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	pfile = GET_MEMFILE(stream);
	dec_assert(pfile);
	if (!pfile)
		return;

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
	{
		m_pRapiFS->x_clearerr(stream);
		return;
	}
	
	if (pfile->pdiskfile)
		clearerr(pfile->pdiskfile);

	pfile->uiFlags &= (~FILE_FLAG_EOF & ~FILE_FLAG_ERROR);
}


int CDecFiles::x_fclose(X_FILE *stream)
{
	CMemDir		*pdir;
	CMemFile1	*pfile;
	int			rc = 0;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
		return(EOF);

	pfile = GET_MEMFILE(stream);
	if (!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	// Delegate to any other filesystems if needed...
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return m_pRapiFS->x_fclose(stream);

	// Get the stream's directory object pointer (if any).
	pdir = pfile->pdir;

	// If this object has been swapped to disk, close it there.
	if (pfile->pdiskfile)
	{
		rc = fclose(pfile->pdiskfile);		 
		pfile->pdiskfile = NULL;
	}

	// Since the stream is being closed, destroy the CMemFile1 object.
	delete pfile;

	if (pdir)
	{
		// If the file is already close, return EOF (mimic CRT)
		if(0 == pdir->uiOpenCount)
		{
			return EOF;
		}

		pdir->uiOpenCount--;
	}

	return(rc);
}


int CDecFiles::x_fcloseall(void)
{
	dec_assert(0);
	return (EOF);
}


int CDecFiles::x_feof(X_FILE *stream)
{
	CMemFile1	*pfile;

	if (!stream)
		return(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	pfile = GET_MEMFILE(stream);
	if (!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return m_pRapiFS->x_feof(stream);

	if (pfile->pdiskfile)
	{
		if (feof(pfile->pdiskfile))
			pfile->uiFlags |= FILE_FLAG_EOF;
		else
			pfile->uiFlags &= ~FILE_FLAG_EOF;
	}

	return(pfile->uiFlags & FILE_FLAG_EOF);
}


int CDecFiles::x_ferror(X_FILE *stream)
{
	CMemFile1	*pfile;

	if (!stream)
		return(0);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	pfile = GET_MEMFILE(stream);
	if (!pfile)
	{
		dec_assert(0);
		return(0);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return m_pRapiFS->x_ferror(stream);
	
	if (pfile->pdiskfile)
	{
		if (ferror(pfile->pdiskfile))
			pfile->uiFlags |= FILE_FLAG_ERROR;
		else
			pfile->uiFlags &= ~FILE_FLAG_ERROR;
	}

	return(pfile->uiFlags & FILE_FLAG_ERROR);
}


int CDecFiles::x_fflush(X_FILE *stream)
{
	CMemFile1	*pfile;
	int			rc = 0;	// Default to successful flush.

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	// fflush(NULL) is supposed to flush all streams opened for output.
	// No Decomposer engine should ever use this type of call, so
	// we are going to treat this as an error for now.
	if (!stream)
		return(EOF);

	pfile = GET_MEMFILE(stream);
	if (!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
	{
		return m_pRapiFS->x_fflush(stream);
	}

	if (pfile->pdiskfile)
		rc = fflush(pfile->pdiskfile);

	return(rc);
}


int CDecFiles::x_fgetc(X_FILE *stream)
{
	CMemDir		*pdir;
	CMemFile1	*pfile;
	int			ch;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
		return(EOF);

	pfile = GET_MEMFILE(stream);
	if (!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
	{
		return m_pRapiFS->x_fgetc(stream);
	}

	if (pfile->pdiskfile)
		return(fgetc(pfile->pdiskfile));

	if (pfile->iGetc != EOF)
	{
		ch = pfile->iGetc;
		pfile->iGetc = EOF;
		pfile->ulPos++;
		return(ch);
	}

	pdir = pfile->pdir;
	if (!pdir)
		return(EOF);

	if (pfile->ulPos >= pdir->ulSize)
	{
		pfile->uiFlags |= FILE_FLAG_EOF;
		return(EOF);
	}

	// Read the byte directly from storage.
	ch = pdir->pStore[pfile->ulPos++] & 0x000000ff;
	return(ch);
}


char *CDecFiles::x_fgets(char *string, int n, X_FILE *stream)
{
	CMemDir		*pdir;
	CMemFile1	*pfile;
	int			iCount = 0;
	int			ch = 0;
	unsigned char *ptr;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(NULL);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream || !string || n <= 0)
		return NULL;

	pfile = GET_MEMFILE(stream);
	if (!pfile)
	{
		dec_assert(0);
		return NULL;
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
	{
		return m_pRapiFS->x_fgets(string, n, stream);
	}

	if (pfile->pdiskfile)
		return(fgets(string, n, pfile->pdiskfile));

	pdir = pfile->pdir;
	if (!pdir)
		return NULL;

	ptr = (unsigned char *)string;
	while (n - 1)
	{
		// Read the byte directly from storage.
		if (pfile->ulPos >= pdir->ulSize)
		{
			ch = EOF;
			break;
		}

		ch = pdir->pStore[pfile->ulPos++];
		*ptr++ = (unsigned char)ch;
		iCount++;

// *** DEBUG *** How do we handle this on EBCDIC platforms?
		// Is it a newline character (ASCII 0x0A)?
		if (ch == 0x0A)
			break;
// *************

		n--;
	}

	if (ch == EOF && iCount == 0)
	{
		pfile->uiFlags |= FILE_FLAG_EOF;
		string = NULL;	// Error or end-of-file reached.
	}

	//
	// NULL terminate the string
	//
	*ptr = '\0';
	return string;
}


X_FILE *CDecFiles::x_fopen(const char *filename, const char *mode)
{
	const char	*pmode;
	X_FILE		*pxfile;
	CMemDir		*pdir;
	CMemFile1	*pfile;
	int			modeflag;
	int			whileflag;
	time_t		current_time;
	bool		bIsMem = false;
	bool		bIsUni = false;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(NULL);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!filename || !mode)
	{
		errno = ENOENT;
		return(NULL);
	}

	// Initialize the in-memory file-system if it hasn't been already.
	if (!InitMemoryFileSystem())
	{
		errno = ENOMEM;
		return 0;
	}

	// Determine which namespace this file is in and delegate if we have to...
	if(!(bIsMem = IS_MEM_NAMESPACE(filename)))
	{
		if(!(bIsUni = IS_UNI_NAMESPACE(filename)))
		{
			if(IS_RAPI_NAMESPACE(filename))
			{
				return m_pRapiFS->x_fopen(filename, mode);
			}
		}
	}
	
	if (!bIsMem && !bIsUni)
	{
		// This is an on-disk file so use the C-runtime Posix fopen()
		// call to open the on-disk file.
		// Get a new X_FILE structure from our pool.
		pxfile = NewFile();
		if (!pxfile)
		{
			errno = ENOENT;
			return(NULL);
		}

		pfile = GET_MEMFILE(pxfile);
		dec_assert (pfile);

		pfile->pdiskfile = fopen(filename, mode);
		if (!pfile->pdiskfile)
		{
			delete pfile;
			return(NULL);
		}

		// OK, opened the on-disk file.
		return(pxfile);
	}

	// Here we know that the filename starts with either
	// mem:// or uni://.

	// First mode character must be 'r', 'w', or 'a'.
	pmode = mode;
	switch (*pmode)
	{
		case 'r':
			modeflag = _O_RDONLY;
			break;
		case 'w':
			modeflag = _O_WRONLY | _O_CREAT | _O_TRUNC;
			break;
		case 'a':
			modeflag = _O_WRONLY | _O_CREAT | _O_APPEND;
			break;
		default:
			errno = EINVAL;
			return(NULL);
			break;
	}

	// There can be up to three more optional mode characters:
	// (1) A single '+' character,
	// (2) One of 't' and 'b' and
	// (3) One of 'c' and 'n'.
	whileflag = 1;
	while (*++pmode && whileflag)
	{
		switch(*pmode)
		{
			case '+':
				if (modeflag & _O_RDWR)
					whileflag = 0;
				else
				{
					modeflag |= _O_RDWR;
					modeflag &= ~(_O_RDONLY | _O_WRONLY);
				}
				break;

#if defined(_WINDOWS)
			case 'b':
				if (modeflag & (_O_TEXT | _O_BINARY))
					whileflag = 0;
				else
					modeflag |= _O_BINARY;
				break;

			case 't':
				if (modeflag & (_O_TEXT | _O_BINARY))
					whileflag = 0;
				else
					modeflag |= _O_TEXT;
				break;
#endif

			default:
				whileflag = 0;
				break;
		}
	}

	current_time = time(NULL);	// Get current system time

	// Locate the named stream in our directory.
	pdir = FindEntry(filename);
	if (pdir)
	{
		// If the file is actually just an attached memory block,
		// do not allow opens in write mode.
		if (pdir->uiFlags & MEMDIR_FLAG_ATTACHED)
		{
			if ((modeflag & _O_TRUNC) || (modeflag & _O_APPEND))
			{
				return(NULL);
			}
		}

		// Get a new X_FILE structure from our pool.
		pxfile = NewFile();
		if (!pxfile)
		{
			errno = ENOENT;
			return(NULL);
		}

		// Save the fopen mode string for possible use later.
		pfile = GET_MEMFILE(pxfile);
		memset(pfile->szMode, 0, sizeof(pfile->szMode));
		strncpy(pfile->szMode, mode, sizeof(pfile->szMode) - 1);

		if (bIsMem)
		{
			if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
			{
				// This is an in-memory file that has been swapped out to disk.
				// Use the C-runtime Posix fopen() call to open the on-disk file.
				pfile->pdiskfile = fopen(pdir->szDiskName, mode);
			}
			else if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W)
			{
				wchar_t szMode[12];

#if defined(SYM_LINUX) || defined(SYM_AIX)	
				mbstate_t state;
				memset(&state,'\0',sizeof(state));
				mbsrtowcs(szMode, &mode, strlen(mode),&state);
#else
				mbstowcs(szMode, mode, strlen(mode));
#endif
				pfile->pdiskfile = NULL;

#if defined(_WINDOWS)
				pfile->pdiskfile = _wfopen(pdir->szDiskNameW, szMode);
#endif

				if (!pfile->pdiskfile)
				{
					errno = ENOENT;
					if (pxfile) delete pxfile;
					return(NULL);
				}
			}
			else
			{
				pfile->pdiskfile = NULL;
			}
		}
		else
		{
			// This is an on-disk file so use the C-runtime Posix fopen()
			// call to open the on-disk file.
			pfile->pdiskfile = fopen(filename, mode);
			if (!pfile->pdiskfile)
			{
				if (pxfile) delete pxfile;
				return(NULL);
			}
		}
	}
	else
	{
		// Did not find the entry in our cache directory.
		if (bIsUni)
			return (uni_fopen(filename, mode));

		// Must be an in-memory file reference at this point.
		// Check the mode parameter to see if we should create it.
		if (!(modeflag & _O_CREAT))
		{
			errno = ENOENT;
			return(NULL);
		}

		// Create the file in our cache.
		pxfile = AddDirEntry(filename, mode);
		if (!pxfile)
		{
			errno = ENOENT;
			return(NULL);
		}

		pfile = GET_MEMFILE(pxfile);
		dec_assert(pfile);

		// Save mode
		memset(pfile->szMode, 0, sizeof(pfile->szMode));
		strncpy(pfile->szMode, mode, sizeof(pfile->szMode) - 1);

		pdir = pfile->pdir;
		pdir->ftLastWrite = current_time;  // Save as last modified time
		pdir->ftCreate = current_time;	  // Save as creation time
	}

	pfile->uiFlags &= ~FILE_FLAG_EOF;
	pfile->ulPos = 0;  // Set file position to start of file.
	pfile->iMode = modeflag;

	if (modeflag & _O_TRUNC)
		pdir->ulSize = 0;

	if (modeflag & _O_APPEND)
		pfile->ulPos = pdir->ulSize;

	pfile->pdir = pdir;
	pdir->ftLastAccess = current_time; // Save as last access time

	if (modeflag & _O_WRONLY ||
		modeflag & _O_RDWR)
	{
		pdir->ftLastWrite = current_time;  // Save as last modified time
	}

	pdir->uiOpenCount++;

	// Found file.  Return pointer to it.
	return(pxfile);
}


int CDecFiles::x_fprintf(X_FILE *stream, const char *format, ...)
{
	#define MAX_TRIES 10
	#define MAX_BUFFER_SIZE 1024
	
	int		nLen = 0;
	int		nRetval = EOF;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(-1);
	
	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	CMemFile1 *pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return EOF;
	}

#if defined(NO_VSNPRINTF)
	char szBuffer[MAX_BUFFER_SIZE * 4];

	//
	// Format the string
	//
	try
	{
		va_list arglist;
		va_start(arglist, format);
		nLen = vsprintf(szBuffer, format, arglist); // unsafe because in NO_VSNPRINTF code block
		va_end(arglist);
	}

	catch(...)
	{
		dec_assert(0);
		return 0;
	}

	if (nLen > 0)
	{
		//
		// Delegate to any other filesystems if needed...
		//
		if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
			nRetval = m_pRapiFS->x_fwrite(szBuffer, sizeof(char), nLen, stream);

		else
			nRetval = x_fwrite(szBuffer, sizeof(char), nLen, stream);
	}

#else
	int		nNumberOfRetries = 0;
	int		nBufferSize = MAX_BUFFER_SIZE;
	char	*pszTemp;
	va_list vl;

	va_start(vl, format);

	pszTemp = (char *)malloc(nBufferSize);
	if (!pszTemp)
		return (EOF);

	while (true)
	{
		// There are some non-standard versions of vsnprintf() that do not
		// conform to either the C99 standard, or their own documentation.
		// 
		// vsnprintf() may return -1 if the buffer is not big enough, or
		// it may return the number of characters that would be needed to
		// create the full string, exclusive of the terminating NUL. We
		// will handle both cases. If -1 is returned, we will advance the 
		// buffer size by our increment amount. If a size is returned, we 
		// will use it (+1 for the NUL) on the next pass.

        // DaveVW 8/17/2005: replaced call to vsnprintf() with ssvsnprintf()
		nLen = ssvsnprintf(pszTemp, nBufferSize, format, vl);
		if ((nLen >= 0) && (nLen < nBufferSize))
		{
			//
			// Delegate to any other filesystems if needed...
			//
			if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
				nRetval = m_pRapiFS->x_fwrite(pszTemp, sizeof(char), nLen, stream);

			else
				nRetval = x_fwrite(pszTemp, sizeof(char), nLen, stream);

			break;
		}

		// Failed to format the output buffer so try again but with
		// a larger buffer.
		nNumberOfRetries++;
		if (nNumberOfRetries > MAX_TRIES)
			break;

		// See if we can get the right buffer size now...
		if ((nLen > 0) && (nLen >= nBufferSize))
			nBufferSize = nLen + 1;
		else
			nBufferSize += MAX_BUFFER_SIZE;

		{
			char* pszRealloced = (char *)realloc(pszTemp, nBufferSize);
			if (!pszRealloced)
			{
				free(pszTemp);
				pszTemp = NULL;
				break;
			}
			pszTemp = pszRealloced;
		}

		//
		// Each call to vsnprintf changes vl.  So, we need to reset it
		// each time.
		//
		va_start(vl, format);
	}

	va_end(vl);

	if (pszTemp)
		free(pszTemp);
#endif

	return nRetval;
}


int CDecFiles::x_fputc(int c, X_FILE *stream)
{
	CMemFile1		*pfile;
	unsigned char	ch;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
		return(EOF);

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_fputc(c, stream));

	if (pfile->pdiskfile)
		return(fputc(c, pfile->pdiskfile));

	ch = c;
	if (x_fwrite(&ch, 1, 1, stream) != 1)
		return(EOF);

	return(c & 0xff);
}


int CDecFiles::x_fputs(const char *szstring, X_FILE *stream)
{
	size_t		len;
	size_t		byteswritten;
	CMemFile1	*pfile;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream || !szstring)
		return(EOF);

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_fputs(szstring, stream));

	if (pfile->pdiskfile)
		return(fputs(szstring, pfile->pdiskfile));

	// Be sure we are tracking a directory entry for this stream.
	if (!pfile->pdir)
		return(EOF);

	len = strlen(szstring);
	byteswritten = x_fwrite(szstring, 1, len, stream);
	if (byteswritten != len)
		return(EOF);

	return(0);
}


size_t CDecFiles::x_fread(void *buffer, size_t size, size_t count, X_FILE *stream)
{
	CMemDir				*pdir;
	CMemFile1			*pfile;
	size_t				numread;
	unsigned char		*ptr;
	unsigned char		*ptrin;
	size_t				sizeBytesLeft;
	size_t				sizeAmountLeft;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(0);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
		return(0);

	// special-case single-byte reads
	if (size == 1 && count == 1)
	{
	  int ch = x_fgetc(stream);
	  if (ch == EOF)
		 return 0;
	  *((unsigned char *)buffer) = ch;
	  return 1;
	}

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(0);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_fread(buffer, size, count, stream));

	//
	// If on-disk, then read from the disk file using standard runtime
	//
	if (pfile->pdiskfile)
		return(fread(buffer, size, count, pfile->pdiskfile));


	// Initialize the in-memory file-system if it hasn't been already.
	if (!InitMemoryFileSystem())
	{
		errno = ENOMEM;
		return(0);
	}

	sizeBytesLeft = size * count;
	if (sizeBytesLeft == 0)
		return 0;

	pdir = pfile->pdir;
	if (!pdir)
		return(0);

	// Check for EOF
	if (pfile->ulPos >= pdir->ulSize)
	{
		pfile->uiFlags |= FILE_FLAG_EOF;
		return 0;
	}
	
	sizeAmountLeft = pdir->ulSize - pfile->ulPos;
	numread = sizeBytesLeft;
	if (numread > sizeAmountLeft)
	{
		// Set the end-of-file flag here because the caller asked us to read
		// more data than is left in the file.
		pfile->uiFlags |= FILE_FLAG_EOF;
		numread = sizeAmountLeft;
	}

	if (numread && pfile->iGetc != EOF)
	{
		unsigned char uch;

		uch = pfile->iGetc;
		pfile->iGetc = EOF;
		ptr = (unsigned char *)buffer;
		*ptr++ = uch;
		buffer = (void *)ptr;
		pfile->ulPos++;
		sizeBytesLeft--;
		numread--;
	}

	// The following code optimizes small read operations (avoids calling memcpy).
	ptr = (unsigned char *)buffer;
	ptrin = pdir->pStore + pfile->ulPos;
	switch (numread)
	{
		case 4:
			*ptr++ = *ptrin++;
		case 3:
			*ptr++ = *ptrin++;
		case 2:
			*ptr++ = *ptrin++;
		case 1:
			*ptr = *ptrin;
		case 0:
			break;
		default:
			memcpy(buffer, (unsigned char *)(pdir->pStore) + pfile->ulPos, numread);
			break;
	}

	pfile->ulPos += numread;
	sizeBytesLeft -= numread;
	return ((size * count - sizeBytesLeft) / size);
}


int CDecFiles::x_fseek(X_FILE *stream, long offset, int origin)
{
	CMemFile1	*pfile;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
		return(EOF);

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_fseek(stream, offset, origin));

	if (pfile->pdiskfile)
		return(fseek(pfile->pdiskfile, offset, origin));

	switch (origin)
	{
		// Offset relative to the beginning of the file.
		case SEEK_SET:
			if ( offset < 0 )
			{
				// We cannot seek backward from the beginning of the file
				return -1;
			}
			pfile->ulPos = offset;
			break;
		// Offset relative to the current file position.
		case SEEK_CUR:
			// See if we are moving backward.  If so
			// verify that we will not be moving beyond 
			// the beginning of the file
			if ( offset < 0  &&	pfile->ulPos < (unsigned)abs(offset) )
			{
				// We cannot seek backward beyond the beginning of the file
				return -1;
			}
			pfile->ulPos += offset;
			break;
		case SEEK_END:
		{
			CMemDir		*pdir;

			pdir = pfile->pdir;
			if (!pdir)
				return(-1);

			// See if we are moving backward.  If so
			// verify that we will not be moving beyond 
			// the beginning of the file
			if ( offset < 0  &&	 pdir->ulSize < (unsigned)abs(offset)  )
			{
				// We cannot seek backward beyond the beginning of the file
				return -1;
			}
			pfile->ulPos = pdir->ulSize + offset;
			break;
		}
		default:
			return(-1);
	}

	// Reset the stream's end-of-file indicator.
	pfile->uiFlags &= ~FILE_FLAG_EOF;
	return(0);
}


long CDecFiles::x_ftell(X_FILE *stream)
{
	CMemFile1	*pfile;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(-1L);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
		return(-1L);

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(-1L);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_ftell(stream));

	if (pfile->pdiskfile)
		return(ftell(pfile->pdiskfile));

	//
	// Ensure ulPos is not larger than the max value of a long
	//
	if(pfile->ulPos > LONG_MAX)
	{
		return(-1L);
	}

	return((long)pfile->ulPos);
}


#define ZERO_BUFFER_SIZE	32 * 1024

int CDecFiles::x_ftruncate(X_FILE *stream, size_t size)
{
	// This function changes the size of a file to the given size.
	// If the file is already larger than size, it is truncated.
	// If the file is smaller than size, bytes between the old and new
	// lengths are written as zeroes.
	// Upon successful completion, this function returns 0.  Otherwise,
	// -1 is returned and errno is set to indicate the error.
	size_t		ulOldSize;
	size_t		ulBytesLeft;
	size_t		ulBytesToWrite;
	CMemDir		*pdir = NULL;
	CMemFile1	*pfile;
	int			iMode;
	int			rc = -1;  // Assume failure
	unsigned char *pbZeroes = NULL;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
	{
		errno = EINVAL;
		return(EOF);
	}

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_ftruncate(stream, size));

	if (pfile->pdiskfile)
	{
		if (!SetEndOfFile(stream, size))
		{
			errno = EINVAL;
			return(EOF);
		}

		return(0);
	}

	// First, make sure the stream is open in a write mode.
	iMode = pfile->iMode & (_O_WRONLY | _O_RDWR);
	if (!iMode)
	{
		errno = EINVAL;
		goto done_ftruncate;
	}

	pdir = pfile->pdir;
	if (!pdir)
	{
		errno = EINVAL;
		goto done_ftruncate;
	}

	ulOldSize = pdir->ulSize;
	if (ulOldSize == size)
	{
		rc = 0;	// Return success!
		goto done_ftruncate;  // Size is not changing, so nothing more to do...
	}

	// See if the file is getting smaller.
	if (size < ulOldSize)
	{
		// Yes, the file is getting smaller.
		// Now just change the file's size to its new, smaller, size.
		pdir->ulSize = size;
		rc = 0;	// Return success!
		goto done_ftruncate;
	}

	// Here we know that the caller wants to increase the size of the file.
	// So, we must write out zeroes from the current end of the file up to
	// the new size.  To accomplish this:
	// 1.  Seek to the end of the file (x_fseek).
	// 2.  Keep writing a buffer filled with zeroes until the new file size is reached.
	if (x_fseek(stream, 0, SEEK_END))
		goto done_ftruncate;

	pbZeroes = new unsigned char[ZERO_BUFFER_SIZE];
	if (!pbZeroes)
	{
		errno = ENOMEM;
		goto done_ftruncate;
	}

	// Fill the buffer with zeroes
	memset(pbZeroes, 0, ZERO_BUFFER_SIZE);

	ulBytesLeft = size - ulOldSize;
	while (ulBytesLeft)
	{
		if (ulBytesLeft > ZERO_BUFFER_SIZE)
			ulBytesToWrite = ZERO_BUFFER_SIZE;
		else
			ulBytesToWrite = ulBytesLeft;

		if (x_fwrite(pbZeroes, ulBytesToWrite, 1, stream) != 1)
			goto done_ftruncate;

		ulBytesLeft -= ulBytesToWrite;
		// Note that when we are finished, we leave the file pointer at the end of the file.
	}

	rc = 0;	// Return success!

done_ftruncate:
	if (pbZeroes)
		delete [] pbZeroes;
	return (rc);
}


size_t CDecFiles::x_fwrite(const void *buffer, size_t size, size_t count, X_FILE *stream)
{
	CMemFile1		*pfile;
	CMemDir			*pdir;
	size_t			ulBytesLeft;
	unsigned char	*ptr;
	unsigned char	*ptrin;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(0);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!buffer || !stream)
		return(0);

	// Initialize the in-memory file-system if it hasn't been already.
	if (!InitMemoryFileSystem())
	{
		errno = ENOMEM;
		return 0;
	}

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(0);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_fwrite(buffer, size, count, stream));

	if (pfile->pdiskfile)
		return(fwrite(buffer, size, count, pfile->pdiskfile));

	if (pfile->iMode & _O_RDONLY)
	{
		// File is open for read-only.
		errno = EACCES;
		return(0);
	}

	ulBytesLeft = size * count;
	if (ulBytesLeft == 0)
		return(0);

	pdir = pfile->pdir;
	if (!pdir)
		return(0);

	if (pfile->iMode & _O_APPEND)
	{
		// File is open for append only, so reset the file position
		// to the last byte in the file to guarantee that we will always
		// be appending to the end of the file.
		pfile->ulPos = pdir->ulSize;
	}

	// Check here to see if we will exceed the maximum size allowed for any
	// one in-memory file.  If so, we need to swap this file out to disk.
	if (m_ulFileSizeThreshold < pfile->pdir->ulSize + ulBytesLeft)
	{
		if (!SwitchMemoryFileToDisk(stream, pfile->pdir))
			return 0;

		// OK, we swapped the in-memory file out to disk.
		// Now just re-direct the fwrite operation to the on-disk file.
		if (pfile->pdiskfile)
			return(fwrite(buffer, size, count, pfile->pdiskfile));
		return 0;  // Just in case pfile->pdiskfile came back NULL
				  // (which it never should if the swap succeeded).
	}

	// Before writing anything to storage, make sure the storage buffer is large
	// enough to satisfy the entire write request.  If it isn't, re-allocate
	// it first.
	if (pfile->ulPos + ulBytesLeft > pdir->ulStoreSize)
	{
		// Can't satisfy the entire write request.  Re-allocate the storage buffer.
		size_t			ulNewSize;

		if (pfile->ulPos > pdir->ulSize)
		{
			// This formula tries to balance between allocating enough memory
			// to minimize the number of re-allocations we need to do and minimizing
			// the amount of memory allocated.  Note that it is now very important to
			// set the maximum in-memory file size due to the * 2 in this formula.
			ulNewSize = (pfile->ulPos * 2) + ulBytesLeft + 256;
		}
		else
		{
			// This formula tries to balance between allocating enough memory
			// to minimize the number of re-allocations we need to do and minimizing
			// the amount of memory allocated.  Note that it is now very important to
			// set the maximum in-memory file size due to the * 2 in this formula.
			ulNewSize = (pdir->ulSize * 2) + ulBytesLeft + 256;
		}

		// Begin critical section.  Only one thread allowed through here
		// at a time.
		if (m_bMultiThread)
			m_CriticalFile.In();

		// Do not allow the in-memory file system size to exceed the maximum size
		// set by the client.
		if (m_ulFSSize + (ulNewSize - pdir->ulStoreSize) > m_ulMaxFSSize)
		{
			// End critical section.
			if (m_bMultiThread)
				m_CriticalFile.Out();

			if (!SwitchMemoryFileToDisk(stream, pfile->pdir))
				return 0;

			// OK, we swapped the in-memory file out to disk.
			// Now just re-direct the fwrite operation to the on-disk file.
			if (pfile->pdiskfile)
				return(fwrite(buffer, size, count, pfile->pdiskfile));
			return 0;  // Just in case pfile->pdiskfile came back NULL
					  // (which it never should if the swap succeeded).
		}

		// Add to our running tab of the in-memory file system size.
		m_ulFSSize += (ulNewSize - pdir->ulStoreSize);

		// End critical section.
		if (m_bMultiThread)
			m_CriticalFile.Out();

		// Re-allocate the storage buffer to its new, larger size.
		ptr = (unsigned char *)realloc(pdir->pStore, ulNewSize);
		if (!ptr)
		{
			// Failed to reallocate the buffer.  Take back the amount of
			// additional space that we were supposed to allocate.
			// Begin critical section.  Only one thread allowed through here
			// at a time.
			if (m_bMultiThread)
				m_CriticalFile.In();

			// Subtract from our running tab of the in-memory file system size.
			m_ulFSSize -= (ulNewSize - pdir->ulStoreSize);

			// End critical section.
			if (m_bMultiThread)
				m_CriticalFile.Out();

			errno = ENOMEM;
			return (0);
		}

		// Note: Do NOT do this assignment on the realloc call above since the
		// realloc can fail and if it does you will orphan the original buffer.
		pdir->pStore = ptr;
		pdir->ulStoreSize = ulNewSize;
	}

	// The following code optimizes small write operations (avoids calling memcpy).
	ptr = pdir->pStore + pfile->ulPos;
	ptrin = (unsigned char *)buffer;
	switch (ulBytesLeft)
	{
		case 4:
			*ptr++ = *ptrin++;
		case 3:
			*ptr++ = *ptrin++;
		case 2:
			*ptr++ = *ptrin++;
		case 1:
			*ptr = *ptrin;
		case 0:
			break;
		default:
			memcpy((unsigned char *)(pdir->pStore) + pfile->ulPos, buffer, ulBytesLeft);
			break;
	}

	pfile->ulPos += ulBytesLeft;
	if (pfile->ulPos > pdir->ulSize)
		pdir->ulSize = pfile->ulPos;

	return count;
}


int CDecFiles::x_remove(const char *path)
{
	CMemDir *pdir;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!path)
		return(EOF);

	if (!IS_MEM_NAMESPACE(path))
	{
		// Determine which namespace this file is in and delegate if we have to...
		if (IS_UNI_NAMESPACE(path))
			return (uni_remove(path));
		else if (IS_RAPI_NAMESPACE(path))
			return (m_pRapiFS->x_remove(path));

		// Default to an on-disk file.
		return(remove(path));
	}

	// Locate the named stream in our directory.
	pdir = FindEntry(path);
	if (!pdir)
		return(EOF);

	// See if the in-memory file has been swapped out to disk.
	if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
	{
		// Yes, the file has been swapped out to disk.
		// Delete the on-disk file.
		remove(pdir->szDiskName);
	}

	if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W)
	{
#if defined(_WINDOWS)
		_wremove(pdir->szDiskNameW);
#endif
	}

	if (pdir->uiOpenCount)
		return(EOF);

	// Free any allocated FAT blocks for this file.
	// Do NOT free blocks that have been attached via x_memattach!!!
	if (pdir->pStore &&
		!(pdir->uiFlags & MEMDIR_FLAG_ATTACHED))
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

	// Begin critical section.
	if (m_bMultiThread)
		m_CriticalDir.In();

	m_ppdir[pdir->index] = NULL;
	m_iCachedDirIndex = pdir->index;

	// End critical section.
	if (m_bMultiThread)
		m_CriticalDir.Out();

	delete pdir;
	return(0);
}


int CDecFiles::x_rename(const char *oldname, const char *newname)
{
	CMemDir *pdir;
	size_t	len;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(-1);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!oldname || !newname)
		return(EOF);

	if (!IS_MEM_NAMESPACE(oldname))
	{
		// Determine which namespace this file is in and delegate if we have to...
		if (IS_UNI_NAMESPACE(oldname))
			return (uni_rename(oldname, newname));
		else if (IS_RAPI_NAMESPACE(oldname))
			return (m_pRapiFS->x_rename(oldname, newname));

		return(rename(oldname, newname));
	}

	// Get the length of the new name.
	len = strlen(newname);
	if (!len || len > sizeof(pdir->szName) - 1)
		return(EOF);

	// Locate the named stream in our directory.
	pdir = FindEntry(oldname);
	if (!pdir)
		return(EOF);

	// Copy the new name over the old one.
	memset(pdir->szName, 0, sizeof(pdir->szName));
	strncpy(pdir->szName, newname, sizeof(pdir->szName) - 1);
	return(0);
}


void CDecFiles::x_rewind(X_FILE *stream)
{
	CMemFile1	*pfile;

	if (!stream)
		return;

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();
	
	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return;
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
	{
		m_pRapiFS->x_rewind(stream);
		return;
	}

	if (pfile->pdiskfile)
		rewind(pfile->pdiskfile);

	pfile->ulPos = 0;
	pfile->uiFlags &= ~FILE_FLAG_EOF;
}


int CDecFiles::x_setvbuf(X_FILE *stream, char *buffer, int mode, size_t size)
{
	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	CMemFile1 *pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_setvbuf(stream, buffer, mode, size));

	//
	// Unsupported by CDecFiles
	//
	return(EOF);
}


#if defined(_WINDOWS)
int CDecFiles::x_fstat(X_FILE *stream, struct _stat *buffer)
#else
int CDecFiles::x_fstat(X_FILE *stream, struct stat *buffer)
#endif
{
	int			rc = EOF;
	CMemDir		*pdir = NULL;
	CMemFile1	*pfile;
	char		*pszPath = NULL;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream || !buffer)
	{
		errno = ENOENT;
		return(EOF);
	}

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_fstat(stream, buffer));

	if (pfile->pdiskfile)
	{
#if defined(_WINDOWS)
		return (_fstat(fileno(pfile->pdiskfile), buffer));
#else
		return (fstat(fileno(pfile->pdiskfile), buffer));
#endif
	}

	pdir = pfile->pdir;
	if (!pdir)
	{
		errno = ENOENT;
		return(EOF);
	}

	pszPath = (char *)malloc(strlen(pdir->szName) + 1);
	if (!pszPath)
	{
		errno = ENOENT;
		return(EOF);
	}

	strcpy(pszPath, pdir->szName);
	rc = x_stat(pszPath, buffer);
	free (pszPath);
	return rc;
}


#if defined(_WINDOWS)
int CDecFiles::x_stat(const char *path, struct _stat *buffer)
#else
int CDecFiles::x_stat(const char *path, struct stat *buffer)
#endif
{
	CMemDir		*pdir;

	// mode values:
	// 00 - Existence only
	// 02 - Write permission
	// 04 - Read permission
	// 06 - Read and write permission

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!path || !buffer)
	{
		errno = ENOENT;
		return(EOF);
	}

	if (!IS_MEM_NAMESPACE(path))
	{
		// Determine which namespace this file is in and delegate if we have to...
		if (IS_UNI_NAMESPACE(path))
			return (uni_stat(path, buffer));
		else if (IS_RAPI_NAMESPACE(path))
			return (m_pRapiFS->x_stat(path, buffer));

#if defined(_WINDOWS)
		return(_stat(path, buffer));
#else
		return(stat(path, buffer));
#endif
	}

	// Locate the named stream in our directory.
	pdir = FindEntry(path);
	if (pdir)
	{
		// Yes, the file exists.
		// See if the file has actually been swapped out to disk.
		if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
		{
			// Yes, the file has been swapped out to disk, so just pass the
			// request on down to the local file system.
#if defined(_WINDOWS)
			return(_stat(pdir->szDiskName, buffer));
#else
			return(stat(pdir->szDiskName, buffer));
#endif
		}

		//
		// Initialize buffer to zero and then set any fields that are non-zero
		// individually.  This is because some platforms (AS400 for example)
		// do not have some of these fields (like st_rdev) and doing it
		// this way removes the need for an ifdef around a particular block.
		// Since st_rdev gets set to zero anyway, if we just memset the whole
		// structure to zero, we guarantee that these fields will be correctly
		// initialized regardless of the platform.  This may change someday
		// but for now it is a cleaner way to do it.
		//
#if defined(_WINDOWS)
		memset(buffer, 0, sizeof(struct _stat));
#else
		memset(buffer, 0, sizeof(struct stat));
#endif
		//
		// Initialize any non-zero fields here
		//
		buffer->st_mode = _S_IFREG;		// File-mode bits (_S_IFDIR = directory, _S_IFREG = normal file)
		buffer->st_nlink = 1;			// Always 1 on non-NTFS file-systems
		buffer->st_size = pdir->ulSize; // File size in bytes
		buffer->st_atime = pdir->ftLastAccess; // Last access time
		buffer->st_mtime = pdir->ftLastWrite;  // Last modified time
		buffer->st_ctime = pdir->ftCreate;	  // Creation time
		return(0);
	}

	// We did not find an entry for this name so return an error.
	errno = ENOENT;
	return(EOF);
}


int CDecFiles::x_ungetc(int c, X_FILE *stream)
{
	CMemFile1		*pfile;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!stream)
		return(EOF);

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_ungetc(c, stream));

	if (pfile->pdiskfile)
		return(ungetc(c, pfile->pdiskfile));

	// Obviously we can't write back a byte that we have not read.
	if (pfile->ulPos == 0)
		return(EOF);

	pfile->ulPos--;
	pfile->iGetc = c;
	pfile->uiFlags &= ~FILE_FLAG_EOF;
	return(c);
}


#if defined(_WINDOWS)
int CDecFiles::x_futime(X_FILE *stream, struct _utimbuf *filetime)
#else
int CDecFiles::x_futime(X_FILE *stream, struct utimbuf *filetime)
#endif
{
	int			rc = EOF;
	CMemDir		*pdir = NULL;
	CMemFile1	*pfile;
	char		*pszPath = NULL;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);
	
	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	//
	// Validate input (filetime can be NULL! - Sets mod/access time to current
	// local time).
	//
	if (!stream)
	{
		errno = ENOENT;
		return(EOF);
	}

	pfile = GET_MEMFILE(stream);
	if(!pfile)
	{
		dec_assert(0);
		return(EOF);
	}

	//
	// Delegate to any other filesystems if needed...
	//
	if (pfile->dwFileSystem & FILE_SYSTEM_RAPI)
		return(m_pRapiFS->x_futime(stream, filetime));

	if (pfile->pdiskfile)
	{
#if defined(_WINDOWS)
		rc = _futime(fileno(pfile->pdiskfile), filetime);
#else
		// futime is not POSIX compliant.  Therefore, if this
		// file is an on-disk file, we must fail this call. The
		// exception is if it was an in-memory file that was
		// swapped to disk, in which case we have access to the
		// file name and can then call x_utime() to accomplish
		// what we need.

		errno = ENOENT;
		return(EOF);
#endif
	}

	pdir = pfile->pdir;
	if (!pdir)
	{
		errno = ENOENT;
		return(EOF);
	}

	pszPath = (char *)malloc(strlen(pdir->szName) + 1);
	if (!pszPath)
	{
		errno = ENOENT;
		return(EOF);
	}

	strcpy(pszPath, pdir->szName);
	rc = x_utime(pszPath, filetime);
	free(pszPath);
	return rc;
}


#if defined(_WINDOWS)
int CDecFiles::x_utime(const char *filename, struct _utimbuf *times)
#else
int CDecFiles::x_utime(const char *filename, struct utimbuf *times)
#endif
{
	CMemDir		*pdir;
	int			rc = 0;

	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(EOF);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	// 'times' is allowed to be NULL!
	if (!filename)
	{
		errno = EINVAL;
		return(EOF);
	}

	if (!IS_MEM_NAMESPACE(filename))
	{
		// Determine which namespace this file is in and delegate if we have to...
		if (IS_UNI_NAMESPACE(filename))
			return (uni_utime(filename, times));
		else if (IS_RAPI_NAMESPACE(filename))
			return (m_pRapiFS->x_utime(filename, times));

#if defined(_WINDOWS)
		return (_utime(filename, times));
#else
		return (utime(filename, times));
#endif
	}

	// Locate the named stream in our directory.
	pdir = FindEntry(filename);
	if (!pdir)
	{
		errno = ENOENT;
		return(EOF);
	}

	// See if the in-memory file has been swapped out to disk.
	if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED)
	{
#if defined(_WINDOWS)
		rc = _utime(pdir->szDiskName, times);
#else
		rc = utime(pdir->szDiskName, times);
#endif
	}
	else if (pdir->uiFlags & MEMDIR_FLAG_SWAPPED_W)
	{
#if defined(_WINDOWS)
		rc = _wutime(pdir->szDiskNameW, times);
#else
		rc = EOF;
#endif
	}

	// If times is not NULL, then use the times that were passed in.
	if (times)
	{
		pdir->ftLastAccess = times->actime;
		pdir->ftLastWrite = times->modtime;
	}
	else
	{
		// Otherwise, use the current local time
		time(&(pdir->ftLastAccess));
		pdir->ftLastWrite = pdir->ftLastAccess;
	}

	return(rc);
}


bool CDecFiles::x_memattach(char *pszName, size_t ulNameBufSize, void *ptr, size_t ulSize)
{
	//
	// Special debug API check.  Compiles to nothing
	// unless special flag defined in DecFS.h
	//
	CHECK_FOR_FAIL_TOPLEVEL_REQUEST(false);

	//
	// Give other threads a chance to run. Compiles to nothing
	// on any platform except Netware.
	//
	YIELD();

	if (!pszName || !ptr || (ulNameBufSize == 0) || (ulSize == 0))
		return false;
	
	return AttachMemoryBlock(pszName, ulNameBufSize, ptr, ulSize);
}


void CDecFiles::SetNetService(INetData *pnet)
{
	m_pnet = pnet;
}


DECSDKLINKAGE int STDCALL DAPI_NewFileSystem(IDecFiles **ppDecFS)
{
	try
	{
		*ppDecFS = new CDecFiles;
		return DAPI_OK;
	}
	catch (...)
	{
		*ppDecFS = NULL;
		return DAPI_ERROR;
	}
}


DECSDKLINKAGE int STDCALL DAPI_DeleteFileSystem(IDecFiles *pDecFS)
{
	if (pDecFS)
	{
		delete pDecFS;
	}

	return DAPI_OK;
}

#if defined(SYM_FAIL_FILESYSTEM_CALLS)

_declspec(dllexport) void STDCALL FailFileSystemCalls(int nFailWhichFSCalls)
{
	g_nFailWhichFSCalls = nFailWhichFSCalls;
}

#endif

