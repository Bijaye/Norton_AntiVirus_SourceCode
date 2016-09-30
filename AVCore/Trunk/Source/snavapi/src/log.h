#ifndef __LOG_H
#define __LOG_H

#include "platform.h"
#include "avshared.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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
);


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
void PrintAndLog(char* szOutputBuffer, LPNAVAPI_CONFIG lpstNAVAPI_Config);


#endif // __LOG_H
