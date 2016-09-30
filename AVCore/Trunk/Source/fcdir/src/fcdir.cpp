#include <stdio.h>
#include <stdlib.h>
#include "fcdir.h"
#include "fileops.h"
#include "shared.h"
#include "md5.h"

#ifdef SYM_UNIX
    #include <unistd.h>

#elif defined (SYM_NLM)
    #include <io.h>

#endif

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void main()
//
//	Description:
//      Program Main
//
//	Parameters:
//      argc
//      argv
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{

    OPTIONS	stOptions = {0};

    // Check number of parameters
	if (argc < 3)
	{
		HelpScreen(argv[0]);
		return (1);
	}
	
	if(GetCmdLineOption( "/DEL", NULL, argc, argv))
	{
		stOptions.wOptions += DELETE_MATCH;		
	}

	if(GetCmdLineOption( "/LOG=", stOptions.szLogFile, argc, argv))
	{
		stOptions.wOptions += LOG_REPORT;		
	}

	if(GetCmdLineOption( "/MD5=", stOptions.szMD5LogFile, argc, argv))
	{
		stOptions.wOptions += MD5_LOG_REPORT;		
	}

	if(GetCmdLineOption( "/FOMD5=", stOptions.szFOMD5LogFile, argc, argv))
	{
		stOptions.wOptions += FOMD5_LOG_REPORT;		
	}

	if (stOptions.wOptions & LOG_REPORT)
	{
		if(stOptions.szLogFile[0] == NULL)
		{
			printf("Error: No report log specified!\n");
			HelpScreen(argv[0]);
			return (1);
		}
	}

	if (stOptions.wOptions & MD5_LOG_REPORT)
	{
		if(stOptions.szMD5LogFile[0] == NULL)
		{
			printf("Error: No MD5 report log (/MD5=) specified!\n");
			HelpScreen(argv[0]);
			return (1);
		}
	}

	if (stOptions.wOptions & FOMD5_LOG_REPORT)
	{
		if(stOptions.szFOMD5LogFile[0] == NULL)
		{
			printf("Error: No File Only MD5 report log (/FOMD5=) specified!\n");
			HelpScreen(argv[0]);
			return (1);
		}
	}

	if (stOptions.wOptions & MD5_LOG_REPORT ||
		stOptions.wOptions & FOMD5_LOG_REPORT)
	{
		if (stOptions.wOptions & DELETE_MATCH ||
			stOptions.wOptions & LOG_REPORT)
		{
			printf("Error: cannot use /DEL and /LOG= options with /MD5= or /FOMD5= options!\n");
			HelpScreen(argv[0]);
			return (1);
		}
	}

	if (stOptions.wOptions & MD5_LOG_REPORT ||
		stOptions.wOptions & FOMD5_LOG_REPORT)
	{
		ProcessDirMD5(argv[1], argv[1], &stOptions);
		printf ("MD5 hash generation is complete!\n");
	}
	else
	{
		ProcessDir(argv[1], argv[1], argv[2], &stOptions);
		printf ("Compare is complete!\n");
	}
	return (0);
}

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void HelpScreen()
//
//	Description:
//      Display the help screen
//
//	Parameters:
//      char* pszFileName
//			The filename of itself, for example, fcdir.exe on Win32.
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void HelpScreen(char* pszFileName)
{

	printf("Usage (compare 2 dirs):\n\t%s dir1 dir2 [options]\n", pszFileName);

	printf("options:\n");
	printf("\t/DEL      Delete file from first directory if matched\n");
	printf("\t/LOG=     Specify full path to log file\n");

	printf("\n");
	printf("\t-or-\n");
	printf("\n");

	printf("Usage (generate MD5 hashes on 1 dir):\n\t%s dir1 /MD5=\n", pszFileName);

	printf("options:\n");
	printf("\t/MD5=     Specify path to dump MD5 binary compare file\n");
	printf("\t/FOMD5=   Same as above except filenames only\n");
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessDir()
//
//	Description:
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpszDir2
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessDir
( 
	LPSTR		lpszPath,
	LPSTR		lpszDir1,
	LPSTR		lpszDir2,
	LPOPTIONS	lpOptions
)
{
	HANDLE			hDir;
	FIND_FILE_DATA	FileData;
	char*			pStrOffset;
	char 			szScanPath[_MAX_PATH]	= {0};

	int iResult = 0;

	// Make copy of passed in path
	strcpy (szScanPath, lpszPath);

	// add *.*	
	if (szScanPath[strlen (szScanPath) - 1] == SLASH)
	{
		strcat (szScanPath, STAR);
	}else
	{
		strcat (szScanPath, SLASH_STAR);
	}
	
	hDir = __FindFirstFile (szScanPath, &FileData);
	
	if (hDir != INVALID_HANDLE )
	{
		
		do
		{
			// Ignore "." and ".." entries
			if ((strcmp(FileData.cFileName, ".") != 0) && (strcmp(FileData.cFileName, "..") != 0 ))
			{

				// Extract path out and append file name
				if ((pStrOffset = strrchr (szScanPath, SLASH)) != NULL)
				{
					*pStrOffset++;
					*pStrOffset = NULL;
					strcat (szScanPath, FileData.cFileName);
				}
				else if ((pStrOffset = strstr(szScanPath, SLASH_STAR)) != NULL)
				{
					*pStrOffset = NULL;
					strcat (szScanPath, (LPSTR)SLASH);
					strcat (szScanPath, FileData.cFileName);
				}
			
				// Check to see if it is a directory
				if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{

                #if defined(SYM_NLM)
                	ThreadSwitchWithDelay();
                #endif

					ProcessDir (szScanPath, lpszDir1, lpszDir2, lpOptions);
				}
				else
				{
    
                #if defined(SYM_NLM)
                	ThreadSwitchWithDelay();
                #endif
            
				   //printf("%s\n",szScanPath);
				   ProcessFile(szScanPath, lpszDir1, lpszDir2, lpOptions);
				
				}

			}

			
		}while(__FindNextFile (hDir, &FileData));

		__FindClose (hDir);
	}
	
	
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessDirMD5()
//
//	Description:
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessDirMD5
( 
	LPSTR		lpszPath,
	LPSTR		lpszDir1,
	LPOPTIONS	lpOptions
)
{
	HANDLE			hDir;
	FIND_FILE_DATA	FileData;
	char*			pStrOffset;
	char 			szScanPath[_MAX_PATH]	= {0};

	int iResult = 0;

	// Make copy of passed in path
	strcpy (szScanPath, lpszPath);

	// add *.*	
	if (szScanPath[strlen (szScanPath) - 1] == SLASH)
	{
		strcat (szScanPath, STAR);
	}else
	{
		strcat (szScanPath, SLASH_STAR);
	}
	
	hDir = __FindFirstFile (szScanPath, &FileData);
	
	if (hDir != INVALID_HANDLE )
	{
		
		do
		{
			// Ignore "." and ".." entries
			if ((strcmp(FileData.cFileName, ".") != 0) && (strcmp(FileData.cFileName, "..") != 0 ))
			{

				// Extract path out and append file name
				if ((pStrOffset = strrchr (szScanPath, SLASH)) != NULL)
				{
					*pStrOffset++;
					*pStrOffset = NULL;
					strcat (szScanPath, FileData.cFileName);
				}
				else if ((pStrOffset = strstr(szScanPath, SLASH_STAR)) != NULL)
				{
					*pStrOffset = NULL;
					strcat (szScanPath, (LPSTR)SLASH);
					strcat (szScanPath, FileData.cFileName);
				}
			
				// Check to see if it is a directory
				if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{

                #if defined(SYM_NLM)
                	ThreadSwitchWithDelay();
                #endif

					ProcessDirMD5 (szScanPath, lpszDir1, lpOptions);
				}
				else
				{
    
                #if defined(SYM_NLM)
                	ThreadSwitchWithDelay();
                #endif
            
				   //printf("%s\n",szScanPath);
				   ProcessFileMD5(szScanPath, lpszDir1, lpOptions);
				
				}

			}

			
		}while(__FindNextFile (hDir, &FileData));

		__FindClose (hDir);
	}
	
	
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessFile()
//
//	Description:
//      Process files
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpszDir2
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessFile
( 
	LPSTR		lpszPath,
	LPSTR		lpszDir1,
	LPSTR		lpszDir2,
	LPOPTIONS	lpOptions
)
{

	FILE*	fpFile1;
	FILE*	fpFile2;
	FILE*	fpLogFile;
	char	szFilePath1[_MAX_PATH];
	char	szFilePath2[_MAX_PATH];
	char	szDiff[_MAX_PATH];
	DWORD	dwResult;

	PathDiff(lpszDir1, lpszPath, szDiff );

	strcpy (szFilePath1, lpszPath);
	strcpy (szFilePath2, lpszDir2);
	strcat (szFilePath2, szDiff);



	///////////////////////////////////////////////////////////////////////////
	// Increment files processed
	///////////////////////////////////////////////////////////////////////////
	lpOptions->dwFilesProcessed++;

	printf("Comparing files...\n");
	printf("%s\n", szFilePath1);
	printf("%s\n", szFilePath2);

	if(lpOptions->wOptions & LOG_REPORT)
	{
		if ((fpLogFile = fopen (lpOptions->szLogFile, "a+t")) != NULL)
		{
			fprintf (fpLogFile, "Comparing files...\n"); 
			fprintf(fpLogFile, "%s\n", szFilePath1);
			fprintf(fpLogFile, "%s\n", szFilePath2);
		} 
		else
		{
			printf("ERROR: Can not write to log file %s!\n", lpOptions->szLogFile);
			exit(1);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Open file 1
	///////////////////////////////////////////////////////////////////////////
	if ((fpFile1 = fopen(szFilePath1, "rb")) == NULL)
	{
		if(lpOptions->wOptions & LOG_REPORT)
		{
			fprintf(fpLogFile, "File %s not found!\n",szFilePath1); 
		}
			
		printf("File %s not found!\n",szFilePath1); 
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// Open file 2
	///////////////////////////////////////////////////////////////////////////
	if ((fpFile2 = fopen(szFilePath2, "rb")) == NULL)
	{
		if(lpOptions->wOptions & LOG_REPORT)
		{
			fprintf(fpLogFile, "File %s not found!\n",szFilePath2);
		}

		printf("File %s not found!\n",szFilePath2);
		fclose(fpFile1);
		return;
	}	

	///////////////////////////////////////////////////////////////////////////
	// Compare files
	///////////////////////////////////////////////////////////////////////////

	dwResult = FileCompare (fpFile1, fpFile2);

	///////////////////////////////////////////////////////////////////////////
	// Close files
	///////////////////////////////////////////////////////////////////////////
	fclose(fpFile1);
	fclose(fpFile2);


	///////////////////////////////////////////////////////////////////////////
	// Check file compare result
	///////////////////////////////////////////////////////////////////////////
	switch (dwResult)
	{
		///////////////////////////////////////////////////////////////////////
		// Files match
		///////////////////////////////////////////////////////////////////////
		case MATCH_SUCCESS:
			if(lpOptions->wOptions & LOG_REPORT)
			{
				fprintf(fpLogFile, "Files match!\n");
			}

			printf("Files match!\n");
			lpOptions->dwFilesMatched++;
			
				
			// Attempt to delete file
			if (lpOptions->wOptions & DELETE_MATCH)
			{
				printf("Deleting file:\n");
				printf("%s\n", szFilePath1);
				
                if(!DeleteFile(szFilePath1))
				{
					if(lpOptions->wOptions & LOG_REPORT)
					{
						fprintf(fpLogFile, "ERROR: Can not delete file!\n");
					}

					printf("ERROR: Can not delete file!\n");
				}
				else
				{	
					if(lpOptions->wOptions & LOG_REPORT)
					{
						fprintf(fpLogFile, "File Deleted!\n");
					}
					
					lpOptions->dwFilesDeleted++;
				}

			}
			break;


		///////////////////////////////////////////////////////////////////////
		// Files are different size
		///////////////////////////////////////////////////////////////////////
		case MATCH_DIFF_SIZE:
			printf ("Files are different sizes!\n");			
			if(lpOptions->wOptions & LOG_REPORT)
			{
				fprintf (fpLogFile, "Files are different sizes!\n");
			}
	
			break;


		///////////////////////////////////////////////////////////////////////
		// Files do not match
		///////////////////////////////////////////////////////////////////////
		case MATCH_FAILED:
			printf ("Files do NOT match!\n");
			if(lpOptions->wOptions & LOG_REPORT)
			{
				fprintf (fpLogFile, "Files do NOT match!\n");
			}
			
			break;


		///////////////////////////////////////////////////////////////////////
		// default
		///////////////////////////////////////////////////////////////////////
		default:
			break;

	}

	printf ("Processed: %d   Matched: %d    Deleted: %d\n\n", lpOptions->dwFilesProcessed,
		                                                      lpOptions->dwFilesMatched,
			 											      lpOptions->dwFilesDeleted);


	if(lpOptions->wOptions & LOG_REPORT)
	{
		fprintf (fpLogFile, "Processed: %d   Matched: %d    Deleted: %d\n\n", lpOptions->dwFilesProcessed,
			                                                                  lpOptions->dwFilesMatched,
				      										                  lpOptions->dwFilesDeleted);

		fclose (fpLogFile);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessFileMD5()
//
//	Description:
//      Process files
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessFileMD5
( 
	LPSTR		lpszPath,
	LPSTR		lpszDir1,
	LPOPTIONS	lpOptions
)
{

	FILE*	fpFile1;
	FILE*	fpMD5LogFile;
	FILE*	fpFOMD5LogFile;
	char	szFilePath1[_MAX_PATH];
	int		nPathLen;
	int		i;
	char	szMD5Hash[33];

	strcpy (szFilePath1, lpszPath);

	///////////////////////////////////////////////////////////////////////////
	// Increment files processed
	///////////////////////////////////////////////////////////////////////////
	lpOptions->dwFilesProcessed++;

	printf("Processing file %s...\n", szFilePath1);

	if (lpOptions->szMD5LogFile[0] != NULL)
	{
		if ((fpMD5LogFile = fopen (lpOptions->szMD5LogFile, "a+t")) != NULL)
		{
			fprintf(fpMD5LogFile, "%s;", szFilePath1);
			// Write to log the filename, which is defined to be
			// from the last '/' or '\', or szFilePath1 if no '/' or '\'.
			nPathLen = strlen(szFilePath1);
			for (i = nPathLen-1; i >= 0; i--)
			{
				if (szFilePath1[i] == '/' || szFilePath1[i] == '\\')
				{
					fprintf(fpMD5LogFile, "%s;", &(szFilePath1[i+1]));
					break;
				}
			}
			if (i < 0)
			{
				// No '/' or '\' found.
				fprintf(fpMD5LogFile, "%s;", szFilePath1);
			}
		} 
		else
		{
			printf("ERROR: Can not write to log file %s!\n", lpOptions->szMD5LogFile);
			exit(1);
		}
	}
	if (lpOptions->szFOMD5LogFile[0] != NULL)
	{
		if ((fpFOMD5LogFile = fopen (lpOptions->szFOMD5LogFile, "a+t")) != NULL)
		{
			// Write to log the filename, which is defined to be
			// from the last '/' or '\', or szFilePath1 if no '/' or '\'.
			nPathLen = strlen(szFilePath1);
			for (i = nPathLen-1; i >= 0; i--)
			{
				if (szFilePath1[i] == '/' || szFilePath1[i] == '\\')
				{
					fprintf(fpFOMD5LogFile, "%s;", &(szFilePath1[i+1]));
					break;
				}
			}
			if (i < 0)
			{
				// No '/' or '\' found.
				fprintf(fpFOMD5LogFile, "%s;", szFilePath1);
			}
		} 
		else
		{
			printf("ERROR: Can not write to log file %s!\n", lpOptions->szFOMD5LogFile);
			exit(1);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Open file 1
	///////////////////////////////////////////////////////////////////////////
	if ((fpFile1 = fopen(szFilePath1, "rb")) == NULL)
	{
		if(lpOptions->wOptions & MD5_LOG_REPORT)
		{
			fprintf(fpMD5LogFile, "File %s not found!\n",szFilePath1); 
		}
		if(lpOptions->wOptions & FOMD5_LOG_REPORT)
		{
			fprintf(fpFOMD5LogFile, "File %s not found!\n",szFilePath1); 
		}
			
		printf("File %s not found!\n",szFilePath1); 
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// Generate MD5 hash.
	///////////////////////////////////////////////////////////////////////////

	GenerateMD5Hash (fpFile1, szMD5Hash);

	///////////////////////////////////////////////////////////////////////////
	// Close files
	///////////////////////////////////////////////////////////////////////////
	fclose(fpFile1);

	// Output MD5 hash result.
	if (lpOptions->wOptions & MD5_LOG_REPORT)
	{
		fprintf(fpMD5LogFile, "%s\n", szMD5Hash);
	}
	if (lpOptions->wOptions & FOMD5_LOG_REPORT)
	{
		fprintf(fpFOMD5LogFile, "%s\n", szMD5Hash);
	}
	printf("MD5 hash: %s\n", szMD5Hash);
		
	printf ("Processed: %d\n\n", lpOptions->dwFilesProcessed);

	if (lpOptions->wOptions & MD5_LOG_REPORT)
	{
		fclose (fpMD5LogFile);
	}
	if (lpOptions->wOptions & FOMD5_LOG_REPORT)
	{
		fclose (fpFOMD5LogFile);
	}
}


// pszMD5HashOut has to point to a character array that's
// 33 bytes long at least.
static bool GenerateMD5Hash
(
    FILE*	fpFile,
	char*	pszMD5HashOut
)
{
	unsigned char szBuffer[MAX_BUFFER_SIZE];

	DWORD	lFileSize;
	int		nBytesRead;
	MD5_CTX rMD5;
	unsigned char szDigest[16];

	// return to top of file
	fseek( fpFile, 0, SEEK_SET );

	// Get file 1 size
	fseek( fpFile, 0, SEEK_END );
	lFileSize = ftell(fpFile);

	// Reset file pointers
	fseek( fpFile, 0, SEEK_SET );

	// initialize the single complex element - the MD5 context
    MD5Init(&rMD5);

	// Generate MD5 hash.
	while ((nBytesRead = fread(&szBuffer,1,MAX_BUFFER_SIZE,fpFile )) > 0)
	{
		MD5Update(&rMD5, szBuffer, nBytesRead);
	}

	MD5Final(szDigest, &rMD5);

	sprintf(pszMD5HashOut,
			"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			szDigest[0],
			szDigest[1],
			szDigest[2],
			szDigest[3],
			szDigest[4],
			szDigest[5],
			szDigest[6],
			szDigest[7],
			szDigest[8],
			szDigest[9],
			szDigest[10],
			szDigest[11],
			szDigest[12],
			szDigest[13],
			szDigest[14],
			szDigest[15]);

	return true;
}


///////////////////////////////////////////////////////////
//
//	Function Name:
//		WORD FileCompare()
//
//	Parameters:
//		fpFile1
//		fpFile2
//
//	Returns:
//		MATCH_SUCCESS		0
//		MATCH_DIFF_SIZE		1
//		MATCH_FAILED		2
//
///////////////////////////////////////////////////////////
DWORD FileCompare
( 
    FILE*	fpFile1,
    FILE*	fpFile2
)
{

	BYTE			szBuffer1[MAX_BUFFER_SIZE];
	BYTE			szBuffer2[MAX_BUFFER_SIZE];

	DWORD	dwBytesRead1;
	DWORD	dwBytesRead2;
	DWORD	lFileSize1;
	DWORD	lFileSize2;

	// return to top of file
	fseek( fpFile1, 0, SEEK_SET );
	fseek( fpFile2, 0, SEEK_SET );

	// Get file 1 size
	fseek( fpFile1, 0, SEEK_END );
	lFileSize1 = ftell(fpFile1);

	// Get file 2 size
	fseek( fpFile2, 0, SEEK_END );
	lFileSize2 = ftell(fpFile2);

	// Reset file pointers
	fseek( fpFile1, 0, SEEK_SET );
	fseek( fpFile2, 0, SEEK_SET );

	// Check for file size differences
	if( lFileSize1 != lFileSize2 )
	{
		return( MATCH_DIFF_SIZE );
	}


	// Compare files
	while ((dwBytesRead1 = fread(&szBuffer1, sizeof(BYTE),sizeof(szBuffer1),fpFile1 )) > 0)
	{	
		dwBytesRead2 = fread(&szBuffer2, sizeof(BYTE),sizeof(szBuffer2),fpFile2);

		if (memcmp(szBuffer1, szBuffer2, dwBytesRead1) != 0)
		{
			return(MATCH_FAILED);
		}
		
	}

	return( MATCH_SUCCESS );

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function Name:
//		WORD PathDiff()
//
//	Description:
//
//	Parameters:
//		lpszFileName1
//		lpszFileName2
//		lpResult
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
WORD PathDiff
( 
	LPSTR lpszFilePath1,
	LPSTR lpszFilePath2,
	LPSTR lpResult
)
{
	
	int		iStrLen1;
	int		iStrLen2;
	int		iCnt;

	
	iStrLen1 = strlen(lpszFilePath1);
	iStrLen2 = strlen(lpszFilePath2);

	lpResult[0] = NULL;

	if( iStrLen1 < iStrLen2 )
	{

		for( iCnt = 0; iCnt <= (iStrLen2-iStrLen1); iCnt++ )
		{
			lpResult[iCnt] = lpszFilePath2[iStrLen1+iCnt];
		}

		lpResult[iCnt] = NULL;
	}

	
	return( 0 );
}


#if !defined (SYM_WIN32)
///////////////////////////////////////////////////////////////////////////////
//
//	Function Name:
//		BOOL DeleteFile()
//
//	Description:
//      Delete a file
//
//	Parameters:
//		lpFileName
//
//	Returns:
//      TRUE
//      FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL DeleteFile(LPCTSTR lpFileName)
{

#if defined (SYM_DOS)
	if (unlink (lpFileName) == 0)
	{
		return (TRUE);
	}

#elif defined (SYM_UNIX)
	if (unlink (lpFileName) == 0)
	{
		return (TRUE);
	}

#elif defined (SYM_NLM)
	if (unlink (lpFileName) == 0)
	{
		return (TRUE);
	}

#endif

	return (FALSE);
}
#endif

