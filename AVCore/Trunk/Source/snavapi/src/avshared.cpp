#include "avshared.h"
#include "log.h"


///////////////////////////////////////////////////////////////////////////////
// Virus hash table
///////////////////////////////////////////////////////////////////////////////
VIRTABLE g_aVirHashTable[VIRTABLE_HASH_SIZE];


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void __HashString()
//
//	Description:
//		Internal function to generate a hash value based on a string
//
//	Parameters:
//		nHashTableSize, szHashString
//
//	Returns:
//		int
//
///////////////////////////////////////////////////////////////////////////////
int __HashString(int nHashTableSize, char* szHashString)
{
	unsigned int key = 0;

	while (szHashString[0] != '\0')
	{
		key <<= 1;
		key = szHashString[0] + key;
		szHashString++;
	}

	return (key % nHashTableSize);
} // __HashString


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL InitializeVirusHashTable ()
//
//	Description:
//		Loads the virus hash table and places it into global g_aVirHashTable
//      variable. Returns TRUE on success, FALSE on error.
//
//	Parameters:
//		none
//
//	Returns:
//		BOOL
//
///////////////////////////////////////////////////////////////////////////////
BOOL InitializeVirusHashTable(NAVAPI_CONFIG stNAVAPI_Config)
{
	char				szFoo[32];
	char				szCurrVirusName[MAX_VIRUS_NAME_SIZE+1];
	LPVIRTABLE			lpCurrVirTable;
	DWORD				dwBufSize;
	HNAVVIRUSDEFTABLE	hTable;
	DWORD				u;
	int					nCnt;
	int					nTemp1;
	int					nHashKey;
	DWORD				dwNumDefs;

	static int isInitialized = 0;

	PrintAndLog("INFO TEST: Loading virus def table", &stNAVAPI_Config);

	// If already initialized, return.
	if (isInitialized)
	{
		PrintAndLog("INFO FUNCTIONS TEST: virus def table already initialized\n", &stNAVAPI_Config);
		return TRUE;
	}
	else
	{
		isInitialized = 1;
	}

	///////////////////////////////////////////////////////////////////////////
	// Load and store virus def table.
	///////////////////////////////////////////////////////////////////////////
	if (!(hTable = NAVLoadVirusDefTable ( stNAVAPI_Config.hNAVEngine )))
	{
		PrintAndLog("NAVLoadVirusDefTable() failed! This function is currently not supported under DOS or UNIX, so it should fail in those cases.\n", &stNAVAPI_Config);
		return FALSE;
	}

	if ( NAV_OK != NAVGetVirusDefTableCount ( hTable, &dwNumDefs ) )
	{
		PrintAndLog ( "NAVGetVirusDefTableCount() failed!\n", &stNAVAPI_Config );
		NAVReleaseVirusDefTable ( hTable );
		exit(1);
	}

	// Set hash table to null.
	for (u=0; u < VIRTABLE_HASH_SIZE; u++) {
		strcpy(g_aVirHashTable[u].szVirusName, "");
		strcpy(g_aVirHashTable[u].szVirusAlias, "");
		strcpy(g_aVirHashTable[u].szVirusInfo, "");
		strcpy(g_aVirHashTable[u].szVirusSize, "");
		g_aVirHashTable[u].next = NULL;
	}

	for ( u = 0, nCnt = 0; u < dwNumDefs; u++ )
	{
		if (nCnt == 600)
		{
			PrintAndLog(".", &stNAVAPI_Config);
			nCnt = 0;
		} else {
			nCnt++;
		}

		dwBufSize = MAX_VIRUS_NAME_SIZE+1;
		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_NAME, szCurrVirusName, &dwBufSize );

		nHashKey = __HashString(VIRTABLE_HASH_SIZE, szCurrVirusName);

		lpCurrVirTable = &(g_aVirHashTable[nHashKey]);
		while (lpCurrVirTable->szVirusName[0] != '\0')
		{
			if (lpCurrVirTable->next == NULL)
			{
				// Create a new entry and set lpCurrVirTable to it and initialize it.
				lpCurrVirTable->next =(LPVIRTABLE) malloc (sizeof (VIRTABLE));
				lpCurrVirTable = lpCurrVirTable->next;
				strcpy(lpCurrVirTable->szVirusName, "");
				strcpy(lpCurrVirTable->szVirusAlias, "");
				strcpy(lpCurrVirTable->szVirusInfo, "");
				strcpy(lpCurrVirTable->szVirusSize, "");
				lpCurrVirTable->next = NULL;
			}
			else
			{
				lpCurrVirTable = lpCurrVirTable->next;
			}
		}

		// We now have the proper virus table entry in the hash table to work with,
		// so we fill in the values of the virus table entry.
		strcpy(lpCurrVirTable->szVirusName, szCurrVirusName);
		dwBufSize = MAX_VIRUS_ALIAS_SIZE+1;
		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_ALIAS, lpCurrVirTable->szVirusAlias, &dwBufSize );
		dwBufSize = MAX_VIRUS_INFO_SIZE+1;
		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_INFO, lpCurrVirTable->szVirusInfo, &dwBufSize );
		dwBufSize = MAX_VIRUS_SIZE+1;
		NAVGetVirusDefTableInfo ( hTable, u, NAV_VI_VIRUS_SIZE, lpCurrVirTable->szVirusSize, &dwBufSize );


		for (nTemp1 = (int) NAV_VI_BOOL_FIRST+1; nTemp1 <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nTemp1++)
		{
			lpCurrVirTable->abVIBoolValues[nTemp1] =
				NAVGetVirusDefTableInfo (hTable, u, (NAVVIRINFO) nTemp1, szFoo, &dwBufSize );
		}

	}


	NAVReleaseVirusDefTable ( hTable );
	
	PrintAndLog("\n", &stNAVAPI_Config);
	return TRUE;
} // BOOL InitializeVirusHashTable(NAVAPI_CONFIG stNAVAPI_Config)


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		LPVIRTABLE GetVirusHashTableEntry ()
//
//	Description:
//		Return the VIRTABLE entry from the hash table corresponding to a given
//      virus name. Returns NULL on error.
//
//	Parameters:
//		szVirusName
//
//	Returns:
//		LPVIRTABLE
//
///////////////////////////////////////////////////////////////////////////////
LPVIRTABLE GetVirusHashTableEntry(char* szVirusName)
{
	LPVIRTABLE			lpCurrVirTable;
	int					nHashKey;

	nHashKey = __HashString(VIRTABLE_HASH_SIZE, szVirusName);
	lpCurrVirTable = &(g_aVirHashTable[nHashKey]);
	while (strcmp(lpCurrVirTable->szVirusName, szVirusName) != 0)
	{
		if (lpCurrVirTable->next == NULL)
			return NULL;
		lpCurrVirTable = lpCurrVirTable->next;
	}
	return lpCurrVirTable;
} // GetVirusHashTableEntry


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		LPVIRTABLE FreeVirTable()
//
//	Description:
//		Frees memory used by Virus Table entry and all linked entries.
//
//	Parameters:
//		lpVirTable
//
//	Returns:
//		LPVIRTABLE
//
///////////////////////////////////////////////////////////////////////////////
LPVIRTABLE FreeVirTable(LPVIRTABLE lpVirTable)
{
	if (lpVirTable != NULL)
	{
		FreeVirTable(lpVirTable->next);

		free(lpVirTable);
	}

	return lpVirTable;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL IsInfected ()
//
//	Description:
//		Determines whether or not the item is infected
//
//	Parameters:
//		hVirusInfo
//
//	Returns:
//		TRUE
//		FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL IsInfected
(
	HNAVVIRUS	hVirusInfo
)
{
	char	szVirusName[30] = {0};

	GetVirusName(hVirusInfo, szVirusName, sizeof(szVirusName));

	if(szVirusName[0] == (char)NULL)
		return(FALSE);
	else
		return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		DWORD GetVirusName ()
//
//	Description:
//		Get the virus name for a given item
//	
//	Parameters:
//		hVirusInfo
//		lpszVirusName
//		dwBufferSize
//
//	Returns:
//		NAV_OK
//		NAV_ERROR
//		NAV_INSUFFICIENT_BUFFER
//
///////////////////////////////////////////////////////////////////////////////
DWORD GetVirusName 
(
	HNAVVIRUS	hVirusInfo,
	LPSTR		lpszVirusName,
	DWORD		dwBufferSize
)
{
	return (NAVGetVirusInfo (hVirusInfo,
			                 NAV_VI_VIRUS_NAME,
				             lpszVirusName,
 							 &dwBufferSize));
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void CopyHNAVVIRUStoLPVIRTABLE ()
//
//	Description:
//		Fills in lpVirTable with information from a given HNAVVIRUS.
//
//	Parameters:
//		hVirusInfo
//		lpVirTable
//
//	Returns:
//		void
//
///////////////////////////////////////////////////////////////////////////////
void CopyHNAVVIRUStoLPVIRTABLE(HNAVVIRUS hVirusInfo, LPVIRTABLE lpVirTable)
{
	int			nIndex				= 0;
	NAVSTATUS	NAVStatus			= 0;
	DWORD		dwNavInfoSize;

	///////////////////////////////////////////////////////////////////////////
	// Get string values
	///////////////////////////////////////////////////////////////////////////
	dwNavInfoSize = sizeof(lpVirTable->szVirusName);
	if ((NAVStatus = NAVGetVirusInfo(hVirusInfo,NAV_VI_VIRUS_NAME,
		lpVirTable->szVirusName, &dwNavInfoSize)) != NAV_OK)
	{
		printf("NAVGetVirusInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		exit(1);
	}
	dwNavInfoSize = sizeof(lpVirTable->szVirusAlias);
	if ((NAVStatus = NAVGetVirusInfo(hVirusInfo,NAV_VI_VIRUS_ALIAS,
		lpVirTable->szVirusAlias, &dwNavInfoSize)) != NAV_OK)
	{
		printf("NAVGetVirusInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		exit(1);
	}
	dwNavInfoSize = sizeof(lpVirTable->szVirusInfo);
	if ((NAVStatus = NAVGetVirusInfo(hVirusInfo,NAV_VI_VIRUS_INFO,
		lpVirTable->szVirusInfo, &dwNavInfoSize)) != NAV_OK)
	{
		printf("NAVGetVirusInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		exit(1);
	}
	dwNavInfoSize = sizeof(lpVirTable->szVirusSize);
	if ((NAVStatus = NAVGetVirusInfo(hVirusInfo,NAV_VI_VIRUS_SIZE,
		lpVirTable->szVirusSize, &dwNavInfoSize)) != NAV_OK)
	{
		printf("NAVGetVirusInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		exit(1);
	}

	
	///////////////////////////////////////////////////////////////////////////
	// Get boolean values
	///////////////////////////////////////////////////////////////////////////
	for (nIndex = (int) NAV_VI_BOOL_FIRST+1; nIndex <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nIndex++)
	{
		lpVirTable->abVIBoolValues[nIndex] =
			NAVGetVirusInfo (hVirusInfo, (NAVVIRINFO) nIndex, NULL, NULL );
	}


	///////////////////////////////////////////////////////////////////////////
	// Set next to null
	///////////////////////////////////////////////////////////////////////////
	lpVirTable->next = NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void GetFullVirusInfo ()
//
//	Description:
//		Gets all the virus information for a given item.
//
//	Parameters:
//		hVirusInfo
//		lpszVirusName
//		uInfoBufferSize
//
//	Returns:
//		void
//
///////////////////////////////////////////////////////////////////////////////
void GetFullVirusInfo 
(
	HNAVVIRUS	hVirusInfo,
	LPSTR		lpszInfoBuffer,
	UINT		uInfoBufferSize
)
{
	int			nIndex				= 0;
	NAVSTATUS	NAVStatus			= 0;
//	UINT		uStrLen				= 0;
	char		szNavInfo[512]		= {0};
	DWORD		dwNavInfoSize		= sizeof(szNavInfo);
	char		szTmpBuffer[1024]	= {0};
	char*		lpszReturnStr;
	DWORD		dwDateTime = 0;
	struct tm*	stDateTime;


	///////////////////////////////////////////////////////////////////////////
	// Allocate memory
	///////////////////////////////////////////////////////////////////////////
	lpszReturnStr = (char*)malloc(uInfoBufferSize);

	// Initialize memory
	memset(lpszReturnStr, 0, uInfoBufferSize);

	///////////////////////////////////////////////////////////////////////////
	// Get string values
	///////////////////////////////////////////////////////////////////////////
	for (nIndex = 0; nIndex < TOTALENUMSTR; nIndex++)
	{
		dwNavInfoSize	= sizeof(szNavInfo);

		if ((NAVStatus = NAVGetVirusInfo (hVirusInfo,
			                              (NAVVIRINFO)gastVirusInfoStr[nIndex].wItemIndex,
									      szNavInfo,
 									      &dwNavInfoSize)) != NAV_OK)
		{
			sprintf(szTmpBuffer, "%s %s\n", gastVirusInfoStr[nIndex].szResultStr,
				                            gNAVAPIReturnValues[NAVStatus]);
		}
		else
		{
			sprintf(szTmpBuffer, "%s %s\n", gastVirusInfoStr[nIndex].szResultStr,
				                            szNavInfo);
		}
	
		strcat(lpszReturnStr, szTmpBuffer);

	}

	///////////////////////////////////////////////////////////////////////////
	// Get detection time
	///////////////////////////////////////////////////////////////////////////
	dwNavInfoSize	= sizeof(szNavInfo);
	if ((NAVStatus = NAVGetVirusInfo (hVirusInfo,
			                          NAV_VI_DETECT_DATE_TIME,
								      szNavInfo,
								      &dwNavInfoSize)) != NAV_OK)
	{
		sprintf(szTmpBuffer, "NAV_VI_DETECT_DATE_TIME:        %s\n", gNAVAPIReturnValues[NAVStatus]);
	}
	else
	{
		dwDateTime = atol (szNavInfo);
		stDateTime = localtime ((const time_t*)&dwDateTime);
		sprintf(szTmpBuffer, "NAV_VI_DETECT_DATE_TIME:        %.2d/%.2d/%.2d %.2d:%.2d:%.2d\n", stDateTime->tm_mon + 1,
			                                                                        stDateTime->tm_mday,
																			        stDateTime->tm_year + 1900,
																					stDateTime->tm_hour,
																					stDateTime->tm_min,
																					stDateTime->tm_sec);
	}

	strcat(lpszReturnStr, szTmpBuffer);
	
	///////////////////////////////////////////////////////////////////////////
	// Get boolean values
	///////////////////////////////////////////////////////////////////////////
	for (nIndex = 0; nIndex < TOTALENUMBOOL; nIndex++)
	{
		NAVStatus = NAVGetVirusInfo (hVirusInfo,
			                         (NAVVIRINFO)gastVirusInfoBool[nIndex].wItemIndex,
									 NULL,
									 NULL);
	
		sprintf(szTmpBuffer, "%s %s\n", gastVirusInfoBool[nIndex].szResultStr, 
			                            gastVirusInfoBool[nIndex].szResultValue[NAVStatus]);

		strcat(lpszReturnStr, szTmpBuffer);
	}

	strcpy(lpszInfoBuffer, lpszReturnStr);

	// Free allocated memory
	free(lpszReturnStr);
}	// GetFullVirusInfo()


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void GetFullVirusInfoLoadSingleVirus()
//
//	Description:
//		Gets all the virus information for a given virus ID using the 
//		NAVLoadVirusDef functions. Fills in lpVirTable with the information.
//
//	Parameters:
//		hVirusInfo
//		dwVirusID
//		lpVirTable
//
//	Returns:
//		void
//
///////////////////////////////////////////////////////////////////////////////
void GetFullVirusInfoLoadSingleVirus(NAVAPI_CONFIG stNAVAPI_Config, DWORD dwVirusID, LPVIRTABLE lpVirTable)
{
	int				nIndex				= 0;
	NAVSTATUS		NAVStatus			= 0;
	char			szTempStrBuf[1024];
	char			szNavInfo[512]		= {0};
	DWORD			dwNavInfoSize;
	HNAVVIRUSDEF	hnavVirusDef;

	if ((hnavVirusDef = NAVLoadVirusDef(stNAVAPI_Config.hNAVEngine, dwVirusID)) == NULL)
	{
		sprintf(szTempStrBuf, "Error running NAVLoadVirusDef.");
		PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
		exit(1);
	}

	///////////////////////////////////////////////////////////////////////////
	// Get string values
	///////////////////////////////////////////////////////////////////////////
	dwNavInfoSize = sizeof(lpVirTable->szVirusName);
	if ((NAVStatus = NAVGetVirusDefInfo(hnavVirusDef,NAV_VI_VIRUS_NAME,
		lpVirTable->szVirusName, &dwNavInfoSize)) != NAV_OK)
	{
		sprintf(szTempStrBuf, "NAVGetVirusDefInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
		exit(1);
	}
	dwNavInfoSize = sizeof(lpVirTable->szVirusAlias);
	if ((NAVStatus = NAVGetVirusDefInfo(hnavVirusDef,NAV_VI_VIRUS_ALIAS,
		lpVirTable->szVirusAlias, &dwNavInfoSize)) != NAV_OK)
	{
		sprintf(szTempStrBuf, "NAVGetVirusDefInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
		exit(1);
	}
	dwNavInfoSize = sizeof(lpVirTable->szVirusInfo);
	if ((NAVStatus = NAVGetVirusDefInfo(hnavVirusDef,NAV_VI_VIRUS_INFO,
		lpVirTable->szVirusInfo, &dwNavInfoSize)) != NAV_OK)
	{
		sprintf(szTempStrBuf, "NAVGetVirusDefInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
		exit(1);
	}
	dwNavInfoSize = sizeof(lpVirTable->szVirusSize);
	if ((NAVStatus = NAVGetVirusDefInfo(hnavVirusDef,NAV_VI_VIRUS_SIZE,
		lpVirTable->szVirusSize, &dwNavInfoSize)) != NAV_OK)
	{
		sprintf(szTempStrBuf, "NAVGetVirusDefInfo returned a value of %s\n", gNAVAPIReturnValues[NAVStatus]);
		PrintAndLog(szTempStrBuf, &stNAVAPI_Config);
		exit(1);
	}

	
	///////////////////////////////////////////////////////////////////////////
	// Get boolean values
	///////////////////////////////////////////////////////////////////////////
	for (nIndex = (int) NAV_VI_BOOL_FIRST+1; nIndex <= (int) NAV_VI_NON_ITEM_SPECIFIC_LAST; nIndex++)
	{
		lpVirTable->abVIBoolValues[nIndex] = 
			NAVGetVirusDefInfo (hnavVirusDef, (NAVVIRINFO) nIndex, NULL, NULL );
	}


	///////////////////////////////////////////////////////////////////////////
	// Set next to null
	///////////////////////////////////////////////////////////////////////////
	lpVirTable->next = NULL;

	// Free loaded virus def.
	NAVReleaseVirusDef(hnavVirusDef);
} // GetFullVirusInfoLoadSingleVirus()
