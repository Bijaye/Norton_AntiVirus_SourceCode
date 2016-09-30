#include "shared.h"

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
)
{

	int		iCmdOpts;
	TCHAR	szArg[255];
	LPTSTR	lpszBuffer;
	TCHAR	szTag[255];

	// Clear result
	if( lpszValue != NULL )
		lpszValue[0] = (char)NULL;

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
DWORD StrToHex(	LPTSTR lpszValue )
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

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ExtractFileExt()
//
//	Description:
//		Extracts the extension part of a file name.
//			example:	passing in [test.file] returns [file]
//
//
//	Parameters:
//		lpszFileName		Pointer full file name including the extension (in variable)
//		lpszFileExt			Pointer to extension (out variable)
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
//		lpszFilePath		Pointer to full file path name (in variable)
//		lpszFileName		Pointer to file name (out variable)
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

	// Find last occurance of "/" or "\"
	if ((lpszTmpFileName = strrchr (szTmpFileName, SLASH)) == NULL)
	{
		// Entire file path is the file.
		strcpy(lpszFileName, lpszFilePath);
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
	if ((lpszTmpPathName = strrchr (szTmpPathName, SLASH)) == NULL)
	{
		strcpy(lpszPathName, "\0");
	}
	else
	{
		*lpszTmpPathName = (char)NULL;
	    strcpy(lpszPathName, szTmpPathName);
	}

}

#if defined (SYM_UNIX)
LPTSTR strupr (LPTSTR lpszString)
{
    int nLen;
    int nIndex;

    if (lpszString)
    {
        nLen = strlen(lpszString);

        for (nIndex = 0; nIndex < nLen; nIndex++)
        {
            lpszString[nIndex] = toupper(lpszString[nIndex]);
        }
    }
    
   return lpszString;
}
#endif

#if defined (SYM_UNIX)
LPTSTR strlwr (LPTSTR lpszString)
{
    int nLen;
    int nIndex;

    if (lpszString)
    {
        nLen = strlen(lpszString);

        for (nIndex = 0; nIndex < nLen; nIndex++)
        {
            lpszString[nIndex] = tolower(lpszString[nIndex]);
        }
    }
    
   return lpszString;
}
#endif
