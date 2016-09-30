#include "scanboot.h"
#include "log.h"


///////////////////////////////////////////////////////////////////////////////
// If load virus hash table succeeded.
///////////////////////////////////////////////////////////////////////////////
static BOOL bVirusHashTableLoadSucceed;


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanBoot ()
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
void ScanBoot (int argc, char* argv[])
{
	SCANBOOTOPTS	stScanBootOpts	= {0};
	NAVAPI_CONFIG	stNAVAPI_Config = {0};
	HNAVVIRUS		hVirusInfo = {0};
	NAVSTATUS		NAVStatus = 0;
	char			szVirusName[30];
	char			szFullVirusInfo[MAX_GET_VIRUS_INFO_SIZE] = {0};
	char			szDefInfo[1024] = {0};
	char			szDetectInfo[1024] = {0};
	char			szRepairInfo[1024] = {0};
	char			szVirTableComparisionInfo[32768] = {0};
	const char		szFullLogMarker[] = {"-------------------------------------------------------------------------------\n"};
	char			szTempStrBuf[4096];
	int				nIndex				= 0;
	LPVIRTABLE		lpVirTable = NULL;
	LPVIRTABLE		lpVirTableDetected = NULL;
	char			szNavInfo[512]		= {0};
	DWORD			dwVirusID;
	DWORD			dwInfoSize;
	unsigned int	unScanIndex;

	// Get boot scan options
	if (!GetScanBootOptions(&stScanBootOpts, argc, argv))
	{
		ScanBootHelp();
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// Initialize NAVAPI configuration structure
	///////////////////////////////////////////////////////////////////////////
	stNAVAPI_Config.dwOptions = stScanBootOpts.dwOptions;
	strcpy (stNAVAPI_Config.szDefsPath, stScanBootOpts.szDefLoc);
	strcpy (stNAVAPI_Config.szNavexInfFile, stScanBootOpts.szNavexInfFile);
	strcpy (stNAVAPI_Config.szReportLogFile, stScanBootOpts.szLogLoc);
	stNAVAPI_Config.wHeuristicLevel = stScanBootOpts.wHeuristicLevel;
	stNAVAPI_Config.dwNumOfTimesToRepeatScan = stScanBootOpts.dwIterations;

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
	NAVVIRUSDBINFOEX sVirusDBInfoEx;

	sVirusDBInfoEx.dwSize = sizeof (sVirusDBInfoEx);

	if (stNAVAPI_Config.dwNumOfTimesToRepeatScan != 0)
	{
		NAVStatus = NAVGetVirusDBInfoEx(stNAVAPI_Config.hNAVEngine, &sVirusDBInfoEx);

		sprintf(szDefInfo, "Using virus definitions Version %ld, %d/%d/%d, sequence %ld.\n",
			sVirusDBInfoEx.dwVersion, (int) sVirusDBInfoEx.wMonth, (int) sVirusDBInfoEx.wDay,
			(int) sVirusDBInfoEx.wYear, sVirusDBInfoEx.dwSequence);
		PrintAndLog(szDefInfo, &stNAVAPI_Config);
	}

	// Load virus hash table
	if (stNAVAPI_Config.dwOptions & INFO_TEST)
	{
		bVirusHashTableLoadSucceed = InitializeVirusHashTable(stNAVAPI_Config);
	}

	///////////////////////////////////////////////////////////////////////////
	// Scan boot record
	///////////////////////////////////////////////////////////////////////////
	if (stNAVAPI_Config.dwNumOfTimesToRepeatScan == 0)
	{
		while(true)
		{
			// Go into infinite loop, continuously scanning boot record.
			NAVScanBoot (stNAVAPI_Config.hNAVEngine, 
	 	                          stScanBootOpts.szDriveLetter[0],
								  &hVirusInfo);
			PrintAndLog (".", &stNAVAPI_Config);
		}
	}
	
	for (unScanIndex=0; unScanIndex < stNAVAPI_Config.dwNumOfTimesToRepeatScan; unScanIndex++)
	{
		NAVStatus = NAVScanBoot (stNAVAPI_Config.hNAVEngine, 
	 	                          stScanBootOpts.szDriveLetter[0],
								  &hVirusInfo);
	}
	if (NAVStatus != NAV_OK) 
	{
		sprintf (szDetectInfo, 
   		         "Scanning boot record of drive %c:..\nNAVAPI ERROR CODE: %s\n",
				 stScanBootOpts.szDriveLetter[0],
				 gNAVAPIReturnValues[NAVStatus]);


		if ((stNAVAPI_Config.dwOptions & PARTIAL_VIRUS_LOG) || (stNAVAPI_Config.dwOptions & FULL_VIRUS_LOG))
		{
			WriteLog (stNAVAPI_Config.szReportLogFile, 
			          "%s\n",
			          szDetectInfo);
		}

		printf(szDetectInfo);
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// Check if virus was found
	///////////////////////////////////////////////////////////////////////////
	if (IsInfected (hVirusInfo))
	{
		// Get Virus name
		GetVirusName (hVirusInfo, szVirusName, sizeof(szVirusName));
		sprintf (szDetectInfo, 
			     "Scanning boot record of drive %c:...\nBoot record is infected with the \"%s\" virus!\n",
				 stScanBootOpts.szDriveLetter[0], 
				 szVirusName);
		printf(szDetectInfo);

		// Get Virus ID
		dwInfoSize = sizeof(szNavInfo);
		if ((NAVStatus = NAVGetVirusInfo(hVirusInfo, NAV_VI_VIRUS_ID, szNavInfo, &dwInfoSize)) != NAV_OK)
		{
			sprintf(szTempStrBuf, "NAVGetVirusDefInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
			PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
			exit(1);
		}
		sscanf(szNavInfo, "%ld", &dwVirusID);

		///////////////////////////////////////////////////////////////////////
		// Get all virus info
		///////////////////////////////////////////////////////////////////////
		if (stNAVAPI_Config.dwOptions & FULL_VIRUS_LOG)
		{
			GetFullVirusInfo(hVirusInfo, szFullVirusInfo, sizeof(szFullVirusInfo));
		}

		///////////////////////////////////////////////////////////////////////
		// Info test, part one. Compare item specific virus info with data
		// returned from the virus table API functions.
		///////////////////////////////////////////////////////////////////////
		if ((stNAVAPI_Config.dwOptions & INFO_TEST) && bVirusHashTableLoadSucceed)
		{
			lpVirTableDetected = new VIRTABLE;
			CopyHNAVVIRUStoLPVIRTABLE(hVirusInfo, lpVirTableDetected);

			lpVirTable = GetVirusHashTableEntry(szVirusName);

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
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusAlias, lpVirTableDetected->szVirusAlias) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefTableInfo returned \"%s\", for NAV_VI_VIRUS_ALIAS.\n",
					lpVirTableDetected->szVirusAlias, 
					lpVirTable->szVirusAlias, nIndex);
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusInfo, lpVirTableDetected->szVirusInfo) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefTableInfo returned \"%s\", for NAV_VI_VIRUS_INFO.\n",
					lpVirTableDetected->szVirusInfo, 
					lpVirTable->szVirusInfo, nIndex);
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusSize, lpVirTableDetected->szVirusSize) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefTableInfo returned \"%s\", for NAV_VI_VIRUS_SIZE.\n",
					lpVirTableDetected->szVirusSize, 
					lpVirTable->szVirusSize, nIndex);
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
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
					PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
				}
			}
			FreeVirTable(lpVirTableDetected);
		}
		///////////////////////////////////////////////////////////////////////
		// Info test, part two. Compare item specific virus info with data
		// returned from the load virus API functions.
		///////////////////////////////////////////////////////////////////////
		if (stNAVAPI_Config.dwOptions & INFO_TEST)
		{
			lpVirTableDetected = new VIRTABLE;
			CopyHNAVVIRUStoLPVIRTABLE(hVirusInfo, lpVirTableDetected);

			lpVirTable = new VIRTABLE;
			GetFullVirusInfoLoadSingleVirus(stNAVAPI_Config, dwVirusID, lpVirTable);

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
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusAlias, lpVirTableDetected->szVirusAlias) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefInfo returned \"%s\", for NAV_VI_VIRUS_ALIAS.\n",
					lpVirTableDetected->szVirusAlias, 
					lpVirTable->szVirusAlias, nIndex);
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusInfo, lpVirTableDetected->szVirusInfo) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefInfo returned \"%s\", for NAV_VI_VIRUS_INFO.\n",
					lpVirTableDetected->szVirusInfo, 
					lpVirTable->szVirusInfo, nIndex);
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
			}
			if (strcmp(lpVirTable->szVirusSize, lpVirTableDetected->szVirusSize) != 0)
			{
				sprintf (szVirTableComparisionInfo,
					"ERROR: NAVGetVirusInfo returned \"%s\", NAVGetVirusDefInfo returned \"%s\", for NAV_VI_VIRUS_SIZE.\n",
					lpVirTableDetected->szVirusSize, 
					lpVirTable->szVirusSize, nIndex);
				PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
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
					PrintAndLog(szVirTableComparisionInfo, &stNAVAPI_Config);
				}
			}
			FreeVirTable(lpVirTableDetected);
			FreeVirTable(lpVirTable);
		}

		///////////////////////////////////////////////////////////////////////////
		// Free virus handle
		///////////////////////////////////////////////////////////////////////////
		if ((NAVStatus = NAVFreeVirusHandle(hVirusInfo)) != NAV_OK)
		{
			printf("%s\n", gNAVAPIReturnValues[NAVStatus]);
			return;
		}

		
		// Check if options to determine whether to repair
		if (stNAVAPI_Config.dwOptions & REPAIR_VIRUS)
		{
			// Attempt to repair boot record
			if ((NAVStatus = NAVRepairBoot (stNAVAPI_Config.hNAVEngine, 
	 									    stScanBootOpts.szDriveLetter[0])) != NAV_OK) 
			{
				sprintf (szRepairInfo, 
					     "Attempting to repair boot record...\nBoot record of was NOT repaired\nNAVAPI ERROR CODE: %s\n",
						  gNAVAPIReturnValues[NAVStatus]);

				printf(szRepairInfo);
			}
			else
			{
				sprintf(szRepairInfo, "Attempting to repair boot record...\nBoot record was repaired successfully!\n");
				printf(szRepairInfo);
			}

		}

		// Write to report log if requested
		if(stNAVAPI_Config.dwOptions & PARTIAL_VIRUS_LOG)
		{
			WriteLog (stNAVAPI_Config.szReportLogFile, 
				      "%s%s\n",
					  szDetectInfo,
					  szRepairInfo);
		}
		else if(stNAVAPI_Config.dwOptions & FULL_VIRUS_LOG)
		{
			WriteLog (stNAVAPI_Config.szReportLogFile, 
				      "%s%s%s%s\n",
					  szDetectInfo,
					  szRepairInfo,
					  szFullLogMarker,
					  szFullVirusInfo);			
		}

	}
	else
	{

		sprintf(szDetectInfo, "Scanning boot record of drive %c:...\nBoot record is clean!\n",stScanBootOpts.szDriveLetter[0]);
		PrintAndLog(szDetectInfo, &stNAVAPI_Config);
	}


	///////////////////////////////////////////////////////////////////////////
	// Close NAVAPI engine
	///////////////////////////////////////////////////////////////////////////
	if ((NAVStatus = NAVEngineClose (stNAVAPI_Config.hNAVEngine)) != NAV_OK)
	{
		printf("NAVAPI ERROR CODE: %s\n", gNAVAPIReturnValues[NAVStatus]);
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL GetBootScanOption()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetScanBootOptions 
(
	LPSCANBOOTOPTS	lpstScanBootOpts,
	int				argc,
	char*			argv[]
)
{
	char	szHeurLevel[10] = {0};
	char	szIterations[10] = {0};

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
	if (!GetCmdLineOption("/DEFS=", lpstScanBootOpts->szDefLoc, argc, argv ) )
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get drive letter to scan
	///////////////////////////////////////////////////////////////////////////
	if (!GetCmdLineOption("/DRV=", lpstScanBootOpts->szDriveLetter, argc, argv ) )
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get heuristics level
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/HEUR=", szHeurLevel, argc, argv ) )
	{
		lpstScanBootOpts->wHeuristicLevel = atoi(szHeurLevel);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get number of iterations
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/ITERATIONS=", szIterations, argc, argv ) )
	{
		lpstScanBootOpts->dwIterations = atoi(szIterations);
		if (lpstScanBootOpts->dwIterations < 0 || lpstScanBootOpts->dwIterations >= 100000000)
		{
			printf("Error: /ITERATIONS= must be set to a value zero or greater and less than 1,000,000.\n");
			exit(1);
		}
	}
	else
	{
		// Default to scanning once per file.
		lpstScanBootOpts->dwIterations = 1;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get NAVEX15.INF full path
	///////////////////////////////////////////////////////////////////////////
	GetCmdLineOption("/INF=", lpstScanBootOpts->szNavexInfFile, argc, argv );
	

	///////////////////////////////////////////////////////////////////////////
	// Get log report file name and setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/PLOG=", lpstScanBootOpts->szLogLoc, argc, argv ) )
	{
		lpstScanBootOpts->dwOptions += PARTIAL_VIRUS_LOG;
	}
	else if (GetCmdLineOption("/FLOG=", lpstScanBootOpts->szLogLoc, argc, argv ) )
	{
		lpstScanBootOpts->dwOptions += FULL_VIRUS_LOG;
	}
	

	///////////////////////////////////////////////////////////////////////////
	// Get info test setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/INFO", NULL, argc, argv ) )
	{
		lpstScanBootOpts->dwOptions += INFO_TEST;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get repair setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/REP", NULL, argc, argv ) )
	{
		lpstScanBootOpts->dwOptions +=  REPAIR_VIRUS;
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate definition directory path
	///////////////////////////////////////////////////////////////////////////
	if (lpstScanBootOpts->szDefLoc[0] == NULL)
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate drive letter
	///////////////////////////////////////////////////////////////////////////
	if (lpstScanBootOpts->szDriveLetter[0] == NULL)
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate log option
	///////////////////////////////////////////////////////////////////////////
	if (((lpstScanBootOpts->dwOptions & PARTIAL_VIRUS_LOG) ||
		(lpstScanBootOpts->dwOptions & FULL_VIRUS_LOG)) &&
		(lpstScanBootOpts->szLogLoc[0] == NULL))
	{
		return (FALSE);
	}
		

	return (TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanBootHelp ()
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void ScanBootHelp (void)
{
	printf("Options:\n");
	printf("   /?               Displays this screen\n");
	printf("   /DEFS=           Specifies the virus defs location\n");
	printf("   /HEUR=           Heuristics level (Defualt is 0)\n");
	printf("   /INF=            Full path to INF file\n");
	printf("   /DRV=            Drive letter to scan\n");
	printf("   /REP             Repair automaticly\n");
	printf("   /INFO            Info function tests\n");
	printf("   /ITERATIONS=     Times to repeat each scan (Default is 1)\n");
   	printf("   /PLOG=           Partial log report\n");
  	printf("   /FLOG=           Full log report\n");
}