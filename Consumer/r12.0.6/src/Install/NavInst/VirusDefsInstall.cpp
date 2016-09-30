/////////////////////////////////////////////////////////////////////////////////////////////
// VirusDefsInstall.cpp
//
// --Contains exported functions used in MSI custom actions for installed Virus Definitions
//
/////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CDefInst.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// InstallDefs(): Instantiates a Definst Class and calls install function
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall InstallDefs(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug InstallDefs()", "Debug Break", NULL);
#endif

	g_Log.Log("InstallDefs() Starting.");
	
	try
	{
		TCHAR szInstallDir[MAX_PATH]={0};
		DWORD dwSize = MAX_PATH;
		MsiGetProperty(hInstall, _T("INSTALLDIR"), szInstallDir, &dwSize);
		
		int iLength = _tcslen(szInstallDir);

		// if the last character of the path is a '\', remove it.
		if (szInstallDir[iLength - 1] == '\\')
			szInstallDir[iLength - 1] = '\0';
			
		CreateDirectory(szInstallDir, NULL);
		CDefinst DI;
		DI.Inst(hInstall);
	}
	catch(exception &ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in InstallDefs()");
	}
	
	g_Log.Log("InstallDefs() Finished.");

	return ERROR_SUCCESS;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// UnInstallDefs(): Instantiates a Definst Class and calls uninstall function
//
/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) UINT __stdcall UnInstallDefs(MSIHANDLE hInstall)
{
#ifdef _DEBUG
	MessageBox(NULL, "Attach to this MessageBox to Debug UnInstallDefs()", "Debug Break", NULL);
#endif

	g_Log.Log("UnInstallDefs() Starting.");

	try
	{
		CDefinst DI;
		DI.UnInst(hInstall);
	}
	catch(exception &ex)
	{
		g_Log.LogEx(ex.what());
	}
	catch(...)
	{
		g_Log.LogEx("Unknown exception in UninstallDefs()");
	}
	
	g_Log.Log("UnInstallDefs() Finished.");

	return ERROR_SUCCESS;
}
