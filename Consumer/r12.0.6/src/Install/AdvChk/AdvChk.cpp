// AdvChk.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "AdvChk.h"
#include "Utils.h"
#include "NProtect.h"
#include "reghelp.h"


//
//
//  FUNCTION: WinMain
//
//  PURPOSE:  Main entry point for the program.
//
//	HOW THIS WORKS: AdvChk checks to see if the adv tools are installed and if they are in the
//  correct location. If they are not installed it determines the correct location then installs
//  them there. If they are installed but not in the correct location it uninstalls them then
//  reinstalls them in the proper location.
//

HINSTANCE ghInstance = NULL;
HINSTANCE ghResInstance = NULL;
TCHAR g_szTarget[MAX_PATH];
CAtlBaseModule _Module;
#include "..\advchkres\resource.h"
#define  SYMC_RESOURCE_DLL _T("\\advchk.loc")


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	//Set up our global hinstance
	ghInstance = hInstance;
    DWORD dwZero = 0;
	TCHAR szPath[_MAX_PATH * 2] = {0};

    DWORD dwRet = ::GetModuleFileName(_Module.GetModuleInstance(),szPath,sizeof(szPath));
    if(dwRet > 0)
    {
        TCHAR* p = _tcsrchr(szPath,_T('\\'));
        *p = _T('\0');
        _tcscat(szPath,SYMC_RESOURCE_DLL);
        ghResInstance = (HINSTANCE)::LoadLibrary(szPath);
		_Module.SetResourceInstance(ghResInstance);
    }
	else
	{
		return(FALSE);
	}
	
	if(!IsNUInstalledWithNProtect(NULL, dwZero))
	{
		DWORD dwCmdLine = ProcessCmdLine(lpCmdLine);
		switch(dwCmdLine)
		{
			case UNINSTALL:
			{
				UnInstall_NProtect();
				break;
			}
			default:
			{
				if(!VerifyAdvToolsRegKeys())  //Make sure we need to install
				{
					Install_NProtect();	 //Then install

					//If we're not on NT and we're installing then we need to pop up a message telling the
					//user that they are going to have to reboot to get nprotect running again.
					if(!IsOSNT() && (dwCmdLine != INSTALL))
					{
						TCHAR szMessage[MAX_PATH] = {0};
						LoadString(_Module.GetResourceInstance(), ERR_NEED_REBOOT, szMessage, MAX_PATH);
						MessageBox(NULL, szMessage, UNERASE_DISPLAYNAME, MB_OK);
					}
				}
			}
		}
	}
	else
	{
		if(IsOSNT())
		{
			//Make sure that NProtect Service is installed.
			VerifyNProtService();

		}

		// Check for the right-click menu to display
		// "Norton Unerase Wizard" instead of "Unerase Wizard"
		CheckForBonsaiShellExtension();

	}
	return(1);
}


BOOL CheckForBonsaiShellExtension()
{
	DWORD dwRet;

	TCHAR szShellOld[100] = {0};

	HINSTANCE hInstance = GetModuleHandle(NULL);

	dwRet = LoadString( _Module.GetResourceInstance(), SHELL_UNERASE_OLD, szShellOld, 100 );
	if( NULL == dwRet )
	{
		// Critical error
		return FALSE;
	}

	HKEY hkShellRB = NULL;
	HKEY hkNPShellEx = NULL;

	//
	// Open up the shell extension key
	if( ERROR_SUCCESS == RegOpenKey( HKEY_LOCAL_MACHINE, _T("Software\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\Shell"), &hkShellRB ) )
	{

		//
		// and look for the pre-NU6 shell extension string
		if( ERROR_SUCCESS == RegOpenKey( hkShellRB, szShellOld, &hkNPShellEx ) )
		{
			//
			// Since the pre-Bonsai key exists, nuke the post-Bonsai key

			RegRecursiveDeleteKey(HKEY_LOCAL_MACHINE, _T("Software\\Classes\\CLSID\\{645FF040-5081-101B-9F08-00AA002F954E}\\Shell\\UnEraseWizard") );

			//
			// Cleanup
			RegCloseKey( hkNPShellEx );
		}

		//
		// Cleanup
		RegCloseKey( hkShellRB );

	}

	return TRUE;
}

DWORD ProcessCmdLine(LPTSTR szCmdLine)
{
	TCHAR* szIndex = NULL;

	szIndex = _tcschr(szCmdLine, '/');

	if(NULL != szIndex)
	{
		szIndex++;

		switch(_istlower(*szIndex) ? _toupper(*szIndex) : *szIndex)
		{
			case 'U':
			{
				return(UNINSTALL);
				break;
			}
			case 'I':
			{
				return(INSTALL);
				break;
			}
			default:
				break;

		}
	}
	return(0);
}

BOOL VerifyAdvToolsRegKeys()
{
	TCHAR szKey[] = REGKEY_UNERASE;
	TCHAR szName[] = INSTALLDIR;
	DWORD nvType = 0;
	TCHAR svValue[MAX_PATH] = {0};
	DWORD nvSize = MAX_PATH;

	//if the InstallDir entry for UnErase is our local directory then
	//we assume that everything is ok.
	if(RegDBGetKeyValueEx(szKey, szName, nvType, svValue, nvSize))
	{
		TCHAR szDir[MAX_PATH] = {0};

		GetModuleDirectory(szDir);

		//The installdir is saved as a short dir name, so we need to convert our's so they'll match.
		GetShortPathName(szDir, szDir, MAX_PATH);

		if(0 == _tcsicmp(svValue, szDir))
				return(TRUE);

	}
	return(FALSE);
}


