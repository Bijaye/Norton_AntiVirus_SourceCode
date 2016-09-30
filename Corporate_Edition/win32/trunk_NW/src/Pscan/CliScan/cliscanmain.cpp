// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#define IncDebugParser
#include "pscan.h"
#include "SymSaferRegistry.h"
#include "finetime.h"
#include "messagebox.h"
#include "ScanSinkFactory.h"
#include "EraserEngine.h"
#include "EraserEngineManager.h"

#define LOG_BUF_SIZE 256
DWORD LogLine (char *line,BOOL write);
char DebugLogFile[IMAX_PATH] = "vpdebug.log";

#include "ResourceLoader.h"

CAtlBaseModule _ModuleRes;
CResourceLoader g_ResLoader(&_ModuleRes, _T("PScanRes.dll"));
CResourceLoader g_ResActa(&_ModuleRes, _T("ActaRes.dll"));
HINSTANCE g_hInstRes = NULL;

HINSTANCE hInstance,hInstLang;
HKEY hMainKey = 0;
HKEY hSystemKey=0;
HKEY hCommonKey = 0;
char HaveUI = FALSE;
char HomeDir[IMAX_PATH];
mSID MainSid;
char SystemRunning=0;
DWORD ThreadsInUse=0;
DEBUGFLAGTYPE debug=0;
BOOL DoScheduleScans=0;
BOOL ScansOK = FALSE;
HANDLE DebugHan=NULL;

extern "C" HANDLE hAsyncScanMutex = NULL;

extern "C" HINSTANCE   hNavNtUtl = NULL;
extern "C" PFNGetFileSecurityDesc          pfnGetFileSecurityDesc = NULL;
extern "C" PFNSetFileSecurityDesc          pfnSetFileSecurityDesc = NULL;
extern "C" PFNFreeFileSecurityDesc         pfnFreeFileSecurityDesc = NULL;
extern "C" PFNCopyAlternateDataStreams2    pfnCopyAlternateDataStreams2 = NULL;
extern "C" PFNFileHasAlternateDataStreams  pfnFileHasAlternateDataStreams = NULL;
extern "C" PFNProcessIsNormalUser          pfnProcessIsNormalUser = NULL;
extern "C" PFNProcessCanUpdateRegKey       pfnProcessCanUpdateRegKey = NULL;
extern "C" PFNPolicyCheckHideDrives        pfnPolicyCheckHideDrives = NULL;
extern "C" PFNPolicyCheckRunOnly           pfnPolicyCheckRunOnly = NULL;
extern "C" PFNPolicyCheckNoEntireNetwork   pfnPolicyCheckNoEntireNetwork = NULL;
extern "C" PFNProcessSetPrivilege          pfnProcessSetPrivilege = NULL;
extern "C" PFNProcessTakeFileOwnership     pfnProcessTakeFileOwnership = NULL;

extern "C" HINSTANCE hSfc = NULL;   
extern "C" PFNSfcIsFileProtected           pfnSfcIsFileProtected = NULL;

char CurrentUserName[NAME_SIZE];

/**************************************************************/
HWND GetConsoleHwnd(void)
{
	 #define MY_BUFSIZE 1024 // buffer size for console window titles
	 HWND hwndFound;         // this is what is returned to the caller
	 char pszNewWindowTitle[MY_BUFSIZE]; // contains fabricated WindowTitle
	 char pszOldWindowTitle[MY_BUFSIZE]; // contains original WindowTitle

	 // fetch current window title

	 GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

	 // format a "unique" NewWindowTitle

	 sssnprintf(pszNewWindowTitle,sizeof(pszNewWindowTitle),"%d/%d",
					 GetTickCount(),
					 GetCurrentProcessId());

	 // change current window title

	 SetConsoleTitle(pszNewWindowTitle);

	 // ensure window title has been updated

	 Sleep(40);

	 // look for NewWindowTitle

	 hwndFound=FindWindow(NULL, pszNewWindowTitle);

	 // restore original window title

	 SetConsoleTitle(pszOldWindowTitle);

	 return(hwndFound);
}

/**************************************************************************************/
//#define X(x) MessageBox(NULL,x,"",0);
#define X(x) LogLine(x,1);
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved) {

	HKEY hkey;
	char lFile[IMAX_PATH];
	char szDebug[DEBUG_STR_LEN];
	DWORD dwUserNameSize;

	REF(lpvReserved);
	if (hInstance == 0) {
		hInstLang = hInstance = hinstDLL;
		RegCreateKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
		RegCreateKey(hMainKey,"SystemScan",&hSystemKey);

		GetStr(hMainKey,"ProductControl\\Debug",szDebug,sizeof(szDebug),".");
		debug = ParseDebugFlag(szDebug);

		if ( !(debug&DEBUGNOWINDOW) && debug ) {
			AllocConsole();
			DebugHan = GetStdHandle(STD_OUTPUT_HANDLE);
			}

		sssnprintf (DebugLogFile,sizeof(DebugLogFile),"vpdebug.log");
		GetStr(hMainKey, "Home Directory",HomeDir, sizeof(HomeDir),".");
		if (HomeDir[0] == 0) {
			GetCurrentDirectory(sizeof(HomeDir),HomeDir);
			PutStr(hMainKey,"Home Directory",HomeDir);
			}


		}

	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:

			InitDLL(hInstance);
			if (RegOpenKey(HKEY_LOCAL_MACHINE,REGHEADER,&hkey) == ERROR_SUCCESS) {
				DWORD size = sizeof(lFile);
				SymSaferRegQueryValueEx(hkey,"Home Directory",0,NULL,(PBYTE)lFile,&size);
				SetCurrentDirectory(lFile);
				RegCloseKey(hkey);
				}

			//Load the Acta strings:
			if (SUCCEEDED(g_ResActa.Initialize()))
			{
				dprintf("Cliscan loaded Acta resource DLL\n");
				hInstLang = g_ResActa.GetResourceInstance();
			}
			else
			{
				dprintf("Failed to load Acta resource DLL\n");
				return FALSE;
			}
				
			//Load the PScan resources:
			if (SUCCEEDED(g_ResLoader.Initialize()))
			{
				dprintf("Cliscan loaded PScan resource DLL\n");
				::g_hInstRes = g_ResLoader.GetResourceInstance();
			}
			else
			{
				dprintf("Failed to load PScan resource DLL\n");
				return FALSE;
			}

            // Load NAVNTUTL. This DLL gives us access to security descriptor
			// functions and multiple stream support.

			dwUserNameSize = sizeof(CurrentUserName);

			GetUserName(CurrentUserName, &dwUserNameSize);

            CheckDataDirs();

			char szNavNTUTLPath[IMAX_PATH];

			// Get the home directory
			GetHomeDir( szNavNTUTLPath, sizeof(szNavNTUTLPath) );

			vpstrnappendfile (szNavNTUTLPath, NAVNTUTL_DLL_NAME, sizeof (szNavNTUTLPath));

            hNavNtUtl = LoadLibrary( szNavNTUTLPath );
            if (hNavNtUtl)
            {
                pfnGetFileSecurityDesc      = 
                        (PFNGetFileSecurityDesc) GetProcAddress( hNavNtUtl, GETFILESECURITYDESCRIPTOR );
                pfnSetFileSecurityDesc      = 
                        (PFNSetFileSecurityDesc) GetProcAddress( hNavNtUtl, SETFILESECURITYDESCRIPTOR );
                pfnFreeFileSecurityDesc      = 
                        (PFNFreeFileSecurityDesc) GetProcAddress( hNavNtUtl, FREEFILESECURITYDESCRIPTOR );
                pfnCopyAlternateDataStreams2 = 
                        (PFNCopyAlternateDataStreams2) GetProcAddress( hNavNtUtl, COPYALTERNATEDATASTREAMS2 );
                pfnProcessIsNormalUser = 
                        (PFNProcessIsNormalUser) GetProcAddress( hNavNtUtl, PROCESSISNORMALUSER);
                pfnProcessCanUpdateRegKey = 
                        (PFNProcessCanUpdateRegKey) GetProcAddress( hNavNtUtl, PROCESSCANUPDATEREGKEY);
                pfnPolicyCheckHideDrives = 
                        (PFNPolicyCheckHideDrives) GetProcAddress( hNavNtUtl, POLICYCHECKHIDEDRIVES);
                pfnPolicyCheckRunOnly = 
                        (PFNPolicyCheckRunOnly) GetProcAddress( hNavNtUtl, POLICYCHECKRUNONLY);
                pfnPolicyCheckNoEntireNetwork = 
                        (PFNPolicyCheckNoEntireNetwork) GetProcAddress( hNavNtUtl, POLICYCHECKNOENTIRENETWORK);
                pfnProcessSetPrivilege = 
                        (PFNProcessSetPrivilege) GetProcAddress( hNavNtUtl, PROCESSSETPRIVILEGE);
                pfnProcessTakeFileOwnership = 
                        (PFNProcessTakeFileOwnership) GetProcAddress( hNavNtUtl, PROCESSTAKEFILEOWNERSHIP);
                pfnFileHasAlternateDataStreams =
                        (PFNFileHasAlternateDataStreams) GetProcAddress( hNavNtUtl, FILEHASALTERNATEDATASTREAMS );
            }

			TCHAR szPath[MAX_PATH];
			ZeroMemory(szPath, sizeof(szPath));
			::GetSystemDirectory( szPath, MAX_PATH );
			_tcscat( szPath, _T("\\"));
			_tcscat( szPath, SFC_DLL_NAME);
			hSfc = LoadLibrary(szPath);

			if (hSfc)
			{
				pfnSfcIsFileProtected = (PFNSfcIsFileProtected) GetProcAddress(hSfc, SFCISFILEPROTECTED);
			}

			break;

		case DLL_PROCESS_DETACH:
            CVirusFoundWrapper().CloseDialog();

			DeinitDLL(hInstance);
			StopScanEngine();
			FreeAllLS();

            // Ensure that the following open registry keys are closed.
            // The following keys caused handle leaks (when left open) at the end of a scan
            // even after the scan results dialog was closed.
            if(hSystemKey)
            {
                RegCloseKey(hSystemKey);
            }

            hCommonKey = GetCommonKey();
            if(hCommonKey)
                RegCloseKey(hCommonKey);

            if(hMainKey)
            {
                RegCloseKey(hMainKey);
            }

            if (hNavNtUtl)
            {
                //null these out before freeing library
                pfnGetFileSecurityDesc = NULL;
                pfnSetFileSecurityDesc = NULL;
                pfnFreeFileSecurityDesc = NULL;
                pfnCopyAlternateDataStreams2 = NULL;
                pfnProcessIsNormalUser = NULL;
                pfnProcessCanUpdateRegKey = NULL;
                pfnPolicyCheckHideDrives = NULL;
                pfnPolicyCheckRunOnly = NULL;
                pfnPolicyCheckNoEntireNetwork = NULL;
                pfnProcessSetPrivilege = NULL;
                pfnProcessTakeFileOwnership = NULL;
                pfnFileHasAlternateDataStreams = NULL;

                FreeLibrary(hNavNtUtl);
                hNavNtUtl = NULL;
            }

			if (hSfc)
			{
                pfnSfcIsFileProtected = NULL;
				FreeLibrary(hSfc);
                hSfc = NULL;
			}
		
			if ( GetCurrentVEHandle() ) 
			{
				ShutDownPattern();
			}

			break;
		}

	return TRUE;
}
/*******************************************************************************************/
DWORD StopScanEngine(void) {

	long    x=0;

	if (SystemRunning)
    {
        if(hAsyncScanMutex)
        {
            CloseHandle(hAsyncScanMutex);
            hAsyncScanMutex = NULL;
        }

        EndEventLog();

        ShutDownPattern();

        FreeAllScans();
        SystemRunning = 0;
        Sleep(100);
        StorageManagerStop(&x);
    }

	return 0;
}
/*******************************************************************************************/
void MainTimer(void) {

	int loops=0;
	char name[256];
	DWORD index;
	HKEY hkey,hmkey;

	while (SystemRunning) {
		if (DoScheduleScans && (loops%(2*60)==0)) {
			if (RegOpenKey(HKEY_CURRENT_USER,REGHEADER"\\Custom Tasks",&hmkey) == ERROR_SUCCESS) {
				index=0;
				while (RegEnumKey(hmkey,index++,name,sizeof(name)) == ERROR_SUCCESS) {
					if (name[8] == '-' && name[13] == '-' && name[18] == '-' && name[23] == '-') {
						if (RegOpenKey(hMainKey,name,&hkey) == ERROR_SUCCESS) {
							if (IsItTime(hkey))
								StartScan(hkey,NULL,NULL,NULL,NULL,TRUE /*Launch Async*/, FALSE, NULL);
							else
								RegCloseKey(hkey);
							}
						}
					}
				}
			}

		if (loops%60 == 0) {
			CheckScans();
			}
		if (loops%(2*60*5) == 0) {
			CheckPattern(FALSE);
			}
		Sleep(500);
		loops++;
		}
}
/*******************************************************************************************/
DWORD StartScanEngine(DWORD slot) {

	DWORD cc;

	SystemRunning = 1;

	VerifyStorageManagerState(S_WANT_CONFIG|S_WANT_WALK);

	if (slot == 0xffffffff) 
    {

        if ( !GetCurrentVEHandle() ) 
        {
		    cc = InitializeScanEngine();
		    if (cc)
			    return cc;
        }

		ScansOK = TRUE;
    }
	else 
    {
		if (slot&0x10) 
        {
            if ( !GetCurrentVEHandle() ) 
            {
    			cc = InitializeScanEngine();
	    		if (cc)
		    		return cc;
            }

			ScansOK = TRUE;
        }

		if (slot&0x20)
			DoScheduleScans = TRUE;
	}

    // Create the mutex to synchronize asynchronous scans
    hAsyncScanMutex = CreateMutex(NULL, FALSE, NULL);
    if(hAsyncScanMutex == NULL)
    {
        return P_ASYNC_SCAN_MUTEX_FAIL;
    }

	GetSid(&MainSid);

	InitEventLog();


	MyBeginThread((THREAD)MainTimer,NULL,"");

	return 0;
}
/********************************************************************************************/
static BOOL First=TRUE;
static DWORD EngineStatus=0xffffffff;
/********************************************************************************************/
DWORD ScanEngineAllDone(void) {

	if (EngineStatus == ERROR_SUCCESS) {
		StopScanEngine();

		First = TRUE;
		EngineStatus = 0xfffffff;
		}
	else {
		long lStatus = 0;
		StorageManagerStop(&lStatus);
		}

    // TODO: Re-examine initialization and shutdown of singleton
    // CScanSinkFactory and CEraserEngine objects.
    // Cleanup here works because callers only call this routine once
    // before CliScan shutdown. However, if CliScan stays loaded and
    // another call to initialize scan engine is made, initialization will fail.
    dprintf("Shutting down CScanSinkFactory\n");
    CScanSinkFactory::Destroy();
    dprintf("Shutting down ERASER\n");
    CEraserEngineManager::DestroyEraserEngines();

	return EngineStatus;
}
/********************************************************************************************/
DWORD VerifyScanEngineRunning(void) {


	if (First) {
		DWORD cc = StartScanEngine(0xffffffff);
		EngineStatus = cc;
		if (cc) {
			dprintf("Error %08X in Start ScanEngine\n",cc);
			}
		else
			First = FALSE;
		}

	return EngineStatus;
}
/********************************************************************************************/
DWORD ManualScanOneFile(HKEY hkey,char *FullPath,SCANCBVIRUS cbVirus,SCANCBPROG cbProgress,BOOL bScanSystem) {

	return DoSyncScan(hkey,NULL,FullPath,NULL,0,NULL,cbVirus,cbProgress,NULL,NULL,bScanSystem);
}
/********************************************************************************************/
DWORD ManualScanDir(HKEY hkey,char *path,BOOL children,char *exts,SCANCBVIRUS cbVirus,SCANCBPROG cbProgress,BOOL bScanSystem) {

	return DoSyncScan(hkey,path,NULL,NULL,children,exts,cbVirus,cbProgress,NULL,NULL,bScanSystem);
}

/*****************************************************************************************************************/
VOID Real_dprintfTagged(DEBUGFLAGTYPE dwTag, const char *format,...) {

	va_list marker;
	char line[MAX_DEBUGPRINT_STR];
	DWORD out;

	// check skipping untagged lines

    if (dwTag == 0 && (debug & DEBUGEXCLUDEUNTAGGED) )
    {
		return;
    }

	if (!debug)
		return;

    DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

    DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
    DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
    DWORD dwHours, dwMins, dwSecs;

    SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

    sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d[_%ld]|", dwHours, dwMins, dwSecs, dwMicros, NTxGetCurrentThreadId() );

	va_start(marker, format);
	ssvsnprintfappend(line,sizeof(line),format,marker);
	line[sizeof(line)-1] = 0;
#ifdef _CONSOLE
		printf("%s", line);
#else
	if (debug&DEBUGLOG)
		LogLine(line,FALSE);

	if (DebugHan)
		WriteConsole(DebugHan,line,strlen(line)+1,&out,NULL);

	if (debug&DEBUGOUTPUT)
		OutputDebugString(line);
#endif
 	va_end(marker);

}

/*****************************************************************************************************************/
void Real_dprintf(const char *format,...) {

	va_list marker;
	char line[MAX_DEBUGPRINT_STR];
	DWORD out;

	// check skipping untagged lines

    if ( debug & DEBUGEXCLUDEUNTAGGED )
    {
		return;
    }

	if (!debug)
		return;

	va_start(marker, format);
	ssvsnprintf(line, sizeof(line), format, marker);
	line[sizeof(line)-1] = 0;
#ifdef _CONSOLE
		printf("%s", line);
#else
	if (debug&DEBUGLOG)
		LogLine(line,FALSE);

	if (DebugHan)
		WriteConsole(DebugHan,line,strlen(line)+1,&out,NULL);

	if (debug&DEBUGOUTPUT)
		OutputDebugString(line);
#endif
 	va_end(marker);

}
/*********************************************************************************************************/
DWORD LogLine (char *line,BOOL write)
{
	FILE *fp;
	int i;

	REF(write);
	fp = fopen (DebugLogFile,"at");
	for (i=0 ; !fp && i<5 ; i++)
    {
		Sleep (50);
		fp = fopen (DebugLogFile,"at");
	}
	if (fp)
    {
		if (line)
			fprintf (fp,"%s",line);
		flushall();
		fclose (fp);
	}

	return 0;
/*
	FILE *fp;
	int i,buflen,linelen;
	static char LogBuf[LOG_BUF_SIZE];

	buflen = NumBytes (LogBuf);

	if (line)
		linelen = NumBytes(line);
	else {
		linelen = 0;
		if (buflen>0)
			write=TRUE;
	}

	if (!write)
		if (buflen + linelen > LOG_BUF_SIZE)
			write = TRUE;

	if (write) {
		fp = fopen (DebugLogFile,"at");
		for (i=0 ; !fp && i<5 ; i++) {
			Sleep (50);
			fp = fopen (DebugLogFile,"at");
		}
		if (fp) {
			if (buflen>0)
				fprintf (fp,"%s",LogBuf);
			if (linelen>0)
				fprintf (fp,"%s",line);
			flushall();
			fclose (fp);
		}
		memset (LogBuf,0,sizeof(LogBuf));
	}
	else
		if (linelen>0)
			StrCat (LogBuf,line);

	return 0;*/
}
