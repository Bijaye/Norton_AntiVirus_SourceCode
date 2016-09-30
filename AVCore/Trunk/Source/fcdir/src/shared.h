#ifndef __SHARED_H
#define __SHARED_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// WIN 32 declarations
#if defined (SYM_WIN32)
	#include <windows.h>

// UNIX declarations
#elif defined (SYM_UNIX)
    #include <ctype.h>
    #define _MAX_PATH   2048

// NLM declarations
#elif defined (SYM_NLM)

#endif


// DOS/Win32 declarations
#if defined (SYM_WIN32) || defined (SYM_DOS)
	#define SLASH	'\\'

// UNIX declarations
#elif defined (SYM_UNIX)
	#define SLASH	'/'

// NLM declarations                                      
#elif defined (SYM_NLM)
	#define SLASH	'\\'

#endif


///////////////////////////////////////////////////////////////////////////////
//
//	Description:
//		Gets parses command line options.
//
//		Name:	
//				GetCmdLineOption()
//
//		Parameters:
//				LPTSTR		lpszTag				
//				LPTSTR		lpszValue
//				int			argc
//				LPTSTR		argv[]
//
//		Returns:
//				TRUE
//				FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetCmdLineOption
( 
	LPTSTR		lpszTag,
	LPTSTR		lpszValue,
	int			argc,
	LPTSTR		argv[] 
);


///////////////////////////////////////////////////////////////////////////////
//
//	Description:
//		Gets parses command line options.
//
//		Name:	
//				StrToHex()
//
//		Parameters:
//				LPTSTR		lpszValue
//
//		Returns:
//				DWORD value
//
///////////////////////////////////////////////////////////////////////////////
DWORD StrToHex
(
	LPTSTR lpszValue
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ExtractFileExt()
//
//	Description:
//		Extracts the name part of a file name.
//			example:	passing in [test.file] returns [test]
//
//
//	Parameters:
//		lpszFileName		Pointer full file name including the extension
//		lpszFileExt			Pointer to extension
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void ExtractFileExt
(
	LPSTR lpszFileName,
	LPSTR lpszFileExt
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ExtractFileNameOnly()
//
//	Description:
//		Extracts the name part of a file name.
//			example:	passing in [test.file] returns [test]
//
//
//	Parameters:
//		lpszFileName		Pointer full file name including the extension
//		lpszName			Pointer to name
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void ExtractFileNameOnly
(
	LPCTSTR lpszFileName,
	LPTSTR	lpszName
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ExtractFileName()
//
//	Description:
//		Extracts the name part of a file name.
//			example:	passing in [c:\some\dir\test.file] returns [test.fil]
//
//
//	Parameters:
//		lpszFilePath		Pointer to full file path name
//		lpszFileName		Pointer to file name
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void ExtractFileName
(
	LPCTSTR lpszFilePath,
	LPTSTR	lpszFileName
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ExtractPathName()
//
//	Description:
//		Extracts the name part of a file name.
//			example:	passing in [c:\some\dir\test.file] returns [c:\some\dir]
//
//
//	Parameters:
//		lpszFilePath		Pointer to full file path name
//		lpszPathName		Pointer to path name
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void ExtractPathName
(
	LPCTSTR lpszFilePath,
	LPTSTR	lpszPathName
);


#if defined (SYM_UNIX)
LPTSTR strlwr 
(
    LPTSTR lpszString
);
#endif

#if defined (SYM_UNIX)
LPTSTR strupr 
(
    LPTSTR lpszString
);
#endif


#endif // __SHARED_H
