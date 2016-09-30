// DecIO.h : Decomposer I/O Interface
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DECIO_H)
#define DECIO_H

#if defined(SYM_LINUX)
	#include <stdio.h>
#endif

#include "DecFS.h"
#include "dectmpf.h"

#include "SymSaferStrings.h"

#if defined(_WINDOWS)
	#include <io.h>
	#include <errno.h>
	#include <sys/utime.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#else
	#include <stdarg.h>
	#include <utime.h>
	#include <unistd.h>		// For ftruncate on UNIX and chsize on Netware
#endif

#if defined(SYM_NLM)
	#include <sys/stat.h>
	#include "dec_assert.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// Interface IDecIO - interface definition for Decomposer file I/O callbacks
// C++ interface for Decomposer stream I/O functions.
// These functions are defined exactly the same as the corresponding
// POSIX functions so that Decomposer clients can base their implementations
// on the POSIX standard.

class IDecIO :
	public IDecIOCB
{
public:
	IDecIO() { }
	virtual ~IDecIO() { }

	// IDecIO methods:
	// Note that the client can be in complete control of what the
	// pszDataSourceName string below is simply by implementing Dec_fopen
	// and providing the necessary translation.  Also note that the
	// client will thus be able to pass any arbitrary name to the Decomposer.
	// The caveat is that because some of the Decomposer engines sometimes
	// need to examine the extension of a file, it would be advisable
	// (but not required) for the client to append a meaningful extension
	// to the name.
	// The default implementation assumes pszDataSourceName to be a filename
	// in the currently operating file system.

	virtual int		Dec_access(const char *path, int mode) = 0;
	virtual int		Dec_chmod(const char *filename, int pmode) = 0;
	virtual void	Dec_clearerr(FILE *stream) = 0;
	virtual int		Dec_fclose(FILE *stream) = 0;
	virtual int		Dec_feof(FILE *stream) = 0;
	virtual int		Dec_ferror(FILE *stream) = 0;
	virtual int		Dec_fflush(FILE *stream) = 0;
	virtual int		Dec_fgetc(FILE *stream) = 0;
	virtual char	*Dec_fgets(char *string, int n, FILE *stream) = 0;
	virtual FILE	*Dec_fopen(const char *filename, const char *mode) = 0;
	virtual int		Dec_fputc(int c, FILE *stream) = 0;
	virtual int		Dec_fputs(const char *szstring, FILE *stream) = 0;
	virtual size_t	Dec_fread(void *buffer, size_t size, size_t count, FILE *stream) = 0;
	virtual int		Dec_fseek(FILE *stream, long offset, int origin) = 0;
	virtual long	Dec_ftell(FILE *stream) = 0;
	virtual int		Dec_ftruncate(FILE *stream, size_t size) = 0;
	virtual size_t	Dec_fwrite(const void *buffer, size_t size, size_t count, FILE *stream) = 0;
	virtual int		Dec_remove(const char *path) = 0;
	virtual int		Dec_rename(const char *oldname, const char *newname) = 0;
	virtual void	Dec_rewind(FILE *stream) = 0;
	virtual int		Dec_setvbuf(FILE *stream, char *buffer, int mode, size_t size) = 0;
#if defined(_WINDOWS)	
	virtual int		Dec_stat(const char *path, struct _stat *buffer) = 0;
	virtual int		Dec_utime(const char *filename, struct _utimbuf *times) = 0;
#else
	virtual int		Dec_stat(const char *path, struct stat *buffer) = 0;
	virtual int		Dec_utime(const char *filename, struct utimbuf *times) = 0;
#endif
	virtual int		Dec_ungetc(int c, FILE *stream) = 0;
	virtual int		Dec_fprintf(FILE *stream, const char *format, ...) = 0;

	virtual void	SetFileSystem(IDecFiles *pfs) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// Class CDecIO - Standard/default implementation of Decomposer file I/O
//                callbacks.

class CDecIO :
	public IDecIO
{
public:
	virtual int		Dec_access(const char *path, int mode)
	{
		return (access(path, mode));
	}

	virtual int		Dec_chmod(const char *filename, int pmode)
	{
		return (chmod(filename, pmode));
	}

	virtual void	Dec_clearerr(FILE *stream)
	{
		clearerr(stream);
		return;
	}

	virtual int		Dec_fclose(FILE *stream)
	{
		return (fclose(stream));
	}

	virtual int		Dec_feof(FILE *stream)
	{
		return (feof(stream));
	}

	virtual int		Dec_ferror(FILE *stream)
	{
		return (ferror(stream));
	}

	virtual int		Dec_fflush(FILE *stream)
	{
		return (fflush(stream));
	}

	virtual int		Dec_fgetc(FILE *stream)
	{
		return (fgetc(stream));
	}

	virtual char	*Dec_fgets(char *string, int n, FILE *stream)
	{
		return fgets(string, n, stream);
	}

	virtual FILE	*Dec_fopen(const char *filename, const char *mode)
	{
		return (fopen(filename, mode));
	}

	virtual int		Dec_fputc(int c, FILE *stream)
	{
		return (fputc(c, stream));
	}

	virtual int		Dec_fputs(const char *szstring, FILE *stream)
	{
		return (fputs(szstring, stream));
	}

	virtual size_t	Dec_fread(void *buffer, size_t size, size_t count, FILE *stream)
	{
		return (fread(buffer, size, count, stream));
	}

	virtual int		Dec_fseek(FILE *stream, long offset, int origin)
	{
		return (fseek(stream, offset, origin));
	}

	virtual long	Dec_ftell(FILE *stream)
	{
		return (ftell(stream));
	}

	virtual int		Dec_ftruncate(FILE *stream, size_t size)
	{
#if defined(_WINDOWS) || defined(SYM_NLM)
		return chsize(fileno(stream), size);
#else
		return ftruncate(fileno(stream), size);
#endif
	}

	virtual size_t	Dec_fwrite(const void *buffer, size_t size, size_t count, FILE *stream)
	{
		return (fwrite(buffer, size, count, stream));
	}

	virtual int		Dec_remove(const char *path)
	{
		#if defined(DEC_REUSE_TEMP_FILES)
			return (dec_rm(path));
		#else
			return (remove(path));
		#endif
	}

	virtual int		Dec_rename(const char *oldname, const char *newname)
	{
		return (rename(oldname, newname));
	}

	virtual void	Dec_rewind(FILE *stream)
	{
		rewind(stream);
		return;
	}

	virtual int		Dec_setvbuf(FILE *stream, char *buffer, int mode, size_t size)
	{
		return (setvbuf(stream, buffer, mode, size));
	}

#if defined(_WINDOWS)	
	virtual int		Dec_stat(const char *path, struct _stat *buffer)
	{
		return (_stat(path, buffer));
	}
#else
	virtual int		Dec_stat(const char *path, struct stat *buffer)
	{
		return (stat(path, buffer));
	}
#endif	 

#if defined(_WINDOWS)	
	virtual int		Dec_utime(const char *filename, struct _utimbuf *times)
	{
		return (_utime(filename, times));
	}
#else
	virtual int		Dec_utime(const char *filename, struct utimbuf *times)
	{
		return (utime(filename, times));
	}
#endif

	virtual int		Dec_ungetc(int c, FILE *stream)
	{
		return (ungetc(c, stream));
	}

	virtual int		Dec_fprintf(FILE *stream, const char *format, ...)
	{
		#define MAX_TRIES 10
		#define MAX_BUFFER_SIZE 1024
		
		int		nLen = 0;
		int		nRetval = -1;

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
			nRetval = Dec_fwrite(szBuffer, sizeof(char), nLen, stream);

#else
		int		nNumberOfRetries = 0;
		int		nBufferSize = MAX_BUFFER_SIZE;
		char	*pszTemp;
		va_list vl;

		va_start(vl, format);

		pszTemp = (char *)malloc(nBufferSize);
		if (!pszTemp)
			return (-1);
  
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

            // DaveVW 8/17/2005: changed vsnprintf() call to ssvsnprintf()
			nLen = ssvsnprintf(pszTemp, nBufferSize, format, vl);
			if ((nLen >= 0) && (nLen < nBufferSize))
			{
				// Successfully formatted the output buffer so write it out.
				nRetval = Dec_fwrite(pszTemp, sizeof(char), nLen, stream);
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

	virtual void	SetFileSystem(IDecFiles * /*pfs*/)
	{
	}
};


// Decomposer API for setting the client callback class pointer.
void DecSetIOCallback(IDecIOCB *pIOCallback);


#endif	// DECIO_H

