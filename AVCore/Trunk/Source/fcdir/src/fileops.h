#ifndef __FILEOPS_H
#define __FILEOPS_H

#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// DOS defines
//-----------------------------------------------------------------------------
#if defined (SYM_DOS)
	#include <dos.h>
	#include <errno.h>

	#define HANDLE	struct _find_t *

	// Attribute defines
	#define FILE_ATTRIBUTE_ARCHIVE			_A_ARCH	
//	#define FILE_ATTRIBUTE_COMPRESSED
	#define FILE_ATTRIBUTE_DIRECTORY		_A_SUBDIR
//	#define FILE_ATTRIBUTE_ENCRYPTED		
	#define FILE_ATTRIBUTE_HIDDEN			_A_HIDDEN
	#define FILE_ATTRIBUTE_NORMAL			_A_NORMAL
//	#define FILE_ATTRIBUTE_OFFLINE
	#define FILE_ATTRIBUTE_READONLY			_A_RDONLY
//	#define FILE_ATTRIBUTE_REPARSE_POINT
//	#define FILE_ATTRIBUTE_SPARSE_FILE
	#define FILE_ATTRIBUTE_SYSTEM			_A_SYSTEM
//	#define FILE_ATTRIBUTE_TEMPORARY

//-----------------------------------------------------------------------------
// WIN32 defines
//-----------------------------------------------------------------------------
#elif defined (SYM_WIN32)

	#include <conio.h>
	#include <string.h>
	#include <windows.h>

//-----------------------------------------------------------------------------
// UNIX defines
//-----------------------------------------------------------------------------
#elif defined (SYM_UNIX)
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
	#include <limits.h>

    #define _MAX_PATH 2048
	#define HANDLE	DIR*


	// Attribute defines
//	#define FILE_ATTRIBUTE_ARCHIVE						
//	#define FILE_ATTRIBUTE_COMPRESSED		
	#define FILE_ATTRIBUTE_DIRECTORY		S_IFDIR
//	#define FILE_ATTRIBUTE_ENCRYPTED
//	#define FILE_ATTRIBUTE_HIDDEN			
	#define FILE_ATTRIBUTE_NORMAL			S_IFREG
//	#define FILE_ATTRIBUTE_OFFLINE
	#define FILE_ATTRIBUTE_READONLY			S_IREAD
//	#define FILE_ATTRIBUTE_REPARSE_POINT
//	#define FILE_ATTRIBUTE_SPARSE_FILE
//	#define FILE_ATTRIBUTE_SYSTEM			
//	#define FILE_ATTRIBUTE_TEMPORARY


//-----------------------------------------------------------------------------
// NLM defines
//-----------------------------------------------------------------------------
#elif defined (SYM_NLM)
	#include <string.h>
	#include <process.h>
	#include <direct.h>
	#include <conio.h>
	#include <nwdir.h>
	#include <nwfile.h>

	// Handle definition
	#define HANDLE	DIR *

	// Attribute defines
	#define FILE_ATTRIBUTE_ARCHIVE			_A_ARCH			
//	#define FILE_ATTRIBUTE_COMPRESSED		
	#define FILE_ATTRIBUTE_DIRECTORY		_A_SUBDIR
//	#define FILE_ATTRIBUTE_ENCRYPTED
	#define FILE_ATTRIBUTE_HIDDEN			_A_HIDDEN
	#define FILE_ATTRIBUTE_NORMAL			_A_NORMAL
//	#define FILE_ATTRIBUTE_OFFLINE
	#define FILE_ATTRIBUTE_READONLY			_A_RDONLY
//	#define FILE_ATTRIBUTE_REPARSE_POINT
//	#define FILE_ATTRIBUTE_SPARSE_FILE
	#define FILE_ATTRIBUTE_SYSTEM			_A_SYSTEM
//	#define FILE_ATTRIBUTE_TEMPORARY
#endif


// Invalid handle return
#define INVALID_HANDLE	((HANDLE)(-1))


//-----------------------------------------------------------------------------
// WIN32 defines
//-----------------------------------------------------------------------------
#if defined (SYM_WIN32)
	typedef WIN32_FIND_DATA		FIND_FILE_DATA;
	typedef WIN32_FIND_DATA	*   LPFIND_FILE_DATA;

#else
//-----------------------------------------------------------------------------
// DOS/UNIX/NLM defines
//-----------------------------------------------------------------------------

// File time structure
typedef struct _FILETIME 
{
    DWORD dwLowDateTime;	// Specifies the low-order 32 bits of the file time. 
    DWORD dwHighDateTime;	// Specifies the high-order 32 bits of the file time. 
}FILETIME; 

// Find file data structure
typedef struct 
{

    DWORD		dwFileAttributes; 
    FILETIME 	ftCreationTime; 
    FILETIME 	ftLastAccessTime; 
    FILETIME 	ftLastWriteTime; 
    DWORD    	nFileSizeHigh; 
    DWORD    	nFileSizeLow; 
    DWORD    	dwReserved0; 
    DWORD    	dwReserved1; 
    char    	cFileName[_MAX_PATH ]; 
    char    	cAlternateFileName[ 14 ]; 

#if defined (SYM_UNIX)  
	TCHAR	 	cPath[_MAX_PATH ];
#endif

}FIND_FILE_DATA, *LPFIND_FILE_DATA;

#endif	// Find file data structure



//-----------------------------------------------------------------------------
// Function:
//  HANDLE __FindFirstFile()
//
// Parameters:
//  lpFileName			pointer to name of file to search for
//	lpFindFileData		pointer to returned information
//
// Description:
//  The __FindFirstFile function searches a directory for a file whose name 
//  matches the specified filename. FindFirstFile examines subdirectory names 
//  as well as filenames. 
//
// Returns:
//	If the function succeeds, the return value is a search handle used in a 
//  subsequent call to __FindNextFile or __FindClose
//
//-----------------------------------------------------------------------------
HANDLE __FindFirstFile
(
	char*				pszFileName,		// pointer to name of file to search for 
	LPFIND_FILE_DATA	lpFindFileData	// pointer to returned information
);


//-----------------------------------------------------------------------------
// Function:
//  BOOL __FindNextFile()
//
// Parameters:
//  hFindFile			handle to search
//	lpFindFileData		pointer to returned information
//
// Description:
//  The __FindNextFile function continues a file search from a previous call to 
//  the __FindFirstFile function.
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//-----------------------------------------------------------------------------
BOOL __FindNextFile
(
	HANDLE				hFindFile,		// handle to search
	LPFIND_FILE_DATA	lpFindFileData	// pointer to returned information
);


//-----------------------------------------------------------------------------
// Function:
//  BOOL __FindClose()
//
// Parameters:
//  hFindFile		file search handle
//
// Description:
//  The __FindClose function closes the specified search handle. The 
//  __FindFirstFile and FindNextFile functions use the search handle to locate
//  files with names that match a given name. 
//
// Returns:
//  TRUE        On success
//  FALSE       On error
//-----------------------------------------------------------------------------
BOOL __FindClose
(
	HANDLE	hFindFile	// file search handle
);


#endif // __FILEOPS_H