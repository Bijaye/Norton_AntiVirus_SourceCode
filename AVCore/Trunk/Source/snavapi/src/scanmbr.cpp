#include "scanmbr.h"
#include "log.h"


///////////////////////////////////////////////////////////////////////////////
// If load virus hash table succeeded.
///////////////////////////////////////////////////////////////////////////////
static BOOL bVirusHashTableLoadSucceed;


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanMBR()
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
void ScanMBR (int argc, char* argv[])
{
	SCANMBROPTS		stScanMBROpts	= {0};
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
	if (!GetScanMBROptions(&stScanMBROpts, argc, argv))
	{
		ScanMBRHelp();
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// Initialize NAVAPI configuration structure
	///////////////////////////////////////////////////////////////////////////
	stNAVAPI_Config.dwOptions = stScanMBROpts.dwOptions;
	strcpy (stNAVAPI_Config.szDefsPath, stScanMBROpts.szDefLoc);
	strcpy (stNAVAPI_Config.szNavexInfFile, stScanMBROpts.szNavexInfFile);
	strcpy (stNAVAPI_Config.szReportLogFile, stScanMBROpts.szLogLoc);
	stNAVAPI_Config.wHeuristicLevel = stScanMBROpts.wHeuristicLevel;
	stNAVAPI_Config.dwNumOfTimesToRepeatScan = stScanMBROpts.dwIterations;


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
	// Scan master boot record
	///////////////////////////////////////////////////////////////////////////
	if (stNAVAPI_Config.dwNumOfTimesToRepeatScan == 0)
	{
		while(true)
		{
			// Go into infinite loop, continuously scanning boot record.
			NAVScanMasterBoot (stNAVAPI_Config.hNAVEngine, 
	 	                                stScanMBROpts.uPhysDriveNum,
								        &hVirusInfo);
			PrintAndLog (".", &stNAVAPI_Config);
		}
	}
	
	for (unScanIndex=0; unScanIndex < stNAVAPI_Config.dwNumOfTimesToRepeatScan; unScanIndex++)
	{
		NAVStatus = NAVScanMasterBoot (stNAVAPI_Config.hNAVEngine, 
	 	                                stScanMBROpts.uPhysDriveNum,
								        &hVirusInfo);
	}
	if (NAVStatus != NAV_OK) 
	{
		sprintf (szDetectInfo, 
   		         "Scanning master boot record of physical drive 0x%x...\nNAVAPI ERROR CODE: %s\nWhile scanning master boot record\n",
				 stScanMBROpts.uPhysDriveNum,
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
			     "Scanning master boot record of physical drive 0x%x...\nMaster boot record is infected with the \"%s\" virus!\n",
				 stScanMBROpts.uPhysDriveNum, 
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
			if ((NAVStatus = NAVRepairMasterBoot (stNAVAPI_Config.hNAVEngine, 
	 									          stScanMBROpts.uPhysDriveNum)) != NAV_OK) 
			{
				sprintf (szRepairInfo, 
					     "Attempting to repair...\nMaster boot record was NOT repaired\nNAVAPI ERROR CODE: %s\n",
						 gNAVAPIReturnValues[NAVStatus]);

				printf(szRepairInfo);
				return;
			}
			else
			{
				sprintf(szRepairInfo, "Attempting to repair...\nMaster boot record was repaired successfully!\n");
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

		sprintf(szDetectInfo, "Scanning master boot record of physical drive 0x%x...\nMaster boot record is clean!\n", stScanMBROpts.uPhysDriveNum);
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
//		BOOL GetScanMBROptions()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetScanMBROptions 
(
	LPSCANMBROPTS	lpstScanMBROpts,
	int				argc,
	char*			argv[]
)
{
	char	szHeurLevel[10] = {0};
	char	szIterations[10] = {0};
	char	szPhysDrive[6] = {0};

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
	if (!GetCmdLineOption("/DEFS=", lpstScanMBROpts->szDefLoc, argc, argv ) )
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get drive letter to scan
	///////////////////////////////////////////////////////////////////////////
	if (!GetCmdLineOption("/DRV=", szPhysDrive, argc, argv ) )
	{
		return (FALSE);
	}
	else
	{
		lpstScanMBROpts->uPhysDriveNum = StrToHex (szPhysDrive);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get heuristics level
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/HEUR=", szHeurLevel, argc, argv ) )
	{
		lpstScanMBROpts->wHeuristicLevel = atoi(szHeurLevel);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get number of iterations
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/ITERATIONS=", szIterations, argc, argv ) )
	{
		lpstScanMBROpts->dwIterations = atoi(szIterations);
		if (lpstScanMBROpts->dwIterations < 0 || lpstScanMBROpts->dwIterations >= 100000000)
		{
			printf("Error: /ITERATIONS= must be set to a value zero or greater and less than 1,000,000.\n");
			exit(1);
		}
	}
	else
	{
		// Default to scanning once per file.
		lpstScanMBROpts->dwIterations = 1;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get NAVEX15.INF full path
	///////////////////////////////////////////////////////////////////////////
	GetCmdLineOption("/INF=", lpstScanMBROpts->szNavexInfFile, argc, argv );
	

	///////////////////////////////////////////////////////////////////////////
	// Get log report file name and setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/PLOG=", lpstScanMBROpts->szLogLoc, argc, argv ) )
	{
		lpstScanMBROpts->dwOptions += PARTIAL_VIRUS_LOG;
	}
	else if (GetCmdLineOption("/FLOG=", lpstScanMBROpts->szLogLoc, argc, argv ) )
	{
		lpstScanMBROpts->dwOptions += FULL_VIRUS_LOG;
	}
	

	///////////////////////////////////////////////////////////////////////////
	// Get info test setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/INFO", NULL, argc, argv ) )
	{
		lpstScanMBROpts->dwOptions += INFO_TEST;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get repair setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/REP", NULL, argc, argv ) )
	{
		lpstScanMBROpts->dwOptions +=  REPAIR_VIRUS;
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate definition directory path
	///////////////////////////////////////////////////////////////////////////
	if (lpstScanMBROpts->szDefLoc[0] == NULL)
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate drive letter
	///////////////////////////////////////////////////////////////////////////
	if (lpstScanMBROpts->uPhysDriveNum < 0x80)
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate log option
	///////////////////////////////////////////////////////////////////////////
	if (((lpstScanMBROpts->dwOptions & PARTIAL_VIRUS_LOG) ||
		(lpstScanMBROpts->dwOptions & FULL_VIRUS_LOG)) &&
		(lpstScanMBROpts->szLogLoc[0] == NULL))
	{
		return (FALSE);
	}
		

	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanMBRHelp ()
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void ScanMBRHelp (void)
{
	printf("Options:\n");
	printf("   /?               Displays this screen\n");
	printf("   /DEFS=           Specifies the virus defs location\n");
	printf("   /HEUR=           Heuristics level (Defualt is 0)\n");
	printf("   /INF=            Full path to INF file\n");
	printf("   /DRV=            Physical Drive number (C=0x80, D=0x81, etc...)\n");
	printf("   /REP             Repair automaticly\n");
	printf("   /INFO            Info function tests\n");
   	printf("   /PLOG=           Partial log report\n");
  	printf("   /FLOG=           Full log report\n");
}