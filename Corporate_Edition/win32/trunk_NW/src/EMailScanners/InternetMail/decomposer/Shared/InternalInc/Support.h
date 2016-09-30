// Support.h
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(SUPPORT_H)
#define SUPPORT_H

#include <stdio.h>
#include <time.h>
#include "dec_assert.h"

#if defined (SYM_NLM)
#include <nwsemaph.h>
#include <nwthread.h>
#endif

////////////////////////////////////////////////////////////////////////
// Helper macro used for handling special namespaces
//

const char gszDriveDelim[] = "\x3a\x5c";                        // External string - use ascii values
const size_t sizeDriveDelim = sizeof(gszDriveDelim);

const char gszNamespaceDelim[] = "://";                         // Internal string - ok to use character constants
const size_t sizeNamespaceDelim = sizeof(gszNamespaceDelim);

//
// This macro searches first for gszNamespaceDelim and if found, will
// set pszStrippedPath to point to pszFullPath minus the namespace
// delimiter.
//
// If a namespace isn't found, then it looks for a drive delimiter
// and if found sets pszStrippedPath to point to pszFullPath minus
// the drive delimiter.
//
// If neither is found, pszStrippedPath is the same as pszFullPath.
//
// TODO: Make sure this will work on the Netware platform.
//
#define STRIP_NAMESPACE(pszFullPath, pszStrippedPath)				\
	pszStrippedPath = strstr(pszFullPath, gszNamespaceDelim);		\
	if (pszStrippedPath)											\
	{																\
		pszStrippedPath += (sizeNamespaceDelim - 1);				\
	}																\
	else															\
	{																\
		pszStrippedPath = strstr(pszFullPath, gszDriveDelim);		\
		if (pszStrippedPath)										\
		{															\
			pszStrippedPath += (sizeDriveDelim - 1);				\
		}															\
		else														\
		{															\
			pszStrippedPath = pszFullPath;							\
		}															\
	}

#define HAS_SPECIAL_NAMESPACE(pszPath) (strstr(pszPath, gszNamespaceDelim) != NULL)

#if !defined(LOAD_WORD) || !defined(LOAD_DWORD) || !defined(SET_WORD) || !defined(SET_DWORD)

	#ifdef	CPU_BIG_ENDIAN

		#define LOAD_WORD(pw)		 Support_GetWord((pw))
		#define LOAD_DWORD(pdw)		 Support_GetDword((pdw))
		#define SET_WORD(pw, val)	 Support_SetWord((pw), (val))
		#define SET_DWORD(pdw,val)	 Support_SetDword((pdw), (val))

	#else	// if LITTLE_ENDIAN (default)
   
		#define LOAD_WORD(pw)		 (*(pw))
		#define LOAD_DWORD(pdw)		 (*(pdw))
		#define SET_WORD(pw, val)	 (*(pw) = (val))
		#define SET_DWORD(pdw,val)	 (*(pdw) = (val))

	#endif

#endif // #if !defined(LOAD_WORD) || !defined(LOAD_DWORD)


#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
// Other support functions

bool	Support_CopyFile(const char *pszExistingFile, const char *pszNewFile, bool bFailIfExists);
bool	Support_SetEndOfFile(const char *pszOriginalFilename, FILE *pOriginalFile, const char *pszMode);
WORD	Support_GetWord (LPWORD pWord);
void	Support_SetWord (LPWORD pWord, WORD wValue);
DWORD	Support_GetDword (LPDWORD pDword);
void	Support_SetDword (LPDWORD pDword, DWORD dwValue);
DWORD	Support_FileSize (FILE *stream);
bool	Support_GetNextToken(char	*lpszString,
							char	*lpszSeparators,
							LPINT	lpnIndex,
							LPINT	lpnLength,
							char	*lpszToken,
							int	nMaxLen);
DWORD	Support_GetTempPath(DWORD nBufferLength, char *lpBuffer);


//
// Time related functions
//
void	Support_UnixTimeToFileTime(time_t *unix_time, LPFILETIME file_time);
bool	Support_DosDateTimeToFileTime(WORD wFatDate, WORD wFatTime, LPFILETIME lpFileTime);
void	Support_FileTimeToUnixTime(const FILETIME *file_time, time_t *unix_time);
bool	Support_FileTimeToDosDateTime(const FILETIME *lpFileTime, LPWORD lpFatDate, LPWORD lpFatTime);
bool	Support_FileTimeToLocalFileTime(const FILETIME *filetime, LPFILETIME localfiletime);

//
// Case insensitive memory comparision
//
int		Support_memicmp(const void *buf1, const void *buf2, unsigned int count);


//
// String manipulation functions
//
char *  Support_strlwr(char *string);
char *  Support_strupr(char *string);
char *  Support_CharNext(const char *lpsz);
char *  Support_CharPrev(const char *lpszStart, const char *lpszCurrent);
char *  ascii_fgets(char *s, int n, FILE *stream);

//
// Process/Thread functions
//
DWORD	Support_GetCurrentProcessId();


/////////////////////////////////////////////////////////////////////////////
//
//                    class CCriticalSection
//
/////////////////////////////////////////////////////////////////////////////

class CCriticalSection
{
public:
	CCriticalSection()
	{
	#if defined (_WINDOWS)
		InitializeCriticalSection(&m_CriticalSection);
	#elif defined (SYM_NLM)
		m_CriticalSection = OpenLocalSemaphore(1);
	#else
		int rc = pthread_mutex_init(&m_CriticalSection, NULL);
		dec_assert(rc == 0);
	#endif
	}

	~CCriticalSection()
	{
	#if defined (_WINDOWS)
		DeleteCriticalSection(&m_CriticalSection);
	#elif defined (SYM_NLM)
		int rc = CloseLocalSemaphore(m_CriticalSection);
		dec_assert(rc == 0);
	#else
		int rc = pthread_mutex_destroy(&m_CriticalSection);
		dec_assert(rc == 0);
	#endif
	}

	inline void In()
	{
		#if defined (_WINDOWS)
			EnterCriticalSection(&m_CriticalSection);
		#elif defined (SYM_NLM)
			int rc = WaitOnLocalSemaphore(m_CriticalSection);
			dec_assert(0 == rc);
		#else
			int rc = pthread_mutex_lock(&m_CriticalSection);
			dec_assert(0 == rc);
		#endif	
	}

	inline void Out()
	{
		#if defined (_WINDOWS)
			LeaveCriticalSection(&m_CriticalSection);
		#elif defined (SYM_NLM)
			int rc = SignalLocalSemaphore(m_CriticalSection);
			dec_assert(0 == rc);
		#else
			int rc = pthread_mutex_unlock(&m_CriticalSection);
			dec_assert(0 == rc);
		#endif
	}

private:
#if defined(_WINDOWS)
	CRITICAL_SECTION m_CriticalSection;
#elif defined (SYM_NLM)
	long m_CriticalSection;
#else
	pthread_mutex_t m_CriticalSection;
#endif
};


#ifdef __cplusplus
}
#endif

#endif	// SUPPORT_H

