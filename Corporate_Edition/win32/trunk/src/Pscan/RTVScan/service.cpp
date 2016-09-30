// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// Required to expose newer APIs.
#define _WIN32_WINNT 0x0501

#include "pscan.h"
#include "ccosinfo.h"
#ifdef WIN32
#include "ccCommandLine.h"
#endif //WIN32
#include "cCriticalSection.h"
#include "SymSaferRegistry.h"
#include "scandlg.h"
#include "WtsSessionPublisher.h"
#include "RtvscanModule.h"
#include "vpexceptionhandling.h"
#include "tlsimpersonate.h"
#include "ccVerifyTrustLoader.h"

#ifndef NLM
#include <windows.h>
#ifdef ENFORCE_LICENSING
#include "license.h"
#include "OEMSettings.h"
#endif // #ifdef ENFORCE_LICENSING
#endif // #ifndef NLM

// Resource relocation
#include "DarwinResCommon.h" 
CResourceLoader g_ResLoader(&_ModuleRes, _T("PScanRes.dll"));
CResourceLoader g_ResActa(&_ModuleRes, _T("ActaRes.dll"));
HINSTANCE g_hInstRes = NULL;

HINSTANCE hInstance=0;
HINSTANCE hInstLang = 0;
char japan = 0;
extern "C" HANDLE                  hServDoneEvent = NULL;
SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwGlobalErr=0,dwStopCode=0;

char running = 0;

HANDLE  g_hInst=0;
HWND    ghwndIntelApp=0;

VOID WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);
DWORD WINAPI HandlerEx(DWORD dwCtrlCode, 
						DWORD dwEventType, LPVOID lpEventData, 
						LPVOID lpContext);
void HandleSessionChange(DWORD dwEventType, const PWTSSESSION_NOTIFICATION psWTSSesssionNotification);

#ifdef _DEBUG
#ifdef WIN32
bool ProcessCOMRegistration(bool bRegister);
#endif // WIN32
void ProcessCommandLine();
#endif // _DEBUG

#ifdef WIN32
extern "C" BOOL  ReportStatusToSCMgr(DWORD dwCurrentState,
									 DWORD dwWin32ExitCode,
									 DWORD dwWaitHint);
static bool IsTrustedFile (const TCHAR *szPath);
#endif // #ifdef WIN32


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

#ifdef _USE_CCW
extern "C" PFNCopyAlternateDataStreams2W    pfnCopyAlternateDataStreams2W = NULL;
extern "C" PFNGetFileSecurityDescW          pfnGetFileSecurityDescW = NULL;
extern "C" PFNSetFileSecurityDescW          pfnSetFileSecurityDescW = NULL;
#endif

INIT_NAMED_LOCK(PscanServiceMutex);

#ifdef WIN32
CRTVScanModule _AtlModule;

// Command line options for RTVScan Module
static LPCTSTR szRegServer[] = {"RegServer"};
static LPCTSTR szUnRegServer[] = {"UnRegServer"};
#endif //WIN32

/******************************************************************************/
VOID main(int argc,char *argv[])
{
//	LCID locale;
//	char lang[32];
//	char lFile[IMAX_PATH];
	char *q;
	char szLang[IMAX_PATH];
	char szFilename[IMAX_PATH];
#ifdef _DEBUG
	int tmpFlag = 0;
#endif // _DEBUG

    //create TLS slot for permissions to be cached
    CTlsImpersonate::CreateSlot();

#ifdef _DEBUG
	// Debug only functionality to register / unregister Rtvscan COM objects.
	// We do not want to give users the ability to unregister Rtvscan.
	// This is left here in case there are issues extracting COM information
	// from the installer.
	if (argc > 1)
		ProcessCommandLine();
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

	InitDLL(hInstance);

#ifdef REAL_SERVICE
	if (InitializeDebugLogging() == ERROR_SUCCESS)
    {
		time_t tStart = time(NULL);

		dprintf("\n\nSystem Start at %s\n\n",ctime(&tStart)); // TODO:  Check that this works.
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

	if (GetLanguage( szLang,szFilename,"%sRRTV.DLL"))
    {
		if (!ssstricmp(szLang,"JPN"))
			japan = 1;
		dprintf("Loading language file %s\n",szFilename);
		hInstLang = LoadLibrary(szFilename);
	}

	if (!hInstLang) // if load failed just use english
		hInstLang = hInstance;

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

	if (hInstLang && hInstLang != g_hInst)
		FreeLibrary(hInstLang);

	DeinitDLL(hInstance);

    // Static C++ objects are going to be destroying shortly, do not depend
    // on them.
    s_bStaticObjectsDestroying = true;

}

#ifdef _DEBUG
void ProcessCommandLine()
{
#ifdef WIN32
	try
	{
		ccLib::CCommandLine cmdLine;
		if (cmdLine.Parse())
		{
			// Handle COM Registration
			// '/RegServer'
			if (cmdLine.GetSwitchSet(szRegServer, sizeof(szRegServer)/sizeof(szRegServer[0])) != false)
			{
				ProcessCOMRegistration(true);
			}

			// Handle COM UnRegistration
			// '/UnRegServer'
			if (cmdLine.GetSwitchSet(szUnRegServer, sizeof(szUnRegServer)/sizeof(szUnRegServer[0])) != false)
			{
				ProcessCOMRegistration(false);
			}
		}
	}
	VP_CATCH_MEMORYEXCEPTIONS(;)
#endif //WIN32
}

#ifdef WIN32
bool ProcessCOMRegistration(bool bRegister)
{
	USES_CONVERSION;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	HRESULT hrAtl = S_OK;

	if (bRegister)
	{
		// Do not continue if registration fails
		hrAtl = _AtlModule.UpdateRegistryFromResource(IDR_SCANMANAGERSERVICE, TRUE);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UpdateRegistryFromResourceS, %x"), hrAtl);
			return false;
		}

        // Do not continue if registration fails
		hrAtl = _AtlModule.UpdateRegistryFromResource(IDR_RESULTSVIEWCOMCALLBACK, TRUE);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UpdateRegistryFromResourceS, %x"), hrAtl);
			return false;
		}

        // Do not continue if registration fails
		hrAtl = _AtlModule.UpdateRegistryFromResource(IDR_VIRUSFOUNDCOMCALLBACK, TRUE);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UpdateRegistryFromResourceS, %x"), hrAtl);
			return false;
		}

		hrAtl = _AtlModule.RegisterServer(TRUE, NULL);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: RegisterServer, %x"), hrAtl);
			return false;
		}

		hrAtl = _AtlModule.RegisterAppId();
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: RegisterAppId, %x"), hrAtl);
			return false;
		}

		// Set the AppID to LocalServer or Service
		::ATL::CRegKey keyAppID;
		::ATL::CRegKey key;
		LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
		if (lRes != ERROR_SUCCESS)
			dprintf(_T("keyAppID.Open() != S_OK, %l\n"), lRes);

		lRes = key.Create(keyAppID, OLE2T(_AtlModule.GetAppId()));
		if (lRes != ERROR_SUCCESS)
			dprintf(_T("key.Create() != S_OK, %l\n"), lRes);

		key.DeleteValue(_T("LocalService"));
		key.DeleteValue(_T("ServiceParameters"));

		key.SetStringValue(_T("ServiceParameters"), _T(""));
		lRes = key.SetStringValue(_T("LocalService"), _T("Symantec AntiVirus"));
		if (lRes != ERROR_SUCCESS)
			dprintf(_T("key.SetStringValue() != S_OK, %l\n"), lRes);
	}
	else
	{
		hrAtl = _AtlModule.UnregisterAppId();
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UnregisterAppId, %x"), hrAtl);
			return false;
		}

		hrAtl = _AtlModule.UnregisterServer(FALSE);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UnregisterServer, %x"), hrAtl);
			return false;
		}

		hrAtl = _AtlModule.UpdateRegistryFromResource(IDR_SCANMANAGERSERVICE, FALSE);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UpdateRegistryFromResourceS, %x"), hrAtl);
			return false;
		}

        // Do not continue if registration fails
		hrAtl = _AtlModule.UpdateRegistryFromResource(IDR_VIRUSFOUNDCOMCALLBACK, FALSE);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UpdateRegistryFromResourceS, %x"), hrAtl);
			return false;
		}

        // Do not continue if registration fails
		hrAtl = _AtlModule.UpdateRegistryFromResource(IDR_RESULTSVIEWCOMCALLBACK, FALSE);
		if (FAILED(hrAtl))
		{
			dprintf(_T("ProcessCommandLine: Failed UpdateRegistryFromResourceS, %x"), hrAtl);
			return false;
		}


		::ATL::CRegKey keyAppID;
		LONG lRes = keyAppID.Open(HKEY_CLASSES_ROOT, _T("AppID"), KEY_WRITE);
		if (lRes != ERROR_SUCCESS)
			dprintf(_T("keyAppID.Open() != S_OK, %l\n"), lRes);
		else
		{
			::ATL::CRegKey key;
			lRes = key.Open(keyAppID, OLE2T(_AtlModule.GetAppId()), KEY_WRITE);
			if (lRes == ERROR_SUCCESS)
			{
				key.DeleteValue(_T("LocalService"));
				key.DeleteValue(_T("ServiceParameters"));
			}
		}
	}

	CoUninitialize();

	// Registration success
	return true;
}
#endif //WIN32
#endif //_DEBUG

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
	BOOL bSetAPOnAlready = FALSE;
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
		if (OEMSettings.IsOEM_CustomerBoot())
		{
			if (OEMSettings.IsOEM_DoEnableAP() == FALSE)   // Factory install only
			{
					//
					// Dell wants AP to be not guarded by bootcount, but keep tested code just in case they change their mind
					//
					if (OEMSettings.CheckOEM_BootCount())
					{
						bSetAPOnAlready = TRUE;
						// enable AP at FIRST CUSTOMER BOOT
						EnableInitialAP();				
						OEMSettings.SetOEM_DoEnableAP(TRUE); // Set so we only do this once.
						License_RegisterSubscription();
						// Catfish:
 						// Call SlicStartSubscription in rtvscan startup instead of config wizard
 						if (OEMSettings.IsOEM_TrialApp())
 							License_StartSubscription();

						if (OEMSettings.IsOEM_EULAAccepted())
							break;

						//break;		// Once we have enabled it, get out.
					} 
			} //if (OEMSettings.IsOEM_DoEnableAP() == FALSE)
			
		} // if (OEMSettings.IsOEM_CustomerBoot())
		else  // Factory mode?
		{
			if (OEMSettings.IsOEM_DoEnableAP() == TRUE)   // Factory install still on?
			{
				if (bSetAPOnAlready)
				{
					//
					// Re-Enter Factory mode
					//
					DisableInitialAP();				
					OEMSettings.SetOEM_DoEnableAP(FALSE); 
				}
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
					InitializeDebugLogging();
/* TODO:  Check with Ian about removing this.
					if (newDebug != debug)
					{
						if ((newDebug&DEBUGLOG) != (debug&DEBUGLOG))
							LogLine(" ",TRUE);
						debug = newDebug;
					}
*/
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
	DWORD cbData;
	HKEY hkey;

	REF(dwArgc);
	REF(lpszArgv);

	HRESULT hr = S_OK;

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (SUCCEEDED(hr))
	{
	    CSecurityDescriptor oSD;
	    oSD.InitializeFromProcessToken();

		hr = CoInitializeSecurity(
									oSD,
									-1,
									NULL,
									NULL,
									RPC_C_AUTHN_LEVEL_DEFAULT,
									RPC_C_IMP_LEVEL_IDENTIFY,
									NULL,
									EOAC_NO_CUSTOM_MARSHAL,
									NULL
									);

		// Don't register our class objects if CoInitializeSecurity fails.
		if (SUCCEEDED(hr))
		{
			// Not checking for RPC_E_TOO_LATE as this should be the first call
			// for this process.
            hr = _AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);

			if (FAILED(hr))
				dprintf("Failed to initialize RTVScan COM components.\n");

			hr = CoResumeClassObjects();
			
			if (FAILED(hr))
				dprintf("CoResumeClassObjects failure: %lu\n", hr);
		}
		else
			dprintf("CoinitializeSecurity failure: %lu\n", hr);
	}
	else
		dprintf("Failed to initialize COM in service_main.\n");

#ifdef REAL_SERVICE
	// RegisterServiceCtrlHandlerEx requires Windows XP, Windows 2000 Professional, Windows Server 2003 or Windows 2000 Server
	sshStatusHandle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, HandlerEx, NULL);
	// The service status handler (sshStatusHandle) does not need to be closed when we are done.
	
	if (!sshStatusHandle)
		goto dump;
#endif

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
#ifdef _USE_CCW
        pfnGetFileSecurityDescW = (PFNGetFileSecurityDescW) GetProcAddress( hNavNtUtl, GETFILESECURITYDESCRIPTORW );
        pfnSetFileSecurityDescW = (PFNSetFileSecurityDescW) GetProcAddress( hNavNtUtl, SETFILESECURITYDESCRIPTORW );
#endif
        pfnFreeFileSecurityDesc      =
                (PFNFreeFileSecurityDesc) GetProcAddress( hNavNtUtl, FREEFILESECURITYDESCRIPTOR );
        pfnCopyAlternateDataStreams2 =
                (PFNCopyAlternateDataStreams2) GetProcAddress( hNavNtUtl, COPYALTERNATEDATASTREAMS2 );
#ifdef _USE_CCW
        pfnCopyAlternateDataStreams2W =
            (PFNCopyAlternateDataStreams2W) GetProcAddress( hNavNtUtl, COPYALTERNATEDATASTREAMS2W );    
#endif
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
	if (OEMSettings.IsOEMBuild())
	{
		License_RegisterSubscription();
		// Catfish:
 		// Call SlicStartSubscription in rtvscan startup instead of config wizard
 		if (OEMSettings.IsOEM_TrialApp())
 			License_StartSubscription();

		if (!OEMSettings.IsOEM_TrialApp())
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
    dprintf("Entering main service wait.\n");
	do
	{
		cc = WaitForSingleObject( hServDoneEvent, 60*60*1000 );

		if( cc == WAIT_TIMEOUT )
		{
			// This used to check licensing information.  I've left the loop in
			// because it might come in handy in the future.
		}

	}
    while (cc == WAIT_TIMEOUT);
    dprintf("Exiting main service wait. cc = %lu\n", cc);

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
    if ( hServDoneEvent != NULL )
    {
        CloseHandle( hServDoneEvent );
        hServDoneEvent = NULL;
    }
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

	CoUninitialize();

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

	dprintf("STOP\n",TRUE); // TODO:  Test.

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
// RestartService()
//
// This routine will stop and restart the RTVScan service.
//
// Notes:
//  Currently implemented by launch of RtvStart.exe which watches for
//  service shutdown and restarts "Symantec AntiVirus" service.
//  Trust checking and CreateProcess() code pulled from vptray's StartScan() which 
//  launches DoScan.exe.
DWORD RestartService()
{
    static const TCHAR  RTVSTART_PARAMETERS[] = _T("-restart");

    DWORD   dwReturn = ERROR_GENERAL;

    // Load up the SAV install directory.
    TCHAR szSavDir[MAX_PATH];
    HKEY  hKey = NULL;
    if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,KEY_QUERY_VALUE,&hKey ) == ERROR_SUCCESS ) 
    {
        DWORD dwSize  = sizeof(szSavDir);
        LONG lResult = SymSaferRegQueryStringValue( hKey, szReg_Val_HomeDir, szSavDir, &dwSize );
        RegCloseKey(hKey);
        if (lResult != ERROR_SUCCESS)
            return ERROR_REG_FAIL;
    }

    // Construct a non-quoted full path to RtvStart.exe. IsTrustedFile() cannot deal with
    // quoted paths.
    TCHAR szRtvStartCmdLine[MAX_PATH + sizeof(RTVSTART_PARAMETERS)/sizeof(TCHAR)];

    ssStrnCpy       ( szRtvStartCmdLine, szSavDir,           sizeof(szRtvStartCmdLine) );
    ssStrnAppendFile( szRtvStartCmdLine, _T("RtvStart.exe"), sizeof(szRtvStartCmdLine) );
    
    // Ensure that the file has a Symantec digital signature before launching it.
    if ( IsTrustedFile( szRtvStartCmdLine ) == false)
        return ERROR_NOT_AUTHORIZED;

    // Construct a quoted full path to RtvStart.exe. The quoting ensures that CreateProcess()
    // only launches our exe and not something else.
    szRtvStartCmdLine[0] = _T('\"');
    szRtvStartCmdLine[1] = _T('\0');
    ssStrnAppend    ( szRtvStartCmdLine, szSavDir,              sizeof (szRtvStartCmdLine) );
    ssStrnAppendFile( szRtvStartCmdLine, _T("RtvStart.exe\" "), sizeof (szRtvStartCmdLine) );
    ssStrnAppend    ( szRtvStartCmdLine, RTVSTART_PARAMETERS,   sizeof (szRtvStartCmdLine) );

    STARTUPINFO         stStartupInfo = {0};
    PROCESS_INFORMATION stProcessInfo = {0};

    stStartupInfo.cb = sizeof(stStartupInfo);

    // Launch RTVStart.exe and, if successful, begin service shutdown.
    if ( CreateProcess( NULL,
                        szRtvStartCmdLine,
                        NULL, NULL, FALSE,
                        NORMAL_PRIORITY_CLASS, NULL, NULL,
                        &stStartupInfo, &stProcessInfo) )
    {
        CloseHandle( stProcessInfo.hThread );
        CloseHandle( stProcessInfo.hProcess );

        // Looks good for RTVStart.exe, now signal ourself to shutdown.
        if ( SetEvent( hServDoneEvent ) )
        {
            dwReturn = ERROR_SUCCESS;
        }
    }

    return ( dwReturn );
}


/*******************************************************************/
// Determine if the specified file is trusted. Copy and adapted from
// ccSymInterfaceLoader.h, CSymInterfaceTrustedLoader class.
static bool IsTrustedFile (const TCHAR *szPath)
{
    if (szPath == NULL)
        return false;

    ccVerifyTrust::ccVerifyTrust_IVerifyTrust objVerifyTrustLoader;
    ccVerifyTrust::IVerifyTrustPtr            ptrVerifyTrust;

    // Load and create the IVerifyTrust interface
    // If we can not create the trust validation object we will let the load continue
    if (SYM_SUCCEEDED (objVerifyTrustLoader.CreateObject(ptrVerifyTrust.m_p)) && ptrVerifyTrust != NULL)
    {
        if (ptrVerifyTrust->Create(true /*bLoadTrustPolicy*/) != ccVerifyTrust::eNoError)
            ptrVerifyTrust.Release();
    }

    // Make sure file exists.
    if (::GetFileAttributes (szPath) == INVALID_FILE_ATTRIBUTES)
        return false;

    // Test for valid symantec digital signature.
    if (ptrVerifyTrust != NULL)
    {
        // Only fail in case of eVerifyError
        // We let all other errors continue because eVerifyError is the only guarantee that 
        // the validation did indeed fail and it was not a subsystem or other error
        if (ptrVerifyTrust->VerifyFile (szPath, ccVerifyTrust::eSymantecSignature) == ccVerifyTrust::eVerifyError)
            return false;
    }
    return true;
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
void Real_vdprintf(DEBUGFLAGTYPE dwTag, LPCSTR format, const va_list &marker)
{
	// check skipping untagged lines

    if (dwTag == 0 && (debug & DEBUGEXCLUDEUNTAGGED) )
		return;

	if (debug&DEBUGPRINT)
    {
        // There is at least one case where our static objects have be
        // destroyed, but this routine is still called. This occurs when the
        // cscan.cpp:ClientScanner() thread attempts to output a debug
        // message. For that case, we simply will ignore synchronization on
        // dprintf during the later stages of RtvScan's shutdown.
        if (s_bStaticObjectsDestroying == false)
            s_DprintfSentry.Enter ();

		Real_vdprintfCommon(dwTag, format, marker);

        if (s_bStaticObjectsDestroying == false)
            s_DprintfSentry.Leave ();
    }
}

//============================================================================
void Real_dprintfTagged( DEBUGFLAGTYPE dwTag, LPCSTR format, ... )
{
    va_list marker;

    va_start(marker, format);
    Real_vdprintfCommon( dwTag, format, marker );
    va_end(marker);
}

//============================================================================
void Real_dprintf( LPCSTR format, ... )
{
    va_list marker;

    va_start(marker, format);
    Real_vdprintfCommon( 0, format, marker );
    va_end(marker);
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
