// SymRegKeys.cpp: implementation of the CSymRegKeys class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "SymRegKeys.h"
#include "AppLauncher.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSymRegKeys::CSymRegKeys()
{

    HKEY hSymRegKey;
    if ( ERROR_SUCCESS == ::RegOpenKeyEx ( HKEY_LOCAL_MACHINE, "Software\\Symantec", 0, KEY_READ, &hSymRegKey ))
    {
        ::RegCloseKey ( hSymRegKey );
        m_bIsAvailable = true;
    }
}

CSymRegKeys::~CSymRegKeys()
{

}

bool CSymRegKeys::CreateReport ( LPCTSTR lpcszFolderPath )
{
    // Command lines for Regedit - MSDN Q168589
    
    TCHAR szCommandLine [MAX_PATH] = {0};

    _stprintf ( szCommandLine, "/e \"%s\\SymRegKeys.reg\" HKEY_LOCAL_MACHINE\\Software\\Symantec", lpcszFolderPath ); 
    
    CAppLauncher AppLauncher;
    
    return AppLauncher.LaunchAppAndWait ("regedit.exe", szCommandLine );
}
