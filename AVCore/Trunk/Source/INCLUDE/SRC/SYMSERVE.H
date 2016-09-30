// Copyright 1992, 1993 Symantec, Peter Norton Group                          
//**********************************************************************
//                                                                      
// $Header:   S:/INCLUDE/VCS/symserve.h_v   1.9   25 Sep 1997 15:40:22   DDREW  $
//                                                                      
// Description: Prototypes for symserve/c
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Note: Please keep this file syncronized with NEWVIR global includes!
//          Elements are used in NAVENG.NLM and NAVEX15.NLM 
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                                                                      
// See Also:                                                            
//                                                                      
//**********************************************************************
// $Log:   S:/INCLUDE/VCS/symserve.h_v  $
// 
//    Rev 1.9   25 Sep 1997 15:40:22   DDREW
// defines and other things for Query interface
// 
//    Rev 1.8   05 Aug 1997 16:52:50   DDREW
// Add protos for LoadLibNLM etc..
// 
//    Rev 1.7   13 Mar 1995 02:54:30   JMILLARD
// 
// add GetColorMonitorCapability
// 
//    Rev 1.6   28 Feb 1995 20:06:18   BARRY
// Fixed compile problem -- HPROCESS only available to NLM on a prototype
// 
//    Rev 1.5   27 Feb 1995 17:26:32   JMILLARD
// add GetProcessId
// 
//    Rev 1.4   19 Feb 1995 01:53:06   JMILLARD
// add GetFunctionPointer
// 
//    Rev 1.3.1   26 Jan 1995 15:57:10   GDZIECI
// Modified GetSymbolValue() prototype to match ImportSymbol().
// 
//    Rev 1.3   22 Dec 1994 05:14:28   JMILLARD
// backout last change - it breaks sympipe/win
// 
//    Rev 1.2   22 Dec 1994 03:25:36   JMILLARD
// make RelinquishControl an empty macro for non-NLM platforms
// 
//    Rev 1.1   12 Dec 1994 19:32:20   DALLEE
// 1. Put #ifdef SYM_NLM around string functions KEITH commented out for Windows.
// 
// 
//    Rev 1.0   07 Dec 1994 09:50:10   MFALLEN
// Initial revision.
// 
//    Rev 1.2   23 Aug 1994 17:27:34   KEITH
// Commented out prototypes for StringTblInit, StringTblTerminate, 
// NLMLoadString, and ConsolePrintString for Windows compiles.
//  
// 
//    Rev 1.1   17 Aug 1994 14:13:32   JMILLARD
// merge residual changes from Quake 2
// 
//    Rev 1.0   16 Aug 1994 13:41:52   KEITH
// Initial revision.
// 
//    Rev 1.18   26 Apr 1994 16:38:56   BRAD
// Removed PVCS Logs
/************************************************************************/

#ifndef _SYMSERVE_H
#define _SYMSERVE_H

//  .h files required to include this .h file
                                                
// forced includes

#include <time.h>               // time_t
#include "symtime.h"            // WTIME
#include "file.h"

#if defined(SYM_NLM)
#include <process.h>            // ThreadSwitch
#endif


//**********************************************************************
//
//  Defines
//
//**********************************************************************

#define LOAD_SCALE              100     // scale factor for load
#define MAX_TIME_SLICE          545     // 60 ms in .11ms ticks
#define SYMBOL_NOT_FOUND        1       // returns from GetSymbolValue

#define LOADLIBRARYSTACKSIZE    16384   // stack for load library thread

#define LOADLIBRARY_WAIT        TRUE    // wait for library to load
#define LOADLIBRARY_DONTWAIT    FALSE

// return codes for GetFunctionPointer

typedef enum tagGET_FUNCTION
{
    FUNCTION_OK = 0,
    FUNCTION_NOT_SUPPORTED,

} GET_FUNCTION;


//**********************************************************************
//
//  Error messages
//
//**********************************************************************

#define SYM_THREAD_NOMEM             1
#define SYM_THREAD_INVAL             2
#define SYM_THREAD_UNDOC             3

#define SYM_THREAD_NOSTART           4
#define SYM_THREAD_NO_ATEXIT         5

#define SYM_THREAD_RESUME_SELF       6
#define SYM_THREAD_NOT_SUSPENDED     7
#define SYM_THREAD_BAD_SUSPEND_ID    8
#define SYM_THREAD_BAD_RESUME_ID     9

#define SYM_THREAD_UNKNOWN          10

#define SYM_MESSAGE                 11

#define SYM_MAX_STR                 12

//*************************************************************************
// String id defines
//*************************************************************************

#define SYM_NLM_STR_SUCCESS             0
#define SYM_NLM_STR_BAD_HANDLE          1
#define SYM_NLM_STR_NO_MORE_HANDLES     2
#define SYM_NLM_STR_NOT_FOUND           3

#define SYM_NLM_STR_MAX_MSG             5       // one extra for LAST_SYM_NLM_STRING_ID

// server error defines

#define SYM_SRV_UNCONNECTED              1      // GetConnectionInfo didn't find
#define SYM_SRV_UNDOCUMENTED             2      // undocumented error
#define SYM_SRV_NOFILE                   3      // no such file
#define SYM_SRV_RENAME_TOO_LONG          4      // new name too long
#define SYM_SRV_RENAME_NO_UNIQUE         5      // can't find unique name
#define SYM_SRV_RENAME_FAILED            6      // rename failed

#define SYM_SRV_MAX_STR                  7

// 
// miscellaneous other defins
//

// values for GetMonitorColorCapability

#define COLOR_MONITOR   1
#define BW_MONITOR      2


// values for wOSLoaderType

#define OS_LOADER_DOS   1   // server OS loaded by DOS
#define OS_LOADER_OS2   2   // server OS loaded by OS/2   




//**********************************************************************
//
//  Typedefs
//
//**********************************************************************

// structure of LOGIN_CONTROL property of users

typedef struct tagLOGINCONTROL           // complete declaration
    {

    BYTE    byAccountExpirationDate[3];
    BYTE    bAcccountDisabled;
    BYTE    byPasswordExpirationDate[3];
    BYTE    byGraceLoginsRemaining;
    WORD    wPasswordExpirationInterval;
    BYTE    byGraceLoginResetValue;
    BYTE    byMinPasswordLength;
    BYTE    byMaxConcurrentConnections;
    BYTE    byAllowedLoginTimeBitMap[42];
    BYTE    byLastLoginDateTime[6];
    BYTE    byRestrictionFlags;
    BYTE    byUnused;
    DWORD   dwMaxDiskUsage;
    WORD    wBadLoginCount;
    DWORD   dwNextResetTime;
    BYTE    byBadLoginAddress[12];
    BYTE    FillTo128[45];

    } LOGINCONTROL;


/*
 * The C module containing all the strings will have one or more arrays
 * of STRSTRUCTs.  A particular string will be uniquely identified by
 * a handle (identifying a STRSTRUCT array) and dwId.
 */

typedef struct {
	UINT    uId;                    // ID tag of this string
	PSTR	pString;                // string
} STRSTRUCT;

typedef STRSTRUCT	*PSTRSTRUCT;

#define	LAST_STRING_ID	        ((UINT)-1)      // ID of last message struct


        struct tagSYMBOLENTRY;                      // incomplete SYMBOLENTRY so I can
typedef struct tagSYMBOLENTRY FAR *LPSYMBOLENTRY;   // typedef LPSYMBOLENTRY

typedef struct tagSYMBOLENTRY           // complete declaration
    {

    LPSYMBOLENTRY   lpNextSymbol;        // next symbol in list
    LPVOID          lpNlmEntry;
    DWORD           dwSymValue;
    LPBYTE          lpcpSymName;

    } SYMBOLENTRY;


typedef struct tagLOADLIBRARY
    {

    LPSTR   lpszLibraryName;            // full path of library to load
    LPSTR   lpszStartUpParameter;       // start parameter(s) for library
    BOOL    bComplete;                  // completion flag for thread
    BOOL    bWaitForStatus;             // wait for library to load flag
    WORD    wErr;                       // error returned by load of library

    } LOADLIBRARY;

typedef LOADLIBRARY FAR *LPLOADLIBRARY;


// OS type definitions for GetOSVersion

#define UNKNOWN_OS          0
#define NOVELL_NETWARE      1


         
//**********************************************************************
//
//  Prototypes
//
//**********************************************************************

// misecallenous server functions
     
WORD wGetServerName(            // get server name
    PBYTE   szServerName,       // server name
    WORD    wStringSize );      // size of name string

WORD wGetUserName(              // get user name
    DWORD   dwConnectId,        // user's connection
    PBYTE   szUserName,         // user name
    WORD    wStringSize );      // size of name string

        WORD       wGetServerLoad(            // get server (% * LOAD_SCALE)
    VOID );                     // no arguments

typedef WORD (FAR *WGETSERVERLOAD)(            // get server (% * LOAD_SCALE)
    VOID );                     // no arguments
         
         
        VOID *       GetSymbolValue(            // get symbol value
    int  iModuleHandle,       // module handle
    char *szSymbolName );     // symbol name

typedef VOID * (FAR *GETSYMBOLVALUE)(            // get symbol value
    int  iModuleHandle,       // module handle
    char *szSymbolName );     // symbol name

int GetFunctionPointer( LPBYTE szFunctionName, LPVOID *lplpfnFunction );

WORD wGetVolumeName(                // get volume name
    DWORD   dwVolumeNumber,         // volume number
    LPBYTE  lpcpVolumeName );       // cpVolume name

WORD GetServerVolumeName(           // get volume name
    DWORD   dwVolumeNumber,         // Volume number
    LPBYTE  lpszVolumeName );       // szVolume name

VOID SaveStartPath( char *argv[] );
VOID GetStartPath( LPBYTE *lpszPath );
WORD SetCWD( LPBYTE szNewPath );

WORD SYM_EXPORT WINAPI wGetFileInfo(
    LPSTR       lpszFileName,           // file name
    LPFILEINFO  lpFinfo);               // file info struct

WORD SYM_EXPORT WINAPI UniqueRename(        // rename to unique name
    LPCSTR  szFileName,                 // file name    
    LPSTR   szNewNameRet);              // new name returned

WORD SYM_EXPORT WINAPI
CreateUniqueFileName(
    LPCSTR  szFileName,                 // orig path and file name    
    LPSTR   szNewNameRet);              // new name returned   


WORD GetOSVersion( LPWORD lpwOsType,
                   LPWORD lpwOsMajorRev,
                   LPWORD lpwOsMinorRev,
                   LPWORD lpwRevisionLevel);

#if defined(SYM_NLM)

WORD LoadLibrary( LPSTR lpszLibraryName,
                  LPSTR lpszStartUpParameter,
                  BOOL bWaitForStatus );
WORD LoadLibraryVP( LPSTR lpszLibraryCmdLine, BOOL bWaitForStatus );

HINSTANCE LoadLibraryNLM( LPSTR lpszLibraryName );
BOOL FreeLibraryNLM( HINSTANCE hLibrary );
LPVOID WINAPI GetProcAddressNLM( HINSTANCE hLibrary, LPSTR lpszFunctionName );

#define PROCIDSIZE 16

typedef    BYTE  NAVPROCID[ PROCIDSIZE +4 ];

#define LIBEXITFUNCTION "LibExitFunction0"    // all Proc id's s/b 16 bytes long
 
typedef struct
    {
    NAVPROCID   NavProcID;
    LPVOID      ProcAddress;
    } NAVNLM_PROC, *LPNAVNLM_PROC;


// thread management functions

VOID RelinquishControl(                 // will give up server
        VOID );

#endif

typedef WORD (FAR *RELINQUISHCONTROL)(  // give up server
        VOID );         

VOID RelinquishControlFriendly(         // will give up server to low-priority
        VOID );
typedef WORD (FAR *RELINQUISHCONTROLFRIENDLY)(  
        VOID );

DWORD RelinquishControlMonitored( DWORD dwId, LPDWORD lpdwLastTime );     // will give up server
typedef DWORD (FAR *RELINQUISHCONTROLMONITORED)(    // give up server
        DWORD dwId, LPDWORD lpdwLastTime );         

WORD wStartThread(                      // platform independent threadswitch
        VOID (*ThreadFunc) (VOID *),    // function that becomes thread
        DWORD  dwStackSize,             // stack size for thread
        VOID  *ThreadParameter);        // argument passed to thread on start

WORD wStartProcess(                     // platform independent (group of threads) 
        VOID (*ThreadFunc) (VOID *),    // function that becomes thread
        DWORD  dwStackSize,             // stack size for thread
        VOID  *ThreadParameter);        // argument passed to thread on start

WORD wRegisterThreadExit(               // register thread exit function
    VOID (*ThreadExitFunction) (VOID) );    // function to call at exit
        
HTHREAD hGetThreadId( VOID );           // get thread id, platform independent
HTHREAD hGetThreadGroupId( VOID );      // get thread group id, platform independent
#ifdef SYM_NLM                          // HPROCESS is only defined for NLM
HPROCESS GetProcessId( VOID );          // get process id, platform independent
#endif

WORD wSetThreadName(                    // set thread name, platform independ.
    LPBYTE szThreadName );              // new thread name
        
WORD wGetThreadName( LPBYTE szThreadName, WORD wStringSize );

VOID DelayThread(                       // delay thread
    DWORD dwMilliSeconds );             // delay in milliseconds
        
VOID DelayThreadTicks(                  // delay thread
    DWORD dwTicks );                    // delay in ticks

WORD wSleepThread(                      // suspend thread
    HTHREAD hThreadId );                // thread to suspend

WORD wWakenThread(                      // resume thread
    HTHREAD hThreadId );                // thread to resume

VOID ServerProcessExit(                 // thread exit
    int nExitCode );                    // exit code

BOOL ServerProcessExited( VOID );       // returns ServerProcessExiting flag

int GetMonitorColorCapability( VOID );


// string functions

#ifdef SYM_NLM

WORD SYM_EXPORT WINAPI
StringTblInit(
    PSTRSTRUCT  pSstr,                  // pointer to string structure
    PHSYMSTR    pHstr);                 // pointer to handle returned

WORD SYM_EXPORT WINAPI
StringTblTerminate(HSYMSTR hStr);       // message handle to close   

WORD SYM_EXPORT WINAPI
NLMLoadString(
    HSYMSTR hStr,                       // string table handle
    UINT    uId,                        // string id tag
    LPSTR   lpBuffer,                   // string buffer
    int     cchBuffer);                 // buffer length

VOID ConsolePrintString( WORD wStringTableIndex,
                         WORD wStringId );
#endif // #ifdef SYM_NLM

// command line functions

VOID SYM_EXPORT WINAPI CommandLineParse(
    int     argc,                       // command line count
    char    *argv[]);                   // command line arguments

// count preceding conversion routines

LPBYTE WINAPI lpbyPcpToSz(
    LPBYTE  pcpString,                  // pcp string input
    LPSTR   szString,                   // sz string output
    DWORD   dwMaxLen);                  // sz buffer size

LPBYTE WINAPI lpbySzToPcp(
    LPSTR   lpszString,                 // sz string input
    LPBYTE  pcpString,                  // pcp string output
    DWORD   dwMaxLen);                  // pcp buffer size

#ifdef SYM_NLM

// prototypes for undocumented NetWare functions

VOID StartSleepNotAllowed( VOID );
VOID EndSleepNotAllowed( VOID );
DWORD GetLoaderType( VOID );

#endif  // SYM_NLM

#endif  // _SYMSERVE_H
