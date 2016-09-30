#include "fileops.h"
#include "shared.h"


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
	LPCTSTR				lpFileName,  
	LPFIND_FILE_DATA	lpFindFileData	
)
{

//-----------------------------------------------------------------------------
// (DOS) __FindFirstFile code
//-----------------------------------------------------------------------------
#if defined (SYM_DOS)
	
	struct _find_t *hFileData;   
	
	hFileData = (_find_t*)malloc(sizeof(_find_t));

	// Set all attributes
	unsigned char cAttrib = _A_ARCH | _A_HIDDEN | _A_NORMAL | _A_RDONLY | _A_SUBDIR | _A_SYSTEM;
	
	// Find file
	if (_dos_findfirst ((char*)lpFileName, cAttrib, (_find_t*)hFileData) != 0) 
	{
		return (NULL);	
	}
	else
	{
		
		// Save file name
		strcpy ((char*)lpFindFileData->cFileName, (char*)hFileData->name);
		
		// Save file attributes
		lpFindFileData->dwFileAttributes = hFileData->attrib;

		// Save file size
		lpFindFileData->nFileSizeLow = hFileData->size;

		return (hFileData);		
	}


//-----------------------------------------------------------------------------
// (WIN32) __FindFirstFile code
//-----------------------------------------------------------------------------
#elif defined (SYM_WIN32)
	return (FindFirstFile (lpFileName, lpFindFileData));


//-----------------------------------------------------------------------------
// (NLM) __FindFirstFile code
//-----------------------------------------------------------------------------
#elif defined (SYM_NLM)

	DIR *hDir, *hDirTmp;

	if ((hDir = opendir (lpFileName)) == NULL)
	{
		return( INVALID_HANDLE );
	}
	else 
	{

		if ((hDirTmp = readdir( hDir)) == NULL)
			return (INVALID_HANDLE);
		
		// Save file name
		strcpy (lpFindFileData->cFileName, hDirTmp->d_name);
		
		// Save file attributes
		lpFindFileData->dwFileAttributes = hDirTmp->d_attr;

		// Save file size
		lpFindFileData->nFileSizeLow = hDirTmp->d_size;
	
		return (hDir);

	}


//-----------------------------------------------------------------------------
// (UNIX) __FindFirstFile code
//-----------------------------------------------------------------------------
#elif defined (SYM_UNIX)

	DIR            *hDir;
	struct dirent  *hReadDir;
	struct stat    stFileData = {0};
	char           szOrigPath[_MAX_PATH] = {0};
	char		   szTmpPath[_MAX_PATH] = {0};
	int	    	   iPathLen=0;
    char           szTmpFileName[_MAX_PATH] = {0};

    ExtractPathName(lpFileName, szTmpFileName);

    if(szTmpFileName[0] == (char)NULL)
    {
        strcpy(szTmpFileName, "/\0");    
    }
        
	if ((hDir = opendir (szTmpFileName)) == NULL)
	{
		return( INVALID_HANDLE );
	}
	else                   
	{
        
        // Make copy of original path
  		//strcpy (szOrigPath, lpFileName);
         ExtractPathName(lpFileName, szOrigPath);
		//strip off any ending slashes
		iPathLen = strlen( szOrigPath );
		if (szOrigPath[iPathLen-1] == '/')
		{
			szOrigPath[iPathLen-1] = (char)NULL;	
		}		
            
        // Read in next file		
		if ((hReadDir = readdir(hDir)) == NULL)
			return (INVALID_HANDLE);
		
		// Get STAT info for file
		sprintf (szTmpPath, "%s/%s", szOrigPath, hReadDir->d_name);		
		lstat (szTmpPath, &stFileData);

		// Save file name
		strcpy (lpFindFileData->cFileName, hReadDir->d_name);

		// Save file name
		strcpy (lpFindFileData->cPath, szOrigPath);
		
		// Save file attributes
		lpFindFileData->dwFileAttributes = stFileData.st_mode;

		// Save file size
		lpFindFileData->nFileSizeLow = stFileData.st_size;

		return (hDir);

	}

#endif


	return (NULL);
}


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
	HANDLE				hFindFile,
	LPFIND_FILE_DATA	lpFindFileData	
)
{

//-----------------------------------------------------------------------------
// (DOS) __FindNextFile code
//-----------------------------------------------------------------------------
#if defined (SYM_DOS)
		
	if (_dos_findnext ((_find_t*)hFindFile) == 0)
	{
		// Save file name
		strcpy (lpFindFileData->cFileName, hFindFile->name);
		      
		// Save file attributes
		lpFindFileData->dwFileAttributes = hFindFile->attrib;

		// Save file size
		lpFindFileData->nFileSizeLow = hFindFile->size;
		
		return (TRUE);
	
	}
	else
	{
		return (FALSE);
	}


//-----------------------------------------------------------------------------
// (WIN32) __FindNextFile code
//-----------------------------------------------------------------------------
#elif defined (SYM_WIN32)
	return (FindNextFile (hFindFile, lpFindFileData));


//-----------------------------------------------------------------------------
// (NLM) __FindNextFile code
//-----------------------------------------------------------------------------
#elif defined (SYM_NLM)

	DIR *hDir;

	if ((hDir = readdir (hFindFile)) == NULL)
		return (FALSE);
	
	// Save file name
	strcpy (lpFindFileData->cFileName, hDir->d_name);
	
	// Save file attributes
	lpFindFileData->dwFileAttributes = hDir->d_attr;

	// Save file size
	lpFindFileData->nFileSizeLow = hDir->d_size;



//-----------------------------------------------------------------------------
// (UNIX) __FindNextFile code
//-----------------------------------------------------------------------------
#elif defined (SYM_UNIX)

	//HANDLE hDir;
	struct dirent  *hReadDir;
	struct stat    stFileData;
	
	char szTmpPath[_MAX_PATH];

	// Get next file
	if ((hReadDir = readdir (hFindFile)) == NULL)
		return (FALSE);
	
	// Get STAT info
	sprintf (szTmpPath,"%s/%s", lpFindFileData->cPath, hReadDir->d_name);
	if( lstat ( szTmpPath, &stFileData ) < 0 ) 
		return (FALSE);
	
	// Save file name
	strcpy (lpFindFileData->cFileName, hReadDir->d_name);
	
	// Save file attributes
	lpFindFileData->dwFileAttributes = stFileData.st_mode;

	// Save file size
	lpFindFileData->nFileSizeLow = stFileData.st_size;

#endif

	return (TRUE);
}


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
	HANDLE				hFindFile
)
{

#if defined (SYM_DOS)
	free((_find_t*)hFindFile);

#elif defined (SYM_WIN32)
	return ( FindClose (hFindFile));

#elif defined (SYM_NLM) || defined (SYM_UNIX)
	closedir( hFindFile );

#endif
 	
	return (TRUE);
}
