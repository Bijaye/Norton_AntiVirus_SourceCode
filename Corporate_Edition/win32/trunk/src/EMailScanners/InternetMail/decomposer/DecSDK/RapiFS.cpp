//------------------------------------------------------------------------
// RapiFS.cpp
//
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) Symantec, Corp. 2001, 2005. All rights reserved.
//------------------------------------------------------------------------

// Fix the defect in IBM's header files for V3.x of Visual Age for C++.
#include "fixibmps.h"

#include "SymSaferStrings.h"

// Project headers
#define DAPI_CPP
#include "DecPlat.h"
#include "DAPI.h"
#include "SDKMacros.h"
#include "DecFS.h"
#include "RapiFs.h"
#include "Support.h"
#include "asc_char.h"
#include "PortableTm.h"

#if defined(_WINDOWS)
	#include "Rapi.h"
	#include <atlbase.h>
#endif

#include <time.h>
#include <errno.h>
#include <stddef.h>
#include "dec_assert.h"

////////////////////////////////////////////////////////////////////////
// CRapiFile implementation
//
						
						//
						// Initialize any needed base class members...
						//
CRapiFile::CRapiFile() : CDecSDKFileBase(FILE_SYSTEM_RAPI)

{
	hFileHandle = NULL;
	pwszFilename = NULL;
	bAppendMode = false;
	bTextMode = false;
	bHaveUngetcBuff = false;
	nChar = '\0';
	pbyBuffer = NULL;	
	nIndexInBuffer = EOF;	// EOF == no buffer yet
}


CRapiFile::~CRapiFile()
{
	if(pbyBuffer)
	{
		free(pbyBuffer);
		pbyBuffer = NULL;
	}

	if(pwszFilename)
	{
		free(pwszFilename);
		pwszFilename = NULL;
	}

	dec_assert(hFileHandle == NULL);
}


////////////////////////////////////////////////////////////////////////
// CRapiFS implementation
//

CRapiFS::CRapiFS()
{
	m_bInitialized = false;
	m_bMultiThread = false;
	m_pszTempPath = NULL;	
	m_hInst = NULL;
	m_pfnCreateFile = NULL;
	m_pfnCloseHandle = NULL;
	m_pfnReadFile = NULL;
	m_pfnWriteFile = NULL;
	m_pfnSetFilePointer = NULL;
	m_pfnGetFileAttribs = NULL;
	m_pfnSetFileAttribs = NULL;
	m_pfnGetFileSize = NULL;
	m_pfnSetEndOfFile = NULL;
	m_pfnGetFileTime = NULL;
	m_pfnSetFileTime = NULL;
	m_pfnMoveFile = NULL;
	m_pfnDeleteFile = NULL;
	m_pfnGetLastError = NULL;
	m_pfnRapiGetError = NULL;
	m_dwLastError = 0;
	m_dwRapiError = 0;
}


CRapiFS::~CRapiFS()
{
#if defined(_WINDOWS)
	if(m_hInst)
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
#endif

	if(m_pszTempPath)
	{
		free(m_pszTempPath);
		m_pszTempPath = NULL;
	}

	m_pfnCreateFile = NULL;
	m_pfnCloseHandle = NULL;
	m_pfnReadFile = NULL;
	m_pfnWriteFile = NULL;
	m_pfnSetFilePointer = NULL;
	m_pfnGetFileAttribs = NULL;
	m_pfnSetFileAttribs = NULL;
	m_pfnGetFileSize = NULL;
	m_pfnSetEndOfFile = NULL;
	m_pfnGetFileTime = NULL;
	m_pfnSetFileTime = NULL;
	m_pfnMoveFile = NULL;
	m_pfnDeleteFile = NULL;
	m_pfnGetLastError = NULL;
	m_pfnRapiGetError = NULL;
	m_dwLastError = 0;
	m_dwRapiError = 0;

	m_bInitialized = false;
}

//
// Load RAPI.DLL and get all of our function pointers...
//
bool CRapiFS::Init()
{
#if !defined(_WINDOWS)
	return false;
#else
	//
	// Try loading the library.
	//
	if(!(m_hInst = LoadLibraryEx(gszRapiFilename, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)))
	{
		//
		// Couldn't find RAPI.DLL
		//
		return false;
	}

	//
	// Get the functions pointers we need
	//
	m_pfnCreateFile = (PFNCREATEFILE)GetProcAddress(m_hInst, "CeCreateFile");                
	m_pfnCloseHandle = (PFNCLOSEHANDLE)GetProcAddress(m_hInst, "CeCloseHandle");
	m_pfnReadFile = (PFNREADFILE)GetProcAddress(m_hInst, "CeReadFile");
	m_pfnWriteFile = (PFNWRITEFILE)GetProcAddress(m_hInst, "CeWriteFile");    
	m_pfnSetFilePointer = (PFNSETFILEPOINTER)GetProcAddress(m_hInst, "CeSetFilePointer");
	m_pfnGetFileAttribs = (PFNGETFILEATTRIBS)GetProcAddress(m_hInst, "CeGetFileAttributes");
	m_pfnSetFileAttribs = (PFNSETFILEATTRIBS)GetProcAddress(m_hInst, "CeSetFileAttributes");
	m_pfnGetFileSize = (PFNGETFILESIZE)GetProcAddress(m_hInst, "CeGetFileSize"); 
	m_pfnSetEndOfFile = (PFNSETENDOFFILE)GetProcAddress(m_hInst, "CeSetEndOfFile");
	m_pfnGetFileTime = (PFNGETFILETIME)GetProcAddress(m_hInst, "CeGetFileTime");
	m_pfnSetFileTime = (PFNSETFILETIME)GetProcAddress(m_hInst, "CeSetFileTime"); 
	m_pfnMoveFile = (PFNMOVEFILE)GetProcAddress(m_hInst, "CeMoveFile");
	m_pfnDeleteFile = (PFNDELETEFILE)GetProcAddress(m_hInst, "CeDeleteFile");    
	m_pfnGetLastError = (PFNGETLASTERROR)GetProcAddress(m_hInst, "CeGetLastError");
	m_pfnRapiGetError = (PFNRAPIGETERROR)GetProcAddress(m_hInst, "CeRapiGetError");

	//
	// Make sure we succeeded
	//
	if(!m_pfnCreateFile		|| 
	  !m_pfnCloseHandle	||
	  !m_pfnReadFile		||
	  !m_pfnWriteFile		||
	  !m_pfnSetFilePointer ||
	  !m_pfnGetFileAttribs ||
	  !m_pfnSetFileAttribs ||
	  !m_pfnGetFileSize	||
	  !m_pfnSetEndOfFile	||
	  !m_pfnGetFileTime	||
	  !m_pfnSetFileTime	||
	  !m_pfnMoveFile		||
	  !m_pfnDeleteFile		|| 
	  !m_pfnGetLastError	||
	  !m_pfnRapiGetError)
	{
		return false;
	}

	m_bInitialized = true;
	return true;
#endif
}


int CRapiFS::x_access(const char *path, int mode)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	USES_CONVERSION;

	if(!m_bInitialized || !path)
	{
		return EOF;
	}

	int nReturn = EOF;
	DWORD dwAttribs = (*m_pfnGetFileAttribs)(A2W(path));

	switch(mode)
	{
		//
		// Existence only
		//
		case 0x00:
			nReturn = 0 ? (dwAttribs != 0xFFFFFFFF) : EOF;
			break;

		//
		// Write permission, or Read/Write permission
		//
		case 0x02:
		case 0x06:
			if(0xFFFFFFFF == dwAttribs)
			{
				//
				// File not found
				//
				nReturn = 0;
			}

			else
			{
				nReturn = 0 ? !(dwAttribs & FILE_ATTRIBUTE_READONLY) : EOF;
			}
			break;

		//
		// Read permission
		//
		case 0x04:
			if(0xFFFFFFFF == dwAttribs)
			{
				//
				// File not found
				//
				nReturn = 0;
			}

			else
			{
				//
				// Files are always readable...
				//
				nReturn = 0;
			}
			break;

		default:
			dec_assert(0);
	}

	return nReturn;
#endif
}


int CRapiFS::x_chmod(const char *filename, int pmode)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	USES_CONVERSION;

	if(!m_bInitialized || !filename)
	{
		return EOF;
	}

	DWORD dwAttribs = 0;

	switch(pmode)
	{
		case _S_IWRITE:
		case _S_IWRITE | _S_IREAD:
			dwAttribs = FILE_ATTRIBUTE_NORMAL;
			break;

		case _S_IREAD:
			dwAttribs = FILE_ATTRIBUTE_READONLY;
			break;

		default:
			dec_assert(0);
			return EOF;
	}

	return (((*m_pfnSetFileAttribs)(A2W(filename), dwAttribs)) ? 0 : EOF);
#endif
}


void CRapiFS::x_clearerr(X_FILE *stream)
{
#if !defined(_WINDOWS)
	return;
#else
	if(!m_bInitialized || !stream)
	{
		return;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);    
	pFile->uiFlags &= (~FILE_FLAG_EOF & ~FILE_FLAG_ERROR);
#endif
}


int CRapiFS::x_fclose(X_FILE *stream)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);   
	int nReturn = (*m_pfnCloseHandle)(pFile->hFileHandle) ? 0 : EOF;

	//
	// Once the file is closed, delete the file and stream objects
	//    
	pFile->hFileHandle = NULL;

	delete pFile;
	pFile = NULL;

	delete stream;
	stream = NULL;

	return nReturn;
#endif
}


int CRapiFS::x_fcloseall(void)
{
	//
	// Not implemented. 
	//
	return EOF;
}


int CRapiFS::x_feof(X_FILE *stream)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	return(pFile->uiFlags & FILE_FLAG_EOF);
#endif
}


int CRapiFS::x_ferror(X_FILE *stream)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	return(pFile->uiFlags & FILE_FLAG_ERROR);
#endif
}


int CRapiFS::x_fflush(X_FILE *stream)
{
	//
	// Not implemented. 
	//
	return EOF;
}


int CRapiFS::x_fgetc(X_FILE *stream)
{
#if !defined(_WINDOWS)
	dec_assert(0);
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	DWORD dwBytesRead = 0;
	char chChar = 0;

	//
	// If we have a char in the ungetc buffer, use it...
	//
	if(pFile->bHaveUngetcBuff)
	{
		pFile->bHaveUngetcBuff = false;
		return pFile->nChar;
	}

	if(!(*m_pfnReadFile)(pFile->hFileHandle,
						(LPVOID)&chChar, 
						sizeof(char), 
						&dwBytesRead, 
						NULL))				// Unsupported
	{
		pFile->uiFlags |= FILE_FLAG_ERROR;
		return EOF;
	}

	//
	//  Are we at EOF?
	//
	if(0 == dwBytesRead)
	{
		pFile->uiFlags |= FILE_FLAG_EOF;
	}

	return (((int)chChar) & 0x00FF);
#endif
}


char *CRapiFS::x_fgets(char *string, int n, X_FILE *stream)
{
#if !defined(_WINDOWS)
	return NULL;
#else
	if(!m_bInitialized || !string || !stream)
	{
		return NULL;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	DWORD dwBytesRead = 0;
	char chChar = 0;
	int nIndex = 0;

	//
	// If we have anything in the ungetc buffer, use it up first
	//
	if(pFile->bHaveUngetcBuff)
	{
		*(string + nIndex++) = (pFile->nChar & 0x00FF);
		pFile->bHaveUngetcBuff = false;
	}

	//
	// This function is horribly inefficient right now. Ideally,
	// we should implement some sort of rudimentary buffering 
	// system here so we can avoid these one byte reads.
	//
	while(nIndex < (n - 1))
	{
		//
		// Read a byte
		//
		if(!(*m_pfnReadFile)(pFile->hFileHandle,
							(LPVOID)&chChar, 
							sizeof(char), 
							&dwBytesRead, 
							NULL))				// Unsupported
		{
			pFile->uiFlags |= FILE_FLAG_ERROR;
			return NULL;
		}

		//
		//  Are we at EOF?
		//
		if(0 == dwBytesRead)
		{
			pFile->uiFlags |= FILE_FLAG_EOF;
			break;
		}

		//
		// Save the character
		//
		*(string + nIndex++) = chChar;

		//
		// Did we get a newline?
		//
		if(ASC_CHR_LF == chChar)
		{
			break;
		}
	}

	*(string + nIndex) = ASC_CHR_NUL;
	return string;
#endif
}


X_FILE *CRapiFS::x_fopen(const char *filename, const char *mode)
{
#if !defined(_WINDOWS)
	return NULL;
#else
	USES_CONVERSION;

	if(!m_bInitialized || !filename || !mode)
	{
		return NULL;
	}

	DWORD dwDesiredAccess = 0;
	DWORD dwCreationDisposition = 0;
	const char *pszMode = mode;
	char *pszStrippedFilename = NULL;
	wchar_t *pwszTemp = NULL;
	int whileflag = 1;
	int nLenOfFilename = 0;

	//
	// Initialize our RAPI file system object
	//
	CRapiFile *pRapiFile = new CRapiFile();
	if(!pRapiFile)
	{
		dec_assert(0);
		goto bailOut;
	}

	//
	// Strip off the rapi:// namespace from the filename if it's present
	//
	pszStrippedFilename = (LPTSTR)(LPCTSTR)strstr(filename, NAMESPACE_RAPI);

	//
	// Did we find the rapi:// namespace?
	//
	if(pszStrippedFilename)
	{
		pszStrippedFilename += NAMESPACE_RAPI_LEN;
	}

	//
	// Nope, just use the filename directly...
	//
	else
	{
		pszStrippedFilename = const_cast<char *>(filename);
	}

	//
	// Save the filename for later. A2W creates a local (stack based)
	// copy of wszTemp, so we need to make a copy of it.
	//
	nLenOfFilename = strlen(pszStrippedFilename) + 1;
	pwszTemp = A2W(pszStrippedFilename);

	pRapiFile->pwszFilename = (wchar_t *)malloc(nLenOfFilename * sizeof(wchar_t));
	if(!pRapiFile->pwszFilename)
	{
		dec_assert(0);
		goto bailOut;
	}

	wcscpy(pRapiFile->pwszFilename, pwszTemp);
	
	//
	// Save the mode for later
	//
	strcpy(pRapiFile->szMode, mode);

	//
	// Translate the mode string into access and disposition flags
	//
	switch (*pszMode)
	{
		case 'r':
			dwDesiredAccess = GENERIC_READ;
			dwCreationDisposition = OPEN_EXISTING;
			break;
		case 'w':
			dwDesiredAccess = GENERIC_WRITE;
			dwCreationDisposition = CREATE_ALWAYS;
			break;
		case 'a':
			dwDesiredAccess = GENERIC_WRITE;
			dwCreationDisposition = OPEN_ALWAYS;
			pRapiFile->bAppendMode = true;
			break;
		default:
			errno = EINVAL;
			goto bailOut;
			break;
	}

	//
	// There can be up to three more optional mode characters:
	// (1) A single '+' character,
	// (2) One of 't' and 'b' and   (Text Mode is UNSUPPORTED)
	// (3) One of 'c' and 'n'.      (UNSUPPORTED)
	//
	while (*++pszMode && whileflag)
	{
		switch(*pszMode)
		{
			case '+':
				// 
				// If the first char of the mode string was 'r'
				//
				if (dwDesiredAccess & GENERIC_READ)
				{
					dwDesiredAccess |= GENERIC_WRITE;
				}

				// 
				// If the first char of the mode string was 'w'
				//
				else if (dwDesiredAccess & GENERIC_WRITE)
				{
					dwDesiredAccess |= GENERIC_READ;
				}
				break;

			case 'b':
				//
				// We assume binary mode, so don't do anything special here
				//
				whileflag = 0;
				break;

			case 't':
				//
				// Set text mode.
				//
				pRapiFile->bTextMode = true;
				whileflag = 0;
				break;

			default:
				whileflag = 0;
				break;
		}
	}

	if(INVALID_HANDLE_VALUE == (pRapiFile->hFileHandle = (*m_pfnCreateFile)(pRapiFile->pwszFilename,
																			dwDesiredAccess, 
																			FILE_SHARE_READ | FILE_SHARE_WRITE,
																			NULL,						 // Ignored 
																			dwCreationDisposition, 
																			FILE_ATTRIBUTE_NORMAL, 
																			NULL)))						 // Ignored
	{
		m_dwLastError = (*m_pfnGetLastError)();
		m_dwRapiError = (*m_pfnRapiGetError)();
		goto bailOut;
	}

	//
	// Save the CRapiFile in our X_FILE pointer...
	//
	return reinterpret_cast<X_FILE *>(pRapiFile);

bailOut:
	if(pRapiFile)
	{
		delete pRapiFile;
		pRapiFile = NULL;
	}

	return NULL;
#endif
}


int CRapiFS::x_fprintf(X_FILE *stream, const char *format, ...)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	#define MAX_TRIES 10
	#define MAX_BUFFER_SIZE 1024
	
	int		nLen = 0;
	int		nRetval = -1;

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
		nLen = ssvsnprintf(pszTemp, nBufferSize, format, vl);
		if (nLen >= 0)
		{
			// Successfully formatted the output buffer so write it out.
			nRetval = x_fwrite(pszTemp, sizeof(char), nLen, stream);
			break;
		}

		// Failed to format the output buffer so try again but with
		// a larger buffer.
		nNumberOfRetries++;
		if (nNumberOfRetries > MAX_TRIES)
			break;

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

	return nRetval;
#endif
}


int CRapiFS::x_fputc(int c, X_FILE *stream)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);;
	DWORD dwBytesWritten = 0;
	char chChar = (char)c;

	//
	// Adjust file pointer if we're in append mode
	//
	if(pFile->bAppendMode)
	{
		if(x_fseek(stream, 0, SEEK_END))
		{
			//
			// The seek failed...
			//
			return EOF;
		}
	}

	if(!(*m_pfnWriteFile)(pFile->hFileHandle,
						 (LPCVOID)&chChar, 
						 sizeof(char), 
						 &dwBytesWritten, 
						 NULL))			// Unsupported
	{
		pFile->uiFlags |= FILE_FLAG_ERROR;
		return EOF;
	}

	return c;
#endif
}


int CRapiFS::x_fputs(const char *szstring, X_FILE *stream)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream || !szstring)
	{
		return EOF;
	}

	if(!(*szstring))
	{
		return 0;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	DWORD dwBytesWritten = 0;

	//
	// Adjust file pointer if we're in append mode
	//
	if(pFile->bAppendMode)
	{
		if(x_fseek(stream, 0, SEEK_END))
		{
			//
			// The seek failed...
			//
			return EOF;
		}
	}

	if(!(*m_pfnWriteFile)(pFile->hFileHandle,
						 (LPCVOID)szstring, 
						 strlen(szstring), 
						 &dwBytesWritten, 
						 NULL))			// Unsupported
	{
		pFile->uiFlags |= FILE_FLAG_ERROR;
		return EOF;
	}

	return (dwBytesWritten);
#endif
}


size_t CRapiFS::x_fread(void *buffer, size_t size, size_t count, X_FILE *stream)
{
#if !defined(_WINDOWS)
	return 0;
#else
	if(!m_bInitialized || !stream)
	{
		return 0;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	DWORD dwBytesRead = 0;
	DWORD dwBytesToRead = (size * count);
	DWORD dwBytesLeftInBuffer = (pFile->nIndexInBuffer == EOF) ? 0 : pFile->dwSizeOfBuffer - pFile->nIndexInBuffer;

	BYTE *pbyTempBuffer = (BYTE *)buffer;

	//
	// If we have anything in the ungetc buffer, use it up first
	//
	if(pFile->bHaveUngetcBuff)
	{
		dwBytesToRead = (size * count) - 1;
		*pbyTempBuffer++ = (pFile->nChar & 0x00FF);
	}

	//
	// Case 1:  The read request can be satisfied
	// by what is left in the buffer
	//
	if(dwBytesToRead <= dwBytesLeftInBuffer)
	{
		//
		// Just copy out what was requested
		//
		memcpy(pbyTempBuffer, pFile->pbyBuffer + pFile->nIndexInBuffer, dwBytesToRead);
		pFile->nIndexInBuffer += dwBytesToRead;
	}

	//
	// Case 2: The read request cannot be satisfied
	// by what is left in the buffer
	//
	else if(dwBytesToRead > dwBytesLeftInBuffer)
	{
		//
		// Copy what is left in the buffer
		// then do a direct read to get the rest
		//
	}

	if(!(*m_pfnReadFile)(pFile->hFileHandle,
						pbyTempBuffer, 
						dwBytesToRead, 
						&dwBytesRead, 
						NULL))				// Unsupported
	{
		pFile->uiFlags |= FILE_FLAG_ERROR;
		return 0;
	}

	//
	//  Are we at EOF?
	//
	if(0 == dwBytesRead)
	{
		pFile->uiFlags |= FILE_FLAG_EOF;
	}

	return (size_t)dwBytesRead;
#endif
}


int CRapiFS::x_fseek(X_FILE *stream, long offset, int origin)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	DWORD dwMoveMethod = FILE_BEGIN;
	DWORD dwReturn = EOF;

	switch(origin)
	{
		case SEEK_SET:
			dwMoveMethod = FILE_BEGIN;
			break;

		case SEEK_CUR:
			dwMoveMethod = FILE_CURRENT;
			break;

		case SEEK_END:
			dwMoveMethod = FILE_END;
			break;

		default:
			dec_assert(0);
	}

	if(0xFFFFFFFF == (dwReturn = (*m_pfnSetFilePointer)(pFile->hFileHandle, 
														offset, 
														0,
														dwMoveMethod)))
	{
		//
		// Might not be an error.  
		//
		if(NO_ERROR == (*m_pfnGetLastError)())
		{
			dwReturn = 0;			
		}

		else
		{
			dwReturn = EOF;
		}
	}

	else
	{
		dwReturn = 0;
	}

	pFile->uiFlags &= ~FILE_FLAG_EOF;

	//
	// x_fseek also negates any ungetc we may have done
	//
	pFile->bHaveUngetcBuff = false;

	//
	// x_fseek also invalidates our read buffer
	//
	pFile->nIndexInBuffer = EOF;

	return (int)dwReturn;
#endif
}


long CRapiFS::x_ftell(X_FILE *stream)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	DWORD dwOffset = 0;
	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	dwOffset = (*m_pfnSetFilePointer)(pFile->hFileHandle,
									 0,
									 NULL,
									 FILE_CURRENT);
	
	//
	// If we have a byte in the ungetc buffer, then our result
	// is one less than the real on-disk position, because we're
	// 'faking' ungetc support.
	//
	if(pFile->bHaveUngetcBuff)
	{
		return dwOffset - 1;
	}

	else
	{
		return dwOffset;
	}
#endif
}


int CRapiFS::x_ftruncate(X_FILE *stream, size_t size)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);

	if(0 == x_fseek(stream, size, SEEK_SET))
	{
		return ((*m_pfnSetEndOfFile)(pFile->hFileHandle) ? 0 : EOF);
	}

	else
	{
		return EOF;
	}
#endif
}


size_t CRapiFS::x_fwrite(const void *buffer, size_t size, size_t count, X_FILE *stream)
{
#if !defined(_WINDOWS)
	return 0;
#else
	if(!m_bInitialized || !stream)
	{
		return 0;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	DWORD dwBytesWritten = 0;

	//
	// Adjust file pointer if we're in append mode
	//
	if(pFile->bAppendMode)
	{
		if(x_fseek(stream, 0, SEEK_END))
		{
			//
			// The seek failed...
			//
			return 0;
		}
	}

	if(!(*m_pfnWriteFile)(pFile->hFileHandle,
						 buffer, 
						 size * count, 
						 &dwBytesWritten, 
						 NULL))			// Unsupported
	{
		pFile->uiFlags |= FILE_FLAG_ERROR;
		return 0;
	}

	return (size_t)dwBytesWritten;
#endif
}


int CRapiFS::x_remove(const char *path)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	USES_CONVERSION;

	if(!m_bInitialized || !path)
	{
		return EOF;
	}

	return (((*m_pfnDeleteFile)(A2W(path))) ? 0 : EOF);
#endif
}


int CRapiFS::x_rename(const char *oldname, const char *newname)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	USES_CONVERSION;

	if(!m_bInitialized || !oldname || !newname)
	{
		return EOF;
	}

	return (((*m_pfnMoveFile)(A2W(oldname), A2W(newname))) ? 0 : EOF);
#endif
}


void CRapiFS::x_rewind(X_FILE *stream)
{
#if !defined(_WINDOWS)
	return;
#else
	if(!m_bInitialized || !stream)
	{
		return;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	if(0xFFFFFFFF == (*m_pfnSetFilePointer)(pFile->hFileHandle, 
											0, 
											0,
											FILE_BEGIN))
	{
		dec_assert(0);
	}

	pFile->uiFlags &= ~FILE_FLAG_EOF;

	//
	// x_rewind also negates any ungetc we may have done
	//
	pFile->bHaveUngetcBuff = false;

	//
	// x_rewind also invalidates our read buffer
	//
	pFile->nIndexInBuffer = EOF;
#endif
}


int CRapiFS::x_setvbuf(X_FILE *stream, char *buffer, int mode, size_t size)
{
	//
	// Not implemented. 
	//
	return EOF;
}


int CRapiFS::x_ungetc(int c, X_FILE *stream)
{
#if !defined(_WINDOWS)
	return EOF;
#else
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	//
	// Cannot push EOF onto the stream (ignored)
	//
	if(EOF == c)
	{
		return c;
	}
	
	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	pFile->nChar = c;
	pFile->bHaveUngetcBuff = true;
	pFile->uiFlags &= ~FILE_FLAG_EOF;
	return c;
#endif
}


bool CRapiFS::x_memattach(char *pszName, size_t ulNameBufSize, void *ptr, size_t ulSize)
{
	//
	// Not implemented. 
	//
	return false;
}


// Since the IDecFiles interface has these defined as pure virtual functions,
// we must implement something for each one.  This was done in the IDecFiles
// interface intentionally so that future derivations from IDecFiles always
// provide a common set of basic functions (even if they are simply stubbed).
// This forces the implementor to at least think about what they want to do
// with these APIs.
void CRapiFS::SetNetService(INetData *pnet)
{
	//
	// Not implemented. 
	//
	dec_assert(0);
}

unsigned long int CRapiFS::GetFileSystemSize()
{
	//
	// Not implemented. 
	//
	dec_assert(0);
	return 0;
}

void CRapiFS::SetFileSystemSize(unsigned long int ulSize)
{
	//
	// Not implemented. 
	//
	dec_assert(0);
}

unsigned long int CRapiFS::GetFileSizeThreshold()
{
	//
	// Not implemented. 
	//
	dec_assert(0);
	return 0;
}

void CRapiFS::SetFileSizeThreshold(unsigned long int ulSize)
{
	//
	// Not implemented. 
	//
	dec_assert(0);
}
   
const char *CRapiFS::GetTempFilePath()
{
#if !defined(_WINDOWS)
	return NULL;
#else
	return m_pszTempPath;
#endif
}

void CRapiFS::SetTempPath(const char *path)
{
#if !defined(_WINDOWS)
	return;
#else
	size_t len;

	if (m_pszTempPath)
	{
		free(m_pszTempPath);
	}

	if (!path)
		len = 0;
	else
		len = strlen(path);

	if (len == 0)
	{
		// No path has been supplied so use some default platform-specific path.
		m_pszTempPath = (char *)malloc(MAX_PATH);
		if (m_pszTempPath)
		{
			memset(m_pszTempPath, 0, MAX_PATH);
			GetTempPath(MAX_PATH, m_pszTempPath);
		}
	}
	else
	{
		len = strlen(path) + 2;
		m_pszTempPath = (char *)malloc(len);
		if (m_pszTempPath)
		{
			memset(m_pszTempPath, 0, len);
			strcpy(m_pszTempPath, path);
		}
	}
#endif
}

const wchar_t *CRapiFS::GetTempFilePathW()
{
	//
	// Not implemented. 
	//
	return NULL;
}

void CRapiFS::SetTempPathW(const wchar_t *path)
{
	//
	// Not implemented. 
	//
	return;
}

bool CRapiFS::GetTempFile(char *pszFilename, int iSize)
{
	//
	// Not implemented. 
	//
	return false;
}

bool CRapiFS::GetTempFileW(wchar_t *pszFilename, int iSize)
{
	//
	// Not implemented. 
	//
	return false;
}

bool CRapiFS::SwitchFileToDisk(const char *filename, char *pszondiskname, int iBufferSize)
{
	//
	// Not implemented. 
	//
	return false;
}

bool CRapiFS::SwitchFileToDiskW(const char *filename, wchar_t *pszondiskname, int iBufferSize)
{
	//
	// Not implemented. 
	//
	return false;
}

bool CRapiFS::AttachMemoryBlock(char *pszName, size_t ulNameBufSize, void *ptr, size_t sizeOfPtr)
{
	//
	// Not implemented. 
	//
	return false;
}

void CRapiFS::SetSingleThreadMode(bool bMode)
{
	m_bMultiThread = !bMode;
}


#if defined(_WINDOWS)
int CRapiFS::x_fstat(X_FILE *stream, struct _stat *buffer)
{
	if(!m_bInitialized || !stream || !buffer)
	{
		return EOF;
	}

	DWORD dwAttribs = 0;
	FILETIME filetimeCreationTime;
	FILETIME filetimeLastAccessTime;
	FILETIME filetimeLastWriteTime;
	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);

	//
	// Initialize buffer and set any non-zero defaults. These values 
	// never change on Windows systems and come from the documentation
	// in MSDN.
	//
	memset(buffer, 0, sizeof(struct _stat));
	buffer->st_nlink = 1;
	
	//
	// Get the file times...
	//
	if(!(*m_pfnGetFileTime)(pFile->hFileHandle,
							&filetimeCreationTime, 
							&filetimeLastAccessTime, 
							&filetimeLastWriteTime))
	{
		return EOF;
	}

	//
	// Convert FILETIME to time_t
	//
	MyFileTimeToUnixTime(&filetimeLastAccessTime, &buffer->st_atime);
	MyFileTimeToUnixTime(&filetimeLastWriteTime, &buffer->st_mtime); 
	MyFileTimeToUnixTime(&filetimeCreationTime, &buffer->st_ctime); 

	//
	// Get the file size
	//
	if(0xFFFFFFFF == (buffer->st_size = (*m_pfnGetFileSize)(pFile->hFileHandle, NULL)))
	{
		return EOF;
	}

	//
	// Get the file attributes
	//
	if(0xFFFFFFFF == (dwAttribs = (*m_pfnGetFileAttribs)(pFile->pwszFilename)))
	{
		return EOF;
	}

	//
	// Map Win32 attribs to UNIX style attribs
	//
	if(dwAttribs & FILE_ATTRIBUTE_DIRECTORY)
	{
		buffer->st_mode |= _S_IFDIR;
	}

	if(dwAttribs & FILE_ATTRIBUTE_READONLY)
	{
		buffer->st_mode |= !(_S_IWRITE);
	}

	if(dwAttribs & FILE_ATTRIBUTE_NORMAL)
	{
		buffer->st_mode |= _S_IFREG;
	}
	
	return 0;
}

int CRapiFS::x_stat(const char *path, struct _stat *buffer)
{
	USES_CONVERSION;

	if(!m_bInitialized || !path || !buffer)
	{
		return EOF;
	}

	int nReturn = EOF;
	DWORD dwAttribs = 0;

	//
	// Strip off the rapi:// namespace from the filename if it's present
	//
	char *pszStrippedFilename = (LPTSTR)(LPCTSTR)strstr(path, NAMESPACE_RAPI);

	//
	// Did we find the rapi:// namespace?
	//
	if(pszStrippedFilename)
	{
		pszStrippedFilename += NAMESPACE_RAPI_LEN;
	}

	//
	// Nope, just use the filename directly...
	//
	else
	{
		pszStrippedFilename = const_cast<char *>(path);
	}

	//
	// We need an open file handle in order to get the stat info
	//
	X_FILE *pFile = x_fopen(pszStrippedFilename, "rb");
	if(!pFile)
	{
		//
		// The fopen will fail if 'path' is a directory.
		// Try just getting the attributes if possible...
		//
		goto tryDirectory;
	}

	nReturn = x_fstat(pFile, buffer);

	//
	// Make sure to close the file we just opened.
	//
	if(EOF == x_fclose(pFile))
	{
		nReturn = EOF;
	}

	return nReturn;

tryDirectory:
	memset(buffer, 0, sizeof(struct _stat));
	buffer->st_nlink = 1;

	//
	// Get the file attributes
	//
	if(0xFFFFFFFF == (dwAttribs = (*m_pfnGetFileAttribs)(A2W(pszStrippedFilename))))
	{
		return nReturn;
	}

	//
	// Map Win32 attribs to UNIX style attribs
	//
	if(dwAttribs & FILE_ATTRIBUTE_DIRECTORY)
	{
		buffer->st_mode |= _S_IFDIR;
	}

	if(dwAttribs & FILE_ATTRIBUTE_READONLY)
	{
		buffer->st_mode |= !(_S_IWRITE);
	}

	if(dwAttribs & FILE_ATTRIBUTE_NORMAL)
	{
		buffer->st_mode |= _S_IFREG;
	}

	return 0;
}

int CRapiFS::x_futime(X_FILE *stream, struct _utimbuf *filetime)
{
	// 'filetime' is allowed to be NULL!
	if(!m_bInitialized || !stream)
	{
		return EOF;
	}

	FILETIME filetimeLastAccessTime;
	FILETIME filetimeLastWriteTime;
	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	
	//
	// If filetime is NULL, then that means use the current local
	// time...
	//
	if(!filetime)
	{
		// Otherwise, use the current local time
		struct _utimbuf timeBuffTemp = { 0 };

		time(&(timeBuffTemp.actime));
		timeBuffTemp.modtime = timeBuffTemp.actime;
	
		//
		// Convert time_t to FILETIME
		//
		MyUnixTimeToFileTime(timeBuffTemp.actime, &filetimeLastAccessTime); 
		MyUnixTimeToFileTime(timeBuffTemp.modtime, &filetimeLastWriteTime); 
	}
	
	//
	// Otherwise, use the supplied filetime pointer
	//
	else
	{
		//
		// Convert time_t to FILETIME
		//
		MyUnixTimeToFileTime(filetime->actime, &filetimeLastAccessTime); 
		MyUnixTimeToFileTime(filetime->modtime, &filetimeLastWriteTime); 
	}

	//
	// Set the file time
	//
	if(!(*m_pfnSetFileTime)(pFile->hFileHandle,
							NULL,
							&filetimeLastAccessTime,
							&filetimeLastWriteTime))
	{
		return EOF;
	}

	return 0;	// Success
}

int CRapiFS::x_utime(const char *filename, struct _utimbuf *times)
{
	// 'times' is allowed to be NULL!
	if(!m_bInitialized || !filename)
	{
		return EOF;
	}

	int nReturn = EOF;

	//
	// We need an open file handle in order to set the file times
	//
	X_FILE *pFile = x_fopen(filename, "r+b");
	if(!pFile)
	{
		return nReturn;
	}

	nReturn = x_futime(pFile, times);

	//
	// Make sure to close the file we just opened.
	//
	if(EOF == x_fclose(pFile))
	{
		nReturn = EOF;
	}

	return nReturn;
}

#else // if defined(_WINDOWS)

int CRapiFS::x_fstat(X_FILE *stream, struct stat *buffer)
{
	return EOF;
}

int CRapiFS::x_stat(const char *path, struct stat *buffer)
{
	return EOF;
}

int CRapiFS::x_futime(X_FILE *stream, struct utimbuf *filetime)
{
	return EOF;
}

int CRapiFS::x_utime(const char *filename, struct utimbuf *times)
{
	return EOF;
}
#endif // if defined(_WINDOWS)


///////////////////////////////////////////////////////////////////
// Helper routines
//

bool CRapiFS::FillInputBuffer(X_FILE *stream)
{
#if !defined(_WINDOWS)
	return false;
#else
	if(!stream)
	{
		return false;
	}

	CRapiFile *pFile = reinterpret_cast<CRapiFile *>(stream);
	
	//
	// If we have no buffer yet, then allocate one
	//
	if(!pFile->pbyBuffer)
	{
		pFile->pbyBuffer = (BYTE *)malloc(RAPI_BUFFER_SIZE);
		dec_assert(pFile->pbyBuffer);
	}

	//
	// We should only be in here if we have exhausted the input
	// or if we've invalidated the buffer (by seeking for example).
	//
	dec_assert(EOF == pFile->nIndexInBuffer);

	//
	// Fill the buffer
	//
	if(!(*m_pfnReadFile)(pFile->hFileHandle,
						pFile->pbyBuffer, 
						RAPI_BUFFER_SIZE, 
						&pFile->dwSizeOfBuffer, 
						NULL))				// Unsupported
	{
		pFile->uiFlags |= FILE_FLAG_ERROR;
		return 0;
	}

	//
	//  Are we at EOF?
	//
	if(0 == pFile->dwSizeOfBuffer)
	{
		pFile->uiFlags |= FILE_FLAG_EOF;
	}

	return true;
#endif
}


