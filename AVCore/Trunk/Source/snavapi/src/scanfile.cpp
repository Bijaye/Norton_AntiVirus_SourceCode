#include "scanfile.h"
#include "shared.h"
#include "fileops.h"
#include "spthread.h"
#include "log.h"
#include "time.h"
#include "md5.h"


#ifndef SYM_WIN32
#include "perf.h"
#define HIGHPART HighLow.HighPart
#define LOWPART HighLow.LowPart
#else
#define HIGHPART HighPart
#define LOWPART LowPart
#endif // #ifdef SYM_UNIX

///////////////////////////////////////////////////////////////////////////////
// Global Declarations
///////////////////////////////////////////////////////////////////////////////
DWORD	gdwTotalScanned     = 0;
DWORD	gdwTotalInfected    = 0; 
DWORD	gdwTotalRepaired    = 0;
DWORD	gdwTotalDeleted     = 0;
DWORD   gThreadHandle       = 0;
DWORD   gdwThreadCount      = 0;
clock_t gStart, gFinish;
double  gScanDuration		= 0;

ULARGE_INTEGER	glargeunTotalTime;

///////////////////////////////////////////////////////////////////////////////
// Mutexes
///////////////////////////////////////////////////////////////////////////////
SMUTEXHANDLE MutexThreadCounter;
SMUTEXHANDLE MutexScannedCounter;
SMUTEXHANDLE MutexPerfTotalTime;

pthread_attr_t pthread_attr;

///////////////////////////////////////////////////////////////////////////////
// If load virus hash table succeeded.
///////////////////////////////////////////////////////////////////////////////
static BOOL bVirusHashTableLoadSucceed;


typedef struct
{
	char szFilePath[_MAX_PATH];	// 1. filepath.
	char szResultCode[64];		// 2. Scan Result Code.
	char szIsInfected[16];		// 3. Is file infected?.
	char szVirusName[64];		// 4. Virus Name.
	char szRepOrDel[32];		// 5. Was Repair or Delete attempted?.
	char szRepOrDelResult[32];	// 6. Repair/Delete result.
	char szRepOrDelResCode[64];	// 7. Rep/Delete Result code.
	unsigned int unScanned;		// 8. Number of files scanned.
	unsigned int unInfected;	// 9. Number of infected files.
	unsigned int unRepaired;	// 10. Number of repaired files.
	unsigned int unDeleted;		// 11. Number of deleted files.
	char szScanTime[64];		// 12. Time to scan this file.
	char szMD5HashAfter[33];	// 14. MD5 hash after repair or delete.
} SCANRESULTS;



///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanFile()
//
//	Description:
//
//	Parameters:
//		argc
//		argv
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void ScanFile (int argc, char* argv[])
{
	SCANFILEOPTS		stScanFileOpts	= {0};
	NAVAPI_CONFIG		stNAVAPI_Config = {0};
	NAVSTATUS			NAVStatus = 0;
	char				szDefInfo[1024] = {0};
	char				szTempStrBuf[4096];
	NAVVIRUSDBINFOEX sVirusDBInfoEx;
	float fHighPart;
	float fLowPart;
	float fTotalTimeInSeconds;
	float fMaxTimeInSeconds;
	BOOL bDoesHardwareSupportPerfCounter;
	ULARGE_INTEGER largeunPerfFreq;
	LARGE_INTEGER largenTemp;
	int nTotalIterations;

	glargeunTotalTime.QuadPart = 0;
	//struct tagNAVVIRUSDBINFOEX sVirusDBInfoEx;
	
	// Get boot scan options
	if (!GetScanFileOptions(&stScanFileOpts, argc, argv))
	{
		ScanFileHelp();
		return;
	}

	// Create mutexes
	sCreateMutex ( &MutexThreadCounter );
	sCreateMutex ( &MutexScannedCounter );
	sCreateMutex ( &MutexPerfTotalTime );

	///////////////////////////////////////////////////////////////////////////
	// Initialize NAVAPI configuration structure
	///////////////////////////////////////////////////////////////////////////
	stNAVAPI_Config.dwOptions = stScanFileOpts.dwOptions;
	strcpy (stNAVAPI_Config.szDefsPath, stScanFileOpts.szDefLoc);
	strcpy (stNAVAPI_Config.szNavexInfFile, stScanFileOpts.szNavexInfFile);
	strcpy (stNAVAPI_Config.szReportLogFile, stScanFileOpts.szLogLoc);
	stNAVAPI_Config.wHeuristicLevel = stScanFileOpts.wHeuristicLevel;
	stNAVAPI_Config.dwNumOfTimesToRepeatScan = stScanFileOpts.dwNumOfTimesToRepeatScan;
	stNAVAPI_Config.dwNumOfTimesToRepeatTotalScan = stScanFileOpts.dwNumOfTimesToRepeatTotalScan;
	stNAVAPI_Config.bTabMode = stScanFileOpts.bTabMode;

	///////////////////////////////////////////////////////////////////////////
	// Start overall timing function.
	///////////////////////////////////////////////////////////////////////////
	gStart = clock();

	///////////////////////////////////////////////////////////////////////////
	// Initialize NAVAPI engine
	///////////////////////////////////////////////////////////////////////////
	if ((stNAVAPI_Config.hNAVEngine = NAVEngineInit (stNAVAPI_Config.szDefsPath,
								            		 stNAVAPI_Config.szNavexInfFile,
										             NULL,
										             stNAVAPI_Config.wHeuristicLevel,
													 0,
										             &NAVStatus)) == NULL)
	{
		printf("%s\n", gNAVAPIReturnValues[NAVStatus]);
		return;
	}


	///////////////////////////////////////////////////////////////////////////
	// Print date and sequence number of definitions
	///////////////////////////////////////////////////////////////////////////
	
	sVirusDBInfoEx.dwSize = sizeof (sVirusDBInfoEx);

	NAVStatus = NAVGetVirusDBInfoEx(stNAVAPI_Config.hNAVEngine, &sVirusDBInfoEx);
	
	sprintf(szDefInfo, "Using virus definitions Version %ld, %d/%d/%d, sequence %ld.\n",
		sVirusDBInfoEx.dwVersion, (int) sVirusDBInfoEx.wMonth, (int) sVirusDBInfoEx.wDay,
		(int) sVirusDBInfoEx.wYear, sVirusDBInfoEx.dwSequence);
	PrintAndLog(szDefInfo, &stNAVAPI_Config);

	// Load virus hash table
	if (stNAVAPI_Config.dwOptions & INFO_TEST)
	{
		bVirusHashTableLoadSucceed = InitializeVirusHashTable(stNAVAPI_Config);
	}

	nTotalIterations = 0;
	do
	{
		// Begin scan here
		if ((stNAVAPI_Config.dwOptions & PROCESS_SINGLE) && (stScanFileOpts.szFilePath[0] != (char)NULL))
		{
			ProcessFile (&stNAVAPI_Config,
						 stScanFileOpts.szFilePath);

		} else if ((stNAVAPI_Config.dwOptions & PROCESS_DIR) && (stScanFileOpts.szFilePath[0] != (char)NULL))
		{

			if(stNAVAPI_Config.dwOptions & MULTI_THREAD_SCANNING)
			{
				SetLimitOpenFilePerProcess( );
				sInitThread ( &pthread_attr );
			}
                        
    		ProcessDir (&stNAVAPI_Config,
						stScanFileOpts.szFilePath);

			if ( stNAVAPI_Config.dwOptions & MULTI_THREAD_SCANNING )
			{
				// wait until all threads are finish running.
				while ( gdwThreadCount > 0 )
				{
					sSleep ( 1000 );
				}
				sDestroyThread ( &pthread_attr );
			}

		}
		if (stScanFileOpts.dwNumOfTimesToRepeatTotalScan == 0)
		{
			nTotalIterations = -1;
		}
		else
		{
			++nTotalIterations;
		}
	}
	while(nTotalIterations < (signed int) stScanFileOpts.dwNumOfTimesToRepeatTotalScan);

	///////////////////////////////////////////////////////////////////////////
	// Close NAVAPI engine
	///////////////////////////////////////////////////////////////////////////
	if ((NAVStatus = NAVEngineClose (stNAVAPI_Config.hNAVEngine)) != NAV_OK)
	{
		sprintf(szTempStrBuf, "NAVAPI ERROR CODE: %s\n", gNAVAPIReturnValues[NAVStatus]);
		PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
	}

	///////////////////////////////////////////////////////////////////////////
	// Finish overall timing function.
	///////////////////////////////////////////////////////////////////////////
	gFinish = clock();
	gScanDuration = (double) (gFinish - gStart) / CLOCKS_PER_SEC;

	// Release mutexes.
    sReleaseMutex ( &MutexThreadCounter );
    sReleaseMutex ( &MutexScannedCounter );
    sReleaseMutex ( &MutexPerfTotalTime );

	PrintAndLog("Scan Complete!\n", &stNAVAPI_Config);

	// Print overall scan time.
	sprintf(szTempStrBuf, "Overall scan time: %f sec\n", gScanDuration);
	PrintAndLog(szTempStrBuf, &stNAVAPI_Config);

	bDoesHardwareSupportPerfCounter = QueryPerformanceFrequency(&largenTemp);
	largeunPerfFreq.HIGHPART = largenTemp.HIGHPART;
	largeunPerfFreq.LOWPART = largenTemp.LOWPART;
	if (bDoesHardwareSupportPerfCounter &&
	    glargeunTotalTime.QuadPart != 0 &&
	    largeunPerfFreq.HIGHPART == 0)
	{
		fHighPart = (float) glargeunTotalTime.HIGHPART / (float) largeunPerfFreq.LOWPART;
		fLowPart = (float) glargeunTotalTime.LOWPART / (float) largeunPerfFreq.LOWPART;
		fTotalTimeInSeconds = fHighPart * ((float) 0xFFFFFFFF + (float) 1);
		fTotalTimeInSeconds += fLowPart;

		fHighPart = (float) 0x6FFFFFFF / (float) largeunPerfFreq.LOWPART;
		fLowPart = (float) 0x6FFFFFFF / (float) largeunPerfFreq.LOWPART;
		fMaxTimeInSeconds = fHighPart * ((float) 0xFFFFFFFF + (float) 1);
		fMaxTimeInSeconds += fLowPart;

		sprintf(szTempStrBuf, "Per file times added up: %0.4f sec, Max: %0.0f sec\n",
			fTotalTimeInSeconds,
			fMaxTimeInSeconds);
		PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
	}
	else
	{
		PrintAndLog("Total Time = N/A\n", &stNAVAPI_Config);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL GetScanFileOptions()
//
//	Description:
//
//	Parameters:
//		lpBootScanOpts
//		argc
//		argv
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetScanFileOptions 
(
	LPSCANFILEOPTS	lpScanFileOpts,
	int				argc,
	char*			argv[]
)
{
	char	szHeurLevel[10] = {0};
	char	szNumOfTimesToRepeatScan[10] = {0};

	///////////////////////////////////////////////////////////////////////////
	// Get command line options
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/?", NULL, argc, argv ) )
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get path to virus definitions
	///////////////////////////////////////////////////////////////////////////
	if (!GetCmdLineOption("/DEFS=", lpScanFileOpts->szDefLoc, argc, argv ) )
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get single file to scan
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/FILE=", lpScanFileOpts->szFilePath, argc, argv ) )
	{
		lpScanFileOpts->dwOptions += PROCESS_SINGLE;
	}
	else if (GetCmdLineOption("/DIR=", lpScanFileOpts->szFilePath, argc, argv ) )
	{
		lpScanFileOpts->dwOptions += PROCESS_DIR;
	}
	else
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get heuristics level
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/HEUR=", szHeurLevel, argc, argv ) )
	{
		lpScanFileOpts->wHeuristicLevel = atoi(szHeurLevel);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get times to repeat scan each file
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/ITERATIONS=", szNumOfTimesToRepeatScan, argc, argv ) )
	{
		lpScanFileOpts->dwNumOfTimesToRepeatScan = atoi(szNumOfTimesToRepeatScan);
		if ((int) lpScanFileOpts->dwNumOfTimesToRepeatScan < 0 || (int) lpScanFileOpts->dwNumOfTimesToRepeatScan >= 100000000)
		{
			printf("Error: /ITERATIONS= must be set to a value zero or greater and less than 1,000,000.\n");
			exit(1);
		}
	}
	else
	{
		// Default to scanning once per file.
		lpScanFileOpts->dwNumOfTimesToRepeatScan = 1;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get times to repeat total scan
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/TOTALITERATIONS=", szNumOfTimesToRepeatScan, argc, argv ) )
	{
		lpScanFileOpts->dwNumOfTimesToRepeatTotalScan = atoi(szNumOfTimesToRepeatScan);
		if ((int) lpScanFileOpts->dwNumOfTimesToRepeatTotalScan < 0 || (int) lpScanFileOpts->dwNumOfTimesToRepeatTotalScan >= 100000000)
		{
			printf("Error: /TOTALITERATIONS= must be set to a value zero or greater and less than 1,000,000.\n");
			exit(1);
		}
	}
	else
	{
		// Default to scanning once per file.
		lpScanFileOpts->dwNumOfTimesToRepeatTotalScan = 1;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get NAVEX15.INF full path
	///////////////////////////////////////////////////////////////////////////
	GetCmdLineOption("/INF=", lpScanFileOpts->szNavexInfFile, argc, argv );
	

	///////////////////////////////////////////////////////////////////////////
	// Get log report file name and setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/PLOG=", lpScanFileOpts->szLogLoc, argc, argv ) )
	{
		lpScanFileOpts->dwOptions += PARTIAL_VIRUS_LOG;
	}
	else if (GetCmdLineOption("/FLOG=", lpScanFileOpts->szLogLoc, argc, argv ) )
	{
		lpScanFileOpts->dwOptions += FULL_VIRUS_LOG;
	}
	

	///////////////////////////////////////////////////////////////////////////
	// Get info test setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/INFO", NULL, argc, argv ) )
	{
		lpScanFileOpts->dwOptions += INFO_TEST;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get repair setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/REP", NULL, argc, argv ) )
	{
		lpScanFileOpts->dwOptions +=  REPAIR_VIRUS;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get Delete setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/DEL", NULL, argc, argv ) )
	{
		lpScanFileOpts->dwOptions +=  DELETE_VIRUS;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get Multithreaded scanning setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/MT", NULL, argc, argv ) )
	{
		lpScanFileOpts->dwOptions +=  MULTI_THREAD_SCANNING;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get MD5 setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/MD5", NULL, argc, argv ) )
	{
		lpScanFileOpts->dwOptions += MD5_HASH;
	}

	
	///////////////////////////////////////////////////////////////////////////
	// Get Tab mode setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/TAB", NULL, argc, argv ) )
	{
		lpScanFileOpts->bTabMode = TRUE;
	}
	else
	{
		lpScanFileOpts->bTabMode = FALSE;
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate definition directory path
	///////////////////////////////////////////////////////////////////////////
	if (lpScanFileOpts->szDefLoc[0] == (char)NULL)
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate definition directory path
	///////////////////////////////////////////////////////////////////////////
	if (lpScanFileOpts->szFilePath[0] == (char)NULL)
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate log option
	///////////////////////////////////////////////////////////////////////////
	if (((lpScanFileOpts->dwOptions & PARTIAL_VIRUS_LOG) ||
		(lpScanFileOpts->dwOptions & FULL_VIRUS_LOG)) &&
		(lpScanFileOpts->szLogLoc[0] == (char)NULL))
	{
		return (FALSE);
	}
		

	return (TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessDir()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void ProcessDir
( 
  	LPNAVAPI_CONFIG		lpstNAVAPI_Config,
    LPSTR				lpszPath
)
{
	HANDLE			hDir;
	FIND_FILE_DATA	FileData;
	char*			pStrOffset;
	char 			szScanPath[_MAX_PATH]	= {0};

	//int iResult = 0;

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
					*pStrOffset = (char)NULL;
					strcat (szScanPath, FileData.cFileName);
				}
				else if ((pStrOffset = strstr(szScanPath, SLASH_STAR)) != NULL)
				{
					*pStrOffset = (char)NULL;
					strcat (szScanPath, (LPSTR)SLASH);
					strcat (szScanPath, FileData.cFileName);
				}

			
				// Check to see if it is a directory
				if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{

                #if defined(SYM_NLM)
                	ThreadSwitchWithDelay();
                #endif

					ProcessDir (lpstNAVAPI_Config,
								szScanPath);
				}
				else
				{
    
                #if defined(SYM_NLM)
                	ThreadSwitchWithDelay();
                #endif
	
					if(lpstNAVAPI_Config->dwOptions & MULTI_THREAD_SCANNING)
					{
						CallThread(lpstNAVAPI_Config, szScanPath);
					}
					else
					{
    				    ProcessFile(lpstNAVAPI_Config,	szScanPath);
					}
				}

			}

			
		}while(__FindNextFile (hDir, &FileData));

		__FindClose (hDir);
	}
	
	
}



#define MAX_BUFFER_SIZE 8096

// Gets the MD5 hash of lpszFilePath, stores it in szMD5.
void GetMD5Hash(const LPSTR lpszFilePath, char szMD5[33])
{
	unsigned char szBuffer[MAX_BUFFER_SIZE];

	DWORD	lFileSize;
	int		nBytesRead;
	MD5_CTX rMD5;
	unsigned char szDigest[16];
	FILE * fpFile;

	if ((fpFile = fopen(lpszFilePath, "rb")) == NULL)
	{
		strcpy(szMD5, "Error opening file");
		return;
	}

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

	sprintf(szMD5,
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

	fclose(fpFile);
}



///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessFile()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void ProcessFile
( 
 	LPNAVAPI_CONFIG		lpstNAVAPI_Config,
	LPSTR				lpszFilePath
)
{

	HNAVVIRUS		hVirusInfo = {0};
	NAVSTATUS		NAVStatus = 0;
	char			szFullVirusInfo[MAX_GET_VIRUS_INFO_SIZE] = {0};
	char			szVirTableComparisionInfo[32768] = {0};
	char			szFilename[_MAX_PATH] = {0};
	char			szExt[_MAX_PATH] = {0};
	const char		szFullLogMarker[] ={"-------------------------------------------------------------------------------\n"};
	SCANRESULTS		sScanResults;
	char			szOutput[4096];
	char			szTempStrBuf[1024];
	int				nIndex				= 0;
	LPVIRTABLE		lpVirTable = NULL;
	LPVIRTABLE		lpVirTableDetected = NULL;
	char			szNavInfo[512]		= {0};
	DWORD			dwVirusID;
	DWORD			dwInfoSize;
	unsigned int	unScanIndex;		// Keeps track of how many repeat scans we have done.
	BOOL			bInfectable;
    BOOL            bDeleteItem = FALSE;
	ULARGE_INTEGER	largeunStart;
	ULARGE_INTEGER	largeunEnd;
	ULARGE_INTEGER	largeunTimeTaken;
	ULARGE_INTEGER	largeunPerfFreq;
	BOOL bDoesHardwareSupportPerfCounter;
	float fHighPart;	// Used as temporary value to calculate fTimeTaken.
	float fLowPart;		// Used as temporary value to calculate fTimeTaken.
	float fTimeTaken;	// The time taken to scan a file, for use in displaying output.
	DWORD dwHighPart;	// Used in division routine.
	DWORD dwHighPartRemainder;	// Used in division routine.
	DWORD dwLowPart;	// Used in division routine.
	LARGE_INTEGER	largenTemp;
	LARGE_INTEGER	largenTemp2;

	// Initialize to zero.
	largeunTimeTaken.HIGHPART = 0;
	largeunTimeTaken.LOWPART = 0;

	ExtractFileName (lpszFilePath,szFilename);
	ExtractFileExt (szFilename,szExt);

	// Increment scanned count
    sLockMutex(&MutexScannedCounter);
	gdwTotalScanned++;
    sUnLockMutex(&MutexScannedCounter);

	if (lpstNAVAPI_Config->dwNumOfTimesToRepeatScan > 1)
	{
		// Scan file once before doing the timed scan, so that the timed
		// scan's time will be closer to the true scan time, and will be more consistent.
		NAVStatus = NAVScanFile (lpstNAVAPI_Config->hNAVEngine,
			                          lpszFilePath,
				   				      szExt,
								      TRUE,
								      &hVirusInfo);
		// Free the virus handle so we don't leak memory if we have a virus.
		if (IsInfected (hVirusInfo))
		{
			NAVStatus = NAVFreeVirusHandle(hVirusInfo);

			sprintf(szTempStrBuf, "ERROR: NAVFreeVirusHandle returned %s.\n", gNAVAPIReturnValues[NAVStatus]);
			if (NAVStatus != NAV_OK)
			{
				PrintAndLog(szTempStrBuf, lpstNAVAPI_Config);
			}
		}
	}

	// Scan file
	if (lpstNAVAPI_Config->dwNumOfTimesToRepeatScan == 0)
	{
		while(TRUE)
		{
			// We don't do timing, since we are in an infinite loop.
			NAVStatus = NAVScanFile (lpstNAVAPI_Config->hNAVEngine,
									  lpszFilePath,
			   						  szExt,
									  TRUE,
									  &hVirusInfo);
			printf(".");
			// Free the virus handle so we don't leak memory if we have a virus.
			if (IsInfected (hVirusInfo))
			{
				NAVStatus = NAVFreeVirusHandle(hVirusInfo);

				sprintf(szTempStrBuf, "ERROR: NAVFreeVirusHandle returned %s.\n", gNAVAPIReturnValues[NAVStatus]);
				if (NAVStatus != NAV_OK)
				{
					PrintAndLog(szTempStrBuf, lpstNAVAPI_Config);
				}
			}
#ifndef SYM_NLM
			// For some reason, this won't work under Netware, we will
			// have no visual indication of this scan on Netware.
			fflush(stdout);
#endif
		}
	}

	// Get start time.
	bDoesHardwareSupportPerfCounter = QueryPerformanceCounter(&largenTemp);

	// Scan file for the amount of iterations we are supposed to do.
	for (unScanIndex=0; unScanIndex < lpstNAVAPI_Config->dwNumOfTimesToRepeatScan; unScanIndex++)
	  {
	    NAVStatus = NAVScanFile (lpstNAVAPI_Config->hNAVEngine,
				     lpszFilePath,
				     szExt,
				     TRUE,
				     &hVirusInfo);
		// Free the virus handle so we don't leak memory if this is not
		// the last scan, and we have a virus.
		if (unScanIndex+1 < lpstNAVAPI_Config->dwNumOfTimesToRepeatScan &&
			IsInfected (hVirusInfo))
		{
			NAVStatus = NAVFreeVirusHandle(hVirusInfo);

			sprintf(szTempStrBuf, "ERROR: NAVFreeVirusHandle returned %s.\n", gNAVAPIReturnValues[NAVStatus]);
			if (NAVStatus != NAV_OK)
			{
				PrintAndLog(szTempStrBuf, lpstNAVAPI_Config);
			}
		}
	  }

	// Get end time and performance counter frequency.
	QueryPerformanceCounter(&largenTemp2);
	largeunStart.HIGHPART = largenTemp.HIGHPART;
	largeunStart.LOWPART = largenTemp.LOWPART;
	largeunEnd.HIGHPART = largenTemp2.HIGHPART;
	largeunEnd.LOWPART = largenTemp2.LOWPART;
	largeunTimeTaken.QuadPart += largeunEnd.QuadPart - largeunStart.QuadPart;
	QueryPerformanceFrequency(&largenTemp);
	largeunPerfFreq.HIGHPART = largenTemp.HIGHPART;
	largeunPerfFreq.LOWPART = largenTemp.LOWPART;

	// Now we divide largeunTimeTaken by lpstNAVAPI_Config->dwNumOfTimesToRepeatScan.
	// Division may lose precision in the lowest significant digits, but this is ok.
	dwHighPart = largeunTimeTaken.HIGHPART;
	dwLowPart = largeunTimeTaken.LOWPART;
	dwHighPartRemainder = dwHighPart - ((dwHighPart / lpstNAVAPI_Config->dwNumOfTimesToRepeatScan) * dwHighPart);
	dwHighPart = dwHighPart / lpstNAVAPI_Config->dwNumOfTimesToRepeatScan;
	dwLowPart = dwLowPart / lpstNAVAPI_Config->dwNumOfTimesToRepeatScan;
	largeunTimeTaken.HIGHPART = dwHighPart;
	dwLowPart = dwLowPart + 
		(unsigned long int) (((float)dwHighPartRemainder/(float)lpstNAVAPI_Config->dwNumOfTimesToRepeatScan)*((float)0xFFFFFFFF+(float)1));
	largeunTimeTaken.LOWPART = dwLowPart;

	// Store scanned file path and scan result code.
	strcpy(sScanResults.szFilePath, lpszFilePath);
	strcpy(sScanResults.szResultCode, gNAVAPIReturnValues[NAVStatus]);

	// Get performance information if supported by hardware.
	if (bDoesHardwareSupportPerfCounter && largeunPerfFreq.HIGHPART == 0)
	{
		fHighPart = (float) largeunTimeTaken.HIGHPART / (float) largeunPerfFreq.LOWPART;
		fLowPart = (float) largeunTimeTaken.LOWPART / (float) largeunPerfFreq.LOWPART;
		fTimeTaken = fHighPart * ((float) 0xFFFFFFFF + (float) 1);
		fTimeTaken += fLowPart;
		sprintf(sScanResults.szScanTime, "%0.10f sec", fTimeTaken);

		sLockMutex(&MutexPerfTotalTime);
		glargeunTotalTime.QuadPart += largeunTimeTaken.QuadPart;
		sUnLockMutex(&MutexPerfTotalTime);
	}
	else
	{
		sprintf(sScanResults.szScanTime, "N/A");
	}

	// Fill in default values for #3-#7.
	strcpy(sScanResults.szIsInfected, "Unknown");
	strcpy(sScanResults.szVirusName, "N/A");
	strcpy(sScanResults.szRepOrDel, "No Repair Or Delete attempted");
	strcpy(sScanResults.szRepOrDelResult, "N/A");
	strcpy(sScanResults.szRepOrDelResCode, "N/A");

	if (NAVStatus != NAV_OK)
	{

#ifdef SYM_WIN32
		// Run NAVIsInfectableFile on this file if the Full Log option (/flog)
		// is specified on the command line.
		if (lpstNAVAPI_Config->dwOptions & FULL_VIRUS_LOG)
		{
			NAVIsInfectableFile(lpstNAVAPI_Config->hNAVEngine,
									lpszFilePath,
									szExt,
									TRUE,
									&bInfectable);
			if (bInfectable)
			{
				PrintAndLog("NAVIsInfectableFile() returned true.\n", lpstNAVAPI_Config);
			}
			else
			{
				PrintAndLog("NAVIsInfectableFile() returned false.\n", lpstNAVAPI_Config);
			}
		}
#endif // #ifdef SYM_WIN32

		// Returning, so set total scanned statistics.
		sScanResults.unScanned = gdwTotalScanned;
		sScanResults.unInfected = gdwTotalInfected;
		sScanResults.unRepaired = gdwTotalRepaired;
		sScanResults.unDeleted = gdwTotalDeleted;

		// Set the "after" MD5 hash.
		if (lpstNAVAPI_Config->dwOptions & MD5_HASH)
		{
			GetMD5Hash(lpszFilePath, sScanResults.szMD5HashAfter);
		}
		else
		{
			strcpy(sScanResults.szMD5HashAfter, "N/A");
		}

		// Print and log output.
		if (!lpstNAVAPI_Config->bTabMode)
		{
			// Normal mode.
			sprintf(szOutput,
					"Scanning file...\n%s\nScan Result Code: %s\n%s, Virus Name: %s\n%s, %s, Result Code: %s\nScan Time: %s\nMD5 Hash after: %s\nScanned: %d   Infected: %d   Repaired: %d   Deleted: %d\n\n",
					sScanResults.szFilePath,
					sScanResults.szResultCode,
					sScanResults.szIsInfected,
					sScanResults.szVirusName,
					sScanResults.szRepOrDel,
					sScanResults.szRepOrDelResult,
					sScanResults.szRepOrDelResCode,
					sScanResults.szScanTime,
					sScanResults.szMD5HashAfter,
					sScanResults.unScanned,
					sScanResults.unInfected,
					sScanResults.unRepaired,
					sScanResults.unDeleted);
		}
		else
		{
			// Tabbed output mode.
			sprintf(szOutput, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%lu;%lu;%lu;%lu\n",
					sScanResults.szFilePath,
					sScanResults.szResultCode,
					sScanResults.szIsInfected,
					sScanResults.szVirusName,
					sScanResults.szRepOrDel,
					sScanResults.szRepOrDelResult,
					sScanResults.szRepOrDelResCode,
					sScanResults.szScanTime,
					sScanResults.szMD5HashAfter,
					sScanResults.unScanned,
					sScanResults.unInfected,
					sScanResults.unRepaired,
					sScanResults.unDeleted);
		}
		PrintAndLog(szOutput, lpstNAVAPI_Config);

		return;
	}

#if defined(SYM_NLM)
    ThreadSwitchWithDelay();
#endif

	// Check if a virus found.
	if (IsInfected (hVirusInfo))
	{
		// Increment infected count
		sLockMutex(&MutexScannedCounter);
		gdwTotalInfected++;
        sUnLockMutex(&MutexScannedCounter);

		// Set virus infection state.
		strcpy(sScanResults.szIsInfected, "Infected");

		// Get Virus name
		GetVirusName (hVirusInfo, sScanResults.szVirusName, sizeof(sScanResults.szVirusName));

		// Get Virus ID
		dwInfoSize = sizeof(szNavInfo);
		if ((NAVStatus = NAVGetVirusInfo(hVirusInfo, NAV_VI_VIRUS_ID, szNavInfo, &dwInfoSize)) != NAV_OK)
		{
			sprintf(szTempStrBuf, "NAVGetVirusDefInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
			PrintAndLog(szTempStrBuf, lpstNAVAPI_Config);
			exit(1);
		}
		sscanf(szNavInfo, "%ld", &dwVirusID);

		///////////////////////////////////////////////////////////////////////
		// Get all virus info
		///////////////////////////////////////////////////////////////////////
		if (lpstNAVAPI_Config->dwOptions & FULL_VIRUS_LOG)
		{
			GetFullVirusInfo(hVirusInfo, szFullVirusInfo, sizeof(szFullVirusInfo));
		}

		///////////////////////////////////////////////////////////////////////
		// Info test, part one. Compare item specific virus info with data
		// returned from the virus table API functions.
		///////////////////////////////////////////////////////////////////////
		if ((lpstNAVAPI_Config->dwOptions & INFO_TEST) && bVirusHashTableLoadSucceed)
		{
			lpVirTableDetected = (LPVIRTABLE) malloc (sizeof (VIRTABLE));
			CopyHNAVVIRUStoLPVIRTABLE(hVirusInfo, lpVirTableDetected);

			lpVirTable = GetVirusHashTableEntry(sScanResults.szVirusName);

			///////////////////////////////////////////////////////////////////////
			// Compare lpVirTableDetected with lpVirTable, printing and logging
			// any differences.
			///////////////////////////////////////////////////////////////////////
			// Compare string values.
			if (strcmp(lpVirTable->szVirusName, lpVirTableDetected->szVirusName) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefTableInfo returned \"%s\", for NAV_VI_VIRUS_NAME.\n",
					lpVirTableDetected->szVirusName, 
					lpVirTable->szVirusName, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusAlias, lpVirTableDetected->szVirusAlias) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefTableInfo returned \"%s\", for NAV_VI_VIRUS_ALIAS.\n",
					lpVirTableDetected->szVirusAlias, 
					lpVirTable->szVirusAlias, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusInfo, lpVirTableDetected->szVirusInfo) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefTableInfo returned \"%s\", for NAV_VI_VIRUS_INFO.\n",
					lpVirTableDetected->szVirusInfo, 
					lpVirTable->szVirusInfo, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusSize, lpVirTableDetected->szVirusSize) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefTableInfo returned \"%s\", for NAV_VI_VIRUS_SIZE.\n",
					lpVirTableDetected->szVirusSize, 
					lpVirTable->szVirusSize, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			// Compare boolean values.
			for (nIndex = (int) NAV_VI_BOOL_FIRST+1;
				nIndex <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nIndex++)
			{
				if (lpVirTable->abVIBoolValues[nIndex] !=
					lpVirTableDetected->abVIBoolValues[nIndex])
				{
					sprintf (szVirTableComparisionInfo,
						"ERROR: NAVGetVirusInfo returned %d, NAVGetVirusDefTableInfo returned %d, for index %d.\n", 
						lpVirTableDetected->abVIBoolValues[nIndex],
						lpVirTable->abVIBoolValues[nIndex], nIndex);
					PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
				}
			}
			FreeVirTable(lpVirTableDetected);
		}
		///////////////////////////////////////////////////////////////////////
		// Info test, part two. Compare item specific virus info with data
		// returned from the load virus API functions.
		///////////////////////////////////////////////////////////////////////
		if (lpstNAVAPI_Config->dwOptions & INFO_TEST)
		{
			lpVirTableDetected = (LPVIRTABLE) malloc (sizeof (VIRTABLE));
			CopyHNAVVIRUStoLPVIRTABLE(hVirusInfo, lpVirTableDetected);

			lpVirTable = (LPVIRTABLE) malloc (sizeof (VIRTABLE));
			GetFullVirusInfoLoadSingleVirus(*lpstNAVAPI_Config, dwVirusID, lpVirTable);

			///////////////////////////////////////////////////////////////////////
			// Compare lpVirTableDetected with lpVirTable, printing and logging
			// any differences.
			///////////////////////////////////////////////////////////////////////
			// Compare string values.
			if (strcmp(lpVirTable->szVirusName, lpVirTableDetected->szVirusName) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefInfo returned \"%s\", for NAV_VI_VIRUS_NAME.\n",
					lpVirTableDetected->szVirusName, 
					lpVirTable->szVirusName, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusAlias, lpVirTableDetected->szVirusAlias) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefInfo returned \"%s\", for NAV_VI_VIRUS_ALIAS.\n",
					lpVirTableDetected->szVirusAlias, 
					lpVirTable->szVirusAlias, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusInfo, lpVirTableDetected->szVirusInfo) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefInfo returned \"%s\", for NAV_VI_VIRUS_INFO.\n",
					lpVirTableDetected->szVirusInfo, 
					lpVirTable->szVirusInfo, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusSize, lpVirTableDetected->szVirusSize) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefInfo returned \"%s\", for NAV_VI_VIRUS_SIZE.\n",
					lpVirTableDetected->szVirusSize, 
					lpVirTable->szVirusSize, nIndex);
				PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
			}
			// Compare boolean values.
			for (nIndex = (int) NAV_VI_BOOL_FIRST+1;
				nIndex <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nIndex++)
			{
				if (lpVirTable->abVIBoolValues[nIndex] !=
					lpVirTableDetected->abVIBoolValues[nIndex])
				{
					sprintf (szVirTableComparisionInfo,
						"ERROR: NAVGetVirusInfo returned %d, NAVGetVirusDefInfo returned %d, for index %d.\n",
						lpVirTableDetected->abVIBoolValues[nIndex],
						lpVirTable->abVIBoolValues[nIndex], nIndex);
					PrintAndLog(szVirTableComparisionInfo, lpstNAVAPI_Config);
				}
			}
			FreeVirTable(lpVirTableDetected);
			FreeVirTable(lpVirTable);
		}

        // Retrieve the DELETE_ITEM flag from the defs before freeing the infection handle:

        bDeleteItem = NAVGetVirusInfo(hVirusInfo, NAV_VI_BOOL_DELETE_ITEM, NULL, NULL);

		///////////////////////////////////////////////////////////////////////////
		// Free virus handle
		///////////////////////////////////////////////////////////////////////////
		if ((NAVStatus = NAVFreeVirusHandle(hVirusInfo)) != NAV_OK)
		{
			sprintf(szTempStrBuf, "ERROR: NAVFreeVirusHandle returned %s.\n", gNAVAPIReturnValues[NAVStatus]);
			if (!lpstNAVAPI_Config->bTabMode)
			{
				PrintAndLog(szTempStrBuf, lpstNAVAPI_Config);
			}

			// Returning, so set total scanned statistics.
			sScanResults.unScanned = gdwTotalScanned;
			sScanResults.unInfected = gdwTotalInfected;
			sScanResults.unRepaired = gdwTotalRepaired;
			sScanResults.unDeleted = gdwTotalDeleted;

			// Set the "after" MD5 hash.
			if (lpstNAVAPI_Config->dwOptions & MD5_HASH)
			{
				GetMD5Hash(lpszFilePath, sScanResults.szMD5HashAfter);
			}
			else
			{
				strcpy(sScanResults.szMD5HashAfter, "N/A");
			}

			// Print and log output.
			if (!lpstNAVAPI_Config->bTabMode)
			{
				// Normal mode.
				sprintf(szOutput,
						"Scanning file...\n%s\nScan Result Code: %s\n%s, Virus Name: %s\n%s, %s, Result Code: %s\nScan Time: %s\nMD5 Hash after: %s\nScanned: %d   Infected: %d   Repaired: %d   Deleted: %d\n\n",
						sScanResults.szFilePath,
						sScanResults.szResultCode,
						sScanResults.szIsInfected,
						sScanResults.szVirusName,
						sScanResults.szRepOrDel,
						sScanResults.szRepOrDelResult,
						sScanResults.szRepOrDelResCode,
						sScanResults.szScanTime,
						sScanResults.szMD5HashAfter,
						sScanResults.unScanned,
						sScanResults.unInfected,
						sScanResults.unRepaired,
						sScanResults.unDeleted);
			}
			else
			{
				// Tabbed output mode.
				sprintf(szOutput, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%lu;%lu;%lu;%lu\n",
						sScanResults.szFilePath,
						sScanResults.szResultCode,
						sScanResults.szIsInfected,
						sScanResults.szVirusName,
						sScanResults.szRepOrDel,
						sScanResults.szRepOrDelResult,
						sScanResults.szRepOrDelResCode,
						sScanResults.szScanTime,
						sScanResults.szMD5HashAfter,
						sScanResults.unScanned,
						sScanResults.unInfected,
						sScanResults.unRepaired,
						sScanResults.unDeleted);
			}
			PrintAndLog(szOutput, lpstNAVAPI_Config);

			return;
		}

			
		if (lpstNAVAPI_Config->dwOptions & REPAIR_VIRUS)
		{
            // Earlier, the defs were queried to see if the proper repair for
            // a file was to delete it (the DELETE_ITEM flag). If that was set,
            // instead of attempting a repair, we just try to delete the file.

            if (bDeleteItem == TRUE)
            {
                // Set if a repair or delete was attempted.
		        strcpy(sScanResults.szRepOrDel, "Repair attempted");

                // Delete the file

                if (unlink(lpszFilePath) != 0)
                {
				    strcpy(sScanResults.szRepOrDelResult, "Failed");
				    strcpy(sScanResults.szRepOrDelResCode, "N/A");
                }
                else
                {
                    strcpy(sScanResults.szRepOrDelResult, "Success");
                    strcpy(sScanResults.szRepOrDelResCode, "N/A");
                    
				    // Increment repaired count
				    sLockMutex(&MutexScannedCounter);
				    gdwTotalRepaired++;
                    sUnLockMutex(&MutexScannedCounter);
                }
            }
            else    // Repair item
            {
		    	// Set if a repair or delete was attempted.
	    		strcpy(sScanResults.szRepOrDel, "Repair attempted");

                if((NAVStatus = NAVRepairFile (lpstNAVAPI_Config->hNAVEngine,
					                        lpszFilePath,
										    szExt)) != NAV_OK )
			    {
				    strcpy(sScanResults.szRepOrDelResult, "Failed");
				    strcpy(sScanResults.szRepOrDelResCode, gNAVAPIReturnValues[NAVStatus]);
			    }
			    else
			    {
				    strcpy(sScanResults.szRepOrDelResult, "Success");
				    strcpy(sScanResults.szRepOrDelResCode, gNAVAPIReturnValues[NAVStatus]);

				    // Increment repaired count
				    sLockMutex(&MutexScannedCounter);
				    gdwTotalRepaired++;
                    sUnLockMutex(&MutexScannedCounter);
        	    }
            }


        #if defined(SYM_NLM)
            ThreadSwitchWithDelay();
        #endif

            
		}
		else if (lpstNAVAPI_Config->dwOptions & DELETE_VIRUS)
		{
			// Set if a repair or delete was attempted.
			strcpy(sScanResults.szRepOrDel, "Delete attempted");

			if ((NAVStatus = NAVDeleteFile (lpstNAVAPI_Config->hNAVEngine,
					                        lpszFilePath,
											szExt)) != NAV_OK )
			{
				strcpy(sScanResults.szRepOrDelResult, "Failed");
				strcpy(sScanResults.szRepOrDelResCode, gNAVAPIReturnValues[NAVStatus]);
			}
			else
			{
				strcpy(sScanResults.szRepOrDelResult, "Success");
				strcpy(sScanResults.szRepOrDelResCode, gNAVAPIReturnValues[NAVStatus]);

				// Increment deleted count
				sLockMutex(&MutexScannedCounter);
				gdwTotalDeleted++;
                sUnLockMutex(&MutexScannedCounter);
			}

        #if defined(SYM_NLM)
            ThreadSwitchWithDelay();
        #endif

		}

		if (lpstNAVAPI_Config->dwOptions & FULL_VIRUS_LOG)
		{
			WriteLog (lpstNAVAPI_Config->szReportLogFile, 
					  "%s%s",
					  szFullLogMarker,
					  szFullVirusInfo);
		}
	}
	else // if file is clean go here
	{

		// Set virus infection state.
		strcpy(sScanResults.szIsInfected, "Clean");
	}

#ifdef SYM_WIN32
	// Run NAVIsInfectableFile on this file if the Full Log option (/flog)
	// is specified on the command line.
	if (lpstNAVAPI_Config->dwOptions & FULL_VIRUS_LOG)
	{
		NAVIsInfectableFile(lpstNAVAPI_Config->hNAVEngine,
								lpszFilePath,
								szExt,
								TRUE,
								&bInfectable);
		if (bInfectable)
		{
			PrintAndLog("NAVIsInfectableFile() returned true.\n", lpstNAVAPI_Config);
		}
		else
		{
			PrintAndLog("NAVIsInfectableFile() returned false.\n", lpstNAVAPI_Config);
		}
	}
#endif // #ifdef SYM_WIN32

	// Returning, so set total scanned statistics.
	sScanResults.unScanned = gdwTotalScanned;
	sScanResults.unInfected = gdwTotalInfected;
	sScanResults.unRepaired = gdwTotalRepaired;
	sScanResults.unDeleted = gdwTotalDeleted;

	// Set the "after" MD5 hash.
	if (lpstNAVAPI_Config->dwOptions & MD5_HASH)
	{
		GetMD5Hash(lpszFilePath, sScanResults.szMD5HashAfter);
	}
	else
	{
		strcpy(sScanResults.szMD5HashAfter, "N/A");
	}

	// Print and log output.
	if (!lpstNAVAPI_Config->bTabMode)
	{
		// Normal mode.
		sprintf(szOutput,
				"Scanning file...\n%s\nScan Result Code: %s\n%s, Virus Name: %s\n%s, %s, Result Code: %s\nScan Time: %s\nMD5 Hash after: %s\nScanned: %d   Infected: %d   Repaired: %d   Deleted: %d\n\n",
				sScanResults.szFilePath,
				sScanResults.szResultCode,
				sScanResults.szIsInfected,
				sScanResults.szVirusName,
				sScanResults.szRepOrDel,
				sScanResults.szRepOrDelResult,
				sScanResults.szRepOrDelResCode,
				sScanResults.szScanTime,
				sScanResults.szMD5HashAfter,
				sScanResults.unScanned,
				sScanResults.unInfected,
				sScanResults.unRepaired,
				sScanResults.unDeleted);
	}
	else
	{
		// Tabbed output mode.
		sprintf(szOutput, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%lu;%lu;%lu;%lu\n",
				sScanResults.szFilePath,
				sScanResults.szResultCode,
				sScanResults.szIsInfected,
				sScanResults.szVirusName,
				sScanResults.szRepOrDel,
				sScanResults.szRepOrDelResult,
				sScanResults.szRepOrDelResCode,
				sScanResults.szScanTime,
				sScanResults.szMD5HashAfter,
				sScanResults.unScanned,
				sScanResults.unInfected,
				sScanResults.unRepaired,
				sScanResults.unDeleted);
	}
	PrintAndLog(szOutput, lpstNAVAPI_Config);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:1
//		void SetLimitOpenFilePerProcess()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void SetLimitOpenFilePerProcess ( )
{

#ifdef SYM_UNIX
    struct rlimit limit;

    if ( getrlimit ( RLIMIT_NOFILE, &limit ) ) {
        printf ("Error: failed to get current open file per process limit\r\n" );
    }else
    {
        limit.rlim_cur = limit.rlim_max;
        if ( setrlimit ( RLIMIT_NOFILE, &limit ) ) {
            printf ("Error: failed to set current open file per process limit\r\n" );
        }
    }

#endif
    
    return;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void thread()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
#if defined(SYM_WIN32)
void __cdecl
#elif defined(SYM_UNIX)
void*
#endif
thread ( LPVOID ptr )
{

    lpThreadParameter ThreadPara = (lpThreadParameter) ptr;

    ProcessFile( &ThreadPara->stNAVAPI_Config, ThreadPara->szFileName);

    if ( ThreadPara )
    {
        free ( ThreadPara );
        ThreadPara = NULL;
    }

    sLockMutex ( &MutexThreadCounter );
	{
		gdwThreadCount--;
	}
    sUnLockMutex ( &MutexThreadCounter );

	sExitThread ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void CallThread()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void CallThread 
( 
	LPNAVAPI_CONFIG	lpstNAVAPI_Config,
	LPSTR			lpszScanFile 
)
{
    lpThreadParameter Threadpara;

    while ( 1 )
    {
        if ( gdwThreadCount < MAX_THREAD_COUNT )
        {
            if ( ( Threadpara = ( lpThreadParameter ) malloc ( sizeof( ThreadParameter ) ) ) == NULL ) {
                printf ("Error: Allocating memory for %s\r\n", lpszScanFile );
                return;
            }
            // protect are thread counter
            sLockMutex ( &MutexThreadCounter );
			{
				gdwThreadCount++;
			}
            sUnLockMutex ( &MutexThreadCounter );

            strcpy( Threadpara->szFileName, lpszScanFile );
            Threadpara->stNAVAPI_Config = *lpstNAVAPI_Config;
            gThreadHandle++;
            Threadpara->dwIndex = gThreadHandle;

            if ( sCreateThread ( (LPVOID)thread, Threadpara, &pthread_attr ) == (SHANDLE)NULL )
            {
                printf ( "Error: failed to create thread : Handel %08X : Thread count %08X\r\n", gThreadHandle, gdwThreadCount );
                if ( Threadpara )
                {
                    free ( Threadpara );
                    Threadpara = NULL;
                }
                sLockMutex ( &MutexThreadCounter );
                {
                    gdwThreadCount--;
                }
                sUnLockMutex ( &MutexThreadCounter );
            }
            break;
        } else sSleep (15);
    }
}



///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanFileHelp ()
//
//	Description:
//		Displays File scan help screen
//
//	Parameters:
//		NONE
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void ScanFileHelp (void)
{
	printf("Options:\n");
	printf("   /?               Displays this screen\n");
	printf("   /DEFS=           Specifies the virus defs location\n");
	printf("   /HEUR=           Heuristics level (Default is 0)\n");
	printf("   /INF=            Full path to INF file\n");
	printf("   /FILE=           Single file to scan\n");
	printf("   /DIR=            Directory to scan\n");
	printf("   /REP             Repair automatically if infected\n");
	printf("   /DEL             Delete automatically if infected\n");

#if defined (SYM_WIN32) || defined (SYM_UNIX)
    printf("   /MT              Multi threaded scanning enabled\n");
#endif
	printf("   /INFO            Info function tests\n");
	printf("   /ITERATIONS=     Times to repeat each scan (Default 1, 0 infinite)\n");
	printf("   /TOTALITERATIONS=Times to repeat total scan (Default 1, 0 infinite)\n");
   	printf("   /PLOG=           Partial log report\n");
  	printf("   /FLOG=           Full log report\n");
	printf("   /MD5             Generate MD5 hash after scan/repair/delete\n");
	printf("   /TAB             Semicolon delimited output mode (/plog only)\n");
}
