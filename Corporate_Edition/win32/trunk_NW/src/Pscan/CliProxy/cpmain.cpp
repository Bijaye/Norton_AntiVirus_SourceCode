// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 1996-2004, 2005, Symantec Corporation, All rights reserved.
// cpmain.cpp

#define IncDebugParser
#include "pscan.h"
#include "SavrtModuleInterface.h"
#include "localtransport.h"
#include "SymSaferRegistry.h"
#include "finetime.h"

#include "DarwinResCommon.h"
CResourceLoader g_ResLoader(&_ModuleRes, _T("PScanRes.dll"));
CResourceLoader g_ResActa(&_ModuleRes, _T("ActaRes.dll"));
HINSTANCE g_hInstRes = NULL;

#define LOG_BUF_SIZE 256
DWORD LogLine (char *line,BOOL write);
char DebugLogFile[IMAX_PATH] = "vpdebug.log";

HINSTANCE hInstance,hInstLang;
HKEY hMainKey = 0;
HKEY hCommonKey = 0;
char HaveUI = FALSE;
char HomeDir[IMAX_PATH];
char gszMoveDir[IMAX_PATH];
char SystemRunning=0;
DEBUGFLAGTYPE debug=0;
HANDLE DebugHan=NULL;

extern "C" HINSTANCE                       hNavNtUtl = NULL;
extern "C" PFNGetFileSecurityDesc          pfnGetFileSecurityDesc = NULL;
extern "C" PFNSetFileSecurityDesc          pfnSetFileSecurityDesc = NULL;
extern "C" PFNFreeFileSecurityDesc         pfnFreeFileSecurityDesc = NULL;
extern "C" PFNCopyAlternateDataStreams2    pfnCopyAlternateDataStreams2 = NULL;
extern "C" PFNProcessIsNormalUser          pfnProcessIsNormalUser = NULL;
extern "C" PFNPolicyCheckHideDrives        pfnPolicyCheckHideDrives = NULL;
extern "C" PFNPolicyCheckRunOnly           pfnPolicyCheckRunOnly = NULL;
extern "C" PFNPolicyCheckNoEntireNetwork   pfnPolicyCheckNoEntireNetwork = NULL;
extern "C" PFNProcessCanUpdateRegKey       pfnProcessCanUpdateRegKey = NULL;
extern "C" PFNProcessSetPrivilege          pfnProcessSetPrivilege = NULL;
extern "C" PFNProcessTakeFileOwnership     pfnProcessTakeFileOwnership = NULL;
extern "C" PFNFileHasAlternateDataStreams  pfnFileHasAlternateDataStreams = NULL;

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
	char szLang[IMAX_PATH];
	char szFilename[IMAX_PATH];
	char lFile[IMAX_PATH];
	char szDebug[DEBUG_STR_LEN];
	DWORD dwUserNameSize;

	REF(lpvReserved);

	if (hInstance == 0) {
		hInstLang = hInstance = hinstDLL;
		RegCreateKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
		
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
			//dll main
			
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
				dprintf("Loaded Acta resource DLL\n");
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
				dprintf("Loaded PScan resource DLL\n");
				::g_hInstRes = g_ResLoader.GetResourceInstance();
			}
			else
			{
				dprintf("Failed to load PScan resource DLL\n");
				return FALSE;
			}

            // Load NAVNTUTL.  This DLL gives us access to security descriptor
			// functions and multiple stream support.

			dwUserNameSize = sizeof(CurrentUserName);

            GetUserName(CurrentUserName, &dwUserNameSize);

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

			break;

		case DLL_PROCESS_DETACH:
			DeinitDLL(hInstance);

            // Closing the open registry keys to eliminate handle leaks
            hCommonKey = GetCommonKey();
            if(hCommonKey)
                RegCloseKey(hCommonKey);
        
            if(hMainKey)
                RegCloseKey(hMainKey);

            if (hNavNtUtl)
            {
                FreeLibrary(hNavNtUtl);

                hNavNtUtl = NULL;
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
            }
			break;
		}

	return TRUE;
}
/*****************************************************************************************************************/
VOID Real_dprintfTagged(DEBUGFLAGTYPE dwTag, const char *format,...) {

	va_list marker;
	char line[MAX_DEBUGPRINT_STR];
	DWORD out;

	if (!debug)
		return;

	// check skipping untagged lines

    if (dwTag == 0 && (debug & DEBUGEXCLUDEUNTAGGED) )
    {
		return;
    }

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

	if (debug&DEBUGOUTPUT)
		OutputDebugString(line);

	if (DebugHan)
		WriteConsole(DebugHan,line,strlen(line)+1,&out,NULL);
#endif

	va_end(marker);

}
/*****************************************************************************************************************/
void Real_dprintf(const char *format,...) {

	va_list marker;
	char line[MAX_DEBUGPRINT_STR];
	DWORD out;

	if (!debug)
		return;

	// check skipping untagged lines

    if ( debug & DEBUGEXCLUDEUNTAGGED )
    {
		return;
    }

	va_start(marker, format);
	ssvsnprintf(line,sizeof(line),format,marker);
	line[sizeof(line)-1] = 0;
#ifdef _CONSOLE
	printf("%s", line);
#else
	if (debug&DEBUGLOG)
		LogLine(line,FALSE);

	if (debug&DEBUGOUTPUT)
		OutputDebugString(line);

	if (DebugHan)
		WriteConsole(DebugHan,line,strlen(line)+1,&out,NULL);
#endif

	va_end(marker);

}
/*********************************************************************************************************/
DWORD LogLine (char *line,BOOL write) {

	FILE *fp;
	int i;

	REF(write);
	fp = fopen (DebugLogFile,"at");
	for (i=0 ; !fp && i<5 ; i++) {
		Sleep (50);
		fp = fopen (DebugLogFile,"at");
		}
	if (fp) {
		if (line)
			fprintf (fp,"%s",line);
		flushall();
		fclose (fp);
		}

	return 0;
}
/**************************************************************************************************************/
int NTSGetComputerName(char *pComputerName,wchar_t *pWchar) {
	DWORD s = NAME_SIZE;
	REF(pWchar);
	return GetComputerName(pComputerName,&s);
}
