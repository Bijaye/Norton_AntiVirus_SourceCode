#ifndef __APIVER_H
#define __APIVER_H

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>

#include "avshared.h"
#include "shared.h"

///////////////////////////////////////////////////////////////////////////////
// Info test options structure
///////////////////////////////////////////////////////////////////////////////
typedef struct {
	char	szDefLoc[_MAX_PATH];
	char	szNavexInfFile[_MAX_PATH];
	WORD	wHeuristicLevel;
	char	szLogLoc[_MAX_PATH];
	DWORD	dwOptions;
} INFOTESTOPTS, *LPINFOTESTOPTS;


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
void Version (int argc, char* argv[]);


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
void InfoTest (int argc, char* argv[]);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL GetInfoTestOptions()
//
//	Description:
//
//	Parameters:
//		lpInfoTestOpts
//		argc
//		argv
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetInfoTestOptions 
(
	LPINFOTESTOPTS	lpINFOTESTOpts,
	int				argc,
	char*			argv[]
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void InfoTestHelp ()
//
//	Description:
//		Displays info test help screen
//
//	Parameters:
//		NONE
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void InfoTestHelp (void);


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
void InfoTestGetVirusDefTableInfo(NAVAPI_CONFIG stNAVAPI_Config);


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
void InfoTestMiscInfoAPIs(NAVAPI_CONFIG stNAVAPI_Config);


#endif // __APIVER_H
