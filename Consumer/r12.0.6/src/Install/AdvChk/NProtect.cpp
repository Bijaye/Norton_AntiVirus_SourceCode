#include "stdafx.h"
#include "NProtect.h"
#include "cwstring.h"
#include "utils.h"
#include "reghelp.h"
#include "resource.h"
#include "..\advchkres\resource.h"

extern HINSTANCE ghInstance;		//AdvChk.cpp
extern CAtlBaseModule _Module;
extern TCHAR g_szTarget[MAX_PATH];	//AdvChk.cpp

#define APP_PATH		_T("\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\")
#define NU_INSTALL_KEY  _T("\\Software\\Symantec\\Norton Utilities\\7.0\\Install")
#define NPUEPROCAPPNAME         "UnErase Process"
#define NPUEPROCCLASSNAME       "NU9:UEProcClass"

#define IDM_SHUTDOWN_EXT        1       // must stay in sync with main
                                        // header file


//Checks to see if Nprotect is installed
BOOL IsNUInstalledWithNProtect(LPTSTR szFullPath, DWORD &dwSize)
{
	if(IsNUInstalled(szFullPath, dwSize))
	{
		PathAppend(szFullPath, _T("NProtect.exe"));
		if(-1 != GetFileAttributes(szFullPath))
		{
			return TRUE;
		}
	}

	return FALSE;
}


//Installs NProtect
DWORD Install_NProtect()
{

	//Look for old UEPROC stuff and Nuke it.
	if(!IsOSNT())
	{
		RegRecursiveDeleteKey(HKEY_LOCAL_MACHINE, _T("\\System\\CurrentControlSet\\Services\\VxD\\NPROTECT"));
		RegDeleteKey(HKEY_LOCAL_MACHINE, APP_PATH _T("UEPROC32.EXE"));
	}

///////////////////////////////////////////////////////////////////
//Start of copied (and very slightly modified) Installshield code
////////////////////////////////////////////////////////////////////

	//Variables needed for IS stuff
	HKEY hTmpKey = NULL;
	DWORD nType, nSize;
	TCHAR szBinString[MAX_PATH] = {0};
	TCHAR szTemp[MAX_PATH] = {0};
	TCHAR szNPRB_Display[MAX_PATH] = {0};
	TCHAR szValue[MAX_PATH] = {0};
	TCHAR szOldBin[MAX_PATH] = {0};
	TCHAR szAppPath[MAX_PATH] = {0};
	TCHAR szBuff[MAX_PATH] = {0};
	myTCHAR szKey;

	RegGetSymantecSharedDir(szTemp, MAX_PATH);
	GetShortPathName(szTemp, szTemp, MAX_PATH);
	GetModuleDirectory(g_szTarget);
	GetShortPathName(g_szTarget, g_szTarget, MAX_PATH);

	_stprintf(szAppPath, _T("%s;%s"), g_szTarget, szTemp);



	szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\shellex";

    RegDBCreateKeyEx( szKey, "" );


    szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\shellex\\PropertySheetHandlers\\{263913E0-07B9-106A-8674-00AA000D6F0D}";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, "", -1 );

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\Shell";


    RegDBCreateKeyEx( szKey, "" );


	// This setting will cause the Recycle Bin to launch UE32 instead of the standard
	// Recycle Bin. We only enable this if they install Norton Protection as well.
	RegDBSetKeyValueEx( szKey, "", REGDB_STRING, "UnEraseWizard", -1 );

    // Right Click section
	// This section creates the item on the right click menu of Recycle bin
	LoadString(_Module.GetResourceInstance(), SHELL_UNERASE, szTemp , MAX_PATH);
	LoadString(_Module.GetResourceInstance(), IDS_NPRB_DISPLAY, szNPRB_Display, MAX_PATH);

	szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\Shell\\UnEraseWizard";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, szTemp, -1 );

    szKey =  BSlash(szBuff, szKey , "Command");
    RegDBCreateKeyEx( szKey, "" );

    // Launch UE32 with the proper command line switch
	if(!IsOSNT())
   		RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget , "UE32.EXE /R"), -1 );
	else
		RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget , "UE32.EXE /A"), -1 );

	//  Add Extensions key so Empty Recycle Bin right-click menu item works
    RegDBCreateKeyEx( NU_INSTALL_KEY, "" );
	RegDBSetKeyValueEx( NU_INSTALL_KEY, "Extensions", REGDB_STRING, g_szTarget, -1 );

	// End Right Click Section
	//

    szKey = "\\SOFTWARE\\Symantec\\Norton Utilities\\Norton Protection";
    RegDBCreateKeyEx( szKey, "" );
    szBinString[0] = 25;
    RegDBSetKeyValueEx( szKey, "TrackerSize", REGDB_BINARY, szBinString, 1 );
    szBinString[0] = 0;
    szBinString[1] = 2;
    RegDBSetKeyValueEx( szKey, "Version", REGDB_BINARY, szBinString, 2 );

    szKey = "\\SOFTWARE\\Microsoft\\Windows\\Help";
    RegDBSetKeyValueEx( szKey, "Ue32.hlp", REGDB_STRING, g_szTarget, -1 );

    szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\UE32.EXE";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "Path", REGDB_STRING, szAppPath, -1 );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget , "UE32.EXE"), -1 );

    szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NPROTECT.EXE";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "Path", REGDB_STRING, szAppPath, -1 );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget, "NPROTECT.EXE"), -1 );

    szKey = "\\SOFTWARE\\Symantec\\Norton Utilities\\Norton UnErase";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "InstallDir", REGDB_STRING, g_szTarget, -1 );


    szKey = "\\SOFTWARE\\Classes\\CLSID\\{263913E0-07B9-106A-8674-00AA000D6F0D}";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, "Norton Protection", -1 );

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{263913E0-07B9-106A-8674-00AA000D6F0D}\\InProcServer32";
    RegDBCreateKeyEx( szKey, "" );

    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget, "USHELLEX.DLL"), -1 );
    RegDBSetKeyValueEx( szKey, "ThreadingModel", REGDB_STRING, "Apartment", -1 );

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{11dde060-38d8-11d1-89e8-00600811c705}";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, "Norton Recycle Bin File Finder", -1 );

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{11dde060-38d8-11d1-89e8-00600811c705}\\InprocServer32";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget , "USHELLEX.DLL"), -1 );
    RegDBSetKeyValueEx( szKey, "ThreadingModel", REGDB_STRING, "Apartment", -1 );

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{11dde061-38d8-11d1-89e8-00600811c705}";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, "Norton Protection File Finder", -1 );

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{11dde061-38d8-11d1-89e8-00600811c705}\\InprocServer32";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget , "USHELLEX.DLL"), -1 );
    RegDBSetKeyValueEx( szKey, "ThreadingModel", REGDB_STRING, "Apartment", -1 );

    RegDBSetDefaultRoot( HKEY_CLASSES_ROOT );

    szKey = "\\CLSID\\{52636240-ECF7-11cd-99BE-0020AF346D41}\\InProcServer32";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, BSlash(szBuff, g_szTarget , "NDRVEX.DLL"), -1 );
    RegDBSetKeyValueEx( szKey, "ThreadingModel", REGDB_STRING, "Apartment", -1 );

    szKey = "\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\shellex\\ContextMenuHandlers\\{52636240-ECF7-11cd-99BE-0020AF346D41}";
    RegDBCreateKeyEx( szKey, "" );
    RegDBSetKeyValueEx( szKey, "", REGDB_STRING, "", -1 );

    RegDBSetDefaultRoot( HKEY_LOCAL_MACHINE );

    ////////////////////////////////////////////////////////
    // Enhanced Recycle bin (if unerasew installed)
    // The following entries are used enable Nprotect and update the
    // appearance of the Recycle Bin
    //
    // 1. Change the Recycle Bin's class name in the registry.
    // 2. Make verb to launch Unerase the default (double-click to open).
    // Change the name of the Recycle Bin


        szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}";
		nSize = MAX_PATH;
        if (RegDBGetKeyValueEx( szKey, "", nType, szValue, nSize ))
		{
            if ( _tcsicmp( szValue, szNPRB_Display ) != 0 )
            {
				szKey = NU_INSTALL_KEY;
				RegDBCreateKeyEx(szKey, "");
				nSize = MAX_PATH;
                if (!RegDBGetKeyValueEx( szKey, "OldBin", nType,
                                         szOldBin, nSize ))
				{
                    RegDBSetKeyValueEx( szKey, "OldBin", REGDB_STRING,
                                        szValue, -1 );
                }
			}
        }
        szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}";
        RegDBSetKeyValueEx( szKey, "", REGDB_STRING, szNPRB_Display, -1 );

		nSize = MAX_PATH;
		// This value is new to Me and Win2k. Modify 'Localized' key if it exists
		if ( RegDBGetKeyValueEx( szKey, "LocalizedString", nType, szValue, nSize ))
		{
			if ( _tcsicmp( szValue, szNPRB_Display ) != 0 )
            {
				szKey = NU_INSTALL_KEY;
				RegDBCreateKeyEx(szKey, "");
				nSize = MAX_PATH;
                if (!RegDBGetKeyValueEx( szKey, "OldBinLocalized", nType, szOldBin, nSize ))
				{
                    RegDBSetKeyValueEx( szKey, "OldBinLocalized", REGDB_STRING, szValue, -1 );
                }
			}
            szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}";
        	RegDBSetKeyValueEx( szKey, "LocalizedString", REGDB_STRING, szNPRB_Display, -1 );
		}

        if ( !IsOSNT() )
       	{
            // Add NPROTECT to list of programs to run
            // at Windows Startup

            szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
            RegDBCreateKeyEx( szKey, "" );
            RegDBSetKeyValueEx( szKey, "NPROTECT", REGDB_STRING, BSlash(szBuff, g_szTarget , "NPROTECT.EXE"), -1 );

            szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices";
            RegDBCreateKeyEx( szKey, "" );
            RegDBSetKeyValueEx( szKey, "NPROTECT", REGDB_STRING, BSlash(szBuff, g_szTarget , "NPROTECT.EXE"), -1 );

            // Turn on use of NP icon

            RegDBSetDefaultRoot( HKEY_CURRENT_USER );

            szKey = "\\SOFTWARE\\Symantec\\Norton Utilities\\Norton Unerase\\StateIcons";
            RegDBCreateKeyEx( szKey, "" );
            RegDBSetKeyValueEx( szKey, "NPShield", REGDB_NUMBER, "1", -1 );
        }


	//Set up the exclusion list
	typedef BOOL (WINAPI * LPFNSETDEFAULTEXCLUSIONS)(void);
	HINSTANCE hLibrary = LoadLibrary("USHELLEX.DLL");

	if(hLibrary > (HINSTANCE)HINSTANCE_ERROR)
	{
		LPFNSETDEFAULTEXCLUSIONS pfnSetDefaultExclusions =
			(LPFNSETDEFAULTEXCLUSIONS) GetProcAddress(hLibrary, "SetDefaultExclusions");
		pfnSetDefaultExclusions();
		FreeLibrary(hLibrary);
	}


	//
	//  Register NPComSvr COM Object
	//
	{
		typedef HRESULT (WINAPI *LPFNDllRegisterServer)(void);
		HINSTANCE hLibrary = LoadLibrary("NPComSvr.DLL");
		if( hLibrary )
		{
			LPFNDllRegisterServer pfnDllRegisterServer = (LPFNDllRegisterServer)GetProcAddress( hLibrary, "DllRegisterServer" );

			if( pfnDllRegisterServer )
			{
				pfnDllRegisterServer();
			}

			FreeLibrary( hLibrary );
		}
	}



	//NT Service and driver
	if(IsOSNT())
	{
		//Make sure our service is installed.
		//LaunchFileInMyDirectory("NPROTECT.EXE", "/serviceuninstall", TRUE);
		LaunchFileInMyDirectory(NPROTECT_SERVICE_FILE, "/serviceinstall", TRUE);

		//Copy the driver over and fail if there is already a driver file in that directory
		CopyDriverToDriversDirectory();

		//Install driver
		InstallNPDriver(TRUE);

		// Start Service
		StartNprotectService();

	}

	//Update the desktop
	SHChangeNotify(SHCNE_UPDATEIMAGE, SHCNF_DWORD|SHCNF_FLUSHNOWAIT, (LPCVOID)-1, 0);

	return(1);
}

//Uninstalls Nprotect
DWORD UnInstall_NProtect()
{
	//NT Specific Section
	if(IsOSNT())
	{
		StopNprotectService();

		//Uninstall driver
		UnInstallNPDriver();

		//Delete Driver file
		if(!DeleteDriverFromDriversDirectory())
			_ASSERTE(FALSE && "Couldn't set the driver to be deleted on reboot");

		LaunchFileInMyDirectory(NPROTECT_SERVICE_FILE, "/serviceuninstall", TRUE);
	}
	else
	{	//9x specific

		ShutdownNprotect95();

		//Restore the recyclebin icons under 9x
		RestoreRecycleBin();
	}

	HKEY hTmpKey = NULL;
	DWORD nType, nSize;
	TCHAR szBinString[MAX_PATH] = {0};
	TCHAR szTemp[MAX_PATH] = {0};
	TCHAR szValue[MAX_PATH] = {0};
	TCHAR szOldBin[MAX_PATH] = {0};
	TCHAR szAppPath[MAX_PATH] = {0};
	TCHAR szBuff[MAX_PATH] = {0};
	TCHAR szNPRB_Display[MAX_PATH] = {0};

	myTCHAR szKey;

	// Load strings to look for in Registry
	LoadString(_Module.GetResourceInstance(), IDS_NPRB_DISPLAY, szNPRB_Display, MAX_PATH);

	//Remove Reg Keys

	RegDBSetDefaultRoot( HKEY_LOCAL_MACHINE );

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\shellex\\PropertySheetHandlers\\{263913E0-07B9-106A-8674-00AA000D6F0D}";
    RegDBDeleteKey(szKey);

	szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\Shell";
    RegDBDeleteKeyValue( szKey, "");

	szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\Shell";
	szKey = BSlash(szBuff, szKey, "UneraseWizard");
    RegDBDeleteKey(szKey);

    szKey =  BSlash(szBuff, szKey , "Command");
    RegDBDeleteKey(szKey);

	szKey = "\\SOFTWARE\\Symantec\\Norton Utilities\\Norton Protection";
    RegDBDeleteKey(szKey);

    szKey = "\\SOFTWARE\\Microsoft\\Windows\\Help";
    RegDBDeleteKeyValue(szKey, "Ue32.hlp");

    szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\UE32.EXE";
    RegDBDeleteKey(szKey);

    szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NPROTECT.EXE";
    RegDBDeleteKey(szKey);

    szKey = "\\SOFTWARE\\Symantec\\Norton Utilities\\Norton UnErase";
    RegDBDeleteKey(szKey);

	szKey = "\\SOFTWARE\\Classes\\CLSID\\{263913E0-07B9-106A-8674-00AA000D6F0D}";
    RegDBDeleteKey(szKey);

	szKey = "\\SOFTWARE\\Classes\\CLSID\\{11dde060-38d8-11d1-89e8-00600811c705}";
    RegDBDeleteKey(szKey);

	szKey = "\\SOFTWARE\\Classes\\CLSID\\{11dde061-38d8-11d1-89e8-00600811c705}";
    RegDBDeleteKey(szKey);

	RegDBDeleteKeyValue(NU_INSTALL_KEY, "Extensions");

    RegDBSetDefaultRoot( HKEY_CLASSES_ROOT );

    szKey = "\\CLSID\\{52636240-ECF7-11cd-99BE-0020AF346D41}";
    RegDBDeleteKey(szKey);

    szKey = "\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\shellex\\ContextMenuHandlers\\{52636240-ECF7-11cd-99BE-0020AF346D41}";
    RegDBDeleteKey(szKey);

	if(!IsOSNT())
	{
		RegDBSetDefaultRoot(HKEY_CURRENT_USER);

		szKey = "\\Software\\Norton Utilities\\Norton Unerase";
		RegDBDeleteKey(szKey);


		RegDBSetDefaultRoot( HKEY_LOCAL_MACHINE );

        szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
        RegDBDeleteKeyValue( szKey, "NPROTECT");

        szKey = "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices";
        RegDBDeleteKeyValue( szKey, "NPROTECT");

	}

    RegDBSetDefaultRoot( HKEY_LOCAL_MACHINE );

	  ////////////////////////////////////////////////////////
    // Enhanced Recycle bin
    // Set the stuff back to normal

    szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}";
	nSize = MAX_PATH;
    if (RegDBGetKeyValueEx( szKey, "", nType, szValue, nSize ))
	{
        if ( _tcsicmp( szValue, szNPRB_Display ) == 0 )
        {
			szKey = NU_INSTALL_KEY;
			nSize = MAX_PATH;
            if (RegDBGetKeyValueEx( szKey, "OldBin", nType,
                                     szOldBin, nSize ))
			{
				RegDBDeleteKeyValue(szKey, "OldBin");
                szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}";
				RegDBSetKeyValueEx( szKey, "", REGDB_STRING, szOldBin, -1 );

            }
		}
    }

	nSize = MAX_PATH;
	// This value is new to Me and Win2k. Modify 'Localized' key if it exists
	if ( RegDBGetKeyValueEx( szKey, "LocalizedString", nType, szValue, nSize ))
	{
		if ( _tcsicmp( szValue, szNPRB_Display ) == 0 )
        {
			szKey = NU_INSTALL_KEY;
			nSize = MAX_PATH;
            if ( RegDBGetKeyValueEx( szKey, "OldBinLocalized", nType, szOldBin, nSize ))
			{
				RegDBDeleteKeyValue(szKey, "OldBinLocalized");
                szKey = "\\SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}";
        		RegDBSetKeyValueEx( szKey, "LocalizedString", REGDB_STRING,szOldBin, -1 );
			}
		}
	}

	//
	//	Get rid of NProtect keys
	//
	RegRecursiveDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec\\Norton Utilities\\Norton Protection");

	// Get rid of UnErase keys
	RegRecursiveDeleteKey(HKEY_CURRENT_USER, "SOFTWARE\\Symantec\\Norton Utilities\\Norton UnErase\\StateIcons");
	RegRecursiveDeleteKey(HKEY_CURRENT_USER, "SOFTWARE\\Symantec\\Norton Utilities\\Norton UnErase");

	//Get rid of the NU
	RegSafeRecursiveDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec\\Norton Utilities\\7.0\\Install");
	RegSafeRecursiveDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec\\Norton Utilities\\7.0");
	RegSafeRecursiveDeleteKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec\\Norton Utilities");
	RegSafeRecursiveDeleteKey(HKEY_CURRENT_USER, "SOFTWARE\\Symantec\\Norton Utilities");


	////  Done with enhanced recyclebin stuff
	//////////////////////////////////////////

	//
	// UnRegister NPComSvr.DLL COM Object
	//
	{
		typedef HRESULT (WINAPI *LPFNDllUnregisterServer)(void);
		HINSTANCE hLibrary = LoadLibrary("NPComSvr.DLL");
		if( hLibrary )
		{
			LPFNDllUnregisterServer pfnDllUnregisterServer = (LPFNDllUnregisterServer)GetProcAddress( hLibrary, "DllUnregisterServer" );

			if( pfnDllUnregisterServer )
			{
				pfnDllUnregisterServer();
			}

			FreeLibrary( hLibrary );
		}
	}

	//Update the desktop
	SHChangeNotify(SHCNE_UPDATEIMAGE, SHCNF_DWORD|SHCNF_FLUSHNOWAIT, (LPCVOID)-1, 0);

    // Remove the nprotect protected directories
    RemoveNProtectDirectory();


	return(1);
}



/////////////////////////////////////////////////////////////////////////////
//
//  UnInstallNPDriver - Uses DeleteService() to delete NPDriver if NPDriver is
//					  running, it will mark the driver for delete on Reboot.
//
//  Params: NONE
//
//  Return value:  BOOL
//
/////////////////////////////////////////////////////////////////////////////
//  07/26/00 - BGARDIN/APIERCE function created
/////////////////////////////////////////////////////////////////////////////
BOOL UnInstallNPDriver()
{
	BOOL bRet = FALSE;
    DWORD       dwErr;

    SC_HANDLE   schSCManager = NULL;
    SC_HANDLE   schNPDriver = NULL;

	LTrace( "BEGIN UninstallNPDriver()");

	/* --------------------------------------------------
    ** Open the service manager
    ** --------------------------------------------------*/
    schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!schSCManager)
	{
		dwErr = GetLastError();
		LTrace( "	Error starting SCManager( %d )", dwErr);
		return(FALSE);
	}

	/* --------------------------------------------------
    ** UnInstall NPDriver
    ** --------------------------------------------------*/
	schNPDriver = OpenService(schSCManager, NPROTECT_DRIVER_NAME, SERVICE_ALL_ACCESS);
    if (!schNPDriver)
	{
		dwErr = GetLastError();
		LTrace( "	Error getting service handle %s - (%d)", NPROTECT_DRIVER_NAME, dwErr);
		return FALSE;
	}

	/* --------------------------------------------------
    ** Stop NPDriver
    ** --------------------------------------------------*/
    SERVICE_STATUS  xServiceStatus = {0};

    ControlService( schNPDriver, SERVICE_CONTROL_STOP, &xServiceStatus );
    QueryServiceStatus( schNPDriver, &xServiceStatus );

    int iCtr = 0;

                                // Wait around until the service stops,
                                // or 5 seconds elapses, which means
                                // the service didn't stop for some reason.

    while( ( xServiceStatus.dwCurrentState != SERVICE_STOPPED ) &&
           ( iCtr++ < 5 ) )
    {
        Sleep( 1000 );
        QueryServiceStatus( schNPDriver, &xServiceStatus );
    }

    if ( xServiceStatus.dwCurrentState == SERVICE_STOPPED )
        bRet = TRUE;

	// This will try and delete the service.
	// If the service is active, it "marks it for deletion"
	// and it is removed during the next reboot.
    if (!DeleteService(schNPDriver))
    {
        dwErr = GetLastError();
		LTrace( "	Error removing service %s - (%d)", NPROTECT_DRIVER_NAME, dwErr);
    }
	else
		bRet = TRUE;

	LTrace( "END UninstallNPDriver() - (%d)", bRet);

	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
//
//  InstallNPDriver - Uses CreateService() to install NPDriver as an NT
//					  Kernel Mode Service
//
//  Params: NONE
//
//  Return value:  BOOL
//
/////////////////////////////////////////////////////////////////////////////
//  07/26/00 - BGARDIN/APIERCE function created
/////////////////////////////////////////////////////////////////////////////
BOOL InstallNPDriver(BOOL bStartService)
{

	BOOL bRet = FALSE;
    DWORD dwErr;
	TCHAR szDriverPath[SI_MAX_PATH] = {0};

    SC_HANDLE   schSCManager = NULL;
    SC_HANDLE   schNPDriver = NULL;

	LTrace( "BEGIN InstallNPDriver()");

	/* --------------------------------------------------
    ** Open the service manager
    ** --------------------------------------------------*/
    schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!schSCManager)
	{
		dwErr = GetLastError();
		LTrace( "	Error starting SCManager( %d )", dwErr);
		return(FALSE);
	}

	/* --------------------------------------------------
    ** Install NPDriver
    ** --------------------------------------------------*/
	GetSystemDirectory(szDriverPath, MAX_PATH);
	_tcscat(szDriverPath, _T("\\"));
	_tcscat(szDriverPath, _T("Drivers"));
	_tcscat(szDriverPath, _T("\\"));
	_tcscat(szDriverPath, NPROTECT_DRIVER_FILE);
	schNPDriver = CreateService( schSCManager,
								 NPROTECT_DRIVER_NAME,
								 NPROTECT_DRIVER_DISPLAY_NAME,
								 SERVICE_ALL_ACCESS,
								 SERVICE_KERNEL_DRIVER,
								 SERVICE_DEMAND_START,
								 SERVICE_ERROR_NORMAL,
								 szDriverPath,
								 NULL,
								 NULL,
								 "SYMEVENT\0\0",
								 NULL,
								 NULL );


    if (!schNPDriver && GetLastError() != ERROR_SERVICE_EXISTS)
	{
		dwErr = GetLastError();
		LTrace( "	Error getting service handle %s - (%d)", NPROTECT_DRIVER_NAME, dwErr);
	}

	if( schNPDriver )
	{
		if (bStartService)
		{
			// We got a valid handle to our service, now let's try and start it up.
			bRet = StartService (schNPDriver, 0, NULL);

			if (!bRet)
			{
				dwErr = GetLastError();
				if (dwErr == ERROR_SERVICE_ALREADY_RUNNING)
				{
					LTrace( "	Service %s alredy running", NPROTECT_DRIVER_NAME);
					bRet = TRUE;
				}
				else
					LTrace( "	Error starting %s - (%d)", NPROTECT_DRIVER_NAME, dwErr);
			}
		}
		else
			bRet = TRUE;

		CloseServiceHandle( schNPDriver );
	}
	CloseServiceHandle( schSCManager );

	LTrace( "END InstallNPDriver() - (%d)", bRet);

	return bRet;
}

// Check to see if NProtect Service is installed and if it's not then install it.
// with the Nprotect.exe /serviceinstall commandline
BOOL VerifyNProtService()
{

    DWORD dwErr;
	TCHAR szDriverPath[SI_MAX_PATH] = {0};

    SC_HANDLE   schSCManager = NULL;
    SC_HANDLE   schNPService = NULL;

	LTrace( "BEGIN VerifyNprotService()");

	/* --------------------------------------------------
    ** Open the service manager
    ** --------------------------------------------------*/
    schSCManager = OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!schSCManager)
		{
			dwErr = GetLastError();
			LTrace( "	Error starting SCManager( %d )", dwErr);
			return(FALSE);
		}

	/* --------------------------------------------------
    ** Install NPDriver
    ** --------------------------------------------------*/
	schNPService = OpenService(	schSCManager,
								NPROTECT_SERVICE_NAME,
								READ_CONTROL
								);

		//If the service doesn't exist, but it is installed then start it up.
		if (!schNPService && GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			dwErr = GetLastError();
			LTrace( "	Service doesn't exist, so we are going to reinstall it");

			TCHAR szFullPath[MAX_PATH+1] = {0};
            DWORD dwSize = MAX_PATH+1;

			if(IsNUInstalledWithNProtect(szFullPath, dwSize))
			{
				_tcscat(szFullPath, _T(" /serviceinstall"));

				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(si));
				ZeroMemory(&pi, sizeof(pi));

				si.cb = sizeof(si);
				if(CreateProcess(NULL, szFullPath,
					NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
					NULL,   //Starting dir
					&si, &pi))
				{
					//now start up the process
					ResumeThread(pi.hThread);

					//Wait for the process to exit.
					while (WAIT_TIMEOUT ==	WaitForSingleObject(pi.hProcess, 100))
					{
						MSG   msg;

						while (PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
						{
							DispatchMessage( &msg );
						}
					}
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}
			}
		}
		else
		{
			CloseServiceHandle( schNPService);
		}

	CloseServiceHandle( schSCManager );

	LTrace( "END VerifyNProtService()");

	return(TRUE);
}

BOOL CopyDriverToDriversDirectory()
{

	TCHAR szDriverDir[MAX_PATH] = {0};
	TCHAR szFullPath[MAX_PATH]	= {0};

	//Set up the source file
	GetModuleDirectory(szFullPath);
	_tcscat(szFullPath, NPROTECT_DRIVER_FILE);

	//Put together the Destination name in the drivers directory
	GetSystemDirectory(szDriverDir, MAX_PATH);
	_tcscat(szDriverDir, _T("\\drivers\\") );
	_tcscat(szDriverDir, NPROTECT_DRIVER_FILE);

	//Will return false if the file already exists in the drivers dir.
	BOOL bResult = CopyFile(szFullPath, szDriverDir, TRUE);

#ifdef _DEBUG
	if(bResult == 0)
	{
		TCHAR szMsgBuf[MAX_PATH];
		DWORD dwRetval = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwRetval,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			szMsgBuf,
			MAX_PATH,
			NULL );

		MessageBox( NULL, szMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	}
#endif



	return(bResult);
}

BOOL DeleteDriverFromDriversDirectory()
{
	TCHAR szDriverDir[MAX_PATH] = {0};

	//Put together the Destination name in the drivers directory
	GetSystemDirectory(szDriverDir, MAX_PATH);
	_tcscat(szDriverDir, _T("\\drivers\\"));
	_tcscat(szDriverDir, NPROTECT_DRIVER_FILE);

	BOOL bResult = MoveFileEx(szDriverDir, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

	return bResult;
}

void LaunchFileInMyDirectory(LPTSTR szFileName, LPTSTR szCmdLine, BOOL bWait)
{
	//First thing to do is make sure that the service is installed
	TCHAR szModName[MAX_PATH]	= {0};
	TCHAR szFullCmdLine[MAX_PATH] = {0};
	TCHAR szDir[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]	= {0};

	GetModuleFileName(ghInstance, szModName, MAX_PATH);
	_tsplitpath(szModName, szDir, szPath, NULL, NULL);

	_stprintf(szFullCmdLine, _T("%s%s%s %s"), szDir, szPath, szFileName, szCmdLine);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);
    if(CreateProcess(NULL, szFullCmdLine,
        NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
        NULL,   //Starting dir
        &si, &pi))
    {

        //now start up the process
        ResumeThread(pi.hThread);

        if(bWait)
		{
			while (WAIT_TIMEOUT ==	WaitForSingleObject(pi.hProcess, 100))
			{
				MSG   msg;

				while (PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
				{
					DispatchMessage( &msg );
				}
			}
		}

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

	}
	return;
}

/////////////////////////////////////////////////////////////////////////////
//
//  StopNprotectService - Function to stop the NPROTECT service so that
//                        uninstall can do it's thing
//
//  Params: NONE
//
//  Return value:  TRUE if service is successfully stopped
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL StopNprotectService(void)
{
    BOOL            bRet = FALSE;
                                        // Open the Service Control Manager

    SC_HANDLE hSCM = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

    if ( hSCM )
    {
                                        // Try to open the service

        SC_HANDLE hService = OpenService( hSCM, NPROTECT_SERVICE_NAME,
                                          SERVICE_ALL_ACCESS );

        if (hService )
        {
            SERVICE_STATUS  xServiceStatus;

            ControlService( hService, SERVICE_CONTROL_STOP, &xServiceStatus );
            QueryServiceStatus( hService, &xServiceStatus );

            int iCtr = 0;

                                        // Wait around until the service stops,
                                        // or 5 seconds elapses, which means
                                        // the service didn't stop for some reason.

            while( ( xServiceStatus.dwCurrentState != SERVICE_STOPPED ) &&
                   ( iCtr++ < 5 ) )
            {
                Sleep( 1000 );
                QueryServiceStatus( hService, &xServiceStatus );
            }

            if ( xServiceStatus.dwCurrentState == SERVICE_STOPPED )
                bRet = TRUE;

            CloseServiceHandle(hService);
        }

        CloseServiceHandle(hSCM);
    }

    LTrace( "StopNprotectService( %d )\n", bRet );

    return( bRet );
}

/////////////////////////////////////////////////////////////////////////////
//
//  StartNprotectService - Function to start the NPROTECT service
//
//  Params: NONE
//
//  Return value:  TRUE if service is successfully started
//
/////////////////////////////////////////////////////////////////////////////
//  11/13/2001 - APierce, function created
/////////////////////////////////////////////////////////////////////////////

BOOL StartNprotectService(void)
{
    BOOL            bRet = FALSE;
                                        // Open the Service Control Manager

    SC_HANDLE hSCM = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

    if ( hSCM )
    {
                                        // Try to open the service

        SC_HANDLE hService = OpenService( hSCM, NPROTECT_SERVICE_NAME,
                                          SERVICE_ALL_ACCESS );

        if (hService )
        {
            SERVICE_STATUS  xServiceStatus;

            StartService( hService, NULL, NULL );
            QueryServiceStatus( hService, &xServiceStatus );

            int iCtr = 0;

                                        // Wait around until the service stops,
                                        // or 5 seconds elapses, which means
                                        // the service didn't stop for some reason.

            while( ( xServiceStatus.dwCurrentState != SERVICE_RUNNING ) &&
                   ( iCtr++ < 5 ) )
            {
                Sleep( 1000 );
                QueryServiceStatus( hService, &xServiceStatus );
            }

            if ( xServiceStatus.dwCurrentState == SERVICE_RUNNING )
                bRet = TRUE;

            CloseServiceHandle(hService);
        }

        CloseServiceHandle(hSCM);
    }

    LTrace( "StartNprotectService( %d )\n", bRet );

    return( bRet );
}


/////////////////////////////////////////////////////////////////////////////
//
//  ShutdownNprotect95 - Function to turn off the NPROTECT program so it can
//                       be uninstalled
//
//  Params: NONE
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  03/11/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////

BOOL ShutdownNprotect95( void )
{
    BOOL            bRet = FALSE;
    HWND            hwnd;

    hwnd = FindWindow( NPUEPROCCLASSNAME, NPUEPROCAPPNAME );

    if ( hwnd != NULL )
    {
        PostMessage( hwnd, WM_COMMAND, IDM_SHUTDOWN_EXT, 0L );
        bRet = TRUE;
    }

    LTrace( "ShutdownNprotect95( %d )\n", bRet );

	//
	// Give UE a chance to close itself down.
	//
	Sleep( 6000 );

	if( NULL != FindWindow( NPUEPROCCLASSNAME, NPUEPROCAPPNAME ) )
		LTrace( "ShutdownNprotect95() timeout expired.\n" );

    return( bRet );
}


/////////////////////////////////////////////////////////////////////////////
//
//  ShutdownUnerase9 -   Function to turn off the UEPROC32.EXE program under
//                       tomahawk
//
//  Params: NONE
//
//  Return value:   TRUE if successful
//
/////////////////////////////////////////////////////////////////////////////
//  03/11/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////


BOOL ShutdownUnerase9( void )
{
    BOOL            bRet = FALSE;
    HWND            hwnd;

    hwnd = FindWindow( NPUEPROCCLASSNAME, NULL );

    if ( hwnd != NULL )
    {
        PostMessage( hwnd, WM_CLOSE, 0, 0L );
        bRet = TRUE;
    }

    LTrace( "ShutdownUnerase9( %d )\n", bRet );

    return( bRet );
}


/////////////////////////////////////////////////////////////////////////////
//
//  RestoreRecycleBinBranch - Restore recycle bin paths
//
//  Params: hTopKey - Branch of registry to modify
//          szOriginalEmpty - Pointer to buffer that will hold the original icon for empty
//          szOriginalFull - Pointer to buffer that will hold the original icon for full
//
//  Return value:   TRUE if all the paths are found
//
/////////////////////////////////////////////////////////////////////////////
//  05/26/98 - Steve Kurtz, function created
/////////////////////////////////////////////////////////////////////////////

BOOL RestoreRecycleBinBranch(HKEY hTopKey,
							 LPCTSTR szOriginalEmpty,LPCTSTR szOriginalFull)
{
    BOOL            bRet = TRUE;
    LPTSTR           lpszRecycleBinKey = _T("SOFTWARE\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\DefaultIcon");
    LPTSTR           lpszEmpty = _T("Empty");
    LPTSTR           lpszFull = _T("Full");
    DWORD           dwType;
    DWORD           dwSize;
    TCHAR           szDefault[MAX_PATH] = {0};
	TCHAR			szWinEmpty[MAX_PATH] = {0};
	TCHAR			szWinFull[MAX_PATH] = {0};

	RegDBSetDefaultRoot( hTopKey );

	dwSize = MAX_PATH - 1;
	RegDBGetKeyValueEx(lpszRecycleBinKey, lpszEmpty, dwType, szWinEmpty, dwSize);

	dwSize = MAX_PATH - 1;
	RegDBGetKeyValueEx(lpszRecycleBinKey, lpszFull, dwType, szWinFull, dwSize);

// at this point, szWinEmpty and szWinFull hold the old values
// if they are different from the original empty and full,change them.
// to the originals

									// Restore original icons
		if ( lstrcmpi( szOriginalEmpty, szWinEmpty ) )
			RegDBSetKeyValueEx(lpszRecycleBinKey, lpszEmpty,  REG_SZ, szOriginalEmpty, _tcslen(szOriginalEmpty) + 1);

		if ( lstrcmpi( szOriginalFull, szWinFull ) )
			RegDBSetKeyValueEx(lpszRecycleBinKey, lpszFull, REG_SZ, szOriginalFull, _tcslen(szOriginalFull) + 1 );

									// Determine current default and reset

		dwSize = MAX_PATH - 1;

		if ( RegDBGetKeyValueEx(lpszRecycleBinKey, "", dwType, szDefault, dwSize ))
		{
			if ( !lstrcmpi( szDefault, szWinEmpty ) )
				RegDBSetKeyValueEx(lpszRecycleBinKey, "", REG_SZ, szOriginalEmpty, _tcslen(szOriginalEmpty) + 1 );
			else
				RegDBSetKeyValueEx(lpszRecycleBinKey, "", REG_SZ, szOriginalFull, _tcslen(szOriginalFull) + 1 );
		}

	return(TRUE);

}

/////////////////////////////////////////////////////////////////////////////
//
//  RestoreRecycleBin - Function to restore the recycle bin to it's original
//                      state
//
//  Params: NONE
//
//  Return value:   TRUE always
//
/////////////////////////////////////////////////////////////////////////////
//  02/14/97 - BruceH, function created
/////////////////////////////////////////////////////////////////////////////


BOOL WINAPI RestoreRecycleBin( void )
{
    BOOL            bRet = TRUE;
    LPTSTR          lpszUneraseKey   = _T("Software\\Symantec\\Norton Utilities\\Norton UnErase\\StateIcons");
    LPTSTR          lpszOEmpty = _T("OriginalEmpty");
    LPTSTR          lpszOFull = _T("OriginalFull");
    LPTSTR          lpszCurrentFull  = _T("CurrentFull");
    LPTSTR          lpszCurrentEmpty = _T("CurrentEmpty");
    DWORD           dwType;
    DWORD           dwSize;
    TCHAR	        szCurrFull[MAX_PATH] ={0};
    TCHAR		    szCurrEmpty[MAX_PATH] ={0};
    TCHAR		    szOFull[MAX_PATH] ={0};
	TCHAR	        szOEmpty[MAX_PATH] ={0};
    TCHAR		    szBuffer1[MAX_PATH] ={0};


    LTrace( "BEGIN RestoreRecycleBin()" );

	RegDBSetDefaultRoot( HKEY_CURRENT_USER );

	// Get unerase icons
    dwSize = MAX_PATH - 1;
	if(RegDBGetKeyValueEx(lpszUneraseKey, lpszCurrentFull, dwType, szCurrFull, dwSize ))
    {
        lstrcpy( szBuffer1, szCurrFull );
        szBuffer1[ lstrlen(szBuffer1) - 2 ] = 0;

        DeleteFile( szBuffer1 );

		dwSize = MAX_PATH - 1;
        if(RegDBGetKeyValueEx(lpszUneraseKey, lpszCurrentEmpty, dwType, szCurrEmpty, dwSize ))
        {
            lstrcpy( szBuffer1, szCurrEmpty );
            szBuffer1[ lstrlen(szBuffer1) - 2 ] = 0;

            DeleteFile( szBuffer1 );
        }
    }
    else
    {
        LTrace( "   Unable to retrieve unerase icons" );
        bRet = FALSE;
    }

    dwSize = MAX_PATH - 1;

    if(RegDBGetKeyValueEx(lpszUneraseKey, lpszOFull, dwType, szOFull, dwSize))
    {
        dwSize = SI_MAX_PATH - 1;

        if (RegDBGetKeyValueEx(lpszUneraseKey, lpszOEmpty, dwType, szOEmpty, dwSize ))
        {
            LTrace( "   szOEmpty=%s", (LPSTR)szOEmpty );
            LTrace( "   szOFull=%s", (LPSTR)szOFull );

			RestoreRecycleBinBranch(HKEY_LOCAL_MACHINE,
									szOEmpty,szOFull);

			RestoreRecycleBinBranch(HKEY_CURRENT_USER,
									szOEmpty,szOFull);
        }
    }
    else
    {
        LTrace( "Unable to retrieve original icons from unerase" );
        bRet = FALSE;
    }

    LTrace( "END RestoreRecycleBin( %d )\n", bRet );

    return ( bRet );
}
