////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MSSystemInfo.cpp: implementation of the MSSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "MSSystemInfo.h"
#include "AppLauncher.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMSSystemInfo::CMSSystemInfo()
{
    // Find the MSINFO32.EXE file to see if this OS supports it.
    //
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &vi );

    if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion < 5 )
        _tcscpy ( m_szInfoAppName, _T("winmsd.exe"));
    else
        _tcscpy ( m_szInfoAppName, _T("msinfo32.exe"));

    DWORD dwAttribs = 0;

    dwAttribs = GetFileAttributes ( m_szInfoAppName );
    
    if ( dwAttribs > 0 )
        m_bIsAvailable = true;
}

CMSSystemInfo::~CMSSystemInfo()
{

}


bool CMSSystemInfo::CreateReport(LPCTSTR lpcszFolderPath)
{
    if ( !m_bIsAvailable )
        return false;
    
    if ( 0 == _tcslen ( lpcszFolderPath ) )
        return false;

    bool bReturn = false;

    TCHAR szCommandLine [MAX_PATH] = {0};

    // We cut out some of the things we don't need since they can take a really long
    // time to accumulate.
    //
    // Supported on Win2K+/ME : MSDN Q255713
    //
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	GetVersionEx( &vi );

    // Win2K/XP or ME
    //
    if (( vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion > 4 ) ||
        ( vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && vi.dwMajorVersion >= 4 && vi.dwMinorVersion >= 90 ))
	{
        _stprintf ( szCommandLine, _T("/nfo \"%s\\msinfo.nfo\""), lpcszFolderPath );
    }

    // ME can handle command lines, but not the /categories
    //
    if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion > 4 )
    {
        _tcscat ( szCommandLine, _T(" /categories +Components+SystemSummary+Resources+SWEnv-SWEnvNetConn-Apps"));
    }

    // NT4 - WINMSD command lines 
    // winmsd.exe puts the report on <current path>\\<machine name>.txt
    //
    if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion < 5 )
	{
        _tcscpy ( szCommandLine, _T("/a /f"));
    }

    CAppLauncher AppLauncher;
    
    // If we failed, just exit
    //
    if (!AppLauncher.LaunchAppAndWait ( m_szInfoAppName, szCommandLine ))
        return false;

    // If NT4 we aren't done yet.
    //
    if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT && vi.dwMajorVersion < 5 )
    {
        TCHAR szMachineName [MAX_PATH] = {0};
        TCHAR szCurrentFile [MAX_PATH] = {0};
        TCHAR szCurrentDirectory [MAX_PATH] = {0};
        TCHAR szTargetFile [MAX_PATH] = {0};
        DWORD dwSize = MAX_PATH;

        if ( GetComputerName ( szMachineName, &dwSize ))
        {
            GetCurrentDirectory ( MAX_PATH, szCurrentDirectory );

            _stprintf ( szCurrentFile, _T("%s\\%s.txt"), szCurrentDirectory, szMachineName );
            _stprintf ( szTargetFile, _T("%s\\%s.txt"), lpcszFolderPath, szMachineName );

            CopyFile ( szCurrentFile, szTargetFile, FALSE );
                
            return true;
        }
        else 
            return false;
    }
    else 
        return true;
}
