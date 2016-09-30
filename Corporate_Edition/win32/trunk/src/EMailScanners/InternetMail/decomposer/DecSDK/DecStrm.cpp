//------------------------------------------------------------------------
// DecStrm.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2000, 2005. All rights reserved.
//------------------------------------------------------------------------

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#define DAPI_CPP
#include "DecPlat.h"
#include "DAPI.h"

#if !defined(_WINDOWS)
	#include <stdarg.h>
#endif

#include "SymSaferStrings.h"

CDecSDKIO::CDecSDKIO()
{
	m_pfs = NULL;
}


CDecSDKIO::~CDecSDKIO()
{
}


int CDecSDKIO::Dec_access(const char *path, int mode)
{
	return (m_pfs->x_access(path, mode));
}


int CDecSDKIO::Dec_chmod(const char *filename, int pmode)
{
	return (m_pfs->x_chmod(filename, pmode));
}


void CDecSDKIO::Dec_clearerr(FILE *stream)
{
	m_pfs->x_clearerr((X_FILE *)stream);
}


int CDecSDKIO::Dec_fclose(FILE *stream)
{
	return (m_pfs->x_fclose((X_FILE *)stream));
}


int CDecSDKIO::Dec_feof(FILE *stream)
{
	return (m_pfs->x_feof((X_FILE *)stream));
}


int CDecSDKIO::Dec_ferror(FILE *stream)
{
	return (m_pfs->x_ferror((X_FILE *)stream));
}


int CDecSDKIO::Dec_fflush(FILE *stream)
{
	return (m_pfs->x_fflush((X_FILE *)stream));
}


int CDecSDKIO::Dec_fgetc(FILE *stream)
{
	return (m_pfs->x_fgetc((X_FILE *)stream));
}

char *CDecSDKIO::Dec_fgets(char *string, int n, FILE *stream)
{
	return (m_pfs->x_fgets(string, n, (X_FILE *)stream));
}


FILE *CDecSDKIO::Dec_fopen(const char *filename, const char *mode)
{
	return ((FILE *)m_pfs->x_fopen(filename, mode));
}


int CDecSDKIO::Dec_fputc(int c, FILE *stream)
{
	return (m_pfs->x_fputc(c, (X_FILE *)stream));
}


int CDecSDKIO::Dec_fputs(const char *szstring, FILE *stream)
{
	return (m_pfs->x_fputs(szstring, (X_FILE *)stream));
}


size_t CDecSDKIO::Dec_fread(void *buffer, size_t size, size_t count, FILE *stream)
{
	return (m_pfs->x_fread(buffer, size, count, (X_FILE *)stream));
}


int CDecSDKIO::Dec_fseek(FILE *stream, long offset, int origin)
{
	return (m_pfs->x_fseek((X_FILE *)stream, offset, origin));
}


long CDecSDKIO::Dec_ftell(FILE *stream)
{
	return (m_pfs->x_ftell((X_FILE *)stream));
}


int CDecSDKIO::Dec_ftruncate(FILE *stream, size_t size)
{
	return (m_pfs->x_ftruncate((X_FILE *)stream, size));
}


size_t CDecSDKIO::Dec_fwrite(const void *buffer, size_t size, size_t count, FILE *stream)
{
	return (m_pfs->x_fwrite(buffer, size, count, (X_FILE *)stream));
}


int CDecSDKIO::Dec_remove(const char *path)
{
	return (m_pfs->x_remove(path));
}


int CDecSDKIO::Dec_rename(const char *oldname, const char *newname)
{
	return (m_pfs->x_rename(oldname, newname));
}


void CDecSDKIO::Dec_rewind(FILE *stream)
{
	m_pfs->x_rewind((X_FILE *)stream);
}


int CDecSDKIO::Dec_setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	return (m_pfs->x_setvbuf((X_FILE *)stream, buffer, mode, size));
}


#if defined(_WINDOWS)
int CDecSDKIO::Dec_stat(const char *path, struct _stat *buffer)
#else
int CDecSDKIO::Dec_stat(const char *path, struct stat *buffer)
#endif
{
	return (m_pfs->x_stat(path, buffer));
}


int CDecSDKIO::Dec_ungetc(int c, FILE *stream)
{
	return (m_pfs->x_ungetc(c, (X_FILE *)stream));
}


#if defined(_WINDOWS)
int CDecSDKIO::Dec_utime(const char *filename, struct _utimbuf *times)
#else
int CDecSDKIO::Dec_utime(const char *filename, struct utimbuf *times)
#endif
{
	return (m_pfs->x_utime(filename, times));
}


int	CDecSDKIO::Dec_fprintf(FILE *stream, const char *format, ...)
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
		nRetval = (m_pfs->x_fwrite(szBuffer, sizeof(char), nLen, (X_FILE *)stream));

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

        // DaveVW 8/17/2005: replaced vsnprintf() with call to ssvsnprintf() known to comply with C99 std
		nLen = ssvsnprintf(pszTemp, nBufferSize, format, vl);
		if ((nLen >= 0) && (nLen < nBufferSize))
		{
			// Successfully formatted the output buffer so write it out.
			nRetval = (m_pfs->x_fwrite(pszTemp, sizeof(char), nLen, (X_FILE *)stream));
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


void CDecSDKIO::SetFileSystem(IDecFiles *pfs)
{
	m_pfs = pfs;
}


DECSDKLINKAGE int STDCALL DAPI_NewIOService(IDecIO **ppDecIO)
{
	try
	{
		*ppDecIO = new CDecSDKIO;
		return DAPI_OK;
	}
	catch (...)
	{
		*ppDecIO = NULL;
		return DAPI_ERROR;
	}
}


DECSDKLINKAGE int STDCALL DAPI_DeleteIOService(IDecIO *pDecIO)
{
	if (pDecIO)
	{
		delete pDecIO;
	}

	return DAPI_OK;
}
