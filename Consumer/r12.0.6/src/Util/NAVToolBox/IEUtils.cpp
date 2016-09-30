#include "stdafx.h"
#include "IEUtils.h"

namespace NAVToolbox
{
//------------------------- Registry Key Values -------------------------
// Registry key for Internet Explorer
const TCHAR szIEKey[] = _T("SOFTWARE\\Microsoft\\Internet Explorer");

// The title of the version key under Internet Explorer
const TCHAR szIEVersion[] = _T("Version");

//---------------------------- CIEPluginUtils ----------------------------
//
// Internet Explorer version numbers use the following format: 
//
//  major version.minor version. build number.sub-build number
//
//   Version          Product
//   -----------------------------------------------------------------
//   4.40.308         Internet Explorer 1.0 (Plus!)
//   4.40.520         Internet Explorer 2.0
//   4.70.1155        Internet Explorer 3.0
//   4.70.1158        Internet Explorer 3.0 (OSR2)
//   4.70.1215        Internet Explorer 3.01
//   4.70.1300        Internet Explorer 3.02 and 3.02a
//   4.71.544         Internet Explorer 4.0 Platform Preview 1.0 (PP1)
//   4.71.1008.3      Internet Explorer 4.0 Platform Preview 2.0 (PP2)
//   4.71.1712.6      Internet Explorer 4.0
//   4.72.2106.8      Internet Explorer 4.01
//   4.72.3110.8      Internet Explorer 4.01 Service Pack 1 (SP1)
//   4.72.3612.1713   Internet Explorer 4.01 Service Pack 2 (SP2)
//   5.00.0518.10     Internet Explorer 5 Developer Preview (Beta 1)
//   5.00.0910.1309   Internet Explorer 5 Beta (Beta 2)
//   5.00.2014.0216   Internet Explorer 5
//   5.00.2314.1003   Internet Explorer 5 (Office 2000)
//   5.00.2614.3500   Internet Explorer 5 (Windows 98 Second Edition)
//   5.00.2516.1900   Internet Explorer 5.01 (Windows 2000 Beta 3, build 5.00.2031) 
//   5.00.2919.800    Internet Explorer 5.01 (Windows 2000 RC1, build 5.00.2072)
//   5.00.2919.3800   Internet Explorer 5.01 (Windows 2000 RC2, build 5.00.2128)
//   5.00.2919.6307   Internet Explorer 5.01 (Also included with 
//                    Office 2000 SR-1, but not installed by default)
//   5.00.2920.0000   Internet Explorer 5.01 (Windows 2000, build 5.00.2195)
//   5.00.3103.1000   Internet Explorer 5.01 SP1 (Windows 2000)
//   5.00.3105.0106   Internet Explorer 5.01 SP1 (Windows 95/98 and Windows NT 4.0) 
//   5.50.3825.1300   Internet Explorer 5.5 Developer Preview (Beta)
//   5.50.4030.2400   Internet Explorer 5.5 & Internet Tools Beta
//   5.50.4134.0100   Windows Me (4.90.3000)
//   5.50.4134.0600   Internet Explorer 5.5
//   5.50.4308.2900   Internet Explorer 5.5 Advanced Security Privacy Beta
//   5.50.4522.1800   Internet Explorer 5.5 Service Pack 1
//
// It can obtained from the registry key: 
//
//  HKEY_LOCAL_MACHINE\Software\Microsoft\Internet Explorer 
//
// If a "Version" value appears in this key, Internet Explorer 4.0 or 
//  later is installed

CIEUtils::CIEUtils ()
{
	DWORD dwValueSize = MAX_PATH;
	TCHAR szVersion[MAX_PATH];
	LPTSTR lpMajor = NULL;
	LPTSTR lpMinor = NULL;
	LPTSTR lpBuild = NULL;
	HKEY hIEKey;
    DWORD dwType = 0;

	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, szIEKey, 0, KEY_QUERY_VALUE, &hIEKey ) == ERROR_SUCCESS) 
	{
		// Query registry value to get the version number.
		if ( RegQueryValueEx( hIEKey, szIEVersion, NULL, &dwType, (LPBYTE)szVersion, &dwValueSize ) == ERROR_SUCCESS ) 	
		{
			if (_tcslen(szVersion))
			{	
                _tcscpy ( m_szVersionString, szVersion);

				// Extract the Major and Minor numbers.
				lpMajor = _tcstok( szVersion, ".");				
				lpMinor = _tcstok( NULL, "." );						

                // check build number
				lpBuild = _tcstok( NULL, "." );

				if ( lpMajor )
                    m_iMajorVersion = atoi( lpMajor);

				if ( lpMinor )
                    m_iMinorVersion = atoi(lpMinor);												

                if ( lpBuild )
					m_iBuild = atoi( lpBuild );
			}  
        } //end of RegQueryValueEx

	    // Close registry key
        //
	    RegCloseKey(hIEKey);
    } 
}
} // end namespace NAVToolbox