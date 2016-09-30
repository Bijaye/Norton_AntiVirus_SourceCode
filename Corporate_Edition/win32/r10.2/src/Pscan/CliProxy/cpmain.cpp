// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 1996-2004, 2005, 2006, Symantec Corporation, All rights reserved.
// cpmain.cpp

#include "pscan.h"
#include "SavrtModuleInterface.h"
#include "localtransport.h"
#include "SymSaferRegistry.h"
#include "CliProxyModule.h"
#include "CliProxyATL_i.c"

HINSTANCE hInstance,hInstLang;
HKEY hMainKey = 0;
HKEY hCommonKey = 0;
char HomeDir[IMAX_PATH];
char gszMoveDir[IMAX_PATH];
char SystemRunning=0;

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

CCliProxyModule _AtlModule;

/**************************************************************************************/
//#define X(x) MessageBox(NULL,x,"",0);
#define X(x) LogLine(x,1);
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved) {

	HKEY hkey;
	char szLang[IMAX_PATH];
	char szFilename[IMAX_PATH];
	char lFile[IMAX_PATH];
	DWORD dwUserNameSize;

	REF(lpvReserved);

	if (hInstance == 0) {
		hInstLang = hInstance = hinstDLL;
		RegCreateKey(HKEY_LOCAL_MACHINE, REGHEADER, &hMainKey);
		
		InitializeDebugLogging();

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

			if (GetLanguage( szLang,szFilename,"%sRRTV.DLL")) {
				dprintf("Loading language file %s\n",szFilename);
				hInstLang = LoadLibrary(szFilename);
				}

			if (!hInstLang) // if load failed just use english
				hInstLang = hInstance;

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

	return _AtlModule.DllMain(fdwReason, NULL);
}
/*****************************************************************************************************************/
VOID Real_dprintfTagged(DEBUGFLAGTYPE dwTag, const char *format,...) {

	va_list marker;

	va_start(marker, format);
	Real_vdprintfCommon(dwTag, format, marker);
	va_end(marker);
}
/*****************************************************************************************************************/
void Real_dprintf(const char *format,...) {
	// Calls to this function used to not have the call time prefixed in the logged line.
	va_list marker;

	va_start(marker, format);
	Real_vdprintfCommon(0, format, marker);
	va_end(marker);
}
/**************************************************************************************************************/
int NTSGetComputerName(char *pComputerName,wchar_t *pWchar) {
	DWORD s = NAME_SIZE;
	REF(pWchar);
	return GetComputerName(pComputerName,&s);
}
