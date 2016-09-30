// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
//	VPTray.cpp
//
//	This module is used to create a bunch of monitor threads for the tray
//
#include <windows.h>
#include <stdio.h>
#include "ClientReg.h"
#include "ClientCCSettingNames.h"
#include "resource.h"
#include <tchar.h>
#include <assert.h>
#include "user.h"
#include "OSUtils.h"
#include "vphtmlhelp.h"
#include "vpstrutils.h"
#include "ScanShared.h"
#include "ccLibDllLink.h"

#ifndef array_sizeof
#define array_sizeof(x) (sizeof(x)/sizeof((x)[0]))
#endif

// licensing related includes
#include "licensehelper.h"
#include "slicwrap.h"
#include "SLICLicenseData.h"
#include "SLICWrap_i.c"

#include <atlbase.h>
#include "SymSaferRegistry.h"
#include "SymSaferStrings.h"
#include "ControlAP.h"

#include "ccTrace.h"
#include "ccSymDebugOutput.h"

#define VPTRAY_EXE_NAME  "VPTray"

ccSym::CDebugOutput g_DebugOutput(_T(VPTRAY_EXE_NAME));
IMPLEMENT_CCTRACE(g_DebugOutput);

#define INITIIDS
#include "ccVerifyTrustLoader.h"
#include "ccThread.h"
#include "ccAlertLoader.h"
#include "ccSettingsLoader.h"
#include "ccSubscriberEx.h"
#include "ccEventFactoryEx.h"
#include "ccEventManagerHelper.h"
#include "ccSettingsChangeEventEx.h"
#include "SrtControlInterface.h"
#undef INITIIDS

#include "oem_obj.h"
#include "EventsSubscriber.h"
#include "savrtmoduleinterface.h"
#include "ScanCouldntShowNotify.h"

int GetLicenseNotifyTimeframe();
typedef BOOL (WINAPI *PFnCreateProcessAsUserA)(HANDLE, LPCTSTR, LPTSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFO, LPPROCESS_INFORMATION);
typedef ULONG (*PFNProcessIsInUserContext)(ULONG * lpbUserContext);

BOOL CALLBACK MissingPatDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);


// MMENDON 06-16-2000 VPTray will now communicate directly with AP through APComm
// MMENDON 06-16-2000 End change

IMP_VPCOMMON_IIDS

// status vars from main window
extern HMENU g_hMenu; 
extern HWND  g_hMain;
extern HINSTANCE g_hResource;
extern HINSTANCE g_hInstance;
extern DWORD g_IconState;
extern BOOL  g_bRunning;
extern HANDLE g_hEvent;
extern BOOL	g_bShellResult;
extern char	g_szLang[4];
extern BOOL g_bShowTray;
extern BOOL g_bLaunchedByInstall;
extern BOOL g_bUserCanStopRTS;
BOOL g_bProcessIsUserContext = FALSE;
BOOL g_bProcessContextVerified = FALSE;
extern HMODULE g_hAdvapi;

extern void MapIcon(int iIcon,UINT action=NIM_MODIFY);
extern void ShowIcon(BOOL bShow);
//extern int WaitForObjectsWithMsgPump( HANDLE *lphObjects, int iNumObjs, DWORD dwTimeout);

DWORD WINAPI Monitor(LPVOID);	  // monitors RTS On/Off and Service active

static void CleanRemoteDir();		// clean up remote install dir
static void DeleteDir(LPCSTR sName);

static void DisplayLicenseAlert();
static void DisplayUnshownScanAlert();

// VTIME convertions
BOOL SystemTimeToVTime(LPSYSTEMTIME psTime, VTIME *pvTime);
FILETIME VTimeToFileTime(VTIME* pvTime);



HANDLE			 g_threads[2];
int              g_nNumThreads = sizeof(g_threads)/sizeof(g_threads[0]);
IServiceControl *g_pService=NULL;

HWND			 g_hNotifyPat=NULL;
HWND			 g_hNotifyMissingPats=NULL;

DWORD			 g_dwDaysOld=90;
DWORD			 g_dwDaysLeft=0;
BOOL			 g_bServer=FALSE;

static CEventsSubscriber* g_pEventsSubscriber;

const int ICON_DISABLED=2;
const int ICON_OFF=1;
const int ICON_ON=0;
const int AUTO_PROTECT_ON = 1;
const int AUTO_PROTECT_OFF = 0;

#define szRTS_LOCK  szReg_Val_RTSScanOnOff "-L"
#define szGeneralAdmin szReg_Key_AdminOnly "\\" szReg_Key_General

#define MAX_MISSING_PATTERN_MSG_LENGTH 1024

#define MAX_WAIT_FOR_CONTROLAP_MSEC  1000


//IBARILE 1/14/03 port CRT fix Siebel defect #1-O5GS2
static bool HasLoggedOff()
{
	TCHAR szUserName[1024];
    szUserName[0] = _T('\0');

    TCHAR szRegValueName[1024];
    vpstrncpy (szRegValueName, szReg_Logged_Off, sizeof (szRegValueName));

    DWORD dNameSize = array_sizeof (szUserName);
	if (GetUserName(szUserName, &dNameSize))
        vpstrnappend (szRegValueName, szUserName, sizeof (szRegValueName));

	// Defect 1-O5GS2 --- only run StartupScan if we detected that there
    // wasn't a successful log off before vptray.exe restarted.
	HKEY hKey = NULL;

	// check and see if we are running on the server
	DWORD dwValue = 0;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS) 
	{
		DWORD dwSize = sizeof(DWORD);
		SymSaferRegQueryValueEx(hKey,szRegValueName,NULL,NULL,(LPBYTE)&dwValue,&dwSize);
		RegCloseKey(hKey);
	}
    return dwValue? true : false;
}

void SetLoggedOff(DWORD dLoggedOff, bool bALL = false)
{
	try
	{
		if (!bALL)
		{
			TCHAR szUserName[1024];
			TCHAR szRegValueName[1024];
            szUserName[0]     = _T('\0');
            szRegValueName[0] = _T('\0');

			DWORD dNameSize = array_sizeof (szUserName);
		    if (GetUserName(szUserName, &dNameSize))
            {
                vpstrncpy    (szRegValueName, szReg_Logged_Off, sizeof (szRegValueName));
                vpstrnappend (szRegValueName, szUserName,       sizeof (szRegValueName));
            }
		    else
                vpstrncpy(szRegValueName, szReg_Logged_Off, sizeof (szRegValueName));

			// Defect 1-O5GS2 --- only run StartupScan if we detected that there wasn't a successful log off
			// before vptray.exe restarted.
			HKEY hKey = NULL;
			DWORD dwValue = dLoggedOff;

			// check and see if we are sunning on the server
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,
				KEY_WRITE,&hKey)== ERROR_SUCCESS) 
			{
				DWORD dwSize = sizeof(DWORD);
				RegSetValueEx(	hKey,
									szRegValueName,
									NULL,
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
			DWORD dwValue = dLoggedOff;

			int dIndex = 0;
			// check and see if we are sunning on the server
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,
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
												NULL,
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
	catch (...)
	{

	}
	
}
//IBARILE 1/14/03 end CRT port

// Determine if the specified file is trusted. Copy and adapted from
// ccSymInterfaceLoader.h, CSymInterfaceTrustedLoader class.
static bool IsTrustedFile (const TCHAR *szPath)
{
	if (szPath == NULL)
		return false;

    ccVerifyTrust::ccVerifyTrust_IVerifyTrust objVerifyTrustLoader;
    ccVerifyTrust::CVerifyTrustPtr            ptrVerifyTrust;

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

/////////////////////////////////////////////////////////////////////////////
// Startup Scan Thread
DWORD WINAPI StartScans(void *) 
{
	IScanConfig *pConfig=NULL;

	// check to see if Startup Scans are enabled
	if (StartupScansEnabled() != TRUE)
		return 0;

	//IBARILE 1/14/03 port CRT fix Siebel defect #1-O5GS2
	// 1-O5GSZ --- Logged on session shouldn't produce startup scan
	if (HasLoggedOff())
	{
        // We need to do this just in case machine crashes and we don't get the
        // shutdown event to clear this flag.
		SetLoggedOff(0);
		return 0;
	}
	else
        SetLoggedOff(0);  // record of existing key.
	//IBARILE 1/14/03 end CRT port

    // Load up the SAV install directory.
    TCHAR szSavDir[MAX_PATH+1] = {0};
	HKEY  hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,KEY_QUERY_VALUE,&hKey) == ERROR_SUCCESS) 
	{
		DWORD dwSize  = sizeof(szSavDir) - sizeof (TCHAR) /*ensure NULL termination*/;
		LONG  lResult = SymSaferRegQueryValueEx(hKey,szReg_Val_HomeDir,NULL,NULL,(LPBYTE)szSavDir,&dwSize);
		RegCloseKey(hKey);
        if (lResult != ERROR_SUCCESS)
            return 0;
	}

    // Construct a non-quoted full path to DoScan.exe. IsTrustedFile() cannot deal with
    // quoted paths.
    TCHAR szDoScanCmdLine[MAX_PATH+1];
    vpstrncpy        (szDoScanCmdLine, szSavDir,         sizeof (szDoScanCmdLine));
    vpstrnappendfile (szDoScanCmdLine, _T("DoScan.exe"), sizeof (szDoScanCmdLine));
    
    // Ensure that the file has a Symantec digital signature before launching it.
    if (IsTrustedFile (szDoScanCmdLine) == false)
        return 0;

    // Construct a quoted full path to DoScan.exe. The quoting ensures that CreateProcess()
    // only launches our exe and not something else.
    szDoScanCmdLine[0] = _T('\"');
    szDoScanCmdLine[1] = _T('\0');
    vpstrnappend     (szDoScanCmdLine, szSavDir,                          sizeof (szDoScanCmdLine));
    vpstrnappendfile (szDoScanCmdLine, _T("DoScan.exe\" /startupscans"),  sizeof (szDoScanCmdLine));

    STARTUPINFO			stStartupInfo = {0};
    PROCESS_INFORMATION stProcessInfo = {0};

    stStartupInfo.cb = sizeof (stStartupInfo);

    // Actually launch the startup scan, but don't wait for it to finish.
    if (CreateProcess (NULL, szDoScanCmdLine, NULL, NULL, FALSE,
                       NORMAL_PRIORITY_CLASS, NULL, NULL, &stStartupInfo, &stProcessInfo))
    {
        CloseHandle (stProcessInfo.hThread);
        CloseHandle (stProcessInfo.hProcess);
    }

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//	Start all  theads of the VP Tray
//
static void StartThreads()
{
	DWORD ThreadId;

	g_threads[0] = CreateThread(NULL,NULL,StartScans,NULL,0,&ThreadId);
	g_threads[1] = CreateThread(NULL,NULL,Monitor,NULL,0,&ThreadId);
}


///////////////////////////////////////////////////////////////////////////////
//	Stops all theads of the VP Tray
//
static void StopThreads()
{
	g_bRunning = FALSE;
	if(g_hEvent)
        SetEvent(g_hEvent);

	WaitForMultipleObjects(g_nNumThreads, g_threads,TRUE,INFINITE);
	
	// close thread handles
	for (int i=0;i<g_nNumThreads;i++)
		CloseHandle(g_threads[i]);

	if(g_hEvent)
	    CloseHandle(g_hEvent);
}



////////////////////////////////////////////////////////////////////////////////
// Open the client application
//
void OnOpen()
{
	IScanConfig *pConfig;
	char* pPath=NULL;
	TCHAR ExePath[MAX_PATH];
	TCHAR CmdLine[MAX_PATH + 2];
	HRESULT hr;
	HWND hWnd;

	// see if the client is already running
	hWnd = FindWindow(NAV_WIN_CLASS_NAME,NULL);
	if (hWnd)
	{
        if(IsIconic(hWnd))
            SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        SetForegroundWindow(GetLastActivePopup(hWnd));
		return;
	}

	// the client is not already running so we will lanch it
	// create a new config object 
	hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_MAIN_ROOT, "", pConfig);
	if (hr==S_OK)
	{
		// get the home key value
		hr = pConfig->GetOption(szReg_Val_HomeDir,(BYTE**)&pPath,1024,(BYTE*)"");
		pConfig->Release();
	}
	// build the exepath
	ExePath[0]='\0';
	if (pPath)
		vpstrncpy (ExePath, pPath, sizeof (ExePath));
    
    vpstrnappendfile (ExePath, "vpc32.exe", sizeof (ExePath));

	// double quote command to prevent possible execution of unexpected program
	if (! _tcschr( ExePath, _T('\"')))
		sssnprintf( CmdLine, sizeof(CmdLine), "\"%s\"", ExePath );
	else
		_tcscpy( CmdLine, ExePath );

/////////Fix for defect 387749. Checks if vptray process is in user context if not//////////////
/////////launches vpc32.exe in the currently logged on users context//////////////////////////////////////////
	DWORD dwUserContext = FALSE;
	HINSTANCE   hNavNtUtl = NULL;
	PFNProcessIsInUserContext pfnProcessIsInUserContext = NULL;
	BOOL bCreateProcessSuccess = FALSE;

	if(g_bProcessContextVerified == FALSE)
	{
		g_bProcessContextVerified = TRUE;

		char szNavNTUTLPath[IMAX_PATH] = "\0";

		// Get the home directory
		HKEY hkey;
		if (RegOpenKey(HKEY_LOCAL_MACHINE,szReg_Key_Main,&hkey) == ERROR_SUCCESS) {
			DWORD size = sizeof(szNavNTUTLPath);
			SymSaferRegQueryValueEx(hkey,szReg_Val_HomeDir,0,NULL,(PBYTE)szNavNTUTLPath,&size);
			RegCloseKey(hkey);
		}

		vpstrnappendfile (szNavNTUTLPath, _T("NAVNTUTL.DLL"), sizeof (szNavNTUTLPath));

		//load the navntutl.dll
		hNavNtUtl = LoadLibrary( szNavNTUTLPath );
		if(hNavNtUtl)
			pfnProcessIsInUserContext = (PFNProcessIsInUserContext) GetProcAddress( hNavNtUtl, _T("ProcessIsInUserContext") );
		if( (pfnProcessIsInUserContext) && (pfnProcessIsInUserContext( &dwUserContext ) == ERROR_SUCCESS) )
		{ 	
			g_bProcessIsUserContext = dwUserContext;

		}
		else //lets assume it is in user context and do what we normally do
			g_bProcessIsUserContext = TRUE;
		
		if (hNavNtUtl)
			FreeLibrary( hNavNtUtl );
	}

	if(g_bProcessIsUserContext == FALSE)
	{
		HANDLE hAccessToken = NULL;
		// Get the access token for the user who is logged on right now
		hAccessToken = GetAccessTokenForLoggedOnUser();

		if(g_hAdvapi == NULL)
		{
			TCHAR szPath[MAX_PATH];
			ZeroMemory(szPath, sizeof(szPath));
			::GetSystemDirectory( szPath, MAX_PATH );
			_tcscat( szPath, _T("\\ADVAPI32.DLL"));
			g_hAdvapi = LoadLibrary(szPath);
		}

		if (hAccessToken)
		{

			if (g_hAdvapi)
			{
				PFnCreateProcessAsUserA pfnCreateProcessAsUserA = NULL;

				pfnCreateProcessAsUserA = (PFnCreateProcessAsUserA)
					GetProcAddress(g_hAdvapi, _T("CreateProcessAsUserA"));

				if (pfnCreateProcessAsUserA)
				{
					STARTUPINFO			si;
					PROCESS_INFORMATION pi; 

					memset( &si, 0, sizeof( STARTUPINFO ) );
					si.cb = sizeof( STARTUPINFO );

					// Use the access token to start vpc32 as the logged on user
					if ( pfnCreateProcessAsUserA(
							hAccessToken,				// handle to user token
    						NULL,						// name of executable module
	    					CmdLine,					// command-line string
		    				NULL,						// SD
			    			NULL,						// SD
				    		FALSE,						// inheritance option
							NULL,						// creation flags
							NULL,						// new environment block
							NULL,						// current directory name
							&si,						// startup information
							&pi) )						// process information
					{
						bCreateProcessSuccess = TRUE;
					}
					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);
				}
			}
		
			CloseHandle(hAccessToken);
			hAccessToken = NULL;
		}
	} 
	
	if (g_bProcessIsUserContext == TRUE || bCreateProcessSuccess == FALSE)
    {

        //Set vptray's invisible window as the foreground window.  Doing so allows Vista's
        //UAC elevation prompt to interrupt the user instead of just flashing on the taskbar
        //when we call ShellExecute.
        SetForegroundWindow(g_hMain);

        // shell execute vpc32
        ShellExecute(g_hMain,"open",CmdLine,NULL,pPath,SW_SHOWNORMAL); 
	}

	if (pPath != NULL)
        CoTaskMemFree(pPath);

////////////////////////////////////////////////////////////////////////////////

}


//////////////////////////////////////////////////////////////////////////////////
//	Toggle the real time key	
//
void OnRealTime()
{
	// Get the target state.
	DWORD flag;
	DWORD uState;
	HMENU hMenu = g_hMenu;

	uState = GetMenuState(hMenu, ID_DF_REALTIME, MF_BYCOMMAND);

	if (uState&MF_CHECKED)
		flag=0;
	else
		flag=1;

	// Run ControlAP to toggle AP.
	// Load the path to the SAV dir -- we don't want to run just any ControlAP.exe!
	std::string strControlAPPath;
	HKEY		hMainKey = NULL;
	DWORD		dwRet;

	dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						 szReg_Key_Main,
						 0,
						 KEY_READ,
						 &hMainKey);
	if (dwRet != ERROR_SUCCESS)
	{
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_TITLE_TOGGLINGAP);
			strError.LoadString(IDS_ERROR_TOGGLINGAP);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	}
	TCHAR szSAVPath[MAX_PATH + 1] = {0};
	DWORD dwSize = sizeof(szSAVPath);

	dwRet = SymSaferRegQueryStringValue(hMainKey,
										szReg_Val_LocalAppDir,
										szSAVPath,
										&dwSize);
	RegCloseKey(hMainKey);
	hMainKey = NULL;
	if (dwRet != ERROR_SUCCESS)
	{
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_TITLE_TOGGLINGAP);
			strError.LoadString(IDS_ERROR_TOGGLINGAP);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	}
	// Create the executable name.
	try
	{
		strControlAPPath = szSAVPath;
		if(*(strControlAPPath.rbegin()) != _T('\\'))	// if last char is not delim
			strControlAPPath.append(1, _T('\\'));		// append delim
		strControlAPPath += _T(CONTROLAP_EXE_NAME);
	}
	VP_CATCH_MEMORYEXCEPTIONS
	(
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_TITLE_TOGGLINGAP);
			strError.LoadString(IDS_ERROR_TOGGLINGAP);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	)
	// Now create the ControlAP process.
	SHELLEXECUTEINFO sExecInfo;

	ZeroMemory(&sExecInfo, sizeof(sExecInfo));
	sExecInfo.cbSize = sizeof(sExecInfo);
	sExecInfo.fMask  = SEE_MASK_NOCLOSEPROCESS; // ask to have sExecInfo.hProcess filled in
	sExecInfo.lpVerb = _T("open"); // run the specified process
	sExecInfo.lpFile = _T(CONTROLAP_EXE_NAME);//strControlAPPath.c_str();
	sExecInfo.lpParameters = flag ? CONTROLAP_PARAM_ENABLE_AP
								  : CONTROLAP_PARAM_DISABLE_AP;
	sExecInfo.lpDirectory  = szSAVPath;
	sExecInfo.nShow		   = SW_HIDE;
    sExecInfo.hwnd         = g_hMain;

    //Set vptray's invisible window as the foreground window.  Doing so allows Vista's
    //UAC elevation prompt to interrupt the user instead of just flashing on the taskbar
    //when we call ShellExecuteEx.
    SetForegroundWindow(g_hMain);

	if (!ShellExecuteEx(&sExecInfo))
	{
		DWORD	dwError = GetLastError();
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_TITLE_TOGGLINGAP);
			strError.LoadString(IDS_ERROR_TOGGLINGAP);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	}
	// Wait for a return value from ControlAP.
	DWORD dwExitCode	= 0;
	bool  bControlAPRan = true;

	SAVASSERT(sExecInfo.hProcess != NULL);
	if (WaitForSingleObject(sExecInfo.hProcess, MAX_WAIT_FOR_CONTROLAP_MSEC) != WAIT_OBJECT_0 ||
		!GetExitCodeProcess(sExecInfo.hProcess, &dwExitCode))
	{
		// We couldn't get ControlAP's return value.
		// It might have worked, so just exit silently.
		bControlAPRan = false;
	}
	CloseHandle(sExecInfo.hProcess);
	if (!bControlAPRan)
		return;
	if (dwExitCode != 0)
	{
		CString strTitle;
		CString strError;

		try
		{
			strTitle.LoadString(IDS_TITLE_TOGGLINGAP);
			strError.LoadString(IDS_ERROR_TOGGLINGAP);
			MessageBox(NULL, strError, strTitle, MB_OK);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		return;
	}
	// Toggle the AP state that we are showing.  Monitor() will verify this in time.
	if (g_IconState != ICON_DISABLED)
		MapIcon(!flag);
	if (flag)
		CheckMenuItem(hMenu, ID_DF_REALTIME, MF_BYCOMMAND | MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_DF_REALTIME, MF_BYCOMMAND | MF_UNCHECKED);
}


//////////////////////////////////////////////////////////////////////////////
//	Monitors RealTime Scan Keys for Icon Status
// 
DWORD WINAPI Monitor(LPVOID) 
{
	BOOL bFirstLoop = TRUE;
	int dLicenseAlertCycles = 1, dLicenseNotifyTimeframe;
	BOOL bInitialInstallAP = TRUE, bAPOffInFactory = FALSE;
	BOOL bEnableAPNow = TRUE;

	// Defs can become corrupt while rtvscan is running and thus while vptray is running, so 
	// we display the dialog from within this method. We only want to inform the user once per session
	// of vptray.exe since it will become annoying.
	BOOL fCorruptDefsMsgAlreadyDisplayed = FALSE;

	IScanConfig* pConfig = NULL, 
			   *pAdmin = NULL;

	DWORD dwOnOff=0,
		  dwLock=0,
		  id=ID_DF_REALTIME;
	
    // tray icon menu
	HMENU  hMenu = g_hMenu;

	CoInitialize(NULL);

	HRESULT hr=S_OK;

	// create a IConfig to get AdiminOnly status for determining if tray icon is visible
	hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_MAIN_ROOT, szReg_Key_AdminOnly, pAdmin);
	if (hr==S_OK) {
		hr = pAdmin->SetSubOption(szReg_Key_General);
	}

	HRESULT loop_hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_STORAGE_ROOT, szReg_Key_Storage_File, pConfig);
	if (loop_hr == S_OK)
	{
		hr = pConfig->SetSubOption(szReg_Key_Storage_RealTime);
	}
	else {
	    	dwLock = 0;
	}


	// Avoid calling multiple times and calling out-of-proc Rtvscan
	BOOL bIsOEMBuild = OEMObj.IsOEMBuild();
	BOOL bIsOEMCustomerBoot = OEMObj.IsOEM_CustomerBoot();
	BOOL bIsOEMNoTriggerEULA = OEMObj.IsOEM_NoTriggerEULA();
	BOOL bIsOEMDoEnableAP = OEMObj.IsOEM_DoEnableAP();
	BOOL bIsOEMShowVPTray = OEMObj.IsOEM_ShowVPTray();
	BOOL bIsOEMTrialApp = OEMObj.IsOEM_TrialApp();
	BOOL bIsOEMInitialized = OEMObj.IsOEM_Initialized();

	if (bIsOEMBuild)
		dLicenseNotifyTimeframe = GetLicenseNotifyTimeframe();

	
	// while tray icon threads are running
	while (g_bRunning)
	{
	    Relinquish(); // drain aparentment message queue 
		
		if (bIsOEMBuild)
		{
			if (bFirstLoop)
			{
				bFirstLoop = FALSE;

				if (!g_bLaunchedByInstall && bIsOEMCustomerBoot)
				{
					// only check bootcount here because here it launches EULA wizard at reboot
					if (OEMObj.CheckOEM_BootCount())
					{
						if (!bIsOEMNoTriggerEULA)
							OEMObj.LaunchConfigWiz(g_hMain);
					}
					else
						OEMObj.DecrementOEM_BootCount();
				}		
				
			} // first loop

			if ((!bIsOEMCustomerBoot) && (!bIsOEMDoEnableAP))
			{
				MapIcon(ICON_OFF);
				bAPOffInFactory = TRUE;
			}
			else
			{
				// GLEE 5/31/05: Fix of 1-4549AT (AutoProtect behaves unexpectedly)
				// Need to set this variable to FALSE so that Monitor() will continue
				// to update icon status at every loop iteration. Otherwise the icon
				// won't get updated right away.
				bAPOffInFactory = FALSE;
			}
		} // OEM Build
		
		if (bAPOffInFactory == FALSE)
		{
        // if the icon isn't loaded, try to load it
		if (g_bShellResult==FALSE)
			MapIcon(g_IconState,NIM_ADD);

		// check reatime status
		// MMENDON 06-16-2000 - now checking AP status using APComm.
        //                      Communicating directly with AP, instead of reg read.
		BOOL dwAPStatus = 0;
		SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
		if( pSAVRT.Get() != NULL )
		{
			if( SUCCEEDED( pSAVRT->GetEnabledState(&dwAPStatus) ) )
			{
				if(dwAPStatus)
					dwAPStatus = AUTO_PROTECT_ON;
			}
		}
		dwOnOff = dwAPStatus;
		
		// double check the running service
		hr = g_pService->GetStatus();

		// if icon shows rtvscan loaded and the icon is opposite AP's
        // current state, change the icon state.  
        // Note:  0 == AP disabled, 0 == AP Icon enabled, logically twisted!
		if (g_IconState != ICON_ON && dwOnOff != AUTO_PROTECT_OFF)
		{
			MapIcon(ICON_ON);
		}
		
        // if service error, and icon doesn't show rtvscan off, set it to off
		else if ( g_IconState != ICON_DISABLED && hr!= S_OK && dwOnOff == AUTO_PROTECT_OFF )
		{
	    	MapIcon(ICON_DISABLED);
		}
        // else if service is running and icon shows rtvscan off, set icon to AP disabled state
		//ap isn't running set the warning icon
	    else if (g_IconState != ICON_OFF && hr==S_OK && dwOnOff == AUTO_PROTECT_OFF )
		{
			MapIcon(ICON_OFF);
		}
		} // !bAPOffInFactory


		// Check to see if we have valid defs on this machine. 
		// We first check to see if the Admin wants us to display a message and then check to see if it's time.
		// Note that this overrides AP since corrupt/missing defs is the cause of AP being OFF.
		BOOL fValidDefs = TRUE;
		if (hr==S_OK)
			{
			// The default value for the "show dlg" reg value is to "show it", or '1'. 
			// If it isn't there, then we default to showing the msg.
			DWORD dwShowBadDefMsg = 1;
			(void)pAdmin->GetIntOption(szReg_Val_DisplayMissingDefMessage,(long*)&dwShowBadDefMsg,(long)1);
			if (dwShowBadDefMsg)
				{
				DWORD	dwCurAttempt = 0;
				DWORD	dwMaxAttempt = DEFAULT_MAX_REMEDIATION_ATTEMPTS;
				HRESULT hr_Cur = pAdmin->GetIntOption(szReg_Val_PattRemediationCurAttempts,(long*)&dwCurAttempt,(long)0);
				HRESULT hr_Max = pAdmin->GetIntOption(szReg_Val_PattRemediationMaxAttempts,(long*)&dwMaxAttempt,(long)DEFAULT_MAX_REMEDIATION_ATTEMPTS);
				// A missing "Cur" value means that we haven't tried to remediate yet and thus we're OK.
				if (SUCCEEDED(hr_Cur))
					{
					// NOTE (brian_schlatter): We still want to add some code that considers the defs to be
					// valid as those that have a non-zero "version" in the registry. However, we would 
					// need to make some changes in rtvscan that sets the "version" to '0' when we find that 
					// we've attempted remediation too many times. I don't think we should do this in Clockwork 
					// timeframe. 
					//
					// In other words, grab the szReg_Val_PatDate value and if it is non-zero, then the defs are ok.
					// We notify the user only when we've exceeded our number of tries today and the version is '0'.
					// The reason we need this is that if the user remediates themselves later (by connecting to the 
					// internet) then we don't keep that error icon on vptray until the next day.

					// A missing value in the registry is NOT an error; we just use the default value.
					BOOL fTooManyAttempts = dwCurAttempt > (FAILED(hr_Max) ? DEFAULT_MAX_REMEDIATION_ATTEMPTS : dwMaxAttempt);
					if (fTooManyAttempts)
						fValidDefs = FALSE;
					}
				}
			}

		// Corrupt/Missing defs override all else.
		if (!fValidDefs && g_IconState != ICON_DISABLED)
			MapIcon(ICON_DISABLED);

		// check locks
		UINT eFlags = MF_BYCOMMAND;

        // create an IConfig to get Realtime status
		// hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_STORAGE_ROOT, szStorage_File, pConfig);
	    if (loop_hr == S_OK)
	    {
			if (pConfig) 
			{
 	        	hr = pConfig->GetIntOption(szRTS_LOCK ,(long*)&dwLock,0);
	    	}
		}

		if (bIsOEMBuild)
			bEnableAPNow = (bIsOEMCustomerBoot); /* && CheckOEM_BootCount() ) || ((!IsOEM_CustomerBoot()) && (IsOEM_DoEnableAP())); */

		if (bEnableAPNow)
		{
			eFlags |= MF_ENABLED;
		}
		else
		{
			eFlags |= MF_DISABLED | MF_GRAYED;
		}
		EnableMenuItem(hMenu, ID_DF_OPEN, eFlags);
		
		eFlags = MF_BYCOMMAND; // reset the flags to default
		if (!bEnableAPNow)
		{
			eFlags |= MF_DISABLED|MF_GRAYED; // grayed out if not first customer boot
		}
		else if ( g_bServer )
	    {
			// running on the server. disable rts
			eFlags |= MF_DISABLED|MF_GRAYED;	
	    }
	    else
	    {
            // Vista change: Vista users will always have the RTS menu available, since the restricted users will
            // get the elevation prompt regardless. RTS menu icon will only be greyed out for managed clients, and if
            // the option is locked by SAV Administrator.

            // However, for backwards compatibility we will keep the previous behaviour for XP clients as is.
            // If the user cannot write to the registry, disable
            if ( (!IsWindowsVista(true)) && (!g_bUserCanStopRTS) )
            {
				eFlags |= MF_DISABLED|MF_GRAYED;	
			}
			// if no rts lock or rts locked ==0 enable rts
			else if ( dwLock==0 )
			{
				eFlags |= MF_ENABLED;
			}
			else
			{
				eFlags |= MF_DISABLED|MF_GRAYED;	
			}
	    }

        // set the tray menu AP Enabled item check to the current state
        if (dwOnOff)
			CheckMenuItem(hMenu,id,MF_BYCOMMAND|MF_CHECKED);
	    else
			CheckMenuItem(hMenu,id,MF_BYCOMMAND|MF_UNCHECKED);
					
        EnableMenuItem(hMenu,id,eFlags);
			
		// check for show icon status if admin key is there
	    if (pAdmin)
	    {
			if (bIsOEMBuild && !bIsOEMInitialized)
			{
				bIsOEMInitialized = OEMObj.IsOEM_Initialized();

				if (bIsOEMShowVPTray)
				{
					if (bEnableAPNow)
						dwOnOff = 1;
					else dwOnOff = 0;

					ShowIcon(TRUE);
				}
				else 
					ShowIcon(FALSE);
			}
			else if( TRUE == g_bShowTray )
			{
				dwOnOff=1;
				hr = pAdmin->GetIntOption( szReg_Val_RegShowVPIcon,(long*)&dwOnOff,(long)1);
				if (dwOnOff)
					ShowIcon(TRUE);
				else
					ShowIcon(FALSE);
			}
	    }

		if (bIsOEMBuild)
		{
			if ((g_bLaunchedByInstall) && (!bIsOEMTrialApp))
			{
				if (bInitialInstallAP)
				{
					bInitialInstallAP = FALSE;
					// 1-2VKRFB --- defect where overinstall does not reset this registry value. In MSI, the registries don't change during overinstall if they are modified
					if (bIsOEMDoEnableAP)
					{
						DWORD uState = GetMenuState( hMenu,id,MF_BYCOMMAND);

						if (uState&MF_CHECKED)
						{

						}
						else
						{
							OnRealTime();
						}
					}
					else
					{
						BOOL dwAPStatus = 0;
						SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
						if( pSAVRT.Get() != NULL )
						{
							if( SUCCEEDED( pSAVRT->GetEnabledState(&dwAPStatus) ) )
							{
								if(dwAPStatus)
									dwAPStatus = AUTO_PROTECT_ON;
							}
						}

						DWORD uState = GetMenuState( hMenu,id,MF_BYCOMMAND);

						if (dwAPStatus == AUTO_PROTECT_ON)
						{
							CheckMenuItem(hMenu,id,MF_BYCOMMAND|MF_CHECKED);
							OnRealTime();		
						}
						else
						{
							
						}
					} // IsOEM_DoEnableAP()
				} // bInitialInstallAP
			} // g_bLaunchedByInstall
		
			if ((dLicenseAlertCycles % dLicenseNotifyTimeframe) == 0)
			{
				//
				// If initialized check License alerts
				//
				if (bIsOEMInitialized)
				{

					SLIC_STATUS slic_status = SLICSTATUS_OK;

					if( License_IsLicenseEnforcingInstall() && License_AlertOnLogin() )
					{
						CComPtr<ISLICLicenseRepositorySCS> slic;

						if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
						{
							HRESULT hr = S_OK;

							CComPtr<ISLICLicense> lic;

							// Check for a license.

							CComBSTR msg;

							hr = slic->CheckLicenseSCS( &slic_status, &msg, &lic.p );

							if( SUCCEEDED(hr) )
							{
								// If the call returned a license, check to see if it's valid.

								if( lic != NULL )
								{
									SSLICLicenseData lic_data;

									lic->GetLicenseData( &lic_data );
								}
							}

							// See if the license needs some alert work.

							slic->ShowAlertSCSOnceADay( slic_status, lic, NULL, 0 );
						}
					}
				}
				else // IsOEMInitialized
				{
					bIsOEMInitialized = OEMObj.IsOEM_Initialized();
				}
			}

			dLicenseAlertCycles = (dLicenseAlertCycles++) % dLicenseNotifyTimeframe;
		} // IsOEMBuild

		// Defs can become corrupt while rtvscan is running and thus while vptray is running, so 
		// we display the dialog from within this method. We only want to inform the user once per session
		// of vptray.exe since it will become annoying.
		if (!fValidDefs && !fCorruptDefsMsgAlreadyDisplayed)
			{
			fCorruptDefsMsgAlreadyDisplayed = TRUE;	
			// This should have been created before we started these threads. 
			SAVASSERT(g_hNotifyMissingPats);
			// show the corrupt/missing pattern file dialog
			ShowWindow(g_hNotifyMissingPats,SW_SHOW);
			}

	    if(g_hEvent)
           	 WaitForSingleObject(g_hEvent,2500);
	}  // End of While
	if (pConfig) 
	{
		pConfig->Release();
	}
	
	if (pAdmin) 
		pAdmin->Release();


	Relinquish(); // always pump before exiting thread
	CoUninitialize();
	return 0;
}
 

///////////////////////////////////////////////////////////////////////////////////
//	Dialog Proc : Handles Dialogs for Missing PatternFile Notifications 
//
BOOL CALLBACK MissingPatDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_INITDIALOG:
		{
			TCHAR szWarnMsg[MAX_MISSING_PATTERN_MSG_LENGTH];
			char *pMessage;
			HRESULT hr;
			IScanConfig *pAdmin=NULL;

			// get the default message
			LoadString(g_hResource,IDS_MISSING_PATTERN,szWarnMsg,MAX_MISSING_PATTERN_MSG_LENGTH);

			// get the missing def message from the register
			hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_MAIN_ROOT, szReg_Key_AdminOnly, pAdmin);
			if (hr==S_OK)
			{
				hr = pAdmin->SetSubOption(szReg_Key_General);
				// read the nowarn message
				hr = pAdmin->GetOption(szReg_Val_MissingDefMessageToDisplay,(BYTE**)&pMessage,MAX_MISSING_PATTERN_MSG_LENGTH,(BYTE*)szWarnMsg);
				// copy the administrator message
				if (hr==S_OK)
				{
					vpstrncpy(szWarnMsg,pMessage, sizeof (szWarnMsg));
					CoTaskMemFree(pMessage);
				}
				pAdmin->Release();
			}

    		//get the message item
			HWND hText = GetDlgItem(hWnd,IDC_MISSING_DEF_MSG);
    
            // We need to set the font on this control to the default font so it will display DBCS characters.
            ::SendMessage(hText, WM_SETFONT, (WPARAM)::GetStockObject( DEFAULT_GUI_FONT ), MAKELPARAM(TRUE, 0));
			
			SetWindowText(hText,szWarnMsg);
			SetForegroundWindow(hWnd);
		}	
		break;
		case WM_COMMAND:
		{
			if (LOWORD(wParam)==IDNO)
				PostMessage(hWnd,WM_CLOSE,0,0);
		}
		break;
		case WM_CLOSE:
		{
			DestroyWindow(hWnd);
			g_hNotifyMissingPats=NULL;
		}
		break;
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////
//	Dialog Proc : Handles Dialogs for PatternFile Notifications 
//
BOOL CALLBACK PatDlgProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_INITDIALOG:
		{
			TCHAR outBuf[512],inBuf[512];
			char *pMessage;
			HRESULT hr;
			IScanConfig *pAdmin=NULL;


			// get the default message
			LoadString(g_hResource,IDS_OUTDATED_PATTERN,inBuf,array_sizeof (inBuf));

			// get the outdateed message from the register
			hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_MAIN_ROOT, szReg_Key_AdminOnly, pAdmin);
			if (hr==S_OK)
			{
				hr = pAdmin->SetSubOption(szReg_Key_General);

				// read the nowarn message
				hr = pAdmin->GetOption(szReg_Val_PatternWarningMessage,(BYTE**)&pMessage,512,(BYTE*)inBuf);

				// copy the administrator message
				if (hr==S_OK)
				{
					vpstrncpy(inBuf,pMessage, sizeof (inBuf));
					CoTaskMemFree(pMessage);
				}

				pAdmin->Release();
			}

    		//get the message item
			HWND hText = GetDlgItem(hWnd,IDC_MESSAGE);
    
            // We need to set the font on this control to the default font so it will display DBCS characters.
            ::SendMessage(hText, WM_SETFONT, (WPARAM)::GetStockObject( DEFAULT_GUI_FONT ), MAKELPARAM(TRUE, 0));
			
			// copy the number of days old
			vpsnprintf(outBuf, sizeof (outBuf), inBuf, g_dwDaysOld);
			SetWindowText(hText,outBuf);

            // If restricted user, want to hide checkbox because restricted
			// users cannot update the \\HKEY_LOCAL_MACHINE hive.
			//
            // Check if restricted user:
            //     See if the current user has the rights to update our hive in the
            //     registry. If so, he can turn off the def update reminder.
            HKEY    hKey = NULL;

            if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGHEADER, 0, KEY_ALL_ACCESS, &hKey) )
            {
                RegCloseKey( hKey );
            }
            else   //(is restricted user)
            {
                // if restricted user, hide def update reminder checkbox
                HWND hCheckBox = GetDlgItem(hWnd,IDC_DONT_REMIND);
                if(hCheckBox)
                    ShowWindow(hCheckBox, SW_HIDE);
            }

			SetForegroundWindow(hWnd);
		}	
		break;
		case WM_COMMAND:
		{
			if (LOWORD(wParam)==IDNO)
				PostMessage(hWnd,WM_CLOSE,0,0);
			else if (LOWORD(wParam)==IDHELP)
			{
				// launch help
				HtmlHelp(g_hMain, SAV_HTML_HELP, HH_HELP_CONTEXT, IDD_OUTDATED_PATTERN);
 			}	 
		}
		break;
		case WM_CLOSE:
		{
			HWND hButton = GetDlgItem(hWnd,IDC_DONT_REMIND);
			if (SendMessage(hButton, BM_GETCHECK, 0, 0)==BST_CHECKED)
			{
				IScanConfig *pConfig;
				HRESULT hr;
				// if they have check the no warn button then copy the pattern file
				// date into the nowarn field
				hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_MAIN_ROOT, "", pConfig);
				if (hr==S_OK)
				{
					VTIME  *pPatDate=NULL;

					hr = pConfig->GetOption(szReg_Val_PatDate,(BYTE**)&pPatDate,sizeof(VTIME),(BYTE*)"");
					if (hr==S_OK)
					{
						hr = pConfig->SetOption(szReg_Value_NoWarnPattern,REG_BINARY,(BYTE*)pPatDate,sizeof(VTIME));
						CoTaskMemFree(pPatDate);
					}
					pConfig->Release();
				}
			}
			DestroyWindow(hWnd);
			g_hNotifyPat=NULL;
		}
		break;
	}
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////////////
// Converts VTIME to a FILETIME
FILETIME VTimeToFileTime(VTIME* pvTime)
{
	SYSTEMTIME tTime;
	FILETIME fTime;
	tTime.wYear  = pvTime->year+1970;
	tTime.wDayOfWeek = 0;
	tTime.wMonth = pvTime->month+1;
	tTime.wDay   = pvTime->day;
	tTime.wHour  = pvTime->hour;
	tTime.wMinute = pvTime->min;
	tTime.wSecond = pvTime->sec;
	tTime.wMilliseconds = 0;
	BOOL b =SystemTimeToFileTime(&tTime,&fTime);
	return fTime;
}


//////////////////////////////////////////////////////////////////////////////////
// Converts a SystemTime to a vTime
BOOL SystemTimeToVTime(LPSYSTEMTIME psTime, VTIME *pvTime)
{
	pvTime->year = (BYTE)psTime->wYear;
	pvTime->month = (BYTE)psTime->wMonth;
	pvTime->day =   (BYTE)psTime->wDay;
	pvTime->hour = (BYTE)psTime->wHour;
	pvTime->min = (BYTE)psTime->wMinute;
	pvTime->sec = (BYTE)psTime->wSecond;
	return TRUE;
}




////////////////////////////////////////////////////////////////////////////////////
// Checks for out dated pattern file
//
void CheckPatternFile()
{
	IScanConfig *pConfig;
	HRESULT hr;

	DWORD  nWarn;

	_int64 longday=864000000000, 
			  tdelta=0;
	
	VTIME  *pPatDate=NULL,
		   *pNoWarn=NULL,
		   vtDefault;

	FILETIME fTime,warnTime,patTime;
	SYSTEMTIME tTime;

	ZeroMemory(&vtDefault,sizeof(vtDefault));
	vtDefault.day=1;

	// get the current local time as a FILETIME
	GetLocalTime(&tTime);
	SystemTimeToFileTime(&tTime,&fTime);



	hr = CreateScanConfig (CLSID_CliProxy, HKEY_VP_MAIN_ROOT, "", pConfig);
	if (hr==S_OK)
	{
		// read the nowarn again user dlg flags
		hr = pConfig->GetOption(szReg_Value_NoWarnPattern,(BYTE**)&pNoWarn,sizeof(VTIME),(BYTE*)&vtDefault);

		if (hr==S_OK)
			warnTime = VTimeToFileTime(pNoWarn);
		// read the pattern file date
		if (hr==S_OK)
			hr = pConfig->GetOption(szReg_Val_PatDate,(BYTE**)&pPatDate,sizeof(VTIME),(BYTE*)"");

		// When defs do not exist, this reg key will be zero; no need to do any processing.
		if ((hr != S_OK) || (pPatDate == NULL) || ((DWORD)pPatDate->year == 0))
			goto _CleanUp;

   		// calucate time difference (only if both good)
		if (hr==S_OK)
		{
			patTime = VTimeToFileTime(pPatDate);
			tdelta = *(_int64*)&fTime -  *(_int64*)&patTime;
		}

		// now we want to get new pattern file life and admin warn flag from admin key
		//hr =pConfig->SetSubOption(szReg_Key_AdminOnly);
		//hr =pConfig->SetSubOption(szReg_Key_General);
		hr=pConfig->SetSubOption(szGeneralAdmin);

		hr = pConfig->GetIntOption(szReg_Val_WarnAfterDays,(long*)&g_dwDaysOld,90);
		longday *= g_dwDaysOld;

		// new we get the admin warn flag
		hr = pConfig->GetIntOption(szReg_Val_DisplayOutdatedMessage,(long*)&nWarn,1);

		BOOL b = CompareFileTime(&warnTime,&patTime);

		if (OEMObj.IsOEMBuild() && !OEMObj.IsOEM_Initialized())
		{
		}
		else if (pNoWarn && pPatDate && (tdelta > longday) && (*(char*)pPatDate) && nWarn && b)
		{
			// show the outdated pattern file dialog
			g_hNotifyPat=CreateDialog(g_hResource,
				MAKEINTRESOURCE(IDD_OUTDATED_PATTERN),NULL,(DLGPROC)PatDlgProc);
			ShowWindow(g_hNotifyPat,SW_SHOW);
		}

_CleanUp:

		// free values
		if (pNoWarn) CoTaskMemFree(pNoWarn);
		if (pPatDate) CoTaskMemFree(pPatDate);
		pConfig->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////////
// check to see if vpray is running on the server
void CheckServerMode()
{
	HKEY hKey = NULL;
	// check and see if we are sunning on the server
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,
		KEY_READ,&hKey)== ERROR_SUCCESS) 
	{
		DWORD dwSize = sizeof(DWORD);
		DWORD dwValue=0;
		SymSaferRegQueryValueEx(hKey,szReg_Val_Client_Type,NULL,NULL,(LPBYTE)&dwValue,&dwSize);
		if (dwValue==4) g_bServer=TRUE;
		dwValue=0;
		SymSaferRegQueryValueEx(hKey,"AllowMenuAccess",NULL,NULL,(LPBYTE)&dwValue,&dwSize);
		if (dwValue==1) g_bServer=FALSE;
		RegCloseKey(hKey);
	}
}


//////////////////////////////////////////////////////////////////////////////////
int GetLicenseNotifyTimeframe()
{
	DWORD dwValue = LICENSE_ALERT_CYCLES;
	HKEY hKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,szReg_Key_Main,NULL,
		KEY_READ,&hKey)== ERROR_SUCCESS) 
	{
		DWORD dwSize = sizeof(DWORD);
		SymSaferRegQueryValueEx(hKey,szReg_License_Notify_Timeframe,NULL,NULL,(LPBYTE)&dwValue,&dwSize);
		RegCloseKey(hKey);
	}

	return dwValue;
}
//////////////////////////////////////////////////////////////////////////////////
// On Start of the Tray
void OnStart()
{
	HRESULT hr;

	CheckServerMode();

	if( FALSE == g_bShowTray )
		ShowIcon(FALSE);
	else
		ShowIcon(TRUE);

	// Start the event subscriber and notify threads.
	g_pEventsSubscriber = new CEventsSubscriber();
	g_pEventsSubscriber->SetTrayWnd(g_hMain);
	g_pEventsSubscriber->Start();

	// grab an interface to IServiceControl
	hr = CoCreateLDVPObject(CLSID_CliProxy,IID_IServiceControl,(void**)&g_pService);
	if (hr==S_OK)
	{
		g_pService->RunUserScheduleScans(1);
		CheckPatternFile();

		// Go ahead and create our dialog here but only show it if we find missing defs.
		g_hNotifyMissingPats = CreateDialog(g_hResource,
				MAKEINTRESOURCE(IDD_MISSING_PATTERN),NULL,(DLGPROC)MissingPatDlgProc);

		StartThreads();
	}
	// Display alerts as necessary.
	DisplayLicenseAlert();
	DisplayUnshownScanAlert();
}


/** Displays a license alert, if necessary. */
static void DisplayLicenseAlert()
{
    // See if we should display a license alert, unless we've been laucnhed by the installer.
	BOOL bDisplayAlert = (!OEMObj.IsOEMBuild()) || 
		(OEMObj.IsOEMBuild() && OEMObj.IsOEM_Initialized());
    if( !g_bLaunchedByInstall && bDisplayAlert)
    {
        SLIC_STATUS slic_status = SLICSTATUS_OK;

        if( License_IsLicenseEnforcingInstall() && License_AlertOnLogin() )
        {
		    CComPtr<ISLICLicenseRepositorySCS> slic;

		    if( SUCCEEDED( slic.CoCreateInstance( CLSID_SLICLicenseRepository, NULL, CLSCTX_INPROC_SERVER ) ) )
		    {
			    HRESULT hr = S_OK;

			    CComPtr<ISLICLicense> lic;

			    // Check for a license.
			    CComBSTR msg;

			    hr = slic->CheckLicenseSCS( &slic_status, &msg, &lic.p );

			    if( SUCCEEDED(hr) )
			    {
				    // If the call returned a license, check to see if it's valid.
				    if( lic != NULL )
				    {
					    SSLICLicenseData lic_data;

					    lic->GetLicenseData( &lic_data );
				    }
			    }

				// See if the license needs some alert work.

				slic->ShowAlertSCSOnceADay( slic_status, lic, NULL, 0 );
            }
        }
    }
}


/** If a scan couldn't show a found risk, show an alert. */
static void DisplayUnshownScanAlert()
{
	// See if a ccSettingsvalue was set to indicate that a scan couldn't show a
	// risk that was found.
    cc::ccSet_ISettingsManager      ccSet;
    ccSettings::ISettingsManagerPtr pSettingsMgr;
    ccSettings::ISettingsPtr        ptrSettings;
	DWORD dwFlag = FALSE;
	bool  bGotCCSetting;

    bGotCCSetting = SYM_SUCCEEDED(ccSet.CreateObject(pSettingsMgr)) &&
		            SYM_SUCCEEDED(pSettingsMgr->GetSettings(szCCSet_Key_Scan_Data, &ptrSettings));
	if (bGotCCSetting)
    {
        // Load the value
        ptrSettings->GetDword(szCCSet_Val_PendingRiskNotification, dwFlag);
	}
	// If the key isn't set, exit.
	if (!dwFlag)
		return;
	// Show the alert.
	CScanCouldntShowNotify *pobjAlert = new CScanCouldntShowNotify();

	pobjAlert->Start(); // It will delete itself when the user clicks ok.
	// Reset the ccSettings flag.
	if (bGotCCSetting)
	{
		ptrSettings->PutDword(szCCSet_Val_PendingRiskNotification, FALSE);
		pSettingsMgr->PutSettings(ptrSettings);
	}
}



/////////////////////////////////////////////////////////////////////////////////
// On Stop of the Tray
void OnStop()
{
	g_pEventsSubscriber->Stop();

	if (g_pEventsSubscriber)
		delete g_pEventsSubscriber;

	if (g_hNotifyMissingPats) 
	{
		DestroyWindow(g_hNotifyMissingPats);
		g_hNotifyMissingPats = NULL;
	}
	StopThreads();
	if (g_pService)
        g_pService->Release();
}


static void CleanRemoteDir()
{
	TCHAR sPath[MAX_PATH];

	// get system dir
	sPath[0]='\0';
	GetSystemDirectory(sPath, array_sizeof (sPath));

	// get the system drive
	TCHAR *pos = _tcschr(sPath,':');
	if (pos)
        *pos='\0';

	// concat remotedir
	vpstrnappend (sPath,":\\$NAVCE$", sizeof (sPath));

	DeleteDir(sPath);
}


static void DeleteDir(LPCSTR sName)
{
	TCHAR sFind[MAX_PATH];
    vpstrncpy        (sFind, sName, sizeof (sFind));
    vpstrnappendfile (sFind, "*.*", sizeof (sFind));

	WIN32_FIND_DATA data;
	HANDLE          hFind = FindFirstFile(sFind,&data);

	if (hFind==INVALID_HANDLE_VALUE)
        return;
	
	BOOL bOk=TRUE;
	while (bOk)
	{
		// skip dots
		if (lstrcmp(data.cFileName,".")==0 || lstrcmp(data.cFileName,"..")==0 )
			goto findnext;

		// build full path to file
		vpstrncpy        (sFind, sName,          sizeof (sFind));
		vpstrnappendfile (sFind, data.cFileName, sizeof (sFind));

		if (data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			DeleteDir(sFind);
		else
			DeleteFile(sFind);
		
findnext:
		bOk=FindNextFile(hFind,&data);
	}

	FindClose(hFind);
	bOk = RemoveDirectory(sName);
}
