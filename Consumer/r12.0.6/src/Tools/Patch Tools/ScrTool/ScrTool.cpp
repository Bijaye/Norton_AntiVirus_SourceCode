// ScrTool.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

bool IsOem(LPCTSTR pcszNavDir);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    CNAVInfo navInfo;
    COSInfo osInfo;

    // Buffer for path to the NAV install dir
    TCHAR szNAVDir[MAX_PATH] = {0};

    // Buffer for ScriptUI.dll file
    TCHAR szScriptUIDll[MAX_PATH] = {0};

    // Buffer for the navw32.hlp file in the NAV directory
    TCHAR szHLPFile[MAX_PATH] = {0};

    // Buffer for the temp directory that this file is running in
    // which contains the help files
    TCHAR szCurDir[MAX_PATH] = {0};

    // Buffer for the help file that we will be copying over the
    // existing one in the NAV directory
    TCHAR szNewHlpFile[MAX_PATH] = {0};

    // Get the NAV directory
    _tcscpy(szNAVDir, navInfo.GetNAVDir());

    // Get the current directory
    GetModuleFileName(hInstance, szCurDir, MAX_PATH);
    TCHAR *pszTemp = _tcsrchr(szCurDir, _TCHAR('\\'));
    *pszTemp = _TCHAR('\0');

    // Get the full path to the NAVW32.hlp file we want to overwrite
    _tcscpy(szHLPFile, szNAVDir);
    _tcscat(szHLPFile, _T("\\NAVW32.hlp"));

    // Get the full path to ScriptUI.dll
    _tcscpy(szScriptUIDll, szNAVDir);
    _tcscat(szScriptUIDll, _T("\\ScriptUI.dll"));

    // Check to see if scriptui.dll exists to determine if this is a
    // script blocking build or not
    if( -1 != GetFileAttributes(szScriptUIDll) )
    {
        //
        // This is a Script Blocking build
        //

        // Get the full path to the new NAVW32SB.hlp
        _tcscpy(szNewHlpFile, szCurDir);
        _tcscat(szNewHlpFile, _T("\\NAVW32SB.hlp"));
        
        // Copy over the correct help file over writing the old one
        CopyFile(szNewHlpFile, szHLPFile, FALSE);
    }
    else
    {
        //
        // This is not a Script Blocking build
        //

        // Get the full path to the new NAVW32.hlp
        _tcscpy(szNewHlpFile, szCurDir);
        _tcscat(szNewHlpFile, _T("\\NAVW32.hlp"));
        
        // Copy over the correct help file over writing the old one
        CopyFile(szNewHlpFile, szHLPFile, FALSE);

        // Now update our LU registered command line to be "NAVWXX NoScrBlock"
        // so future LU patches can distinguish them and don't have to jump
        // through all the hoops that we are right now!!!

        TCHAR szNoScrProductName[MAX_PATH] = {0};

        if( osInfo.IsWinNT() )
            _tcscpy(szNoScrProductName, _T("NAVNT NoScrBlock"));
        else
            _tcscpy(szNoScrProductName, _T("NAV95 NoScrBlock"));

        HKEY hKey;
        // Open the regkey for the NAV command line
        if( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                          _T("SOFTWARE\\Symantec\\Norton Antivirus\\LiveUpdate\\CmdLines\\CmdLine2"),
                                          0,
                                          KEY_ALL_ACCESS,
                                          &hKey) )
        {
            RegSetValueEx(hKey, _T("ProductName"), 0, REG_SZ, reinterpret_cast<LPBYTE>(szNoScrProductName), _tcslen(szNoScrProductName)+1 );
            RegCloseKey(hKey);
        }
    }

    // Now check if this is an OEM build or not to copy over the
    // correct Support.hlp file

    // Get the full path to the existing support.hlp file
    _tcscpy(szHLPFile, szNAVDir);
    _tcscat(szHLPFile, _T("\\support.hlp"));

    if( IsOem(szNAVDir) )
    {
        //
        // This is an OEM build use supoem.hlp from the current directory
        //

        // Get the full path to the new Supoem.hlp
        _tcscpy(szNewHlpFile, szCurDir);
        _tcscat(szNewHlpFile, _T("\\supoem.hlp"));
    }
    else
    {
        //
        // This is NOT an OEM build use support.hlp from the current directory
        //

        // Get the full path to the new Support.hlp
        _tcscpy(szNewHlpFile, szCurDir);
        _tcscat(szNewHlpFile, _T("\\support.hlp"));
    }

    // Copy over the correct help file over writing the old one
    CopyFile(szNewHlpFile, szHLPFile, FALSE);

	return 0;
}


bool IsOem(LPCTSTR pcszNavDir)
{
    bool bRet = false;

    // The build is OEM if the value for CFGWIZ_OEM in cfgwiz.dat
    // is 1
    TCHAR szCfgWizDat[MAX_PATH] = {0};
    _tcscpy(szCfgWizDat, pcszNavDir);
    _tcscat(szCfgWizDat, _T("\\CfgWiz.dat"));

    // Allocate a navopts structure
    HNAVOPTS32 hCfgWiz = NULL;

    if( NAVOPTS32_OK == NavOpts32_Allocate( &hCfgWiz ) )
	{
        // Open the current file
        NAVOPTS32_STATUS status = NAVOPTS32_OK;

        #ifdef _UNICODE
			status = NavOpts32_LoadU( szCfgWizDat, hCfgWiz, TRUE );
		#else
			status = NavOpts32_Load( szCfgWizDat, hCfgWiz, TRUE );
		#endif

		if( status == NAVOPTS32_OK )
		{
            DWORD dwOEMValue;

            // Get the OEM value from CfgWiz.dat
            if( NAVOPTS32_OK == NavOpts32_GetDwordValue( hCfgWiz, CFGWIZ_OEM, &dwOEMValue, 0 ) )
            {
                // If the OEM value is not 0 then it's an OEM build
                if( 0 != dwOEMValue )
                {
                    bRet = true;
                }
            }
        }
    }

    // Free the navopts structure
    if( hCfgWiz )
	    NavOpts32_Free(hCfgWiz);

    return bRet;
}