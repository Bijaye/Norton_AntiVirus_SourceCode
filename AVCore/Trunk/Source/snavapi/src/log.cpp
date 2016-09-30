#include "log.h"
#include "spthread.h"


///////////////////////////////////////////////////////////////////////////////
// Mutexes
///////////////////////////////////////////////////////////////////////////////
SMUTEXHANDLE MutexLogFile;


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL WriteLog ()
//
//	Description:
//		Write to a report log file
//
//	Parameters:
//		szFileName
//		szFormat
//		...
//
//	Returns:
//		TRUE
//		FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL WriteLog
( 
	LPTSTR		szFileName,
	LPCTSTR		szFormat,
	... 
)
{
	FILE*	fpLogFile;
    va_list ArgList;
	static int initialized = 0;

	if (!initialized)
	{
		// Create mutex
		sCreateMutex ( &MutexLogFile );
		initialized = 1;
	}

	sLockMutex(&MutexLogFile);
	// Open report file
	if ((fpLogFile = fopen(szFileName, "a+t")) == NULL)
	{
		printf("ERROR: Can not open file %s!", szFileName);
		return (FALSE);
	}


	// Write to file formated output
	va_start (ArgList, szFormat);
	vfprintf(fpLogFile, szFormat, ArgList);
	va_end (ArgList);
	
	// Close file
	fclose(fpLogFile);
	sUnLockMutex(&MutexLogFile);

	return (TRUE);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void PrintAndLog ()
//
//	Description:
//      Prints and logs if logging options are set.
//
//	Parameters:
//		szOutputBuffer
//		lpstNAVAPI_Config
//
//	Returns:
//      Outputs text to the stdout and/or the log files.
//
///////////////////////////////////////////////////////////////////////////////
void PrintAndLog(char* szOutputBuffer, LPNAVAPI_CONFIG lpstNAVAPI_Config)
{
	printf("%s", szOutputBuffer);
	// Check if we should write to log.
	if ((lpstNAVAPI_Config->dwOptions & PARTIAL_VIRUS_LOG) || (lpstNAVAPI_Config->dwOptions & FULL_VIRUS_LOG))
	{
		WriteLog (lpstNAVAPI_Config->szReportLogFile, "%s", szOutputBuffer);
	}
}
