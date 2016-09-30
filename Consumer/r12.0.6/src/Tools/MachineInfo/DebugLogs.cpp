// DebugLogs.cpp: implementation of the CDebugLogs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "DebugLogs.h"
#include "NAVInfo.h"
#include "FileIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebugLogs::CDebugLogs()
{
    CNAVInfo NAVInfo;
    if ( NAVInfo.GetNAVDir ()[0] != '\0' )
        m_bIsAvailable = true;
}

CDebugLogs::~CDebugLogs()
{

}


bool CDebugLogs::CreateReport ( LPCTSTR lpcszFolderPath )
{
    CNAVInfo NAVInfo;
    CFileIO FileIO;

    // Get all the logs from \Program Files\NAV . This will grab the Activity Log too.
    //
    FileIO.CopyFiles ( NAVInfo.GetNAVDir (), _T("*.log"), lpcszFolderPath, _T("ACTIVITY.LOG") );
    
    // Get all logs from \Program Files\Common Files\Symantec Shared
    //
    FileIO.CopyFiles ( NAVInfo.GetSymantecCommonDir (), _T("*.log"), lpcszFolderPath );

    // Get all logs from \Program Files\Symantec\LiveUpdate 
    //

	HRESULT hr;
    HKEY  hkey;
    TCHAR szSymantecDir [MAX_PATH] = {0};
    DWORD dwType, dwBuffSize = MAX_PATH * sizeof(TCHAR);

    // Read the Program Files directory
    //
    if ( ERROR_SUCCESS ==
		 (hr = ::RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                _T("software\\microsoft\\windows\\currentversion"),
                                0, KEY_QUERY_VALUE, &hkey )) )
    {
        if ( ERROR_SUCCESS ==
			(hr = ::RegQueryValueEx ( hkey, _T("ProgramFilesDir"), NULL, &dwType,
                                      LPBYTE(szSymantecDir), &dwBuffSize )) )
        {
            if ( !*szSymantecDir )
                return false;

            // Get the second to last char in the path.
            //
            TCHAR* pszDirNull = _tcsrchr ( szSymantecDir, '\0');

            TCHAR* pszLastSlash = CharPrev( szSymantecDir, pszDirNull );

            // If the ending character is a slash, null it.
            //            
            if ( 0 == _tcscmp ( pszLastSlash, "\\"))
            {
                *pszLastSlash = '\0';
            }
        }

        ::RegCloseKey ( hkey );
        
        _tcscat ( szSymantecDir, _T("\\Symantec\\LiveUpdate") );
        FileIO.CopyFiles ( szSymantecDir, _T("*.log"), lpcszFolderPath );
    }

    return true;
}
