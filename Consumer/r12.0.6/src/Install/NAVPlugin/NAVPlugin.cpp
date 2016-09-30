

#include "NAVLegacyUninst.h"
#include "helper.h"

//Globals
HINSTANCE ghInstance;
TCHAR szLogFile[MAX_PATH] = {0};
CDebugLog* UninstLog;
BOOL g_bNT = FALSE;

HINSTANCE hInst;
BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  dw_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hInst = hInstance;
    switch (dw_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
            // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
            DisableThreadLibraryCalls((HMODULE)hInstance);

            GetTempPath(sizeof(szLogFile), szLogFile);
			_tcscat(szLogFile, _T("NAVUnins.log"));
			UninstLog = new CDebugLog(szLogFile, true);
			break;

        case DLL_PROCESS_DETACH:
			if (UninstLog != NULL)
			    delete UninstLog;
			break;
    }

    return TRUE;
}



extern "C" HRESULT __declspec(dllexport) GetNames(BOOL bAllNames, LPTSTR lpszNameList, DWORD dwSize)
{		
	
	TCHAR* current = lpszNameList;
	
	// If a force cleanup is specified, we display the NAV ProductName.
	if(bAllNames == TRUE)
	{
		_tcscpy(current, _T("Norton AntiVirus"));
		return S_OK;
	}
	
	else
	{
		if(CheckPreviousVersion(lpszNameList) == E_FAIL)
			return E_FAIL;
		else
			return S_OK;
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////
// This function deletes InstallShield versions of NAV (6 and 7) and MSI version of NAV (8)
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" HRESULT __declspec(dllexport) CleanUp(LPCTSTR szProduct, LPCTSTR szDirectory)
{
	
	// If no directory is specified... we locate the path to NAV INSTALLDIR and uninstall.
	if (szDirectory == NULL)
	{
		if((_tcscmp(szProduct, _T("Norton AntiVirus 2000")) == 0) || (_tcscmp(szProduct, _T("Norton AntiVirus 2001")) == 0))
		{
			CNAVLegacyUninst LegacyUninst;
			LegacyUninst.Uninstall(szDirectory);
			return S_OK;
		}
		else if (_tcscmp(szProduct, _T("Norton AntiVirus 2002")) == 0)
		{
			CleanupWindowsInstallerInfo(_T("{3075C5C3-0807-4924-AF8F-FF27052C12AE}"), g_bNT, FALSE);
			RemoveNavRegKeys();
			return S_OK;
		}
		
		else return E_FAIL;
	}

	// We know it's a FORCE uninstall, we pass the directory our uninstaller.
	else
	{
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx (&osvi);

		// Check OS type
		if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		{	
			g_bNT = FALSE;
		}

		else if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
		{
			g_bNT = TRUE;
		}
		
		// Blast away the MSI links... if we uninstalling MSI version of NAV.
		CleanupWindowsInstallerInfo(_T("{3075C5C3-0807-4924-AF8F-FF27052C12AE}"), g_bNT, FALSE);
		RemoveNavRegKeys();

		// Remove extra backslash.
		TCHAR* pszTemp = NULL;
		pszTemp = _tcsrchr(szDirectory, '\\');
		if (pszTemp != NULL)
			*pszTemp = _T('\0');

		// Call NAV Legacy Uninstaller
		if(_tcscmp(szProduct, _T("Norton AntiVirus")) == 0)
		{
			CNAVLegacyUninst LegacyUninst;
			LegacyUninst.Uninstall(szDirectory);
			return S_OK;
		}
		else
			return E_FAIL;
	}
	


}
