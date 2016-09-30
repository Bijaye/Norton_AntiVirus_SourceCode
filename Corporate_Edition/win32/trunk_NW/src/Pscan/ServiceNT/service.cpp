// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#define IncDebugParser

// Required to expose newer APIs.
#define _WIN32_WINNT 0x0501

#include "pscan.h"
#include "ccosinfo.h"
#include "finetime.h"
#include "cCriticalSection.h"
#include "SymSaferRegistry.h"
#include "scandlg.h"
#include "WtsSessionPublisher.h"

#ifndef NLM
#include <windows.h>
#ifdef ENFORCE_LICENSING
#include "oem_common.h"
#include "license.h"
#include "oem_obj.h"
#endif // #ifdef ENFORCE_LICENSING
#endif // #ifndef NLM

#include "DarwinResCommon.h" 
CResourceLoader g_ResLoader(&_ModuleRes, _T("PScanRes.dll"));
CResourceLoader g_ResActa(&_ModuleRes, _T("ActaRes.dll"));
HINSTANCE g_hInstRes = NULL;

#define LOG_BUF_SIZE 256
HINSTANCE hInstance=0;
HINSTANCE hInstLang = 0;
char japan = 0;
DEBUGFLAGTYPE debug = 0;
char HaveUI = TRUE;
extern "C" HANDLE                  hServDoneEvent = NULL;
SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwGlobalErr=0,dwStopCode=0;

char running = 0;

HANDLE  g_hInst=0;
HWND    ghwndIntelApp=0;
HWND GetConsoleHwnd(void);
char DebugLogFile[IMAX_PATH];

VOID WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);
DWORD WINAPI HandlerEx(DWORD dwCtrlCode, 
						DWORD dwEventType, LPVOID lpEventData, 
						LPVOID lpContext);
void HandleSessionChange(DWORD dwEventType, const PWTSSESSION_NOTIFICATION psWTSSesssionNotification);

#ifdef WIN32
extern "C" BOOL  ReportStatusToSCMgr(DWORD dwCurrentState,
									 DWORD dwWin32ExitCode,
									 DWORD dwWaitHint);
#endif // #ifdef WIN32

DWORD LogLine (char *line,BOOL write);

// This is used to determine if the static C++ objects have likely already
// been destroyed. So, code should not depend on them being there.
static bool s_bStaticObjectsDestroying = false;

char AbortNow = FALSE;

//IBARILE 1/14/03 Port CRT fix for Siebel defect # 1-O5GS2
void SetLoggedOff(DWORD dLoggedOff, BOOL bALL);
//IBARILE 1/14/03 end port

extern "C" HINSTANCE                       hNavNtUtl = NULL;
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

extern "C" HINSTANCE                       hSfc = NULL;
extern "C" PFNSfcIsFileProtected	       pfnSfcIsFileProtected = NULL;

INIT_NAMED_LOCK(PscanServiceMutex);

/******************************************************************************/
#ifdef REAL_SERVICE
	static HANDLE han = NULL;
#endif
/******************************************************************************/
VOID main(int argc,char *argv[])
{
//	LCID locale;
//	char lang[32];
//	char lFile[IMAX_PATH];
	char *q;
#ifdef REAL_SERVICE
	DWORD cbData;
	HKEY hkey = NULL;
	char szDebug[DEBUG_STR_LEN];
#endif
	char szLang[IMAX_PATH];
	char szFilename[IMAX_PATH];
#ifdef _DEBUG
	int tmpFlag = 0;
#endif // _DEBUG

	SERVICE_TABLE_ENTRY dispatchTable[] = {
		  { SERVICE_NAME, service_main },
		  { NULL, NULL }
	};


    //  CRT Fix: 1-RPZBJ - University of Tsukuba: SAVCE has vulnerability issue with IME2000/2002
    //  This api function call disables IME for the application.  It doesn't stop IME, it just
    //  indicates our applicaiton should not interact with IME.
    {
        typedef BOOL (WINAPI *LP_ImmDisableIME)(DWORD);

		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath));
		::GetSystemDirectory( szPath, MAX_PATH );
		_tcscat( szPath, _T("\\IMM32.dll"));
		
		HMODULE hLib = LoadLibrary( szPath );
        LP_ImmDisableIME func;
        if ( hLib )
        {
            func = (LP_ImmDisableIME)GetProcAddress( hLib, "ImmDisableIME" );
            if ( func )
            {
                func((DWORD)-1);
            }
            FreeLibrary(hLib);
        }
    }
    //  End CRT Fix: 1-RPZBJ

    REF(argc);

#ifdef _DEBUG
	// For Debug builds, tell the CRT heap to track memory allocations
	// and dump a table of unfreed blocks at application exit.
	tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
//	tmpFlag |= _CRTDBG_CHECK_CRT_DF; // add this back to track CRT's own allocs as well
	_CrtSetDbgFlag( tmpFlag );
#endif // _DEBUG

	g_hInst = GetModuleHandle(argv[0]);
	hInstance = (HINSTANCE)g_hInst;
	sssnprintf (DebugLogFile,sizeof(DebugLogFile),"vpdebug.log");

	InitDLL(hInstance);

#ifdef REAL_SERVICE
	RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER"\\ProductControl", &hkey);

	if (hkey)
    {
		cbData = sizeof(szDebug);
		if( SymSaferRegQueryValueEx(hkey, szReg_Val_Debug, NULL, NULL, (LPBYTE)szDebug, &cbData) != ERROR_SUCCESS )
			memset(szDebug, 0, sizeof(szDebug));

		debug = ParseDebugFlag(szDebug);

		if (debug&DEBUGLOG)
        {
			time_t t;
			char str[128];
			t = time(NULL);
			sssnprintf(str,sizeof(str),"\n\nSystem Start at %s\n\n",ctime(&t));
			LogLine(str,TRUE);
		}

		RegCloseKey(hkey);
	}

	if ( !(debug&DEBUGNOWINDOW) && debug )
    {
		AllocConsole();
		han = GetStdHandle(STD_OUTPUT_HANDLE);
	}
#endif

	q = strrchr(argv[0],'\\');
	if (q)
		*q = 0;
	SetCurrentDirectory(argv[0]);

	//Load the Acta strings:
	if (SUCCEEDED(g_ResActa.Initialize(false)))
	{
		dprintf("Loaded Acta resource DLL\n");
		hInstLang = g_ResActa.GetResourceInstance();
	}
	else
	{
		dprintf("Failed to load Acta resource DLL\n");
	}

	//Load the PScan resources:
	if (SUCCEEDED(g_ResLoader.Initialize(false)))
	{
		dprintf("Loaded PScan resource DLL\n");
		::g_hInstRes = g_ResLoader.GetResourceInstance();
	}
	else
	{
		dprintf("Failed to load PScan resource DLL\n");
	}

	if ((hInstLang != NULL) && (::g_hInstRes != NULL))
	{
		dprintf ("Trying to Start "SERVICE_NAME" Service\n");
#ifdef REAL_SERVICE
		StartServiceCtrlDispatcher(dispatchTable);
#else
		service_main(0,NULL);
#endif
	}
	else
	{
		dprintf("A resource DLL could not be found - unable to attempt service start.\n");
	}

    UnLoadScanDlg();

	DeinitDLL(hInstance);

    // Static C++ objects are going to be destroying shortly, do not depend
    // on them.
    s_bStaticObjectsDestroying = true;

}
/*******************************************************************************/
void Report(int i) {

	EVENTBLOCK gl;
	char line[128];

	memset(&gl,0,sizeof(EVENTBLOCK));

	switch (i)
    {
		case 1: gl.Description = "Licence Data is invalid.\r\nReal-Time scanning for viruses has been deactivated.";break;
		case 2: gl.Description = "Trial Time has expired.\r\nReal-Time scanning for viruses has been deactivated.";break;
		case 3: gl.Description = "Trial Time has expired.\r\nReal-Time scanning for viruses will be deactivated.";break;
		case 4: sssnprintf(line,sizeof(line),"Could  not start Service Engine err=%08X",dwGlobalErr); gl.Description = line;break;
	}
	gl.logger = 2;
	gl.Category = GL_CAT_SECURITY;
	gl.Event = GL_EVENT_STARTUP;

	GlobalLog(&gl);
}
/*******************************************************************************/
#ifdef REAL_SERVICE

#ifndef NLM
#ifdef ENFORCE_LICENSING
void ConfigAPOnOff(DWORD dOn)
{
	// Enable all AutoProtect stuff 
	TCHAR szRegKeyName[1024];
	HKEY hKey = NULL;
	DWORD dwBufLen = sizeof(DWORD);
	DWORD dValue  = dOn; 

	sssnprintf(szRegKeyName, sizeof(szRegKeyName), "%s\\Storages\\FileSystem\\RealTimeScan", _T(szReg_Key_Main));
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegKeyName, 0, KEY_WRITE, &hKey) )
	{
		if (ERROR_SUCCESS == RegSetValueEx(hKey, szReg_Val_RTSScanOnOff, 0, REG_DWORD, (LPBYTE)&dValue, dwBufLen))
		{
		}
		RegCloseKey(hKey);
	}

	sssnprintf(szRegKeyName, sizeof(szRegKeyName), "%s\\Storages\\InternetMail\\RealTimeScan", szReg_Key_Main);
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegKeyName, 0, KEY_WRITE, &hKey) )
	{
		dwBufLen = sizeof(DWORD);
		dValue  = dOn;  
		if (ERROR_SUCCESS == RegSetValueEx(hKey, szReg_Val_RTSScanOnOff, 0, REG_DWORD, (LPBYTE)&dValue, dwBufLen))
		{
		}
		RegCloseKey(hKey);
	}

	sssnprintf(szRegKeyName, sizeof(szRegKeyName), "%s\\Storages\\LotusNotes\\RealTimeScan", szReg_Key_Main);
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegKeyName, 0, KEY_WRITE, &hKey) )
	{
		dwBufLen = sizeof(DWORD);
		dValue  = dOn; 
		if (ERROR_SUCCESS == RegSetValueEx(hKey, szReg_Val_RTSScanOnOff, 0, REG_DWORD, (LPBYTE)&dValue, dwBufLen))
		{
		}
		RegCloseKey(hKey);
	}

	sssnprintf(szRegKeyName, sizeof(szRegKeyName), "%s\\Storages\\MicrosoftExchangeClient\\RealTimeScan", szReg_Key_Main);
	if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, szRegKeyName, 0, KEY_WRITE, &hKey) )
	{
		dwBufLen = sizeof(DWORD);
		dValue  = dOn;  
		if (ERROR_SUCCESS == RegSetValueEx(hKey, szReg_Val_RTSScanOnOff, 0, REG_DWORD, (LPBYTE)&dValue, dwBufLen))
		{
		}
		RegCloseKey(hKey);
	}
}

void EnableInitialAP()
{
	ConfigAPOnOff(1);
}

void DisableInitialAP()
{
	ConfigAPOnOff(0);
}
void OEMCustomerBootThread(void *nothing) 
{
	DWORD dWaitStatus = 0;
			while (1)
			{
				//
				// Checking Customer Boot is fickle during bootup as far 
				// as SysPrep is concerned. Registry switches get toggled during
				// boot and rtvscan just launches too quickly sometimes before
				// SysPrep has time to determine if a customer boot is
				// enabled.
				//
				// So launch this thread to check and protect customer at earliest possible time
				//
		if (OEMObj.IsOEM_CustomerBoot())
				{

			if (OEMObj.IsOEM_DoEnableAP() == FALSE)   // Factory install only
					{
						//
						// Dell wants AP to be not guarded by bootcount, but keep tested code just in case they change their mind
						//
				if (OEMObj.CheckOEM_BootCount())
						{
							// enable AP at FIRST CUSTOMER BOOT
							EnableInitialAP();				
					OEMObj.SetOEM_DoEnableAP(TRUE); // Set so we only do this once.
							License_RegisterSubscription();
							// Catfish:
 							// Call SlicStartSubscription in rtvscan startup instead of config wizard
 							if (OEMObj.IsOEM_TrialApp())
 							   License_StartSubscription();

							break;		// Once we have enabled it, get out.
						}
						//else
						//{
						//	DisableInitialAP();
						//}
					}
					else 
					{
						break;	
					}
				}

				// If exit get out!
				dWaitStatus = WaitForSingleObject( hServDoneEvent, 5000);

				if( dWaitStatus != WAIT_TIMEOUT )
				{	
					break;   // oops server is shutting down
				}
			} // while
}

#endif // #ifdef ENFORCE_LICENSING
#endif // #ifndef NLM

void serverTimer(void *nothing)
{
	HKEY hkey = NULL;
	DWORD cbData;
	HWND hwnd;
	HMENU hmenu;
	DEBUGFLAGTYPE newDebug;
	char szDebug[DEBUG_STR_LEN];
	HANDLE hRegChangeEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	REF(nothing);

	if (hRegChangeEvent != NULL && RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER"\\ProductControl", &hkey) == ERROR_SUCCESS)
	{
		bool bExit = false;
		HANDLE rhServerTimerEvents[] = { hRegChangeEvent, hServDoneEvent };

		while (!bExit)
		{
			RegNotifyChangeKeyValue(hkey,TRUE,REG_NOTIFY_CHANGE_LAST_SET,hRegChangeEvent,TRUE);
			switch (::WaitForMultipleObjects(2, rhServerTimerEvents, false, INFINITE))
			{
			case WAIT_OBJECT_0:
#ifdef SAV_HANDLE_EXCEPTIONS
				__try
				{
#endif //SAV_HANDLE_EXCEPTIONS
					cbData = sizeof(szDebug);
					if( (SymSaferRegQueryValueEx(hkey, szReg_Val_Debug, NULL, NULL, (LPBYTE)szDebug, &cbData)) != ERROR_SUCCESS )
						memset(szDebug, 0, sizeof(szDebug));

					newDebug = ParseDebugFlag(szDebug);
					if (newDebug != debug)
					{
						if ((newDebug&DEBUGLOG) != (debug&DEBUGLOG))
							LogLine(" ",TRUE);

						if( newDebug && !(newDebug&DEBUGNOWINDOW) && han == NULL )
						{
							// We're debugging, the user did not specify DEBUGNOWINDOW, and
							// there is no current window ... create a debug window.
							AllocConsole();
							han = GetStdHandle(STD_OUTPUT_HANDLE);
							hwnd = GetConsoleHwnd();
							hmenu = GetSystemMenu(hwnd,FALSE);
							EnableMenuItem(hmenu,SC_CLOSE,MF_DISABLED|MF_GRAYED);
							Sleep(500);
							dprintf("Debug Window Now Active\n");
						}
						else if( newDebug == 0 || (newDebug&DEBUGNOWINDOW && han != NULL) )
						{
							// We're no longer debugging, or the user specified DEBUGNOWINDOW
							// and there is a current window .. close the debug window
							dprintf("Request to close Debug Window\n");
							Sleep(2000);
							han = NULL;
							FreeConsole();
						}
						debug = newDebug;
					}
#ifdef SAV_HANDLE_EXCEPTIONS
				}
				__except(1)
				{
					NTxBeginThread(serverTimer,NULL);
					bExit = true;
				}
#endif //SAV_HANDLE_EXCEPTIONS
				break;
			case WAIT_OBJECT_0 + 1:
				bExit = true;
				break;
			}
		}
	}
	else
	{
		dprintf(_T("Failed to initialize debug registry watch thread\n"));
	}

	RegCloseKey(hkey);
	CloseHandle(hRegChangeEvent);
}
#endif

/*******************************************************************************/
VOID WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	DWORD cc;
	HWND hwnd;
	HMENU hmenu;
	DWORD cbData;
	HKEY hkey = NULL;

	REF(dwArgc);
	REF(lpszArgv);
	
#ifdef REAL_SERVICE
	// RegisterServiceCtrlHandlerEx requires Windows XP, Windows 2000 Professional, Windows Server 2003 or Windows 2000 Server
	sshStatusHandle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, HandlerEx, NULL);
	// The service status handler (sshStatusHandle) does not need to be closed when we are done.
	
	if (!sshStatusHandle)
		goto dump;
#endif
	hwnd = GetConsoleHwnd();
	hmenu = GetSystemMenu(hwnd,FALSE);
	EnableMenuItem(hmenu,SC_CLOSE,MF_DISABLED|MF_GRAYED);

	ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ssStatus.dwServiceSpecificExitCode = 0;

#ifdef WIN32
	// Takes aproximately 18 seconds to fully startup on P4 class machine - use 30 for margin of error. 
	ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 30*1000);

	// This seems suspicious and should be reviewed - we are reporting that we are up and running 
	// long before we really are in that state. This may be so we can attach to the debugger
	// without timing out. Since it isn't causing any known problems I am not going to change it.
	if (!ReportStatusToSCMgr(SERVICE_RUNNING,NO_ERROR,0))
		goto dump;
#endif // #ifdef WIN32

#ifdef REAL_SERVICE
	hServDoneEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if (hServDoneEvent == (HANDLE)NULL)
		goto dump;
#endif

    // Load NAVNTUTL if we're running on NT. This DLL gives
    // us access to security descriptor functions and multiple
    // stream support.

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
	hSfc = LoadLibrary( szPath );
	if (hSfc)
	{
		pfnSfcIsFileProtected = (PFNSfcIsFileProtected) GetProcAddress( hSfc, SFCISFILEPROTECTED);
	}

#ifndef NLM
#ifdef ENFORCE_LICENSING
#ifdef REAL_SERVICE
	if (OEMObj.IsOEMBuild())
	{
		License_RegisterSubscription();
		// Catfish:
 		// Call SlicStartSubscription in rtvscan startup instead of config wizard
 		if (OEMObj.IsOEM_TrialApp())
 			License_StartSubscription();

		if (!OEMObj.IsOEM_TrialApp())
			NTxBeginThread(OEMCustomerBootThread,NULL);	
	}
#endif
#endif
#endif
//		_asm int 3;

	cc = InitPscan();
	if (cc)
    {
		dwGlobalErr = cc;
		dwStopCode = 10;
		dprintf("PScan Did Not Install, Error Code=%08X\n",cc);
		Report(4);
		goto dump;
	}

	dprintf("Pscan installed ok\n");

#ifdef WIN32
	if (!ReportStatusToSCMgr(SERVICE_RUNNING,NO_ERROR,0))
		goto cleanup;
#endif // #ifdef WIN32

	running = 1;

#ifdef REAL_SERVICE

	// Start the thread that will watch for Debug flag changes.

	NTxBeginThread(serverTimer,NULL);

	// This is the main loop of the main service thread.

	do
	{
		cc = WaitForSingleObject( hServDoneEvent, 60*60*1000 );

		if( cc == WAIT_TIMEOUT )
		{
			// This used to check licensing information.  I've left the loop in
			// because it might come in handy in the future.
		}

	} while (cc == WAIT_TIMEOUT);

#else

	getch();

#endif

	running = 0;

cleanup:
#ifdef WIN32
	// Notify APEnabler of shutdown
	APEnablerShutdown();
	AdminForcedLUShutdown();
	ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 60*1000);
#endif //WIN32

	running = 2;
	LOCK();

#ifdef REAL_SERVICE
	 if (hServDoneEvent != NULL)
		  CloseHandle(hServDoneEvent);
#endif

	if (!AbortNow)
		DeInitPscan();
#ifdef WIN32
	ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 30*1000);
#endif // #ifdef WIN32

	UNLOCK();

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

	if (hSfc)
	{
		FreeLibrary(hSfc);

		hSfc = NULL;
		pfnSfcIsFileProtected = NULL;
	}

dump:
	RegOpenKey(HKEY_LOCAL_MACHINE, REGHEADER"\\ProductControl", &hkey);

	if (hkey)
    {
		cbData = sizeof(DWORD);
		RegSetValueEx(hkey, "LastStatusCode", 0, REG_DWORD,(BYTE*)&dwGlobalErr, cbData);
		RegCloseKey(hkey);
	}

	dprintf("Status = %08X\n",dwGlobalErr);

//	if (WaitForMess == 1) {
//		dprintf("Waiting for messageBox to close.");
//		while (WaitForMess == 1) {
//			ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 3000);
//			Sleep(1000);
//			dprintf(".");
//			}
//		dprintf("\n");
//		}

	if (debug&DEBUGPRINT)
    {
		dprintf("Closing...Debug screen will close in 2 sec.\n");
#ifdef WIN32
		ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 3000);
#endif // #ifdef WIN32
		Sleep(2000);
	}

	if (debug&DEBUGLOG)
      	LogLine("STOP\n",TRUE);

#ifdef WIN32
    // This has got to be the very last thing this function does. Once the SCM
    // is informed that we're stopped, the main thread will regain control and
    // may finish before this thread (the service thread). If you have last
    // minute process cleanup, it belongs either above this call or else at the
    // end of the real main() above.
	if (sshStatusHandle != 0)
		ReportStatusToSCMgr(SERVICE_STOPPED, dwStopCode,0);
#endif // #ifdef WIN32

	return;
}
/*******************************************************************/
DWORD Terminate(void)
{
	AbortNow = TRUE;
#ifdef WIN32
	ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 3000);
#endif // #ifdef WIN32
	SetEvent(hServDoneEvent);

	return 0;
}
/*******************************************************************/

DWORD WINAPI HandlerEx(DWORD dwCtrlCode, 
									DWORD dwEventType, LPVOID lpEventData, 
									LPVOID lpContext)
{
	DWORD dwResult = ERROR_CALL_NOT_IMPLEMENTED;
	REF(lpContext);

	switch(dwCtrlCode)
    {
		case SERVICE_CONTROL_STOP:
			dprintf(_T("Service notified that it should stop\n"));
			ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 60*1000);
			SetEvent(hServDoneEvent);
			return NO_ERROR;
		case SERVICE_CONTROL_SHUTDOWN:
			dprintf(_T("Service notified that system is shutting down\n"));
			//IBARILE 1/14/03 Port CRT fix for Siebel defect # 1-O5GS2
			SetLoggedOff(0, TRUE);   // Assume reboot/shutdown/reinstall if we are shutting down.
			//IBARILE 1/14/03 end Port CRT
			PutVal(hMainKey,"Status",0);
			ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 60*1000);
			SetEvent(hServDoneEvent);
			return NO_ERROR;
		case SERVICE_CONTROL_SESSIONCHANGE:
		{
			// HandleSessionChange will log control specific messages and notify all
			// registered listeners.
			CWtsSessionPublisher* poPublisher = CWtsSessionPublisher::Instance();
			if (poPublisher)
			{
				poPublisher->HandleSessionChange(dwEventType, (PWTSSESSION_NOTIFICATION) lpEventData);
			}
			dwResult = NO_ERROR;
			break;
		}
		default:
			dprintf(_T("Unhandled service control message: %u\n"), dwCtrlCode);
			dwResult = ERROR_CALL_NOT_IMPLEMENTED;
			break;
	}
	return dwResult;
}

/************************************************************************/
BOOL ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode,
                         DWORD dwWaitHint)
{
	BOOL fResult;

	// Disable control requests until the service is started.
	//
	if (dwCurrentState == SERVICE_START_PENDING)
		ssStatus.dwControlsAccepted = 0;
	else
	{
        ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		// If we are in the process of stopping we no longer care about session changes
		if (SERVICE_STOP_PENDING != dwCurrentState && SERVICE_STOPPED != dwCurrentState &&
			SERVICE_PAUSE_PENDING != dwCurrentState && SERVICE_PAUSED != dwCurrentState)
		{
			// If this OS supports session change notification then add that filter. 
			// Win2K does NOT support SERVICE_ACCEPT_SESSIONCHANGE.
			ccLib::COSInfo oOSInfo;
			if (oOSInfo.IsWinXP(true))
			{
				ssStatus.dwControlsAccepted |= SERVICE_ACCEPT_SESSIONCHANGE;
			}
		}
	}

	ssStatus.dwCurrentState = dwCurrentState;
	ssStatus.dwWin32ExitCode = dwWin32ExitCode;

	// The checkpoint should be incremented for pending operations and reset to
	// zero when set to a final state. This logic assumes we don't go from one pending state to another
	// without going to one of the final states in between which resets the checkpoint to zero.
	static DWORD g_dwServiceStatusCheckPoint=0;
	if (SERVICE_START_PENDING == dwCurrentState || SERVICE_STOP_PENDING == dwCurrentState ||
		SERVICE_PAUSE_PENDING == dwCurrentState || SERVICE_CONTINUE_PENDING == dwCurrentState)
	{
		g_dwServiceStatusCheckPoint++;
	}
	else
	{
		g_dwServiceStatusCheckPoint = 0;
	}
	ssStatus.dwCheckPoint = g_dwServiceStatusCheckPoint;

	ssStatus.dwWaitHint = dwWaitHint;

	dprintf("Setting service status to State=%u; Error=%u; CheckPoint=%u; WaitHint=%u\n",dwCurrentState, dwWin32ExitCode, ssStatus.dwCheckPoint, dwWaitHint);
#ifdef REAL_SERVICE
	if (!(fResult = SetServiceStatus(
				 sshStatusHandle,    // service reference handle
				 &ssStatus)))        // SERVICE_STATUS structure
   {
        // If an error occurs, stop the service.
		dprintf("Unexpected error calling SetServiceStatus...stopping service\n");
		SetEvent(hServDoneEvent);
	}
#else // Not REAL_SERVICE
	fResult = TRUE;
#endif // #ifdef REAL_SERVICE

	return fResult;
}

//============================================================================
static CriticalSection s_DprintfSentry;
void Real_dprintfTaggedImpl(DEBUGFLAGTYPE dwTag, LPCSTR format, va_list* marker)
{
	DWORD out;
	char line[MAX_DEBUGPRINT_STR];

	// check skipping untagged lines

    if (dwTag == 0 && (debug & DEBUGEXCLUDEUNTAGGED) )
		return;

	if (HaveUI && (debug&DEBUGPRINT))
    {
        // There is at least one case where our static objects have be
        // destroyed, but this routine is still called. This occurs when the
        // cscan.cpp:ClientScanner() thread attempts to output a debug
        // message. For that case, we simply will ignore synchronization on
        // dprintf during the later stages of RtvScan's shutdown.
        if (s_bStaticObjectsDestroying == false)
            s_DprintfSentry.Enter ();

        DWORD dwFineTime = GetFineLinearTimeEx( ONEDAY_MAX_RES );

        DWORD dwSeconds = dwFineTime / ONEDAY_MAXRES_TICKS_PER_SECOND;
        DWORD dwMicros  = OneDayMaxResToMicroSeconds( dwFineTime % ONEDAY_MAXRES_TICKS_PER_SECOND );
        DWORD dwHours, dwMins, dwSecs;

        SecondsToHoursMinsSecs( dwSeconds, dwHours, dwMins, dwSecs );

		if (debug&DEBUGTHREADIDS)
            sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d[_%X]|", dwHours, dwMins, dwSecs, dwMicros, NTxGetCurrentThreadId( ) );
		else
			sssnprintf(line, sizeof(line), "%02d:%02d:%02d.%06d|", dwHours, dwMins, dwSecs, dwMicros );

		ssvsnprintfappend(line, sizeof(line), format, *marker);
		line[sizeof(line)-1] = 0;

#ifdef REAL_SERVICE
		if (han)
			WriteConsole(han,line,strlen(line)+1,&out,NULL);
#endif

		if (debug&DEBUGLOG)
			LogLine(line,FALSE);

		if (debug&DEBUGOUTPUT)
			OutputDebugString(line);

        if (s_bStaticObjectsDestroying == false)
            s_DprintfSentry.Leave ();
    }
}

//============================================================================
void Real_dprintfTagged( DEBUGFLAGTYPE dwTag, LPCSTR format, ... )
{
    va_list marker;

    va_start(marker, format);

    Real_dprintfTaggedImpl( dwTag, format, &marker );

    va_end(marker);
}

//============================================================================
void Real_dprintf( LPCSTR format, ... )
{
    if (debug & DEBUGEXCLUDEUNTAGGED)
        return;

    va_list marker;

    va_start(marker, format);

    Real_dprintfTaggedImpl( 0, format, &marker );

    va_end(marker);
}

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
/************************************************************************************/
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
		//flushall();
		fflush(fp);
		fclose (fp);
	}

	return 0;


// I have put it back to writting asap because when I am tring to debug crash problems I need every line flushed ASAP.

/*	FILE *fp;
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

	return 0; */
}

//IBARILE 1/14/03 port CRT fix Siebel defect #1-O5GS2
void SetLoggedOff(DWORD dLoggedOff, BOOL bALL)
{
	if (!bALL)
	{
		TCHAR szUserName[1024];
		TCHAR szRegValueName[1024];
		DWORD dNameSize = 1024;
		HKEY hKey = NULL;



		_tcscpy(szUserName, "");
		_tcscpy(szRegValueName, "");
		
		if (GetUserName(szUserName, &dNameSize))
			sssnprintf(szRegValueName, sizeof(szRegValueName), "%s%s", szReg_Logged_Off, szUserName);
		else
            _tcscpy(szRegValueName, szReg_Logged_Off);

		// Defect 1-O5GS2 --- only run StartupScan if we detected that there wasn't a successful log off
		// before vptray.exe restarted.

		// check and see if we are sunning on the server
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,0,
			KEY_WRITE,&hKey)== ERROR_SUCCESS) 
		{
			DWORD dwSize = sizeof(DWORD);
			RegSetValueEx(	hKey,
								szRegValueName,
								0,
								REG_DWORD,
								(BYTE*)&dLoggedOff,
								dwSize );

			RegCloseKey(hKey);
		}
	}
	else // Reset ALL ---> we are doing a reboot
	{
		// Defect 1-O5GS2 --- only run StartupScan if we detected that there wasn't a successful log off
		// before vptray.exe restarted.
		HKEY hKey = NULL;
		int dIndex = 0;

		// check and see if we are sunning on the server
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,0,
			KEY_READ | KEY_WRITE,&hKey)== ERROR_SUCCESS) 
		{
			LONG dStatus = ERROR_SUCCESS;
			TCHAR sNameKey[1024];
			TCHAR sKeyValueValue[1024];
			DWORD dSizeName = 1024;
			DWORD dSizeValue = 1024;
			DWORD lpType = REG_DWORD;
			while (dStatus == ERROR_SUCCESS)
			{
				dSizeName = 1024;
				dSizeValue = 1024;
				dStatus = RegEnumValue(hKey, dIndex, sNameKey, &dSizeName, 0, &lpType, (LPBYTE) sKeyValueValue, &dSizeValue);
			
				if ((SUCCEEDED(dStatus)) && (dStatus != ERROR_NO_MORE_ITEMS))
				{
					if (_tcsstr(sNameKey, szReg_Logged_Off) != NULL)   // LoggedOff param found
					{
						DWORD dwSize = sizeof(DWORD);
						RegSetValueEx(	hKey,
											sNameKey,
											0,
											REG_DWORD,
											(BYTE*)&dLoggedOff,
											dwSize );
					}
				}

				dIndex++;
			}

			RegCloseKey(hKey);
		}
    }
}
//IBARILE 1/14/03 end CRT port
