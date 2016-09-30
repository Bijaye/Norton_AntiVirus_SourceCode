// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// nlmstuff.cpp 
//
// $Header:   S:/NAVAPI/VCS/nlmstuff.cpv   1.5   02 Sep 1998 13:00:42   dhertel  $
//
// NLM routines copied from other env/proj areas
//
//*************************************************************************
// $Log:   S:/NAVAPI/VCS/nlmstuff.cpv  $
// 
//    Rev 1.5   02 Sep 1998 13:00:42   dhertel
// Zero out all allocated memory blocks
// 
//    Rev 1.4   13 Aug 1998 17:18:38   DHERTEL
// Changed the name of NAVLIB.NLM to NAVAPI.NLM, and renamed NAVLIB.* source
// files to NAVAPI (where possible) or NAVAPINW (where necessary)
// 
//    Rev 1.3   11 Aug 1998 20:23:28   DHERTEL
// #ifdef'ed out the Netware 3.11 support
// 
//    Rev 1.2   06 Aug 1998 21:05:46   DHERTEL
// Added helper NLMs to allow 3.11 support
// 
//    Rev 1.1   30 Jul 1998 21:52:48   DHERTEL
// Changes for NLM packaging of NAVAPI.
// 
//    Rev 1.0   29 Jul 1998 18:34:50   DHERTEL
// Initial revision.
//*************************************************************************

#if defined(SYM_NLM)  // whole file


#include <stdio.h>
#include <time.h>           // time_t
#include <share.h>
#include <errno.h>          // EFAILURE
#include <sys/stat.h>       // struct stat
#include <io.h>             // access
#include <process.h>        // spawnlp
#include <stdarg.h>

#include <fileengd.h>       // FEsopen()
#include <nwenvrn.h>        // GetServerInformation()
#include <nwconn.h>         // GetConnectionInformation
#include <nwdir.h>          // GetVolumeName, chdir
#include <niterror.h>       // ERR_VOLUME_DOES_NOT_EXIST
#include <conio.h>          // {Get,Set}CurrentScreen
#include <advanced.h>       // ImportSymbol()

#include "platform.h"
#include "xapi.h"
#include "file.h"

#include "symserve.h"       // prototypes 


//***************************************************************************
// types
//
// Source: YODA\INCLUDE\SCREEN.H 
//***************************************************************************

#define CONSOLE_CREATE_FAILED       1
#define SET_CURRENT_SCREEN_FAILED   2
#define SCREEN_DESTROY_FAILED       3
#define SCR_NOT_ON_CONSOLE          4
#define SCR_NO_SCREEN               5
#define SCR_UNDOC                   6


//***************************************************************************
// types
//
// Source: YODA\SYMSERVE\SERVUTIL.C 
//***************************************************************************

#define MAX_HANDLE_TO_NAME 10
#define MAX_MODULENAME_SIZE 16

typedef struct
    {
    int     hHandle;
    BYTE    szHandleName[MAX_MODULENAME_SIZE];  
    } HANDLE_TO_NAME, *LPHANDLE_TO_NAME;

LPVOID   (*lpfnQueryInterface)( LPNAVNLM_PROC *lpNavNlmProc);
VOID     (*lpfnLibExit)(VOID);


//*************************************************************************
// data
//
// Source: YODA\SYMSERVE\SRVSTR.C 
//*************************************************************************

STATIC BYTE szStartPath[256];
// STATIC char szCommandLine[256];


HANDLE_TO_NAME HandleToName[ MAX_HANDLE_TO_NAME ];


// the following strings are used by SYM_NLM's that may not have access to
// the string management system, and so are accessed directly by name

BYTE szCantUnload[ ] =
"NAVAPI (Norton AntiVirus scanning library) is in use by another module.\n";

// answer to Unload module anyway' at NetWare system console - translate if
// NetWare itself has been translated and expects another answer

BYTE byDontUnload    = 'n';


// PLEASE DO NOT TRANSLATE "System Console" !!!!!!!!!! This is the name
// of the NetWare system console screen, and must remain untranslated unless
// NetWare has been translated and the name of this screen has changed

BYTE szSystemConsoleName[ ] = "System Console";

typedef void* (*LPGETPROCADDRESS)(char* szSymbolName);
typedef void* (*LPGETSYMBOLVALUE)(int iModuleHandle, char *szSymbolName);

#if defined(SUPPORT311)
LPGETSYMBOLVALUE gpfnImportSymbol = NULL;
#endif


//***************************************************************************
// prototypes
//***************************************************************************

static BOOL FileCompressionPossible();
static VOID LoadLibraryThread(LPLOADLIBRARY lpLoadLib);
static WORD wStartProcess(VOID (*ThreadFunc) (VOID *), DWORD dwStackSize, VOID *ThreadParameter); 
static VOID StripExtension(LPSTR lpszTmpDest, LPSTR lpszTmpSrc);
static BOOL SetHandleToName(int nNlmHandle, LPSTR lpszTmpBuf);
static LPHANDLE_TO_NAME GetNameFromHandle(int nNlmHandle);
static VOID* GetSymbolValue(int iModuleHandle, char *szSymbolName);         
#if defined(SUPPORT311)
       int LoadHelperNLM();
static VOID FixupPathname(BYTE *OsLibPath);
#endif
       VOID SaveStartPath(char *argv[]);
static VOID GetStartPath(LPBYTE *lpszPath);
       void DebugLog(char* szFormat, ...);


//*************************************************************************
// FileCompressionPossible()
//
// I'm not sure why NV_FileOpen() checks this flag instead of just 
// assuming it is always true.
//
// Source: YODA\EXEC\OSLIB.C
//*************************************************************************

static BOOL FileCompressionPossible()
{
    UINT            nErr;
    FILE_SERV_INFO  ServerInfo;
    static BOOL     bFileCompressionPossible = FALSE;
    static BOOL     bInitialized = FALSE;

    if (!bInitialized)
    {
        nErr = GetServerInformation(sizeof(FILE_SERV_INFO), &ServerInfo);
        if (nErr == 0)
            bFileCompressionPossible = (ServerInfo.netwareVersion != 3);

        bInitialized = TRUE;
    }

    return bFileCompressionPossible;
}


//*************************************************************************
// NV_FileOpen()
//
// Source: YODA\NAVSCAN\S_INIT.CPP
//*************************************************************************

HFILE /*SYM_EXPORT WINAPI*/ NV_FileOpen (
    LPSTR   lpName, 
    UINT    uOpenMode )
{
    SDWORD  nAccess         = uOpenMode & (O_WRONLY | O_RDWR);
    SDWORD  nShare          = SH_COMPAT;
    SDWORD  nPermission     = S_IREAD | S_IWRITE;
    SDWORD  nFlagBits       = NO_RIGHTS_CHECK_ON_OPEN_BIT;
    BYTE    byDataStream    = 0;
    HFILE   hFile           = NULL;


    // Note:1  ==============================================================
    //
    //      nAccess is set to uOpenMode & (O_WRONLY | O_RDWR). This will
    //      limit it to be O_RDONLY, O_WRONLY or O_READWRITE. Other bits
    //      will be ignored. For our present purposes, this will do.
    //
    //      When FIL4_PROTECT opened a file with DENY NONE set, open
    //      failed, as the file was already open.
    //
    //      Hence this change.
    //
    //      -- MLELE 12/11/95
    //
    // ======================================================================

    //  If lpName is NULL, return -1 handle
    if (lpName == NULL)
        return -1;


    // if files can be compressed, then tell him not to uncompress the 
    // file because I looked at it.

    if (FileCompressionPossible())
//  if (bFileCompressionPossible)
        {
        nFlagBits |= LEAVE_FILE_COMPRESSED_DATA_BIT;
        }

    hFile = (HFILE)FEsopen( lpName, nAccess, nShare,
                            nPermission, nFlagBits, byDataStream );

    return(hFile);
}


//*************************************************************************
// [NAV]MemAllocPtr()
//
// The original version of this function is far more elaborate.
//
// This must be exported as it is called by the NAVEX/NAVENGE NLMs.
//
// Source: YODA\SYMASSRT\MYMEMORY.C
//*************************************************************************

VOID *MemAllocPtr(UINT uMemType, size_t dwSize)   
{
    void* p = malloc(dwSize);
    if (p != NULL)
        memset(p, 0, dwSize);
    return p;
}


//*************************************************************************
// [NAV]MemFreePtr()
//
// The original version of this function is far more elaborate.
//
// This must be exported as it is called by the NAVEX/NAVENGE NLMs.
//
// Source: YODA\SYMASSRT\MYMEMORY.C
//*************************************************************************

VOID MemFreePtr(VOID *lpMemory)   
{
    if (lpMemory != NULL)
        free(lpMemory);
}


//***************************************************************************
// LoadLibraryThread()
// 
// function for thread to load a library. This thread is
// used to allocate the time required to load a 
// thread (which is all run-time library time (without
// thread switches)) where it can be seen, and so that it
// doesn't mask any of our thread switch problems!
// 
// It also lets us set up to use the console screen for the
// NLM loading messages - which otherwise corrupt our screen
// this is mostly a problem for loading third-party NLM's
// This thread is started as a separate thread group so that
// any screen changes we make don't affect the caller - what
// was happening was that we used to change the screen on the
// caller's thread and start up in the same group - but if the
// caller tried to do screen input while we had it switched
// he would explode since you can't do input from the system
// console.
// 
// Source: YODA\SYMSERVE\SERVUTIL.C 
//***************************************************************************

static VOID LoadLibraryThread (LPLOADLIBRARY lpLoadLib)
{
    int     nSysScreen = -1;
    int     nCurScreen = -1;
    WORD    wErr;
    BOOL    bWaitForStatus  = lpLoadLib->bWaitForStatus;


    // I can no longer set the thread name, since I need GetSymbol to run it,
    // and GetSymbol can't be used until I have loaded the library, which
    // this routine does!

////(void) wSetThreadName( FUNCNAM );

    // now change this thread group to the system console - that way
    // any traffic loading the NLM goes to the console - not to the
    // user's screen.

    nCurScreen = GetCurrentScreen();

    nSysScreen = CreateScreen(szSystemConsoleName, 0);
    if (nSysScreen != EFAILURE)
        {
        SetCurrentScreen(nSysScreen);
        }

    if (!bWaitForStatus)
        {
        lpLoadLib->wErr = NOERR;
        lpLoadLib->bComplete = TRUE;    // tell the caller I'm done
        }

    wErr = spawnlp(P_NOWAIT,
                   lpLoadLib->lpszLibraryName,
                   lpLoadLib->lpszLibraryName,
                   lpLoadLib->lpszStartUpParameter,
                   NULL );

    if (bWaitForStatus)
        {
        lpLoadLib->wErr = wErr;
        lpLoadLib->bComplete = TRUE;    // tell the caller I'm done
        }
    else
        {

        // free the memory he gave me
        if( lpLoadLib->lpszLibraryName )
            {
            MemFreePtr( lpLoadLib->lpszLibraryName );
            }
        if( lpLoadLib->lpszStartUpParameter )
            {
            MemFreePtr( lpLoadLib->lpszStartUpParameter );
            }
        }

    // restore the screen - although in this case the thread group is ending
    // so this should be going away, but I put it back just to be orderly

    if (nCurScreen != -1)
        {
        SetCurrentScreen(nCurScreen);
        }

    // destroy my copy of the console screen

    if (nSysScreen != -1)
        {
        DestroyScreen( nSysScreen );
        }
    }

//***************************************************************************
// LoadLibraryNLM()
// 
// Source: YODA\SYMSERVE\SERVUTIL.C 
//***************************************************************************

HINSTANCE LoadLibraryNLM( LPSTR lpszLibraryName )
{
    WORD            wErr = NOERR;
    LOADLIBRARY     LoadLib;
    int             nNlmHandle;
    BYTE            szTmpBuf[SYM_MAX_PATH+1];
    
    nNlmHandle      = 0;

// This test is for secure console. If we detct that the base path is
//  sys:system in any combination, we truncate the load path to just the
//  file name to suit the rule of no fully qualified path.

    STRUPR( lpszLibraryName );
    STRCPY( szTmpBuf, szStartPath );
    STRUPR( szTmpBuf );
    if( 0==STRCMP( szTmpBuf, "SYS:SYSTEM\\") || 
        0==STRCMP( szTmpBuf, "SYS:\\SYSTEM\\") ||
        0==STRCMP( szTmpBuf, "SYS:/SYSTEM\\") )
        {
        NameReturnFile ( lpszLibraryName, &szTmpBuf );
        LoadLib.lpszLibraryName = &szTmpBuf;
        }
    else
        {
        LoadLib.lpszLibraryName = lpszLibraryName;
        }

    LoadLib.lpszStartUpParameter = NULL;
    LoadLib.bWaitForStatus = TRUE;
    LoadLib.bComplete = FALSE;
    LoadLib.wErr = NOERR;

    wErr = wStartProcess( LoadLibraryThread, LOADLIBRARYSTACKSIZE, &LoadLib );
    if (wErr == NOERR)
        {
        while( !LoadLib.bComplete )
            {
            RelinquishControl( );
            }
        }

    if ( !LoadLib.wErr )
        {
        szTmpBuf[0] = 0;
        szTmpBuf[1] = 0;                // maybe Unicode??
        NameReturnFile ( lpszLibraryName, &szTmpBuf );
        nNlmHandle = FindNLMHandle( &szTmpBuf );
        if(nNlmHandle)
            {
            StripExtension(  &szTmpBuf, &szTmpBuf ); 
            SetHandleToName( nNlmHandle, &szTmpBuf ); 
            }
        }

    return( (HINSTANCE)nNlmHandle );
}


//***************************************************************************
// FreeLibraryNLM()
// 
// Source: YODA\SYMSERVE\SERVUTIL.C 
//***************************************************************************

BOOL FreeLibraryNLM( HINSTANCE hLibrary )
{
    LPNAVNLM_PROC   lpNavNlmProc;
    LPHANDLE_TO_NAME lpH2N;
    BYTE        ModuleName[255];


    lpNavNlmProc = NULL;                    // stop compiler whine

    // see if we can map the handle to a library we loaded ourselves
    lpH2N = GetNameFromHandle((int)hLibrary);
    if( !lpH2N )
        return( TRUE );

    // fabricate a Query Function name from modulename 
    STRCPY( &ModuleName, lpH2N->szHandleName);
    STRCAT( &ModuleName, "_QUERY_INTERFACE" );

    // see if the module and function is still loaded
    lpfnQueryInterface = GetProcAddressNLM( hLibrary, &ModuleName );
    if( !lpfnQueryInterface )
        return( TRUE );
    
    // call the Module Name's Query interface to get a pointer to it's
    //  NLM function array
    if( (*lpfnQueryInterface)( &lpNavNlmProc ) )
        return( TRUE );

    // so far so good, now look up the global identifier "LibExitFunction" in
    //  the NlmProcArray and call it
    while( *lpNavNlmProc->NavProcID )
        {
        if (STRCMP(&lpNavNlmProc->NavProcID, LIBEXITFUNCTION) == 0 )
            {
            lpfnLibExit = lpNavNlmProc->ProcAddress;

            // call the LibExit routine and remove it from our local list
            if(!lpfnLibExit)
                return( TRUE );

            (*lpfnLibExit)( );

            // DRH 7/29/98
            // wait until the NLM has disappeared
            while (FindNLMHandle( lpH2N->szHandleName ))
                RelinquishControl( );

            lpH2N->hHandle = 0;
            MEMSET( &lpH2N->szHandleName, 0, MAX_MODULENAME_SIZE );
            return( FALSE );
            }

        lpNavNlmProc++;
        }

    return( TRUE );
}


//***************************************************************************
// GetProcAddressNLM()
// 
// ImportSymbol() is available on NetWare 3.12+ and 4.x.
// The Yoda environment supports older verions by loading a custom
// NLM (selected at run time based on the NetWare version) that
// implements the same functionality using GetSymbolValue().
//
// Source: YODA\SYMSERVE\SERVUTIL.C 
//***************************************************************************

LPVOID GetProcAddressNLM(
    HINSTANCE   hLibrary,
    LPSTR       lpszFunctionName )
{
    LPVOID      lpProcAdd = NULL;

#if defined(SUPPORT311)
    if (gpfnImportSymbol != NULL)
        lpProcAdd = gpfnImportSymbol((int)hLibrary, lpszFunctionName);
#else
    lpProcAdd = ( ImportSymbol((int)hLibrary,(char*)lpszFunctionName) );
#endif

    return( lpProcAdd );
}


//**********************************************************************
// wStartProcess()
//
// Source: YODA\SYMSERVE\SERVUTIL.C 
//**********************************************************************

static 
WORD wStartProcess(                     // platform independent 
        VOID (*ThreadFunc) (VOID *),    // function that becomes thread
        DWORD  dwStackSize,             // stack size for thread
        VOID  *ThreadParameter)         // argument passed to thread on start
{
    int     nErr;               // O/S error return
    WORD    wErr;               // my error return

#ifdef SYM_NLM

    nErr = BeginThreadGroup( ThreadFunc, NULL, dwStackSize, ThreadParameter );

    if (nErr == EFAILURE)
        {
        switch (errno)
            {
            case ENOMEM:
                wErr = SYM_THREAD_NOMEM;
                break;

            case EINVAL:
                wErr = SYM_THREAD_INVAL;
                break;

            default:
                wErr = SYM_THREAD_UNDOC;
                break;
            }
        }
    else
        {
        wErr = NOERR;
        }


#endif  // ifdef SYM_NLM

    return wErr;
}


//**********************************************************************
// StripExtension()
//
// Source: YODA\SYMSERVE\SERVUTIL.C 
//**********************************************************************

VOID StripExtension( LPSTR lpszTmpDest, LPSTR lpszTmpSrc )
{
    while( (*lpszTmpSrc != '.') && (*lpszTmpSrc != 0) )
        *lpszTmpDest++ = *lpszTmpSrc++;
    *lpszTmpDest = 0;
}


//***************************************************************************
// SetHandleToName()
// 
// Puts a handle in the HandleTo name array
// 
// nNlmHandle is an NLM module handle 
// lpszTmpBuf is a module name string ( 8 bytes or less )
// 
// Returns FALSE if ok
// 
// Source: YODA\SYMSERVE\SERVUTIL.C 
//***************************************************************************

static BOOL SetHandleToName( int nNlmHandle, LPSTR lpszTmpBuf )
{
    LPHANDLE_TO_NAME    lpH2N;
    int         nCnt;

    lpH2N = &HandleToName;
    
    nCnt = MAX_HANDLE_TO_NAME;
    while( lpH2N->hHandle && nCnt )
        {
        lpH2N++;
        nCnt--;
        }
    
    if( nCnt )
        {
        lpH2N->hHandle = nNlmHandle;
        STRCPY( &lpH2N->szHandleName, lpszTmpBuf );
        return( FALSE );
        }

    return( TRUE );
}

//***************************************************************************
// GetNameFromHandle()
// 
// Finds a handle in the HandleTo name array
// 
// Returns lpH2N if ok, NULL if no match
// 
// Source: YODA\SYMSERVE\SERVUTIL.C 
//***************************************************************************

static LPHANDLE_TO_NAME GetNameFromHandle( int nNlmHandle )
{
    LPHANDLE_TO_NAME    lpH2N;
    int         nCnt;

    lpH2N = &HandleToName;
    
    nCnt = MAX_HANDLE_TO_NAME;
    while( nCnt )
        {
        if(lpH2N->hHandle == nNlmHandle)
            break;
        lpH2N++;
        nCnt--;
        }
    
    if( nCnt )
        {
        return( lpH2N );
        }

    return( NULL );
}


//**********************************************************************
// GetSymbolValue()
//
// Source: YODA\SYMSERVE\SERVUT31.C 
//**********************************************************************

#if 0

static VOID * GetSymbolValue(            // get symbol value
    int  iModuleHandle,           // module handle
    char *szSymbolName )          // symbol name

{
    VOID   *pSymValue = NULL;

#ifdef SYM_NLM

    EXTERN  LPSYMBOLENTRY   ExternalPublicList;

    LPSYMBOLENTRY   lpSymbol = ExternalPublicList;

    BYTE            cpSymbol[256];
    BYTE            bySymLen;

    // convert to count-preceded string

    bySymLen = STRLEN( szSymbolName );
    cpSymbol[0] = bySymLen;
    STRNCPY( &cpSymbol[1], szSymbolName, cpSymbol[0] );

    while (lpSymbol != NULL)
        {
        if ( bySymLen == lpSymbol->lpcpSymName[0] )
            {
            if (!STRNCMP( &cpSymbol[1], &lpSymbol->lpcpSymName[1], bySymLen ))
                {
                pSymValue = (VOID *)lpSymbol->dwSymValue;
                break;
                }
            }
        lpSymbol = lpSymbol->lpNextSymbol;
        }

#endif 

    // module handle is not used here, included only to match ImportSymbol()
    (VOID) iModuleHandle;

    return pSymValue;
}

#endif


//**********************************************************************
// RelinquishControl()
//
// Relinquishes control, unfriendly version - does not allow
// low-priority threads to run
//
// Source: YODA\SYMSERVE\SERVUT40.C 
//**********************************************************************

VOID RelinquishControl ( VOID )
{

#ifdef DEBUG_TIMING

#ifdef SYM_NLM

    STATIC  DWORD dwLastTimerValue = 0;
            DWORD dwCurrentTimerValue;
            DWORD dwTimeChange = 0;

        
    dwCurrentTimerValue = GetHighResolutionTimer( );

    if (dwLastTimerValue != 0)
        {
        if (dwLastTimerValue > dwCurrentTimerValue)
            {
            dwTimeChange = dwLastTimerValue - dwCurrentTimerValue;
            }
        else
            {
            dwTimeChange = dwCurrentTimerValue - dwLastTimerValue;
            }

        if (dwTimeChange > MAX_TIME_CHANGE_ALLOWED)
            {
            AssertPause( );
            }
        }

    TimeHistoryNoId[wTimeHistoryIndexNoId] = dwTimeChange;

    wTimeHistoryIndexNoId += 1;

    if (wTimeHistoryIndexNoId >= MAX_TIME_HISTORY)
        wTimeHistoryIndexNoId = 0;

    dwLastTimerValue = dwCurrentTimerValue;

#endif

#endif

    ThreadSwitch( );
}


//**********************************************************************
// DebugBreak()
//
// Source: CORE\SYMKRNL\SYMASSER.CPP
//**********************************************************************

VOID DebugBreak( VOID );
#pragma aux DebugBreak    = 0xcc parm caller [];



//**********************************************************************
// AssertFailedLine()
//
// Source: CORE\SYMKRNL\SYMASSER.CPP
//**********************************************************************

#ifndef SYM_DEBUG

// AssertFailedLine is prototyped in the Core headers if SYM_DEBUG is
// set, but we need it prototyped always.

EXTERNC VOID WINAPI AssertFailedLine(const LPSTR lpFileName, int iLine);        // [in] line number where it occured

#endif


VOID WINAPI AssertFailedLine(           // Display assertion failed information
    const LPSTR lpFileName,             // [in] name of the file that err
    int         iLine )                 // [in] line number where it occured
{
    ConsolePrintf ( "Assertion failed: %s, line %d", lpFileName, iLine );
    DebugBreak( );
}


//**********************************************************************
// [NAV]wSleepThread()
//
// This must be exported as it is called by the NAVEX/NAVENGE NLMs.
//
// Source: YODA\SYMSERVE\THRDMGMT.C
//**********************************************************************

WORD wSleepThread( HTHREAD hThreadId )
{

#ifdef DEBUG_TIMING

    DWORD   dwHiResTimer = GetHighResolutionTimer( );

#endif

    int     nErr;
    WORD    wErr;

#ifdef SYM_NLM

#ifdef DEBUG_TIMING

    if (dwThreadStartTime != 0 &&
        hGetThreadGroupId( ) == hThreadGroupId &&
        dwHiResTimer-dwThreadStartTime > MAX_HI_RES)
        {
        ConsolePrintf( "%d sleep thread overtime at %x\n",
            dwHiResTimer-dwThreadStartTime,
            ((LPDWORD)(&dwHiResTimer))[3]);
        AssertPause( );
        }

#endif

#ifdef DEBUG_TIMING
    SetThreadStartTime( );
#endif

    nErr = SuspendThread( hThreadId );

#ifdef DEBUG_TIMING
    SetThreadStartTime( );
#endif

    switch (nErr)
        {
        case ESUCCESS:
            wErr = NOERR;
            break;
        case EBADHNDL:
            wErr = SYM_THREAD_BAD_SUSPEND_ID;
            break;
        default:
            wErr = SYM_THREAD_UNKNOWN;
            break;
        }

#endif
    
    return wErr;
}


//**********************************************************************
// [NAV]wWakenThread()
//
// This must be exported as it is called by the NAVEX/NAVENGE NLMs.
//
// Source: YODA\SYMSERVE\THRDMGMT.C
//**********************************************************************

WORD wWakenThread( HTHREAD hThreadId )  
{

    int     nErr;
    WORD    wErr;

#ifdef SYM_NLM
    nErr = ResumeThread( hThreadId );

    switch (nErr)
        {
        case ESUCCESS:
            wErr = NOERR;
            break;
        case EINVAL:
            wErr = SYM_THREAD_RESUME_SELF;
            break;
        case EWRNGKND:
            wErr = SYM_THREAD_NOT_SUSPENDED;
            break;
        case EBADHNDL:
            wErr = SYM_THREAD_BAD_SUSPEND_ID;
            break;
        default:
            wErr = SYM_THREAD_UNKNOWN;
            break;
        }

#endif
    
    return wErr;
}


//**********************************************************************
// [NAV]SetConsoleFocus()
//
// Sets input focus to the system console
//
// Returns OK, CONSOLE_CREATE_FAILED, SET_CURRENT_SCREEN_FAILED
//
// WARNING!    This routine cannot be debugged with Soft-Ice or any
//             other debugger - they modify the input focus and cause
//             the routine to fail!
//
// This must be exported as it is called by the NAVEX/NAVENGE NLMs.
//
// Source: YODA\PLATFORM\SCREEN.C
//**********************************************************************

WORD SetConsoleFocus ( int *pnPreviousInputFocus,  // previous handle
                       int *pnSystemConsole      ) // handle to console
{
    WORD    wErr = NOERR;

#ifdef SYM_NLM

    int     nErr;

    int     nPreviousInputFocus;
    int     nSystemConsole;

    // this is extern because this is used by child nlms that may not
    // have the string loading routines available

    EXTERN BYTE  szSystemConsoleName[];


    //  save the current screen

    nPreviousInputFocus = GetCurrentScreen( );

    if (!nPreviousInputFocus)
        {
        // no current I/O focus, proceed anyway, just can't restore later
        }

    //  create a copy of the console to set the console input focus

    nSystemConsole = CreateScreen( szSystemConsoleName, (BYTE) 0);

    if (nSystemConsole == EFAILURE)
        {
        wErr = CONSOLE_CREATE_FAILED;  // create failed
        goto BailOut;
        }

    //  set the I/O focus to the console copy, if it isn't already

    if (nSystemConsole != nPreviousInputFocus)
        {
        nErr = SetCurrentScreen( nSystemConsole );

        if (nErr != ESUCCESS)
            {
            wErr = SET_CURRENT_SCREEN_FAILED;

            goto BailOut;
            }
        }

BailOut:

    if (wErr == NOERR)     // if ok, return the stuff he wanted
        {
        *pnPreviousInputFocus = nPreviousInputFocus;
        *pnSystemConsole      = nSystemConsole;
        }

#endif  // ifdef SYM_NLM

    return wErr;
}


//**********************************************************************
// [NAV]RestoreFocus()
//
// Restores input focus from the system console to previous screen
//
// Returns OK, SCREEN_DESTROY_FAILED, SET_CURRENT_SCREEN_FAILED
//
// WARNING!    This routine cannot be debugged with Soft-Ice or any
//             other debugger - they modify the input focus and cause
//             the routine to fail!
//
// This must be exported as it is called by the NAVEX/NAVENGE NLMs.
//
// Source: YODA\PLATFORM\SCREEN.C
//**********************************************************************

WORD RestoreFocus( int *pnPreviousInputFocus,  // previous handle
                   int *pnSystemConsole      ) // handle to console copy
{
    WORD    wErr = NOERR;

#ifdef SYM_NLM

    int     nErr;


    //  restore the original I/O focus

    nErr  = SetCurrentScreen( *pnPreviousInputFocus );

    if (nErr == ESUCCESS)
        {

        //  destroy copy of console

        nErr = DestroyScreen( *pnSystemConsole );

        if (nErr != ESUCCESS)
            {
            wErr = SCREEN_DESTROY_FAILED;

            goto BailOut;
            }
        }
    else
        {
        wErr = SET_CURRENT_SCREEN_FAILED;

        goto BailOut;
        }


BailOut:

#endif  // ifdef SYM_NLM

    return wErr;
}


//**********************************************************************
// [NAV]EmptyTypeAhead()
//
// Empties the input typeahead
//
// This must be exported as it is called by the NAVEX/NAVENGE NLMs.
//
// Source: YODA\PLATFORM\SCREEN.C
//**********************************************************************

VOID EmptyTypeAhead ( VOID )
{
    int     nChar;


    //  make sure the input queue has no typeahead in it

    while (kbhit( ))
        {
        nChar = getch( );       // pitch the character

        if (nChar == EOF)
            {
            break;              // leave on error
            }

        if (nChar == 0)         // check for multibyte input (extended key)
            {
            (VOID) getch( );
            }
        }
}


//**********************************************************************
// LoadHelperNLM()
//
// Loads either NAVL311.NLM or NAVL312.NLM.
// Obtains the address of a GetProcAddress() function in the helper NLM.
// Uses the GetProcAddress() function to get the address of version 
// specific helper functions in the helper NLM.
//
// TODO: We may need to create a thread to do the spawn call as is done
// in LoadLibraryNLM/FreeLibraryNLM to meet requirements for adequate
// ThreadSwitch() calls.  We can't use those functions as is because
// they assume the availability of GetProcAddressNLM (which is what the
// helper NLMs implement!) and assume a slightly different helper NLM
// design.
//
// Returns TRUE on success, FALSE on failure
//**********************************************************************

#if defined(SUPPORT311)

int LoadHelperNLM()
{
    FILE_SERV_INFO ServerInfo;
    UINT nErr;
    char* szName;
    char* lpszPath;
    char szPath[260];
    char szArg[20];
    LPGETPROCADDRESS pfnGetProcAddr;

    // Decide on the helper NLM name based on the server version.
    nErr = GetServerInformation(sizeof(FILE_SERV_INFO), &ServerInfo);
    if (nErr != 0)
        return FALSE;
    if (ServerInfo.netwareVersion == 3 && ServerInfo.netwareSubVersion == 11)
        szName = "NAVL311.NLM";
    else
        szName = "NAVL312.NLM";

    // Build full path name of helper NLM.
    GetStartPath(&lpszPath);
    strcpy(szPath, lpszPath);

    FixupPathname(szPath);
    strncat(szPath, szName, sizeof(szPath) - strlen(szPath) - 1);

    // Build command line argument for helper NLM
    sprintf(szArg, "%d", &pfnGetProcAddr);

    // Load the helper NLM.
    ThreadSwitch();
    pfnGetProcAddr = 0;
    if (spawnlp(P_NOWAIT, szPath, szPath, szArg, NULL) == -1)
        return FALSE;

    // Wait for the helper NLM to load and fill in the address
    // of it's GetProcAddress function.
    while (pfnGetProcAddr == NULL)
        ThreadSwitch();

    // Get the addresses of the helper functions.
    gpfnImportSymbol = (*pfnGetProcAddr)("NAVLIB_ImportSymbol");

    return TRUE;
}

#endif


//**********************************************************************
// FixupPathName()
//
// Source: YODA\EXEC\OSLIB.C
//**********************************************************************

#if defined(SUPPORT311)

static VOID FixupPathname( BYTE *OsLibPath )
    {

    UINT    nCnt;
    BYTE    *pbyTmp, *pzVolName;
    BYTE    *pzSys = "SYS";
    BYTE    *pzSystem = "SYSTEM";

            
    // trim off servername by finding colon and backing up
    pbyTmp = OsLibPath;
    nCnt = STRLEN( pbyTmp );
    pzVolName = 0;

    while( ( *pbyTmp != ':' ) && ( *pbyTmp != 0   ) && ( nCnt ) )
        {
        if( *pbyTmp == '\\' )
            {
            pzVolName = pbyTmp;
            }
        pbyTmp++;
        nCnt--;
        }
    
    if( *pbyTmp == ':' )
        {
            
        // we may or may not have passed a server name
        if( pzVolName != 0 )
            {
            pzVolName++;                    // past the slash
            }
        else
            {
            pzVolName = OsLibPath;
            }

        pbyTmp = pzVolName;

        // now look for words "sys" and then "system"
        
        // test for sys
        nCnt = 3;
        while( (nCnt) && (*pbyTmp == *pzSys) )
            {
            pbyTmp++;
            pzSys++;
            nCnt--;
            }

        if( nCnt )
            goto errexit;

        if (*pbyTmp == ':' )
            pbyTmp++;

        if( (*pbyTmp == '/') || (*pbyTmp == '\\') )
            pbyTmp++;

        // test for system
        nCnt = 6;
        while( (nCnt) && (*pbyTmp == *pzSystem) )
            {
            pbyTmp++;
            pzSystem++;
            nCnt--;
            }

        if( nCnt )
            goto errexit;

        if( (*pbyTmp == '/') || (*pbyTmp == '\\') )
            pbyTmp++;

        if( *pbyTmp == 0 )
            *OsLibPath = 0;                   
        }


errexit:

    return;
    }

#endif


//**********************************************************************
// SaveStartPath()
//
// Source: YODA\SYMSERVE\SERVUTIL.C
//**********************************************************************

VOID SaveStartPath( char *argv[] )
{
    int i,nLen;

    StringMaxCopy( szStartPath, argv[0], sizeof( szStartPath ));

    nLen = STRLEN( szStartPath ) - 1;

    for (i=nLen; i>0; i--)
        {
        if (szStartPath[i] == '\\')
            {
            szStartPath[i+1] = EOS;
            break;
            }
        }

    return;
}


//**********************************************************************
// GetStartPath()
//
// Source: YODA\SYMSERVE\SERVUTIL.C
//**********************************************************************

static VOID GetStartPath( LPBYTE *lpszPath )
{
    *lpszPath = szStartPath;
}


//**********************************************************************
// DebugLog()
//**********************************************************************

#if 0 // #if defined(SYM_DEBUG)

void DebugLog(char* szFormat, ...)
{
    va_list args;
    char szTemp[128];
    FILE* pFile;
    
    va_start(args, szFormat);
    vsprintf(szTemp, szFormat, args);
    ConsolePrintf(szTemp);
    pFile = fopen("SYS:\\DAVID\\LOG.TXT", "a+");
    if (pFile)
    {
        fputs(szTemp, pFile);
        fclose(pFile);
    }
    va_end(args);
}

#endif


#endif  // SYM_NLM

