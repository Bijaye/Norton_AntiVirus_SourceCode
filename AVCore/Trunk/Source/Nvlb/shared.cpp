
#include "shared.h"


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
)
{
	char	szExt[_MAX_PATH]={0};
	LPCTSTR	lpszExt;

	// Make a local copy of string
	strcpy(szExt, lpszFileName);

	// Find last occurance of "."
	if ((lpszExt = strrchr (szExt, '.')) == NULL)
	{
		strcpy(lpszFileExt, "\0");
	}
	else
	{
		*lpszExt++;
	    strcpy(lpszFileExt, lpszExt);
	}

}



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
	LPTSTR lpszName
)
{
	char	szTmpName[_MAX_PATH]={0};
	LPTSTR	lpszTmpName;

	// Make a local copy of string
	strcpy(szTmpName, lpszFileName );

	// Find last occurance of "."
	if ((lpszTmpName = strrchr (szTmpName, '.')) == NULL)
	{
		strcpy(lpszName, szTmpName);
	}
	else
	{
		*lpszTmpName = '\0';
	    strcpy(lpszName, szTmpName);
	}

}


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
	LPTSTR lpszFileName
)
{
	char	szTmpFileName[_MAX_PATH]={0};
	LPTSTR	lpszTmpFileName;

	// Make a local copy of string
	strcpy(szTmpFileName, lpszFilePath );

	// Find last occurance of "."
	if ((lpszTmpFileName = strrchr (szTmpFileName, '\\')) == NULL)
	{
		strcpy(lpszFileName, "\0");
	}
	else
	{
		*lpszTmpFileName++;
	    strcpy(lpszFileName, lpszTmpFileName);
	}
}



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
	LPTSTR lpszPathName
)
{

	char	szTmpPathName[_MAX_PATH]={0};
	LPSTR	lpszTmpPathName;

	// Make a local copy of string
	strcpy(szTmpPathName, lpszFilePath );

	// Find last occurance of "."
	if ((lpszTmpPathName = strrchr (szTmpPathName, '\\')) == NULL)
	{
		strcpy(lpszPathName, "\0");
	}
	else
	{
		*lpszTmpPathName = NULL;
	    strcpy(lpszPathName, szTmpPathName);
	}

}



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
)
{
	char	szOrigCurrDir[_MAX_PATH]={0};
	char	szPath[_MAX_PATH]={0};
	char*	lpszPath;
	char	szTempPath[_MAX_PATH]={0};
	char*	lpszTempPath;
	int		iPathLen = 0;


	lpszPath = (LPSTR)&szPath;
	lpszTempPath = (LPSTR)&szTempPath;


	// Add an ending back slash
	strcpy (szPath, lpszDir);
	iPathLen = strlen( szPath);
	if( szPath[iPathLen-1] != '\\')
	{
		strcat( szPath, "\\");
	}

	// Save current directory
	GetCurrentDirectory (sizeof (szOrigCurrDir), szOrigCurrDir);

	// Check if directory exists
	while (*lpszPath)
	{
 		*lpszTempPath = *lpszPath;
	
		if (*lpszPath == '\\')
		{
			if (!SetCurrentDirectory (szTempPath))
			{
				if( !CreateDirectory (szTempPath, NULL))
				{
					return (FALSE);
				}
			}
		}

		*lpszPath++;
		*lpszTempPath++;
	}

	// Restore original current directory
	SetCurrentDirectory(szOrigCurrDir);
	return (TRUE);
}


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
)
{

	int		iCmdOpts;
	TCHAR	szArg[255];
	LPTSTR	lpszBuffer;
	TCHAR	szTag[255];

	// Clear result
	if( lpszValue != NULL )
		lpszValue[0] = NULL;

	// Make a copy and upper case string
	strcpy( szTag, lpszTag );
	strupr( szTag );

	// Iterate between options

	for( iCmdOpts = 1; iCmdOpts < argc; iCmdOpts++ )
	{

		// Make local copy of argv[n]
		strcpy( szArg, argv[iCmdOpts] );

		// Upper case string
		strupr( szArg );

	
		// Find tag and return value
		if( ( lpszBuffer = strstr( szArg, szTag ) ) != NULL )
		{

			if( lpszValue != NULL )
			{
				strcpy( lpszValue, &argv[iCmdOpts][strlen( szTag )]);
			}
			
			return( TRUE );	
		}
		
	}
	
	return( FALSE );
}


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
DWORD StrToHex(	LPCTSTR lpszValue )
{

	DWORD			dwResult = 0;
	TCHAR			szBuffer[12];
	unsigned int	iOffset;


	// Make copy of value and upper case
	strcpy( szBuffer, lpszValue );
	strupr( szBuffer );


	// Check for 0x prefix
	if( (szBuffer[0] == '0') && ( szBuffer[1] == 'X') )
	{
		iOffset = 2;	
	}else
	{
		iOffset = 0;
	}
	

	// iterate through array
	for( iOffset; iOffset<strlen(szBuffer); iOffset++ )
	{
		switch(szBuffer[iOffset])
		{

			case '0':
				(dwResult <<= 4);
				dwResult += 0x0;
				break;

			case '1':
				(dwResult <<= 4);
				dwResult += 0x1;
				break;

			case '2':
				(dwResult <<= 4);
				dwResult += 0x2;
				break;

			case '3':
				(dwResult <<= 4);
				dwResult += 0x3;
				break;

			case '4':
				(dwResult <<= 4);
				dwResult += 0x4;
				break;

			case '5':
				(dwResult <<= 4);
				dwResult += 0x5;
				break;

			case '6':
				(dwResult <<= 4);
				dwResult += 0x6;
				break;

			case '7':
				(dwResult <<= 4);
				dwResult += 0x7;
				break;

			case '8':
				(dwResult <<= 4);
				dwResult += 0x8;
				break;

			case '9':
				(dwResult <<= 4);
				dwResult += 0x9;
				break;

			case 'A':
				(dwResult <<= 4);
				dwResult += 0xA;
				break;

			case 'B':
				(dwResult <<= 4);
				dwResult += 0xB;
				break;

			case 'C':
				(dwResult <<= 4);
				dwResult += 0xC;
				break;

			case 'D':
				(dwResult <<= 4);
				dwResult += 0xD;
				break;

			case 'E':
				(dwResult <<= 4);
				dwResult += 0xE;
				break;

			case 'F':
				(dwResult <<= 4);
				dwResult += 0xF;
				break;

		
		}

	}

	return( dwResult );
}