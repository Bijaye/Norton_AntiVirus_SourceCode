// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// DoScan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "vpcommon.h"
#include "DoScan.h"
#include "clientreg.h"
#include "vpstrutils.h"
#include "ScanShared.h"
#include "shfolder.h"

#define szReg_Value_CmdLineScan "CmdLineScan"

#ifndef array_sizeof
#define array_sizeof(x) (sizeof(x)/sizeof((x)[0]))
#endif

IMP_VPCOMMON_IIDS

// Global Variables:
static HINSTANCE s_hInst;                               // current instance
bool             g_bRunning = true;

#include <string>
#include <iostream>
using namespace std;

#define STARTUP_SCANS			"/startupscans"
#define CMDLINE_SCAN			"/cmdlinescan"
#define HAS_UI					"/UI"
#define SCAN_ALL_DRIVES			"/SCANALLDRIVES"
#define HELP_CMD1				"/?"
#define HELP_CMD2				"/h"
#define HELP_CMD3				"/help"
#define HAS_LOG_FILE			"/LOGFILE="
#define DEFAULT_LOG_FILE        "DoScan.log"
#define DEFAULT_LOG_FILE_OLD    "c:\\DoScan.log"

WINOLEAPI CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

// SHFOLDER.DLL
typedef HRESULT (WINAPI *PFNSHGetFoldPathA)(HWND hwndOwner, int nFolder, HANDLE hToken, DWORD dwFlags, LPTSTR pszPath);
#define SHARED_VERSION_DATA_DIRECTORY   _T("7.5")  // This should probably be defined in vpcommon.h

///////////////////////////////////////////////////////////////////////////////
//
// Function name: IsWinNT
//
// Description  : Checks to see if the operating system is Windows NT.
//
// Return Values: TRUE is the OS is WinNT
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL IsWinNT()
{
    OSVERSIONINFO   osInfo;
    BOOL            bRet = FALSE;

    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 

    if ( GetVersionEx (&osInfo) )
    {
        if ( osInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
        {   
            bRet = TRUE;
        }
    }

    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetAppDataLogsDir
//
// Description  : Loads SHFOLDER.DLL and calls SHGetFolderPath() to get the
//                non-roaming application data directory (CSIDL_LOCAL_APPDATA)
//                for the current user.
//
//                Currently, this function requires Windows 2000 or later.
//
// Return Values: ERROR_SUCCESS or other error code.
//
// Argument     : [out]  LPTSTR lpAppDataLogsDir - Buffer to get the directory
//                [in]   size_t nAppDirLogsbytes - size of lpAppDataLogsDir
//
///////////////////////////////////////////////////////////////////////////////
// 3/5/2000 -   TCASHIN: Function created / header added 
//              Function partially borrowed from UserDir.cpp - GetSystemDataDir()
///////////////////////////////////////////////////////////////////////////////
DWORD GetAppDataLogsDir( LPTSTR lpAppDataLogsDir, size_t nAppDirLogsbytes )
{

    auto PFNSHGetFoldPathA pfnSHGetFolderPathA = NULL;

    auto DWORD          dwError = ERROR_SUCCESS;
    auto TCHAR          szAppData[MAX_PATH] = {0};
    auto TCHAR          szSystemDir[MAX_PATH] = {0};
    auto TCHAR          szSHFolderDLL[MAX_PATH] ={0};
    auto HINSTANCE      hFolderDLL = NULL;
    auto HRESULT        hr = S_OK;

    if ( !lpAppDataLogsDir )
    {
        return ERROR_INVALID_PARAMETER;
    }

    if ( IsWinNT() )
    {
        // Get the system directory
        if ( GetSystemDirectory(szSystemDir, sizeof(szSystemDir)) )
        {
            // Append the DLL name
            sssnprintf ( szSHFolderDLL, sizeof(szSHFolderDLL), _T("%s\\shfolder.dll"), szSystemDir );

            // Load it.
            hFolderDLL = LoadLibrary( szSHFolderDLL );

            if ( hFolderDLL )
            {
                // Get the function
                pfnSHGetFolderPathA = (PFNSHGetFoldPathA)GetProcAddress( hFolderDLL, _T("SHGetFolderPathA") );

                if ( pfnSHGetFolderPathA )
                {
                    // Per machine, application
                    hr = pfnSHGetFolderPathA( NULL, CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE, 
                                                  NULL, 0, szAppData);
                }
                    
                if ( SUCCEEDED(hr) )
                {
                    _tcscpy( lpAppDataLogsDir, szAppData ); 
                    sssnprintf( lpAppDataLogsDir, nAppDirLogsbytes,"%s\\%s\\%s\\%s", lpAppDataLogsDir, 
                    SYMANTEC_COMMON_DATA_DIR, 
                    SHARED_VERSION_DATA_DIRECTORY,
                    SYMANTEC_LOG_DIRECTORY);

                    // Let's just double check that this directory exists
                    HANDLE hDir = NULL;
                    WIN32_FIND_DATA finddata = {0};
                    hDir = FindFirstFile( lpAppDataLogsDir, &finddata );
                    
                    if ( hDir && hDir != INVALID_HANDLE_VALUE )
                    {
                        dwError = ERROR_SUCCESS;
                        FindClose( hDir );
                    }
                    else
                        dwError = GetLastError();
                }
                else
                    dwError = GetLastError();
                

                FreeLibrary( hFolderDLL );
            }
            else
                dwError = GetLastError();
        }
        else
            dwError = GetLastError();
    }
    else
    {
        // Not Windows 2000.
        dwError = ERROR_NOT_SUPPORTED;
    }

    return dwError;
}

/**********************************************************************************/
DWORD RegCopyValues(HKEY source,HKEY dest)
{

    DWORD index=0;
    char name[IMAX_PATH];
    DWORD namesize = sizeof(name);
    BYTE data[IMAX_PATH];
    DWORD datasize = sizeof(data);
    DWORD type;
	HRESULT dwRet = E_FAIL;
	BOOL bFoundBadSet = FALSE;

    while ( RegEnumValue(source,index++,name,&namesize,NULL,&type,data,&datasize) == ERROR_SUCCESS )
	{
        if (RegSetValueEx(dest,name,0,type,data,datasize) == ERROR_SUCCESS)
		{
			if (!bFoundBadSet)
				dwRet = ERROR_SUCCESS;
		}
		else 
		{
			bFoundBadSet = TRUE;
			dwRet = E_FAIL;
		}

        namesize = sizeof(name);
        datasize = sizeof(data);
    }

    return dwRet;
}

/**********************************************************************************/
DWORD RegCopyKeys(HKEY source,HKEY dest)
{

    DWORD index = 0;
    char key[IMAX_PATH];
    HKEY hskey,hdkey;

    while ( RegEnumKey(source,index++,key,sizeof(key)) == ERROR_SUCCESS )
    {
        if ( RegCreateKey(dest,key,&hdkey) == ERROR_SUCCESS )
        {
            if ( RegOpenKey(source,key,&hskey) == ERROR_SUCCESS )
            {
                RegCopyKeys(hskey,hdkey);
                RegCloseKey(hskey);
            }
            RegCloseKey(hdkey);
        }
    }

    return RegCopyValues(source,dest);
}

///////////////////////////////////////////////////////////////////////////
//  Implementation of IScanCallback
//  This Inteface is used to pause and continue  Idle scan
interface IScanCallbackImp :  public IScanCallback
{
    LONG m_refcount;

    IScanCallbackImp() 
        : m_refcount(1)
    {}

    ~IScanCallbackImp()
    {}
    
    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&m_refcount);
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG r = InterlockedDecrement(&m_refcount);
        if (r==0)
            delete this;
        return r;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(const IID & riid,void **ppvObj) 
    {
        *ppvObj = NULL;
        if (riid == IID_IScanCallback || riid==IID_IUnknown)
        {
            *ppvObj =this;
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    VI(ULONG) Progress(PPROGRESSBLOCK Progress)
    {
        if (!g_bRunning && Progress->Status != S_DONE && Progress->Status != S_SUSPENDED)
        {
            Progress->Status=S_STOPPING;
            return S_OK;
        }

        if (Progress->Status==S_STARTING) 
            SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_IDLE);

        Relinquish();
        return S_OK;
    }

    VI(ULONG) RTSProgress(PREALTIMEPROGRESSBLOCK Progress)
    {
        Progress;
        return S_OK;
    }

    VI(ULONG) Virus(const char *line)   
    {
        line;
        return S_OK;
    }

    VI(ULONG) ScanInfo(PEVENTBLOCK pEventBlk)
    { 
        pEventBlk;
        return S_OK; 
    }
};

static HRESULT PerformScan(LPSTR FullKey)
{
    char        *pGuid   = NULL;
    IScanConfig *pConfig = NULL;

    // create a new config object based on the scan key
    HRESULT hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_USER_SCANS, FullKey, pConfig);
    if (hr == S_OK)
    {
        hr = pConfig->GetOption("",(BYTE**)&pGuid,1024,(BYTE*)"");
        pConfig->Release();
    }

    if (hr == S_OK)
    {   
        // open the guid scan key
        hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_USER_SCANS, pGuid, pConfig);
        if (hr==S_OK)
            CoTaskMemFree(pGuid);
        else 
            return hr;

        // Reset the status of user scan to S_ABORTED, if it is not S_DONE, S_SUSPENDED, S_ABORTED
        // or S_DELAYED or S_NEVER_RUN.  This situation could occur if a user scan was interrupted
        // when it was in progress (by unloading RTVSCAN or system crash or system reboot)
        DWORD dwStatus = S_NEVER_RUN;
        hr = pConfig->GetIntOption(szReg_Value_ScanStatus,(long*)&dwStatus, S_NEVER_RUN);
        if(hr == S_OK)
        {
            if( !(dwStatus == S_DONE || dwStatus == S_SUSPENDED || 
                  dwStatus == S_NEVER_RUN || dwStatus == S_ABORTED || dwStatus == S_DELAYED) )
            {
                dwStatus = S_ABORTED;
                hr = pConfig->SetOption(szReg_Value_ScanStatus,REG_DWORD,(BYTE *)&dwStatus,sizeof(dwStatus));
            }
        }

        // reset the "Logger" to LOGGER_Startup
        DWORD dwLoggerType = LOGGER_Startup;
        pConfig->SetOption(szReg_Value_Logger, REG_DWORD, (BYTE *)&dwLoggerType, sizeof(dwLoggerType));

        // create a scan object
        IScan2 *pScan;
        hr = CoCreateLDVPObject (CLSID_CliProxy, IID_IScan2, (void**)&pScan);
        if (hr == S_OK)
        {
            // open the scan object
            IScanCallbackImp* cb;

            try
            {
                cb = new IScanCallbackImp();
                // AddRef'd by ctor.
            }
            catch(bad_alloc&)
            {
                // Can continue without a scan callback for DoScan.
            }

            hr = pScan->Open(cb, pConfig);
            // start the scan and wait
            if (hr == S_OK)
                hr = pScan->StartScan(FALSE /*FALSE=Launch Sync*/, TRUE /* TRUE=SyncScanWaitForUI*/);

            if (cb)
                cb->Release();

            pScan->Release();
        }
            
        pConfig->ControlDLL( CD_UNLOAD_ALL,0);    
        pConfig->Release();
    }

	return hr;
}

#if 0
/////////////////////////////////////////////////////////////////////////////
// Get the Time Duration
static bool GetTheTimeDuration(SYSTEMTIME *ptSystemTime1, SYSTEMTIME* ptSystemTime2, TCHAR* szDuration)
{
	vpstrncpy(szDuration, "Duration = ", sizeof(szDuration));
	//FILETIME FileTime1, FileTime2, FileTimeDiff;
	//SYSTEMTIME SystemTimeDiff;
	//SystemTimeToFileTime(ptSystemTime1, &FileTime1);
	//SystemTimeToFileTime(ptSystemTime2, &FileTime2);
	//FileTimeDiff.dwLowDateTime = FileTime2.dwLowDateTime - FileTime1.dwLowDateTime;
	//FileTimeDiff.dwHighDateTime = FileTime2.dwHighDateTime - FileTime1.dwHighDateTime;
	//if (FileTimeToSystemTime(&FileTimeDiff, &SystemTimeDiff))
	if (ptSystemTime2 && ptSystemTime1)
	{
		TCHAR szTimeTemp[1024];
		if ((ptSystemTime2->wDay - ptSystemTime1->wDay) > 0)
		{
			_stprintf(szTimeTemp, "%d Day(s) ", ptSystemTime2->wDay - ptSystemTime1->wDay);
			vpstrnappend(szDuration, szTimeTemp, sizeof(szDuration));
		}

		if ((ptSystemTime2->wHour - ptSystemTime1->wHour) > 0)
		{
			_stprintf(szTimeTemp, "%d Hour(s)", ptSystemTime2->wHour - ptSystemTime1->wHour);
			vpstrnappend(szDuration, szTimeTemp, sizeof(szDuration));
		}

		if ((ptSystemTime2->wMinute - ptSystemTime1->wMinute) > 0)
		{
			_stprintf(szTimeTemp, "%d Minute(s) ", ptSystemTime2->wMinute - ptSystemTime1->wMinute);
			vpstrnappend(szDuration, szTimeTemp, sizeof(szDuration));
		}

		if ((ptSystemTime2->wSecond - ptSystemTime1->wSecond) > 0)
		{
			_stprintf(szTimeTemp, "%d Seconds(s) ", ptSystemTime2->wSecond - ptSystemTime1->wSecond);
			vpstrnappend(szDuration, szTimeTemp, sizeof(szDuration));
		}

		vpstrnappend(szDuration, "\n", sizeof(szDuration));
			
		return true;
	}

	return false;
}
#endif
/////////////////////////////////////////////////////////////////////////////
// Get the Current Time in string format
static void GetTheCurrentTimeStr(TCHAR *szTimeStr, SYSTEMTIME& tSystemTime)
{
	TCHAR szDate[1024];
	TCHAR szTime[1024];
	
	GetLocalTime(&tSystemTime);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT,DATE_SHORTDATE,&tSystemTime,NULL,szDate,sizeof(szDate));
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &tSystemTime,NULL,szTime,sizeof(szTime)); 
	_stprintf(szTimeStr, "%s %s", szDate, szTime);
}
/////////////////////////////////////////////////////////////////////////////
// Logs Cmd Line status
static void LogCmdLineStatus(FILE *fPtr,char *vFormat,...)
{
	if (fPtr)
	{
		va_list vMarker;
		char szLine[1024];

		va_start(vMarker, vFormat);
		vsprintf(szLine,vFormat,vMarker);
		fprintf(fPtr, szLine);
		fflush(fPtr);
		va_end(vFormat);
	}
}
/////////////////////////////////////////////////////////////////////////////
// Logs Cmd Line status
int ParseCmdLineForLogFile( LPCTSTR lpCmdLine, TCHAR* szLogFileName)
{
    TCHAR *ppszLogFileName = NULL; 

    if ((ppszLogFileName = const_cast<LPTSTR>(_tcsstr(lpCmdLine, HAS_LOG_FILE))) != NULL)
	{
        ppszLogFileName += (TCHAR) _tcslen(HAS_LOG_FILE);
		if (*ppszLogFileName)
		{
            ppszLogFileName++;   // first "
            TCHAR *pEndQuote = _tcsstr(ppszLogFileName, "\"");
			if (pEndQuote)
			{
                size_t tmp = pEndQuote - ppszLogFileName + 1; // 1 for the NULL character that vpstrncpy adds
                vpstrncpy(szLogFileName, ppszLogFileName, tmp);
				return 1;
			}
			else
            {
                return -1;   // bad parsing
            }
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// Create temporary (Quick)Scan keys
bool AddTempQuickScanKeys(LPCTSTR szCmdLine, 
                          LPCTSTR szKeyName,
                          LPTSTR szFullKey, const DWORD dwFullKeySize,
                          LPTSTR szFullCmdLinePathKey, const DWORD dwFullCmdLinePathKeySize,
                          LPTSTR szFullCustomTasksPathKey, const DWORD dwFullCustomTasksPathKeySize,
                          LPTSTR szFullScanKey, const DWORD dwFullScanKeySize,
                          LPTSTR szFullQuickScanTemplatePathKey, const DWORD dwFullQuickScanTemplatePathKeySize)
{
	bool bOK = false;

	vpstrncpy(szFullKey, szReg_Value_CmdLineScan, dwFullKeySize);
	vpstrnappend(szFullKey, "\\", dwFullKeySize);
	vpstrnappend(szFullKey, szKeyName, dwFullKeySize);

	// Create the real key
	TCHAR szFullPathKey[1024];
	vpstrncpy(szFullPathKey, szReg_Key_Main, sizeof(szFullPathKey));
	vpstrnappend(szFullPathKey, "\\", sizeof(szFullPathKey));
	vpstrnappend(szFullPathKey, szReg_Key_CustomTasks, sizeof(szFullPathKey));
	vpstrncpy(szFullCustomTasksPathKey, szFullPathKey, dwFullCustomTasksPathKeySize);
	vpstrnappend(szFullPathKey, "\\", sizeof(szFullPathKey));
	vpstrnappend(szFullPathKey, szReg_Value_CmdLineScan, sizeof(szFullPathKey));
	vpstrncpy(szFullCmdLinePathKey, szFullPathKey, dwFullCmdLinePathKeySize);
	vpstrnappend(szFullPathKey, "\\", sizeof(szFullPathKey));
	vpstrnappend(szFullPathKey, szKeyName, sizeof(szFullPathKey));

	DWORD			dwDisp=0;
	HKEY			hKey;

	if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_CURRENT_USER,
								szFullPathKey,
								0,
				 				_T(""),
								REG_OPTION_NON_VOLATILE,
								KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
								NULL,
								&hKey,
								&dwDisp
								) )
	{
		// Write GUID for scan --- use same one
		if (ERROR_SUCCESS != RegSetValueEx(hKey,"",0,REG_SZ,(LPBYTE)szKeyName,_tcslen(szKeyName) * sizeof(TCHAR)))
		{

		}


		RegCloseKey(hKey);
		hKey = NULL;
		dwDisp = 0;

		// Create GUID for the scan
		vpstrncpy(szFullScanKey,  szFullCustomTasksPathKey, dwFullScanKeySize);
		vpstrnappend(szFullScanKey, "\\", dwFullScanKeySize);
		vpstrnappend(szFullScanKey, szKeyName, dwFullScanKeySize);

		vpstrncpy(szFullQuickScanTemplatePathKey, szReg_Key_Main, dwFullQuickScanTemplatePathKeySize);
		vpstrnappend(szFullQuickScanTemplatePathKey, "\\", dwFullQuickScanTemplatePathKeySize);
		vpstrnappend(szFullQuickScanTemplatePathKey, szReg_Key_LocalScans, dwFullQuickScanTemplatePathKeySize);
		vpstrnappend(szFullQuickScanTemplatePathKey, "\\", dwFullQuickScanTemplatePathKeySize);
		vpstrnappend(szFullQuickScanTemplatePathKey, szReg_Key_Default_QuickScan_Options, dwFullQuickScanTemplatePathKeySize);

		if( ERROR_SUCCESS == RegCreateKeyEx(	HKEY_CURRENT_USER,
									szFullScanKey,
									0,
				 					_T(""),
									REG_OPTION_NON_VOLATILE,
									KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
									NULL,
									&hKey,
									&dwDisp
									) )
		{
			HKEY hKeyFullScanTemplate = NULL;
			if ( RegOpenKey(HKEY_LOCAL_MACHINE, szFullQuickScanTemplatePathKey,&hKeyFullScanTemplate) == ERROR_SUCCESS )
			{
				RegCopyKeys(hKeyFullScanTemplate, hKey);
				RegCloseKey(hKeyFullScanTemplate);
				bOK = true;
			}

			// Any other options?
			// Deposit options from command line
			//----------------------------------
			
			if (_tcsstr(szCmdLine, SCAN_ALL_DRIVES) != NULL)
			{
				DWORD dwValue = 1;
				RegSetValueEx(hKey,szReg_Val_ScanAllDrives,0,REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD));
			}
#if 0
			else if (_tcsstr(szCmdLine, HAS_UI) != NULL)
			{
				// DisplayStatusDialog
				DWORD dwValue = 1;
				RegSetValueEx(hKey,szReg_Value_DisplayStatusDlg,0,REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD));
				RegSetValueEx(hKey,szReg_Val_CloseScan,0,REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD));
				dwValue = 0;
				RegSetValueEx(hKey,szReg_Value_DisplayStatusDlg_Thrt,0,REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD));
			}
#endif

			// By default the U.I. comes up when a threat is detected.
			// Let's turn it off!
			DWORD dwValue = 0;
			RegSetValueEx(hKey,szReg_Value_DisplayStatusDlg_Thrt,0,REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD));
			dwValue = 0;
			RegSetValueEx(hKey,szReg_Value_DisplayStatusDlg,0,REG_DWORD,(LPBYTE)&dwValue,sizeof(DWORD));
			RegCloseKey(hKey);
		}
	}

	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
// Cleanup temporary (Quick)Scan keys
bool DeleteTempQuickScanKeys(LPCTSTR szKeyName, LPTSTR szFullCmdLinePathKey, LPTSTR szFullCustomTasksPathKey)
{
	HKEY hKey = NULL;

	// Delete the Scan Key
	if ( RegOpenKey(HKEY_CURRENT_USER, szFullCmdLinePathKey,&hKey) == ERROR_SUCCESS )
	{
		RegDeleteKey(hKey, szKeyName);
		RegCloseKey(hKey);
	}

	// Try to Delete the Scan Key
	if ( RegOpenKey(HKEY_CURRENT_USER, szFullCustomTasksPathKey,&hKey) == ERROR_SUCCESS )
	{
		RegDeleteKey(hKey, szReg_Value_CmdLineScan);
		RegCloseKey(hKey);
	}

	// Try to Delete the Scan Info Key
	if ( RegOpenKey(HKEY_CURRENT_USER, szFullCustomTasksPathKey,&hKey) == ERROR_SUCCESS )
	{
		RegDeleteKey(hKey, szKeyName);
		RegCloseKey(hKey);
	}

	// Try to Delete the Scan Key
	if ( RegOpenKey(HKEY_CURRENT_USER, szFullCustomTasksPathKey,&hKey) == ERROR_SUCCESS )
	{
		RegDeleteKey(hKey, szReg_Value_CmdLineScan);
		RegCloseKey(hKey);
	}

	return true;
}

bool IsHelpCommand(LPCTSTR lpCmdLine)
{
	return (_tcsicmp (lpCmdLine, HELP_CMD1) == 0 || _tcsicmp(lpCmdLine, HELP_CMD2) == 0 ||
		_tcsicmp(lpCmdLine, HELP_CMD3) == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Process all startup scans
static DWORD StartScans(LPCTSTR lpCmdLine) 
{
    IScanConfig *pConfig=NULL;
    HRESULT hr = S_OK;
	DWORD nRet = 0;
    FILE *fPtr = NULL;
    DWORD  dwerr = ERROR_SUCCESS;

    CoInitializeEx(NULL, NULL);

	if (_tcsicmp (lpCmdLine, STARTUP_SCANS) == 0 && StartupScansEnabled() == TRUE)
	{
		hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_USER_SCANS, szReg_Value_Startup, pConfig);
		if (hr==S_OK)
		{
			int   nCurOption=0;
			char *pKeyName=NULL;

			hr = pConfig->EnumSubOptions(nCurOption,&pKeyName);
			while (hr == S_OK && g_bRunning)
			{
				Relinquish(); // drain apartment message queue 

				char szFullKey[1024];
				vpstrncpy    (szFullKey, szReg_Value_Startup, sizeof (szFullKey));
				vpstrnappend (szFullKey, "\\",                sizeof (szFullKey));
				vpstrnappend (szFullKey, pKeyName,            sizeof (szFullKey));
				CoTaskMemFree(pKeyName);

				PerformScan (szFullKey);

				++nCurOption;
				hr = pConfig->EnumSubOptions (nCurOption, &pKeyName);
			}
			pConfig->Release();
		}
	}
	else if (_tcsstr(lpCmdLine, CMDLINE_SCAN) != NULL )
	{
        // Open the log file
        TCHAR szLogFileName[1024];
        int  bSetLogFile = ParseCmdLineForLogFile(lpCmdLine, szLogFileName);
            
        dwerr = ERROR_SUCCESS;
        if (bSetLogFile <= 0) {
            // Determine Symantec's log directory on this machine
            dwerr = GetAppDataLogsDir(szLogFileName, 1024);

            if ( ERROR_SUCCESS == dwerr ) {
                vpsnprintf(szLogFileName, 1024, "%s\\%s", szLogFileName, DEFAULT_LOG_FILE);
            }
            else {
                vpsnprintf(szLogFileName, 1024, "%s", DEFAULT_LOG_FILE_OLD);
            }
        }

        fPtr = fopen(szLogFileName, "w");

        // Stop if cannot open the file
        if ( fPtr == NULL ) {
            TCHAR szErrorText[1024] = _T("");
            TCHAR szErrorTitle[64] = _T("");

            ::LoadString(s_hInst, IDS_DOSCAN_FILERR_TEXT, szErrorText, 1024);
            ::LoadString(s_hInst, IDS_DOSCAN_FILERR_TITLE, szErrorTitle, 64);

            vpsnprintf(szErrorText, 1024, "%s %s.", szErrorText, szLogFileName);
            MessageBox( NULL, szErrorText, szErrorTitle, MB_OK );
            nRet = 1;
        }
        else {
            HANDLE hMutex = CreateMutex(NULL, TRUE, _T("Global\\DoScanCmdLine"));
            if ( !hMutex || (GetLastError() == ERROR_ALREADY_EXISTS) ) {
                LogCmdLineStatus(fPtr, "CmdLineScan is already running");
                nRet = 2;
            }
            else {
                // Proceed with the /cmdlinescan
                Relinquish(); 
                hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_USER_SCANS, szReg_Value_CmdLineScan, pConfig);
                if (hr == S_OK) {
                    UUID        myID;
                    BYTE        *szTaskname=NULL;
                    TCHAR       *szKeyName=NULL;
                    
                    //First, generate a unique name 
                    DWORD dwErr = UuidCreate(&myID);
                    if ( RPC_S_OK == dwErr ) {
                        dwErr = UuidToString(&myID, &szTaskname);

                        if( RPC_S_OK == dwErr ) {
                            dwErr = 0;
                            try {
                                szKeyName = reinterpret_cast<TCHAR *> (szTaskname);

                                TCHAR szFullKey[1024];
                                TCHAR szFullCmdLinePathKey[1024];
                                TCHAR szFullCustomTasksPathKey[1024];
                                TCHAR szFullScanKey[1024];
                                TCHAR szFullQuickScanTemplatePathKey[1024];

                                if (AddTempQuickScanKeys(lpCmdLine, 
                                                            szKeyName, 
                                                            szFullKey, sizeof(szFullKey), 
                                                            szFullCmdLinePathKey, sizeof(szFullCmdLinePathKey),
                                                            szFullCustomTasksPathKey, sizeof(szFullCustomTasksPathKey),
                                                            szFullScanKey, sizeof(szFullScanKey),
                                                            szFullQuickScanTemplatePathKey, sizeof(szFullQuickScanTemplatePathKey)) )
                                {
                                    SYSTEMTIME tSystemTimeBefore;
                                    SYSTEMTIME tSystemTimeAfter;
                                    TCHAR szTimeStr[1024];
                                    GetTheCurrentTimeStr(szTimeStr, tSystemTimeBefore);
                                    TCHAR szQuickScanFormatStr[1024];
                                    ::LoadString(s_hInst, IDS_STARTED_QUICKSCAN, szQuickScanFormatStr, 1024);
                                    LogCmdLineStatus(fPtr, szQuickScanFormatStr, szTimeStr);
                                    DWORD dStartTickCount = GetTickCount();
                                    
                                    // Start scans
                                    HRESULT hr = PerformScan (szFullKey);
                                    DWORD dEndTickCount = GetTickCount();
                                    GetTheCurrentTimeStr(szTimeStr, tSystemTimeAfter);
                                    ::LoadString(s_hInst, IDS_DURATION_QUICKSCAN, szQuickScanFormatStr, 1024);
                                    LogCmdLineStatus(fPtr, szQuickScanFormatStr, (dEndTickCount - dStartTickCount) / 1000);

                                    ::LoadString(s_hInst, IDS_RESULT_QUICKSCAN, szQuickScanFormatStr, 1024);
                                    LogCmdLineStatus(fPtr, szQuickScanFormatStr, hr);
                                    

                                    DeleteTempQuickScanKeys(szKeyName, szFullCmdLinePathKey, szFullCustomTasksPathKey);
                                    
                                }  // Setup Temp Scan keys succeeded

                                RpcStringFree(&szTaskname);
                                pConfig->Release();
                            }
                            catch(std::bad_alloc &)
                            {
                                RpcStringFree(&szTaskname);
                                pConfig->Release();
                                dwErr = 0xFFFFFFFF;
                            }
                            catch(exception&)
                            {
                                RpcStringFree(&szTaskname);
                                pConfig->Release();
                                dwErr = 0xFFFFFFFF;
                            }
                        }
                    }

                    // Convert dwErr to nRet
                    nRet = dwErr;           
                }
            }

            // Release hMutex
            if (hMutex) {
                ReleaseMutex(hMutex);
                CloseHandle(hMutex);
            }
        }
        
        // Close the log file
        if (fPtr) {
            fclose(fPtr);
        }
	}
	else 
	{
        TCHAR szHelpText[2048] = _T("");
        TCHAR szHelpText2[2048] = _T("");
        TCHAR szHelpTitle[64] = _T("");
        TCHAR szLogFileName[1024] = _T("");

		::LoadString(s_hInst, IDS_DOSCAN_HELP_TEXT, szHelpText, 1024);
		::LoadString(s_hInst, IDS_DOSCAN_HELP_TITLE, szHelpTitle, 64);

        dwerr = GetAppDataLogsDir(szLogFileName, 1024);

        if (ERROR_SUCCESS == dwerr)
            vpsnprintf(szLogFileName, 1024, "%s\\%s", szLogFileName, DEFAULT_LOG_FILE);
        else
            vpsnprintf(szLogFileName, 1024, "%s", DEFAULT_LOG_FILE_OLD);

        vpsnprintf(szHelpText2, 2048, szHelpText, szLogFileName);
        MessageBox( NULL, szHelpText2, szHelpTitle, MB_OK );
		nRet = 1; // returns 1 for all other command line parameters
	}
    
    // Cleanup
    Relinquish(); // aways pump before exiting the thread
    CoUninitialize();
    return nRet;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    hPrevInstance;
    nCmdShow;
	DWORD dwRetCode = 0;

    s_hInst = hInstance;

    dwRetCode = StartScans(lpCmdLine);
	
    return dwRetCode;
}
