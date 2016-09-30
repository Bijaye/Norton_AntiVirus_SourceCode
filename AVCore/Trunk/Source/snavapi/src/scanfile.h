#ifndef __SCANFILE_H
#define __SCANFILE_H

#include "platform.h"
#include "avshared.h"
#include <stdio.h>
#include <stdlib.h>

#if defined (SYM_WIN32)

#elif defined (SYM_UNIX)
    #include <sys/types.h>
    #include <limits.h>

    #if defined(SYM_BSD_X86)
        #include <sys/time.h>
    #endif

    #include <sys/resource.h>

#elif defined (SYM_NLM)
    #include <nwmisc.h>
#endif


// Win32 declarations
#if defined (SYM_WIN32)
	#define SLASH	'\\'
	#define STAR	"*.*"
	#define SLASH_STAR	"\\*.*"

// UNIX declarations
#elif defined (SYM_UNIX)
	#define SLASH	'/'
	#define STAR	"*"
	#define SLASH_STAR	"/*"

// NLM declarations
#elif defined (SYM_NLM)
	#define SLASH	'\\'
	#define STAR	"*.*"
	#define SLASH_STAR	"\\*.*"

#endif


#define MAX_THREAD_COUNT 4


///////////////////////////////////////////////////////////////////////////////
// Scan file options structure
///////////////////////////////////////////////////////////////////////////////
typedef struct {
	char	szFilePath[_MAX_PATH];
	char	szDefLoc[_MAX_PATH];
	char	szNavexInfFile[_MAX_PATH];
	WORD	wHeuristicLevel;
	DWORD	dwNumOfTimesToRepeatScan;
	DWORD	dwNumOfTimesToRepeatTotalScan;
	BOOL	bTabMode;
	char	szLogLoc[_MAX_PATH];
	DWORD	dwOptions;
} SCANFILEOPTS, *LPSCANFILEOPTS;


///////////////////////////////////////////////////////////////////////////////
// Thread parameters
///////////////////////////////////////////////////////////////////////////////
typedef struct _threadparameter
{
    char szFileName[_MAX_PATH];
    NAVAPI_CONFIG	stNAVAPI_Config;
    DWORD dwIndex;
} ThreadParameter, * lpThreadParameter;


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanFile()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void ScanFile
(
	int argc, 
	char* argv[]
);


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
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanFileHelp()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void ScanFileHelp (void);


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
);

// Gets the MD5 hash of lpszFilePath, stores it in szMD5.
void GetMD5Hash(
	const LPSTR lpszFilePath,
	char szMD5[32]
);

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
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void SetLimitOpenFilePerProcess()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
void SetLimitOpenFilePerProcess ( );



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
thread ( LPVOID ptr );


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
	LPNAVAPI_CONFIG		lpstNAVAPI_Config,
	LPSTR				lpszScanFile 
);


#endif // __SCANFILE_H
