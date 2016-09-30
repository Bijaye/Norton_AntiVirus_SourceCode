#ifndef __FCDIR_H
#define __FCDIR_H
                                    

// Added after platform.h was removed from the fcdir
// project.
typedef unsigned long DWORD;
typedef unsigned short int  WORD;



// DOS/Win32 declarations
#if defined (SYM_WIN32) || defined (SYM_DOS)
	#define STAR		"*.*"
	#define SLASH		'\\'
	#define SLASH_STAR	"\\*.*" 

// UNIX declarations
#elif defined (SYM_UNIX)
	#define STAR		"*"
	#define SLASH		'/'
	#define SLASH_STAR	"/*" 

// NLM declarations
#elif defined (SYM_NLM)
	#define STAR		"*.*"
	#define SLASH		'\\'
	#define SLASH_STAR	"\\*.*" 

#endif


#ifdef SYM_UNIX
    #define _MAX_PATH   2048
#endif


// File compare defines
#define	MAX_BUFFER_SIZE		8192


#define	MATCH_SUCCESS		0
#define	MATCH_DIFF_SIZE		1
#define	MATCH_FAILED		2

#define	DELETE_MATCH		1
#define	LOG_REPORT			2
#define	MD5_LOG_REPORT		4
#define	FOMD5_LOG_REPORT	8

typedef struct
{
	char	szLogFile[_MAX_PATH];
	char	szMD5LogFile[_MAX_PATH];
	char	szFOMD5LogFile[_MAX_PATH];
	WORD	wOptions;
	DWORD	dwFilesProcessed;
	DWORD	dwFilesMatched;
	DWORD	dwFilesDeleted;
} OPTIONS, *LPOPTIONS;


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void HelpScreen(char* pszFileName)
//
//	Description:
//      Display the help screen
//
//	Parameters:
//      char* pszFileName
//			The filename of itself, for example, fcdir.exe on Win32.
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void HelpScreen(char* pszFileName);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessDir()
//
//	Description:
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpszDir2
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessDir
( 
	char*		pszPath,
	char*		pszDir1,
	char*		pszDir2,
	LPOPTIONS	lpOptions
);
       

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessDirMD5()
//
//	Description:
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessDirMD5
( 
	char*		pszPath,
	char*		pszDir1,
	LPOPTIONS	lpOptions
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessFile()
//
//	Description:
//      Process files
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpszDir2
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessFile
( 
	char*		pszPath,
	char*		pszDir1,
	char*		pszDir2,
	LPOPTIONS	lpOptions
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ProcessFileMD5()
//
//	Description:
//      Process files
//
//	Parameters:
//      lpszPath
//      lpszDir1
//      lpOptions
//
//	Returns:
//      NONE
//
///////////////////////////////////////////////////////////////////////////////
void ProcessFileMD5
( 
	char*		pszPath,
	char*		pszDir1,
	LPOPTIONS	lpOptions
);


// pszMD5HashOut has to point to a character array that's
// 33 bytes long at least.
static bool GenerateMD5Hash
(
    FILE*	fpFile,
	char*	pszMD5HashOut
);


///////////////////////////////////////////////////////////
//
//	Function Name:
//		DWORD FileCompare()
//
//	Parameters:
//		fpFile1
//		fpFile2
//
//	Returns:
//		MATCH_SUCCESS		0
//		MATCH_DIFF_SIZE		1
//		MATCH_FAILED		2
//
///////////////////////////////////////////////////////////
DWORD FileCompare
( 
    FILE*	fpFile1,
    FILE*	fpFile2
);

///////////////////////////////////////////////////////////////////////////////
//
//	Function Name:
//		WORD PathDiff()
//
//	Parameters:
//		lpszFileName1
//		lpszFileName2
//		lpResult
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
WORD PathDiff
( 
	char* pszFilePath1,
	char* pszFilePath2,
	char* pszResult
);


#if !defined (SYM_WIN32)
///////////////////////////////////////////////////////////////////////////////
//
//	Function Name:
//		BOOL DeleteFile()
//
//	Description:
//      Delete a file
//
//	Parameters:
//		lpFileName
//
//	Returns:
//      TRUE
//      FALSE
//
///////////////////////////////////////////////////////////////////////////////
bool DeleteFile(char* pszFileName);
#endif  // SYM_WIN32

#endif // __FCDIR_H
