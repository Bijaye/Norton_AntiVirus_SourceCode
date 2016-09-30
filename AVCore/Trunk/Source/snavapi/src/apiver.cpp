#include "apiver.h"
#include "spthread.h"
#include "log.h"


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void Version ()
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
void Version (int argc, char* argv[])
{
	char	szVersionNum[80]={0};
	
	NAVGetNavapiVersionString (szVersionNum, sizeof(szVersionNum));
	printf("NAVAPI version %s\n" ,szVersionNum);

}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void InfoTest ()
//
//	Description:
//      Tests the information functions of NAVAPI.
//
//	Parameters:
//		argc
//		argv
//
//	Returns:
//      Outputs results to the stdout and/or the log files.
//
///////////////////////////////////////////////////////////////////////////////
void InfoTest (int argc, char* argv[])
{
	INFOTESTOPTS	stInfoTestOpts	= {0};
	NAVAPI_CONFIG	stNAVAPI_Config = {0};
	NAVSTATUS		NAVStatus = 0;
	TCHAR			szOutputBuffer[2048];
	NAVVIRUSDBINFOEX sVirusDBInfoEx;

	// Get info test options
	if (!GetInfoTestOptions(&stInfoTestOpts, argc, argv))
	{
		InfoTestHelp();
		return;
	}

	// Initialize NAVAPI configuration structure
	stNAVAPI_Config.dwOptions = stInfoTestOpts.dwOptions;
	strcpy (stNAVAPI_Config.szDefsPath, stInfoTestOpts.szDefLoc);
	strcpy (stNAVAPI_Config.szNavexInfFile, stInfoTestOpts.szNavexInfFile);
	strcpy (stNAVAPI_Config.szReportLogFile, stInfoTestOpts.szLogLoc);
	stNAVAPI_Config.wHeuristicLevel = stInfoTestOpts.wHeuristicLevel;

	// Initialize NAVAPI engine
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

	// Print date and sequence number of definitions
	

	sVirusDBInfoEx.dwSize = sizeof (sVirusDBInfoEx);

	NAVStatus = NAVGetVirusDBInfoEx(stNAVAPI_Config.hNAVEngine, &sVirusDBInfoEx);

	sprintf(szOutputBuffer, "Using virus definitions Version %ld, %d/%d/%d, sequence %ld.\n",
		sVirusDBInfoEx.dwVersion, (int) sVirusDBInfoEx.wMonth, (int) sVirusDBInfoEx.wDay,
		(int) sVirusDBInfoEx.wYear, sVirusDBInfoEx.dwSequence);
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);

	///////////////////////////////////////////////////////////////////////////////
	// GetVirusDefTableInfo test
	///////////////////////////////////////////////////////////////////////////////
	InfoTestGetVirusDefTableInfo(stNAVAPI_Config);

	///////////////////////////////////////////////////////////////////////////////
	// NAVGetVirusDefCount, NAVGetVirusDefMaxCount, and
	// NAVGetVirusDefDetectCount test
	///////////////////////////////////////////////////////////////////////////////
	InfoTestMiscInfoAPIs(stNAVAPI_Config);

	// Close NAVAPI engine
	if ((NAVStatus = NAVEngineClose (stNAVAPI_Config.hNAVEngine)) != NAV_OK)
		printf("NAVAPI ERROR CODE: %s\n", gNAVAPIReturnValues[NAVStatus]);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL GetInfoTestOptions()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetInfoTestOptions 
(
	LPINFOTESTOPTS	lpstInfoTestOpts,
	int				argc,
	char*			argv[]
)
{
	char	szHeurLevel[10] = {0};

	///////////////////////////////////////////////////////////////////////////
	// Get command line options
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/?", NULL, argc, argv ) )
		return (FALSE);

	///////////////////////////////////////////////////////////////////////////
	// Get path to virus definitions
	///////////////////////////////////////////////////////////////////////////
	if (!GetCmdLineOption("/DEFS=", lpstInfoTestOpts->szDefLoc, argc, argv ) )
		return (FALSE);

	///////////////////////////////////////////////////////////////////////////
	// Get heuristics level
	///////////////////////////////////////////////////////////////////////////
	if(GetCmdLineOption("/HEUR=", szHeurLevel, argc, argv ) )
		lpstInfoTestOpts->wHeuristicLevel = atoi(szHeurLevel);

	///////////////////////////////////////////////////////////////////////////
	// Get NAVEX15.INF full path
	///////////////////////////////////////////////////////////////////////////
	GetCmdLineOption("/INF=", lpstInfoTestOpts->szNavexInfFile, argc, argv );
	
	///////////////////////////////////////////////////////////////////////////
	// Get log report file name and setting
	///////////////////////////////////////////////////////////////////////////
	if (GetCmdLineOption("/PLOG=", lpstInfoTestOpts->szLogLoc, argc, argv ) )
		lpstInfoTestOpts->dwOptions += PARTIAL_VIRUS_LOG;
	else if (GetCmdLineOption("/FLOG=", lpstInfoTestOpts->szLogLoc, argc, argv ) )
		lpstInfoTestOpts->dwOptions += FULL_VIRUS_LOG;

	///////////////////////////////////////////////////////////////////////////
	// Validate definition directory path
	///////////////////////////////////////////////////////////////////////////
	if (lpstInfoTestOpts->szDefLoc[0] == '\0')
		return (FALSE);

	///////////////////////////////////////////////////////////////////////////
	// Validate log option
	///////////////////////////////////////////////////////////////////////////
	if (((lpstInfoTestOpts->dwOptions & PARTIAL_VIRUS_LOG) ||
		(lpstInfoTestOpts->dwOptions & FULL_VIRUS_LOG)) &&
		(lpstInfoTestOpts->szLogLoc[0] == '\0'))
		return (FALSE);

	return (TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void InfoTestHelp ()
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void InfoTestHelp (void)
{
	printf("Options:\n");
	printf("   /?               Displays this screen\n");
	printf("   /DEFS=           Specifies the virus defs location\n");
	printf("   /HEUR=           Heuristics level (Defualt is 0)\n");
	printf("   /INF=            Full path to INF file\n");
   	printf("   /PLOG=           Partial log report\n");
  	printf("   /FLOG=           Full log report\n");
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void InfoTestGetVirusDefTableInfo ()
//
//	Description:
//		This enumerates over all viruses using the load virus table functions
//		and reports if any of the table's bool values are returning the same
//		value for all virus samples in the table.
//		Tests NAVReleaseVirusDefTable, NAVLoadVirusDefTable, NAVGetVirusDefTableInfo,
//		and NAVGetVirusDefTableCount.
//
//	Parameters:
//		NONE
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void InfoTestGetVirusDefTableInfo(NAVAPI_CONFIG stNAVAPI_Config)
{
	HNAVVIRUSDEFTABLE hTable;
	DWORD             dwBufSize;
	DWORD             dwNumDefs;
	TCHAR             szFoo[256];
	TCHAR			  szOutputBuffer[2048];
	int               nCnt;
	DWORD             u;
	char              sTemp[256];

	// bBoolVals will hold the previous set of BOOL values in bBoolVals[2] and
	// the current set in bBoolVals[1].  bBoolVals[0] will each be set to [1] as
	// a particular value is found to be different at any time.  This is to
	// check to make sure a particular value is not always returning the
	// exact same value.
	// NAV_VI_NON_ITEM_SPECIFIC_LAST is defined in avshared.h.
	BOOL              bBoolVals[3][NAV_VI_NON_ITEM_SPECIFIC_LAST+1];
	int               nTemp1, nTemp2;
	BOOL              bFirst=TRUE;

	sprintf(szOutputBuffer, "\n\nBegin GetVirusDefTableInfo test:\n");
	strcat(szOutputBuffer, "[This enumerates over all viruses using the load virus table functions\n");
	strcat(szOutputBuffer, "and reports if any of the table's bool values are returning the same\n");
	strcat(szOutputBuffer, "value for all virus samples in the table. Uses NAVReleaseVirusDefTable,\n");
	strcat(szOutputBuffer, "NAVLoadVirusDefTable, NAVGetVirusDefTableInfo, and NAVGetVirusDefTableCount.]\n");
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);

	// Initialize the bBoolVals to all zeros.
	for (nTemp1=0; nTemp1<3; nTemp1++)
	   for (nTemp2=0; nTemp2<=(int)NAV_VI_NON_ITEM_SPECIFIC_LAST; nTemp2++)
		  bBoolVals[nTemp1][nTemp2] = 0;

	hTable = NAVLoadVirusDefTable ( stNAVAPI_Config.hNAVEngine );

	if ( !hTable )
	{
		PrintAndLog("NAVLoadVirusDefTable() failed! This function is currently not supported under DOS or UNIX, so it should fail in those cases.\n", &stNAVAPI_Config);
		return;
	}

	if ( NAV_OK != NAVGetVirusDefTableCount ( hTable, &dwNumDefs ) )
	{
		PrintAndLog("NAVGetVirusDefTableCount() failed!\n", &stNAVAPI_Config);
		NAVReleaseVirusDefTable ( hTable );
		exit(1);
	}

	for ( u = 0, nCnt = 0; u < dwNumDefs; u++ )
	{
		if (nCnt == 400)
		{
			printf(".");
			nCnt = 0;
		} else {
			nCnt++;
		}

		dwBufSize = 256;

		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_NAME, sTemp, &dwBufSize );
		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_ALIAS, sTemp, &dwBufSize );
		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_INFO, sTemp, &dwBufSize );
		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_SIZE, sTemp, &dwBufSize );

		for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nTemp1++)
			bBoolVals[1][nTemp1] = 
			   NAVGetVirusDefTableInfo (hTable, u, (NAVVIRINFO) nTemp1, szFoo, &dwBufSize );

		// Now we must note any of the booleans that have changed since the last virus.
		// These changes will be noted in bBoolVals[0].  A '1' will indicate that this item has
		// changed at one point (once it changes to '1', it stays set that way through the
		// entire check).  Only if a value is the same throughout the whole table will a
		// position remain '0' at the end of this for-loop.

		if (TRUE == bFirst)
		   bFirst = FALSE;
		else
		   for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nTemp1++)
			  if (0 == bBoolVals[0][nTemp1])
				 if (bBoolVals[1][nTemp1] != bBoolVals[2][nTemp1])
					bBoolVals[0][nTemp1] = 1;

		// Now we need to copy all of the current values to the "previous" slot
		// in bBoolVals[2].

		for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nTemp1++)
		   bBoolVals[2][nTemp1] = bBoolVals[1][nTemp1];

	}// end for ( u = 0, nCnt = 0; u < dwNumDefs; u++ )

	NAVReleaseVirusDefTable ( hTable );

	printf("\n");

	for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nTemp1++)
	{
	   if (0 == bBoolVals[0][nTemp1])
	   {
			sprintf(szOutputBuffer, "WARNING: GetVirusDefTableInfo item %d returned %d on all viruses.\n",
				nTemp1, bBoolVals[1][nTemp1]);
			PrintAndLog(szOutputBuffer, &stNAVAPI_Config);
	   }
	}

	sprintf(szOutputBuffer, "End GetVirusDefTableInfo test: %ld entries processed.\n", dwNumDefs);
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);
} // void InfoTestGetVirusDefTableInfo(NAVAPI_CONFIG stNAVAPI_Config)


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void InfoTestCompareVirusDefTableInfo ()
//
//	Description:
//		Tests info API functions NAVGetVirusDefCount, NAVGetVirusDefMaxCount,
//		and NAVGetVirusDefDetectCount.
//
//	Parameters:
//		NONE
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void InfoTestMiscInfoAPIs(NAVAPI_CONFIG stNAVAPI_Config)
{
	DWORD dwCount;
	TCHAR			  szOutputBuffer[2048];

	sprintf(szOutputBuffer, "\n\nBegin Misc Info APIs test:\n");
	strcat(szOutputBuffer, "[This tests NAVGetVirusDefCount, NAVGetVirusDefMaxCount, and\n");
	strcat(szOutputBuffer, "NAVGetVirusDefDetectCount. There is no real way to verify these\n");
	strcat(szOutputBuffer, "values except knowing what they should be.]\n");
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);

	// Test NAVGetVirusDefCount
	if ( NAV_OK != NAVGetVirusDefCount ( stNAVAPI_Config.hNAVEngine, &dwCount ) )
	{
		printf ( "ERROR: NAVGetVirusDefCount() failed!\n" );
		return;
	}
	sprintf(szOutputBuffer, "NAVGetVirusDefCount returned %d.\n", dwCount);
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);

	// Test NAVGetVirusDefMaxCount
	if ( NAV_OK != NAVGetVirusDefMaxCount ( stNAVAPI_Config.hNAVEngine, &dwCount ) )
	{
		printf ( "ERROR: NAVGetVirusDefMaxCount() failed!\n" );
		return;
	}
	sprintf(szOutputBuffer, "NAVGetVirusDefMaxCount returned %d.\n", dwCount);
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);

	// Test NAVGetVirusDefDetectCount
	if ( NAV_OK != NAVGetVirusDefDetectCount ( stNAVAPI_Config.hNAVEngine, &dwCount ) )
	{
		printf ( "ERROR: NAVGetVirusDefDetectCount() failed!\n" );
		return;
	}
	sprintf(szOutputBuffer, "NAVGetVirusDefDetectCount returned %d.\n", dwCount);
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);

	sprintf(szOutputBuffer, "End Misc Info APIs test.\n");
	PrintAndLog(szOutputBuffer, &stNAVAPI_Config);
} // void InfoTestMiscDefAPIs(NAVAPI_CONFIG stNAVAPI_Config)
