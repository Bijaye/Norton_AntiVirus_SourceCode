// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// savaudit.cpp 
//

#include "stdafx.h"

// CreateWaitableTimer function is now loaded dynamically (incompatible with Win95)
//     so this may not be necessary -- leaving it in just to be safe.  ESCHOOP 6/2/01
#define _WIN32_WINNT 0x0400     // this picks up the CreateWaitableTimer API's

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>

#define SCSAUDIT_EXPORTS	1

#define NOTRANSMAN 1
#include "vpcommon.h"
#define SERVER 1
#include "mngdprod.h"
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"

#include <tlhelp32.h> // process enum functions for Win9X

// Pacific Standard Time Offset from GMT (in seconds)
#define PST_GMT_OFFSET            28800

#define SCF_SETTINGS_INTEGRATOR_SUCCESS_CODE	0

// The default maximum number of times that we will try to apply a specific policy
#define DEFAULT_MAX_POLICY_FAIL_COUNT           2

// defines for the registry key "Type"

#define TYPE_IS_SERVER  0
#define TYPE_IS_CLIENT  1


#include "clientreg.h"

#include "scsaudit.h"

#ifdef NLM
#define REGHEADER "\\VirusProtect6"
#else
#define REGBASE REGROOT"\\VirusProtect6"
#define CURRENTVER "CurrentVersion"
#define REGHEADER REGBASE"\\"CURRENTVER
#define VP5_REGHEADER REGROOT"\\VirusProtect5\\"CURRENTVER
#endif

#if 0

// because of the trend scan engine we must compile the netware version using zp1, therefore we must pad our selfs.
typedef struct
{
    BYTE year;      // 0 = 1970;
    // all the rest are the same as struct tm
    BYTE month;     // 0 - 11
    BYTE day;       // 1 - 31
    BYTE hour;      // 0 - 23
    BYTE min;       // 0 - 59
    BYTE sec;       // 0 - 59
    WORD pad;
} VTIME;

// this is the returned product information packet

// items my be added to end ONLY
// this struct is transmited on the wire and it's size can not change.
// all array sizes are hard coded.
typedef struct {
	DWORD MySize;
	DWORD pattVer;
	DWORD ProductVersion;
	char DomainName[48];
	char Mom[48];
	char ComputerName[48];
	char License[32];
	DWORD Flags; // isPrimary=1 | isShairing = 2
	DWORD InstalledProducts;
	DWORD TransportVersion;
	VTIME TimeOfLastVirus;
	VTIME TimeOfLastScan;
	DWORD PatternFileDate; // days since 1970
	VTIME GRCTime;
	DWORD NumberOfClients;
	DWORD EngineVersion;
	char ScanEngineVendor[4];  // Changed from 48 to 4 bytes to free up space for needed info
                               // Should not be a problem because vendor name is always "NAV"
	char Reserved[4]; // reclaimed from ScanEngineVendor for future use

	// SCF management info
	SCFVERSION SCFVersion;
	VTIME SCFSettingsUpdateTime;

    DWORD dwPatternRevision;    // NAV daily revision number
    DWORD dwPatternSequence;    // IBM sequence number
    char GroupName[SIZEOF_CLIENTGROUPNAME];			// [SIZEOF_CLIENTGROUPNAME]
	char ConsolePassword[64];
	DWORD ServiceRelease;
	DWORD ComPatNum;
	BYTE GuidData[16];
	} PONGDATA;


// defines for Flags in PONGDATA

#define PF_PRIMARY                   0x00000001
#define PF_SHARE                     0x00000002
#define PF_PLATFORM_NLM              0x00000004
#define PF_PLATFORM_WINNT            0x00000008
#define PF_PLATFORM_WIN95            0x00000010
#define PF_HAS_TRANSMAN              0x00000020
#define PF_REG_BY_FILE               0x00000040
#define PF_NETWARE_5                 0x00001000
#define PF_NETWARE_4                 0x00000080
#define PF_NETWARE_3                 0x00000100
#define PF_SUPPORT_CLIENTS           0x00000200
#define PF_USING_AMS                 0x00000400
#define PF_WANT_PATTERN_UPDATES      0x00000800

#define OSTYPE_Unknown         0 
#define OSTYPE_DOS             1
#define OSTYPE_LINIX           2
#define OSTYPE_MAC             3
#define OSTYPE_MP_RAS          4
#define OSTYPE_NetWare         5
#define OSTYPE_NFS             6
#define OSTYPE_OS2             7
#define OSTYPE_OS2_WARP        8
#define OSTYPE_SCO             9 
#define OSTYPE_ULTRIX          10
#define OSTYPE_UNIX            11
#define OSTYPE_UNIXWare        12
#define OSTYPE_VMS             13
#define OSTYPE_Windows         14
#define OSTYPE_WindowsNT       15
#define OSTYPE_Windows95       16

#endif
              
// globals

HKEY hMainKey = NULL;
HKEY hProductControlKey = NULL;
HKEY hPattManKey = NULL;
HKEY hClientKey = NULL;
HKEY hForwardKey = NULL;
HKEY hSystemKey = NULL;
HKEY hCommonKey = NULL;
HKEY hQuarantineKey = NULL;

BOOL bVerbose = FALSE;
BOOL bRunningAsService = FALSE;
BOOL bRunningAsServer = FALSE;
BOOL bRoamClient = TRUE;
BOOL bRoamQuarantine = FALSE;
BOOL bRoamGRC = FALSE;
BOOL bRoamServer = FALSE;
BOOL bRoamAlerts = FALSE;
BOOL bRoamESM = FALSE;
BOOL bRoamITA = FALSE;

// #define IMAX_PATH	1024

void *_GetData(HKEY hkey,char *sval,void *in,int len,void *def,BOOL *DefUsed,DWORD *type,DWORD *outSize)
{

    DWORD dwType=0;
    DWORD cbData=0;
    DWORD cc = 0xffffffff;
    char t[IMAX_PATH];
    char *q;

    _tcsncpy(t,sval,IMAX_PATH-1);    t[IMAX_PATH - 1] = 0;
    q = _tcsrchr(t,'\\');
    if ( q )
    {
        HKEY hckey;
        *q = 0;
        if ( RegOpenKey(hkey,t,&hckey) != ERROR_SUCCESS )
        {
            if ( RegOpenKey(hCommonKey,t,&hckey) != ERROR_SUCCESS )
            {
                *q = '\\';
                goto usedef;
            }
        }
        _GetData(hckey,q+1,in,len,def,DefUsed,type,outSize);
        *q = '\\';
        RegCloseKey(hckey);
        return in;
    }

    if ( hkey )
    {
//      *in = 0;
        cbData = len;
        cc = SymSaferRegQueryValueEx(hkey, t, 0, &dwType,(unsigned char *)(LPVOID)in, &cbData);
    }

    if ( cc == ERROR_SUCCESS )
    {
        if ( DefUsed )
            *DefUsed = FALSE;
    }
    else
    {
        if ( DefUsed && *DefUsed )
            goto usedef;

        hkey = hCommonKey;
        if ( hkey )
        {
//          *in = 0;
            cbData = len;
            cc = SymSaferRegQueryValueEx(hkey, t, 0, &dwType,(unsigned char *)(LPVOID)in, &cbData);
            if ( cc != ERROR_SUCCESS )
            {
                usedef:     if ( def )
                {
                    __try{
                        memcpy(in,def,len);
                    }
                    __except(1)
                    {
                        // sometimes in WIN32 the len of the def buffer is too short to read from to fill the dest buffer, just trap the exception and go on
                        // this most likly will not happen on netware until novell starts using virtual memory alot more thay they are now.
                    }
                }
                if ( DefUsed )
                    *DefUsed = TRUE;
                dwType = 0xffffffff;
                cbData = len;
            }
            else if ( DefUsed )
                *DefUsed = TRUE;
        }
        else
        {
            goto usedef;
        }
    }

    if ( type )
        *type = dwType;
    if ( outSize )
        *outSize = cbData;

    return in;
}

/*****************************************************************************************************/
void *GetData(HKEY hkey,char *sval,void *in,int len,void *def,BOOL *DefUsed)
{
    return _GetData(hkey,sval,in,len,def,DefUsed,NULL,NULL);
}

/******************************************************************************/
DWORD _GetVal(HKEY *hkey,char *sval,DWORD def)
{

    DWORD val = 0;
    BOOL du = 1;

    while ( *hkey && du )
    {
        GetData(*hkey,sval,&val,sizeof(DWORD),&def,&du);
        hkey++;
    }

    if ( du )
        GetData(0,sval,&val,sizeof(DWORD),&def,NULL);

    return val;
}

/******************************************************************************/
char *_GetStr(HKEY *hkey,char *sval,char *in,int len,char *def)
{

    BOOL du = 1;

    while ( *hkey && du )
    {
        GetData(*hkey,sval,in,len,def,&du);
        hkey++;
    }

    if ( du )
        GetData(0,sval,in,len,def,NULL);

    return in;
}


/******************************************************************************/
DWORD GetVal(HKEY hkey,char *sval,DWORD def)
{

    DWORD val = 0;

    GetData(hkey,sval,&val,sizeof(DWORD),&def,NULL);

    return val;
}

/******************************************************************************/
char *GetStr(HKEY hkey,char *sval,char *in,int len,char *def)
{

    GetData(hkey,sval,in,len,def,NULL);
    return in;
}

/*****************************************************************************************************/
DWORD VTime_to_TimeT(VTIME cur)
{

    struct tm tm;
    DWORD t;

    tm.tm_year  =  cur.year + 70;
    tm.tm_mon   =  cur.month;
    tm.tm_mday  =  cur.day;
    tm.tm_hour  =  cur.hour;
    tm.tm_min   =  cur.min;
    tm.tm_sec   =  cur.sec;

    // Let the C-Runtime determine if this is daylight savings time or not
    // by passing -1 into the tm_isdst field

    tm.tm_isdst = -1;

    t = mktime(&tm);
    if ( t == 0xffffffff )
        t = 0;

    return t;
}



BOOL g_bWin9X = FALSE; 
BOOL g_bWinNT = FALSE;
BOOL g_bWin2000 = FALSE;
BOOL bWinsockPresent = FALSE;
HINSTANCE g_hKernel32 = NULL;
HINSTANCE g_hAdvapi32 = NULL;

// WinNT/2000 specific functions
typedef SC_HANDLE (WINAPI* LPFNOPENSCMANAGER)(LPCTSTR lpMachineName, LPCTSTR lpDatabaseName, DWORD dwDesiredAccess);
typedef SC_HANDLE (WINAPI* LPFNCREATESERVICE)(SC_HANDLE hSCManager, LPCTSTR lpServiceName, LPCTSTR lpDisplayName, DWORD dwDesiredAccess, DWORD dwServiceType, DWORD dwStartType, DWORD dwErrorControl, LPCTSTR lpBinaryPathName, LPCTSTR lpLoadOrderGroup, LPDWORD lpdwTagId, LPCTSTR lpDependencies, LPCTSTR lpServiceStartName, LPCTSTR lpPassword);
typedef BOOL (WINAPI* LPFNDELETESERVICE)(SC_HANDLE hService);
typedef BOOL (WINAPI* LPFNQUERYSERVICESTATUS)(SC_HANDLE hService, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNSTARTSERVICE)(SC_HANDLE hService, DWORD dwNumServiceArgs, LPCTSTR *lpServiceArgVectors);
typedef BOOL (WINAPI* LPFNSTARTSERVICECTRLDISPATCHER)(LPSERVICE_TABLE_ENTRY lpServiceStartTable);
typedef SERVICE_STATUS_HANDLE (WINAPI* LPFNREGISTERSERVICECTRLHANDLER)(LPCTSTR lpServiceName, LPHANDLER_FUNCTION lpHandlerProc);
typedef BOOL (WINAPI* LPFNCONTROLSERVICE)(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNSETSERVICESTATUS)(SERVICE_STATUS_HANDLE hServiceStatus, LPSERVICE_STATUS lpServiceStatus);
typedef BOOL (WINAPI* LPFNCLOSESERVICEHANDLE)(SC_HANDLE hSCObject);
typedef HANDLE (WINAPI* LPFNCREATEWAITABLETIMER)(LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCTSTR lpTimerName);
typedef BOOL (WINAPI* LPFNSETWAITABLETIMER)(HANDLE hTimer, const LARGE_INTEGER *pDueTime, LONG lPeriod, PTIMERAPCROUTINE pfnCompletionRoutine, LPVOID lpArgToCompletionRoutine, BOOL fResume);
typedef BOOL (WINAPI* LPFNCANCELWAITABLETIMER)(HANDLE hTimer);
 
LPFNOPENSCMANAGER lpfnOpenSCManager = NULL;
LPFNCREATESERVICE lpfnCreateService = NULL;
LPFNDELETESERVICE lpfnDeleteService = NULL;
LPFNQUERYSERVICESTATUS lpfnQueryServiceStatus = NULL;
LPFNSTARTSERVICE lpfnStartService = NULL;
LPFNSTARTSERVICECTRLDISPATCHER lpfnStartServiceCtrlDispatcher = NULL;
LPFNREGISTERSERVICECTRLHANDLER lpfnRegisterServiceCtrlHandler = NULL;
LPFNCONTROLSERVICE lpfnControlService = NULL;
LPFNSETSERVICESTATUS lpfnSetServiceStatus = NULL;
LPFNCLOSESERVICEHANDLE lpfnCloseServiceHandle = NULL;
LPFNCREATEWAITABLETIMER lpfnCreateWaitableTimer = NULL;
LPFNSETWAITABLETIMER lpfnSetWaitableTimer = NULL;
LPFNCANCELWAITABLETIMER lpfnCancelWaitableTimer = NULL;

//Win9X specific functions
typedef HANDLE (WINAPI* LPFNCREATETOOLHELP32SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI* LPFNPROCESS32FIRST)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI* LPFNPROCESS32NEXT)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef DWORD (WINAPI* LPFNREGISTERSERVICEPROCESS)(DWORD dwProcessId, DWORD dwType);

LPFNCREATETOOLHELP32SNAPSHOT lpfnCreateToolhelp32Snapshot = NULL;
LPFNPROCESS32FIRST lpfnProcess32First = NULL;
LPFNPROCESS32NEXT lpfnProcess32Next = NULL;
LPFNREGISTERSERVICEPROCESS lpfnRegisterServiceProcess = NULL;

//
//  FUNCTION: CheckWindowsVersion
//
//  PURPOSE: determine Windows platform - 2000, NT or 9X
//
//  RETURN VALUE:  BOOL for success in determining platform
//    
//  COMMENTS:  Sets global variables g_bWin2000, g_bWinNT and g_bWin9X
//
BOOL CheckWindowsVersion()
{
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

    GetVersionEx( &ver );

	if(ver.dwMajorVersion >= 5)
		g_bWin2000 = TRUE;
	else if(ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
		g_bWinNT = TRUE;
	else if(ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		g_bWin9X = TRUE;

	if(g_bWin2000 || g_bWinNT || g_bWin9X)
		return TRUE;

    return FALSE;
}


//
//  FUNCTION: DeinitWindowsLibraries
//
//  PURPOSE: decrement reference count on any librariesload any libraries specific to Windows version
//
//  RETURN VALUE:  BOOL for success/failure
//    
//
BOOL DeinitWindowsLibraries()
{
	BOOL bFreedKernel32 = TRUE;
	BOOL bFreedAdvapi32 = TRUE;

	if(g_bWin2000 || g_bWinNT)
	{
		if(g_hKernel32)
			bFreedKernel32 = FreeLibrary(g_hKernel32);
		if(g_hAdvapi32)
			bFreedAdvapi32 = FreeLibrary(g_hAdvapi32);
	}
	else
	{
		if(g_hKernel32)
			bFreedKernel32 = FreeLibrary(g_hKernel32);
	}
	return (bFreedAdvapi32 && bFreedKernel32);
}

//
//  FUNCTION: InitNTLibraries
//
//  PURPOSE: load libraries and save entrypoints to 
//			 NT specific functions.
//
//  RETURN VALUE:  BOOL for success in obtaining all entrypoints
//    
//  COMMENTS:  LoadLibrary/GetProcAddress permits linking one
//             app for all versions.  Some Win9X versions may 
//             link with NT specific functions, but this behavior
//             does not appear to be consistent across all Win9X
//             versions.  Better to play it safe.
//
BOOL InitNTLibraries()
{
	BOOL bKernel32LoadError = FALSE;
	BOOL bAdvapi32LoadError = FALSE;

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\advapi32.dll"));
	g_hAdvapi32 = LoadLibrary(szPath);
	if(!g_hAdvapi32) 
	{
		return FALSE;
	}

	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\Kernel32.dll"));
    g_hKernel32 = LoadLibrary( szPath );
    if( g_hKernel32 == NULL ) 
	{
		FreeLibrary(g_hAdvapi32);
		return FALSE;
	}

#ifdef _UNICODE
	lpfnOpenSCManager = (LPFNOPENSCMANAGER)GetProcAddress(g_hAdvapi32, _T("OpenSCManagerW"));
	lpfnCreateService = (LPFNCREATESERVICE)GetProcAddress(g_hAdvapi32, _T("CreateServiceW"));
	lpfnStartService = (LPFNSTARTSERVICE)GetProcAddress(g_hAdvapi32, _T("StartServiceW));
	lpfnStartServiceCtrlDispatcher = (LPFNSTARTSERVICECTRLDISPATCHER)GetProcAddress(g_hAdvapi32, _T("StartServiceCtrlDispatcherW"));
	lpfnRegisterServiceCtrlHandler = (LPFNREGISTERSERVICECTRLHANDLER)(GetProceAddress(g_hAdvapi32, _T("RegisterServiceCtrlHandlerW"));
	lpfnCreateWaitableTimer = (LPFNCREATEWAITABLETIMER)GetProcAddress(g_hKernel32, _T("CreateWaitableTimerW"));
#else
	lpfnOpenSCManager = (LPFNOPENSCMANAGER)GetProcAddress(g_hAdvapi32, _T("OpenSCManagerA"));
	lpfnCreateService = (LPFNCREATESERVICE)GetProcAddress(g_hAdvapi32, _T("CreateServiceA"));
	lpfnStartService = (LPFNSTARTSERVICE)GetProcAddress(g_hAdvapi32, _T("StartServiceA"));
	lpfnStartServiceCtrlDispatcher = (LPFNSTARTSERVICECTRLDISPATCHER)GetProcAddress(g_hAdvapi32, _T("StartServiceCtrlDispatcherA"));
	lpfnRegisterServiceCtrlHandler = (LPFNREGISTERSERVICECTRLHANDLER)GetProcAddress(g_hAdvapi32, _T("RegisterServiceCtrlHandlerA"));
	lpfnCreateWaitableTimer = (LPFNCREATEWAITABLETIMER)GetProcAddress(g_hKernel32, _T("CreateWaitableTimerA"));
#endif
	lpfnDeleteService = (LPFNDELETESERVICE)GetProcAddress(g_hAdvapi32, _T("DeleteService"));
	lpfnQueryServiceStatus = (LPFNQUERYSERVICESTATUS)GetProcAddress(g_hAdvapi32, _T("QueryServiceStatus"));
	lpfnControlService = (LPFNCONTROLSERVICE)GetProcAddress(g_hAdvapi32, _T("ControlService"));
	lpfnSetServiceStatus = (LPFNSETSERVICESTATUS)GetProcAddress(g_hAdvapi32, _T("SetServiceStatus"));
	lpfnCloseServiceHandle = (LPFNCLOSESERVICEHANDLE)GetProcAddress(g_hAdvapi32, _T("CloseServiceHandle"));
	lpfnSetWaitableTimer = (LPFNSETWAITABLETIMER)GetProcAddress(g_hKernel32, _T("SetWaitableTimer"));
	lpfnCancelWaitableTimer = (LPFNCANCELWAITABLETIMER)GetProcAddress(g_hKernel32, _T("CancelWaitableTimer"));

	if(!lpfnOpenSCManager || 
	   !lpfnCreateService || 
	   !lpfnDeleteService || 
	   !lpfnQueryServiceStatus || 
	   !lpfnStartService || 
	   !lpfnStartServiceCtrlDispatcher ||
	   !lpfnRegisterServiceCtrlHandler ||
	   !lpfnControlService || 
	   !lpfnSetServiceStatus || 
	   !lpfnCloseServiceHandle)
		bAdvapi32LoadError = TRUE;

	if(!lpfnCreateWaitableTimer ||
	   !lpfnSetWaitableTimer ||
	   !lpfnCancelWaitableTimer)
		bKernel32LoadError = TRUE;

	if(bAdvapi32LoadError || bKernel32LoadError)
	{
		FreeLibrary(g_hKernel32);
		FreeLibrary(g_hAdvapi32);

		if(bKernel32LoadError)
		{
		}

		if(bAdvapi32LoadError)
		{
		}
		return FALSE;
	}
	return TRUE;
}

//
//  FUNCTION: InitWin9XLibraries
//
//  PURPOSE: load Win9X specific libraries and save entrypoints 
//			 to needed functions.
//
//  RETURN VALUE:  BOOL for success in obtaining all entrypoints
//    
//  COMMENTS:  LoadLibrary/GetProcAddress permits linking one
//             app for all versions.  Some WinNT/2000 versions may 
//             link with Win9X specific functions, but this behavior
//             does not appear to be consistent across all versions.
//			   Better to play it safe.
//
//			   Uses global g_hKernel32.
//
BOOL InitWin9XLibraries()
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\Kernel32.dll"));
    g_hKernel32 = LoadLibrary( szPath );
    if( g_hKernel32 == NULL ) 
	{
		return FALSE;
	}

#ifdef _UNICODE
	lpfnProcess32First = (LPFNPROCESS32FIRST)GetProcAddress(g_hKernel32, _T("Process32FirstW"));
	lpfnProcess32Next = (LPFNPROCESS32NEXT)GetProcAddress(g_hKernel32, _T("Process32NextW"));
#else
	lpfnProcess32First = (LPFNPROCESS32FIRST)GetProcAddress(g_hKernel32, _T("Process32First"));
	lpfnProcess32Next = (LPFNPROCESS32NEXT)GetProcAddress(g_hKernel32, _T("Process32Next"));
#endif
	lpfnCreateToolhelp32Snapshot = (LPFNCREATETOOLHELP32SNAPSHOT)GetProcAddress(g_hKernel32, _T("CreateToolhelp32Snapshot"));
	lpfnRegisterServiceProcess = (LPFNREGISTERSERVICEPROCESS)GetProcAddress(g_hKernel32, _T("RegisterServiceProcess"));

	if(!lpfnCreateToolhelp32Snapshot || !lpfnProcess32First || !lpfnProcess32Next || !lpfnRegisterServiceProcess) 
	{
		FreeLibrary(g_hKernel32);
		return FALSE;
	}
	return TRUE;
}

//
//  FUNCTION: InitWindowsLibraries
//
//  PURPOSE: load any libraries specific to Windows version
//
//  RETURN VALUE:  BOOL for success/failure
//    
//
BOOL InitWindowsLibraries()
{
	BOOL bInitialized = FALSE;

	if(!CheckWindowsVersion())
		return FALSE;

	if(g_bWin2000 || g_bWinNT)
		bInitialized = InitNTLibraries();
	else 
		bInitialized = InitWin9XLibraries();

	return bInitialized;
}

// here are my audit criteria, they need to get to the packet handler somehow

//  j,  f,  m,  a,  m,  j,  j,  a,  s,  o,  n,  d
int nDaysPerMonth[] = { 31, 29, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31};

// this converts time to something linear - it doesn't need to be exact, as long as everybody uses the same scale
// and the subranges don't overlap

int YearMonthDaysToLinear( int nYear, int nMonth, int nDays )
{
    int nLinearReturned;

    if ( nMonth > 0 && nMonth <= 12 )
    {
        nLinearReturned = (nYear * 366 + nMonth) * 31 + nDays;
    }
    else
    {
        nLinearReturned = 0;
    }

    return nLinearReturned;
}

// converts the product version to something linear

int VersionSubverBuildToLinear( int nVersion, int nSubver, int nBuild )
{
    int nLinearReturned = (nVersion * 100 + nSubver) * 65536 + nBuild;

    return nLinearReturned;
}

// return values for the following 3 functions

// Handle the Audit checks and verifications for a received LDVP Pong packet

int ProcessLDVPPongPacket( PDESIRED_STATE pDesiredState, PACTUAL_STATE pActualState, PONGDATA *lpPongData, int nPongDataSize )
{
    DWORD dwRet = ERROR_SUCCESS;

    if ( nPongDataSize >= sizeof(PONGDATA) )
    {
        // process pong values

        DWORD PattVer = lpPongData->PatternVersion;
        pActualState->nPattVerYear = (unsigned short)( PattVer >> 18) + 1998;
        pActualState->nPattVerMonth= (unsigned short)(( PattVer >> 14) & 0x0f);
        pActualState->nPattVerDay  = (unsigned short)(( PattVer >> 9) & 0x1f);
        pActualState->nPattVerRev  = (unsigned short)( PattVer & 0x1FF);

        pActualState->dwEngineVersion = lpPongData->EngineVersion;

        pActualState->nProductVersion = (lpPongData->ProductVersion & 0xffff) / 100;
        pActualState->nProductSubver = (lpPongData->ProductVersion & 0xffff) % 100;
        pActualState->nProductBuild = lpPongData->ProductVersion >> 16;

        pActualState->bSCFInstalledAndManaged = (lpPongData->Flags & PF_SCF_INSTALLED) != 0;
        pActualState->bSCFEnabled = (lpPongData->Flags & PF_SCF_DISABLED) == 0;
        pActualState->bSCFLastPolicyOK = (lpPongData->Flags & PF_SCF_POLICY_ERROR) == 0;

        // at this point we apply whatever criteria we were given for versions,
        // dates, etc.

        int nMinDate = YearMonthDaysToLinear( pDesiredState->nMinPattVerYear, pDesiredState->nMinPattVerMonth, pDesiredState->nMinPattVerDay );
        int nMaxDate = YearMonthDaysToLinear( pDesiredState->nMaxPattVerYear, pDesiredState->nMaxPattVerMonth, pDesiredState->nMaxPattVerDay );

        int nPattDate = YearMonthDaysToLinear( pActualState->nPattVerYear, pActualState->nPattVerMonth, pActualState->nPattVerDay );

        int nMinVersion = VersionSubverBuildToLinear( pDesiredState->nMinProductVersion, pDesiredState->nMinProductSubver, pDesiredState->nMinProductBuild );
        int nMaxVersion = VersionSubverBuildToLinear( pDesiredState->nMaxProductVersion, pDesiredState->nMaxProductSubver, pDesiredState->nMaxProductBuild );

        int nVersion = VersionSubverBuildToLinear( pActualState->nProductVersion, pActualState->nProductSubver, pActualState->nProductBuild );

        if ( nPattDate >= nMinDate && nPattDate <= nMaxDate &&
             // nVersion >= nMinVersion && nVersion <= nMaxVersion &&
             pActualState->dwEngineVersion >= pDesiredState->dwMinEngineVersion && 
             pActualState->dwEngineVersion <= pDesiredState->dwMaxEngineVersion )
        {
            dwRet = PASSED_SETTINGS_AUDIT;

            if ( pDesiredState->bAuditSymantecClientFirewall )
            {
                if ( pDesiredState->bAuditSymantecClientFirewall )
                {
                    if ( pActualState->bSCFInstalledAndManaged &&
                         pActualState->bSCFEnabled &&
                         pActualState->bSCFLastPolicyOK )
                    {
                        dwRet = PASSED_SETTINGS_AUDIT;
                    }
                    else
                    {
                        dwRet = FAILED_SETTINGS_AUDIT;
                    }
                }
            }
        }
        else
        {
            dwRet = FAILED_SETTINGS_AUDIT;
        }
    }

    return dwRet;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:	SCF_GetSCFManagementData
//
// Description:		Retrieves SCF configuration data.
//
// Parameters:      SCFMGMTDATA* _scf_data - pointer to an SCF info block.
//
// Return Values:	None.
//
///////////////////////////////////////////////////////////////////////////////
void SCF_GetSCFManagementData( SCFMGMTDATA* _scf_data )
{
#ifdef WIN32
	HKEY _scf_managed = NULL;
	char _buffer[IMAX_PATH];
	VTIME _zerotime = {0};

	memset( _scf_data, 0x00, sizeof(SCFMGMTDATA) );

	// Get managed SCF status from various Registry locations.

	// "Software\\Symantec\\Managed Products\\Symantec Internet Security" key
	sssnprintf( _buffer, sizeof(_buffer), "%s\\%s", szReg_Key_Sym_ManagedProducts, szReg_Key_Sym_MP_SCF );
	if( RegOpenKey( HKEY_LOCAL_MACHINE, _buffer, &_scf_managed ) == ERROR_SUCCESS )
	{
		// "Managed"
		_scf_data->m_is_managed = GetVal( _scf_managed, szReg_Val_Sym_MP_Managed, 0 ) == 0 ? FALSE : TRUE;
		if( _scf_data->m_is_managed == TRUE )
		{
			// SCF is managed get the rest of the data.
			HKEY _temp = NULL;

			// "Version"
			GetData( _scf_managed, szReg_Val_Sym_MP_Version, &_scf_data->m_version, sizeof(_scf_data->m_version), NULL, NULL );

			// "Status" // must be == 1 to be considered enabled
			_scf_data->m_is_enabled = GetVal( _scf_managed, szReg_Val_Sym_MP_Status, 0 ) == 1 ? TRUE : FALSE;

			// "$\ManagedProducts\SCF"
			sssnprintf( _buffer, sizeof(_buffer), "%s\\%s", szReg_Key_ManagedProducts, szReg_Key_SCF );
			if( RegOpenKey( hMainKey, _buffer, &_temp ) == ERROR_SUCCESS )
			{
				// "SCFPolicyUpdateTime"
				GetData( _temp, szReg_Val_SCFPolicyUpdateTime, &_scf_data->m_current_policy, sizeof(_scf_data->m_current_policy), &_zerotime, NULL );

				// "SCFPolicyUpdateLastRetCode"
				_scf_data->m_last_policy_import_ret_code = GetVal( _temp, szReg_Val_SCFPolicyUpdateLastRC, SCF_SETTINGS_INTEGRATOR_SUCCESS_CODE );

				// "LastPolicyFail"
				GetData( _temp, szReg_Val_SCFPolicyLastFail, &_scf_data->m_last_policy_fail, sizeof(_scf_data->m_current_policy), &_zerotime, NULL );

				// "LastPolicyFailCount"
				_scf_data->m_last_policy_fail_count = GetVal( _temp, szReg_Val_SCFPolicyLastFailCount, 0 );

				// "MaxFailCountForPolicy"
				_scf_data->m_last_policy_fail_count_max = GetVal( _temp, szReg_Val_SCFPolicyMaxFailCount, DEFAULT_MAX_POLICY_FAIL_COUNT );

				// "SCFPolicyOriginalFileName"
				GetStr( _temp, szReg_Val_SCFPolicyOriginalFileName, _scf_data->m_current_policy_file_name, sizeof(_scf_data->m_current_policy_file_name), NULL );

				RegCloseKey( _temp );
			}
		}

		RegCloseKey( _scf_managed );
	}
#else
	// SCF Managability is not supported on other platforms
	memset( _scf_data, 0x00, sizeof(SCFMGMTDATA) );
#endif // WIN32
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name:	SCF_GetSCFPongData
//
// Description:		Retrieves SCF specific data for the PONG packet.
//
// Return Values:	None.
//
///////////////////////////////////////////////////////////////////////////////
void SCF_GetSCFPongData( PONGDATA* _pong )
{
	// Get the SCF management data
	SCFMGMTDATA _mgmt;
	SCF_GetSCFManagementData( &_mgmt );

	// If SCF is installed, get its data, else leave it all set to zero.
	if( _mgmt.m_is_managed == TRUE )
	{
		// SCF is installed (and managed).
		_pong->Flags |= PF_SCF_INSTALLED;

		// Is SCF enabled?
		if( _mgmt.m_is_enabled == FALSE )
			_pong->Flags |= PF_SCF_DISABLED;

		// SCF Version.
		_pong->SCFVersion = _mgmt.m_version;

		// Last update time for SCF Settings.
		_pong->SCFSettingsUpdateTime = _mgmt.m_current_policy;

		// Was there an error on the last SCF Policy update
		if( _mgmt.m_last_policy_import_ret_code != SCF_SETTINGS_INTEGRATOR_SUCCESS_CODE )
			_pong->Flags |= PF_SCF_POLICY_ERROR;
	}
}

DWORD VTime2TimeT(VTIME cur)
{

    struct tm tm;
    DWORD t;

    tm.tm_year  =  cur.year + 70;
    tm.tm_mon   =  cur.month;
    tm.tm_mday  =  cur.day;
    tm.tm_hour  =  cur.hour;
    tm.tm_min   =  cur.min;
    tm.tm_sec   =  cur.sec;
    // Let the C-Runtime determine if this is daylight savings time or not
    // by passing -1 into the tm_isdst field (CDR) (SCR37830 & SCR37802)
    tm.tm_isdst = -1;

    t = mktime(&tm);
    if ( t == 0xffffffff )
        t = 0;

    return t;
}


DWORD DaysOfVTime(VTIME *vt)
{

    DWORD t = VTime2TimeT(*vt);

#ifndef NLM    
    // Adjust def timestamp for local timezone
    t += ( PST_GMT_OFFSET - _timezone );
#endif    
    return t / (60*60*24);
}

PONGDATA *GetPongData(PONGDATA *pong)
{

    HKEY hkey;
    VTIME vt;

    memset(pong,0,sizeof(PONGDATA));

    pong->PatternVersion = GetVal(hMainKey,"UsingPattern",0);
    GetData(hMainKey,"PatternFileDate",&vt,sizeof(vt),NULL,NULL);
    pong->PatternFileDate = DaysOfVTime(&vt);
    // Get NAV daily rev number and IBM sequence number
    pong->PatternRevision = GetVal(hMainKey,"PatternFileRevision",0);
    pong->PatternSequence = GetVal(hMainKey,"PatternFileSequence",0);

#if defined NLM
    pong->Flags |= PF_PLATFORM_NLM;

/*this is how NW6 version is presented
	if( ( NWversion == 5 ) && ( NWsubVersion == 0x3c ) )
	{
		NWversion = 6;
	}
*/
    switch ( NWversion )
    {
        case 3: pong->Flags |= PF_NETWARE_3; break;
        case 4: pong->Flags |= PF_NETWARE_4; break;
        case 5:
				if(  NWsubVersion == 0x3c  )
				{
					pong->Flags |= PF_NETWARE_6;
					break;
				}
				else
				{
        			pong->Flags |= PF_NETWARE_5;
        			break;
				}
    }
#elif defined WINNT
    pong->Flags |= PF_PLATFORM_WINNT;
#elif defined WIN95
    pong->Flags |= PF_PLATFORM_WIN95;
#endif // WIN95

    // see if the client needs full definitions for some reason, either backrev too far back
    // or microdefs won't/didn't work for some reason, or both

    if ( GetVal(hMainKey,"ManageClients",0) )
        pong->Flags |= PF_SUPPORT_CLIENTS;

    if ( GetVal(hProductControlKey,"ManageThisComputer",0) == 0 )
    {
        strcpy(pong->DomainName,"CLIENT_SET");
		GetStr(hMainKey,"ClientGroup",pong->GroupName,sizeof(pong->GroupName),"");
        if ( GetVal(hPattManKey,"UpdateClients",1) )
            pong->Flags |= PF_WANT_PATTERN_UPDATES;
    }
    else
    {
        GetStr(hMainKey,"DomainName",pong->DomainName,sizeof(pong->DomainName),"DEFDOMAIN");
        GetStr(hMainKey,"Parent",pong->Mom,sizeof(pong->Mom),"");
        if ( RegOpenKey(hMainKey,szReg_Key_DomainData,&hkey) == ERROR_SUCCESS )
        {
            pong->Flags |= PF_PRIMARY;
            if ( GetVal(hPattManKey,"UpdateChildren",0) )
                pong->Flags |= PF_SHARE;
            RegCloseKey(hkey);
        }
    }
//    GetStr(hMainKey,"LicenseNumber",pong->License,sizeof(pong->License),"");
    GetStr(hMainKey,"ConsolePassword",pong->ConsolePassword,sizeof(pong->ConsolePassword),"");
    GetStr(hMainKey,"ScanEngineVendor",pong->ScanEngineVendor,sizeof(pong->ScanEngineVendor),"NAV");
	pong->ScanEngineVendor[sizeof(pong->ScanEngineVendor) - 1] = 0x00; // make sure that this is NULL terminated

    pong->ServiceRelease = GetVal(hMainKey,"ServiceRelease",0);

    GetData(hMainKey,"TimeOfLastVirus",&pong->TimeOfLastVirus,sizeof(VTIME),NULL,NULL);
    pong->InstalledProducts = GetVal(hMainKey,"InstalledProducts",1);
    pong->TransportVersion = TRANSPORT_VERSION;
    GetData(hMainKey,"TimeOfLastSCan",&pong->TimeOfLastScan,sizeof(VTIME),NULL,NULL);

	// If we are a Server, report our GRCGRPUpdateTime instead of GRCUpdateTime
	if( GetVal( hMainKey, "Type", TYPE_IS_CLIENT ) == TYPE_IS_SERVER )
		GetData(hMainKey, szReg_Key_AllGroups "\\GRCGRPUpdateTime",&pong->GRCTime,sizeof(VTIME),NULL,NULL);
	else
		GetData(hMainKey,"GRCUpdateTime",&pong->GRCTime,sizeof(VTIME),NULL,NULL);

    pong->MySize = sizeof(PONGDATA);
    memset(pong->GuidData,0,16);
    GetData(hMainKey,"GUID",pong->GuidData,16,NULL,NULL);

    pong->EngineVersion = GetVal(hMainKey,"ScanEngineVersion",0);
    pong->ComPatNum = COM_PAT_NUM;

	// SCF management info.
	SCF_GetSCFPongData( pong );

	// if FileSystem Real Time protection is disabled, add that fact to the Flags
	if( GetVal( hMainKey, "Storages\\FileSystem\\RealTimeScan\\OnOff", 0 ) == 0 )
		pong->Flags |= PF_FILESYSREALTIME_DISABLED;

   return pong;
}

                       
DWORD InitRegKeys( )
{
    // set up all the important reg keys so I don't contantly be reading the registry

    DWORD cc = RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
    if ( cc != ERROR_SUCCESS )
        return cc;

    RegOpenKey(hMainKey, "PatternManager", &hPattManKey);
    RegOpenKey(hMainKey, "ProductControl", &hProductControlKey);
    RegOpenKey(hMainKey, "ClientScan", &hClientKey);
    RegOpenKey(hMainKey, "ForwardScan", &hForwardKey);
    RegOpenKey(hMainKey, "Quarantine", &hQuarantineKey);
    RegOpenKey(hMainKey, "SystemScan", &hSystemKey);
    RegOpenKey(hMainKey, "Common", &hCommonKey);

    return 0;
}

void DeInitRegKeys( )
{
    RegCloseKey( hMainKey );

    RegCloseKey( hPattManKey );
    RegCloseKey( hClientKey );
    RegCloseKey( hForwardKey );
    RegCloseKey( hQuarantineKey );
    RegCloseKey( hSystemKey );
    RegCloseKey( hCommonKey );
	RegCloseKey( hProductControlKey );
}

TCHAR szValueListKey[ ] = _T("Storages\\Filesystem\\RealTimeScan\\" );
TCHAR *szValueList[ ] = {
    _T("OnOff" ),
    _T("Writes" ),
    _T("Execs" ),
    _T("Reads" ),
    _T("FileType" ),
    _T("Exts" ),
    _T("Heuristics" ),
    _T("HeuristicsLevel" )};

DWORD dwValueListDefaults[ ] = {
    1,                      // _T("OnOff" ),         
    1,                      // _T("Writes" ),        
    1,                      // _T("Execs" ),        // not actually used
    1,                      // _T("Reads" ),         
    1,                      // _T("FileType" ),      
    0,                      // _T("Exts" ),         // this is a string, the default value is empty
    1,                      // _T("Heuristics" ),    
    2,                      // _T("HeuristicsLevel" )
};

int ProcessSAVSettings( PDESIRED_STATE pDesiredState, PACTUAL_STATE pActualState )
{
    DWORD dwRet;

	HKEY hValueListKey;
	RegOpenKey(hMainKey, szValueListKey, &hValueListKey);

	TCHAR szExts[1024];

	TCHAR *szExtExport = NULL;

    pActualState->dwOnOff = GetVal( hValueListKey, szValueList[0], dwValueListDefaults[0] );
    pActualState->dwWrites		= GetVal( hValueListKey, szValueList[1], dwValueListDefaults[1] );
    pActualState->dwExecs		= GetVal( hValueListKey, szValueList[2], dwValueListDefaults[2] );
    pActualState->dwReads		= GetVal( hValueListKey, szValueList[3], dwValueListDefaults[3] );
    pActualState->dwFileType	= GetVal( hValueListKey, szValueList[4], dwValueListDefaults[4] );
    GetStr( hValueListKey, szValueList[5], szExts, sizeof( szExts ), (char *) dwValueListDefaults[5] );

	szExtExport = (TCHAR *) malloc( sizeof( szExts ) );
	memcpy( szExtExport, szExts, sizeof( szExts ) );
	pActualState->szExts = szExtExport;

    pActualState->dwHeuristics	= GetVal( hValueListKey, szValueList[6], dwValueListDefaults[6] );
    pActualState->dwHeuristicsLevel = GetVal( hValueListKey, szValueList[7], dwValueListDefaults[7] );

    // if we are scanning extensions, but there are no extensions, change to
    // scan all files - this was added for 7.5, I think

    if ( pActualState->dwFileType == 1 && pActualState->szExts == NULL )
    {
        pActualState->dwFileType = 0;
    }

    // now check the criteria

    if ( pActualState->dwOnOff == pDesiredState->dwExpectedOnOff &&
         pActualState->dwWrites == pDesiredState->dwExpectedWrites &&
         // pActualState->dwExecs == dwExpectedExecs &&		// don't check these - they don't mean anything
         pActualState->dwReads == pDesiredState->dwExpectedReads &&
         pActualState->dwFileType == pDesiredState->dwExpectedFileType &&
         pActualState->dwHeuristics == pDesiredState->dwExpectedHeuristics && 
         pActualState->dwHeuristicsLevel >= pDesiredState->dwMinHeuristicsLevel )
    {
        dwRet = PASSED_NAVAP_AUDIT;
    }
    else
    {
        dwRet = FAILED_NAVAP_AUDIT;
    }

    return dwRet;
}


BOOL Win9XIsNAVRunning(BOOL& bRunning)
{
	HKEY	pKey = NULL;
	BOOL	bSuccess = FALSE; // function executed successfully?
	DWORD	dwRunning = 0,
			dwSize = sizeof( DWORD );

	//Find out if the service is running by looking at the registry key
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_READ,
										&pKey) )
	{
		//Read the ServiceRunning value
		if( ERROR_SUCCESS == SymSaferRegQueryValueEx(	pKey,
							 szReg_Val_ServiceRunning,
							 NULL,
							 NULL,
							 (BYTE*)&dwRunning,
							 &dwSize	)) {
			bSuccess = TRUE;
			bRunning = (BOOL)(dwRunning != KEYVAL_NOTRUNNING);	
		}
		RegCloseKey( pKey );
	}
	return bSuccess;	
}
	

BOOL IsServiceRunning(SC_HANDLE schSCManager,LPCTSTR szService)
{
    SC_HANDLE   schService;
    SERVICE_STATUS  status;

    schService = OpenService(schSCManager, szService, SERVICE_QUERY_STATUS);

    if ( schService == NULL )
        return FALSE;

    lpfnQueryServiceStatus(schService, &status);

    lpfnCloseServiceHandle(schService);

    return(status.dwCurrentState == SERVICE_RUNNING);
}


BOOL IsNAVRunning( )
{
    BOOL bRet = FALSE;
	BOOL bRunning = FALSE;
	DWORD dwProcessID = 0;
    SC_HANDLE   schSCManager;

	if(!g_bWin9X)
	{
		schSCManager = lpfnOpenSCManager(
									NULL,                   // machine (NULL == local)
									NULL,                   // database (NULL == default)
									SC_MANAGER_ALL_ACCESS   // access required
									);
		if ( schSCManager )
		{
	        bRet = IsServiceRunning( schSCManager, _T(SERVICE_NAME) );
	        lpfnCloseServiceHandle(schSCManager);
		}
	}
	else // Win9X
	{
		Win9XIsNAVRunning(bRunning);
		bRet = bRunning;
	}
    return bRet;
}

DWORD VerifySAVRunning( PDESIRED_STATE pDesiredState, PACTUAL_STATE pActualState )
{
	(void) pDesiredState;
	(void) pActualState;

	return IsNAVRunning( ) ? PASSED_SAVAP_RUNNING : FAILED_SAV_SERVICE_NOT_RUNNING;
}



DWORD VerifySCFRunning( PDESIRED_STATE pDesiredState, PACTUAL_STATE pActualState )
{
	(void) pDesiredState;
	(void) pActualState;

	return PASSED_SCF_RUNNING;
}



DWORD RunLiveUpdate( PDESIRED_STATE pDesiredState, PACTUAL_STATE pActualState )
{
	(void) pDesiredState;
	(void) pActualState;

	return PASSED_LIVE_UPDATE_AUDIT;
}

BOOL FreeActualState( PACTUAL_STATE pActualState )
{
    if ( pActualState )
    {
        if ( pActualState->szExts )
        {
            free( pActualState->szExts );
			pActualState->szExts = NULL;
        }
    }

	free( pActualState );

	return TRUE;
}



SCSAUDIT_API BOOL AuditSCS( PDESIRED_STATE pDesiredState, PACTUAL_STATE *ppActualState, LPDWORD lpdwResult )
{
	DWORD dwRet = FAILED_NAVAP_AUDIT;
	BOOL  bRet = FALSE;

	PACTUAL_STATE pActualState = ( PACTUAL_STATE ) malloc( sizeof( ACTUAL_STATE ) );
	if ( pActualState )
	{
		memset( pActualState, 0, sizeof( ACTUAL_STATE ) );

		*ppActualState = pActualState;

		if ( InitRegKeys( ) == ERROR_SUCCESS )
		{
            if ( InitWindowsLibraries( ) )
            {
    			PONGDATA Pong;
    
    			GetPongData( &Pong );
    
    			dwRet = ProcessLDVPPongPacket( pDesiredState, pActualState, &Pong, sizeof( Pong ) );
    			if ( dwRet == PASSED_SETTINGS_AUDIT )
    			{
    				dwRet = ProcessSAVSettings( pDesiredState, pActualState );
    				if ( dwRet == PASSED_NAVAP_AUDIT )
    				{
    					dwRet = VerifySAVRunning( pDesiredState, pActualState );
    					if ( dwRet == PASSED_SAVAP_RUNNING )
    					{   
                            if ( pDesiredState->bAuditSymantecClientFirewall )
                            {
                                dwRet = VerifySCFRunning( pDesiredState, pActualState );
                                if ( dwRet != PASSED_SCF_RUNNING )
                                {
                                    goto AllDone;
                                }
                            }

    						if ( pDesiredState->bRunLiveUpdate )
    						{
    							dwRet = RunLiveUpdate( pDesiredState, pActualState );
    						}
    					}
    				}
    			}

AllDone:

                DeinitWindowsLibraries( );
            }

			DeInitRegKeys( );
		}
	}
	else
	{
		dwRet = ERROR_NOT_ENOUGH_MEMORY;
	}

	*lpdwResult = dwRet;

	bRet = ( dwRet == PASSED_SAVAP_RUNNING || 
             dwRet == PASSED_SCF_RUNNING ||
             dwRet == PASSED_LIVE_UPDATE_AUDIT ); 

	return bRet;
}
