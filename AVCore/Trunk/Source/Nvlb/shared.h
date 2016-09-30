#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifdef __cplusplus
extern "C" {
#endif

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


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL CreateDir()
//
//	Description:
//		Directory path
//
//	Parameters:
//		lpszDir				Pointer Directory name
//
//	Returns:
//		TRUE
//		FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL CreateDir
(
	LPCTSTR lpszDir
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL GetCmdLineOption()
//
//	Description:
//		Retrieves command line options. If tag is found the function returns 
//      TRUE. Otherwise it returns FALSE
//
//	Parameters:
//		lpszTag				Pointer to option tag name
//		lpszValue			Pointer to return value
//
//	Returns:
//		TRUE
//		FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetCmdLineOption
( 
	LPCTSTR		lpszTag,
	LPTSTR		lpszValue,
	int			argc,
	LPCTSTR		argv[] 
);



///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		DWORD StrToHex()
//
//	Description:
//		Converts a string to hex
//
//	Parameters:
//		lpszValue			Pointer to string
//
//	Returns:
//		Returns hex value of input string.
//
///////////////////////////////////////////////////////////////////////////////
DWORD StrToHex( LPCTSTR lpszValue );

#ifdef __cplusplus
}
#endif