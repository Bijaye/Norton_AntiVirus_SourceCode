#include "scanmem.h"
#include "log.h"


///////////////////////////////////////////////////////////////////////////////
// If load virus hash table succeeded.
///////////////////////////////////////////////////////////////////////////////
static BOOL bVirusHashTableLoadSucceed;


void ScanMemory(int argc, char* argv[])
{
	SCANMEMOPTS		stScanMemOpts	= {0};
	NAVAPI_CONFIG	stNAVAPI_Config = {0};
	HNAVVIRUS		hVirusInfo = {0};
	NAVSTATUS		NAVStatus = 0;
	char			szVirusName[30];
	char			szFullVirusInfo[MAX_GET_VIRUS_INFO_SIZE] = {0};
	char			szDetectInfo[1024] = {0};
	const char		szFullLogMarker[] = {"-------------------------------------------------------------------------------\n"};
	char			szDefInfo[1024] = {0};
	char			szVirTableComparisionInfo[32768] = {0};
	char			szTempStrBuf[4096];
	int				nIndex				= 0;
	LPVIRTABLE		lpVirTable = NULL;
	LPVIRTABLE		lpVirTableDetected = NULL;
	char			szNavInfo[512]		= {0};
	DWORD			dwVirusID;
	DWORD			dwInfoSize;

	// Get boot scan options
	if (!GetScanMemoryOptions(&stScanMemOpts, argc, argv))
	{
		ScanMemoryHelp();
		return;
	}


	///////////////////////////////////////////////////////////////////////////
	// Initialize NAVAPI configuration structure
	///////////////////////////////////////////////////////////////////////////
	stNAVAPI_Config.dwOptions = stScanMemOpts.dwOptions;
	strcpy (stNAVAPI_Config.szDefsPath, stScanMemOpts.szDefLoc);
	strcpy (stNAVAPI_Config.szNavexInfFile, stScanMemOpts.szNavexInfFile);
	strcpy (stNAVAPI_Config.szReportLogFile, stScanMemOpts.szLogLoc);
	stNAVAPI_Config.wHeuristicLevel = stScanMemOpts.wHeuristicLevel;


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
	
	///////////////////////////////////////////////////////////////////////////
	// Scan memory
	///////////////////////////////////////////////////////////////////////////
	if ((NAVStatus = NAVScanMemory (stNAVAPI_Config.hNAVEngine, 
	 	                            &hVirusInfo)) != NAV_OK) 
	{
		sprintf (szDetectInfo, 
   		         "Scanning memory...\nNAVAPI ERROR CODE: %s\n",
				 gNAVAPIReturnValues[NAVStatus]);

		PrintAndLog(szDetectInfo, &stNAVAPI_Config);

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
			     "Scanning memory...\nMemory is infected with the \"%s\" virus!\n",
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


		// Write to report log if requested
		if(stNAVAPI_Config.dwOptions & PARTIAL_VIRUS_LOG)
		{
			WriteLog (stNAVAPI_Config.szReportLogFile, 
				      "%s\n",
					   szDetectInfo);
		}
		else if(stNAVAPI_Config.dwOptions & FULL_VIRUS_LOG)
		{
			WriteLog (stNAVAPI_Config.szReportLogFile, 
				      "%s%s%s\n",
					  szDetectInfo,
					  szFullLogMarker,
					  szFullVirusInfo);			
		}

	}
	else
	{
		sprintf(szDetectInfo, "Scanning memory...\nMemory is clean!\n");
		PrintAndLog(szDetectInfo, &stNAVAPI_Config);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL GetScanMemoryOptions()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetScanMemoryOptions 
(
	LPSCANMEMOPTS	lpstScanMemOpts,
	int				argc,
	char*			argv[]
)
{
	char	szHeurLevel[10] = {0};

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
	if (!GetCmdLineOption("/DEFS=", lpstScanMemOpts->szDefLoc, argc, argv ) )
	{
		return (FALSE);
	}

	///////////////////////////////////////////////////////////////////////////
	// Get heuristics level
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/HEUR=", szHeurLevel, argc, argv ) )
	{
		lpstScanMemOpts->wHeuristicLevel = atoi(szHeurLevel);
	}


	///////////////////////////////////////////////////////////////////////////
	// Get NAVEX15.INF full path
	///////////////////////////////////////////////////////////////////////////
	GetCmdLineOption("/INF=", lpstScanMemOpts->szNavexInfFile, argc, argv );
	

	///////////////////////////////////////////////////////////////////////////
	// Get log report file name and setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/PLOG=", lpstScanMemOpts->szLogLoc, argc, argv ) )
	{
		lpstScanMemOpts->dwOptions += PARTIAL_VIRUS_LOG;
	}
	else if (GetCmdLineOption("/FLOG=", lpstScanMemOpts->szLogLoc, argc, argv ) )
	{
		lpstScanMemOpts->dwOptions += FULL_VIRUS_LOG;
	}
	

	///////////////////////////////////////////////////////////////////////////
	// Get info test setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/INFO", NULL, argc, argv ) )
	{
		lpstScanMemOpts->dwOptions += INFO_TEST;
	}


	///////////////////////////////////////////////////////////////////////////
	// Get repair setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/REP", NULL, argc, argv ) )
	{
		lpstScanMemOpts->dwOptions +=  REPAIR_VIRUS;
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate definition directory path
	///////////////////////////////////////////////////////////////////////////
	if (lpstScanMemOpts->szDefLoc[0] == NULL)
	{
		return (FALSE);
	}


	///////////////////////////////////////////////////////////////////////////
	// Validate log option
	///////////////////////////////////////////////////////////////////////////
	if (((lpstScanMemOpts->dwOptions & PARTIAL_VIRUS_LOG) ||
		(lpstScanMemOpts->dwOptions & FULL_VIRUS_LOG)) &&
		(lpstScanMemOpts->szLogLoc[0] == NULL))
	{
		return (FALSE);
	}
		

	return (TRUE);
}


void ScanMemoryHelp(void)
{
	printf("Options:\n");
	printf("   /?               Displays this screen\n");
	printf("   /DEFS=           Specifies the virus defs location\n");
	printf("   /HEUR=           Heuristics level (Defualt is 0)\n");
	printf("   /INF=            Full path to INF file\n");
	printf("   /INFO            Info function tests\n");
   	printf("   /PLOG=           Partial log report\n");
  	printf("   /FLOG=           Full log report\n");
}
